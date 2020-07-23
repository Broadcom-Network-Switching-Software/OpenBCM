/*! \file bcm56996_a0_fp_range_check_grp_set.c
 *
 * Chip Specific Stage range check grp set function
 * for (56996_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_chip_internal.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56996_a0_fp_range_check_grp_set(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmfp_stage_id_t stage_id,
                                   bcmltd_sid_t sid,
                                   int pipe_id,
                                   int range_check_grp_id,
                                   bcmfp_range_check_id_bmp_t bmp)
{

    SHR_FUNC_ENTER(unit);

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56996_a0_ingress_range_check_grp_set(unit,
                                                               op_arg->trans_id,
                                                               sid,
                                                               pipe_id,
                                                               range_check_grp_id,
                                                               bmp));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
