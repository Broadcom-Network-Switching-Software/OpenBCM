/*! \file bcmudf_chunk.c
 *
 * UDF Transform Handler
 * This file contains field transform information for UDF
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmudf/bcmltx_udf_chunk.h>
#include <bcmltx/bcmudf/bcmltx_udf_pipe_validate.h>
#include <bcmdrd/bcmdrd_dev.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_UDF

int
bcmltx_udf_chunk_transform(int unit,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_transform_arg_t *arg)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t offset = 0;
    uint32_t hw_idx = 0;
    uint32_t num_hw_field = 0;
    uint32_t hw_field_offset = 0;
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\t transform\n")));

    if ((arg->values != 2) || (arg->value == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"\t transform argument missing\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_hw_field = arg->fields;
    hw_field_offset = arg->value[0] * arg->value[1];

    /* Update with user value */
    for (i = 0; i < in->count; i++) {
        for (j = 0; j < num_hw_field; j++) {
            if (in->field[i]->id == arg->field[j]) {
                offset = j * hw_field_offset;
            }
        }

        hw_idx = in->field[i]->idx + offset;
        out->field[i]->id = arg->rfield[hw_idx];
        out->field[i]->data = in->field[i]->data;
    }

exit:

    SHR_FUNC_EXIT();
}

int
bcmltx_udf_chunk_rev_transform(int unit,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_transform_arg_t *arg)
{
    uint32_t i = 0;
    uint32_t lt_field_idx = 0;
    uint32_t offset = 0;
    uint32_t hw_field_offset = 0;
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\t Reverse transform\n")));

    if ((arg->values != 2) || (arg->value == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"\t transform argument missing\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    hw_field_offset = arg->value[0] * arg->value[1];
    for (i = 0; i < in->count; i++) {
        if (i < hw_field_offset) {
            lt_field_idx = 0;
            offset = 0;
        } else {
            lt_field_idx = 1;
            offset = hw_field_offset;
        }
        out->field[i]->id = arg->rfield[lt_field_idx];
        out->field[i]->idx = i - offset;
        out->field[i]->data = in->field[i]->data;
    }
    out->count = in->count;

 exit:

    SHR_FUNC_EXIT();
}

int
bcmltx_udf_pipe_validate(int unit,
                         bcmlt_opcode_t opcode,
                         const bcmltd_fields_t *in,
                         const bcmltd_field_val_arg_t *arg)
{
    int blktype;
    uint32_t idx = 0;
    uint32_t pipe_map = 0;
    const bcmdrd_chip_info_t *cinfo;

    SHR_FUNC_ENTER(unit);

    cinfo = bcmdrd_dev_chip_info_get(unit);

    for (idx = 0; idx < in->count; idx++) {
        if (in->field[idx]->id == UDFt_PIPEf) {
            blktype = bcmdrd_chip_blktype_get_by_name(cinfo, "ipipe");
            SHR_IF_ERR_EXIT
                (bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &pipe_map));

            if (!(pipe_map & (1 << in->field[idx]->data))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
