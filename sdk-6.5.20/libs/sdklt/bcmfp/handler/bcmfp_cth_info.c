/*! \file bcmfp_cth_group_info.c
 *
 * APIs for FP group info LTs custom handler.
 *
 * This file contains internal functions to
 * lookup group related internal information for
 * group info LTs.
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
#include <bcmltd/chip/bcmltd_fp_constants.h>
#include <bcmfp/bcmfp_cth_info.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmfp/generated/bcmfp_ha.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*******************************************************************************
               INTERNAL FUNCTIONS
 */

static int
bcmfp_cth_out_fields_set(int unit,
                         uint32_t *out_idx,
                         bcmltd_fields_t *out,
                         uint32_t fid,
                         uint32_t data,
                         uint32_t arr_index)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    BCMFP_CTH_OUT_COUNT_CHECK(*out_idx, out->count);
    out->field[*out_idx]->id = fid;
    out->field[*out_idx]->data = data;
    out->field[*out_idx]->idx = arr_index;
    (*out_idx)++;

exit:
    SHR_FUNC_EXIT();

}
static int
bcmfp_cth_out_fields_init(int unit,
                         uint32_t fid,
                         uint16_t start_index,
                         uint16_t end_index,
                         uint32_t *out_idx,
                         bcmltd_fields_t *out)
{
    uint32_t offset_idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out, SHR_E_PARAM);

    for (offset_idx = start_index; offset_idx < end_index;
         offset_idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_cth_out_fields_set(unit, out_idx, out, fid, 0, offset_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
               GROUP INFO FUNCTIONS
 */

int
bcmfp_psg_info_update(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t tbl_id,
                      bcmfp_stage_t *stage,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out)
{
    int rv;
    uint32_t idx = 0, part_idx = 0;
    bcmfp_qual_offset_info_t *q_offset = NULL;
    bcmfp_qualifier_t qualifier;
    bcmfp_tbl_offset_info_t *qfi;
    uint8_t offset_idx = 0;
    uint32_t psg_id = 0;
    uint32_t out_idx = 0;
    bcmfp_group_oper_info_t *presel_group_oper_info = NULL;
    uint32_t fid = 0;
    uint32_t max_group_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    BCMFP_ALLOC(q_offset,
                sizeof(bcmfp_qual_offset_info_t),
                "bcmfpPsgInfoQualOffset");

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id
              ==  stage->tbls.psg_info_tbl->psg_key_fid) {
          SHR_IF_ERR_VERBOSE_EXIT(bcmfp_fid_value_u32_get(unit,
                                               in->field[idx],
                                               stage->tbls.psg_info_tbl->psg_key_fid,
                                               &psg_id));
          }
    }

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id
              == stage->tbls.psg_info_tbl->part_key_fid) {
          SHR_IF_ERR_VERBOSE_EXIT(bcmfp_fid_value_u32_get(unit,
                                               in->field[idx],
                                               stage->tbls.psg_info_tbl->part_key_fid,
                                               &part_idx));
          }
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_max_psg_id_get(unit,
                                                       stage->stage_id,
                                                       &max_group_id));
    if (psg_id > max_group_id) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_oper_info_get(unit,
                                          stage->stage_id,
                                          psg_id,
                                          &presel_group_oper_info));

    if (!(presel_group_oper_info->valid)) {
          SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Copy key to out fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.psg_info_tbl->psg_key_fid,
                            psg_id, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.psg_info_tbl->part_key_fid,
                            part_idx, 0));

    for (idx = 0; idx < stage->tbls.psg_tbl->fid_count; idx++) {
         qualifier = stage->tbls.psg_tbl->qualifiers_fid_info[idx].qualifier;

         /* Non qualifier FIDs */
         if (qualifier == 0) {
             continue;
         }
         qfi = &stage->tbls.psg_info_tbl->qual_fid_info[qualifier];
         sal_memset(q_offset, 0, sizeof(bcmfp_qual_offset_info_t));

         rv = bcmfp_group_qual_offset_info_get(unit,
                                               stage->stage_id,
                                               psg_id,
                                               presel_group_oper_info,
                                               TRUE,
                                               (part_idx - 1),
                                               idx,
                                               0,
                                               q_offset);

         if (SHR_SUCCESS(rv)) {

             /* Check if qualifier LT field are present */
             if (qfi->count_fid == 0 && qfi->offset_fid == 0 &&
                 qfi->width_fid == 0 && qfi->order_fid == 0) {
                 continue;
             }

             if (q_offset->num_offsets > FP_INFO_MAX) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
             }

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_set(unit, &out_idx, out, qfi->count_fid,
                                           q_offset->num_offsets, 0));

             for (offset_idx = 0; offset_idx < q_offset->num_offsets;
                  offset_idx++) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                               qfi->offset_fid,
                                               q_offset->offset[offset_idx],
                                               offset_idx));
             }

             /* Fill empty info */
             fid = qfi->offset_fid;
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_init(unit,
                                           fid,
                                           q_offset->num_offsets,
                                           FP_INFO_MAX,
                                           &out_idx,
                                           out));

             for (offset_idx = 0; offset_idx < q_offset->num_offsets;
                  offset_idx++) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                               qfi->width_fid,
                                               q_offset->width[offset_idx],
                                               offset_idx));
             }

             /* Fill empty info */
             fid = qfi->width_fid;
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_init(unit,
                                           fid,
                                           q_offset->num_offsets,
                                           FP_INFO_MAX,
                                           &out_idx,
                                           out));

         } else {
             /* Check if qualifier LT field are present */
             if (qfi->count_fid == 0 && qfi->offset_fid == 0 &&
                 qfi->width_fid == 0 && qfi->order_fid == 0) {
                 continue;
             }

             /* Fill empty info */
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                           qfi->count_fid, 0, 0));


             fid = qfi->offset_fid;
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_init(
                     unit,
                     fid,
                     0,
                     FP_INFO_MAX,
                     &out_idx,
                     out));

             fid = qfi->width_fid;
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_init(
                     unit,
                     fid,
                     0,
                     FP_INFO_MAX,
                     &out_idx,
                     out));

         }

         /* Fill empty info */
         fid = qfi->order_fid;
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_cth_out_fields_init(unit,
                                       fid,
                                       0,
                                       FP_INFO_MAX,
                                       &out_idx,
                                       out));
    }

    out->count = out_idx;

exit:
    SHR_FREE(q_offset);
    SHR_FUNC_EXIT();
}

