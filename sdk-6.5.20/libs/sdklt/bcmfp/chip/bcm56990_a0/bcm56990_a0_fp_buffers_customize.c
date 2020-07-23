/*! \file bcm56990_a0_fp_buffers_customize.c
 *
 * Chip Specific Stage buffers customize function
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
bcm56990_a0_fp_buffers_customize(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 uint32_t flags,
                                 bcmfp_stage_t *stage,
                                 bcmfp_group_oper_info_t *opinfo,
                                 bcmfp_pdd_config_t *pdd_config,
                                 bcmfp_key_ids_t *key_ids,
                                 bcmfp_buffers_t *buffers)
{
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_id = stage->stage_id;

    switch (stage_id) {
        case BCMFP_STAGE_ID_EXACT_MATCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_a0_em_buffers_custom_set(unit,
                                                         op_arg->trans_id,
                                                         flags,
                                                         stage,
                                                         opinfo,
                                                         pdd_config,
                                                         key_ids,
                                                         buffers));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();

}
