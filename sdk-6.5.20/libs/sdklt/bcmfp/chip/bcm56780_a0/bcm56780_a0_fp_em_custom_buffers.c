/*! \file bcm56780_a0_fp_em_custom_buffers.c
 *
 * APIs to intsall/uninstall to configurations to h/w
 * memories/registers to create/destroy FP EM group for
 * Trident4-X9 device(56780_A0).
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmptm/bcmptm_uft.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_pdd_internal.h>
#include <bcmfp/bcmfp_cth_pdd.h>
#include <bcmfp/bcmfp_ptm_internal.h>
#include <bcmfp/bcmfp_chip_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static uint32_t kap_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t kmsk_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t key_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t data_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];

#define BCM56780_A0_EXACT_MATCH_NUM_LOOKUPS (2)

STATIC int
bcm56780_a0_keygen_mask_init(int unit,
                             uint32_t trans_id,
                             bcmfp_stage_t *stage,
                             bcmfp_group_id_t group_id,
                             bcmfp_group_oper_info_t *opinfo,
                             uint8_t part,
                             bcmdrd_sid_t kgp_mask_sid,
                             uint32_t *kgp_mask_entry)
{
    int rv = 0;
    uint32_t offset = 0;
    uint32_t *prof_entry = NULL;
    uint32_t entry_buffer[12] = {0};
    bcmfp_stage_id_t stage_id;
    bcmfp_qual_offset_info_t q_offset;
    size_t size = 0;
    bcmltd_sid_t sid = BCMLTD_SID_INVALID;
    bcmltd_fid_t qual_fid = 0;
    uint16_t num_fid = 0;
    uint32_t arr_idx = 0, arr_depth = 0;
    shr_error_t rv_except = SHR_E_NOT_FOUND;
    const bcmlrd_field_data_t *field_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(kgp_mask_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->tbls.rule_tbl != NULL ) {
        sid = stage->tbls.rule_tbl->sid;
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    stage_id = stage->stage_id;

    prof_entry = kgp_mask_entry;
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);

    size = sizeof(bcmfp_qual_offset_info_t);
    num_fid = stage->tbls.group_tbl->fid_count;
    for (qual_fid = 0; qual_fid < num_fid; qual_fid++) {
        rv = bcmlrd_field_get(unit, sid, qual_fid, &field_data);
        if (rv == SHR_E_UNAVAIL) {
            continue;
        }
        if (field_data->depth) {
            arr_depth = field_data->depth;
        } else {
            arr_depth = 1;
        }
        for (arr_idx = 0; arr_idx < arr_depth; arr_idx++) {
            sal_memset(&q_offset, 0, size);
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (bcmfp_group_qual_offset_info_get(unit,
                                                  stage_id,
                                                  group_id,
                                                  opinfo,
                                                  FALSE,
                                                  part,
                                                  qual_fid,
                                                  arr_idx,
                                                  &q_offset),
                                                  rv_except);
            for (offset = 0;
                 offset < q_offset.num_offsets;
                 offset++) {
                if (q_offset.width[offset] > 0) {
                    SHR_BITSET_RANGE(entry_buffer,
                                     q_offset.offset[offset],
                                     q_offset.width[offset]);
                }
            }
        }
    }

    bcmdrd_pt_field_set(unit,
                        kgp_mask_sid,
                        prof_entry,
                        DATAf,
                        entry_buffer);
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_keymask_install(int unit,
                               uint32_t trans_id,
                               bcmfp_stage_t *stage,
                               void *group_opinfo)
{
    bcmdrd_sid_t kmask_sid = IFTA90_E2T_00_KEY_MASK_TABLEm;
    uint32_t *profile_data[] = { kmsk_entry[unit] };
    bcmfp_group_oper_info_t *opinfo = NULL;
    uint32_t *prof_entry = NULL;
    uint32_t ltid = 0;
    uint32_t idx = 0, num_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    size_t size = 0;
    bcmltd_sid_t tbl_id;
    int pipe_id = -1;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    bcmfp_group_id_t group_id = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    tbl_id = stage->tbls.group_tbl->sid;
    ltid = stage->tbls.entry_tbl->sid;
    pipe_id = opinfo->tbl_inst;
    group_id = opinfo->group_id;

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_KEY_MASK_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }

    prof_entry = kmsk_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_keygen_mask_init(unit,
                                      trans_id,
                                      stage,
                                      group_id,
                                      opinfo,
                                      0,
                                      kmask_sid,
                                      prof_entry));

    for (idx = 0; idx < num_profiles; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_insert(unit,
                                    trans_id,
                                    pipe_id,
                                    tbl_id,
                                    sid_arr[idx],
                                    profile_data[0],
                                    opinfo->action_res_id));
    }

exit:
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_keymask_uninstall(int unit,
                                 uint32_t trans_id,
                                 void *group_opinfo,
                                 bcmfp_stage_t *stage)
{
    int tbl_inst = -1;
    uint32_t ltid = 0;
    uint32_t idx = 0, num_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    bcmltd_sid_t req_ltid;
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    tbl_inst = opinfo->tbl_inst;
    req_ltid = stage->tbls.group_tbl->sid;
    ltid = stage->tbls.entry_tbl->sid;

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_KEY_MASK_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }

    req_ltid = stage->tbls.group_tbl->sid;

    for (idx = 0; idx < num_profiles; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_delete(unit,
                                    trans_id,
                                    tbl_inst,
                                    req_ltid,
                                    sid_arr[idx],
                                    opinfo->action_res_id));
    }

exit:
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_dpolicy_install(int unit,
                               uint32_t trans_id,
                               bcmfp_stage_t *stage,
                               void *group_opinfo,
                               uint32_t *edw)
{
    uint8_t lookup_id = 0;
    uint32_t value = 0;
    uint32_t miss_ltpr_en = 0;
    uint32_t *prof_entry = NULL;
    bcmdrd_fid_t fid;
    uint32_t *profile_data[] = { kap_entry[unit] };
    uint32_t miss_ltpr_profile_data = 0;
    uint32_t em_default_policy_sid;
    uint32_t em_miss_ltpr_sid;
    uint32_t ltid = 0;
    uint32_t idx = 0, num_profiles = 0;
    uint32_t num_ltpr_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    bcmfp_group_oper_info_t *opinfo = NULL;
    int pipe_id = -1;
    bcmltd_sid_t tbl_id;
    bcmfp_ext_codes_t *ext_codes = NULL;
    size_t size = 0;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ltpr_sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    pipe_id = opinfo->tbl_inst;
    ext_codes = &(opinfo->ext_codes[0]);
    tbl_id = stage->tbls.group_tbl->sid;
    ltid = stage->tbls.entry_tbl->sid;

    prof_entry = kap_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);

    em_default_policy_sid = IFTA90_E2T_00_ARRAY_MISS_POLICYm;
    em_miss_ltpr_sid = IFTA90_E2T_00_MISS_LTPR_PROFILE_TABLEm;

    fid = DATAf;
    bcmdrd_pt_field_set(unit,
                        em_default_policy_sid,
                        prof_entry,
                        fid,
                        edw);

    value = ext_codes->default_pdd_prof_index;
    fid = PDD_PROFILE_INDEXf;
    bcmdrd_pt_field_set(unit,
                        em_default_policy_sid,
                        prof_entry,
                        fid,
                        &value);

    value = ext_codes->default_sbr_prof_index;
    fid = SBR_PROFILE_INDEXf;
    bcmdrd_pt_field_set(unit,
                        em_default_policy_sid,
                        prof_entry,
                        fid,
                        &value);

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    BCMFP_ALLOC(ltpr_sid_arr,
                (r_cnt * sizeof(bcmdrd_sid_t)),
                "bcmfpLtprSidArr");
    miss_ltpr_en = 0;
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));

       ptid = ptinfo[BCMFP_EM_LKUP_MISS_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }

        ptid = ptinfo[BCMFP_EM_LKUP_MISS_LTPR_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          ltpr_sid_arr,
                                          num_ltpr_profiles,
                                          ptid)) {
            ltpr_sid_arr[num_ltpr_profiles] = ptid;
            num_ltpr_profiles++;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_lookup_id_get(unit,
                                                  ptsid_arr[idx],
                                                  &lookup_id));
        SHR_BITSET(&miss_ltpr_en, lookup_id);
    }

    fid = ENABLEf;
    bcmdrd_pt_field_set(unit,
                        em_miss_ltpr_sid,
                        &miss_ltpr_profile_data,
                        fid,
                        &miss_ltpr_en);

    for (idx = 0; idx < num_profiles; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_insert(unit,
                                    trans_id,
                                    pipe_id,
                                    tbl_id,
                                    sid_arr[idx],
                                    profile_data[0],
                                    opinfo->action_res_id));
    }

    for (idx = 0; idx < num_ltpr_profiles; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_insert(unit,
                                    trans_id,
                                    pipe_id,
                                    tbl_id,
                                    ltpr_sid_arr[idx],
                                    &miss_ltpr_profile_data,
                                    opinfo->action_res_id));
    }

exit:
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (ltpr_sid_arr != NULL) {
        SHR_FREE(ltpr_sid_arr);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_dpolicy_uninstall(int unit,
                                 uint32_t trans_id,
                                 void *group_opinfo,
                                 bcmfp_stage_t *stage)
{
    uint32_t ltid = 0;
    uint32_t idx = 0, num_profiles = 0;
    uint32_t num_ltpr_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    bcmltd_sid_t req_ltid;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ltpr_sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;
    int pipe_id = -1;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    pipe_id = opinfo->tbl_inst;
    req_ltid = stage->tbls.group_tbl->sid;
    ltid = stage->tbls.entry_tbl->sid;

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    BCMFP_ALLOC(ltpr_sid_arr,
                (r_cnt * sizeof(bcmdrd_sid_t)),
                "bcmfpLtprSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_LKUP_MISS_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }

        ptid = ptinfo[BCMFP_EM_LKUP_MISS_LTPR_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          ltpr_sid_arr,
                                          num_ltpr_profiles,
                                          ptid)) {
            ltpr_sid_arr[num_ltpr_profiles] = ptid;
            num_ltpr_profiles++;
        }
    }

    for (idx = 0; idx < num_profiles; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_delete(unit,
                                    trans_id,
                                    pipe_id,
                                    req_ltid,
                                    sid_arr[idx],
                                    opinfo->action_res_id));
    }
    for (idx = 0; idx < num_ltpr_profiles; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_delete(unit,
                                    trans_id,
                                    pipe_id,
                                    req_ltid,
                                    ltpr_sid_arr[idx],
                                    opinfo->action_res_id));
    }

exit:
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (ltpr_sid_arr != NULL) {
        SHR_FREE(ltpr_sid_arr);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_psg_profile_install(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_t *stage,
                                   void *group_opinfo,
                                   uint32_t *psg_data)
{
    int index = 0;
    uint32_t *prof_entry = NULL;
    bcmfp_ref_count_t *ref_counts = NULL;
    bcmdrd_fid_t fid;
    bcmdrd_sid_t em_psg_profile_sid = 0;
    uint32_t data_size = 0;
    uint32_t **profile_data = NULL;
    uint32_t ltid = 0;
    uint32_t idx = 0, num_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    bcmfp_group_oper_info_t *opinfo = NULL;
    size_t size = 0;
    bcmfp_stage_id_t stage_id;
    int pipe_id = -1;
    bcmltd_sid_t tbl_id;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    bcmfp_ext_codes_t *ext_codes = NULL;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(psg_data, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    prof_entry = kap_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);

    stage_id = stage->stage_id;
    tbl_id = stage->tbls.group_tbl->sid;
    ltid = stage->tbls.entry_tbl->sid;
    ext_codes = &(opinfo->ext_codes[0]);
    pipe_id = opinfo->tbl_inst;

    em_psg_profile_sid = IFTA90_E2T_00_LTS_PRE_SELm;
    fid = DATAf;
    bcmdrd_pt_field_set(unit,
                        em_psg_profile_sid,
                        prof_entry,
                        fid,
                        psg_data);

    index = -1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_prof_ref_count_get(unit,
                                      stage_id,
                                      pipe_id,
                                      &ref_counts));

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_PRESEL_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }

    BCMFP_ALLOC(profile_data,
                (num_profiles * sizeof(uint32_t *)),
                "bcmfpProfileDataPtr");
    for (idx = 0; idx < num_profiles; idx++) {
        data_size = (BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
        BCMFP_ALLOC(profile_data[idx],
                    data_size,
                    "bcmfpProfileData");
        sal_memcpy(profile_data[idx], prof_entry, data_size);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_profile_add(unit,
                           trans_id,
                           pipe_id,
                           tbl_id,
                           num_profiles,
                           sid_arr,
                           profile_data,
                           ref_counts,
                           &index));

    ext_codes->psg_prof_index = (uint32_t)index;
exit:
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (profile_data != NULL) {
        for (idx = 0; idx < num_profiles; idx++) {
            if (profile_data[idx] != NULL) {
                SHR_FREE(profile_data[idx]);
            }
        }
        SHR_FREE(profile_data);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_psg_profile_uninstall(int unit,
                                     uint32_t trans_id,
                                     bcmfp_stage_t *stage,
                                     void *group_opinfo)
{
    int index = -1;
    bcmfp_ref_count_t *ref_counts = NULL;
    uint32_t ltid = 0;
    uint32_t idx = 0, num_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    bcmltd_sid_t req_ltid;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_ext_codes_t *ext_codes = NULL;
    bcmfp_stage_id_t stage_id;
    int pipe_id = -1;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *ptsid_arr = NULL;
    bcmdrd_sid_t *sid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    stage_id = stage->stage_id;
    req_ltid = stage->tbls.group_tbl->sid;
    ext_codes = &(opinfo->ext_codes[0]);
    pipe_id = opinfo->tbl_inst;

    index = (int)ext_codes->psg_prof_index;

    req_ltid = stage->tbls.group_tbl->sid;
    ltid = stage->tbls.entry_tbl->sid;

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_PRESEL_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }

    if (-1 != index) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_psg_prof_ref_count_get(unit,
                                          stage_id,
                                          pipe_id,
                                          &ref_counts));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_profile_delete(unit,
                                  trans_id,
                                  pipe_id,
                                  req_ltid,
                                  num_profiles,
                                  sid_arr,
                                  ref_counts,
                                  index));
    }
exit:
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_pdd_profile_install(int unit,
                                   uint32_t trans_id,
                                   bool default_entry,
                                   void *group_opinfo,
                                   bcmfp_stage_t *stage,
                                   uint32_t *pdd_bitmap)
{
    int index = 0;
    uint32_t *prof_entry = NULL;
    bcmfp_ref_count_t *ref_counts = NULL;
    bcmdrd_fid_t fid;
    bcmdrd_sid_t em_pdd_profile_sid = 0;
    uint32_t data_size = 0;
    uint32_t **profile_data = NULL;
    uint32_t ltid = 0;
    uint32_t idx = 0, num_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    bcmfp_group_oper_info_t *opinfo = NULL;
    size_t size = 0;
    bcmfp_stage_id_t stage_id;
    int pipe_id = -1;
    bcmltd_sid_t tbl_id;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *ptsid_arr = NULL;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmfp_ext_codes_t *ext_codes = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(pdd_bitmap, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    prof_entry = kap_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);

    stage_id = stage->stage_id;
    tbl_id = stage->tbls.group_tbl->sid;
    ltid = stage->tbls.entry_tbl->sid;
    ext_codes = &(opinfo->ext_codes[0]);
    pipe_id = opinfo->tbl_inst;

    em_pdd_profile_sid = IFTA90_E2T_00_PDD_PROFILE_TABLE_0m;
    fid = DATAf;
    bcmdrd_pt_field_set(unit,
                        em_pdd_profile_sid,
                        prof_entry,
                        fid,
                        pdd_bitmap);

    index = -1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_action_prof_ref_count_get(unit,
                                         stage_id,
                                         pipe_id,
                                         &ref_counts));

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_PDD_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }

    BCMFP_ALLOC(profile_data,
                (num_profiles * sizeof(uint32_t *)),
                "bcmfpProfileDataPtr");

    for (idx = 0; idx < num_profiles; idx++) {
        data_size = (BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
        BCMFP_ALLOC(profile_data[idx],
                    data_size,
                    "bcmfpProfileData");
        sal_memcpy(profile_data[idx], prof_entry, data_size);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_profile_add(unit,
                           trans_id,
                           pipe_id,
                           tbl_id,
                           num_profiles,
                           sid_arr,
                           profile_data,
                           ref_counts,
                           &index));

    if (TRUE == default_entry) {
        ext_codes->default_pdd_prof_index = (uint32_t)index;
    } else {
        ext_codes->pdd_prof_index = (uint32_t)index;
    }
exit:
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (profile_data != NULL) {
        for (idx = 0; idx < num_profiles; idx++) {
            if (profile_data[idx] != NULL) {
                SHR_FREE(profile_data[idx]);
            }
        }
        SHR_FREE(profile_data);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_pdd_profile_uninstall(int unit,
                                     uint32_t trans_id,
                                     bool default_entry,
                                     void *group_opinfo,
                                     bcmfp_stage_t *stage)
{
    int index = -1;
    bcmfp_ref_count_t *ref_counts = NULL;
    uint32_t ltid = 0;
    uint32_t idx = 0, num_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    bcmltd_sid_t req_ltid;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_ext_codes_t *ext_codes = NULL;
    bcmfp_stage_id_t stage_id;
    int pipe_id = -1;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    stage_id = stage->stage_id;
    req_ltid = stage->tbls.group_tbl->sid;
    ext_codes = &(opinfo->ext_codes[0]);
    pipe_id = opinfo->tbl_inst;

    if (TRUE == default_entry) {
        index = (int)ext_codes->default_pdd_prof_index;
    } else {
        index = (int)ext_codes->pdd_prof_index;
    }

    req_ltid = stage->tbls.group_tbl->sid;
    ltid = stage->tbls.entry_tbl->sid;

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_PDD_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }

    if (-1 != index) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_action_prof_ref_count_get(unit,
                                             stage_id,
                                             pipe_id,
                                             &ref_counts));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_profile_delete(unit,
                                  trans_id,
                                  pipe_id,
                                  req_ltid,
                                  num_profiles,
                                  sid_arr,
                                  ref_counts,
                                  index));
    }
exit:
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_key_attrib_install(int unit,
                                  uint32_t trans_id,
                                  bcmfp_stage_t *stage,
                                  void *group_opinfo)
{
    int index = 0;
    uint8_t num_profiles = 0;
    uint32_t *prof_entry = NULL;
    bcmfp_ref_count_t *ref_counts = NULL;
    uint32_t bucket_mode = 0, idx = 0;
    bcmdrd_fid_t fid;
    uint32_t data_size = 0;
    uint32_t **profile_data = NULL;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    bcmfp_group_oper_info_t *opinfo = NULL;
    size_t size = 0;
    bcmfp_stage_id_t stage_id;
    int pipe_id = -1;
    bcmltd_sid_t tbl_id, ltid;
    bcmfp_ext_codes_t *ext_codes = NULL;
    bcmdrd_sid_t em_key_sid = 0;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    bcmfp_group_mode_t group_mode;
    uint32_t num_key_entries = 0;
    uint32_t num_data_entries = 0;
    uint32_t key_width = 0;
    uint32_t entry_sz_0, entry_sz_1;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    uint16_t key_type_size = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    prof_entry = kap_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);

    stage_id = stage->stage_id;
    tbl_id = stage->tbls.group_tbl->sid;
    ltid = stage->tbls.entry_tbl->sid;
    ext_codes = &(opinfo->ext_codes[0]);
    pipe_id = opinfo->tbl_inst;
    group_mode = opinfo->group_mode;

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    em_key_sid = stage->misc_info->em_key_attribute_sid;
    fid = BUCKET_MODEf;
    bcmdrd_pt_field_set(unit,
                        em_key_sid,
                        prof_entry,
                        fid,
                        &bucket_mode);

    key_type_size = 4;
    entry_sz_0 = stage->key_data_size_info->key_part_size[group_mode][0];
    entry_sz_1 = stage->key_data_size_info->key_part_size[group_mode][1];

    if (group_mode == BCMFP_GROUP_MODE_SINGLE) {
        num_key_entries = BCM56780_A0_FP_EM_KEY_ONE_BASE_ENTRY;
        num_data_entries = BCM56780_A0_FP_EM_DATA_ZERO_BASE_ENTRY;
        key_width = (((opinfo->key_size + key_type_size + 3) / 4) - 1);
        ext_codes->num_base_entries = 1;
    } else if (group_mode == BCMFP_GROUP_MODE_DBLINTER) {
        num_key_entries = BCM56780_A0_FP_EM_KEY_TWO_BASE_ENTRY;
        if (opinfo->key_size > entry_sz_0) {
            key_width = ((((opinfo->key_size - entry_sz_0) + 3) / 4) - 1);
        } else {
            key_width = 0;
        }

        num_data_entries = BCM56780_A0_FP_EM_DATA_ZERO_BASE_ENTRY;
        ext_codes->num_base_entries = 2;
    } else {
        num_key_entries = BCM56780_A0_FP_EM_KEY_THREE_BASE_ENTRY;
        if (opinfo->key_size >
            (entry_sz_0 + entry_sz_1)) {
            key_width = ((((opinfo->key_size -
                            (entry_sz_0 + entry_sz_1)) + 3) / 4) - 1);
        } else {
            key_width = 0;
        }

        num_data_entries = BCM56780_A0_FP_EM_DATA_ONE_BASE_ENTRY;
        ext_codes->num_base_entries = 4;
    }

    fid = KEY_BASE_WIDTHf;
    bcmdrd_pt_field_set(unit,
                        em_key_sid,
                        prof_entry,
                        fid,
                        &num_key_entries);

    fid = DATA_BASE_WIDTHf;
    bcmdrd_pt_field_set(unit,
                        em_key_sid,
                        prof_entry,
                        fid,
                        &num_data_entries);

    fid = KEY_WIDTHf;
    bcmdrd_pt_field_set(unit,
                        em_key_sid,
                        prof_entry,
                        fid,
                        &key_width);

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_KEY_ATTRIBUTES_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }
    BCMFP_ALLOC(profile_data,
                (num_profiles * sizeof(uint32_t *)),
                "bcmfpProfileDataPtr");
    for (idx = 0; idx < num_profiles; idx++) {
        data_size = (BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
        BCMFP_ALLOC(profile_data[idx],
                    data_size,
                    "bcmfpProfileData");
        sal_memcpy(profile_data[idx], prof_entry, data_size);
    }

    if (!BCMFP_STAGE_FLAGS_TEST(stage,
                               BCMFP_STAGE_HW_FLOW_TRACKER)) {
        index = -1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_em_key_attrib_prof_ref_count_get(unit,
                                                    stage_id,
                                                    pipe_id,
                                                    &ref_counts));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_profile_add(unit,
                               trans_id,
                               pipe_id,
                               tbl_id,
                               num_profiles,
                               sid_arr,
                               profile_data,
                               ref_counts,
                               &index));
    } else {
        /* BCMFP_STAGE_ID_FLOW_TRACKER */
        for (idx = 0; idx < num_profiles; idx++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_index_insert(unit,
                                        trans_id,
                                        pipe_id,
                                        tbl_id,
                                        sid_arr[idx],
                                        profile_data[idx],
                                        opinfo->action_res_id));
        }
        index = opinfo->action_res_id;
    }

    ext_codes->em_key_attrib_index = (uint32_t)index;

