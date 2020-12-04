/*! \file bcm56990_a0_fp_src_class_mode_set.c
 *
 * Chip Specific stage src class mode set function
 * for (56990_A0).
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
#include <bcmfp/chip/bcm56990_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56990_a0_fp_src_class_mode_set(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmfp_stage_id_t stage_id,
                                  bcmltd_sid_t sid,
                                  int pipe_id,
                                  uint32_t mode)
{

    SHR_FUNC_ENTER(unit);

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_a0_ingress_src_class_mode_set(unit,
                                                              op_arg->trans_id,
                                                              sid,
                                                              pipe_id,
                                                              mode));
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_a0_em_src_class_mode_set(unit,
                                                         op_arg->trans_id,
                                                         sid,
                                                         pipe_id,
                                                         mode ));

            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
