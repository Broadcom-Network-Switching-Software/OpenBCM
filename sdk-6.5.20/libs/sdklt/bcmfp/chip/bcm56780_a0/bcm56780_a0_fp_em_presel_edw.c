/*! \file bcm56780_a0_fp_em_presel_edw.c
 *
 * Exact-match FP presel policy entry buffer get
 * function for Trident4-X9(56780_A0).
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
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_oper_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56780_a0_fp_em_presel_edw_get(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_group_oper_info_t *opinfo,
                                  uint32_t **edw)
{
    uint32_t fv = 0;
    uint32_t ebuff[32];
    uint8_t part = 0;
    uint8_t num_parts = 0;
    uint16_t sbit = 0;
    uint16_t ebit = 0;
    uint8_t idx = 0;
    bcmfp_ext_codes_t *ext_codes = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(edw, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit, TRUE, opinfo->flags, &num_parts));

    for (part = 0; part < num_parts; part++) {
        ext_codes = &(opinfo->ext_codes[part]);
        sal_memset(ebuff, 0, sizeof(ebuff));
        /* LTID must be present. */
        if (ext_codes->ltid != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->ltid;
            bcmdrd_field_set(ebuff, 0, 4, &fv);
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        /* key_type must be present. */
        if (ext_codes->em_key_attrib_index != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->em_key_attrib_index;
            bcmdrd_field_set(ebuff, 5, 8, &fv);
        }
        /* For wider groups, update pdd, sbr index in last slice */
        if (part == (num_parts - 1)) {

            /* Per entry PDD profile index can exist. */
            if (ext_codes->pdd_prof_index != BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->pdd_prof_index;
                bcmdrd_field_set(ebuff, 9, 13, &fv);
            } else {
                fv = 1;
                bcmdrd_field_set(ebuff, 19, 19, &fv);
            }

            /* Per entry SBR profile index can exist. */
            if (ext_codes->sbr_prof_index != BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->sbr_prof_index;
                bcmdrd_field_set(ebuff, 14, 18, &fv);
            } else {
                fv = 1;
                bcmdrd_field_set(ebuff, 20, 20, &fv);
            }
        }
        /* 10 level-1 4 bit extractors. */
        sbit = 22;
        ebit = 28;
        for (idx = 0; idx < 10; idx++) {
            if (ext_codes->l1_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e4_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }
        /* 5 level-1 16 bit extractors. */
        sbit = 92;
        ebit = 99;
        for (idx = 0; idx < 5; idx++) {
            if (ext_codes->l1_e16_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e16_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 8;
            ebit += 8;
        }
        /* 2 level-1 32 bit extractors. */
        sbit = 132;
        ebit = 137;
        for (idx = 0; idx < 2; idx++) {
            if (ext_codes->l1_e32_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e32_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 6;
            ebit += 6;
        }
        /* 10 level-1 4 bit extractors. */
        sbit = 144;
        ebit = 150;
        for (idx = 10; idx < 20; idx++) {
            if (ext_codes->l1_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e4_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }
        /* 5 level-1 16 bit extractors. */
        sbit = 214;
        ebit = 221;
        for (idx = 5; idx < 10; idx++) {
            if (ext_codes->l1_e16_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e16_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 8;
            ebit += 8;
        }
        /* 2 level-1 32 bit extractors. */
        sbit = 254;
        ebit = 259;
        for (idx = 2; idx < 4; idx++) {
            if (ext_codes->l1_e32_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e32_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 6;
            ebit += 6;
        }

        /* 6 level-2 4 bit extractors. */
        sbit = 266;
        ebit = 271;
        for (idx = 0; idx < 6; idx++) {
            if (ext_codes->l2_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l2_e4_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 6;
            ebit += 6;
        }
        /* 6 level-2 4 bit extractors. */
        sbit = 302;
        ebit = 307;
        for (idx = 10; idx < 16; idx++) {
            if (ext_codes->l2_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l2_e4_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 6;
            ebit += 6;
        }

        /* 2 level-3 1 bit extractors. */
        sbit = 338;
        ebit = 345;
        for (idx = 0; idx < 2; idx++) {
            if (ext_codes->l3_e1_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l3_e1_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 8;
            ebit += 8;
        }
        /* 2 level-3 1 bit extractors. */
        sbit = 354;
        ebit = 361;
        for (idx = 2; idx < 4; idx++) {
            if (ext_codes->l3_e1_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l3_e1_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 8;
            ebit += 8;
        }

        /* 4 level-3 2 bit extractors. */
        sbit = 370;
        ebit = 376;
        for (idx = 0; idx < 4; idx++) {
            if (ext_codes->l3_e2_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l3_e2_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }
        /* 4 level-3 2 bit extractors. */
        sbit = 398;
        ebit = 404;
        for (idx = 4; idx < 8; idx++) {
            if (ext_codes->l3_e2_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l3_e2_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }

        /* 6 level-3 4 bit extractors. */
        sbit = 426;
        ebit = 426;
        for (idx = 0; idx < 6; idx++) {
            if (ext_codes->l3_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                if (ext_codes->l1_e4_sel[idx] !=
                    BCMFP_EXT_SELCODE_DONT_CARE) {
                    fv = 1;
                } else if (ext_codes->l2_e4_sel[idx] !=
                    BCMFP_EXT_SELCODE_DONT_CARE) {
                    fv = 0;
                } else {
                    fv = 0;
                }
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 1;
            ebit += 1;
        }
        /* 6 level-3 4 bit extractors. */
        sbit = 432;
        ebit = 432;
        for (idx = 10; idx < 16; idx++) {
            if (ext_codes->l3_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                if (ext_codes->l1_e4_sel[idx] !=
                    BCMFP_EXT_SELCODE_DONT_CARE) {
                    fv = 1;
                } else if (ext_codes->l2_e4_sel[idx] !=
                    BCMFP_EXT_SELCODE_DONT_CARE) {
                    fv = 0;
                } else {
                    fv = 0;
                }
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 1;
            ebit += 1;
        }

        /*
         * 6 level-1 16 bit extractors. These extractors
         * directly extrs bits to the final key.
         */
        sbit = 438;
        ebit = 445;
        for (idx = 10; idx < 16; idx++) {
            if (ext_codes->l1_e16_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e16_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 8;
            ebit += 8;
        }

        /*
         * 6 level-1 32 bit extractors. These extractors
         * directly extrs bits to the final key.
         */
        sbit = 486;
        ebit = 491;
        for (idx = 4; idx < 10; idx++) {
            if (ext_codes->l1_e32_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e32_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 6;
            ebit += 6;
        }

        /* Set the valid bit */
        fv = 1;
        bcmdrd_field_set(ebuff, 522, 522, &fv);

        /*
         * Copy 522 bits from ebuff to edw[part][0] as per TLB format
         * DATA0 - 395: 80 [316 bits]
         * DATA1 - 795:480 [316 bits]
         */
        SHR_BITCOPY_RANGE(&(edw[part][0]), 80, ebuff, 0, 316);
        SHR_BITCOPY_RANGE(&(edw[part][0]), 480, ebuff, 316, 316);
    }
exit:
    SHR_FUNC_EXIT();
}

