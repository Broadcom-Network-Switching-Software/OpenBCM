/*! \file bcm56780_a0_dna_2_4_13_field.c
 *
 * BCM56780_A0 dna_2_4_13 FIELD driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/chip/bcm56780_a0/dna_2_4_13/bcm56780_a0_dna_2_4_13_field_lt_map.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FP
static
bcmint_field_compress_tbls_info_t bcm56780_a0_compress_tbls_info = {
    .index_mapped_ethertype_sid = "FP_COMPRESSION_MATCH_REMAP_0",
    .index_key_ethertype_fid = "FP_COMPRESSION_MATCH_REMAP_0_ID",
    .index_val_ethertype_fid = "ORIGINAL_VALUE",
    .index_map_val_ethertype_fid = "REMAPPED_VALUE",
    .index_mapped_ipprotocol_sid = "FP_COMPRESSION_MATCH_REMAP_1",
    .index_key_ipprotocol_fid = "FP_COMPRESSION_MATCH_REMAP_1_ID",
    .index_val_ipprotocol_fid = "ORIGINAL_VALUE",
    .index_map_val_ipprotocol_fid = "REMAPPED_VALUE",
    .direct_mapped_ttl_sid = "FP_COMPRESSION_INDEX_REMAP_0",
    .direct_val_ttl_fid = "FP_COMPRESSION_INDEX_REMAP_0_ID",
    .direct_map_val_ttl_fid = "REMAPPED_VALUE",
    .direct_mapped_tcp_sid = "FP_COMPRESSION_INDEX_REMAP_1",
    .direct_val_tcp_fid = "FP_COMPRESSION_INDEX_REMAP_1_ID",
    .direct_map_val_tcp_fid = "REMAPPED_VALUE",
    .direct_mapped_tos_sid = "FP_COMPRESSION_INDEX_REMAP_2",
    .direct_val_tos_fid = "FP_COMPRESSION_INDEX_REMAP_2_ID",
    .direct_map_val_tos_fid = "REMAPPED_VALUE",
    .pipe_fid = "PIPE",
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid = "FP_COMPRESSION_OPERMODE_PIPEUNIQUE",
};

static
bcmint_field_tbls_info_t bcm56780_a0_ifp_tbls_info = {
    .group_sid = "DT_IFP_GRP_TEMPLATE",
    .group_key_fid = "DT_IFP_GRP_TEMPLATE_INDEX",
    .rule_sid = "DT_IFP_RULE_TEMPLATE",
    .rule_key_fid = "IFP_RULE_TEMPLATE_INDEX",
    .entry_rule_key_fid = "DT_IFP_RULE_TEMPLATE_INDEX",
    .policy_sid = "DT_IFP_ACTION_TEMPLATE",
    .policy_key_fid = "IFP_ACTION_TEMPLATE_INDEX",
    .entry_policy_key_fid = "DT_IFP_ACTION_TEMPLATE_INDEX",
    .entry_sid = "DT_IFP_ENTRY",
    .entry_key_fid = "DT_IFP_ENTRY_INDEX",
    .pdd_sid = "DT_IFP_PDD_TEMPLATE",
    .pdd_key_fid = "DT_IFP_PDD_TEMPLATE_INDEX",
    .sbr_sid = "DT_IFP_SBR_TEMPLATE",
    .sbr_key_fid = "DT_IFP_SBR_TEMPLATE_INDEX",
    .presel_entry_sid = "DT_IFP_PRESEL_TEMPLATE",
    .presel_entry_key_fid = "IFP_PRESEL_TEMPLATE_INDEX",
    .grp_presel_entry_key_fid = "DT_IFP_PRESEL_TEMPLATE_INDEX",
    .presel_entry_count_fid = "NUM_DT_IFP_PRESEL_TEMPLATE_INDEX",
    .presel_group_sid = "DT_IFP_PRESEL_GRP_TEMPLATE",
    .presel_group_key_fid = "DT_IFP_PRESEL_GRP_TEMPLATE_INDEX",
    .ctr_sid = NULL,
    .ctr_key_fid = "FLEX_CTR_ACTION",
    .meter_sid = "METER_ING_FP_TEMPLATE",
    .meter_key_fid = "METER_ING_FP_TEMPLATE_ID",
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid = "FP_ING_OPERMODE",
    .flex_ctr_idx0_fid = "ING_OBJ_BUS_EM_FT_OPAQUE_OBJ_OR_IFP_OPAQUE_OBJ",
    .flex_ctr_idx1_fid = "ING_OBJ_BUS_IFP_OPAQUE_OBJ1",
    .flex_ctr_idx2_fid = "ING_OBJ_BUS_IFP_OPAQUE_OBJ2",
    .flex_ctr_idx3_fid = "ING_OBJ_BUS_INGRESS_QOS_REMAP_VALUE_OR_IFP_OPAQUE_OBJ",
    .flex_ctr_idx4_fid = "ING_OBJ_BUS_EM_FT_HIT_INDEX_LO_OR_IFP_OPAQUE_OBJ",
    .flex_ctr_idx5_fid = "ING_OBJ_BUS_EM_FT_HIT_INDEX_HI_OR_IFP_OPAQUE_OBJ",
    .flex_ctr_idx6_fid = "ING_OBJ_BUS_L3_DST_CLASS_ID_OR_IFP_OPAQUE_OBJ",
    .flex_ctr_idx7_fid = "ING_OBJ_BUS_VFP_VLAN_XLATE_OR_IFP_OPAQUE_OBJ",
    .flex_ctr_idx8_fid = "ING_OBJ_BUS_IFP_LAG_DGM_ACTION_SET_OR_IFP_OPAQUE_OBJ",
    .flex_ctr_idx9_fid = "ING_OBJ_BUS_IFP_SAMPLER_INDEX_OR_IFP_OPAQUE_OBJ",
    .compression_type_fid = "COMPRESSION_TYPE",
    .compression_type_cnt_fid = "NUM_COMPRESSION_TYPE",
    .grp_oper_mode_fid = "MODE_OPER",
    
    .entry_enable_fid = "DT_IFP_GRP_TEMPLATE_INDEX",
    .def_sbr_key_fid = NULL,
    .def_pdd_key_fid = NULL,
    .def_policy_key_fid = NULL,
    .entry_enable_fid = "ENABLE",
    .entry_priority_fid = "ENTRY_PRIORITY",
    .group_info_sid = "FP_ING_GRP_TEMPLATE_INFO",
    .group_info_key_fid = "FP_ING_GRP_TEMPLATE_ID",
    .group_info_num_parts_fid = "NUM_PARTITION_ID",
    .group_info_entries_created_fid = "NUM_ENTRIES_CREATED",
    .group_info_entries_tentative_fid = "NUM_ENTRIES_TENTATIVE",
    .group_info_partition_sid = "FP_ING_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_partition_key_fid = "FP_ING_GRP_TEMPLATE_PARTITION_INFO_ID",
    .group_info_action_sid = "FP_ING_PDD_TEMPLATE_PARTITION_INFO",
    .group_info_action_key_fid = "FP_ING_PDD_TEMPLATE_PARTITION_INFO_ID",
    .group_info_presel_sid = "FP_ING_PRESEL_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_presel_key_fid = "FP_ING_PRESEL_GRP_TEMPLATE_PARTITION_INFO_ID",
    .group_info_common_partition_key_fid = "PARTITION_ID",
    .group_info_num_slice_fid = "NUM_SLICE_ID",
    .group_info_slice_fid = "SLICE_ID",
    .compress_sid = "FP_CONFIG",
    .compress_src_ip4_only_sid = "FP_ING_COMP_SRC_IP4_ONLY",
    .compress_src_ip6_only_sid = "FP_ING_COMP_SRC_IP6_ONLY",
    .compress_dst_ip4_only_sid = "FP_ING_COMP_DST_IP4_ONLY",
    .compress_dst_ip6_only_sid = "FP_ING_COMP_DST_IP6_ONLY",
    .slice_info_sid = "FP_ING_SLICE_INFO",
    .slice_info_key_fid = "FP_ING_SLICE_ID",
    .slice_info_pipe_fid = "PIPE",
    .slice_info_entry_in_use_fid = "ENTRY_INUSE_CNT",
    .slice_info_entry_max_fid = "ENTRY_MAXIMUM",
    .action_cont_info_sid = "DT_IFP_ACTION_TEMPLATE_CONTAINER_INFO",
    .action_cont_info_key_fid = "FIELD_ACTION",
    .stage_info_sid = "FP_ING_INFO",
    .stage_info_num_slices_fid = "NUM_SLICES",
    .group_info_act_res_fid = "HW_LTID",
    .presel_entry_prio_fid = "ENTRY_PRIORITY",
    .meter_info_sid = "METER_ING_FP_DEVICE_INFO",
    .meter_info_num_meters_fid = "NUM_METERS",
    .meter_info_num_meters_per_pipe_fid = "NUM_METERS_PER_PIPE",
    .egress_mask_profile_sid = "PORT_ING_EGR_BLOCK_3",
    .egress_mask_profile_key_fid = "PORT_ING_EGR_BLOCK_ID",
    .egress_mask_profile_ports_fid = "MASK_EGR_PORTS",
    .egress_mask_profile_section_fid = "PROFILE_SECTION",
    .egress_mask_profile_target_fid = "MASK_TARGET",
    .egress_mask_profile_action_fid = "MASK_ACTION",
};

static
bcmint_field_color_action_tbls_info_t bcm56780_a0_ifp_color_action_tbls_info = {
    .num_pools = 4,
    .pipe_cfg_sid = "DT_IFP_METER_ACTION_ACTION_TEMPLATE_PIPE_CONFIG",
    .pipe_cfg_fid = "PIPEUNIQUE",
    .pipe_fid = "PIPE",
    .pdd_tbl_sid = "DT_IFP_METER_ACTION_PDD_TEMPLATE",
    .pdd_tbl_key_fid = "DT_IFP_METER_ACTION_PDD_TEMPLATE_ID",
    .sbr_sid = "DT_IFP_METER_ACTION_SBR_TEMPLATE",
    .sbr_key_fid = "DT_IFP_METER_ACTION_SBR_TEMPLATE_ID",
    .tbl_sid = {
        "DT_IFP_METER_ACTION_ACTION_TEMPLATE_0",
        "DT_IFP_METER_ACTION_ACTION_TEMPLATE_1",
        "DT_IFP_METER_ACTION_ACTION_TEMPLATE_2",
        "DT_IFP_METER_ACTION_ACTION_TEMPLATE_3",
    },
    .tbl_key_fid = {
        "DT_IFP_METER_ACTION_TEMPLATE_ID",
        "DT_IFP_METER_ACTION_TEMPLATE_ID",
        "DT_IFP_METER_ACTION_TEMPLATE_ID",
        "DT_IFP_METER_ACTION_TEMPLATE_ID",
    },
};

static
bcmint_field_tbls_info_t bcm56780_a0_em_tbls_info = {
    .group_sid = "DT_EM_FP_GRP_TEMPLATE",
    .group_key_fid = "DT_EM_FP_GRP_TEMPLATE_INDEX",
    .rule_sid = "DT_EM_FP_RULE_TEMPLATE",
    .rule_key_fid = "EM_FP_RULE_TEMPLATE_INDEX",
    .entry_rule_key_fid = "DT_EM_FP_RULE_TEMPLATE_INDEX",
    .policy_sid = "DT_EM_FP_ACTION_TEMPLATE",
    .policy_key_fid = "EM_FP_ACTION_TEMPLATE_INDEX",
    .entry_policy_key_fid = "DT_EM_FP_ACTION_TEMPLATE_INDEX",
    .entry_sid = "DT_EM_FP_ENTRY",
    .entry_key_fid = "DT_EM_FP_ENTRY_INDEX",
    .pdd_sid = "DT_EM_FP_PDD_TEMPLATE",
    .pdd_key_fid = "DT_EM_FP_PDD_TEMPLATE_INDEX",
    .sbr_sid = "DT_EM_FP_SBR_TEMPLATE",
    .sbr_key_fid = "DT_EM_FP_SBR_TEMPLATE_INDEX",
    .presel_entry_sid = "DT_EM_FP_PRESEL_TEMPLATE",
    .presel_entry_key_fid = "EM_FP_PRESEL_TEMPLATE_INDEX",
    .grp_presel_entry_key_fid = "DT_EM_FP_PRESEL_TEMPLATE_INDEX",
    .presel_entry_count_fid = "NUM_DT_EM_FP_PRESEL_TEMPLATE_INDEX",
    .presel_group_sid = "DT_EM_FP_PRESEL_GRP_TEMPLATE",
    .presel_group_key_fid = "DT_EM_FP_PRESEL_GRP_TEMPLATE_INDEX",
    .ctr_sid = NULL,
    .ctr_key_fid = "FLEX_CTR_ACTION",
    .meter_sid = NULL,
    .meter_key_fid = NULL,
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid = NULL,
    .flex_ctr_idx0_fid = "ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0",
    .flex_ctr_idx1_fid = "ING_OBJ_BUS_EM_FT_OPAQUE_OBJ1",
    .compression_type_fid = NULL,
    .compression_type_cnt_fid = NULL,
    .grp_oper_mode_fid = "MODE_OPER",
    .entry_enable_fid = "DT_EM_GRP_TEMPLATE_INDEX",
    .def_sbr_key_fid = "DEFAULT_DT_EM_FP_SBR_TEMPLATE_INDEX",
    .def_pdd_key_fid = "DEFAULT_DT_EM_FP_PDD_TEMPLATE_INDEX",
    .def_policy_key_fid = "DEFAULT_DT_EM_FP_ACTION_TEMPLATE_INDEX",
    .entry_enable_fid = "ENABLE",
    .entry_priority_fid = NULL,
    .group_info_sid = "FP_EM_GRP_TEMPLATE_INFO",
    .group_info_key_fid = "FP_EM_GRP_TEMPLATE_ID",
    .group_info_num_parts_fid = "NUM_PARTITION_ID",
    .group_info_entries_created_fid = "NUM_ENTRIES_CREATED",
    .group_info_entries_tentative_fid = NULL,
    .group_info_partition_sid = "FP_EM_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_partition_key_fid = "FP_EM_GRP_TEMPLATE_PARTITION_INFO_ID",
    .group_info_action_sid = "FP_EM_PDD_TEMPLATE_PARTITION_INFO",
    .group_info_action_key_fid = "FP_EM_PDD_TEMPLATE_PARTITION_INFO_ID",
    .group_info_presel_sid = "FP_EM_PRESEL_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_presel_key_fid = "FP_EM_PRESEL_GRP_TEMPLATE_PARTITION_INFO_ID",
    .group_info_common_partition_key_fid = "PARTITION_ID",
    .slice_info_sid = NULL,
    .action_cont_info_sid = "DT_EM_FP_ACTION_TEMPLATE_CONTAINER_INFO",
    .action_cont_info_key_fid = "FIELD_ACTION",
    .stage_info_sid = NULL,
    .group_info_act_res_fid = "HW_LTID",
    .presel_entry_prio_fid = "ENTRY_PRIORITY",
    .meter_info_sid = NULL,
};

static
bcmint_field_tbls_info_t bcm56780_a0_ft_tbls_info = {
    .group_sid = "DT_EM_FT_GRP_TEMPLATE",
    .group_key_fid = "DT_EM_FT_GRP_TEMPLATE_INDEX",
    .rule_sid = "DT_EM_FT_RULE_TEMPLATE",
    .rule_key_fid = "EM_FT_RULE_TEMPLATE_INDEX",
    .entry_rule_key_fid = "DT_EM_FT_RULE_TEMPLATE_INDEX",
    .policy_sid = "DT_EM_FT_ACTION_TEMPLATE",
    .policy_key_fid = "EM_FT_ACTION_TEMPLATE_INDEX",
    .entry_policy_key_fid = "DT_EM_FT_ACTION_TEMPLATE_INDEX",
    .entry_sid = "DT_EM_FT_ENTRY",
    .entry_key_fid = "DT_EM_FT_ENTRY_INDEX",
    .pdd_sid = "DT_EM_FT_PDD_TEMPLATE",
    .pdd_key_fid = "DT_EM_FT_PDD_TEMPLATE_INDEX",
    .sbr_sid = "DT_EM_FT_SBR_TEMPLATE",
    .sbr_key_fid = "DT_EM_FT_SBR_TEMPLATE_INDEX",
    .presel_entry_sid = "DT_EM_FT_PRESEL_TEMPLATE",
    .presel_entry_key_fid = "EM_FT_PRESEL_TEMPLATE_INDEX",
    .grp_presel_entry_key_fid = "DT_EM_FT_PRESEL_TEMPLATE_INDEX",
    .presel_entry_count_fid = "NUM_DT_EM_FT_PRESEL_TEMPLATE_INDEX",
    .presel_group_sid = "DT_EM_FT_PRESEL_GRP_TEMPLATE",
    .presel_group_key_fid = "DT_EM_FT_PRESEL_GRP_TEMPLATE_INDEX",
    .ctr_sid = NULL,
    .ctr_key_fid = "FLEX_CTR_ACTION",
    .meter_sid = NULL,
    .meter_key_fid = NULL,
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid = NULL,
    .flex_ctr_idx0_fid = "ING_OBJ_BUS_EM_FT_OPAQUE_OBJ0",
    .flex_ctr_idx1_fid = "ING_OBJ_BUS_EM_FT_OPAQUE_OBJ1",
    .compression_type_fid = NULL,
    .compression_type_cnt_fid = NULL,
    .grp_oper_mode_fid = "MODE_OPER",
    .entry_enable_fid = "DT_EM_FT_GRP_TEMPLATE_INDEX",
    .def_sbr_key_fid = "DEFAULT_DT_EM_FT_SBR_TEMPLATE_INDEX",
    .def_pdd_key_fid = "DEFAULT_DT_EM_FT_PDD_TEMPLATE_INDEX",
    .def_policy_key_fid = "DEFAULT_DT_EM_FT_ACTION_TEMPLATE_INDEX",
    .entry_priority_fid = NULL,
    .group_info_sid = "FP_FT_GRP_TEMPLATE_INFO",
    .group_info_key_fid = "FP_FT_GRP_TEMPLATE_ID",
    .group_info_num_parts_fid = "NUM_PARTITION_ID",
    .group_info_entries_created_fid = "NUM_ENTRIES_CREATED",
    .group_info_entries_tentative_fid = NULL,
    .group_info_partition_sid = "FP_FT_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_partition_key_fid = "FP_FT_GRP_TEMPLATE_PARTITION_INFO_ID",
    .group_info_action_sid = "FP_FT_PDD_TEMPLATE_PARTITION_INFO",
    .group_info_action_key_fid = "FP_FT_PDD_TEMPLATE_PARTITION_INFO_ID",
    .group_info_presel_sid = "FP_FT_PRESEL_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_presel_key_fid = "FP_FT_PRESEL_GRP_TEMPLATE_PARTITION_INFO_ID",
    .group_info_common_partition_key_fid = "PARTITION_ID",
    .slice_info_sid = NULL,
    .action_cont_info_sid = "DT_EM_FT_ACTION_TEMPLATE_CONTAINER_INFO",
    .action_cont_info_key_fid = "FIELD_ACTION",
    .stage_info_sid = NULL,
    .group_info_act_res_fid = "HW_LTID",
    .presel_entry_prio_fid = "ENTRY_PRIORITY",
    .meter_info_sid = NULL,
};

static
bcmint_field_tbls_info_t bcm56780_a0_vfp_tbls_info = {
    .group_sid = "DT_VFP_GRP_TEMPLATE",
    .group_key_fid = "DT_VFP_GRP_TEMPLATE_INDEX",
    .rule_sid = "DT_VFP_RULE_TEMPLATE",
    .rule_key_fid = "VFP_RULE_TEMPLATE_INDEX",
    .entry_rule_key_fid = "DT_VFP_RULE_TEMPLATE_INDEX",
    .policy_sid = "DT_VFP_ACTION_TEMPLATE",
    .policy_key_fid = "VFP_ACTION_TEMPLATE_INDEX",
    .entry_policy_key_fid = "DT_VFP_ACTION_TEMPLATE_INDEX",
    .entry_sid = "DT_VFP_ENTRY",
    .entry_key_fid = "DT_VFP_ENTRY_INDEX",
    .pdd_sid = "DT_VFP_PDD_TEMPLATE",
    .pdd_key_fid = "DT_VFP_PDD_TEMPLATE_INDEX",
    .sbr_sid = "DT_VFP_SBR_TEMPLATE",
    .sbr_key_fid = "DT_VFP_SBR_TEMPLATE_INDEX",
    .presel_entry_sid = "DT_VFP_PRESEL_TEMPLATE",
    .presel_entry_key_fid = "VFP_PRESEL_TEMPLATE_INDEX",
    .grp_presel_entry_key_fid = "DT_VFP_PRESEL_TEMPLATE_INDEX",
    .presel_entry_count_fid = "NUM_DT_VFP_PRESEL_TEMPLATE_INDEX",
    .presel_group_sid = "DT_VFP_PRESEL_GRP_TEMPLATE",
    .presel_group_key_fid = "DT_VFP_PRESEL_GRP_TEMPLATE_INDEX",
    .ctr_sid = NULL,
    .ctr_key_fid = "FLEX_CTR_ACTION",
    .meter_sid = NULL,
    .meter_key_fid = NULL,
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid = "FP_VLAN_OPERMODE_PIPEUNIQUE",
    .flex_ctr_idx0_fid = "ING_OBJ_BUS_VFP_OPAQUE_OBJ0",
    .flex_ctr_idx1_fid = NULL,
    .compression_type_fid = NULL,
    .compression_type_cnt_fid = NULL,
    .grp_oper_mode_fid = "MODE_OPER",
    .entry_enable_fid = "DT_VFP_GRP_TEMPLATE_INDEX",
    .def_sbr_key_fid = NULL,
    .def_pdd_key_fid = NULL,
    .def_policy_key_fid = NULL,
    .entry_enable_fid = "ENABLE",
    .entry_priority_fid = "ENTRY_PRIORITY",
    .group_info_sid = "FP_VLAN_GRP_TEMPLATE_INFO",
    .group_info_key_fid = "FP_VLAN_GRP_TEMPLATE_ID",
    .group_info_num_parts_fid = "NUM_PARTITION_ID",
    .group_info_entries_created_fid = "NUM_ENTRIES_CREATED",
    .group_info_entries_tentative_fid = "NUM_ENTRIES_TENTATIVE",
    .group_info_partition_sid = "FP_VLAN_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_partition_key_fid = "FP_VLAN_GRP_TEMPLATE_PARTITION_INFO_ID",
    .group_info_action_sid = "FP_VLAN_PDD_TEMPLATE_PARTITION_INFO",
    .group_info_action_key_fid = "FP_VLAN_PDD_TEMPLATE_PARTITION_INFO_ID",
    .group_info_presel_sid = "FP_VLAN_PRESEL_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_presel_key_fid = "FP_VLAN_PRESEL_GRP_TEMPLATE_PARTITION_INFO_ID",
    .group_info_common_partition_key_fid = "PARTITION_ID",
    .group_info_num_slice_fid = "NUM_SLICE_ID",
    .group_info_slice_fid = "SLICE_ID",
    .slice_info_sid = "FP_VLAN_SLICE_INFO",
    .slice_info_key_fid = "FP_VLAN_SLICE_ID",
    .slice_info_pipe_fid = "PIPE",
    .slice_info_entry_in_use_fid = "ENTRY_INUSE_CNT",
    .slice_info_entry_max_fid = "ENTRY_MAXIMUM",
    .action_cont_info_sid = "DT_VFP_ACTION_TEMPLATE_CONTAINER_INFO",
    .action_cont_info_key_fid = "FIELD_ACTION",
    .stage_info_sid = "FP_VLAN_INFO",
    .stage_info_num_slices_fid = "NUM_SLICES",
    .group_info_act_res_fid = "HW_LTID",
    .presel_entry_prio_fid = "ENTRY_PRIORITY",
    .meter_info_sid = NULL,
};

static
bcmint_field_tbls_info_t bcm56780_a0_efp_tbls_info = {
    .group_sid = "DT_EFP_GRP_TEMPLATE",
    .group_key_fid = "DT_EFP_GRP_TEMPLATE_INDEX",
    .rule_sid = "DT_EFP_RULE_TEMPLATE",
    .rule_key_fid = "EFP_RULE_TEMPLATE_INDEX",
    .entry_rule_key_fid = "DT_EFP_RULE_TEMPLATE_INDEX",
    .policy_sid = "DT_EFP_ACTION_TEMPLATE",
    .policy_key_fid = "EFP_ACTION_TEMPLATE_INDEX",
    .entry_policy_key_fid = "DT_EFP_ACTION_TEMPLATE_INDEX",
    .entry_sid = "DT_EFP_ENTRY",
    .entry_key_fid = "DT_EFP_ENTRY_INDEX",
    .pdd_sid = "DT_EFP_PDD_TEMPLATE",
    .pdd_key_fid = "DT_EFP_PDD_TEMPLATE_INDEX",
    .sbr_sid = "DT_EFP_SBR_TEMPLATE",
    .sbr_key_fid = "DT_EFP_SBR_TEMPLATE_INDEX",
    .presel_entry_sid = "DT_EFP_PRESEL_TEMPLATE",
    .presel_entry_key_fid = "EFP_PRESEL_TEMPLATE_INDEX",
    .grp_presel_entry_key_fid = "DT_EFP_PRESEL_TEMPLATE_INDEX",
    .presel_entry_count_fid = "NUM_DT_EFP_PRESEL_TEMPLATE_INDEX",
    .presel_group_sid = "DT_EFP_PRESEL_GRP_TEMPLATE",
    .presel_group_key_fid = "DT_EFP_PRESEL_GRP_TEMPLATE_INDEX",
    .ctr_sid = NULL,
    .ctr_key_fid = "FLEX_CTR_ACTION",
    .meter_sid = "METER_EGR_FP_TEMPLATE",
    .meter_key_fid = "METER_EGR_FP_TEMPLATE_ID",
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid= "FP_EGR_OPERMODE_PIPEUNIQUE",
    .flex_ctr_idx0_fid = "EGR_OBJ_BUS_EFP_OPAQUE_OBJ1",
    .flex_ctr_idx1_fid = "EGR_OBJ_BUS_EFP_OPAQUE_OBJ2",
    .compression_type_fid = NULL,
    .compression_type_cnt_fid = NULL,
    .grp_oper_mode_fid = "MODE_OPER",
    .entry_enable_fid = "DT_EFP_GRP_TEMPLATE_INDEX",
    .def_sbr_key_fid = NULL,
    .def_pdd_key_fid = NULL,
    .def_policy_key_fid = NULL,
    .entry_enable_fid = "ENABLE",
    .entry_priority_fid = "ENTRY_PRIORITY",
    .group_info_sid = "FP_EGR_GRP_TEMPLATE_INFO",
    .group_info_key_fid = "FP_EGR_GRP_TEMPLATE_ID",
    .group_info_num_parts_fid = "NUM_PARTITION_ID",
    .group_info_entries_created_fid = "NUM_ENTRIES_CREATED",
    .group_info_entries_tentative_fid = "NUM_ENTRIES_TENTATIVE",
    .group_info_partition_sid = "FP_EGR_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_partition_key_fid = "FP_EGR_GRP_TEMPLATE_PARTITION_INFO_ID",
    .group_info_action_sid = "FP_EGR_PDD_TEMPLATE_PARTITION_INFO",
    .group_info_action_key_fid = "FP_EGR_PDD_TEMPLATE_PARTITION_INFO_ID",
    .group_info_presel_sid = "FP_EGR_PRESEL_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_presel_key_fid = "FP_EGR_PRESEL_GRP_TEMPLATE_PARTITION_INFO_ID",
    .group_info_common_partition_key_fid = "PARTITION_ID",
    .group_info_num_slice_fid = "NUM_SLICE_ID",
    .group_info_slice_fid = "SLICE_ID",
    .slice_info_sid = "FP_EGR_SLICE_INFO",
    .slice_info_key_fid = "FP_EGR_SLICE_ID",
    .slice_info_pipe_fid = "PIPE",
    .slice_info_entry_in_use_fid = "ENTRY_INUSE_CNT",
    .slice_info_entry_max_fid = "ENTRY_MAXIMUM",
    .action_cont_info_sid = "DT_EFP_ACTION_TEMPLATE_CONTAINER_INFO",
    .action_cont_info_key_fid = "FIELD_ACTION",
    .stage_info_sid = "FP_EGR_INFO",
    .stage_info_num_slices_fid = "NUM_SLICES",
    .group_info_act_res_fid = "HW_LTID",
    .presel_entry_prio_fid = "ENTRY_PRIORITY",
    .meter_info_sid = "METER_EGR_FP_DEVICE_INFO",
    .meter_info_num_meters_fid = "NUM_METERS",
    .meter_info_num_meters_per_pipe_fid = "NUM_METERS_PER_PIPE",
};

static
bcmint_field_color_action_tbls_info_t bcm56780_a0_efp_color_action_tbls_info = {
    .num_pools = 1,
    .pipe_cfg_sid = "DT_EFP_METER_ACTION_ACTION_TEMPLATE_PIPE_CONFIG",
    .pipe_cfg_fid = "PIPEUNIQUE",
    .pipe_fid = "PIPE",
    .pdd_tbl_sid = "DT_EFP_METER_ACTION_PDD_TEMPLATE",
    .pdd_tbl_key_fid = "DT_EFP_METER_ACTION_PDD_TEMPLATE_ID",
    .sbr_sid = "DT_EFP_METER_ACTION_SBR_TEMPLATE",
    .sbr_key_fid = "DT_EFP_METER_ACTION_SBR_TEMPLATE_ID",
    .tbl_sid = {
        "DT_EFP_METER_ACTION_ACTION_TEMPLATE_0",
    },
    .tbl_key_fid = {
        "DT_EFP_METER_ACTION_TEMPLATE_ID",
    },
};



/*!
 * \brief Initialize Field Qualifiers, Actions and Presel database.
 *
 * \param [in] unit Unit Number.
 * \param [in|out] stage pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_dna_2_4_13_ltsw_field_stage_db_init(int unit,
                                         bcmint_field_stage_info_t *stage_info)
{
    int rv;
    const char *tbl_desc;
    uint32_t ltid = 0, fid = 0;
    SHR_FUNC_ENTER(unit);

    if (stage_info->stage_id == bcmiFieldStageIngress) {
        /* Initialise lt table info */
        stage_info->tbls_info = &bcm56780_a0_ifp_tbls_info;
        stage_info->color_action_tbls_info = &bcm56780_a0_ifp_color_action_tbls_info;

        /* Initialise qual db counts */
        stage_info->qual_db_count = (sizeof(bcm56780_a0_dna_2_4_13_ifp_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->presel_qual_db_count =
                                   (sizeof(bcm56780_a0_dna_2_4_13_ifp_presel_qual_data)) /
                                   (sizeof(bcm_field_qual_map_t));
        stage_info->action_db_count = (sizeof(bcm56780_a0_dna_2_4_13_ifp_action_data)) /
                                            (sizeof(bcm_field_action_map_t));

        /* Initialise lt map db */
        stage_info->qual_lt_map_db = bcm56780_a0_dna_2_4_13_ifp_qual_data;
        stage_info->presel_qual_lt_map_db = bcm56780_a0_dna_2_4_13_ifp_presel_qual_data;
        stage_info->action_lt_map_db = bcm56780_a0_dna_2_4_13_ifp_action_data;

        /*
         * Check if field ING_CMD_BUS_SFC_IFP_METER_PROFILE_INDEX is present
         * in DT_IFP_ACTION_TEMPLATE. If present, 4 meter pools can be
         * supported. Else, only 2 can be supported.
         */
        rv = bcmlt_table_field_id_get_by_name(unit,
                                              stage_info->tbls_info->policy_sid,
                                              "ING_CMD_BUS_SFC_IFP_METER_PROFILE_INDEX",
                                              &ltid,
                                              &fid);
        if (rv != SHR_E_NONE) {
            rv = SHR_E_NONE;
            stage_info->color_action_tbls_info->num_pools = 2;
        }
    } else if (stage_info->stage_id == bcmiFieldStageExactMatch) {
        /* Initialise lt table info */
        stage_info->tbls_info = &bcm56780_a0_em_tbls_info;
        rv = bcmlt_table_desc_get(unit,
                         bcm56780_a0_em_tbls_info.group_sid, &tbl_desc);
        if (SHR_FAILURE(rv)) {
            SHR_EXIT();
        }

        stage_info->qual_db_count = (sizeof(bcm56780_a0_dna_2_4_13_ifp_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->presel_qual_db_count =
                                   (sizeof(bcm56780_a0_dna_2_4_13_ifp_presel_qual_data)) /
                                   (sizeof(bcm_field_qual_map_t));
        stage_info->action_db_count = (sizeof(bcm56780_a0_dna_2_4_13_em_action_data)) /
                                            (sizeof(bcm_field_action_map_t));

        /* Initialise lt map db */
        stage_info->qual_lt_map_db = bcm56780_a0_dna_2_4_13_ifp_qual_data;
        stage_info->presel_qual_lt_map_db = bcm56780_a0_dna_2_4_13_ifp_presel_qual_data;

        /*stage_info->action_lt_map_db = bcm56780_a0_ifp_action_data;*/
        stage_info->action_lt_map_db = bcm56780_a0_dna_2_4_13_em_action_data;
    } else if (stage_info->stage_id == bcmiFieldStageFlowTracker) {
        /* Initialise lt table info */
        stage_info->tbls_info = &bcm56780_a0_ft_tbls_info;
        rv = bcmlt_table_desc_get(unit,
                         bcm56780_a0_ft_tbls_info.group_sid, &tbl_desc);
        if (SHR_FAILURE(rv)) {
            SHR_EXIT();
        }

        stage_info->qual_db_count = (sizeof(bcm56780_a0_dna_2_4_13_ifp_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->presel_qual_db_count =
                                   (sizeof(bcm56780_a0_dna_2_4_13_ifp_presel_qual_data)) /
                                   (sizeof(bcm_field_qual_map_t));
        stage_info->action_db_count = (sizeof(bcm56780_a0_dna_2_4_13_em_action_data)) /
                                            (sizeof(bcm_field_action_map_t));

        /* Initialise lt map db */
        stage_info->qual_lt_map_db = bcm56780_a0_dna_2_4_13_ifp_qual_data;

        stage_info->presel_qual_lt_map_db = bcm56780_a0_dna_2_4_13_ifp_presel_qual_data;

        /*stage_info->action_lt_map_db = bcm56780_a0_ifp_action_data;*/
        stage_info->action_lt_map_db = bcm56780_a0_dna_2_4_13_em_action_data;
    } else if (stage_info->stage_id == bcmiFieldStageEgress) {
        /* Initialise lt table info */
        stage_info->tbls_info = &bcm56780_a0_efp_tbls_info;
        stage_info->color_action_tbls_info = &bcm56780_a0_efp_color_action_tbls_info;

        /* Initialise qual db counts */
        stage_info->qual_db_count = (sizeof(bcm56780_a0_dna_2_4_13_efp_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->presel_qual_db_count =
                                   (sizeof(bcm56780_a0_dna_2_4_13_efp_presel_qual_data)) /
                                                 (sizeof(bcm_field_qual_map_t));
        stage_info->action_db_count = (sizeof(bcm56780_a0_dna_2_4_13_efp_action_data)) /
                                            (sizeof(bcm_field_action_map_t));

        /* Initialise lt map db */
        stage_info->qual_lt_map_db = bcm56780_a0_dna_2_4_13_efp_qual_data;

        stage_info->presel_qual_lt_map_db = bcm56780_a0_dna_2_4_13_efp_presel_qual_data;

        stage_info->action_lt_map_db = bcm56780_a0_dna_2_4_13_efp_action_data;

    } else if (stage_info->stage_id == bcmiFieldStageVlan) {
        /* Initialise lt table info */
        stage_info->tbls_info = &bcm56780_a0_vfp_tbls_info;
        rv = bcmlt_table_desc_get(unit,
                         bcm56780_a0_vfp_tbls_info.group_sid, &tbl_desc);
        if (SHR_FAILURE(rv)) {
            SHR_EXIT();
        }

        /* Initialise qual db counts */
        stage_info->qual_db_count = (sizeof(bcm56780_a0_dna_2_4_13_vfp_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->presel_qual_db_count =
                                    (sizeof(bcm56780_a0_dna_2_4_13_vfp_presel_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->action_db_count = (sizeof(bcm56780_a0_dna_2_4_13_vfp_action_data)) /
                                               (sizeof(bcm_field_action_map_t));

        /* Initialise lt map db */
        stage_info->qual_lt_map_db = bcm56780_a0_dna_2_4_13_vfp_qual_data;

        stage_info->presel_qual_lt_map_db = bcm56780_a0_dna_2_4_13_vfp_presel_qual_data;

        stage_info->action_lt_map_db = bcm56780_a0_dna_2_4_13_vfp_action_data;
    } else {
        /* Do nothing */
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_dna_2_4_13_ltsw_field_control_lt_info_init(
        int unit,
        bcmint_field_control_info_t *control_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(control_info, SHR_E_PARAM);

    control_info->comp_tbls_info = &bcm56780_a0_compress_tbls_info;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_dna_2_4_13_ltsw_field_manual_enum_to_lt_enum_value_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        uint32 qual_enum,
        bcm_qual_field_t *qual_info)
{
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    switch (qual) {

        case bcmFieldQualifyFirstDropReason:
        case bcmFieldQualifyDropReason:
        case bcmFieldQualifyRecircleHdrFirstDropReason:
        case bcmFieldQualifyRecircleHdrDropReason:

            if ((stage_info->stage_id == bcmiFieldStageIngress) ||
                (stage_info->stage_id == bcmiFieldStageVlan)) {

                switch (qual_enum) {
                    case bcmFieldDropReasonNoDrop:
                        qual_info->enum_value = 0;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonCMLFlags:
                        qual_info->enum_value = 8;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL2SrcStaticMove:
                        qual_info->enum_value = 9;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL2SrcDiscard:
                        qual_info->enum_value = 10;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonMacSaMulticast:
                        qual_info->enum_value = 11;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonOuterTpidCheckFailed:
                        qual_info->enum_value = 12;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonIncomingPvlanCheckFailed:
                        qual_info->enum_value = 13;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonPktIntegrityCheckFailed:
                        qual_info->enum_value = 14;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonProtocolPktDrop:
                        qual_info->enum_value = 15;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonMembershipCheckFailed:
                        qual_info->enum_value = 16;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonSpanningTreeCheckFailed:
                        qual_info->enum_value = 17;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL2DstLookupMiss:
                        qual_info->enum_value = 18;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL2DstDiscard:
                        qual_info->enum_value = 19;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL3DstLookupMiss:
                        qual_info->enum_value = 20;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL3DstDiscard:
                        qual_info->enum_value = 21;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL3HdrError:
                        qual_info->enum_value = 22;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL3TTLError:
                        qual_info->enum_value = 23;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonIPMCL3IIFOrRPAIDCheckFailed:
                        qual_info->enum_value = 24;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonTunnelTTLCheckFailed:
                        qual_info->enum_value = 25;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonTunnelShimHdrError:
                        qual_info->enum_value = 26;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonTunnelObjectValidationFailed:
                        qual_info->enum_value = 27;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonTunnelAdaptDrop:
                        qual_info->enum_value = 28;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonPVLANDrop:
                        qual_info->enum_value = 29;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonVFP:
                        qual_info->enum_value = 30;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonIFP:
                        qual_info->enum_value = 31;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonIFPMeter:
                        qual_info->enum_value = 32;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonDSTFP:
                        qual_info->enum_value = 33;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonMplsProtectionDrop:
                        qual_info->enum_value = 34;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonMplsLabelActionInvalid:
                        qual_info->enum_value = 35;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonMplsTermPolicySelectTableDrop:
                        qual_info->enum_value = 36;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonMPLSReservedLabelExposed:
                        qual_info->enum_value = 37;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonMplsTTLError:
                        qual_info->enum_value = 38;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonMplsEcnError:
                        qual_info->enum_value = 39;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEMFT:
                        qual_info->enum_value = 40;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonIVXLT:
                        qual_info->enum_value = 41;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonUrpfCheckFailed:
                        qual_info->enum_value = 97;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonSrcPortKnockoutDrop:
                        qual_info->enum_value = 98;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonLagFailoverPortDown:
                        qual_info->enum_value = 99;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonSplitHorizonCheckFailed:
                        qual_info->enum_value = 100;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonDstLinkDown:
                        qual_info->enum_value = 101;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonBlockMaskDrop:
                        qual_info->enum_value = 102;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL3MtuCheckFailed:
                        qual_info->enum_value = 103;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonSeqNumCheckFailed:
                        qual_info->enum_value = 104;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL3IIFEqL3OIF:
                        qual_info->enum_value = 105;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonStormControlDrop:
                        qual_info->enum_value = 106;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrMembershipCheckFailed:
                        qual_info->enum_value = 107;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrSpanningTreeCheckFailed:
                        qual_info->enum_value = 108;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonDstPBMZero:
                        qual_info->enum_value = 109;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonMplsCtrlPktDrop:
                        qual_info->enum_value = 110;
                        qual_info->enum_mask = 255;
                        break;
                    default:
                        LOG_ERROR(BSL_LS_BCM_FP,
                           (BSL_META_U(unit, "Error: Enum value not available for provided stage.\n")));
                        SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }

            } else if (stage_info->stage_id == bcmiFieldStageEgress) {

                switch (qual_enum) {
                    case bcmFieldDropReasonNoDrop:
                        qual_info->enum_value = 0;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL2OIFDrop:
                        qual_info->enum_value = 128;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrMembershipDrop:
                        qual_info->enum_value = 129;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonDvpMembershipDrop:
                        qual_info->enum_value = 130;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonTtlDrop:
                        qual_info->enum_value = 131;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL3SameIntfDrop:
                        qual_info->enum_value = 132;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonL2SamePortDrop:
                        qual_info->enum_value = 133;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonSplitHorizonDrop:
                        qual_info->enum_value = 134;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonSpanningTreeDisableDrop:
                        qual_info->enum_value = 135;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonSpanningTreeBlockDrop:
                        qual_info->enum_value = 136;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEFP:
                        qual_info->enum_value = 137;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrMeterDrop:
                        qual_info->enum_value = 138;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrMtuDrop:
                        qual_info->enum_value = 139;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrVxltDrop:
                        qual_info->enum_value = 140;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrCellTooLargeDrop:
                        qual_info->enum_value = 141;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrCellTooSmallDrop:
                        qual_info->enum_value = 142;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonQosRemarkingDrop:
                        qual_info->enum_value = 143;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonSvpEqualDvpDrop:
                        qual_info->enum_value = 144;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonInvalid1588Pkt:
                        qual_info->enum_value = 145;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonFlexEditorDrop:
                        qual_info->enum_value = 146;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrRecircleHdrDrop:
                        qual_info->enum_value = 147;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonIfaMdDeleteDrop:
                        qual_info->enum_value = 148;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonIngPortGroupLimit:
                        qual_info->enum_value = 1;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonIngPortServicePoolLimit:
                        qual_info->enum_value = 2;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonIngHeadroomPoolLimit:
                        qual_info->enum_value = 4;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrQueueLimit:
                        qual_info->enum_value = 5;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonEgrPortServicePoolLimit:
                        qual_info->enum_value = 6;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonWREDCheck:
                        qual_info->enum_value = 7;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonIngRecircleHdrDrop:
                        qual_info->enum_value = 42;
                        qual_info->enum_mask = 255;
                        break;
                    case bcmFieldDropReasonMtopIpv4Gateway:
                        qual_info->enum_value = 43;
                        qual_info->enum_mask = 255;
                        break;
                    default:
                        LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit, "Error: Enum value not available for provided stage.\n")));
                        SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }
            }
            break;

        default:
            SHR_ERR_EXIT(SHR_E_NONE);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_dna_2_4_13_ltsw_field_manual_lt_enum_value_to_enum_get(
        int unit,
        bcm_field_entry_t entry,
        bcm_field_qualify_t qual,
        bcm_qual_field_t *qual_info,
        uint32 *qual_enum)
{
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    switch (qual) {
        case bcmFieldQualifyFirstDropReason:
        case bcmFieldQualifyDropReason:
        case bcmFieldQualifyRecircleHdrFirstDropReason:
        case bcmFieldQualifyRecircleHdrDropReason:

            if ((stage_info->stage_id == bcmiFieldStageIngress) ||
                (stage_info->stage_id == bcmiFieldStageVlan)) {

                switch (qual_info->enum_value) {
                    case 0:
                        *qual_enum = bcmFieldDropReasonNoDrop;
                        break;
                    case 8:
                        *qual_enum = bcmFieldDropReasonCMLFlags;
                        break;
                    case 9:
                        *qual_enum = bcmFieldDropReasonL2SrcStaticMove;
                        break;
                    case 10:
                        *qual_enum = bcmFieldDropReasonL2SrcDiscard;
                        break;
                    case 11:
                        *qual_enum = bcmFieldDropReasonMacSaMulticast;
                        break;
                    case 12:
                        *qual_enum = bcmFieldDropReasonOuterTpidCheckFailed;
                        break;
                    case 13:
                        *qual_enum = bcmFieldDropReasonIncomingPvlanCheckFailed;
                        break;
                    case 14:
                        *qual_enum = bcmFieldDropReasonPktIntegrityCheckFailed;
                        break;
                    case 15:
                        *qual_enum = bcmFieldDropReasonProtocolPktDrop;
                        break;
                    case 16:
                        *qual_enum = bcmFieldDropReasonMembershipCheckFailed;
                        break;
                    case 17:
                        *qual_enum = bcmFieldDropReasonSpanningTreeCheckFailed;
                        break;
                    case 18:
                        *qual_enum = bcmFieldDropReasonL2DstLookupMiss;
                        break;
                    case 19:
                        *qual_enum = bcmFieldDropReasonL2DstDiscard;
                        break;
                    case 20:
                        *qual_enum = bcmFieldDropReasonL3DstLookupMiss;
                        break;
                    case 21:
                        *qual_enum = bcmFieldDropReasonL3DstDiscard;
                        break;
                    case 22:
                        *qual_enum = bcmFieldDropReasonL3HdrError;
                        break;
                    case 23:
                        *qual_enum = bcmFieldDropReasonL3TTLError;
                        break;
                    case 24:
                        *qual_enum = bcmFieldDropReasonIPMCL3IIFOrRPAIDCheckFailed;
                        break;
                    case 25:
                        *qual_enum = bcmFieldDropReasonTunnelTTLCheckFailed;
                        break;
                    case 26:
                        *qual_enum = bcmFieldDropReasonTunnelShimHdrError;
                        break;
                    case 27:
                        *qual_enum = bcmFieldDropReasonTunnelObjectValidationFailed;
                        break;
                    case 28:
                        *qual_enum = bcmFieldDropReasonTunnelAdaptDrop;
                        break;
                    case 29:
                        *qual_enum = bcmFieldDropReasonPVLANDrop;
                        break;
                    case 30:
                        *qual_enum = bcmFieldDropReasonVFP;
                        break;
                    case 31:
                        *qual_enum = bcmFieldDropReasonIFP;
                        break;
                    case 32:
                        *qual_enum = bcmFieldDropReasonIFPMeter;
                        break;
                    case 33:
                        *qual_enum = bcmFieldDropReasonDSTFP;
                        break;
                    case 34:
                        *qual_enum = bcmFieldDropReasonMplsProtectionDrop;
                        break;
                    case 35:
                        *qual_enum = bcmFieldDropReasonMplsLabelActionInvalid;
                        break;
                    case 36:
                        *qual_enum = bcmFieldDropReasonMplsTermPolicySelectTableDrop;
                        break;
                    case 37:
                        *qual_enum = bcmFieldDropReasonMPLSReservedLabelExposed;
                        break;
                    case 38:
                        *qual_enum = bcmFieldDropReasonMplsTTLError;
                        break;
                    case 39:
                        *qual_enum = bcmFieldDropReasonMplsEcnError;
                        break;
                    case 40:
                        *qual_enum = bcmFieldDropReasonEMFT;
                        break;
                    case 41:
                        *qual_enum = bcmFieldDropReasonIVXLT;
                        break;
                    case 97:
                        *qual_enum = bcmFieldDropReasonUrpfCheckFailed;
                        break;
                    case 98:
                        *qual_enum = bcmFieldDropReasonSrcPortKnockoutDrop;
                        break;
                    case 99:
                        *qual_enum = bcmFieldDropReasonLagFailoverPortDown;
                        break;
                    case 100:
                        *qual_enum = bcmFieldDropReasonSplitHorizonCheckFailed;
                        break;
                    case 101:
                        *qual_enum = bcmFieldDropReasonDstLinkDown;
                        break;
                    case 102:
                        *qual_enum = bcmFieldDropReasonBlockMaskDrop;
                        break;
                    case 103:
                        *qual_enum = bcmFieldDropReasonL3MtuCheckFailed;
                        break;
                    case 104:
                        *qual_enum = bcmFieldDropReasonSeqNumCheckFailed;
                        break;
                    case 105:
                        *qual_enum = bcmFieldDropReasonL3IIFEqL3OIF;
                        break;
                    case 106:
                        *qual_enum = bcmFieldDropReasonStormControlDrop;
                        break;
                    case 107:
                        *qual_enum = bcmFieldDropReasonEgrMembershipCheckFailed;
                        break;
                    case 108:
                        *qual_enum = bcmFieldDropReasonEgrSpanningTreeCheckFailed;
                        break;
                    case 109:
                        *qual_enum = bcmFieldDropReasonDstPBMZero;
                        break;
                    case 110:
                        *qual_enum = bcmFieldDropReasonMplsCtrlPktDrop;
                        break;
                    default:
                        SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }

            } else if (stage_info->stage_id == bcmiFieldStageEgress) {

                switch (qual_info->enum_value) {
                    case 0:
                        *qual_enum = bcmFieldDropReasonNoDrop;
                        break;
                    case 128:
                        *qual_enum = bcmFieldDropReasonL2OIFDrop;
                        break;
                    case 129:
                        *qual_enum = bcmFieldDropReasonEgrMembershipDrop;
                        break;
                    case 130:
                        *qual_enum = bcmFieldDropReasonDvpMembershipDrop;
                        break;
                    case 131:
                        *qual_enum = bcmFieldDropReasonTtlDrop;
                        break;
                    case 132:
                        *qual_enum = bcmFieldDropReasonL3SameIntfDrop;
                        break;
                    case 133:
                        *qual_enum = bcmFieldDropReasonL2SamePortDrop;
                        break;
                    case 134:
                        *qual_enum = bcmFieldDropReasonSplitHorizonDrop;
                        break;
                    case 135:
                        *qual_enum = bcmFieldDropReasonSpanningTreeDisableDrop;
                        break;
                    case 136:
                        *qual_enum = bcmFieldDropReasonSpanningTreeBlockDrop;
                        break;
                    case 137:
                        *qual_enum = bcmFieldDropReasonEFP;
                        break;
                    case 138:
                        *qual_enum = bcmFieldDropReasonEgrMeterDrop;
                        break;
                    case 139:
                        *qual_enum = bcmFieldDropReasonEgrMtuDrop;
                        break;
                    case 140:
                        *qual_enum = bcmFieldDropReasonEgrVxltDrop;
                        break;
                    case 141:
                        *qual_enum = bcmFieldDropReasonEgrCellTooLargeDrop;
                        break;
                    case 142:
                        *qual_enum = bcmFieldDropReasonEgrCellTooSmallDrop;
                        break;
                    case 143:
                        *qual_enum = bcmFieldDropReasonQosRemarkingDrop;
                        break;
                    case 144:
                        *qual_enum = bcmFieldDropReasonSvpEqualDvpDrop;
                        break;
                    case 145:
                        *qual_enum = bcmFieldDropReasonInvalid1588Pkt;
                        break;
                    case 146:
                        *qual_enum = bcmFieldDropReasonFlexEditorDrop;
                        break;
                    case 147:
                        *qual_enum = bcmFieldDropReasonEgrRecircleHdrDrop;
                        break;
                    case 148:
                        *qual_enum = bcmFieldDropReasonIfaMdDeleteDrop;
                        break;
                    case 1:
                        *qual_enum = bcmFieldDropReasonIngPortGroupLimit;
                        break;
                    case 2:
                        *qual_enum = bcmFieldDropReasonIngPortServicePoolLimit;
                        break;
                    case 4:
                        *qual_enum = bcmFieldDropReasonIngHeadroomPoolLimit;
                        break;
                    case 5:
                        *qual_enum = bcmFieldDropReasonEgrQueueLimit;
                        break;
                    case 6:
                        *qual_enum = bcmFieldDropReasonEgrPortServicePoolLimit;
                        break;
                    case 7:
                        *qual_enum = bcmFieldDropReasonWREDCheck;
                        break;
                    case 42:
                        *qual_enum = bcmFieldDropReasonIngRecircleHdrDrop;
                        break;
                    case 43:
                        *qual_enum = bcmFieldDropReasonMtopIpv4Gateway;
                        break;
                    default:
                        SHR_ERR_EXIT(SHR_E_UNAVAIL);
                }
            }
            break;

        default:
            SHR_ERR_EXIT(SHR_E_NONE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief field driver function variable for bcm56780_a0 dna_2_4_13 device.
 */
static mbcm_ltsw_field_drv_t bcm56780_a0_dna_2_4_13_ltsw_field_drv = {
    .field_stage_db_init = bcm56780_a0_dna_2_4_13_ltsw_field_stage_db_init,
    .field_control_lt_info_init =
                    bcm56780_a0_dna_2_4_13_ltsw_field_control_lt_info_init,
    .field_auto_enum_to_lt_enum_value_get =
          bcm56780_a0_dna_2_4_13_ltsw_field_auto_enum_to_lt_enum_value_get,
    .field_auto_lt_enum_value_to_enum_get =
          bcm56780_a0_dna_2_4_13_ltsw_field_auto_lt_enum_value_to_enum_get,
    .field_manual_enum_to_lt_enum_value_get =
        bcm56780_a0_dna_2_4_13_ltsw_field_manual_enum_to_lt_enum_value_get,
    .field_manual_lt_enum_value_to_enum_get =
        bcm56780_a0_dna_2_4_13_ltsw_field_manual_lt_enum_value_to_enum_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_dna_2_4_13_ltsw_field_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv_set(unit, &bcm56780_a0_dna_2_4_13_ltsw_field_drv));

exit:
    SHR_FUNC_EXIT();
}

