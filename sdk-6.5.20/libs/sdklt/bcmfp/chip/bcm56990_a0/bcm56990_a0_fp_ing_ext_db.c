/*! \file bcm56990_a0_fp_ing_ext_db.c
 *
 *  IFP multi hierarchy extractors configuration DB
 *  initialization function for Tomahawk-4(56990_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56990_a0_fp.h>
#include <bcmfp/bcmfp_keygen_api.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_a0_ingress_ext_cfg_db_init(int unit,
                                          bcmfp_stage_t *stage)
{
    int level = 0;                   /* Extractor hierarchy level.          */
    int gran = 0;                    /* Extractor granularity.              */
    int ext_num = 0;                 /* Extractor number.                   */
    int part;                        /* Entry part number.                  */
    bcmfp_keygen_ext_attrs_t ext_attrs;
    bcmfp_keygen_ext_attrs_t temp_attrs;
    bcmfp_keygen_ext_attrs_t *attrs = &temp_attrs;
    uint32_t ext_attrs_size = 0;
    BCMFP_KEYGEN_EXT_CFG_DECL;              /* Extractors config declaration.      */
    bcmfp_keygen_ext_cfg_db_t *ext_cfg_db = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    ext_attrs_size = sizeof(bcmfp_keygen_ext_attrs_t);
    sal_memset(attrs, 0, ext_attrs_size);

    BCMFP_ALLOC(ext_cfg_db,
                sizeof(bcmfp_keygen_ext_cfg_db_t), "bcmfpIngExtCfgDb");
    stage->kgn_ext_cfg_db_arr[BCMFP_KEYGEN_MODE_SINGLE] = ext_cfg_db;

    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2S1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2S1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2S2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2S2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S1, 128, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S2, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2S2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 152, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 168, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2S2, 184, attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 196, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 200, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 204, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 212, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 216, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 220, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 224, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2S2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S1, 0, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S1, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S1, 36, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S1, 52, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2S1, BCMFP_KEYGEN_EXT_SECTION_L3S1, 68, &ext_attrs);
            /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 16, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 84, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 88, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_8_11);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 92, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 96, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 100, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 104, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_CN);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 108, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 112, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 116, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 120, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 124, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 132, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 136, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 144, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 146, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 148, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 150, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 154, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
         BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 158, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2S2, BCMFP_KEYGEN_EXT_SECTION_L3S1, 159, &ext_attrs);

    level = 3;
    gran = 160;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_L3S1, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, &ext_attrs);

    /* Intra Slice Double Wide - 320 bit mode */
    ext_cfg_db = NULL;
    BCMFP_ALLOC(ext_cfg_db,
                sizeof(bcmfp_keygen_ext_cfg_db_t), "bcmfpIngExtCfgDb");
    stage->kgn_ext_cfg_db_arr[BCMFP_KEYGEN_MODE_DBLINTRA] = ext_cfg_db;
    sal_memset(&ext_attrs, 0, ext_attrs_size);

    /* Initialize Level_1 32-bit extractor section for part 0. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section for part 0. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 128, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section for part 0. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 152, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 168, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 184, attrs);

    /* Initialize Level_1 4-bit extractors section for part 0. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 196, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 200, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 204, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 212, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 216, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 220, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 224, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section for part 0. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 36, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 52, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 68, attrs);

    /* Initialize Level_2 4-bit extractors section for part 0. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 16, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 84, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 88, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_8_11);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 92, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 96, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 100, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 104, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_CN);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 108, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 112, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 116, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 120, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 124, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 132, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 136, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section for part 0. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 144, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 146, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 148, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 150, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 154, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section for part 0. */
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
         BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 158, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 159, &ext_attrs);

    level = 3;
    gran = 160;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);

    /* Initialize Level_1 16-bit extractors section for part 1. */
    level = 1;
    gran = 16;
    ext_num = 0;
    part = 1;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 48, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 128, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E16_ISDW, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 144, attrs);

    /* Initialize Level_2 16-bit extractors section for part 1. */
    level = 2;
    gran = 16;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 16, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 48, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 128, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 144, attrs);

    level = 3;
    gran = 160;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);

    /* 320 bits mode - Initialize Level_1 32-bit extractor section. */
    ext_cfg_db = NULL;
    BCMFP_ALLOC(ext_cfg_db,
                   sizeof(bcmfp_keygen_ext_cfg_db_t), "bcmfpIngExtCfgDb");
    stage->kgn_ext_cfg_db_arr[BCMFP_KEYGEN_MODE_DBLINTER] = ext_cfg_db;

    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 128, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 152, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 168, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 184, attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 196, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 200, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 204, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 212, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 216, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 220, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 224, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 36, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 52, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 68, &ext_attrs);
 /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 16, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 84, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 88, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_8_11);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 92, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 96, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 100, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 104, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_CN);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 108, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 112, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 116, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 120, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 124, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 132, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 136, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 144, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 146, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 148, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 150, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 154, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
         BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 158, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 159, &ext_attrs);

    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 1;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 128, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 152, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 168, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 184, attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 196, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 200, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 204, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 212, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 216, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 220, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 224, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 20, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 36, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 52, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 68, &ext_attrs);
    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 16, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 84, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 88, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 92, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 96, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 100, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 104, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 108, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 112, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 116, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 120, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 124, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 132, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 136, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 144, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 146, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 148, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 150, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 154, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 158, &ext_attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 159, &ext_attrs);

    level = 3;
    gran = 160;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);


    /* 480 bit mode - Initialize Level_1 32-bit extractor section. */
    ext_cfg_db = NULL;
    BCMFP_ALLOC(ext_cfg_db,
                   sizeof(bcmfp_keygen_ext_cfg_db_t), "bcmfpIngExtCfgDb");
    stage->kgn_ext_cfg_db_arr[BCMFP_KEYGEN_MODE_TRIPLE] = ext_cfg_db;

    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS1, 128, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 152, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 168, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 184, attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 196, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 200, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 204, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 212, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 216, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 220, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 224, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2AS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 20, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_B);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 36, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_C);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 52, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_CLASS_ID_D);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS1, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 68, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IPBM);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 16, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 84, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 88, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_CLASS_ID_BITS_8_11);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 92, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 96, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 100, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 104, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
        BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_PKT_RESOLUTION);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_CN);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 108, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_INT_PRI);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 112, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_IFP_DROP_VECTOR);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 116, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_MH_OPCODE);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 120, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DST_CONTAINER_A);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 124, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 132, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 136, &ext_attrs);
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 144, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 146, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 148, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 150, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 154, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w,
         BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_EM_FIRST_LOOKUP_HIT);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 158, &ext_attrs);

    sal_memset(&ext_attrs, 0, ext_attrs_size);
    SHR_BITSET(ext_attrs.w, BCMFP_KEYGEN_EXT_ATTR_NOT_WITH_PMUX_DROP);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2AS2, BCMFP_KEYGEN_EXT_SECTION_L3AS1, 159, &ext_attrs);


    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 1;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS1, 128, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 152, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 168, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 184, attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 196, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 200, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 204, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 212, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 216, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 220, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 224, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2BS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 20, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 36, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 52, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS1, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 68, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 16, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 84, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 88, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 92, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 96, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 100, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 104, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 108, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 112, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 116, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 120, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 124, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 132, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 136, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 144, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 146, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 148, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 150, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 154, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 158, &ext_attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2BS2, BCMFP_KEYGEN_EXT_SECTION_L3BS1, 159, &ext_attrs);

    /* Initialize Level_1 32-bit extractor section. */
    level = 1;
    gran = 32;
    ext_num = 0;
    part = 2;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 32, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E32, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 32, attrs);

    /* Initialize Level_1 16-bit extractors section. */
    level = 1;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS1, 128, attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 64, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 80, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 96, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 112, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E16, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 128, attrs);

    /* Initialize Level_1 8-bit extractors section. */
    level = 1;
    gran = 8;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 144, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 152, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 160, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 168, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 176, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E8, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 184, attrs);

    /* Initialize Level_1 4-bit extractors section. */
    level = 1;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 192, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 196, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 200, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 204, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 208, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 212, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 216, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 220, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 224, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L1E4, BCMFP_KEYGEN_EXT_SECTION_L2CS2, 228, attrs);

    /* Initialize Level_2 16-bit extractors section. */
    level = 2;
    gran = 16;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 0, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 20, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 36, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 52, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2CS1, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 68, &ext_attrs);

    /* Initialize Level_2 4-bit extractors section. */
    level = 2;
    gran = 4;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 16, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 84, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 88, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 92, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 96, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 100, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 104, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 7),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 108, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 8),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 112, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 9),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 116, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 10),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 120, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 11),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 124, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 12),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 128, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 13),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 132, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 14),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 136, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 15),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 140, &ext_attrs);

    /* Initialize Level2 2-bit extractors section. */
    level = 2;
    gran = 2;
    ext_num = 0;
    sal_memset(&ext_attrs, 0, ext_attrs_size);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 144, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 146, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 2),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 148, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 3),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 150, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 4),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 152, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 5),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 154, &ext_attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 6),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 156, &ext_attrs);

    /* Initialize Level2 1-bit extractors section. */
    gran = 1;
    ext_num = 0;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, ext_num,
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 158, &ext_attrs);

    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level, gran, (ext_num + 1),
        BCMFP_KEYGEN_EXT_SECTION_L2CS2, BCMFP_KEYGEN_EXT_SECTION_L3CS1, 159, &ext_attrs);


    level = 3;
    gran = 160;
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 0, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_L3AS1, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 1, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_L3BS1, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);
    BCMFP_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, 2, level, gran, 0,
        BCMFP_KEYGEN_EXT_SECTION_L3CS1, BCMFP_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);

exit:
    SHR_FUNC_EXIT();
}
