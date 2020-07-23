/*! \file bcmfp_cth_filter.c

 *
 * APIs for FILTER create, update and delete.
 *
 * This file contains functions to create, update,
 * delete FILTERs into HW.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/******************************************************************************
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
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/bcmfp_trans_internal.h>
#include <bcmfp/bcmfp_action_internal.h>
#include <bcmfp/bcmfp_cth_filter.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bcmdrd/bcmdrd_field.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_filter_buffers_allocate(int unit,
                              uint32_t bflags,
                              bcmltd_sid_t tbl_id,
                              bcmfp_stage_t *stage,
                              bcmfp_buffers_t *buffers)
{
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    uint32_t **dedw = NULL;
    uint32_t **pdd = NULL;
    uint32_t **dpdd = NULL;
    uint32_t **sbr = NULL;
    uint32_t **dsbr = NULL;
    uint32_t **psg = NULL;
    uint32_t **qos = NULL;
    uint32_t **dqos = NULL;
    size_t size_one = 0;
    size_t size_two = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffers, SHR_E_PARAM);

    size_one = (sizeof(uint32_t *) * BCMFP_ENTRY_PARTS_MAX);
    size_two = sizeof(uint32_t) * BCMFP_MAX_WSIZE;

    if (bflags & BCMFP_BUFFERS_CREATE_EKW) {
        BCMFP_ALLOC(ekw, size_one, "bcmfpEkw");
        buffers->ekw = ekw;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(ekw[idx], size_two, "bcmfpEkwPart");
        }
    }

    if (bflags & BCMFP_BUFFERS_CREATE_EDW) {
        BCMFP_ALLOC(edw, size_one, "bcmfpEdw");
        buffers->edw = edw;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(edw[idx], size_two, "bcmfpEdwPart");
        }
    }

    /* if stage supports pdd allocate space for PDD profile data */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_POLICY_TYPE_PDD) &&
        (bflags & BCMFP_BUFFERS_CREATE_PDD)) {
        BCMFP_ALLOC(pdd, size_one, "bcmfpPddProfile");
        buffers->pdd = pdd;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(pdd[idx], size_two, "bcmfpPddProfilePart");
        }
    }

    /* if stage supports SBR allocate space for SBR profile data. */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_RESOLUTION_SBR) &&
        (bflags & BCMFP_BUFFERS_CREATE_SBR)) {
        BCMFP_ALLOC(sbr, size_one, "bcmfpSbrProfile");
        buffers->sbr = sbr;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(sbr[idx], size_two, "bcmfpSbrProfilePart");
        }
    }

    /* if stage supports PSG allocate space for PSG profile data. */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_PRESEL_KEY_DYNAMIC) &&
        (bflags & BCMFP_BUFFERS_CREATE_PDD)) {
        BCMFP_ALLOC(psg, size_one, "bcmfpPsgProfile");
        buffers->psg = psg;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(psg[idx], size_two, "bcmfpPsgProfilePart");
        }
    }

    /* if stage supports default policy allocate space for dedw. */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_DEFAULT_POLICY) &&
        (bflags & BCMFP_BUFFERS_CREATE_DEDW)) {
        BCMFP_ALLOC(dedw, size_one, "bcmfpDefaultEdw");
        buffers->dedw = dedw;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(dedw[idx], size_two, "bcmfpDefaultEdwPart");
        }
    }

    /* if stage supports default policy  and PDD allocate space for dpdd. */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_DEFAULT_POLICY) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_POLICY_TYPE_PDD) &&
        (bflags & BCMFP_BUFFERS_CREATE_DPDD)) {
        BCMFP_ALLOC(dpdd, size_one, "bcmfpDefaultPdd");
        buffers->dpdd = dpdd;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(dpdd[idx], size_two, "bcmfpDefaultPddPart");
        }
    }

    /* if stage supports default policy and SBR allocate space for dsbr. */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_RESOLUTION_SBR) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_DEFAULT_POLICY) &&
        (bflags & BCMFP_BUFFERS_CREATE_DSBR)) {
        BCMFP_ALLOC(dsbr, size_one, "bcmfpDefaultPdd");
        buffers->dsbr = dsbr;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(dsbr[idx], size_two, "bcmfpDefaultSbrPart");
        }
    }

    /* if stage supports QOS profiles allocate space for QOS profile data. */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_QOS_PROFILE) &&
        (bflags & BCMFP_BUFFERS_CREATE_QOS)) {
        BCMFP_ALLOC(qos, size_one, "bcmfpQos");
        buffers->qos = qos;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(qos[idx], size_two, "bcmfpQosPart");
        }
    }

    /*
     * if stage supports default QOS profiles allocate space for default
     *  QOS profile data.
     */
    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_QOS_PROFILE) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_DEFAULT_POLICY) &&
        (bflags & BCMFP_BUFFERS_CREATE_DQOS)) {
        BCMFP_ALLOC(dqos, size_one, "bcmfpDefaultQos");
        buffers->dqos = dqos;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(dqos[idx], size_two, "bcmfpDefaultQosPart");
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_buffers_clear(int unit, bcmfp_filter_t *filter)
{
    size_t size_two = 0;
    uint8_t num_parts = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    size_two = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
    num_parts = BCMFP_ENTRY_PARTS_MAX;
    if (filter->buffers.ekw != NULL) {
        for (idx = 0; idx < num_parts; idx++) {
            if (filter->buffers.ekw[idx] == NULL) {
                continue;
            }
            sal_memset(filter->buffers.ekw[idx], 0, size_two);
        }
    }

    if (filter->buffers.edw != NULL) {
        for (idx = 0; idx < num_parts; idx++) {
            if (filter->buffers.edw[idx] == NULL) {
                continue;
            }
            sal_memset(filter->buffers.edw[idx], 0, size_two);
        }
    }

    if (filter->buffers.pdd != NULL) {
        for (idx = 0; idx < num_parts; idx++) {
            if (filter->buffers.pdd[idx] == NULL) {
                continue;
            }
            sal_memset(filter->buffers.pdd[idx], 0, size_two);
        }
    }

    if (filter->buffers.sbr != NULL) {
        for (idx = 0; idx < num_parts; idx++) {
            if (filter->buffers.sbr[idx] == NULL) {
                continue;
            }
            sal_memset(filter->buffers.sbr[idx], 0, size_two);
        }
    }

    if (filter->buffers.psg != NULL) {
        for (idx = 0; idx < num_parts; idx++) {
            if (filter->buffers.psg[idx] == NULL) {
                continue;
            }
            sal_memset(filter->buffers.psg[idx], 0, size_two);
        }
    }

    if (filter->buffers.qos != NULL) {
        for (idx = 0; idx < num_parts; idx++) {
            if (filter->buffers.qos[idx] == NULL) {
                continue;
            }
            sal_memset(filter->buffers.qos[idx], 0, size_two);
        }
    }

    if (filter->buffers.dedw != NULL) {
        for (idx = 0; idx < num_parts; idx++) {
            if (filter->buffers.dedw[idx] == NULL) {
                continue;
            }
            sal_memset(filter->buffers.dedw[idx], 0, size_two);
        }
    }

    if (filter->buffers.dpdd != NULL) {
        for (idx = 0; idx < num_parts; idx++) {
            if (filter->buffers.dpdd[idx] == NULL) {
                continue;
            }
            sal_memset(filter->buffers.dpdd[idx], 0, size_two);
        }
    }

    if (filter->buffers.dsbr != NULL) {
        for (idx = 0; idx < num_parts; idx++) {
            if (filter->buffers.dsbr[idx] == NULL) {
                continue;
            }
            sal_memset(filter->buffers.dsbr[idx], 0, size_two);
        }
    }

    if (filter->buffers.dqos != NULL) {
        for (idx = 0; idx < num_parts; idx++) {
            if (filter->buffers.dqos[idx] == NULL) {
                continue;
            }
            sal_memset(filter->buffers.dqos[idx], 0, size_two);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_pse_config_get(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmfp_stage_t *stage,
                      uint32_t *pse_ids,
                      uint8_t num_pse_ids,
                      bcmfp_filter_t *filter)
{
    int rv;
    bcmltd_fid_t key_fid;
    bcmltd_sid_t tbl_id;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmfp_pse_config_t *pse_config = NULL;
    size_t num_fid = 0;
    size_t size = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    key_fid = stage->tbls.pse_tbl->key_fid;
    tbl_id = stage->tbls.pse_tbl->sid;
    num_fid = stage->tbls.pse_tbl->lrd_info.num_fid;

    size = (sizeof(bcmfp_pse_config_t *) * num_pse_ids);
    BCMFP_ALLOC(filter->pse_config,
                size,
                "bcmfpAclPreselEntryConfigArray");

    for (idx = 0; idx < num_pse_ids; idx++) {
        /* Get the FP pse LT entry saved in IMM. */
        rv = bcmfp_imm_entry_lookup(unit,
                                    tbl_id,
                                    &key_fid,
                                    &pse_ids[idx],
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
            filter->pse_config[idx] = NULL;
            filter->num_pse_configs++;
            continue;
        }
        SHR_IF_ERR_EXIT(rv);

        /* Assign key and data from in and out. */
        key = in.field[0];
        if (out.count != 0) {
            data = out.field[0];
        }

        /* Get the FP pse LT entry config of FILTER. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pse_config_get(unit,
                               op_arg,
                               tbl_id,
                               stage->stage_id,
                               key,
                               data,
                               &pse_config));
        filter->pse_config[idx] = pse_config;
        pse_config = NULL;
        filter->num_pse_configs++;
        in.field = NULL;
        in.count = 0;
        out.field = NULL;
        out.count = 0;
        key = NULL;
        data = NULL;
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_pse_config_free(unit,pse_config);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_rule_qual_presence_validate(int unit,
                                         bool is_presel,
                                         uint16_t idx,
                                         bcmfp_filter_t *filter)
{
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;
    bcmfp_qualifier_t rule_qualifier = 0;
    bcmfp_qualifier_t group_qualifier = 0;
    bcmfp_qualifier_t pbmp_qualifier = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_NULL_CHECK(filter->group_config, SHR_E_PARAM);

    if (is_presel == FALSE) {
        SHR_NULL_CHECK(filter->rule_config, SHR_E_PARAM);
        qual_bitmap = filter->group_config->qual_bitmap;
        qual_data = filter->rule_config->qual_data;
        pbmp_qualifier = filter->rule_config->pbmp_qual_id;
    } else {
        qual_bitmap = filter->psg_config->qual_bitmap;
        qual_data = filter->pse_config[idx]->qual_data;
    }


    while (qual_data != NULL) {
        rule_qualifier = qual_data->qual_id;
        if (is_presel == FALSE) {
            qual_bitmap = filter->group_config->qual_bitmap;
        } else {
            qual_bitmap = filter->psg_config->qual_bitmap;
        }
        while (qual_bitmap != NULL) {
            group_qualifier = qual_bitmap->qual;
            if (rule_qualifier == group_qualifier) {
                break;
            }
            qual_bitmap = qual_bitmap->next;
        }
        if (qual_bitmap == NULL) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
        qual_data = qual_data->next;
    }
    /* Verify if the pbmp type qualifier is present */
    if (pbmp_qualifier != 0)  {
        qual_bitmap = filter->group_config->qual_bitmap;
        while (qual_bitmap != NULL) {
            group_qualifier = qual_bitmap->qual;
            if (pbmp_qualifier == group_qualifier) {
                break;
            }
            qual_bitmap = qual_bitmap->next;
        }
        if (qual_bitmap == NULL) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_rule_qual_dm_validate(int unit,
                                   bool is_presel,
                                   uint16_t idx,
                                   bcmfp_filter_t *filter)
{
    uint8_t word = 0;
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;
    bcmfp_qualifier_t rule_qualifier = 0;
    bcmfp_qualifier_t group_qualifier = 0;
    bcmfp_qualifier_t pbmp_qualifier = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_NULL_CHECK(filter->group_config, SHR_E_PARAM);

    if (is_presel == FALSE) {
        SHR_NULL_CHECK(filter->rule_config, SHR_E_PARAM);
        qual_bitmap = filter->group_config->qual_bitmap;
        qual_data = filter->rule_config->qual_data;
        pbmp_qualifier = filter->rule_config->pbmp_qual_id;
    } else {
        qual_bitmap = filter->psg_config->qual_bitmap;
        qual_data = filter->pse_config[idx]->qual_data;
    }

    while (qual_data != NULL) {
        rule_qualifier = qual_data->qual_id;
        if (is_presel == FALSE) {
            qual_bitmap = filter->group_config->qual_bitmap;
        } else {
            qual_bitmap = filter->psg_config->qual_bitmap;
        }
        while (qual_bitmap != NULL) {
            group_qualifier = qual_bitmap->qual;
            if (rule_qualifier == group_qualifier) {
                break;
            }
            qual_bitmap = qual_bitmap->next;
        }
        if (qual_bitmap == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        qual_data = qual_data->next;
    }
    /* Add validation for pbmp type qualifier */
    if (pbmp_qualifier != 0)  {
        qual_bitmap = filter->group_config->qual_bitmap;
        while (qual_bitmap != NULL) {
            group_qualifier = qual_bitmap->qual;
            if (pbmp_qualifier == group_qualifier) {
                for (word = 0; word < BCMDRD_PBMP_WORD_MAX; word++) {
                    if ((qual_bitmap->bitmap[word] &
                                filter->rule_config->pbmp_data.w[word]) !=
                            filter->rule_config->pbmp_data.w[word])  {
                        SHR_ERR_EXIT(SHR_E_CONFIG);
                    }
                    if ((qual_bitmap->bitmap[word] &
                                filter->rule_config->pbmp_mask.w[word]) !=
                            filter->rule_config->pbmp_mask.w[word])  {
                        SHR_ERR_EXIT(SHR_E_CONFIG);
                    }
                }
                break;
            }
            qual_bitmap = qual_bitmap->next;
        }
        if (qual_bitmap == NULL) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    }


exit:

    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_rule_compress_qual_validate(int unit,
                                         bcmfp_stage_id_t stage_id,
                                         bcmfp_filter_t *filter)
{
    bcmfp_tbl_compress_fid_info_t *cfid_info = NULL;
    uint16_t idx1 = 0;
    uint16_t idx2 = 0;
    uint16_t idx = 0;
    int src_sbit = 0;
    int width = 0;
    uint32_t fid = 0;
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_qualifier_t qual_id = 0;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    bool ones_started = FALSE;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    if (filter->rule_config == NULL) {
       SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    qualifiers_fid_info = stage->tbls.rule_tbl->qualifiers_fid_info;

    /* Chek if no compression masks have holes in between(non LPM) */
    for (idx1 = 0; idx1 < BCMFP_COMPRESS_TYPES_MAX; idx1++) {
        if (filter->rule_config->compress_types[idx1] == FALSE) {
            continue;
        }
        cfid_info = stage->tbls.compress_fid_info[idx1];
        ones_started = FALSE;
        for (idx2 = 0; idx2 < cfid_info->num_fids; idx2++) {
            fid = cfid_info->key_fids[idx2];
            src_sbit = cfid_info->fid_offsets[idx2];
            width = cfid_info->fid_widths[idx2];
            qual_data = filter->rule_config->qual_data;
            qual_id = qualifiers_fid_info[fid].qualifier;
            while (qual_data != NULL) {
                if (qual_id == qual_data->qual_id) {
                    qual_data->compress = TRUE;
                    break;
                }
                qual_data = qual_data->next;
            }

            if (qual_data == NULL && ones_started == TRUE) {
                filter->entry_state =
                    BCMFP_ENTRY_NON_LPM_COMPRESS_MASK_IN_RULE;
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            if (qual_data == NULL) {
                continue;
            }

            for (idx = src_sbit; idx < width; idx++) {
                if (SHR_BITGET(qual_data->mask, idx)) {
                    ones_started = TRUE;
                } else if (ones_started == TRUE) {
                    filter->entry_state =
                        BCMFP_ENTRY_NON_LPM_COMPRESS_MASK_IN_RULE;
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }
            }
        }
    }

    for (idx1 = 0; idx1 < BCMFP_COMPRESS_TYPES_MAX; idx1++) {
        if (filter->rule_config->compress_types[idx1] == TRUE)  {
            if (filter->group_config->compress_types[idx1] == FALSE)  {
                filter->entry_state =
                    BCMFP_ENTRY_ALPM_COMPRESSION_OFF_IN_GROUP_ON_IN_RULE;
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_action_single_wide_conflict_check(int unit,
                                        bcmfp_stage_t *stage,
                                        bcmfp_action_t action1,
                                        bcmfp_action_t action2,
                                        bool *conflict)
{
    uint32_t *conflict_fid_arr = NULL;
    bcmfp_action_cfg_t *action_cfg = NULL;
    uint8_t index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_action_cfg_get(unit,
                                  stage->stage_id,
                                  action1,
                                  &action_cfg));
    conflict_fid_arr = action_cfg->single_wide_conflict_fid;

    /*
     * check if the actions are conflicting in singlw wide mode alone.
     * These include actions that are not logically conflicting but
     * are overlay in HW.
     */
    while (conflict_fid_arr[index] != 0) {
        if (conflict_fid_arr[index] == action2) {
            *conflict = TRUE;
            SHR_EXIT();
        }
        index++;
    }
exit:
  SHR_FUNC_EXIT();
}

static int
bcmfp_filter_policy_action_data_split(int unit,
                                      bcmfp_stage_t *stage,
                                      bcmfp_action_data_t **action_data,
                                      bcmfp_action_data_t **action_data1,
                                      bool split_possible)
{
    bool conflict = FALSE;
    bcmfp_action_data_t *action_data2 = *action_data;
    bcmfp_action_data_t *action_data3 = *action_data;
    bcmfp_action_data_t *action_data1_tail = *action_data1;
    bcmfp_action_data_t *prev_action_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (*action_data == NULL) {
        SHR_EXIT();
    }

    while (action_data2 != NULL) {
        action_data3 = *action_data;
        prev_action_data = action_data3;
        while (action_data3 != NULL) {
            if (action_data3->action == action_data2->action) {
                prev_action_data = action_data3;
                action_data3 = action_data3->next;
                continue;
            }
            conflict = FALSE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_action_single_wide_conflict_check(unit,
                                                         stage,
                                                         action_data2->action,
                                                         action_data3->action,
                                                         &conflict));
            if (conflict == TRUE) {
                /*
                 * Move the actions conficting with action_data2 from
                 * action_data to action_data1
                 */
                if (split_possible == FALSE) {
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }
                prev_action_data->next = action_data3->next;
                if (*action_data1 == NULL) {
                    *action_data1 = action_data3;
                    action_data1_tail = *action_data1;
                } else {
                    action_data1_tail->next = action_data3;
                    action_data1_tail = action_data3;
                }

            } else {
                prev_action_data = action_data3;
            }
            action_data3 = action_data3->next;
            if (action_data1_tail!= NULL) {
                action_data1_tail->next = NULL;
            }
        }
        action_data2 = action_data2->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_policy_actions_split(int unit,
                                  bcmfp_stage_t *stage,
                                  bcmfp_filter_t *filter)
{
    bcmfp_group_mode_t group_mode;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bool split_possible = FALSE;
    bcmfp_action_data_t **action_data = NULL;
    bcmfp_action_data_t **split_action_data = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    opinfo = filter->group_oper_info;
    group_mode = opinfo->group_mode;

    /* actions can be spread across 2 parts only for the modes below. */
    if (group_mode == BCMFP_GROUP_MODE_DBLINTER ||
        group_mode == BCMFP_GROUP_MODE_TRIPLE) {
        split_possible = TRUE;
    } else {
        split_possible = FALSE;
    }
    /*
     * split the confliciting actions in action_data to action_data1
     * if actions 1,2,3 in action_data are conflicting,
     * action 1 is retained in action_data and actions 2 and 3
     * are moved to split_action_data
     */
    action_data = &(filter->policy_config->action_data);
    split_action_data = &(filter->policy_config->action_data1);

    rv = bcmfp_filter_policy_action_data_split(unit,
                                               stage,
                                               action_data,
                                               split_action_data,
                                               split_possible);
    SHR_IF_ERR_EXIT(rv);

    /* actions can be spread across 3 parts only for triple wide mode. */
    if (group_mode == BCMFP_GROUP_MODE_TRIPLE) {
            split_possible = TRUE;
    } else {
        split_possible = FALSE;
    }
    /* split the conflicting actions in action_data1 to action_data2 */

    action_data = &(filter->policy_config->action_data1);
    split_action_data = &(filter->policy_config->action_data2);

    /*
     * the policy->action_data by this next step is split into 3 parts
     * action_data,action_data1,action_data2 each corresponding to
     * action_data to be inserted in part 0,1 and 2 respectively.
     */

    rv = bcmfp_filter_policy_action_data_split(unit,
                                               stage,
                                               action_data,
                                               split_action_data,
                                               split_possible);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_policy_action_conflicts_validate(int unit,
                                              bool is_default,
                                              bcmfp_stage_t *stage,
                                              bcmfp_filter_t *filter)
{
    bcmfp_action_data_t *action_data1 = NULL;
    bcmfp_action_data_t *action_data2 = NULL;
    bcmfp_action_data_t *action_data = NULL;
    bcmfp_action_t action1 = 0;
    bcmfp_action_t action2 = 0;
    bcmfp_action_data_t *action_data_part[BCMFP_ACTION_MAX_PARTS];
    uint8_t part_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    /* Invalid view type */
    if (filter->policy_config->viewtype ==
            BCMFP_ACTION_VIEWTYPE_COUNT)  {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (is_default == FALSE) {
        action_data_part[0] = filter->policy_config->action_data;
        action_data_part[1] = filter->policy_config->action_data1;
        action_data_part[2] = filter->policy_config->action_data2;
    } else {
        action_data_part[0] = filter->dpolicy_config->action_data;
        action_data_part[1] = filter->dpolicy_config->action_data1;
        action_data_part[2] = filter->dpolicy_config->action_data2;
    }

    /* check for conflicting actions in action_data across all parts. */
    for (part_id = 0; part_id < BCMFP_ACTION_MAX_PARTS; part_id++) {
        action_data = action_data_part[part_id];
        action_data1 = action_data;

        while (action_data1 != NULL) {
            action1 = action_data1->action;
            action_data2 = action_data;
            while (action_data2 != NULL) {
                action2 = action_data2->action;
                if (action1 == action2) {
                    action_data2 = action_data2->next;
                    continue;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_action_conflict_check(unit,
                                                 stage,
                                                 action1,
                                                 action2));
                action_data2 = action_data2->next;
            }
            action_data1 = action_data1->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_policy_action_presence_validate(int unit,
                                             bool is_default,
                                             bcmfp_stage_t *stage,
                                             bcmfp_filter_t *filter)
{
    bcmfp_action_data_t *action_data1 = NULL;
    bcmfp_action_data_t *action_data2 = NULL;
    bcmfp_action_t action1 = 0;
    bcmfp_action_t action2 = 0;
    bcmfp_action_cfg_t *action_cfg = NULL;
    uint32_t action_flag = BCMFP_ACTIONS_IN_FLAT_POLICY;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (is_default == FALSE) {
        action_data1 = filter->policy_config->action_data;
    } else {
        action_data1 = filter->dpolicy_config->action_data;
    }

    while (action_data1 != NULL) {
        action1 = action_data1->action;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_action_cfg_get(unit,
                                  stage->stage_id,
                                  action1,
                                  &action_cfg));
        /*
         * if the action is of FLAT POLICT type,
         * pdd is not required for the action
         */
        if ((action_cfg->flags & action_flag)) {
            action_data1 = action_data1->next;
            continue;
        }
        if (is_default == FALSE) {
            action_data2 =
                filter->pdd_config->action_data;
        } else {
            action_data2 =
                filter->dpdd_config->action_data;
        }
        while (action_data2 != NULL) {
            action2 = action_data2->action;
            if (action1 == action2) {
                break;
            }
            action_data2 = action_data2->next;
        }
        if (action_data2 == NULL) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
        action_data1 = action_data1->next;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_insert_hw_perpipe(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t tbl_id,
                               bcmfp_stage_t *stage,
                               bcmfp_filter_t *filter)
{
    int pipe_id = -1;
    bcmfp_entry_profiles_t profiles;
    bcmfp_group_id_t group_id = 0;
    bcmfp_entry_id_t entry_id = 0;
    uint32_t entry_prio = 0;
    size_t size = 0;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    uint32_t eflags = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_stage_id_t stage_id;
    bcmfp_stage_oper_mode_t oper_mode = 0;
    bcmfp_stage_oper_mode_t global_pipe_aware = 0;
    int pipe_min = -1;
    int pipe_max = -1;
    bcmdrd_pbmp_t pbmp_key;
    bcmdrd_pbmp_t pbmp_mask;
    bcmdrd_pbmp_t *ports_key = NULL;
    bcmdrd_pbmp_t *ports_mask = NULL;
    uint32_t *ipbm_key = NULL;
    uint32_t *ipbm_mask = NULL;
    bool ports_bmp_present = FALSE;
    bcmlrd_fid_t ports_bmp_fid = 0;
    bcmfp_qualifier_t ports_bmp_qualifier = 0;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    uint32_t pipe_map = 0;
    bool enable = 0;
    bcmfp_qual_data_t *qual_data = NULL;
    uint8_t part = 0;
    bcmfp_buffers_t *fp_buffers = NULL;
    size_t size_buf = 0;
    uint8_t num_parts = 0;
    uint8_t idx = 0;
    uint32_t bflags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    pipe_id = filter->group_config->pipe_id;
    group_id = filter->group_config->group_id;
    entry_id = filter->entry_config->entry_id;
    entry_prio = filter->entry_config->priority;
    enable = filter->entry_config->enable;

    stage_id = stage->stage_id;
    ekw = filter->buffers.ekw;
    edw = filter->buffers.edw;
    size = sizeof(bcmfp_entry_profiles_t);
    sal_memset(&profiles, 0, size);
    profiles.pdd = filter->buffers.pdd;
    profiles.psg = filter->buffers.psg;
    profiles.sbr = filter->buffers.sbr;

    size = sizeof(bcmfp_buffers_t);
    BCMFP_ALLOC(fp_buffers, size, "bcmfpHwInsertPerPipeBuffers");
    bflags = BCMFP_BUFFERS_CREATE_EKW;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       fp_buffers));
    opinfo = filter->group_oper_info;
    if (opinfo->ref_count == 0) {
        eflags |= BCMFP_ENTRY_FIRST_IN_GROUP;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_mode_get(unit,
                                   stage_id,
                                   &oper_mode));

    global_pipe_aware =
        BCMFP_STAGE_OPER_MODE_GLOBAL_PIPE_AWARE;
    if (oper_mode == global_pipe_aware) {
        eflags |= BCMFP_ENTRY_WRITE_PER_PIPE_IN_GLOBAL;
        pipe_min = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_dev_valid_pipes_get(unit,
                                       stage_id,
                                       &pipe_map));
        pipe_max = shr_util_popcount(pipe_map) - 1;
    } else {
        pipe_min = filter->group_config->pipe_id;
        pipe_max = filter->group_config->pipe_id;
    }

    qualifiers_fid_info =
        stage->tbls.rule_tbl->qualifiers_fid_info;

    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_IPBM_PARTS_SUPPORT)) {

        for (pipe_id = pipe_min; pipe_id <= pipe_max; pipe_id++) {
            if (pipe_id != pipe_min) {
                eflags |= BCMFP_ENTRY_SAME_KEY;
            }

            /* Copy original buffer information */
            size_buf = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
            num_parts = BCMFP_ENTRY_PARTS_MAX;
            if (fp_buffers->ekw != NULL) {
                for (idx = 0; idx < num_parts; idx++) {
                    if (fp_buffers->ekw[idx] == NULL) {
                        continue;
                    }
                    sal_memcpy(fp_buffers->ekw[idx],
                               filter->buffers.ekw[idx],
                               size_buf);
                }
            }

            ekw = fp_buffers->ekw;

            for (part = 0; part < stage->tbls.rule_tbl->num_ipbm_parts;
                 part++) {

                 ports_bmp_present = FALSE;

                 ports_bmp_fid
                 = stage->tbls.rule_tbl->ipbm_key_fid[part][pipe_id];
                 ports_bmp_qualifier =
                     qualifiers_fid_info[ports_bmp_fid].qualifier;

                 if (filter->rule_config != NULL) {
                     qual_data = filter->rule_config->qual_data;
                 }
                 while (qual_data != NULL) {
                     if (qual_data->qual_id == ports_bmp_qualifier) {
                         ipbm_key = (qual_data->key);
                         ipbm_mask = (qual_data->mask);
                         ports_bmp_present = TRUE;
                         break;
                     }
                     qual_data = qual_data->next;
                 }
                 /*
                  * Calculate ekw/edw for each pipe
                  * if the PBMP type qualifier
                  * is present in the rule.
                  */
                 if (ports_bmp_present == TRUE) {
                     if (BCMFP_STAGE_FLAGS_TEST(stage,
                         BCMFP_STAGE_IPBM_SUPPORT)) {
                         *ipbm_mask = (*ipbm_mask ^ *ipbm_key);
                     }

                     SHR_IF_ERR_VERBOSE_EXIT
                         (bcmfp_qual_set(unit,
                                         stage_id,
                                         group_id,
                                         filter->group_oper_info,
                                         0,
                                         ports_bmp_qualifier,
                                         ports_bmp_qualifier,
                                         0,
                                         NULL,
                                         ipbm_key,
                                         ipbm_mask,
                                         ekw));
                 }
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_entry_insert(unit,
                                        eflags,
                                        op_arg,
                                        pipe_id,
                                        stage_id,
                                        tbl_id,
                                        group_id,
                                        filter->group_oper_info,
                                        entry_id,
                                        entry_prio,
                                        ekw,
                                        edw,
                                        &profiles,
                                        enable));
        }
    } else {
        if (filter->rule_config != NULL &&
                (filter->rule_config->pbmp_type != BCMFP_NONE)) {
            ports_bmp_present = TRUE;
            ports_key =
                &(filter->rule_config->pbmp_data);
            ports_mask =
                &(filter->rule_config->pbmp_mask);
            switch (filter->rule_config->pbmp_type)  {
                case BCMFP_INPORTS:
                    ports_bmp_fid = stage->tbls.rule_tbl->inports_key_fid;
                    break;
                case BCMFP_SYSTEM_PORTS:
                    ports_bmp_fid = stage->tbls.rule_tbl->system_ports_key_fid;
                    break;
                case BCMFP_DEVICE_PORTS:
                    ports_bmp_fid = stage->tbls.rule_tbl->device_ports_key_fid;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                    break;
            }
            ports_bmp_qualifier =
                qualifiers_fid_info[ports_bmp_fid].qualifier;
        }
        for (pipe_id = pipe_min; pipe_id <= pipe_max; pipe_id++) {
            if (pipe_id != pipe_min) {
                eflags |= BCMFP_ENTRY_SAME_KEY;
            }
            /*
             * Calculate ekw/edw for each pipe if the PBMP type qualifier
             * is present in the rule.
             */
            if (ports_bmp_present == TRUE) {
                BCMDRD_PBMP_CLEAR(pbmp_key);
                BCMDRD_PBMP_CLEAR(pbmp_mask);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_device_ipbm_to_pipe_ipbmp(unit,
                                                     ports_key,
                                                     pipe_id,
                                                     &pbmp_key));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_device_ipbm_to_pipe_ipbmp(unit,
                                                     ports_mask,
                                                     pipe_id,
                                                     &pbmp_mask));

                if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_IPBM_SUPPORT)) {
                    BCMDRD_PBMP_XOR(pbmp_mask, pbmp_key);
                    BCMDRD_PBMP_CLEAR(pbmp_key);
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_qual_set(unit,
                                    stage_id,
                                    group_id,
                                    filter->group_oper_info,
                                    0,
                                    ports_bmp_qualifier,
                                    ports_bmp_qualifier,
                                    0,
                                    NULL,
                                    pbmp_key.w,
                                    pbmp_mask.w,
                                    ekw));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_entry_insert(unit,
                                        eflags,
                                        op_arg,
                                        pipe_id,
                                        stage_id,
                                        tbl_id,
                                        group_id,
                                        filter->group_oper_info,
                                        entry_id,
                                        entry_prio,
                                        ekw,
                                        edw,
                                        &profiles,
                                        enable));
        }
    }
exit:
    bcmfp_filter_buffers_free(unit, fp_buffers);
    SHR_FREE(fp_buffers);
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_insert_hw(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t tbl_id,
                    bcmfp_stage_t *stage,
                    bool is_presel,
                    uint16_t presel_idx,
                    bcmfp_filter_t *filter)
{
    int pipe_id = -1;
    bcmfp_entry_profiles_t profiles;
    bcmfp_group_id_t group_id = 0;
    bcmfp_entry_id_t entry_id = 0;
    uint32_t entry_prio = 0;
    size_t size = 0;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    uint32_t eflags = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_stage_id_t stage_id;
    bcmfp_stage_oper_mode_t oper_mode = 0;
    bcmfp_pse_config_t *pse_config = NULL;
    bool enable = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    if (is_presel == FALSE &&
        BCMFP_STAGE_FLAGS_TEST(stage,
                               BCMFP_STAGE_HW_FLOW_TRACKER)) {
        SHR_EXIT();
    }

    pipe_id = filter->group_config->pipe_id;
    group_id = filter->group_config->group_id;


    stage_id = stage->stage_id;
    ekw = filter->buffers.ekw;
    edw = filter->buffers.edw;
    size = sizeof(bcmfp_entry_profiles_t);
    sal_memset(&profiles, 0, size);
    if (is_presel == FALSE) {
        profiles.pdd = filter->buffers.pdd;
        profiles.psg = filter->buffers.psg;
        profiles.sbr = filter->buffers.sbr;
    }

    if (is_presel == TRUE) {
       eflags |= BCMFP_ENTRY_PRESEL;
       if (!filter->group_config->num_presels) {
           eflags |= BCMFP_ENTRY_PRESEL_DEFAULT;
           entry_id = group_id;
           entry_prio = 0;
           enable = filter->group_config->enable;
       } else {
           /* coverity[deref_parm] */
           pse_config = filter->pse_config[presel_idx];
           entry_id = pse_config->pse_id;
           entry_prio = pse_config->priority;
           enable = pse_config->enable;
       }
    } else  {
        entry_id = filter->entry_config->entry_id;
        entry_prio = filter->entry_config->priority;
        enable = filter->entry_config->enable;
    }

    opinfo = filter->group_oper_info;
    if (opinfo->ref_count == 0 &&
        is_presel == FALSE) {
        eflags |= BCMFP_ENTRY_FIRST_IN_GROUP;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_mode_get(unit,
                                   stage_id,
                                   &oper_mode));

    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_KEY_TYPE_PRESEL) &&
        BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_INPORTS_SUPPORT) &&
        (oper_mode != BCMFP_STAGE_OPER_MODE_GLOBAL) &&
        (is_presel == FALSE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_insert_hw_perpipe(unit,
                                            op_arg,
                                            tbl_id,
                                            stage,
                                            filter));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_entry_insert(unit,
                                eflags,
                                op_arg,
                                pipe_id,
                                stage_id,
                                tbl_id,
                                group_id,
                                filter->group_oper_info,
                                entry_id,
                                entry_prio,
                                ekw,
                                edw,
                                &profiles,
                                enable));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_delete_hw(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_t *stage,
                       bcmfp_filter_t *filter)
{
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_COUNT;
    bcmfp_group_id_t group_id = 0;
    bcmfp_entry_id_t entry_id = 0;
    bcmfp_group_oper_info_t *op_info = NULL;
    uint32_t **ekw = NULL;
    uint32_t flags = 0;
    uint16_t num_presels = 0;
    bcmfp_pse_id_t pse_id = 0;
    uint16_t idx = 0;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_rule_config_t *rule_config = NULL;
    bcmltd_sid_t pse_tbl_id;
    bcmfp_entry_profiles_t entry_profiles;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    stage_id = stage->stage_id;
    group_id = filter->entry_config->group_id;
    op_info = filter->group_oper_info;
    entry_id = filter->entry_config->entry_id;
    group_config = filter->group_config;
    rule_config = filter->rule_config;
    ekw = filter->buffers.ekw;
    sal_memset(&entry_profiles, 0, sizeof(bcmfp_entry_profiles_t));
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_em_entry_build_ekw(unit,
                                      op_arg,
                                      tbl_id,
                                      stage_id,
                                      group_id,
                                      filter->group_oper_info,
                                      entry_id,
                                      group_config,
                                      rule_config,
                                      ekw));

    }
    if (op_info->ref_count > 1) {
        SHR_IF_ERR_EXIT
            (bcmfp_ptm_entry_delete(unit,
                                    flags,
                                    op_arg,
                                    stage_id,
                                    tbl_id,
                                    group_id,
                                    filter->group_oper_info,
                                    entry_id,
                                    ekw,
                                    &entry_profiles));
       filter->sbr_profile_index = entry_profiles.sbr_index;
       SHR_EXIT();
    }

    flags = BCMFP_ENTRY_LAST_IN_GROUP;
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_KEY_TYPE_PRESEL)) {
        pse_tbl_id = stage->tbls.pse_tbl->sid;
        flags |= BCMFP_ENTRY_PRESEL;
        num_presels = group_config->num_presels;
        for (idx = 0; idx < num_presels; idx++) {
            if (group_config->presel_ids[idx] == 0) {
                continue;
            }
            pse_id = group_config->presel_ids[idx];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_entry_delete(unit,
                                        flags,
                                        op_arg,
                                        stage_id,
                                        pse_tbl_id,
                                        group_id,
                                        filter->group_oper_info,
                                        pse_id,
                                        NULL,
                                        &entry_profiles));
        }

        if (num_presels == 0) {
            flags |= BCMFP_ENTRY_PRESEL_DEFAULT;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_entry_delete(unit,
                                        flags,
                                        op_arg,
                                        stage_id,
                                        pse_tbl_id,
                                        group_id,
                                        filter->group_oper_info,
                                        group_id,
                                        NULL,
                                        &entry_profiles));
        }
    }

    if (!BCMFP_STAGE_FLAGS_TEST(stage,
                                BCMFP_STAGE_HW_FLOW_TRACKER)) {
        flags = BCMFP_ENTRY_LAST_IN_GROUP;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_entry_delete(unit,
                                    flags,
                                    op_arg,
                                    stage_id,
                                    tbl_id,
                                    group_id,
                                    filter->group_oper_info,
                                    entry_id,
                                    ekw,
                                    &entry_profiles));
       filter->sbr_profile_index = entry_profiles.sbr_index;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_group_install_flags_get(int unit,
                           bcmfp_stage_t *stage,
                           bcmfp_filter_t *filter,
                           uint32_t *gi_flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(gi_flags, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_KEY_TYPE_PRESEL)) {
        *gi_flags |= BCMFP_GROUP_INSTALL_KEYGEN_PROFILE;
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
        *gi_flags |= BCMFP_GROUP_INSTALL_KEY_ATTRIB_PROFILE;
        *gi_flags |= BCMFP_GROUP_INSTALL_PSG_PROFILE;
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_POLICY_TYPE_PDD) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_PDD_PROFILES_GLOBAL) &&
        filter->group_config->pdd_id != 0) {
        *gi_flags |= BCMFP_GROUP_INSTALL_PDD_PROFILE;
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_RESOLUTION_SBR) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_SBR_PROFILES_GLOBAL) &&
        filter->group_config->sbr_id != 0) {
        *gi_flags |= BCMFP_GROUP_INSTALL_SBR_PROFILE;
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_DEFAULT_POLICY) &&
        filter->group_config->default_policy_id != 0) {
        *gi_flags |= BCMFP_GROUP_INSTALL_DPOLICY;
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_POLICY_TYPE_PDD) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_DEFAULT_POLICY) &&
        filter->group_config->default_pdd_id != 0) {
        *gi_flags |= BCMFP_GROUP_INSTALL_DPDD_PROFILE;
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_RESOLUTION_SBR) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_DEFAULT_POLICY) &&
        filter->group_config->default_sbr_id != 0) {
        *gi_flags |= BCMFP_GROUP_INSTALL_DSBR_PROFILE;
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_QOS_PROFILE) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_DEFAULT_POLICY) &&
        filter->group_config->default_policy_id != 0) {
        *gi_flags |= BCMFP_GROUP_INSTALL_DQOS_PROFILE;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_group_config_process(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t tbl_id,
                                  bcmfp_stage_t *stage,
                                  bcmfp_filter_t *filter)
{
    uint32_t gi_flags = 0;
    bcmfp_group_id_t group_id = 0;
    bcmfp_buffers_t *buffers = NULL;
    bcmltd_sid_t sbr_req_ltid;
    bcmfp_group_oper_info_t *opinfo = NULL;
    uint8_t num_parts = 0;
    uint8_t part = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_group_install_flags_get(unit,
                                             stage,
                                             filter,
                                             &gi_flags));
    if (gi_flags == 0) {
        SHR_EXIT();
    }

    opinfo = filter->group_oper_info;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 TRUE,
                                 opinfo->flags,
                                 &num_parts));

    buffers = &(filter->buffers);
    group_id = filter->entry_config->group_id;

    if ((opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_SBR_GROUP) &&
        (gi_flags & BCMFP_GROUP_INSTALL_SBR_PROFILE)) {
        sbr_req_ltid = stage->tbls.sbr_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_profile_install(unit,
                                       op_arg,
                                       opinfo->tbl_inst,
                                       sbr_req_ltid,
                                       stage->stage_id,
                                       group_id,
                                       0,
                                       buffers->sbr,
                                       num_parts,
                                       &(filter->sbr_profile_index),
                                       filter->group_config->sbr_id));

        opinfo->sbr_id = filter->sbr_profile_index;
        for (part = 0; part < num_parts; part++) {
             opinfo->ext_codes[part].sbr_prof_index = opinfo->sbr_id;
        }
    }
    if (gi_flags & BCMFP_GROUP_INSTALL_DSBR_PROFILE) {
         sbr_req_ltid = stage->tbls.sbr_tbl->sid;
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_sbr_profile_install(unit,
                                        op_arg,
                                        opinfo->tbl_inst,
                                        sbr_req_ltid,
                                        stage->stage_id,
                                        group_id,
                                        0,
                                        buffers->dsbr,
                                        num_parts,
                                        &(filter->dsbr_profile_index),
                                        filter->group_config->default_sbr_id));
        opinfo->def_sbr_id = filter->dsbr_profile_index;
        for (part = 0; part < num_parts; part++) {
            opinfo->ext_codes[part].default_sbr_prof_index = opinfo->def_sbr_id;
        }
    }

    /*
     * default_policy_enable is to be set if
     * the group_config'sdefault policy is not NULL
     */
    if (filter->group_config->default_policy_id != 0) {
        for (part = 0; part < num_parts; part++) {
             opinfo->ext_codes[part].default_policy_enable = 1;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_profiles_install(unit,
                                op_arg,
                                gi_flags,
                                stage,
                                group_id,
                                filter->group_oper_info,
                                buffers));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_entry_config_process(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t tbl_id,
                                  bcmfp_stage_t *stage,
                                  bcmfp_filter_t *filter)
{
    uint32_t gi_flags = 0;
    bcmfp_group_id_t group_id = 0;
    bcmfp_buffers_t *buffers = NULL;
    bcmltd_sid_t sbr_req_ltid;
    bcmfp_group_oper_info_t *opinfo = NULL;
    uint8_t num_parts = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    opinfo = filter->group_oper_info;
    buffers = &(filter->buffers);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 TRUE,
                                 opinfo->flags,
                                 &num_parts));

    if ((opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_GROUP_SBR_ENTRY) &&
        (filter->entry_config->sbr_id != 0) &&
        (filter->sbr_dont_install == FALSE)) {
        sbr_req_ltid = stage->tbls.sbr_tbl->sid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_profile_install(unit,
                                       op_arg,
                                       opinfo->tbl_inst,
                                       sbr_req_ltid,
                                       stage->stage_id,
                                       0,
                                       filter->entry_config->entry_id,
                                       buffers->sbr,
                                       num_parts,
                                       &(filter->sbr_profile_index),
                                       filter->entry_config->sbr_id));
    }
    if (!BCMFP_STAGE_FLAGS_TEST(stage,
            BCMFP_STAGE_ACTION_QOS_PROFILE)) {
        SHR_EXIT();
    }

    if (filter->entry_config->policy_id != 0) {
        gi_flags = BCMFP_GROUP_INSTALL_QOS_PROFILE;
    }

    if (gi_flags == 0) {
        SHR_EXIT();
    }

    group_id = filter->entry_config->group_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_profiles_install(unit,
                                op_arg,
                                gi_flags,
                                stage,
                                group_id,
                                filter->group_oper_info,
                                buffers));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_rule_config_process(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t tbl_id,
                              bcmfp_stage_t *stage,
                              bcmfp_filter_t *filter)
{
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;
    bcmfp_group_id_t group_id = 0;
    uint32_t *bitmap = NULL;
    uint16_t pipe_id = 0, part = 0;
    bcmlrd_fid_t ports_bmp_fid = 0;
    bcmfp_qualifier_t ports_bmp_qualifier = 0;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    bool ports_bmp_present = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_NULL_CHECK(filter->rule_config, SHR_E_INTERNAL);
    SHR_NULL_CHECK(filter->group_config, SHR_E_INTERNAL);

    qual_data = filter->rule_config->qual_data;
    group_id = filter->group_config->group_id;

    qualifiers_fid_info =
            stage->tbls.rule_tbl->qualifiers_fid_info;
    while (qual_data != NULL) {

        if (BCMFP_STAGE_FLAGS_TEST(stage,
                                   BCMFP_STAGE_IPBM_PARTS_SUPPORT)) {
            ports_bmp_present = FALSE;
            for (pipe_id = 0; pipe_id < BCMFP_MAX_IPBM_PIPES; pipe_id++) {
                for (part = 0; part < BCMFP_MAX_IPBM_PARTS; part++) {
                    ports_bmp_fid
                        = stage->tbls.rule_tbl->ipbm_key_fid[part][pipe_id];
                    ports_bmp_qualifier =
                        qualifiers_fid_info[ports_bmp_fid].qualifier;
                    if (ports_bmp_qualifier == qual_data->qual_id) {
                        ports_bmp_present = TRUE;
                        break;
                    }
                }
            }
            if (ports_bmp_present == TRUE) {
                qual_data = qual_data->next;
                continue;
            }
        }

        /*
         * Get the qualifier bitmap info from FILTER
         * group config.
         */
        qual_bitmap = filter->group_config->qual_bitmap;
        while (qual_bitmap != NULL) {
            if (qual_bitmap->qual ==
                qual_data->qual_id) {
                break;
            }
            qual_bitmap = qual_bitmap->next;
        }
        /* Qualifier is not present in the group. */
        if (qual_bitmap == NULL) {
            qual_data = qual_data->next;
            continue;
        }

        /* if partial get the bitmap */
        if (qual_bitmap->partial) {
            bitmap = qual_bitmap->bitmap;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_qual_set(unit,
                            stage->stage_id,
                            group_id,
                            filter->group_oper_info,
                            qual_data->flags,
                            qual_data->qual_id,
                            qual_data->fid,
                            qual_data->fidx,
                            bitmap,
                            qual_data->key,
                            qual_data->mask,
                            filter->buffers.ekw));
        qual_data = qual_data->next;
        bitmap = NULL;
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_KEY_TYPE_FIXED)) {
        bcmfp_group_type_t group_type;
        bcmfp_group_slice_mode_t slice_mode;
        group_type = filter->group_config->type;
        slice_mode = filter->group_config->slice_mode;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_key_match_type_set(unit,
                                            stage,
                                            group_id,
                                            filter->group_oper_info,
                                            slice_mode,
                                            group_type,
                                            filter->buffers.ekw));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_policy_config_process(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t tbl_id,
                                bool is_default,
                                bcmfp_stage_t *stage,
                                bcmfp_filter_t *filter)
{
    bcmfp_group_id_t group_id = 0;
    bcmfp_pdd_id_t pdd_id = 0;
    bcmfp_pdd_oper_type_t pdd_type;
    bcmfp_action_data_t *action_data = NULL;
    bcmfp_action_data_t *action_data_part[BCMFP_ACTION_MAX_PARTS];
    uint32_t **edw = NULL;
    uint32_t **qos = NULL;
    uint8_t part_id = 0;
    bcmfp_action_viewtype_t viewtype = BCMFP_ACTION_VIEWTYPE_DEFAULT;
    bcmfp_action_cfg_t *action_cfg = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    if (is_default == FALSE) {
        SHR_NULL_CHECK(filter->policy_config, SHR_E_INTERNAL);
    } else {
        SHR_NULL_CHECK(filter->dpolicy_config, SHR_E_INTERNAL);
    }
    SHR_NULL_CHECK(filter->group_config, SHR_E_INTERNAL);

    group_id = filter->group_config->group_id;
    if (is_default == FALSE) {
        action_data = filter->policy_config->action_data;
        action_data_part[0] = filter->policy_config->action_data;
        action_data_part[1] = filter->policy_config->action_data1;
        action_data_part[2] = filter->policy_config->action_data2;
        pdd_id = filter->group_config->pdd_id;
        pdd_type = filter->group_oper_info->pdd_type;
        edw = filter->buffers.edw;
        qos = filter->buffers.qos;
        viewtype = filter->policy_config->viewtype;
    } else {
        action_data = filter->dpolicy_config->action_data;
        pdd_id = filter->group_config->default_pdd_id;
        pdd_type = BCMFP_PDD_OPER_TYPE_DEFAULT_POLICY;
        edw = filter->buffers.dedw;
        qos = filter->buffers.dqos;
        viewtype = filter->dpolicy_config->viewtype;
    }

    /*
     * Populating non PDD stage FP Filters edw
     * with multiple views supported on
     * policy table
     */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
            BCMFP_STAGE_POLICY_MULTI_VIEW) &&
        !(BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_POLICY_TYPE_PDD))) {
        for (part_id = 0; part_id < BCMFP_ACTION_MAX_PARTS; part_id++) {
            action_data = action_data_part[part_id];
            while (action_data != NULL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_action_viewtype_set(unit,
                                               stage->stage_id,
                                               group_id,
                                               action_data->action,
                                               action_data->param,
                                               edw,
                                               viewtype,
                                               part_id));
                action_data = action_data->next;
            }
        }
    }
    /*
     * Populating non PDD stype FP FILTERs edw with
     * a single viewtype in policy table
     */
    if (!(BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_POLICY_TYPE_PDD)) &&
        !(BCMFP_STAGE_FLAGS_TEST(stage,
                BCMFP_STAGE_POLICY_MULTI_VIEW))) {
        for (part_id = 0; part_id < BCMFP_ACTION_MAX_PARTS; part_id++) {
            action_data = action_data_part[part_id];
            while (action_data != NULL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_action_set(unit,
                                      stage->stage_id,
                                      group_id,
                                      action_data->action,
                                      action_data->param,
                                      edw,
                                      part_id));
                action_data = action_data->next;
            }
        }
    }
    /* Populating non HASH based and PDD stype FP FILTERs edw */
    if ((BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_POLICY_TYPE_PDD)) &&
        !(BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_ENTRY_TYPE_HASH))) {
        while (action_data != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_action_set(unit,
                                      stage->stage_id,
                                      pdd_id,
                                      group_id,
                                      pdd_type,
                                      filter->group_oper_info,
                                      action_data->action,
                                      action_data->param,
                                      edw));
            action_data = action_data->next;
        }
    }

    /*
     * Populating HASH based FP(EM) FILTERs edw.
     * Note: All HASH based FPs have PDD stype policy.
     */
    if ((BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_POLICY_TYPE_PDD)) &&
        (BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_ENTRY_TYPE_HASH))) {
        while (action_data != NULL) {
            action_cfg = stage->action_cfg_db->action_cfg[action_data->action];
            /*!
             * ACTION_METER_ENABLE and ACTION_CTR_ENABLE
             * are added as dummy actions in the policy temlplate LT
             * of EM. These are added to get the offsets of the
             * meter/ctr action set in policy. And also to have 1:1
             * correspondance between the PDD temaplate actions
             * and the policy template actions. These actions
             * are initialized with the BCMFP_ACTIONS_SOFT flag
             * so that the edw programming can be skipped for these
             * actions.
             */
            if (action_cfg != NULL &&
                    !(action_cfg->flags & BCMFP_ACTION_SOFT)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_em_action_set(unit,
                                         stage->stage_id,
                                         group_id,
                                         pdd_type,
                                         filter->group_oper_info,
                                         pdd_id,
                                         action_data->action,
                                         action_data->param,
                                         edw,
                                         qos));
            }
            action_data = action_data->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_pdd_config_process(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t tbl_id,
                             bool is_default,
                             bcmfp_stage_t *stage,
                             bcmfp_filter_t *filter)
{
    bcmfp_pdd_id_t pdd_id = 0;
    bcmfp_pdd_oper_type_t pdd_type;
    uint32_t **pdd = NULL;
    bcmfp_pdd_config_t *pdd_config = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_NULL_CHECK(filter->group_config, SHR_E_INTERNAL);
    if (is_default == true) {
        pdd_id = filter->group_config->default_pdd_id;
        pdd = filter->buffers.dpdd;
        pdd_type = BCMFP_PDD_OPER_TYPE_DEFAULT_POLICY;
        pdd_config = filter->dpdd_config;
    } else {
        pdd_id = filter->group_config->pdd_id;
        pdd = filter->buffers.pdd;
        pdd_type = filter->group_oper_info->pdd_type;
        pdd_config = filter->pdd_config;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_profile_entry_get(unit,
                                     op_arg,
                                     stage,
                                     pdd_id,
                                     filter->group_config->group_id,
                                     filter->group_oper_info,
                                     pdd_type,
                                     pdd,
                                     pdd_config));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_sbr_priority_set(int unit,
                       bcmfp_stage_t *stage,
                       uint16_t start_bit,
                       uint16_t end_bit,
                       uint8_t part_id,
                       uint32_t priority,
                       uint32_t **sbr_profile_data)
{
    uint16_t idx = 0;
    uint16_t sbr_profile_sid_columns = 0;
    uint16_t offset_idx = 0;
    uint16_t column_width = 0;
    uint16_t column_start_bit = 0;
    uint16_t column_end_bit = 0;
    bcmfp_stage_hw_field_info_t *field_info = NULL;
    uint16_t index = 0;
    uint16_t column = 0;
    uint32_t *profile_data = NULL;
    uint16_t bit_pos = 0;
    uint16_t column_seg_bit = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sbr_profile_data, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* Identify the column. */
    sbr_profile_sid_columns =
        stage->sbr_hw_info->sbr_profile_sid_columns;
    field_info =
        stage->sbr_hw_info->priority_field_info;
    for (idx = 0; idx < sbr_profile_sid_columns; idx++) {
        for (offset_idx = 0;
            offset_idx < field_info[idx].num_offsets;
            offset_idx++) {
            column_width +=
                field_info[offset_idx].width[offset_idx];
        }
        if (start_bit <= column_width &&
            end_bit <= column_width) {
            column = idx;
            break;
        }
    }

    /* Identify the star_bit and end_bit in a column. */
    column_start_bit = start_bit;
    column_end_bit = end_bit;
    for (idx = 0; idx < column; idx++) {
        for (offset_idx = 0;
            offset_idx < field_info[idx].num_offsets;
            offset_idx++) {
            column_start_bit -=
                field_info[offset_idx].width[offset_idx];
            column_end_bit -=
                field_info[offset_idx].width[offset_idx];
        }
    }

    /* Set the priority in right segments of column. */
    index = ((part_id * sbr_profile_sid_columns) + column);
    field_info = &(stage->sbr_hw_info->priority_field_info[column]);
    profile_data = sbr_profile_data[index];
    column_seg_bit = column_start_bit;
    for (idx = column_start_bit;
         idx <= column_end_bit;
         idx++) {
        for (offset_idx = 0;
             offset_idx < field_info->num_offsets;
             offset_idx++) {
            if ((field_info->offset[offset_idx] +
                field_info->width[offset_idx] - 1) <
                idx) {
                if ((offset_idx + 1) >=
                    field_info->num_offsets) {
                     SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                column_seg_bit = idx +
                    (field_info->offset[offset_idx + 1] -
                    (field_info->offset[offset_idx] +
                     field_info->width[offset_idx] - 1) - 1);
            } else {
                break;
            }
        }
        /* coverity[address_of] */
        /* coverity[ptr_arith] */
        if (SHR_BITGET(&priority, bit_pos)) {
            SHR_BITSET(profile_data, column_seg_bit);
        }
        bit_pos++;
        column_seg_bit++;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_sbr_config_process(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t tbl_id,
                             bool is_default,
                             bcmfp_stage_t *stage,
                             bcmfp_filter_t *filter)
{
    uint8_t part_id = 0;
    uint32_t start_bit = 0;
    uint32_t end_bit = 0;
    uint16_t idx = 0;
    uint16_t phy_cont_id = 0;
    uint16_t sbr_phy_cont_id = 0;
    bcmfp_sbr_id_t pdd_id = 0;
    bcmfp_action_data_t *action_data = NULL;
    bcmfp_action_cfg_t *action_cfg = NULL;
    bcmfp_action_offset_cfg_t *offset_cfg = NULL;
    uint32_t *pdd_bitmap[BCMFP_ENTRY_PARTS_MAX] = { NULL };
    uint32_t **sbr = NULL;
    uint8_t num_parts = 0;
    uint8_t keygen_parts = 0;
    uint8_t sbr_part_id = 0;
    bcmfp_pdd_oper_type_t pdd_type;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_NULL_CHECK(filter->sbr_config, SHR_E_INTERNAL);

    if (is_default == true) {
        pdd_id = filter->group_config->default_pdd_id;
    } else {
        pdd_id = filter->group_config->pdd_id;
    }

    if (pdd_id == 0) {
        SHR_EXIT();
    }
    if (filter->pdd_config == NULL) {
        SHR_EXIT();
    }

    opinfo = filter->group_oper_info;
    if (is_default == true) {
        pdd_type = BCMFP_PDD_OPER_TYPE_DEFAULT_POLICY;
    } else {
        pdd_type = opinfo->pdd_type;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_num_parts_get(unit,
                                 stage->stage_id,
                                 pdd_id,
                                 pdd_type,
                                 &num_parts));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &keygen_parts));

    for (part_id = 0; part_id < num_parts; part_id++) {
        BCMFP_ALLOC(pdd_bitmap[part_id],
                    sizeof(uint32_t) * BCMFP_MAX_WSIZE,
                    "bcmfpPddBitmapForSbrData");
    }

    for (part_id = 0; part_id < num_parts; part_id++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_bitmap_get(unit,
                                  stage->stage_id,
                                  pdd_id,
                                  part_id,
                                  pdd_type,
                                  pdd_bitmap[part_id]));
    }

    if (is_default == true) {
        action_data = filter->dsbr_config->action_data;
        sbr = filter->buffers.dsbr;
    } else {
        action_data = filter->sbr_config->action_data;
        sbr = filter->buffers.sbr;
    }
    while (action_data != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_action_cfg_get(unit,
                                  stage->stage_id,
                                  action_data->action,
                                  &action_cfg));
        if (action_cfg->flags &
            BCMFP_ACTION_SBR_NONE) {
            action_data = action_data->next;
            continue;
        }
        offset_cfg = &(action_cfg->offset_cfg);
        idx = 0;
        while (offset_cfg->width[idx] != 0) {
            phy_cont_id = offset_cfg->phy_cont_id[idx];
            sbr_phy_cont_id = offset_cfg->sbr_phy_cont_id[idx];
            for (part_id = 0;
                part_id < num_parts;
                part_id++) {
                if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE)) {
                    sbr_part_id = (keygen_parts - 1) - part_id;
                } else {
                    sbr_part_id = part_id;
                }
                if (SHR_BITGET(pdd_bitmap[part_id],
                               phy_cont_id)) {
                    start_bit = sbr_phy_cont_id * 3;
                    end_bit = start_bit + 2;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmfp_filter_sbr_priority_set(unit,
                                                stage,
                                                start_bit,
                                                end_bit,
                                                sbr_part_id,
                                                action_data->param,
                                                sbr));
                    break;
                }
            }
            idx++;
        }

        action_data = action_data->next;
    }
    /*
     * The actions which are of type BCMFP_ACTION_SBR_INTERNAL (the actions
     * that are mapped to SBR physical containers but not exposed in SBR LT) are
     * extracted
     * The SBR priority for these actions is set to BCMFP_SBR_INTERNAL_PRI
     */
    if (is_default == true) {
        action_data = filter->dpdd_config->action_data;
    } else {
        action_data = filter->pdd_config->action_data;
    }
    while (action_data != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_action_cfg_get(unit,
                                  stage->stage_id,
                                  action_data->action,
                                  &action_cfg));
        if (!(action_cfg->flags &
            BCMFP_ACTION_SBR_INTERNAL)) {
            action_data = action_data->next;
            continue;
        }
        offset_cfg = &(action_cfg->offset_cfg);
        idx = 0;
        while (offset_cfg->width[idx] != 0) {
            phy_cont_id = offset_cfg->phy_cont_id[idx];
            sbr_phy_cont_id = offset_cfg->sbr_phy_cont_id[idx];
            for (part_id = 0;
                part_id < num_parts;
                part_id++) {
                if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE)) {
                    sbr_part_id = (keygen_parts - 1) - part_id;
                } else {
                    sbr_part_id = part_id;
                }
                if (SHR_BITGET(pdd_bitmap[part_id],
                               phy_cont_id)) {
                    start_bit = sbr_phy_cont_id * 3;
                    end_bit = start_bit + 2;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmfp_filter_sbr_priority_set(unit,
                                                stage,
                                                start_bit,
                                                end_bit,
                                                sbr_part_id,
                                                BCMFP_SBR_INTERNAL_PRI,
                                                sbr));
                    break;
                }
            }
            idx++;
        }

        action_data = action_data->next;
    }


