/*! \file rm_tcam_profile.c
 *
 * Infrastructure APIs to manage profile tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_internal.h>
#include "rm_tcam_profile.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

static uint32_t req_ew[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_MAX_PT_FIELD_WORDS];
static uint32_t rsp_ew[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_MAX_PT_FIELD_WORDS];

/*******************************************************************************
 * Private Functions
 */
STATIC int
bcmptm_rm_tcam_profile_entry_insert(int unit,
                           uint32_t trans_id,
                           int tbl_inst,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t profile_sid,
                           uint32_t *profile_data,
                           int index)
{
     uint64_t req_flags = 0;
     uint32_t rsp_flags = 0;
     bcmltd_sid_t rsp_ltid;
     size_t rsp_entry_wsize = 0;
     bcmbd_pt_dyn_info_t pt_dyn_info;

     SHR_FUNC_ENTER(unit);

     SHR_NULL_CHECK(profile_data, SHR_E_PARAM);

     pt_dyn_info.index = index;
     pt_dyn_info.tbl_inst = tbl_inst;

     rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, profile_sid);

     sal_memset(rsp_ew[unit], 0 , BCMPTM_MAX_PT_FIELD_WORDS * sizeof(uint32_t));
     SHR_IF_ERR_VERBOSE_EXIT(
         bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    profile_data,
                                    rsp_entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew[unit],
                                    &rsp_ltid,
                                    &rsp_flags));

    SHR_IF_ERR_VERBOSE_EXIT((req_ltid != rsp_ltid) ? SHR_E_INTERNAL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_profile_entry_delete(int unit,
                           uint32_t trans_id,
                           int tbl_inst,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t profile_sid,
                           int index)
{
     uint64_t req_flags = 0;
     uint32_t rsp_flags = 0;
     bcmltd_sid_t rsp_ltid;
     size_t rsp_entry_wsize = 0;
     bcmbd_pt_dyn_info_t pt_dyn_info;

     SHR_FUNC_ENTER(unit);

     pt_dyn_info.index = index;
     pt_dyn_info.tbl_inst = tbl_inst;

     rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, profile_sid);

     sal_memset(req_ew[unit], 0 , BCMPTM_MAX_PT_FIELD_WORDS * sizeof(uint32_t));
     sal_memset(rsp_ew[unit], 0 , BCMPTM_MAX_PT_FIELD_WORDS * sizeof(uint32_t));
     SHR_IF_ERR_VERBOSE_EXIT(
         bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    req_ew[unit],
                                    rsp_entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew[unit],
                                    &rsp_ltid,
                                    &rsp_flags));

    SHR_IF_ERR_VERBOSE_EXIT((req_ltid != rsp_ltid) ? SHR_E_INTERNAL : SHR_E_NONE);

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_profile_index_alloc(int unit,
                          uint32_t trans_id,
                          int tbl_inst,
                          bcmltd_sid_t req_ltid,
                          uint8_t num_profiles,
                          bcmdrd_sid_t *profile_sids,
                          uint32_t *profile_data[],
                          uint32_t *profile_ref_count,
                          int *index)
{
     int idx = 0;
     int min_idx = 0;
     int max_idx = 0;
     int free_idx = -1;
     uint8_t profile = 0;
     shr_error_t rv = SHR_E_NONE;

     SHR_FUNC_ENTER(unit);

     SHR_NULL_CHECK(index, SHR_E_PARAM);
     SHR_NULL_CHECK(profile_sids, SHR_E_PARAM);
     SHR_NULL_CHECK(profile_data, SHR_E_PARAM);
     SHR_NULL_CHECK(profile_ref_count, SHR_E_PARAM);

     min_idx = bcmptm_pt_index_min(unit, profile_sids[0]);
     max_idx = bcmptm_pt_index_max(unit, profile_sids[0]);
     for (idx = min_idx; idx <= max_idx; idx++) {

         if (!profile_ref_count[idx]) {
             free_idx = idx;
             continue;
         }

         rv = bcmptm_rm_tcam_profile_entry_lookup(unit,
                                         trans_id,
                                         tbl_inst,
                                         req_ltid,
                                         profile_sids[0],
                                         profile_data[0],
                                         idx);
         SHR_IF_ERR_EXIT_EXCEPT_IF(rv , SHR_E_NOT_FOUND);
         if (SHR_FAILURE(rv)) {
             continue;
         }

         for (profile = 1; profile < num_profiles; profile++) {
             rv = bcmptm_rm_tcam_profile_entry_lookup(unit,
                                             trans_id,
                                             tbl_inst,
                                             req_ltid,
                                             profile_sids[profile],
                                             profile_data[profile],
                                             idx);
             if (SHR_FAILURE(rv)) {
                 break;
             }
         }

         if (profile == num_profiles && SHR_SUCCESS(rv)) {
             *index = idx;
             SHR_EXIT();
         }
     }

     SHR_IF_ERR_VERBOSE_EXIT((free_idx == -1) ? SHR_E_RESOURCE : SHR_E_NONE);
     *index = free_idx;
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_profile_entry_lookup(int unit,
                           uint32_t trans_id,
                           int tbl_inst,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t profile_sid,
                           uint32_t *profile_data,
                           int index)
{
     uint64_t req_flags = 0;
     uint32_t rsp_flags = 0;
     bcmltd_sid_t rsp_ltid;
     size_t rsp_entry_wsize = 0;
     bcmbd_pt_dyn_info_t pt_dyn_info;

     SHR_FUNC_ENTER(unit);

     SHR_NULL_CHECK(profile_data, SHR_E_PARAM);

     pt_dyn_info.index = index;
     pt_dyn_info.tbl_inst = tbl_inst;

     rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, profile_sid);

     sal_memset(rsp_ew[unit], 0 , BCMPTM_MAX_PT_FIELD_WORDS * sizeof(uint32_t));
     SHR_IF_ERR_VERBOSE_EXIT(
         bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    profile_sid,
                                    (void *)&pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    rsp_entry_wsize,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    rsp_ew[unit],
                                    &rsp_ltid,
                                    &rsp_flags));

    if (sal_memcmp(profile_data, rsp_ew[unit],
            rsp_entry_wsize * sizeof(uint32_t))) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_profile_add(int unit,
                  uint32_t trans_id,
                  int tbl_inst,
                  bcmltd_sid_t req_ltid,
                  uint8_t num_profiles,
                  bcmdrd_sid_t *profile_sids,
                  uint32_t *profile_data[],
                  uint32_t *profile_ref_count,
                  int *profile_index)
{
    int index = 0;
    uint8_t profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_data, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_index, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_ref_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_profile_index_alloc(unit,
                                  trans_id,
                                  tbl_inst,
                                  req_ltid,
                                  num_profiles,
                                  profile_sids,
                                  profile_data,
                                  profile_ref_count,
                                  &index));

    if (profile_ref_count[index]) {
        profile_ref_count[index] += 1;
        *profile_index = index;
        SHR_EXIT();
    }

    for (profile = 0; profile < num_profiles; profile++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_profile_entry_insert(unit,
                                       trans_id,
                                       tbl_inst,
                                       req_ltid,
                                       profile_sids[profile],
                                       profile_data[profile],
                                       index));
    }

    profile_ref_count[index] += 1;
    *profile_index = index;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_profile_delete(int unit,
                     uint32_t trans_id,
                     int tbl_inst,
                     bcmltd_sid_t req_ltid,
                     uint8_t num_profiles,
                     bcmltd_sid_t *profile_sids,
                     uint32_t *profile_ref_count,
                     int profile_index)
{
    uint8_t profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_ref_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT((profile_index == -1) ? SHR_E_PARAM : SHR_E_NONE);

    if (!profile_ref_count[profile_index]) {
        SHR_EXIT();
    }

    profile_ref_count[profile_index] -= 1;

    for (profile = 0; profile < num_profiles; profile++) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmptm_rm_tcam_profile_entry_delete(unit,
                                       trans_id,
                                       tbl_inst,
                                       req_ltid,
                                       profile_sids[profile],
                                       profile_index));
    }

exit:
    SHR_FUNC_EXIT();
}