exit:
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (profile_data != NULL) {
        for (idx = 0; idx < num_profiles; idx++) {
            if (profile_data[idx] != NULL) {
                SHR_FREE(profile_data[idx]);
            }
        }
        SHR_FREE(profile_data);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_key_attrib_uninstall(int unit,
                                    uint32_t trans_id,
                                    bcmfp_stage_t *stage,
                                    void *group_opinfo)
{
    int index = -1;
    uint8_t num_profiles = 0;
    uint32_t idx = 0;
    bcmfp_ref_count_t *ref_counts = NULL;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    bcmltd_sid_t req_ltid, ltid;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_ext_codes_t *ext_codes = NULL;
    bcmfp_stage_id_t stage_id;
    int pipe_id = -1;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    stage_id = stage->stage_id;
    req_ltid = stage->tbls.group_tbl->sid;
    ltid = stage->tbls.entry_tbl->sid;
    ext_codes = &(opinfo->ext_codes[0]);
    pipe_id = opinfo->tbl_inst;

    index = (int)ext_codes->em_key_attrib_index;

    req_ltid = stage->tbls.group_tbl->sid;

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    if (-1 != index) {
        BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
        for (idx = 0; idx < r_cnt; idx++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_fp_em_bank_sids_get(unit,
                                                 ptsid_arr[idx],
                                                 ptinfo));
            ptid = ptinfo[BCMFP_EM_KEY_ATTRIBUTES_PTID];
            if (!bcm56780_a0_fp_em_sid_exists(unit,
                                              sid_arr,
                                              num_profiles,
                                              ptid)) {
                sid_arr[num_profiles] = ptid;
                num_profiles++;
            }
        }

        if (!BCMFP_STAGE_FLAGS_TEST(stage,
                                    BCMFP_STAGE_HW_FLOW_TRACKER)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_em_key_attrib_prof_ref_count_get(unit,
                                                        stage_id,
                                                        pipe_id,
                                                        &ref_counts));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_profile_delete(unit,
                                      trans_id,
                                      pipe_id,
                                      req_ltid,
                                      num_profiles,
                                      sid_arr,
                                      ref_counts,
                                      index));
        } else {
            /* BCMFP_STAGE_ID_FLOW_TRACKER */
            for (idx = 0; idx < num_profiles; idx++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_ptm_index_delete(unit,
                                            trans_id,
                                            pipe_id,
                                            req_ltid,
                                            sid_arr[idx],
                                            opinfo->action_res_id));
            }
        }
    }