int
bcmfp_group_info_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_t *stage,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out)
{
    uint8_t parts_count = 0;
    uint32_t group_flags = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    uint32_t group_id = 0;
    bcmfp_buffers_t *req_buffers = NULL;
    bcmfp_buffers_t *rsp_buffers = NULL;
    uint32_t bflags = 0;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    size_t size = 0;
    bcmptm_rm_tcam_group_hw_info_t *hw_info = NULL;
    uint32_t out_idx = 0;
    uint32_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    BCMFP_ALLOC(attrs,
                sizeof(bcmptm_rm_tcam_entry_attrs_t),
                "bcmfpGroupInfoEntryAttrs");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_value_u32_get(unit,
                                 in->field[0],
                                 stage->tbls.group_info_tbl->key_fid,
                                 &group_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_check_get(unit,
                                   stage->stage_id,
                                   group_id,
                                   &opinfo));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_mode_to_flags_get(unit,
                                       stage,
                                       opinfo->group_mode,
                                       &group_flags));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit, FALSE, group_flags, &parts_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.group_info_tbl->hw_ltid,
                            opinfo->action_res_id, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.group_info_tbl->num_partition_id,
                            parts_count, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.group_info_tbl->num_entries_created,
                            opinfo->ref_count, 0));

    if (!(BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH))) {
        if (opinfo->ref_count != 0) {

            size = sizeof(bcmfp_buffers_t);
            BCMFP_ALLOC(req_buffers, size, "bcmfpGroupInfoReqBuffers");
            BCMFP_ALLOC(rsp_buffers, size, "bcmfpGroupInfoRspBuffers");
            bflags = BCMFP_BUFFERS_CREATE_EKW;
            bflags |= BCMFP_BUFFERS_CREATE_EDW;

            SHR_IF_ERR_VERBOSE_EXIT
               (bcmfp_filter_buffers_allocate(unit,
                                              bflags,
                                              stage->tbls.entry_tbl->sid,
                                              stage,
                                              rsp_buffers));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_filter_buffers_allocate(unit,
                                               bflags,
                                               stage->tbls.entry_tbl->sid,
                                               stage,
                                               req_buffers));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_group_lookup(unit,
                                        req_flags,
                                        op_arg,
                                        stage->stage_id,
                                        stage->tbls.entry_tbl->sid,
                                        group_id,
                                        opinfo,
                                        req_buffers,
                                        rsp_buffers,
                                        attrs));
            hw_info = attrs->grp_hw_info;
        }
        if (hw_info != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                    stage->tbls.group_info_tbl->num_entries_tentative,
                    hw_info->num_entries_tentative, 0));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                    stage->tbls.group_info_tbl->num_entries_tentative,
                    0, 0));
        }
    }

    /* Copy key to out fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.group_info_tbl->key_fid,
                            group_id, 0));
    out->count = out_idx;

exit:
    bcmfp_filter_buffers_free(unit, req_buffers);
    SHR_FREE(req_buffers);
    bcmfp_filter_buffers_free(unit, rsp_buffers);
    SHR_FREE(rsp_buffers);
    SHR_FREE(attrs);
    SHR_FREE(hw_info);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_info_qual_array_update (int unit,
                        uint32_t trans_id,
                        bcmfp_stage_t *stage,
                        uint32_t group_id,
                        uint32_t part_id,
                        uint32_t qualifier,
                        uint32_t array_depth,
                        uint32_t *out_idx,
                        bcmltd_fields_t *out)
{
    int rv;
    uint16_t elem_id = 0;
    bcmfp_qual_offset_info_t *q_offset = NULL;
    bcmfp_tbl_offset_info_t *qfi;
    uint8_t offset_idx = 0;
    uint8_t num_valid_offsets = 0;
    uint8_t actual_idx = 0;
    uint32_t fid = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(out_idx, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    BCMFP_ALLOC(q_offset,
                (FP_UDF_CONTAINER_MAX * sizeof(bcmfp_qual_offset_info_t)),
                "bcmfpGroupInfoQualArrayOffsetInfo");

    /* Check if qualifier LT field are present */
    qfi = &stage->tbls.group_part_tbl->qual_fid_info[qualifier];
    if (qfi->count_fid == 0 && qfi->offset_fid == 0 &&
        qfi->width_fid == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_check_get(unit, stage->stage_id, group_id, &opinfo));

    for (elem_id = 0; elem_id < array_depth; elem_id++) {

         sal_memset((q_offset + elem_id), 0, sizeof(bcmfp_qual_offset_info_t));

         rv = bcmfp_group_qual_offset_info_get(unit,
                                               stage->stage_id,
                                               group_id,
                                               opinfo,
                                               FALSE,
                                               part_id,
                                               qualifier,
                                               elem_id,
                                               (q_offset + elem_id));

         if (SHR_SUCCESS(rv)) {

             num_valid_offsets = 0;
             for (offset_idx = 0; offset_idx < (q_offset + elem_id)->num_offsets;
                  offset_idx++) {
                  if ((q_offset + elem_id)->width[offset_idx] != 0) {
                      num_valid_offsets++;
                  }
             }

             if (num_valid_offsets > FP_INFO_MAX) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
             }

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_set(unit, out_idx, out,
                                           qfi->count_fid,
                                           num_valid_offsets, elem_id));

         } else {

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_set(unit, out_idx, out,
                                           qfi->count_fid,
                                           0, elem_id));
        }
    }

    actual_idx = 0;

    for (elem_id = 0; elem_id < array_depth; elem_id++) {
         if ((q_offset + elem_id) != NULL) {
             for (offset_idx = 0; offset_idx < (q_offset + elem_id)->num_offsets;
                  offset_idx++) {
                 if ((q_offset + elem_id)->width[offset_idx] != 0) {
                     SHR_IF_ERR_VERBOSE_EXIT
                         (bcmfp_cth_out_fields_set(unit, out_idx, out,
                                                   qfi->offset_fid,
                                                   (q_offset + elem_id)->offset[offset_idx],
                                                   actual_idx));
                     actual_idx++;
                 }
             }
         }
    }

    /* Fill empty info */
    fid = qfi->offset_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_init(unit,
                                  fid,
                                  actual_idx,
                                  FP_INFO_MAX,
                                  out_idx,
                                  out));

    actual_idx = 0;

    for (elem_id = 0; elem_id < array_depth; elem_id++) {
        if ((q_offset + elem_id) != NULL) {
            for (offset_idx = 0; offset_idx < (q_offset + elem_id)->num_offsets;
                 offset_idx++) {
                if ((q_offset + elem_id)->width[offset_idx] != 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmfp_cth_out_fields_set(unit, out_idx, out,
                                                  qfi->width_fid,
                                                  (q_offset + elem_id)->width[offset_idx],
                                                  actual_idx));
                    actual_idx++;
                }
            }
        }
    }

    /* Fill empty info */
    fid = qfi->width_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_init(unit,
                                  fid,
                                  actual_idx,
                                  FP_INFO_MAX,
                                  out_idx,
                                  out));

    actual_idx = 0;

    for (elem_id = 0; elem_id < array_depth; elem_id++) {
        if ((q_offset + elem_id) != NULL) {
            for (offset_idx = 0; offset_idx < (q_offset + elem_id)->num_offsets;
                 offset_idx++) {
                if ((q_offset + elem_id)->width[offset_idx] != 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmfp_cth_out_fields_set(unit, out_idx, out,
                                                  qfi->order_fid,
                                                  (offset_idx + 1),
                                                  actual_idx));
                    actual_idx++;
                }
            }
        }
    }

    /* Fill empty info */
    fid = qfi->order_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_init(unit,
                                   fid,
                                   actual_idx,
                                   FP_INFO_MAX,
                                   out_idx,
                                   out));

exit:
    SHR_FREE(q_offset);
    SHR_FUNC_EXIT();
}

int
bcmfp_action_cont_info_first_search(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmfp_stage_id_t stage_id,
                                    bcmltd_fields_t *out,
                                    const bcmltd_generic_arg_t *arg)
{
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    size_t size = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, sizeof(bcmltd_field_t *),
                "bcmfpGroupInfoFirstKeyField");
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* first valid action_id is 1 for these LTs */
    
    size = sizeof(bcmltd_field_t);
    insert.field[0] = shr_fmm_alloc();
    if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
    } else {
        insert.count += 1;
        sal_memset(insert.field[0], 0, size);
        insert.field[0]->id = 0;
        insert.field[0]->data = 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_action_cont_info_lookup(unit,
                                       op_arg,
                                       stage,
                                       &insert,
                                       out));

exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_action_cont_info_next_search(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmfp_stage_id_t stage_id,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg)
{
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    size_t action_id_count = 0;
    size_t size = 0;
    uint32_t curr_action_id = 0;
    uint32_t next_action_id = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, sizeof(bcmltd_field_t *),
                "bcmfpGroupInfoNextKeyField");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id , &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_symbols_count_get(unit,
                                        arg->sid,
                                        0,
                                        &action_id_count));

    /* obtain the action_id from the input data - in*/
    if (in->field[idx]->id == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_value_u32_get(unit,
                                     in->field[idx],
                                     0,
                                     &curr_action_id));
    }

    if (curr_action_id < (action_id_count-1)) {
        next_action_id = curr_action_id + 1;
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();

        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = 0;
            insert.field[0]->data = next_action_id;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_action_cont_info_lookup(unit,
                                           op_arg,
                                           stage,
                                           &insert,
                                           out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_action_cont_info_lookup(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_stage_t *stage,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out)
{
    uint32_t out_idx = 0;
    uint8_t num_containers = 0;
    uint32_t idx = 0;
    uint32_t action_id = 0;
    uint16_t cont_id = 0;
    uint16_t cont_size = 0;
    bcmfp_action_cfg_t *action_cfg = NULL;
    bcmfp_action_offset_cfg_t offset_cfg;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    /* Obtain the action_id from the input data - in */
    if (in->field[idx]->id == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_value_u32_get(unit,
                                     in->field[idx],
                                     0,
                                     &action_id));
    }

    /* Copy key to out fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit,
                                  &out_idx,
                                  out,
                                  0,
                                  action_id,
                                  0));

    /* Loop through action_cfg_db and extract container info */
    action_cfg = stage->action_cfg_db->action_cfg[action_id];

    /* Populate container ids */
    if (action_cfg != NULL) {
        offset_cfg = action_cfg->offset_cfg;
        for (idx = 0; idx < BCMFP_ACTION_PARAM_SZ; idx++) {
            if (offset_cfg.width[idx] == 0) {
                break;
            }
            num_containers++;
            if (num_containers > BCMFP_MAX_CONTS_PER_ACTION) {
                num_containers--;
                break;
            }
            cont_id = offset_cfg.phy_cont_id[idx];
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_set(unit,
                                           &out_idx,
                                           out,
                                           2,
                                           cont_id,
                                           idx));
        }
    }

    /* Populate the container sizes */
    num_containers = 0;

    action_cfg = stage->action_cfg_db->action_cfg[action_id];

    if (action_cfg != NULL) {
        offset_cfg = action_cfg->offset_cfg;
        for (idx = 0; idx < BCMFP_ACTION_PARAM_SZ; idx++) {
            if (offset_cfg.width[idx] == 0) {
                break;
            }
            num_containers++;
            if (num_containers > BCMFP_MAX_CONTS_PER_ACTION) {
                num_containers--;
                break;
            }
            cont_id = offset_cfg.phy_cont_id[idx];
            cont_size = stage->action_cfg_db->container_size[cont_id];

            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_set(unit,
                                           &out_idx,
                                           out,
                                           3,
                                           cont_size,
                                           idx));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit,
                                  &out_idx, out,
                                  1,
                                  num_containers,
                                  0));
    out->count = out_idx;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_partition_info_update(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t tbl_id,
                      bcmfp_stage_t *stage,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out)
{
    int rv;
    bcmfp_group_oper_info_t *opinfo = NULL;
    uint32_t idx = 0;
    bcmfp_qual_offset_info_t *q_offset = NULL;
    bcmfp_qualifier_t qualifier;
    bcmfp_tbl_offset_info_t *qfi;
    uint32_t part_idx = 0;
    uint8_t offset_idx = 0;
    bcmfp_buffers_t *req_buffers = NULL;
    bcmfp_buffers_t *rsp_buffers = NULL;
    uint32_t bflags = 0;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    size_t size = 0;
    bcmptm_rm_tcam_group_hw_info_t *hw_info = NULL;
    uint8_t num_valid_offsets = 0;
    uint8_t actual_idx = 0;
    uint32_t group_id = 0;
    uint32_t out_idx = 0;
    uint32_t group_flags = 0;
    uint8_t parts_count = 0;
    bcmlrd_field_def_t field_def;
    uint32_t fid = 0;
    uint32_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    BCMFP_ALLOC(q_offset,
                sizeof(bcmfp_qual_offset_info_t),
                "bcmfpGroupPartInfoQualOffsetInfo");
    BCMFP_ALLOC(attrs, sizeof(bcmptm_rm_tcam_entry_attrs_t),
                "bcmfpGroupPartInfoEntryAttrs");

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id ==
              stage->tbls.group_part_tbl->group_key_fid) {
              SHR_IF_ERR_VERBOSE_EXIT
                  (bcmfp_fid_value_u32_get(unit,
                                   in->field[idx],
                                   stage->tbls.group_part_tbl->group_key_fid,
                                   &group_id));
          }
    }

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id ==
              stage->tbls.group_part_tbl->part_key_fid) {
              SHR_IF_ERR_VERBOSE_EXIT
                  (bcmfp_fid_value_u32_get(unit,
                                   in->field[idx],
                                   stage->tbls.group_part_tbl->part_key_fid,
                                   &part_idx));
          }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_check_get(unit,
                                         stage->stage_id,
                                         group_id,
                                         &opinfo));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_mode_to_flags_get(unit,
                                       stage,
                                       opinfo->group_mode,
                                       &group_flags));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit, FALSE, group_flags, &parts_count));

    if ((part_idx == 0) || (part_idx > parts_count)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Copy key to out fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.group_part_tbl->group_key_fid,
                            group_id, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.group_part_tbl->part_key_fid,
                            part_idx, 0));

    if ((BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH))) {

        /* EM key attributes index info */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                stage->tbls.group_part_tbl->em_key_attrib_sid,
                                opinfo->ext_codes[part_idx - 1].em_key_attrib_index,
                                0));
    }

    /* Slice Info */
    if (!(BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH))) {

        if (opinfo->ref_count != 0) {
            size = sizeof(bcmfp_buffers_t);
            BCMFP_ALLOC(req_buffers, size, "bcmfpGroupPartInfoReqBuffers");
            BCMFP_ALLOC(rsp_buffers, size, "bcmfpGroupPartInfoRspBuffers");
            bflags = BCMFP_BUFFERS_CREATE_EKW;
            bflags |= BCMFP_BUFFERS_CREATE_EDW;

            SHR_IF_ERR_VERBOSE_EXIT
               (bcmfp_filter_buffers_allocate(unit,
                                              bflags,
                                              stage->tbls.entry_tbl->sid,
                                              stage,
                                              rsp_buffers));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_filter_buffers_allocate(unit,
                                               bflags,
                                               stage->tbls.entry_tbl->sid,
                                               stage,
                                               req_buffers));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_group_lookup(unit,
                                        req_flags,
                                        op_arg,
                                        stage->stage_id,
                                        stage->tbls.entry_tbl->sid,
                                        group_id,
                                        opinfo,
                                        req_buffers,
                                        rsp_buffers,
                                        attrs));
            hw_info = attrs->grp_hw_info;
        }

        if (hw_info != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                    stage->tbls.group_part_tbl->num_slice_fid,
                                    hw_info->num_slices_depth_wise, 0));
            for (idx = 0; idx < hw_info->num_slices_depth_wise; idx++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                        stage->tbls.group_part_tbl->slice_fid,
                                        hw_info->slice[part_idx - 1][idx],
                                        idx));
            }

            /* Fill empty info */
            fid = stage->tbls.group_part_tbl->slice_fid;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_init(unit,
                                          fid,
                                          hw_info->num_slices_depth_wise,
                                          FP_INFO_MAX,
                                          &out_idx,
                                          out));

        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                    stage->tbls.group_part_tbl->num_slice_fid,
                                    0, 0));

            /* Fill empty info */
            fid = stage->tbls.group_part_tbl->slice_fid;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_init(unit,
                                          fid,
                                          0,
                                          FP_INFO_MAX,
                                          &out_idx,
                                          out));
        }

        if ((hw_info != NULL) && (stage->tbls.group_part_tbl->virtual_slice_fid != 0)) {
            for (idx = 0; idx < hw_info->num_slices_depth_wise; idx++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                        stage->tbls.group_part_tbl->virtual_slice_fid,
                                        hw_info->virtual_slice[part_idx - 1][idx],
                                        idx));
            }

            /* Fill empty info */
            fid = stage->tbls.group_part_tbl->virtual_slice_fid;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_init(unit,
                                          fid,
                                          hw_info->num_slices_depth_wise,
                                          FP_INFO_MAX,
                                          &out_idx,
                                          out));

        } else if (stage->tbls.group_part_tbl->virtual_slice_fid != 0) {

            /* Fill empty info */
            fid = stage->tbls.group_part_tbl->virtual_slice_fid;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_init(unit,
                                          fid,
                                          0,
                                          FP_INFO_MAX,
                                          &out_idx,
                                          out));
        }
    }

    /* Qualifier Info */
    
    if (stage->tbls.group_part_tbl->qual_fid_info) {
        for (idx = 0; idx < stage->tbls.group_tbl->fid_count; idx++) {
            qualifier = stage->tbls.group_tbl->qualifiers_fid_info[idx].qualifier;

            /* Non qualifier FIDs */
            if (qualifier == 0) {
                continue;
            }
            qfi = &stage->tbls.group_part_tbl->qual_fid_info[qualifier];
            sal_memset(q_offset, 0, sizeof(bcmfp_qual_offset_info_t));

            rv = bcmlrd_table_field_def_get(unit,
                                            stage->tbls.group_tbl->sid,
                                            idx,
                                            &field_def);

            /* No support for this qualifier in this device, then skip */
            if (rv == SHR_E_UNAVAIL) {
                continue;
            }

            /* Process array FIDs */
            if (field_def.depth != 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_group_info_qual_array_update(unit,
                                                        op_arg->trans_id,
                                                        stage,
                                                        group_id,
                                                        (part_idx - 1),
                                                        idx,
                                                        field_def.depth,
                                                        &out_idx,
                                                        out));

                continue;
            }

            rv = bcmfp_group_qual_offset_info_get(unit,
                                                  stage->stage_id,
                                                  group_id,
                                                  opinfo,
                                                  FALSE,
                                                  (part_idx - 1),
                                                  idx,
                                                  0,
                                                  q_offset);
            if (SHR_SUCCESS(rv)) {

                /* Check if qualifier LT field are present */
                if (qfi->count_fid == 0 && qfi->offset_fid == 0 &&
                    qfi->width_fid == 0 && qfi->order_fid == 0) {
                    continue;
                }

                num_valid_offsets = 0;
                for (offset_idx = 0; offset_idx < q_offset->num_offsets;
                        offset_idx++) {
                    if (q_offset->width[offset_idx] != 0) {
                        num_valid_offsets++;
                    }
                }

                if (num_valid_offsets > FP_INFO_MAX) {
                   SHR_ERR_EXIT(SHR_E_INTERNAL);
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                              qfi->count_fid,
                                              num_valid_offsets, 0));

                actual_idx = 0;
                for (offset_idx = 0; offset_idx < q_offset->num_offsets;
                        offset_idx++) {
                    if (q_offset->width[offset_idx] != 0) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                                      qfi->offset_fid,
                                                      q_offset->offset[offset_idx],
                                                      actual_idx));
                        actual_idx++;
                    }

                }

                /* Fill empty info */
                fid = qfi->offset_fid;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_cth_out_fields_init(unit,
                                               fid,
                                               actual_idx,
                                               FP_INFO_MAX,
                                               &out_idx,
                                               out));

                actual_idx = 0;
                for (offset_idx = 0; offset_idx < q_offset->num_offsets;
                        offset_idx++) {
                    if (q_offset->width[offset_idx] != 0) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                                      qfi->width_fid,
                                                      q_offset->width[offset_idx],
                                                      actual_idx));
                        actual_idx++;
                    }
                }

                /* Fill empty info */
                fid = qfi->width_fid;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_cth_out_fields_init(unit,
                                               fid,
                                               actual_idx,
                                               FP_INFO_MAX,
                                               &out_idx,
                                               out));

                actual_idx = 0;
                for (offset_idx = 0; offset_idx < q_offset->num_offsets;
                        offset_idx++) {
                    if (q_offset->width[offset_idx] != 0) {
                        SHR_IF_ERR_VERBOSE_EXIT
                        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                                  qfi->order_fid,
                                                  (offset_idx + 1),
                                                  actual_idx));
                        actual_idx++;
                    }
                }

                /* Fill empty info */
                fid = qfi->order_fid;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_cth_out_fields_init(unit,
                                               fid,
                                               actual_idx,
                                               FP_INFO_MAX,
                                               &out_idx,
                                               out));

            } else {

                /* Check if qualifier LT field are present */
                if (qfi->count_fid == 0 && qfi->offset_fid == 0 &&
                    qfi->width_fid == 0 && qfi->order_fid == 0) {
                    continue;
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                              qfi->count_fid, 0, 0));

                /* Fill empty info */
                fid = qfi->offset_fid;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_cth_out_fields_init(unit,
                                               fid,
                                               0,
                                               FP_INFO_MAX,
                                               &out_idx,
                                               out));

                fid = qfi->width_fid;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_cth_out_fields_init(unit,
                                               fid,
                                               0,
                                               FP_INFO_MAX,
                                               &out_idx,
                                               out));

                fid = qfi->order_fid;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_cth_out_fields_init(unit,
                                               fid,
                                               0,
                                               FP_INFO_MAX,
                                               &out_idx,
                                               out));

            }
        }
    }

    out->count = out_idx;

