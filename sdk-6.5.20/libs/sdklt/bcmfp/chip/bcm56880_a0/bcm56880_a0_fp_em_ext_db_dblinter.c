/*! \file bcm56880_a0_fp_em_ext_db_dblinter.c
 *
 * FP exact match single wide multi hierarchy extractors
 * configuration DB initialization function for
 * Trident4(56880_A0).
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
bcm56880_a0_fp_em_ext_db_init_dblinter(int unit,
                                       bcmfp_stage_t *stage)
{
    uint8_t idx = 0;
    uint8_t level = 0;
    uint8_t part = 0;
    uint16_t gran = 0;
    uint16_t ext_num = 0;
    uint16_t offset_one = 0;
    uint16_t offset_two = 0;
    bcmfp_keygen_ext_attrs_t ext_attrs;
    bcmfp_keygen_ext_cfg_db_t *ext_cfg_db = NULL;
    BCMFP_KEYGEN_EXT_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sal_memset(&ext_attrs, 0, sizeof(bcmfp_keygen_ext_attrs_t));

    BCMFP_ALLOC(ext_cfg_db,
                sizeof(bcmfp_keygen_ext_cfg_db_t), "bcmfpIngExtCfgDb");
    stage->kgn_ext_cfg_db_arr[BCMFP_KEYGEN_MODE_DBLINTER] = ext_cfg_db;

    /* #############Initialize Level-1 16-bit extractors############# */
    level = 1;
    gran = 16;
    ext_num = 0;
    for (idx = 0; idx < 5; idx++) {
        offset_one = (gran * idx);
        offset_two = 32 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E16, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L2S1, offset_one);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 1, BCMFP_KEYGEN_EXT_SECTION_L3S1, offset_two);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    ext_num = 5;
    for (idx = 0; idx < 5; idx++) {
        offset_one = (gran * idx);
        offset_two = 32 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E16, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L2S2, offset_one);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 1, BCMFP_KEYGEN_EXT_SECTION_L3S2, offset_two);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    ext_num = 10;
    for (idx = 0; idx < 5; idx++) {
        offset_one = 148 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E16, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_FK, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    /* #############Initialize Level-1 32-bit extractors############# */
    level = 1;
    gran = 32;
    ext_num = 0;
    for (idx = 0; idx < 2; idx++) {
        offset_one = 80 + (gran * idx);
        offset_two = 112 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E32, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L2S1, offset_one);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 1, BCMFP_KEYGEN_EXT_SECTION_L3S1, offset_two);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    ext_num = 2;
    for (idx = 0; idx < 2; idx++) {
        offset_one = 80 + (gran * idx);
        offset_two = 112 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E32, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L2S2, offset_one);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 1, BCMFP_KEYGEN_EXT_SECTION_L3S2, offset_two);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    /* #############Initialize Level-1 4-bit extractors############# */
    level = 1;
    gran = 4;
    ext_num = 0;
    for (idx = 0; idx < 13; idx++) {
        offset_one =  (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_ONEHOT_PLGE_ADD
            (unit, 0, part, 3, gran, (ext_num + idx));
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E4, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L3S3, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }
    ext_num = 13;
    for (idx = 0; idx < 2; idx++) {
        offset_one = 52 + (gran * idx);
        offset_two = (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_ONEHOT_PLGE_ADD
            (unit, 0, part, 3, gran, (ext_num + idx));
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E4, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L3S3, offset_one);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 1, BCMFP_KEYGEN_EXT_SECTION_L3S1, offset_two);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }
    ext_num = 15;
    for (idx = 0; idx < 6; idx++) {
        offset_one = 8 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E4, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L3S1, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    ext_num = 21;
    for (idx = 0; idx < 13; idx++) {
        offset_one =  (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_ONEHOT_PLGE_ADD
            (unit, 0, part, 3, gran, (ext_num + idx));
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E4, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L3S4, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }
    ext_num = 34;
    for (idx = 0; idx < 2; idx++) {
        offset_one = 52 + (gran * idx);
        offset_two = (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_ONEHOT_PLGE_ADD
            (unit, 0, part, 3, gran, (ext_num + idx));
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E4, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L3S4, offset_one);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 1, BCMFP_KEYGEN_EXT_SECTION_L3S2, offset_two);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }
    ext_num = 36;
    for (idx = 0; idx < 6; idx++) {
        offset_one = 8 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L1E4, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L3S2, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    /* #############Initialize Level-2 4-bit extractors############# */
    level = 2;
    gran = 4;
    ext_num = 0;
    for (idx = 0; idx < 15; idx++) {
        offset_one = 60 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_ONEHOT_PLGE_ADD
            (unit, 0, part, 3, gran, (ext_num + idx));
        BCMFP_KEYGEN_EXT_CONFLICT_PLGE_ADD
            (unit, 0, part, 1, gran, (ext_num + idx));
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L2S1, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L3S3, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    ext_num = 21;
    for (idx = 0; idx < 15; idx++) {
        offset_one = 60 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_ONEHOT_PLGE_ADD
            (unit, 0, part, 3, gran, (ext_num + idx));
        BCMFP_KEYGEN_EXT_CONFLICT_PLGE_ADD
            (unit, 0, part, 1, gran, (ext_num + idx));
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L2S2, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_L3S4, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    /* #############Initialize Level-3 4-bit extractors############# */
    level = 3;
    gran = 4;
    ext_num = 0;
    for (idx = 0; idx < 15; idx++) {
        offset_one = 28 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L3S3, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_FK, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    ext_num = 21;
    for (idx = 0; idx < 15; idx++) {
        offset_one = 88 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L3S4, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_FK, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    /* #############Initialize Level-3 2-bit extractors############# */
    level = 3;
    gran = 2;
    ext_num = 0;
    for (idx = 0; idx < 6; idx++) {
        offset_one = 4 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L3S1, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_FK, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    ext_num = 6;
    for (idx = 0; idx < 6; idx++) {
        offset_one = 16 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L3S2, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_FK, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    /* #############Initialize Level-3 1-bit extractors############# */
    level = 3;
    gran = 1;
    ext_num = 0;
    for (idx = 0; idx < 2; idx++) {
        offset_one = (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L3S1, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_FK, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    ext_num = 2;
    for (idx = 0; idx < 2; idx++) {
        offset_one = 2 + (gran * idx);
        BCMFP_KEYGEN_EXT_CFG_INIT(unit);
        BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num + idx);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_L3S2, 0);
        BCMFP_KEYGEN_EXT_SECTION_ADD
            (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_FK, offset_one);
        BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
    }

    /* ###############Initialize Level-4 extractors############### */
    level = 4;
    gran = 228;
    ext_num = 0;
    offset_one = 0;
    BCMFP_KEYGEN_EXT_CFG_INIT(unit);
    BCMFP_KEYGEN_EXT_PLGE_ADD(unit, part, level, gran, ext_num);
    BCMFP_KEYGEN_EXT_SECTION_ADD
        (unit, 1, 0, BCMFP_KEYGEN_EXT_SECTION_FK, 0);
    BCMFP_KEYGEN_EXT_SECTION_ADD
        (unit, 0, 0, BCMFP_KEYGEN_EXT_SECTION_DISABLE, offset_one);
    BCMFP_KEYGEN_EXT_CFG_INSERT(unit, ext_cfg_db, &ext_attrs);
exit:
    SHR_FUNC_EXIT();
}
