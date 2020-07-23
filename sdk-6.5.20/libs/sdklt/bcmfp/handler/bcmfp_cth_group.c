/*! \file bcmfp_cth_group.c
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
               GROUP CONFIG GET FUNCTIONS
 */
static int
bcmfp_group_id_get(int unit,
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

    if (stage->tbls.group_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    group_id_fid = stage->tbls.group_tbl->key_fid;

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

static int
bcmfp_group_enable_get(int unit,
                   bcmfp_stage_t *stage,
                   bcmltd_field_t *buffer,
                   bool *enable)
{
    uint32_t enable_fid = 0;
    uint64_t enable_u64 = 1;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    if (stage->tbls.group_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    enable_fid = stage->tbls.group_tbl->enable_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             enable_fid,
                             (void *)buffer,
                             &enable_u64),
         SHR_E_NOT_FOUND);

    *enable = enable_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_auto_expand_get(int unit,
                            bcmfp_stage_t *stage,
                            bcmltd_field_t *buffer,
                            bool *auto_expand)
{
    uint32_t auto_expand_fid = 0;
    uint64_t enaable_u64 = 1;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(auto_expand, SHR_E_PARAM);

    if (stage->tbls.group_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    auto_expand_fid = stage->tbls.group_tbl->auto_expand_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             auto_expand_fid,
                             (void *)buffer,
                             &enaable_u64),
         SHR_E_NOT_FOUND);

    *auto_expand = enaable_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_mode_get(int unit,
                     bcmfp_stage_t *stage,
                     bcmltd_field_t *buffer,
                     bcmfp_group_mode_t *group_mode)
{
    uint32_t mode_fid = 0;
    uint32_t mode_auto_fid = 0;
    uint64_t mode_u64 = 0;
    uint64_t mode_auto_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(group_mode, SHR_E_PARAM);

    if (stage->tbls.group_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    mode_auto_fid = stage->tbls.group_tbl->mode_auto_fid;
    mode_auto_u64 = TRUE;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             mode_auto_fid,
                             (void *)buffer,
                             &mode_auto_u64),
         SHR_E_NOT_FOUND);
    if (mode_auto_u64 == TRUE) {
        *group_mode = BCMFP_GROUP_MODE_AUTO;
        SHR_EXIT();
    }

    mode_fid = stage->tbls.group_tbl->mode_fid;
    mode_u64 = 1;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             mode_fid,
                             (void *)buffer,
                             &mode_u64),
         SHR_E_NOT_FOUND);
    switch (mode_u64) {
        case 0:
            *group_mode = BCMFP_GROUP_MODE_NONE;
            break;
        case 1:
            *group_mode = BCMFP_GROUP_MODE_SINGLE;
            break;
        case 2:
            *group_mode = BCMFP_GROUP_MODE_DBLINTRA;
            break;
        case 3:
            *group_mode = BCMFP_GROUP_MODE_DBLINTER;
            break;
        case 4:
            *group_mode = BCMFP_GROUP_MODE_TRIPLE;
            break;
        case 5:
            *group_mode = BCMFP_GROUP_MODE_QUAD;
            break;
        case 6:
            *group_mode = BCMFP_GROUP_MODE_HALF;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_slice_type_get(int unit,
                     bcmfp_stage_t *stage,
                     bcmltd_field_t *buffer,
                     bcmfp_group_slice_type_t *group_slice_type)
{
    uint32_t group_slice_type_fid = 0;
    uint64_t group_slice_type_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(group_slice_type, SHR_E_PARAM);

    if (stage->tbls.group_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    group_slice_type_fid = stage->tbls.group_tbl->group_slice_type_fid;
    group_slice_type_u64 = 0;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             group_slice_type_fid,
                             (void *)buffer,
                             &group_slice_type_u64),
         SHR_E_NOT_FOUND);
    switch (group_slice_type_u64) {
        case 0:
            *group_slice_type = BCMFP_GROUP_SLICE_TYPE_NONE;
            break;
        case 1:
            *group_slice_type = BCMFP_GROUP_SLICE_TYPE_SMALL;
            break;
        case 2:
            *group_slice_type = BCMFP_GROUP_SLICE_TYPE_LARGE;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_presel_params_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmltd_field_t *buffer,
                        uint8_t *num_presels,
                        uint32_t **presel_ids,
                        uint32_t *psg_id)
{
    uint32_t psg_id_fid = 0;
    uint64_t psg_id_u64 = 0;
    uint32_t num_presels_fid = 0;
    uint64_t num_presels_u64 = 0;
    uint32_t presel_id_fid = 0;
    uint64_t presel_id_u64 = 0;
    uint32_t *presel_ids_u32 = NULL;
    uint8_t index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(num_presels, SHR_E_PARAM);
    SHR_NULL_CHECK(presel_ids, SHR_E_PARAM);
    SHR_NULL_CHECK(psg_id, SHR_E_PARAM);

    psg_id_fid = stage->tbls.group_tbl->psg_id_fid;
    if (psg_id_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 psg_id_fid,
                                 (void *)buffer,
                                 &psg_id_u64),
             SHR_E_NOT_FOUND);
        *psg_id = psg_id_u64 & 0xFFFFFFFF;
    }

    num_presels_fid = stage->tbls.group_tbl->presel_ids_count_fid;
    presel_id_fid = stage->tbls.group_tbl->presel_ids_fid;

    if ((num_presels_fid != 0) && (presel_id_fid != 0)){
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 num_presels_fid,
                                 (void *)buffer,
                                 &num_presels_u64),
             SHR_E_NOT_FOUND);
        *num_presels = num_presels_u64 & 0xFFFFFFFF;
        if (num_presels_u64) {
            BCMFP_ALLOC(presel_ids_u32,
                    sizeof(uint32_t) * num_presels_u64,
                    "bcmfpGroupPreselIdArray");
        }

        for (index = 0; index < num_presels_u64; index++) {
            presel_id_u64 = 0;
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (bcmfp_fid_array_value_get(unit,
                                           presel_id_fid,
                                           index,
                                           (void *)buffer,
                                           &presel_id_u64),
                    SHR_E_NOT_FOUND);
            presel_ids_u32[index] = presel_id_u64 & 0xFFFFFFFF;
       }
       *presel_ids = presel_ids_u32;
   }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(presel_ids_u32);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_default_policy_meter_id_get(int unit,
                                        bcmfp_stage_t *stage,
                                        bcmltd_field_t *buffer,
                                        uint32_t *default_meter_id)
{
    uint32_t default_meter_id_fid = 0;
    uint64_t default_meter_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(default_meter_id, SHR_E_PARAM);

    if (stage->tbls.group_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    default_meter_id_fid = stage->tbls.group_tbl->default_meter_id_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             default_meter_id_fid,
                             (void *)buffer,
                             &default_meter_id_u64),
            SHR_E_NOT_FOUND);

    *default_meter_id = default_meter_id_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_default_policy_eflex_ctr_params_get(int unit,
                                                bcmfp_stage_t *stage,
                                                bcmltd_field_t *buffer,
                                                uint32_t *default_flex_ctr_action)
{
    uint32_t default_flex_ctr_action_fid = 0;
    uint64_t default_flex_ctr_action_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(default_flex_ctr_action, SHR_E_PARAM);

    if (stage->tbls.group_tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    default_flex_ctr_action_fid = stage->tbls.group_tbl->default_flex_ctr_action_fid;

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             default_flex_ctr_action_fid,
                             (void *)buffer,
                             &default_flex_ctr_action_u64),
        SHR_E_NOT_FOUND);

    *default_flex_ctr_action = default_flex_ctr_action_u64 & 0xFFFFFFFF;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_policy_params_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmltd_field_t *buffer,
                        uint32_t *pdd_id,
                        uint32_t *default_pdd_id,
                        uint32_t *default_policy_id)
{
    uint32_t pdd_id_fid = 0;
    uint64_t pdd_id_u64 = 0;
    uint32_t default_pdd_id_fid = 0;
    uint64_t default_pdd_id_u64 = 0;
    uint32_t default_policy_id_fid = 0;
    uint64_t default_policy_id_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(pdd_id, SHR_E_PARAM);
    SHR_NULL_CHECK(default_pdd_id, SHR_E_PARAM);
    SHR_NULL_CHECK(default_policy_id, SHR_E_PARAM);

    pdd_id_fid = stage->tbls.group_tbl->pdd_id_fid;
    if (pdd_id_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 pdd_id_fid,
                                 (void *)buffer,
                                 &pdd_id_u64),
             SHR_E_NOT_FOUND);
        *pdd_id = pdd_id_u64 & 0xFFFFFFFF;
    }

    default_pdd_id_fid =
        stage->tbls.group_tbl->default_pdd_id_fid;
    if (default_pdd_id_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 default_pdd_id_fid,
                                 (void *)buffer,
                                 &default_pdd_id_u64),
             SHR_E_NOT_FOUND);
        *default_pdd_id = default_pdd_id_u64 & 0xFFFFFFFF;
    }

    default_policy_id_fid =
        stage->tbls.group_tbl->default_policy_id_fid;
    if (default_policy_id_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 default_policy_id_fid,
                                 (void *)buffer,
                                 &default_policy_id_u64),
             SHR_E_NOT_FOUND);
        *default_policy_id = default_policy_id_u64 & 0xFFFFFFFF;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_hash_key_params_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmltd_field_t *buffer,
                        uint8_t *group_ltid,
                        uint8_t *group_lookup_id,
                        bool *group_ltid_auto)
{
    uint32_t group_ltid_fid = 0;
    uint64_t group_ltid_u64 = 0;
    uint32_t group_lookup_id_fid = 0;
    uint64_t group_lookup_id_u64 = 0;
    uint32_t group_ltid_auto_fid = 0;
    uint64_t group_ltid_auto_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(group_ltid, SHR_E_PARAM);
    SHR_NULL_CHECK(group_lookup_id, SHR_E_PARAM);
    SHR_NULL_CHECK(group_ltid_auto, SHR_E_PARAM);

    group_ltid_fid = stage->tbls.group_tbl->group_ltid_fid;
    if (group_ltid_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 group_ltid_fid,
                                 (void *)buffer,
                                 &group_ltid_u64),
             SHR_E_NOT_FOUND);
        *group_ltid = group_ltid_u64 & 0xFFFFFFFF;
    }

    group_lookup_id_fid =
        stage->tbls.group_tbl->group_lookup_id_fid;
    if (group_lookup_id_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 group_lookup_id_fid,
                                 (void *)buffer,
                                 &group_lookup_id_u64),
             SHR_E_NOT_FOUND);
        *group_lookup_id = group_lookup_id_u64 & 0xFFFFFFFF;
    }

    group_ltid_auto_fid =
        stage->tbls.group_tbl->group_ltid_auto_fid;
    if (group_ltid_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 group_ltid_auto_fid,
                                 (void *)buffer,
                                 &group_ltid_auto_u64),
             SHR_E_NOT_FOUND);
        *group_ltid_auto = group_ltid_auto_u64 & 0x1;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_fixed_key_params_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmltd_field_t *buffer,
                        bcmfp_group_slice_mode_t *slice_mode,
                        bcmfp_group_type_t *group_type)
{
    uint32_t slice_mode_fid = 0;
    uint64_t slice_mode_u64 = 0;
    uint32_t group_type_fid = 0;
    uint64_t group_type_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_mode, SHR_E_PARAM);
    SHR_NULL_CHECK(group_type, SHR_E_PARAM);

    slice_mode_fid = stage->tbls.group_tbl->slice_mode_fid;
    if (slice_mode_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 slice_mode_fid,
                                 (void *)buffer,
                                 &slice_mode_u64),
             SHR_E_NOT_FOUND);
        *slice_mode = slice_mode_u64 & 0xFFFFFFFF;
    }

    group_type_fid = stage->tbls.group_tbl->group_type_fid;
    if (group_type_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 group_type_fid,
                                 (void *)buffer,
                                 &group_type_u64),
             SHR_E_NOT_FOUND);
        *group_type = group_type_u64 & 0xFFFFFFFF;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_vslice_key_params_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmltd_field_t *buffer,
                        uint8_t *vslice_group)
{
    uint32_t vslice_group_fid = 0;
    uint64_t vslice_group_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(vslice_group, SHR_E_PARAM);

    vslice_group_fid = stage->tbls.group_tbl->vslice_group_fid;
    if (vslice_group_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 vslice_group_fid,
                                 (void *)buffer,
                                 &vslice_group_u64),
             SHR_E_NOT_FOUND);
        *vslice_group = vslice_group_u64 & 0xFF;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_priority_params_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmltd_field_t *buffer,
                        uint32_t *sbr_id,
                        uint32_t *priority,
                        uint32_t *default_sbr_id)
{
    uint32_t sbr_id_fid = 0;
    uint64_t sbr_id_u64 = 0;
    uint32_t default_sbr_id_fid = 0;
    uint64_t default_sbr_id_u64 = 0;
    uint32_t prio_fid = 0;
    uint64_t prio_u64 = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(sbr_id, SHR_E_PARAM);
    SHR_NULL_CHECK(priority, SHR_E_PARAM);

    sbr_id_fid = stage->tbls.group_tbl->sbr_id_fid;
    if (sbr_id_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 sbr_id_fid,
                                 (void *)buffer,
                                 &sbr_id_u64),
             SHR_E_NOT_FOUND);
        *sbr_id = sbr_id_u64 & 0xFFFFFFFF;
    }

    default_sbr_id_fid =
        stage->tbls.group_tbl->default_sbr_id_fid;
    if (default_sbr_id_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 default_sbr_id_fid,
                                 (void *)buffer,
                                 &default_sbr_id_u64),
             SHR_E_NOT_FOUND);
        *default_sbr_id = default_sbr_id_u64 & 0xFFFFFFFF;
    }

    prio_fid = stage->tbls.group_tbl->priority_fid;
    if (prio_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 prio_fid,
                                 (void *)buffer,
                                 &prio_u64),
             SHR_E_NOT_FOUND);
        *priority = prio_u64 & 0xFFFFFFFF;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_pipe_id_get(int unit,
                        bcmfp_stage_t *stage,
                        bcmltd_field_t *buffer,
                        int *tbl_inst)
{
    uint32_t pipe_id_fid = 0;
    uint64_t pipe_id_u64 = 0;
    bcmfp_stage_oper_mode_t oper_mode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_oper_mode_get(unit,
                                   stage->stage_id,
                                   &oper_mode));

    if (oper_mode != BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
        *tbl_inst = -1;
        SHR_EXIT();
    }

    pipe_id_fid = stage->tbls.group_tbl->pipe_id_fid;
    if (pipe_id_fid != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_value_get(unit,
                                 pipe_id_fid,
                                 (void *)buffer,
                                 &pipe_id_u64),
             SHR_E_NOT_FOUND);
        *tbl_inst = pipe_id_u64 & 0xF;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_compress_params_get(int unit,
                               bcmfp_stage_t *stage,
                               bcmltd_field_t *buffer,
                               bool *compress_types,
                               bool *compress_group)
{
    uint64_t compress_type_u64 = 0;
    uint32_t compress_type_fid = 0;
    uint64_t num_compress_type_u64 = 0;
    uint32_t num_compress_type_fid = 0;
    uint32_t num_compress_types = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(compress_types, SHR_E_PARAM);
    SHR_NULL_CHECK(compress_group, SHR_E_PARAM);

    num_compress_type_fid =
        stage->tbls.group_tbl->num_compression_type_fid;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_fid_value_get(unit,
                             num_compress_type_fid,
                             (void *)buffer,
                             &num_compress_type_u64),
         SHR_E_NOT_FOUND);
    num_compress_types = num_compress_type_u64;

    if (num_compress_types == 0) {
        SHR_EXIT();
    }

    for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
        compress_types[idx] = FALSE;
    }
    compress_type_fid =
        stage->tbls.group_tbl->compression_type_array_fid;
    for (idx = 0; idx < num_compress_types; idx++) {
        compress_type_u64 = 0;
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_fid_array_value_get(unit,
                                       compress_type_fid,
                                       idx,
                                       (void *)buffer,
                                       &compress_type_u64),
             SHR_E_NOT_FOUND);
        compress_types[compress_type_u64] = TRUE;
    }

    *compress_group = FALSE;
    for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
        if (compress_types[idx] == TRUE) {
            *compress_group = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_qualifiers_bitmap_get(int unit,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_t *stage,
                        bcmltd_field_t *buffer,
                        bcmfp_qual_bitmap_t **qb)
{
    bool is_partial = FALSE;
    bcmlrd_fid_t pbmp_fid = 0;
    uint32_t *bitmap = NULL;
    bcmfp_qual_bitmap_t *bitmap_one = NULL;
    bcmlrd_field_def_t field_def;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    bcmdrd_pbmp_t *pbmp = NULL;
    bcmfp_qualifier_t previous_qual = 0;
    bcmfp_qualifier_t curr_qual = 0;
    bool pbmp_type_qual = TRUE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(buffer, SHR_E_PARAM);
    SHR_NULL_CHECK(qb, SHR_E_PARAM);

    qualifiers_fid_info =
        stage->tbls.group_tbl->qualifiers_fid_info;
    if (qualifiers_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(pbmp,
                sizeof(bcmdrd_pbmp_t),
                "bcmfpPbmp");

    *qb = NULL;
    while (buffer != NULL) {
        if (qualifiers_fid_info[buffer->id].qualifier == 0) {
            buffer = buffer->next;
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        buffer->id,
                                        &field_def));
        if ((buffer->id == stage->tbls.group_tbl->qual_inports_fid) ||
            (buffer->id == stage->tbls.group_tbl->qual_system_ports_fid) ||
            (buffer->id == stage->tbls.group_tbl->qual_device_ports_fid)) {
            pbmp_type_qual = TRUE;
        } else {
            pbmp_type_qual = FALSE;
        }
        if ((field_def.depth == 0) || (pbmp_type_qual == TRUE)) {
            curr_qual =
                qualifiers_fid_info[buffer->id].qualifier;
        } else {
            curr_qual =
                qualifiers_fid_info[buffer->id].qualifier + buffer->idx;
        }

        if (curr_qual != previous_qual) {
            bitmap_one = NULL;
            BCMFP_ALLOC(bitmap_one,
                        sizeof(bcmfp_qual_bitmap_t),
                        "bcmfpGroupQualBitmap");
            /*
             * Add the new node to the linked list of
             * qualifiers bitmap.
             */
            bitmap_one->next = *qb;
            *qb = bitmap_one;
        } else if (bitmap_one == NULL) {
            /*
             * This should never happen. Added to avoid
             * coverity error
             */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        bitmap = bitmap_one->bitmap;
        /* Update the qualifier. */
        if (pbmp_type_qual == TRUE) {
            bitmap_one->qual = curr_qual;
            pbmp_fid = buffer->id;
            sal_memset(pbmp, 0, sizeof(bcmdrd_pbmp_t));
            bitmap_one->fid = buffer->id;
            bitmap_one->fidx = 0;
            /* Update the bitmap for the qualifiers of array type */
            while (buffer != NULL)  {
                SHR_IF_ERR_EXIT
                    (bcmfp_ports_bitmap_get(unit,
                                            buffer,
                                            pbmp));
                buffer = buffer->next;
                if ((buffer != NULL) &&
                    (buffer->id != pbmp_fid)) {
                    break;
                }
            }
            sal_memcpy(bitmap, pbmp->w, sizeof(pbmp->w));
        } else {
            /* Is qualifier bitmap is partial. */
            bitmap_one->qual = curr_qual;
            bitmap_one->fid = buffer->id;
            if (field_def.depth == 0 && field_def.width > 64) {
                bitmap_one->fidx = 0;
            } else {
                bitmap_one->fidx = buffer->idx;
            }
            SHR_IF_ERR_EXIT
                (bcmfp_fid_value_is_partial(unit,
                                            field_def.width,
                                            buffer->data,
                                            &is_partial));
            bitmap_one->partial = is_partial;

            bitmap = bitmap_one->bitmap;
            SHR_IF_ERR_EXIT
                (bcmfp_tbl_field_value_get(unit,
                                           buffer->data,
                                           field_def.width,
                                           buffer->idx,
                                           bitmap));
            previous_qual = curr_qual;
            buffer = buffer->next;
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_qual_bitmap_free(unit, *qb);
        *qb = NULL;
    }
    SHR_FREE(pbmp);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_qualifier_fid_get(int unit,
                              bcmfp_stage_t *stage,
                              bcmfp_qualifier_t qualifier,
                              bcmlrd_fid_t *fid)
{
    uint16_t idx = 0;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fid, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    qualifiers_fid_info =
        stage->tbls.group_tbl->qualifiers_fid_info;
    if (qualifiers_fid_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < stage->tbls.group_tbl->fid_count; idx++) {
        if (qualifiers_fid_info[idx].qualifier == qualifier) {
            *fid = idx;
            break;
        }
    }

    if (idx == stage->tbls.group_tbl->fid_count) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}
static int
bcmfp_group_config_dump(int unit,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_config_t *group_config)
{
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;
    bcmlrd_field_def_t field_def;
    bcmlrd_fid_t fid = 0;
    bcmfp_stage_t *stage = NULL;
    int idx = 0;
    bcmfp_qualifier_t qualifier;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_config, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.group_tbl == NULL ||
        stage->tbls.group_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (stage->tbls.group_tbl->key_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit, "Group ID = %d\n"),
            group_config->group_id));
    }

    if (stage->tbls.group_tbl->mode_fid != 0) {
        char *group_mode_string[] = BCMFP_GROUP_MODE_STRINGS;
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "Group Mode = %s\n"),
                   group_mode_string[group_config->mode]));
    }

    if (stage->tbls.group_tbl->presel_ids_count_fid != 0 &&
        stage->tbls.group_tbl->presel_ids_fid != 0) {
        if (stage->tbls.group_tbl->psg_id_fid != 0) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "Presel group ID = %d"),
                       group_config->psg_id));
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "Number of presel entries = %d\n"),
                   group_config->num_presels));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "Presel entry IDs = ")));
        for (idx = 0; idx < group_config->num_presels; idx++) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "%d"),
                       group_config->presel_ids[idx]));
            if (idx != ((group_config->num_presels - 1))) {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, ", ")));
            }
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
    }

    if (stage->tbls.group_tbl->pdd_id_fid != 0 ||
        stage->tbls.group_tbl->default_policy_id_fid != 0 ||
        stage->tbls.group_tbl->default_pdd_id_fid != 0) {
        if (stage->tbls.group_tbl->pdd_id_fid != 0) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "PDD ID = %d\n"),
                       group_config->pdd_id));
        }
        if (stage->tbls.group_tbl->default_policy_id_fid != 0) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "Default policy ID = %d\n"),
                       group_config->default_policy_id));
        }
        if (stage->tbls.group_tbl->default_pdd_id_fid != 0) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "Default pdd ID = %d\n"),
                       group_config->default_pdd_id));
        }
    }

    if (stage->tbls.group_tbl->group_lookup_id_fid != 0 &&
        stage->tbls.group_tbl->group_ltid_fid != 0 &&
        stage->tbls.group_tbl->group_ltid_auto_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "Group lookup ID = %d\n"),
                   group_config->lookup_id));
        if (group_config->ltid_auto == 0) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "LTID auto enabled? - %s\n"), "NO"));
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "Group HW LTID = %d\n"),
                       group_config->action_res_id));
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "LTID auto enabled? - %s\n"), "YES"));
        }
    }

    if (stage->tbls.group_tbl->group_type_fid != 0 &&
        stage->tbls.group_tbl->slice_mode_fid != 0) {
        char *group_type_strings[] = BCMFP_GROUP_TYPE_STRINGS;
        char *slice_mode_strings[] = BCMFP_SLICE_MODE_STRINGS;
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "Group type = %s\n"),
                   group_type_strings[group_config->type]));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "Slice mode = %s\n"),
                   slice_mode_strings[group_config->slice_mode]));
    }

    if (stage->tbls.group_tbl->vslice_group_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "Virtual slice group = %d\n"),
                   group_config->vslice_group));
    }

    if (stage->tbls.group_tbl->priority_fid != 0 ||
        stage->tbls.group_tbl->sbr_id_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "Group priority = %d\n"),
                   group_config->priority));
        if (stage->tbls.group_tbl->sbr_id_fid != 0) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "SBR ID = %d\n"),
                       group_config->sbr_id));
        }

    }

    if (stage->tbls.group_tbl->pipe_id_fid != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "Pipe Id = %d\n"),
                   group_config->pipe_id));
    }

    qual_bitmap = group_config->qual_bitmap;
    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit, "Qualifier bitmaps:\n")));
    while (qual_bitmap != NULL) {
        sal_memset(&field_def, 0, sizeof(bcmlrd_field_def_t));
        qualifier = qual_bitmap->qual;
        SHR_IF_ERR_EXIT
            (bcmfp_group_qualifier_fid_get(unit,
                                           stage,
                                           qualifier,
                                           &fid));
        SHR_IF_ERR_EXIT
            (bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        fid,
                                        &field_def));
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "%s = "), field_def.name));
        for (idx = (field_def.width / 32); idx >= 0; idx--) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit, "0x%x_"),
                       qual_bitmap->bitmap[idx]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit, "\n")));
        qual_bitmap = qual_bitmap->next;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_config_init(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_config_t *group_config)
{
    bcmfp_stage_oper_mode_t oper_mode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_config, SHR_E_PARAM);

    group_config->mode = BCMFP_GROUP_MODE_AUTO;

    SHR_IF_ERR_EXIT
        (bcmfp_stage_oper_mode_get(unit,
                                   stage_id,
                                   &oper_mode));

    if (oper_mode != BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
        group_config->pipe_id = -1;
    } else {
        group_config->pipe_id = 0;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_config_get(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t tbl_id,
                       bcmfp_stage_id_t stage_id,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       bcmfp_group_config_t **config)
{
    bcmfp_stage_t *stage = NULL;
    bcmltd_field_t *buffer = NULL;
    uint32_t group_id = 0;
    uint32_t *presel_ids = NULL;
    uint8_t num_presels = 0;
    uint32_t psg_id = 0;
    uint32_t pdd_id = 0;
    uint32_t default_pdd_id = 0;
    uint32_t default_policy_id = 0;
    uint32_t default_sbr_id = 0;
    uint32_t default_meter_id = 0;
    uint32_t default_flex_ctr_action = 0;
    uint8_t action_res_id = 0;
    uint8_t lookup_id = 0;
    bool ltid_auto = 0;
    uint8_t vslice_group = 0;
    uint32_t sbr_id = 0;
    uint32_t priority = 0;
    int pipe_id = 0;
    bool *compress_types = NULL;
    bool *compress_group = NULL;
    bcmfp_group_mode_t mode = BCMFP_GROUP_MODE_COUNT;
    bcmfp_group_slice_type_t group_slice_type = BCMFP_GROUP_SLICE_TYPE_COUNT;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;
    bcmfp_group_slice_mode_t slice_mode = BCMFP_GROUP_SLICE_MODE_COUNT;
    bcmfp_group_type_t group_type = BCMFP_GROUP_TYPE_COUNT;
    bool enable = 1;
    /* Default enable */
    bool auto_expand = 1;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (stage->tbls.group_tbl == NULL ||
        stage->tbls.group_tbl->sid != tbl_id) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(group_config,
                sizeof(bcmfp_group_config_t),
                "bcmfpGroupConfig");
    *config = group_config;

    /* Get the group id */
    buffer = (bcmltd_field_t *)key;
    SHR_IF_ERR_EXIT
        (bcmfp_group_id_get(unit,
                            stage,
                            buffer,
                            &group_id));
    group_config->group_id = group_id;

    SHR_IF_ERR_EXIT
        (bcmfp_group_config_init(unit,
                                 stage_id,
                                 group_config));

    /*
     * if data is NULL no need to extract the
     * remaining config.
     */
    if (data == NULL) {
        SHR_EXIT();
    }

    /* Get the group enable. */
    if (stage->tbls.group_tbl->enable_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_enable_get(unit,
                                  stage,
                                  buffer,
                                  &enable));
        group_config->enable = enable;
    }

    /* Get the group mode. */
    if (stage->tbls.group_tbl->mode_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_mode_get(unit,
                                  stage,
                                  buffer,
                                  &mode));
        group_config->mode = mode;
    }

    /* get the group hint - slice type. */
    if (stage->tbls.group_tbl->group_slice_type_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_slice_type_get(unit,
                                  stage,
                                  buffer,
                                  &group_slice_type));
        group_config->group_slice_type = group_slice_type;
    }

    /* get the group hint - auto_expansion. */
    if (stage->tbls.group_tbl->auto_expand_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_auto_expand_get(unit,
                                         stage,
                                         buffer,
                                         &auto_expand));
    }
    group_config->auto_expand = auto_expand;
    /* Get preselection entry count and IDs. */
    if (stage->tbls.group_tbl->presel_ids_count_fid != 0 &&
        stage->tbls.group_tbl->presel_ids_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_presel_params_get(unit,
                                           stage,
                                           buffer,
                                           &num_presels,
                                           &presel_ids,
                                           &psg_id));
        group_config->num_presels = num_presels;
        group_config->presel_ids = presel_ids;
        group_config->psg_id = psg_id;
    }

    /* Get policy related remote key ids. */
    if (stage->tbls.group_tbl->pdd_id_fid != 0 ||
        stage->tbls.group_tbl->default_policy_id_fid != 0 ||
        stage->tbls.group_tbl->default_pdd_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_policy_params_get(unit,
                                           stage,
                                           buffer,
                                           &pdd_id,
                                           &default_pdd_id,
                                           &default_policy_id));
        group_config->pdd_id = pdd_id;
        group_config->default_pdd_id = default_pdd_id;
        group_config->default_policy_id = default_policy_id;
    }

    /* Get the default policy meter ID */
    if (stage->tbls.group_tbl->default_meter_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_default_policy_meter_id_get(unit,
                                                     stage,
                                                     buffer,
                                                     &default_meter_id));
        group_config->default_meter_id = default_meter_id;
    }

    /* Get the deault policy flex ctr action and object */
    if (stage->tbls.group_tbl->default_flex_ctr_action_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_default_policy_eflex_ctr_params_get(unit,
                                                            stage,
                                                           buffer,
                                        &default_flex_ctr_action));
        group_config->default_flex_ctr_action = default_flex_ctr_action;

    }

    /* Get the parameters specific to HASH KEY type stages. */
    if (stage->tbls.group_tbl->group_lookup_id_fid != 0 &&
        stage->tbls.group_tbl->group_ltid_fid != 0 &&
        stage->tbls.group_tbl->group_ltid_auto_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_hash_key_params_get(unit,
                                             stage,
                                             buffer,
                                             &action_res_id,
                                             &lookup_id,
                                             &ltid_auto));
        group_config->action_res_id = action_res_id;
        group_config->lookup_id = lookup_id;
        group_config->ltid_auto = ltid_auto;
    }

    /* Get the parameters specific to FIXED KEY type stages. */
    if (stage->tbls.group_tbl->group_type_fid != 0 &&
        stage->tbls.group_tbl->slice_mode_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_fixed_key_params_get(unit,
                                              stage,
                                              buffer,
                                              &slice_mode,
                                              &group_type));
        group_config->type = group_type;
        group_config->slice_mode = slice_mode;
    }

    /* Get the parameters specific to VSLICE KEY type stages. */
    if (stage->tbls.group_tbl->vslice_group_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_vslice_key_params_get(unit,
                                               stage,
                                               buffer,
                                               &vslice_group));
        group_config->vslice_group = vslice_group;
    }

    /* Get the priority parameters */
    if (stage->tbls.group_tbl->priority_fid != 0 ||
        stage->tbls.group_tbl->sbr_id_fid != 0 ||
        stage->tbls.group_tbl->default_sbr_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_priority_params_get(unit,
                                             stage,
                                             buffer,
                                             &sbr_id,
                                             &priority,
                                             &default_sbr_id));
        group_config->priority = priority;
        group_config->sbr_id = sbr_id;
        group_config->default_sbr_id = default_sbr_id;
    }

    /* Get the pipe id */
    if (stage->tbls.group_tbl->pipe_id_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        SHR_IF_ERR_EXIT
            (bcmfp_group_pipe_id_get(unit,
                                     stage,
                                     buffer,
                                     &pipe_id));
        group_config->pipe_id = pipe_id;
    }

    /* Get if the group is enabled for compression or not? */
    if (stage->tbls.group_tbl->compression_type_array_fid != 0 &&
        stage->tbls.group_tbl->num_compression_type_fid != 0) {
        buffer = (bcmltd_field_t *)data;
        compress_types = group_config->compress_types;
        compress_group = &(group_config->compress);
        SHR_IF_ERR_EXIT
            (bcmfp_group_compress_params_get(unit,
                                             stage,
                                             buffer,
                                             compress_types,
                                             compress_group));
    }

    /* Get the QSET and bitmaps of qualifiers */
    SHR_IF_ERR_EXIT
        (bcmfp_group_qualifiers_bitmap_get(unit,
                                           tbl_id,
                                           stage,
                                           buffer,
                                           &qual_bitmap));
    group_config->qual_bitmap = qual_bitmap;
    /* Update the groups QSET. */
    while (qual_bitmap != NULL) {
        SHR_BITSET(group_config->qset.w, qual_bitmap->qual);
        qual_bitmap = qual_bitmap->next;
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmfp_group_config_free(unit,group_config);
        *config = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_group_remote_config_get(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_group_id_t group_id,
                              bcmfp_group_config_t **config)
{
    int rv;
    bcmltd_fid_t key_fid;
    bcmltd_sid_t tbl_id;
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *key = NULL;
    bcmltd_field_t *data = NULL;
    bcmfp_group_config_t *group_config = NULL;
    size_t num_fid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(config, SHR_E_PARAM);

    *config = NULL;

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    key_fid = stage->tbls.group_tbl->key_fid;
    tbl_id = stage->tbls.group_tbl->sid;
    num_fid = stage->tbls.group_tbl->lrd_info.num_fid;

    /* Get the FP group LT entry saved in IMM. */
    rv = bcmfp_imm_entry_lookup(unit,
                                tbl_id,
                                &key_fid,
                                &group_id,
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

    /* Get the FP group LT entry config of ACL. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage->stage_id,
                                key,
                                data,
                                &group_config));
    *config = group_config;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_config_free(int unit,
                        bcmfp_group_config_t *group_config)
{

    SHR_FUNC_ENTER(unit);

    if (group_config == NULL) {
        SHR_EXIT();
    }

    if (group_config->presel_ids) {
        SHR_FREE(group_config->presel_ids);
    }
    bcmfp_qual_bitmap_free(unit,group_config->qual_bitmap);
    SHR_FREE(group_config);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_config_compress_check(int unit,
                   bcmfp_stage_t *stage,
                   bcmfp_group_config_t *group_config,
                   bcmfp_tbl_compress_fid_info_t *compress_fid_info,
                   bool *compressible)
{
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    uint8_t num_compress_fids = 0;
    uint8_t idx = 0;
    bool compress_fid_found = FALSE;
    uint8_t num_compress_fids_found = 0;
    uint32_t fid = 0;
    bcmfp_qualifier_t qual_id = 0;
    int sbit = 0;
    int width = 0;
    uint32_t *bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_config, SHR_E_PARAM);
    SHR_NULL_CHECK(compressible, SHR_E_PARAM);

    *compressible = FALSE;
    if (compress_fid_info == NULL ||
        group_config->compress == FALSE ||
        group_config->qual_bitmap == NULL) {
        SHR_EXIT();
    }

    qualifiers_fid_info =
        stage->tbls.group_tbl->qualifiers_fid_info;
    num_compress_fids =
        compress_fid_info->num_fids;
    for (idx = 0; idx < num_compress_fids; idx++) {
        compress_fid_found = FALSE;
        qual_bitmap = group_config->qual_bitmap;
        fid = compress_fid_info->bitmap_fids[idx];
        sbit = compress_fid_info->fid_offsets[idx];
        width = compress_fid_info->fid_widths[idx];
        qual_id = qualifiers_fid_info[fid].qualifier;
        while (qual_bitmap != NULL) {
            bitmap = qual_bitmap->bitmap;
            if (qual_bitmap->qual == qual_id &&
                bcmfp_bits_ones(bitmap, sbit, width)) {
                 compress_fid_found = TRUE;
                 qual_bitmap->compress = TRUE;
                 num_compress_fids_found++;
                 break;
            }
            qual_bitmap = qual_bitmap->next;
        }
        if (compress_fid_found == FALSE) {
            break;
        }
    }
    if ((num_compress_fids_found != num_compress_fids) &&
        (num_compress_fids_found != 0)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (num_compress_fids_found == num_compress_fids) {
        *compressible = TRUE;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_config_compress_qual_remove(int unit,
                            bcmfp_group_config_t *group_config)
{
    bcmfp_qual_bitmap_t *temp_qual_bitmap = NULL;
    bcmfp_qual_bitmap_t *curr_qual_bitmap = NULL;
    bcmfp_qual_bitmap_t *prev_qual_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_config, SHR_E_PARAM);

    curr_qual_bitmap = group_config->qual_bitmap;
    while (curr_qual_bitmap != NULL) {
        if (curr_qual_bitmap->compress == TRUE) {
            if (prev_qual_bitmap != NULL) {
                prev_qual_bitmap->next =
                    curr_qual_bitmap->next;
            } else {
                group_config->qual_bitmap =
                    curr_qual_bitmap->next;
            }
            temp_qual_bitmap = curr_qual_bitmap;
            curr_qual_bitmap = curr_qual_bitmap->next;
            SHR_FREE(temp_qual_bitmap);
            temp_qual_bitmap = NULL;
        } else {
            prev_qual_bitmap = curr_qual_bitmap;
            curr_qual_bitmap = curr_qual_bitmap->next;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_config_cid_qual_add(int unit,
                   bcmfp_stage_t *stage,
                   bcmfp_group_config_t *group_config,
                   bcmfp_tbl_compress_fid_info_t *compress_fid_info)
{
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info = NULL;
    uint8_t idx = 0;
    bool cid_fid_found = FALSE;
    bcmfp_qualifier_t qual_id = 0;
    uint32_t cid_bitmap_fid = 0;
    uint8_t cid_size;
    uint8_t num_cid_words = 0;
    bcmfp_qual_bitmap_t *new_qual_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_config, SHR_E_PARAM);

    if (compress_fid_info == NULL ||
        group_config->compress == FALSE) {
        SHR_EXIT();
    }
    qualifiers_fid_info =
        stage->tbls.group_tbl->qualifiers_fid_info;

    qual_bitmap = group_config->qual_bitmap;
    cid_bitmap_fid = compress_fid_info->cid_bitmap_fid;
    qual_id =
        qualifiers_fid_info[cid_bitmap_fid].qualifier;
    while (qual_bitmap != NULL) {
        if (qual_bitmap->qual == qual_id) {
            cid_fid_found = TRUE;
            break;
        }
        qual_bitmap = qual_bitmap->next;
    }
    if (cid_fid_found == TRUE) {
        SHR_EXIT();
    }

    BCMFP_ALLOC(new_qual_bitmap,
            sizeof(bcmfp_qual_bitmap_t),
            "bcmfpGroupSrcCidBitmap");
    new_qual_bitmap->qual = qual_id;
    new_qual_bitmap->fid = qual_id;
    new_qual_bitmap->fidx = 0;
    cid_size = compress_fid_info->cid_size;
    num_cid_words = ((cid_size / 32) + 1);
    for (idx = 0; idx < num_cid_words; idx++) {
        if (idx == (num_cid_words - 1)) {
            new_qual_bitmap->bitmap[idx] =
                ((1 << (cid_size % 32)) - 1);
        } else {
            new_qual_bitmap->bitmap[idx] = ~0;
        }
    }
    new_qual_bitmap->next = group_config->qual_bitmap;
    group_config->qual_bitmap = new_qual_bitmap;
    new_qual_bitmap = NULL;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_config_compress(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_group_config_t *group_config)
{
    bcmfp_tbl_compress_fid_info_t *compress_fid_info = NULL;
    bcmfp_stage_t *stage = NULL;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_config, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (group_config->compress == FALSE) {
        SHR_EXIT();
    }

    /* Remove all compress eligible qualifiers bitmap. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_compress_qual_remove(unit,
                                                 group_config));

    /* Add CID qialifier BITMAP */
    for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
        if (group_config->compress_types[idx] == FALSE) {
            continue;
        }
        compress_fid_info = stage->tbls.compress_fid_info[idx];
        SHR_IF_ERR_EXIT
            (bcmfp_group_config_cid_qual_add(unit,
                                             stage,
                                             group_config,
                                             compress_fid_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
               QSET PROCESS FUNCTIONS
 */
static int
bcmfp_group_keygen_qual_info_alloc(int unit,
                        bcmfp_group_config_t *group_config,
                        bcmfp_keygen_cfg_t *keygen_cfg)
{
    int qual_id_count = 0;
    uint16_t idx = 0;
    size_t array_size = 0;
    size_t bitmap_size = 0;
    bcmfp_qualifier_t qual_id;
    bcmfp_qual_bitmap_t *qual_bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_config, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    if (group_config->qual_bitmap == NULL) {
        SHR_EXIT();
    }

    qual_bitmap = group_config->qual_bitmap;
    while (qual_bitmap != NULL) {
        qual_id_count++;
        qual_bitmap = qual_bitmap->next;
    }
    array_size = qual_id_count * sizeof(bcmfp_keygen_qual_info_t);
    BCMFP_ALLOC(keygen_cfg->qual_info_arr,
                array_size,
                "bcmfpQualArrayInGroup");
    keygen_cfg->qual_info_count = qual_id_count;
    idx = 0;
    qual_bitmap = group_config->qual_bitmap;
    bitmap_size = sizeof(uint32_t) * BCMFP_MAX_WSIZE;
    while (qual_bitmap != NULL) {
        qual_id = qual_bitmap->qual;
        if (keygen_cfg->qual_cfg_info_db->qual_cfg_info[qual_id] == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        keygen_cfg->qual_info_arr[idx].qual_id = qual_id;
        if (qual_bitmap->partial == TRUE) {
            sal_memcpy(keygen_cfg->qual_info_arr[idx].bitmap,
                       qual_bitmap->bitmap,
                       bitmap_size);
            keygen_cfg->qual_info_arr[idx].partial = TRUE;
        }
        idx++;
        qual_bitmap = qual_bitmap->next;
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(keygen_cfg->qual_info_arr);
        keygen_cfg->qual_info_arr = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_keygen_modes_get(int unit,
                             bcmfp_stage_t *stage,
                             bcmfp_group_mode_t group_mode,
                             bcmfp_keygen_mode_t *kg_mode,
                             bcmfp_keygen_mode_t *kge_mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(kg_mode, SHR_E_PARAM);
    SHR_NULL_CHECK(kge_mode, SHR_E_PARAM);

    if (group_mode == BCMFP_GROUP_MODE_HALF) {
        *kg_mode = BCMFP_KEYGEN_MODE_HALF;
        *kge_mode = BCMFP_KEYGEN_MODE_SINGLE;
    } else if (group_mode == BCMFP_GROUP_MODE_SINGLE) {
        *kg_mode = BCMFP_KEYGEN_MODE_SINGLE;
        *kge_mode = BCMFP_KEYGEN_MODE_SINGLE;
    } else if (group_mode ==
        BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {
        *kg_mode = BCMFP_KEYGEN_MODE_SINGLE_ASET_WIDE;
        *kge_mode = BCMFP_KEYGEN_MODE_SINGLE;
    } else if (group_mode ==
        BCMFP_GROUP_MODE_SINGLE_ASET_NARROW) {
        *kg_mode = BCMFP_KEYGEN_MODE_SINGLE_ASET_NARROW;
        *kge_mode = BCMFP_KEYGEN_MODE_SINGLE;
    } else if (group_mode == BCMFP_GROUP_MODE_DBLINTRA) {
        *kg_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
        *kge_mode = BCMFP_KEYGEN_MODE_DBLINTRA;
    } else if (group_mode == BCMFP_GROUP_MODE_DBLINTER) {
        *kg_mode = BCMFP_KEYGEN_MODE_DBLINTER;
        *kge_mode = BCMFP_KEYGEN_MODE_DBLINTER;
    } else if (group_mode == BCMFP_GROUP_MODE_TRIPLE) {
        *kg_mode = BCMFP_KEYGEN_MODE_TRIPLE;
        *kge_mode = BCMFP_KEYGEN_MODE_TRIPLE;
    } else if (group_mode == BCMFP_GROUP_MODE_QUAD) {
        *kg_mode = BCMFP_KEYGEN_MODE_QUAD;
        *kge_mode = BCMFP_KEYGEN_MODE_QUAD;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_qset_size_get(int unit,
                          bcmfp_keygen_cfg_t *keygen_cfg,
                          uint16_t *rkey_size)
{
    uint16_t idx = 0;
    uint16_t bit = 0;
    uint16_t qual_size = 0;
    uint16_t qual_cfg_max_size = 0;
    bcmfp_qualifier_t qual_id;
    uint32_t *bitmap = NULL;
    bcmfp_keygen_qual_cfg_info_t **qual_cfg_info1 = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_cfg, SHR_E_PARAM);

    qual_cfg_info1 =
         keygen_cfg->qual_cfg_info_db->qual_cfg_info;

    if (qual_cfg_info1 == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *rkey_size = 0;
    for (idx = 0; idx < keygen_cfg->qual_info_count; idx++) {

        qual_id =  keygen_cfg->qual_info_arr[idx].qual_id;

        if (qual_cfg_info1[qual_id] == NULL) {
            continue;
        }
        /*Get the max qualifier size */
        SHR_IF_ERR_EXIT
            (bcmfp_keygen_qual_cfg_max_size_get(unit,
                                        keygen_cfg,
                                        qual_id,
                                        &qual_cfg_max_size));

        if (keygen_cfg->qual_info_arr[idx].partial == TRUE) {
            /* calculate the bitmap size and use it */
            qual_size = 0;
            bitmap = keygen_cfg->qual_info_arr[idx].bitmap;
            for (bit = 0; bit < qual_cfg_max_size; bit++) {
                if (SHR_BITGET(bitmap, bit)) {
                    qual_size++;
                }
            }
        } else {
            qual_size = qual_cfg_max_size;
        }

        (*rkey_size) += qual_size;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_key_size_get(int unit,
                         bcmfp_keygen_oper_t *keygen_oper,
                         uint16_t *key_size)
{
    uint16_t qual_idx = 0, off_idx = 0, ksz = 0;
    bcmfp_keygen_qual_offset_t *offset_arr = NULL;
    bcmfp_keygen_qual_offset_info_t *qual_offset_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_oper, SHR_E_PARAM);

    qual_offset_info = keygen_oper->qual_offset_info;
    SHR_NULL_CHECK(qual_offset_info, SHR_E_PARAM);

    for (qual_idx = 0; qual_idx < qual_offset_info->size; qual_idx++) {
        offset_arr = &qual_offset_info->offset_arr[qual_idx];
        SHR_NULL_CHECK(offset_arr, SHR_E_PARAM);
        for (off_idx = 0; off_idx < offset_arr->num_offsets; off_idx++) {
            if (ksz <= offset_arr->offset[off_idx]) {
                ksz = (offset_arr->offset[off_idx] + offset_arr->width[off_idx]);
            }
        }
    }

    *key_size = ksz;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_aset_size_get(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmfp_stage_id_t stage_id,
                          uint32_t pdd_id,
                          uint16_t *aset_size)
{
    int rv;
    bcmfp_pdd_config_t *pdd_config = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_action_data_t *action_data = NULL;
    uint16_t idx = 0;
    bcmfp_action_t act_id;
    uint32_t container_bmp[BCMFP_MAX_PDD_WORDS] = {0};
    bcmfp_action_offset_cfg_t a_offset;
    bcmfp_action_cfg_t **action_cfg;
    uint16_t *container_sizes = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(aset_size, SHR_E_PARAM);

    if (pdd_id == 0) {
        *aset_size = 0;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    action_cfg = stage->action_cfg_db->action_cfg;

    rv = bcmfp_pdd_remote_config_get(unit,
                                     op_arg->trans_id,
                                     stage_id,
                                     pdd_id,
                                     &pdd_config);
    if (rv == SHR_E_NOT_FOUND || pdd_config == NULL) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "BCMFP: Group doesnt have "
                    "a valid PDD configuration, PDD_ID:%u\n"),
                    pdd_id));
        *aset_size = 0;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    action_data = pdd_config->action_data;

    *aset_size = 0;
    container_sizes = stage->action_cfg_db->container_size;
    while (action_data != NULL) {

        act_id = action_data->action;
        if (action_cfg[act_id] == NULL ||
            action_cfg[act_id]->flags & BCMFP_ACTION_NOT_VALID) {

            /*
             * This is to handle the case the action is not mapped
             * in database. Happens in EM case since group config
             * looks at ASET also
             */
            if (action_cfg[act_id] != NULL) {
                LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit, "BCMFP: Action is not"
                " mapped to physical container.Action: %s\n"),
                action_cfg[act_id]->action_name));
            }
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
        a_offset = action_cfg[act_id]->offset_cfg;

        for (idx = 0; idx < BCMFP_ACTION_PARAM_SZ; idx++) {
            if (a_offset.width[idx] > 0 ) {

                /*
                 * Check the container bitmap
                 * whether the container is already set
                 */
                if (!(0 == SHR_BITGET(container_bmp,
                           a_offset.phy_cont_id[idx]))) {
                    continue;
                } else {

                    /* Set the corrsponding cont bit in bitmap */
                    SHR_BITSET(container_bmp,
                           a_offset.phy_cont_id[idx]);

                    /* Add the container size to aset_size */
                    (*aset_size) +=
                       container_sizes[a_offset.phy_cont_id[idx]];
                }
            }
        }
        action_data = action_data->next;
    }

exit:
    if (pdd_config != NULL) {
        bcmfp_pdd_config_free(unit, pdd_config);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_validate_view(int unit,
                          bcmfp_stage_t *stage,
                          bcmfp_group_mode_t group_mode,
                          uint16_t qset_size)
{

    bcmfp_qset_aset_size_info_t *key_data_sz = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    key_data_sz = stage->key_data_size_info;

    if (key_data_sz == NULL) {
        SHR_EXIT();
    }

    if (qset_size > key_data_sz->key_size[group_mode]) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

exit:
    SHR_FUNC_EXIT();
}
/*
 * This function is used to calculate the
 * max key offset (max key size) that can
 * be accomondated in the entry. This is used
 * for cases where the key and policy are configured
 * in the same entry.
 */
static int
bcmfp_group_key_max_size_get(int unit,
                             bcmfp_stage_t *stage,
                             bcmfp_group_mode_t group_mode,
                             uint16_t aset_size,
                             uint16_t *qset_size)
{
    bcmfp_qset_aset_size_info_t *key_data_sz = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    key_data_sz = stage->key_data_size_info;

    if (key_data_sz == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *qset_size = 0;

    if (aset_size <= key_data_sz->key_data_size[group_mode]) {
        *qset_size = key_data_sz->key_data_size[group_mode]
                    - aset_size;
    }

    if (*qset_size > key_data_sz->key_size[group_mode]) {
        *qset_size = key_data_sz->key_size[group_mode];
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_flex_hash_validate_view(int unit,
                                    bcmfp_stage_t *stage,
                                    bcmfp_group_mode_t group_mode,
                                    uint16_t qset_size,
                                    uint16_t aset_size)
{
    shr_error_t rv = SHR_E_NONE;
    bcmfp_qset_aset_size_info_t *key_data_sz = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    key_data_sz = stage->key_data_size_info;

    if (key_data_sz == NULL) {
        SHR_EXIT();
    }

    if ((qset_size + aset_size) <= key_data_sz->key_data_size[group_mode]) {
        rv = SHR_E_NONE;
    } else {
        rv = SHR_E_CONFIG;
    }

    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_ext_codes_get(int unit,
                          bcmfp_keygen_oper_t *keygen_oper,
                          bcmfp_group_t *grp_cfg)
{
    uint8_t part = 0;
    uint8_t gran = 0;
    bcmfp_keygen_ext_ctrl_sel_info_t *keygen_ext_code = NULL;
    uint8_t level = 0;
    uint8_t ext_num = 0;
    uint16_t ext_code_idx = 0;
    bcmfp_ext_codes_t *grp_ext_code = NULL;
    uint8_t ctrl_sel_val = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_oper, SHR_E_PARAM);

    for (ext_code_idx = 0;
        ext_code_idx < keygen_oper->ext_ctrl_sel_info_count;
        ext_code_idx++) {
        keygen_ext_code = &(keygen_oper->ext_ctrl_sel_info[ext_code_idx]);
        part = keygen_ext_code->part;
        gran = keygen_ext_code->gran;
        level = keygen_ext_code->level;
        ext_num = keygen_ext_code->ext_num;
        ctrl_sel_val = keygen_ext_code->ctrl_sel_val;
        grp_ext_code = &(grp_cfg->ext_codes[part]);
        if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER) {
            if (level == 1) {
                if (gran == 32) {
                    grp_ext_code->l1_e32_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 16) {
                    grp_ext_code->l1_e16_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 8) {
                    grp_ext_code->l1_e8_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 4) {
                    grp_ext_code->l1_e4_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 2) {
                    grp_ext_code->l1_e2_sel[ext_num] = ctrl_sel_val;
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            } else if (level == 2) {
                if (gran == 16) {
                    grp_ext_code->l2_e16_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 4) {
                    grp_ext_code->l2_e4_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 2) {
                    grp_ext_code->l2_e2_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 1) {
                    grp_ext_code->l2_e1_sel[ext_num] = ctrl_sel_val;
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            } else if (level == 3) {
                if (gran == 4) {
                    grp_ext_code->l3_e4_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 2) {
                    grp_ext_code->l3_e2_sel[ext_num] = ctrl_sel_val;
                } else if (gran == 1) {
                    grp_ext_code->l3_e1_sel[ext_num] = ctrl_sel_val;
                } else {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            } else {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_A) {
            grp_ext_code->aux_tag_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_B) {
            grp_ext_code->aux_tag_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_C) {
            grp_ext_code->aux_tag_c_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_AUX_TAG_D) {
            grp_ext_code->aux_tag_d_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_TCP_FN) {
            grp_ext_code->tcp_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_TOS_FN) {
            grp_ext_code->tos_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_TTL_FN) {
            grp_ext_code->ttl_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_A) {
            grp_ext_code->class_id_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_B) {
            grp_ext_code->class_id_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_C) {
            grp_ext_code->class_id_cont_c_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_CLASS_ID_D) {
            grp_ext_code->class_id_cont_d_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_A) {
            grp_ext_code->src_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_B) {
            grp_ext_code->src_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_DST_A) {
            grp_ext_code->dst_cont_a_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_DST_B) {
            grp_ext_code->dst_cont_b_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_0) {
            grp_ext_code->src_dest_cont_0_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_SRC_DST_1) {
            grp_ext_code->src_dest_cont_1_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_IPBM_PRESENT) {
            grp_ext_code->ipbm_present = 1;
            grp_ext_code->ipbm_source = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_L3_L4_NORM) {
            grp_ext_code->normalize_l3_l4_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_MAC_NORM) {
            grp_ext_code->normalize_mac_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_UDF) {
            grp_ext_code->udf_sel = keygen_ext_code->ctrl_sel_val;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_PMUX) {
            grp_ext_code->pmux_sel[ctrl_sel_val] = 1;
        } else if (keygen_ext_code->ctrl_sel == BCMFP_KEYGEN_EXT_CTRL_SEL_ALT_TTL_FN) {
            grp_ext_code->alt_ttl_fn_sel = keygen_ext_code->ctrl_sel_val;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_qual_fid_get(int unit,
                         bcmfp_group_config_t *gc,
                         bcmfp_qualifier_t qual_id,
                         bcmltd_fid_t *fid,
                         uint16_t *fidx)
{
    bcmfp_qual_bitmap_t *bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);
    SHR_NULL_CHECK(fidx, SHR_E_PARAM);

    bitmap =  gc->qual_bitmap;
    while (bitmap != NULL) {
        if (qual_id == bitmap->qual) {
            *fid = bitmap->fid;
            *fidx = bitmap->fidx;
            break;
        }
        bitmap = bitmap->next;
    }

    if (bitmap == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_qual_offset_get(int unit,
                            bcmfp_group_config_t *gc,
                            bcmfp_keygen_oper_t *keygen_oper,
                            bcmfp_group_t *grp_cfg)
{
    uint8_t part = 0;
    uint8_t parts_count = 0;
    uint16_t qual_idx = 0;
    uint16_t offset = 0;
    uint16_t num_offsets = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_qual_offset_info_t *group_qual = NULL;
    bcmfp_keygen_qual_offset_info_t *qual_offset_info = NULL;
    bcmltd_fid_t fid = 0;
    uint16_t fidx = 0;
    bcmfp_qualifier_t qual_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);
    SHR_NULL_CHECK(grp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(keygen_oper, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, grp_cfg->stage_id, &stage));

    SHR_IF_ERR_EXIT
        (bcmfp_group_parts_count(unit, FALSE, grp_cfg->flags, &parts_count));

    for (part = 0; part < parts_count; part++) {
        group_qual = &(grp_cfg->qual_arr[part]);
        qual_offset_info = &(keygen_oper->qual_offset_info[part]);
        group_qual->size = qual_offset_info->size;
        if (qual_offset_info->size == 0) {
            group_qual->qid_arr = NULL;
            group_qual->offset_arr = NULL;
            group_qual->fid_arr = NULL;
            group_qual->fidx_arr = NULL;
            continue;
        }
        BCMFP_ALLOC(group_qual->qid_arr,
                    qual_offset_info->size * sizeof(bcmfp_qualifier_t),
                    "bcmfpGrpQualInfo");
        BCMFP_ALLOC(group_qual->fid_arr,
                    qual_offset_info->size * sizeof(bcmltd_fid_t),
                    "bcmfpGrpQualFidArray");
        BCMFP_ALLOC(group_qual->fidx_arr,
                    qual_offset_info->size * sizeof(uint16_t),
                    "bcmfpGrpQualFidxArray");
        BCMFP_ALLOC(group_qual->offset_arr,
                    qual_offset_info->size * sizeof(bcmfp_qual_offset_info_t),
                    "bcmfpGrpQualOffsetInfo");
        for (qual_idx = 0; qual_idx < qual_offset_info->size; qual_idx++) {
            qual_id = qual_offset_info->qid_arr[qual_idx];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_qual_fid_get(unit,
                                          gc,
                                          qual_id,
                                          &fid,
                                          &fidx));
            group_qual->qid_arr[qual_idx] =
                qual_offset_info->qid_arr[qual_idx];
            group_qual->fid_arr[qual_idx] = fid;
            group_qual->fidx_arr[qual_idx] = fidx;
            fid = 0;
            fidx = 0;
            num_offsets = 0;
            for (offset = 0; offset < BCMFP_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
                 if (!qual_offset_info->offset_arr[qual_idx].width[offset]) {
                     continue;
                 }

                 group_qual->offset_arr[qual_idx].offset[offset] =
                     qual_offset_info->offset_arr[qual_idx].offset[offset];
                 group_qual->offset_arr[qual_idx].width[offset] =
                     qual_offset_info->offset_arr[qual_idx].width[offset];
                 if (grp_cfg->group_mode == BCMFP_GROUP_MODE_HALF &&
                     !(BCMFP_STAGE_FLAGS_TEST(stage,
                       BCMFP_STAGE_MODE_HALF_NOT_SUPPORTED))) {
                     group_qual->offset_arr[qual_idx].offset[offset] -= 80;
                 }
                 num_offsets++;
                 if (num_offsets ==
                     qual_offset_info->offset_arr[qual_idx].num_offsets) {
                     break;
                 }
            }
            group_qual->offset_arr[qual_idx].num_offsets = (offset + 1);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_oper_get(int unit,
                     bcmfp_group_config_t *gc,
                     bcmfp_keygen_oper_t *keygen_oper,
                     bcmfp_group_t *grp_cfg)
{
    uint8_t part = 0;
    uint8_t parts_count = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(gc, SHR_E_PARAM);

    if (keygen_oper == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 grp_cfg->flags,
                                 &parts_count));

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, grp_cfg->stage_id, &stage));

    if (keygen_oper->qual_offset_info) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_qual_offset_get(unit,
                                         gc,
                                         keygen_oper,
                                         grp_cfg));
    }

    for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
        bcmfp_ext_codes_init(unit,
                             &(grp_cfg->ext_codes[part]));
    }

    if (keygen_oper->ext_ctrl_sel_info) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_ext_codes_get(unit,
                                       keygen_oper,
                                       grp_cfg));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_keygen_process(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_group_config_t *gc,
                           bcmfp_group_oper_info_t *opinfo)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_keygen_cfg_t keygen_cfg;
    bcmfp_keygen_oper_t keygen_oper;
    uint8_t num_parts = 0;
    bcmfp_keygen_mode_t kg_mode = BCMFP_KEYGEN_MODE_COUNT;
    bcmfp_keygen_mode_t kge_mode = BCMFP_KEYGEN_MODE_COUNT;
    bcmfp_group_mode_t group_mode = BCMFP_GROUP_MODE_COUNT;
    bcmfp_group_mode_t min_mode = BCMFP_GROUP_MODE_COUNT;
    bcmfp_group_mode_t max_mode = BCMFP_GROUP_MODE_COUNT;
    uint16_t qset_size = 0;
    uint16_t aset_size = 0;
    uint32_t data_width = 0;
    uint16_t key_size = 0;
    uint32_t group_flags = 0;
    bcmfp_part_id_t part_id = 0;
    shr_error_t rv = SHR_E_NONE;
    bool group_created = FALSE;
    bcmfp_group_t *fg = NULL;
    uint16_t qual_max_offset = 0;
    uint8_t part = 0;
    bcmfp_group_qual_offset_info_t *group_qual = NULL;
    uint8_t action_res_id = 0;
    uint8_t pdd_parts = 0;
    bcmfp_group_mode_t pdd_group_mode = BCMFP_GROUP_MODE_NONE;
    bcmfp_pdd_config_t *pdd_config = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&keygen_cfg, 0, sizeof(bcmfp_keygen_cfg_t));

    SHR_NULL_CHECK(gc, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);


    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    keygen_cfg.qual_cfg_info_db = stage->kgn_qual_cfg_info_db;
    if (keygen_cfg.qual_cfg_info_db == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT
        (bcmfp_group_keygen_qual_info_alloc(unit,
                                            gc,
                                            &keygen_cfg));
    if (gc->pdd_id != 0) {
        SHR_IF_ERR_EXIT
            (bcmfp_pdd_remote_config_get(unit,
                                         op_arg->trans_id,
                                         stage_id,
                                         gc->pdd_id,
                                         &pdd_config));

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_pdd_num_parts_get(unit,
                                     stage_id,
                                     gc->pdd_id,
                                     opinfo->pdd_type,
                                     &pdd_parts),
                SHR_E_NOT_FOUND);

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_pdd_group_mode_get(unit,
                                      stage_id,
                                      gc->pdd_id,
                                      opinfo->pdd_type,
                                      &pdd_group_mode),
                SHR_E_NOT_FOUND);
    }

    if (gc->mode == BCMFP_GROUP_MODE_AUTO) {
        min_mode = BCMFP_GROUP_MODE_HALF;
        max_mode = BCMFP_GROUP_MODE_COUNT;
    } else if (gc->mode == BCMFP_GROUP_MODE_SINGLE) {
        min_mode = BCMFP_GROUP_MODE_SINGLE;
        max_mode = (BCMFP_GROUP_MODE_SINGLE_ASET_WIDE + 1);
    } else {
        min_mode = gc->mode;
        max_mode = (gc->mode + 1);
    }
    for (group_mode = min_mode;
         group_mode < max_mode;
         group_mode++) {

        sal_memset(&keygen_oper, 0, sizeof(bcmfp_keygen_oper_t));

        if (group_mode == BCMFP_GROUP_MODE_AUTO) {
            continue;
        }
        /*
         * ASET_NARROW mode is tried before ASET_WIDE mode for
         * both aset and qset process. Keywidth of ASET_NARROW
         * mode is bigger than ASET_WIDE mode. So below check
         * is not needed for ASET_WIDE mode.
         */
        if ((pdd_group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) &&
            (group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW)) {
            continue;
        }

        if (!SHR_BITGET(stage->group_mode_bmp.w, group_mode)) {
            continue;
        }

        group_flags = 0;
        SHR_IF_ERR_EXIT
            (bcmfp_group_keygen_modes_get(unit,
                                          stage,
                                          group_mode,
                                          &kg_mode,
                                          &kge_mode));

        keygen_cfg.ext_cfg_db =
                   stage->kgn_ext_cfg_db_arr[kge_mode];
        if (keygen_cfg.ext_cfg_db == NULL) {
            continue;
        }
        keygen_cfg.mode= kg_mode;

        SHR_IF_ERR_EXIT
            (bcmfp_group_mode_to_flags_get(unit,
                                           stage,
                                           group_mode,
                                           &group_flags));
        SHR_IF_ERR_EXIT
            (bcmfp_group_parts_count(unit,
                                     FALSE,
                                     group_flags,
                                     &num_parts));

        if (pdd_parts > num_parts) {
            rv = SHR_E_CONFIG;
            continue;
        }

        keygen_cfg.num_parts = num_parts;
        if (BCMFP_STAGE_FLAGS_TEST(stage,
            BCMFP_STAGE_ENTRY_TYPE_HASH)) {

            SHR_IF_ERR_EXIT
                (bcmfp_group_qset_size_get(unit,
                                           &keygen_cfg,
                                           &qset_size));
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (bcmfp_pdd_data_width_get(unit,
                                          stage_id,
                                          gc->pdd_id,
                                          part_id,
                                          opinfo->pdd_type,
                                          &data_width),
                 SHR_E_NOT_FOUND);
            aset_size = data_width;

            rv = bcmfp_group_validate_view(unit,
                                           stage,
                                           group_mode,
                                           qset_size);
            if (SHR_FAILURE(rv)) {
                continue;
            }
        }

        if (BCMFP_STAGE_FLAGS_TEST(stage,
                                   BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
            /* the keygen_flag is used by keygen algorithm
             * to ensure that the maximum qualifier offset
             * doesn't exceed the keygen_cfg->max_qual_offset
             * This is applicable for hash stages as the
             * key and policy are part of the same entry in
             * Hardware. This flag is used for restricting
             * the keysize so that the key and policy combined
             * donot exceed the entry width.
             */
            keygen_cfg.flags = keygen_cfg.flags |
                               BCMFP_KEYGEN_CFG_FLAGS_QUAL_MAX_OFFSET;

            SHR_IF_ERR_EXIT
                (bcmfp_group_key_max_size_get(unit,
                                              stage,
                                              group_mode,
                                              aset_size,
                                              &qual_max_offset));
            keygen_cfg.qual_max_offset = qual_max_offset;
        }

        if (keygen_cfg.qual_info_count != 0) {
            rv = bcmfp_keygen_cfg_process(unit,
                                          &keygen_cfg,
                                          &keygen_oper);
        }

        if (rv == SHR_E_CONFIG || rv == SHR_E_RESOURCE) {
            rv = SHR_E_CONFIG;
            continue;
        } else if (SHR_SUCCESS(rv)) {
            if (gc->num_presels == 0) {
                group_flags |=
                    BCMFP_GROUP_WITH_DEFAULT_PRESEL;
            }
            if (BCMFP_STAGE_FLAGS_TEST(stage,
                                       BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
                if (keygen_oper.qual_offset_info) {
                    SHR_IF_ERR_EXIT
                        (bcmfp_group_key_size_get(unit,
                                                  &keygen_oper,
                                                  &key_size));
                    keygen_oper.key_size = key_size;

                    rv = bcmfp_group_flex_hash_validate_view(unit,
                                                             stage,
                                                             group_mode,
                                                             key_size,
                                                             aset_size);
                    if (SHR_FAILURE(rv)) {
                        continue;
                    }
                }
            }
            break;
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    /*
     * Exit if there is any error. Otherwise save the
     * generated operational information to HA space.
     */
    SHR_IF_ERR_EXIT(rv);

    
    group_created = TRUE;
    BCMFP_ALLOC(fg, sizeof(bcmfp_group_t), "bcmfpGroupOperInfo");

    if (gc->pdd_id != 0) {
        group_flags |= BCMFP_GROUP_WITH_PDD_PROFILE;
    }
    if (gc->psg_id != 0) {
        group_flags |= BCMFP_GROUP_WITH_DYNAMIC_PRESEL;
    }
    if (gc->auto_expand != 0) {
        group_flags |= BCMFP_GROUP_WITH_AUTO_EXPAND;
    }
    fg->flags = group_flags;
    fg->ltid = tbl_id;
    fg->group_id = gc->group_id;
    fg->group_prio = gc->priority;
    fg->num_presel_ids = gc->num_presels;
    fg->pipe_id = gc->pipe_id;
    fg->stage_id = stage->stage_id;
    fg->group_mode = group_mode;
    fg->compress = gc->compress;
    fg->key_size = keygen_oper.key_size;
    fg->data_size = aset_size;
    fg->group_slice_type = gc->group_slice_type;
    sal_memcpy(fg->compress_types,
               gc->compress_types,
               (sizeof(bool) * BCMFP_COMPRESS_TYPES_MAX));

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_get(unit, gc, &keygen_oper, fg));

    SHR_IF_ERR_EXIT
        (bcmfp_keygen_oper_free(unit, num_parts, &keygen_oper));

    SHR_IF_ERR_EXIT
        (bcmfp_group_action_res_id_allocate(unit,
                                            fg->pipe_id,
                                            fg->stage_id,
                                            &action_res_id,
                                            fg->group_prio));
    fg->action_res_id = action_res_id;

    for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
        fg->ext_codes[part].ltid = action_res_id;
        fg->ext_codes[part].pdd_prof_index =
                            BCMFP_EXT_SELCODE_DONT_CARE;
        fg->ext_codes[part].sbr_prof_index =
                            BCMFP_EXT_SELCODE_DONT_CARE;
        fg->ext_codes[part].psg_prof_index =
                            BCMFP_EXT_SELCODE_DONT_CARE;
    }

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_set(unit, fg));

    /*
     * If valid PDD id is associated to group but pdd parts
     * is zero means ASET doesnot fit in any of the supported
     * group modes.
     */
    if (gc->pdd_id != 0 && pdd_parts == 0 && pdd_config) {
        opinfo->aset_error = TRUE;
    }
    /*
     * If ASET fits in one of the supported mode but ASET
     * mode is more than qset mode provided in group template.
     */
    if (pdd_parts != 0 && pdd_parts > num_parts && pdd_config) {
        opinfo->aset_error = TRUE;
    }
exit:
    if (fg != NULL && group_created == TRUE) {
        for (part = 0; part < num_parts; part++) {
            group_qual = &(fg->qual_arr[part]);
            if (group_qual->qid_arr != NULL) {
                sal_free(group_qual->qid_arr);
            }
            if (group_qual->fid_arr != NULL) {
                sal_free(group_qual->fid_arr);
            }
            if (group_qual->fidx_arr != NULL) {
                sal_free(group_qual->fidx_arr);
            }
            if (group_qual->offset_arr != NULL) {
                sal_free(group_qual->offset_arr);
            }
        }
        SHR_FREE(fg);
    }
    if (keygen_cfg.qual_info_arr != NULL) {
        SHR_FREE(keygen_cfg.qual_info_arr);
    }
    bcmfp_pdd_config_free(unit, pdd_config);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_selcode_process(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmltd_sid_t tbl_id,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_config_t *gc)
{
    uint8_t part = 0;
    uint8_t num_parts = 0;
    bcmfp_group_t *fg = NULL;
    int group_qual_id_count = 0;
    shr_error_t rv = SHR_E_NONE;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_mode_t group_mode = BCMFP_GROUP_MODE_COUNT;
    bcmfp_group_mode_t min_mode = BCMFP_GROUP_MODE_COUNT;
    bcmfp_group_mode_t max_mode = BCMFP_GROUP_MODE_COUNT;
    bool reset_flag = TRUE;
    bcmfp_group_mode_t original_group_mode = BCMFP_GROUP_MODE_COUNT;
    bcmfp_group_qual_offset_info_t *group_qual = NULL;
    uint16_t qual_idx = 0;
    bcmfp_qualifier_t qual_id = 0;
    bcmltd_fid_t fid = 0;
    uint16_t fidx = 0;
    uint32_t group_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(fg, sizeof(bcmfp_group_t), "bcmfpGroup");

    fg->ltid = tbl_id;
    fg->group_id = gc->group_id;
    fg->group_prio = gc->priority;
    fg->action_res_id = gc->vslice_group;
    fg->num_presel_ids = gc->num_presels;
    fg->pipe_id = gc->pipe_id;
    fg->stage_id = stage->stage_id;
    BCMFP_QSET_COPY(fg->qset, gc->qset);
    fg->qual_bitmap = gc->qual_bitmap;
    fg->group_mode = gc->mode;

    if (BCMFP_STAGE_FLAGS_TEST(stage,
            BCMFP_STAGE_KEY_TYPE_FIXED)) {
        fg->group_slice_mode = gc->slice_mode;
        fg->group_port_pkt_type = gc->type;
        reset_flag = FALSE;
        SHR_IF_ERR_EXIT
            (bcmfp_group_slice_mode_to_group_mode(unit,
                                       fg->group_slice_mode,
                                       &fg->group_mode));
        gc->mode = fg->group_mode;
        for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
            bcmfp_ext_codes_init(unit, &(fg->ext_codes[part]));
        }

        SHR_IF_ERR_EXIT
            (bcmfp_group_combined_selcode_get(unit, stage, fg));

        SHR_IF_ERR_EXIT
            (bcmfp_group_selcode_key_get(unit, stage, fg));
    }

    SHR_BITCOUNT_RANGE(fg->qset.w,
                       group_qual_id_count,
                       0, BCMFP_QUALIFIERS_COUNT);

    if (gc->mode == BCMFP_GROUP_MODE_AUTO){
        min_mode = BCMFP_GROUP_MODE_SINGLE;
        max_mode = BCMFP_GROUP_MODE_COUNT;
    } else {
        min_mode = gc->mode;
        max_mode = (gc->mode + 1);
    }

    for (group_mode = min_mode;
         group_mode < max_mode;
         group_mode++) {
        if (group_mode == BCMFP_GROUP_MODE_AUTO) {
            continue;
        }

        if (!SHR_BITGET(stage->group_mode_bmp.w, group_mode)) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmfp_group_mode_to_flags_get(unit,
                                           stage,
                                           group_mode,
                                           &fg->flags));
        SHR_IF_ERR_EXIT
            (bcmfp_group_parts_count(unit,
                                     FALSE,
                                     fg->flags,
                                     &num_parts));
        original_group_mode = fg->group_mode;
        fg->group_mode = group_mode;

        rv = bcmfp_group_selcode_assign(unit,
                                        fg,
                                        stage,
                                        group_qual_id_count,
                                        num_parts,
                                        reset_flag);
        if (rv == SHR_E_CONFIG || rv == SHR_E_RESOURCE) {
            fg->group_mode = original_group_mode;
            rv = SHR_E_CONFIG;
            continue;
        } else if (SHR_SUCCESS(rv)) {
            fg->group_mode = group_mode;
            break;
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    /*
     * Exit if there is any error. Otherwise save the
     * generated operational information to HA space.
     */
    SHR_IF_ERR_EXIT(rv);

    for (part = 0; part < num_parts; part++) {
        group_qual = &(fg->qual_arr[part]);
        if (group_qual->size) {
            BCMFP_ALLOC(group_qual->fid_arr,
                    (group_qual->size * sizeof(bcmltd_fid_t)),
                    "bcmfpGrpQualFidArray");
            BCMFP_ALLOC(group_qual->fidx_arr,
                    (group_qual->size * sizeof(uint16_t)),
                    "bcmfpGrpQualFidxArray");
        }
        for (qual_idx = 0; qual_idx < group_qual->size; qual_idx++) {
            qual_id = group_qual->qid_arr[qual_idx];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_qual_fid_get(unit,
                                          gc,
                                          qual_id,
                                          &fid,
                                          &fidx));
            group_qual->fid_arr[qual_idx] = fid;
            group_qual->fidx_arr[qual_idx] = fidx;
        }
    }
    if (gc->auto_expand != 0) {
        group_flags |= BCMFP_GROUP_WITH_AUTO_EXPAND;
    }

    fg->flags |= group_flags;

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_set(unit, fg));
exit:
    if (fg != NULL) {
        for (part = 0; part < num_parts; part++) {
            group_qual = &(fg->qual_arr[part]);
            if (group_qual->qid_arr != NULL) {
                sal_free(group_qual->qid_arr);
            }
            if (group_qual->fid_arr != NULL) {
                sal_free(group_qual->fid_arr);
            }
            if (group_qual->fidx_arr != NULL) {
                sal_free(group_qual->fidx_arr);
            }
            if (group_qual->offset_arr != NULL) {
                sal_free(group_qual->offset_arr);
            }
        }
        SHR_FREE(fg);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_qset_process(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t tbl_id,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_config_t *gc,
                         bcmfp_group_oper_info_t *opinfo)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_KEY_TYPE_PRESEL)) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_keygen_process(unit,
                                        op_arg,
                                        tbl_id,
                                        stage_id,
                                        gc,
                                        opinfo));
    } else if (BCMFP_STAGE_FLAGS_TEST(stage,
               BCMFP_STAGE_KEY_TYPE_SELCODE) ||
               BCMFP_STAGE_FLAGS_TEST(stage,
               BCMFP_STAGE_KEY_TYPE_FIXED)) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_selcode_process(unit,
                                         op_arg,
                                         tbl_id,
                                         stage_id,
                                         gc));
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
               ASET PROCESS FUNCTIONS
 */
static int
bcmfp_group_aset_process(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t tbl_id,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_config_t *gc,
                         bcmfp_group_oper_info_t *opinfo,
                         bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_pdd_config_t *pdd_config = NULL;
    bcmfp_pdd_oper_type_t pdd_type = BCMFP_PDD_OPER_TYPE_NONE;
    bool oper_status = FALSE;
    bcmltd_sid_t pdd_tbl_id = BCMLTD_SID_INVALID;
    bcmfp_pdd_attrs_t pdd_attrs;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    sal_memset(&pdd_attrs, 0, sizeof(bcmfp_pdd_attrs_t));
    if (gc->pdd_id != 0) {
        pdd_attrs.pdd_per_group = TRUE;
    }

    if (stage->tbls.sbr_tbl == NULL) {
        pdd_attrs.sbr_not_supported = TRUE;
    } else {
        if (gc->sbr_id != 0) {
            pdd_attrs.sbr_per_group = TRUE;
        }
    }

    /* Capture the groups PDD operational type. */
    SHR_IF_ERR_EXIT
        (bcmfp_pdd_oper_type_get(unit, &pdd_attrs, &pdd_type));

    opinfo->pdd_type = pdd_type;

    /* If PDD is not per group do nothing here. */
    if (pdd_attrs.pdd_per_group == FALSE) {
        SHR_EXIT();
    }

    /*
     * If PDD operational info is already present
     * (created for some other group), then use it.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_pdd_oper_status_get(unit,
                                   stage_id,
                                   gc->pdd_id,
                                   pdd_type,
                                   &oper_status));
    if (oper_status == TRUE) {
        SHR_EXIT();
    }

    /* Create PDD operational information. */
    if (idp_info != NULL &&
        idp_info->sid_type == BCMFP_SID_TYPE_PDD_TEMPLATE) {
        pdd_tbl_id = stage->tbls.pdd_tbl->sid;
        SHR_IF_ERR_EXIT
            (bcmfp_pdd_config_get(unit,
                                  op_arg->trans_id,
                                  pdd_tbl_id,
                                  stage_id,
                                  idp_info->key,
                                  idp_info->data,
                                  &pdd_config));
    } else {
        SHR_IF_ERR_EXIT
            (bcmfp_pdd_remote_config_get(unit,
                                         op_arg->trans_id,
                                         stage_id,
                                         gc->pdd_id,
                                         &pdd_config));
        /* PDD template ID is associated but not created. */
        if (pdd_config == NULL) {
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT
        (bcmfp_pdd_config_process(unit,
                                  op_arg,
                                  tbl_id,
                                  stage_id,
                                  pdd_config,
                                  gc->pdd_id,
                                  &pdd_attrs,
                                  0));
exit:
    bcmfp_pdd_config_free(unit, pdd_config);
    SHR_FUNC_EXIT();
}
/*******************************************************************************
               DEFAULT POLICY PROCESS FUNCTIONS
 */
static int
bcmfp_group_daset_process(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_config_t *gc,
                          bcmfp_group_oper_info_t *opinfo)
{
    bcmfp_pdd_id_t pdd_id = 0;
    uint8_t pdd_parts = 0;
    uint16_t aset_size = 0;
    uint16_t default_policy_data_width = 0;
    bcmfp_stage_t *stage = NULL;
    bool pdd_oper_status = FALSE;
    bcmfp_pdd_cfg_t *pdd_cfg_info = NULL;
    bcmfp_pdd_oper_t *pdd_oper_info = NULL;
    uint8_t valid_pdd_data = FALSE;
    bcmfp_pdd_config_t *pdd_config = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (gc->default_pdd_id == 0) {
        SHR_EXIT();
    }

    pdd_id = gc->default_pdd_id;

    pdd_parts = 0;
    SHR_IF_ERR_EXIT
        (bcmfp_group_aset_size_get(unit,
                                   op_arg,
                                   stage_id,
                                   pdd_id,
                                   &aset_size));

    default_policy_data_width =
        stage->misc_info->default_policy_data_width;
    if (aset_size > default_policy_data_width) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit, "BCMFP: Group's default "
                         "policy width exceeds.pdd_id:%u"
                         "current default policy width:%u,"
                         "Max default policy width::%u\n"),
                         pdd_id, aset_size,
                         default_policy_data_width));
       SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /*
     * If this PDD is already used for default policy of some other
     * group, use it.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_pdd_oper_status_get(unit,
                                   stage_id,
                                   pdd_id,
                                   BCMFP_PDD_OPER_TYPE_DEFAULT_POLICY,
                                   &pdd_oper_status));
    if (pdd_oper_status == TRUE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmfp_pdd_remote_config_get(unit,
                                     op_arg->trans_id,
                                     stage_id,
                                     pdd_id,
                                     &pdd_config));
    /* PDD template ID is associated but not created. */
    if (pdd_config == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmfp_pdd_config_info_set(unit,
                                   op_arg,
                                   stage_id,
                                   pdd_id,
                                   gc->mode,
                                   TRUE,
                                   TRUE,
                                   TRUE,
                                   pdd_config,
                                   &valid_pdd_data,
                                   &pdd_cfg_info));
    if (valid_pdd_data == FALSE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmfp_pdd_cfg_process(unit,
                               pdd_cfg_info,
                               &pdd_oper_info,
                               &pdd_parts));
    /*
     * Update the PDD info i.e, container bitmap, section
     * offsets and action offsets in pdd oper info
     */
    SHR_IF_ERR_EXIT
        (bcmfp_pdd_oper_info_set(unit,
                                 stage_id,
                                 pdd_id,
                                 BCMFP_PDD_OPER_TYPE_DEFAULT_POLICY,
                                 pdd_parts,
                                 gc->mode,
                                 default_policy_data_width,
                                 pdd_oper_info));

exit:
    bcmfp_pdd_config_free(unit, pdd_config);
    bcmfp_pdd_config_info_free(unit, pdd_cfg_info);
    bcmfp_pdd_oper_info_free(unit, pdd_parts, pdd_oper_info);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
               GROUP CONFIG VALIDATE FUNCTIONS
 */
static int
bcmfp_group_compress_config_validate(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_config_t *group_config,
                            bcmfp_group_oper_info_t *opinfo)
{
    bcmfp_tbl_compress_fid_info_t *compress_fid_info = NULL;
    bcmfp_stage_t *stage = NULL;
    uint8_t idx = 0;
    bool compressible = TRUE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_config, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (group_config->compress == FALSE) {
        SHR_EXIT();
    }

    /* Check which compression types are expected to be applied. */
    for (idx = 0; idx < BCMFP_COMPRESS_TYPES_MAX; idx++) {
        if (group_config->compress_types[idx] == FALSE) {
            continue;
        }
        compress_fid_info = stage->tbls.compress_fid_info[idx];
        compressible = FALSE;
        SHR_IF_ERR_EXIT
            (bcmfp_group_config_compress_check(unit,
                                               stage,
                                               group_config,
                                               compress_fid_info,
                                               &compressible));
        if (compressible == FALSE) {
            opinfo->state = BCMFP_GROUP_STATE_NON_COMPRESSIBLE;
            SHR_EXIT();
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_prio_validate(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            uint32_t template_id,
                            void *user_data)
{
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    if (opinfo->group_prio == idp_info->group_prio &&
        group_id != idp_info->group_id) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_pse_config_validate(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmfp_stage_id_t stage_id,
                                bcmfp_group_config_t *gc,
                                bcmfp_group_oper_info_t *opinfo)
{
    bcmfp_idp_info_t group_idp_info;
    bcmfp_pse_id_t pse_id = 0;
    bool not_mapped = FALSE;
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    /* Map presel entries associated to group. */
    for (idx = 0; idx < gc->num_presels; idx++) {
        if (gc->presel_ids[idx] == 0) {
            continue;
        }

        pse_id = gc->presel_ids[idx];

       /* Process group associated to this pse. */
        SHR_IF_ERR_EXIT
            (bcmfp_pse_group_map_check(unit,
                                       stage_id,
                                       pse_id,
                                       &not_mapped));

        /* Return if no groups are associated to the pse */
        if (not_mapped == TRUE) {
            continue;
        }

        sal_memset(&group_idp_info, 0, sizeof(bcmfp_idp_info_t));
        group_idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
        group_idp_info.sid_type = BCMFP_SID_TYPE_PSE_TEMPLATE;
        group_idp_info.event_reason = BCMIMM_ENTRY_INSERT;
        group_idp_info.group_prio = gc->priority;
        group_idp_info.group_id = gc->group_id;
        SHR_IF_ERR_EXIT
            (bcmfp_pse_group_map_traverse(unit,
                                          stage_id,
                                          pse_id,
                                          bcmfp_group_prio_validate,
                                          (void *)(&group_idp_info)));

    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_config_validate(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmltd_sid_t tbl_id,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_config_t *gc,
                            bcmfp_group_oper_info_t *opinfo)
{
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    if (gc->mode == BCMFP_GROUP_MODE_NONE) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Validate group pse configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_pse_config_validate(unit,
                                         op_arg,
                                         stage_id,
                                         gc,
                                         opinfo));

    /* Validate the group compression configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_compress_config_validate(unit,
                                              op_arg,
                                              stage_id,
                                              gc,
                                              opinfo));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
               GROUP CONFIG PROCESS FUNCTIONS
 */
static int
bcmfp_group_config_process(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_t *stage,
                           bcmfp_group_config_t *gc,
                           bcmfp_group_oper_info_t *opinfo,
                           bcmfp_idp_info_t *idp_info)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(gc, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    /*
     * Process the ASET. Create offsets and widths
     * for the given groups ASET and save it in HA
     * space.
     */
    rv = bcmfp_group_aset_process(unit,
                                  op_arg,
                                  tbl_id,
                                  stage->stage_id,
                                  gc,
                                  opinfo,
                                  idp_info);
    if (rv == SHR_E_CONFIG) {
        opinfo->aset_error = TRUE;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    /*
     * Process the QSET. Create offsets and widths
     * for the given groups QSET and save it in HA
     * space.
     */
    rv = bcmfp_group_qset_process(unit,
                                  op_arg,
                                  tbl_id,
                                  stage->stage_id,
                                  gc,
                                  opinfo);
    if (rv == SHR_E_CONFIG) {
        opinfo->aset_error = TRUE;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    /*
     * Process the default policy. Create offsets and
     * widths for the given groups ASET and save it in
     * HA space.
     */
    rv = bcmfp_group_daset_process(unit,
                                   op_arg,
                                   tbl_id,
                                   stage->stage_id,
                                   gc,
                                   opinfo);
    if (rv == SHR_E_CONFIG) {
        opinfo->default_aset_error = TRUE;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);

    opinfo->valid = TRUE;
    opinfo->enable = gc->enable;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_ro_fields_add(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmltd_fields_t *ro_fields)
{
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_fid_t oper_mode_fid;
    bcmltd_fid_t oper_ltid_fid;

    SHR_FUNC_ENTER(unit);

    if (ro_fields == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    if (opinfo->valid) {
        oper_mode_fid = stage->tbls.group_tbl->mode_oper_fid;
        if (oper_mode_fid != 0) {
            bcmfp_group_mode_t group_mode = opinfo->group_mode;
            uint32_t oper_mode = 0;
            switch (group_mode) {
            case BCMFP_GROUP_MODE_HALF:
                    oper_mode = 6;
                    break;
                case BCMFP_GROUP_MODE_SINGLE:
                case BCMFP_GROUP_MODE_SINGLE_ASET_NARROW:
                case BCMFP_GROUP_MODE_SINGLE_ASET_WIDE:
                    oper_mode = 1;
                    break;
                case BCMFP_GROUP_MODE_DBLINTRA:
                    oper_mode = 2;
                    break;
                case BCMFP_GROUP_MODE_DBLINTER:
                    oper_mode = 3;
                    break;
                case BCMFP_GROUP_MODE_TRIPLE:
                    oper_mode = 4;
                    break;
                case BCMFP_GROUP_MODE_QUAD:
                    oper_mode = 5;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            SHR_IF_ERR_EXIT
                (bcmfp_ltd_field_add(unit,
                                     oper_mode_fid,
                                     oper_mode,
                                     ro_fields));
        }

        oper_ltid_fid = stage->tbls.group_tbl->group_ltid_oper_fid;
        if (oper_ltid_fid != 0) {
            uint16_t action_res_id = opinfo->action_res_id;
            SHR_IF_ERR_EXIT
                (bcmfp_ltd_field_add(unit,
                                     oper_ltid_fid,
                                     action_res_id,
                                     ro_fields));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_maps_add(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_group_config_t *gc)
{
    uint16_t idx = 0;
    bcmfp_group_id_t group_id = 0;
    bcmfp_pse_id_t pse_id = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);

    group_id = gc->group_id;

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /* Map presel entries associated to group. */
    for (idx = 0; idx < gc->num_presels; idx++) {
        if (gc->presel_ids[idx] == 0) {
            continue;
        }
        pse_id = gc->presel_ids[idx];
        SHR_IF_ERR_EXIT
            (bcmfp_pse_group_map_add(unit,
                                     stage_id,
                                     pse_id,
                                     group_id));
    }

   /* Map presel group associated to group. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_PRESEL_KEY_DYNAMIC) &&
        (gc->psg_id != 0)) {
        SHR_IF_ERR_EXIT
            (bcmfp_psg_group_map_add(unit,
                                     stage_id,
                                     gc->psg_id,
                                     group_id));
    }

    /* Map PDD associated to group. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_POLICY_TYPE_PDD) &&
        (gc->pdd_id != 0)) {
        SHR_IF_ERR_EXIT
            (bcmfp_pdd_group_map_add(unit,
                                     stage_id,
                                     gc->pdd_id,
                                     group_id));
    }

    /* Map SBR associated to group. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ACTION_RESOLUTION_SBR) &&
        (gc->sbr_id != 0)) {
        SHR_IF_ERR_EXIT
            (bcmfp_sbr_group_map_add(unit,
                                     stage_id,
                                     gc->sbr_id,
                                     group_id));
    }

    /* Map default PDD associated to group. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_POLICY_TYPE_PDD) &&
        BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_DEFAULT_POLICY) &&
        (gc->default_pdd_id != 0)) {
        SHR_IF_ERR_EXIT
            (bcmfp_dpdd_group_map_add(unit,
                                      stage_id,
                                      gc->default_pdd_id,
                                      group_id));
    }

    /* Map default Policy associated to group. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_DEFAULT_POLICY) &&
        (gc->default_policy_id != 0)) {
        SHR_IF_ERR_EXIT
            (bcmfp_dpolicy_group_map_add(unit,
                                         stage_id,
                                         gc->default_policy_id,
                                         group_id));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_maps_delete(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_group_config_t *gc)
{
    uint16_t idx = 0;
    bcmfp_group_id_t group_id = 0;
    bcmfp_pse_id_t pse_id = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    group_id = gc->group_id;

    /* Map presel entries associated to group. */
    for (idx = 0; idx < gc->num_presels; idx++) {
        if (gc->presel_ids[idx] == 0) {
            continue;
        }
        pse_id = gc->presel_ids[idx];
        SHR_IF_ERR_EXIT
            (bcmfp_pse_group_map_delete(unit,
                                        stage_id,
                                        pse_id,
                                        group_id));
    }

    /* Map presel group associated to group. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_PRESEL_KEY_DYNAMIC) &&
        (gc->psg_id != 0)) {
        SHR_IF_ERR_EXIT
            (bcmfp_psg_group_map_delete(unit,
                                     stage_id,
                                     gc->psg_id,
                                     group_id));
    }

    /* Map PDD associated to group. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_POLICY_TYPE_PDD) &&
        (gc->pdd_id != 0)) {
        SHR_IF_ERR_EXIT
            (bcmfp_pdd_group_map_delete(unit,
                                     stage_id,
                                     gc->pdd_id,
                                     group_id));
    }

    /* Map SBR associated to group. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_ACTION_RESOLUTION_SBR) &&
        (gc->sbr_id != 0)) {
        SHR_IF_ERR_EXIT
            (bcmfp_sbr_group_map_delete(unit,
                                     stage_id,
                                     gc->sbr_id,
                                     group_id));
    }

    /* Map default PDD associated to group. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_POLICY_TYPE_PDD) &&
        BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_DEFAULT_POLICY) &&
        (gc->default_pdd_id != 0)) {
        SHR_IF_ERR_EXIT
            (bcmfp_dpdd_group_map_delete(unit,
                                         stage_id,
                                         gc->default_pdd_id,
                                         group_id));
    }

    /* Map default Policy associated to group. */
    if (BCMFP_STAGE_FLAGS_TEST(stage,
        BCMFP_STAGE_DEFAULT_POLICY) &&
        (gc->default_policy_id != 0)) {
        SHR_IF_ERR_EXIT
            (bcmfp_dpolicy_group_map_delete(unit,
                                            stage_id,
                                            gc->default_policy_id,
                                            group_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
               GROUP INTER DEPENDENCY FUNCTIONS
 */
static int
bcmfp_group_idp_pse_insert(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_entry_id_t entry_id,
                           bcmfp_group_id_t group_id,
                           void *user_data)
{
    int rv;
    bcmfp_idp_info_t *idp_info = user_data;
    bcmfp_filter_t *filter = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_pse_config_t *pse_config = NULL;
    uint16_t num_presels = 0;
    uint16_t idx = 0;
    bcmfp_pse_id_t pse_id = 0;
    size_t size = 0;
    bcmltd_sid_t tbl_id;
    bcmltd_sid_t pse_tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    pse_id = idp_info->template_id;
    tbl_id = stage->tbls.entry_tbl->sid;
    pse_tbl_id = stage->tbls.pse_tbl->sid;
    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * If group is not operational means no entries in the
     * group can be inserted.
     */
    group_id = entry_config->group_id;
    rv = bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);
    filter->group_oper_info = opinfo;

    /*
     * Not the first entry in the group So just insert the
     * filter without presels.
     */
    if (opinfo->ref_count != 0) {
        /* Insert the filter corresponding to entry ID. */
        SHR_IF_ERR_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Update the operational state of the entry ID. */
        tbl_id = stage->tbls.entry_tbl->sid;
        key_fid = stage->tbls.entry_tbl->key_fid;
        data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
        entry_state = filter->entry_state;
        SHR_IF_ERR_EXIT
            (bcmfp_imm_entry_update(unit,
                                    tbl_id,
                                    &key_fid,
                                    &entry_id,
                                    1,
                                    &data_fid,
                                    &entry_state,
                                    1));
        if (entry_state == BCMFP_ENTRY_SUCCESS) {
            opinfo->ref_count++;
        }
        SHR_EXIT();
    }

    /*
     * Fetch the group operational information and presel
     * entries operational information. Remaining configs
     * will be created in bcmfp_filter_create.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));
    filter->group_config = group_config;

    num_presels = group_config->num_presels;
    size = (sizeof(bcmfp_pse_config_t *) * num_presels);
    BCMFP_ALLOC(filter->pse_config,
                size,
                "bcmfpFilterPreselEntryConfigArray");
    for (idx = 0; idx < num_presels; idx++) {
        if (group_config->presel_ids[idx] == 0) {
            continue;
        }
        if (group_config->presel_ids[idx] == pse_id) {
             /*
              * Should use incoming key and data in callback
              * this presel entry is not yet present in IMM>
              */
             SHR_IF_ERR_EXIT
                 (bcmfp_pse_config_get(unit,
                                       idp_info->op_arg,
                                       pse_tbl_id,
                                       stage_id,
                                       idp_info->key,
                                       idp_info->data,
                                       &pse_config));
        } else {
             SHR_IF_ERR_EXIT
                 (bcmfp_pse_remote_config_get(unit,
                                              idp_info->op_arg,
                                              stage_id,
                                              group_config->presel_ids[idx],
                                              &pse_config));
        }
        filter->pse_config[idx] = pse_config;
        filter->num_pse_configs++;
        pse_config = NULL;
    }

    /* Create the filter corresponding to entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    /* Update the operational state of the entry id. */
    tbl_id = stage->tbls.entry_tbl->sid;
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
    if (entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_pse_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_entry_id_t entry_id,
                           bcmfp_group_id_t group_id,
                           void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;
    bcmfp_filter_t *filter = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_pse_id_t pse_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    pse_id = idp_info->template_id;
    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * If entry ID is not operational then this filter
     * was not created. So no need to do anything.
     */
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        bcmfp_pse_id_t last_pse_id = 0;
        uint16_t num_presels = 0;
        uint16_t idx = 0;
        /* Get the group configuration for the group ID. */
        SHR_IF_ERR_EXIT
            (bcmfp_group_remote_config_get(unit,
                                           idp_info->op_arg,
                                           stage_id,
                                           group_id,
                                           &group_config));
        filter->group_config = group_config;
        last_pse_id = group_config->presel_ids[0];
        num_presels = group_config->num_presels;
        for (idx = 0; idx < num_presels; idx++) {
            if (group_config->presel_ids[idx] == pse_id) {
               group_config->presel_ids[num_presels -1] = pse_id;
               group_config->presel_ids[idx] = last_pse_id;
               group_config->num_presels--;
               break;
            }
        }
        /* Insert the filter corresponding to entry ID. */
        tbl_id = stage->tbls.entry_tbl->sid;
        SHR_IF_ERR_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
            opinfo->ref_count++;
        }
    } else {
        /* Delete the filter corresponding to entry ID. */
        tbl_id = stage->tbls.entry_tbl->sid;
        SHR_IF_ERR_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
            opinfo->ref_count--;
        }
    }
    /* Update the operational state of the entry id. */
    tbl_id = stage->tbls.entry_tbl->sid;
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = BCMFP_ENTRY_PRESEL_NOT_CREATED;
    SHR_IF_ERR_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_pse_update(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_group_id_t group_id,
                           void *user_data)
{
    int rv;
    bcmfp_group_oper_info_t *opinfo = NULL;
    uint32_t psg_id = 0;
    bcmltd_sid_t pse_tbl_id;
    bcmfp_filter_t *filter = NULL;
    bcmfp_idp_info_t *idp_info = user_data;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_pse_config_t *pse_config = NULL;
    bcmfp_stage_t *stage = NULL;
    size_t size = 0;
    shr_error_t rv_except = SHR_E_NOT_FOUND;
    bcmfp_entry_id_traverse_cb cb_func;
    bcmfp_psg_config_t *psg_config = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(rv);
    if ((opinfo == NULL) || (opinfo->ref_count == 0)) {
        /* No entry installed in the group. */
        SHR_EXIT();
    }

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");
    filter->group_oper_info = opinfo;

    /* Create the buffers required for ekw, wdw and profiles. */
    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    pse_tbl_id = stage->tbls.pse_tbl->sid;

    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));
    filter->group_config = group_config;

    size = sizeof(bcmfp_pse_config_t *);
    BCMFP_ALLOC(filter->pse_config,
            size,
            "bcmfpFilterPreselEntryConfigArray");

    SHR_IF_ERR_EXIT
        (bcmfp_pse_config_get(unit,
                              idp_info->op_arg,
                              pse_tbl_id,
                              stage_id,
                              idp_info->key,
                              idp_info->data,
                              &pse_config));
    filter->pse_config[0] = pse_config;
    filter->num_pse_configs = 1;

    /* Get the PSG LT entry configuration of the filter. */
    psg_id = filter->group_config->psg_id;
    if (filter->psg_config == NULL && psg_id != 0) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_psg_remote_config_get(unit,
                                         idp_info->op_arg,
                                         stage_id,
                                         psg_id,
                                         &psg_config),
                                         rv_except);
        filter->psg_config = psg_config;
    }

    SHR_IF_ERR_EXIT
        (bcmfp_filter_presel_update(unit,
                                    idp_info->op_arg,
                                    stage->stage_id,
                                    filter));

    /* Traverse through all entries in the group to update the oper state. */
    idp_info->entry_state = filter->entry_state;

    cb_func  = bcmfp_entry_idp_pse_process;
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_traverse(unit,
                                        stage_id,
                                        group_id,
                                        cb_func,
                                        user_data));



exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_pse_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_idp_info_t *idp_info)
{
    bcmimm_entry_event_t event_reason;
    bcmfp_entry_id_traverse_cb cb_func;
    void *user_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    event_reason = idp_info->event_reason;
    user_data = (void *)idp_info;
    if (event_reason == BCMIMM_ENTRY_INSERT) {
        cb_func = bcmfp_group_idp_pse_insert;
    } else if (event_reason == BCMIMM_ENTRY_DELETE) {
        cb_func = bcmfp_group_idp_pse_delete;
    } else if (event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_idp_pse_update(unit,
                                        stage_id,
                                        group_id,
                                        user_data));
    } else {
         SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (event_reason != BCMIMM_ENTRY_UPDATE)  {
        SHR_IF_ERR_EXIT
            (bcmfp_group_entry_map_traverse(unit,
                                            stage_id,
                                            group_id,
                                            cb_func,
                                            user_data));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_config_delete_idp(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t tbl_id,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_group_id_t group_id,
                              bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_entry_id_traverse_cb cb_func;
    bcmfp_idp_info_t gidp_info;
    void *user_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_config_dump(unit,
                                     tbl_id,
                                     stage_id,
                                     group_config));
    }

    /*
     * Delete the maping between group and remote keys
     * associated to group like presel entries, pdd,
     * sbr, presel group, default pdd, default policy.
 */
    SHR_IF_ERR_EXIT
        (bcmfp_group_maps_delete(unit, stage_id, group_config));

    /* Remove the entries from the group if present. */
    sal_memset(&gidp_info, 0, sizeof(bcmfp_idp_info_t));
    gidp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    gidp_info.sid_type = BCMFP_SID_TYPE_GRP_TEMPLATE;
    gidp_info.event_reason = BCMIMM_ENTRY_DELETE;
    user_data = (void *)(&gidp_info);
    cb_func = bcmfp_entry_idp_group_process;
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_traverse(unit,
                                        stage_id,
                                        group_id,
                                        cb_func,
                                        user_data));

    /* Delete the group operational information from HA. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_delete(unit,
                                      stage_id,
                                      group_id));

exit:
    bcmfp_group_config_free(unit, group_config);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_config_insert_idp(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t tbl_id,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_group_id_t group_id,
                              bcmfp_idp_info_t *idp_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_config_dump(unit,
                                     tbl_id,
                                     stage_id,
                                     group_config));
    }

    /*
     * Update the maping between group and remote keys
     * associated to group like presel entries, pdd,
     * sbr, presel group, default pdd, default policy.
 */
    SHR_IF_ERR_EXIT
        (bcmfp_group_maps_add(unit, stage_id, group_config));

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_create(unit,
                                      stage_id,
                                      group_id,
                                      &opinfo));
    opinfo->valid = FALSE;
    opinfo->state = BCMFP_GROUP_STATE_SUCCESS;

    /* Validate the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_validate(unit,
                                     op_arg,
                                     tbl_id,
                                     stage->stage_id,
                                     group_config,
                                     opinfo));

    /* Compress the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_compress(unit,
                                     op_arg,
                                     stage_id,
                                     group_id,
                                     group_config));

    /*
     * Process the group configuration. It involves
     * allocating offsets and widths to all of the
     * qualifiers and actions in the group.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_process(unit,
                                    op_arg,
                                    tbl_id,
                                    stage,
                                    group_config,
                                    opinfo,
                                    idp_info));

exit:
    bcmfp_group_config_free(unit, group_config);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_sbr_insert(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_group_id_t group_id,
                             void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_sbr_config_t *sbr_config = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    tbl_id = stage->tbls.entry_tbl->sid;

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));

    filter->group_config = group_config;

    tbl_id = stage->tbls.sbr_tbl->sid;
    /* Get the SBR configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_sbr_config_get(unit,
                              idp_info->op_arg,
                              tbl_id,
                              stage_id,
                              idp_info->key,
                              idp_info->data,
                              &sbr_config));
    filter->sbr_config = sbr_config;

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    tbl_id = stage->tbls.entry_tbl->sid;

    /* Create the filter corresponding to entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    /* Update the operational state of the entry id. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_sbr_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_group_id_t group_id,
                             void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /*
     * If entry ID is not operational then this filter
     * was not created. Now create to reflect the right
     * entry operatioal state.
     */
    tbl_id = stage->tbls.entry_tbl->sid;
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        filter->event_reason = BCMIMM_ENTRY_DELETE;
        filter->sid_type = BCMFP_SID_TYPE_SBR_TEMPLATE;
        /* Create the filter corresponding to entry ID. */
        SHR_IF_ERR_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;
        /* Update the entry state. */
        entry_state = BCMFP_ENTRY_SBR_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}


static int
bcmfp_group_idp_sbr_update_hw(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        void *user_data)
{
    bcmfp_idp_info_t *idp_info = user_data;
    bcmfp_filter_t *filter = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmltd_sid_t tbl_id;
    bcmfp_pdd_config_t *pdd_config = NULL;
    bcmfp_sbr_config_t *sbr_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;
    uint8_t num_parts = 0;

    SHR_FUNC_ENTER(unit);

     SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    /*
     * If there are no entries installed in the group
     * nothing need to be done, When the first entry
     * in the group is installed SBR profiles will be
     * installed.
     */
    if (opinfo->ref_count == 0) {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    user_data = (void *)idp_info;
    tbl_id = stage->tbls.sbr_tbl->sid;
    BCMFP_ALLOC(filter,
                sizeof(bcmfp_filter_t),
                "bcmfpSbrUpdateFilter");

    /* Create the buffers required for ekw, edw and profiles. */
    SHR_IF_ERR_EXIT
        (bcmfp_filter_buffers_allocate(unit,
                                       BCMFP_BUFFERS_CREATE_SBR,
                                       tbl_id,
                                       stage,
                                       &(filter->buffers)));

    filter->group_oper_info = opinfo;

    SHR_IF_ERR_EXIT
        (bcmfp_sbr_config_get(unit,
                              idp_info->op_arg,
                              tbl_id,
                              stage_id,
                              idp_info->key,
                              idp_info->data,
                              &sbr_config));
    filter->sbr_config = sbr_config;

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));
    filter->group_config = group_config;

    SHR_IF_ERR_EXIT
    (bcmfp_pdd_remote_config_get(unit,
                                 idp_info->op_arg->trans_id,
                                 stage_id,
                                 filter->group_config->pdd_id,
                                 &pdd_config));
    filter->pdd_config = pdd_config;

    SHR_IF_ERR_EXIT
        (bcmfp_filter_sbr_config_update_process(unit,
                                         idp_info->op_arg,
                                         tbl_id,
                                         FALSE,
                                         stage_id,
                                         filter));

    SHR_IF_ERR_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &num_parts));
    SHR_IF_ERR_EXIT
        (bcmfp_sbr_profile_update(unit,
                                  idp_info->op_arg->trans_id,
                                  opinfo->tbl_inst,
                                  stage->tbls.sbr_tbl->sid,
                                  stage_id,
                                  filter->buffers.sbr,
                                  num_parts,
                                  opinfo->sbr_id));
exit:
    if (filter != NULL) {
        bcmfp_filter_configs_free(unit, filter);
        bcmfp_filter_buffers_free(unit, &(filter->buffers));
        SHR_FREE(filter);
    }
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_psg_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_entry_id_t entry_id,
                           bcmfp_group_id_t group_id,
                           void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

     /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;
    /*
     * If entry ID is not operational then this filter
     * was not created. Now create to reflect the right
     * entry operatioal state.
     */
    tbl_id = stage->tbls.entry_tbl->sid;
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        filter->event_reason = BCMIMM_ENTRY_DELETE;
        filter->sid_type = BCMFP_SID_TYPE_PSG_TEMPLATE;
        /* Create the filter corresponding to entry ID. */
        SHR_IF_ERR_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;
        /* Update the entry state. */
        entry_state = BCMFP_ENTRY_PSG_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();

}

static int
bcmfp_group_idp_psg_insert(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_entry_id_t entry_id,
                           bcmfp_group_id_t group_id,
                           void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_psg_config_t *psg_config = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));

    filter->group_config = group_config;

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    tbl_id = stage->tbls.psg_tbl->sid;
    /* Get the PDD configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_psg_config_get(unit,
                              idp_info->op_arg,
                              tbl_id,
                              stage_id,
                              idp_info->key,
                              idp_info->data,
                              &psg_config));
    filter->psg_config = psg_config;

    /* Create the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    /* Update the operational state of the entry id. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_pdd_insert(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_group_id_t group_id,
                             void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_pdd_config_t *pdd_config = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    tbl_id = stage->tbls.entry_tbl->sid;

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));

    filter->group_config = group_config;

    tbl_id = stage->tbls.pdd_tbl->sid;
    /* Get the PDD configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_pdd_config_get(unit,
                              idp_info->op_arg->trans_id,
                              tbl_id,
                              stage_id,
                              idp_info->key,
                              idp_info->data,
                              &pdd_config));
    filter->pdd_config = pdd_config;

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    tbl_id = stage->tbls.entry_tbl->sid;
    /* Create the filter corresponding to entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    /* Update the operational state of the entry id. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}


static int
bcmfp_group_idp_pdd_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_entry_id_t entry_id,
                             bcmfp_group_id_t group_id,
                             void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /*
     * If entry ID is not operational then this filter
     * was not created. Now create to reflect the right
     * entry operatioal state.
     */
    tbl_id = stage->tbls.entry_tbl->sid;
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        filter->event_reason = BCMIMM_ENTRY_DELETE;
        filter->sid_type = BCMFP_SID_TYPE_PDD_TEMPLATE;
        /* Create the filter corresponding to entry ID. */
        SHR_IF_ERR_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;
        /* Update the entry state. */
        entry_state = BCMFP_ENTRY_PDD_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_dpolicy_insert(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_entry_id_t entry_id,
                               bcmfp_group_id_t group_id,
                               void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_policy_config_t *policy_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));
    filter->group_config = group_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.policy_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_config_get(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage_id,
                                 idp_info->key,
                                 idp_info->data,
                                 &policy_config));
    filter->dpolicy_config = policy_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /* Create the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_create(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    /* Update the operational state of the entry id. */
    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_dpolicy_update(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_entry_id_t entry_id,
                               bcmfp_group_id_t group_id,
                               void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state_curr;
    bcmfp_entry_state_t entry_state_prev;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_policy_config_t *policy_config = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));
    filter->group_config = group_config;

    /*
     * Should use incoming key and data as the data is not
     * yet present in IMM.
     */
    tbl_id = stage->tbls.policy_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_policy_config_get(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage_id,
                                 idp_info->key,
                                 idp_info->data,
                                 &policy_config));
    filter->dpolicy_config = policy_config;

    /*
     * Keep the group id  and entry state for incrementing
     * or decrementing its entry  reference count at the
     * end of this function.
     */
    entry_state_prev = filter->entry_config->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /* Update the filter corresponding to entry ID. */
    tbl_id = stage->tbls.entry_tbl->sid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_filter_update(unit,
                             idp_info->op_arg,
                             tbl_id,
                             stage->stage_id,
                             NULL,
                             filter));

    if (filter->entry_state == BCMFP_ENTRY_SUCCESS &&
        entry_state_prev != BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count++;
    }

    if (filter->entry_state != BCMFP_ENTRY_SUCCESS &&
        entry_state_prev == BCMFP_ENTRY_SUCCESS) {
        opinfo->ref_count--;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    entry_state_curr = filter->entry_state;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state_curr,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_dpolicy_delete(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_entry_id_t entry_id,
                               bcmfp_group_id_t group_id,
                               void *user_data)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_entry_config_t *entry_config = NULL;
    bcmfp_filter_t *filter = NULL;
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t key_fid = 0;
    bcmltd_fid_t data_fid = 0;
    bcmfp_entry_state_t entry_state;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_idp_info_t *idp_info = user_data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    BCMFP_ALLOC(filter, sizeof(bcmfp_filter_t), "bcmfpFilter");

    /* Get the entry configuration for the entry ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_remote_config_get(unit,
                                       idp_info->op_arg,
                                       stage_id,
                                       entry_id,
                                       &entry_config));
    filter->entry_config = entry_config;

    /*
     * Keep the group id for decrementing its entry
     * reference count at the end of this function.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));
    filter->group_oper_info = opinfo;

    /*
     * If entry ID is not operational then this filter
     * was not created. Now create to reflect the right
     * entry operatioal state.
     */
    tbl_id = stage->tbls.entry_tbl->sid;
    if (entry_config->entry_state != BCMFP_ENTRY_SUCCESS) {
        filter->event_reason = BCMIMM_ENTRY_DELETE;
        filter->sid_type = BCMFP_SID_TYPE_POLICY_TEMPLATE;
        /* Create the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_create(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        entry_state = filter->entry_state;
    } else {
        /* Delete the filter corresponding to entry ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_filter_delete(unit,
                                 idp_info->op_arg,
                                 tbl_id,
                                 stage->stage_id,
                                 filter));
        /* Decrement the group reference count. */
        opinfo->ref_count--;

        /* Update the entry state. */
        entry_state = BCMFP_ENTRY_POLICY_NOT_CREATED;
    }

    key_fid = stage->tbls.entry_tbl->key_fid;
    data_fid = stage->tbls.entry_tbl->oprtnl_state_fid;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_imm_entry_update(unit,
                                tbl_id,
                                &key_fid,
                                &entry_id,
                                1,
                                &data_fid,
                                &entry_state,
                                1));
exit:
    bcmfp_filter_configs_free(unit, filter);
    SHR_FREE(filter);
    SHR_FUNC_EXIT();
}

/*
 * This function deletes the current groups and recreates
 * them for corresponding PDD/SBR/PSG.
 */
static int
bcmfp_group_idp_common_process(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_group_id_t group_id,
                               void *user_data)
{
    bcmimm_entry_event_t event_reason;
    bcmfp_idp_info_t *idp_info = user_data;
    bcmfp_stage_t *stage = NULL;
    bcmltd_sid_t tbl_id = BCMLTD_SID_INVALID;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    event_reason = idp_info->event_reason;

    if ((event_reason != BCMIMM_ENTRY_INSERT) &&
        (event_reason != BCMIMM_ENTRY_DELETE) &&
        (event_reason != BCMIMM_ENTRY_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    tbl_id = stage->tbls.group_tbl->sid;

    SHR_IF_ERR_EXIT
        (bcmfp_group_config_delete_idp(unit,
                                       idp_info->op_arg,
                                       tbl_id,
                                       stage_id,
                                       group_id,
                                       idp_info));

    SHR_IF_ERR_EXIT
        (bcmfp_group_config_insert_idp(unit,
                                       idp_info->op_arg,
                                       tbl_id,
                                       stage_id,
                                       group_id,
                                       idp_info));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_psg_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_idp_info_t *idp_info)
{
    bcmimm_entry_event_t event_reason;
    bcmfp_entry_id_traverse_cb cb_func;
    void *user_data = NULL;
    bool not_mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    user_data = (void *)idp_info;
    event_reason = idp_info->event_reason;

    SHR_IF_ERR_EXIT
        (bcmfp_group_idp_common_process(unit,
                                     stage_id,
                                     group_id,
                                     idp_info));

   /* Check entries in the group if present */
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_check(unit,
                                     stage_id,
                                     group_id,
                                     &not_mapped));

    /* Return if no entries are associated to the group */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        cb_func = bcmfp_group_idp_psg_insert;
    } else if (event_reason == BCMIMM_ENTRY_DELETE) {
        cb_func = bcmfp_group_idp_psg_delete;
    } else if (event_reason == BCMIMM_ENTRY_UPDATE) {
        cb_func = bcmfp_group_idp_psg_insert;
    } else {
         SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_traverse(unit,
                                        stage_id,
                                        group_id,
                                        cb_func,
                                        user_data));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_pdd_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_idp_info_t *idp_info)
{
    bcmimm_entry_event_t event_reason;
    bcmfp_entry_id_traverse_cb cb_func;
    void *user_data = NULL;
    bool not_mapped = FALSE;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    user_data = (void *)idp_info;
    event_reason = idp_info->event_reason;

    if (event_reason != BCMIMM_ENTRY_DELETE) {
        if (idp_info->entry_state
            == BCMFP_ENTRY_GROUP_ASET_DOESNT_FIT) {

            SHR_IF_ERR_EXIT
            (bcmfp_group_oper_info_get(unit,
                                       stage_id,
                                       group_id,
                                       &opinfo));
            opinfo->aset_error = TRUE;
        }
        SHR_IF_ERR_EXIT
            (bcmfp_group_idp_common_process(unit,
                                         stage_id,
                                         group_id,
                                         idp_info));
    }

   /* Check entries in the group if present */
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_check(unit,
                                     stage_id,
                                     group_id,
                                     &not_mapped));

    /* Return if no entries are associated to the group */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        cb_func = bcmfp_group_idp_pdd_insert;
    } else if (event_reason == BCMIMM_ENTRY_DELETE) {
        cb_func = bcmfp_group_idp_pdd_delete;
    } else if (event_reason == BCMIMM_ENTRY_UPDATE) {
        cb_func = bcmfp_group_idp_pdd_insert;
    } else {
         SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_traverse(unit,
                                        stage_id,
                                        group_id,
                                        cb_func,
                                        user_data));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_sbr_process(int unit,
                            bcmfp_stage_id_t stage_id,
                            bcmfp_group_id_t group_id,
                            bcmfp_idp_info_t *idp_info)
{
    bcmimm_entry_event_t event_reason;
    bcmfp_entry_id_traverse_cb cb_func;
    void *user_data = NULL;
    bool not_mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    user_data = (void *)idp_info;
    event_reason = idp_info->event_reason;

    if (event_reason == BCMIMM_ENTRY_UPDATE) {
        SHR_ERR_EXIT
            (bcmfp_group_idp_sbr_update_hw(unit,
                                         stage_id,
                                         group_id,
                                         idp_info));

    } else if (event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_idp_common_process(unit,
                                         stage_id,
                                         group_id,
                                         idp_info));

    }

   /* Check entries in the group if present */
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_check(unit,
                                     stage_id,
                                     group_id,
                                     &not_mapped));

    /* Return if no entries are associated to the group */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        cb_func = bcmfp_group_idp_sbr_insert;
    } else if (event_reason == BCMIMM_ENTRY_DELETE) {
        cb_func = bcmfp_group_idp_sbr_delete;
    }  else {
         SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_traverse(unit,
                                        stage_id,
                                        group_id,
                                        cb_func,
                                        user_data));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_group_idp_dpolicy_process(int unit,
                                bcmfp_stage_id_t stage_id,
                                bcmfp_group_id_t group_id,
                                bcmfp_idp_info_t *idp_info)
{
    bcmimm_entry_event_t event_reason;
    bcmfp_entry_id_traverse_cb cb_func;
    void *user_data = NULL;
    bool not_mapped = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(idp_info, SHR_E_PARAM);

    user_data = (void *)idp_info;
    event_reason = idp_info->event_reason;

    SHR_IF_ERR_EXIT
        (bcmfp_group_idp_common_process(unit,
                                     stage_id,
                                     group_id,
                                     idp_info));

   /* Check entries in the group if present */
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_check(unit,
                                     stage_id,
                                     group_id,
                                     &not_mapped));

    /* Return if no entries are associated to the group */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    if (event_reason == BCMIMM_ENTRY_INSERT) {
        cb_func = bcmfp_group_idp_dpolicy_insert;
    } else if (event_reason == BCMIMM_ENTRY_DELETE) {
        cb_func = bcmfp_group_idp_dpolicy_delete;
    } else if (event_reason == BCMIMM_ENTRY_UPDATE) {
        cb_func = bcmfp_group_idp_dpolicy_update;
    } else {
         SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_traverse(unit,
                                        stage_id,
                                        group_id,
                                        cb_func,
                                        user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_idp_process(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint32_t template_id,
                        void *user_data)
{
    bcmfp_idp_info_t *idp_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    idp_info = user_data;

    /*
     * Enable atomic transaction because there might be
     * multiple PTM opcode requests for single LT opcode
     * request. Because single PDD/PSG/SBR/PSE can be
     * associated to multiple groups and those groups
     * might be associated to multiple entries.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_trans_atomic_state_set(unit,
                                      idp_info->op_arg->trans_id,
                                      TRUE));

    if (idp_info->sid_type == BCMFP_SID_TYPE_PSE_TEMPLATE) {
        idp_info->template_id = template_id;
        SHR_IF_ERR_EXIT
            (bcmfp_group_idp_pse_process(unit,
                                         stage_id,
                                         group_id,
                                         idp_info));
    }

    if (idp_info->sid_type == BCMFP_SID_TYPE_PSG_TEMPLATE) {
        idp_info->template_id = template_id;
        SHR_IF_ERR_EXIT
            (bcmfp_group_idp_psg_process(unit,
                                         stage_id,
                                         group_id,
                                         idp_info));
    }

    if (idp_info->sid_type == BCMFP_SID_TYPE_SBR_TEMPLATE) {
        idp_info->template_id = template_id;
        SHR_IF_ERR_EXIT
            (bcmfp_group_idp_sbr_process(unit,
                                         stage_id,
                                         group_id,
                                         idp_info));
    }

    if (idp_info->sid_type == BCMFP_SID_TYPE_PDD_TEMPLATE) {
        idp_info->template_id = template_id;
        SHR_IF_ERR_EXIT
            (bcmfp_group_idp_pdd_process(unit,
                                         stage_id,
                                         group_id,
                                         idp_info));
    }

    if (idp_info->sid_type == BCMFP_SID_TYPE_POLICY_TEMPLATE) {
        idp_info->template_id = template_id;
        SHR_IF_ERR_EXIT
            (bcmfp_group_idp_dpolicy_process(unit,
                                             stage_id,
                                             group_id,
                                             idp_info));
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
               GROUP OPCODE DISPATCH FUNCTIONS
 */
int
bcmfp_group_config_insert(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_config_t *group_config = NULL;
    uint32_t group_id = 0;
    bcmltd_field_t *in_data = NULL;
    bcmltd_field_t *non_default_data = NULL;
    bool not_mapped = FALSE;
    bcmfp_idp_info_t idp_info;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /*
     * Build list of fields with non default values from
     * the incoming data.
     */
    in_data = (bcmltd_field_t *)data;
    SHR_IF_ERR_EXIT
        (bcmfp_ltd_non_default_data_get(unit,
                                        tbl_id,
                                        in_data,
                                        &non_default_data,
                                        &(stage->imm_buffers)));

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage_id,
                                key,
                                non_default_data,
                                &group_config));

    if (LOG_CHECK_VERBOSE(BSL_LOG_MODULE)) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_config_dump(unit,
                                     tbl_id,
                                     stage_id,
                                     group_config));
    }

    /*
     * Update the maping between group and remote keys
     * associated to group like presel entries, pdd,
     * sbr, presel group, default pdd, default policy.
 */
    SHR_IF_ERR_EXIT
        (bcmfp_group_maps_add(unit, stage_id, group_config));

    /* Create a dummy operational info */
    group_id = group_config->group_id;
    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_create(unit,
                                      stage_id,
                                      group_id,
                                      &opinfo));
    opinfo->valid = FALSE;
    opinfo->state = BCMFP_GROUP_STATE_SUCCESS;
    /* Validate the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_validate(unit,
                                     op_arg,
                                     tbl_id,
                                     stage->stage_id,
                                     group_config,
                                     opinfo));

    /* Compress the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_compress(unit,
                                     op_arg,
                                     stage_id,
                                     group_id,
                                     group_config));

    /*
     * Process the group configuration. It involves
     * allocating offsets and widths to all of the
     * qualifiers and actions in the group.
     */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_process(unit,
                                    op_arg,
                                    tbl_id,
                                    stage,
                                    group_config,
                                    opinfo,
                                    NULL));

    /* Add read only LT fields to output_fields. */
    group_id = group_config->group_id;
    if (opinfo->state == BCMFP_GROUP_STATE_SUCCESS) {
        SHR_IF_ERR_EXIT
            (bcmfp_group_ro_fields_add(unit,
                                       stage_id,
                                       group_id,
                                       output_fields));
    }

    /* Insert entries in the group if present */
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_check(unit,
                                     stage_id,
                                     group_id,
                                     &not_mapped));

    /* Return if no entries are associated to the group */
    if (not_mapped == TRUE) {
        SHR_EXIT();
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_GRP_TEMPLATE;
    idp_info.key = key;
    idp_info.data = data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_INSERT;

    user_data = (void *)(&idp_info);
    cb_func = bcmfp_entry_idp_group_process;
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_traverse(unit,
                                        stage_id,
                                        group_id,
                                        cb_func,
                                        user_data));
    /* Group not operational */
    if (SHR_FUNC_VAL_IS(SHR_E_CONFIG)) {
        SHR_EXIT();
    }
