/*! \file bcm56780_a0_fp_action_conflict_check.c
 *
 * Chip Specific Stage Action Conflict Check function
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

STATIC int
bcm56780_a0_fp_common_action_conflict_check(int unit,
                                            bcmfp_action_t action1,
                                            bcmfp_action_t action2)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

int
bcm56780_a0_fp_action_conflict_check(int unit,
                                     bcmfp_stage_t *stage,
                                     bcmfp_action_t action1,
                                     bcmfp_action_t action2)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_fp_common_action_conflict_check(unit,
                                                     action1,
                                                     action2));

exit:
    SHR_FUNC_EXIT();

}
