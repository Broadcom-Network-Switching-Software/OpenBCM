/*! \file bcmfp_cth_ctr_entry.c
 *
 * APIs for FP counter entry LT.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate counter config provided
 * using counter entry LT.
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
#include <bcmimm/bcmimm_basic.h>
#include <bcmfp/bcmfp_cth_ctr.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_ctr_entry_id_get(int unit,
                 bcmfp_stage_t *stage,
                 bcmltd_field_t *buffer,
                 bcmfp_ctr_entry_id_t *ctr_entry_id)
{
    uint32_t ctr_entry_id_fid = 0;
    uint64_t ctr_entry_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_entry_id, SHR_E_PARAM);

    if (stage->tbls.ctr_entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ctr_entry_id_fid = stage->tbls.ctr_entry_tbl->key_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             ctr_entry_id_fid,
                             (void *)buffer,
                             &ctr_entry_id_u64),
            SHR_E_NOT_FOUND);

    *ctr_entry_id = ctr_entry_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_ctr_entry_pipe_id_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmltd_field_t *buffer,
                        int *pipe_id)
{
    uint32_t pipe_id_fid = 0;
    uint64_t pipe_id_u64 = 0;
    bcmfp_stage_oper_mode_t oper_mode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_mode_get(unit,
                                   stage->stage_id,
                                   &oper_mode));

    if (oper_mode == BCMFP_STAGE_OPER_MODE_GLOBAL) {
        *pipe_id = -1;
        SHR_EXIT();
    }

    pipe_id_fid = stage->tbls.ctr_entry_tbl->pipe_id_fid;
    if (pipe_id_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 pipe_id_fid,
                                 (void *)buffer,
                                 &pipe_id_u64),
                SHR_E_NOT_FOUND);
        *pipe_id = pipe_id_u64 & 0xF;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_ctr_entry_pool_id_get(int unit,
                 bcmfp_stage_t *stage,
                 bcmltd_field_t *buffer,
                 uint8_t *ctr_entry_pool_id)
{
    uint32_t ctr_entry_pool_id_fid = 0;
    uint64_t ctr_entry_pool_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_entry_pool_id, SHR_E_PARAM);

    if (stage->tbls.ctr_entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ctr_entry_pool_id_fid = stage->tbls.ctr_entry_tbl->pool_id_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             ctr_entry_pool_id_fid,
                             (void *)buffer,
                             &ctr_entry_pool_id_u64),
            SHR_E_NOT_FOUND);

    *ctr_entry_pool_id = ctr_entry_pool_id_u64 & 0xFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_ctr_entry_color_get(int unit,
                 bcmfp_stage_t *stage,
                 bcmltd_field_t *buffer,
                 bcmfp_ctr_color_t *ctr_entry_color)
{
    uint32_t ctr_entry_color_fid = 0;
    uint64_t ctr_entry_color_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_entry_color, SHR_E_PARAM);

    if (stage->tbls.ctr_entry_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    ctr_entry_color_fid = stage->tbls.ctr_entry_tbl->color_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             ctr_entry_color_fid,
                             (void *)buffer,
                             &ctr_entry_color_u64),
            SHR_E_NOT_FOUND);

    *ctr_entry_color = ctr_entry_color_u64 & 0xFF;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_entry_config_get(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmfp_ctr_entry_config_t **config)
{
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *buffer = NULL;
    uint32_t ctr_entry_id = 0;
    uint8_t ctr_entry_pool_id = 0;
    int ctr_entry_pipe_id = -1;
    bcmfp_ctr_entry_config_t *ctr_entry_config = NULL;
    bcmfp_ctr_color_t ctr_entry_color = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.ctr_entry_tbl == NULL ||
        stage->tbls.ctr_entry_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(ctr_entry_config,
                sizeof(bcmfp_ctr_entry_config_t),
                "bcmfpCounterEntryConfig");
    *config = ctr_entry_config;

    /* Get the counter entry ID */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_entry_id_get(unit, stage, buffer, &ctr_entry_id));
    ctr_entry_config->ctr_entry_id = ctr_entry_id;

    /*
     * if data is NULL no need to extract the
     * remaining config.
     */
    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get the counter entry pool ID */
    buffer = (bcmltd_field_t *)data;
    if (stage->tbls.ctr_entry_tbl->pool_id_fid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ctr_entry_pool_id_get(unit,
                                         stage,
                                         buffer,
                                         &ctr_entry_pool_id));
        ctr_entry_config->pool_id = ctr_entry_pool_id;
    }

    /* Get the counter entry pipe ID */
    buffer = (bcmltd_field_t *)data;
    if (stage->tbls.ctr_entry_tbl->pipe_id_fid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ctr_entry_pipe_id_get(unit,
                                         stage,
                                         buffer,
                                         &ctr_entry_pipe_id));
        ctr_entry_config->pipe_id = ctr_entry_pipe_id;
    }

    /* Get the counter entry color */
    buffer = (bcmltd_field_t *)data;
    if (stage->tbls.ctr_entry_tbl->color_fid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ctr_entry_color_get(unit,
                                       stage,
                                       buffer,
                                       &ctr_entry_color));
        ctr_entry_config->color = ctr_entry_color;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_entry_remote_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmfp_stage_id_t stage_id,
                       bcmfp_ctr_entry_id_t ctr_entry_id,
                       bcmfp_ctr_entry_config_t **config)
{
    int rv;
    bcmltd_fid_t key_fid;
    bcmltd_sid_t tbl_id;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmfp_ctr_entry_config_t *ctr_entry_config = NULL;
    size_t num_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    *config = NULL;


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_fid = stage->tbls.ctr_entry_tbl->key_fid;
    tbl_id = stage->tbls.ctr_entry_tbl->sid;
    num_fid = stage->tbls.ctr_entry_tbl->lrd_info.num_fid;

    /* Get the FP counter entry saved in IMM. */
    rv = bcmfp_imm_entry_lookup(unit,
                                tbl_id,
                                &key_fid,
                                &ctr_entry_id,
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

    /* Get the FP counter entry LT entry config */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_entry_config_get(unit,
                                   op_arg,
                                   tbl_id,
                                   stage->stage_id,
                                   key,
                                   data,
                                   &ctr_entry_config));
    *config = ctr_entry_config;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_entry_config_free(int unit,
                bcmfp_ctr_entry_config_t *config)
{
    SHR_FUNC_ENTER(unit);

    if (config != NULL) {
        SHR_FREE(config);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_ctr_entry_config_dump(int unit,
                bcmltd_sid_t tbl_id,
                bcmfp_stage_t *stage,
                bcmfp_ctr_entry_config_t *ctr_entry_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_entry_config, SHR_E_PARAM);

    if (stage->tbls.ctr_entry_tbl->key_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Ctr Entry ID = %d\n"),
            ctr_entry_config->ctr_entry_id));
    }

    /* Dump the counter entry pool ID */
    if (stage->tbls.ctr_entry_tbl->pool_id_fid) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Ctr pool ID = %d\n"),
            ctr_entry_config->pool_id));
    }

    /* Dump the counter entry pipe ID */
    if (stage->tbls.ctr_entry_tbl->pipe_id_fid) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Ctr pipe ID = %d\n"),
            ctr_entry_config->pipe_id));
    }

    /* Dump the counter entry color */
    if (stage->tbls.ctr_entry_tbl->color_fid) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Ctr color = %d\n"),
            ctr_entry_config->color));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_ctr_entry_config_process(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t tbl_id,
                               bcmfp_stage_id_t stage_id,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               bcmltd_fields_t *output_fields,
                               bcmimm_entry_event_t imm_event)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_ctr_entry_config_t *ctr_entry_config = NULL;
    bcmfp_ctr_entry_id_t ctr_entry_id = 0;
    int pipe_id = -1;
    uint8_t pool_id = 0;
    bcmfp_ctr_color_t color = CTR_COLOR_COUNT;
    bool not_mapped = FALSE;
    bcmfp_idp_info_t idp_info;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /* Get the counter entry configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_entry_config_get(unit,
                                    op_arg,
                                    tbl_id,
                                    stage_id,
                                    key,
                                    data,
                                    &ctr_entry_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ctr_entry_config_dump(unit,
                                         tbl_id,
                                         stage,
                                         ctr_entry_config));
    }

    /* Caliculate the oounter HW index and save it in HA. */
    ctr_entry_id = ctr_entry_config->ctr_entry_id;
    pool_id = ctr_entry_config->pool_id;
    pipe_id = ctr_entry_config->pipe_id;
    color = ctr_entry_config->color;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_counter_hw_idx_set(unit,
                                  stage,
                                  ctr_entry_id,
                                  pipe_id,
                                  pool_id,
                                  color,
                                  imm_event));

    /* Process entries associated to this counter entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_entry_map_check(unit,
                                   stage_id,
                                   ctr_entry_id,
                                   &not_mapped));

    /*
     * Return if no entries are associated to the counter
     * entry.
     */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_CTR_ENTRY;
    idp_info.key = key;
    idp_info.data = data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = imm_event;

    user_data = (void *)(&idp_info);
    cb_func = bcmfp_entry_idp_ctr_entry_process;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_entry_map_traverse(unit,
                                      stage_id,
                                      ctr_entry_id,
                                      cb_func,
                                      user_data));