exit:
    bcmfp_group_config_free(unit, group_config);
    SHR_FUNC_EXIT();
}

int
bcmfp_group_config_update(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    uint16_t num_fid = 0;
    bcmltd_field_t *in_key = NULL;
    bcmltd_field_t *in_data = NULL;
    bcmltd_fields_t updated_fields;
    bcmfp_stage_t *stage = NULL;
    const bcmltd_field_t *updated_data = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&updated_fields,0,sizeof(bcmltd_fields_t));

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_group_config_delete(unit,
                                   op_arg,
                                   tbl_id,
                                   stage_id,
                                   key,
                                   data,
                                   output_fields));

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    in_key = (bcmltd_field_t *)key;
    in_data = (bcmltd_field_t *)data;
    num_fid = stage->tbls.group_tbl->lrd_info.num_fid;
    SHR_IF_ERR_EXIT
        (bcmfp_ltd_updated_data_get(unit,
                                    tbl_id,
                                    num_fid,
                                    in_key,
                                    in_data,
                                    &updated_fields));

    updated_data =
        (const bcmltd_field_t *)(updated_fields.field[0]);

    SHR_IF_ERR_EXIT
        (bcmfp_group_config_insert(unit,
                                   op_arg,
                                   tbl_id,
                                   stage_id,
                                   key,
                                   updated_data,
                                   output_fields));
