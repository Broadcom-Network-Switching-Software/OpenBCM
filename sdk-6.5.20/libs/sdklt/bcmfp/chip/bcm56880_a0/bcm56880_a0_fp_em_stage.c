/*! \file bcm56880_a0_fp_em_stage.c
 *
 * API to initialize stage attributes for
 * Trident4(56880_A0) device.
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
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm_uft.h>
#include <bcmfp/bcmfp_strings_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56880_a0_fp_em_bank_info_set(int unit,
                                bcmfp_stage_t *stage)
{
    int rv = SHR_E_NONE;
    uint32_t idx = 0;
    uint32_t num_sbr_profiles = 0;
    uint32_t num_hit_profiles = 0;
    uint32_t ptinfo[BCMFP_EM_PTID_COUNT];
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    bcmltd_sid_t ltid = 0;
    bcmdrd_sid_t ptid = 0;
    bcmdrd_sid_t *hit_sid_arr = NULL;
    bcmdrd_sid_t *sid_arr = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;

    SHR_FUNC_ENTER(unit);

    ltid = stage->tbls.entry_tbl->sid;
    rv = bcmptm_cth_uft_lt_lts_tcam_get(unit, ltid,
                                        NULL, 0, &r_cnt);
    if (SHR_FAILURE(rv) || (r_cnt == 0)) {
        /* Zero or one bank. No need to copy. */
        stage->sbr_hw_info->num_sbr_sids = 0;
        stage->profiles_hw_info.sbr_profile_hw_info->rows = 0;
        stage->profiles_hw_info.sbr_profile_hw_info->columns = 0;
        stage->hit_context->hit_idx_profiles = 0;
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
    BCMFP_ALLOC(hit_sid_arr, (r_cnt * sizeof(bcmdrd_sid_t)), "bcmfpHitSidArr");
    for (idx = 0; idx < r_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56880_a0_fp_em_bank_sids_get(unit,
                                             ptsid_arr[idx],
                                             ptinfo));
        ptid = ptinfo[BCMFP_EM_SBR_PTID];
        if (!bcm56880_a0_fp_em_sid_exists(unit,
                                          sid_arr,
                                          num_sbr_profiles,
                                          ptid)) {
            sid_arr[num_sbr_profiles] = ptid;
            num_sbr_profiles++;
        }

        ptid = ptinfo[BCMFP_EM_HIT_INDEX_PTID];
        if (!bcm56880_a0_fp_em_sid_exists(unit,
                                          hit_sid_arr,
                                          num_hit_profiles,
                                          ptid)) {
            hit_sid_arr[num_hit_profiles] = ptid;
            num_hit_profiles++;
        }
    }

    stage->sbr_hw_info->num_sbr_sids = num_sbr_profiles;
    stage->profiles_hw_info.sbr_profile_hw_info->rows = num_sbr_profiles;
    stage->profiles_hw_info.sbr_profile_hw_info->columns = 1;
    for (idx = 0; idx < num_sbr_profiles; idx++) {
        stage->sbr_hw_info->sbr_sid[idx] = sid_arr[idx];
        stage->profiles_hw_info.sbr_profile_hw_info->sids[idx][0] = sid_arr[idx];
    }

    stage->hit_context->hit_idx_profiles = num_hit_profiles;
    for (idx = 0; idx < num_hit_profiles; idx++) {
        stage->hit_context->hit_idx_profile_tbls[idx] = hit_sid_arr[idx];
    }

