/*! \file bcm56780_a0_fp_meter_actions_set.c
 *
 * Chip Specific Stage meter actions set function
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
bcm56780_a0_fp_meter_actions_set(int unit,
                                 bcmfp_stage_t *stage,
                                 bcmfp_group_id_t group_id,
                                 bcmfp_pdd_id_t pdd_id,
                                 bcmfp_group_oper_info_t *opinfo,
                                 bcmcth_meter_fp_policy_fields_t *meter_entry,
                                 uint32_t **edw)
{
    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_id = stage->stage_id;

    if (stage_id == BCMFP_STAGE_ID_LOOKUP ||
        stage_id == BCMFP_STAGE_ID_EGRESS ||
        stage_id == BCMFP_STAGE_ID_INGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_common_meter_actions_set(unit,
                                                     group_id,
                                                     pdd_id,
                                                     opinfo,
                                                     meter_entry,
                                                     edw));
    }

exit:
    SHR_FUNC_EXIT();

}
