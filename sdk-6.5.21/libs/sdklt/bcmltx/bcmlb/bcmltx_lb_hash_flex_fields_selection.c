/*! \file bcmltx_lb_hash_flex_fields_selection.c
 *
 * LB_HASH_FLEX_FIELDS_SELECTION Transform Handler
 * This file contains field transform information for
 * LB_HASH_FLEX_FIELDS_SELECTION LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmlb/bcmltx_lb_hash_bin_udf_chunk_mask.h>
#include <bcmltx/bcmlb/bcmltx_lb_hash_bin_udf_chunk_select.h>
#include <bcmltx/bcmlb/bcmltx_lb_hash_bin_flex_field_select.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_LB

/*!
 * \brief lb hash bin flex field selection transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_lb_hash_bin_flex_field_select_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    uint32_t idx = 0;
    uint32_t out_idx = 0;
    uint32_t field_select = 0;
    uint32_t l4_offset = 0;
    uint32_t chunks = 0;
    uint32_t chunk_idx = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "\t bcmltx_lb_hash_bin_flex_field_select_transform\n")));

    if (in->count == 0) {
        SHR_FUNC_EXIT();
    }

    if (arg->rfield == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (arg->value == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }


    for (idx = 0; idx < in->count; idx++) {
        if (in->field[idx]->id == arg->field[0]) { /* Selector */
            field_select = in->field[idx]->data;
        } else if (in->field[idx]->id == arg->field[1]) { /* Udf Chunk */
            chunk_idx = in->field[idx]->data;
        } else if (in->field[idx]->id == arg->field[2]) { /* L4 Offset */
            l4_offset = in->field[idx]->data;
        }
    }

    /* Update 'out' based on selectors */
    out_idx = 0;
    out->field[out_idx]->id = arg->rfield[0];
    out->field[out_idx]->data = field_select;
    out_idx++;

    chunks = arg->value[0];
    switch (field_select) {
        case 0: /* L4_DATA */
            out->field[out_idx]->id = arg->rfield[1];
            out->field[out_idx]->data = l4_offset;
            out_idx++;
            break;

        case 1: /* UDF */
            out->field[out_idx]->id = arg->rfield[2];
            out->field[out_idx]->data = chunks - (chunk_idx % chunks) - 1;
            out_idx++;

            out->field[out_idx]->id = arg->rfield[3];
            out->field[out_idx]->data = chunk_idx / chunks;
            out_idx++;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = out_idx;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief lb hash bin flex field selection reverse transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_lb_hash_bin_flex_field_select_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    uint32_t idx = 0;
    uint32_t out_idx = 0;
    uint32_t field_select = 0;
    uint32_t l4_offset = 0;
    uint32_t chunks = 0;
    uint32_t udf_idx = 0;
    uint32_t chunk_idx = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "\t bcmltx_lb_hash_bin_flex_field_select_rev_transform\n")));

    if (in->count == 0) {
        SHR_FUNC_EXIT();
    }

    if (arg->rfield == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (arg->value == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }


    for (idx = 0; idx < in->count; idx++) {
        if (in->field[idx]->id == arg->field[0]) { /* Selector */
            field_select = in->field[idx]->data;
        } else if (in->field[idx]->id == arg->field[1]) { /* L4 offset */
            l4_offset = in->field[idx]->data;
        } else if (in->field[idx]->id == arg->field[2]) { /* Chunk Idx */
            chunk_idx = in->field[idx]->data;
        } else if (in->field[idx]->id == arg->field[3]) { /* Udf Idx */
            udf_idx = in->field[idx]->data;
        }
    }

    /* Update 'out' based on selectors */
    out_idx = 0;
    out->field[out_idx]->id = arg->rfield[0];
    out->field[out_idx]->data = field_select;
    out_idx++;

    chunks = arg->value[0];
    switch (field_select) {
        case 0: /* L4 Offset */
            out->field[out_idx]->id = arg->rfield[2];
            out->field[out_idx]->data = l4_offset;
            out_idx++;
            break;

        case 1: /* UDF Chunk */
            out->field[out_idx]->id = arg->rfield[1];
            out->field[out_idx]->data = chunks - (chunk_idx + 1);
            out->field[out_idx]->data += (udf_idx * chunks);
            out_idx++;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = out_idx;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LB hash bin udf chunk selection transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_lb_hash_bin_udf_chunk_select_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    uint8_t  udf_id = 0;
    uint8_t  chunk_id = 0;
    uint8_t  num_chunks = 0;
    uint32_t in_idx = 0;
    uint32_t out_idx = 0;
    uint32_t dst_idx = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "\t bcmltx_lb_hash_bin_udf_chunk_select_transform\n")));

    if (in->count == 0) {
        SHR_EXIT();
    }

    if ((arg->rfield == NULL) || (arg->value == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    num_chunks = arg->value[0];

    for (in_idx = 0; in_idx < in->count; in_idx++) {

        if (in->field[in_idx]->id == arg->field[0]) {

            dst_idx = in->field[in_idx]->idx;
            out->field[out_idx]->id = arg->rfield[dst_idx];
            out->field[out_idx]->idx = 0;
            out->field[out_idx]->data = in->field[in_idx]->data;
            out_idx++;

        } else if (in->field[in_idx]->id == arg->field[1]) {

            udf_id = in->field[in_idx]->data / num_chunks;
            chunk_id = in->field[in_idx]->data % num_chunks;

            dst_idx = (in->field[in_idx]->idx * 2) + arg->value[1];

            out->field[out_idx]->id   = arg->rfield[dst_idx];
            out->field[out_idx]->data = num_chunks - (chunk_id + 1);

            out->field[out_idx+1]->id   = arg->rfield[dst_idx+1];
            out->field[out_idx+1]->data = udf_id;
            out_idx += 2;
        }
    }

    out->count = out_idx;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LB flex bin udf chunk selection reverse transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_lb_hash_bin_udf_chunk_select_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    uint8_t  udf_id = 0;
    uint8_t  chunk_id = 0;
    uint8_t  num_chunks = 0;
    uint32_t in_idx = 0;
    uint32_t out_idx = 0;
    uint32_t dst_idx = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "\t bcmltx_lb_hash_bin_udf_chunk_select_rev_transform\n")));

    if (in->count == 0) {
        SHR_EXIT();
    }

    if ((arg->rfield == NULL) || (arg->value == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    num_chunks = arg->value[0];

    for (in_idx = 0; in_idx < arg->value[1]; in_idx++) {
        dst_idx = in_idx;
        out->field[out_idx]->id = arg->rfield[0];
        out->field[out_idx]->idx = dst_idx;
        out->field[out_idx]->data = in->field[in_idx]->data;
        out_idx++;
    }

    for (; in_idx < in->count; in_idx += 2) {
        dst_idx = (in_idx - arg->value[1]) / 2;

        udf_id = in->field[in_idx+1]->data;
        chunk_id = num_chunks - (in->field[in_idx]->data + 1);

        out->field[out_idx]->id = arg->rfield[1];
        out->field[out_idx]->idx = dst_idx;
        out->field[out_idx]->data = (udf_id * num_chunks) + chunk_id;
        out_idx++;
    }

    out->count = out_idx;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LB hash bin udf chunk mask transform.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_lb_hash_bin_udf_chunk_mask_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    uint32_t in_idx = 0;
    uint32_t out_idx = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "\t bcmltx_lb_hash_bin_udf_chunk_mask_transform\n")));

    if (in->count == 0) {
        SHR_EXIT();
    }

    if ((arg->field == NULL) || (arg->rfield == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (in_idx = 0; in_idx < in->count; in_idx++) {
        out->field[out_idx]->id = arg->rfield[in_idx];
        out->field[out_idx]->data = in->field[in_idx]->data;
        out_idx++;
    }

    out->count = out_idx;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LB hash bin udf chunk mask reverse transform.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmltx_lb_hash_bin_udf_chunk_mask_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg)
{
    uint32_t in_idx = 0;
    uint32_t out_idx = 0;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "\t bcmltx_lb_hash_bin_udf_chunk_mask_rev_transform\n")));

    if (in->count == 0) {
        SHR_EXIT();
    }

    if ((arg->field == NULL) || (arg->rfield == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (in_idx = 0; in_idx < in->count; in_idx++) {
        out->field[out_idx]->id = arg->rfield[0];
        out->field[out_idx]->idx = in_idx;
        out->field[out_idx]->data = in->field[in_idx]->data;
        out_idx++;
    }

    out->count = out_idx;

exit:
    SHR_FUNC_EXIT();
}