exit:
    for (part_id = 0; part_id < num_parts; part_id++) {
        SHR_FREE(pdd_bitmap[part_id]);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_sbr_config_update_process(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t tbl_id,
                             bool is_default,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_filter_t *filter)
{
    bcmfp_stage_t *stage = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (filter->sbr_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_sbr_config_process(unit,
                                             op_arg,
                                             tbl_id,
                                             is_default,
                                             stage,
                                             filter));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_psg_config_process(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t tbl_id,
                             bcmfp_stage_t *stage,
                             bcmfp_filter_t *filter)
{
    uint32_t psg_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_NULL_CHECK(filter->psg_config, SHR_E_INTERNAL);

    psg_id =
        filter->psg_config->psg_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_info_get(unit,
                                     stage->stage_id,
                                     psg_id,
                                     filter->buffers.psg));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_pse_config_process(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t tbl_id,
                             bcmfp_stage_t *stage,
                             uint8_t presel_idx,
                             bcmfp_filter_t *filter)
{
    bcmfp_group_id_t group_id = 0;
    bcmfp_pse_config_t *pse_config = NULL;
    bcmfp_qual_data_t *qual_data = NULL;
    bcmfp_action_data_t *action_data = NULL;
    bcmfp_qual_bitmap_t *qbitmap = NULL;
    uint32_t *bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    pse_config =
        filter->pse_config[presel_idx];
    if (pse_config == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    group_id =
        filter->group_config->group_id;
    if (BCMFP_STAGE_FLAGS_TEST(stage,
            BCMFP_STAGE_PRESEL_KEY_DYNAMIC)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_presel_edw_get(unit,
                                  stage->stage_id,
                                  group_id,
                                  filter->group_oper_info,
                                  filter->buffers.edw));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_presel_edw_get(unit,
                                  stage->stage_id,
                                  group_id,
                                  filter->group_oper_info,
                                  filter->buffers.ekw));
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
            BCMFP_STAGE_PRESEL_KEY_DYNAMIC)) {
        group_id =
            filter->group_config->psg_id;
    }

    qual_data = pse_config->qual_data;
    while (qual_data != NULL) {
        if (BCMFP_STAGE_FLAGS_TEST(stage,
                  BCMFP_STAGE_PRESEL_KEY_DYNAMIC)) {
            qbitmap = filter->psg_config->qual_bitmap;
            while (qbitmap != NULL) {
                if ((qbitmap->fid  + qbitmap->fidx) ==
                    (qual_data->fid + qual_data->fidx)) {
                    break;
                }
                qbitmap = qbitmap->next;
            }
            if (qbitmap == NULL) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            bitmap = qbitmap->bitmap;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_qual_presel_set(unit,
                                   stage->stage_id,
                                   group_id,
                                   filter->group_oper_info,
                                   qual_data->qual_id,
                                   qual_data->fid,
                                   qual_data->fidx,
                                   bitmap,
                                   qual_data->key,
                                   qual_data->mask,
                                   filter->buffers.ekw));
        qual_data = qual_data->next;
    }
    action_data = pse_config->action_data;
    while (action_data != NULL) {
        if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_PRESEL_KEY_DYNAMIC)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_presel_action_set(unit,
                                         stage->stage_id,
                                         action_data->action,
                                         action_data->param,
                                         action_data->flags,
                                         filter->group_oper_info,
                                         filter->buffers.edw));
            action_data = action_data->next;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_presel_action_set(unit,
                                         stage->stage_id,
                                         action_data->action,
                                         action_data->param,
                                         action_data->flags,
                                         filter->group_oper_info,
                                         filter->buffers.ekw));
            action_data = action_data->next;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_meter_config_process(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t tbl_id,
                                  bcmfp_stage_t *stage,
                                  bcmfp_filter_t *filter,
                                  bool first_phase,
                                  bool *reinsert_filter,
                                  bool is_default)
{
    int rv;
    bcmcth_meter_fp_policy_fields_t meter_entry;
    size_t size = 0;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_COUNT;
    bcmfp_group_id_t group_id = 0;
    bcmfp_meter_id_t meter_id = 0;
    bcmfp_pdd_id_t pdd_id = 0;
    uint32_t **edw = NULL;
    bool meters_global = FALSE;
    uint8_t slice_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);
    SHR_NULL_CHECK(reinsert_filter, SHR_E_PARAM);

    if (is_default == TRUE) {
        meter_id = filter->group_config->default_meter_id;
        pdd_id = filter->group_config->default_pdd_id;
        edw = filter->buffers.dedw;
    } else {
        meter_id = filter->entry_config->meter_id;
        pdd_id = filter->group_config->pdd_id;
        edw = filter->buffers.edw;
    }
    group_id = filter->entry_config->group_id;
    stage_id = stage->stage_id;
    *reinsert_filter = FALSE;

    /* Do nothing if meter ID is not specified. */
    if (meter_id == 0) {
        SHR_EXIT();
    }

    /* Do nothing if meter ID does not exist. */
    rv = bcmcth_meter_entry_valid(unit, stage_id, meter_id);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    opinfo = filter->group_oper_info;
    if (first_phase == TRUE && (opinfo->ref_count == 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_pool_global(unit,
                                      stage_id,
                                      &meters_global));

        if (meters_global == FALSE) {
            *reinsert_filter = TRUE;
            SHR_EXIT();
        }
    }

    if (first_phase == FALSE && (opinfo->ref_count == 0)) {
        slice_id = opinfo->slice_id;
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmcth_meter_entry_insert(unit,
                                      op_arg->trans_id,
                                      stage_id,
                                      slice_id,
                                      meter_id));
    }

    size = sizeof(bcmcth_meter_fp_policy_fields_t);
    sal_memset(&meter_entry, 0, size);
    /*
     * Lookup should not fail as meter id presence
     * is confirmed above.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_entry_lookup(unit,
                                   stage_id,
                                   meter_id,
                                   &meter_entry));

    /* Update the FP entry edw with meter parameters. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_meter_actions_set(unit,
                                 stage,
                                 group_id,
                                 pdd_id,
                                 filter->group_oper_info,
                                 &meter_entry,
                                 edw));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_ctr_config_process(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t tbl_id,
                                bcmfp_stage_t *stage,
                                bcmfp_filter_t *filter,
                                bool is_default)
{
    bcmfp_ctr_params_t ctr_params;
    bcmfp_group_id_t group_id = 0;
    bcmfp_pdd_id_t pdd_id = 0;
    bcmfp_policy_config_t *policy_config = NULL;
    uint32_t **edw=NULL;


    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    if (filter->group_config != NULL) {
        group_id = filter->group_config->group_id;
        if (is_default == TRUE) {
            pdd_id = filter->group_config->default_pdd_id;
        } else {
            pdd_id = filter->group_config->pdd_id;
        }
    }

    if (is_default == TRUE) {
        policy_config = filter->dpolicy_config;
        edw = filter->buffers.dedw;
    } else {
        policy_config = filter->policy_config;
        edw = filter->buffers.edw;
    }

    sal_memset(&ctr_params, 0, sizeof(bcmfp_ctr_params_t));
    if (BCMFP_STAGE_FLAGS_TEST(stage,
             BCMFP_STAGE_LEGACY_CTR_SUPPORT)) {
       if (is_default == TRUE) {
           SHR_EXIT();
       }
       if (filter->ctr_entry_config != NULL) {
           ctr_params.ctr_id =
               filter->ctr_entry_config->ctr_entry_id;
           ctr_params.ctr_pipe_id =
               filter->ctr_entry_config->pipe_id;
           ctr_params.ctr_pool_id =
               filter->ctr_entry_config->pool_id;
           ctr_params.ctr_color =
              filter->ctr_entry_config->color;
       } else {
           SHR_EXIT();
       }
    } else if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_FLEX_CTR_SUPPORT)) {
        if (is_default == TRUE) {
            SHR_EXIT();
        }
        if (filter->entry_config != NULL) {
            ctr_params.flex_ctr_base_idx =
                filter->entry_config->flex_ctr_base_idx;
            ctr_params.flex_ctr_offset_mode =
                filter->entry_config->flex_ctr_offset_mode;
            ctr_params.flex_ctr_pool_id =
                filter->entry_config->flex_ctr_pool_id;
            ctr_params.flex_ctr_action =
                filter->entry_config->flex_ctr_action;
        }
        if (policy_config != NULL) {
            ctr_params.flex_ctr_r_count =
                policy_config->flex_ctr_r_count;
            ctr_params.flex_ctr_y_count =
                policy_config->flex_ctr_y_count;
            ctr_params.flex_ctr_g_count =
                policy_config->flex_ctr_g_count;
        }
    } else if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_EFLEX_CTR_SUPPORT)) {
        if (is_default == TRUE) {
            if (filter->group_config != NULL) {
                ctr_params.flex_ctr_action =
                filter->group_config->default_flex_ctr_action;
            }
        } else {
            if (filter->entry_config != NULL) {
                ctr_params.flex_ctr_action =
                filter->entry_config->flex_ctr_action;
            }
        }
        if (ctr_params.flex_ctr_action == 0) {
            SHR_EXIT();
        }
        if (policy_config != NULL) {
            ctr_params.flex_ctr_r_count =
                policy_config->flex_ctr_r_count;
            ctr_params.flex_ctr_y_count =
                policy_config->flex_ctr_y_count;
            ctr_params.flex_ctr_g_count =
                policy_config->flex_ctr_g_count;
            ctr_params.flex_ctr_object =
                policy_config->flex_ctr_object;
            ctr_params.flex_ctr_container_id =
                policy_config->flex_ctr_container_id;
        }
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_counter_actions_set(unit,
                                   stage,
                                   group_id,
                                   pdd_id,
                                   filter->group_oper_info,
                                   &ctr_params,
                                   edw));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_buffers_customize(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmfp_stage_t *stage,
                               bcmfp_filter_t *filter)
{
    uint32_t flags = 0;
    bcmfp_key_ids_t key_ids;
    bcmfp_group_oper_info_t *opinfo = NULL;
    int start_bit = 0;
    int end_bit = 0;
    uint8_t num_parts = 0;
    bcmfp_pdd_config_t *pdd_config = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    opinfo = filter->group_oper_info;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit, FALSE, opinfo->flags, &num_parts));

    if (opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_GROUP_SBR_ENTRY) {
        if (BCMFP_STAGE_FLAGS_TEST(stage,
                        BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE)) {
            start_bit = (stage->pdd_hw_info->raw_policy_width -
                       stage->misc_info->per_entry_sbr_raw_data_offset) ;
            end_bit = (start_bit + stage->misc_info->sbr_index_size - 1);
            bcmdrd_field_set(filter->buffers.edw[num_parts - 1],
                             start_bit,
                             end_bit,
                             &(filter->sbr_profile_index));
        }
        /* sbr profile index goes into part 0 for EM/EM_FT */
        if (BCMFP_STAGE_FLAGS_TEST(stage,
                        BCMFP_STAGE_ENTRY_TYPE_HASH)) {
            start_bit = (stage->pdd_hw_info->raw_policy_width -
                       stage->misc_info->per_entry_sbr_raw_data_offset) ;
            end_bit = (start_bit + stage->misc_info->sbr_index_size - 1);
            bcmdrd_field_set(filter->buffers.edw[0],
                             start_bit,
                             end_bit,
                             &(filter->sbr_profile_index));
        }
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        flags = BCMFP_BUFFERS_CUSTOMIZE_EKW;
        flags |= BCMFP_BUFFERS_CUSTOMIZE_EDW;
    }

    if (flags == 0) {
        SHR_EXIT();
    }

    sal_memset(&key_ids, 0, sizeof(bcmfp_key_ids_t));
    key_ids.entry_id = filter->entry_config->entry_id;
    key_ids.group_id = filter->entry_config->group_id;
    key_ids.pdd_id = filter->group_config->pdd_id;
    pdd_config = filter->pdd_config;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_buffers_customize(unit,
                                 op_arg,
                                 flags,
                                 stage,
                                 filter->group_oper_info,
                                 pdd_config,
                                 &key_ids,
                                 &(filter->buffers)));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_configs_process(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t tbl_id,
                             bcmfp_stage_t *stage,
                             bcmfp_filter_t *filter)
{
    bcmfp_group_id_t group_id = 0;
    bcmfp_stage_id_t stage_id;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmltd_sid_t pse_tbl_id;
    bool reinsert_filter = FALSE;
    bool default_reinsert_filter = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    if (filter->rule_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_rule_config_process(unit,
                                              op_arg,
                                              tbl_id,
                                              stage,
                                              filter));
    }

    if (filter->policy_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_policy_config_process(unit,
                                                op_arg,
                                                tbl_id,
                                                FALSE,
                                                stage,
                                                filter));
    }

    if (filter->dpolicy_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_policy_config_process(unit,
                                                op_arg,
                                                tbl_id,
                                                TRUE,
                                                stage,
                                                filter));
    }

    if (filter->sbr_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_sbr_config_process(unit,
                                             op_arg,
                                             tbl_id,
                                             FALSE,
                                             stage,
                                             filter));
    }

    if (filter->dsbr_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_sbr_config_process(unit,
                                             op_arg,
                                             tbl_id,
                                             TRUE,
                                             stage,
                                             filter));
    }

    if (filter->pdd_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_pdd_config_process(unit,
                                             op_arg,
                                             tbl_id,
                                             FALSE,
                                             stage,
                                             filter));
    }

    if (filter->dpdd_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_pdd_config_process(unit,
                                             op_arg,
                                             tbl_id,
                                             TRUE,
                                             stage,
                                             filter));
    }

    if (filter->psg_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_psg_config_process(unit,
                                             op_arg,
                                             tbl_id,
                                             stage,
                                             filter));
    }

    if (!BCMFP_STAGE_FLAGS_TEST(stage,
             BCMFP_STAGE_NO_METER_SUPPORT)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_meter_config_process(unit,
                                               op_arg,
                                               tbl_id,
                                               stage,
                                               filter,
                                               TRUE,
                                               &reinsert_filter,
                                               FALSE));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_meter_config_process(unit,
                                               op_arg,
                                               tbl_id,
                                               stage,
                                               filter,
                                               TRUE,
                                               &default_reinsert_filter,
                                               TRUE));

    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
             BCMFP_STAGE_EFLEX_CTR_SUPPORT) ||
        BCMFP_STAGE_FLAGS_TEST(stage,
             BCMFP_STAGE_FLEX_CTR_SUPPORT) ||
        BCMFP_STAGE_FLAGS_TEST(stage,
             BCMFP_STAGE_LEGACY_CTR_SUPPORT)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_ctr_config_process(unit,
                                             op_arg,
                                             tbl_id,
                                             stage,
                                             filter,
                                             FALSE));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_ctr_config_process(unit,
                                             op_arg,
                                             tbl_id,
                                             stage,
                                             filter,
                                             TRUE));
    }

    group_id = filter->entry_config->group_id;
    stage_id = stage->stage_id;
    opinfo = filter->group_oper_info;
    if (filter->group_config != NULL &&
        opinfo->ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_group_config_process(unit,
                                               op_arg,
                                               tbl_id,
                                               stage,
                                               filter));
    }

    /* Disable group for non presel stages.*/
    if ((!(BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_KEY_TYPE_PRESEL))) &&
        (filter->group_config != NULL) &&
        (filter->group_config->enable == false)) {
        filter->entry_config->enable = false;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_entry_config_process(unit,
                                           op_arg,
                                           tbl_id,
                                           stage,
                                           filter));
    /*
     * Do chip specific and stage specific customizations
     * to different buffers created for filter.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_customize(unit,
                                        op_arg,
                                        stage,
                                        filter));
    /* Insert the filter to HW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_insert_hw(unit,
                                op_arg,
                                tbl_id,
                                stage,
                                FALSE,
                                0,
                                filter));

    /*
     * Reinsertion is required in case stage with per
     * slice meter pools. Reinsertion is required only
     * for first entry in the group.
     */
    if (reinsert_filter == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_meter_config_process(unit,
                                               op_arg,
                                               tbl_id,
                                               stage,
                                               filter,
                                               FALSE,
                                               &reinsert_filter,
                                               FALSE));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_insert_hw(unit,
                                    op_arg,
                                    tbl_id,
                                    stage,
                                    FALSE,
                                    0,
                                    filter));
    }

    /*
     * If the entry is not first in group no need to install
     * presel entries in the group.
     */

    if (opinfo->ref_count != 0) {
        SHR_EXIT();
    }

    /* Insert presel entries if group has presel entries. */
    if (filter->pse_config != NULL) {
        uint8_t idx = 0;
        pse_tbl_id = stage->tbls.pse_tbl->sid;
        for (idx = 0; idx < filter->num_pse_configs; idx++) {
            bcmfp_filter_buffers_clear(unit, filter);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_filter_pse_config_process(unit,
                                              op_arg,
                                              tbl_id,
                                              stage,
                                              idx,
                                              filter));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_filter_insert_hw(unit,
                                     op_arg,
                                     pse_tbl_id,
                                     stage,
                                     TRUE,
                                     idx,
                                     filter));
        }
    }

    /* Default presel entry if no presel entries are provided. */
    if ((filter->group_config->num_presels  == 0) &&
        BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_KEY_TYPE_PRESEL)) {
        bcmfp_filter_buffers_clear(unit, filter);
        pse_tbl_id = stage->tbls.pse_tbl->sid;
        if (BCMFP_STAGE_FLAGS_TEST(stage,
                BCMFP_STAGE_PRESEL_KEY_DYNAMIC)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_presel_edw_get(unit,
                                      stage_id,
                                      group_id,
                                      filter->group_oper_info,
                                      filter->buffers.edw));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_presel_edw_get(unit,
                                      stage_id,
                                      group_id,
                                      filter->group_oper_info,
                                      filter->buffers.ekw));
        }
        /* coverity[var_deref_model] */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_insert_hw(unit,
                                    op_arg,
                                    pse_tbl_id,
                                    stage,
                                    TRUE,
                                    0,
                                    filter));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_configs_free(int unit, bcmfp_filter_t *filter)
{
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    if (filter == NULL) {
        SHR_EXIT();
    }

    if (filter->group_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_config_free(unit, filter->group_config));
        filter->group_config = NULL;
    }
    if (filter->entry_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_config_free(unit, filter->entry_config));
        filter->entry_config = NULL;
    }
    if (filter->rule_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_rule_config_free(unit, filter->rule_config));
        filter->rule_config = NULL;
    }
    if (filter->policy_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_config_free(unit, filter->policy_config));
        filter->policy_config = NULL;
    }
    if (filter->psg_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_psg_config_free(unit, filter->psg_config));
        filter->psg_config = NULL;
    }
    if (filter->ctr_entry_config != NULL) {
        SHR_FREE(filter->ctr_entry_config);
        filter->ctr_entry_config = NULL;
    }
    if (filter->pse_config != NULL) {
       for (idx = 0; idx < filter->num_pse_configs; idx++) {
           if (filter->pse_config[idx] != NULL) {
               SHR_IF_ERR_VERBOSE_EXIT
                   (bcmfp_pse_config_free(unit, filter->pse_config[idx]));
               filter->pse_config[idx] = NULL;
           }
       }
       SHR_FREE(filter->pse_config);
    }
    if (filter->pdd_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_config_free(unit, filter->pdd_config));
        filter->pdd_config = NULL;
    }
    if (filter->sbr_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_config_free(unit, filter->sbr_config));
        filter->sbr_config = NULL;
    }
    if (filter->dpdd_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_config_free(unit, filter->dpdd_config));
        filter->dpdd_config = NULL;
    }
    if (filter->dsbr_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_config_free(unit, filter->dsbr_config));
        filter->dsbr_config = NULL;
    }
    if (filter->dpolicy_config != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_policy_config_free(unit, filter->dpolicy_config));
        filter->dpolicy_config = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_buffers_free(int unit, bcmfp_buffers_t *buffers)
{
    uint8_t idx = 0;
    size_t size = BCMFP_ENTRY_PARTS_MAX;

    SHR_FUNC_ENTER(unit);

    if (buffers == NULL) {
        SHR_EXIT();
    }

    if (buffers->ekw != NULL) {
        for (idx = 0; idx < size; idx++) {
            SHR_FREE(buffers->ekw[idx]);
        }
        SHR_FREE(buffers->ekw);
    }
    if (buffers->edw != NULL) {
        for (idx = 0; idx < size; idx++) {
            SHR_FREE(buffers->edw[idx]);
        }
        SHR_FREE(buffers->edw);
    }
    if (buffers->dedw != NULL) {
        for (idx = 0; idx < size; idx++) {
            SHR_FREE(buffers->dedw[idx]);
        }
        SHR_FREE(buffers->dedw);
    }
    if (buffers->pdd != NULL) {
        for (idx = 0; idx < size; idx++) {
            SHR_FREE(buffers->pdd[idx]);
        }
        SHR_FREE(buffers->pdd);
    }
    if (buffers->dpdd != NULL) {
        for (idx = 0; idx < size; idx++) {
            SHR_FREE(buffers->dpdd[idx]);
        }
        SHR_FREE(buffers->dpdd);
    }
    if (buffers->sbr != NULL) {
        for (idx = 0; idx < size; idx++) {
            SHR_FREE(buffers->sbr[idx]);
        }
        SHR_FREE(buffers->sbr);
    }
    if (buffers->dsbr != NULL) {
        for (idx = 0; idx < size; idx++) {
            SHR_FREE(buffers->dsbr[idx]);
        }
        SHR_FREE(buffers->dsbr);
    }
    if (buffers->psg != NULL) {
        for (idx = 0; idx < size; idx++) {
            SHR_FREE(buffers->psg[idx]);
        }
        SHR_FREE(buffers->psg);
    }
    if (buffers->qos != NULL) {
        for (idx = 0; idx < size; idx++) {
            SHR_FREE(buffers->qos[idx]);
        }
        SHR_FREE(buffers->qos);
    }
    if (buffers->dqos != NULL) {
        for (idx = 0; idx < size; idx++) {
            SHR_FREE(buffers->dqos[idx]);
        }
        SHR_FREE(buffers->dqos);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_group_pse_validate(int unit,
                                int tbl_inst,
                                bcmfp_stage_id_t stage_id,
                                uint32_t group_id,
                                uint32_t priority,
                                uint8_t lookup_id,
                                uint8_t num_presel_ids)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   BCMFP_GROUP_ID_ALL,
                                   &group_oper_info));

    for (idx = 0; idx < stage->misc_info->num_groups; idx++)  {
        if (group_oper_info[idx].valid == FALSE) {
            continue;
        }
        if (group_oper_info[idx].group_id == group_id ||
            group_oper_info[idx].ref_count == 0) {
            continue;
        }
        /*
         * Two groups with default presel cannot have
         * same lookup_id/slice_id.
         */
        if (group_oper_info[idx].tbl_inst == tbl_inst) {
            if (BCMFP_STAGE_FLAGS_TEST(stage,
                                       BCMFP_STAGE_ENTRY_TYPE_HASH)) {
                if ((group_oper_info[idx].slice_id == lookup_id) &&
                    (group_oper_info[idx].flags &
                     BCMFP_GROUP_WITH_DEFAULT_PRESEL) &&
                    (num_presel_ids == 0)) {
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }
            } else {
                if ((group_oper_info[idx].slice_id == lookup_id) &&
                    (group_oper_info[idx].group_prio == priority) &&
                    (group_oper_info[idx].flags &
                     BCMFP_GROUP_WITH_DEFAULT_PRESEL) &&
                    (num_presel_ids == 0)) {
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_group_multimode_validate(int unit,
                                int tbl_inst,
                                bcmfp_stage_id_t stage_id,
                                bcmfp_group_oper_info_t *opinfo,
                                bcmfp_group_config_t *gconfig)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   BCMFP_GROUP_ID_ALL,
                                   &group_oper_info));

    for (idx = 0; idx < stage->misc_info->num_groups; idx++)  {
        if (group_oper_info[idx].valid == FALSE) {
            continue;
        }
        if (group_oper_info[idx].group_id == gconfig->group_id ||
            group_oper_info[idx].ref_count == 0) {
            continue;
        }
        /* Multi mode checks */
        if (group_oper_info[idx].tbl_inst == tbl_inst) {
            if ((group_oper_info[idx].group_prio == gconfig->priority) &&
                group_oper_info[idx].group_mode != opinfo->group_mode) {
                /*
                 * 1. slice sharing is feasible for only single/double/triple
                 * wide groups with same priority. if a new group with any
                 * other mode (like half mode, intra slice, quad) is created
                 * with same priority as existing group, Resource sharing is
                 * not feasible.
                 */

                /* Sharing not possible if one is half mode
                 * and other is not half mode.
                 */
                if ((group_oper_info[idx].group_mode == BCMFP_GROUP_MODE_HALF
                       || opinfo->group_mode == BCMFP_GROUP_MODE_HALF)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                       "Group ID = %d and Group ID = %d has same group"
                       " priority, but one of the group mode is HALF mode."
                       " Multi Mode resource sharing is not feasible.\n"),
                       gconfig->group_id, group_oper_info[idx].group_id));
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }

                /* Sharing not possible if one is DBLINTRA mode
                 * and other is not DBLINTRA mode.
                 */
                if ((group_oper_info[idx].group_mode == BCMFP_GROUP_MODE_DBLINTRA
                       || opinfo->group_mode == BCMFP_GROUP_MODE_DBLINTRA)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                       "Group ID = %d and Group ID = %d has same group"
                       " priority, but one of the group mode is DBLINTRA mode."
                       " Multi Mode resource sharing is not feasible.\n"),
                       gconfig->group_id, group_oper_info[idx].group_id));
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }

                /* Sharing not possible if one is QUAD mode
                 * and other is not QUAD mode.
                 */
                if ((group_oper_info[idx].group_mode == BCMFP_GROUP_MODE_QUAD
                       || opinfo->group_mode == BCMFP_GROUP_MODE_QUAD)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                       "Group ID = %d and Group ID = %d has same group"
                       " priority, but one of the group mode is QUAD mode."
                       " Multi Mode resource sharing is not feasible.\n"),
                       gconfig->group_id, group_oper_info[idx].group_id));
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }

                
                if (opinfo->ref_count == 0 &&
                    opinfo->group_mode > group_oper_info[idx].group_mode) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "\nMulti Mode slice sharing is "
                        "feasible only if the widest mode group entries "
                        "are inserted first.\n"
                        "\nChange group priority to allocate different "
                        "set of slices\n")));
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_group_config_check(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t tbl_id,
                                bcmfp_stage_t *stage,
                                bcmfp_filter_t *filter)
{
    int rv;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_config_t *gconfig = NULL;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_COUNT;
    shr_error_t rv_except = SHR_E_NOT_FOUND;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    /* Group ID is not specified but it is must. */
    if (filter->entry_config->group_id == 0) {
        filter->entry_state = BCMFP_ENTRY_GROUP_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /*
     * Group ID is specified and present in IMM but this call
     * is invoked when group ID is deleted and associated to
     * some entry.
     */
    if ((filter->event_reason == BCMIMM_ENTRY_DELETE) &&
        (filter->sid_type == BCMFP_SID_TYPE_GRP_TEMPLATE)) {
        filter->entry_state = BCMFP_ENTRY_GROUP_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Get the group LT entry configuration of the filter. */
    stage_id = stage->stage_id;
    group_id = filter->entry_config->group_id;
    if (filter->group_config == NULL && group_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_group_remote_config_get(unit,
                                           op_arg,
                                           stage_id,
                                           group_id,
                                           &gconfig),
                                           rv_except);
        filter->group_config = gconfig;
    }

    if (filter->group_config == NULL) {
        filter->entry_state = BCMFP_ENTRY_GROUP_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    opinfo = filter->group_oper_info;
    /*
     * All non zero group ID created must have a group opertaional
     * information.
     */
    if (opinfo == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (opinfo->state == BCMFP_GROUP_STATE_NON_COMPRESSIBLE) {
        filter->entry_state = BCMFP_ENTRY_PARTIAL_COMPRESS_QUAL_BITMAP_IN_GROUP;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
                               BCMFP_STAGE_KEY_TYPE_PRESEL)) {
        gconfig = filter->group_config;
        rv = bcmfp_filter_group_pse_validate(unit,
                                             gconfig->pipe_id,
                                             stage_id,
                                             group_id,
                                             gconfig->priority,
                                             gconfig->lookup_id,
                                             gconfig->num_presels);
        if (rv == SHR_E_CONFIG) {
            filter->entry_state = BCMFP_ENTRY_GROUP_MASKED;
        }
        SHR_IF_ERR_EXIT(rv);
    }

    if (opinfo->valid == FALSE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                   "Group ID = %d is not operational."
                   "So Entry ID = %d cannot be inserted.\n"),
                   group_id, filter->entry_config->entry_id));
        filter->entry_state = BCMFP_ENTRY_GROUP_NOT_OPERATIONAL;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (opinfo->aset_error == TRUE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                   "Group ID = %d aset doesnt fit the configuration."
                   "So Entry ID = %d cannot be inserted.\n"),
                   group_id, filter->entry_config->entry_id));
        filter->entry_state = BCMFP_ENTRY_GROUP_ASET_DOESNT_FIT;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (opinfo->default_aset_error == TRUE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                   "Group ID = %d default aset doesnt fit the configuration."
                   "So Entry ID = %d cannot be inserted.\n"),
                   group_id, filter->entry_config->entry_id));
        filter->entry_state = BCMFP_ENTRY_GROUP_DEFAULT_ASET_DOESNT_FIT;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    if (opinfo->qset_error == TRUE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                   "Group ID = %d qset doesnt fit the configuration."
                   "So Entry ID = %d cannot be inserted.\n"),
                   group_id, filter->entry_config->entry_id));
        filter->entry_state = BCMFP_ENTRY_GROUP_QSET_DOESNT_FIT;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
                               BCMFP_STAGE_KEY_TYPE_PRESEL) &&
        !(BCMFP_STAGE_FLAGS_TEST(stage,
                                BCMFP_STAGE_ENTRY_TYPE_HASH))) {
        gconfig = filter->group_config;
        rv = bcmfp_filter_group_multimode_validate(unit,
                                             gconfig->pipe_id,
                                             stage_id,
                                             opinfo,
                                             gconfig);
        if (rv == SHR_E_CONFIG) {
            filter->entry_state = BCMFP_ENTRY_GROUP_MULTIMODE_CHECK_FAILED;
        }
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_rule_config_check(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t tbl_id,
                               bcmfp_stage_t *stage,
                               bcmfp_filter_t *filter)
{
    int rv;
    bcmfp_rule_id_t rule_id = 0;
    bcmfp_rule_config_t *rconfig = NULL;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_COUNT;
    shr_error_t rv_except = SHR_E_NOT_FOUND;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    /* Rule ID is not specified. */
    if (filter->entry_config->rule_id == 0) {
        SHR_EXIT();
    }

    /*
     * Rule ID is specified and present in IMM but this call
     * is invoked when rule ID is deleted and associated to
     * some entry.
     */
    if ((filter->event_reason == BCMIMM_ENTRY_DELETE) &&
        (filter->sid_type == BCMFP_SID_TYPE_RULE_TEMPLATE)) {
        filter->entry_state = BCMFP_ENTRY_RULE_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Get the rule LT entry configuration of the filter. */
    stage_id = stage->stage_id;
    rule_id = filter->entry_config->rule_id;
    if (filter->rule_config == NULL && rule_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_rule_remote_config_get(unit,
                                          op_arg,
                                          stage_id,
                                          rule_id,
                                          &rconfig),
                                          rv_except);
        filter->rule_config = rconfig;
    }

    /* Rule ID is specified but not created. */
    if (filter->rule_config == NULL && rule_id != 0) {
        filter->entry_state = BCMFP_ENTRY_RULE_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check all qualifiers are present in group as well. */
    rv = bcmfp_filter_rule_qual_presence_validate(unit,
                                                  FALSE,
                                                  0,
                                                  filter);
    if (rv == SHR_E_CONFIG) {
        filter->entry_state = BCMFP_ENTRY_RULE_QSET_NOT_IN_GROUP;
    }
    SHR_IF_ERR_EXIT(rv);

    /*
     * Check all qualifiers data and mask values are not
     * exceeding the bitmaps specified in group.
     */
    rv = bcmfp_filter_rule_qual_dm_validate(unit,
                                            FALSE,
                                            0,
                                            filter);
    if (rv == SHR_E_CONFIG) {
        filter->entry_state = BCMFP_ENTRY_RULE_QSET_WIDTH_EXCEEDS;
    }
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT
        (bcmfp_filter_rule_compress_qual_validate(unit,
                                                  stage_id,
                                                  filter));

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function is used to check if PDD template is
 * required to be mapped to the filter. This is done by
 * checking if any of the actions in the policy_data are
 * PDD actions. If all actions are FLAT POLICY type,
 * then pdd is not required for the given filter.
 */
static int
bcmfp_filter_policy_pdd_requirement_check(int unit,
                                          bcmfp_stage_t *stage,
                                          bcmfp_filter_t *filter,
                                          bool is_default,
                                          bool *pdd_required)
{
    uint32_t action_flag = BCMFP_ACTIONS_IN_FLAT_POLICY;
    bcmfp_action_cfg_t *action_cfg = NULL;
    bcmfp_action_data_t *action_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    if (is_default == FALSE) {
        action_data = filter->policy_config->action_data;
    } else {
        action_data = filter->dpolicy_config->action_data;
    }

    while (action_data != NULL) {
        /* Get Action Configuration */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_action_cfg_get(unit,
                                  stage->stage_id,
                                  action_data->action,
                                  &action_cfg));
        if (action_cfg->flags & action_flag) {
            action_data = action_data->next;
            continue;
        } else {
            /*
             * non flat policy action encountered
             * => pdd is required for this filter.
             */
            *pdd_required = TRUE;
            SHR_EXIT();
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_policy_config_check(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t tbl_id,
                                 bcmfp_stage_t *stage,
                                 bcmfp_filter_t *filter)
{
    int rv;
    bcmfp_policy_id_t policy_id = 0;
    bcmfp_policy_config_t *pconfig = NULL;
    bcmfp_pdd_id_t pdd_id = 0;
    bcmfp_pdd_config_t *pdd_config = NULL;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_COUNT;
    shr_error_t rv_except = SHR_E_NOT_FOUND;
    bool pdd_required = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    /*
     * Policy ID is specified and present in IMM but this call
     * is invoked when policy ID is deleted and associated to
     * some entry.
     */
    if ((filter->event_reason == BCMIMM_ENTRY_DELETE) &&
        (filter->sid_type == BCMFP_SID_TYPE_POLICY_TEMPLATE)) {
        filter->entry_state = BCMFP_ENTRY_POLICY_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Get the policy LT entry configuration of the filter. */
    stage_id = stage->stage_id;
    policy_id = filter->entry_config->policy_id;
    if (filter->policy_config == NULL && policy_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_policy_remote_config_get(unit,
                                            op_arg,
                                            stage_id,
                                            policy_id,
                                            &pconfig),
                                            rv_except);
        filter->policy_config = pconfig;
    }

    /* Get the PDD LT entry configuration of the filter. */
    pdd_id = filter->group_config->pdd_id;
    if (filter->pdd_config == NULL && pdd_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_pdd_remote_config_get(unit,
                                         op_arg->trans_id,
                                         stage_id,
                                         pdd_id,
                                         &pdd_config),
                                         rv_except);
        filter->pdd_config = pdd_config;
    }

    /* Policy is specified but not created. */
    if (filter->policy_config == NULL && policy_id != 0) {
        filter->entry_state = BCMFP_ENTRY_POLICY_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* PDD is specified but not created. */
    if (filter->pdd_config == NULL && pdd_id != 0) {
        filter->entry_state = BCMFP_ENTRY_PDD_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Policy ID is not specified. */
    if (filter->entry_config->policy_id == 0) {
        SHR_EXIT();
    }

    /* Policy is specified and created but PDD is not specified. */
    if (pdd_id == 0 && policy_id != 0 &&
        BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_POLICY_TYPE_PDD)) {
        /*
         *  pdd might not be required for all filter entries.
         *  This is valid for cases where the policy data includes
         *  both FLAT POLICY actions and PDD policy actions.
         *  PDD is not required to be mapped to the filter when all
         *  actions in the policy data are of FLAT POLICY type.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_policy_pdd_requirement_check(unit,
                                                       stage,
                                                       filter,
                                                       FALSE,
                                                       &pdd_required));
        if (pdd_required == TRUE) {
            filter->entry_state =
                BCMFP_ENTRY_POLICY_PRESENT_PDD_MISSING;
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    }

    /* split the action_data across parts
     * this is valid only for non-PDD stages and used mainly
     * to split actions that conflict in single slice alone to
     * multiple parts. These include include actions that are overlay
     * in hardware but donot conflict logically.
     */
    if (!(BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_POLICY_TYPE_PDD))) {
        rv = bcmfp_filter_policy_actions_split(unit,
                                               stage,
                                               filter);
        if (rv == SHR_E_CONFIG) {
            filter->entry_state = BCMFP_ENTRY_POLCIY_ACTIONS_CONFLICT;
        }
        SHR_IF_ERR_EXIT(rv);
    }

    /* Check for action conflict checks */
    rv = bcmfp_filter_policy_action_conflicts_validate(unit,
                                                       FALSE,
                                                       stage,
                                                       filter);
    if (rv == SHR_E_CONFIG) {
        filter->entry_state = BCMFP_ENTRY_POLCIY_ACTIONS_CONFLICT;
    }
    SHR_IF_ERR_EXIT(rv);

    /* Check all actions are enabled in corresponding PDD. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_POLICY_TYPE_PDD)) {
        /*
         * It is required to check if a corresponding PDD action
         * is present for action in policy_data only for PDD actions.
         * For FLAT policy actions this check should be skipped.
         * This is valid for cases for policy with both PDD actions
         * and FLAT policy actions.
         */
        rv = bcmfp_filter_policy_action_presence_validate(unit,
                                                          FALSE,
                                                          stage,
                                                          filter);
        if (rv == SHR_E_CONFIG) {
            filter->entry_state =
                BCMFP_ENTRY_POLICY_ACTION_NOT_IN_PDD;
        }
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_presel_config_check(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t tbl_id,
                                 bcmfp_stage_t *stage,
                                 bcmfp_filter_t *filter)
{
    int rv;
    bcmfp_psg_id_t psg_id = 0;
    bcmfp_psg_config_t *psg_config = NULL;
    uint32_t *presel_ids = NULL;
    uint8_t num_presels = 0;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_COUNT;
    shr_error_t rv_except = SHR_E_NOT_FOUND;
    uint16_t idx = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_group_oper_info_t *psg_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    /*
     * Presels needs to be validated during first entry of group
     * is installed.
     */
    stage_id = stage->stage_id;
    opinfo = filter->group_oper_info;
    if (opinfo->ref_count != 0) {
        SHR_EXIT();
    }

    /* If stage doesnot support presel, exit.*/
    if (!BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_KEY_TYPE_PRESEL)) {
        SHR_EXIT();
    }

    /* Get the PSG LT entry configuration of the filter. */
    psg_id = filter->group_config->psg_id;
    if (filter->psg_config == NULL && psg_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_psg_remote_config_get(unit,
                                         op_arg,
                                         stage_id,
                                         psg_id,
                                         &psg_config),
                                         rv_except);
        filter->psg_config = psg_config;
    }

    /* Get the PSE LT entries configuration of the filter. */
    presel_ids = filter->group_config->presel_ids;
    num_presels = filter->group_config->num_presels;
    if ((filter->num_pse_configs == 0) &&
        num_presels != 0 && presel_ids != NULL) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_filter_pse_config_get(unit,
                                         op_arg,
                                         stage,
                                         presel_ids,
                                         num_presels,
                                         filter),
                                         rv_except);
    }

    /* If presels are specified but not created. */
    for (idx = 0; idx < filter->num_pse_configs; idx++) {
        if (filter->pse_config[idx] == NULL) {
            filter->entry_state = BCMFP_ENTRY_PRESEL_NOT_CREATED;
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    }

    /* presel group is deleted */
    if ((filter->event_reason == BCMIMM_ENTRY_DELETE) &&
            (filter->sid_type == BCMFP_SID_TYPE_PSG_TEMPLATE)) {
        filter->entry_state = BCMFP_ENTRY_PSG_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (filter->psg_config == NULL && psg_id != 0) {
        filter->entry_state = BCMFP_ENTRY_PSG_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* No presels are specified. */
    if (filter->num_pse_configs == 0) {
        SHR_EXIT();
    }

    /* If presel doesnot support dynamic key, no more checks.*/
    if (!BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_PRESEL_KEY_DYNAMIC)) {
        SHR_EXIT();
    }


    /* Presel entries present but presel group is missing. */
    if (filter->psg_config == NULL) {
        filter->entry_state = BCMFP_ENTRY_PSE_PRESENT_PSG_MISSING;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check all qualifiers are present in presel group as well */
    for (idx = 0; idx < filter->num_pse_configs; idx++) {
        rv = bcmfp_filter_rule_qual_presence_validate(unit,
                                                      TRUE,
                                                      idx,
                                                      filter);
        if (rv == SHR_E_CONFIG) {
            filter->entry_state =
                BCMFP_ENTRY_PRESEL_QSET_NOT_IN_PSG;
        }
        SHR_IF_ERR_EXIT(rv);
    }

    /* check if the qualifiers in presel group can be accomodated
     * in the HW
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_oper_info_get(unit,
                                          stage_id,
                                          filter->psg_config->psg_id,
                                          &psg_oper_info));
    if (psg_oper_info->valid == FALSE &&
        psg_oper_info->qset_error == TRUE) {
        filter->entry_state =
            BCMFP_ENTRY_PRESEL_GROUP_QSET_DOESNT_FIT;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /*
     * Check all qualifiers data and mask values are not
     * exceeding the bitmaps specified in presel group.
     */
    for (idx = 0; idx < filter->num_pse_configs; idx++) {
        rv = bcmfp_filter_rule_qual_dm_validate(unit,
                                                TRUE,
                                                idx,
                                                filter);
        if (rv == SHR_E_CONFIG) {
            filter->entry_state =
                BCMFP_ENTRY_PRESEL_QSET_WIDTH_EXCEEDS;
        }
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_priority_config_check(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t tbl_id,
                                   bcmfp_stage_t *stage,
                                   bcmfp_filter_t *filter)
{
    bcmfp_sbr_id_t sbr_id = 0;
    bcmfp_sbr_config_t *sbr_config = NULL;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_COUNT;
    shr_error_t rv_except = SHR_E_NOT_FOUND;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    /*
     * Priority needs to be validated during first entry of group
     * is installed.
     */
    stage_id = stage->stage_id;
    opinfo = filter->group_oper_info;
    if (opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_SBR_GROUP) {
        if (opinfo->ref_count != 0) {
            SHR_EXIT();
        }
    }

    /* If stage doesnot support SBR, exit.*/
    if (!BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ACTION_RESOLUTION_SBR)) {
        SHR_EXIT();
    }

    /* Get the SBR LT entry configuration of the filter. */
    if (opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_SBR_GROUP) {
        sbr_id = filter->group_config->sbr_id;
    } else {
        sbr_id = filter->entry_config->sbr_id;
    }

    if (filter->sbr_config == NULL && sbr_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_sbr_remote_config_get(unit,
                                         op_arg,
                                         stage_id,
                                         sbr_id,
                                         &sbr_config),
                                         rv_except);
        filter->sbr_config = sbr_config;
    }

    /* SBR ID is specified but not created. */
    if (filter->sbr_config == NULL && sbr_id != 0) {
        filter->entry_state = BCMFP_ENTRY_SBR_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* SBR should come either from group or entry. */
    if (filter->group_config->sbr_id != 0 &&
        filter->entry_config->sbr_id != 0) {
        filter->entry_state =
                BCMFP_ENTRY_SBR_PRESENT_IN_GROUP_AND_ENTRY;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_dpolicy_config_check(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t tbl_id,
                                  bcmfp_stage_t *stage,
                                  bcmfp_filter_t *filter)
{
    int rv;
    bcmfp_policy_id_t dpolicy_id = 0;
    bcmfp_policy_config_t *dpconfig = NULL;
    bcmfp_pdd_id_t dpdd_id = 0;
    bcmfp_pdd_config_t *dpdd_config = NULL;
    bcmfp_sbr_id_t dsbr_id = 0;
    bcmfp_sbr_config_t *dsbr_config = NULL;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_COUNT;
    shr_error_t rv_except = SHR_E_NOT_FOUND;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bool pdd_required = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    /*
     * Defauly policy is required during first entry of group
     * is installed.
     */
    stage_id = stage->stage_id;
    opinfo = filter->group_oper_info;
    if (opinfo->ref_count != 0) {
        SHR_EXIT();
    }

    /* If stage doesnot support SBR, exit.*/
    if (!BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_DEFAULT_POLICY)) {
        SHR_EXIT();
    }

    /* Get the default policy LT entry configuration of the filter. */
    dpolicy_id = filter->group_config->default_policy_id;
    if (filter->dpolicy_config == NULL && dpolicy_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_policy_remote_config_get(unit,
                                            op_arg,
                                            stage_id,
                                            dpolicy_id,
                                            &dpconfig),
                                            rv_except);
       filter->dpolicy_config = dpconfig;
    }

    /* Get the default PDD  LT entry configuration of the filter. */
    dpdd_id = filter->group_config->default_pdd_id;
    if (filter->dpdd_config == NULL && dpdd_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_pdd_remote_config_get(unit,
                                         op_arg->trans_id,
                                         stage_id,
                                         dpdd_id,
                                         &dpdd_config),
                                         rv_except);
        filter->dpdd_config = dpdd_config;
    }

    /* Get the default SBR LT entry configuration of the filter. */
    dsbr_id = filter->group_config->default_sbr_id;
    if (filter->dsbr_config == NULL && dsbr_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_sbr_remote_config_get(unit,
                                         op_arg,
                                         stage_id,
                                         dsbr_id,
                                         &dsbr_config),
                                         rv_except);
        filter->dsbr_config = dsbr_config;
    }
    /* Default policy is not specified but not created. */
    if (filter->dpolicy_config == NULL && dpolicy_id != 0) {
        filter->entry_state = BCMFP_ENTRY_DPOLICY_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Default PDD is specified but not created. */
    if (filter->dpdd_config == NULL && dpdd_id != 0) {
        filter->entry_state = BCMFP_ENTRY_DPDD_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Default SBR is specified but not created. */
    if (filter->dsbr_config == NULL && dsbr_id != 0) {
        filter->entry_state = BCMFP_ENTRY_DSBR_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Default policy is not mentioned. */
    if (filter->group_config->default_policy_id == 0) {
        SHR_EXIT();
    }

    /*
     * Default policy is specified and created but PDD is
     * not specified.
     */
    if (filter->group_config->default_pdd_id == 0 &&
        filter->group_config->default_policy_id != 0 &&
        BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_POLICY_TYPE_PDD)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_policy_pdd_requirement_check(unit,
                                                       stage,
                                                       filter,
                                                       TRUE,
                                                       &pdd_required));
        if (pdd_required == TRUE) {
            filter->entry_state =
                BCMFP_ENTRY_DPOLICY_PRESENT_DPDD_MISSING;
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    }

    /*
     * Default policy is specified and created but SBR is
     * not specified.
     */
    if (filter->group_config->default_sbr_id == 0 &&
        filter->group_config->default_policy_id != 0 &&
        BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ACTION_RESOLUTION_SBR)) {
        filter->entry_state =
            BCMFP_ENTRY_DPOLICY_PRESENT_DSBR_MISSING;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Check for action conflict checks */
    rv = bcmfp_filter_policy_action_conflicts_validate(unit,
                                                       TRUE,
                                                       stage,
                                                       filter);
    if (rv == SHR_E_CONFIG) {
        filter->entry_state =
            BCMFP_ENTRY_DPOLCIY_ACTIONS_CONFLICT;
    }
    SHR_IF_ERR_EXIT(rv);

    /* Check all actions are enabled in corresponding PDD. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_POLICY_TYPE_PDD)) {
        rv = bcmfp_filter_policy_action_presence_validate(unit,
                                                          TRUE,
                                                          stage,
                                                          filter);
        if (rv == SHR_E_CONFIG) {
            filter->entry_state =
                BCMFP_ENTRY_DPOLICY_ACTION_NOT_IN_DPDD;
        }
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_ctr_entry_config_check(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmltd_sid_t tbl_id,
                                    bcmfp_stage_t *stage,
                                    bcmfp_filter_t *filter)
{
    bcmfp_ctr_entry_id_t ctr_entry_id = 0;
    bcmfp_ctr_entry_config_t *ctr_entry_config = NULL;
    shr_error_t rv_except = SHR_E_NOT_FOUND;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    /* If stage doesnot support counter entry LT, exit.*/
    if (!BCMFP_STAGE_FLAGS_TEST(stage,
         BCMFP_STAGE_LEGACY_CTR_SUPPORT)) {
        SHR_EXIT();
    }

    /*
     * Ctr entry ID is specified and present in IMM but this call
     * is invoked when ctr entry ID is deleted and associated to
     * some entry.
     */
    if ((filter->event_reason == BCMIMM_ENTRY_DELETE) &&
        (filter->sid_type == BCMFP_SID_TYPE_CTR_ENTRY)) {
        filter->entry_state = BCMFP_ENTRY_COUNTER_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Get the counter entry LT entry configuration of the filter. */
    ctr_entry_id = filter->entry_config->ctr_entry_id;
    if (filter->ctr_entry_config == NULL && ctr_entry_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_ctr_entry_remote_config_get(unit,
                                               op_arg,
                                               stage->stage_id,
                                               ctr_entry_id,
                                               &ctr_entry_config),
                                               rv_except);
        filter->ctr_entry_config = ctr_entry_config;
    }

    /* No legacy counter is attached. */
    if (filter->entry_config->ctr_entry_id == 0) {
        SHR_EXIT();
    }

    /* Legacy counter is attached but not created. */
    if (filter->ctr_entry_config == NULL) {
        filter->entry_state = BCMFP_ENTRY_COUNTER_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_meter_config_check(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t tbl_id,
                                bcmfp_stage_t *stage,
                                bcmfp_filter_t *filter)
{
    int rv;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_COUNT;
    bcmfp_meter_id_t meter_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    /* If stage doesnot support meters, exit.*/
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_NO_METER_SUPPORT)) {
        SHR_EXIT();
    }

    /* No meter attached. */
    if (filter->entry_config->meter_id == 0) {
        SHR_EXIT();
    }

    /*
     * Meter ID is specified and present in IMM but this call
     * is invoked when meter ID is deleted and associated to
     * some entry.
     */
    if ((filter->event_reason == BCMIMM_ENTRY_DELETE) &&
        (filter->sid_type == BCMFP_SID_TYPE_METER_TEMPLATE)) {
        filter->entry_state = BCMFP_ENTRY_METER_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    stage_id = stage->stage_id;
    meter_id = filter->entry_config->meter_id;
    rv = bcmcth_meter_entry_valid(unit, stage_id, meter_id);

    /* Meter is attached but not created. */
    if (rv == SHR_E_NOT_FOUND) {
        filter->entry_state = BCMFP_ENTRY_METER_NOT_CREATED;
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_configs_check(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_t *stage,
                           bcmfp_filter_t *filter)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    if (filter->entry_config == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Get the group configuration if not already
     * provided in filter and validate it.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_group_config_check(unit,
                                         op_arg,
                                         tbl_id,
                                         stage,
                                         filter));
    /*
     * Get the rule configuration if not already
     * provided in filter and validate it.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_rule_config_check(unit,
                                        op_arg,
                                        tbl_id,
                                        stage,
                                        filter));
    /*
     * Get the policy and pdd configuration if not
     * already provided in filter and validate it.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_policy_config_check(unit,
                                          op_arg,
                                          tbl_id,
                                          stage,
                                          filter));

    /*
     * Get the presel entry and presel group configs
     * if not already provided in filter and validate
     * it.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_presel_config_check(unit,
                                          op_arg,
                                          tbl_id,
                                          stage,
                                          filter));
    /*
     * Validate the group priority and SBR configuration
     * if required.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_priority_config_check(unit,
                                            op_arg,
                                            tbl_id,
                                            stage,
                                            filter));
    /*
     * Get the default policy and default pdd configs
     * if not already provided in filter and validate
     * it.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_dpolicy_config_check(unit,
                                           op_arg,
                                           tbl_id,
                                           stage,
                                           filter));
    /*
     * Validate the meter ID attached to the filter.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_meter_config_check(unit,
                                         op_arg,
                                         tbl_id,
                                         stage,
                                         filter));
    /*
     * Get the counter entry config and validate it.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_ctr_entry_config_check(unit,
                                             op_arg,
                                             tbl_id,
                                             stage,
                                             filter));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_create(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t tbl_id,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_filter_t *filter)
{
    int rv;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (filter->entry_config == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * Validate all the configurations featched and update the
     * entry state.
     */
    rv = bcmfp_filter_configs_check(unit,
                                    op_arg,
                                    tbl_id,
                                    stage,
                                    filter);
    if (rv == SHR_E_CONFIG) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    /*
     * If the group is enabled for compression, compress the
     * rule in the filter.
     */
    rv = bcmfp_filter_compress(unit,
                               op_arg,
                               stage->stage_id,
                               BCMFP_OPCODE_INSERT,
                               NULL,
                               filter);
    if (rv == SHR_E_CONFIG) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    if (filter->list_compressed == TRUE) {
        SHR_EXIT();
    }

    /* Create the buffers required for ekw, wdw and profiles. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       BCMFP_BUFFERS_CREATE_ALL,
                                       tbl_id,
                                       stage,
                                       &(filter->buffers)));

    /*
     * Process all the configurations and populate  all the
     * required buffers and push the data down to ptm to
     * insert the FILTER info HW.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_configs_process(unit,
                                      op_arg,
                                      tbl_id,
                                      stage,
                                      filter));

exit:
    bcmfp_filter_buffers_free(unit, &(filter->buffers));
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_copy_cid_update(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmfp_stage_id_t stage_id,
                             bcmltd_sid_t tbl_id,
                             uint8_t compress_type,
                             bcmfp_cid_t *cid,
                             bcmfp_cid_t *cid_mask,
                             bcmfp_entry_id_t parent_eid,
                             bcmfp_filter_t *filter)
{
    size_t size = 0;
    bcmfp_buffers_t *req_buffers = NULL;
    bcmfp_buffers_t *rsp_buffers = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    int pipe_id = -1;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_entry_id_t entry_id = 0;
    uint32_t entry_prio = 0;
    bcmltd_fid_t cid_fid = 0;
    bcmfp_qualifier_t cid_qual_id = 0;
    uint32_t bflags = 0;
    bool enable = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cid, SHR_E_PARAM);
    SHR_NULL_CHECK(cid_mask, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    group_id = filter->entry_config->group_id;
    entry_id = filter->entry_config->entry_id;
    entry_prio = filter->entry_config->priority;
    enable = filter->entry_config->enable;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    group_oper_info = filter->group_oper_info;
    pipe_id = group_oper_info->tbl_inst;

    /* READ:Do filter lookup from PTcache. */
    size = sizeof(bcmfp_buffers_t);
    BCMFP_ALLOC(req_buffers, size, "bcmfpCidUpdateReqBuffers");
    BCMFP_ALLOC(rsp_buffers, size, "bcmfpCidUpdateRspBuffers");
    bflags = BCMFP_BUFFERS_CREATE_EKW;
    bflags |= BCMFP_BUFFERS_CREATE_EDW;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       rsp_buffers));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       req_buffers));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_entry_lookup(unit,
                                0,
                                op_arg,
                                stage_id,
                                tbl_id,
                                group_id,
                                filter->group_oper_info,
                                parent_eid,
                                req_buffers,
                                rsp_buffers));

    /* MODIFY:Update cid and cid_mask in ekw buffers. */
    cid_fid = stage->tbls.compress_fid_info[compress_type]->cid_key_fid;
    cid_qual_id =
        stage->tbls.rule_tbl->qualifiers_fid_info[cid_fid].qualifier;
    ekw = rsp_buffers->ekw;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_qual_set(unit,
                        stage_id,
                        group_id,
                        filter->group_oper_info,
                        0,
                        cid_qual_id,
                        cid_qual_id,
                        0,
                        NULL,
                        cid->w,
                        cid_mask->w,
                        ekw));

    /* WRITE:Insert the filter to HW again. */
    edw = rsp_buffers->edw;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_entry_insert(unit,
                                0,
                                op_arg,
                                pipe_id,
                                stage_id,
                                tbl_id,
                                group_id,
                                filter->group_oper_info,
                                entry_id,
                                entry_prio,
                                ekw,
                                edw,
                                NULL,
                                enable));
exit:
    bcmfp_filter_buffers_free(unit, rsp_buffers);
    bcmfp_filter_buffers_free(unit, req_buffers);
    SHR_FREE(req_buffers);
    SHR_FREE(rsp_buffers);
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_cid_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmfp_stage_id_t stage_id,
                        bcmltd_sid_t tbl_id,
                        uint8_t compress_type,
                        bcmfp_cid_t *cid,
                        bcmfp_cid_t *cid_mask,
                        bcmfp_filter_t *filter)
{
    size_t size = 0;
    bcmfp_buffers_t *req_buffers = NULL;
    bcmfp_buffers_t *rsp_buffers = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    int pipe_id = -1;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_entry_id_t entry_id = 0;
    uint32_t entry_prio = 0;
    bcmltd_fid_t cid_fid = 0;
    bcmfp_qualifier_t cid_qual_id = 0;
    uint32_t bflags = 0;
    bool enable = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cid, SHR_E_PARAM);
    SHR_NULL_CHECK(cid_mask, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    group_id = filter->entry_config->group_id;
    entry_id = filter->entry_config->entry_id;
    entry_prio = filter->entry_config->priority;
    enable = filter->entry_config->enable;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    group_oper_info = filter->group_oper_info;
    pipe_id = group_oper_info->tbl_inst;

    /* READ:Do filter lookup from PTcache. */
    size = sizeof(bcmfp_buffers_t);
    BCMFP_ALLOC(req_buffers, size, "bcmfpCidUpdateReqBuffers");
    BCMFP_ALLOC(rsp_buffers, size, "bcmfpCidUpdateRspBuffers");
    bflags = BCMFP_BUFFERS_CREATE_EKW;
    bflags |= BCMFP_BUFFERS_CREATE_EDW;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       rsp_buffers));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       req_buffers));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_entry_lookup(unit,
                                0,
                                op_arg,
                                stage_id,
                                tbl_id,
                                group_id,
                                filter->group_oper_info,
                                entry_id,
                                req_buffers,
                                rsp_buffers));

    /* MODIFY:Update cid and cid_mask in ekw buffers. */
    cid_fid = stage->tbls.compress_fid_info[compress_type]->cid_key_fid;
    cid_qual_id =
        stage->tbls.rule_tbl->qualifiers_fid_info[cid_fid].qualifier;
    ekw = rsp_buffers->ekw;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_qual_set(unit,
                        stage_id,
                        group_id,
                        filter->group_oper_info,
                        0,
                        cid_qual_id,
                        cid_qual_id,
                        0,
                        NULL,
                        cid->w,
                        cid_mask->w,
                        ekw));

    /* WRITE:Insert the filter to HW again. */
    edw = rsp_buffers->edw;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_entry_insert(unit,
                                0,
                                op_arg,
                                pipe_id,
                                stage_id,
                                tbl_id,
                                group_id,
                                filter->group_oper_info,
                                entry_id,
                                entry_prio,
                                ekw,
                                edw,
                                NULL,
                                enable));
