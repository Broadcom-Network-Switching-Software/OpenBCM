/*! \file bcmfp_profile.c
 *
 * Infrastructure APIs to manage profile tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ptm_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_profile_index_alloc(int unit,
                          uint32_t trans_id,
                          int tbl_inst,
                          bcmltd_sid_t req_ltid,
                          uint8_t num_profiles,
                          bcmdrd_sid_t *profile_sids,
                          uint32_t *profile_data[],
                          bcmfp_ref_count_t *ref_counts,
                          int *index,
                          int max_idx,
                          int min_idx)
{
     int idx = 0;
     int free_idx = 0;
     uint8_t profile = 0;
     shr_error_t rv = SHR_E_NONE;

     SHR_FUNC_ENTER(unit);

     SHR_NULL_CHECK(index, SHR_E_PARAM);
     SHR_NULL_CHECK(profile_sids, SHR_E_PARAM);
     SHR_NULL_CHECK(profile_data, SHR_E_PARAM);
     SHR_NULL_CHECK(ref_counts, SHR_E_PARAM);

     if (min_idx < 0 || max_idx < 0) {
         SHR_ERR_EXIT(SHR_E_PARAM);
     }

     for (idx = min_idx; idx <= max_idx; idx++) {

         if (!ref_counts[idx].ref_count) {
             free_idx = idx;
             continue;
         }

         rv = bcmfp_ptm_index_lookup(unit,
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
             rv = bcmfp_ptm_index_lookup(unit,
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

     SHR_IF_ERR_EXIT((free_idx == -1) ? SHR_E_RESOURCE : SHR_E_NONE);
     *index = free_idx;
exit:
    SHR_FUNC_EXIT();
}

/*
 * add the profile in a free entry between the
 * range provided (min_idx:max_idx)
 */
int
bcmfp_profile_add_range(int unit,
                        uint32_t trans_id,
                        int tbl_inst,
                        bcmltd_sid_t req_ltid,
                        uint8_t num_profiles,
                        bcmdrd_sid_t *profile_sids,
                        uint32_t *profile_data[],
                        bcmfp_ref_count_t *ref_counts,
                        int *profile_index,
                        int max_idx,
                        int min_idx)
{
    int index = 0;
    uint8_t profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_data, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_index, SHR_E_PARAM);
    SHR_NULL_CHECK(ref_counts, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_profile_index_alloc(unit,
                                  trans_id,
                                  tbl_inst,
                                  req_ltid,
                                  num_profiles,
                                  profile_sids,
                                  profile_data,
                                  ref_counts,
                                  &index,
                                  max_idx,
                                  min_idx));

    if (ref_counts[index].ref_count) {
        ref_counts[index].ref_count += 1;
        *profile_index = index;
        SHR_EXIT();
    }

    for (profile = 0; profile < num_profiles; profile++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_insert(unit,
                                    trans_id,
                                    tbl_inst,
                                    req_ltid,
                                    profile_sids[profile],
                                    profile_data[profile],
                                    index));
    }

    ref_counts[index].ref_count += 1;
    *profile_index = index;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_profile_add(int unit,
                  uint32_t trans_id,
                  int tbl_inst,
                  bcmltd_sid_t req_ltid,
                  uint8_t num_profiles,
                  bcmdrd_sid_t *profile_sids,
                  uint32_t *profile_data[],
                  bcmfp_ref_count_t *ref_counts,
                  int *profile_index)
{
    int index = 0;
    int max_idx;
    int min_idx;
    uint8_t profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_data, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_index, SHR_E_PARAM);
    SHR_NULL_CHECK(ref_counts, SHR_E_PARAM);

    min_idx = bcmdrd_pt_index_min(unit, profile_sids[0]);
    max_idx = bcmdrd_pt_index_max(unit, profile_sids[0]);

    SHR_IF_ERR_EXIT(
        bcmfp_profile_index_alloc(unit,
                                  trans_id,
                                  tbl_inst,
                                  req_ltid,
                                  num_profiles,
                                  profile_sids,
                                  profile_data,
                                  ref_counts,
                                  &index,
                                  max_idx,
                                  min_idx));

    if (ref_counts[index].ref_count) {
        ref_counts[index].ref_count += 1;
        *profile_index = index;
        SHR_EXIT();
    }

    for (profile = 0; profile < num_profiles; profile++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_insert(unit,
                                    trans_id,
                                    tbl_inst,
                                    req_ltid,
                                    profile_sids[profile],
                                    profile_data[profile],
                                    index));
    }

    ref_counts[index].ref_count += 1;
    *profile_index = index;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_profile_delete(int unit,
                     uint32_t trans_id,
                     int tbl_inst,
                     bcmltd_sid_t req_ltid,
                     uint8_t num_profiles,
                     bcmltd_sid_t *profile_sids,
                     bcmfp_ref_count_t *ref_counts,
                     int profile_index)
{
    uint8_t profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(ref_counts, SHR_E_PARAM);

    SHR_IF_ERR_EXIT((profile_index == -1) ? SHR_E_PARAM : SHR_E_NONE);

    ref_counts[profile_index].ref_count -= 1;

    if (ref_counts[profile_index].ref_count) {
        SHR_EXIT();
    }

    for (profile = 0; profile < num_profiles; profile++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_delete(unit,
                                    trans_id,
                                    tbl_inst,
                                    req_ltid,
                                    profile_sids[profile],
                                    profile_index));
    }

exit:
    SHR_FUNC_EXIT();
}
