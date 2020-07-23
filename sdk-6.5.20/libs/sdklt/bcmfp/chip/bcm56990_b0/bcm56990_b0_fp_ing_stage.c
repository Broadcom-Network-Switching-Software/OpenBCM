/*! \file bcm56990_b0_fp_ing_stage.c
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
bcmfp_bcm56990_b0_ingress_device_consts_init(int unit,
                                             bcmfp_stage_t *stage,
                                             bool warm)
{
    bcmfp_group_mode_t mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;
    bcmfp_stage_hw_field_info_t *colored_actions_enable = NULL;
    uint8_t hw_field_idx = 0;
    uint16_t hw_field_offset[] = {179};
    uint16_t hw_field_width[] = {1};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    BCMFP_STAGE_FLAGS_INIT(stage);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_KEY_TYPE_PRESEL);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_INPORTS_SUPPORT);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_KEYGEN_PROFILE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_EFLEX_CTR_SUPPORT);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_SUPPORTS_COLORED_ACTIONS);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_MODE_HALF_NOT_SUPPORTED);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_IDP_MAP_TYPE_HASH);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_IPBM_SUPPORT);

    for (mode = 0; mode < BCMFP_GROUP_MODE_COUNT; mode++) {
        stage->hw_entry_info[mode] = NULL;
    }

    stage->num_group_action_res_ids = 16;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_idp_maps_modulo_set(unit, stage));

    /* Single wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpIngHwEntryInfSingleGrp");
    hw_entry_info->sid = IFP_TCAMm;
    hw_entry_info->sid_data_only = IFP_POLICY_TABLEm;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = IFP_LOGICAL_TABLE_SELECTm;
    hw_entry_info->presel_sid_data_only = IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->max_entries = ((512 * 3) + (256 * 6));
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->max_non_conflicting_groups = 9;
    stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_SINGLE);
    hw_entry_info = NULL;

    /* Intra Slice Double Wide mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpIngHwEntryInfSingleGrp");
    hw_entry_info->sid = IFP_TCAMm;
    hw_entry_info->sid_data_only = IFP_POLICY_TABLEm;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = IFP_LOGICAL_TABLE_SELECTm;
    hw_entry_info->presel_sid_data_only = IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->max_entries = ((512 / 2 * 3) );
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->max_non_conflicting_groups = 9;
    stage->hw_entry_info[BCMFP_GROUP_MODE_DBLINTRA] = hw_entry_info;
    hw_entry_info = NULL;

    /* Inter Slice Double wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpIngHwEntryInfDblInterGrp");
    hw_entry_info->sid = IFP_TCAMm;
    hw_entry_info->sid_data_only = IFP_POLICY_TABLEm;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = IFP_LOGICAL_TABLE_SELECTm;
    hw_entry_info->presel_sid_data_only = IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->max_entries = ((512 * 1) + (256 * 2));
    hw_entry_info->max_non_conflicting_groups = 3;
    stage->hw_entry_info[BCMFP_GROUP_MODE_DBLINTER] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_DBLINTER);
    hw_entry_info = NULL;

    /* Triple wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpIngHwEntryInfTripleGrp");
    hw_entry_info->sid = IFP_TCAMm;
    hw_entry_info->sid_data_only = IFP_POLICY_TABLEm;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->presel_sid = IFP_LOGICAL_TABLE_SELECTm;
    hw_entry_info->presel_sid_data_only = IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->presel_aggr_view = 1;
    hw_entry_info->max_entries = ((512 * 1) + (256 * 2));
    hw_entry_info->max_non_conflicting_groups = 3;
    stage->hw_entry_info[BCMFP_GROUP_MODE_TRIPLE] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_TRIPLE);
    hw_entry_info = NULL;

    /* Initialize the stage miscellaneous information */
    BCMFP_ALLOC(stage->misc_info,
                sizeof(bcmfp_stage_misc_info_t),
                "bcmfpIngStageMiscInfo");
    stage->misc_info->num_group_action_res_ids = 16;
    stage->misc_info->num_groups = 128;
    stage->misc_info->num_keygen_prof = 32;
    stage->misc_info->num_qos_prof = 0;
    stage->misc_info->shared_action_res_id_stage_id_bmp =
                      (1 << BCMFP_STAGE_ID_EXACT_MATCH);

    BCMFP_ALLOC(colored_actions_enable,
                sizeof(bcmfp_stage_hw_field_info_t),
                "bcmfpIngColoredAction");

    colored_actions_enable->num_offsets = 1;

    BCMFP_ALLOC(colored_actions_enable->offset,
            sizeof(uint16_t) * colored_actions_enable->num_offsets,
            "bcmfpIngColoredActionOffset");
    BCMFP_ALLOC(colored_actions_enable->width,
            sizeof(uint16_t) * colored_actions_enable->num_offsets,
            "bcmfpIngColoredActionOffesetWidth");

    for (hw_field_idx = 0;
            hw_field_idx < colored_actions_enable->num_offsets;
            hw_field_idx++) {
        colored_actions_enable->offset[hw_field_idx] =
            hw_field_offset[hw_field_idx];
        colored_actions_enable->width[hw_field_idx] =
            hw_field_width[hw_field_idx];
    }
    stage->misc_info->colored_actions_enable = colored_actions_enable;

    if (!warm)  {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_bcm56990_b0_ifp_spare_debug_set(unit));
    }

exit:
    /* coverity[leaked_storage] */
    SHR_FUNC_EXIT();
}
