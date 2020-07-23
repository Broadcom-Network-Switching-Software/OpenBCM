/*! \file bcm56996_a0_fp_vlan_action_conflict.c
 *
 *  VFP action conflict check function for Tomahawk-4GG(56996_A0).
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
bcmfp_bcm56996_a0_vlan_action_cfg_conflict_check(int unit,
                                                 bcmfp_action_t action1,
                                                 bcmfp_action_t action)
{
    uint32_t fid = 0;

    switch (action1) {
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_DROPf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_DROP_CANCELf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_DROPf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_DROP_CANCELf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_L3_TNL_TYPEf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_TNL_AUTOf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_TNL_AUTOf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_L3_TNL_TYPEf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_COPY_TO_CPUf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_COPY_TO_CPU_CANCELf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_COPY_TO_CPUf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_COPY_TO_CPU_CANCELf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_OUTER_DOT1Pf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_CPU_COSf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_INTPRIf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_CPU_COSf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_CPU_COSf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_OUTER_DOT1Pf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_INTPRIf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_VRFf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_VFIf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_ADD_INNER_TAGf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_REPLACE_INNER_TAGf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_DELETE_INNER_TAGf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_VFIf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_VRFf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_ADD_INNER_TAGf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_REPLACE_INNER_TAGf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_DELETE_INNER_TAGf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_SET_FWD_VLAN_TAGf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_REPLACE_OUTER_TAGf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_ADD_OUTER_TAGf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_ADD_OUTER_TAGf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_REPLACE_OUTER_TAGf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_SET_FWD_VLAN_TAGf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_COLORf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_DO_NOT_LEARNf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_VLAN_CLASS_0f:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_VLAN_CLASS_1f:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_COLORf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_DO_NOT_LEARNf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_VLAN_CLASS_0f;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_VLAN_CLASS_1f;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_ENABLE_VLAN_CHECKf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_DISABLE_VLAN_CHECKf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_DISABLE_VLAN_CHECKf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_ENABLE_VLAN_CHECKf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_NEW_SVPf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_DISABLE_VLAN_CHECKf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_OUTER_DOT1P_TO_INNER_DOT1Pf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_INNER_DOT1P_TO_OUTER_DOT1Pf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_OUTER_DOT1P_TO_INNER_DOT1Pf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_INNER_DOT1P_TO_OUTER_DOT1Pf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_OUTER_CFI_TO_INNER_CFIf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_INNER_CFI_TO_OUTER_CFIf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_OUTER_CFI_TO_INNER_CFIf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_INNER_CFI_TO_OUTER_CFIf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_INNER_VLANID_TO_OUTER_VLANIDf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_OUTER_VLANID_TO_INNER_VLANIDf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_OUTER_VLANID_TO_INNER_VLANIDf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_INNER_VLANID_TO_OUTER_VLANIDf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_MPLS_DISABLEf:
        case FP_VLAN_POLICY_TEMPLATEt_ACTION_MPLS_ENABLEf:
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_MPLS_DISABLEf;
            BCMFP_ACTIONS_CONFLICT(fid);
            fid = FP_VLAN_POLICY_TEMPLATEt_ACTION_MPLS_DISABLEf;
            BCMFP_ACTIONS_CONFLICT(fid);
            break;
        default:
            break;
    }
    return SHR_E_NONE;
}
