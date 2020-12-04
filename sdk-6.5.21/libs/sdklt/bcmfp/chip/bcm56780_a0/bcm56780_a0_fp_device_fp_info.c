/*! \file bcm56780_a0_fp_device_fp_info.c
 *
 * Temporary file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56780_a0_fp.h>
#include <bcmfp/bcmfp_tbl_internal.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV
int
bcm56780_a0_fp_info_tbls_init(int unit,
                              bcmfp_stage_t *stage)
{
    bcmltd_fid_t fid;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    fid = stage->tbls.pdd_tbl->aux_flex_ctr_valid_fid;

    stage->tbls.pdd_info_tbl->action_fid_info[fid].count_fid
    = stage->tbls.pdd_info_tbl->action_flex_ctr_valid_fid_info.count_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].offset_fid
    = stage->tbls.pdd_info_tbl->action_flex_ctr_valid_fid_info.offset_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].width_fid
    = stage->tbls.pdd_info_tbl->action_flex_ctr_valid_fid_info.width_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].order_fid
    = stage->tbls.pdd_info_tbl->action_flex_ctr_valid_fid_info.order_fid;

    fid = stage->tbls.pdd_tbl->aux_drop_code_fid;

    stage->tbls.pdd_info_tbl->action_fid_info[fid].count_fid
    = stage->tbls.pdd_info_tbl->action_drop_code_fid_info.count_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].offset_fid
    = stage->tbls.pdd_info_tbl->action_drop_code_fid_info.offset_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].width_fid
    = stage->tbls.pdd_info_tbl->action_drop_code_fid_info.width_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].order_fid
    = stage->tbls.pdd_info_tbl->action_drop_code_fid_info.order_fid;

    fid = stage->tbls.pdd_tbl->aux_drop_priority_fid;

    stage->tbls.pdd_info_tbl->action_fid_info[fid].count_fid
    = stage->tbls.pdd_info_tbl->action_drop_priority_fid_info.count_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].offset_fid
    = stage->tbls.pdd_info_tbl->action_drop_priority_fid_info.offset_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].width_fid
    = stage->tbls.pdd_info_tbl->action_drop_priority_fid_info.width_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].order_fid
    = stage->tbls.pdd_info_tbl->action_drop_priority_fid_info.order_fid;

    fid = stage->tbls.pdd_tbl->aux_trace_id_fid;

    stage->tbls.pdd_info_tbl->action_fid_info[fid].count_fid
    = stage->tbls.pdd_info_tbl->action_trace_id_fid_info.count_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].offset_fid
    = stage->tbls.pdd_info_tbl->action_trace_id_fid_info.offset_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].width_fid
    = stage->tbls.pdd_info_tbl->action_trace_id_fid_info.width_fid;
    stage->tbls.pdd_info_tbl->action_fid_info[fid].order_fid
    = stage->tbls.pdd_info_tbl->action_trace_id_fid_info.order_fid;

    fid = stage->tbls.group_tbl->drop_data_fid;
    if (fid != 0) {
        stage->tbls.group_part_tbl->qual_fid_info[fid].count_fid
            = stage->tbls.group_part_tbl->drop_data_fid_info.count_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].offset_fid
            = stage->tbls.group_part_tbl->drop_data_fid_info.offset_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].width_fid
            = stage->tbls.group_part_tbl->drop_data_fid_info.width_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].order_fid
            = stage->tbls.group_part_tbl->drop_data_fid_info.order_fid;
    }

    fid = stage->tbls.group_tbl->drop_decision_fid;
    if (fid != 0) {
        stage->tbls.group_part_tbl->qual_fid_info[fid].count_fid
            = stage->tbls.group_part_tbl->drop_decision_fid_info.count_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].offset_fid
            = stage->tbls.group_part_tbl->drop_decision_fid_info.offset_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].width_fid
            = stage->tbls.group_part_tbl->drop_decision_fid_info.width_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].order_fid
            = stage->tbls.group_part_tbl->drop_decision_fid_info.order_fid;
    }

    fid = stage->tbls.group_tbl->trace_vector_fid;
    if (fid != 0) {
        stage->tbls.group_part_tbl->qual_fid_info[fid].count_fid
            = stage->tbls.group_part_tbl->trace_vector_fid_info.count_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].offset_fid
            = stage->tbls.group_part_tbl->trace_vector_fid_info.offset_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].width_fid
            = stage->tbls.group_part_tbl->trace_vector_fid_info.width_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].order_fid
            = stage->tbls.group_part_tbl->trace_vector_fid_info.order_fid;
    }

    fid = stage->tbls.group_tbl->hash_a0_fid;
    if (fid != 0) {
        stage->tbls.group_part_tbl->qual_fid_info[fid].count_fid
            = stage->tbls.group_part_tbl->hash_a0_fid_info.count_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].offset_fid
            = stage->tbls.group_part_tbl->hash_a0_fid_info.offset_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].width_fid
            = stage->tbls.group_part_tbl->hash_a0_fid_info.width_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].order_fid
            = stage->tbls.group_part_tbl->hash_a0_fid_info.order_fid;
    }

    fid = stage->tbls.group_tbl->hash_a1_fid;
    if (fid != 0) {
        stage->tbls.group_part_tbl->qual_fid_info[fid].count_fid
            = stage->tbls.group_part_tbl->hash_a1_fid_info.count_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].offset_fid
            = stage->tbls.group_part_tbl->hash_a1_fid_info.offset_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].width_fid
            = stage->tbls.group_part_tbl->hash_a1_fid_info.width_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].order_fid
            = stage->tbls.group_part_tbl->hash_a1_fid_info.order_fid;
    }

    fid = stage->tbls.group_tbl->hash_b0_fid;
    if (fid != 0) {
        stage->tbls.group_part_tbl->qual_fid_info[fid].count_fid
            = stage->tbls.group_part_tbl->hash_b0_fid_info.count_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].offset_fid
            = stage->tbls.group_part_tbl->hash_b0_fid_info.offset_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].width_fid
            = stage->tbls.group_part_tbl->hash_b0_fid_info.width_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].order_fid
            = stage->tbls.group_part_tbl->hash_b0_fid_info.order_fid;
    }

    fid = stage->tbls.group_tbl->hash_b1_fid;
    if (fid != 0) {
        stage->tbls.group_part_tbl->qual_fid_info[fid].count_fid
            = stage->tbls.group_part_tbl->hash_b1_fid_info.count_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].offset_fid
            = stage->tbls.group_part_tbl->hash_b1_fid_info.offset_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].width_fid
            = stage->tbls.group_part_tbl->hash_b1_fid_info.width_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].order_fid
            = stage->tbls.group_part_tbl->hash_b1_fid_info.order_fid;
    }

    fid = stage->tbls.group_tbl->hash_c0_fid;
    if (fid != 0) {
        stage->tbls.group_part_tbl->qual_fid_info[fid].count_fid
            = stage->tbls.group_part_tbl->hash_c0_fid_info.count_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].offset_fid
            = stage->tbls.group_part_tbl->hash_c0_fid_info.offset_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].width_fid
            = stage->tbls.group_part_tbl->hash_c0_fid_info.width_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].order_fid
            = stage->tbls.group_part_tbl->hash_c0_fid_info.order_fid;
    }

    fid = stage->tbls.group_tbl->hash_c1_fid;
    if (fid != 0) {
        stage->tbls.group_part_tbl->qual_fid_info[fid].count_fid
            = stage->tbls.group_part_tbl->hash_c1_fid_info.count_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].offset_fid
            = stage->tbls.group_part_tbl->hash_c1_fid_info.offset_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].width_fid
            = stage->tbls.group_part_tbl->hash_c1_fid_info.width_fid;
        stage->tbls.group_part_tbl->qual_fid_info[fid].order_fid
            = stage->tbls.group_part_tbl->hash_c1_fid_info.order_fid;
    }

    fid = stage->tbls.psg_tbl->drop_decision_fid;
    if (fid != 0) {
        stage->tbls.psg_info_tbl->qual_fid_info[fid].count_fid
            = stage->tbls.psg_info_tbl->drop_decision_fid_info.count_fid;
        stage->tbls.psg_info_tbl->qual_fid_info[fid].offset_fid
            = stage->tbls.psg_info_tbl->drop_decision_fid_info.offset_fid;
        stage->tbls.psg_info_tbl->qual_fid_info[fid].width_fid
            = stage->tbls.psg_info_tbl->drop_decision_fid_info.width_fid;
        stage->tbls.psg_info_tbl->qual_fid_info[fid].order_fid
            = stage->tbls.psg_info_tbl->drop_decision_fid_info.order_fid;
    }
exit:
    SHR_FUNC_EXIT();

}
