/*! \file bcmfp_group_idp_cqset.c
 *
 * APIs for processing groups when inter dependency calls
 * invoked from conflict qualifier QSET.
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
#include <bcmlrd/bcmlrd_types.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_tbl_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/bcmfp_cth_conflict_qset.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_group_idp_conflict_qset_insert(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_entry_id_t entry_id,
                           bcmfp_group_id_t group_id,
                           void *user_data)
{
    int rv;
    bcmfp_idp_info_t *idp_info = user_data;
    bcmfp_filter_t *filter = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * If group is not operational means no entries in the
     * group can be inserted.
     */
    group_id = entry_config->group_id;
    rv = bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);
    filter->group_oper_info = opinfo;

    /*
     * Fetch the group operational information.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));
    filter->group_config = group_config;

    /* Insert the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    /* Update the operational state of the entry ID. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
    if (entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_conflict_qset_delete(int unit,
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

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

     /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    SHR_IF_ERR_EXIT
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
        filter->sid_type = BCMFP_SID_TYPE_CONFLICT_QSET;
        /* Create the filter corresponding to entry ID. */
        SHR_IF_ERR_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;
        /* Update the entry state. */
        entry_state =
            BCMFP_ENTRY_CONFLICT_QUALIFIER_SET_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_EXIT
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
bcmfp_group_idp_conflict_qset_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            void *user_data)
{
    bcmimm_entry_event_t event_reason;
    bcmfp_entry_id_traverse_cb cb_func;
    bool not_mapped = FALSE;
    bcmfp_idp_info_t *idp_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    idp_info = user_data;
    event_reason = idp_info->event_reason;


    if (event_reason != BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_idp_common_process(unit,
                                            stage_id,
                                            group_id,
                                            idp_info));
    }

    /* Check entries in the group if present */
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_check(unit,
                                     stage_id,
                                     group_id,
                                     &not_mapped));

    /* Return if no entries are associated to the group */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        cb_func = bcmfp_group_idp_conflict_qset_insert;
    } else if (event_reason == BCMIMM_ENTRY_DELETE) {
        cb_func = bcmfp_group_idp_conflict_qset_delete;
    } else if (event_reason == BCMIMM_ENTRY_UPDATE) {
        cb_func = bcmfp_group_idp_conflict_qset_insert;
    } else {
         SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_traverse(unit,
                                        stage_id,
                                        group_id,
                                        cb_func,
                                        user_data));
exit:
    SHR_FUNC_EXIT();
}
