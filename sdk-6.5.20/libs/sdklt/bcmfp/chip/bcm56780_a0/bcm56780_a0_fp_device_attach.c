/*! \file bcm56780_a0_fp_device_attach.c
 *
 * BCMFP driver attach function for device bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_util.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56780_a0_fp.h>
#include <bcmfp/bcmfp_chip_internal.h>
#include <bcmfp/bcmfp_tbl_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

/*!
 * \brief Intialize the different tables info that does
 *  not depend on NPL variant.
 *  Example: PDD templates have auxiliary field actions
 *  which are not part  of policy template. These are not
 *  NPL generated so tooling is not able to assign actions
 *  to these LT fields in PDD template.
 *
 * \param [in] unit   Logical device id.
 * \param [in] stage  BCMFP stage pointer.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters
 */
STATIC int
bcm56780_a0_fp_device_misc_init(int unit,
                                bcmfp_stage_t *stage)
{
    uint16_t idx = 0;
    bcmfp_tbl_action_fid_info_t *actions_fid_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->tbls.pdd_tbl != NULL) {
        actions_fid_info =
            stage->tbls.pdd_tbl->actions_fid_info;
        if (actions_fid_info == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        for (idx = 0;
             idx < stage->tbls.pdd_tbl->fid_count;
             idx++) {
            if (actions_fid_info[idx].action == 0 &&
                idx != stage->tbls.pdd_tbl->key_fid) {
                actions_fid_info[idx].action = idx;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_fp_device_attach(int unit)
{
    bcmfp_stage_id_t stage_id = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    stage_id = BCMFP_STAGE_ID_LOOKUP;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_device_misc_init(unit, stage));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_info_tbls_init(unit, stage));
    }

    stage_id = BCMFP_STAGE_ID_INGRESS;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_device_misc_init(unit, stage));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_info_tbls_init(unit, stage));
    }

    stage_id = BCMFP_STAGE_ID_EGRESS;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_device_misc_init(unit, stage));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_info_tbls_init(unit, stage));
    }

    stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_device_misc_init(unit, stage));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_info_tbls_init(unit, stage));
    }

    stage_id = BCMFP_STAGE_ID_FLOW_TRACKER;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    if (stage != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_device_misc_init(unit, stage));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_info_tbls_init(unit, stage));
    }

exit:
    SHR_FUNC_EXIT();
}
