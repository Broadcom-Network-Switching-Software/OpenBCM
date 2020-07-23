/*! \file bcm56990_a0_fp_vlan_stage.c
 *
 * API to initialize VLAN FP stage attributes for Tomahawk-4(56990_A0) device.
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
#include <bcmfp/chip/bcm56990_a0_fp.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_a0_vlan_device_consts_init(int unit,
                                          bcmfp_stage_t *stage)
{
    bcmfp_group_mode_t mode;
    bcmfp_stage_hw_entry_info_t *hw_entry_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    BCMFP_STAGE_FLAGS_INIT(stage);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_KEY_TYPE_SELCODE);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_NO_METER_SUPPORT);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_EFLEX_CTR_SUPPORT);
    BCMFP_STAGE_FLAGS_ADD(stage, BCMFP_STAGE_IDP_MAP_TYPE_HASH);

    for (mode = 0; mode < BCMFP_GROUP_MODE_COUNT; mode++) {
        stage->hw_entry_info[mode] = NULL;
    }

    stage->num_group_action_res_ids = 4;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_idp_maps_modulo_set(unit, stage));

    /* Single wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpVlanHwEntryInfoSingleGrp");
    hw_entry_info->sid = VFP_TCAMm;
    hw_entry_info->sid_data_only = VFP_POLICY_TABLEm;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->max_entries = 512;
    hw_entry_info->max_non_conflicting_groups = 4;
    stage->hw_entry_info[BCMFP_GROUP_MODE_SINGLE] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_SINGLE);
    hw_entry_info = NULL;

    /* Intra Slice Double wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpVlanHwEntryInfoDblIntraGrp");
    hw_entry_info->sid = VFP_TCAMm;
    hw_entry_info->sid_data_only = VFP_POLICY_TABLEm;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->max_entries = (128 * 4);
    hw_entry_info->max_non_conflicting_groups = 4;
    stage->hw_entry_info[BCMFP_GROUP_MODE_DBLINTRA] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_DBLINTRA);
    hw_entry_info = NULL;

    /* Inter Slice Double wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpVlanHwEntryInfoDblInterGrp");
    hw_entry_info->sid = VFP_TCAMm;
    hw_entry_info->sid_data_only = VFP_POLICY_TABLEm;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->max_entries = (256 * 2);
    hw_entry_info->max_non_conflicting_groups = 2;
    stage->hw_entry_info[BCMFP_GROUP_MODE_DBLINTER] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_DBLINTER);
    hw_entry_info = NULL;

    /* Quad wide Mode. */
    BCMFP_ALLOC(hw_entry_info,
                sizeof(bcmfp_stage_hw_entry_info_t),
                "bcmfpVlanHwEntryInfoQuadGrp");
    hw_entry_info->sid = VFP_TCAMm;
    hw_entry_info->sid_data_only = VFP_POLICY_TABLEm;
    hw_entry_info->key_fid = KEYf;
    hw_entry_info->mask_fid = MASKf;
    hw_entry_info->valid_fid = VALIDf;
    hw_entry_info->aggr_view = 0;
    hw_entry_info->max_entries = 128;
    hw_entry_info->max_non_conflicting_groups = 1;
    stage->hw_entry_info[BCMFP_GROUP_MODE_QUAD] = hw_entry_info;
    SHR_BITSET(stage->group_mode_bmp.w, BCMFP_GROUP_MODE_QUAD);
    hw_entry_info = NULL;

    /* Initialize the stage miscellaneous information */
    BCMFP_ALLOC(stage->misc_info,
                sizeof(bcmfp_stage_misc_info_t),
                "bcmfpVlanStageMiscInfo");
    stage->misc_info->num_group_action_res_ids = 0;
    stage->misc_info->num_groups = 32;
    stage->misc_info->num_keygen_prof = 0;
    stage->misc_info->num_qos_prof = 0;

exit:
    SHR_FUNC_EXIT();
}