exit:
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_fp_em_profiles_install(int unit,
                                   uint32_t stage_id,
                                   uint32_t trans_id,
                                   uint32_t flags,
                                   bcmfp_group_id_t group_id,
                                   bcmfp_group_oper_info_t *opinfo,
                                   bcmfp_buffers_t *buffers)
{
    bcmfp_stage_t *stage = NULL;
    uint8_t num_parts = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(buffers, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &num_parts));

    if (flags & BCMFP_GROUP_INSTALL_PSG_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_psg_profile_install(unit,
                                                trans_id,
                                                stage,
                                                (void *)opinfo,
                                                buffers->psg[0]));
    }

    if (flags & BCMFP_GROUP_INSTALL_KEY_ATTRIB_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_key_attrib_install(unit,
                                               trans_id,
                                               stage,
                                               (void *)opinfo));
    }

    if (flags & BCMFP_GROUP_INSTALL_KEYGEN_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_keymask_install(unit,
                                            trans_id,
                                            stage,
                                            (void *)opinfo));
    }

    if (flags & BCMFP_GROUP_INSTALL_PDD_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_pdd_profile_install(unit,
                                                trans_id,
                                                FALSE,
                                                (void *)opinfo,
                                                stage,
                                                buffers->pdd[0]));
    }

    if (flags & BCMFP_GROUP_INSTALL_DPDD_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_pdd_profile_install(unit,
                                                trans_id,
                                                TRUE,
                                                (void *)opinfo,
                                                stage,
                                                buffers->dpdd[0]));
    }

    if (flags & BCMFP_GROUP_INSTALL_DPOLICY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_dpolicy_install(unit,
                                            trans_id,
                                            stage,
                                            (void *)opinfo,
                                            buffers->dedw[0]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_fp_em_profiles_uninstall(int unit,
                                     uint32_t stage_id,
                                     uint32_t trans_id,
                                     uint32_t flags,
                                     bcmfp_group_id_t group_id,
                                     bcmfp_group_oper_info_t *opinfo)
{
    bcmfp_stage_t *stage = NULL;
    uint8_t num_parts = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &num_parts));

    if (flags & BCMFP_GROUP_INSTALL_PSG_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_psg_profile_uninstall(unit,
                                                  trans_id,
                                                  stage,
                                                  (void *)opinfo));
    }

    if (flags & BCMFP_GROUP_INSTALL_KEY_ATTRIB_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_key_attrib_uninstall(unit,
                                                 trans_id,
                                                 stage,
                                                 (void *)opinfo));
    }

    if (flags & BCMFP_GROUP_INSTALL_KEYGEN_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_keymask_uninstall(unit,
                                              trans_id,
                                              (void *)opinfo,
                                              stage));
    }

    if (flags & BCMFP_GROUP_INSTALL_PDD_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_pdd_profile_uninstall(unit,
                                                  trans_id,
                                                  FALSE,
                                                  (void *)opinfo,
                                                  stage));
    }

    if (flags & BCMFP_GROUP_INSTALL_DPDD_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_pdd_profile_uninstall(unit,
                                                  trans_id,
                                                  TRUE,
                                                  (void *)opinfo,
                                                  stage));
    }

    if (flags & BCMFP_GROUP_INSTALL_DPOLICY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_em_dpolicy_uninstall(unit,
                                              trans_id,
                                              (void *)opinfo,
                                              stage));
    }
