/*! \file bcm56996_a0_fp_vlan_action_db.c
 *
 * VFP action configuration DB initialization function for Tomahawk-4GG(56996_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56996_a0_vlan_action_cfg_db_init(int unit,
                                          bcmfp_stage_t *stage)
{
    uint32_t fid = 0;
    BCMFP_ACTION_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* MEMORY VFP_POLICY_TABLE */
    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_DROPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 34, 2, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_DROP_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 34, 2, 2);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_COPY_TO_CPUf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 23, 2, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_COPY_TO_CPU_CANCELf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 23, 2, 2);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_OUTER_DOT1Pf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 75, 3, -1, 21, 2, 2);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_ADD_OUTER_TAGf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 78, 12, -1, 90, 3, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_REPLACE_OUTER_TAGf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 78, 12, -1, 90, 3, 2);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_SET_FWD_VLAN_TAGf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 78, 12, -1, 90, 3, 3);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_CPU_COSf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 25, 6, -1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_INTPRIf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 70, 4, -1, 18, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_BFD_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
           0, 0, 16, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_COLORf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 68, 2, -1, 17, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_DO_NOT_LEARNf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 33, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_DISABLE_VLAN_CHECKf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 31, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_ENABLE_VLAN_CHECKf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 31, 1, 0);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_INNER_HDR_DSCP_CHANGE_DISABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 32, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_VRFf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 0, 13, -1, 36, 2, 3);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_L3_IIF_SETf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 0, 13, -1, 36, 2, 2);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_OUTER_CFIf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 74, 1, -1, 19, 2, 2);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_PRI_MODIFIERf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 13, 2, -1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_MPLS_ENABLEf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 93, 1, 1, 66, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_MPLS_DISABLEf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 93, 1, 1, 66, 1, 0);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_MPLS_TERMINATIONf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 94, 1, 1, 67, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_IPV4_TERMINATIONf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 94, 1, 1, 62, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_IPV6_TERMINATIONf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 94, 1, 1, 64, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_L3_TNL_TERMINATIONf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 94, 1, 1, 65, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_IPV4_MC_TERMINATIONf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 94, 1, 1, 61, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_IPV6_MC_TERMINATIONf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 94, 1, 1, 63, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_ARP_RARP_TERMINATIONf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 94, 1, 1, 15, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_PHB_ING_IP_DSCP_TO_INT_PRI_REMAP_IDf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 112, 1, 1, 113, 6, -1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_TNL_AUTOf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 119, 1, -1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_USE_OUTER_HDR_DSCPf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 120, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_USE_OUTER_HDR_TTLf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 121, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_VLAN_CLASS_0f;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 147, 10, -1, 123, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_VLAN_CLASS_1f;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 137, 10, -1, 122, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_VISIBILITY_ENABLEf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 165, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_MATCH_IDf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 157, 8, -1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_L3_TNL_TYPEf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 119, 1, 0, 95, 3, -1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_SVPf;
    BCMFP_ACTION_CFG_ADD_TWO(unit, stage, fid,
            0, 0, 98, 13, -1, 111, 1, 1);

    fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_VFIf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 124, 13, -1);

    fid = FP_VLAN_POLICY_TEMPLATEt_CTR_ING_EFLEX_OBJECTf;
    BCMFP_ACTION_CFG_ADD_ONE(unit, stage, fid,
            0, 0, 43, 10, -1);

exit:
    SHR_FUNC_EXIT();
}
