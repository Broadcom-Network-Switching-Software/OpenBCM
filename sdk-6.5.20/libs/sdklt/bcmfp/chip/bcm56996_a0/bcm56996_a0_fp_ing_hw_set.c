/*! \file bcm56996_a0_fp_ing_hw_set.c
 *
 *  APIs to update ingress field processor register for Tomahawk-4G device(56996_A0)
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
#include <bcmfp/bcmfp_ptm_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56996_a0_ifp_spare_debug_set(int unit)
{
    int index = 0;
    uint32_t profile_data = 0;
    uint32_t enable = 1;
    uint32_t rsp_flags = 0;
    int pipe_id = -1;
    bcmdrd_sid_t profile_sid = IFP_SPARE_DEBUGr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             profile_sid,
                             &profile_data,
                             FLEX_CTR_OBJ_BUS_CONTAINER_ID_ENf,
                             &enable));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_index_insert(unit,
                                BCMPTM_DIRECT_TRANS_ID,
                                pipe_id,
                                0,
                                profile_sid,
                                &profile_data,
                                index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    BCMPTM_REQ_FLAGS_COMMIT,
                                    profile_sid,
                                    NULL,
                                    NULL,
                                    NULL,
                                    BCMPTM_OP_NOP,
                                    NULL,
                                    0,
                                    0,
                                    BCMPTM_DIRECT_TRANS_ID,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}