exit:
    bcmfp_filter_buffers_free(unit, rsp_buffers);
    bcmfp_filter_buffers_free(unit, req_buffers);
    SHR_FREE(req_buffers);
    SHR_FREE(rsp_buffers);
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_copy(int unit,
                  const bcmltd_op_arg_t *op_arg,
                  bcmfp_stage_id_t stage_id,
                  bcmltd_sid_t tbl_id,
                  bcmfp_entry_id_t child_eid,
                  bcmfp_filter_t *filter)
{
    size_t size = 0;
    bcmfp_buffers_t *req_buffers = NULL;
    bcmfp_buffers_t *rsp_buffers = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    int pipe_id = -1;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_entry_id_t parent_eid = 0;
    uint32_t entry_prio = 0;
    uint32_t bflags = 0;
    bool enable = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    group_id = filter->entry_config->group_id;
    parent_eid = filter->entry_config->entry_id;
    entry_prio = filter->entry_config->priority;
    enable = filter->entry_config->enable;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    group_oper_info = filter->group_oper_info;
    pipe_id = group_oper_info->tbl_inst;

    /* READ:Do filter lookup from PTcache for parent entry ID. */
    size = sizeof(bcmfp_buffers_t);
    BCMFP_ALLOC(req_buffers, size,
                "bcmFilterfpListCompressUpdateReqBuffers");
    BCMFP_ALLOC(rsp_buffers, size,
                "bcmFilterfpListCompressUpdateRspBuffers");
    bflags = BCMFP_BUFFERS_CREATE_EKW;
    bflags |= BCMFP_BUFFERS_CREATE_EDW;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       rsp_buffers));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       req_buffers));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_entry_lookup(unit,
                                0,
                                op_arg,
                                stage_id,
                                tbl_id,
                                group_id,
                                filter->group_oper_info,
                                parent_eid,
                                req_buffers,
                                rsp_buffers));

    /*
     * WRITE:Insert the child entry ID with data from
     * parent entry ID.
     */
    ekw = rsp_buffers->ekw;
    edw = rsp_buffers->edw;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_entry_insert(unit,
                                0,
                                op_arg,
                                pipe_id,
                                stage_id,
                                tbl_id,
                                group_id,
                                filter->group_oper_info,
                                child_eid,
                                entry_prio,
                                ekw,
                                edw,
                                NULL,
                                enable));
