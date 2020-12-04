/*! \file bcmfp_cth_sbr.c
 *
 * APIs for FP sbr template LTs custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate sbr config provided
 * using sbr template LTs.
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
#include <bcmfp/bcmfp_cth_filter.h>
#include <bcmfp/bcmfp_ptm_internal.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm_uft.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

STATIC int
bcmfp_sbr_id_get(int unit,
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

    if (stage->tbls.sbr_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sbr_id_fid = stage->tbls.sbr_tbl->key_fid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_value_get(unit,
                             sbr_id_fid,
                             (void *)buffer,
                             &sbr_id_u64));

    *sbr_id = sbr_id_u64 & 0xFFFFFFFF;
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_sbr_action_data_get(int unit,
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

    if (stage->tbls.sbr_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    actions_fid_info =
        stage->tbls.sbr_tbl->actions_fid_info;
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
                    "bcmfpSbrActionData");
        /*
         * Add the new node to the linked list of
         * action data.
         */
        action_data->next = *ad;
        *ad = action_data;

        /* Update the action. */
        action_data->action =
            actions_fid_info[buffer->id].action;

        /* Update whether the action priority */
        action_data->param = buffer->data & 0xFFFFFFFF;

        action_data = NULL;
        buffer = buffer->next;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_sbr_config_free(int unit,
                      bcmfp_sbr_config_t *sbr_config)
{
    bcmfp_action_data_t *action_data = NULL;

    SHR_FUNC_ENTER(unit);

    if (sbr_config == NULL) {
        SHR_EXIT();
    }

    action_data = sbr_config->action_data;
    while (action_data != NULL) {
        sbr_config->action_data = action_data->next;
        SHR_FREE(action_data);
        action_data = sbr_config->action_data;
    }

    SHR_FREE(sbr_config);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_sbr_config_get(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     bcmltd_sid_t tbl_id,
                     bcmfp_stage_id_t stage_id,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     bcmfp_sbr_config_t **config)
{
    uint32_t sbr_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *buffer = NULL;
    bcmfp_sbr_config_t *sbr_config = NULL;
    bcmfp_action_data_t *action_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.sbr_tbl == NULL ||
        stage->tbls.sbr_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(sbr_config,
                sizeof(bcmfp_sbr_config_t),
                "bcmfpSbrConfig");
    *config = sbr_config;

    /* Get the SBR ID */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_id_get(unit, stage, buffer, &sbr_id));
    sbr_config->sbr_id = sbr_id;

    /*
     * if data is NULL no need to extract the
     * remaining config.
     */
    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get the SBR actions priority. */
    buffer = (bcmltd_field_t *)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_action_data_get(unit, stage, buffer, &action_data));
    sbr_config->action_data = action_data;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_sbr_remote_config_get(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_sbr_id_t sbr_id,
                            bcmfp_sbr_config_t **config)
{
    bcmltd_fid_t key_fid;
    bcmltd_sid_t tbl_id;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmfp_sbr_config_t *sbr_config = NULL;
    size_t num_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    *config = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_fid = stage->tbls.sbr_tbl->key_fid;
    tbl_id = stage->tbls.sbr_tbl->sid;
    num_fid = stage->tbls.sbr_tbl->lrd_info.num_fid;

    /* Get the FP sbr LT entry saved in IMM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_lookup(unit,
                                tbl_id,
                                &key_fid,
                                &sbr_id,
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

    /* Get the FP sbr LT entry config of ACL. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage->stage_id,
                              key,
                              data,
                              &sbr_config));
    *config = sbr_config;

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_sbr_action_fid_get(int unit,
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
        stage->tbls.sbr_tbl->actions_fid_info;
    if (actions_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < stage->tbls.sbr_tbl->fid_count; idx++) {
        if (actions_fid_info[idx].action == action) {
            *fid = idx;
            break;
        }
    }

    if (idx == stage->tbls.sbr_tbl->fid_count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_sbr_config_dump(int unit,
                      bcmltd_sid_t tbl_id,
                      bcmfp_stage_t *stage,
                      bcmfp_sbr_config_t *sbr_config)
{
    bcmfp_action_data_t *action_data = NULL;
    bcmlrd_field_def_t field_def;
    bcmlrd_fid_t fid = 0;
    bcmfp_action_t action;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(sbr_config, SHR_E_PARAM);

    if (stage->tbls.sbr_tbl->key_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "SBR ID = %d\n"),
            sbr_config->sbr_id));
    }

    action_data = sbr_config->action_data;
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit, "Actions priority:\n")));
    while (action_data != NULL) {
        sal_memset(&field_def, 0, sizeof(bcmlrd_field_def_t));
        action = action_data->action;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_action_fid_get(unit,
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

static int
bcmfp_sbr_entry_uft_hw_index_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 uint32_t entry_id,
                                 uint32_t src_id,
                                 void *user_data,
                                 bool *cond_met)
{
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    uint32_t *temp_hw_index = NULL;
    uint32_t group_id = 0;
    bcmfp_idp_info_t *idp_info;
    bcmfp_filter_t filter;
    bcmimm_lookup_type_t lkup_type = BCMIMM_LOOKUP;
    bcmltd_fields_t *data = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_sid_t tbl_id;

    SHR_FUNC_ENTER(unit);

    sal_memset(&filter, 0, sizeof(bcmfp_filter_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    tbl_id = stage->tbls.entry_tbl->sid;

    idp_info = (bcmfp_idp_info_t *)user_data;

    if (idp_info->entry_id == entry_id) {
        SHR_EXIT();
    }

    sal_memset(&filter,0,sizeof(bcmfp_filter_t));

    temp_hw_index = (uint32_t *)idp_info->generic_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));

    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        bcmfp_entry_config_free(unit,entry_config);
        entry_config = NULL;
        SHR_EXIT();
    }

    /* incr the entry count mapped to the sbr_id */
    idp_info->entry_or_group_count += 1;

    group_id = entry_config->group_id;

    /* get the group oper info to extract the pdd_type */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    filter.entry_config = entry_config;
    filter.group_oper_info = opinfo;
    filter.lkup_type = lkup_type;

    /* extract the sbr hw index from the entry data in HW */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_lookup(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage_id,
                             &filter,
                             data));
    *temp_hw_index = filter.sbr_profile_index;
    *cond_met = TRUE;

