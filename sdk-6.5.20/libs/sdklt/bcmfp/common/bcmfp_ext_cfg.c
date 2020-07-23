/*! \file bcmfp_ext_cfg.c
 *
 * APIs to operate on FP extractor configurations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_ext_cfg_db_cleanup(int unit,
                         bcmfp_stage_t *stage)
{
    uint8_t level = 0;
    bcmfp_keygen_mode_t  kgmode;
    bcmfp_keygen_ext_section_t kgsection;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* Main TCAM extractor database. */
    for (kgmode = 0; kgmode < BCMFP_KEYGEN_MODE_COUNT; kgmode++) {

        if (stage->kgn_ext_cfg_db_arr[kgmode] == NULL) {
            continue;
        }

        for (level = 0; level < BCMFP_KEYGEN_EXT_LEVEL_COUNT; level++) {
            SHR_FREE(stage->kgn_ext_cfg_db_arr[kgmode]->ext_cfg[level]);
        }

        for (kgsection = 0;
             kgsection < BCMFP_KEYGEN_EXT_SECTION_COUNT;
             kgsection++) {
            SHR_FREE(stage->kgn_ext_cfg_db_arr[kgmode]->sec_cfg[kgsection]);
        }

        SHR_FREE(stage->kgn_ext_cfg_db_arr[kgmode]);
    }

    /* Presel TCAM extractor database. */
    for (kgmode = 0; kgmode < BCMFP_KEYGEN_MODE_COUNT; kgmode++) {

        if (stage->kgn_presel_ext_cfg_db_arr[kgmode] == NULL) {
            continue;
        }

        for (level = 0; level < BCMFP_KEYGEN_EXT_LEVEL_COUNT; level++) {
            SHR_FREE(stage->kgn_presel_ext_cfg_db_arr[kgmode]->ext_cfg[level]);
        }

        for (kgsection = 0;
             kgsection < BCMFP_KEYGEN_EXT_SECTION_COUNT;
             kgsection++) {
            SHR_FREE
                (stage->kgn_presel_ext_cfg_db_arr[kgmode]->sec_cfg[kgsection]);
        }

        SHR_FREE(stage->kgn_presel_ext_cfg_db_arr[kgmode]);
    }
exit:
    SHR_FUNC_EXIT();
}
