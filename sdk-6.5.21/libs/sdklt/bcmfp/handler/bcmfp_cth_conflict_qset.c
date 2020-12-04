/*! \file bcmfp_cth_conflict_qset.c
 *
 * APIs for FP conflict qset LTs custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate conflict qset config
 * provided using conflict QSET LTs.
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
#include <bcmfp/bcmfp_cth_conflict_qset.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*******************************************************************************
               CONFLICT QSET CONFIG GET FUNCTIONS
 */

int
bcmfp_conflict_qset_config_get(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t tbl_id,
                               bcmfp_stage_id_t stage_id,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               bcmfp_conflict_qset_config_t **config)
{
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    bcmfp_conflict_qset_config_t *cqset_config = NULL;
    bcmlrd_fid_t cqset_id_fid = 0;
    uint64_t cqset_id_u64 = 0;
    bcmltd_field_t *buffer = NULL;
    bcmfp_stage_t *stage = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.conflict_qset_tbl == NULL ||
        stage->tbls.conflict_qset_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(cqset_config,
                sizeof(bcmfp_conflict_qset_config_t),
                "bcmfpConflictQsetConfig");
    *config = cqset_config;

    cqset_id_fid = stage->tbls.conflict_qset_tbl->key_fid;

    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             cqset_id_fid,
                             (void *)buffer,
                             &cqset_id_u64),
                             SHR_E_NOT_FOUND);

    cqset_config->conflict_qset_id = cqset_id_u64 & 0xFFFFFFFF;

    /*
     * if data is NULL no need to extract the
     * remaining config.
     */
    if (data == NULL) {
        SHR_EXIT();
    }

    qualifiers_fid_info =
        stage->tbls.conflict_qset_tbl->qualifiers_fid_info;
    if (qualifiers_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    size = (sizeof(uint32_t) *
            stage->tbls.conflict_qset_tbl->lrd_info.num_fid);
    BCMFP_ALLOC(cqset_config->qual_id,
                size,
                "bcmfpConflictQsetQualIdArray");
    BCMFP_ALLOC(cqset_config->qual_number,
                size,
                "bcmfpConflictQsetQualNumberArray");

    buffer = (bcmltd_field_t *)data;
    while (buffer != NULL) {
        if (qualifiers_fid_info[buffer->id].qualifier == 0) {
            buffer = buffer->next;
            continue;
        }
        cqset_config->qual_id[cqset_config->num_quals] =
                      qualifiers_fid_info[buffer->id].qualifier;
        cqset_config->qual_number[cqset_config->num_quals] =
                      buffer->data & 0xFF;
        cqset_config->num_quals++;
        buffer = buffer->next;
    }
exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_conflict_qset_config_free(unit, cqset_config);
        *config = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_remote_config_get(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_conflict_qset_id_t cqset_id,
                                  bcmfp_conflict_qset_config_t **config)
{
    shr_error_t rv = SHR_E_NONE;
    bcmltd_fid_t key_fid;
    bcmltd_sid_t tbl_id;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmfp_conflict_qset_config_t *cqset_config = NULL;
    size_t num_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    *config = NULL;

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_fid = stage->tbls.conflict_qset_tbl->key_fid;
    tbl_id = stage->tbls.conflict_qset_tbl->sid;
    num_fid = stage->tbls.conflict_qset_tbl->lrd_info.num_fid;

    /* Get the FP conflict QSET LT entry saved in IMM. */
    rv = bcmfp_imm_entry_lookup(unit,
                                tbl_id,
                                &key_fid,
                                &cqset_id,
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

    /* Get the FP conflict QSET LT entry config of ACL. */
    SHR_IF_ERR_EXIT
        (bcmfp_conflict_qset_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage->stage_id,
                                key,
                                data,
                                &cqset_config));
    *config = cqset_config;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_conflict_qset_config_dump(int unit,
                                bcmltd_sid_t tbl_id,
                                bcmfp_stage_t *stage,
                                bcmfp_conflict_qset_config_t *config)
{
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Conflict QSET ID = %d\n"),
        config->conflict_qset_id));

    for (idx = 0; idx < config->num_quals; idx++) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "Qualifier ID = %d Number = %d\n"),
                    config->qual_id[idx],
                    config->qual_number[idx]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_config_insert(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_conflict_qset_config_t *cqset_config = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_conflict_qset_id_t cqset_id = 0;
    bool not_mapped = FALSE;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
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
        (bcmfp_conflict_qset_config_get(unit,
                                        op_arg,
                                        tbl_id,
                                        stage_id,
                                        key,
                                        non_default_data,
                                        &cqset_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_conflict_qset_config_dump(unit,
                                             tbl_id,
                                             stage,
                                             cqset_config));
    }

    /* Process group associated to this conflict qset. */
    cqset_id = cqset_config->conflict_qset_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_conflict_qset_group_map_check(unit,
                                             stage_id,
                                             cqset_id,
                                             &not_mapped));

    /* Return if no groups are associated to the conflict qset */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_CONFLICT_QSET;
    idp_info.key = key;
    idp_info.data = data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_INSERT;
    idp_info.entry_state = state;
    cb_func = bcmfp_group_idp_process;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_conflict_qset_group_map_traverse(unit,
                                                stage_id,
                                                cqset_id,
                                                cb_func,
                                                (void *)(&idp_info)));
