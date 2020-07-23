/*! \file bcm56990_a0_fp_pdd_profile_entry_get.c
 *
 * Chip Specific Stage pdd profile entry get function
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
bcm56990_a0_fp_pdd_profile_entry_get(int unit,
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

    switch (stage_id) {
        case BCMFP_STAGE_ID_EXACT_MATCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_a0_fp_em_pdd_profile_entry_get(unit,
                                                         op_arg->trans_id,
                                                         stage->stage_id,
                                                         pdd_id,
                                                         group_id,
                                                         opinfo,
                                                         pdd_type,
                                                         pdd_profile,
                                                         pdd_config));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();

}