exit:
    bcmfp_filter_buffers_free(unit, rsp_buffers);
    bcmfp_filter_buffers_free(unit, req_buffers);
    SHR_FREE(req_buffers);
    SHR_FREE(rsp_buffers);
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_update_sbr(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_filter_t *filter)
{
    uint32_t sbr_profile_index = 0;
    bcmfp_stage_t *stage = NULL;
    uint8_t num_parts = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    SHR_NULL_CHECK(filter, SHR_E_PARAM);
    opinfo = filter->group_oper_info;
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /* Do a filter lookup to get the SBR index. */
    if (opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_GROUP_SBR_ENTRY ||
        opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_SBR_ENTRY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_lookup(unit,
                                 op_arg,
                                 stage->tbls.entry_tbl->sid,
                                 stage_id,
                                 filter,
                                 NULL));
        sbr_profile_index = filter->sbr_profile_index;
    }

    /* Allocate the buffers for SBR profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       BCMFP_BUFFERS_CREATE_SBR,
                                       tbl_id,
                                       stage,
                                       &(filter->buffers)));

    /* Populate the SBR profile buffers. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_sbr_config_update_process(unit,
                                                op_arg,
                                                tbl_id,
                                                FALSE,
                                                stage_id,
                                                filter));
    /* Get the num parts of the group. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &num_parts));

    /* Update the SBR profile in hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_profile_update(unit,
                                  op_arg->trans_id,
                                  opinfo->tbl_inst,
                                  stage->tbls.sbr_tbl->sid,
                                  stage_id,
                                  filter->buffers.sbr,
                                  num_parts,
                                  sbr_profile_index));
exit:
    bcmfp_filter_buffers_free(unit, &(filter->buffers));
    SHR_FUNC_EXIT();
}

static int
bcmfp_filter_update_with_di(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmltd_sid_t tbl_id,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_filter_t *filter_prev,
                            bcmfp_filter_t *filter_curr,
                            bool *entry_updated)
{
    bcmfp_rule_id_t rule_id = 0;
    bcmfp_rule_config_t *rconfig = NULL;
    bcmfp_entry_id_t entry_id = 0;
    bcmfp_entry_config_t *econfig = NULL;
    uint8_t idx = 0;
    bool entry_compressed = FALSE;
    bcmfp_filter_t filter_temp;

    SHR_FUNC_ENTER(unit);

    sal_memset(&filter_temp, 0, sizeof(bcmfp_filter_t));

    SHR_NULL_CHECK(entry_updated, SHR_E_PARAM);
    *entry_updated = FALSE;
    SHR_NULL_CHECK(filter_curr, SHR_E_PARAM);

    if (filter_prev != NULL) {
        if (filter_curr->entry_config == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (filter_prev->entry_config == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (filter_prev->entry_config->entry_state !=
            BCMFP_ENTRY_SUCCESS) {
            SHR_EXIT();
        }
        if (filter_prev->entry_config->rule_id == 0) {
            SHR_EXIT();
        }
        if (filter_prev->entry_config->group_id !=
            filter_curr->entry_config->group_id) {
            SHR_EXIT();
        }

        rule_id = filter_prev->entry_config->rule_id;
        rconfig = filter_prev->rule_config;
        if (rconfig == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_rule_remote_config_get(unit,
                                              op_arg,
                                              stage_id,
                                              rule_id,
                                              &rconfig));
            filter_prev->rule_config = rconfig;
        }

        if (rconfig == NULL) {
            SHR_EXIT();
        }

        for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
            if (rconfig->compress_types[idx] == TRUE) {
                entry_compressed = TRUE;
                break;
            }
        }

        if (entry_compressed == FALSE) {
            SHR_EXIT();
        }

        *entry_updated = TRUE;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_delete(unit,
                                 op_arg,
                                 tbl_id,
                                 stage_id,
                                 filter_prev));

        filter_curr->group_oper_info->ref_count--;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_create(unit,
                                 op_arg,
                                 tbl_id,
                                 stage_id,
                                 filter_curr));

        filter_curr->group_oper_info->ref_count++;
    } else {
        if (filter_curr->entry_config == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (filter_curr->entry_config->rule_id == 0) {
            SHR_EXIT();
        }
        if (filter_curr->entry_config->entry_state !=
            BCMFP_ENTRY_SUCCESS) {
            SHR_EXIT();
        }
        entry_id = filter_curr->entry_config->entry_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_remote_config_get(unit,
                                           op_arg,
                                           stage_id,
                                           entry_id,
                                           &econfig));
        filter_temp.entry_config = econfig;
        rule_id = filter_curr->entry_config->rule_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_rule_remote_config_get(unit,
                                          op_arg,
                                          stage_id,
                                          rule_id,
                                          &rconfig));
        filter_temp.rule_config = rconfig;
        for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
            if (rconfig->compress_types[idx] == TRUE) {
                entry_compressed = TRUE;
                break;
            }
        }

        if (entry_compressed == FALSE) {
            SHR_EXIT();
        }

        *entry_updated = TRUE;

        filter_temp.group_oper_info = filter_curr->group_oper_info;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_delete(unit,
                                 op_arg,
                                 tbl_id,
                                 stage_id,
                                 &filter_temp));

        filter_curr->group_oper_info->ref_count--;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_create(unit,
                                 op_arg,
                                 tbl_id,
                                 stage_id,
                                 filter_curr));

        filter_curr->group_oper_info->ref_count++;
    }

exit:
    if (entry_updated != NULL) {
        if (*entry_updated == TRUE) {
            bcmfp_filter_configs_free(unit, filter_curr);
            bcmfp_filter_configs_free(unit, filter_prev);
        }
    }
    bcmfp_filter_configs_free(unit, &filter_temp);
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_update(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t tbl_id,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_filter_t *filter_prev,
                    bcmfp_filter_t *filter_curr)
{
    bool global_pool = FALSE;
    bcmfp_entry_config_t *entry_config_dst = NULL;
    bcmfp_entry_state_t curr_state;
    bcmfp_entry_state_t prev_state;
    bcmfp_meter_id_t meter_id_curr = 0;
    bcmfp_meter_id_t meter_id_prev = 0;
    bcmfp_group_id_t group_id_prev = 0;
    bcmfp_group_id_t group_id_curr = 0;
    bcmfp_sbr_id_t sbr_id_prev = 0;
    bcmfp_sbr_id_t sbr_id_curr = 0;
    bcmfp_rule_config_t *incoming_rconfig = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_filter_t *temp_filter_prev = NULL;
    bool entry_updated = FALSE;
    bcmfp_filter_t *filter_temp = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter_curr, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_update_with_di(unit,
                                     op_arg,
                                     tbl_id,
                                     stage_id,
                                     filter_prev,
                                     filter_curr,
                                     &entry_updated));

    if (entry_updated == TRUE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (filter_curr->entry_config == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (filter_prev != NULL) {
       if (filter_prev->entry_config == NULL) {
           SHR_ERR_EXIT(SHR_E_PARAM);
       }
    }

    /*
     * Need to copy current entry config in case filter
     * was created successfully but now failed in
     * bcmfp_filter_create, then need to delete the prev
     * filter. In case of IDP(Interdependecy updates),
     * previous and current entry configs are same. So
     * bcmfp_filter_update call from IDP updates will
     * have filter_prev set to NULL. filter_prev is non
     * NULL only when bcmfp_filter_update is called from
     * bcmfp_entry_config_update.
     */
    BCMFP_ALLOC(entry_config_dst,
                sizeof(bcmfp_entry_config_t),
                "bcmfpFilterUpdateEntryConfig");
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_config_copy(unit,
                                 filter_curr->entry_config,
                                 entry_config_dst));

    /*
     * Entry state in both filter_curr and filter_prev
     * is always same because entry_state is RO field and
     * and cannot be updated by application. For previous
     * state should be copied from filter_curr as
     * filter_prev can be NULL some times.
     */
    prev_state = filter_curr->entry_config->entry_state;

    /* Save the previous and current meter IDs. */
    meter_id_curr = filter_curr->entry_config->meter_id;
    group_id_curr = filter_curr->entry_config->group_id;
    sbr_id_curr = filter_curr->entry_config->sbr_id;
    if (filter_prev != NULL) {
       meter_id_prev = filter_prev->entry_config->meter_id;
       group_id_prev = filter_prev->entry_config->group_id;
       sbr_id_prev = filter_prev->entry_config->sbr_id;
    } else {
       meter_id_prev = filter_curr->entry_config->meter_id;
       group_id_prev = filter_curr->entry_config->group_id;
       sbr_id_prev = filter_curr->entry_config->sbr_id;
    }

    if (sbr_id_prev == sbr_id_curr &&
        group_id_prev == group_id_curr &&
        prev_state == BCMFP_ENTRY_SUCCESS &&
        sbr_id_prev != 0) {
        filter_curr->sbr_dont_install = TRUE;
        if (filter_prev == NULL) {
            filter_temp = filter_curr;
        } else {
            filter_temp = filter_prev;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_lookup(unit,
                                 op_arg,
                                 tbl_id,
                                 stage_id,
                                 filter_temp,
                                 NULL));
        filter_curr->sbr_profile_index =
                     filter_temp->sbr_profile_index;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_create(unit,
                             op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter_curr));

    /* Current state after tried updating the entry in HW. */
    curr_state = filter_curr->entry_state;

    /*
     * If filter was successfully created previously but now
     * failed with updated configurations, then delete the
     * previous filter from HW.
     */
    if (prev_state == BCMFP_ENTRY_SUCCESS &&
        curr_state != BCMFP_ENTRY_SUCCESS &&
        (group_id_prev == group_id_curr)) {
        SHR_FREE(filter_curr->entry_config);
        if (filter_prev != NULL) {
            filter_curr->entry_config =
                filter_prev->entry_config;
            filter_prev->entry_config = NULL;
        } else {
            filter_curr->entry_config = entry_config_dst;
            entry_config_dst = NULL;
        }
        /*
         * Need to set the entry_state to SUCCESS
         * otherwise, bcmfp_filter_delete doesnot
         * delete the entry from HW.
         */
        filter_curr->entry_state = prev_state;

        /* Delete the previous install filter from HW. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_delete(unit,
                                 op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter_curr));
        /*
         * Revert the entrt_state to the actual
         * generated in bcmfp_filter_insert call
         * to update the entry state RO field.
         */
        filter_curr->entry_state = curr_state;
    }

    /*
     * If the previous meter and current meter are
     * not same and previous meter is installed in
     * HW successfully, then that meter has to be
     * removed from HW if it is not a global meter.
     */
    if (prev_state == BCMFP_ENTRY_SUCCESS &&
       (meter_id_curr != meter_id_prev) &&
       (meter_id_prev != 0)) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_pool_global(unit,
                                      stage->stage_id,
                                      &global_pool));
        if (global_pool == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_meter_entry_delete(unit,
                                           op_arg->trans_id,
                                           stage->stage_id,
                                           meter_id_prev));
        }
    }