exit:
    bcmfp_conflict_qset_config_free(unit, cqset_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_config_update(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_conflict_qset_config_t *cqset_config = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_conflict_qset_id_t cqset_id = 0;
    bool not_mapped = FALSE;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
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
        (bcmfp_conflict_qset_config_get(unit,
                                        op_arg,
                                        tbl_id,
                                        stage_id,
                                        key,
                                        non_default_data,
                                        &cqset_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_conflict_qset_config_dump(unit,
                                             tbl_id,
                                             stage,
                                             cqset_config));
    }

    /* Process group associated to this conflict qset. */
    cqset_id = cqset_config->conflict_qset_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_conflict_qset_group_map_check(unit,
                                             stage_id,
                                             cqset_id,
                                             &not_mapped));

    /* Return if no groups are associated to the conflict qset */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_CONFLICT_QSET;
    idp_info.key = key;
    idp_info.data = data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_UPDATE;
    idp_info.entry_state = state;
    cb_func = bcmfp_group_idp_process;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_conflict_qset_group_map_traverse(unit,
                                                stage_id,
                                                cqset_id,
                                                cb_func,
                                                (void *)(&idp_info)));
exit:
    bcmfp_conflict_qset_config_free(unit, cqset_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_config_delete(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_conflict_qset_config_t *cqset_config = NULL;
    bcmfp_idp_info_t idp_info;
    bcmfp_entry_state_t state = BCMFP_ENTRY_SUCCESS;
    bcmfp_conflict_qset_id_t cqset_id = 0;
    bool not_mapped = FALSE;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
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
        (bcmfp_conflict_qset_config_get(unit,
                                        op_arg,
                                        tbl_id,
                                        stage_id,
                                        key,
                                        non_default_data,
                                        &cqset_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_conflict_qset_config_dump(unit,
                                             tbl_id,
                                             stage,
                                             cqset_config));
    }

    /* Process group associated to this conflict qset. */
    cqset_id = cqset_config->conflict_qset_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_conflict_qset_group_map_check(unit,
                                             stage_id,
                                             cqset_id,
                                             &not_mapped));

    /* Return if no groups are associated to the conflict qset */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_CONFLICT_QSET;
    idp_info.key = key;
    idp_info.data = data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_DELETE;
    idp_info.entry_state = state;
    cb_func = bcmfp_group_idp_process;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_conflict_qset_group_map_traverse(unit,
                                                stage_id,
                                                cqset_id,
                                                cb_func,
                                                (void *)(&idp_info)));
exit:
    bcmfp_conflict_qset_config_free(unit, cqset_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_config_free(int unit,
                           bcmfp_conflict_qset_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(config, SHR_E_NONE);

    SHR_FREE(config->qual_id);
    SHR_FREE(config->qual_number);
    SHR_FREE(config);

exit:
    SHR_FUNC_EXIT();
}