exit:
    SHR_FREE(q_offset);
    SHR_FREE(attrs);
    bcmfp_filter_buffers_free(unit, req_buffers);
    SHR_FREE(req_buffers);
    bcmfp_filter_buffers_free(unit, rsp_buffers);
    SHR_FREE(rsp_buffers);
    SHR_FREE(hw_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_info_update(int unit,
                      uint32_t trans_id,
                      bcmltd_sid_t tbl_id,
                      bcmfp_stage_t *stage,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out)
{
    int rv;
    uint32_t idx = 0;
    bcmfp_action_oper_info_t *a_offset = NULL;
    bcmfp_action_t action;
    bcmfp_tbl_offset_info_t *qfi;
    uint32_t part_idx = 0;
    uint8_t offset_idx = 0;
    uint32_t pdd_id = 0;
    uint32_t out_idx = 0;
    uint32_t fid = 0;
    uint32_t max_pdd_id = 0;
    uint8_t pdd_num_parts = 0;
    bcmfp_pdd_oper_type_t oper_type = BCMFP_PDD_OPER_TYPE_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    BCMFP_ALLOC(a_offset,
                sizeof(bcmfp_action_oper_info_t),
                "bcmfpPddInfoActionOffsetInfo");

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id
              ==  stage->tbls.pdd_info_tbl->pdd_key_fid) {
          SHR_IF_ERR_VERBOSE_EXIT(bcmfp_fid_value_u32_get(unit,
                                               in->field[idx],
                                                stage->tbls.pdd_info_tbl->pdd_key_fid,
                                               &pdd_id));
          }
    }

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id
              == stage->tbls.pdd_info_tbl->part_key_fid) {
          SHR_IF_ERR_VERBOSE_EXIT(bcmfp_fid_value_u32_get(unit,
                                               in->field[idx],
                                               stage->tbls.pdd_info_tbl->part_key_fid,
                                               &part_idx));
          }
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_max_pdd_id_get(unit,
                                                       stage->stage_id,
                                                       &max_pdd_id));
    if (pdd_id > max_pdd_id) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if ((BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ACTION_RESOLUTION_SBR))) {
        oper_type = BCMFP_PDD_OPER_TYPE_PDD_SBR_GROUP;
    } else if ((BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_POLICY_TYPE_PDD))) {
        oper_type = BCMFP_PDD_OPER_TYPE_PDD_GROUP;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_num_parts_get(unit,
                                 stage->stage_id,
                                 pdd_id,
                                 oper_type,
                                 &pdd_num_parts));

    if (part_idx > pdd_num_parts) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Copy key to out fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.pdd_info_tbl->pdd_key_fid,
                            pdd_id, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.pdd_info_tbl->part_key_fid,
                            part_idx, 0));

    for (idx = 0; idx < stage->tbls.pdd_tbl->fid_count; idx++) {
         action = stage->tbls.pdd_tbl->actions_fid_info[idx].action;

         /* Non action FIDs */
         if (action == 0) {
             continue;
         }
         qfi = &stage->tbls.pdd_info_tbl->action_fid_info[action];
         sal_memset(a_offset, 0, sizeof(bcmfp_action_oper_info_t));

         rv = bcmfp_pdd_action_offset_info_get(unit,
                                               stage->stage_id,
                                               pdd_id,
                                               (part_idx - 1),
                                               action,
                                               oper_type,
                                               a_offset);
         if (SHR_SUCCESS(rv)) {

             /* Check if action LT field are present */
             if (qfi->count_fid == 0 && qfi->offset_fid == 0 &&
                 qfi->width_fid == 0 && qfi->order_fid == 0) {
                 continue;
             }

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                           qfi->count_fid,
                                           a_offset->num_offsets, 0));

             for (offset_idx = 0; offset_idx < a_offset->num_offsets;
                  offset_idx++) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                               qfi->offset_fid,
                                               a_offset->offset[offset_idx],
                                               offset_idx));
             }

             /* Fill empty info */
             fid = qfi->offset_fid;
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_init(unit,
                                           fid,
                                           a_offset->num_offsets,
                                           FP_INFO_MAX,
                                           &out_idx,
                                           out));

             for (offset_idx = 0; offset_idx < a_offset->num_offsets;
                  offset_idx++) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                               qfi->width_fid,
                                               a_offset->width[offset_idx],
                                               offset_idx));
             }

             /* Fill empty info */
             fid = qfi->width_fid;
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_init(unit,
                                           fid,
                                           a_offset->num_offsets,
                                           FP_INFO_MAX,
                                           &out_idx,
                                           out));
         } else {

             /* Check if action LT field are present */
             if (qfi->count_fid == 0 && qfi->offset_fid == 0 &&
                 qfi->width_fid == 0 && qfi->order_fid == 0) {
                 continue;
             }

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                           qfi->count_fid, 0, 0));

             /* Fill empty info */
             fid = qfi->offset_fid;
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_init(unit,
                                           fid,
                                           0,
                                           FP_INFO_MAX,
                                           &out_idx,
                                           out));

             fid = qfi->width_fid;
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_cth_out_fields_init(unit,
                                           fid,
                                           0,
                                           FP_INFO_MAX,
                                           &out_idx,
                                           out));
         }

         fid = qfi->order_fid;
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_cth_out_fields_init(unit,
                                       fid,
                                       0,
                                       FP_INFO_MAX,
                                       &out_idx,
                                       out));

    }

    out->count = out_idx;

