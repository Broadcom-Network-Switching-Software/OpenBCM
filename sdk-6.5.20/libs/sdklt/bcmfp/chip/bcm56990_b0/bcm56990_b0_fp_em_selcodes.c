/*! \file bcm56990_b0_fp_em_selcodes.c
 *
 * APIs to intsall/uninstall to configurations to h/w
 * memories/registers to create/destroy the IFP group for
 * Tomahawk-4 B0 device(56990_B0).
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
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_ptm_internal.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_types.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static uint32_t kgp_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t kgp2_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];
static uint32_t ap_entry[BCMDRD_CONFIG_MAX_UNITS][BCMFP_ENTRY_WORDS_MAX];

static bcmdrd_sid_t em_profile_sids[] = {
                                    EXACT_MATCH_KEY_GEN_PROGRAM_PROFILEm,
                                    EXACT_MATCH_KEY_GEN_MASKm
                                 };

static bcmdrd_sid_t em_pdd_profile_sid[] = { EXACT_MATCH_ACTION_PROFILEm };
static bcmdrd_sid_t em_qos_profile_sid[] = { EXACT_MATCH_QOS_ACTIONS_PROFILEm };
static bcmdrd_sid_t em_entry_sids[] = { EXACT_MATCH_2m, EXACT_MATCH_4m };
static bcmdrd_sid_t em_default_policy_sid[] = { EXACT_MATCH_DEFAULT_POLICYm };

STATIC int
bcmfp_bcm56990_b0_keygen_mask_init(int unit,
                                   uint32_t trans_id,
                                   bcmfp_stage_t *stage,
                                   bcmfp_group_id_t group_id,
                                   bcmfp_group_oper_info_t *opinfo,
                                   uint8_t part,
                                   bcmdrd_sid_t kgp_mask_sid,
                                   uint32_t *kgp_mask_entry)
{
    int rv = 0;
    uint16_t fidx = 0;
    uint16_t fidx_max = 0;
    uint32_t offset = 0;
    uint32_t *prof_entry = NULL;
    uint32_t entry_buffer[5] = {0};
    bcmfp_stage_id_t stage_id;
    bcmfp_qual_offset_info_t q_offset;
    bcmlrd_field_def_t field_def;
    size_t size = 0;
    bcmltd_fid_t qual_fid = 0;
    bcmltd_sid_t tbl_id;
    uint16_t num_fid = 0;
    shr_error_t rv_except = SHR_E_NOT_FOUND;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(kgp_mask_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage_id = BCMFP_STAGE_ID_EXACT_MATCH;

    prof_entry = kgp_mask_entry;
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);

    size = sizeof(bcmfp_qual_offset_info_t);
    num_fid = stage->tbls.group_tbl->fid_count;
    tbl_id = stage->tbls.group_tbl->sid;
    for (qual_fid = 0; qual_fid < num_fid; qual_fid++) {
        rv = bcmlrd_table_field_def_get(unit,
                                        tbl_id,
                                        qual_fid,
                                        &field_def);
        /* No support for this qualifier in this device, then skip */
        if (rv == SHR_E_UNAVAIL) {
            continue;
        }

        if (field_def.depth > 0) {
            fidx_max = field_def.depth - 1;
        } else {
            fidx_max = 0;
        }

        for (fidx = 0; fidx <= fidx_max; fidx++) {
            sal_memset(&q_offset, 0, size);
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (bcmfp_group_qual_offset_info_get(unit,
                                                  stage_id,
                                                  group_id,
                                                  opinfo,
                                                  FALSE,
                                                  part,
                                                  qual_fid,
                                                  fidx,
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
                        SEARCH_MASKf,
                        entry_buffer);
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_bcm56990_b0_em_keygen_install(int unit,
                                    uint32_t trans_id,
                                    bcmfp_stage_t *stage,
                                    void *group_opinfo,
                                    uint8_t num_parts)
{
    bcmdrd_sid_t kgp_sid = em_profile_sids[0];
    bcmdrd_sid_t kgp2_sid = em_profile_sids[1];
    uint32_t *profile_data[] = { NULL, NULL };
    int index = 0;
    uint8_t part = 0;
    bcmfp_group_oper_info_t *opinfo = NULL;
    uint32_t *prof_entry = NULL;
    bcmfp_ref_count_t *ref_counts = NULL;
    size_t size = 0;
    bcmltd_sid_t tbl_id;
    int pipe_id = -1;
    bcmfp_stage_id_t stage_id;
    bcmfp_ext_codes_t *ext_codes = NULL;
    bcmfp_group_id_t group_id = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    profile_data[0] = kgp_entry[unit];
    profile_data[1] = kgp2_entry[unit];

    stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    tbl_id = stage->tbls.group_tbl->sid;
    ext_codes = opinfo->ext_codes;
    pipe_id = opinfo->tbl_inst;
    group_id = opinfo->group_id;

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

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_keygen_mask_init(unit,
                                                trans_id,
                                                stage,
                                                group_id,
                                                opinfo,
                                                part,
                                                kgp2_sid,
                                                prof_entry));
        index = -1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_keygen_prof_ref_count_get(unit,
                                             stage_id,
                                             pipe_id,
                                             &ref_counts));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_profile_add(unit,
                              trans_id,
                              pipe_id,
                              tbl_id,
                              2,
                              em_profile_sids,
                              profile_data,
                              ref_counts,
                              &index));
        ext_codes[part].keygen_index = (uint32_t)index;
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_bcm56990_b0_em_keygen_uninstall(int unit,
                                      uint32_t trans_id,
                                      void *group_opinfo,
                                      bcmfp_stage_t *stage,
                                      uint8_t num_parts)
{
    int index = -1;
    int tbl_inst = -1;
    uint8_t part = 0;
    bcmfp_ref_count_t *ref_counts = NULL;
    bcmltd_sid_t req_ltid;
    bcmfp_stage_id_t stage_id;
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_ext_codes_t *ext_codes = NULL;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    tbl_inst = opinfo->tbl_inst;
    req_ltid = stage->tbls.group_tbl->sid;
    ext_codes = opinfo->ext_codes;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_keygen_prof_ref_count_get(unit,
                                         stage_id,
                                         tbl_inst,
                                         &ref_counts));

    req_ltid = stage->tbls.group_tbl->sid;
    for (part = 0; part < num_parts; part++) {
        index = (int)ext_codes[part].keygen_index;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_profile_delete(unit,
                                  trans_id,
                                  tbl_inst,
                                  req_ltid,
                                  2,
                                  em_profile_sids,
                                  ref_counts,
                                  index));
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_bcm56990_b0_em_pdd_profile_install(int unit,
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
    uint32_t *profile_data[] = { ap_entry[unit] };
    bcmfp_group_oper_info_t *opinfo = NULL;
    size_t size = 0;
    bcmfp_stage_id_t stage_id;
    int pipe_id = -1;
    bcmltd_sid_t tbl_id;
    bcmfp_ext_codes_t *ext_codes = NULL;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(pdd_bitmap, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    prof_entry = ap_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);

    stage_id = stage->stage_id;
    tbl_id = stage->tbls.group_tbl->sid;
    ext_codes = &(opinfo->ext_codes[0]);
    pipe_id = opinfo->tbl_inst;

    fid = ACTION_SET_BITMAPf;
    bcmdrd_pt_field_set(unit,
                        em_pdd_profile_sid[0],
                        prof_entry,
                        fid,
                        pdd_bitmap);

    index = -1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_action_prof_ref_count_get(unit,
                                         stage_id,
                                         pipe_id,
                                         &ref_counts));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_profile_add(unit,
                           trans_id,
                           pipe_id,
                           tbl_id,
                           1,
                           em_pdd_profile_sid,
                           profile_data,
                           ref_counts,
                           &index));

    if (TRUE == default_entry) {
        ext_codes->default_pdd_prof_index = (uint32_t)index;
    } else {
        ext_codes->pdd_prof_index = (uint32_t)index;
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_bcm56990_b0_em_pdd_profile_uninstall(int unit,
                                           uint32_t trans_id,
                                           bool default_entry,
                                           void *group_opinfo,
                                           bcmfp_stage_t *stage)
{
    int index = -1;
    bcmfp_ref_count_t *ref_counts = NULL;
    bcmltd_sid_t req_ltid;
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
    ext_codes = &(opinfo->ext_codes[0]);
    pipe_id = opinfo->tbl_inst;

    if (TRUE == default_entry) {
        index = (int)ext_codes->default_pdd_prof_index;
    } else {
        index = (int)ext_codes->pdd_prof_index;
    }

    req_ltid = stage->tbls.group_tbl->sid;
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
                                  1,
                                  em_pdd_profile_sid,
                                  ref_counts,
                                  index));
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_bcm56990_b0_em_qos_profile_install(int unit,
                                         uint32_t trans_id,
                                         bool default_entry,
                                         bcmfp_stage_t *stage,
                                         void *group_opinfo,
                                         uint32_t *qos_policy_data)
{
    uint32_t *prof_entry = NULL;
    bcmfp_ref_count_t *ref_counts = NULL;
    uint32_t *profile_data[] = { ap_entry[unit] };
    bcmfp_group_oper_info_t *opinfo = NULL;
    bcmfp_stage_id_t stage_id;
    bcmltd_sid_t tbl_id;
    int pipe_id = -1;
    size_t size = 0;
    int index = -1;
    bcmfp_ext_codes_t *ext_codes = NULL;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    tbl_id = stage->tbls.group_tbl->sid;
    ext_codes = opinfo->ext_codes;
    pipe_id = opinfo->tbl_inst;

    prof_entry = ap_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);

    bcmdrd_field_set(prof_entry, 0, 107, qos_policy_data);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_qos_prof_ref_count_get(unit,
                                      stage_id,
                                      pipe_id,
                                      &ref_counts));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_profile_add(unit,
                           trans_id,
                           pipe_id,
                           tbl_id,
                           1,
                           em_qos_profile_sid,
                           profile_data,
                           ref_counts,
                           &index));
    if (TRUE == default_entry) {
        ext_codes->default_qos_prof_index = (uint32_t)index;
    } else {
        ext_codes->qos_prof_index = (uint32_t)index;
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_bcm56990_b0_em_qos_profile_uninstall(int unit,
                                           uint32_t trans_id,
                                           bool default_entry,
                                           bcmfp_stage_t *stage,
                                           void *group_opinfo)
{
    bcmfp_ref_count_t *ref_counts = NULL;
    bcmltd_sid_t req_ltid;
    bcmfp_stage_id_t stage_id;
    bcmfp_group_oper_info_t *opinfo = NULL;
    int pipe_id = -1;
    bcmfp_ext_codes_t *ext_codes = NULL;
    int index = -1;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    pipe_id = opinfo->tbl_inst;
    ext_codes = &(opinfo->ext_codes[0]);
    req_ltid = stage->tbls.group_tbl->sid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_qos_prof_ref_count_get(unit,
                                      stage_id,
                                      pipe_id,
                                      &ref_counts));
    if (TRUE == default_entry) {
        index = (int)ext_codes->default_qos_prof_index;
    } else {
        index = (int)ext_codes->qos_prof_index;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_profile_delete(unit,
                              trans_id,
                              pipe_id,
                              req_ltid,
                              1,
                              em_qos_profile_sid,
                              ref_counts,
                              index));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56990_b0_em_buffers_custom_set(int unit,
                                     uint32_t trans_id,
                                     uint32_t flags,
                                     bcmfp_stage_t *stage,
                                     bcmfp_group_oper_info_t *opinfo,
                                     bcmfp_pdd_config_t *pdd_config,
                                     bcmfp_key_ids_t *key_ids,
                                     bcmfp_buffers_t *buffers)
{
    uint32_t *prof_entry = NULL;
    uint8_t num_parts = 0;
    bcmdrd_sid_t em_mem_sid;
    uint32_t entry_buffer[BCMFP_ENTRY_WORDS_MAX];
    uint32_t bit, start_bit;
    uint32_t value = 0;
    uint32_t value_1 = 1;
    uint32_t value_2 = 2;
    uint32_t value_7 = 7;
    uint32_t value2 = 0;
    size_t size = 0;
    bcmfp_group_mode_t group_mode;
    uint32_t em_class_id = 0;
    uint32_t pdd_prof_index = 0;
    uint32_t qos_prof_index = 0;
    uint32_t **ekw = NULL;
    uint32_t **edw = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key_ids, SHR_E_PARAM);
    SHR_NULL_CHECK(buffers, SHR_E_PARAM);

    ekw = buffers->ekw;
    edw = buffers->edw;

    group_mode = opinfo->group_mode;
    prof_entry = ap_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);
    sal_memset(entry_buffer, 0, size);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit,
                                 FALSE,
                                 opinfo->flags,
                                 &num_parts));

    em_mem_sid = em_entry_sids[0];
    value = 1;
    if (opinfo->group_mode == BCMFP_GROUP_MODE_DBLINTER) {
        /* set the em mem as wide mem */
        em_mem_sid = em_entry_sids[1];
        /* set the valid and key type fields */
        if ((flags & BCMFP_BUFFERS_CUSTOMIZE_EDW) &&
             (flags & BCMFP_BUFFERS_CUSTOMIZE_EKW)) {
            bcmdrd_pt_field_set(unit,
                                em_mem_sid,
                                prof_entry,
                                BASE_VALID_2f,
                                &value_2);
            bcmdrd_pt_field_set(unit,
                                em_mem_sid,
                                prof_entry,
                                BASE_VALID_3f,
                                &value_7);
        }
        value = stage->key_data_size_info->em_mode[group_mode];
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            KEY_TYPEf,
                            &value);
    }

    /* set the valid and key type fields */
    value = 1;
    if (flags & BCMFP_BUFFERS_CUSTOMIZE_EDW) {
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            BASE_VALID_0f,
                            &value_1);
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            BASE_VALID_1f,
                            &value_2);
    }
    value = stage->key_data_size_info->em_mode[group_mode];
    bcmdrd_pt_field_set(unit,
                        em_mem_sid,
                        prof_entry,
                        KEY_TYPEf,
                        &value);

    /* update key field */
    if (group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_WIDE) {
        /* 128 bit mode */
        for (start_bit = 0,
            bit = 0;
            bit < 20;
            start_bit++,bit++) {
            if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
            }
        }
        value = stage->key_data_size_info->key_part_size[group_mode][0];
        for (start_bit = 52; bit < value; start_bit++,bit++) {
            if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
            }
        }

        value = stage->key_data_size_info->key_parts_fid[group_mode][0];
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            value,
                            entry_buffer);

        sal_memset(entry_buffer, 0, size);
        value = stage->key_data_size_info->key_part_size[group_mode][1];
        for (bit = 0; bit < value; start_bit++,bit++) {
            if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
            }
        }
        value = stage->key_data_size_info->key_parts_fid[group_mode][1];
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            value,
                            entry_buffer);
    } else if (group_mode == BCMFP_GROUP_MODE_SINGLE_ASET_NARROW) {
        /* 160 bit mode */
        value = stage->key_data_size_info->key_parts_fid[group_mode][0];
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            value,
                            ekw[0]);
        value = stage->key_data_size_info->key_part_size[group_mode][0];
        value2 = stage->key_data_size_info->key_part_size[group_mode][1];
        for (start_bit = value, bit = 0; bit < value2; start_bit++,bit++) {
            if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
            }
        }
        value = stage->key_data_size_info->key_parts_fid[group_mode][1];
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            value,
                            entry_buffer);
    } else {
        /* 320 bit mode */
        value = stage->key_data_size_info->key_parts_fid[group_mode][0];
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            value,
                            ekw[0]);
        value = stage->key_data_size_info->key_part_size[group_mode][0];
        for (start_bit = value,bit = 0; start_bit < 160; start_bit++,bit++) {
             if (SHR_BITGET(ekw[0], start_bit)) {
                 SHR_BITSET(entry_buffer, bit);
             }
        }

        value = stage->key_data_size_info->key_part_size[group_mode][1];
        for (start_bit = 0; bit < value; start_bit++,bit++) {
            if (SHR_BITGET(ekw[1], start_bit)) {
                SHR_BITSET(entry_buffer, bit);
            }
        }
        value = stage->key_data_size_info->key_parts_fid[group_mode][1];
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            value,
                            entry_buffer);

        sal_memset(entry_buffer, 0, size);
        value = stage->key_data_size_info->key_part_size[group_mode][2];
        for (bit = 0; bit < value; start_bit++,bit++) {
            if (SHR_BITGET(ekw[1], start_bit)) {
                SHR_BITSET(entry_buffer, bit);
            }
        }
        value = stage->key_data_size_info->key_parts_fid[group_mode][2];
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            value,
                            entry_buffer);
    }

    /* clear EKW */
    sal_memset(ekw[0], 0, size);
    sal_memset(ekw[1], 0, size);

    /* Update EKW */
    sal_memcpy(ekw[0], prof_entry, size);

    if (flags & BCMFP_BUFFERS_CUSTOMIZE_EDW) {
        /* Build EDW */
        sal_memset(prof_entry, 0, size);
        value = stage->key_data_size_info->policy_data_fid[group_mode];
        bcmdrd_pt_field_set(unit,
                            em_mem_sid,
                            prof_entry,
                            value,
                            edw[0]);
        /* Action profile ID */
        if (-1 != opinfo->ext_codes[0].pdd_prof_index) {
            pdd_prof_index = opinfo->ext_codes[0].pdd_prof_index;
            value =
            stage->key_data_size_info->action_prof_id_fid[group_mode];
            bcmdrd_pt_field_set(unit,
                                em_mem_sid,
                                prof_entry,
                                value,
                                &pdd_prof_index);
        }
        /* QOS profile ID */
        if (-1 != opinfo->ext_codes[0].qos_prof_index) {
            qos_prof_index = opinfo->ext_codes[0].qos_prof_index;
            value =
            stage->key_data_size_info->qos_prof_id_fid[group_mode];
            bcmdrd_pt_field_set(unit,
                                em_mem_sid,
                                prof_entry,
                                value,
                                &qos_prof_index);
        }
        /* EM class id */
        if (0 != opinfo->ext_codes[0].em_class_id) {
            value =
            stage->key_data_size_info->em_class_id_fid[group_mode];
            em_class_id = opinfo->ext_codes[0].em_class_id;
            bcmdrd_pt_field_set(unit,
                                em_mem_sid,
                                prof_entry,
                                value,
                                &em_class_id);
        }
        /* Clear EDW */
        sal_memset(edw[0], 0, size);
        /* update EDW */
        sal_memcpy(edw[0], prof_entry, size);
   }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_bcm56990_b0_em_dpolicy_install(int unit,
                                  uint32_t trans_id,
                                  bcmfp_stage_t *stage,
                                  void *group_opinfo,
                                  uint32_t *edw)
{
    uint32_t value = 0;
    uint32_t *prof_entry = NULL;
    bcmdrd_fid_t fid;
    uint32_t *profile_data[] = { ap_entry[unit] };
    bcmfp_group_oper_info_t *opinfo = NULL;
    int pipe_id = -1;
    bcmltd_sid_t tbl_id;
    bcmfp_ext_codes_t *ext_codes = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    pipe_id = opinfo->tbl_inst;
    ext_codes = &(opinfo->ext_codes[0]);
    tbl_id = stage->tbls.group_tbl->sid;

    prof_entry = ap_entry[unit];
    size = (sizeof(uint32_t) * BCMFP_ENTRY_WORDS_MAX);
    sal_memset(prof_entry, 0, size);

    fid = ACTION_DATAf;
    bcmdrd_pt_field_set(unit,
                        em_default_policy_sid[0],
                        prof_entry,
                        fid,
                        edw);

    value = ext_codes->default_pdd_prof_index;
    fid = ACTION_PROFILE_IDf;
    bcmdrd_pt_field_set(unit,
                        em_default_policy_sid[0],
                        prof_entry,
                        fid,
                        &value);

    value = ext_codes->default_qos_prof_index;
    fid = QOS_PROFILE_IDf;
    bcmdrd_pt_field_set(unit,
                        em_default_policy_sid[0],
                        prof_entry,
                        fid,
                        &value);

    value = ext_codes->default_em_class_id;
    fid = EXACT_MATCH_CLASS_IDf;
    bcmdrd_pt_field_set(unit,
                        em_default_policy_sid[0],
                        prof_entry,
                        fid,
                        &value);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_index_insert(unit,
                               trans_id,
                               pipe_id,
                               tbl_id,
                               em_default_policy_sid[0],
                               profile_data[0],
                               ext_codes->keygen_index));

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_bcm56990_b0_em_dpolicy_uninstall(int unit,
                                       uint32_t trans_id,
                                       void *group_opinfo,
                                       bcmfp_stage_t *stage)
{
    bcmltd_sid_t req_ltid;
    bcmfp_group_oper_info_t *opinfo = NULL;
    int pipe_id = -1;
    bcmfp_ext_codes_t *ext_codes = NULL;

    SHR_FUNC_ENTER(unit);

    opinfo = group_opinfo;
    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    pipe_id = opinfo->tbl_inst;
    req_ltid = stage->tbls.group_tbl->sid;
    ext_codes = &(opinfo->ext_codes[0]);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ptm_index_delete(unit,
                                trans_id,
                                pipe_id,
                                req_ltid,
                                em_default_policy_sid[0],
                                ext_codes->keygen_index));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56990_b0_em_profiles_install(int unit,
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

    if (flags & BCMFP_GROUP_INSTALL_KEYGEN_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_keygen_install(unit,
                                                 trans_id,
                                                 stage,
                                                 (void *)opinfo,
                                                 num_parts));
    }

    if (flags & BCMFP_GROUP_INSTALL_PDD_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_pdd_profile_install(unit,
                                                 trans_id,
                                                 FALSE,
                                                 (void *)opinfo,
                                                 stage,
                                                 buffers->pdd[0]));
    }

    if (flags & BCMFP_GROUP_INSTALL_QOS_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_qos_profile_install(unit,
                                                 trans_id,
                                                 FALSE,
                                                 stage,
                                                 (void *)opinfo,
                                                 buffers->qos[0]));
    }

    if (flags & BCMFP_GROUP_INSTALL_DPDD_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_pdd_profile_install(unit,
                                                 trans_id,
                                                 TRUE,
                                                 (void *)opinfo,
                                                 stage,
                                                 buffers->dpdd[0]));
    }

    if (flags & BCMFP_GROUP_INSTALL_DQOS_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_qos_profile_install(unit,
                                                 trans_id,
                                                 TRUE,
                                                 stage,
                                                 (void *)opinfo,
                                                 buffers->dqos[0]));
    }

    if (flags & BCMFP_GROUP_INSTALL_DPOLICY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_dpolicy_install(unit,
                                                 trans_id,
                                                 stage,
                                                 (void *)opinfo,
                                                 buffers->dedw[0]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_bcm56990_b0_em_profiles_uninstall(int unit,
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

    if (flags & BCMFP_GROUP_INSTALL_KEYGEN_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_keygen_uninstall(unit,
                                                   trans_id,
                                                   (void *)opinfo,
                                                   stage,
                                                   num_parts));
    }

    if (flags & BCMFP_GROUP_INSTALL_PDD_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_pdd_profile_uninstall(unit,
                                                   trans_id,
                                                   FALSE,
                                                   (void *)opinfo,
                                                   stage));
    }

    if (flags & BCMFP_GROUP_INSTALL_QOS_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_qos_profile_uninstall(unit,
                                                   trans_id,
                                                   FALSE,
                                                   stage,
                                                   (void *)opinfo));
    }

    if (flags & BCMFP_GROUP_INSTALL_DPDD_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_pdd_profile_uninstall(unit,
                                                   trans_id,
                                                   TRUE,
                                                   (void *)opinfo,
                                                   stage));
    }

    if (flags & BCMFP_GROUP_INSTALL_DQOS_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_qos_profile_uninstall(unit,
                                                   trans_id,
                                                   TRUE,
                                                   stage,
                                                   (void *)opinfo));
    }

    if (flags & BCMFP_GROUP_INSTALL_DPOLICY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_em_dpolicy_uninstall(unit,
                                                   trans_id,
                                                   (void *)opinfo,
                                                   stage));
    }

exit:
    SHR_FUNC_EXIT();
}

