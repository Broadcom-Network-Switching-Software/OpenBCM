/*! \file bcmfp_cth_range_check.c
 *
 * APIs for LT Range Check Group entry
 *
 * This file contains function definitions for
 * Range Check Group entry LT.
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
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

STATIC int
bcmfp_range_check_group_id_get(int unit,
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

    if (stage->tbls.range_check_group_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    group_id_fid = stage->tbls.range_check_group_tbl->key_fid;

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

STATIC int
bcmfp_range_check_group_pipe_id_get(int unit,
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

    if (stage->tbls.range_check_group_tbl == NULL) {
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

    pipe_id_fid =
        stage->tbls.range_check_group_tbl->pipe_id_fid;
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

STATIC int
bcmfp_range_check_id_bitmap_get(int unit,
                           bcmfp_stage_t *stage,
                           bcmltd_field_t *buffer,
                           bcmfp_range_check_id_bmp_t *bitmap)
{
    uint32_t range_check_id_fid = 0;
    uint64_t range_check_id_u64 = 0;
    bcmfp_tbl_range_check_group_t *rcg_tbl = NULL;
    uint16_t index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(bitmap, SHR_E_PARAM);

    if (stage->tbls.range_check_group_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    rcg_tbl = stage->tbls.range_check_group_tbl;
    range_check_id_fid = rcg_tbl->rcid_fid;

    for (index = 0; index < BCMFP_MAX_RANGE_CHECK_ID; index++) {
        range_check_id_u64 = 0;
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_array_value_get(unit,
                                       range_check_id_fid,
                                       index,
                                       (void *)buffer,
                                       &range_check_id_u64),
             SHR_E_NOT_FOUND);
        if (range_check_id_u64) {
            SHR_BITSET(bitmap->w, index);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_range_check_group_config_get(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        bcmfp_range_check_group_config_t **scc)
{
    size_t size = 0;
    bcmfp_range_check_group_config_t *config = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *buffer = NULL;
    uint32_t rcg_id = 0;
    int pipe_id = -1;
    bcmfp_range_check_id_bmp_t rcid_bitmap;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.range_check_group_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    size = sizeof(bcmfp_range_check_group_config_t);
    BCMFP_ALLOC(config, size, "bcmfpRangeCheckGroupConfig");
    *scc = config;

    /* Get the range check group ID */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_range_check_group_id_get(unit,
                                        stage,
                                        buffer,
                                        &rcg_id));
    config->group_id = rcg_id;

    /* Get the source class pipe id. */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_range_check_group_pipe_id_get(unit,
                                             stage,
                                             buffer,
                                             &pipe_id));
    config->pipe_id = pipe_id;
    /*
     * if data is NULL no need to extract the
     * remaining config.
     */
    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get the range ID bitmap */
    buffer = (bcmltd_field_t *)data;
    size = sizeof(bcmfp_range_check_id_bmp_t);
    sal_memset(&rcid_bitmap, 0, size);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_range_check_id_bitmap_get(unit,
                                         stage,
                                         buffer,
                                         &rcid_bitmap));
    sal_memcpy(&(config->rcid_bitmap), &rcid_bitmap, size);

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_range_check_group_insert(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t tbl_id,
                               bcmfp_stage_id_t stage_id,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               bcmltd_fields_t *output_fields)
{
    bcmfp_range_check_group_config_t *rcgc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_range_check_group_config_get(unit,
                                            op_arg,
                                            tbl_id,
                                            stage_id,
                                            key,
                                            data,
                                            &rcgc));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_range_check_grp_set(unit,
                                   op_arg,
                                   stage_id,
                                   tbl_id,
                                   rcgc->pipe_id,
                                   rcgc->group_id,
                                   rcgc->rcid_bitmap));

exit:
    if (rcgc != NULL) {
        SHR_FREE(rcgc);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_range_check_group_delete(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t tbl_id,
                               bcmfp_stage_id_t stage_id,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               bcmltd_fields_t *output_fields)
{
    bcmfp_range_check_group_config_t *rcgc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_range_check_group_config_get(unit,
                                            op_arg,
                                            tbl_id,
                                            stage_id,
                                            key,
                                            data,
                                            &rcgc));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_range_check_grp_set(unit,
                                   op_arg,
                                   stage_id,
                                   tbl_id,
                                   rcgc->pipe_id,
                                   rcgc->group_id,
                                   rcgc->rcid_bitmap));

exit:
    if (rcgc != NULL) {
        SHR_FREE(rcgc);
    }
    SHR_FUNC_EXIT();
}