exit:
    bcmfp_filter_configs_free(unit, &filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_sbr_group_uft_hw_index_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 uint32_t group_id,
                                 uint32_t sbr_id,
                                 void *user_data,
                                 bool *cond_met)
{
    bcmfp_group_oper_info_t *opinfo = NULL;
    uint32_t *temp_hw_index = NULL;
    bcmfp_idp_info_t *idp_info;

    SHR_FUNC_ENTER(unit);

    idp_info = (bcmfp_idp_info_t *)user_data;
    temp_hw_index = (uint32_t *)idp_info->generic_data;

    /* check if the grp is successfully created */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));


    if (opinfo->state == BCMFP_GROUP_STATE_SUCCESS) {
        /*
         * get the sbr hw idx mapped to the group_id
         * for cases where valid entries are still
         * attached to the group
         */
        if (opinfo->ref_count != 0) {
            /* incr the entry count mapped to the sbr_id */
            idp_info->entry_or_group_count += 1;
            *temp_hw_index = opinfo->sbr_id;
            *cond_met = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_uft_sbr_hw_index_get(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_sbr_id_t sbr_id,
                           void *user_data)
{
    bool not_mapped = FALSE;
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_id_cond_traverse_cb cb_func;
    bcmfp_idp_info_t *idp_info;
    bool cond_met = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

     /* check for groups mapped to the sbr_id */
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_group_map_check(unit,
                                   stage_id,
                                   sbr_id,
                                   &not_mapped));
     /*
      * check if the groups mapped to the sbr_id have any succesful
      * entries installed in hw
      */
     if (not_mapped == FALSE) {
         cb_func = bcmfp_sbr_group_uft_hw_index_get;
         SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_group_map_cond_traverse(unit,
                                               stage_id,
                                               sbr_id,
                                               cb_func,
                                               user_data,
                                               &cond_met));
     }

     /*
      * if none of the groups' entries using the sbr_id are
      * succesfully installed,check if any entries are
      * mapped to the sbr_id
      */
     idp_info = user_data;
     /*
      * if the valid grps using the sbr_id is non-zero
      * no need to account for the entries using the same sbr_id
      */
     if (idp_info->entry_or_group_count) {
         SHR_EXIT();
     }

     not_mapped = TRUE;
     cond_met = FALSE;
     if (BCMFP_STAGE_FLAGS_TEST(stage,
                 BCMFP_STAGE_SBR_PER_ENTRY)) {
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_sbr_entry_map_check(unit,
                                        stage_id,
                                        sbr_id,
                                        &not_mapped));
     }

      /*
       * check if the entries mapped to the sbr_id aresuccesful
       * entries installed in hw
       */
     if (not_mapped == FALSE) {
         cb_func = bcmfp_sbr_entry_uft_hw_index_get;
         SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_entry_map_cond_traverse(unit,
                                               stage_id,
                                               sbr_id,
                                               cb_func,
                                               user_data,
                                               &cond_met));
     }

