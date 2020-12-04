/*! \file rm_tcam_fp_pdd_utils.c
 *
 *  PDD APIs.
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
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>
#include "rm_tcam_fp_pdd.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_tcam_fp_pdd_profiles_count_get(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              uint8_t pipe,
                              uint16_t *pdd_count)
{
    uint8_t num_slices = 0;
    uint8_t slice_id = 0;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;
    const bcmptm_rm_tcam_slice_info_t *slice_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(pdd_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_stage_attr_slices_count_get(unit,
                                                    ltid,
                                                    ltid_info,
                                                    pipe,
                                                    &num_slices));
    rm_more_info = ltid_info->rm_more_info;
    *pdd_count = 0;
    for (slice_id = 0; slice_id < num_slices; slice_id++) {
        slice_info = &(rm_more_info->slice_info[slice_id]);
        *pdd_count += slice_info->pdd_profiles_count;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_pdd_profiles_ref_count_init(int unit,
                        bcmptm_rm_tcam_fp_stage_t *stage_ptr)
{
    size_t size = 0;
    uint32_t *pdd_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_ptr, SHR_E_PARAM);

    pdd_ptr = (uint32_t *)((uint8_t *)stage_ptr +
              stage_ptr->pdd_ref_count_seg);
    size = (sizeof(uint32_t) * stage_ptr->num_pdd_profiles);
    if (stage_ptr->num_pdd_profiles) {
        sal_memset(pdd_ptr, 0, size);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_pdd_profiles_ref_count_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  int pipe_id,
                  bcmptm_rm_tcam_fp_stage_t *stage_fc,
                  uint8_t slice_id,
                  uint32_t **ref_count_ptr)
{
    uint8_t idx = 0;
    uint16_t pdd_profiles_total = 0;
    const bcmptm_rm_tcam_more_info_t *rm_more_info = NULL;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);
    SHR_NULL_CHECK(ref_count_ptr, SHR_E_PARAM);

    /* For global mode, use the memory created for pipe 0
     * As only either global mode or pipe mode can exists
     * at a gvien time
     */
    if (pipe_id == BCMPTM_RM_TCAM_GLOBAL_PIPE) {
        pipe_id = 0;
    }

    rm_more_info = ltid_info->rm_more_info;

    for (idx = 0; idx < slice_id; idx++) {
        pdd_profiles_total +=
            rm_more_info->slice_info[idx].pdd_profiles_count;
    }

    /* Fetch the PDD profiles segment start pointer of a slice. */
    *ref_count_ptr = (uint32_t *)
                     (((uint8_t *)stage_fc) +
                     stage_fc->pdd_ref_count_seg +
                     (pdd_profiles_total *
                     sizeof(uint32_t)));
exit:
    SHR_FUNC_EXIT();
}