exit:
    if (hit_sid_arr) {
        SHR_FREE(hit_sid_arr);
    }
    if (sid_arr) {
        SHR_FREE(sid_arr);
    }
    if (ptsid_arr) {
        SHR_FREE(ptsid_arr);
    }
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_fp_em_device_consts_init(int unit,
                                     bcmfp_stage_t *stage)
{
    uint32_t idx = 0;
    uint32_t botp_bus_cont_id = 0;
    uint8_t mux_update = 0, mux_sel = 0;
    bcmfp_group_mode_t mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;
    bcmfp_qset_aset_size_info_t *key_data_sz_info = NULL;
    bcmfp_pdd_hw_section_info_t *section_info = NULL;
    bcmfp_tbl_group_t *grp_tbl = NULL;
    bcmfp_tbl_sbr_t *sbr_tbl = NULL;
    const bcmlrd_table_pcm_info_t *pcm_info = NULL;
    const bcmlrd_field_pdd_info_t *pdd_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);
    grp_tbl = stage->tbls.group_tbl;
    SHR_NULL_CHECK(grp_tbl, SHR_E_PARAM);

    sbr_tbl = stage->tbls.sbr_tbl;

    BCMFP_STAGE_FLAGS_INIT(stage);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_KEY_TYPE_PRESEL);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_PRESEL_KEY_DYNAMIC);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_ENTRY_TYPE_HASH);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_POLICY_TYPE_PDD);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_PDD_PROFILES_GLOBAL);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_PSG_PROFILES_GLOBAL);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_NO_METER_SUPPORT);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_DEFAULT_POLICY);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_PDD_INFO_ONLY_IN_FIRST_SLICE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_IDP_MAP_TYPE_HASH);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_MODE_HALF_NOT_SUPPORTED);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_UFT_BANKS_DYNAMIC);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_EFLEX_CTR_SUPPORT);

    if (sbr_tbl != NULL) {
        BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_SBR_PROFILES_GLOBAL);
        BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_ACTION_RESOLUTION_SBR);
        BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_SBR_UFTMGR_SUPPORT);
    }
    if (sbr_tbl != NULL &&
        stage->stage_id == BCMFP_STAGE_ID_EXACT_MATCH) {
        /* add per entry SBR support for EM_FP alone */
        BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_SBR_PER_ENTRY);
    }

    if (stage->stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
        BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_HW_FLOW_TRACKER);
    }

    for (mode = 0; mode < BCMFP_GROUP_MODE_COUNT; mode++) {
        stage->hw_entry_info[mode] = NULL;
    }

    stage->num_group_action_res_ids = 16;
    stage->num_group_sbr_ids = 32;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_idp_maps_modulo_set(unit, stage));

    /* Aset and Qset size stucture */
    BCMFP_ALLOC(key_data_sz_info,
                sizeof(bcmfp_qset_aset_size_info_t),
                "bcmfpEmKeyDataSizeInfo");

    /* Aset narrow Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpEmHwEntryInfoNarrowGrp");
    hw_entry_info->sid = IFTA90_E2T_00_B0_SINGLEm;
    hw_entry_info->sid_data_only = IFTA90_E2T_00_B0_SINGLEm;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = IFTA90_E2T_00_LTS_TCAM_0_ONLYm;
    hw_entry_info->presel_sid_data_only = IFTA90_E2T_00_LTS_TCAM_0_DATA_ONLYm;
    hw_entry_info->presel_aggr_view = 0;
    hw_entry_info->max_entries = 131072;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->max_non_conflicting_groups = 2;
    stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_SINGLE);
    stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_SINGLE);
    hw_entry_info = NULL;

    key_data_sz_info->key_size[BCMFP_GROUP_MODE_SINGLE] = 113;
    key_data_sz_info->data_size[BCMFP_GROUP_MODE_SINGLE] = 113;
    key_data_sz_info->key_data_size[BCMFP_GROUP_MODE_SINGLE] = 113;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_SINGLE][0] = 113;

   /* The MSB index of the data portion for single mode entries */
    key_data_sz_info->data_start_offset[BCMFP_GROUP_MODE_SINGLE] = 119;

    /* Aset Wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpEmHwEntryInfoWideGrp");
    hw_entry_info->sid = IFTA90_E2T_00_B0_DOUBLEm;
    hw_entry_info->sid_data_only = IFTA90_E2T_00_B0_DOUBLEm;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = IFTA90_E2T_00_LTS_TCAM_0_ONLYm;
    hw_entry_info->presel_sid_data_only = IFTA90_E2T_00_LTS_TCAM_0_DATA_ONLYm;
    hw_entry_info->presel_aggr_view = 0;
    hw_entry_info->max_entries = 65536;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->max_non_conflicting_groups = 2;
    stage->hw_entry_info[BCMFP_GROUP_MODE_DBLINTER] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_DBLINTER);
    hw_entry_info = NULL;

    key_data_sz_info->key_size[BCMFP_GROUP_MODE_DBLINTER] = 230;
    key_data_sz_info->data_size[BCMFP_GROUP_MODE_DBLINTER] = 200;
    key_data_sz_info->key_data_size[BCMFP_GROUP_MODE_DBLINTER] = 230;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_DBLINTER][0] = 113;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_DBLINTER][1] = 117;

    /* The MSB index of the data portion for double mode entries */
    key_data_sz_info->data_start_offset[BCMFP_GROUP_MODE_DBLINTER] = 239;


    /* Inter Slice Double wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpEmHwEntryInfoDblInterGrp");
    hw_entry_info->sid = IFTA90_E2T_00_B0_QUADm;
    hw_entry_info->sid_data_only = IFTA90_E2T_00_B0_QUADm;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = IFTA90_E2T_00_LTS_TCAM_0_ONLYm;
    hw_entry_info->presel_sid_data_only = IFTA90_E2T_00_LTS_TCAM_0_DATA_ONLYm;
    hw_entry_info->presel_aggr_view = 0;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->max_entries = 32768;
    hw_entry_info->max_non_conflicting_groups = 1;
    stage->hw_entry_info[BCMFP_GROUP_MODE_QUAD] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_QUAD);
    hw_entry_info = NULL;

    key_data_sz_info->key_size[BCMFP_GROUP_MODE_QUAD] = 347;
    key_data_sz_info->data_size[BCMFP_GROUP_MODE_QUAD] = 200;
    key_data_sz_info->key_data_size[BCMFP_GROUP_MODE_QUAD] = 464;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_QUAD][0] = 113;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_QUAD][1] = 117;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_QUAD][2] = 117;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_QUAD][3] = 117;
    /* The MSB index of the data portion for quad mode entries */
    key_data_sz_info->data_start_offset[BCMFP_GROUP_MODE_QUAD] = 479;

    stage->key_data_size_info = key_data_sz_info;
    key_data_sz_info = NULL;

    /* Initialize the stage miscellaneous information */
    BCMFP_ALLOC(stage->misc_info,
                sizeof(bcmfp_stage_misc_info_t),
                "bcmfpEmStageMiscInfo");
    stage->misc_info->em_key_attribute_sid = IFTA90_E2T_00_KEY_ATTRIBUTESm;
    stage->misc_info->num_group_action_res_ids = 16;
    stage->misc_info->num_groups = 16;
    stage->misc_info->num_keygen_prof = 0;
    stage->misc_info->num_presel_groups = 1;
    stage->misc_info->num_pdd_profiles = 32;
    stage->misc_info->num_sbr_profiles = 32;
    stage->misc_info->num_em_key_attrib_profile = 16;
    stage->misc_info->max_em_banks = 12;
    stage->misc_info->default_policy_data_width = 200;
    stage->misc_info->per_entry_pdd_raw_data_offset = 9;
    stage->misc_info->per_entry_sbr_raw_data_offset = 9;
    stage->misc_info->per_entry_pdd_sbr_raw_data_offset = 9;
    stage->misc_info->per_group_pdd_sbr_raw_data_offset = 0;
    stage->misc_info->sbr_index_size = 4;

    /* PDD HW Info */
    BCMFP_ALLOC(stage->pdd_hw_info,
                sizeof(bcmfp_pdd_hw_info_t),
                "bcmfpEmStagePddHwInfo");
    stage->pdd_hw_info->num_max_pdd_parts = 1;
    stage->pdd_hw_info->pdd_bitmap_size = 177;
    stage->pdd_hw_info->raw_policy_width = 200;
    stage->pdd_hw_info->read_pdd_bitmap_from_lsb = FALSE;
    stage->pdd_hw_info->write_pdd_data_from_lsb = FALSE;
    stage->pdd_hw_info->section_align_bits = 8;

    /* PDD Section Info */
    BCMFP_ALLOC(section_info,
                sizeof(bcmfp_pdd_hw_section_info_t),
                "bcmfpEmStagePddSectionInfo");
    section_info->num_pdd_sections = 3;
    section_info->section_id[0] = 0;
    section_info->section_start_bit[0] = 0;
    section_info->section_end_bit[0] = 58;
    section_info->section_id[1] = 1;
    section_info->section_start_bit[1] = 59;
    section_info->section_end_bit[1] = 117;
    section_info->section_id[2] = 2;
    section_info->section_start_bit[2] = 118;
    section_info->section_end_bit[2] = 176;
    stage->pdd_hw_info->section_hw_info = section_info;

    /* SBR HW information */
    BCMFP_ALLOC(stage->sbr_hw_info,
                sizeof(bcmfp_sbr_hw_info_t),
                "bcmfpEmStageSbrHwInfo");
    stage->sbr_hw_info->sbr_profile_sid_columns = 1;

    /* SBR Priority field info */
    BCMFP_ALLOC(stage->sbr_hw_info->priority_field_info,
                sizeof(bcmfp_stage_hw_field_info_t),
                "bcmfpEmStageSbrPriorityFieldHwInfo");

    stage->sbr_hw_info->priority_field_info[0].num_offsets = 1;

    BCMFP_ALLOC(stage->sbr_hw_info->priority_field_info[0].offset,
                sizeof(uint16_t),
                "bcmfpEmStageSbrPriorityFieldFirstColumnOffset");
    BCMFP_ALLOC(stage->sbr_hw_info->priority_field_info[0].width,
                sizeof(uint16_t),
                "bcmfpEmStageSbrPriorityFieldFirstColumnWidth");
    stage->sbr_hw_info->priority_field_info[0].offset[0] = 0;
    stage->sbr_hw_info->priority_field_info[0].width[0] = 528;

    if (stage->stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
        stage->sbr_hw_info->num_sbr_sids = 2;
        BCMFP_ALLOC(stage->sbr_hw_info->sbr_sid,
                    (sizeof(bcmdrd_sid_t) * 2),
                    "bcmfpEmStageSbrSids");
        stage->sbr_hw_info->sbr_sid[0] = IFTA80_SBR_PROFILE_TABLE_1m;
        stage->sbr_hw_info->sbr_sid[1] = IFTA80_SBR_PROFILE_TABLE_2m;
    } else {
        stage->sbr_hw_info->num_sbr_sids = 4;
        BCMFP_ALLOC(stage->sbr_hw_info->sbr_sid,
                    (sizeof(bcmdrd_sid_t) * 4),
                    "bcmfpEmStageSbrSids");
        stage->sbr_hw_info->sbr_sid[0] = IFTA90_SBR_PROFILE_TABLE_0m;
        stage->sbr_hw_info->sbr_sid[1] = IFTA90_SBR_PROFILE_TABLE_1m;
        stage->sbr_hw_info->sbr_sid[2] = IFTA80_SBR_PROFILE_TABLE_1m;
        stage->sbr_hw_info->sbr_sid[3] = IFTA80_SBR_PROFILE_TABLE_2m;
    }
    /*
     * For EM and EM_FT only half of the sbr entries can be used.
     * The upper half and lower half of the sbr profile table
     * for EM and EM_FT need to be mirrored.
     */
    stage->sbr_hw_info->max_sbr_idx = 15;
    stage->sbr_hw_info->min_sbr_idx = 0;

    /* SBR profile hardware information. */
    BCMFP_ALLOC(stage->profiles_hw_info.sbr_profile_hw_info,
                sizeof(bcmfp_profile_hw_info_t),
                "bcmfpEmStageSbrProfileHwInfo");
    stage->profiles_hw_info.sbr_profile_hw_info->columns = 1;
    if (stage->stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
        stage->profiles_hw_info.sbr_profile_hw_info->rows = 2;
        stage->profiles_hw_info.sbr_profile_hw_info->sids[0][0] =
            IFTA80_SBR_PROFILE_TABLE_1m;
        stage->profiles_hw_info.sbr_profile_hw_info->sids[1][0] =
            IFTA80_SBR_PROFILE_TABLE_2m;
    } else {
        stage->profiles_hw_info.sbr_profile_hw_info->rows = 4;
        stage->profiles_hw_info.sbr_profile_hw_info->sids[0][0] =
            IFTA90_SBR_PROFILE_TABLE_0m;
        stage->profiles_hw_info.sbr_profile_hw_info->sids[1][0] =
            IFTA90_SBR_PROFILE_TABLE_1m;
        stage->profiles_hw_info.sbr_profile_hw_info->sids[2][0] =
            IFTA80_SBR_PROFILE_TABLE_1m;
        stage->profiles_hw_info.sbr_profile_hw_info->sids[3][0] =
            IFTA80_SBR_PROFILE_TABLE_2m;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_pcm_conf_get(unit, grp_tbl->hit_index_sid, &pcm_info));
    if (!pcm_info->field_count || pcm_info->field_info == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Hit index LT is missing for %s "
                 "stage\n"), bcmfp_stage_string(stage->stage_id)));
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < pcm_info->field_count; idx++) {
        if (pcm_info->field_info[idx].id == grp_tbl->hit_index_fid) {
            pdd_info = pcm_info->field_info[idx].pdd_info;
            break;
        }
    }
    if ((pdd_info == NULL) || (pdd_info->count == 0) || (pdd_info->info == NULL)) {
        LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Hit index container info is missing for %s "
                 "stage\n"), bcmfp_stage_string(stage->stage_id)));
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * BOTP_MUX_DATA is a 5-bit field.
     * MSB (bit 4) indicates if the MUX is enabled.
     * Remaining 4 bits indicate the mux select value.
     * BOTP_MUX has ING_OBJ1 (20 containers of 16 bits = 320 bits) as the input bus.
     * MUX output is a 32 bit value which requires 2 containers to be updated.
     * To get the mux_sel value, the container id has to be divided by 2.
     */
    botp_bus_cont_id = (pdd_info->info[0].phy_cont_id - 130);
    mux_sel = (botp_bus_cont_id / 2);
    mux_update = (1 << 4);

    /* hit context info */
    BCMFP_ALLOC(stage->hit_context,
                sizeof(bcmfp_hit_context_t),
                "bcmfpEmStageHitContextInfo");
    stage->hit_context->botp_value = (mux_update | mux_sel);
    if (stage->stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
        stage->hit_context->hit_idx_profiles = 2;
        stage->hit_context->hit_idx_profile_tbls[0] = IFTA80_E2T_00_HIT_INDEX_PROFILEm;
        stage->hit_context->hit_idx_profile_tbls[1] = IFTA80_E2T_01_HIT_INDEX_PROFILEm;
    } else {
        stage->hit_context->hit_idx_profiles = 8;
        stage->hit_context->hit_idx_profile_tbls[0] = IFTA90_E2T_00_HIT_INDEX_PROFILEm;
        stage->hit_context->hit_idx_profile_tbls[1] = IFTA90_E2T_01_HIT_INDEX_PROFILEm;
        stage->hit_context->hit_idx_profile_tbls[2] = IFTA90_E2T_02_HIT_INDEX_PROFILEm;
        stage->hit_context->hit_idx_profile_tbls[3] = IFTA90_E2T_03_HIT_INDEX_PROFILEm;
        stage->hit_context->hit_idx_profile_tbls[4] = IFTA80_E2T_00_HIT_INDEX_PROFILEm;
        stage->hit_context->hit_idx_profile_tbls[5] = IFTA80_E2T_01_HIT_INDEX_PROFILEm;
        stage->hit_context->hit_idx_profile_tbls[6] = IFTA80_E2T_02_HIT_INDEX_PROFILEm;
        stage->hit_context->hit_idx_profile_tbls[7] = IFTA80_E2T_03_HIT_INDEX_PROFILEm;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_fp_em_bank_info_set(unit, stage));
exit:
    /*
     * Allocated memory is cleaned up by bcmfp_em_cleanup()
     * during component shutdown.
     */
    /* coverity[leaked_storage] */
    SHR_FUNC_EXIT();
}