exit:
     SHR_FUNC_EXIT();
}

int
bcmfp_uft_sbr_entry_alloc(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          int pipe_id,
                          bcmltd_sid_t req_ltid,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_entry_id_t entry_id,
                          uint32_t **sbr,
                          uint8_t num_parts,
                          uint32_t *hw_index,
                          bcmfp_sbr_id_t sbr_id)
{
    uint8_t row = 0;
    uint8_t col = 0;
    uint8_t sbr_col = 0;
    bcmfp_profile_hw_info_t *sbr_profile_hw_info = NULL;
    bool paired_lookups = FALSE;
    uint16_t entry_index[BCMFP_SBR_ENTRY_CNT_MAX] = {0};
    uint16_t entry_count = 0;
    uint16_t entry_idx = 0;
    bcmfp_stage_t *stage = NULL;
    bcmptm_uft_sbr_lt_lookup_t lookup_type = BCMPTM_UFT_SBR_LOOKUP_INVALID;
    bcmdrd_sid_t profile_sid;
    uint32_t *profile_data = NULL;
    bcmdrd_sid_t *spt_sid = NULL;
    bcmfp_idp_info_t idp_info;
    uint32_t temp_hw_index = 0;
    void *user_data = NULL;
    void *generic_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sbr, SHR_E_PARAM);
    SHR_NULL_CHECK(hw_index, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sbr_profile_hw_info = stage->profiles_hw_info.sbr_profile_hw_info;
    if (sbr_profile_hw_info->columns > BCMFP_SBR_PROFILE_PARTS_MAX) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (sbr_profile_hw_info->rows == 0) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE)) {
        sbr_col = ((num_parts - 1) * sbr_profile_hw_info->columns);
    }

     /* paired lookup is true only for hash lts alone (EM_FP/EM_FT) */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        paired_lookups = TRUE;
        entry_count = 2;
    } else {
        paired_lookups = FALSE;
        entry_count = 1;
    }

    lookup_type = BCMPTM_UFT_SBR_LOOKUP0;

    /* get the sbr index to be used in all rows */
    BCMFP_ALLOC(spt_sid,
                sizeof(bcmdrd_sid_t) * sbr_profile_hw_info->rows,
                "bcmfpSbrSidArr");

    for (row = 0; row < sbr_profile_hw_info->rows; row++) {
        spt_sid[row] = sbr_profile_hw_info->sids[row][0];
    }


    /*
     * check whether the sbr_id is already installed
     * i.e if this sbr_id is in use by other groups/entries
     */
    generic_data = (void *)&temp_hw_index;
    idp_info.generic_data = generic_data;
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.group_id = group_id;
    idp_info.entry_id = entry_id;

    /*
     * this count is used to account for all entries succesfully
     * installed in HW that are using sbr_template_index - sbr_id
     */
    idp_info.entry_or_group_count = 0;
    user_data = &idp_info;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_uft_sbr_hw_index_get(unit,
                                    op_arg,
                                    stage_id,
                                    sbr_id,
                                    user_data));

    /*
     * if there are other entries using the sbr_id
     * use the same hw_index used by them
     */
    if (idp_info.entry_or_group_count != 0) {
        *hw_index = temp_hw_index;
        SHR_EXIT();
    }

    /*
     * call the UFT MGR Api only if this is the first time the sbr_id is
     * being installed in HW
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_sbr_multi_tables_entry_alloc(unit,
                                                 sbr_profile_hw_info->rows,
                                                 spt_sid,
                                                 req_ltid,
                                                 lookup_type,
                                                 paired_lookups,
                                                 entry_count,
                                                 &entry_index[0]));

    /* program the sbr profiles at the allocated entries */
    for (row = 0; row < sbr_profile_hw_info->rows; row++) {
         for (col = 0; col < sbr_profile_hw_info->columns; col++) {
             profile_sid = sbr_profile_hw_info->sids[row][col];
             profile_data = sbr[sbr_col + col];
             for (entry_idx = 0; entry_idx < entry_count; entry_idx++) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmfp_ptm_index_insert(unit,
                                             op_arg->trans_id,
                                             pipe_id,
                                             req_ltid,
                                             profile_sid,
                                             profile_data,
                                             entry_index[entry_idx]));
             }
         }
    }

    /*
     * set the hw_index to the min of the allocated entries.
     * This is because for EM_FP the profile index to be
     * programmed in the entry ranges from 0-15
     */
    for (entry_idx = 0; entry_idx < entry_count; entry_idx++) {
        if (entry_idx == 0) {
            *hw_index = entry_index[entry_idx];
            continue;
        }
        if (entry_index[entry_idx] < *hw_index) {
            *hw_index = entry_index[entry_idx];
        }
    }

