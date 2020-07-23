/*! \file bcm56990_b0_fp_em_stage.c
 *
 * API to initialize stage attributes for Tomahawk-4 B0(56990_B0) device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/chip/bcm56990_b0_fp.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_b0_exactmatch_device_consts_init(int unit,
                                                bcmfp_stage_t *stage)
{
    bcmfp_group_mode_t mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;
    bcmfp_qset_aset_size_info_t *key_data_sz_info = NULL;
    bcmfp_pdd_hw_section_info_t *section_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    BCMFP_STAGE_FLAGS_INIT(stage);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_KEY_TYPE_PRESEL);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_ENTRY_TYPE_HASH);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_POLICY_TYPE_PDD);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_KEYGEN_PROFILE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_ACTION_PROFILE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_DEFAULT_POLICY);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_PDD_PROFILES_GLOBAL);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_EFLEX_CTR_SUPPORT);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_ACTION_QOS_PROFILE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_PDD_INFO_ONLY_IN_FIRST_SLICE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_PDD_READ_FROM_MSB);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_IDP_MAP_TYPE_HASH);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_IPBM_SUPPORT);

    for (mode = 0; mode < BCMFP_GROUP_MODE_COUNT; mode++) {
        stage->hw_entry_info[mode] = NULL;
    }
    /* max key gen profiles available for EM are only 16 */
    stage->num_group_action_res_ids = 16;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_idp_maps_modulo_set(unit, stage));

    /* Aset and Qset size stucture */
    BCMFP_ALLOC(key_data_sz_info,
                sizeof(bcmfp_qset_aset_size_info_t),
                "bcmfpEmKeyDataSizeInfo");

    /* Aset Narrow Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpEmHwEntryInfoNarrowGrp");
    hw_entry_info->sid = EXACT_MATCH_2m;
    hw_entry_info->sid_data_only = EXACT_MATCH_2m;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = EXACT_MATCH_LOGICAL_TABLE_SELECTm;
    hw_entry_info->presel_sid_data_only = EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->max_entries = 16384;
    hw_entry_info->max_non_conflicting_groups = 2;
    stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_SINGLE_ASET_NARROW);
    hw_entry_info = NULL;
    key_data_sz_info->key_size[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] = 160;
    key_data_sz_info->data_size[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] = 18;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW][0] = 115;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW][1] = 45;
    key_data_sz_info->policy_data_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] =
                                         MODE160v_POLICY_DATAf;
    key_data_sz_info->data_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] =
                                         MODE160v_DATAf;
    key_data_sz_info->action_prof_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] =
                                         MODE160v_DATA_TYPEf;
    key_data_sz_info->qos_prof_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] =
                                         MODE160v_QOS_PROFILE_IDf;
    key_data_sz_info->em_class_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] =
                                         MODE160v_EXACT_MATCH_CLASS_IDf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW][0] =
                                         MODE160v_KEY_0_ONLYf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW][1] =
                                         MODE160v_KEY_1_ONLYf;
    key_data_sz_info->em_mode[BCMFP_GROUP_MODE_SINGLE_ASET_NARROW] = 1;

    /* Aset Wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpEmHwEntryInfoWideGrp");
    hw_entry_info->sid = EXACT_MATCH_2m;
    hw_entry_info->sid_data_only = EXACT_MATCH_2m;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = EXACT_MATCH_LOGICAL_TABLE_SELECTm;
    hw_entry_info->presel_sid_data_only = EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->max_entries = 16384;
    hw_entry_info->max_non_conflicting_groups = 2;
    stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_SINGLE_ASET_WIDE);
    hw_entry_info = NULL;
    key_data_sz_info->key_size[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] = 128;
    key_data_sz_info->data_size[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] = 50;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE][0] = 115;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE][1] = 13;
    key_data_sz_info->policy_data_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] =
                                         MODE128v_POLICY_DATAf;
    key_data_sz_info->data_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] =
                                         MODE128v_DATAf;
    key_data_sz_info->action_prof_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] =
                                         MODE128v_DATA_TYPEf;
    key_data_sz_info->qos_prof_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] =
                                         MODE128v_QOS_PROFILE_IDf;
    key_data_sz_info->em_class_id_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] =
                                         MODE128v_EXACT_MATCH_CLASS_IDf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE][0] =
                                         MODE128v_KEY_0_ONLYf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE][1] =
                                         MODE128v_KEY_1_ONLYf;
    key_data_sz_info->em_mode[BCMFP_GROUP_MODE_SINGLE_ASET_WIDE] = 0;

    /* Inter Slice Double wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpEmHwEntryInfoDblInterGrp");
    hw_entry_info->sid = EXACT_MATCH_4m;
    hw_entry_info->sid_data_only = EXACT_MATCH_4m;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = EXACT_MATCH_LOGICAL_TABLE_SELECTm;
    hw_entry_info->presel_sid_data_only = EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->max_entries = 16384;
    hw_entry_info->max_non_conflicting_groups = 1;
    stage->hw_entry_info[BCMFP_GROUP_MODE_DBLINTER] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_DBLINTER);
    hw_entry_info = NULL;
    key_data_sz_info->key_size[BCMFP_GROUP_MODE_DBLINTER] = 320;
    key_data_sz_info->data_size[BCMFP_GROUP_MODE_DBLINTER] = 60;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_DBLINTER][0] = 115;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_DBLINTER][1] = 117;
    key_data_sz_info->key_part_size[BCMFP_GROUP_MODE_DBLINTER][2] = 88;
    key_data_sz_info->policy_data_fid[BCMFP_GROUP_MODE_DBLINTER] =
                                         MODE320v_POLICY_DATAf;
    key_data_sz_info->data_fid[BCMFP_GROUP_MODE_DBLINTER] =
                                         MODE320v_DATAf;
    key_data_sz_info->action_prof_id_fid[BCMFP_GROUP_MODE_DBLINTER] =
                                         MODE320v_DATA_TYPEf;
    key_data_sz_info->qos_prof_id_fid[BCMFP_GROUP_MODE_DBLINTER] =
                                         MODE320v_QOS_PROFILE_IDf;
    key_data_sz_info->em_class_id_fid[BCMFP_GROUP_MODE_DBLINTER] =
                                         MODE320v_EXACT_MATCH_CLASS_IDf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_DBLINTER][0] =
                                         MODE320v_KEY_0_ONLYf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_DBLINTER][1] =
                                         MODE320v_KEY_1_ONLYf;
    key_data_sz_info->key_parts_fid[BCMFP_GROUP_MODE_DBLINTER][2] =
                                         MODE320v_KEY_2_ONLYf;
    key_data_sz_info->em_mode[BCMFP_GROUP_MODE_DBLINTER] = 2;

    stage->key_data_size_info = key_data_sz_info;
    key_data_sz_info = NULL;

    /* Initialize the stage miscellaneous information */
    BCMFP_ALLOC(stage->misc_info,
                sizeof(bcmfp_stage_misc_info_t),
                "bcmfpEmStageMiscInfo");
    stage->misc_info->num_group_action_res_ids = 16;
    stage->misc_info->num_groups = 16;
    stage->misc_info->num_keygen_prof = 16;
    stage->misc_info->num_qos_prof = 128;
    stage->misc_info->num_presel_groups = 0;
    stage->misc_info->num_pdd_profiles = 32;
    stage->misc_info->num_sbr_profiles = 0;
    stage->misc_info->default_policy_data_width = 60;
    stage->misc_info->per_entry_pdd_raw_data_offset = 0;
    stage->misc_info->per_entry_sbr_raw_data_offset = 0;
    stage->misc_info->per_entry_pdd_sbr_raw_data_offset = 0;
    stage->misc_info->per_group_pdd_sbr_raw_data_offset = 0;
    stage->misc_info->shared_action_res_id_stage_id_bmp =
                      (1 << BCMFP_STAGE_ID_INGRESS);

    /* PDD HW Info */
    BCMFP_ALLOC(stage->pdd_hw_info,
                sizeof(bcmfp_pdd_hw_info_t),
                "bcmfpIngStagePddHwInfo");
    stage->pdd_hw_info->num_max_pdd_parts = 1;
    stage->pdd_hw_info->pdd_bitmap_size = 27;
    stage->pdd_hw_info->raw_policy_width = 160;
    stage->pdd_hw_info->read_pdd_bitmap_from_lsb = TRUE;
    stage->pdd_hw_info->write_pdd_data_from_lsb = TRUE;
    stage->pdd_hw_info->section_align_bits = 0;

    /* PDD Section Info */
    BCMFP_ALLOC(section_info,
                sizeof(bcmfp_pdd_hw_section_info_t),
                "bcmfpIngStagePddSectionInfo");
    section_info->num_pdd_sections = 1;
    section_info->section_id[0] = 0;
    section_info->section_start_bit[0] = 0;
    section_info->section_end_bit[0] = 26;
    section_info->section_id[1] = 0;
    section_info->section_start_bit[1] = 0;
    section_info->section_end_bit[1] = 0;
    section_info->section_id[2] = 0;
    section_info->section_start_bit[2] = 0;
    section_info->section_end_bit[2] = 0;
    stage->pdd_hw_info->section_hw_info = section_info;

exit:
    SHR_FREE(key_data_sz_info);
    SHR_FUNC_EXIT();
}