exit:
    SHR_FUNC_EXIT();
}


int
bcm56780_a0_fp_em_buffers_custom_set(int unit,
                                     uint32_t trans_id,
                                     uint32_t flags,
                                     bcmfp_stage_t *stage,
                                     bcmfp_group_oper_info_t *opinfo,
                                     bcmfp_pdd_config_t *pdd_config,
                                     bcmfp_key_ids_t *key_ids,
                                     bcmfp_buffers_t *buffers)
{
    uint32_t *key_prof_entry = NULL;
    uint32_t *data_prof_entry = NULL;
    bcmdrd_sid_t em_mem_sid;
    bcmdrd_fid_t valid_fid = 0;
    uint32_t entry_buffer[BCMFP_ENTRY_WORDS_MAX];
    uint32_t bit, start_bit, s_bit, e_bit;
    uint32_t policy_width = 0;
    uint32_t value = 0;
    uint32_t data_size = 0;
    uint32_t valid_0 = 0;
    uint32_t valid_1 = 0;
    uint32_t valid_2 = 0;
    uint32_t valid_3 = 0;
    uint32_t entry_sz_0 = 0;
    uint32_t entry_sz_1 = 0;
    uint32_t entry_sz_2 = 0;
    uint32_t entry_sz_3 = 0;
    uint32_t data_part_sz = 0;
    bcmfp_ext_codes_t *ext_codes = NULL;
    size_t size = 0;
    bcmfp_group_mode_t group_mode;
    bcmfp_pdd_id_t pdd_id = 0;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(key_ids, SHR_E_PARAM);
    SHR_NULL_CHECK(buffers, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_fp_common_buffers_custom_set(unit,
                                                  trans_id,
                                                  flags,
                                                  stage,
                                                  opinfo,
                                                  pdd_config,
                                                  key_ids,
                                                  buffers));

    ekw = buffers->ekw;
    edw = buffers->edw;
    pdd_id = key_ids->pdd_id;

    key_prof_entry = key_entry[unit];
    data_prof_entry = data_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(key_prof_entry, 0, size);
    sal_memset(data_prof_entry, 0, size);
    sal_memset(entry_buffer, 0, size);

    group_mode = opinfo->group_mode;
    ext_codes = &(opinfo->ext_codes[0]);

    if (group_mode == BCMFP_GROUP_MODE_QUAD) {
        em_mem_sid = IFTA90_E2T_00_B0_QUADm;
    } else if (group_mode == BCMFP_GROUP_MODE_DBLINTER) {
        em_mem_sid = IFTA90_E2T_00_B0_DOUBLEm;
    } else {
        em_mem_sid = IFTA90_E2T_00_B0_SINGLEm;
    }

    /* update key field */
    valid_0 = 1;
    if (group_mode == BCMFP_GROUP_MODE_SINGLE) {
        entry_sz_0 = stage->key_data_size_info->key_part_size[group_mode][0];
        value = entry_sz_0;
        if (opinfo->key_size < entry_sz_0) {
            value = opinfo->key_size;
        }
        sal_memset(entry_buffer, 0, size);
        for (bit = 0; bit < value; bit++) {
            if (SHR_BITGET(ekw[0], bit)) {
                 SHR_BITSET(entry_buffer, bit);
            }
        }
        if (value > 0) {
            s_bit = 7;
            e_bit = (s_bit + value - 1);
            bcmdrd_field_set(key_prof_entry,
                             s_bit,
                             e_bit,
                             entry_buffer);
        }
        ext_codes->key_field_sbit[0] = BCM56780_A0_FP_EM_KEY_TYPE_SBIT;
        ext_codes->key_field_width[0] = (value + BCM56780_A0_FP_EM_KEY_TYPE_WIDTH);
        ext_codes->num_key_fields = 1;
    } else if (group_mode == BCMFP_GROUP_MODE_DBLINTER) {
        entry_sz_0 = stage->key_data_size_info->key_part_size[group_mode][0];
        entry_sz_1 = stage->key_data_size_info->key_part_size[group_mode][1];
        value = entry_sz_0;
        if (opinfo->key_size < entry_sz_0) {
            value = opinfo->key_size;
        }
        sal_memset(entry_buffer, 0, size);
        for (start_bit = 0, bit = 0; bit < value; start_bit++, bit++) {
            if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
            }
        }

        if (value > 0) {
            s_bit = 7;
            e_bit = (s_bit + value - 1);
            bcmdrd_field_set(key_prof_entry,
                             s_bit,
                             e_bit,
                             entry_buffer);
        }

        ext_codes->key_field_sbit[0] = BCM56780_A0_FP_EM_KEY_TYPE_SBIT;
        ext_codes->key_field_width[0] = (value +
                                         BCM56780_A0_FP_EM_KEY_TYPE_WIDTH);
        ext_codes->num_key_fields = 1;

        if (opinfo->key_size > entry_sz_0) {
            value = (opinfo->key_size - entry_sz_0);
            if (value > entry_sz_1) {
                value = entry_sz_1;
            }

            sal_memset(entry_buffer, 0, size);
            for (bit = 0; bit < value; start_bit++,bit++) {
                if (SHR_BITGET(ekw[0], start_bit)) {
                    SHR_BITSET(entry_buffer, bit);
                }
            }

            s_bit = 123;
            e_bit = (s_bit + value - 1);
            bcmdrd_field_set(key_prof_entry,
                             s_bit,
                             e_bit,
                             entry_buffer);
            ext_codes->key_field_sbit[1] = 123;
            ext_codes->key_field_width[1] = value;
            ext_codes->num_key_fields = 2;
        }
    } else {
        /* 320 bit mode */
        /* key_0 */
        entry_sz_0 = stage->key_data_size_info->key_part_size[group_mode][0];
        entry_sz_1 = stage->key_data_size_info->key_part_size[group_mode][1];
        entry_sz_2 = stage->key_data_size_info->key_part_size[group_mode][2];
        entry_sz_3 = stage->key_data_size_info->key_part_size[group_mode][3];

        value = entry_sz_0;
        if (opinfo->key_size < entry_sz_0) {
            value = opinfo->key_size;
        }
        sal_memset(entry_buffer, 0, size);
        for (start_bit = 0, bit = 0; bit < value; start_bit++,bit++) {
            if (SHR_BITGET(ekw[0], start_bit)) {
                SHR_BITSET(entry_buffer, bit);
            }
        }
        if (value > 0) {
            s_bit = 7;
            e_bit = (s_bit + value - 1);
            bcmdrd_field_set(key_prof_entry,
                             s_bit,
                             e_bit,
                             entry_buffer);
        }

        ext_codes->key_field_sbit[0] = BCM56780_A0_FP_EM_KEY_TYPE_SBIT;
        ext_codes->key_field_width[0] = (value +
                                         BCM56780_A0_FP_EM_KEY_TYPE_WIDTH);
        ext_codes->num_key_fields = 1;

        /* key_1 */
        if (opinfo->key_size > entry_sz_0) {
            value = (opinfo->key_size - entry_sz_0);
            if (value > entry_sz_1) {
                value = entry_sz_1;
            }
            sal_memset(entry_buffer, 0, size);
            for (bit = 0; bit < value; start_bit++,bit++) {
                if (SHR_BITGET(ekw[0], start_bit)) {
                    SHR_BITSET(entry_buffer, bit);
                }
            }
            s_bit = 123;
            e_bit = (s_bit + value - 1);
            bcmdrd_field_set(key_prof_entry,
                             s_bit,
                             e_bit,
                             entry_buffer);
            ext_codes->key_field_sbit[1] = 123;
            ext_codes->key_field_width[1] = value;
            ext_codes->num_key_fields = 2;

            /* key_2 */
            if (opinfo->key_size > (entry_sz_0 + entry_sz_1)) {
                value = (opinfo->key_size - (entry_sz_0 + entry_sz_1));
                if (value > entry_sz_2) {
                    value = entry_sz_2;
                }
                sal_memset(entry_buffer, 0, size);
                for (bit = 0; bit < value; start_bit++,bit++) {
                    if (SHR_BITGET(ekw[0], start_bit)) {
                        SHR_BITSET(entry_buffer, bit);
                    }
                }
                s_bit = 243;
                e_bit = (s_bit + value - 1);
                bcmdrd_field_set(key_prof_entry,
                                 s_bit,
                                 e_bit,
                                 entry_buffer);

                ext_codes->key_field_sbit[2] = 243;
                ext_codes->key_field_width[2] = value;
                ext_codes->num_key_fields = 3;

                /* key_3 */
                if (opinfo->key_size >
                    (entry_sz_0 + entry_sz_1 + entry_sz_2)) {
                    value = (opinfo->key_size -
                             (entry_sz_0 + entry_sz_1 + entry_sz_2));
                    if (value > entry_sz_3) {
                        value = entry_sz_3;
                    }
                    sal_memset(entry_buffer, 0, size);
                    for (bit = 0; bit < value; start_bit++,bit++) {
                        if (SHR_BITGET(ekw[0], start_bit)) {
                            SHR_BITSET(entry_buffer, bit);
                        }
                    }
                    s_bit = 363;
                    e_bit = (s_bit + value - 1);
                    bcmdrd_field_set(key_prof_entry,
                                     s_bit,
                                     e_bit,
                                     entry_buffer);

                    ext_codes->key_field_sbit[3] = 363;
                    ext_codes->key_field_width[3] = value;
                    ext_codes->num_key_fields = 4;
                }
            }
        }
    }

    if (flags & BCMFP_BUFFERS_CUSTOMIZE_EDW) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_policy_width_get(unit,
                                        stage->stage_id,
                                        pdd_id,
                                        opinfo->pdd_type,
                                        &policy_width));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_data_width_get(unit,
                                      stage->stage_id,
                                      pdd_id,
                                      0,
                                      opinfo->pdd_type,
                                      &data_size));
        /* Build EDW */
        entry_sz_0 = stage->key_data_size_info->key_part_size[group_mode][0];
        if (group_mode == BCMFP_GROUP_MODE_SINGLE) {
            if (data_size > 0) {
                start_bit = (policy_width - data_size);
                sal_memset(entry_buffer, 0, size);
                for (bit = 0; bit < data_size; start_bit++, bit++) {
                    if (SHR_BITGET(edw[0], start_bit)) {
                        SHR_BITSET(entry_buffer, bit);
                    }
                }
                e_bit = 119;
                s_bit = (e_bit - data_size + 1);
                bcmdrd_field_set(data_prof_entry,
                                 s_bit,
                                 e_bit,
                                 entry_buffer);
            }
        } else if (group_mode == BCMFP_GROUP_MODE_DBLINTER) {
            entry_sz_0 = stage->key_data_size_info->key_part_size[group_mode][0];
            entry_sz_1 = stage->key_data_size_info->key_part_size[group_mode][1];
            data_part_sz = 0;
            if (opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_GROUP_SBR_ENTRY) {
                data_size += stage->misc_info->per_entry_sbr_raw_data_offset;
            }
            start_bit = (policy_width - data_size);
            if (data_size > entry_sz_1) {
                data_part_sz = (data_size - entry_sz_1);
                value = data_part_sz;
                sal_memset(entry_buffer, 0, size);
                for (bit = 0; bit < value; start_bit++, bit++) {
                    if (SHR_BITGET(edw[0], start_bit)) {
                        SHR_BITSET(entry_buffer, bit);
                    }
                }

                if (value > 0) {
                    e_bit = 119;
                    s_bit = (e_bit - value + 1);
                    bcmdrd_field_set(data_prof_entry,
                                     s_bit,
                                     e_bit,
                                     entry_buffer);
                }
            }

            value = (data_size - data_part_sz);
            sal_memset(entry_buffer, 0, size);
            for (bit = 0; bit < value; start_bit++, bit++) {
                if (SHR_BITGET(edw[0], start_bit)) {
                    SHR_BITSET(entry_buffer, bit);
                }
            }

            if (value > 0) {
                e_bit = 239;
                s_bit = (e_bit - value + 1);
                bcmdrd_field_set(data_prof_entry,
                                 s_bit,
                                 e_bit,
                                 entry_buffer);
            }
        } else {
            entry_sz_0 = stage->key_data_size_info->key_part_size[group_mode][0];
            entry_sz_1 = stage->key_data_size_info->key_part_size[group_mode][1];
            entry_sz_2 = stage->key_data_size_info->key_part_size[group_mode][2];
            entry_sz_3 = stage->key_data_size_info->key_part_size[group_mode][3];

            data_part_sz = 0;
            if (opinfo->pdd_type == BCMFP_PDD_OPER_TYPE_PDD_GROUP_SBR_ENTRY) {
                data_size += stage->misc_info->per_entry_sbr_raw_data_offset;
            }
            start_bit = (policy_width - data_size);
            if (data_size > entry_sz_3) {
                if (data_size > (entry_sz_2 + entry_sz_3)) {
                    if (data_size >
                        (entry_sz_1 + entry_sz_2 + entry_sz_3)) {
                        data_part_sz = (data_size -
                                        (entry_sz_1 + entry_sz_2 + entry_sz_3));
                        value = data_part_sz;
                        sal_memset(entry_buffer, 0, size);
                        for (bit = 0; bit < value; start_bit++, bit++) {
                            if (SHR_BITGET(edw[0], start_bit)) {
                                SHR_BITSET(entry_buffer, bit);
                            }
                        }

                        if (value > 0) {
                            e_bit = 119;
                            s_bit = (e_bit - value + 1);
                            bcmdrd_field_set(data_prof_entry,
                                             s_bit,
                                             e_bit,
                                             entry_buffer);
                        }
                    }

                    value = ((data_size - data_part_sz) -
                             (entry_sz_2 + entry_sz_3));
                    data_part_sz += value;
                    sal_memset(entry_buffer, 0, size);
                    for (bit = 0; bit < value; start_bit++, bit++) {
                        if (SHR_BITGET(edw[0], start_bit)) {
                            SHR_BITSET(entry_buffer, bit);
                        }
                    }

                    if (value > 0) {
                        e_bit = 239;
                        s_bit = (e_bit - value + 1);
                        bcmdrd_field_set(data_prof_entry,
                                         s_bit,
                                         e_bit,
                                         entry_buffer);
                    }
                }

                value = ((data_size - data_part_sz) -
                         entry_sz_3);
                data_part_sz += value;
                sal_memset(entry_buffer, 0, size);
                for (bit = 0; bit < value; start_bit++, bit++) {
                    if (SHR_BITGET(edw[0], start_bit)) {
                        SHR_BITSET(entry_buffer, bit);
                    }
                }

                if (value > 0) {
                    e_bit = 359;
                    s_bit = (e_bit - value + 1);
                    bcmdrd_field_set(data_prof_entry,
                                     s_bit,
                                     e_bit,
                                     entry_buffer);
                }
            }

            value = (data_size - data_part_sz);
            sal_memset(entry_buffer, 0, size);
            for (bit = 0; bit < value; start_bit++, bit++) {
                if (SHR_BITGET(edw[0], start_bit)) {
                    SHR_BITSET(entry_buffer, bit);
                }
            }

            if (value > 0) {
                e_bit = 479;
                s_bit = (e_bit - value + 1);
                bcmdrd_field_set(data_prof_entry,
                                 s_bit,
                                 e_bit,
                                 entry_buffer);
            }
        }

        /* Clear EDW */
        sal_memset(edw[0], 0, size);
        /* update EDW */
        sal_memcpy(edw[0], data_prof_entry, size);
    }

    if (flags & BCMFP_BUFFERS_CUSTOMIZE_EDW) {
        if (group_mode == BCMFP_GROUP_MODE_SINGLE) {
            valid_fid = BASE_VALIDf;
        } else {
            valid_fid = BASE_VALID_0f;
        }
        valid_0 = 1;
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            key_prof_entry,
                            valid_fid,
                            &valid_0);

        if (group_mode != BCMFP_GROUP_MODE_SINGLE) {
            valid_1 = 2;
            bcmdrd_pt_field_set(unit,
                                em_mem_sid,
                                key_prof_entry,
                                BASE_VALID_1f,
                                &valid_1);

            if (group_mode == BCMFP_GROUP_MODE_QUAD) {
                valid_2 = 2;
                bcmdrd_pt_field_set(unit,
                                    em_mem_sid,
                                    key_prof_entry,
                                    BASE_VALID_2f,
                                    &valid_2);
                valid_3 = 7;
                bcmdrd_pt_field_set(unit,
                                    em_mem_sid,
                                    key_prof_entry,
                                    BASE_VALID_3f,
                                    &valid_3);
            }
        }
    }

    /* Create a profile in KEY_ATTRIBUTES table. */
    value = ext_codes->em_key_attrib_index;

    /* Use profile indes as KEY_TYPE. */
    bcmdrd_pt_field_set(unit,
                        em_mem_sid,
                        key_prof_entry,
                        KEY_TYPEf,
                        &value);

    /* clear EKW */
    sal_memset(ekw[0], 0, size);
    sal_memset(ekw[1], 0, size);

    /* Update EKW */
    sal_memcpy(ekw[0], key_prof_entry, size);

 exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_dpolicies_copy(int unit,
                             uint32_t trans_id,
                             bcmfp_stage_t *stage)
{
    uint32_t data_size = 0;
    uint32_t *profile_data = NULL;
    uint32_t miss_ltpr_profile_data = 0;
    uint16_t idx = 0, num_profiles = 0;
    int num_ltpr_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    int pipe_id = -1;
    bcmltd_sid_t req_ltid;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *ptsid_arr = NULL;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ltpr_sid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    int min_idx = 0;
    int max_idx = 0;
    int profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    req_ltid = stage->tbls.entry_tbl->sid;

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, req_ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0 || r_cnt == 1) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, req_ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));


    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    BCMFP_ALLOC(ltpr_sid_arr,
                (r_cnt * sizeof(bcmdrd_sid_t)),
                "bcmfpLtprSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_LKUP_MISS_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
        ptid = ptinfo[BCMFP_EM_LKUP_MISS_LTPR_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          ltpr_sid_arr,
                                          num_ltpr_profiles,
                                          ptid)) {
            ltpr_sid_arr[num_ltpr_profiles] = ptid;
            num_ltpr_profiles++;
        }
    }

    if (num_profiles <= 1) {
        SHR_EXIT();
    }

    data_size = (BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    BCMFP_ALLOC(profile_data, data_size, "bcmfpProfileData");

    min_idx = bcmdrd_pt_index_min(unit, sid_arr[0]);
    max_idx = bcmdrd_pt_index_max(unit, sid_arr[0]);
    for (idx = min_idx; idx <= max_idx; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_read(unit,
                                  trans_id,
                                  pipe_id,
                                  req_ltid,
                                  sid_arr[0],
                                  profile_data,
                                  idx));

        for (profile = 1; profile < num_profiles; profile++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_index_insert(unit,
                                        trans_id,
                                        pipe_id,
                                        req_ltid,
                                        sid_arr[profile],
                                        profile_data,
                                        idx));
        }
    }

    min_idx = bcmdrd_pt_index_min(unit, ltpr_sid_arr[0]);
    max_idx = bcmdrd_pt_index_max(unit, ltpr_sid_arr[0]);
    for (idx = min_idx; idx <= max_idx; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_read(unit,
                                  trans_id,
                                  pipe_id,
                                  req_ltid,
                                  ltpr_sid_arr[0],
                                  &miss_ltpr_profile_data,
                                  idx));
        for (profile = 1; profile < num_ltpr_profiles; profile++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_index_insert(unit,
                                        trans_id,
                                        pipe_id,
                                        req_ltid,
                                        ltpr_sid_arr[profile],
                                        &miss_ltpr_profile_data,
                                        idx));
        }
    }