exit:
    SHR_FREE(spt_sid);
    SHR_FUNC_EXIT();
}

static int
bcmfp_uft_mgr_sbr_prof_ref_count_get(int unit,
                                     bcmfp_stage_id_t stage_id,
                                     bcmfp_ref_count_t **ref_count)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_oper_mode_t oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
    int pipe_id = -1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));

    /*
     * UFT-MGR/SBR-RM operates in global mode alone.
     * So, the profile ref count for the stages
     * supported by UFT-MGR is maintained in pipe0's ref count
     * for per-pipe mode ang in global ref count in global mode.
     */
    if (oper_mode != BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
        pipe_id = -1;
    } else if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
        pipe_id = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_prof_ref_count_get(unit,
                                      stage_id,
                                      pipe_id,
                                      ref_count));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_sbr_profile_install(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          int pipe_id,
                          bcmltd_sid_t req_ltid,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_entry_id_t entry_id,
                          uint32_t **sbr,
                          uint8_t num_parts,
                          uint32_t *hw_index,
                          bcmfp_sbr_id_t sbr_id)
{
    uint32_t *profile_data_arr[BCMFP_SBR_PROFILE_PARTS_MAX];
    int index = -1;
    bcmfp_ref_count_t *ref_count = NULL;
    bcmdrd_sid_t first_row_sids[BCMFP_SBR_PROFILE_PARTS_MAX];
    uint32_t *profile_data = NULL;
    bcmdrd_sid_t profile_sid;
    uint8_t row = 0;
    uint8_t col = 0;
    uint8_t sbr_col = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_profile_hw_info_t *sbr_profile_hw_info = NULL;
    int max_idx = 0;
    int min_idx = 0;
    uint8_t profile = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    shr_error_t rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sbr, SHR_E_PARAM);
    SHR_NULL_CHECK(hw_index, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit,
                                   stage_id,
                                   &stage_oper_info));

    sbr_profile_hw_info = stage->profiles_hw_info.sbr_profile_hw_info;
    if (sbr_profile_hw_info->columns > BCMFP_SBR_PROFILE_PARTS_MAX) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (sbr_profile_hw_info->rows == 0) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE)) {
        sbr_col = ((num_parts - 1) * sbr_profile_hw_info->columns);
    }

    for (col = 0; col < sbr_profile_hw_info->columns; col++) {
        profile_data_arr[col] = sbr[sbr_col + col];
        first_row_sids[col] = sbr_profile_hw_info->sids[0][col];
    }

    /*
     * use the bcmptm_sbr_entry_alloc API if supported on the first row sids
     * else continue with allocating sbr entry using profile_add_range apis
     */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_SBR_UFTMGR_SUPPORT)) {
        /*
         * pipe_id : -1 as the UFT-MGR/SBR-RM operates
         * only in global mode
         */
        rv = bcmfp_uft_sbr_entry_alloc(unit,
                                       op_arg,
                                       -1,
                                       req_ltid,
                                       stage_id,
                                       group_id,
                                       entry_id,
                                       sbr,
                                       num_parts,
                                       hw_index,
                                       sbr_id);
        if (rv == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_uft_mgr_sbr_prof_ref_count_get(unit,
                                                      stage_id,
                                                      &ref_count));

            ref_count[*hw_index].ref_count += 1;
            /*
             * set the flag which indicates that the
             * sbr_hw index for the stage is aloocated by
             * the UFT MGR*/
            stage_oper_info->sbr_uftmgr_allocated = TRUE;
            SHR_EXIT();
        }
       /*
        * proceed with allocation of sbr_entries only when
        * sbr uft manager doesnt support the allocation for the
        * required sbr sids
        */
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    }

     SHR_IF_ERR_VERBOSE_EXIT
         (bcmfp_sbr_prof_ref_count_get(unit,
                                       stage_id,
                                       pipe_id,
                                       &ref_count));

     /*
      * for hash lts only half of the entries in sbr_profile_table
      * can be used. The entries in the first half of the table must
      * be the same as the entires in the second half
      * => Search for free indices only in the second half of the
      * sbr profile table for these LTs
      */
     if (BCMFP_STAGE_FLAGS_TEST(stage,
                 BCMFP_STAGE_ENTRY_TYPE_HASH)) {
         max_idx = stage->sbr_hw_info->max_sbr_idx;
         min_idx = stage->sbr_hw_info->min_sbr_idx;

         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_profile_add_range(unit,
                                      op_arg->trans_id,
                                      pipe_id,
                                      req_ltid,
                                      sbr_profile_hw_info->columns,
                                      first_row_sids,
                                      profile_data_arr,
                                      ref_count,
                                      &index,
                                      max_idx,
                                      min_idx));
         /* insert the profile in the first half of sbr profile table in row0 */
         for (profile = 0; profile < sbr_profile_hw_info->columns; profile++) {
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_ptm_index_insert(unit,
                                         op_arg->trans_id,
                                         pipe_id,
                                         req_ltid,
                                         first_row_sids[profile],
                                         profile_data_arr[profile],
                                         (index +
                                          stage->sbr_hw_info->max_sbr_idx + 1)));
         }
     } else {
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_profile_add(unit,
                                op_arg->trans_id,
                                pipe_id,
                                req_ltid,
                                sbr_profile_hw_info->columns,
                                first_row_sids,
                                profile_data_arr,
                                ref_count,
                                &index));
     }

     for (row = 1; row < sbr_profile_hw_info->rows; row++) {
         for (col = 0; col < sbr_profile_hw_info->columns; col++) {
             profile_sid = sbr_profile_hw_info->sids[row][col];
             profile_data = sbr[sbr_col + col];
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_ptm_index_insert(unit,
                                         op_arg->trans_id,
                                         pipe_id,
                                         req_ltid,
                                         profile_sid,
                                         profile_data,
                                         index));

             /*
              * for hash lts only half of the entries in sbr_profile_table
              * can be used. The entries in the first half of the table must
              * be the same as the entires in the second half
              */

             if (BCMFP_STAGE_FLAGS_TEST(stage,
                         BCMFP_STAGE_ENTRY_TYPE_HASH)) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmfp_ptm_index_insert(unit,
                                             op_arg->trans_id,
                                             pipe_id,
                                             req_ltid,
                                             profile_sid,
                                             profile_data,
                                             (index +
                                              stage->sbr_hw_info->max_sbr_idx + 1)));
             }
         }
     }
     *hw_index = index;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_uft_sbr_entry_free(int unit,
                         uint32_t trans_id,
                         int pipe_id,
                         bcmltd_sid_t req_ltid,
                         bcmfp_stage_id_t stage_id,
                         uint32_t hw_index)
{
    uint8_t row = 0;
    uint8_t col = 0;
    uint16_t entry_count = 0;
    uint16_t entry_idx = 0;
    bcmfp_stage_t *stage = NULL;
    bcmdrd_sid_t profile_sid;
    bcmfp_profile_hw_info_t *sbr_profile_hw_info = NULL;
    uint16_t entry_index[BCMFP_SBR_ENTRY_CNT_MAX] = {0};
    bcmdrd_sid_t *spt_sid = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sbr_profile_hw_info = stage->profiles_hw_info.sbr_profile_hw_info;

    if (BCMFP_STAGE_FLAGS_TEST(stage,
                BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        entry_count = 2;
        entry_index[0] = hw_index+
                         stage->sbr_hw_info->max_sbr_idx + 1;
        entry_index[1] = hw_index;
    } else {
        entry_count = 1;
        entry_index[0] = hw_index;
    }

    BCMFP_ALLOC(spt_sid,
                sizeof(bcmdrd_sid_t) * sbr_profile_hw_info->rows,
                "bcmfpSbrSidArr");

    for (row = 0; row < sbr_profile_hw_info->rows; row++) {
        spt_sid[row] = sbr_profile_hw_info->sids[row][0];
    }

    /* free the sbr entry only when ref_count is 0 */

    /* free the sbr entries allocated by UFT in every tile/row */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_sbr_multi_tables_entry_free(unit,
                                                sbr_profile_hw_info->rows,
                                                spt_sid,
                                                entry_count,
                                                &entry_index[0]));

    /* delete the sbr entries in HW */
    for (row = 0; row < sbr_profile_hw_info->rows; row++) {
        for (col = 0; col < sbr_profile_hw_info->columns; col++) {
            profile_sid = sbr_profile_hw_info->sids[row][col];
            for (entry_idx = 0; entry_idx < entry_count; entry_idx++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_ptm_index_delete(unit,
                                            trans_id,
                                            pipe_id,
                                            req_ltid,
                                            profile_sid,
                                            entry_index[entry_idx]));
            }
        }
    }
