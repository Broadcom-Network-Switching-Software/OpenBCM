/*! \file field.c
 *
 * BCM56780_A0 Field Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <bcm/types.h>
#include <bcm/field.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw/chip/bcm56780_a0/variant_dispatch.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/xfs/field.h>
#include <bcm_int/ltsw/xfs/field_destination.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB1Chunk0_map[] = {
    {
        .lt_field_name = "CONTAINER_1_BYTE",
        .num_offsets = 1,
        .offset = {0},
        .width = {8},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB1Chunk1_map[] = {
    {
        .lt_field_name = "CONTAINER_1_BYTE",
        .num_offsets = 1,
        .offset = {1},
        .width = {8},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB1Chunk2_map[] = {
    {
        .lt_field_name = "CONTAINER_1_BYTE",
        .num_offsets = 1,
        .offset = {2},
        .width = {8},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB1Chunk3_map[] = {
    {
        .lt_field_name = "CONTAINER_1_BYTE",
        .num_offsets = 1,
        .offset = {3},
        .width = {8},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB1Chunk4_map[] = {
    {
        .lt_field_name = "CONTAINER_1_BYTE",
        .num_offsets = 1,
        .offset = {4},
        .width = {8},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB1Chunk5_map[] = {
    {
        .lt_field_name = "CONTAINER_1_BYTE",
        .num_offsets = 1,
        .offset = {5},
        .width = {8},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB1Chunk6_map[] = {
    {
        .lt_field_name = "CONTAINER_1_BYTE",
        .num_offsets = 1,
        .offset = {6},
        .width = {8},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB1Chunk7_map[] = {
    {
        .lt_field_name = "CONTAINER_1_BYTE",
        .num_offsets = 1,
        .offset = {7},
        .width = {8},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk0_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {0},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk1_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {1},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk2_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {2},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk3_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {3},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk4_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {4},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk5_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {5},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk6_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {6},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk7_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {7},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk8_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {8},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk9_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {9},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk10_map[] = {
    {
        .lt_field_name = "CONTAINER_2_BYTE",
        .num_offsets = 1,
        .offset = {10},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};

bcm_field_qual_map_t bcm56780_a0_udf_qual_data[] = {
    {
        .qual = bcmiFieldQualifyB2Chunk0,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk0_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk1,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk1_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk2,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk2_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk3,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk3_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk4,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk4_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk5,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk5_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk6,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk6_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk7,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk7_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk8,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk8_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk9,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk9_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk10,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk10_map,
    },
    {
        .qual = bcmiFieldQualifyB1Chunk0,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB1Chunk0_map,
    },
    {
        .qual = bcmiFieldQualifyB1Chunk1,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB1Chunk1_map,
    },
    {
        .qual = bcmiFieldQualifyB1Chunk2,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB1Chunk2_map,
    },
    {
        .qual = bcmiFieldQualifyB1Chunk3,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB1Chunk3_map,
    },
    {
        .qual = bcmiFieldQualifyB1Chunk4,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB1Chunk4_map,
    },
    {
        .qual = bcmiFieldQualifyB1Chunk5,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB1Chunk5_map,
    },
    {
        .qual = bcmiFieldQualifyB1Chunk6,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB1Chunk6_map,
    },
    {
        .qual = bcmiFieldQualifyB1Chunk7,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB1Chunk7_map,
    },
};

static const bcm_field_control_map_info_t bcm_field_control_DlbMonitorIngressRandomSeed_map[] = {
    {
        .lt_name       = "DLB_ECMP_CONTROL",
        .lt_field_name = "FP_ING_SEED",
    },
};
static const bcm_field_control_map_info_t bcm_field_control_TrunkDlbMonitorIngressRandomSeed_map[] = {
    {
        .lt_name       = "DLB_TRUNK_CONTROL",
        .lt_field_name = "FP_ING_SEED",
    },
};
static const bcm_field_control_map_info_t bcm_field_control_RedirectIngressVlanCheck_map[] = {
    {
        .func_set      = bcmi_ltsw_port_ifp_egr_vlan_check_enable_set,
        .func_get      = bcmi_ltsw_port_ifp_egr_vlan_check_enable_get,
    },
};

const bcm_field_control_map_t bcm56780_a0_field_control_data[] = {
    {
        .control = bcmFieldControlRedirectIngressVlanCheck,
        .num_maps = 1,
        .map = bcm_field_control_RedirectIngressVlanCheck_map,
    },
    {
        .control = bcmFieldControlDlbMonitorIngressRandomSeed,
        .num_maps = 1,
        .map = bcm_field_control_DlbMonitorIngressRandomSeed_map,
    },
    {
        .control = bcmFieldControlTrunkDlbMonitorIngressRandomSeed,
        .num_maps = 1,
        .map = bcm_field_control_TrunkDlbMonitorIngressRandomSeed_map,
    },
};
/******************************************************************************
 * Private functions
 */