exit:
    bcmfp_filter_configs_free(unit, filter_curr);
    bcmfp_filter_configs_free(unit, filter_prev);
    bcmfp_rule_config_free(unit, incoming_rconfig);
    SHR_FREE(entry_config_dst);
    SHR_FREE(temp_filter_prev);
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_delete(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t tbl_id,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_filter_t *filter)
{
    bcmfp_group_id_t group_id = 0;
    bcmfp_rule_id_t rule_id = 0;
    bcmfp_meter_id_t meter_id = 0;
    bcmfp_rule_config_t *rule_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    size_t size_one = 0;
    size_t size_two = 0;
    uint8_t idx = 0;
    uint32_t **ekw = NULL;
    bool global_pool = FALSE;
    uint32_t gi_flags = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_sid_t sbr_req_ltid;
    bcmfp_entry_id_t entry_id = 0;
    bool lc_status = 0;
    uint32_t sbr_hw_index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /*
     * If entry state is not success it will not be
     * installed in HW. No need to decrement the
     * ref_count in group operational information.
     */
    if (filter->entry_state != BCMFP_ENTRY_SUCCESS) {
        SHR_EXIT();
    }

    group_id = filter->entry_config->group_id;
    rule_id = filter->entry_config->rule_id;
    entry_id = filter->entry_config->entry_id;
    stage_id = stage->stage_id;

    entry_state = filter->entry_config->entry_state;
    if (entry_state != BCMFP_ENTRY_SUCCESS) {
        SHR_EXIT();
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_LIST_COMPRESSION)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_list_compress_status_get(unit,
                                                  stage_id,
                                                  entry_id,
                                                  &lc_status));
        filter->list_compressed = lc_status;
    }

    /*
     * If the group is enabled for compression, delete the
     * ALPM rules created for this filter.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_compress(unit,
                               op_arg,
                               stage_id,
                               BCMFP_OPCODE_DELETE,
                               NULL,
                               filter));
    if (filter->list_compressed == TRUE) {
        SHR_EXIT();
    }

    opinfo = filter->group_oper_info;

    /* Get the filter configs required. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ENTRY_TYPE_HASH) ||
        opinfo->ref_count == 1) {
        if (filter->group_config == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_remote_config_get(unit,
                                               op_arg,
                                               stage_id,
                                               group_id,
                                               &group_config));
            filter->group_config = group_config;
        }
    }
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ENTRY_TYPE_HASH) &&
        (rule_id != 0)) {
        if (filter->rule_config == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_rule_remote_config_get(unit,
                                              op_arg,
                                              stage_id,
                                              rule_id,
                                              &rule_config));
            filter->rule_config = rule_config;
        }
    }

    /* Allocate filter buffers. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        size_one = (sizeof(uint32_t *) *
                    BCMFP_ENTRY_PARTS_MAX);
        size_two = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
        BCMFP_ALLOC(ekw, size_one, "bcmfpEkw");
        filter->buffers.ekw = ekw;
        for (idx = 0; idx < BCMFP_ENTRY_PARTS_MAX; idx++) {
            BCMFP_ALLOC(ekw[idx], size_two, "bcmfpEkwPart");
        }
    }

    /* Delete the filter from the HW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_delete_hw(unit,
                                op_arg,
                                tbl_id,
                                stage,
                                filter));

    /* Get the group install flags for the stage as */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ENTRY_TYPE_HASH) ||
        opinfo->ref_count == 1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_group_install_flags_get(unit,
                                                  stage,
                                                  filter,
                                                  &gi_flags));
    }

    if (gi_flags != 0 && opinfo->ref_count == 1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_profiles_uninstall(unit,
                                      op_arg,
                                      gi_flags,
                                      stage,
                                      group_id,
                                      filter->group_oper_info));
        if (opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_SBR_GROUP) {
            sbr_req_ltid = stage->tbls.sbr_tbl->sid;
            /*
             * skip deleting sbr when sbr_id is 0
             * follows similar check to sbr_profile_install
             * call in bcmfp_entry_config_process
             */
            if (filter->group_config->sbr_id != 0 ) {
                sbr_hw_index = opinfo->ext_codes[0].sbr_prof_index;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_sbr_profile_uninstall(unit,
                                                 op_arg->trans_id,
                                                 opinfo->tbl_inst,
                                                 sbr_req_ltid,
                                                 stage->stage_id,
                                                 sbr_hw_index));
            }
            /* uninsatll the default sbr_id for hash stages */
            if (gi_flags & BCMFP_GROUP_INSTALL_DSBR_PROFILE) {
                if (filter->group_config->default_sbr_id != 0 ) {
                    sbr_hw_index =
                        opinfo->ext_codes[0].default_sbr_prof_index;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmfp_sbr_profile_uninstall(unit,
                                                     op_arg->trans_id,
                                                     opinfo->tbl_inst,
                                                     sbr_req_ltid,
                                                     stage->stage_id,
                                                     sbr_hw_index));
                }
            }

        }
    }

    if (opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_GROUP_SBR_ENTRY) {
        sbr_req_ltid = stage->tbls.sbr_tbl->sid;
        /*
         * skip deleting sbr when sbr_id is 0
         * follows similar check to sbr_profile_install
         * call in bcmfp_entry_config_process
         */
        if (filter->entry_config->sbr_id != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_sbr_profile_uninstall(unit,
                                             op_arg->trans_id,
                                             opinfo->tbl_inst,
                                             sbr_req_ltid,
                                             stage->stage_id,
                                             filter->sbr_profile_index));
        }
    }

    /* Detach the meters if entry has meter ID. */
    meter_id = filter->entry_config->meter_id;
    if (meter_id != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_pool_global(unit,
                                      stage_id,
                                      &global_pool));
        /*
         * If Meter Pool is not global, Meter entry
         * will be programmed in H/W when FP entry
         * is installed. Similarly, Meter entry should
         * be deleted when the FP entry is uninstalled.
         * If Meter Pool is Global, Meter entry will
         * be programmed and deleted solely by Meter
         * module.
         */
        if (global_pool == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_meter_entry_delete(unit,
                                           op_arg->trans_id,
                                           stage_id,
                                           meter_id));
        }
    }