exit:
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (ltpr_sid_arr != NULL) {
        SHR_FREE(ltpr_sid_arr);
    }
    if (profile_data != NULL) {
        SHR_FREE(profile_data);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_keymasks_copy(int unit,
                             uint32_t trans_id,
                             bcmfp_stage_t *stage)
{
    uint32_t data_size = 0;
    uint32_t *profile_data = NULL;
    uint16_t idx = 0, num_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    int pipe_id = -1;
    bcmltd_sid_t req_ltid;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    int min_idx = 0;
    int max_idx = 0;
    int profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    req_ltid = stage->tbls.entry_tbl->sid;

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, req_ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0 || r_cnt == 1) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, req_ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_KEY_MASK_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }

    if (num_profiles <= 1) {
        SHR_EXIT();
    }

    data_size = (BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    BCMFP_ALLOC(profile_data, data_size, "bcmfpProfileData");

    min_idx = bcmdrd_pt_index_min(unit, sid_arr[0]);
    max_idx = bcmdrd_pt_index_max(unit, sid_arr[0]);
    for (idx = min_idx; idx <= max_idx; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ptm_index_read(unit,
                                  trans_id,
                                  pipe_id,
                                  req_ltid,
                                  sid_arr[0],
                                  profile_data,
                                  idx));

        for (profile = 1; profile < num_profiles; profile++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_index_insert(unit,
                                        trans_id,
                                        pipe_id,
                                        req_ltid,
                                        sid_arr[profile],
                                        profile_data,
                                        idx));
        }
    }

