/*! \file rm_tcam_fp_profiles.c
 *
 *  FP profile table(SBR,PDD, Presel group) management APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>
#include "rm_tcam_fp_pdd.h"
#include "rm_tcam_fp_sbr.h"
#include "rm_tcam_fp_psg.h"
#include "rm_tcam_fp_profiles.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_fp_profiles_reserve(int unit,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd,
                                   uint8_t slice_id,
                                   uint8_t num_slices)
{
    bool reserve_pdd = FALSE;
    bool reserve_psg = FALSE;
    uint8_t *hw_indexes = NULL;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;

    if ((SHR_BITGET(iomd->entry_attrs->flags.w,
                    BCMPTM_FP_FLAGS_PDD_PROF_PER_GROUP)) &&
        (SHR_BITGET(iomd->entry_attrs->flags.w,
                    BCMPTM_FP_FLAGS_FIRST_ENTRY_IN_GROUP))) {
         reserve_pdd = TRUE;
    }  else if (SHR_BITGET(iomd->entry_attrs->flags.w,
                           BCMPTM_FP_FLAGS_PDD_PROF_PER_ENTRY)) {
         reserve_pdd = TRUE;
    }

    if (reserve_pdd == TRUE && stage_fc->num_pdd_profiles) {
        hw_indexes = iomd->entry_attrs->pdd_profile_indexes;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_pdd_profiles_reserve(unit,
                                                    iomd,
                                                    slice_id,
                                                    num_slices,
                                                    hw_indexes));
    }

    if ((SHR_BITGET(iomd->entry_attrs->flags.w,
        BCMPTM_FP_FLAGS_FIRST_ENTRY_IN_GROUP)) &&
        (SHR_BITGET(iomd->entry_attrs->flags.w,
         BCMPTM_FP_FLAGS_PRESEL_GROUP))) {
        reserve_psg = TRUE;
    }

    if (reserve_psg == TRUE && stage_fc->num_presel_groups) {
        hw_indexes = iomd->entry_attrs->presel_group_indexes;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_presel_group_reserve(unit,
                                                    iomd,
                                                    slice_id,
                                                    num_slices,
                                                    hw_indexes));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_profiles_unreserve(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    bool unreserve_pdd = FALSE;
    bool unreserve_sbr = FALSE;
    bool unreserve_psg = FALSE;
    bool per_group_pdd = FALSE;
    bool per_group_sbr = FALSE;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;

    if ((SHR_BITGET(iomd->entry_attrs->flags.w,
         BCMPTM_FP_FLAGS_PDD_PROF_PER_GROUP)) &&
        (SHR_BITGET(iomd->entry_attrs->flags.w,
         BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP))) {
         unreserve_pdd = TRUE;
         per_group_pdd = TRUE;
    }  else if (SHR_BITGET(iomd->entry_attrs->flags.w,
         BCMPTM_FP_FLAGS_PDD_PROF_PER_ENTRY)) {
         unreserve_pdd = TRUE;
    }

    if (unreserve_pdd == TRUE && stage_fc->num_pdd_profiles) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_pdd_profiles_unreserve(unit,
                                                      iomd,
                                                      per_group_pdd));
    }

    if ((SHR_BITGET(iomd->entry_attrs->flags.w,
         BCMPTM_FP_FLAGS_SBR_PROF_PER_GROUP)) &&
        (SHR_BITGET(iomd->entry_attrs->flags.w,
         BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP))) {
         unreserve_sbr = TRUE;
         per_group_sbr = TRUE;
    }  else if (SHR_BITGET(iomd->entry_attrs->flags.w,
         BCMPTM_FP_FLAGS_SBR_PROF_PER_ENTRY)) {
         unreserve_sbr = TRUE;
    }
    if (unreserve_sbr == TRUE && stage_fc->num_sbr_profiles) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_sbr_profiles_unreserve(unit,
                                                      iomd,
                                                      per_group_sbr));
    }

    if ((SHR_BITGET(iomd->entry_attrs->flags.w,
        BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP)) &&
        (SHR_BITGET(iomd->entry_attrs->flags.w,
         BCMPTM_FP_FLAGS_PRESEL_GROUP))) {
        unreserve_psg = TRUE;
    }

    if (unreserve_psg == TRUE && stage_fc->num_presel_groups) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_presel_group_unreserve(unit, iomd));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_profiles_copy(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd,
                                uint8_t new_slice_id)
{
    uint8_t idx = 0;
    uint8_t num_slices = 0;
    uint8_t tile_id = 0;
    bool sbr_psg_copy = TRUE;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;
    if (stage_fc->num_pdd_profiles) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_pdd_profiles_copy(unit,
                                                 iomd,
                                                 new_slice_id));
    }
    if (stage_fc->num_sbr_profiles ||
        stage_fc->num_presel_groups) {
        rm_more_info = iomd->ltid_info->rm_more_info;
        slice_fc = iomd->slice_fc;
        num_slices = iomd->num_slices;
        tile_id = rm_more_info->slice_info[new_slice_id].tile_id;
        for (idx = 0; idx < num_slices; idx++) {
            if (idx == new_slice_id) {
                continue;
            }
            if ((tile_id ==
               rm_more_info->slice_info[idx].tile_id) &&
               (slice_fc[idx].primary_grp_id != -1)) {
               sbr_psg_copy = FALSE;
               break;
            }
        }
    }
    if (stage_fc->num_sbr_profiles && (sbr_psg_copy == TRUE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_sbr_profiles_copy(unit,
                                                 iomd,
                                                 new_slice_id));
    }
    if (stage_fc->num_presel_groups && (sbr_psg_copy == TRUE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_presel_groups_copy(unit,
                                                  iomd,
                                                  new_slice_id));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_profiles_clear(int unit,
                                 bcmptm_rm_tcam_entry_iomd_t *iomd,
                                 uint8_t slice_id)
{
    uint8_t idx = 0;
    uint8_t num_slices = 0;
    uint8_t tile_id = 0;
    bool sbr_psg_clear = TRUE;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    stage_fc = iomd->stage_fc;

    if (stage_fc->num_pdd_profiles) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_pdd_profiles_clear(unit, iomd, slice_id));
    }

    if (stage_fc->num_sbr_profiles ||
        stage_fc->num_presel_groups) {
        rm_more_info = iomd->ltid_info->rm_more_info;
        slice_fc = iomd->slice_fc;
        num_slices = iomd->num_slices;
        tile_id = rm_more_info->slice_info[slice_id].tile_id;
        for (idx = 0; idx < num_slices; idx++) {
            if (idx == slice_id) {
                continue;
            }
            if ((tile_id ==
               rm_more_info->slice_info[idx].tile_id) &&
               (slice_fc[idx].primary_grp_id != -1)) {
               sbr_psg_clear = FALSE;
               break;
            }
        }
    }
    if (stage_fc->num_sbr_profiles && (sbr_psg_clear == TRUE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_sbr_profiles_clear(unit, iomd, slice_id));
    }
    if (stage_fc->num_presel_groups && (sbr_psg_clear == TRUE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_presel_groups_clear(unit, iomd, slice_id));
    }
exit:
    SHR_FUNC_EXIT();
}
