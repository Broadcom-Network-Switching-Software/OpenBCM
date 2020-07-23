/*! \file bcm56990_b0_fp_ing_tbls.c
 *
 * API to initialize ingress FP stage logical tables and
 * some FIDs in each of those logical table for
 * Tomahawk-4 B0(56990_B0).
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
#include <bcmfp/bcmfp_tbl_internal.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

bcmfp_tbl_qualifier_fid_info_t
bcm56990_b0_ingress_group_qualifiers_fid_info[FP_ING_GRP_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_qualifier_fid_info_t
bcm56990_b0_ingress_rule_qualifiers_fid_info[FP_ING_RULE_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_action_fid_info_t
bcm56990_b0_ingress_policy_actions_fid_info[FP_ING_POLICY_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_qualifier_fid_info_t
bcm56990_b0_ingress_pse_qualifiers_fid_info
                          [FP_ING_PRESEL_ENTRY_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_fid_remote_info_t bcm56990_b0_compress_remote_sid_info[4];
bcmfp_tbl_fid_remote_maps_t bcm56990_b0_compress_ipv4_dst_remote_maps[9];
bcmfp_tbl_fid_remote_maps_t bcm56990_b0_compress_ipv4_src_remote_maps[9];
bcmfp_tbl_fid_remote_maps_t bcm56990_b0_compress_ipv6_dst_remote_maps[9];
bcmfp_tbl_fid_remote_maps_t bcm56990_b0_compress_ipv6_src_remote_maps[9];
bcmfp_tbl_fid_remote_map_t bcm56990_b0_compress_ipv4_dst_remote_map[10];
bcmfp_tbl_fid_remote_map_t bcm56990_b0_compress_ipv4_src_remote_map[10];
bcmfp_tbl_fid_remote_map_t bcm56990_b0_compress_ipv6_dst_remote_map[9];
bcmfp_tbl_fid_remote_map_t bcm56990_b0_compress_ipv6_src_remote_map[9];

static void bcmfp_bcm56990_b0_ingress_group_qualifiers_map(void);
static void bcmfp_bcm56990_b0_ingress_rule_qualifiers_map(void);
static void bcmfp_bcm56990_b0_ingress_policy_actions_map(void);
static void bcmfp_bcm56990_b0_ingress_pse_qualifiers_map(void);

bcmfp_tbl_group_t bcm56990_b0_ing_group_tbl = {
    .sid = FP_ING_GRP_TEMPLATEt,
    .key_fid = FP_ING_GRP_TEMPLATEt_FP_ING_GRP_TEMPLATE_IDf,
    .pipe_id_fid = FP_ING_GRP_TEMPLATEt_PIPEf,
    .priority_fid = FP_ING_GRP_TEMPLATEt_ENTRY_PRIORITYf,
    .mode_fid = FP_ING_GRP_TEMPLATEt_MODEf,
    .mode_auto_fid = FP_ING_GRP_TEMPLATEt_MODE_AUTOf,
    .mode_oper_fid = FP_ING_GRP_TEMPLATEt_MODE_OPERf,
    .presel_ids_count_fid = FP_ING_GRP_TEMPLATEt_FP_ING_PRESEL_ENTRY_COUNTf,
    .presel_ids_fid = FP_ING_GRP_TEMPLATEt_FP_ING_PRESEL_ENTRY_TEMPLATE_IDf,
    .presel_sid = FP_ING_PRESEL_ENTRY_TEMPLATEt,
    .presel_key_fid =
            FP_ING_PRESEL_ENTRY_TEMPLATEt_FP_ING_PRESEL_ENTRY_TEMPLATE_IDf,
    .presel_mixed_src_class_mask_fid =
            FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASS_MASKf,
    .src_class_sid = FP_ING_SRC_CLASS_MODEt,
    .qual_inports_fid = FP_ING_GRP_TEMPLATEt_QUAL_INPORTS_BITMAPf,
    .qual_system_ports_fid = FP_ING_GRP_TEMPLATEt_QUAL_SYSTEM_PORTS_BITMAPf,
    .qual_device_ports_fid = FP_ING_GRP_TEMPLATEt_QUAL_DEVICE_PORTS_BITMAPf,
    .qualifiers_fid_info = bcm56990_b0_ingress_group_qualifiers_fid_info,
    .enable_fid = FP_ING_GRP_TEMPLATEt_ENABLEf,
    .auto_expand_fid = FP_ING_GRP_TEMPLATEt_AUTO_EXPANDf,
    .compression_type_array_fid = FP_ING_GRP_TEMPLATEt_COMPRESSION_TYPEf,
    .num_compression_type_fid = FP_ING_GRP_TEMPLATEt_NUM_COMPRESSION_TYPEf,
    .fid_count = FP_ING_GRP_TEMPLATEt_FIELD_COUNT,
};

bcmfp_tbl_group_info_t bcm56990_b0_ing_group_info_tbl = {
   .sid = FP_ING_GRP_TEMPLATE_INFOt,
   .key_fid = FP_ING_GRP_TEMPLATE_INFOt_FP_ING_GRP_TEMPLATE_IDf,
   .hw_ltid =  FP_ING_GRP_TEMPLATE_INFOt_HW_LTIDf,
   .num_entries_created =  FP_ING_GRP_TEMPLATE_INFOt_NUM_ENTRIES_CREATEDf,
   .num_entries_tentative = FP_ING_GRP_TEMPLATE_INFOt_NUM_ENTRIES_TENTATIVEf,
   .num_partition_id =  FP_ING_GRP_TEMPLATE_INFOt_NUM_PARTITION_IDf,
   .fid_count = FP_ING_GRP_TEMPLATE_PARTITION_INFOt_FIELD_COUNT,
};

bcmfp_tbl_rule_t bcm56990_b0_ing_rule_tbl = {
    .sid = FP_ING_RULE_TEMPLATEt,
    .key_fid = FP_ING_RULE_TEMPLATEt_FP_ING_RULE_TEMPLATE_IDf,
    .inports_key_fid = FP_ING_RULE_TEMPLATEt_QUAL_INPORTSf,
    .inports_mask_fid = FP_ING_RULE_TEMPLATEt_QUAL_INPORTS_MASKf,
    .system_ports_key_fid = FP_ING_RULE_TEMPLATEt_QUAL_SYSTEM_PORTSf,
    .system_ports_mask_fid = FP_ING_RULE_TEMPLATEt_QUAL_SYSTEM_PORTS_MASKf,
    .device_ports_key_fid = FP_ING_RULE_TEMPLATEt_QUAL_DEVICE_PORTSf,
    .device_ports_mask_fid = FP_ING_RULE_TEMPLATEt_QUAL_DEVICE_PORTS_MASKf,
    .qualifiers_fid_info = bcm56990_b0_ingress_rule_qualifiers_fid_info,
    .compression_type_array_fid = FP_ING_RULE_TEMPLATEt_COMPRESSION_TYPEf,
    .num_compression_type_fid = FP_ING_RULE_TEMPLATEt_NUM_COMPRESSION_TYPEf,
    .fid_count = FP_ING_RULE_TEMPLATEt_FIELD_COUNT,
};

bcmfp_tbl_policy_t bcm56990_b0_ing_policy_tbl = {
    .sid = FP_ING_POLICY_TEMPLATEt,
    .key_fid = FP_ING_POLICY_TEMPLATEt_FP_ING_POLICY_TEMPLATE_IDf,
    .flex_ctr_r_count_fid = FP_ING_POLICY_TEMPLATEt_ACTION_FLEX_CTR_R_COUNTf,
    .flex_ctr_y_count_fid = FP_ING_POLICY_TEMPLATEt_ACTION_FLEX_CTR_Y_COUNTf,
    .flex_ctr_g_count_fid = FP_ING_POLICY_TEMPLATEt_ACTION_FLEX_CTR_G_COUNTf,
    .flex_ctr_object_fid = FP_ING_POLICY_TEMPLATEt_CTR_ING_EFLEX_OBJECTf,
    .flex_ctr_container_id_fid = FP_ING_POLICY_TEMPLATEt_CTR_ING_EFLEX_CONTAINER_IDf,
    .actions_fid_info = bcm56990_b0_ingress_policy_actions_fid_info,
    .fid_count = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT,
};

bcmfp_tbl_pse_t bcm56990_b0_ing_pse_tbl = {
    .sid = FP_ING_PRESEL_ENTRY_TEMPLATEt,
    .key_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_FP_ING_PRESEL_ENTRY_TEMPLATE_IDf,
    .priority_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_ENTRY_PRIORITYf,
    .fid_count = FP_ING_PRESEL_ENTRY_TEMPLATEt_FIELD_COUNT,
    .enable_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_ENABLEf,
    .qualifiers_fid_info = bcm56990_b0_ingress_pse_qualifiers_fid_info,
};

bcmfp_tbl_entry_t bcm56990_b0_ing_entry_tbl = {
    .sid = FP_ING_ENTRYt,
    .key_fid = FP_ING_ENTRYt_FP_ING_ENTRY_IDf,
    .oprtnl_state_fid = FP_ING_ENTRYt_OPERATIONAL_STATEf,
    .priority_fid = FP_ING_ENTRYt_ENTRY_PRIORITYf,
    .group_id_fid = FP_ING_ENTRYt_FP_ING_GRP_TEMPLATE_IDf,
    .rule_id_fid = FP_ING_ENTRYt_FP_ING_RULE_TEMPLATE_IDf,
    .policy_id_fid = FP_ING_ENTRYt_FP_ING_POLICY_TEMPLATE_IDf,
    .meter_id_fid = FP_ING_ENTRYt_METER_ING_FP_TEMPLATE_IDf,
    .flex_ctr_base_idx_fid = FP_ING_ENTRYt_CTR_ING_FLEX_BASE_INDEXf,
    .flex_ctr_offset_mode_fid = FP_ING_ENTRYt_CTR_ING_FLEX_OFFSET_MODEf,
    .flex_ctr_pool_id_fid = FP_ING_ENTRYt_CTR_ING_FLEX_POOL_NUMBERf,
    .flex_ctr_action_fid = FP_ING_ENTRYt_CTR_ING_EFLEX_ACTIONf,
    .enable_fid = FP_ING_ENTRYt_ENABLEf,
    .fid_count = FP_ING_ENTRYt_FIELD_COUNT,
};

bcmfp_tbl_src_class_t bcm56990_b0_ing_src_class_tbl = {
    .sid = FP_ING_SRC_CLASS_MODEt,
    .pipe_id_fid = FP_ING_SRC_CLASS_MODEt_PIPEf,
    .mode_fid = FP_ING_SRC_CLASS_MODEt_SRC_CLASS_MODEf,
    .fid_count = FP_ING_SRC_CLASS_MODEt_FIELD_COUNT,
};

bcmfp_tbl_range_check_group_t bcm56990_b0_ing_range_check_group_tbl = {
    .sid = FP_ING_RANGE_CHECK_GROUPt,
    .key_fid = FP_ING_RANGE_CHECK_GROUPt_FP_ING_RANGE_CHECK_GROUP_IDf,
    .pipe_id_fid = FP_ING_RANGE_CHECK_GROUPt_PIPEf,
    .rcid_fid = FP_ING_RANGE_CHECK_GROUPt_FP_ING_RANGE_GROUPf,
    .fid_count = FP_ING_RANGE_CHECK_GROUPt_FIELD_COUNT,
};

bcmfp_tbl_meter_t bcm56990_b0_ing_meter_tbl = {
    .sid = METER_ING_FP_TEMPLATEt,
    .key_fid = METER_ING_FP_TEMPLATEt_METER_ING_FP_TEMPLATE_IDf,
    .fid_count = METER_ING_FP_TEMPLATEt_FIELD_COUNT,
};

static void
bcmfp_bcm56990_b0_ingress_group_qualifiers_map(void)
{
    uint32_t fid;
    bcmfp_tbl_qualifier_fid_info_t *group_qualifiers_fid_info = NULL;

    group_qualifiers_fid_info = bcm56990_b0_ingress_group_qualifiers_fid_info;
    for (fid = 0; fid < FP_ING_GRP_TEMPLATEt_FIELD_COUNT; fid++) {
        group_qualifiers_fid_info[fid].qualifier = fid;
    }

    fid = FP_ING_GRP_TEMPLATEt_ENTRY_PRIORITYf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_ENABLEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_PIPEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_MODE_AUTOf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_MODEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_MODE_OPERf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_FP_ING_PRESEL_ENTRY_COUNTf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_FP_ING_PRESEL_ENTRY_TEMPLATE_IDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_FP_ING_PDD_TEMPLATE_IDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_FP_ING_SBR_TEMPLATE_IDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_FP_ING_PRESEL_GRP_TEMPLATE_IDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_NUM_COMPRESSION_TYPEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_COMPRESSION_TYPEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_ING_GRP_TEMPLATEt_AUTO_EXPANDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
}

static void
bcmfp_bcm56990_b0_ingress_rule_qualifiers_map(void)
{
    uint32_t fid;
    uint32_t map_fid;
    bcmfp_tbl_qualifier_fid_info_t *group_qualifiers_fid_info = NULL;
    bcmfp_tbl_qualifier_fid_info_t *rule_qualifiers_fid_info = NULL;

    rule_qualifiers_fid_info = bcm56990_b0_ingress_rule_qualifiers_fid_info;
    group_qualifiers_fid_info = bcm56990_b0_ingress_group_qualifiers_fid_info;

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_IDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OUTER_VLAN_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_ID_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OUTER_VLAN_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_PRIf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OUTER_VLAN_PRI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_PRI_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OUTER_VLAN_PRI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_CFIf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OUTER_VLAN_CFI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OUTER_VLAN_CFI_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OUTER_VLAN_CFI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_MACf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_MAC_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_MAC_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_MAC_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_MACf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_MAC_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_MAC_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_MAC_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP4f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_IP4_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP4_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_IP4_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_IP4f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_IP4_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_IP4_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_IP4_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP6u_UPPERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_IP6_BITMAPu_UPPERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP6_MASKu_UPPERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_IP6_BITMAPu_UPPERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP6u_LOWERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_IP6_BITMAPu_LOWERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP6_MASKu_LOWERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_IP6_BITMAPu_LOWERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_IP6u_UPPERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_IP6_BITMAPu_UPPERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_IP6u_LOWERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_IP6_BITMAPu_LOWERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_IP6_MASKu_UPPERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_IP6_BITMAPu_UPPERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_IP6_MASKu_LOWERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_IP6_BITMAPu_LOWERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_ETHERTYPEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_ETHERTYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_ETHERTYPE_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_ETHERTYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L4SRC_PORTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L4SRC_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L4SRC_PORT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L4SRC_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_ICMP_TYPE_CODEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_ICMP_TYPE_CODE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_ICMP_TYPE_CODE_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_ICMP_TYPE_CODE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L4DST_PORTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L4DST_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L4DST_PORT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L4DST_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_IDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ID_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_TTLf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_TTL_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_EXPf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_EXP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_EXP_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_EXP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_BOSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_BOS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_BOS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_BOS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7A_HASH_UPPERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7A_HASH_UPPER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7A_HASH_UPPER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7A_HASH_UPPER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7A_HASH_LOWERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7A_HASH_LOWER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7A_HASH_LOWER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7A_HASH_LOWER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7B_HASH_UPPERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7B_HASH_UPPER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7B_HASH_UPPER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7B_HASH_UPPER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7B_HASH_LOWERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7B_HASH_LOWER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7B_HASH_LOWER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7B_HASH_LOWER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7A_HASH_UPPERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7A_HASH_UPPER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7A_HASH_UPPER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7A_HASH_UPPER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7A_HASH_LOWERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7A_HASH_LOWER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7A_HASH_LOWER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7A_HASH_LOWER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7B_HASH_UPPERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7B_HASH_UPPER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7B_HASH_UPPER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7B_HASH_UPPER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7B_HASH_LOWERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7B_HASH_LOWER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RTAG7B_HASH_LOWER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RTAG7B_HASH_LOWER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_CTRL_WORDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_CTRL_WORD_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_CTRL_WORD_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_CTRL_WORD_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP6_FLOW_LABELf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP6_FLOW_LABEL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP6_FLOW_LABEL_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP6_FLOW_LABEL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_TTLf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_TTL_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_TOSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_TOS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_TOS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_TOS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_PROTOCOLf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_PROTOCOL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_PROTOCOL_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_PROTOCOL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_TCP_FLAGSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_TCP_FLAGS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_TCP_FLAGS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_TCP_FLAGS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_PROTOf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_PROTO_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_PROTO_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_PROTO_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_SECOND_EH_PROTOf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_SECOND_EH_PROTO_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_SECOND_EH_PROTO_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_SECOND_EH_PROTO_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODE_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_MFf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_FLAGS_MF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_MF_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_FLAGS_MF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_DFf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_FLAGS_DF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_DF_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_FLAGS_DF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FRAGf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_FRAG_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DROP_PKTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DROP_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DROP_PKT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DROP_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DOSATTACK_PKTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DOSATTACK_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DOSATTACK_PKT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DOSATTACK_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L4_PKTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L4_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L4_PKT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L4_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_NON_OR_FIRST_FRAGMENTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_NON_OR_FIRST_FRAGMENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_NON_OR_FIRST_FRAGMENT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_NON_OR_FIRST_FRAGMENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_ICMP_ERROR_PKTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_ICMP_ERROR_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_ICMP_ERROR_PKT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_ICMP_ERROR_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_SRC_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_SRC_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_SRC_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_SRC_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_COMPRESSED_L3_SRC_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_COMPRESSED_L3_SRC_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_COMPRESSED_L3_SRC_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_COMPRESSED_L3_SRC_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_COMPRESSED_L3_DST_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_COMPRESSED_L3_DST_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_COMPRESSED_L3_DST_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_COMPRESSED_L3_DST_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_DST_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_DST_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_DST_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_DST_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_SRC_LPM_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_SRC_LPM_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_SRC_LPM_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_SRC_LPM_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_DST_LPM_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_DST_LPM_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_DST_LPM_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_DST_LPM_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_TNL_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_TNL_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_TNL_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_TNL_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2CACHE_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2CACHE_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2CACHE_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2CACHE_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_BOS_TERMINATEDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_BOS_TERMINATED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_BOS_TERMINATED_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_BOS_TERMINATED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_PORT_SYSTEM_CLASSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_PORT_SYSTEM_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_PORT_SYSTEM_CLASS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_PORT_SYSTEM_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_IIF_CLASS_IDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_IIF_CLASS_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_IIF_CLASS_ID_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_IIF_CLASS_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VLAN_CLASSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VLAN_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VLAN_CLASS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VLAN_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FP_VLAN_CLASS0f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FP_VLAN_CLASS0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FP_VLAN_CLASS0_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FP_VLAN_CLASS0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FP_VLAN_CLASS1f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FP_VLAN_CLASS1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FP_VLAN_CLASS1_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FP_VLAN_CLASS1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FP_VLAN_PORT_GRPf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FP_VLAN_PORT_GRP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FP_VLAN_PORT_GRP_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FP_VLAN_PORT_GRP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FP_ING_GRP_SEL_CLASSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FP_ING_GRP_SEL_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FP_ING_GRP_SEL_CLASS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FP_ING_GRP_SEL_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_UDF_CLASSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_UDF_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_UDF_CLASS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_UDF_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_SRC_CLASSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_SRC_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_SRC_CLASS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_SRC_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_DST_CLASSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_DST_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_DST_CLASS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_DST_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_SRC_CLASSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_SRC_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_SRC_CLASS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_SRC_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_DST_CLASSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_DST_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_DST_CLASS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_DST_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_EM_GROUP_CLASS_ID_0f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_EM_GROUP_CLASS_ID_0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_EM_GROUP_CLASS_ID_0_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_EM_GROUP_CLASS_ID_0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FWD_VLAN_VALIDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FWD_VLAN_VALID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FWD_VLAN_VALID_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FWD_VLAN_VALID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_PORTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_PORT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_TRUNKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_TRUNK_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_TRUNK_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SRC_TRUNK_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_PORTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_PORT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_TRUNKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_TRUNK_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_TRUNK_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DST_TRUNK_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_NHOPf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_NHOP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_NHOP_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_NHOP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OVERLAY_DST_VPf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OVERLAY_DST_VP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OVERLAY_DST_VP_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OVERLAY_DST_VP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OVERLAY_ECMPf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OVERLAY_ECMP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OVERLAY_ECMP_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OVERLAY_ECMP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_MC_GROUPf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_MC_GROUP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_MC_GROUP_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_MC_GROUP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FWD_VLAN_IDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FWD_VLAN_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FWD_VLAN_ID_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FWD_VLAN_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VRFf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VRF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VRF_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VRF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VFIf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VFI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VFI_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VFI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VPNf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VPN_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VPN_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VPN_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_IIFf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_IIF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_IIF_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_IIF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INPORTSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INPORTS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INPORTS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INPORTS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INPORTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INPORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INPORT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INPORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SYSTEM_PORTSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SYSTEM_PORTS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SYSTEM_PORTS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SYSTEM_PORTS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DEVICE_PORTSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DEVICE_PORTS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_DEVICE_PORTS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_DEVICE_PORTS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SVP_PORTSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SVP_PORTS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_SVP_PORTS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_SVP_PORTS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INT_PRIf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INT_PRI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INT_PRI_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INT_PRI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_COLORf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_COLOR_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INT_CN_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INT_CN_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VLAN_OUTER_PRESENTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VLAN_OUTER_PRESENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VLAN_OUTER_PRESENT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VLAN_OUTER_PRESENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OPAQUE_TAG_PRESENTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_PRESENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OPAQUE_TAG_PRESENT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_PRESENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INNER_L2_VLAN_OUTER_PRESENTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INNER_L2_VLAN_OUTER_PRESENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INNER_L2_VLAN_OUTER_PRESENT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INNER_L2_VLAN_OUTER_PRESENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_BYTES_0_7_AFTER_L2HEADERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_BYTES_0_7_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_BYTES_0_7_AFTER_L2HEADER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_BYTES_0_7_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_BYTES_16_23_AFTER_L2HEADERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_BYTES_16_23_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_BYTES_16_23_AFTER_L2HEADER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_BYTES_16_23_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_BYTES_24_31_AFTER_L2HEADERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_BYTES_24_31_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_BYTES_24_31_AFTER_L2HEADER_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_BYTES_24_31_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INNER_DST_IP6u_UPPERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INNER_DST_IP6_BITMAPu_UPPERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INNER_DST_IP6u_LOWERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INNER_DST_IP6_BITMAPu_LOWERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INNER_DST_IP6_MASKu_UPPERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INNER_DST_IP6_BITMAPu_UPPERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INNER_DST_IP6_MASKu_LOWERf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INNER_DST_IP6_BITMAPu_LOWERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_CHECKSUM_VALIDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_CHECKSUM_VALID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_CHECKSUM_VALID_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_CHECKSUM_VALID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_BFD_DISCRIMINATORf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_BFD_DISCRIMINATOR_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_BFD_DISCRIMINATOR_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_BFD_DISCRIMINATOR_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_PKT_LENGTHf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_PKT_LENGTH_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_PKT_LENGTH_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_PKT_LENGTH_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OPAQUE_TAG_LOWf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_LOW_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OPAQUE_TAG_LOW_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_LOW_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OPAQUE_TAG_HIGHf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_HIGH_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OPAQUE_TAG_HIGH_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_HIGH_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INNER_L2_OPAQUE_TAGf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INNER_L2_OPAQUE_TAG_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INNER_L2_OPAQUE_TAG_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INNER_L2_OPAQUE_TAG_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RANGE_CHECKID_BMPf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RANGE_CHECKID_BMP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RANGE_CHECKID_BMP_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RANGE_CHECKID_BMP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RANGE_CHECK_GRP_ID_BMPf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RANGE_CHECK_GRP_ID_BMP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RANGE_CHECK_GRP_ID_BMP_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RANGE_CHECK_GRP_ID_BMP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ACTIONf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ACTION_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_TNL_TYPEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_TNL_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_LOOPBACK_TYPEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_LOOPBACK_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_LOOPBACKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_LOOPBACK_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_LOOPBACK_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_LOOPBACK_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_PROTECTION_DATA_DROPf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_PROTECTION_DATA_DROP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_PROTECTION_DATA_DROP_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_PROTECTION_DATA_DROP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_SRC_STATICf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_SRC_STATIC_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_SRC_STATIC_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_SRC_STATIC_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2STATION_MOVEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2STATION_MOVE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2STATION_MOVE_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2STATION_MOVE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_IN_TNLf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_IN_TNL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_IN_TNL_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_IN_TNL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_PKT_RESOLUTIONf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_PKT_RESOLUTION_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_PKT_RESOLUTION_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_PKT_RESOLUTION_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_HITf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_HITf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_CLASS_IDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_CLASS_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_CLASS_ID_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_CLASS_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_TYPEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_IP_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L2_FORMATf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L2_FORMAT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MYSTATIONTCAM_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MYSTATIONTCAM_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MYSTATIONTCAM_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MYSTATIONTCAM_2_HITf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MYSTATIONTCAM_2_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MYSTATIONTCAM_2_HIT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MYSTATIONTCAM_2_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_ROUTABLE_PKTf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_ROUTABLE_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_ROUTABLE_PKT_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_ROUTABLE_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INNER_TPIDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INNER_TPID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_OUTER_TPIDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_OUTER_TPID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INNER_L2_OUTER_TPIDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INNER_L2_OUTER_TPID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_FWD_TYPEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_FWD_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_ING_STP_STATEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_ING_STP_STATE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_UDF_CHUNKSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_UDF_CHUNKS_BITMAPf;
    map_fid = group_qualifiers_fid_info[map_fid].qualifier;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_UDF_CHUNKS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_UDF_CHUNKS_BITMAPf;
    map_fid = group_qualifiers_fid_info[map_fid].qualifier;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_IP_PROTOCOLf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_IP_PROTOCOL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_IP_PROTOCOL_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_IP_PROTOCOL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_ETHERTYPEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_ETHERTYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_ETHERTYPE_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_ETHERTYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TTL0f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TTL0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TTL0_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TTL0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TTL1f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TTL1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TTL1_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TTL1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TOS0f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TOS0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TOS0_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TOS0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TOS1f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TOS1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TOS1_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TOS1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TCP_FLAGS0f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TCP_FLAGS0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TCP_FLAGS0_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TCP_FLAGS0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TCP_FLAGS1f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TCP_FLAGS1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TCP_FLAGS1_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TCP_FLAGS1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_TNL_IP_TTLf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_TNL_IP_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_TNL_IP_TTL_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_TNL_IP_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TNL_IP_TTLf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TNL_IP_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_C_TNL_IP_TTL_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_C_TNL_IP_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_FLAGSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_FLAGS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_FLAGS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_FLAGS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_MD_HDR_TYPEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_MD_HDR_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_MD_HDR_TYPE_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_MD_HDR_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_HOP_LIMIT_CHECK_STATUSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_HOP_LIMIT_CHECK_STATUS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_HOP_LIMIT_CHECK_STATUS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_HOP_LIMIT_CHECK_STATUS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_MAX_LENGTH_CHECKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_MAX_LENGTH_CHECK_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_MAX_LENGTH_CHECK_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_MAX_LENGTH_CHECK_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_TYPEf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_TYPE_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_UDP_CHECKSUM_ZEROf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_UDP_CHECKSUM_ZERO_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_UDP_CHECKSUM_ZERO_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_UDP_CHECKSUM_ZERO_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_MC_GROUPf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_MC_GROUP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_MC_GROUP_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_MC_GROUP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_PRESEL_CLASSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_PRESEL_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_PRESEL_CLASS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_PRESEL_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_RAL_DETECTEDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_RAL_DETECTED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_RAL_DETECTED_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_RAL_DETECTED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_GAL_DETECTEDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_GAL_DETECTED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_GAL_DETECTED_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_GAL_DETECTED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_CW_DETECTEDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_CW_DETECTED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_CW_DETECTED_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_CW_DETECTED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_A_0_15f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_A_0_15_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_A_0_15_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_A_0_15_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_B_0_15f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_B_0_15_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_B_0_15_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_B_0_15_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_A_31_16f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_A_31_16_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_A_31_16_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_A_31_16_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_B_31_16f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_B_31_16_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_B_31_16_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_B_31_16_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RH_FIRST_4_BYTESf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RH_FIRST_4_BYTES_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RH_FIRST_4_BYTES_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RH_FIRST_4_BYTES_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RH_NEXT_4_BYTESf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RH_NEXT_4_BYTES_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_RH_NEXT_4_BYTES_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_RH_NEXT_4_BYTES_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_VNIDf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_VNID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_VNID_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_VNID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_FLAGSf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_FLAGS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_FLAGS_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_FLAGS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_RSVD_24f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_RSVD_24_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_RSVD_24_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_RSVD_24_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_RSVD_8f;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_RSVD_8_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_RULE_TEMPLATEt_QUAL_VXLAN_RSVD_8_MASKf;
    map_fid = FP_ING_GRP_TEMPLATEt_QUAL_VXLAN_RSVD_8_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

}

static void
bcmfp_bcm56990_b0_ingress_compression_remote_fid_map(void)
{
    /* L3 Compression DST IPV4 */
    bcm56990_b0_compress_ipv4_dst_remote_map[0].fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_IP4f;
    bcm56990_b0_compress_ipv4_dst_remote_map[0].offset = 0;
    bcm56990_b0_compress_ipv4_dst_remote_map[0].width = 32;

    bcm56990_b0_compress_ipv4_dst_remote_maps[0].fid = L3_IPV4_COMP_DSTt_IPV4f;
    bcm56990_b0_compress_ipv4_dst_remote_maps[0].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_dst_remote_maps[0].remote_map = &bcm56990_b0_compress_ipv4_dst_remote_map[0];

    bcm56990_b0_compress_ipv4_dst_remote_map[1].fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_MACf;
    bcm56990_b0_compress_ipv4_dst_remote_map[1].offset = 0;
    bcm56990_b0_compress_ipv4_dst_remote_map[1].width = 48;

    bcm56990_b0_compress_ipv4_dst_remote_maps[1].fid = L3_IPV4_COMP_DSTt_MACf;
    bcm56990_b0_compress_ipv4_dst_remote_maps[1].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_dst_remote_maps[1].remote_map = &bcm56990_b0_compress_ipv4_dst_remote_map[1];

    bcm56990_b0_compress_ipv4_dst_remote_map[2].fid = FP_ING_RULE_TEMPLATEt_QUAL_VRFf;
    bcm56990_b0_compress_ipv4_dst_remote_map[2].offset = 0;
    bcm56990_b0_compress_ipv4_dst_remote_map[2].width = 13;

    bcm56990_b0_compress_ipv4_dst_remote_maps[2].fid = L3_IPV4_COMP_DSTt_VRFf;
    bcm56990_b0_compress_ipv4_dst_remote_maps[2].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_dst_remote_maps[2].remote_map = &bcm56990_b0_compress_ipv4_dst_remote_map[2];

    bcm56990_b0_compress_ipv4_dst_remote_map[3].fid = FP_ING_RULE_TEMPLATEt_QUAL_L4DST_PORTf;
    bcm56990_b0_compress_ipv4_dst_remote_map[3].offset = 0;
    bcm56990_b0_compress_ipv4_dst_remote_map[3].width = 16;

    bcm56990_b0_compress_ipv4_dst_remote_maps[3].fid = L3_IPV4_COMP_DSTt_L4_PORTf;
    bcm56990_b0_compress_ipv4_dst_remote_maps[3].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_dst_remote_maps[3].remote_map = &bcm56990_b0_compress_ipv4_dst_remote_map[3];

    bcm56990_b0_compress_ipv4_dst_remote_map[4].fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_PROTOCOLf;
    bcm56990_b0_compress_ipv4_dst_remote_map[4].offset = 0;
    bcm56990_b0_compress_ipv4_dst_remote_map[4].width = 8;

    bcm56990_b0_compress_ipv4_dst_remote_maps[4].fid = L3_IPV4_COMP_DSTt_IP_PROTOCOLf;
    bcm56990_b0_compress_ipv4_dst_remote_maps[4].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_dst_remote_maps[4].remote_map = &bcm56990_b0_compress_ipv4_dst_remote_map[4];

    bcm56990_b0_compress_ipv4_dst_remote_map[5].fid = FP_ING_RULE_TEMPLATEt_QUAL_TCP_FLAGSf;
    bcm56990_b0_compress_ipv4_dst_remote_map[5].offset = 0;
    bcm56990_b0_compress_ipv4_dst_remote_map[5].width = 8;

    bcm56990_b0_compress_ipv4_dst_remote_maps[5].fid = L3_IPV4_COMP_DSTt_TCP_FLAGSf;
    bcm56990_b0_compress_ipv4_dst_remote_maps[5].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_dst_remote_maps[5].remote_map = &bcm56990_b0_compress_ipv4_dst_remote_map[5];

    bcm56990_b0_compress_ipv4_dst_remote_map[6].fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_DFf;
    bcm56990_b0_compress_ipv4_dst_remote_map[6].offset = 0;
    bcm56990_b0_compress_ipv4_dst_remote_map[6].width = 1;

    bcm56990_b0_compress_ipv4_dst_remote_map[7].fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_MFf;
    bcm56990_b0_compress_ipv4_dst_remote_map[7].offset = 0;
    bcm56990_b0_compress_ipv4_dst_remote_map[7].width = 1;

    bcm56990_b0_compress_ipv4_dst_remote_maps[6].fid = L3_IPV4_COMP_DSTt_IP_FLAGSf;
    bcm56990_b0_compress_ipv4_dst_remote_maps[6].num_remote_map = 2;
    bcm56990_b0_compress_ipv4_dst_remote_maps[6].remote_map = &bcm56990_b0_compress_ipv4_dst_remote_map[6];

    bcm56990_b0_compress_ipv4_dst_remote_map[8].fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_A_0_15f;
    bcm56990_b0_compress_ipv4_dst_remote_map[8].offset = 0;
    bcm56990_b0_compress_ipv4_dst_remote_map[8].width = 16;

    bcm56990_b0_compress_ipv4_dst_remote_maps[7].fid = L3_IPV4_COMP_DSTt_CLASS_IDf;
    bcm56990_b0_compress_ipv4_dst_remote_maps[7].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_dst_remote_maps[7].remote_map = &bcm56990_b0_compress_ipv4_dst_remote_map[8];

    bcm56990_b0_compress_remote_sid_info[0].sid = L3_IPV4_COMP_DSTt;
    bcm56990_b0_compress_remote_sid_info[0].remote_maps = bcm56990_b0_compress_ipv4_dst_remote_maps;
    bcm56990_b0_compress_remote_sid_info[0].num_remote_maps = 8;

    /* L3 Compression SRC IPV4 */
    bcm56990_b0_compress_ipv4_src_remote_map[0].fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP4f;
    bcm56990_b0_compress_ipv4_src_remote_map[0].offset = 0;
    bcm56990_b0_compress_ipv4_src_remote_map[0].width = 32;

    bcm56990_b0_compress_ipv4_src_remote_maps[0].fid = L3_IPV4_COMP_SRCt_IPV4f;
    bcm56990_b0_compress_ipv4_src_remote_maps[0].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_src_remote_maps[0].remote_map = &bcm56990_b0_compress_ipv4_src_remote_map[0];

    bcm56990_b0_compress_ipv4_src_remote_map[1].fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_MACf;
    bcm56990_b0_compress_ipv4_src_remote_map[1].offset = 0;
    bcm56990_b0_compress_ipv4_src_remote_map[1].width = 48;

    bcm56990_b0_compress_ipv4_src_remote_maps[1].fid = L3_IPV4_COMP_SRCt_MACf;
    bcm56990_b0_compress_ipv4_src_remote_maps[1].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_src_remote_maps[1].remote_map = &bcm56990_b0_compress_ipv4_src_remote_map[1];

    bcm56990_b0_compress_ipv4_src_remote_map[2].fid = FP_ING_RULE_TEMPLATEt_QUAL_VRFf;
    bcm56990_b0_compress_ipv4_src_remote_map[2].offset = 0;
    bcm56990_b0_compress_ipv4_src_remote_map[2].width = 13;

    bcm56990_b0_compress_ipv4_src_remote_maps[2].fid = L3_IPV4_COMP_SRCt_VRFf;
    bcm56990_b0_compress_ipv4_src_remote_maps[2].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_src_remote_maps[2].remote_map = &bcm56990_b0_compress_ipv4_src_remote_map[2];

    bcm56990_b0_compress_ipv4_src_remote_map[3].fid = FP_ING_RULE_TEMPLATEt_QUAL_L4SRC_PORTf;
    bcm56990_b0_compress_ipv4_src_remote_map[3].offset = 0;
    bcm56990_b0_compress_ipv4_src_remote_map[3].width = 16;

    bcm56990_b0_compress_ipv4_src_remote_maps[3].fid = L3_IPV4_COMP_SRCt_L4_PORTf;
    bcm56990_b0_compress_ipv4_src_remote_maps[3].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_src_remote_maps[3].remote_map = &bcm56990_b0_compress_ipv4_src_remote_map[3];

    bcm56990_b0_compress_ipv4_src_remote_map[4].fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_PROTOCOLf;
    bcm56990_b0_compress_ipv4_src_remote_map[4].offset = 0;
    bcm56990_b0_compress_ipv4_src_remote_map[4].width = 8;

    bcm56990_b0_compress_ipv4_src_remote_maps[4].fid = L3_IPV4_COMP_SRCt_IP_PROTOCOLf;
    bcm56990_b0_compress_ipv4_src_remote_maps[4].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_src_remote_maps[4].remote_map = &bcm56990_b0_compress_ipv4_src_remote_map[4];

    bcm56990_b0_compress_ipv4_src_remote_map[5].fid = FP_ING_RULE_TEMPLATEt_QUAL_TCP_FLAGSf;
    bcm56990_b0_compress_ipv4_src_remote_map[5].offset = 0;
    bcm56990_b0_compress_ipv4_src_remote_map[5].width = 8;

    bcm56990_b0_compress_ipv4_src_remote_maps[5].fid = L3_IPV4_COMP_SRCt_TCP_FLAGSf;
    bcm56990_b0_compress_ipv4_src_remote_maps[5].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_src_remote_maps[5].remote_map = &bcm56990_b0_compress_ipv4_src_remote_map[5];

    bcm56990_b0_compress_ipv4_src_remote_map[6].fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_DFf;
    bcm56990_b0_compress_ipv4_src_remote_map[6].offset = 0;
    bcm56990_b0_compress_ipv4_src_remote_map[6].width = 1;

    bcm56990_b0_compress_ipv4_src_remote_map[7].fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_FLAGS_MFf;
    bcm56990_b0_compress_ipv4_src_remote_map[7].offset = 0;
    bcm56990_b0_compress_ipv4_src_remote_map[7].width = 1;

    bcm56990_b0_compress_ipv4_src_remote_maps[6].fid = L3_IPV4_COMP_SRCt_IP_FLAGSf;
    bcm56990_b0_compress_ipv4_src_remote_maps[6].num_remote_map = 2;
    bcm56990_b0_compress_ipv4_src_remote_maps[6].remote_map = &bcm56990_b0_compress_ipv4_src_remote_map[6];

    bcm56990_b0_compress_ipv4_src_remote_map[8].fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_B_0_15f;
    bcm56990_b0_compress_ipv4_src_remote_map[8].offset = 0;
    bcm56990_b0_compress_ipv4_src_remote_map[8].width = 16;

    bcm56990_b0_compress_ipv4_src_remote_maps[7].fid = L3_IPV4_COMP_SRCt_CLASS_IDf;
    bcm56990_b0_compress_ipv4_src_remote_maps[7].num_remote_map = 1;
    bcm56990_b0_compress_ipv4_src_remote_maps[7].remote_map = &bcm56990_b0_compress_ipv4_src_remote_map[8];

    bcm56990_b0_compress_remote_sid_info[1].sid = L3_IPV4_COMP_SRCt;
    bcm56990_b0_compress_remote_sid_info[1].remote_maps = bcm56990_b0_compress_ipv4_src_remote_maps;
    bcm56990_b0_compress_remote_sid_info[1].num_remote_maps = 8;

    /* L3 Compression DST IPV6 */
    bcm56990_b0_compress_ipv6_dst_remote_map[0].fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_IP6u_LOWERf;
    bcm56990_b0_compress_ipv6_dst_remote_map[0].offset = 0;
    bcm56990_b0_compress_ipv6_dst_remote_map[0].width = 64;

    bcm56990_b0_compress_ipv6_dst_remote_maps[0].fid = L3_IPV6_COMP_DSTt_IPV6u_LOWERf;
    bcm56990_b0_compress_ipv6_dst_remote_maps[0].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_dst_remote_maps[0].remote_map = &bcm56990_b0_compress_ipv6_dst_remote_map[0];

    bcm56990_b0_compress_ipv6_dst_remote_map[1].fid = FP_ING_RULE_TEMPLATEt_QUAL_DST_IP6u_UPPERf;
    bcm56990_b0_compress_ipv6_dst_remote_map[1].offset = 0;
    bcm56990_b0_compress_ipv6_dst_remote_map[1].width = 64;

    bcm56990_b0_compress_ipv6_dst_remote_maps[1].fid = L3_IPV6_COMP_DSTt_IPV6u_UPPERf;
    bcm56990_b0_compress_ipv6_dst_remote_maps[1].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_dst_remote_maps[1].remote_map = &bcm56990_b0_compress_ipv6_dst_remote_map[1];

    bcm56990_b0_compress_ipv6_dst_remote_map[2].fid = FP_ING_RULE_TEMPLATEt_QUAL_VRFf;
    bcm56990_b0_compress_ipv6_dst_remote_map[2].offset = 0;
    bcm56990_b0_compress_ipv6_dst_remote_map[2].width = 13;

    bcm56990_b0_compress_ipv6_dst_remote_maps[2].fid = L3_IPV6_COMP_DSTt_VRFf;
    bcm56990_b0_compress_ipv6_dst_remote_maps[2].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_dst_remote_maps[2].remote_map = &bcm56990_b0_compress_ipv6_dst_remote_map[2];

    bcm56990_b0_compress_ipv6_dst_remote_map[3].fid = FP_ING_RULE_TEMPLATEt_QUAL_L4DST_PORTf;
    bcm56990_b0_compress_ipv6_dst_remote_map[3].offset = 0;
    bcm56990_b0_compress_ipv6_dst_remote_map[3].width = 16;

    bcm56990_b0_compress_ipv6_dst_remote_maps[3].fid = L3_IPV6_COMP_DSTt_L4_PORTf;
    bcm56990_b0_compress_ipv6_dst_remote_maps[3].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_dst_remote_maps[3].remote_map = &bcm56990_b0_compress_ipv6_dst_remote_map[3];

    bcm56990_b0_compress_ipv6_dst_remote_map[4].fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_PROTOCOLf;
    bcm56990_b0_compress_ipv6_dst_remote_map[4].offset = 0;
    bcm56990_b0_compress_ipv6_dst_remote_map[4].width = 8;

    bcm56990_b0_compress_ipv6_dst_remote_maps[4].fid = L3_IPV6_COMP_DSTt_IP_PROTOCOLf;
    bcm56990_b0_compress_ipv6_dst_remote_maps[4].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_dst_remote_maps[4].remote_map = &bcm56990_b0_compress_ipv6_dst_remote_map[4];

    bcm56990_b0_compress_ipv6_dst_remote_map[5].fid = FP_ING_RULE_TEMPLATEt_QUAL_TCP_FLAGSf;
    bcm56990_b0_compress_ipv6_dst_remote_map[5].offset = 0;
    bcm56990_b0_compress_ipv6_dst_remote_map[5].width = 8;

    bcm56990_b0_compress_ipv6_dst_remote_maps[5].fid = L3_IPV6_COMP_DSTt_TCP_FLAGSf;
    bcm56990_b0_compress_ipv6_dst_remote_maps[5].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_dst_remote_maps[5].remote_map = &bcm56990_b0_compress_ipv6_dst_remote_map[5];

    bcm56990_b0_compress_ipv6_dst_remote_map[6].fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_A_0_15f;
    bcm56990_b0_compress_ipv6_dst_remote_map[6].offset = 0;
    bcm56990_b0_compress_ipv6_dst_remote_map[6].width = 16;

    bcm56990_b0_compress_ipv6_dst_remote_maps[6].fid = L3_IPV6_COMP_DSTt_CLASS_IDf;
    bcm56990_b0_compress_ipv6_dst_remote_maps[6].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_dst_remote_maps[6].remote_map = &bcm56990_b0_compress_ipv6_dst_remote_map[6];

    bcm56990_b0_compress_remote_sid_info[2].sid = L3_IPV6_COMP_DSTt;
    bcm56990_b0_compress_remote_sid_info[2].remote_maps = bcm56990_b0_compress_ipv6_dst_remote_maps;
    bcm56990_b0_compress_remote_sid_info[2].num_remote_maps = 7;

    /* L3 Compression SRC IPV6 */
    bcm56990_b0_compress_ipv6_src_remote_map[0].fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP6u_LOWERf;
    bcm56990_b0_compress_ipv6_src_remote_map[0].offset = 0;
    bcm56990_b0_compress_ipv6_src_remote_map[0].width = 64;

    bcm56990_b0_compress_ipv6_src_remote_maps[0].fid = L3_IPV6_COMP_SRCt_IPV6u_LOWERf;
    bcm56990_b0_compress_ipv6_src_remote_maps[0].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_src_remote_maps[0].remote_map = &bcm56990_b0_compress_ipv6_src_remote_map[0];

    bcm56990_b0_compress_ipv6_src_remote_map[1].fid = FP_ING_RULE_TEMPLATEt_QUAL_SRC_IP6u_UPPERf;
    bcm56990_b0_compress_ipv6_src_remote_map[1].offset = 0;
    bcm56990_b0_compress_ipv6_src_remote_map[1].width = 64;

    bcm56990_b0_compress_ipv6_src_remote_maps[1].fid = L3_IPV6_COMP_SRCt_IPV6u_UPPERf;
    bcm56990_b0_compress_ipv6_src_remote_maps[1].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_src_remote_maps[1].remote_map = &bcm56990_b0_compress_ipv6_src_remote_map[1];

    bcm56990_b0_compress_ipv6_src_remote_map[2].fid = FP_ING_RULE_TEMPLATEt_QUAL_VRFf;
    bcm56990_b0_compress_ipv6_src_remote_map[2].offset = 0;
    bcm56990_b0_compress_ipv6_src_remote_map[2].width = 13;

    bcm56990_b0_compress_ipv6_src_remote_maps[2].fid = L3_IPV6_COMP_SRCt_VRFf;
    bcm56990_b0_compress_ipv6_src_remote_maps[2].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_src_remote_maps[2].remote_map = &bcm56990_b0_compress_ipv6_src_remote_map[2];

    bcm56990_b0_compress_ipv6_src_remote_map[3].fid = FP_ING_RULE_TEMPLATEt_QUAL_L4SRC_PORTf;
    bcm56990_b0_compress_ipv6_src_remote_map[3].offset = 0;
    bcm56990_b0_compress_ipv6_src_remote_map[3].width = 16;

    bcm56990_b0_compress_ipv6_src_remote_maps[3].fid = L3_IPV6_COMP_SRCt_L4_PORTf;
    bcm56990_b0_compress_ipv6_src_remote_maps[3].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_src_remote_maps[3].remote_map = &bcm56990_b0_compress_ipv6_src_remote_map[3];

    bcm56990_b0_compress_ipv6_src_remote_map[4].fid = FP_ING_RULE_TEMPLATEt_QUAL_IP_PROTOCOLf;
    bcm56990_b0_compress_ipv6_src_remote_map[4].offset = 0;
    bcm56990_b0_compress_ipv6_src_remote_map[4].width = 8;

    bcm56990_b0_compress_ipv6_src_remote_maps[4].fid = L3_IPV6_COMP_SRCt_IP_PROTOCOLf;
    bcm56990_b0_compress_ipv6_src_remote_maps[4].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_src_remote_maps[4].remote_map = &bcm56990_b0_compress_ipv6_src_remote_map[4];

    bcm56990_b0_compress_ipv6_src_remote_map[5].fid = FP_ING_RULE_TEMPLATEt_QUAL_TCP_FLAGSf;
    bcm56990_b0_compress_ipv6_src_remote_map[5].offset = 0;
    bcm56990_b0_compress_ipv6_src_remote_map[5].width = 8;

    bcm56990_b0_compress_ipv6_src_remote_maps[5].fid = L3_IPV6_COMP_SRCt_TCP_FLAGSf;
    bcm56990_b0_compress_ipv6_src_remote_maps[5].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_src_remote_maps[5].remote_map = &bcm56990_b0_compress_ipv6_src_remote_map[5];

    bcm56990_b0_compress_ipv6_src_remote_map[6].fid = FP_ING_RULE_TEMPLATEt_QUAL_L3_L4_COMPRESSION_ID_B_0_15f;
    bcm56990_b0_compress_ipv6_src_remote_map[6].offset = 0;
    bcm56990_b0_compress_ipv6_src_remote_map[6].width = 16;

    bcm56990_b0_compress_ipv6_src_remote_maps[6].fid = L3_IPV6_COMP_SRCt_CLASS_IDf;
    bcm56990_b0_compress_ipv6_src_remote_maps[6].num_remote_map = 1;
    bcm56990_b0_compress_ipv6_src_remote_maps[6].remote_map = &bcm56990_b0_compress_ipv6_src_remote_map[6];

    bcm56990_b0_compress_remote_sid_info[3].sid = L3_IPV6_COMP_SRCt;
    bcm56990_b0_compress_remote_sid_info[3].remote_maps = bcm56990_b0_compress_ipv6_src_remote_maps;
    bcm56990_b0_compress_remote_sid_info[3].num_remote_maps = 7;
}