exit:
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (profile_data != NULL) {
        SHR_FREE(profile_data);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_key_attribs_copy(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_t *stage)
{
    bcmfp_ref_count_t *ref_counts = NULL;
    uint32_t data_size = 0;
    uint32_t *profile_data = NULL;
    uint16_t idx = 0, num_profiles = 0;
    bcmfp_stage_id_t stage_id;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    int pipe_id = -1;
    bcmltd_sid_t req_ltid;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    int min_idx = 0;
    int max_idx = 0;
    int profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage_id = stage->stage_id;
    req_ltid = stage->tbls.entry_tbl->sid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_em_key_attrib_prof_ref_count_get(unit,
                                         stage_id,
                                         pipe_id,
                                         &ref_counts));

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, req_ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0 || r_cnt == 1) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, req_ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid =  ptinfo[BCMFP_EM_KEY_ATTRIBUTES_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }

    if (num_profiles <= 1) {
        SHR_EXIT();
    }

    data_size = (BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    BCMFP_ALLOC(profile_data, data_size, "bcmfpProfileData");

    min_idx = bcmdrd_pt_index_min(unit, sid_arr[0]);
    max_idx = bcmdrd_pt_index_max(unit, sid_arr[0]);
    for (idx = min_idx; idx <= max_idx; idx++) {
        if (ref_counts[idx].ref_count) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_index_read(unit,
                                      trans_id,
                                      pipe_id,
                                      req_ltid,
                                      sid_arr[0],
                                      profile_data,
                                      idx));

            for (profile = 1; profile < num_profiles; profile++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_ptm_index_insert(unit,
                                            trans_id,
                                            pipe_id,
                                            req_ltid,
                                            sid_arr[profile],
                                            profile_data,
                                            idx));
            }
        }
    }

