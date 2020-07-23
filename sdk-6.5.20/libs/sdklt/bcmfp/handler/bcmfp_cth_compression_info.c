/*! \file bcmfp_cth_compression_info.c
 *
 * APIs for field compression LT custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate entries in field compression
 * info LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <shr/shr_util.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmfp/bcmfp_compression_info.h>
#include <bcmfp/bcmfp_tbl_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_compression_info_lookup(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    bcmlrd_sid_t sid = 0;
    uint8_t fid_idx = 0;
    uint8_t idx = 0;
    uint32_t data = 0;
    bcmlrd_fid_t fid = 0;
    int array_size = BCMFP_COMPRESSION_INFO_FIDS_MAX_IN_CATEGORY;
    bcmlrd_fid_t fid_array[array_size];
    bcmfp_tbl_compression_info_t *compression_info = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmlrd_map_t const *map = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, BCMFP_STAGE_ID_INGRESS, &stage));

    compression_info = stage->tbls.compression_info_tbl;
    sid = compression_info->sid;
    SHR_IF_ERR_EXIT
        (bcmlrd_map_get(unit, sid, &map));

    /*
     * Fill the incoming key info again in out fields. Without this lookup
     * is failing in IMM. Seems IMM is expecting key field also in out
     * fields.
     */
    out->field[idx]->id = in->field[idx]->id;
    out->field[idx]->data = in->field[idx]->data;
    idx++;

    /* Fill index remap compression fid data. */
    sal_memset(fid_array, 0, sizeof(fid_array));
    fid_array[0] = compression_info->index_remap_0_fid;
    fid_array[1] = compression_info->index_remap_1_fid;
    fid_array[2] = compression_info->index_remap_2_fid;
    fid_array[3] = compression_info->index_remap_3_fid;
    for (fid_idx = 0; fid_idx < array_size; fid_idx++) {
        fid = fid_array[fid_idx];
        if (fid == 0) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (stage->tbls.compression_info_get(unit,
                                              in->field[0]->data,
                                              fid,
                                              &data));
        out->field[idx]->id = fid;
        out->field[idx]->data = data;
        idx++;
    }

    /* Fill match remap compression fid data. */
    sal_memset(fid_array, 0, sizeof(fid_array));
    fid_array[0] = compression_info->match_remap_0_fid;
    fid_array[1] = compression_info->match_remap_1_fid;
    for (fid_idx = 0; fid_idx < array_size; fid_idx++) {
        fid = fid_array[fid_idx];
        if (fid == 0) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (stage->tbls.compression_info_get(unit,
                                              in->field[0]->data,
                                              fid,
                                              &data));
        out->field[idx]->id = fid;
        out->field[idx]->data = data;
        idx++;
    }

    /* Fill range check fid data. */
    sal_memset(fid_array, 0, sizeof(fid_array));
    fid_array[0] = compression_info->range_check_0_fid;
    fid_array[1] = compression_info->range_check_1_fid;
    fid_array[2] = compression_info->range_check_2_fid;
    fid_array[3] = compression_info->range_check_3_fid;
    for (fid_idx = 0; fid_idx < array_size; fid_idx++) {
        fid = fid_array[fid_idx];
        if (fid == 0) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (stage->tbls.compression_info_get(unit,
                                              in->field[0]->data,
                                              fid,
                                              &data));
        out->field[idx]->id = fid;
        out->field[idx]->data = data;
        idx++;
    }

    /* Update the total fields populated so far in out fields. */
    out->count = idx;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_compression_info_insert(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{

    return SHR_E_FAIL;
}

int
bcmfp_compression_info_update(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    return SHR_E_FAIL;
}

int
bcmfp_compression_info_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{

    return SHR_E_FAIL;
}

int
bcmfp_compression_info_validate(int unit,
                            bcmlt_opcode_t opcode,
                            const bcmltd_fields_t *in,
                            const bcmltd_generic_arg_t *arg)
{

    return SHR_E_NONE;
}

int
bcmfp_compression_info_first(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t imm;
    size_t size = 0;
    bcmfp_tbl_compression_info_t *compression_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&imm, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, BCMFP_STAGE_ID_INGRESS, &stage));

    compression_info = stage->tbls.compression_info_tbl;

    /* Fill the first valid key data in local structure(imm) and do lookup. */
    size = sizeof(bcmltd_field_t *);
    BCMFP_ALLOC(imm.field, size, "bcmfpComprssionInfoFirst");
    size = sizeof(bcmltd_field_t);
    imm.field[0] = shr_fmm_alloc();
    imm.field[0]->id = compression_info->key_fid;
    imm.field[0]->data = BCMFP_COMPRESSION_INFO_KEY_ID_FIRST;
    imm.count = BCMFP_COMPRESSION_INFO_KEY_FID_COUNT;

    SHR_IF_ERR_EXIT
        (bcmfp_compression_info_lookup(unit, op_arg, &imm, out, arg));
exit:
    if (imm.field != NULL) {
        if (imm.field[0] != NULL) {
            shr_fmm_free(imm.field[0]);
        }
        SHR_FREE(imm.field);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_compression_info_next(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    bcmltd_fields_t imm;
    size_t size = 0;
    bcmfp_tbl_compression_info_t *compression_info = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&imm, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_compression_info_lookup(unit, op_arg, in, out, arg));

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, BCMFP_STAGE_ID_INGRESS, &stage));

    compression_info = stage->tbls.compression_info_tbl;

    /* Fill the next key data in local structure(imm) and do lookup. */
    size = sizeof(bcmltd_field_t *);
    BCMFP_ALLOC(imm.field, size, "bcmfpComprssionInfoNext");
    size = sizeof(bcmltd_field_t);
    imm.field[0] = shr_fmm_alloc();
    imm.field[0]->id = compression_info->key_fid;
    imm.field[0]->data = (in->field[0]->data + 1);
    imm.count = BCMFP_COMPRESSION_INFO_KEY_FID_COUNT;

    SHR_IF_ERR_EXIT
        (bcmfp_compression_info_lookup(unit, op_arg, &imm, out, arg));
exit:
    if (imm.field != NULL) {
        if (imm.field[0] != NULL) {
            shr_fmm_free(imm.field[0]);
        }
        SHR_FREE(imm.field);
    }
    SHR_FUNC_EXIT();
}
