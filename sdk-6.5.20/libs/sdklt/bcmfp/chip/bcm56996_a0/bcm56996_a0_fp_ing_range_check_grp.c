/*! \file bcm56996_a0_fp_ing_range_check_grp.c
 *
 *  APIs to update range_check_grp for Tomahawk-4G device(56996_A0)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmfp/bcmfp_cth_range_check.h>
#include <bcmfp/bcmfp_ptm_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static uint32_t data[BCMFP_ENTRY_WORDS_MAX];

int
bcmfp_bcm56996_a0_ingress_range_check_grp_set(int unit,
                                              uint32_t trans_id,
                                              bcmltd_sid_t sid,
                                              int pipe_id,
                                              int range_check_grp_id,
                                              bcmfp_range_check_id_bmp_t bmp)
{
    bcmdrd_sid_t mem_sid;
    bcmdrd_fid_t fid;
    uint32_t *data_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    data_ptr = &data[0];

    if ((range_check_grp_id >= 0) &&
        (range_check_grp_id <= 7)) {
        mem_sid = IFP_RANGE_CHECK_MASK_Am;
    } else if ((range_check_grp_id > 7) &&
        (range_check_grp_id <= 15)) {
        mem_sid = IFP_RANGE_CHECK_MASK_Bm;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (range_check_grp_id) {
        case 0:
             fid = IFP_RANGE_CHECK_MASK_0f;
             break;
        case 1:
             fid = IFP_RANGE_CHECK_MASK_1f;
             break;
        case 2:
             fid = IFP_RANGE_CHECK_MASK_2f;
             break;
        case 3:
             fid = IFP_RANGE_CHECK_MASK_3f;
             break;
        case 4:
             fid = IFP_RANGE_CHECK_MASK_4f;
             break;
        case 5:
             fid = IFP_RANGE_CHECK_MASK_5f;
             break;
        case 6:
             fid = IFP_RANGE_CHECK_MASK_6f;
             break;
        case 7:
             fid = IFP_RANGE_CHECK_MASK_7f;
             break;
        case 8:
             fid = IFP_RANGE_CHECK_MASK_8f;
             break;
        case 9:
             fid = IFP_RANGE_CHECK_MASK_9f;
             break;
        case 10:
             fid = IFP_RANGE_CHECK_MASK_10f;
             break;
        case 11:
             fid = IFP_RANGE_CHECK_MASK_11f;
             break;
        case 12:
             fid = IFP_RANGE_CHECK_MASK_12f;
             break;
        case 13:
             fid = IFP_RANGE_CHECK_MASK_13f;
             break;
        case 14:
             fid = IFP_RANGE_CHECK_MASK_14f;
             break;
        case 15:
             fid = IFP_RANGE_CHECK_MASK_15f;
             break;
             /* coverity[dead_error_begin] */
        default:
             SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(data, 0, BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_index_read(unit,
                              trans_id,
                              pipe_id,
                              sid,
                              mem_sid,
                              data_ptr,
                              0));
    bcmdrd_pt_field_set(unit,
                        mem_sid,
                        data_ptr,
                        fid,
                        bmp.w);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_index_insert(unit,
                                trans_id,
                                pipe_id,
                                sid,
                                mem_sid,
                                data_ptr,
                                0));

exit:
    SHR_FUNC_EXIT();
}