exit:
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (profile_data != NULL) {
        SHR_FREE(profile_data);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_psg_profiles_copy(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_t *stage)
{
    bcmfp_ref_count_t *ref_counts = NULL;
    uint32_t data_size = 0;
    uint32_t *profile_data = NULL;
    uint16_t idx = 0, num_profiles = 0;
    bcmfp_stage_id_t stage_id;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    int pipe_id = -1;
    bcmltd_sid_t req_ltid;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    int min_idx = 0;
    int max_idx = 0;
    int profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage_id = stage->stage_id;
    req_ltid = stage->tbls.entry_tbl->sid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_prof_ref_count_get(unit,
                                         stage_id,
                                         pipe_id,
                                         &ref_counts));

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, req_ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0 || r_cnt == 1) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, req_ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid =  ptinfo[BCMFP_EM_PRESEL_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }

    if (num_profiles <= 1) {
        SHR_EXIT();
    }

    data_size = (BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    BCMFP_ALLOC(profile_data, data_size, "bcmfpProfileData");

    min_idx = bcmdrd_pt_index_min(unit, sid_arr[0]);
    max_idx = bcmdrd_pt_index_max(unit, sid_arr[0]);
    for (idx = min_idx; idx <= max_idx; idx++) {
        if (ref_counts[idx].ref_count) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_index_read(unit,
                                      trans_id,
                                      pipe_id,
                                      req_ltid,
                                      sid_arr[0],
                                      profile_data,
                                      idx));

            for (profile = 1; profile < num_profiles; profile++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_ptm_index_insert(unit,
                                            trans_id,
                                            pipe_id,
                                            req_ltid,
                                            sid_arr[profile],
                                            profile_data,
                                            idx));
            }
        }
    }

