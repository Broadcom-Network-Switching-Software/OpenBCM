/*! \file bcm56990_a0_field.c
 *
 * BCM56990_A0 field subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/xgs/field.h>
#include <bcm_int/ltsw/chip/bcm56990_a0/field_lt_map.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcmltd/chip/bcmltd_str.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

#define BCMI_TH4_IPBM_INDEX_PER_PIPE 64

static
bcmint_field_compress_tbls_info_t bcm56990_a0_compress_tbls_info = {
    .direct_mapped_ttl_zero_sid = "FP_COMPRESSION_IP_TTL",
    .direct_val_ttl_zero_fid = "TTL",
    .direct_map_val_ttl_zero_fid = "COMPRESSED_IP_TTL_0",
    .direct_mapped_ttl_one_sid = "FP_COMPRESSION_IP_TTL",
    .direct_val_ttl_one_fid = "TTL",
    .direct_map_val_ttl_one_fid = "COMPRESSED_IP_TTL_1",
    .direct_mapped_tos_zero_sid = "FP_COMPRESSION_IP_TOS",
    .direct_val_tos_zero_fid = "TOS",
    .direct_map_val_tos_zero_fid = "COMPRESSED_IP_TOS_0",
    .direct_mapped_tos_one_sid = "FP_COMPRESSION_IP_TOS",
    .direct_val_tos_one_fid = "TOS",
    .direct_map_val_tos_one_fid = "COMPRESSED_IP_TOS_1",
    .direct_mapped_tcp_zero_sid = "FP_COMPRESSION_TCP_FLAGS",
    .direct_val_tcp_zero_fid = "TCP_FLAGS",
    .direct_map_val_tcp_zero_fid = "COMPRESSED_TCP_FLAGS_0",
    .direct_mapped_tcp_one_sid = "FP_COMPRESSION_TCP_FLAGS",
    .direct_val_tcp_one_fid = "TCP_FLAGS",
    .direct_map_val_tcp_one_fid = "COMPRESSED_TCP_FLAGS_1",
    .direct_mapped_tunnelttl_sid = "FP_COMPRESSION_TNL_IP_TTL",
    .direct_val_tunnelttl_fid = "TTL",
    .direct_map_val_tunnelttl_fid = "COMPRESSED_TTL",
    .direct_mapped_ipprotocol_sid = "FP_COMPRESSION_IP_PROTOCOL",
    .direct_val_ipprotocol_fid = "IP_PROTOCOL",
    .direct_map_val_ipprotocol_fid = "COMPRESSED_IP_PROTOCOL",
    .index_mapped_ethertype_sid = "FP_COMPRESSION_ETHERTYPE",
    .index_key_ethertype_fid = "FP_COMPRESSION_ETHERTYPE_ID",
    .index_val_ethertype_fid = "ETHERTYPE",
    .index_map_val_ethertype_fid = "COMPRESSED_ETHERTYPE",
    .pipe_fid = "PIPE",
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid = "FP_COMPRESSION_OPERMODE_PIPEUNIQUE",
};

bcmint_field_tbls_info_t bcm56990_a0_ifp_tbls_info = {
    .group_sid = "FP_ING_GRP_TEMPLATE",
    .group_key_fid = "FP_ING_GRP_TEMPLATE_ID",
    .rule_sid = "FP_ING_RULE_TEMPLATE",
    .rule_key_fid = "FP_ING_RULE_TEMPLATE_ID",
    .entry_rule_key_fid = "FP_ING_RULE_TEMPLATE_ID",
    .policy_sid = "FP_ING_POLICY_TEMPLATE",
    .policy_key_fid = "FP_ING_POLICY_TEMPLATE_ID",
    .entry_policy_key_fid = "FP_ING_POLICY_TEMPLATE_ID",
    .entry_sid = "FP_ING_ENTRY",
    .entry_key_fid = "FP_ING_ENTRY_ID",
    .pdd_sid = NULL,
    .pdd_key_fid = NULL,
    .sbr_sid = NULL,
    .sbr_key_fid = NULL,
    .presel_entry_sid = "FP_ING_PRESEL_ENTRY_TEMPLATE",
    .presel_entry_key_fid = "FP_ING_PRESEL_ENTRY_TEMPLATE_ID",
    .grp_presel_entry_key_fid = "FP_ING_PRESEL_ENTRY_TEMPLATE_ID",
    .presel_entry_count_fid = "FP_ING_PRESEL_ENTRY_COUNT",
    .presel_group_sid = NULL,
    .presel_group_key_fid = NULL,
    .ctr_sid = "FP_ING_ENTRY",
    .ctr_key_fid = "CTR_ING_EFLEX_ACTION",
    .meter_sid = "METER_ING_FP_TEMPLATE",
    .meter_key_fid = "METER_ING_FP_TEMPLATE_ID",
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid = "FP_ING_OPERMODE",
    .flex_ctr_idx0_fid = "CTR_ING_EFLEX_OBJECT",
    .flex_ctr_idx1_fid = NULL,
    .grp_oper_mode_fid = "MODE_OPER",
    .src_class_mode_sid = "FP_ING_SRC_CLASS_MODE",
    .src_class_mode_key_fid = "SRC_CLASS_MODE",
    .group_info_partition_sid = "FP_ING_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_partition_key_fid = "FP_ING_GRP_TEMPLATE_ID",
    .group_info_common_partition_key_fid = "PARTITION_ID",
    .group_info_slice_fid = "SLICE_ID",
    .group_info_sid = "FP_ING_GRP_TEMPLATE_INFO",
    .group_info_key_fid = "FP_ING_GRP_TEMPLATE_ID",
    .group_info_num_parts_fid = "NUM_PARTITION_ID",
    .group_info_entries_created_fid = "NUM_ENTRIES_CREATED",
    .group_info_entries_tentative_fid = "NUM_ENTRIES_TENTATIVE",
    .entry_enable_fid = "ENABLE",
    .entry_priority_fid = "ENTRY_PRIORITY",
    .slice_info_sid = "FP_ING_SLICE_INFO",
    .slice_info_key_fid = "FP_ING_SLICE_ID",
    .slice_info_pipe_fid = "PIPE",
    .slice_info_entry_in_use_fid = "ENTRY_INUSE_CNT",
    .slice_info_entry_max_fid = "ENTRY_MAXIMUM",
    .stage_info_sid = "FP_ING_INFO",
    .stage_info_num_slices_fid = "NUM_SLICES",
    .stage_info_num_entries_fid = "NUM_ENTRIES",
    .group_info_act_res_fid = "HW_LTID",
    .meter_info_sid = "METER_ING_FP_DEVICE_INFO",
    .meter_info_num_meters_fid = "NUM_METERS",
    .meter_info_num_meters_per_pipe_fid = "NUM_METERS_PER_PIPE",
    .presel_entry_prio_fid = "ENTRY_PRIORITY",
    .action_info_cpu_match_id_fid = "ACTION_MATCH_ID",
    .action_l3_switch_nhop_fid = "ACTION_SWITCH_TO_L3UC",
    .action_l3_switch_ecmp_fid = "ACTION_SWITCH_TO_ECMP",
    .action_redirect_egr_nhop_fid = "ACTION_REDIRECT_TO_NHOP",
    .action_redirect_egr_ecmp_fid = "ACTION_REDIRECT_TO_ECMP",
    .redirect_profile_sid = "FP_ING_ADD_REDIRECT_DATA",
    .redirect_profile_key_fid = "FP_ING_ADD_REDIRECT_DATA_ID",
    .redirect_profile_ports_fid = "REDIRECT_PORTS",
    .egress_mask_profile_sid = "FP_ING_REMOVE_REDIRECT_DATA",
    .egress_mask_profile_key_fid = "FP_ING_REMOVE_REDIRECT_DATA_ID",
    .egress_mask_profile_ports_fid = "REDIRECT_PORTS",
    .delayed_redirect_profile_sid = "FP_DELAYED_REDIRECT",
    .delayed_redirect_profile_key_fid = "FP_DELAYED_REDIRECT_ID",
    .delayed_redirect_profile_ports_fid = "EGR_PORT_ID",
    .delayed_redirect_profile_elephant_pkts_fid = "ELEPHANT_PKT",
    .delayed_drop_profile_sid = "FP_DELAYED_DROP",
    .delayed_drop_profile_key_fid = "FP_DELAYED_DROP_ID",
    .delayed_drop_profile_ports_fid = "EGR_PORT_ID",
    .delayed_drop_profile_elephant_pkts_fid = "ELEPHANT_PKT",
    .action_mirror_cont_id_fid = "ACTION_MIRROR_INSTANCE_ID",
    .action_unmod_redirect_to_port_fid = "ACTION_UNMODIFIED_REDIRECT_TO_PORT",
    .action_unmod_redirect_to_trunk_fid = "ACTION_UNMODIFIED_REDIRECT_TO_TRUNK",
    .action_mirror_cont_enable_fid = "ACTION_MIRROR_INSTANCE_ENABLE",
    .compression_type_fid = "COMPRESSION_TYPE",
    .compression_type_cnt_fid = "NUM_COMPRESSION_TYPE",
};

bcmint_field_tbls_info_t bcm56990_a0_efp_tbls_info = {
    .group_sid = "FP_EGR_GRP_TEMPLATE",
    .group_key_fid = "FP_EGR_GRP_TEMPLATE_ID",
    .rule_sid = "FP_EGR_RULE_TEMPLATE",
    .rule_key_fid = "FP_EGR_RULE_TEMPLATE_ID",
    .entry_rule_key_fid = "FP_EGR_RULE_TEMPLATE_ID",
    .policy_sid = "FP_EGR_POLICY_TEMPLATE",
    .policy_key_fid = "FP_EGR_POLICY_TEMPLATE_ID",
    .entry_policy_key_fid = "FP_EGR_POLICY_TEMPLATE_ID",
    .entry_sid = "FP_EGR_ENTRY",
    .entry_key_fid = "FP_EGR_ENTRY_ID",
    .pdd_sid = NULL,
    .pdd_key_fid = NULL,
    .sbr_sid = NULL,
    .sbr_key_fid = NULL,
    .presel_entry_sid = NULL,
    .presel_entry_key_fid = NULL,
    .grp_presel_entry_key_fid = NULL,
    .presel_entry_count_fid = NULL,
    .presel_group_sid = NULL,
    .presel_group_key_fid = NULL,
    .ctr_sid = "FP_EGR_ENTRY",
    .ctr_key_fid = "CTR_EGR_EFLEX_ACTION",
    .meter_sid = "METER_EGR_FP_TEMPLATE",
    .meter_key_fid = "METER_EGR_FP_TEMPLATE_ID",
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid= "FP_EGR_OPERMODE_PIPEUNIQUE",
    .flex_ctr_idx0_fid = "CTR_EGR_EFLEX_OBJECT",
    .flex_ctr_idx1_fid = NULL,
    .grp_oper_mode_fid = "MODE_OPER",
    .entry_enable_fid = "ENABLE",
    .entry_priority_fid = "ENTRY_PRIORITY",
    .group_info_partition_sid = "FP_EGR_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_partition_key_fid = "FP_EGR_GRP_TEMPLATE_ID",
    .group_info_common_partition_key_fid = "PARTITION_ID",
    .group_info_slice_fid = "VIRTUAL_SLICE_ID",
    .group_info_sid = "FP_EGR_GRP_TEMPLATE_INFO",
    .group_info_key_fid = "FP_EGR_GRP_TEMPLATE_ID",
    .group_info_num_parts_fid = "NUM_PARTITION_ID",
    .group_info_entries_created_fid = "NUM_ENTRIES_CREATED",
    .group_info_entries_tentative_fid = "NUM_ENTRIES_TENTATIVE",
    .entry_enable_fid = "ENABLE",
    .slice_info_sid = "FP_EGR_SLICE_INFO",
    .slice_info_key_fid = "FP_EGR_SLICE_ID",
    .slice_info_pipe_fid = "PIPE",
    .slice_info_entry_in_use_fid = "ENTRY_INUSE_CNT",
    .slice_info_entry_max_fid = "ENTRY_MAXIMUM",
    .stage_info_sid = "FP_EGR_INFO",
    .stage_info_num_slices_fid = "NUM_SLICES",
    .stage_info_num_entries_fid = "NUM_ENTRIES",
    .group_info_act_res_fid = "HW_LTID",
    .meter_info_sid = NULL,
    .presel_entry_prio_fid = NULL,
    .action_info_cpu_match_id_fid = NULL,
};

bcmint_field_tbls_info_t bcm56990_a0_vfp_tbls_info = {
    .group_sid = "FP_VLAN_GRP_TEMPLATE",
    .group_key_fid = "FP_VLAN_GRP_TEMPLATE_ID",
    .rule_sid = "FP_VLAN_RULE_TEMPLATE",
    .rule_key_fid = "FP_VLAN_RULE_TEMPLATE_ID",
    .entry_rule_key_fid = "FP_VLAN_RULE_TEMPLATE_ID",
    .policy_sid = "FP_VLAN_POLICY_TEMPLATE",
    .policy_key_fid = "FP_VLAN_POLICY_TEMPLATE_ID",
    .entry_policy_key_fid = "FP_VLAN_POLICY_TEMPLATE_ID",
    .entry_sid = "FP_VLAN_ENTRY",
    .entry_key_fid = "FP_VLAN_ENTRY_ID",
    .pdd_sid = NULL,
    .pdd_key_fid = NULL,
    .sbr_sid = NULL,
    .sbr_key_fid = NULL,
    .presel_entry_sid = NULL,
    .presel_entry_key_fid = NULL,
    .grp_presel_entry_key_fid = NULL,
    .presel_entry_count_fid = NULL,
    .presel_group_sid = NULL,
    .presel_group_key_fid = NULL,
    .ctr_sid = "FP_VLAN_ENTRY",
    .ctr_key_fid = "CTR_ING_EFLEX_ACTION",
    .meter_sid = NULL,
    .meter_key_fid = NULL,
    .oper_mode_sid = "FP_CONFIG",
    .oper_mode_fid = "FP_VLAN_OPERMODE_PIPEUNIQUE",
    .flex_ctr_idx0_fid = "CTR_ING_EFLEX_OBJECT",
    .flex_ctr_idx1_fid = NULL,
    .grp_oper_mode_fid = "MODE_OPER",
    .entry_enable_fid = "ENABLE",
    .entry_priority_fid = "ENTRY_PRIORITY",
    .group_info_partition_sid = "FP_VLAN_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_partition_key_fid = "FP_VLAN_GRP_TEMPLATE_ID",
    .group_info_common_partition_key_fid = "PARTITION_ID",
    .group_info_slice_fid = "VIRTUAL_SLICE_ID",
    .group_info_sid = "FP_VLAN_GRP_TEMPLATE_INFO",
    .group_info_key_fid = "FP_VLAN_GRP_TEMPLATE_ID",
    .group_info_num_parts_fid = "NUM_PARTITION_ID",
    .group_info_entries_created_fid = "NUM_ENTRIES_CREATED",
    .group_info_entries_tentative_fid = "NUM_ENTRIES_TENTATIVE",
    .entry_enable_fid = "ENABLE",
    .slice_info_sid = "FP_VLAN_SLICE_INFO",
    .slice_info_key_fid = "FP_VLAN_SLICE_ID",
    .slice_info_pipe_fid = "PIPE",
    .slice_info_entry_in_use_fid = "ENTRY_INUSE_CNT",
    .slice_info_entry_max_fid = "ENTRY_MAXIMUM",
    .stage_info_sid = "FP_VLAN_INFO",
    .stage_info_num_slices_fid = "NUM_SLICES",
    .stage_info_num_entries_fid = "NUM_ENTRIES",
    .group_info_act_res_fid = "HW_LTID",
    .meter_info_sid = NULL,
    .presel_entry_prio_fid = NULL,
    .action_info_cpu_match_id_fid = "ACTION_MATCH_ID",
};

bcmint_field_tbls_info_t bcm56990_a0_em_tbls_info = {
    .group_sid = "FP_EM_GRP_TEMPLATE",
    .group_key_fid = "FP_EM_GRP_TEMPLATE_ID",
    .rule_sid = "FP_EM_RULE_TEMPLATE",
    .rule_key_fid = "FP_EM_RULE_TEMPLATE_ID",
    .entry_rule_key_fid = "FP_EM_RULE_TEMPLATE_ID",
    .policy_sid = "FP_EM_POLICY_TEMPLATE",
    .policy_key_fid = "FP_EM_POLICY_TEMPLATE_ID",
    .entry_policy_key_fid = "FP_EM_POLICY_TEMPLATE_ID",
    .entry_sid = "FP_EM_ENTRY",
    .entry_key_fid = "FP_EM_ENTRY_ID",
    .pdd_sid = "FP_EM_PDD_TEMPLATE",
    .pdd_key_fid = "FP_EM_PDD_TEMPLATE_ID",
    .sbr_sid = NULL,
    .sbr_key_fid = NULL,
    .presel_entry_sid = "FP_EM_PRESEL_ENTRY_TEMPLATE",
    .presel_entry_key_fid = "FP_EM_PRESEL_ENTRY_TEMPLATE_ID",
    .grp_presel_entry_key_fid = "FP_EM_PRESEL_ENTRY_TEMPLATE_ID",
    .presel_entry_count_fid = "FP_EM_PRESEL_ENTRY_COUNT",
    .presel_group_sid = NULL,
    .presel_group_key_fid = NULL,
    .ctr_sid = "FP_EM_ENTRY",
    .ctr_key_fid = "CTR_ING_EFLEX_ACTION",
    .meter_sid = "METER_ING_FP_TEMPLATE",
    .meter_key_fid = "METER_ING_FP_TEMPLATE_ID",
    .flex_ctr_idx0_fid = "CTR_ING_EFLEX_OBJECT",
    .flex_ctr_idx1_fid = NULL,
    .grp_oper_mode_fid = "MODE_OPER",
    .src_class_mode_sid = "FP_EM_SRC_CLASS_MODE",
    .src_class_mode_key_fid = "SRC_CLASS_MODE",
    .group_info_partition_sid = "FP_EM_GRP_TEMPLATE_PARTITION_INFO",
    .group_info_partition_key_fid = "FP_EM_GRP_TEMPLATE_ID",
    .group_info_common_partition_key_fid = "PARTITION_ID",
    .group_info_slice_fid = "SLICE_ID",
    .entry_enable_fid = "ENABLE",
    .entry_priority_fid = NULL,
    .stage_info_sid = "FP_EM_INFO",
    .stage_info_num_entries_fid = "NUM_ENTRIES",
    .group_info_act_res_fid = "HW_LTID",
    .meter_info_sid = "METER_ING_FP_DEVICE_INFO",
    .meter_info_num_meters_fid = "NUM_METERS",
    .meter_info_num_meters_per_pipe_fid = "NUM_METERS_PER_PIPE",
    .presel_entry_prio_fid = "ENTRY_PRIORITY",
    .action_info_cpu_match_id_fid = "ACTION_MATCH_ID",
    .action_l3_switch_nhop_fid = "ACTION_SWITCH_TO_L3UC",
    .action_l3_switch_ecmp_fid = "ACTION_SWITCH_TO_ECMP",
    .action_redirect_egr_nhop_fid = "ACTION_REDIRECT_TO_NHOP",
    .action_redirect_egr_ecmp_fid = "ACTION_REDIRECT_TO_ECMP",
    .redirect_profile_sid = "FP_ING_ADD_REDIRECT_DATA",
    .redirect_profile_key_fid = "FP_ING_ADD_REDIRECT_DATA_ID",
    .redirect_profile_ports_fid = "REDIRECT_PORTS",
    .egress_mask_profile_sid = "FP_ING_REMOVE_REDIRECT_DATA",
    .egress_mask_profile_key_fid = "FP_ING_REMOVE_REDIRECT_DATA_ID",
    .egress_mask_profile_ports_fid = "REDIRECT_PORTS",
    .delayed_redirect_profile_sid = "FP_DELAYED_REDIRECT",
    .delayed_redirect_profile_key_fid = "FP_DELAYED_REDIRECT_ID",
    .delayed_redirect_profile_ports_fid = "EGR_PORT_ID",
    .delayed_redirect_profile_elephant_pkts_fid = "ELEPHANT_PKT",
    .delayed_drop_profile_sid = "FP_DELAYED_DROP",
    .delayed_drop_profile_key_fid = "FP_DELAYED_DROP_ID",
    .delayed_drop_profile_ports_fid = "EGR_PORT_ID",
    .delayed_drop_profile_elephant_pkts_fid = "ELEPHANT_PKT",
    .action_mirror_cont_id_fid = "ACTION_MIRROR_INSTANCE_ID",
    .action_unmod_redirect_to_port_fid = "ACTION_UNMODIFIED_REDIRECT_TO_PORT",
    .action_unmod_redirect_to_trunk_fid = "ACTION_UNMODIFIED_REDIRECT_TO_TRUNK",
    .action_mirror_cont_enable_fid = "ACTION_MIRROR_INSTANCE_ENABLE",
    .action_ctr_enable = "ACTION_CTR_ENABLE",
};

static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk0_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {0},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk1_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {1},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk2_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {2},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk3_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {3},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk4_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {4},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk5_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {5},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk6_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {6},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk7_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {7},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk8_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {8},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk9_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {9},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk10_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {10},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk11_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {11},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk12_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {12},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk13_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {13},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk14_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {14},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};
static bcm_field_qual_map_info_t bcm_field_udf_qual_bcmiFieldQualifyB2Chunk15_map[] = {
    {
        .lt_field_name = "QUAL_UDF_CHUNKS",
        .num_offsets = 1,
        .offset = {15},
        .width = {16},
        .em = BCMI_FIELD_STAGE_LTMAP_ALL,
    },
};

bcm_field_qual_map_t bcm56990_a0_udf_qual_data[] = {
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
        .qual = bcmiFieldQualifyB2Chunk11,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk11_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk12,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk12_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk13,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk13_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk14,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk14_map,
    },
    {
        .qual = bcmiFieldQualifyB2Chunk15,
        .num_maps = 1,
        .map = bcm_field_udf_qual_bcmiFieldQualifyB2Chunk15_map,
    },
};

static int
bcm56990_a0_ltsw_efp_group_mode_port_pkt_type_init(
        int unit,
        bcmint_field_stage_info_t *stage_info)
{
    bcmint_field_efp_key_qual_map_t *efp_key_qual_map;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_PARAM);

    efp_key_qual_map = &stage_info->efp_key_qual_map[0];
    efp_key_qual_map->grp_mode = "L2_SINGLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_ANY";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL2Single;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeDefault;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyCpuQueue);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);

    efp_key_qual_map = &stage_info->efp_key_qual_map[1];
    efp_key_qual_map->grp_mode = "L3_ANY_SINGLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_IP";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3SingleAny;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeIp;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpFrag);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTcpControl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4SrcPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIcmpTypeCode);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTtl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTos);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);

    efp_key_qual_map = &stage_info->efp_key_qual_map[2];
    efp_key_qual_map->grp_mode = "L3_ANY_SINGLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_NONIP";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3SingleAny;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeNonIp;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyCpuQueue);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);

    efp_key_qual_map = &stage_info->efp_key_qual_map[3];
    efp_key_qual_map->grp_mode = "L3_SINGLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_IPV4";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3Single;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeIp4;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpFrag);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTcpControl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4SrcPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIcmpTypeCode);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTtl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTos);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);

    efp_key_qual_map = &stage_info->efp_key_qual_map[4];
    efp_key_qual_map->grp_mode = "L3_SINGLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_IPV6";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3Single;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeIp6;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp6);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp6);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp6High);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp6High);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTos);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);

    efp_key_qual_map = &stage_info->efp_key_qual_map[5];
    efp_key_qual_map->grp_mode = "L3_SINGLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_NONIP";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3Single;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeNonIp;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyCpuQueue);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);

    efp_key_qual_map = &stage_info->efp_key_qual_map[6];
    efp_key_qual_map->grp_mode = "L3_DOUBLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_IPV4";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3Double;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeIp4;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyCpuQueue);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpFrag);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTcpControl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4SrcPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIcmpTypeCode);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTtl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTos);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);

    efp_key_qual_map = &stage_info->efp_key_qual_map[7];
    efp_key_qual_map->grp_mode = "L3_DOUBLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_IPV6";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3Double;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeIp6;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp6);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp6);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp6High);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp6High);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTos);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpFrag);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTcpControl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4SrcPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIcmpTypeCode);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTtl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyExtensionHeaderSubCode);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyExtensionHeaderType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);

    efp_key_qual_map = &stage_info->efp_key_qual_map[8];
    efp_key_qual_map->grp_mode = "L3_DOUBLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_NONIP";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3Double;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeNonIp;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyCpuQueue);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2PayLoad);


    efp_key_qual_map = &stage_info->efp_key_qual_map[9];
    efp_key_qual_map->grp_mode = "L3_ANY_DOUBLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_HIGIG_PACKET_ANY";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3DoubleAny;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeHigig;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyCpuQueue);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);

    efp_key_qual_map = &stage_info->efp_key_qual_map[10];
    efp_key_qual_map->grp_mode = "L3_ANY_DOUBLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_FRONT_PACKET_ANY";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3DoubleAny;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeFrontPanel;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyCpuQueue);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpFrag);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTcpControl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4SrcPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIcmpTypeCode);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTtl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTos);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);

    efp_key_qual_map = &stage_info->efp_key_qual_map[11];
    efp_key_qual_map->grp_mode = "L3_ALT_DOUBLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_IPV4";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3DoubleAlternate;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeIp4;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyCpuQueue);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpFrag);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTcpControl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4SrcPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIcmpTypeCode);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTtl);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTos);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);

    efp_key_qual_map = &stage_info->efp_key_qual_map[12];
    efp_key_qual_map->grp_mode = "L3_ALT_DOUBLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_IPV6";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3DoubleAlternate;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeIp6;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyCpuQueue);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp6);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp6);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcIp6High);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstIp6High);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyTos);

    efp_key_qual_map = &stage_info->efp_key_qual_map[13];
    efp_key_qual_map->grp_mode = "L3_ALT_DOUBLE_WIDE";
    efp_key_qual_map->port_pkt_type = "PORT_ANY_PACKET_NONIP";
    efp_key_qual_map->mode_type = bcmFieldGroupModeTypeL3DoubleAlternate;
    efp_key_qual_map->packet_type = bcmFieldGroupPacketTypeNonIp;
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL4Ports);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClass);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEgressClassL3Interface);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyColor);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2Format);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL3Routable);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyMirrorCopy);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlan);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanCfi);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOuterVlanPri);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVlanFormat);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIngressInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyInterfaceClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL2);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifySrcClassL3 );
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDstClassL3);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyOutPort);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyCpuQueue);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIpType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingVlanId);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVrf);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyVpn);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyIntCongestionNotification);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyDrop);
    BCM_FIELD_QSET_ADD(efp_key_qual_map->key_qset, bcmFieldQualifyL2PayLoad);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_stage_info_init(int unit,
                                       bcmint_field_stage_info_t *stage_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_info, SHR_E_PARAM);

    if (stage_info->stage_id == bcmiFieldStageIngress) {
        /* Initialise stage flags */
        stage_info->flags |= BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL;
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_SUPPORTS_COLORED_ACTIONS;
        stage_info->flags |= BCMINT_FIELD_STAGE_LEGACY_POLICER_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_PROFILE_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_COMPRESSION_SUPPORTED;
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO;

        /* Initialise lt table info */
        stage_info->tbls_info = &bcm56990_a0_ifp_tbls_info;

        /* Initialise qual db counts */
        stage_info->qual_db_count = (sizeof(bcm56990_a0_ifp_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->group_qual_db_count = (sizeof(bcm56990_a0_ifp_group_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->udf_qual_db_count = (sizeof(bcm56990_a0_udf_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->presel_qual_db_count =
                                   (sizeof(bcm56990_a0_ifp_presel_qual_data)) /
                                   (sizeof(bcm_field_qual_map_t));
        stage_info->action_db_count = (sizeof(bcm56990_a0_ifp_action_data)) /
                                            (sizeof(bcm_field_action_map_t));

        /* Initialise lt map db */
        stage_info->qual_lt_map_db = bcm56990_a0_ifp_qual_data;

        stage_info->group_qual_lt_map_db = bcm56990_a0_ifp_group_qual_data;

        stage_info->presel_qual_lt_map_db = bcm56990_a0_ifp_presel_qual_data;

        stage_info->udf_qual_lt_map_db = bcm56990_a0_udf_qual_data;

        stage_info->action_lt_map_db = bcm56990_a0_ifp_action_data;

    } else if (stage_info->stage_id == bcmiFieldStageEgress) {

        /* Initialise stage flags */
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_SUPPORTS_COLORED_ACTIONS;
        stage_info->flags |= BCMINT_FIELD_STAGE_KEY_TYPE_FIXED;
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO;

        /* Initialise lt table info */
        stage_info->tbls_info = &bcm56990_a0_efp_tbls_info;

        /* Initialise qual db counts */
        stage_info->qual_db_count = (sizeof(bcm56990_a0_efp_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->group_qual_db_count = (sizeof(bcm56990_a0_efp_group_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->presel_qual_db_count =
                                   (sizeof(bcm56990_a0_efp_presel_qual_data)) /
                                                 (sizeof(bcm_field_qual_map_t));
        stage_info->action_db_count = (sizeof(bcm56990_a0_efp_action_data)) /
                                            (sizeof(bcm_field_action_map_t));

        /* Initialise lt map db */
        stage_info->qual_lt_map_db = bcm56990_a0_efp_qual_data;

        stage_info->group_qual_lt_map_db = bcm56990_a0_efp_group_qual_data;

        stage_info->presel_qual_lt_map_db = bcm56990_a0_efp_presel_qual_data;

        stage_info->action_lt_map_db = bcm56990_a0_efp_action_data;

        SHR_IF_ERR_EXIT
            (bcm56990_a0_ltsw_efp_group_mode_port_pkt_type_init(unit, stage_info));
    } else if (stage_info->stage_id == bcmiFieldStageVlan) {

        /* Initialise stage flags */
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_SUPPORTS_COLORED_ACTIONS;
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO;

        /* Initialise lt table info */
        stage_info->tbls_info = &bcm56990_a0_vfp_tbls_info;

        /* Initialise qual db counts */
        stage_info->qual_db_count = (sizeof(bcm56990_a0_vfp_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->group_qual_db_count = (sizeof(bcm56990_a0_vfp_group_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->udf_qual_db_count = (sizeof(bcm56990_a0_udf_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->presel_qual_db_count =
                                    (sizeof(bcm56990_a0_vfp_presel_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->action_db_count = (sizeof(bcm56990_a0_vfp_action_data)) /
                                               (sizeof(bcm_field_action_map_t));

        /* Initialise lt map db */
        stage_info->qual_lt_map_db = bcm56990_a0_vfp_qual_data;

        stage_info->group_qual_lt_map_db = bcm56990_a0_vfp_group_qual_data;

        stage_info->presel_qual_lt_map_db = bcm56990_a0_vfp_presel_qual_data;

        stage_info->udf_qual_lt_map_db = bcm56990_a0_udf_qual_data;

        stage_info->action_lt_map_db = bcm56990_a0_vfp_action_data;

    }   if (stage_info->stage_id == bcmiFieldStageExactMatch) {
        /* Initialise stage flags */
        stage_info->flags |= BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL;
        stage_info->flags |= BCMINT_FIELD_STAGE_SUPPORTS_COLORED_ACTIONS;
        stage_info->flags |= BCMINT_FIELD_STAGE_LEGACY_POLICER_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_POLICY_TYPE_PDD;
        stage_info->flags |= BCMINT_FIELD_STAGE_DEFAULT_ENTRY_SUPPORT;
        stage_info->flags |= BCMINT_FIELD_STAGE_PIPE_MODE_NOT_SUPPORTED;

        /* Initialise lt table info */
        stage_info->tbls_info = &bcm56990_a0_em_tbls_info;

        /* Initialise qual db counts */
        stage_info->qual_db_count = (sizeof(bcm56990_a0_ifp_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->group_qual_db_count = (sizeof(bcm56990_a0_ifp_group_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->udf_qual_db_count = (sizeof(bcm56990_a0_udf_qual_data)) /
                                            (sizeof(bcm_field_qual_map_t));
        stage_info->presel_qual_db_count =
                                   (sizeof(bcm56990_a0_ifp_presel_qual_data)) /
                                   (sizeof(bcm_field_qual_map_t));
        stage_info->action_db_count = (sizeof(bcm56990_a0_ifp_action_data)) /
                                            (sizeof(bcm_field_action_map_t));

        /* Initialise lt map db */
        stage_info->qual_lt_map_db = bcm56990_a0_ifp_qual_data;

        stage_info->group_qual_lt_map_db = bcm56990_a0_ifp_group_qual_data;

        stage_info->presel_qual_lt_map_db = bcm56990_a0_ifp_presel_qual_data;

        stage_info->udf_qual_lt_map_db = bcm56990_a0_udf_qual_data;

        stage_info->action_lt_map_db = bcm56990_a0_ifp_action_data;

    } else {
        /* Do nothing */
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_field_control_lt_info_init(
        int unit,
        bcmint_field_control_info_t *control_info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(control_info, SHR_E_PARAM);

    control_info->comp_tbls_info = &bcm56990_a0_compress_tbls_info;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Field sub driver functions for bcm56990_a0.
 */
static mbcm_ltsw_field_drv_t bcm56990_a0_field_sub_drv = {
    .field_stage_info_init = bcm56990_a0_ltsw_field_stage_info_init,
    .field_control_lt_info_init = bcm56990_a0_ltsw_field_control_lt_info_init,
    .field_auto_enum_to_lt_enum_value_get =
        bcm56990_a0_ltsw_field_auto_enum_to_lt_enum_value_get,
    .field_auto_lt_enum_value_to_enum_get =
        bcm56990_a0_ltsw_field_auto_lt_enum_value_to_enum_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_field_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_drv_set(unit, &bcm56990_a0_field_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
