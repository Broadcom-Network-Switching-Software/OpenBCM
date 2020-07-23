/*! \file bcm56780_a0_fp_vlan_presel_edw.c
 *
 * IFP/VFP/EFP presel policy entry buffer get
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
bcm56780_a0_fp_vlan_presel_edw_get(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_group_oper_info_t *opinfo,
                                  uint32_t **edw)
{
    uint32_t fv = 0;
    uint32_t ebuff[16];
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
        /* Lookup mode */
        (part) ? (fv = 1) : (fv = 0);
        bcmdrd_field_set(ebuff, 0, 0, &fv);
        /* LTID must be present. */
        if (ext_codes->ltid != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->ltid;
            bcmdrd_field_set(ebuff, 1, 4, &fv);
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        /* key_type must be present. */
        if (ext_codes->key_type != BCMFP_EXT_SELCODE_DONT_CARE) {
            fv = ext_codes->key_type;
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
        /* 22 level-1 4 bit extractors. */
        sbit = 21;
        ebit = 27;
        for (idx = 0; idx < 22; idx++) {
            if (ext_codes->l1_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e4_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }
        /* 5 level-1 16 bit extractors. */
        sbit = 175;
        ebit = 182;
        for (idx = 5; idx < 10; idx++) {
            if (ext_codes->l1_e16_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e16_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }
        /* 2 level-1 32 bit extractors. */
        sbit = 210;
        ebit = 214;
        for (idx = 0; idx < 2; idx++) {
            if (ext_codes->l1_e32_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e32_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 5;
            ebit += 5;
        }
        /* 15 level-2 4 bit extractors. */
        sbit = 220;
        ebit = 225;
        for (idx = 0; idx < 15; idx++) {
            if (ext_codes->l2_e4_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l2_e4_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 6;
            ebit += 6;
        }
        /* 2 level-3 1 bit extractors. */
        sbit = 310;
        ebit = 317;
        for (idx = 0; idx < 2; idx++) {
            if (ext_codes->l3_e1_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l3_e1_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 8;
            ebit += 8;
        }
        /* 7 level-3 2 bit extractors. */
        sbit = 326;
        ebit = 332;
        for (idx = 0; idx < 7; idx++) {
            if (ext_codes->l3_e2_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l3_e2_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }
        /* 15 level-3 4 bit extractors. */
        sbit = 375;
        ebit = 375;
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
        /*
         * 5 level-1 16 bit extractors. These extractors
         * directly extrs bits to the final key.
         */
        sbit = 390;
        ebit = 396;
        for (idx = 0; idx < 5; idx++) {
            if (ext_codes->l1_e16_sel[idx] !=
                BCMFP_EXT_SELCODE_DONT_CARE) {
                fv = ext_codes->l1_e16_sel[idx];
                bcmdrd_field_set(ebuff, sbit, ebit, &fv);
            }
            sbit += 7;
            ebit += 7;
        }

        /* Set the valid bit */
        fv = opinfo->enable;
        bcmdrd_field_set(ebuff, 425, 425, &fv);

        /*
         * Copy 426 bits from ebuff to edw[part][0] as per TLB format
         * DATA0 - 331: 80 [252 bits]
         * DATA1 - 667:416 [252 bits]
         */
        SHR_BITCOPY_RANGE(&(edw[part][0]), 80, ebuff, 0, 252);
        SHR_BITCOPY_RANGE(&(edw[part][0]), 416, ebuff, 252, 252);
    }
exit:
    SHR_FUNC_EXIT();
}