exit:
    SHR_FREE(a_offset);
    SHR_FUNC_EXIT();
}

int
bcmfp_grp_info_first_search(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    uint32_t max_group_id = 0;
    uint32_t idx = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    shr_error_t rv = SHR_E_NONE;
    size_t size = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, sizeof(bcmltd_field_t *),
                "bcmfpGroupInfoFirstKeyField");
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_group_id_get(unit,
                                      stage_id,
                                      &max_group_id));

    /* Find first valid group */
    for (idx = 1; idx <= max_group_id; idx++) {
         rv = (bcmfp_group_oper_info_check_get(unit,
                                               stage_id,
                                               idx,
                                               &opinfo));
         if (rv == SHR_E_NONE) {
             found = TRUE;
             break;
         }
    }

    if (found) {

        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = stage->tbls.group_info_tbl->key_fid;
            insert.field[0]->data = idx;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_info_update(unit,
                                     op_arg,
                                     arg->sid,
                                     stage,
                                     &insert,
                                     out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_grp_info_next_search(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{


    uint32_t group_id = 0;
    uint32_t idx = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    shr_error_t rv = SHR_E_NONE;
    size_t size = 0;
    uint32_t max_group_id = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);
    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, sizeof(bcmltd_field_t *),
                "bcmfpGroupInfoNextKeyField");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id , &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_group_id_get(unit,
                                      stage_id,
                                      &max_group_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_value_u32_get(unit,
                                 in->field[0],
                                 stage->tbls.group_info_tbl->key_fid,
                                 &group_id));
    /* Find next valid group */
    for (idx = (group_id+1); idx <= max_group_id; idx++) {
         rv = (bcmfp_group_oper_info_check_get(unit,
                                         stage_id,
                                         idx,
                                         &opinfo));
         if (rv == SHR_E_NONE) {
             found = TRUE;
             break;
         }
    }

    if (found) {
        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = stage->tbls.group_info_tbl->key_fid;
            insert.field[0]->data = idx;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_info_update(unit,
                                     op_arg,
                                     arg->sid,
                                     stage,
                                     &insert,
                                     out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }


exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_grp_part_info_first_search(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmfp_stage_id_t stage_id,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    uint32_t max_group_id = 0;
    uint32_t idx = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    shr_error_t rv = SHR_E_NONE;
    size_t size = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);
    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, 2 * sizeof(bcmltd_field_t *),
                "bcmfpGroupPartInfoFirstKeyField");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_max_group_id_get(unit,
                                      stage_id,
                                      &max_group_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id , &stage));

    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    /* Find first valid group */
    for (idx = 1; idx <= max_group_id; idx++) {
         rv = (bcmfp_group_oper_info_check_get(unit,
                                         stage_id,
                                         idx,
                                         &opinfo));
         if (rv == SHR_E_NONE) {
             found = TRUE;
             break;
         }
    }

    if (found) {

        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = stage->tbls.group_part_tbl->group_key_fid;
            insert.field[0]->data = idx;
        }

        insert.field[1] = shr_fmm_alloc();
        if (insert.field[1] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[1], 0, size);
            insert.field[1]->id = stage->tbls.group_part_tbl->part_key_fid;
            insert.field[1]->data = 1;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_partition_info_update(unit,
                                               op_arg,
                                               arg->sid,
                                               stage,
                                               &insert,
                                               out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_grp_part_info_next_search(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmfp_stage_id_t stage_id,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    uint32_t max_group_id = 0;
    uint32_t idx = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    shr_error_t rv = SHR_E_NONE;
    size_t size = 0;
    uint32_t final_grp = 0, final_part = 0;
    uint32_t group_id = 0, part_idx = 0;
    uint32_t group_flags = 0;
    uint8_t parts_count = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field,  2 * sizeof(bcmltd_field_t *),
                "bcmfpPartGroupInfoNextKeyField");

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_max_group_id_get(unit,
                                 stage_id,
                                 &max_group_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id , &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id
              == stage->tbls.group_part_tbl->group_key_fid) {
          SHR_IF_ERR_VERBOSE_EXIT(bcmfp_fid_value_u32_get(unit,
                                               in->field[idx],
                                               stage->tbls.group_part_tbl->group_key_fid,
                                               &group_id));
          }
    }

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id
              == stage->tbls.group_part_tbl->part_key_fid) {
          SHR_IF_ERR_VERBOSE_EXIT(bcmfp_fid_value_u32_get(unit,
                                               in->field[idx],
                                               stage->tbls.group_part_tbl->part_key_fid,
                                               &part_idx));
          }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_check_get(unit,
                                   stage->stage_id,
                                   group_id,
                                   &opinfo));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_mode_to_flags_get(unit,
                                       stage,
                                       opinfo->group_mode,
                                       &group_flags));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit, FALSE, group_flags, &parts_count));

    if (part_idx < parts_count) {

        final_part = part_idx + 1;
        final_grp  = group_id;
        found = TRUE;

    } else {

        /* Find next valid group */
        for (idx = (group_id + 1); idx <= max_group_id; idx++) {
             rv = (bcmfp_group_oper_info_check_get(unit,
                                             stage_id,
                                             idx,
                                             &opinfo));
             if (rv == SHR_E_NONE) {
                 found = TRUE;
                 break;
             }
        }

        final_part = 1;
        final_grp = idx;
    }


    if (found) {
        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = stage->tbls.group_part_tbl->group_key_fid;
            insert.field[0]->data = final_grp;
        }

        insert.field[1] = shr_fmm_alloc();
        if (insert.field[1] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[1], 0, size);
            insert.field[1]->id = stage->tbls.group_part_tbl->part_key_fid;
            insert.field[1]->data = final_part;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_partition_info_update(unit,
                                               op_arg,
                                               arg->sid,
                                               stage,
                                               &insert,
                                               out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();

}