exit:
    SHR_FREE(spt_sid);
    SHR_FUNC_EXIT();

}


int
bcmfp_sbr_profile_uninstall(int unit,
                            uint32_t trans_id,
                            int pipe_id,
                            bcmltd_sid_t req_ltid,
                            bcmfp_stage_id_t stage_id,
                            uint32_t hw_index)
{
    int index = -1;
    bcmfp_ref_count_t *ref_count = NULL;
    bcmdrd_sid_t first_row_sids[BCMFP_SBR_PROFILE_PARTS_MAX];
    bcmdrd_sid_t profile_sid;
    uint8_t row = 0;
    uint8_t col = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_profile_hw_info_t *sbr_profile_hw_info = NULL;
    uint8_t profile = 0;
    shr_error_t rv = SHR_E_NONE;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit,
                                   stage_id,
                                   &stage_oper_info));


    /*
     *  Use UFT sbr entry free for LTs which are supported
     *  by UFT manager
     */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_SBR_UFTMGR_SUPPORT) &&
            stage_oper_info->sbr_uftmgr_allocated) {

        SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_uft_mgr_sbr_prof_ref_count_get(unit,
                                                      stage_id,
                                                      &ref_count));
        /*
         * Error raised when trying to free hw_index
         * which is not associated with any grp or entry
         */
        if (ref_count[hw_index].ref_count == 0) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        ref_count[hw_index].ref_count -=1 ;
        /*
         * if there are still valid entries using the sbr hw index
         * skip deleting the sbr entry
         */
        if (ref_count[hw_index].ref_count) {
            SHR_EXIT();
        }
        /*
         * Entry is to be deleted from all pipes irrespective of
         * global mode or per-pipe mode as UFT-MGR operates only in
         * global mode.
         */
        rv = bcmfp_uft_sbr_entry_free(unit,
                                      trans_id,
                                      -1,
                                      req_ltid,
                                      stage_id,
                                      hw_index);
        SHR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_prof_ref_count_get(unit,
                                      stage_id,
                                      pipe_id,
                                      &ref_count));

    sbr_profile_hw_info = stage->profiles_hw_info.sbr_profile_hw_info;
    for (col = 0; col < sbr_profile_hw_info->columns; col++) {
        first_row_sids[col] = sbr_profile_hw_info->sids[0][col];
    }

    index = hw_index;
    /*
     * for hash lts only half of the entries in sbr_profile_table
     * can be used. The entries in the first half of the table must
     * be the same as the entires in the second half
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_profile_delete(unit,
                              trans_id,
                              pipe_id,
                              req_ltid,
                              sbr_profile_hw_info->columns,
                              first_row_sids,
                              ref_count,
                              index));
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        for (profile = 0; profile < sbr_profile_hw_info->columns; profile++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_index_delete(unit,
                                        trans_id,
                                        pipe_id,
                                        req_ltid,
                                        first_row_sids[profile],
                                        (index+
                                         stage->sbr_hw_info->max_sbr_idx + 1)));
        }
    }
    if (ref_count[index].ref_count != 0) {
        SHR_EXIT();
    }

    for (row = 1; row < sbr_profile_hw_info->rows; row++) {
        for (col = 0; col < sbr_profile_hw_info->columns; col++) {
            profile_sid = sbr_profile_hw_info->sids[row][col];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_index_delete(unit,
                                        trans_id,
                                        pipe_id,
                                        req_ltid,
                                        profile_sid,
                                        index));
            /*
             * for hash lts the entries in the first half of the table
             * are the same as the entires in the second half
             * Delete the entries in the second half as well
             */
            if (BCMFP_STAGE_FLAGS_TEST(stage,
                BCMFP_STAGE_ENTRY_TYPE_HASH)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_ptm_index_delete(unit,
                                            trans_id,
                                            pipe_id,
                                            req_ltid,
                                            profile_sid,
                                            (index +
                                             stage->sbr_hw_info->max_sbr_idx + 1)));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_sbr_profile_update(int unit,
                         uint32_t trans_id,
                         int pipe_id,
                         bcmltd_sid_t req_ltid,
                         bcmfp_stage_id_t stage_id,
                         uint32_t **sbr,
                         uint8_t num_parts,
                         uint32_t hw_index)
{
    uint32_t *profile_data = NULL;
    bcmdrd_sid_t profile_sid;
    uint8_t row = 0;
    uint8_t col = 0;
    uint8_t sbr_col = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_profile_hw_info_t *sbr_profile_hw_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sbr, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sbr_profile_hw_info = stage->profiles_hw_info.sbr_profile_hw_info;
    if (sbr_profile_hw_info->columns > BCMFP_SBR_PROFILE_PARTS_MAX) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (BCMFP_STAGE_FLAGS_TEST(stage,
              BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE)) {
        sbr_col = ((num_parts - 1) * sbr_profile_hw_info->columns);
    }

    for (row = 0; row < sbr_profile_hw_info->rows; row++) {
        for (col = 0; col < sbr_profile_hw_info->columns; col++) {
            profile_sid = sbr_profile_hw_info->sids[row][col];
            profile_data = sbr[sbr_col + col];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_index_insert(unit,
                                        trans_id,
                                        pipe_id,
                                        req_ltid,
                                        profile_sid,
                                        profile_data,
                                        hw_index));
            /*
             * for hash lts the entries in the first half of the table
             * are the same as the entires in the second half
             * Update the entries in the second half as well
             */
            if (BCMFP_STAGE_FLAGS_TEST(stage,
                BCMFP_STAGE_ENTRY_TYPE_HASH)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_ptm_index_insert(unit,
                                            trans_id,
                                            pipe_id,
                                            req_ltid,
                                            profile_sid,
                                            profile_data,
                                            (hw_index +
                                             stage->sbr_hw_info->max_sbr_idx + 1)));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_sbr_config_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_sbr_config_t *sbr_config = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_sbr_id_t sbr_id = 0;
    bool not_mapped = FALSE;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;

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
        (bcmfp_sbr_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              non_default_data,
                              &sbr_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_config_dump(unit,
                                   tbl_id,
                                   stage,
                                   sbr_config));
    }

    /* Process groups associated to this sbr. */
    sbr_id = sbr_config->sbr_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_group_map_check(unit,
                                   stage_id,
                                   sbr_id,
                                   &not_mapped));

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_SBR_TEMPLATE;
    idp_info.key = key;
    idp_info.data = non_default_data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_INSERT;
    idp_info.entry_state = state;
    user_data = &idp_info;
    if (not_mapped == FALSE) {
        cb_func = bcmfp_group_idp_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_group_map_traverse(unit,
                                          stage_id,
                                          sbr_id,
                                          cb_func,
                                          user_data));
    }

    /* Process entries associated to this sbr. */
    not_mapped = TRUE;
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_SBR_PER_ENTRY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_entry_map_check(unit,
                                       stage_id,
                                       sbr_id,
                                       &not_mapped));
    }

    if (not_mapped == FALSE) {
        cb_func = bcmfp_entry_idp_sbr_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_entry_map_traverse(unit,
                                          stage_id,
                                          sbr_id,
                                          cb_func,
                                          user_data));
    }

