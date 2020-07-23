/*! \file bcmfp_cth_src_class.c
 *
 * APIs for LT Source Class Mode entry
 *
 * This file contains function definitions for
 * Source Class Mode entry LT.
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
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_src_class_mode_get(int unit,
                     bcmfp_stage_t *stage,
                     bcmltd_field_t *buffer,
                     uint32_t *src_class_mode)
{
    uint32_t mode_fid = 0;
    uint64_t mode_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(src_class_mode, SHR_E_PARAM);

    if (stage->tbls.src_class_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *src_class_mode = 0;
    mode_fid = stage->tbls.src_class_tbl->mode_fid;
    if (mode_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 mode_fid,
                                 (void *)buffer,
                                 &mode_u64),
                SHR_E_NOT_FOUND);
        *src_class_mode = mode_u64 & 0xF;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_src_class_pipe_id_get(int unit,
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

    if (stage->tbls.src_class_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_mode_get(unit,
                                   stage->stage_id,
                                   &oper_mode));

    if (oper_mode == BCMFP_STAGE_OPER_MODE_GLOBAL) {
        *pipe_id = -1;
        SHR_EXIT();
    }

    pipe_id_fid = stage->tbls.src_class_tbl->pipe_id_fid;
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
bcmfp_src_class_config_get(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           bcmfp_src_class_config_t **scc)
{
    size_t size = 0;
    bcmltd_field_t *buffer = NULL;
    bcmfp_stage_t *stage = NULL;
    int pipe_id = -1;
    bcmfp_src_class_config_t *config = NULL;
    uint32_t mode = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(scc, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.src_class_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    size = sizeof(bcmfp_src_class_config_t);
    BCMFP_ALLOC(config, size, "bcmfpSrcClassConfig");
    *scc = config;

    /* Get the source class pipe id. */
    if (key != NULL) {
        buffer = (bcmltd_field_t *)key;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_src_class_pipe_id_get(unit,
                                         stage,
                                         buffer,
                                         &pipe_id));
    }
    config->pipe_id = pipe_id;

    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get the source class mode. */
    buffer = (bcmltd_field_t *)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_src_class_mode_get(unit,
                                  stage,
                                  buffer,
                                  &mode));
    config->mode = mode;


exit:
    SHR_FUNC_EXIT();

}

int
bcmfp_src_class_insert(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmltd_fields_t *output_fields)
{
    bcmfp_src_class_config_t *scc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_src_class_config_get(unit,
                                    op_arg,
                                    tbl_id,
                                    stage_id,
                                    key,
                                    data,
                                    &scc));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_src_class_mode_set(unit,
                                  op_arg,
                                  stage_id,
                                  tbl_id,
                                  scc->pipe_id,
                                  scc->mode));
exit:
    if (scc != NULL) {
        SHR_FREE(scc);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_src_class_delete(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmltd_fields_t *output_fields)
{
    bcmfp_src_class_config_t *scc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_src_class_config_get(unit,
                                    op_arg,
                                    tbl_id,
                                    stage_id,
                                    key,
                                    data,
                                    &scc));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_src_class_mode_set(unit,
                                  op_arg,
                                  stage_id,
                                  tbl_id,
                                  scc->pipe_id,
                                  scc->mode));
exit:
    if (scc != NULL) {
        SHR_FREE(scc);
    }
    SHR_FUNC_EXIT();
}