int
bcmfp_psg_info_first_search(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_id_t stage_id,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    uint32_t max_group_id = 0;
    uint32_t idx = 0;
    bcmfp_group_oper_info_t *presel_group_oper_info = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    size_t size = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);
    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, 2 * sizeof(bcmltd_field_t *),
                "bcmfpPsgInfoFirstKeyField");

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_max_psg_id_get(unit,
                                 stage_id,
                                 &max_group_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id , &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Find first valid group */
    for (idx = 1; idx <= max_group_id; idx++) {
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_presel_group_oper_info_get(unit,
                                               stage->stage_id,
                                               idx,
                                               &presel_group_oper_info));
         if (presel_group_oper_info->valid) {
             found = TRUE;
             break;
         }
    }

    if (found) {

        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = stage->tbls.psg_info_tbl->psg_key_fid;
            insert.field[0]->data = idx;
        }

        insert.field[1] = shr_fmm_alloc();
        if (insert.field[1] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[1], 0, size);
            insert.field[1]->id = stage->tbls.psg_info_tbl->part_key_fid;
            insert.field[1]->data = 1;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_psg_info_update(unit,
                                     trans_id,
                                     arg->sid,
                                     stage,
                                     &insert,
                                     out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_info_next_search(int unit,
                        uint32_t trans_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    uint32_t max_group_id = 0;
    uint32_t idx = 0;
    bcmfp_group_oper_info_t *presel_group_oper_info = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    size_t size = 0;
    uint32_t psg_id = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);
    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, 2 * sizeof(bcmltd_field_t *),
                "bcmfpPsgInfoNextKeyField");

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_max_psg_id_get(unit,
                                 stage_id,
                                 &max_group_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id , &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_fid_value_u32_get(unit, in->field[0],
                                         stage->tbls.psg_info_tbl->psg_key_fid,
                                         &psg_id));
    /* Find first valid group */
    for (idx = (psg_id + 1); idx <= max_group_id; idx++) {
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_presel_group_oper_info_get(unit,
                                               stage->stage_id,
                                               idx,
                                               &presel_group_oper_info));
         if (presel_group_oper_info->valid) {
             found = TRUE;
             break;
         }
    }

    if (found) {
        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = stage->tbls.psg_info_tbl->psg_key_fid;
            insert.field[0]->data = idx;
        }

        insert.field[1] = shr_fmm_alloc();
        if (insert.field[1] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[1], 0, size);
            insert.field[1]->id = stage->tbls.psg_info_tbl->part_key_fid;
            insert.field[1]->data = 1;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_psg_info_update(unit,
                                     trans_id,
                                     arg->sid,
                                     stage,
                                     &insert,
                                     out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_info_first_search(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_id_t stage_id,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    uint32_t max_pdd_id = 0;
    uint32_t idx = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    size_t size = 0;
    bool found = FALSE;
    uint8_t pdd_num_parts = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, 2 * sizeof(bcmltd_field_t *),
                "bcmfpPddInfoFirstKeyField");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id , &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_max_pdd_id_get(unit,
                                 stage_id,
                                 &max_pdd_id));
    /* Find next valid group */
    for (idx = 1; idx <= max_pdd_id; idx++) {
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmfp_pdd_num_parts_get(unit,
                                      stage->stage_id,
                                      idx,
                                      BCMFP_PDD_OPER_TYPE_PDD_SBR_GROUP,
                                      &pdd_num_parts));
         if (pdd_num_parts != 0) {
             found = TRUE;
             break;
         }
    }

    if (found) {
        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = stage->tbls.pdd_info_tbl->pdd_key_fid;
            insert.field[0]->data = idx;
        }

        insert.field[1] = shr_fmm_alloc();
        if (insert.field[1] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[1], 0, size);
            insert.field[1]->id = stage->tbls.pdd_info_tbl->part_key_fid;
            insert.field[1]->data = 1;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_info_update(unit,
                                     trans_id,
                                     arg->sid,
                                     stage,
                                     &insert,
                                     out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();

}

int
bcmfp_pdd_info_next_search(int unit,
                        uint32_t trans_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    uint32_t max_pdd_id = 0;
    uint32_t idx = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    size_t size = 0;
    uint32_t final_pdd = 0, final_part = 0;
    uint32_t pdd_id = 0, part_idx = 0;
    bool found = FALSE;
    uint8_t pdd_num_parts = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, 2 * sizeof(bcmltd_field_t *),
                "bcmfpPddInfoNextKeyField");

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stage_max_pdd_id_get(unit,
                                 stage_id,
                                 &max_pdd_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id , &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id
              ==  stage->tbls.pdd_info_tbl->pdd_key_fid) {
          SHR_IF_ERR_VERBOSE_EXIT(bcmfp_fid_value_u32_get(unit,
                                               in->field[idx],
                                               stage->tbls.pdd_info_tbl->pdd_key_fid,
                                               &pdd_id));
          }
    }

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id
              == stage->tbls.pdd_info_tbl->part_key_fid) {
          SHR_IF_ERR_VERBOSE_EXIT(bcmfp_fid_value_u32_get(unit,
                                               in->field[idx],
                                               stage->tbls.pdd_info_tbl->part_key_fid,
                                               &part_idx));
          }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_num_parts_get(unit,
                                 stage->stage_id,
                                 pdd_id,
                                 BCMFP_PDD_OPER_TYPE_PDD_SBR_GROUP,
                                 &pdd_num_parts));

    if (part_idx < pdd_num_parts) {
        final_part = part_idx + 1;
        final_pdd  = pdd_id;
        found = TRUE;
    } else {
        /* Find next valid group */
        for (idx = (pdd_id + 1); idx <= max_pdd_id; idx++) {
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_pdd_num_parts_get(unit,
                                          stage->stage_id,
                                          idx,
                                          BCMFP_PDD_OPER_TYPE_PDD_SBR_GROUP,
                                          &pdd_num_parts));
             if (pdd_num_parts != 0) {
                 found = TRUE;
                 break;
             }
        }
        final_part = 1;
        final_pdd = idx;
    }

    if (found) {
        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = stage->tbls.pdd_info_tbl->pdd_key_fid;
            insert.field[0]->data = final_pdd;
        }

        insert.field[1] = shr_fmm_alloc();
        if (insert.field[1] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[1], 0, size);
            insert.field[1]->id = stage->tbls.pdd_info_tbl->part_key_fid;
            insert.field[1]->data = final_part;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_info_update(unit,
                                     trans_id,
                                     arg->sid,
                                     stage,
                                     &insert,
                                     out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();

}

int
bcmfp_stage_info_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_t *stage,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out)
{

    bcmfp_buffers_t *req_buffers = NULL;
    bcmfp_buffers_t *rsp_buffers = NULL;
    uint32_t bflags = 0;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    size_t size = 0;
    bcmptm_rm_tcam_stage_hw_info_t *hw_info = NULL;
    uint32_t out_idx = 0;
    uint32_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    BCMFP_ALLOC(attrs,
                sizeof(bcmptm_rm_tcam_entry_attrs_t),
                "bcmfpStageInfoEntryAttrs");


    if (!(BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH))) {
        size = sizeof(bcmfp_buffers_t);
        BCMFP_ALLOC(req_buffers, size, "bcmfpGroupInfoReqBuffers");
        BCMFP_ALLOC(rsp_buffers, size, "bcmfpGroupInfoRspBuffers");
        bflags = BCMFP_BUFFERS_CREATE_EKW;
        bflags |= BCMFP_BUFFERS_CREATE_EDW;

        SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_filter_buffers_allocate(unit,
                                          bflags,
                                          stage->tbls.entry_tbl->sid,
                                          stage,
                                          rsp_buffers));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_buffers_allocate(unit,
                                           bflags,
                                           stage->tbls.entry_tbl->sid,
                                           stage,
                                           req_buffers));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_stage_lookup(unit,
                                    req_flags,
                                    op_arg,
                                    stage->stage_id,
                                    stage->tbls.entry_tbl->sid,
                                    req_buffers,
                                    rsp_buffers,
                                    attrs));
        hw_info = attrs->stage_hw_info;
        if (hw_info != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                    stage->tbls.stage_info_tbl->num_slices,
                    hw_info->num_slices, 0));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                    stage->tbls.stage_info_tbl->num_slices,
                    0, 0));
        }
    }

    out->count = out_idx;