exit:
    bcmfp_filter_buffers_free(unit, &(filter->buffers));
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_lookup(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t tbl_id,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_filter_t *filter,
                    bcmltd_fields_t *data)
{
    size_t size = 0;
    uint32_t bflags = 0, req_flags = 0;
    bcmfp_group_id_t group_id = 0;
    bcmfp_entry_id_t entry_id = 0;
    bcmfp_rule_id_t rule_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_buffers_t *req_buffers = NULL;
    bcmfp_buffers_t *rsp_buffers = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_rule_config_t *rule_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    uint32_t **ekw = NULL;
    int start_bit = 0;
    int end_bit = 0;
    uint8_t num_parts = 0;
    bcmfp_group_mode_t group_mode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    group_id = filter->entry_config->group_id;
    entry_id = filter->entry_config->entry_id;
    rule_id = filter->entry_config->rule_id;
    opinfo = filter->group_oper_info;
    group_mode = opinfo->group_mode;

    size = sizeof(bcmfp_buffers_t);
    BCMFP_ALLOC(req_buffers, size,
                "bcmFilterfpListCompressUpdateReqBuffers");
    BCMFP_ALLOC(rsp_buffers, size,
                "bcmFilterfpListCompressUpdateRspBuffers");
    bflags = BCMFP_BUFFERS_CREATE_EKW;
    bflags |= BCMFP_BUFFERS_CREATE_EDW;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       rsp_buffers));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       req_buffers));

    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_remote_config_get(unit,
                                           op_arg,
                                           stage_id,
                                           group_id,
                                           &group_config));
        filter->group_config = group_config;

        /*
         * skip extracting rule config for entries with no
         * rule_template_id attached (the case where rule_id = 0)
         */
        if (rule_id != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_rule_remote_config_get(unit,
                                              op_arg,
                                              stage_id,
                                              rule_id,
                                              &rule_config));
        }
        filter->rule_config = rule_config;

        ekw = req_buffers->ekw;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_em_entry_build_ekw(unit,
                                      op_arg,
                                      tbl_id,
                                      stage_id,
                                      group_id,
                                      opinfo,
                                      entry_id,
                                      filter->group_config,
                                      filter->rule_config,
                                      ekw));
    }

    if (op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW) {
        req_flags |= BCMFP_ENTRY_READ_FROM_HW;
    }

    if (filter->entry_config->entry_state == BCMFP_ENTRY_SUCCESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_entry_lookup(unit,
                                    req_flags,
                                    op_arg,
                                    stage_id,
                                    tbl_id,
                                    group_id,
                                    opinfo,
                                    entry_id,
                                    req_buffers,
                                    rsp_buffers));
    }

    /*
     * Retrive the SBR index from response buffer as entry to SBR
     * index is not saved in anywhere. This is required to either
     * delete or update the SBR profile associated to the entry.
     */
    if (opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_GROUP_SBR_ENTRY &&
        filter->entry_config->entry_state == BCMFP_ENTRY_SUCCESS) {
        if (BCMFP_STAGE_FLAGS_TEST(stage,
                  BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE)) {
            /* Get the num parts of the group. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_parts_count(unit,
                                         FALSE,
                                         opinfo->flags,
                                         &num_parts));
            start_bit = (stage->pdd_hw_info->raw_policy_width -
                         stage->misc_info->per_entry_sbr_raw_data_offset);
            end_bit = (start_bit + stage->misc_info->sbr_index_size - 1);
            bcmdrd_field_get(rsp_buffers->edw[num_parts - 1],
                             start_bit,
                             end_bit,
                             &(filter->sbr_profile_index));
        }
        if (BCMFP_STAGE_FLAGS_TEST(stage,BCMFP_STAGE_ENTRY_TYPE_HASH)) {
            start_bit = stage->key_data_size_info->data_start_offset[group_mode] + 1 -
                        stage->misc_info->per_entry_sbr_raw_data_offset;
            end_bit = (start_bit + stage->misc_info->sbr_index_size - 1);
            /*sbr index is always in part 0 for EM and EM_FT*/
            bcmdrd_field_get(rsp_buffers->ekw[0],
                             start_bit,
                             end_bit,
                             &(filter->sbr_profile_index));

        }
    }
