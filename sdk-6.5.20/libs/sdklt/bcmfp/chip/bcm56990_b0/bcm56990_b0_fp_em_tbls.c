/*! \file bcm56990_b0_fp_em_tbls.c
 *
 * API to initialize exact match FP stage logical tables and
 * some FIDs in each of those logical table for Tomahawk-4 B0(56990_B0).
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
bcm56990_b0_em_group_qualifiers_fid_info[FP_EM_GRP_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_qualifier_fid_info_t
bcm56990_b0_em_rule_qualifiers_fid_info[FP_EM_RULE_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_action_fid_info_t
bcm56990_b0_em_policy_actions_fid_info[FP_EM_POLICY_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_action_fid_info_t
bcm56990_b0_em_pdd_actions_fid_info[FP_EM_PDD_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_qualifier_fid_info_t
bcm56990_b0_em_pse_qualifiers_fid_info
                          [FP_EM_PRESEL_ENTRY_TEMPLATEt_FIELD_COUNT];

static void bcmfp_bcm56990_b0_em_group_qualifiers_map(void);
static void bcmfp_bcm56990_b0_em_rule_qualifiers_map(void);
static void bcmfp_bcm56990_b0_em_policy_actions_map(void);
static void bcmfp_bcm56990_b0_em_pdd_actions_map(void);
static void bcmfp_bcm56990_b0_em_pse_qualifiers_map(void);

bcmfp_tbl_group_t bcm56990_b0_em_group_tbl = {
    .sid = FP_EM_GRP_TEMPLATEt,
    .key_fid = FP_EM_GRP_TEMPLATEt_FP_EM_GRP_TEMPLATE_IDf,
    .default_policy_id_fid =
             FP_EM_GRP_TEMPLATEt_DEFAULT_FP_EM_POLICY_TEMPLATE_IDf,
    .default_pdd_id_fid =
             FP_EM_GRP_TEMPLATEt_DEFAULT_FP_EM_PDD_TEMPLATE_IDf,
    .default_meter_id_fid = FP_EM_GRP_TEMPLATEt_DEFAULT_METER_ING_FP_TEMPLATE_IDf,
    .default_flex_ctr_action_fid = FP_EM_GRP_TEMPLATEt_DEFAULT_CTR_ING_EFLEX_ACTIONf,
    .pipe_id_fid = FP_EM_GRP_TEMPLATEt_PIPEf,
    .priority_fid = FP_EM_GRP_TEMPLATEt_ENTRY_PRIORITYf,
    .group_ltid_auto_fid= FP_EM_GRP_TEMPLATEt_LTID_AUTOf,
    .group_ltid_fid = FP_EM_GRP_TEMPLATEt_LTIDf,
    .group_ltid_oper_fid = FP_EM_GRP_TEMPLATEt_LTID_OPERf,
    .group_lookup_id_fid = FP_EM_GRP_TEMPLATEt_LOOKUPf,
    .mode_fid = FP_EM_GRP_TEMPLATEt_MODEf,
    .mode_auto_fid = FP_EM_GRP_TEMPLATEt_MODE_AUTOf,
    .mode_oper_fid = FP_EM_GRP_TEMPLATEt_MODE_OPERf,
    .presel_ids_count_fid = FP_EM_GRP_TEMPLATEt_FP_EM_PRESEL_ENTRY_COUNTf,
    .presel_ids_fid = FP_EM_GRP_TEMPLATEt_FP_EM_PRESEL_ENTRY_TEMPLATE_IDf,
    .pdd_id_fid = FP_EM_GRP_TEMPLATEt_FP_EM_PDD_TEMPLATE_IDf,
    .presel_sid = FP_EM_PRESEL_ENTRY_TEMPLATEt,
    .presel_key_fid =
            FP_EM_PRESEL_ENTRY_TEMPLATEt_FP_EM_PRESEL_ENTRY_TEMPLATE_IDf,
    .presel_mixed_src_class_mask_fid =
            FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASS_MASKf,
    .src_class_sid = FP_EM_SRC_CLASS_MODEt,
    .qualifiers_fid_info = bcm56990_b0_em_group_qualifiers_fid_info,
    .enable_fid = FP_EM_GRP_TEMPLATEt_ENABLEf,
    .fid_count = FP_EM_GRP_TEMPLATEt_FIELD_COUNT,
};

bcmfp_tbl_group_info_t bcm56990_b0_em_group_info_tbl = {
   .sid = FP_EM_GRP_TEMPLATE_INFOt,
   .key_fid = FP_EM_GRP_TEMPLATE_INFOt_FP_EM_GRP_TEMPLATE_IDf,
   .hw_ltid =  FP_EM_GRP_TEMPLATE_INFOt_HW_LTIDf,
   .num_entries_created =  FP_EM_GRP_TEMPLATE_INFOt_NUM_ENTRIES_CREATEDf,
   .num_partition_id =  FP_EM_GRP_TEMPLATE_INFOt_NUM_PARTITION_IDf,
   .fid_count = FP_EM_GRP_TEMPLATE_INFOt_FIELD_COUNT,
};

bcmfp_tbl_rule_t bcm56990_b0_em_rule_tbl = {
    .sid = FP_EM_RULE_TEMPLATEt,
    .key_fid = FP_EM_RULE_TEMPLATEt_FP_EM_RULE_TEMPLATE_IDf,
    .qualifiers_fid_info = bcm56990_b0_em_rule_qualifiers_fid_info,
    .fid_count = FP_EM_RULE_TEMPLATEt_FIELD_COUNT,
};

bcmfp_tbl_policy_t bcm56990_b0_em_policy_tbl = {
    .sid = FP_EM_POLICY_TEMPLATEt,
    .key_fid = FP_EM_POLICY_TEMPLATEt_FP_EM_POLICY_TEMPLATE_IDf,
    .flex_ctr_r_count_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FLEX_CTR_R_COUNTf,
    .flex_ctr_y_count_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FLEX_CTR_Y_COUNTf,
    .flex_ctr_g_count_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FLEX_CTR_G_COUNTf,
    .flex_ctr_object_fid = FP_EM_POLICY_TEMPLATEt_CTR_ING_EFLEX_OBJECTf,
    .flex_ctr_container_id_fid = FP_EM_POLICY_TEMPLATEt_CTR_ING_EFLEX_CONTAINER_IDf,
    .actions_fid_info = bcm56990_b0_em_policy_actions_fid_info,
    .fid_count = FP_EM_POLICY_TEMPLATEt_FIELD_COUNT,
};

bcmfp_tbl_pdd_t bcm56990_b0_em_pdd_tbl = {
    .sid = FP_EM_PDD_TEMPLATEt,
    .key_fid = FP_EM_PDD_TEMPLATEt_FP_EM_PDD_TEMPLATE_IDf,
    .actions_fid_info = bcm56990_b0_em_pdd_actions_fid_info,
    .fid_count = FP_EM_PDD_TEMPLATEt_FIELD_COUNT,
};

bcmfp_tbl_pse_t bcm56990_b0_em_pse_tbl = {
    .sid = FP_EM_PRESEL_ENTRY_TEMPLATEt,
    .key_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_FP_EM_PRESEL_ENTRY_TEMPLATE_IDf,
    .priority_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_ENTRY_PRIORITYf,
    .fid_count = FP_EM_PRESEL_ENTRY_TEMPLATEt_FIELD_COUNT,
    .enable_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_ENABLEf,
    .qualifiers_fid_info = bcm56990_b0_em_pse_qualifiers_fid_info,
};

bcmfp_tbl_entry_t bcm56990_b0_em_entry_tbl = {
    .sid = FP_EM_ENTRYt,
    .key_fid = FP_EM_ENTRYt_FP_EM_ENTRY_IDf,
    .oprtnl_state_fid = FP_EM_ENTRYt_OPERATIONAL_STATEf,
    .group_id_fid = FP_EM_ENTRYt_FP_EM_GRP_TEMPLATE_IDf,
    .rule_id_fid = FP_EM_ENTRYt_FP_EM_RULE_TEMPLATE_IDf,
    .policy_id_fid = FP_EM_ENTRYt_FP_EM_POLICY_TEMPLATE_IDf,
    .meter_id_fid = FP_EM_ENTRYt_METER_ING_FP_TEMPLATE_IDf,
    .flex_ctr_base_idx_fid = FP_EM_ENTRYt_CTR_ING_FLEX_BASE_INDEXf,
    .flex_ctr_offset_mode_fid = FP_EM_ENTRYt_CTR_ING_FLEX_OFFSET_MODEf,
    .flex_ctr_pool_id_fid = FP_EM_ENTRYt_CTR_ING_FLEX_POOL_NUMBERf,
    .flex_ctr_action_fid = FP_EM_ENTRYt_CTR_ING_EFLEX_ACTIONf,
    .enable_fid = FP_EM_ENTRYt_ENABLEf,
    .fid_count = FP_EM_ENTRYt_FIELD_COUNT,
};

bcmfp_tbl_src_class_t bcm56990_b0_em_src_class_tbl = {
    .sid = FP_EM_SRC_CLASS_MODEt,
    .pipe_id_fid = FP_EM_SRC_CLASS_MODEt_PIPEf,
    .mode_fid = FP_EM_SRC_CLASS_MODEt_SRC_CLASS_MODEf,
    .fid_count = FP_EM_SRC_CLASS_MODEt_FIELD_COUNT,
};

bcmfp_tbl_meter_t bcm56990_b0_em_meter_tbl = {
    .sid = METER_ING_FP_TEMPLATEt,
    .key_fid = METER_ING_FP_TEMPLATEt_METER_ING_FP_TEMPLATE_IDf,
    .fid_count = METER_ING_FP_TEMPLATEt_FIELD_COUNT,
};

static void
bcmfp_bcm56990_b0_em_group_qualifiers_map(void)
{
    uint32_t fid;
    bcmfp_tbl_qualifier_fid_info_t *group_qualifiers_fid_info = NULL;

    group_qualifiers_fid_info = bcm56990_b0_em_group_qualifiers_fid_info;
    for (fid = 0; fid < FP_EM_GRP_TEMPLATEt_FIELD_COUNT; fid++) {
         group_qualifiers_fid_info[fid].qualifier = fid;
    }
    fid = FP_EM_GRP_TEMPLATEt_PIPEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_ENTRY_PRIORITYf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_ENABLEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_MODE_AUTOf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_MODEf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_MODE_OPERf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_FP_EM_PRESEL_ENTRY_COUNTf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_FP_EM_PRESEL_ENTRY_TEMPLATE_IDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_LTID_AUTOf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_LTIDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_LTID_OPERf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_LOOKUPf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_DEFAULT_FP_EM_POLICY_TEMPLATE_IDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_DEFAULT_FP_EM_PDD_TEMPLATE_IDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_FP_EM_PDD_TEMPLATE_IDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_DEFAULT_METER_ING_FP_TEMPLATE_IDf;
    group_qualifiers_fid_info[fid].qualifier = 0;
    fid = FP_EM_GRP_TEMPLATEt_DEFAULT_CTR_ING_EFLEX_ACTIONf;
    group_qualifiers_fid_info[fid].qualifier = 0;
}

static void
bcmfp_bcm56990_b0_em_rule_qualifiers_map(void)
{
    uint32_t fid;
    uint32_t map_fid;
    bcmfp_tbl_qualifier_fid_info_t *rule_qualifiers_fid_info = NULL;

    rule_qualifiers_fid_info = bcm56990_b0_em_rule_qualifiers_fid_info;

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INPORTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INPORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_OUTER_VLAN_IDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_OUTER_VLAN_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_OUTER_VLAN_PRIf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_OUTER_VLAN_PRI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_OUTER_VLAN_CFIf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_OUTER_VLAN_CFI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_SRC_MACf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_SRC_MAC_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_DST_MACf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_DST_MAC_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_SRC_IP4f;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_SRC_IP4_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_DST_IP4f;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_DST_IP4_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_SRC_IP6u_UPPERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_SRC_IP6_BITMAPu_UPPERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_DST_IP6u_UPPERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_DST_IP6_BITMAPu_UPPERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_SRC_IP6u_LOWERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_SRC_IP6_BITMAPu_LOWERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_DST_IP6u_LOWERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_DST_IP6_BITMAPu_LOWERf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_ETHERTYPEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_ETHERTYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_ICMP_TYPE_CODEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_ICMP_TYPE_CODE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_L4SRC_PORTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_L4SRC_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_L4DST_PORTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_L4DST_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_IDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_TTLf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_BOSf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_BOS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MPLS_CTRL_WORDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MPLS_CTRL_WORD_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_IP6_FLOW_LABELf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_IP6_FLOW_LABEL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_TTLf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_TNL_IP_TTLf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_TNL_IP_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_C_TNL_IP_TTLf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_C_TNL_IP_TTL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_TOSf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_TOS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_IP_PROTOCOLf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_IP_PROTOCOL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_TCP_FLAGSf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_TCP_FLAGS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_PROTOf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_PROTO_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_IP_SECOND_EH_PROTOf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_IP_SECOND_EH_PROTO_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_IP_FLAGS_MFf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_IP_FLAGS_MF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_IP_FLAGS_DFf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_IP_FLAGS_DF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_IP_FRAGf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_IP_FRAG_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_DOSATTACK_PKTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_DOSATTACK_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_L4_PKTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_L4_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_OPAQUE_TAG_LOWf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_LOW_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_OPAQUE_TAG_HIGHf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_HIGH_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INNER_L2_OPAQUE_TAGf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INNER_L2_OPAQUE_TAG_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_NON_OR_FIRST_FRAGMENTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_NON_OR_FIRST_FRAGMENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_ICMP_ERROR_PKTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_ICMP_ERROR_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_FWD_VLAN_VALIDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_FWD_VLAN_VALID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_FWD_VLAN_IDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_FWD_VLAN_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_UDF_CLASSf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_UDF_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_VRFf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_VRF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_VPNf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_VPN_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_VFIf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_VFI_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_VLAN_OUTER_PRESENTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_VLAN_OUTER_PRESENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_OPAQUE_TAG_PRESENTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_PRESENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INNER_L2_VLAN_OUTER_PRESENTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INNER_L2_VLAN_OUTER_PRESENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INNER_L2_OPAQUE_TAG_PRESENTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INNER_L2_OPAQUE_TAG_PRESENT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_BYTES_0_7_AFTER_L2HEADER_MASKf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_BYTES_0_7_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADER_MASKf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_BYTES_8_15_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_BYTES_16_23_AFTER_L2HEADERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_BYTES_16_23_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_BYTES_16_23_AFTER_L2HEADER_MASKf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_BYTES_16_23_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_BYTES_24_31_AFTER_L2HEADERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_BYTES_24_31_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_BYTES_24_31_AFTER_L2HEADER_MASKf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_BYTES_24_31_AFTER_L2HEADER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_IP_CHECKSUM_VALIDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_IP_CHECKSUM_VALID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_BFD_DISCRIMINATORf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_BFD_DISCRIMINATOR_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_PKT_LENGTHf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_PKT_LENGTH_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_RANGE_CHECKID_BMPf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_RANGE_CHECKID_BMP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_RANGE_CHECK_GRP_ID_BMPf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_RANGE_CHECK_GRP_ID_BMP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ACTIONf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MPLS_FWD_LABEL_ACTION_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_TNL_TYPEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_TNL_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_LOOPBACK_TYPEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_LOOPBACK_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_LOOPBACKf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_LOOPBACK_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);


    fid = FP_EM_RULE_TEMPLATEt_QUAL_PROTECTION_DATA_DROPf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_PROTECTION_DATA_DROP_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_IN_TNLf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_IN_TNL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_IP_TYPEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_IP_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_L2_FORMATf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_L2_FORMAT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MYSTATIONTCAM_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MYSTATIONTCAM_2_HITf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MYSTATIONTCAM_2_HIT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_L3_ROUTABLE_PKTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_L3_ROUTABLE_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INNER_L2_L3_ROUTABLE_PKTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INNER_L2_L3_ROUTABLE_PKT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INNER_TPIDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INNER_TPID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_OUTER_TPIDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_OUTER_TPID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INNER_L2_OUTER_TPIDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INNER_L2_OUTER_TPID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_FWD_TYPEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_FWD_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_ING_STP_STATEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_ING_STP_STATE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_UDF_CHUNKSf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_UDF_CHUNKS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_C_IP_PROTOCOLf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_C_IP_PROTOCOL_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_C_ETHERTYPEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_C_ETHERTYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_C_TTL0f;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_C_TTL0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_C_TTL1f;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_C_TTL1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_C_TOS0f;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_C_TOS0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_C_TOS1f;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_C_TOS1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_C_TCP_FLAGS0f;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_C_TCP_FLAGS0_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_C_TCP_FLAGS1f;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_C_TCP_FLAGS1_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_FLAGSf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_FLAGS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_MD_HDR_TYPEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_MD_HDR_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_HOP_LIMIT_CHECK_STATUSf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_HOP_LIMIT_CHECK_STATUS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_MAX_LENGTH_CHECKf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_MAX_LENGTH_CHECK_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_TYPEf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_HDR_TYPE_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_INBAND_TELEMETRY_UDP_CHECKSUM_ZEROf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_INBAND_TELEMETRY_UDP_CHECKSUM_ZERO_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_VLAN_CLASSf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_VLAN_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_SRC_PORTf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_SRC_PORT_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_L3_IIF_CLASS_IDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_L3_IIF_CLASS_ID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_L3_IIFf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_L3_IIF_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_PRESEL_CLASSf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_PRESEL_CLASS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_RAL_DETECTEDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_RAL_DETECTED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_GAL_DETECTEDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_GAL_DETECTED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_CW_DETECTEDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_MPLS_SPECIAL_LABEL_CW_DETECTED_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_RH_FIRST_4_BYTESf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_RH_FIRST_4_BYTES_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_RH_NEXT_4_BYTESf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_RH_NEXT_4_BYTES_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_VXLAN_VNIDf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_VXLAN_VNID_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_VXLAN_FLAGSf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_VXLAN_FLAGS_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_VXLAN_RSVD_24f;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_VXLAN_RSVD_24_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_VXLAN_RSVD_8f;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_VXLAN_RSVD_8_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_RTAG7A_HASH_UPPERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_RTAG7A_HASH_UPPER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_RTAG7A_HASH_LOWERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_RTAG7A_HASH_LOWER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_RTAG7B_HASH_UPPERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_RTAG7B_HASH_UPPER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_RULE_TEMPLATEt_QUAL_RTAG7B_HASH_LOWERf;
    map_fid = FP_EM_GRP_TEMPLATEt_QUAL_RTAG7B_HASH_LOWER_BITMAPf;
    rule_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(rule_qualifiers_fid_info[fid].flags.w, BCMFP_QUALIFIER_FID_FLAGS_DATA);

}

static void
bcmfp_bcm56990_b0_em_policy_actions_map(void)
{
    uint32_t fid;
    bcmfp_tbl_action_fid_info_t *policy_actions_fid_info = NULL;

    policy_actions_fid_info = bcm56990_b0_em_policy_actions_fid_info;
    for (fid = 0; fid < FP_EM_POLICY_TEMPLATEt_FIELD_COUNT; fid++) {
        policy_actions_fid_info[fid].action = fid;
    }
    fid = FP_EM_POLICY_TEMPLATEt_ACTION_MIRROR_INSTANCE_IDf;
    policy_actions_fid_info[fid].action = FP_EM_POLICY_TEMPLATEt_FIELD_COUNT;

    fid = FP_EM_POLICY_TEMPLATEt_ACTION_MIRROR_INSTANCE_ENABLEf;
    policy_actions_fid_info[fid].action = FP_EM_POLICY_TEMPLATEt_FIELD_COUNT + 4;
}

static void
bcmfp_bcm56990_b0_em_pdd_actions_map(void)
{
    uint32_t fid;
    uint32_t map_fid;
    bcmfp_tbl_action_fid_info_t *policy_actions_fid_info = NULL;
    bcmfp_tbl_action_fid_info_t *pdd_actions_fid_info = NULL;

    policy_actions_fid_info = bcm56990_b0_em_policy_actions_fid_info;
    pdd_actions_fid_info = bcm56990_b0_em_pdd_actions_fid_info;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_METER_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_METER_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_CTR_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_CTR_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_R_DROPf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_R_DROPf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_Y_DROPf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_Y_DROPf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_G_DROPf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_G_DROPf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_MIRROR_INSTANCE_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_MIRROR_INSTANCE_ENABLEf;
    map_fid = policy_actions_fid_info[map_fid].action;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_MIRROR_INSTANCE_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_MIRROR_INSTANCE_IDf;
    map_fid = policy_actions_fid_info[map_fid].action;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_MATCH_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_MATCH_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_R_COPY_TO_CPUf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_R_COPY_TO_CPUf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_R_COPY_TO_CPU_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_R_COPY_TO_CPU_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_R_SWITCH_TO_CPU_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_R_SWITCH_TO_CPU_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_R_SWITCH_TO_CPU_REINSATEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_R_SWITCH_TO_CPU_REINSATEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_R_COPY_AND_SWITCH_TO_CPU_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_R_COPY_AND_SWITCH_TO_CPU_CANCELf;
    pdd_actions_fid_info[fid].action = map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_R_COPY_TO_CPU_WITH_TIMESTAMPf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_R_COPY_TO_CPU_WITH_TIMESTAMPf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_Y_COPY_TO_CPUf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_Y_COPY_TO_CPUf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_Y_COPY_TO_CPU_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_Y_COPY_TO_CPU_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_Y_SWITCH_TO_CPU_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_Y_SWITCH_TO_CPU_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_Y_SWITCH_TO_CPU_REINSATEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_Y_SWITCH_TO_CPU_REINSATEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_Y_COPY_AND_SWITCH_TO_CPU_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_Y_COPY_AND_SWITCH_TO_CPU_CANCELf;
    pdd_actions_fid_info[fid].action = map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_Y_COPY_TO_CPU_WITH_TIMESTAMPf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_Y_COPY_TO_CPU_WITH_TIMESTAMPf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_G_COPY_TO_CPUf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_G_COPY_TO_CPUf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_G_COPY_TO_CPU_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_G_COPY_TO_CPU_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_G_SWITCH_TO_CPU_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_G_SWITCH_TO_CPU_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_G_SWITCH_TO_CPU_REINSATEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_G_SWITCH_TO_CPU_REINSATEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_G_COPY_AND_SWITCH_TO_CPU_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_G_COPY_AND_SWITCH_TO_CPU_CANCELf;
    pdd_actions_fid_info[fid].action = map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_G_COPY_TO_CPU_WITH_TIMESTAMPf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_G_COPY_TO_CPU_WITH_TIMESTAMPf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DO_NOT_CUT_THROUGHf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DO_NOT_CUT_THROUGHf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DO_NOT_CHANGE_TTLf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DO_NOT_CHANGE_TTLf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_CHANGE_PKT_L2_FIELDSf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_CHANGE_PKT_L2_FIELDSf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_CHANGE_PKT_L2_FIELDS_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_CHANGE_PKT_L2_FIELDS_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_SWITCH_TO_L3UCf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_SWITCH_TO_L3UCf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_SWITCH_TO_ECMPf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_SWITCH_TO_ECMPf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_L3_SWITCH_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_L3_SWITCH_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_ING_CLASS_ID_SELECTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_ING_CLASS_ID_SELECTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_FP_ING_CLASS_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FP_ING_CLASS_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_NEW_CPU_COSf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_NEW_CPU_COSf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_NEW_SERVICE_POOL_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_NEW_SERVICE_POOL_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_R_DROP_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_R_DROP_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_Y_DROP_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_Y_DROP_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_G_DROP_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_G_DROP_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_MIRROR_OVERRIDEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_MIRROR_OVERRIDEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_SFLOW_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_SFLOW_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_MIRROR_ING_FLEX_SFLOW_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_MIRROR_ING_FLEX_SFLOW_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_MIRROR_FLOW_CLASS_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_MIRROR_FLOW_CLASS_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_MIRROR_FLOW_CLASSf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_MIRROR_FLOW_CLASSf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_VISIBILITY_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_VISIBILITY_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_TO_PORTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_TO_PORTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_TO_TRUNKf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_TO_TRUNKf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_UNMODIFIED_REDIRECT_TO_PORTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_UNMODIFIED_REDIRECT_TO_PORTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_UNMODIFIED_REDIRECT_TO_TRUNKf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_UNMODIFIED_REDIRECT_TO_TRUNKf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_TO_NHOPf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_TO_NHOPf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_TO_ECMPf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_TO_ECMPf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_UC_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_UC_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_PORTS_BROADCAST_PKTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_PORTS_BROADCAST_PKTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_ADD_PORTS_BROADCASTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_ADD_PORTS_BROADCASTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_REMOVE_PORTS_BROADCASTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_REMOVE_PORTS_BROADCASTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_PORTS_VLAN_BROADCASTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_PORTS_VLAN_BROADCASTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_PORTS_VLAN_BROADCAST_FP_INGf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_PORTS_VLAN_BROADCAST_FP_INGf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_L2_MC_GROUP_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_L2_MC_GROUP_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_REDIRECT_L3_MC_NHOP_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_REDIRECT_L3_MC_NHOP_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_FP_ING_REMOVE_REDIRECT_DATA_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FP_ING_REMOVE_REDIRECT_DATA_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DISABLE_SRC_PRUNINGf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DISABLE_SRC_PRUNINGf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_FP_ING_ADD_REDIRECT_DATA_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FP_ING_ADD_REDIRECT_DATA_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_ING_TIMESTAMP_INSERTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_ING_TIMESTAMP_INSERTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_ING_TIMESTAMP_INSERT_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_ING_TIMESTAMP_INSERT_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_EGR_TIMESTAMP_INSERTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_EGR_TIMESTAMP_INSERTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_EGR_TIMESTAMP_INSERT_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_EGR_TIMESTAMP_INSERT_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DISABLE_SRC_PRUNINGf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DISABLE_SRC_PRUNINGf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_FLEX_CTR_R_COUNTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FLEX_CTR_R_COUNTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_FLEX_CTR_Y_COUNTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FLEX_CTR_Y_COUNTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_FLEX_CTR_G_COUNTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FLEX_CTR_G_COUNTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_CTR_ING_EFLEX_OBJECTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_CTR_ING_EFLEX_OBJECTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_CTR_ING_EFLEX_CONTAINER_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_CTR_ING_EFLEX_CONTAINER_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_ETRAP_COLOR_DISABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_ETRAP_COLOR_DISABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_ETRAP_COLOR_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_ETRAP_COLOR_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_ETRAP_LOOKUP_DISABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_ETRAP_LOOKUP_DISABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_ETRAP_LOOKUP_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_ETRAP_LOOKUP_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_ETRAP_QUEUE_DISABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_ETRAP_QUEUE_DISABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_ETRAP_QUEUE_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_ETRAP_QUEUE_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DLB_ECMP_MONITOR_DISABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DLB_ECMP_MONITOR_DISABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DLB_ECMP_MONITOR_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DLB_ECMP_MONITOR_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_MIRROR_EGR_ZERO_PAYLOAD_PROFILE_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_MIRROR_EGR_ZERO_PAYLOAD_PROFILE_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_FP_DELAYED_DROP_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FP_DELAYED_DROP_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DELAYED_DROP_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DELAYED_DROP_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_FP_DELAYED_REDIRECT_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_FP_DELAYED_REDIRECT_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DELAYED_REDIRECT_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DELAYED_REDIRECT_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DELAYED_REDIRECT_PORTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DELAYED_REDIRECT_PORTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_ECMP_HASHf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_ECMP_HASHf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_INBAND_TELEMETRY_ENCAP_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_ENCAP_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_INBAND_TELEMETRY_ENCAP_DISABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_ENCAP_DISABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_INBAND_TELEMETRY_PROFILE_IDXf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_PROFILE_IDXf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_INBAND_TELEMETRY_SAMPLE_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_SAMPLE_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_INBAND_TELEMETRY_FLOW_CLASS_IDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_FLOW_CLASS_IDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_INBAND_TELEMETRY_TERM_ACTION_PROFILE_IDXf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_TERM_ACTION_PROFILE_IDXf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_INBAND_TELEMETRY_EGR_LOOPBACK_PROFILE_IDXf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_EGR_LOOPBACK_PROFILE_IDXf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_INBAND_TELEMETRY_MMU_STAT_REQ_0f;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_MMU_STAT_REQ_0f;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_INBAND_TELEMETRY_MMU_STAT_REQ_1f;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_INBAND_TELEMETRY_MMU_STAT_REQ_1f;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_NEW_SERVICE_POOL_PRECEDENCEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_NEW_SERVICE_POOL_PRECEDENCEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DGMf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DGMf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DGM_BIASf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DGM_BIASf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DGM_THRESHOLDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DGM_THRESHOLDf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DGM_COSTf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DGM_COSTf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_PROTECTION_SWITCHING_DROP_OVERRIDEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_PROTECTION_SWITCHING_DROP_OVERRIDEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DYNAMIC_ECMP_CANCELf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DYNAMIC_ECMP_CANCELf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DYNAMIC_ECMP_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DYNAMIC_ECMP_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DCN_ENABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DCN_ENABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_DCN_DISABLEf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_DCN_DISABLEf;
    pdd_actions_fid_info[fid].action= map_fid;

    fid = FP_EM_PDD_TEMPLATEt_ACTION_GREEN_TO_PIDf;
    map_fid = FP_EM_POLICY_TEMPLATEt_ACTION_GREEN_TO_PIDf;
    pdd_actions_fid_info[fid].action= map_fid;
}

static void
bcmfp_bcm56990_b0_em_pse_qualifiers_map(void)
{
    uint32_t fid;
    uint32_t map_fid;
    bcmfp_tbl_qualifier_fid_info_t *pse_qualifiers_fid_info = NULL;

    pse_qualifiers_fid_info =
        bcm56990_b0_em_pse_qualifiers_fid_info;

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_IP_TYPEf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_IP_TYPEf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_L4_PKTf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_L4_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_L4_PKT_MASKf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_L4_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_TNL_TYPEf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_TNL_TYPEf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_HIT_MASKf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HIT_MASKf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HIT_MASKf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_DOSATTACK_PKTf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_DOSATTACK_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_DOSATTACK_PKT_MASKf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_DOSATTACK_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_ING_STP_STATEf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_ING_STP_STATEf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_VLAN_VALIDf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_VLAN_VALIDf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_VLAN_VALID_MASKf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_VLAN_VALIDf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_TYPEf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_TYPEf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_DROP_PKTf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_DROP_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_DROP_PKT_MASKf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_DROP_PKTf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASSf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASSf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
                BCMFP_QUALIFIER_FID_FLAGS_DATA);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASS_MASKf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASSf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_MASK);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_PRESEL_CLASSf;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_PRESEL_CLASSf;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_ACTION);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L2f;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L2f;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_ACTION);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L3_L4f;
    map_fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_NORMALIZE_L3_L4f;
    pse_qualifiers_fid_info[fid].qualifier = map_fid;
    SHR_BITSET(pse_qualifiers_fid_info[fid].flags.w,
               BCMFP_QUALIFIER_FID_FLAGS_ACTION);
}

int
bcmfp_bcm56990_b0_em_tbls_init(int unit,
                               bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_tbl = &bcm56990_b0_em_group_tbl;
    stage->tbls.group_info_tbl = &bcm56990_b0_em_group_info_tbl;
    stage->tbls.rule_tbl = &bcm56990_b0_em_rule_tbl;
    stage->tbls.policy_tbl = &bcm56990_b0_em_policy_tbl;
    stage->tbls.entry_tbl = &bcm56990_b0_em_entry_tbl;
    stage->tbls.pse_tbl = &bcm56990_b0_em_pse_tbl;
    stage->tbls.pdd_tbl = &bcm56990_b0_em_pdd_tbl;
    stage->tbls.src_class_tbl = &bcm56990_b0_em_src_class_tbl;
    stage->tbls.meter_tbl = &bcm56990_b0_em_meter_tbl;
    (void)bcmfp_bcm56990_b0_em_group_qualifiers_map();
    (void)bcmfp_bcm56990_b0_em_rule_qualifiers_map();
    (void)bcmfp_bcm56990_b0_em_policy_actions_map();
    (void)bcmfp_bcm56990_b0_em_pdd_actions_map();
    (void)bcmfp_bcm56990_b0_em_pse_qualifiers_map();

exit:
    SHR_FUNC_EXIT();
}
