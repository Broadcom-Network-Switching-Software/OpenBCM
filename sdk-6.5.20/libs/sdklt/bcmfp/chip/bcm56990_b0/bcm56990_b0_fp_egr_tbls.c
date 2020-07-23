/*! \file bcm56990_b0_fp_egr_tbls.c
 *
 * API to initialize Egress FP stage logical tables and some FIDs
 * in each of those logical table for Tomahawk-4 B0(56990_B0).
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
#include <bcmfp/bcmfp_types_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

bcmfp_tbl_qualifier_fid_info_t
bcm56990_b0_egress_group_qualifiers_fid_info[FP_EGR_GRP_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_qualifier_fid_info_t
bcm56990_b0_egress_rule_qualifiers_fid_info[FP_EGR_RULE_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_action_fid_info_t
bcm56990_b0_egress_policy_actions_fid_info[FP_EGR_POLICY_TEMPLATEt_FIELD_COUNT];

static void bcmfp_bcm56990_b0_egress_group_qualifiers_map(void);
static void bcmfp_bcm56990_b0_egress_rule_qualifiers_map(void);
static void bcmfp_bcm56990_b0_egress_policy_actions_map(void);

bcmfp_tbl_group_t bcm56990_b0_egr_group_tbl = {
    .sid = FP_EGR_GRP_TEMPLATEt,
    .key_fid = FP_EGR_GRP_TEMPLATEt_FP_EGR_GRP_TEMPLATE_IDf,
    .pipe_id_fid = FP_EGR_GRP_TEMPLATEt_PIPEf,
    .priority_fid = FP_EGR_GRP_TEMPLATEt_ENTRY_PRIORITYf,
    .slice_mode_fid = FP_EGR_GRP_TEMPLATEt_MODEf,
    .vslice_group_fid = FP_EGR_GRP_TEMPLATEt_VIRTUAL_SLICE_GRPf,
    .group_type_fid = FP_EGR_GRP_TEMPLATEt_PORT_PKT_TYPEf,
    .qualifiers_fid_info = bcm56990_b0_egress_group_qualifiers_fid_info,
    .enable_fid = FP_EGR_GRP_TEMPLATEt_ENABLEf,
    .auto_expand_fid = FP_EGR_GRP_TEMPLATEt_AUTO_EXPANDf,
    .fid_count = FP_EGR_GRP_TEMPLATEt_FIELD_COUNT,
};

bcmfp_tbl_group_info_t bcm56990_b0_egr_group_info_tbl = {
   .sid = FP_EGR_GRP_TEMPLATE_INFOt,
   .key_fid = FP_EGR_GRP_TEMPLATE_INFOt_FP_EGR_GRP_TEMPLATE_IDf,
   .hw_ltid =  FP_EGR_GRP_TEMPLATE_INFOt_HW_LTIDf,
   .num_entries_created =  FP_EGR_GRP_TEMPLATE_INFOt_NUM_ENTRIES_CREATEDf,
   .num_entries_tentative = FP_EGR_GRP_TEMPLATE_INFOt_NUM_ENTRIES_TENTATIVEf,
   .num_partition_id =  FP_EGR_GRP_TEMPLATE_INFOt_NUM_PARTITION_IDf,
   .fid_count = FP_EGR_GRP_TEMPLATE_INFOt_FIELD_COUNT,
};

bcmfp_tbl_rule_t bcm56990_b0_egr_rule_tbl = {
    .sid = FP_EGR_RULE_TEMPLATEt,
    .key_fid = FP_EGR_RULE_TEMPLATEt_FP_EGR_RULE_TEMPLATE_IDf,
    .qualifiers_fid_info = bcm56990_b0_egress_rule_qualifiers_fid_info,
    .fid_count = FP_EGR_RULE_TEMPLATEt_FIELD_COUNT,
};

bcmfp_tbl_policy_t bcm56990_b0_egr_policy_tbl = {
    .sid = FP_EGR_POLICY_TEMPLATEt,
    .key_fid = FP_EGR_POLICY_TEMPLATEt_FP_EGR_POLICY_TEMPLATE_IDf,
    .actions_fid_info = bcm56990_b0_egress_policy_actions_fid_info,
    .flex_ctr_object_fid = FP_EGR_POLICY_TEMPLATEt_CTR_EGR_EFLEX_OBJECTf,
    .fid_count = FP_EGR_POLICY_TEMPLATEt_FIELD_COUNT,
};

bcmfp_tbl_entry_t bcm56990_b0_egr_entry_tbl = {
    .sid = FP_EGR_ENTRYt,
    .key_fid = FP_EGR_ENTRYt_FP_EGR_ENTRY_IDf,
    .oprtnl_state_fid = FP_EGR_ENTRYt_OPERATIONAL_STATEf,
    .priority_fid = FP_EGR_ENTRYt_ENTRY_PRIORITYf,
    .group_id_fid = FP_EGR_ENTRYt_FP_EGR_GRP_TEMPLATE_IDf,
    .rule_id_fid = FP_EGR_ENTRYt_FP_EGR_RULE_TEMPLATE_IDf,
    .policy_id_fid = FP_EGR_ENTRYt_FP_EGR_POLICY_TEMPLATE_IDf,
    .meter_id_fid = FP_EGR_ENTRYt_METER_EGR_FP_TEMPLATE_IDf,
    .flex_ctr_action_fid = FP_EGR_ENTRYt_CTR_EGR_EFLEX_ACTIONf,
    .enable_fid = FP_EGR_ENTRYt_ENABLEf,
    .fid_count = FP_EGR_ENTRYt_FIELD_COUNT,
};

static void
bcmfp_bcm56990_b0_egress_group_qualifiers_map(void)
{
    uint32_t fid;
    bcmfp_tbl_qualifier_fid_info_t *group_qualifiers_fid_info = NULL;

    group_qualifiers_fid_info = bcm56990_b0_egress_group_qualifiers_fid_info;
    for (fid = 0; fid < FP_EGR_GRP_TEMPLATEt_FIELD_COUNT; fid++) {
        group_qualifiers_fid_info[fid].qualifier = fid;
    }
    fid = FP_EGR_GRP_TEMPLATEt_PIPEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EGR_GRP_TEMPLATEt_ENTRY_PRIORITYf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EGR_GRP_TEMPLATEt_ENABLEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EGR_GRP_TEMPLATEt_MODEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EGR_GRP_TEMPLATEt_PORT_PKT_TYPEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EGR_GRP_TEMPLATEt_VIRTUAL_SLICE_GRPf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EGR_GRP_TEMPLATEt_AUTO_EXPANDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
}

static void
bcmfp_bcm56990_b0_egress_rule_qualifiers_map(void)
{
    uint32_t fid;
    uint32_t map_fid;
    bcmfp_tbl_qualifier_fid_info_t *rule_qualifiers_fid_info = NULL;

    rule_qualifiers_fid_info = bcm56990_b0_egress_rule_qualifiers_fid_info;

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_EGR_NHOP_CLASS_IDf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_EGR_NHOP_CLASS_IDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_EGR_NHOP_CLASS_ID_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_EGR_NHOP_CLASS_IDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_EGR_L3_IIF_CLASS_IDf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_EGR_L3_IIF_CLASS_IDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_EGR_L3_IIF_CLASS_ID_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_EGR_L3_IIF_CLASS_IDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRV6_SRH_PKTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRV6_SRH_PKTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRV6_SRH_PKT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRV6_SRH_PKTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_L4_PKTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_L4_PKTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_L4_PKT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_L4_PKTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_INT_PRIf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_INT_PRIf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_INT_PRI_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_INT_PRIf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_COLORf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_COLORf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_L2_FORMATf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_L2_FORMATf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_ETHERTYPEf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_ETHERTYPEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_ETHERTYPE_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_ETHERTYPEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRC_MACf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRC_MACf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRC_MAC_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRC_MACf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DST_MACf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DST_MACf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DST_MAC_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DST_MACf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_LOOPBACK_PP_PORTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_PP_PORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_LOOPBACK_PP_PORT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_PP_PORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_LOOPBACK_TRAFFIC_CLASSf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_TRAFFIC_CLASSf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_LOOPBACK_TRAFFIC_CLASS_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_TRAFFIC_CLASSf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_LOOPBACK_COLORf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_COLORf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_LOOPBACK_MASQUERADE_PKT_PROFILEf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_MASQUERADE_PKT_PROFILEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_LOOPBACK_MASQUERADE_PKT_PROFILE_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_MASQUERADE_PKT_PROFILEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_VISIBILITY_PKTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_VISIBILITY_PKTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_VISIBILITY_PKT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_VISIBILITY_PKTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_LOOPBACK_SRC_PORTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_SRC_PORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_LOOPBACK_SRC_PORT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_SRC_PORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_LOOPBACK_TYPEf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_LOOPBACK_TYPEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_INPORTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_INPORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_INPORT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_INPORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_L3_ROUTABLE_PKTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_L3_ROUTABLE_PKTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_L3_ROUTABLE_PKT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_L3_ROUTABLE_PKTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_IDf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_OUTER_VLAN_IDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_ID_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_OUTER_VLAN_IDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_CFIf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_OUTER_VLAN_CFIf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_CFI_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_OUTER_VLAN_CFIf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_PRIf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_OUTER_VLAN_PRIf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_OUTER_VLAN_PRI_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_OUTER_VLAN_PRIf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_VLAN_OUTER_PRESENTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_VLAN_OUTER_PRESENTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_VLAN_OUTER_PRESENT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_VLAN_OUTER_PRESENTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_ING_CLASS_ID_TYPEf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_ING_CLASS_ID_TYPEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_ING_CLASS_IDf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_ING_CLASS_IDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_ING_CLASS_ID_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_ING_CLASS_IDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_PORT_GRP_OR_PORT_SYSTEM_CLASSf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_PORT_GRP_OR_PORT_SYSTEM_CLASSf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_PORT_GRP_OR_PORT_SYSTEM_CLASS_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_PORT_GRP_OR_PORT_SYSTEM_CLASSf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_OUTPORTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_OUTPORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_OUTPORT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_OUTPORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_CPU_COSf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_CPU_COSf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_CPU_COS_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_CPU_COSf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_IP_TYPEf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_IP_TYPEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_FWD_VLAN_IDf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_FWD_VLAN_IDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_FWD_VLAN_ID_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_FWD_VLAN_IDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_VRFf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_VRFf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_VRF_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_VRFf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_VPNf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_VPNf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_VPN_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_VPNf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SFLOW_SAMPLEDf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SFLOW_SAMPLEDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SFLOW_SAMPLED_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SFLOW_SAMPLEDf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_FWD_TYPEf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_FWD_TYPEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_INT_CNf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_INT_CNf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_INT_CN_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_INT_CNf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DROP_PKTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DROP_PKTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DROP_PKT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DROP_PKTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_IP_FRAGf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_IP_FRAGf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_MIRR_COPYf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_MIRR_COPYf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_MIRR_COPY_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_MIRR_COPYf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_TCP_FLAGSf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_TCP_FLAGSf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_TCP_FLAGS_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_TCP_FLAGSf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_L4DST_PORTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_L4DST_PORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_L4DST_PORT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_L4DST_PORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_L4SRC_PORTf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_L4SRC_PORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_L4SRC_PORT_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_L4SRC_PORTf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_ICMP_TYPE_CODEf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_ICMP_TYPE_CODEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_ICMP_TYPE_CODE_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_ICMP_TYPE_CODEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_TTLf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_TTLf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_TTL_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_TTLf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_IP_PROTOCOLf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_IP_PROTOCOLf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_IP_PROTOCOL_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_IP_PROTOCOLf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP4f;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DST_IP4f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP4_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DST_IP4f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP4f;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRC_IP4f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP4_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRC_IP4f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_TOSf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_TOSf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_TOS_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_TOSf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6u_UPPERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRC_IP6f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_UPPER);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6u_LOWERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRC_IP6f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_LOWER);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6_MASKu_UPPERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRC_IP6f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_UPPER);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6_MASKu_LOWERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRC_IP6f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_LOWER);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6u_UPPERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DST_IP6f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_UPPER);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6u_LOWERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DST_IP6f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_LOWER);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6_MASKu_UPPERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DST_IP6f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_UPPER);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6_MASKu_LOWERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DST_IP6f;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_LOWER);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6_HIGHf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRC_IP6_HIGHf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_SRC_IP6_HIGH_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_SRC_IP6_HIGHf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6_HIGHf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DST_IP6_HIGHf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_DST_IP6_HIGH_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_DST_IP6_HIGHf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODEf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODE_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODEf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_PROTOf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_PROTOf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_PROTO_MASKf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_PROTOf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_BYTES_0_7_AFTER_L2HEADERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_BYTES_0_7_AFTER_L2HEADERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_BYTES_0_7_AFTER_L2HEADERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_BYTES_0_7_AFTER_L2HEADERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_BYTES_16_17_AFTER_L2HEADERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_BYTES_16_17_AFTER_L2HEADERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EGR_RULE_TEMPLATEt_QUAL_BYTES_16_17_AFTER_L2HEADERf;
    map_fid = FP_EGR_GRP_TEMPLATEt_QUAL_BYTES_16_17_AFTER_L2HEADERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);
}

static void
bcmfp_bcm56990_b0_egress_policy_actions_map(void)
{
    uint32_t fid;
    bcmfp_tbl_action_fid_info_t *policy_actions_fid_info = NULL;

    policy_actions_fid_info = bcm56990_b0_egress_policy_actions_fid_info;
    for (fid = 0; fid < FP_EGR_POLICY_TEMPLATEt_FIELD_COUNT; fid++) {
        policy_actions_fid_info[fid].action = fid;
    }
}

int
bcmfp_bcm56990_b0_egress_tbls_init(int unit,
                                   bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_tbl = &bcm56990_b0_egr_group_tbl;
    stage->tbls.group_info_tbl = &bcm56990_b0_egr_group_info_tbl;
    stage->tbls.rule_tbl = &bcm56990_b0_egr_rule_tbl;
    stage->tbls.policy_tbl = &bcm56990_b0_egr_policy_tbl;
    stage->tbls.entry_tbl = &bcm56990_b0_egr_entry_tbl;
    (void)bcmfp_bcm56990_b0_egress_group_qualifiers_map();
    (void)bcmfp_bcm56990_b0_egress_rule_qualifiers_map();
    (void)bcmfp_bcm56990_b0_egress_policy_actions_map();
exit:
    SHR_FUNC_EXIT();
}
