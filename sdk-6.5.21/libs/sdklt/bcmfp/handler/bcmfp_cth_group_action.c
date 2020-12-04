/*! \file bcmfp_cth_group_action.c
 *
 * APIs for FP group template LTs custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate group config provided
 * using group template LTs.
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
#include <bcmfp/bcmfp_cth_pdd.h>
#include <bcmfp/bcmfp_grp_selcode.h>
#include <bcmfp/bcmfp_cth_filter.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*******************************************************************************
               GROUP TEMPLATE ACTION DISPATCH FUNCTIONS
 */
int
bcmfp_group_template_action_insert(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t tbl_id,
                                   bcmfp_stage_id_t stage_id,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_template_action_delete(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t tbl_id,
                                   bcmfp_stage_id_t stage_id,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}


int
bcmfp_group_template_action_update(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmltd_sid_t tbl_id,
                                   bcmfp_stage_id_t stage_id,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *in_key = NULL;
    bcmltd_fields_t updated_fields;
    const bcmltd_field_t *updated_data = NULL;
    bcmfp_group_config_t *updated_group_config = NULL;
    bcmfp_group_update_state_t status = BCMFP_GROUP_UPDATE_SUCCESS;
    bcmltd_fid_t key_fid;
    bcmltd_fid_t data_fid;
    size_t num_fid = 0;
    bcmltd_sid_t status_tbl_id;
    bcmfp_group_id_t group_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.group_template_action_tbl->lrd_info.num_fid;


    SHR_IF_ERR_EXIT
        (bcmfp_ltd_updated_data_get(unit,
                                    tbl_id,
                                    num_fid,
                                    in_key,
                                    in_data,
                                    &updated_fields));
    updated_data =
        (const bcmltd_field_t *)(updated_fields.field[0]);

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage_id,
                                key,
                                updated_data,
                                &updated_group_config));
    updated_group_config->enable = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_config_update_staging(unit,
                                           op_arg,
                                           tbl_id,
                                           stage_id,
                                           updated_group_config,
                                           &status));

    group_id = updated_group_config->group_id;
    key_fid = stage->tbls.group_template_status_tbl->key_fid;
    status_tbl_id = stage->tbls.group_template_status_tbl->sid;
    num_fid = stage->tbls.group_template_status_tbl->lrd_info.num_fid;
    data_fid = stage->tbls.group_template_status_tbl->status_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_imm_entry_insert(unit,
                                status_tbl_id,
                                &key_fid,
                                &group_id,
                                1),
                                SHR_E_EXISTS);
    /* Update the group update status */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                status_tbl_id,
                                &key_fid,
                                &group_id,
                                1,
                                &data_fid,
                                &status,
                                1));

exit:
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    bcmfp_group_config_free(unit, updated_group_config);
    SHR_FUNC_EXIT();
}