static void
bcmfp_bcm56990_b0_ingress_policy_actions_map(void)
{
    uint32_t fid;
    bcmfp_tbl_action_fid_info_t *policy_actions_fid_info = NULL;

    policy_actions_fid_info = bcm56990_b0_ingress_policy_actions_fid_info;
    for (fid = 0; fid < FP_ING_POLICY_TEMPLATEt_FIELD_COUNT; fid++) {
        policy_actions_fid_info[fid].action = fid;
    }
    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MIRROR_INSTANCE_IDf;
    policy_actions_fid_info[fid].action = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT;

    fid = FP_ING_POLICY_TEMPLATEt_ACTION_MIRROR_INSTANCE_ENABLEf;
    policy_actions_fid_info[fid].action = FP_ING_POLICY_TEMPLATEt_FIELD_COUNT + 4;
}

static void
bcmfp_bcm56990_b0_ingress_pse_qualifiers_map(void)
{
    uint32_t fid;
    uint32_t map_fid;
    bcmfp_tbl_qualifier_fid_info_t *pse_qualifiers_fid_info = NULL;

    pse_qualifiers_fid_info =
        bcm56990_b0_ingress_pse_qualifiers_fid_info;

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_IP_TYPEf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_IP_TYPEf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_L4_PKTf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_L4_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_L4_PKT_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_L4_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_TNL_TYPEf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_TNL_TYPEf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_HIT_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HIT_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_L3_TNL_HITf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_L3_TNL_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_L3_TNL_HIT_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_L3_TNL_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_DOSATTACK_PKTf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_DOSATTACK_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_DOSATTACK_PKT_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_DOSATTACK_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_ING_STP_STATEf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_ING_STP_STATEf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_VLAN_VALIDf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_VLAN_VALIDf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_VLAN_VALID_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_VLAN_VALIDf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_TYPEf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_TYPEf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_PKT_RESOLUTIONf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_PKT_RESOLUTIONf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_PKT_RESOLUTION_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_PKT_RESOLUTIONf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_DROP_PKTf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_DROP_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_DROP_PKT_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_DROP_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASSf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASSf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASS_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASSf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_LTIDf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_LTIDf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_LTID_MASKf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_LTIDf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_PRESEL_CLASSf;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_PRESEL_CLASSf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_ACTION);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L2f;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L2f;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_ACTION);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L3_L4f;
    map_fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L3_L4f;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_ACTION);
}

