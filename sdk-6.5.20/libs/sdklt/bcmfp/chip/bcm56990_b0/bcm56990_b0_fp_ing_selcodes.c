/*! \file bcm56990_b0_fp_ing_selcodes.c
 *
 *  APIs to intsall/uninstall to configurations to h/w
 *  memories/registers to create/destroy the IFP group for
 *  Tomahawk-4 B0 device(56990_B0).
 *  .
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56990_b0_fp.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmfp/bcmfp_oper_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static uint32_t kgp_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t kgp2_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t kgp_isdw_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];

static bcmdrd_sid_t ing_profile_sids[] = {
                                    IFP_KEY_GEN_PROGRAM_PROFILEm,
                                    IFP_KEY_GEN_PROGRAM_PROFILE2m,
                                    IFP_KEY_GEN_PROGRAM_PROFILE_ISDWm };

static const bcmdrd_fid_t pmux_sel[] =
    {
        POST_EXTRACTOR_MUX_0_SELf,
        POST_EXTRACTOR_MUX_1_SELf,
        POST_EXTRACTOR_MUX_2_SELf,
        POST_EXTRACTOR_MUX_3_SELf,
        POST_EXTRACTOR_MUX_4_SELf,
        POST_EXTRACTOR_MUX_5_SELf,
        POST_EXTRACTOR_MUX_6_SELf,
        POST_EXTRACTOR_MUX_7_SELf,
        POST_EXTRACTOR_MUX_8_SELf,
        POST_EXTRACTOR_MUX_9_SELf,
        POST_EXTRACTOR_MUX_10_SELf,
        POST_EXTRACTOR_MUX_11_SELf,
        POST_EXTRACTOR_MUX_12_SELf,
        POST_EXTRACTOR_MUX_13_SELf,
        POST_EXTRACTOR_MUX_14_SELf
    };

int
bcmfp_bcm56990_b0_ingress_profiles_install(int unit,
                                           uint32_t stage_id,
                                           uint32_t trans_id,
                                           uint32_t flags,
                                           bcmfp_group_id_t group_id,
                                           bcmfp_group_oper_info_t *opinfo,
                                           bcmfp_buffers_t *buffers)
{
    bcmdrd_sid_t kgp_sid = ing_profile_sids[0];
    bcmdrd_sid_t kgp2_sid = ing_profile_sids[1];
    bcmdrd_sid_t kgp_isdw_sid = ing_profile_sids[2];
    uint32_t *profile_data[] = { NULL, NULL, NULL };
    int index = 0;
    uint8_t idx = 0;
    uint8_t part = 0;
    uint8_t num_parts = 0;
    uint32_t value = 0;
    uint32_t *prof_entry = NULL;
    bcmfp_ref_count_t *kgp_ref_count = NULL;
    bcmdrd_fid_t fid;
    uint32_t group_flags = 0;
    size_t size = 0;
    bcmltd_sid_t tbl_id;
    int pipe_id = -1;
    bcmfp_ext_codes_t *ext_codes = NULL;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(buffers, SHR_E_PARAM);

    profile_data[0] = kgp_entry[unit];
    profile_data[1] = kgp2_entry[unit];
    profile_data[2] = kgp_isdw_entry[unit];

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    tbl_id = stage->tbls.group_tbl->sid;
    group_flags = opinfo->flags;
    ext_codes = opinfo->ext_codes;
    pipe_id = opinfo->tbl_inst;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 group_flags,
                                 &num_parts));
    if (opinfo->group_mode == BCMFP_GROUP_MODE_DBLINTRA)  {
        part = 0;
        prof_entry = kgp_entry[unit];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_ext_codes_init(unit,
                                              ext_codes,
                                              part,
                                              kgp_sid,
                                              prof_entry));

        prof_entry = kgp2_entry[unit];
        size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
        sal_memset(prof_entry, 0, size);
        /* Set Post MUX extractor selcode values. */
        for (idx = 0; idx < 15; idx++) {
            if (-1 != ext_codes[part].pmux_sel[idx]) {
                value = ext_codes[part].pmux_sel[idx];
                fid = pmux_sel[idx];
                bcmdrd_pt_field_set(unit,
                                    kgp2_sid,
                                    prof_entry,
                                    fid,
                                    &value);
            }
        }
        part = 1;
        prof_entry = kgp_isdw_entry[unit];
        size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
        sal_memset(prof_entry, 0, size);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_ext_codes_init(unit,
                                              ext_codes,
                                              part,
                                              kgp_isdw_sid,
                                              prof_entry));
        index = -1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_prof_ref_count_get(unit,
                                             stage_id,
                                             pipe_id,
                                             &kgp_ref_count));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_profile_add(unit,
                               trans_id,
                               pipe_id,
                               tbl_id,
                               3,
                               ing_profile_sids,
                               profile_data,
                               kgp_ref_count,
                               &index));
        ext_codes[0].keygen_index = (uint32_t)index;
        ext_codes[1].keygen_index = (uint32_t)index;
    } else {
        for (part = 0; part < num_parts; part++) {

            prof_entry = kgp_entry[unit];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56990_b0_ext_codes_init(unit,
                                                  ext_codes,
                                                  part,
                                                  kgp_sid,
                                                  prof_entry));
            prof_entry = kgp2_entry[unit];
            size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
            sal_memset(prof_entry, 0, size);

            /* Set Post MUX extractor selcode values. */
            for (idx = 0; idx < 15; idx++) {
                if (-1 != ext_codes[part].pmux_sel[idx]) {
                    value = ext_codes[part].pmux_sel[idx];
                    fid = pmux_sel[idx];
                    bcmdrd_pt_field_set(unit,
                            kgp2_sid,
                            prof_entry,
                            fid,
                            &value);
                }
            }

            index = -1;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_keygen_prof_ref_count_get(unit,
                                                 stage_id,
                                                 pipe_id,
                                                 &kgp_ref_count));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_profile_add(unit,
                                   trans_id,
                                   pipe_id,
                                   tbl_id,
                                   3,
                                   ing_profile_sids,
                                   profile_data,
                                   kgp_ref_count,
                                   &index));
            ext_codes[part].keygen_index = (uint32_t)index;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56990_b0_ingress_profiles_uninstall(int unit,
                                             uint32_t stage_id,
                                             uint32_t trans_id,
                                             uint32_t flags,
                                             bcmfp_group_id_t group_id,
                                             bcmfp_group_oper_info_t *opinfo)
{
    int index = -1;
    int tbl_inst = -1;
    uint8_t part = 0;
    uint8_t num_parts = 0;
    bcmfp_ref_count_t *kgp_ref_count = NULL;
    bcmltd_sid_t req_ltid ;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &num_parts));
    tbl_inst = opinfo->tbl_inst;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_keygen_prof_ref_count_get(unit,
                                        stage_id,
                                        tbl_inst,
                                        &kgp_ref_count));

    req_ltid = stage->tbls.group_tbl->sid;
    for (part = 0; part < num_parts; part++) {
        index = (int)opinfo->ext_codes[part].keygen_index;
        SHR_IF_ERR_EXIT(
            bcmfp_profile_delete(unit,
                                 trans_id,
                                 tbl_inst,
                                 req_ltid,
                                 3,
                                 ing_profile_sids,
                                 kgp_ref_count,
                                 index));
    }

exit:
    SHR_FUNC_EXIT();
}