exit:
    bcmfp_sbr_config_free(unit, sbr_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_sbr_config_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_sbr_config_t *sbr_config = NULL;
    uint16_t num_fid = 0;
    bcmltd_field_t *in_key = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_fields_t updated_fields;
    const bcmltd_field_t *updated_data = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_sbr_id_t sbr_id = 0;
    bool not_mapped = FALSE;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;

    SHR_FUNC_ENTER(unit);

    sal_memset(&updated_fields, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.sbr_tbl->lrd_info.num_fid;
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
        (bcmfp_sbr_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              updated_data,
                              &sbr_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_config_dump(unit,
                                   tbl_id,
                                   stage,
                                   sbr_config));
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_SBR_TEMPLATE;
    idp_info.key = key;
    idp_info.data = updated_data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_UPDATE;
    idp_info.entry_state = state;
    user_data = &idp_info;

    /* Process groups associated to this sbr. */
    sbr_id = sbr_config->sbr_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_group_map_check(unit,
                                   stage_id,
                                   sbr_id,
                                   &not_mapped));
    if (not_mapped == FALSE) {
        cb_func = bcmfp_group_idp_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_group_map_traverse_first(unit,
                                          stage_id,
                                          sbr_id,
                                          cb_func,
                                          user_data));
    }

    /* Process entries associated to this sbr. */
    not_mapped = TRUE;
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_SBR_PER_ENTRY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_entry_map_check(unit,
                                       stage_id,
                                       sbr_id,
                                       &not_mapped));
    }

    if (not_mapped == FALSE) {
        cb_func = bcmfp_entry_idp_sbr_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_entry_map_traverse(unit,
                                          stage_id,
                                          sbr_id,
                                          cb_func,
                                          user_data));
    }
