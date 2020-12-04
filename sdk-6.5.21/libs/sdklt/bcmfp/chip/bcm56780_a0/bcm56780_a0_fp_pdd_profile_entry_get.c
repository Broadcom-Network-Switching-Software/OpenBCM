/*! \file bcm56780_a0_fp_pdd_profile_entry_get.c
 *
 * Chip Specific Stage pdd profile entry get function
 * for Trident4-X9(56780_A0).
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
#include <bcmfp/chip/bcm56780_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56780_a0_fp_pdd_profile_entry_get(int unit,
                                     const bcmltd_op_arg_t *op_arg,
                                     bcmfp_stage_t *stage,
                                     bcmfp_pdd_id_t pdd_id,
                                     bcmfp_group_id_t group_id,
                                     bcmfp_group_oper_info_t *opinfo,
                                     bcmfp_pdd_oper_type_t pdd_type,
                                     uint32_t **pdd_profile,
                                     bcmfp_pdd_config_t *pdd_config)
{
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_id = stage->stage_id;

    if (stage_id == BCMFP_STAGE_ID_LOOKUP ||
        stage_id == BCMFP_STAGE_ID_EGRESS ||
        stage_id == BCMFP_STAGE_ID_INGRESS ||
        stage_id == BCMFP_STAGE_ID_EXACT_MATCH ||
        stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_common_pdd_profile_entry_get(unit,
                                                         op_arg->trans_id,
                                                         stage->stage_id,
                                                         pdd_id,
                                                         group_id,
                                                         opinfo,
                                                         pdd_type,
                                                         pdd_profile,
                                                         pdd_config));
    }

exit:
    SHR_FUNC_EXIT();

}