exit:
    bcmfp_ctr_entry_config_free(unit, ctr_entry_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_entry_config_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmltd_fields_t *output_fields)
{
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bcmimm_entry_event_t imm_event = BCMIMM_ENTRY_INSERT;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

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
        (bcmfp_ctr_entry_config_process(unit,
                                        op_arg,
                                        tbl_id,
                                        stage_id,
                                        key,
                                        non_default_data,
                                        output_fields,
                                        imm_event));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_entry_config_update(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmltd_fields_t *output_fields)
{
    uint16_t num_fid = 0;
    bcmimm_entry_event_t imm_event = BCMIMM_ENTRY_UPDATE;
    bcmfp_stage_t *stage = NULL;
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
    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.ctr_entry_tbl->lrd_info.num_fid;
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
        (bcmfp_ctr_entry_config_process(unit,
                                        op_arg,
                                        tbl_id,
                                        stage_id,
                                        in_key,
                                        updated_data,
                                        output_fields,
                                        imm_event));
exit:
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    SHR_FUNC_EXIT();
}

int
bcmfp_ctr_entry_config_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    bcmimm_entry_event_t imm_event = BCMIMM_ENTRY_DELETE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ctr_entry_config_process(unit,
                                        op_arg,
                                        tbl_id,
                                        stage_id,
                                        key,
                                        data,
                                        output_fields,
                                        imm_event));
exit:
    SHR_FUNC_EXIT();
}