exit:
    bcmfp_sbr_config_free(unit, sbr_config);
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    SHR_FUNC_EXIT();
}

int
bcmfp_sbr_config_delete(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_sbr_config_t *sbr_config = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_sbr_id_t sbr_id = 0;
    bool not_mapped = FALSE;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;

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
        (bcmfp_sbr_config_get(unit,
                              op_arg,
                              tbl_id,
                              stage_id,
                              key,
                              non_default_data,
                              &sbr_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_config_dump(unit,
                                   tbl_id,
                                   stage,
                                   sbr_config));
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_SBR_TEMPLATE;
    idp_info.key = key;
    idp_info.data = non_default_data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_DELETE;
    idp_info.entry_state = state;
    user_data = &idp_info;

    /* Process groups associated to this sbr. */
    sbr_id = sbr_config->sbr_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_sbr_group_map_check(unit,
                                   stage_id,
                                   sbr_id,
                                   &not_mapped));
    if (not_mapped == FALSE) {
        cb_func = bcmfp_group_idp_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_group_map_traverse(unit,
                                          stage_id,
                                          sbr_id,
                                          cb_func,
                                          user_data));
    }

    /* Process entries associated to this sbr. */
    not_mapped = TRUE;
    if (BCMFP_STAGE_FLAGS_TEST(stage,
                    BCMFP_STAGE_SBR_PER_ENTRY)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_entry_map_check(unit,
                                       stage_id,
                                       sbr_id,
                                       &not_mapped));
    }

    if (not_mapped == FALSE) {
        cb_func = bcmfp_entry_idp_sbr_process;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_sbr_entry_map_traverse(unit,
                                          stage_id,
                                          sbr_id,
                                          cb_func,
                                          user_data));
    }
exit:
    bcmfp_sbr_config_free(unit, sbr_config);
    SHR_FUNC_EXIT();
}
