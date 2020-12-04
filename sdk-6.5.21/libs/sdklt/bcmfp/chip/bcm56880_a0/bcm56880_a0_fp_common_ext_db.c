/*! \file bcm56880_a0_fp_common_ext_db.c
 *
 * IFP/EFP/VFP multi hierarchy extractors configuration DB
 * initialization function for Trident4(56880_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56880_a0_fp.h>
#include <bcmfp/bcmfp_keygen_api.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56880_a0_fp_common_ext_db_init(int unit,
                               bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_KEY_TYPE_PRESEL)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56880_a0_fp_common_presel_ext_db_init(unit, stage));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_fp_common_ext_db_init_single(unit, stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_fp_common_ext_db_init_dblinter(unit, stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_fp_common_ext_db_init_triple(unit, stage));
exit:
    SHR_FUNC_EXIT();
}
