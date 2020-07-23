/*! \file bcm56780_a0_fp_common_presel_ext_db.c
 *
 * IFP/VFP/EFP presel TCAM  extractors configuration DB
 * initialization function for Trident4-X9(56780_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56780_a0_fp.h>
#include <bcmfp/bcmfp_keygen_api.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56780_a0_fp_common_presel_ext_db_init(int unit,
                                      bcmfp_stage_t *stage)
{
    uint32_t ext_attrs_size = 0;
    bcmfp_keygen_ext_attrs_t ext_attrs;
    bcmfp_keygen_ext_cfg_db_t *ext_cfg_db = NULL;
    BCMFP_KEYGEN_EXT_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    BCMFP_ALLOC(ext_cfg_db,
                sizeof(bcmfp_keygen_ext_cfg_db_t), "bcmfpIngPreselExtCfgDb");
    stage->kgn_presel_ext_cfg_db_arr[BCMFP_KEYGEN_MODE_SINGLE] = ext_cfg_db;

    ext_attrs_size = sizeof(bcmfp_keygen_ext_attrs_t);
    sal_memset(&ext_attrs, 0, ext_attrs_size);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 4, 0,
                             BCMFP_KEYGEN_EXT_SECTION_L1E4,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 4, 1,
                             BCMFP_KEYGEN_EXT_SECTION_L1E4,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             4, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 4, 2,
                             BCMFP_KEYGEN_EXT_SECTION_L1E4,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             8, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 4, 3,
                             BCMFP_KEYGEN_EXT_SECTION_L1E4,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             12, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 4, 4,
                             BCMFP_KEYGEN_EXT_SECTION_L1E4,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             16, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 4, 5,
                             BCMFP_KEYGEN_EXT_SECTION_L1E4,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             20, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 4, 6,
                             BCMFP_KEYGEN_EXT_SECTION_L1E4,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             24, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 4, 7,
                             BCMFP_KEYGEN_EXT_SECTION_L1E4,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             28, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 8, 0,
                             BCMFP_KEYGEN_EXT_SECTION_L1E8,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             32, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 8, 1,
                             BCMFP_KEYGEN_EXT_SECTION_L1E8,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             40, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 8, 2,
                             BCMFP_KEYGEN_EXT_SECTION_L1E8,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             48, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 8, 3,
                             BCMFP_KEYGEN_EXT_SECTION_L1E8,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             56, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 8, 4,
                             BCMFP_KEYGEN_EXT_SECTION_L1E8,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             64, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 1, 8, 5,
                             BCMFP_KEYGEN_EXT_SECTION_L1E8,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             72, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, 2, 80, 0,
                             BCMFP_KEYGEN_EXT_SECTION_FK,
                             BCMFP_KEYGEN_EXT_SECTION_DISABLE,
                             0, &ext_attrs);
exit:
    SHR_FUNC_EXIT();
}