exit:
    bcmfp_ltd_buffers_free(unit, &updated_fields, num_fid);
    SHR_FUNC_EXIT();
}
int
bcmfp_group_config_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t tbl_id,
                          bcmfp_stage_id_t stage_id,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          bcmltd_fields_t *output_fields)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_id_t group_id = 0;
    bcmfp_idp_info_t idp_info;
    bcmfp_group_config_t *group_config = NULL;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;
    bcmfp_group_oper_info_t *opinfo = NULL;
    shr_error_t rv_except = SHR_E_NOT_FOUND;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_config_get(unit,
                                op_arg,
                                tbl_id,
                                stage_id,
                                key,
                                data,
                                &group_config));

    group_id = group_config->group_id;
    bcmfp_group_config_free(unit, group_config);

    /* Get the group configuration. */
    SHR_IF_ERR_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));

    /*
     * Delete the maping between group and remote keys
     * associated to group like presel entries, pdd,
     * sbr, presel group, default pdd, default policy.
 */
    SHR_IF_ERR_EXIT
        (bcmfp_group_maps_delete(unit, stage_id, group_config));

    /* Remove the entries from the group if present. */
    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = (bcmltd_op_arg_t *)op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_GRP_TEMPLATE;
    idp_info.key = key;
    idp_info.data = data;
    idp_info.output_fields = output_fields;
    idp_info.event_reason = BCMIMM_ENTRY_DELETE;
    group_id = group_config->group_id;
    user_data = (void *)(&idp_info);
    cb_func = bcmfp_entry_idp_group_process;
    SHR_IF_ERR_EXIT
        (bcmfp_group_entry_map_traverse(unit,
                                        stage_id,
                                        group_id,
                                        cb_func,
                                        user_data));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo),
                                   rv_except);

   if (opinfo != NULL) {
       /*
        * Delete the group operational state from the
        * HA space.
        */
       SHR_IF_ERR_EXIT
           (bcmfp_group_oper_info_delete(unit,
                                         stage_id,
                                         group_id));
       SHR_IF_ERR_EXIT
           (bcmfp_group_params_reset(unit,
                                     stage_id,
                                     group_id));
   }
exit:
    if (group_config != NULL) {
        bcmfp_group_config_free(unit, group_config);
    }
    SHR_FUNC_EXIT();
}