static int
bcm56780_a0_ltsw_field_control_info_init(
        int unit,
        bcmint_field_control_info_t *control_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(control_info, SHR_E_PARAM);

    control_info->control_lt_map_db = bcm56780_a0_field_control_data;

    /* Initialise qual db counts */
    control_info->control_db_count = (sizeof(bcm56780_a0_field_control_data)) /
                                                (sizeof(bcm_field_control_map_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_control_lt_info_init(unit, control_info));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_ltsw_field_stage_info_init(
        int unit,
        bcmint_field_stage_info_t *stage_info)
{
    SHR_FUNC_ENTER(unit);

    if (stage_info->stage_id == bcmiFieldStageIngress) {
        /* Initialise stage flags */
        stage_info->flags |= BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICY_TYPE_PDD;
        stage_info->flags |= BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR;
        stage_info->flags |= BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC;
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_SUPPORTS_COLORED_ACTIONS;
        stage_info->flags |= BCMINT_FIELD_STAGE_COMPRESSION_SUPPORTED;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICER_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_PROFILE_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICER_POOL_HINT_SUPPORT;

        /* Initialize variant specific qual/action/presel DB's */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_stage_db_init(unit, stage_info));

        /* Initialise qual db counts */
        stage_info->udf_qual_db_count = (sizeof(bcm56780_a0_udf_qual_data)) /
            (sizeof(bcm_field_qual_map_t));

        /* Initialise lt map db */
        stage_info->udf_qual_lt_map_db = bcm56780_a0_udf_qual_data;
    } else if (stage_info->stage_id == bcmiFieldStageExactMatch) {
        /* Initialise stage flags */
        stage_info->flags |= BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICY_TYPE_PDD;
        stage_info->flags |= BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR;
        stage_info->flags |= BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC;
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_DEFAULT_ENTRY_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_PIPE_MODE_NOT_SUPPORTED;
        stage_info->flags |= BCMINT_FIELD_STAGE_SINGLE_SLICE_ONLY;

        /* Initialize variant specific qual/action/presel DB's */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_stage_db_init(unit, stage_info));

    } else if (stage_info->stage_id == bcmiFieldStageFlowTracker) {
        /* Initialise stage flags */
        stage_info->flags |= BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICY_TYPE_PDD;
        stage_info->flags |= BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR;
        stage_info->flags |= BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC;
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_DEFAULT_ENTRY_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_PIPE_MODE_NOT_SUPPORTED;
        stage_info->flags |= BCMINT_FIELD_STAGE_SINGLE_SLICE_ONLY;

        /* Initialize variant specific qual/action/presel DB's */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_stage_db_init(unit, stage_info));

    } else if (stage_info->stage_id == bcmiFieldStageEgress) {

        /* Initialise stage flags */
        stage_info->flags |= BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICY_TYPE_PDD;
        stage_info->flags |= BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR;
        stage_info->flags |= BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC;
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_SUPPORTS_COLORED_ACTIONS;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICER_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT;

        /* Initialize variant specific qual/action/presel DB's */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_stage_db_init(unit, stage_info));

    } else if (stage_info->stage_id == bcmiFieldStageVlan) {
        /* Initialise stage flags */
        stage_info->flags |= BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICY_TYPE_PDD;
        stage_info->flags |= BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR;
        stage_info->flags |= BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC;
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_SUPPORTS_COLORED_ACTIONS;

        /* Initialize variant specific qual/action/presel DB's */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_stage_db_init(unit, stage_info));

    } else {
        /* Do nothing */
    }

    if (stage_info->stage_id == bcmiFieldStageIngress) {

        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_field_ing_match_id_enums_init(unit, stage_info));

    } else if (stage_info->stage_id == bcmiFieldStageEgress) {

        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_field_egr_match_id_enums_init(unit, stage_info));

    } else if (stage_info->stage_id == bcmiFieldStageVlan) {

        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_field_vlan_match_id_enums_init(unit, stage_info));

    } else {
        /* do nothing */
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_field_stage_group_sbr_prof_init(unit, stage_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief mbcm driver mapping for bcm56780_a0
 */
static mbcm_ltsw_field_drv_t bcm56780_a0_ltsw_field_drv = {
    .field_control_info_init = bcm56780_a0_ltsw_field_control_info_init,
    .field_stage_info_init = bcm56780_a0_ltsw_field_stage_info_init,
    .field_ingress_profile_init    = xfs_ltsw_field_ingress_profile_init,
    .field_ingress_profile_deinit  = xfs_ltsw_field_ingress_profile_deinit,
    .field_lt_qual_name_update = xfs_ltsw_field_lt_qual_name_update,
    .field_action_value_set = xfs_ltsw_field_action_value_set,
    .field_action_value_get = xfs_ltsw_field_action_value_get,
    .field_action_profile_index_get = xfs_ltsw_field_action_profile_index_get,
    .field_action_profile_add = xfs_ltsw_field_action_profile_add,
    .field_action_profile_get = xfs_ltsw_field_action_profile_get,
    .field_action_profile_del = xfs_ltsw_field_action_profile_del,
    .field_action_meter_cont_get = xfs_ltsw_field_action_meter_cont_get,
    .field_compression_set = xfs_ltsw_field_compression_set,
    .field_compression_del = xfs_ltsw_field_compression_del,
    .field_qualify_macro_values_get = xfs_ltsw_field_qualify_macro_values_get,
    .field_ifp_clear = xfs_ltsw_ifp_clear,
    .field_vfp_clear = xfs_ltsw_vfp_clear,
    .field_efp_clear = xfs_ltsw_efp_clear,
    .field_em_clear = xfs_ltsw_em_clear,
    .field_ft_clear = xfs_ltsw_ft_clear,
    .field_clear = xfs_ltsw_field_clear,
    .field_port_filter_enable_set = xfs_ltsw_field_port_filter_enable_set,
    .field_udf_qset_set = xfs_ltsw_field_udf_qset_set,
    .field_udf_qset_del = xfs_ltsw_field_udf_qset_del,
    .field_udf_qual_set = xfs_ltsw_field_udf_qual_set,
    .field_udf_qual_get = xfs_ltsw_field_udf_qual_get,
    .field_udf_obj_get = xfs_ltsw_field_udf_obj_get,
    .field_group_mode_set = xfs_ltsw_field_group_mode_set,
    .field_group_pipe_get = xfs_ltsw_field_group_pipe_get,
    .field_group_pbmp_get = xfs_ltsw_field_group_pbmp_get,
    .field_pbmp_index_get = xfs_ltsw_field_pbmp_index_get,
    .field_pbmp_get = xfs_ltsw_field_pbmp_get,
    .field_internal_entry_enable_get = xfs_ltsw_field_internal_entry_enable_get,
    .field_destination_init = xfs_ltsw_field_destination_init,
    .field_destination_deinit = xfs_ltsw_field_destination_deinit,
    .field_destination_mirror_index_get = xfs_ltsw_field_destination_mirror_index_get,
    .field_destination_entry_add = xfs_ltsw_field_destination_entry_add,
    .field_destination_entry_delete = xfs_ltsw_field_destination_entry_delete,
    .field_destination_entry_traverse = xfs_ltsw_field_destination_entry_traverse,
    .field_destination_entry_get = xfs_ltsw_field_destination_entry_get,
    .field_destination_flexctr_attach = xfs_ltsw_field_destination_flexctr_attach,
    .field_destination_flexctr_detach = xfs_ltsw_field_destination_flexctr_detach,
    .field_destination_flexctr_info_get = xfs_ltsw_field_destination_flexctr_info_get,
    .field_destination_flexctr_object_set = xfs_ltsw_field_destination_flexctr_object_set,
    .field_destination_flexctr_object_get = xfs_ltsw_field_destination_flexctr_object_get,
    .field_qual_gport_validate = xfs_ltsw_field_qual_gport_validate,
    .field_qualify_copytocpureasonhigh_set = xfs_ltsw_field_qualify_copytocpureasonhigh_set,
    .field_qualify_copytocpureasonhigh_get = xfs_ltsw_field_qualify_copytocpureasonhigh_get,
    .field_qual_info_lt_name_get = xfs_ltsw_field_qual_info_lt_name_get,
    .field_policer_attach = xfs_ltsw_field_policer_attach,
    .field_policer_detach = xfs_ltsw_field_policer_detach,
    .field_qual_mask_lt_field_get = xfs_ltsw_field_qual_mask_lt_field_get,
    .field_qual_value_set = xfs_ltsw_field_qual_value_set,
    .field_qual_value_get = xfs_ltsw_field_qual_value_get,
    .field_qual_info_set = xfs_ltsw_field_qual_info_set,
    .field_entry_table_id_get = xfs_ltsw_field_entry_table_id_get,
    .field_aacl_class_id_get = xfs_ltsw_field_aacl_class_id_get,
    .field_action_stat_group_lt_field_validate =
        xfs_ltsw_field_action_stat_group_lt_field_validate,
    .field_group_udf_qset_update_with_hints =
        xfs_ltsw_field_group_udf_qset_update_with_hints,
    .field_action_lt_map_valid_info_get = xfs_ltsw_field_action_lt_map_valid_info_get,
    .field_action_mirror_set = xfs_ltsw_field_action_mirror_set,
    .field_action_mirror_remove = xfs_ltsw_field_action_mirror_remove,
    .field_action_mirror_index_get = xfs_ltsw_field_action_mirror_index_get,
    .field_group_mode_get = xfs_ltsw_field_group_mode_get,
    .field_qualify_mhopcode_set = xfs_ltsw_field_qualify_mhopcode_set,
    .field_qualify_mhopcode_get = xfs_ltsw_field_qualify_mhopcode_get,
};

/******************************************************************************
 * Public functions
 */
int
bcm56780_a0_ltsw_field_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_drv_set(unit, &bcm56780_a0_ltsw_field_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ltsw_variant_drv_attach(unit,
                                             BCM56780_A0_LTSW_VARIANT_FIELD));

exit:
    SHR_FUNC_EXIT();
}
