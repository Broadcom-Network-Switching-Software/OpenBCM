/*! \file bcm56780_a0_fp_vlan_stage.c
 *
 * API to initialize vlan stage attributes for
 * Trident4-X9(56780_A0) device.
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
#include <bcmfp/chip/bcm56780_a0_fp.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmptm/bcmptm_uft.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56780_a0_fp_vlan_bank_info_set(int unit,
                                bcmfp_stage_t *stage)
{
    int rv = SHR_E_NONE;
    int idx = 0;
    bcmdrd_sid_t bank_sid = 0;
    bcmdrd_sid_t sbr_sid[2] = {0};
    bcmptm_pt_banks_info_t  pt_banks_info;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_cth_uft_bank_info_get_from_lt(unit,
                                              stage->tbls.entry_tbl->sid,
                                              0,
                                              &pt_banks_info);
    if (SHR_FAILURE(rv) || (pt_banks_info.bank_cnt == 0)) {
        stage->sbr_hw_info->num_sbr_sids = 0;
        stage->profiles_hw_info.sbr_profile_hw_info->rows = 0;
        stage->profiles_hw_info.sbr_profile_hw_info->columns = 0;
        stage->sbr_hw_info->sbr_sid[0] = 0;
        stage->sbr_hw_info->sbr_sid[1] = 0;
        stage->profiles_hw_info.sbr_profile_hw_info->sids[0][0] = 0;
        stage->profiles_hw_info.sbr_profile_hw_info->sids[1][0] = 0;
        SHR_EXIT();
    }

    for (idx = 0; idx < pt_banks_info.bank_cnt; idx++) {
        bank_sid = pt_banks_info.bank[idx].bank_sid;
        switch (bank_sid) {
        case MEMDB_TCAM_IFTA40_MEM0_0_ONLYm:
        case MEMDB_TCAM_IFTA40_MEM0_1_ONLYm:
        case MEMDB_TCAM_IFTA40_MEM0_2_ONLYm:
        case MEMDB_TCAM_IFTA40_MEM0_3_ONLYm:
            sbr_sid[0] = IFTA40_SBR_PROFILE_TABLE_0m;
            break;
        case MEMDB_TCAM_IFTA40_MEM1_0_ONLYm:
        case MEMDB_TCAM_IFTA40_MEM1_1_ONLYm:
        case MEMDB_TCAM_IFTA40_MEM1_2_ONLYm:
        case MEMDB_TCAM_IFTA40_MEM1_3_ONLYm:
            sbr_sid[1] = IFTA40_SBR_PROFILE_TABLE_1m;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if ((sbr_sid[0] != 0) && (sbr_sid[1] != 0)) {
        stage->sbr_hw_info->num_sbr_sids = 2;
        stage->profiles_hw_info.sbr_profile_hw_info->rows = 2;
        stage->profiles_hw_info.sbr_profile_hw_info->columns = 1;
        stage->sbr_hw_info->sbr_sid[0] = sbr_sid[0];
        stage->sbr_hw_info->sbr_sid[1] = sbr_sid[1];
        stage->profiles_hw_info.sbr_profile_hw_info->sids[0][0] = sbr_sid[0];
        stage->profiles_hw_info.sbr_profile_hw_info->sids[1][0] = sbr_sid[1];
    } else {
        if (sbr_sid[0] != 0) {
            stage->sbr_hw_info->num_sbr_sids = 1;
            stage->profiles_hw_info.sbr_profile_hw_info->rows = 1;
            stage->profiles_hw_info.sbr_profile_hw_info->columns = 1;
            stage->sbr_hw_info->sbr_sid[0] = sbr_sid[0];
            stage->profiles_hw_info.sbr_profile_hw_info->sids[0][0] = sbr_sid[0];
        } else if (sbr_sid[1] != 0) {
            stage->sbr_hw_info->num_sbr_sids = 1;
            stage->profiles_hw_info.sbr_profile_hw_info->rows = 1;
            stage->profiles_hw_info.sbr_profile_hw_info->columns = 1;
            stage->sbr_hw_info->sbr_sid[0] = sbr_sid[1];
            stage->profiles_hw_info.sbr_profile_hw_info->sids[0][0] = sbr_sid[1];
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_fp_vlan_device_consts_init(int unit,
                               bcmfp_stage_t *stage)
{
    bcmfp_group_mode_t mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;
    bcmfp_pdd_hw_section_info_t *section_info = NULL;
    bcmfp_tbl_sbr_t *sbr_tbl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sbr_tbl = stage->tbls.sbr_tbl;

    BCMFP_STAGE_FLAGS_INIT(stage);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_KEY_TYPE_PRESEL);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_PRESEL_KEY_DYNAMIC);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_POLICY_TYPE_PDD);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_NO_METER_SUPPORT);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_MODE_HALF_NOT_SUPPORTED);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_EFLEX_CTR_SUPPORT);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_IDP_MAP_TYPE_HASH);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_UFT_BANKS_DYNAMIC);

    /* add sbr flags only if the SBR_PROFILE_TABLE is allocated to FP */
    if (sbr_tbl != NULL) {
        BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_ACTION_RESOLUTION_SBR);
        BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_SBR_PROFILES_GLOBAL);
        BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_SBR_PER_ENTRY);
        BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_SBR_UFTMGR_SUPPORT);
    }

    for (mode = 0; mode < BCMFP_GROUP_MODE_COUNT; mode++) {
        stage->hw_entry_info[mode] = NULL;
    }

    stage->num_group_action_res_ids = 16;
    stage->num_group_sbr_ids = 32;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_idp_maps_modulo_set(unit, stage));

    /* Single wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpIngHwEntryInfoSingleGrp");
    hw_entry_info->sid = MEMDB_TCAM_IFTA40_MEM1_0_ONLYm;
    hw_entry_info->sid_data_only = MEMDB_TCAM_IFTA40_MEM1_0_DATA_ONLYm;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = IFTA40_T4T_01_LTS_TCAM_0m;
    hw_entry_info->presel_sid_data_only = 0;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->max_entries = (256 * 4);
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;

    /* Assuming 1 Tile for VFP, 1 tile has 4 slices */
    hw_entry_info->max_non_conflicting_groups = 4;
    stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_SINGLE);
    hw_entry_info = NULL;

    /* Inter double wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpIngHwEntryInfoDblInterGrp");
    hw_entry_info->sid = MEMDB_TCAM_IFTA40_MEM1_0_ONLYm;
    hw_entry_info->sid_data_only = MEMDB_TCAM_IFTA40_MEM1_0_DATA_ONLYm;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = IFTA40_T4T_01_LTS_TCAM_0m;
    hw_entry_info->presel_sid_data_only = 0;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->max_entries = (256 * 2);
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;

    /* Assuming 1 Tile for VFP, 1 tile has 4 slices */
    hw_entry_info->max_non_conflicting_groups = 2;
    stage->hw_entry_info[BCMFP_GROUP_MODE_DBLINTER] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_DBLINTER);
    hw_entry_info = NULL;

    /* Triple wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpIngHwEntryInfoTripleGrp");
    hw_entry_info->sid = MEMDB_TCAM_IFTA40_MEM1_0_ONLYm;
    hw_entry_info->sid_data_only = MEMDB_TCAM_IFTA40_MEM1_0_DATA_ONLYm;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = IFTA40_T4T_01_LTS_TCAM_0m;
    hw_entry_info->presel_sid_data_only = 0;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->max_entries = (256 * 1);
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;

    /* Assuming 1 Tile for VFP, 1 tile has 4 slices */
    hw_entry_info->max_non_conflicting_groups = 1;
    stage->hw_entry_info[BCMFP_GROUP_MODE_TRIPLE] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_TRIPLE);
    hw_entry_info = NULL;

    /* Initialize the stage miscellaneous information */
    BCMFP_ALLOC(stage->misc_info,
                sizeof(bcmfp_stage_misc_info_t),
                "bcmfpIngStageMiscInfo");
    stage->misc_info->num_group_action_res_ids = 16;
    stage->misc_info->num_groups = 64;
    stage->misc_info->num_keygen_prof = 0;
    stage->misc_info->num_presel_groups = 1;
    stage->misc_info->num_pdd_profiles = 128;
    stage->misc_info->num_sbr_profiles = 32;
    stage->misc_info->per_entry_pdd_raw_data_offset = 10;
    stage->misc_info->per_entry_sbr_raw_data_offset = 10;
    stage->misc_info->per_entry_pdd_sbr_raw_data_offset = 10;
    stage->misc_info->per_group_pdd_sbr_raw_data_offset = 0;
    stage->misc_info->sbr_index_size = 5;

    /* PDD HW Info */
    BCMFP_ALLOC(stage->pdd_hw_info,
                sizeof(bcmfp_pdd_hw_info_t),
                "bcmfpIngStagePddHwInfo");
    stage->pdd_hw_info->num_max_pdd_parts = 1;
    stage->pdd_hw_info->pdd_bitmap_size = 141;
    stage->pdd_hw_info->raw_policy_width = 160;
    stage->pdd_hw_info->read_pdd_bitmap_from_lsb = FALSE;
    stage->pdd_hw_info->write_pdd_data_from_lsb = FALSE;
    stage->pdd_hw_info->pdd_data_mixed_msb_lsb = TRUE;
    stage->pdd_hw_info->section_align_bits = 8;

    /* PDD Section Info */
    BCMFP_ALLOC(section_info,
                sizeof(bcmfp_pdd_hw_section_info_t),
                "bcmfpIngStagePddSectionInfo");
    section_info->num_pdd_sections = 2;
    section_info->section_id[0] = 0;
    section_info->section_start_bit[0] = 0;
    section_info->section_end_bit[0] = 69;
    section_info->section_is_lsb[0] = TRUE;
    section_info->section_align_bits[0] = 4;
    section_info->section_id[1] = 1;
    section_info->section_start_bit[1] = 70;
    section_info->section_end_bit[1] = 140;
    section_info->section_is_lsb[1] = FALSE;
    section_info->section_align_bits[1] = 8;
    stage->pdd_hw_info->section_hw_info = section_info;

    /* SBR HW information */
    BCMFP_ALLOC(stage->sbr_hw_info,
                sizeof(bcmfp_sbr_hw_info_t),
                "bcmfpIngStageSbrHwInfo");
    stage->sbr_hw_info->sbr_profile_sid_columns = 1;

    /* SBR Priority field info */
    BCMFP_ALLOC(stage->sbr_hw_info->priority_field_info,
                sizeof(bcmfp_stage_hw_field_info_t),
                "bcmfpIngStageSbrPriorityFieldHwInfo");

    stage->sbr_hw_info->priority_field_info[0].num_offsets = 1;

    BCMFP_ALLOC(stage->sbr_hw_info->priority_field_info[0].offset,
                sizeof(uint16_t),
                "bcmfpIngStageSbrPriorityFieldFirstColumnOffset");
    BCMFP_ALLOC(stage->sbr_hw_info->priority_field_info[0].width,
                sizeof(uint16_t),
                "bcmfpIngStageSbrPriorityFieldFirstColumnWidth");
    stage->sbr_hw_info->priority_field_info[0].offset[0] = 0;
    stage->sbr_hw_info->priority_field_info[0].width[0] = 360;

    stage->sbr_hw_info->num_sbr_sids = 2;
    BCMFP_ALLOC(stage->sbr_hw_info->sbr_sid,
                (sizeof(bcmdrd_sid_t) * 2),
                "bcmfpVlanStageSbrSids");
    stage->sbr_hw_info->sbr_sid[0] = IFTA40_SBR_PROFILE_TABLE_0m;
    stage->sbr_hw_info->sbr_sid[1] = IFTA40_SBR_PROFILE_TABLE_1m;

    /* SBR profile hardware information. */
    BCMFP_ALLOC(stage->profiles_hw_info.sbr_profile_hw_info,
                sizeof(bcmfp_profile_hw_info_t),
                "bcmfpVlanStageSbrProfileHwInfo");
    stage->profiles_hw_info.sbr_profile_hw_info->rows = 2;
    stage->profiles_hw_info.sbr_profile_hw_info->columns = 1;
    stage->profiles_hw_info.sbr_profile_hw_info->sids[0][0] =
                           IFTA40_SBR_PROFILE_TABLE_0m;
    stage->profiles_hw_info.sbr_profile_hw_info->sids[1][0] =
                           IFTA40_SBR_PROFILE_TABLE_1m;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_fp_vlan_bank_info_set(unit, stage));
exit:
    SHR_FUNC_EXIT();
}
