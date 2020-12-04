/*! \file bcm56990_b0_fp_device_fp_info.c
 *
 * API to initialize FIDs for the group info logical tables for the device
 * Tomahawk-4(bcm56990_b0).
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
#include <bcmfp/bcmfp_info_tbl_map.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int bcmfp_bcm56990_b0_ingress_group_info_qualifiers_map(int);
static int bcmfp_bcm56990_b0_egress_group_info_qualifiers_map(int);
static int bcmfp_bcm56990_b0_lookup_group_info_qualifiers_map(int);
static int bcmfp_bcm56990_b0_em_group_info_qualifiers_map(int);
static int bcmfp_bcm56990_b0_em_group_info_actions_map(int);
static int bcmfp_bcm56990_b0_ingress_pse_info_qualifiers_map(int unit);
static int bcmfp_bcm56990_b0_em_pse_info_qualifiers_map(int unit);

bcmfp_tbl_offset_info_t bcm56990_b0_ingress_group_info_qualifiers_fid_info[FP_ING_GRP_TEMPLATEt_FIELD_COUNT];
bcmfp_tbl_offset_info_t bcm56990_b0_egress_group_info_qualifiers_fid_info[FP_EGR_GRP_TEMPLATEt_FIELD_COUNT];
bcmfp_tbl_offset_info_t bcm56990_b0_lookup_group_info_qualifiers_fid_info[FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT];
bcmfp_tbl_offset_info_t bcm56990_b0_em_group_info_qualifiers_fid_info[FP_EM_GRP_TEMPLATEt_FIELD_COUNT];
bcmfp_tbl_offset_info_t bcm56990_b0_em_group_info_actions_fid_info[FP_EM_POLICY_TEMPLATEt_FIELD_COUNT];
bcmfp_tbl_offset_info_t bcm56990_b0_ingress_pse_info_qualifiers_fid_info[FP_ING_PRESEL_ENTRY_TEMPLATEt_FIELD_COUNT];
bcmfp_tbl_offset_info_t bcm56990_b0_em_pse_info_qualifiers_fid_info[FP_ING_PRESEL_ENTRY_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_presel_info_t bcm56990_b0_ingress_pse_info_tbl = {
    .sid = FP_ING_PRESEL_TEMPLATE_PARTITION_INFOt,
    .qual_fid_info = bcm56990_b0_ingress_pse_info_qualifiers_fid_info,
    .fid_count = FP_ING_PRESEL_TEMPLATE_PARTITION_INFOt_FIELD_COUNT,
};

bcmfp_tbl_presel_info_t bcm56990_b0_em_pse_info_tbl = {
    .sid = FP_EM_PRESEL_TEMPLATE_PARTITION_INFOt,
    .qual_fid_info = bcm56990_b0_em_pse_info_qualifiers_fid_info,
    .fid_count = FP_EM_PRESEL_TEMPLATE_PARTITION_INFOt_FIELD_COUNT,
};

bcmfp_tbl_group_partition_info_t bcm56990_b0_ingress_group_part_info_tbl = {
    .sid = FP_ING_GRP_TEMPLATE_PARTITION_INFOt,
    .group_key_fid = FP_ING_GRP_TEMPLATE_PARTITION_INFOt_FP_ING_GRP_TEMPLATE_PARTITION_INFO_IDf,
    .part_key_fid = FP_ING_GRP_TEMPLATE_PARTITION_INFOt_PARTITION_IDf,
    .num_slice_fid = FP_ING_GRP_TEMPLATE_PARTITION_INFOt_NUM_SLICE_IDf,
    .slice_fid = FP_ING_GRP_TEMPLATE_PARTITION_INFOt_SLICE_IDf,
    .qual_fid_info = bcm56990_b0_ingress_group_info_qualifiers_fid_info,
    .fid_count = FP_ING_GRP_TEMPLATE_PARTITION_INFOt_FIELD_COUNT,
};

bcmfp_tbl_group_partition_info_t bcm56990_b0_egress_group_part_info_tbl = {
    .sid = FP_EGR_GRP_TEMPLATE_PARTITION_INFOt,
    .group_key_fid = FP_EGR_GRP_TEMPLATE_PARTITION_INFOt_FP_EGR_GRP_TEMPLATE_PARTITION_INFO_IDf,
    .part_key_fid = FP_EGR_GRP_TEMPLATE_PARTITION_INFOt_PARTITION_IDf,
    .num_slice_fid = FP_EGR_GRP_TEMPLATE_PARTITION_INFOt_NUM_SLICE_IDf,
    .slice_fid = FP_EGR_GRP_TEMPLATE_PARTITION_INFOt_SLICE_IDf,
    .virtual_slice_fid = FP_EGR_GRP_TEMPLATE_PARTITION_INFOt_VIRTUAL_SLICE_IDf,
    .qual_fid_info = bcm56990_b0_egress_group_info_qualifiers_fid_info,
    .fid_count = FP_EGR_GRP_TEMPLATE_PARTITION_INFOt_FIELD_COUNT,
};

bcmfp_tbl_group_partition_info_t bcm56990_b0_lookup_group_part_info_tbl = {
    .sid = FP_VLAN_GRP_TEMPLATE_PARTITION_INFOt,
    .group_key_fid = FP_VLAN_GRP_TEMPLATE_PARTITION_INFOt_FP_VLAN_GRP_TEMPLATE_PARTITION_INFO_IDf,
    .part_key_fid = FP_VLAN_GRP_TEMPLATE_PARTITION_INFOt_PARTITION_IDf,
    .num_slice_fid = FP_VLAN_GRP_TEMPLATE_PARTITION_INFOt_NUM_SLICE_IDf,
    .slice_fid = FP_VLAN_GRP_TEMPLATE_PARTITION_INFOt_SLICE_IDf,
    .virtual_slice_fid = FP_VLAN_GRP_TEMPLATE_PARTITION_INFOt_VIRTUAL_SLICE_IDf,
    .qual_fid_info = bcm56990_b0_lookup_group_info_qualifiers_fid_info,
    .fid_count = FP_VLAN_GRP_TEMPLATE_PARTITION_INFOt_FIELD_COUNT,
};

bcmfp_tbl_pdd_info_t bcm56990_b0_em_group_pdd_info_tbl = {
    .sid = FP_EM_PDD_TEMPLATE_PARTITION_INFOt,
    .pdd_key_fid = FP_EM_PDD_TEMPLATE_PARTITION_INFOt_FP_EM_PDD_TEMPLATE_PARTITION_INFO_IDf,
    .part_key_fid = FP_EM_PDD_TEMPLATE_PARTITION_INFOt_PARTITION_IDf,
    .group_key_fid = FP_EM_PDD_TEMPLATE_PARTITION_INFOt_FP_EM_GRP_TEMPLATE_IDf,
    .action_fid_info = bcm56990_b0_em_group_info_actions_fid_info,
    .fid_count = FP_EM_PDD_TEMPLATE_PARTITION_INFOt_FIELD_COUNT,
};

bcmfp_tbl_group_partition_info_t bcm56990_b0_em_group_part_info_tbl = {
    .sid = FP_EM_GRP_TEMPLATE_PARTITION_INFOt,
    .group_key_fid = FP_EM_GRP_TEMPLATE_PARTITION_INFOt_FP_EM_GRP_TEMPLATE_PARTITION_INFO_IDf,
    .part_key_fid = FP_EM_GRP_TEMPLATE_PARTITION_INFOt_PARTITION_IDf,
    .em_key_attrib_sid = FP_EM_GRP_TEMPLATE_PARTITION_INFOt_EM_KEY_ATTRIBUTE_INDEXf,
    .qual_fid_info = bcm56990_b0_em_group_info_qualifiers_fid_info,
    .fid_count = FP_EM_GRP_TEMPLATE_PARTITION_INFOt_FIELD_COUNT,
};

bcmfp_tbl_stage_info_t bcm56990_b0_ing_stage_info_tbl = {
    .sid = FP_ING_INFOt,
    .num_slices = FP_ING_INFOt_NUM_SLICESf,
    .fid_count = 3,
};

bcmfp_tbl_slice_info_t bcm56990_b0_ing_slice_info_tbl = {
    .sid = FP_ING_SLICE_INFOt,
    .key_fid = FP_ING_SLICE_INFOt_FP_ING_SLICE_IDf,
    .pipe_fid = FP_ING_SLICE_INFOt_PIPEf,
    .num_entries_total = FP_ING_SLICE_INFOt_ENTRY_MAXIMUMf,
    .num_entries_created = FP_ING_SLICE_INFOt_ENTRY_INUSE_CNTf,
    .fid_count = 4,
};

bcmfp_tbl_stage_info_t bcm56990_b0_egress_stage_info_tbl = {
    .sid = FP_EGR_INFOt,
    .num_slices = FP_EGR_INFOt_NUM_SLICESf,
    .fid_count = 3,
};

bcmfp_tbl_slice_info_t bcm56990_b0_egress_slice_info_tbl = {
    .sid = FP_EGR_SLICE_INFOt,
    .key_fid = FP_EGR_SLICE_INFOt_FP_EGR_SLICE_IDf,
    .pipe_fid = FP_EGR_SLICE_INFOt_PIPEf,
    .num_entries_total = FP_EGR_SLICE_INFOt_ENTRY_MAXIMUMf,
    .num_entries_created = FP_EGR_SLICE_INFOt_ENTRY_INUSE_CNTf,
    .fid_count = 4,
};

bcmfp_tbl_stage_info_t bcm56990_b0_lookup_stage_info_tbl = {
    .sid = FP_VLAN_INFOt,
    .num_slices = FP_VLAN_INFOt_NUM_SLICESf,
    .fid_count = 3,
};

bcmfp_tbl_slice_info_t bcm56990_b0_lookup_slice_info_tbl = {
    .sid = FP_VLAN_SLICE_INFOt,
    .key_fid = FP_VLAN_SLICE_INFOt_FP_VLAN_SLICE_IDf,
    .pipe_fid = FP_VLAN_SLICE_INFOt_PIPEf,
    .num_entries_total = FP_VLAN_SLICE_INFOt_ENTRY_MAXIMUMf,
    .num_entries_created = FP_VLAN_SLICE_INFOt_ENTRY_INUSE_CNTf,
    .fid_count = 4,
};

int
bcm56990_b0_fp_ingress_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_part_tbl = &bcm56990_b0_ingress_group_part_info_tbl;
    stage->tbls.stage_info_tbl = &bcm56990_b0_ing_stage_info_tbl;
    stage->tbls.slice_info_tbl = &bcm56990_b0_ing_slice_info_tbl;
    stage->tbls.pse_info_tbl = &bcm56990_b0_ingress_pse_info_tbl;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_ingress_group_info_qualifiers_map(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_ingress_pse_info_qualifiers_map(unit));
exit:
    SHR_FUNC_EXIT();

}

int
bcm56990_b0_fp_egress_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_part_tbl = &bcm56990_b0_egress_group_part_info_tbl;
    stage->tbls.stage_info_tbl = &bcm56990_b0_egress_stage_info_tbl;
    stage->tbls.slice_info_tbl = &bcm56990_b0_egress_slice_info_tbl;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_egress_group_info_qualifiers_map(unit));

exit:
    SHR_FUNC_EXIT();

}

int
bcm56990_b0_fp_em_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_part_tbl = &bcm56990_b0_em_group_part_info_tbl;
    stage->tbls.pdd_info_tbl = &bcm56990_b0_em_group_pdd_info_tbl;
    stage->tbls.pse_info_tbl = &bcm56990_b0_em_pse_info_tbl;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_em_group_info_actions_map(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_em_group_info_qualifiers_map(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_em_pse_info_qualifiers_map(unit));
exit:
    SHR_FUNC_EXIT();

}

int
bcm56990_b0_fp_lookup_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_part_tbl = &bcm56990_b0_lookup_group_part_info_tbl;
    stage->tbls.stage_info_tbl = &bcm56990_b0_lookup_stage_info_tbl;
    stage->tbls.slice_info_tbl = &bcm56990_b0_lookup_slice_info_tbl;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_bcm56990_b0_lookup_group_info_qualifiers_map(unit));

exit:
    SHR_FUNC_EXIT();

}


static int
bcmfp_bcm56990_b0_ingress_group_info_qualifiers_map(int unit)
{
    bcmfp_tbl_offset_info_t *qual_fid_info;
    SHR_FUNC_ENTER(unit);
    qual_fid_info = bcm56990_b0_ingress_group_info_qualifiers_fid_info;

    sal_memset(qual_fid_info, 0,
               sizeof(bcm56990_b0_ingress_group_info_qualifiers_fid_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fp_ing_grp_template_partition_info_map(unit, qual_fid_info));

exit:
    SHR_FUNC_EXIT();
}


static int
bcmfp_bcm56990_b0_egress_group_info_qualifiers_map(int unit)
{
    bcmfp_tbl_offset_info_t *qual_fid_info;
    SHR_FUNC_ENTER(unit);
    qual_fid_info = bcm56990_b0_egress_group_info_qualifiers_fid_info;

    sal_memset(qual_fid_info, 0,
               sizeof(bcm56990_b0_egress_group_info_qualifiers_fid_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fp_egr_grp_template_partition_info_map(unit, qual_fid_info));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_bcm56990_b0_lookup_group_info_qualifiers_map(int unit)
{
    bcmfp_tbl_offset_info_t *qual_fid_info;
    SHR_FUNC_ENTER(unit);
    qual_fid_info = bcm56990_b0_lookup_group_info_qualifiers_fid_info;

    sal_memset(qual_fid_info, 0,
               sizeof(bcm56990_b0_lookup_group_info_qualifiers_fid_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fp_vlan_grp_template_partition_info_map(unit, qual_fid_info));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_bcm56990_b0_em_group_info_qualifiers_map(int unit)
{
    bcmfp_tbl_offset_info_t *qual_fid_info;
    SHR_FUNC_ENTER(unit);
    qual_fid_info = bcm56990_b0_em_group_info_qualifiers_fid_info;

    sal_memset(qual_fid_info, 0,
               sizeof(bcm56990_b0_em_group_info_qualifiers_fid_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fp_em_grp_template_partition_info_map(unit, qual_fid_info));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_bcm56990_b0_em_group_info_actions_map(int unit)
{
    bcmfp_tbl_offset_info_t *act_fid_info;
    SHR_FUNC_ENTER(unit);
    act_fid_info = bcm56990_b0_em_group_info_actions_fid_info;

    sal_memset(act_fid_info, 0,
               sizeof(bcm56990_b0_em_group_info_actions_fid_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fp_em_pdd_template_partition_info_map(unit, act_fid_info));

exit:
    SHR_FUNC_EXIT();

}

static int
bcmfp_bcm56990_b0_ingress_pse_info_qualifiers_map(int unit)
{
    bcmfp_tbl_offset_info_t *qual_fid_info;
    SHR_FUNC_ENTER(unit);
    qual_fid_info = bcm56990_b0_ingress_pse_info_qualifiers_fid_info;

    sal_memset(qual_fid_info, 0,
               sizeof(bcm56990_b0_ingress_pse_info_qualifiers_fid_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fp_ing_presel_template_partition_info_map(unit, qual_fid_info));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_bcm56990_b0_em_pse_info_qualifiers_map(int unit)
{
    bcmfp_tbl_offset_info_t *qual_fid_info;
    SHR_FUNC_ENTER(unit);
    qual_fid_info = bcm56990_b0_em_pse_info_qualifiers_fid_info;

    sal_memset(qual_fid_info, 0,
               sizeof(bcm56990_b0_em_pse_info_qualifiers_fid_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fp_em_presel_template_partition_info_map(unit, qual_fid_info));

exit:
    SHR_FUNC_EXIT();
}