int
bcmfp_bcm56990_b0_ing_tbls_init(int unit,
                                bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_tbl = &bcm56990_b0_ing_group_tbl;
    stage->tbls.group_info_tbl = &bcm56990_b0_ing_group_info_tbl;
    stage->tbls.rule_tbl = &bcm56990_b0_ing_rule_tbl;
    stage->tbls.policy_tbl = &bcm56990_b0_ing_policy_tbl;
    stage->tbls.entry_tbl = &bcm56990_b0_ing_entry_tbl;
    stage->tbls.pse_tbl = &bcm56990_b0_ing_pse_tbl;
    stage->tbls.src_class_tbl = &bcm56990_b0_ing_src_class_tbl;
    stage->tbls.range_check_group_tbl = &bcm56990_b0_ing_range_check_group_tbl;
    stage->tbls.meter_tbl = &bcm56990_b0_ing_meter_tbl;
    stage->tbls.num_compress_fid_remote_info = 4;
    stage->tbls.compress_fid_remote_info = bcm56990_b0_compress_remote_sid_info;
    (void)bcmfp_bcm56990_b0_ingress_group_qualifiers_map();
    (void)bcmfp_bcm56990_b0_ingress_rule_qualifiers_map();
    (void)bcmfp_bcm56990_b0_ingress_policy_actions_map();
    (void)bcmfp_bcm56990_b0_ingress_pse_qualifiers_map();
    (void)bcmfp_bcm56990_b0_ingress_compression_remote_fid_map();
exit:
    SHR_FUNC_EXIT();
}