exit:
    bcmfp_filter_buffers_free(unit, req_buffers);
    SHR_FREE(req_buffers);
    bcmfp_filter_buffers_free(unit, rsp_buffers);
    SHR_FREE(rsp_buffers);
    SHR_FREE(attrs);
    SHR_FREE(hw_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_slice_info_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_t *stage,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out)
{

    uint32_t slice_id = 0;
    bcmfp_buffers_t *req_buffers = NULL;
    bcmfp_buffers_t *rsp_buffers = NULL;
    uint32_t bflags = 0;
    bcmptm_rm_tcam_entry_attrs_t *attrs = NULL;
    size_t size = 0;
    bcmptm_rm_tcam_slice_hw_info_t *hw_info = NULL;
    uint32_t out_idx = 0;
    uint32_t req_flags = 0;
    int pipe_id = -1;
    uint32_t pipe = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    BCMFP_ALLOC(attrs,
                sizeof(bcmptm_rm_tcam_entry_attrs_t),
                "bcmfpStageInfoEntryAttrs");

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id ==
              stage->tbls.slice_info_tbl->key_fid) {
              SHR_IF_ERR_VERBOSE_EXIT
                  (bcmfp_fid_value_u32_get(unit,
                                   in->field[idx],
                                   stage->tbls.slice_info_tbl->key_fid,
                                   &slice_id));
          }
    }

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id ==
              stage->tbls.slice_info_tbl->pipe_fid) {
              SHR_IF_ERR_VERBOSE_EXIT
                  (bcmfp_fid_value_u32_get(unit,
                                   in->field[idx],
                                   stage->tbls.slice_info_tbl->pipe_fid,
                                   &pipe));
              pipe_id = pipe;
          }
    }

    if (!(BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH))) {
        size = sizeof(bcmfp_buffers_t);
        BCMFP_ALLOC(req_buffers, size, "bcmfpGroupInfoReqBuffers");
        BCMFP_ALLOC(rsp_buffers, size, "bcmfpGroupInfoRspBuffers");
        bflags = BCMFP_BUFFERS_CREATE_EKW;
        bflags |= BCMFP_BUFFERS_CREATE_EDW;

        SHR_IF_ERR_VERBOSE_EXIT
           (bcmfp_filter_buffers_allocate(unit,
                                          bflags,
                                          stage->tbls.entry_tbl->sid,
                                          stage,
                                          rsp_buffers));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_buffers_allocate(unit,
                                           bflags,
                                           stage->tbls.entry_tbl->sid,
                                           stage,
                                           req_buffers));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_slice_lookup(unit,
                                    req_flags,
                                    op_arg,
                                    stage->stage_id,
                                    stage->tbls.entry_tbl->sid,
                                    slice_id,
                                    pipe_id,
                                    req_buffers,
                                    rsp_buffers,
                                    attrs));

        hw_info = attrs->slice_hw_info;
        if (hw_info != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                    stage->tbls.slice_info_tbl->num_entries_total,
                    hw_info->num_entries_total, 0));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                    stage->tbls.slice_info_tbl->num_entries_created,
                    hw_info->num_entries_created, 0));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                    stage->tbls.slice_info_tbl->num_entries_total,
                    0, 0));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                    stage->tbls.slice_info_tbl->num_entries_created,
                    0, 0));
        }
    }

    /* Copy key to out fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                            stage->tbls.slice_info_tbl->key_fid,
                            slice_id, 0));
    if (pipe_id != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_cth_out_fields_set(unit, &out_idx, out,
                                stage->tbls.slice_info_tbl->pipe_fid,
                                pipe_id, 0));
    }
    out->count = out_idx;

exit:
    bcmfp_filter_buffers_free(unit, req_buffers);
    SHR_FREE(req_buffers);
    bcmfp_filter_buffers_free(unit, rsp_buffers);
    SHR_FREE(rsp_buffers);
    SHR_FREE(attrs);
    SHR_FREE(hw_info);
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_info_first_search(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    uint32_t idx = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    shr_error_t rv = SHR_E_NONE;
    size_t size = 0;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, sizeof(bcmltd_field_t *),
                "bcmfpStageInfoFirstKeyField");

    rv = (bcmfp_stage_get(unit, stage_id, &stage));
    if (rv == SHR_E_NONE) {
        found = TRUE;
    }
    if (found) {

        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            sal_memset(insert.field[0], 0, size);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_info_update(unit,
                                     op_arg,
                                     arg->sid,
                                     stage,
                                     &insert,
                                     out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_info_next_search(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_slice_info_first_search(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    uint32_t idx = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    bcmltd_fields_t outfld;
    size_t size = 0;
    bool found = FALSE;
    bcmfp_stage_oper_mode_t oper_mode;

    SHR_FUNC_ENTER(unit);

    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    sal_memset(&outfld, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, (2 * sizeof(bcmltd_field_t *)),
                "bcmfpGroupInfoFirstKeyField");

    BCMFP_ALLOC(outfld.field, (2 * sizeof(bcmltd_field_t *)),
                "bcmfpGroupInfoFirstOutField");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (idx = 0; idx < 2; idx++) {
        /* Fill the OUT field info */
        size = sizeof(bcmltd_field_t);
        outfld.field[idx] = shr_fmm_alloc();
        if (outfld.field[idx] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            outfld.count += 1;
            sal_memset(outfld.field[idx], 0, size);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_info_update(unit,
                                 op_arg,
                                 arg->sid,
                                 stage,
                                 &insert,
                                 &outfld));

    for (idx = 0; idx < outfld.count; idx++) {
        if (outfld.field[idx]->id ==
            stage->tbls.stage_info_tbl->num_slices) {
            found = TRUE;
            break;
        }
    }

    SHR_IF_ERR_EXIT
    (bcmfp_stage_oper_mode_get(unit,
                               stage->stage_id,
                               &oper_mode));

    if (found && (outfld.field[idx]->data > 0)) {

        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = stage->tbls.slice_info_tbl->key_fid;
            insert.field[0]->data = 0;
        }

        if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
            /* Fill the IN field info */
            size = sizeof(bcmltd_field_t);
            insert.field[1] = shr_fmm_alloc();
            if (insert.field[1] == NULL) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "No memory for "
                    "KEY field in BCMFP IMM update\n")));
                SHR_ERR_EXIT(SHR_E_MEMORY);
            } else {
                insert.count += 1;
                sal_memset(insert.field[1], 0, size);
                insert.field[1]->id = stage->tbls.slice_info_tbl->pipe_fid;
                insert.field[1]->data = 0;
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_slice_info_update(unit,
                                     op_arg,
                                     arg->sid,
                                     stage,
                                     &insert,
                                     out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    if (outfld.field != NULL) {
        for (idx = 0; idx < outfld.count; idx++) {
            shr_fmm_free(outfld.field[idx]);
        }
        SHR_FREE(outfld.field);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_slice_info_next_search(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg)
{
    uint32_t idx = 0;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fields_t insert;
    bcmltd_fields_t outfld;
    size_t size = 0;
    bool found = FALSE;
    uint32_t slice_id = 0;
    bcmfp_stage_oper_mode_t oper_mode;
    uint32_t pipe_id = 0;
    uint32_t pipe_map = 0;
    uint32_t pipe_max = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&outfld, 0, sizeof(bcmltd_fields_t));
    sal_memset(&insert, 0, sizeof(bcmltd_fields_t));
    BCMFP_ALLOC(insert.field, (2 * sizeof(bcmltd_field_t *)),
                "bcmfpGroupInfoFirstKeyField");

    BCMFP_ALLOC(outfld.field, (2 * sizeof(bcmltd_field_t *)),
                "bcmfpGroupInfoFirstOutField");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (idx = 0; idx < 2; idx++) {
        /* Fill the OUT field info */
        size = sizeof(bcmltd_field_t);
        outfld.field[idx] = shr_fmm_alloc();
        if (outfld.field[idx] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            outfld.count += 1;
            sal_memset(outfld.field[idx], 0, size);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_info_update(unit,
                                 op_arg,
                                 arg->sid,
                                 stage,
                                 &insert,
                                 &outfld));

    SHR_IF_ERR_EXIT
     (bcmfp_stage_oper_mode_get(unit,
                                stage->stage_id,
                                &oper_mode));

    if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_dev_valid_pipes_get(unit,
                                       stage->stage_id,
                                       &pipe_map));
        pipe_max = shr_util_popcount(pipe_map) - 1;
    }

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id ==
              stage->tbls.slice_info_tbl->key_fid) {
              SHR_IF_ERR_VERBOSE_EXIT
                  (bcmfp_fid_value_u32_get(unit,
                                   in->field[idx],
                                   stage->tbls.slice_info_tbl->key_fid,
                                   &slice_id));
          }
    }

    for  (idx = 0; idx < in->count; idx++) {
          if (in->field[idx]->id ==
              stage->tbls.slice_info_tbl->pipe_fid) {
              SHR_IF_ERR_VERBOSE_EXIT
                  (bcmfp_fid_value_u32_get(unit,
                                   in->field[idx],
                                   stage->tbls.slice_info_tbl->pipe_fid,
                                   &pipe_id));
          }
    }

    for (idx = 0; idx < outfld.count; idx++) {
        if (outfld.field[idx]->id ==
            stage->tbls.stage_info_tbl->num_slices) {
            found = TRUE;
            break;
        }
    }

    if (found && (slice_id < outfld.field[idx]->data)) {

        /* Fill the IN field info */
        size = sizeof(bcmltd_field_t);
        insert.field[0] = shr_fmm_alloc();
        if (insert.field[0] == NULL) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "No memory for "
                "KEY field in BCMFP IMM update\n")));
            SHR_ERR_EXIT(SHR_E_MEMORY);
        } else {
            insert.count += 1;
            sal_memset(insert.field[0], 0, size);
            insert.field[0]->id = stage->tbls.slice_info_tbl->key_fid;
            insert.field[0]->data = (slice_id + 1);
        }

        if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
            /* Fill the IN field info */
            size = sizeof(bcmltd_field_t);
            insert.field[1] = shr_fmm_alloc();
            if (insert.field[1] == NULL) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "No memory for "
                    "KEY field in BCMFP IMM update\n")));
                SHR_ERR_EXIT(SHR_E_MEMORY);
            } else {
                insert.count += 1;
                sal_memset(insert.field[1], 0, size);
                insert.field[1]->id = stage->tbls.slice_info_tbl->pipe_fid;
                insert.field[1]->data = pipe_id;
            }

            if ((slice_id + 1) == outfld.field[idx]->data) {
                insert.field[0]->data = 0;
                insert.field[1]->data = (pipe_id + 1);
                if (pipe_id == pipe_max) {
                    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
                }
            }
        } else {
            if (((slice_id + 1) == outfld.field[idx]->data)) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_slice_info_update(unit,
                                     op_arg,
                                     arg->sid,
                                     stage,
                                     &insert,
                                     out));
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (insert.field != NULL) {
        for (idx = 0; idx < insert.count; idx++) {
            shr_fmm_free(insert.field[idx]);
        }
        SHR_FREE(insert.field);
    }
    if (outfld.field != NULL) {
        for (idx = 0; idx < outfld.count; idx++) {
            shr_fmm_free(outfld.field[idx]);
        }
        SHR_FREE(outfld.field);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_info_event_process(int unit,
                         const char *event,
                         uint64_t ev_data)
{
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmevm_extend_ev_data_t *extended_ev_data = NULL;
    bool group_info_get = FALSE;
    bool group_part_get = FALSE;
    bool pdd_info_get = FALSE;
    bool psg_info_get = FALSE;
    bool stage_info_get = FALSE;
    bool slice_info_get = FALSE;
    bcmltd_sid_t info_sid = BCMLTD_SID_INVALID;
    uint32_t trans_id = 0;
    const bcmltd_fields_t *in = NULL;
    bcmltd_fields_t *out = NULL;
    bool assign_rv = TRUE;
    bcmltd_op_arg_t op_arg;

    SHR_FUNC_ENTER(unit);

    extended_ev_data =
         (bcmevm_extend_ev_data_t *)(unsigned long)ev_data;
    if (extended_ev_data == NULL) {
       /*
        * Return value cannot be conveyed back in this case.
        */
        assign_rv = FALSE;
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* From here return value can be convayed back to caller module. */
    if (extended_ev_data->count != BCMFP_INFO_EVENT_PARAMS_COUNT) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }
    if (extended_ev_data->data == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(event, SHR_E_PARAM);
    if (sal_strcmp(event, "FP_INFO_LOOKUP") != 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    info_sid = extended_ev_data->data[0];
    trans_id = extended_ev_data->data[1];
    sal_memset(&op_arg, 0, sizeof(bcmltd_op_arg_t));
    op_arg.trans_id = trans_id;
    in = (bcmltd_fields_t *)(unsigned long)(extended_ev_data->data[2]);
    out = (bcmltd_fields_t *)(unsigned long)(extended_ev_data->data[3]);
    for (stage_id = 0;
         stage_id < BCMFP_STAGE_ID_COUNT;
         stage_id++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_get(unit, stage_id, &stage));
        if (stage == NULL) {
            continue;
        }

        if (stage->tbls.group_info_tbl != NULL) {
            if (stage->tbls.group_info_tbl->sid == info_sid) {
                group_info_get = TRUE;
                break;
            }
        }
        if (stage->tbls.group_part_tbl != NULL) {
            if (stage->tbls.group_part_tbl->sid == info_sid) {
                group_part_get = TRUE;
                break;
            }
        }
        if (stage->tbls.pdd_info_tbl != NULL) {
            if (stage->tbls.pdd_info_tbl->sid == info_sid) {
                pdd_info_get = TRUE;
                break;
            }
        }
        if (stage->tbls.psg_info_tbl != NULL) {
            if (stage->tbls.psg_info_tbl->sid == info_sid) {
                psg_info_get = TRUE;
                break;
            }
        }
        if (stage->tbls.stage_info_tbl != NULL) {
            if (stage->tbls.stage_info_tbl->sid == info_sid) {
                stage_info_get = TRUE;
                break;
            }
        }
        if (stage->tbls.slice_info_tbl != NULL) {
            if (stage->tbls.slice_info_tbl->sid == info_sid) {
                slice_info_get = TRUE;
                break;
            }
        }
    }

    if (stage_id == BCMFP_STAGE_ID_COUNT) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (group_info_get == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_info_update(unit,
                                     &op_arg,
                                     info_sid,
                                     stage,
                                     in,
                                     out));
    } else if (group_part_get == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_partition_info_update(unit,
                                               &op_arg,
                                               info_sid,
                                               stage,
                                               in,
                                               out));
    } else if (pdd_info_get == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_info_update(unit,
                                   trans_id,
                                   info_sid,
                                   stage,
                                   in,
                                   out));
    } else if (psg_info_get == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_psg_info_update(unit,
                                   trans_id,
                                   info_sid,
                                   stage,
                                   in,
                                   out));
    } else if (stage_info_get == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_info_update(unit,
                                     &op_arg,
                                     info_sid,
                                     stage,
                                     in,
                                     out));
    } else if (slice_info_get == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_slice_info_update(unit,
                                     &op_arg,
                                     info_sid,
                                     stage,
                                     in,
                                     out));
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    if (assign_rv == TRUE && SHR_FUNC_ERR()) {
        /*
         * Since extended_ev_data->data is constant,
         * using extended_ev_data->count to convey
         * the error in this function by setting it
         * to zero.
         */
        extended_ev_data->count = 0;
    }
    SHR_FUNC_EXIT();
}

void
bcmfp_info_event_cb(int unit,
                    const char *event,
                    uint64_t ev_data)
{
    (void)bcmfp_info_event_process(unit, event, ev_data);
}
