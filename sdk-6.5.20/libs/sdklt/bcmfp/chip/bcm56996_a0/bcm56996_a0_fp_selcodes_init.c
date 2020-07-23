/*! \file bcm56996_a0_fp_selcodes_init.c
 *
 * APIs to intsall/uninstall to configurations to h/w
 * memories/registers to create/destroy the IFP group for
 * Tomahawk-4G device(56996_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static const bcmdrd_fid_t l1_32_sel[] = /* 32 bit extractors. */
{
    L1_C_E32_SEL_0f,
    L1_C_E32_SEL_1f,
    L1_B_E32_SEL_0f,
    L1_B_E32_SEL_1f
};
static const bcmdrd_fid_t l1_16_sel[] = /* 16 bit extractors. */
{
    L1_C_E16_SEL_0f,
    L1_C_E16_SEL_1f,
    L1_C_E16_SEL_2f,
    L1_C_E16_SEL_3f,
    L1_C_E16_SEL_4f,
    L1_B_E16_SEL_0f,
    L1_B_E16_SEL_1f,
    L1_B_E16_SEL_2f,
    L1_B_E16_SEL_3f,
    L1_B_E16_SEL_4f,
};
static const bcmdrd_fid_t l1_16_sel_isdw[] = /* 16 bit extractors. */
{
    L1_ISDW_E16_SEL_0f,
    L1_ISDW_E16_SEL_1f,
    L1_ISDW_E16_SEL_2f,
    L1_ISDW_E16_SEL_3f,
    L1_ISDW_E16_SEL_4f,
    L1_ISDW_E16_SEL_5f,
    L1_ISDW_E16_SEL_6f,
    L1_ISDW_E16_SEL_7f,
    L1_ISDW_E16_SEL_8f,
    L1_ISDW_E16_SEL_9f,
};
static const bcmdrd_fid_t l1_8_sel[] = /* 8 bit extractors. */
{
    L1_A_E8_SEL_0f,
    L1_A_E8_SEL_1f,
    L1_A_E8_SEL_2f,
    L1_A_E8_SEL_3f,
    L1_A_E8_SEL_4f,
    L1_A_E8_SEL_5f,
};
static const bcmdrd_fid_t l1_4_sel[] = /* 4 bit extractors. */
{
    L1_A_E4_SEL_0f,
    L1_A_E4_SEL_1f,
    L1_A_E4_SEL_2f,
    L1_A_E4_SEL_3f,
    L1_A_E4_SEL_4f,
    L1_A_E4_SEL_5f,
    L1_A_E4_SEL_6f,
    L1_A_E4_SEL_7f,
    L1_A_E4_SEL_8f,
    L1_A_E4_SEL_9f
};

static const bcmdrd_fid_t l2_16_sel[] = /* 16 bit extractors. */
{
    L2_E16_SEL_0f,
    L2_E16_SEL_1f,
    L2_E16_SEL_2f,
    L2_E16_SEL_3f,
    L2_E16_SEL_4f,
};
static const bcmdrd_fid_t l2_4_sel[] = /* 4 bit extractors. */
{
    L2_E4_SEL_0f,
    L2_E4_SEL_1f,
    L2_E4_SEL_2f,
    L2_E4_SEL_3f,
    L2_E4_SEL_4f,
    L2_E4_SEL_5f,
    L2_E4_SEL_6f,
    L2_E4_SEL_7f,
    L2_E4_SEL_8f,
    L2_E4_SEL_9f,
    L2_E4_SEL_10f,
    L2_E4_SEL_11f,
    L2_E4_SEL_12f,
    L2_E4_SEL_13f,
    L2_E4_SEL_14f,
    L2_E4_SEL_15f,
};
static const bcmdrd_fid_t l2_2_sel[] = /* 2 bit extractors. */
{
    L2_E2_SEL_0f,
    L2_E2_SEL_1f,
    L2_E2_SEL_2f,
    L2_E2_SEL_3f,
    L2_E2_SEL_4f,
    L2_E2_SEL_5f,
    L2_E2_SEL_6f
};
static const bcmdrd_fid_t l2_1_sel[] = /* 1 bit extractors. */
{
    L2_E1_SEL_0f,
    L2_E1_SEL_1f,
};


int
bcmfp_bcm56996_a0_ext_codes_init(int unit,
                                 bcmfp_ext_codes_t *ext_codes,
                                 uint8_t part,
                                 bcmdrd_sid_t kgp_sid,
                                 uint32_t *kgp_entry)
{

    uint8_t idx = 0;
    uint32_t value = 0;
    uint32_t *prof_entry = NULL;
    bcmdrd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(kgp_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(ext_codes, SHR_E_PARAM);

    prof_entry = kgp_entry;
    sal_memset(prof_entry, 0, sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);

    if ((kgp_sid == IFP_KEY_GEN_PROGRAM_PROFILE_ISDWm) && (part == 1)) {
        /* Set 16bit extractor selcode values. */
        for (idx = 0; idx < 10; idx++) {
            if (-1 != ext_codes[part].l1_e16_sel[idx]) {
                value = ext_codes[part].l1_e16_sel[idx];
                fid = l1_16_sel_isdw[idx];
                bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
            }
        }
        SHR_EXIT();
    }

    /* Set 32bit extractor selcode values. */
    for (idx = 0; idx < 4; idx++) {
        if (-1 != ext_codes[part].l1_e32_sel[idx]) {
            value = ext_codes[part].l1_e32_sel[idx];
            fid = l1_32_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 10; idx++) {
        if (-1 != ext_codes[part].l1_e16_sel[idx]) {
            value = ext_codes[part].l1_e16_sel[idx];
            fid = l1_16_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 8bit extractor selcode values. */
    for (idx = 0; idx < 6; idx++) {
        if (-1 != ext_codes[part].l1_e8_sel[idx]) {
            value = ext_codes[part].l1_e8_sel[idx];
            fid = l1_8_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 10; idx++) {
        if (-1 != ext_codes[part].l1_e4_sel[idx]) {
            value = ext_codes[part].l1_e4_sel[idx];
            fid = l1_4_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 16bit extractor selcode values. */
    for (idx = 0; idx < 5; idx++) {
        if (-1 != ext_codes[part].l2_e16_sel[idx]) {
            value = ext_codes[part].l2_e16_sel[idx];
            fid = l2_16_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 4bit extractor selcode values. */
    for (idx = 0; idx < 16; idx++) {
        if (-1 != ext_codes[part].l2_e4_sel[idx]) {
            value = ext_codes[part].l2_e4_sel[idx];
            fid = l2_4_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 2bit extractor selcode values. */
    for (idx = 0; idx < 7; idx++) {
        if (-1 != ext_codes[part].l2_e2_sel[idx]) {
            value = ext_codes[part].l2_e2_sel[idx];
            fid = l2_2_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }

    /* Set 1bit extractor selcode values. */
    for (idx = 0; idx < 2; idx++) {
        if (-1 != ext_codes[part].l2_e1_sel[idx]) {
            value = ext_codes[part].l2_e1_sel[idx];
            fid = l2_1_sel[idx];
            bcmdrd_pt_field_set(unit, kgp_sid, prof_entry, fid, &value);
        }
    }



exit:
    SHR_FUNC_EXIT();
}