exit:
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (profile_data != NULL) {
        SHR_FREE(profile_data);
    }
    SHR_FUNC_EXIT();
}

STATIC int
bcm56780_a0_em_pdd_profiles_copy(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_t *stage)
{
    bcmfp_ref_count_t *ref_counts = NULL;
    uint32_t data_size = 0;
    uint32_t *profile_data = NULL;
    uint16_t idx = 0, num_profiles = 0;
    bcmfp_stage_id_t stage_id;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    int pipe_id = -1;
    bcmltd_sid_t req_ltid;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    int min_idx = 0;
    int max_idx = 0;
    int profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage_id = stage->stage_id;
    req_ltid = stage->tbls.entry_tbl->sid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_action_prof_ref_count_get(unit,
                                         stage_id,
                                         pipe_id,
                                         &ref_counts));

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, req_ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0 || r_cnt == 1) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, req_ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    BCMFP_ALLOC(sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid =  ptinfo[BCMFP_EM_PRESEL_PTID];
        if (!bcm56780_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_profiles,
                                          ptid)) {
            sid_arr[num_profiles] = ptid;
            num_profiles++;
        }
    }

    if (num_profiles <= 1) {
        SHR_EXIT();
    }

    data_size = (BCMFP_ENTRY_WORDS_MAX * sizeof(uint32_t));
    BCMFP_ALLOC(profile_data, data_size, "bcmfpProfileData");

    min_idx = bcmdrd_pt_index_min(unit, sid_arr[0]);
    max_idx = bcmdrd_pt_index_max(unit, sid_arr[0]);
    for (idx = min_idx; idx <= max_idx; idx++) {
        if (ref_counts[idx].ref_count) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ptm_index_read(unit,
                                      trans_id,
                                      pipe_id,
                                      req_ltid,
                                      sid_arr[0],
                                      profile_data,
                                      idx));

            for (profile = 1; profile < num_profiles; profile++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_ptm_index_insert(unit,
                                            trans_id,
                                            pipe_id,
                                            req_ltid,
                                            sid_arr[profile],
                                            profile_data,
                                            idx));
            }
        }
    }

exit:
    if (ptsid_arr != NULL) {
        SHR_FREE(ptsid_arr);
    }
    if (sid_arr != NULL) {
        SHR_FREE(sid_arr);
    }
    if (profile_data != NULL) {
        SHR_FREE(profile_data);
    }
    SHR_FUNC_EXIT();
}


STATIC int
bcm56780_a0_em_existing_presels_copy_to_new_banks(int unit,
                                                  bcmfp_stage_id_t stage_id,
                                                  uint32_t trans_id)
{
    uint32_t    bnk_idx = 0;
    uint32_t    ltid = 0, ptid = 0, ptid_0 = 0, ptidx = 0;
    uint32_t    entry_sz = 0;
    uint32_t    bank_sid = 0, bank_sid_0 = 0;
    uint32_t    index_min = 0, index_max = 0, entry_cnt = 0;
    uint32_t    ptinfo[BCMFP_EM_PTID_COUNT];
    uint32_t    ptinfo_0[BCMFP_EM_PTID_COUNT];
    uint32_t    rsp_entry_wsize = 0;
    uint32_t    rsp_flags = 0;
    void        *buf = NULL;
    bcmltd_sid_t    rsp_ltid = 0;
    bcmfp_stage_t   *stage = NULL;
    bcmptm_misc_info_t  misc_info;
    bcmbd_pt_dyn_info_t pt_info;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    bcmfp_stage_oper_info_t *stage_oper = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    ltid = stage->tbls.entry_tbl->sid;

    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt));
    if (r_cnt == 0 || r_cnt == 1) {
        /* Zero or one bank. No need to copy. */
        SHR_EXIT();
    }

    ptsid_arr_cnt = r_cnt;
    BCMFP_ALLOC(ptsid_arr,
                ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                "bcmfpLtsArray");
    SHR_IF_ERR_EXIT
        (bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        ptsid_arr, ptsid_arr_cnt,
                                        &r_cnt));

    /* Read data from bank id 0 and write to all banks */
    bank_sid_0 = ptsid_arr[0];

     sal_memset(ptinfo_0, 0, sizeof(ptinfo_0));
     SHR_IF_ERR_VERBOSE_EXIT
         (bcm56780_a0_fp_em_bank_sids_get(unit,
                                          bank_sid_0,
                                          ptinfo_0));

     for (ptidx = BCMFP_EM_PTID_MIN; ptidx < BCMFP_EM_PTID_COUNT; ptidx++) {
        /* Presel is aggr view TCAM */
        if (ptidx != BCMFP_EM_LTS_TCAM_PTID) {
            continue;
        }
        ptid_0 = ptinfo_0[ptidx];
        sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
        sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
        index_min = bcmdrd_pt_index_min(unit, ptid_0);
        index_max = bcmdrd_pt_index_max(unit, ptid_0);
        entry_sz = (bcmdrd_pt_entry_wsize(unit, ptid_0) * 4);
        entry_cnt = (index_max - index_min + 1);
        pt_info.index = 0;
        pt_info.tbl_inst = -1;
        misc_info.dma_enable = TRUE;
        misc_info.dma_entry_count = entry_cnt;
        rsp_entry_wsize = (entry_cnt * entry_sz);
        BCMFP_ALLOC(buf, rsp_entry_wsize, "bcmfpEntryBuf");
        /* Read the entire table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        0,
                                        ptid_0,
                                        &pt_info,
                                        NULL,
                                        &misc_info,
                                        BCMPTM_OP_READ,
                                        NULL,
                                        rsp_entry_wsize,
                                        ltid,
                                        trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &rsp_ltid,
                                        &rsp_flags));
        /* write data to all other banks */
        for (bnk_idx = 1; bnk_idx < r_cnt; bnk_idx++) {
            bank_sid = ptsid_arr[bnk_idx];

            sal_memset(ptinfo, 0, sizeof(ptinfo));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_fp_em_bank_sids_get(unit,
                                                 bank_sid,
                                                 ptinfo));
            ptid = ptinfo[ptidx];
            if (ptid == ptid_0) {
                continue;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_ltm_mreq_indexed_lt(unit,
                                            0,
                                            ptid,
                                            &pt_info,
                                            NULL,
                                            &misc_info,
                                            BCMPTM_OP_WRITE,
                                            buf,
                                            rsp_entry_wsize,
                                            ltid,
                                            trans_id,
                                            NULL,
                                            NULL,
                                            NULL,
                                            &rsp_ltid,
                                            &rsp_flags));
        }
        SHR_FREE(buf);
    }

exit:
    SHR_FREE(ptsid_arr);
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_fp_em_profiles_copy(int unit,
                                uint32_t trans_id,
                                bcmfp_stage_t *stage)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_em_psg_profiles_copy(unit,
                                         trans_id,
                                         stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_em_key_attribs_copy(unit,
                                           trans_id,
                                           stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_em_keymasks_copy(unit,
                                        trans_id,
                                        stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_em_pdd_profiles_copy(unit,
                                         trans_id,
                                         stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_em_dpolicies_copy(unit,
                                        trans_id,
                                        stage));
   SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_em_existing_presels_copy_to_new_banks(unit,
                                                stage->stage_id,
                                                trans_id));
exit:
    SHR_FUNC_EXIT();
}
