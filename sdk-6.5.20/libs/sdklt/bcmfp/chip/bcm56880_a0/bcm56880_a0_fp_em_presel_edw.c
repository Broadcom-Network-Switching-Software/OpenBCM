/*! \file bcm56880_a0_fp_em_presel_edw.c
 *
 * Exact-match FP presel policy entry buffer get
 * function for Trident4(56880_A0).
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
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_oper_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56880_a0_fp_em_presel_edw_get(int unit,
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
        /* 21 level-1 4 bit extractors. */
        sbit = 22;
        ebit = 28;
        for (idx = 0; idx < 21; idx++) {
            if (ext_codes->l1_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e4_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }
        /* 5 level-1 16 bit extractors. */
        sbit = 169;
        ebit = 176;
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
        sbit = 209;
        ebit = 214;
        for (idx = 0; idx < 2; idx++) {
            if (ext_codes->l1_e32_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e32_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 6;
            ebit += 6;
        }
        /* 21 level-1 4 bit extractors. */
        sbit = 221;
        ebit = 227;
        for (idx = 21; idx < 42; idx++) {
            if (ext_codes->l1_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e4_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }
        /* 5 level-1 16 bit extractors. */
        sbit = 368;
        ebit = 375;
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
        sbit = 408;
        ebit = 413;
        for (idx = 2; idx < 4; idx++) {
            if (ext_codes->l1_e32_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e32_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 6;
            ebit += 6;
        }

        /* 15 level-2 4 bit extractors. */
        sbit = 420;
        ebit = 425;
        for (idx = 0; idx < 15; idx++) {
            if (ext_codes->l2_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l2_e4_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 6;
            ebit += 6;
        }
        /* 15 level-2 4 bit extractors. */
        sbit = 510;
        ebit = 515;
        for (idx = 21; idx < 36; idx++) {
            if (ext_codes->l2_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l2_e4_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 6;
            ebit += 6;
        }

        /* 2 level-3 1 bit extractors. */
        sbit = 600;
        ebit = 607;
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
        sbit = 616;
        ebit = 623;
        for (idx = 2; idx < 4; idx++) {
            if (ext_codes->l3_e1_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l3_e1_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 8;
            ebit += 8;
        }

        /* 6 level-3 2 bit extractors. */
        sbit = 632;
        ebit = 638;
        for (idx = 0; idx < 6; idx++) {
            if (ext_codes->l3_e2_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l3_e2_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }
        /* 6 level-3 2 bit extractors. */
        sbit = 674;
        ebit = 680;
        for (idx = 6; idx < 12; idx++) {
            if (ext_codes->l3_e2_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l3_e2_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }

        /* 15 level-3 4 bit extractors. */
        sbit = 716;
        ebit = 716;
        for (idx = 0; idx < 15; idx++) {
            if (ext_codes->l3_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                if (ext_codes->l2_e4_sel[idx] !=
                    BCMFP_EXT_SELCODE_DONT_CARE) {
                    fv = 0;
                } else if (ext_codes->l1_e4_sel[idx] !=
                    BCMFP_EXT_SELCODE_DONT_CARE) {
                    fv = 1;
                } else {
                    fv = 0;
                }
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 1;
            ebit += 1;
        }
        /* 15 level-3 4 bit extractors. */
        sbit = 731;
        ebit = 731;
        for (idx = 21; idx < 36; idx++) {
            if (ext_codes->l3_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                if (ext_codes->l2_e4_sel[idx] !=
                    BCMFP_EXT_SELCODE_DONT_CARE) {
                    fv = 0;
                } else if (ext_codes->l1_e4_sel[idx] !=
                    BCMFP_EXT_SELCODE_DONT_CARE) {
                    fv = 1;
                } else {
                    fv = 0;
                }
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 1;
            ebit += 1;
        }

        /*
         * 9 level-1 16 bit extractors. These extractors
         * directly extrs bits to the final key.
         */
        sbit = 746;
        ebit = 753;
        for (idx = 10; idx < 19; idx++) {
            if (ext_codes->l1_e16_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e16_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 8;
            ebit += 8;
        }

        /*
         * 2 level-1 32 bit extractors. These extractors
         * directly extrs bits to the final key.
         */
        sbit = 818;
        ebit = 823;
        for (idx = 4; idx < 6; idx++) {
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
        bcmdrd_field_set(ebuff, 830, 830, &fv);

        /* Copy 830 bits from ebuff to edw[part][0] */
        bcmdrd_field_get(ebuff, 0, 4, edw[part * 2]);
        bcmdrd_field_get(ebuff, 5, 484, edw[(part * 2) + 1]);
        bcmdrd_field_get(ebuff, 485, 830, edw[(part * 2) + 2]);
    }
exit:
    SHR_FUNC_EXIT();
}

