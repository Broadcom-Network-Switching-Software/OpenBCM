/*! \file bcm56990_a0_fp_device_fp_info.c
 *
 * API to initialize FIDs for the group info logical tables for the device
 * Tomahawk-4(bcm56990_a0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56990_a0_fp.h>
#include <bcmfp/bcmfp_tbl_internal.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static void bcmfp_bcm56990_a0_ingress_group_info_qualifiers_map(void);
static void bcmfp_bcm56990_a0_em_group_info_actions_map(void);

bcmfp_tbl_offset_info_t bcm56990_a0_ingress_group_info_qualifiers_fid_info[FP_ING_GRP_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_offset_info_t bcm56990_a0_em_group_info_actions_fid_info[FP_EM_POLICY_TEMPLATEt_FIELD_COUNT];

bcmfp_tbl_group_partition_info_t bcm56990_a0_ingress_group_part_info_tbl = {
    .sid = FP_ING_GRP_TEMPLATE_PARTITION_INFOt,
    .group_key_fid = 0,
    .part_key_fid = 1,
    .num_slice_fid = 2,
    .slice_fid = 3,
    .qual_fid_info = bcm56990_a0_ingress_group_info_qualifiers_fid_info,
    .fid_count = 4,
};

bcmfp_tbl_group_partition_info_t bcm56990_a0_egress_group_part_info_tbl = {
    .sid = FP_EGR_GRP_TEMPLATE_PARTITION_INFOt,
    .group_key_fid = 0,
    .part_key_fid = 1,
    .num_slice_fid = 2,
    .slice_fid = 3,
    .virtual_slice_fid = 4,
    .qual_fid_info = NULL,
    .fid_count = 5,
};

bcmfp_tbl_group_partition_info_t bcm56990_a0_lookup_group_part_info_tbl = {
    .sid = FP_VLAN_GRP_TEMPLATE_PARTITION_INFOt,
    .group_key_fid = 0,
    .part_key_fid = 1,
    .num_slice_fid = 2,
    .slice_fid = 3,
    .virtual_slice_fid = 4,
    .qual_fid_info = NULL,
    .fid_count = 5,
};

bcmfp_tbl_pdd_info_t bcm56990_a0_em_group_pdd_info_tbl = {
    .sid = FP_EM_PDD_TEMPLATE_PARTITION_INFOt,
    .pdd_key_fid = 0,
    .part_key_fid = 1,
    .action_fid_info = bcm56990_a0_em_group_info_actions_fid_info,
    .fid_count = 12,
};

bcmfp_tbl_group_partition_info_t bcm56990_a0_em_group_part_info_tbl = {
    .sid = FP_EM_GRP_TEMPLATE_PARTITION_INFOt,
    .group_key_fid = 0,
    .part_key_fid = 1,
    .qual_fid_info = NULL,
    .em_key_attrib_sid = 30,
    .fid_count = 35,
};

bcmfp_tbl_stage_info_t bcm56990_a0_ing_stage_info_tbl = {
    .sid = FP_ING_INFOt,
    .num_slices = FP_ING_INFOt_NUM_SLICESf,
    .fid_count = 3,
};

bcmfp_tbl_slice_info_t bcm56990_a0_ing_slice_info_tbl = {
    .sid = FP_ING_SLICE_INFOt,
    .key_fid = FP_ING_SLICE_INFOt_FP_ING_SLICE_IDf,
    .pipe_fid = FP_ING_SLICE_INFOt_PIPEf,
    .num_entries_total = FP_ING_SLICE_INFOt_ENTRY_MAXIMUMf,
    .num_entries_created = FP_ING_SLICE_INFOt_ENTRY_INUSE_CNTf,
    .fid_count = 4,
};

bcmfp_tbl_stage_info_t bcm56990_a0_egress_stage_info_tbl = {
    .sid = FP_EGR_INFOt,
    .num_slices = FP_EGR_INFOt_NUM_SLICESf,
    .fid_count = 3,
};

bcmfp_tbl_slice_info_t bcm56990_a0_egress_slice_info_tbl = {
    .sid = FP_EGR_SLICE_INFOt,
    .key_fid = FP_EGR_SLICE_INFOt_FP_EGR_SLICE_IDf,
    .pipe_fid = FP_EGR_SLICE_INFOt_PIPEf,
    .num_entries_total = FP_EGR_SLICE_INFOt_ENTRY_MAXIMUMf,
    .num_entries_created = FP_EGR_SLICE_INFOt_ENTRY_INUSE_CNTf,
    .fid_count = 4,
};

bcmfp_tbl_stage_info_t bcm56990_a0_lookup_stage_info_tbl = {
    .sid = FP_VLAN_INFOt,
    .num_slices = FP_VLAN_INFOt_NUM_SLICESf,
    .fid_count = 3,
};

bcmfp_tbl_slice_info_t bcm56990_a0_lookup_slice_info_tbl = {
    .sid = FP_VLAN_SLICE_INFOt,
    .key_fid = FP_VLAN_SLICE_INFOt_FP_VLAN_SLICE_IDf,
    .pipe_fid = FP_VLAN_SLICE_INFOt_PIPEf,
    .num_entries_total = FP_VLAN_SLICE_INFOt_ENTRY_MAXIMUMf,
    .num_entries_created = FP_VLAN_SLICE_INFOt_ENTRY_INUSE_CNTf,
    .fid_count = 4,
};

int
bcm56990_a0_fp_ingress_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_part_tbl = &bcm56990_a0_ingress_group_part_info_tbl;
    stage->tbls.stage_info_tbl = &bcm56990_a0_ing_stage_info_tbl;
    stage->tbls.slice_info_tbl = &bcm56990_a0_ing_slice_info_tbl;
    (void)bcmfp_bcm56990_a0_ingress_group_info_qualifiers_map();

exit:
    SHR_FUNC_EXIT();

}

int
bcm56990_a0_fp_egress_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_part_tbl = &bcm56990_a0_egress_group_part_info_tbl;
    stage->tbls.stage_info_tbl = &bcm56990_a0_egress_stage_info_tbl;
    stage->tbls.slice_info_tbl = &bcm56990_a0_egress_slice_info_tbl;

exit:
    SHR_FUNC_EXIT();

}

int
bcm56990_a0_fp_em_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_part_tbl = &bcm56990_a0_em_group_part_info_tbl;
    stage->tbls.pdd_info_tbl = &bcm56990_a0_em_group_pdd_info_tbl;
    (void)bcmfp_bcm56990_a0_em_group_info_actions_map();

exit:
    SHR_FUNC_EXIT();

}

int
bcm56990_a0_fp_lookup_info_tbls_init(int unit,
                                      bcmfp_stage_t *stage)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    stage->tbls.group_part_tbl = &bcm56990_a0_lookup_group_part_info_tbl;
    stage->tbls.stage_info_tbl = &bcm56990_a0_lookup_stage_info_tbl;
    stage->tbls.slice_info_tbl = &bcm56990_a0_lookup_slice_info_tbl;

exit:
    SHR_FUNC_EXIT();

}


static void
bcmfp_bcm56990_a0_ingress_group_info_qualifiers_map(void)
{
    bcmfp_tbl_offset_info_t *qual_fid_info;
    qual_fid_info = bcm56990_a0_ingress_group_info_qualifiers_fid_info;

    sal_memset(qual_fid_info, 0,
               sizeof(bcm56990_a0_ingress_group_info_qualifiers_fid_info));

    qual_fid_info[17].offset_fid
     = 5;
    qual_fid_info[17].width_fid
     = 6;
    qual_fid_info[17].order_fid
     = 7;
    qual_fid_info[17].count_fid
     = 4;
}


static void
bcmfp_bcm56990_a0_em_group_info_actions_map(void)
{
    bcmfp_tbl_offset_info_t *act_fid_info;
    act_fid_info = bcm56990_a0_em_group_info_actions_fid_info;

    sal_memset(act_fid_info, 0,
               sizeof(bcm56990_a0_em_group_info_actions_fid_info));

    /* copy to cpu */
    act_fid_info[49].offset_fid
    = 3;
    act_fid_info[49].width_fid
    = 4;
    act_fid_info[49].count_fid
    = 2;

}