exit:
    bcmfp_filter_buffers_free(unit, rsp_buffers);
    bcmfp_filter_buffers_free(unit, req_buffers);
    SHR_FREE(req_buffers);
    SHR_FREE(rsp_buffers);
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_pse_lookup(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_filter_t *filter,
                        bcmltd_fields_t *data)
{

    bcmfp_stage_t *stage = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    size_t size = 0;
    uint32_t bflags = 0, req_flags = 0;
    bcmfp_buffers_t *req_buffers = NULL;
    bcmfp_buffers_t *rsp_buffers = NULL;
    bcmfp_pse_id_t pse_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    group_id = filter->entry_config->group_id;
    pse_id = filter->pse_config[0]->pse_id;
    opinfo = filter->group_oper_info;

    size = sizeof(bcmfp_buffers_t);
    BCMFP_ALLOC(req_buffers, size,
                "bcmfpPseLookupReqBuffers");
    BCMFP_ALLOC(rsp_buffers, size,
                "bcmfpPseLookupRspBuffers");
    bflags = BCMFP_BUFFERS_CREATE_EKW;
    bflags |= BCMFP_BUFFERS_CREATE_EDW;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       rsp_buffers));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       req_buffers));

    if (op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW) {
        req_flags |= BCMFP_ENTRY_READ_FROM_HW;
    }

    req_flags |= BCMFP_ENTRY_PRESEL;

    if (filter->entry_config->entry_state == BCMFP_ENTRY_SUCCESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_entry_lookup(unit,
                                    req_flags,
                                    op_arg,
                                    stage_id,
                                    tbl_id,
                                    group_id,
                                    opinfo,
                                    pse_id,
                                    req_buffers,
                                    rsp_buffers));
    }


exit:
    bcmfp_filter_buffers_free(unit, rsp_buffers);
    bcmfp_filter_buffers_free(unit, req_buffers);
    SHR_FREE(req_buffers);
    SHR_FREE(rsp_buffers);
    SHR_FUNC_EXIT();
}

int
bcmfp_filter_presel_update(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_filter_t *filter)
{
    int rv;
    bcmltd_sid_t tbl_id;
    bcmltd_sid_t pse_tbl_id;
    uint8_t idx = 0;
    uint32_t psg_id = 0;
    uint32_t bflags = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(filter, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    tbl_id = stage->tbls.entry_tbl->sid;
    pse_tbl_id = stage->tbls.pse_tbl->sid;
    psg_id = filter->group_config->psg_id;

    if (filter->psg_config != NULL && psg_id != 0)  {
        /* Check all qualifiers are present in presel group as well */
        for (idx = 0; idx < filter->num_pse_configs; idx++) {
            rv = bcmfp_filter_rule_qual_presence_validate(unit,
                                                          TRUE,
                                                          idx,
                                                          filter);
           if (rv == SHR_E_CONFIG) {
               filter->entry_state = BCMFP_ENTRY_PRESEL_QSET_NOT_IN_PSG;
           }
           SHR_IF_ERR_EXIT(rv);
        }
    }

    bflags = BCMFP_BUFFERS_CREATE_EKW;
    bflags |= BCMFP_BUFFERS_CREATE_EDW;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       bflags,
                                       tbl_id,
                                       stage,
                                       &(filter->buffers)));

    /* Create the buffers required for ekw, wdw and profiles. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_pse_config_process(unit,
                                         op_arg,
                                         tbl_id,
                                         stage,
                                         0,
                                         filter));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_insert_hw(unit,
                                op_arg,
                                pse_tbl_id,
                                stage,
                                TRUE,
                                0,
                                filter));


exit:
    bcmfp_filter_buffers_free(unit, &(filter->buffers));
    SHR_FUNC_EXIT();
}
