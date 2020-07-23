/*! \file bcm56880_a0_dna_4_6_6_sbr.c
 *
 * Management of strength profile table and strength field for strength
 * based-resolution (SBR) in BCM56880_A0 DNA_4_6_6.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/sbr.h>
#include <bcm_int/ltsw/sbr_int.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/* Fields for DESTINATION_FP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t destination_fp_strength_profile_flds_0[] = {
    {
        .name = CNG_STRENGTHs,
        .val  = 0
    },
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 4
    },
    {
        .name = IFP_OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 0
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 4
    }
};

/* Fields for DESTINATION_FP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t destination_fp_strength_profile_flds_1[] = {
    {
        .name = CNG_STRENGTHs,
        .val  = 4
    },
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 0
    },
    {
        .name = IFP_OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 0
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 0
    }
};

/* Fields for DESTINATION_FP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t destination_fp_strength_profile_flds_2[] = {
    {
        .name = CNG_STRENGTHs,
        .val  = 4
    },
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 4
    },
    {
        .name = IFP_OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 4
    }
};

/* Fields for DESTINATION_FP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t destination_fp_strength_profile_flds_3[] = {
    {
        .name = CNG_STRENGTHs,
        .val  = 0
    },
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 0
    },
    {
        .name = IFP_OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 0
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 0
    }
};

/* Fields for DESTINATION_FP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t destination_fp_strength_profile_flds_4[] = {
    {
        .name = CNG_STRENGTHs,
        .val  = 0
    },
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 0
    },
    {
        .name = IFP_OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 0
    }
};

/* Fields for EGR_DVP_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_dvp_str_profile_flds_0[] = {
    {
        .name = CLASSID_STRs,
        .val  = 0
    },
    {
        .name = DIP_LOW_STRs,
        .val  = 0
    },
    {
        .name = OBJ_TABLE_SEL_1s,
        .val  = 0
    },
    {
        .name = TUNNEL_IDX_STRs,
        .val  = 0
    },
    {
        .name = VC_INDEX_STRs,
        .val  = 0
    }
};

/* Fields for EGR_DVP_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_dvp_str_profile_flds_1[] = {
    {
        .name = CLASSID_STRs,
        .val  = 0
    },
    {
        .name = DIP_LOW_STRs,
        .val  = 3
    },
    {
        .name = OBJ_TABLE_SEL_1s,
        .val  = 6
    },
    {
        .name = TUNNEL_IDX_STRs,
        .val  = 6
    },
    {
        .name = VC_INDEX_STRs,
        .val  = 6
    }
};

/* Fields for EGR_DVP_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_dvp_str_profile_flds_2[] = {
    {
        .name = CLASSID_STRs,
        .val  = 6
    },
    {
        .name = DIP_LOW_STRs,
        .val  = 3
    },
    {
        .name = OBJ_TABLE_SEL_1s,
        .val  = 6
    },
    {
        .name = TUNNEL_IDX_STRs,
        .val  = 6
    },
    {
        .name = VC_INDEX_STRs,
        .val  = 6
    }
};

/* Fields for EGR_L2_OIF_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l2_oif_str_profile_flds_0[] = {
    {
        .name = TOS_PTR_STRs,
        .val  = 0
    }
};

/* Fields for EGR_L2_OIF_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l2_oif_str_profile_flds_1[] = {
    {
        .name = TOS_PTR_STRs,
        .val  = 3
    }
};

/* Fields for EGR_L3_NEXT_HOP_1_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_next_hop_1_str_profile_flds_0[] = {
    {
        .name = CLASSID_STRs,
        .val  = 0
    },
    {
        .name = MAC_DA_STRs,
        .val  = 0
    }
};

/* Fields for EGR_L3_NEXT_HOP_1_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_next_hop_1_str_profile_flds_1[] = {
    {
        .name = CLASSID_STRs,
        .val  = 4
    },
    {
        .name = MAC_DA_STRs,
        .val  = 0
    }
};

/* Fields for EGR_L3_NEXT_HOP_1_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_next_hop_1_str_profile_flds_2[] = {
    {
        .name = CLASSID_STRs,
        .val  = 0
    },
    {
        .name = MAC_DA_STRs,
        .val  = 3
    }
};

/* Fields for EGR_L3_NEXT_HOP_1_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_next_hop_1_str_profile_flds_3[] = {
    {
        .name = CLASSID_STRs,
        .val  = 4
    },
    {
        .name = MAC_DA_STRs,
        .val  = 3
    }
};

/* Fields for EGR_L3_NEXT_HOP_2_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_next_hop_2_str_profile_flds_0[] = {
    {
        .name = CLASSID_STRs,
        .val  = 0
    }
};

/* Fields for EGR_L3_NEXT_HOP_2_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_next_hop_2_str_profile_flds_1[] = {
    {
        .name = CLASSID_STRs,
        .val  = 3
    }
};

/* Fields for EGR_L3_OIF_1_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_oif_1_str_profile_flds_0[] = {
    {
        .name = L3_VPN_INDEX_STRs,
        .val  = 0
    },
    {
        .name = MAC_SA_STRs,
        .val  = 0
    },
    {
        .name = OBJ_TABLE_SEL_1s,
        .val  = 0
    },
    {
        .name = TUNNEL_IDX_STRs,
        .val  = 0
    },
    {
        .name = VFI_STRs,
        .val  = 0
    }
};

/* Fields for EGR_L3_OIF_1_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_oif_1_str_profile_flds_1[] = {
    {
        .name = L3_VPN_INDEX_STRs,
        .val  = 3
    },
    {
        .name = MAC_SA_STRs,
        .val  = 0
    },
    {
        .name = OBJ_TABLE_SEL_1s,
        .val  = 3
    },
    {
        .name = TUNNEL_IDX_STRs,
        .val  = 3
    },
    {
        .name = VFI_STRs,
        .val  = 3
    }
};

/* Fields for EGR_L3_OIF_1_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_oif_1_str_profile_flds_2[] = {
    {
        .name = L3_VPN_INDEX_STRs,
        .val  = 3
    },
    {
        .name = MAC_SA_STRs,
        .val  = 3
    },
    {
        .name = OBJ_TABLE_SEL_1s,
        .val  = 3
    },
    {
        .name = TUNNEL_IDX_STRs,
        .val  = 3
    },
    {
        .name = VFI_STRs,
        .val  = 3
    }
};

/* Fields for EGR_L3_OIF_1_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_oif_1_str_profile_flds_3[] = {
    {
        .name = L3_VPN_INDEX_STRs,
        .val  = 0
    },
    {
        .name = MAC_SA_STRs,
        .val  = 3
    },
    {
        .name = OBJ_TABLE_SEL_1s,
        .val  = 3
    },
    {
        .name = TUNNEL_IDX_STRs,
        .val  = 0
    },
    {
        .name = VFI_STRs,
        .val  = 0
    }
};

/* Fields for EGR_L3_TNL_0_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_tnl_0_str_profile_flds_0[] = {
    {
        .name = TNL_WORD_1_STRs,
        .val  = 6
    }
};

/* Fields for EGR_L3_TNL_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_tnl_str_profile_flds_0[] = {
    {
        .name = IP_DIP_HI_STRs,
        .val  = 0
    },
    {
        .name = IP_DIP_STRs,
        .val  = 0
    },
    {
        .name = SEQ_NUM_STRs,
        .val  = 0
    },
    {
        .name = TNL_WORD_2_STRs,
        .val  = 0
    }
};

/* Fields for EGR_L3_TNL_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_tnl_str_profile_flds_1[] = {
    {
        .name = IP_DIP_HI_STRs,
        .val  = 3
    },
    {
        .name = IP_DIP_STRs,
        .val  = 0
    },
    {
        .name = SEQ_NUM_STRs,
        .val  = 0
    },
    {
        .name = TNL_WORD_2_STRs,
        .val  = 3
    }
};

/* Fields for EGR_L3_TNL_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_tnl_str_profile_flds_2[] = {
    {
        .name = IP_DIP_HI_STRs,
        .val  = 3
    },
    {
        .name = IP_DIP_STRs,
        .val  = 3
    },
    {
        .name = SEQ_NUM_STRs,
        .val  = 3
    },
    {
        .name = TNL_WORD_2_STRs,
        .val  = 0
    }
};

/* Fields for EGR_L3_TNL_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_l3_tnl_str_profile_flds_3[] = {
    {
        .name = IP_DIP_HI_STRs,
        .val  = 3
    },
    {
        .name = IP_DIP_STRs,
        .val  = 3
    },
    {
        .name = SEQ_NUM_STRs,
        .val  = 0
    },
    {
        .name = TNL_WORD_2_STRs,
        .val  = 0
    }
};

/* Fields for EGR_MPLS_VPN_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_mpls_vpn_str_profile_flds_0[] = {
    {
        .name = IFA_ATTR_STRs,
        .val  = 0
    }
};

/* Fields for EGR_MPLS_VPN_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_mpls_vpn_str_profile_flds_1[] = {
    {
        .name = IFA_ATTR_STRs,
        .val  = 3
    }
};

/* Fields for EGR_VFI_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_vfi_str_profile_flds_0[] = {
    {
        .name = CLASSID_STRs,
        .val  = 0
    }
};

/* Fields for EGR_VFI_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_vfi_str_profile_flds_1[] = {
    {
        .name = CLASSID_STRs,
        .val  = 3
    }
};

/* Fields for EGR_VXLT_2_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_vxlt_2_str_profile_flds_0[] = {
    {
        .name = EVXLT2_VSID_STRs,
        .val  = 3
    }
};

/* Fields for EGR_VXLT_DW_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_vxlt_dw_str_profile_flds_0[] = {
    {
        .name = DOT1P_PTR_STRs,
        .val  = 4
    },
    {
        .name = TAG_STRs,
        .val  = 3
    },
    {
        .name = VXLT_CTRL_STRs,
        .val  = 4
    }
};

/* Fields for EGR_VXLT_DW_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_vxlt_dw_str_profile_flds_1[] = {
    {
        .name = DOT1P_PTR_STRs,
        .val  = 4
    },
    {
        .name = TAG_STRs,
        .val  = 0
    },
    {
        .name = VXLT_CTRL_STRs,
        .val  = 4
    }
};

/* Fields for EGR_VXLT_DW_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_vxlt_dw_str_profile_flds_2[] = {
    {
        .name = DOT1P_PTR_STRs,
        .val  = 0
    },
    {
        .name = TAG_STRs,
        .val  = 3
    },
    {
        .name = VXLT_CTRL_STRs,
        .val  = 4
    }
};

/* Fields for EGR_VXLT_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_vxlt_str_profile_flds_0[] = {
    {
        .name = DOT1P_PTR_STRs,
        .val  = 4
    },
    {
        .name = TAG_STRs,
        .val  = 3
    },
    {
        .name = VXLT_CTRL_STRs,
        .val  = 4
    }
};

/* Fields for EGR_VXLT_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_vxlt_str_profile_flds_1[] = {
    {
        .name = DOT1P_PTR_STRs,
        .val  = 4
    },
    {
        .name = TAG_STRs,
        .val  = 0
    },
    {
        .name = VXLT_CTRL_STRs,
        .val  = 4
    }
};

/* Fields for EGR_VXLT_STR_PROFILE. */
static const
bcmint_sbr_profile_fld_t egr_vxlt_str_profile_flds_2[] = {
    {
        .name = DOT1P_PTR_STRs,
        .val  = 0
    },
    {
        .name = TAG_STRs,
        .val  = 3
    },
    {
        .name = VXLT_CTRL_STRs,
        .val  = 4
    }
};

/* Fields for ING_DVP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_dvp_strength_profile_flds_0[] = {
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 0
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 0
    },
    {
        .name = NO_CUT_THRU_STRENGTHs,
        .val  = 4
    }
};

/* Fields for ING_DVP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_dvp_strength_profile_flds_1[] = {
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 4
    },
    {
        .name = NO_CUT_THRU_STRENGTHs,
        .val  = 0
    }
};

/* Fields for ING_DVP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_dvp_strength_profile_flds_2[] = {
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 0
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 0
    },
    {
        .name = NO_CUT_THRU_STRENGTHs,
        .val  = 0
    }
};

/* Fields for ING_ECMP_MEMBER_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_ecmp_member_1_strength_profile_flds_0[] = {
    {
        .name = NHOP_INDEX_1_STRENGTHs,
        .val  = 4
    }
};

/* Fields for ING_ECMP_MEMBER_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_ecmp_member_1_strength_profile_flds_1[] = {
    {
        .name = NHOP_INDEX_1_STRENGTHs,
        .val  = 0
    }
};

/* Fields for ING_IPMC_REMAP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_ipmc_remap_strength_profile_flds_0[] = {
    {
        .name = REMAPPED_IPMC_INDEX_STRENGTHs,
        .val  = 5
    }
};

/* Fields for ING_L3_NEXT_HOP_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_l3_next_hop_1_strength_profile_flds_0[] = {
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 4
    },
    {
        .name = DVP_STRENGTHs,
        .val  = 4
    },
    {
        .name = FORWARDING_TYPE_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 4
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 4
    },
    {
        .name = SYSTEM_OPCODE_STRENGTHs,
        .val  = 4
    }
};

/* Fields for ING_L3_NEXT_HOP_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_l3_next_hop_1_strength_profile_flds_1[] = {
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 0
    },
    {
        .name = DVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = FORWARDING_TYPE_STRENGTHs,
        .val  = 0
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 0
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 0
    },
    {
        .name = SYSTEM_OPCODE_STRENGTHs,
        .val  = 0
    }
};

/* Fields for ING_L3_NEXT_HOP_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_l3_next_hop_1_strength_profile_flds_2[] = {
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 4
    },
    {
        .name = DVP_STRENGTHs,
        .val  = 4
    },
    {
        .name = FORWARDING_TYPE_STRENGTHs,
        .val  = 0
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 4
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 4
    },
    {
        .name = SYSTEM_OPCODE_STRENGTHs,
        .val  = 0
    }
};

/* Fields for ING_L3_NEXT_HOP_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_l3_next_hop_2_strength_profile_flds_0[] = {
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = DVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 5
    },
    {
        .name = MEMBERSHIP_STG_STRENGTHs,
        .val  = 0
    }
};

/* Fields for ING_L3_NEXT_HOP_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_l3_next_hop_2_strength_profile_flds_1[] = {
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = DVP_STRENGTHs,
        .val  = 5
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 5
    },
    {
        .name = MEMBERSHIP_STG_STRENGTHs,
        .val  = 5
    }
};

/* Fields for ING_L3_NEXT_HOP_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_l3_next_hop_2_strength_profile_flds_2[] = {
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 0
    },
    {
        .name = DVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 0
    },
    {
        .name = MEMBERSHIP_STG_STRENGTHs,
        .val  = 0
    }
};

/* Fields for ING_L3_NEXT_HOP_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_l3_next_hop_2_strength_profile_flds_3[] = {
    {
        .name = DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = DVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs,
        .val  = 5
    },
    {
        .name = MEMBERSHIP_STG_STRENGTHs,
        .val  = 5
    }
};

/* Fields for ING_SYSTEM_DESTINATION_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_system_destination_strength_profile_flds_0[] = {
    {
        .name = NHOP_INDEX_2_STRENGTHs,
        .val  = 0
    }
};

/* Fields for ING_SYSTEM_DESTINATION_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t ing_system_destination_strength_profile_flds_1[] = {
    {
        .name = NHOP_INDEX_2_STRENGTHs,
        .val  = 3
    }
};

/* Fields for L2_HOST_LOOKUP0_DEFAULT_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l2_host_lookup0_default_strength_profile_flds_0[] = {
    {
        .name = L2_DST_CLASS_ID_STRENGTHs,
        .val  = 1
    },
    {
        .name = L2_DST_DESTINATION_STRENGTHs,
        .val  = 1
    },
    {
        .name = L2_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 1
    },
    {
        .name = L2_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 1
    },
    {
        .name = L2_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 1
    }
};

/* Fields for L2_HOST_NARROW_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l2_host_narrow_strength_profile_flds_0[] = {
    {
        .name = L2_DST_DESTINATION_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 4
    }
};

/* Fields for L2_HOST_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l2_host_strength_profile_flds_0[] = {
    {
        .name = L2_DST_CLASS_ID_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2_DST_DESTINATION_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 4
    },
    {
        .name = L2_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 4
    }
};

/* Fields for L2_IPV4_MULTICAST_SG_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l2_ipv4_multicast_sg_strength_profile_flds_0[] = {
    {
        .name = L2_DST_CLASS_ID_STRENGTHs,
        .val  = 6
    },
    {
        .name = L2_DST_DESTINATION_STRENGTHs,
        .val  = 6
    },
    {
        .name = L2_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 6
    },
    {
        .name = L2_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 6
    },
    {
        .name = L2_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 6
    }
};

/* Fields for L2_IPV4_MULTICAST_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l2_ipv4_multicast_strength_profile_flds_0[] = {
    {
        .name = L2_DST_CLASS_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = L2_DST_DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = L2_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 5
    },
    {
        .name = L2_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 5
    },
    {
        .name = L2_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 5
    }
};

/* Fields for L2_IPV6_MULTICAST_SG_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l2_ipv6_multicast_sg_strength_profile_flds_0[] = {
    {
        .name = L2_DST_CLASS_ID_STRENGTHs,
        .val  = 6
    },
    {
        .name = L2_DST_DESTINATION_STRENGTHs,
        .val  = 6
    },
    {
        .name = L2_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 6
    },
    {
        .name = L2_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 6
    },
    {
        .name = L2_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 6
    }
};

/* Fields for L2_IPV6_MULTICAST_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l2_ipv6_multicast_strength_profile_flds_0[] = {
    {
        .name = L2_DST_CLASS_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = L2_DST_DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = L2_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 5
    },
    {
        .name = L2_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 5
    },
    {
        .name = L2_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 5
    }
};

/* Fields for L3_IIF_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_iif_strength_profile_flds_0[] = {
    {
        .name = CLASS_ID_STRENGTHs,
        .val  = 3
    },
    {
        .name = VRF_STRENGTHs,
        .val  = 3
    }
};

/* Fields for L3_IIF_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_iif_strength_profile_flds_1[] = {
    {
        .name = CLASS_ID_STRENGTHs,
        .val  = 0
    },
    {
        .name = VRF_STRENGTHs,
        .val  = 3
    }
};

/* Fields for L3_IIF_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_iif_strength_profile_flds_2[] = {
    {
        .name = CLASS_ID_STRENGTHs,
        .val  = 0
    },
    {
        .name = VRF_STRENGTHs,
        .val  = 0
    }
};

/* Fields for L3_IIF_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_iif_strength_profile_flds_3[] = {
    {
        .name = CLASS_ID_STRENGTHs,
        .val  = 3
    },
    {
        .name = VRF_STRENGTHs,
        .val  = 0
    }
};

/* Fields for L3_IPV4_ADDRESS_COMPRESSION_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv4_address_compression_strength_profile_flds_0[] = {
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 7
    },
    {
        .name = L3_SRC_CLASS_ID_STRENGTHs,
        .val  = 7
    }
};

/* Fields for L3_IPV4_ALPM_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv4_alpm_strength_profile_flds_0[] = {
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_DESTINATION_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_DESTINATION_TYPE_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_MISC_CTRL_0_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_MISC_CTRL_1_STRENGTHs,
        .val  = 2
    }
};

/* Fields for L3_IPV4_MULTICAST_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv4_multicast_1_strength_profile_flds_0[] = {
    {
        .name = EXPECTED_L3_IIF_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 5
    },
    {
        .name = RPA_ID_STRENGTHs,
        .val  = 5
    }
};

/* Fields for L3_IPV4_MULTICAST_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv4_multicast_2_strength_profile_flds_0[] = {
    {
        .name = EXPECTED_L3_IIF_STRENGTHs,
        .val  = 6
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 6
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 6
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 6
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 6
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 6
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 6
    },
    {
        .name = RPA_ID_STRENGTHs,
        .val  = 6
    }
};

/* Fields for L3_IPV4_MULTICAST_DEFIP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv4_multicast_defip_strength_profile_flds_0[] = {
    {
        .name = EXPECTED_L3_IIF_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 3
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 3
    },
    {
        .name = RPA_ID_STRENGTHs,
        .val  = 3
    }
};

/* Fields for L3_IPV4_UNICAST_DEFIP_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv4_unicast_defip_strength_profile_flds_0[] = {
    {
        .name = ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_SRC_CLASS_ID_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_SRC_DESTINATION_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_SRC_DESTINATION_TYPE_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_SRC_MISC_CTRL_0_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_SRC_MISC_CTRL_1_STRENGTHs,
        .val  = 3
    },
    {
        .name = NHOP_2_OR_ECMP_GROUP_INDEX_1_STRENGTHs,
        .val  = 3
    }
};

/* Fields for L3_IPV4_UNICAST_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv4_unicast_strength_profile_flds_0[] = {
    {
        .name = ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_SRC_CLASS_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_SRC_DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_SRC_DESTINATION_TYPE_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_SRC_MISC_CTRL_0_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_SRC_MISC_CTRL_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = NHOP_2_OR_ECMP_GROUP_INDEX_1_STRENGTHs,
        .val  = 5
    }
};

/* Fields for L3_IPV6_ADDRESS_COMPRESSION_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv6_address_compression_strength_profile_flds_0[] = {
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 7
    },
    {
        .name = L3_SRC_CLASS_ID_STRENGTHs,
        .val  = 7
    }
};

/* Fields for L3_IPV6_ALPM_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv6_alpm_strength_profile_flds_0[] = {
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_DESTINATION_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_DESTINATION_TYPE_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_MISC_CTRL_0_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_MISC_CTRL_1_STRENGTHs,
        .val  = 2
    }
};

/* Fields for L3_IPV6_MULTICAST_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv6_multicast_1_strength_profile_flds_0[] = {
    {
        .name = EXPECTED_L3_IIF_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 5
    },
    {
        .name = RPA_ID_STRENGTHs,
        .val  = 5
    }
};

/* Fields for L3_IPV6_MULTICAST_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv6_multicast_2_strength_profile_flds_0[] = {
    {
        .name = EXPECTED_L3_IIF_STRENGTHs,
        .val  = 6
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 6
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 6
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 6
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 6
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 6
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 6
    },
    {
        .name = RPA_ID_STRENGTHs,
        .val  = 6
    }
};

/* Fields for L3_IPV6_MULTICAST_DEFIP_128_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv6_multicast_defip_128_strength_profile_flds_0[] = {
    {
        .name = EXPECTED_L3_IIF_STRENGTHs,
        .val  = 4
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 4
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 4
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 4
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 4
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 4
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 4
    },
    {
        .name = RPA_ID_STRENGTHs,
        .val  = 4
    }
};

/* Fields for L3_IPV6_MULTICAST_DEFIP_64_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv6_multicast_defip_64_strength_profile_flds_0[] = {
    {
        .name = EXPECTED_L3_IIF_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 3
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 3
    },
    {
        .name = RPA_ID_STRENGTHs,
        .val  = 3
    }
};

/* Fields for L3_IPV6_UNICAST_DEFIP_128_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv6_unicast_defip_128_strength_profile_flds_0[] = {
    {
        .name = ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_SRC_CLASS_ID_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_SRC_DESTINATION_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_SRC_DESTINATION_TYPE_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_SRC_MISC_CTRL_0_STRENGTHs,
        .val  = 3
    },
    {
        .name = L3_SRC_MISC_CTRL_1_STRENGTHs,
        .val  = 3
    },
    {
        .name = NHOP_2_OR_ECMP_GROUP_INDEX_1_STRENGTHs,
        .val  = 3
    }
};

/* Fields for L3_IPV6_UNICAST_DEFIP_64_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv6_unicast_defip_64_strength_profile_flds_0[] = {
    {
        .name = ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_CLASS_ID_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_DESTINATION_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_DESTINATION_TYPE_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_MISC_CTRL_0_STRENGTHs,
        .val  = 2
    },
    {
        .name = L3_SRC_MISC_CTRL_1_STRENGTHs,
        .val  = 2
    },
    {
        .name = NHOP_2_OR_ECMP_GROUP_INDEX_1_STRENGTHs,
        .val  = 2
    }
};

/* Fields for L3_IPV6_UNICAST_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ipv6_unicast_strength_profile_flds_0[] = {
    {
        .name = ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_SRC_CLASS_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_SRC_DESTINATION_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_SRC_DESTINATION_TYPE_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_SRC_MISC_CTRL_0_STRENGTHs,
        .val  = 5
    },
    {
        .name = L3_SRC_MISC_CTRL_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = NHOP_2_OR_ECMP_GROUP_INDEX_1_STRENGTHs,
        .val  = 5
    }
};

/* Fields for L3_IP_LOOKUP0_DEFAULT_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ip_lookup0_default_strength_profile_flds_0[] = {
    {
        .name = ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX_STRENGTHs,
        .val  = 1
    },
    {
        .name = EXPECTED_L3_IIF_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 1
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 1
    },
    {
        .name = NHOP_2_OR_ECMP_GROUP_INDEX_1_STRENGTHs,
        .val  = 1
    },
    {
        .name = RPA_ID_STRENGTHs,
        .val  = 1
    }
};

/* Fields for L3_IP_LOOKUP1_DEFAULT_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t l3_ip_lookup1_default_strength_profile_flds_0[] = {
    {
        .name = EXPECTED_L3_IIF_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_DST_CLASS_ID_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_DST_DESTINATION_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_DST_DESTINATION_TYPE_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_DST_MISC_CTRL_0_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_DST_MISC_CTRL_1_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_SRC_CLASS_ID_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_SRC_DESTINATION_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_SRC_DESTINATION_TYPE_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_SRC_MISC_CTRL_0_STRENGTHs,
        .val  = 1
    },
    {
        .name = L3_SRC_MISC_CTRL_1_STRENGTHs,
        .val  = 1
    },
    {
        .name = MTU_PROFILE_PTR_STRENGTHs,
        .val  = 1
    },
    {
        .name = RPA_ID_STRENGTHs,
        .val  = 1
    }
};

/* Fields for PROTOCOL_PKT_FORWARD_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t protocol_pkt_forward_strength_profile_flds_0[] = {
    {
        .name = CLASS_ID_STRENGTHs,
        .val  = 4
    }
};

/* Fields for PROTOCOL_PKT_FORWARD_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t protocol_pkt_forward_strength_profile_flds_1[] = {
    {
        .name = CLASS_ID_STRENGTHs,
        .val  = 0
    }
};

/* Fields for SYS_PORT_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t sys_port_strength_profile_flds_0[] = {
    {
        .name = ING_PP_PORT_STRENGTHs,
        .val  = 4
    }
};

/* Fields for SYS_PORT_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t sys_port_strength_profile_flds_1[] = {
    {
        .name = ING_PP_PORT_STRENGTHs,
        .val  = 0
    }
};

/* Fields for VFI_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vfi_strength_profile_flds_0[] = {
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 3
    }
};

/* Fields for VFI_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vfi_strength_profile_flds_1[] = {
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 0
    }
};

/* Fields for VLAN_XLATE_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_1_strength_profile_flds_0[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 5
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 1
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 0
    }
};

/* Fields for VLAN_XLATE_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_1_strength_profile_flds_1[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 5
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 5
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 5
    }
};

/* Fields for VLAN_XLATE_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_1_strength_profile_flds_2[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 5
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 5
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 5
    }
};

/* Fields for VLAN_XLATE_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_1_strength_profile_flds_3[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 5
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 5
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 5
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 5
    }
};

/* Fields for VLAN_XLATE_1_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_1_strength_profile_flds_4[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 5
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 5
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 5
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 5
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 5
    }
};

/* Fields for VLAN_XLATE_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_2_strength_profile_flds_0[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 4
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 1
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 0
    }
};

/* Fields for VLAN_XLATE_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_2_strength_profile_flds_1[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 4
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 4
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 4
    }
};

/* Fields for VLAN_XLATE_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_2_strength_profile_flds_2[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 4
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 4
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 4
    }
};

/* Fields for VLAN_XLATE_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_2_strength_profile_flds_3[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 4
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 4
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 4
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 4
    }
};

/* Fields for VLAN_XLATE_2_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_2_strength_profile_flds_4[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 4
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 4
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 4
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 4
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 4
    }
};

/* Fields for VLAN_XLATE_3_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_3_strength_profile_flds_0[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 6
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 1
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 0
    }
};

/* Fields for VLAN_XLATE_3_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_3_strength_profile_flds_1[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 6
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 6
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 6
    }
};

/* Fields for VLAN_XLATE_3_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_3_strength_profile_flds_2[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 6
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 0
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 6
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 6
    }
};

/* Fields for VLAN_XLATE_3_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_3_strength_profile_flds_3[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 6
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 6
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 6
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 6
    }
};

/* Fields for VLAN_XLATE_3_STRENGTH_PROFILE. */
static const
bcmint_sbr_profile_fld_t vlan_xlate_3_strength_profile_flds_4[] = {
    {
        .name = L2_IIF_STRENGTHs,
        .val  = 0
    },
    {
        .name = L3_IIF_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_CTRL_ID_1_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_CTRL_ID_STRENGTHs,
        .val  = 6
    },
    {
        .name = OPAQUE_OBJ0_STRENGTHs,
        .val  = 6
    },
    {
        .name = SVP_STRENGTHs,
        .val  = 6
    },
    {
        .name = VFI_STRENGTHs,
        .val  = 6
    },
    {
        .name = VLAN_TAG_PRESERVE_CTRL_STRENGTHs,
        .val  = 6
    }
};

/* Entry for DESTINATION_FP_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t destination_fp_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_CNG_NO_IFP_OPAQUE_CTRL_ID,
        .key_name = DESTINATION_FP_STRENGTH_PROFILE_INDEXs,
        .fld_info = destination_fp_strength_profile_flds_0,
        .fld_num  = 4,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_DESTINATION_NO_IFP_OPAQUE_CTRL_ID_NO_L2MC_L3MC_L2_OIF_SYS_DST_VALID,
        .key_name = DESTINATION_FP_STRENGTH_PROFILE_INDEXs,
        .fld_info = destination_fp_strength_profile_flds_1,
        .fld_num  = 4,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = DESTINATION_FP_STRENGTH_PROFILE_INDEXs,
        .fld_info = destination_fp_strength_profile_flds_2,
        .fld_num  = 4,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = DESTINATION_FP_STRENGTH_PROFILE_INDEXs,
        .fld_info = destination_fp_strength_profile_flds_3,
        .fld_num  = 4,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_CNG_NO_DESTINATION_NO_L2MC_L3MC_L2_OIF_SYS_DST_VALID,
        .key_name = DESTINATION_FP_STRENGTH_PROFILE_INDEXs,
        .fld_info = destination_fp_strength_profile_flds_4,
        .fld_num  = 4,
        .dummy    = 0
    }
};

/* Entry for EGR_DVP_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_dvp_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = EGR_DVP_STR_PROFILE_INDEXs,
        .fld_info = egr_dvp_str_profile_flds_0,
        .fld_num  = 5,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_CLASSID,
        .key_name = EGR_DVP_STR_PROFILE_INDEXs,
        .fld_info = egr_dvp_str_profile_flds_1,
        .fld_num  = 5,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_DVP_STR_PROFILE_INDEXs,
        .fld_info = egr_dvp_str_profile_flds_2,
        .fld_num  = 5,
        .dummy    = 0
    }
};

/* Entry for EGR_L2_OIF_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_l2_oif_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = EGR_L2_OIF_STR_PROFILE_INDEXs,
        .fld_info = egr_l2_oif_str_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_L2_OIF_STR_PROFILE_INDEXs,
        .fld_info = egr_l2_oif_str_profile_flds_1,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for EGR_L3_NEXT_HOP_1_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_l3_next_hop_1_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = EGR_L3_NEXT_HOP_1_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_next_hop_1_str_profile_flds_0,
        .fld_num  = 2,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_MAC_DA,
        .key_name = EGR_L3_NEXT_HOP_1_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_next_hop_1_str_profile_flds_1,
        .fld_num  = 2,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_CLASSID,
        .key_name = EGR_L3_NEXT_HOP_1_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_next_hop_1_str_profile_flds_2,
        .fld_num  = 2,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_L3_NEXT_HOP_1_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_next_hop_1_str_profile_flds_3,
        .fld_num  = 2,
        .dummy    = 0
    }
};

/* Entry for EGR_L3_NEXT_HOP_2_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_l3_next_hop_2_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = EGR_L3_NEXT_HOP_2_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_next_hop_2_str_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_L3_NEXT_HOP_2_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_next_hop_2_str_profile_flds_1,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for EGR_L3_OIF_1_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_l3_oif_1_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = EGR_L3_OIF_1_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_oif_1_str_profile_flds_0,
        .fld_num  = 5,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_MAC_SA,
        .key_name = EGR_L3_OIF_1_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_oif_1_str_profile_flds_1,
        .fld_num  = 5,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_L3_OIF_1_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_oif_1_str_profile_flds_2,
        .fld_num  = 5,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L3_VPN_INDEX_NO_TUNNEL_IDX_NO_VFI,
        .key_name = EGR_L3_OIF_1_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_oif_1_str_profile_flds_3,
        .fld_num  = 5,
        .dummy    = 0
    }
};

/* Entry for EGR_L3_TNL_0_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_l3_tnl_0_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_L3_TNL_0_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_tnl_0_str_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for EGR_L3_TNL_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_l3_tnl_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = EGR_L3_TNL_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_tnl_str_profile_flds_0,
        .fld_num  = 4,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_IP_DIP_NO_SEQ_NUM,
        .key_name = EGR_L3_TNL_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_tnl_str_profile_flds_1,
        .fld_num  = 4,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_TNL_WORD_2,
        .key_name = EGR_L3_TNL_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_tnl_str_profile_flds_2,
        .fld_num  = 4,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_SEQ_NUM_NO_TNL_WORD_2,
        .key_name = EGR_L3_TNL_STR_PROFILE_INDEXs,
        .fld_info = egr_l3_tnl_str_profile_flds_3,
        .fld_num  = 4,
        .dummy    = 0
    }
};

/* Entry for EGR_MPLS_VPN_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_mpls_vpn_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = EGR_MPLS_VPN_STR_PROFILE_INDEXs,
        .fld_info = egr_mpls_vpn_str_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_MPLS_VPN_STR_PROFILE_INDEXs,
        .fld_info = egr_mpls_vpn_str_profile_flds_1,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for EGR_VFI_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_vfi_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = EGR_VFI_STR_PROFILE_INDEXs,
        .fld_info = egr_vfi_str_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_VFI_STR_PROFILE_INDEXs,
        .fld_info = egr_vfi_str_profile_flds_1,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for EGR_VXLT_2_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_vxlt_2_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_VXLT_2_STR_PROFILE_INDEXs,
        .fld_info = egr_vxlt_2_str_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for EGR_VXLT_DW_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_vxlt_dw_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_VXLT_DW_STR_PROFILE_INDEXs,
        .fld_info = egr_vxlt_dw_str_profile_flds_0,
        .fld_num  = 3,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_TAG,
        .key_name = EGR_VXLT_DW_STR_PROFILE_INDEXs,
        .fld_info = egr_vxlt_dw_str_profile_flds_1,
        .fld_num  = 3,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_DOT1P_PTR,
        .key_name = EGR_VXLT_DW_STR_PROFILE_INDEXs,
        .fld_info = egr_vxlt_dw_str_profile_flds_2,
        .fld_num  = 3,
        .dummy    = 0
    }
};

/* Entry for EGR_VXLT_STR_PROFILE. */
static
bcmint_sbr_profile_ent_t egr_vxlt_str_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = EGR_VXLT_STR_PROFILE_INDEXs,
        .fld_info = egr_vxlt_str_profile_flds_0,
        .fld_num  = 3,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_TAG,
        .key_name = EGR_VXLT_STR_PROFILE_INDEXs,
        .fld_info = egr_vxlt_str_profile_flds_1,
        .fld_num  = 3,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_DOT1P_PTR,
        .key_name = EGR_VXLT_STR_PROFILE_INDEXs,
        .fld_info = egr_vxlt_str_profile_flds_2,
        .fld_num  = 3,
        .dummy    = 0
    }
};

/* Entry for ING_DVP_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t ing_dvp_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_DESTINATION_NO_L2MC_L3MC_L2_OIF_SYS_DST_VALID,
        .key_name = ING_DVP_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_dvp_strength_profile_flds_0,
        .fld_num  = 3,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_NO_CUT_THRU,
        .key_name = ING_DVP_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_dvp_strength_profile_flds_1,
        .fld_num  = 3,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = ING_DVP_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_dvp_strength_profile_flds_2,
        .fld_num  = 3,
        .dummy    = 0
    }
};

/* Entry for ING_ECMP_MEMBER_1_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t ing_ecmp_member_1_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = ING_ECMP_MEMBER_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_ecmp_member_1_strength_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = ING_ECMP_MEMBER_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_ecmp_member_1_strength_profile_flds_1,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for ING_IPMC_REMAP_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t ing_ipmc_remap_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = ING_IPMC_REMAP_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_ipmc_remap_strength_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for ING_L3_NEXT_HOP_1_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t ing_l3_next_hop_1_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = ING_L3_NEXT_HOP_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_l3_next_hop_1_strength_profile_flds_0,
        .fld_num  = 6,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = ING_L3_NEXT_HOP_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_l3_next_hop_1_strength_profile_flds_1,
        .fld_num  = 6,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_FORWARDING_TYPE_NO_SYSTEM_OPCODE,
        .key_name = ING_L3_NEXT_HOP_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_l3_next_hop_1_strength_profile_flds_2,
        .fld_num  = 6,
        .dummy    = 0
    }
};

/* Entry for ING_L3_NEXT_HOP_2_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t ing_l3_next_hop_2_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_DVP_NO_MEMBERSHIP_STG,
        .key_name = ING_L3_NEXT_HOP_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_l3_next_hop_2_strength_profile_flds_0,
        .fld_num  = 4,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = ING_L3_NEXT_HOP_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_l3_next_hop_2_strength_profile_flds_1,
        .fld_num  = 4,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = ING_L3_NEXT_HOP_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_l3_next_hop_2_strength_profile_flds_2,
        .fld_num  = 4,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_DVP,
        .key_name = ING_L3_NEXT_HOP_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_l3_next_hop_2_strength_profile_flds_3,
        .fld_num  = 4,
        .dummy    = 0
    }
};

/* Entry for ING_SYSTEM_DESTINATION_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t ing_system_destination_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = ING_SYSTEM_DESTINATION_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_system_destination_strength_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = ING_SYSTEM_DESTINATION_STRENGTH_PROFILE_INDEXs,
        .fld_info = ing_system_destination_strength_profile_flds_1,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for L2_HOST_LOOKUP0_DEFAULT_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l2_host_lookup0_default_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L2_HOST_LOOKUP0_DEFAULT_STRENGTH_PROFILE_INDEXs,
        .fld_info = l2_host_lookup0_default_strength_profile_flds_0,
        .fld_num  = 5,
        .dummy    = 0
    }
};

/* Entry for L2_HOST_NARROW_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l2_host_narrow_strength_profile_ent[] = {
    {
        .index    = -1,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = NULL,
        .fld_info = l2_host_narrow_strength_profile_flds_0,
        .fld_num  = 4,
        .dummy    = 0
    }
};

/* Entry for L2_HOST_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l2_host_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L2_HOST_STRENGTH_PROFILE_INDEXs,
        .fld_info = l2_host_strength_profile_flds_0,
        .fld_num  = 5,
        .dummy    = 0
    }
};

/* Entry for L2_IPV4_MULTICAST_SG_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l2_ipv4_multicast_sg_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L2_IPV4_MULTICAST_SG_STRENGTH_PROFILE_INDEXs,
        .fld_info = l2_ipv4_multicast_sg_strength_profile_flds_0,
        .fld_num  = 5,
        .dummy    = 0
    }
};

/* Entry for L2_IPV4_MULTICAST_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l2_ipv4_multicast_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L2_IPV4_MULTICAST_STRENGTH_PROFILE_INDEXs,
        .fld_info = l2_ipv4_multicast_strength_profile_flds_0,
        .fld_num  = 5,
        .dummy    = 0
    }
};

/* Entry for L2_IPV6_MULTICAST_SG_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l2_ipv6_multicast_sg_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L2_IPV6_MULTICAST_SG_STRENGTH_PROFILE_INDEXs,
        .fld_info = l2_ipv6_multicast_sg_strength_profile_flds_0,
        .fld_num  = 5,
        .shr_ent  = &l2_ipv4_multicast_sg_strength_profile_ent[0],
        .dummy    = 1
    }
};

/* Entry for L2_IPV6_MULTICAST_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l2_ipv6_multicast_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L2_IPV6_MULTICAST_STRENGTH_PROFILE_INDEXs,
        .fld_info = l2_ipv6_multicast_strength_profile_flds_0,
        .fld_num  = 5,
        .shr_ent  = &l2_ipv4_multicast_strength_profile_ent[0],
        .dummy    = 1
    }
};

/* Entry for L3_IIF_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_iif_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IIF_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_iif_strength_profile_flds_0,
        .fld_num  = 2,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_CLASS_ID,
        .key_name = L3_IIF_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_iif_strength_profile_flds_1,
        .fld_num  = 2,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = L3_IIF_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_iif_strength_profile_flds_2,
        .fld_num  = 2,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_VRF,
        .key_name = L3_IIF_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_iif_strength_profile_flds_3,
        .fld_num  = 2,
        .dummy    = 0
    }
};

/* Entry for L3_IPV4_ADDRESS_COMPRESSION_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv4_address_compression_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV4_ADDRESS_COMPRESSION_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv4_address_compression_strength_profile_flds_0,
        .fld_num  = 2,
        .dummy    = 0
    }
};

/* Entry for L3_IPV4_ALPM_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv4_alpm_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV4_ALPM_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv4_alpm_strength_profile_flds_0,
        .fld_num  = 8,
        .dummy    = 0
    }
};

/* Entry for L3_IPV4_MULTICAST_1_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv4_multicast_1_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV4_MULTICAST_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv4_multicast_1_strength_profile_flds_0,
        .fld_num  = 8,
        .dummy    = 0
    }
};

/* Entry for L3_IPV4_MULTICAST_2_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv4_multicast_2_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV4_MULTICAST_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv4_multicast_2_strength_profile_flds_0,
        .fld_num  = 8,
        .dummy    = 0
    }
};

/* Entry for L3_IPV4_MULTICAST_DEFIP_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv4_multicast_defip_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV4_MULTICAST_DEFIP_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv4_multicast_defip_strength_profile_flds_0,
        .fld_num  = 8,
        .dummy    = 0
    }
};

/* Entry for L3_IPV4_UNICAST_DEFIP_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv4_unicast_defip_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV4_UNICAST_DEFIP_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv4_unicast_defip_strength_profile_flds_0,
        .fld_num  = 12,
        .dummy    = 0
    }
};

/* Entry for L3_IPV4_UNICAST_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv4_unicast_strength_profile_ent[] = {
    {
        .index    = -1,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = NULL,
        .fld_info = l3_ipv4_unicast_strength_profile_flds_0,
        .fld_num  = 12,
        .dummy    = 0
    }
};

/* Entry for L3_IPV6_ADDRESS_COMPRESSION_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv6_address_compression_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV6_ADDRESS_COMPRESSION_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv6_address_compression_strength_profile_flds_0,
        .fld_num  = 2,
        .shr_ent  = &l3_ipv4_address_compression_strength_profile_ent[0],
        .dummy    = 1
    }
};

/* Entry for L3_IPV6_ALPM_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv6_alpm_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV6_ALPM_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv6_alpm_strength_profile_flds_0,
        .fld_num  = 8,
        .shr_ent  = &l3_ipv4_alpm_strength_profile_ent[0],
        .dummy    = 1
    }
};

/* Entry for L3_IPV6_MULTICAST_1_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv6_multicast_1_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV6_MULTICAST_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv6_multicast_1_strength_profile_flds_0,
        .fld_num  = 8,
        .shr_ent  = &l3_ipv4_multicast_1_strength_profile_ent[0],
        .dummy    = 1
    }
};

/* Entry for L3_IPV6_MULTICAST_2_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv6_multicast_2_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV6_MULTICAST_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv6_multicast_2_strength_profile_flds_0,
        .fld_num  = 8,
        .shr_ent  = &l3_ipv4_multicast_2_strength_profile_ent[0],
        .dummy    = 1
    }
};

/* Entry for L3_IPV6_MULTICAST_DEFIP_128_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv6_multicast_defip_128_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV6_MULTICAST_DEFIP_128_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv6_multicast_defip_128_strength_profile_flds_0,
        .fld_num  = 8,
        .dummy    = 0
    }
};

/* Entry for L3_IPV6_MULTICAST_DEFIP_64_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv6_multicast_defip_64_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV6_MULTICAST_DEFIP_64_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv6_multicast_defip_64_strength_profile_flds_0,
        .fld_num  = 8,
        .shr_ent  = &l3_ipv4_multicast_defip_strength_profile_ent[0],
        .dummy    = 1
    }
};

/* Entry for L3_IPV6_UNICAST_DEFIP_128_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv6_unicast_defip_128_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV6_UNICAST_DEFIP_128_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv6_unicast_defip_128_strength_profile_flds_0,
        .fld_num  = 12,
        .shr_ent  = &l3_ipv4_unicast_defip_strength_profile_ent[0],
        .dummy    = 1
    }
};

/* Entry for L3_IPV6_UNICAST_DEFIP_64_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv6_unicast_defip_64_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IPV6_UNICAST_DEFIP_64_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ipv6_unicast_defip_64_strength_profile_flds_0,
        .fld_num  = 12,
        .dummy    = 0
    }
};

/* Entry for L3_IPV6_UNICAST_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ipv6_unicast_strength_profile_ent[] = {
    {
        .index    = -1,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = NULL,
        .fld_info = l3_ipv6_unicast_strength_profile_flds_0,
        .fld_num  = 12,
        .dummy    = 0
    }
};

/* Entry for L3_IP_LOOKUP0_DEFAULT_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ip_lookup0_default_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IP_LOOKUP0_DEFAULT_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ip_lookup0_default_strength_profile_flds_0,
        .fld_num  = 10,
        .dummy    = 0
    }
};

/* Entry for L3_IP_LOOKUP1_DEFAULT_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t l3_ip_lookup1_default_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = L3_IP_LOOKUP1_DEFAULT_STRENGTH_PROFILE_INDEXs,
        .fld_info = l3_ip_lookup1_default_strength_profile_flds_0,
        .fld_num  = 13,
        .dummy    = 0
    }
};

/* Entry for PROTOCOL_PKT_FORWARD_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t protocol_pkt_forward_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = PROTOCOL_PKT_FORWARD_STRENGTH_PROFILE_INDEXs,
        .fld_info = protocol_pkt_forward_strength_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = PROTOCOL_PKT_FORWARD_STRENGTH_PROFILE_INDEXs,
        .fld_info = protocol_pkt_forward_strength_profile_flds_1,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for SYS_PORT_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t sys_port_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = SYS_PORT_STRENGTH_PROFILE_INDEXs,
        .fld_info = sys_port_strength_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = SYS_PORT_STRENGTH_PROFILE_INDEXs,
        .fld_info = sys_port_strength_profile_flds_1,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for VFI_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t vfi_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_DEF,
        .key_name = VFI_STRENGTH_PROFILE_INDEXs,
        .fld_info = vfi_strength_profile_flds_0,
        .fld_num  = 1,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NONE,
        .key_name = VFI_STRENGTH_PROFILE_INDEXs,
        .fld_info = vfi_strength_profile_flds_1,
        .fld_num  = 1,
        .dummy    = 0
    }
};

/* Entry for VLAN_XLATE_1_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t vlan_xlate_1_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP_NO_VLAN_TAG_PRESERVE_CTRL,
        .key_name = VLAN_XLATE_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_1_strength_profile_flds_0,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF_NO_SVP,
        .key_name = VLAN_XLATE_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_1_strength_profile_flds_1,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP,
        .key_name = VLAN_XLATE_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_1_strength_profile_flds_2,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF,
        .key_name = VLAN_XLATE_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_1_strength_profile_flds_3,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF,
        .key_name = VLAN_XLATE_1_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_1_strength_profile_flds_4,
        .fld_num  = 8,
        .dummy    = 0
    }
};

/* Entry for VLAN_XLATE_2_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t vlan_xlate_2_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP_NO_VLAN_TAG_PRESERVE_CTRL,
        .key_name = VLAN_XLATE_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_2_strength_profile_flds_0,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF_NO_SVP,
        .key_name = VLAN_XLATE_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_2_strength_profile_flds_1,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP,
        .key_name = VLAN_XLATE_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_2_strength_profile_flds_2,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF,
        .key_name = VLAN_XLATE_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_2_strength_profile_flds_3,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF,
        .key_name = VLAN_XLATE_2_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_2_strength_profile_flds_4,
        .fld_num  = 8,
        .dummy    = 0
    }
};

/* Entry for VLAN_XLATE_3_STRENGTH_PROFILE. */
static
bcmint_sbr_profile_ent_t vlan_xlate_3_strength_profile_ent[] = {
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP_NO_VLAN_TAG_PRESERVE_CTRL,
        .key_name = VLAN_XLATE_3_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_3_strength_profile_flds_0,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF_NO_SVP,
        .key_name = VLAN_XLATE_3_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_3_strength_profile_flds_1,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP,
        .key_name = VLAN_XLATE_3_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_3_strength_profile_flds_2,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF,
        .key_name = VLAN_XLATE_3_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_3_strength_profile_flds_3,
        .fld_num  = 8,
        .dummy    = 0
    },
    {
        .index    = 0,
        .ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF,
        .key_name = VLAN_XLATE_3_STRENGTH_PROFILE_INDEXs,
        .fld_info = vlan_xlate_3_strength_profile_flds_4,
        .fld_num  = 8,
        .dummy    = 0
    }
};

/*
 * Strength profile table database for version 0.
 * The tables should be sorted by the table hdl value from small to large.
 */
static const
bcmint_sbr_profile_tbl_t sbr_profile_tbl_db[BCMI_LTSW_SBR_PTH_COUNT] = {
    /* DESTINATION_FP_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_DESTINATION_FP] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_DESTINATION_FP,
        .name      = DESTINATION_FP_STRENGTH_PROFILEs,
        .ent_info  = destination_fp_strength_profile_ent,
        .ent_num   = 5
    },
    /* EGR_DVP_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_DVP] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1,
        .name      = EGR_DVP_STR_PROFILEs,
        .ent_info  = egr_dvp_str_profile_ent,
        .ent_num   = 3
    },
    /* EGR_L2_OIF_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_L2_OIF] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_L2_OIF,
        .name      = EGR_L2_OIF_STR_PROFILEs,
        .ent_info  = egr_l2_oif_str_profile_ent,
        .ent_num   = 2
    },
    /* EGR_L3_NEXT_HOP_1_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_1] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1,
        .name      = EGR_L3_NEXT_HOP_1_STR_PROFILEs,
        .ent_info  = egr_l3_next_hop_1_str_profile_ent,
        .ent_num   = 4
    },
    /* EGR_L3_NEXT_HOP_2_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_2] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1,
        .name      = EGR_L3_NEXT_HOP_2_STR_PROFILEs,
        .ent_info  = egr_l3_next_hop_2_str_profile_ent,
        .ent_num   = 2
    },
    /* EGR_L3_OIF_1_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1,
        .name      = EGR_L3_OIF_1_STR_PROFILEs,
        .ent_info  = egr_l3_oif_1_str_profile_ent,
        .ent_num   = 4
    },
    /* EGR_L3_TNL_0_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_L3_TNL_0] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_L2_OIF,
        .name      = EGR_L3_TNL_0_STR_PROFILEs,
        .ent_info  = egr_l3_tnl_0_str_profile_ent,
        .ent_num   = 1
    },
    /* EGR_L3_TNL_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_L3_TNL] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_L2_OIF,
        .name      = EGR_L3_TNL_STR_PROFILEs,
        .ent_info  = egr_l3_tnl_str_profile_ent,
        .ent_num   = 4
    },
    /* EGR_MPLS_VPN_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_MPLS_VPN] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_L2_OIF,
        .name      = EGR_MPLS_VPN_STR_PROFILEs,
        .ent_info  = egr_mpls_vpn_str_profile_ent,
        .ent_num   = 2
    },
    /* EGR_VFI_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_VFI] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_L2_OIF,
        .name      = EGR_VFI_STR_PROFILEs,
        .ent_info  = egr_vfi_str_profile_ent,
        .ent_num   = 2
    },
    /* EGR_VXLT_2_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_VXLT_2] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_VXLT_2,
        .name      = EGR_VXLT_2_STR_PROFILEs,
        .ent_info  = egr_vxlt_2_str_profile_ent,
        .ent_num   = 1
    },
    /* EGR_VXLT_DW_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_VXLT_DW] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_VXLT_2,
        .name      = EGR_VXLT_DW_STR_PROFILEs,
        .ent_info  = egr_vxlt_dw_str_profile_ent,
        .ent_num   = 3
    },
    /* EGR_VXLT_STR_PROFILE. */
    [BCMI_LTSW_SBR_PTH_EGR_VXLT] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_EGR_VXLT_2,
        .name      = EGR_VXLT_STR_PROFILEs,
        .ent_info  = egr_vxlt_str_profile_ent,
        .ent_num   = 3
    },
    /* ING_DVP_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_ING_DVP] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_ING_DVP,
        .name      = ING_DVP_STRENGTH_PROFILEs,
        .ent_info  = ing_dvp_strength_profile_ent,
        .ent_num   = 3
    },
    /* ING_ECMP_MEMBER_1_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_ING_ECMP_MEMBER_1] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_ING_ECMP_MEMBER_1,
        .name      = ING_ECMP_MEMBER_1_STRENGTH_PROFILEs,
        .ent_info  = ing_ecmp_member_1_strength_profile_ent,
        .ent_num   = 2
    },
    /* ING_IPMC_REMAP_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_ING_IPMC_REMAP] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_2,
        .name      = ING_IPMC_REMAP_STRENGTH_PROFILEs,
        .ent_info  = ing_ipmc_remap_strength_profile_ent,
        .ent_num   = 1
    },
    /* ING_L3_NEXT_HOP_1_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_1] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_2,
        .name      = ING_L3_NEXT_HOP_1_STRENGTH_PROFILEs,
        .ent_info  = ing_l3_next_hop_1_strength_profile_ent,
        .ent_num   = 3
    },
    /* ING_L3_NEXT_HOP_2_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_2] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_2,
        .name      = ING_L3_NEXT_HOP_2_STRENGTH_PROFILEs,
        .ent_info  = ing_l3_next_hop_2_strength_profile_ent,
        .ent_num   = 4
    },
    /* ING_SYSTEM_DESTINATION_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_ING_SYSTEM_DESTINATION] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_ING_SYSTEM_DESTINATION,
        .name      = ING_SYSTEM_DESTINATION_STRENGTH_PROFILEs,
        .ent_info  = ing_system_destination_strength_profile_ent,
        .ent_num   = 2
    },
    /* L2_HOST_LOOKUP0_DEFAULT_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L2_HOST_LOOKUP0_DEFAULT] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L2_HOST_LOOKUP0_DEFAULT_STRENGTH_PROFILEs,
        .ent_info  = l2_host_lookup0_default_strength_profile_ent,
        .ent_num   = 1
    },
    /* L2_HOST_NARROW_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L2_HOST_NARROW] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L2_HOST_NARROW_STRENGTH_PROFILEs,
        .ent_info  = l2_host_narrow_strength_profile_ent,
        .ent_num   = 1
    },
    /* L2_HOST_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L2_HOST] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L2_HOST_STRENGTH_PROFILEs,
        .ent_info  = l2_host_strength_profile_ent,
        .ent_num   = 1
    },
    /* L2_IPV4_MULTICAST_SG_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L2_IPV4_MULTICAST_SG] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L2_IPV4_MULTICAST_SG_STRENGTH_PROFILEs,
        .ent_info  = l2_ipv4_multicast_sg_strength_profile_ent,
        .ent_num   = 1
    },
    /* L2_IPV4_MULTICAST_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L2_IPV4_MULTICAST] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L2_IPV4_MULTICAST_STRENGTH_PROFILEs,
        .ent_info  = l2_ipv4_multicast_strength_profile_ent,
        .ent_num   = 1
    },
    /* L2_IPV6_MULTICAST_SG_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L2_IPV6_MULTICAST_SG] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L2_IPV6_MULTICAST_SG_STRENGTH_PROFILEs,
        .ent_info  = l2_ipv6_multicast_sg_strength_profile_ent,
        .ent_num   = 1
    },
    /* L2_IPV6_MULTICAST_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L2_IPV6_MULTICAST] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L2_IPV6_MULTICAST_STRENGTH_PROFILEs,
        .ent_info  = l2_ipv6_multicast_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IIF_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IIF] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_ING_SYSTEM_DESTINATION,
        .name      = L3_IIF_STRENGTH_PROFILEs,
        .ent_info  = l3_iif_strength_profile_ent,
        .ent_num   = 4
    },
    /* L3_IPV4_ADDRESS_COMPRESSION_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV4_ADDRESS_COMPRESSION] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP,
        .name      = L3_IPV4_ADDRESS_COMPRESSION_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv4_address_compression_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV4_ALPM_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV4_ALPM] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP,
        .name      = L3_IPV4_ALPM_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv4_alpm_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV4_MULTICAST_1_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_1] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L3_IPV4_MULTICAST_1_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv4_multicast_1_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV4_MULTICAST_2_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_2] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L3_IPV4_MULTICAST_2_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv4_multicast_2_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV4_MULTICAST_DEFIP_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_DEFIP] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP,
        .name      = L3_IPV4_MULTICAST_DEFIP_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv4_multicast_defip_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV4_UNICAST_DEFIP_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP,
        .name      = L3_IPV4_UNICAST_DEFIP_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv4_unicast_defip_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV4_UNICAST_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L3_IPV4_UNICAST_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv4_unicast_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV6_ADDRESS_COMPRESSION_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV6_ADDRESS_COMPRESSION] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP,
        .name      = L3_IPV6_ADDRESS_COMPRESSION_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv6_address_compression_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV6_ALPM_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV6_ALPM] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP,
        .name      = L3_IPV6_ALPM_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv6_alpm_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV6_MULTICAST_1_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_1] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L3_IPV6_MULTICAST_1_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv6_multicast_1_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV6_MULTICAST_2_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_2] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L3_IPV6_MULTICAST_2_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv6_multicast_2_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV6_MULTICAST_DEFIP_128_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_DEFIP_128] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP,
        .name      = L3_IPV6_MULTICAST_DEFIP_128_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv6_multicast_defip_128_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV6_MULTICAST_DEFIP_64_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_DEFIP_64] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP,
        .name      = L3_IPV6_MULTICAST_DEFIP_64_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv6_multicast_defip_64_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV6_UNICAST_DEFIP_128_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST_DEFIP_128] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP,
        .name      = L3_IPV6_UNICAST_DEFIP_128_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv6_unicast_defip_128_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV6_UNICAST_DEFIP_64_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST_DEFIP_64] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP,
        .name      = L3_IPV6_UNICAST_DEFIP_64_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv6_unicast_defip_64_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IPV6_UNICAST_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L3_IPV6_UNICAST_STRENGTH_PROFILEs,
        .ent_info  = l3_ipv6_unicast_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IP_LOOKUP0_DEFAULT_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IP_LOOKUP0_DEFAULT] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L3_IP_LOOKUP0_DEFAULT_STRENGTH_PROFILEs,
        .ent_info  = l3_ip_lookup0_default_strength_profile_ent,
        .ent_num   = 1
    },
    /* L3_IP_LOOKUP1_DEFAULT_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_L3_IP_LOOKUP1_DEFAULT] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_L2_HOST,
        .name      = L3_IP_LOOKUP1_DEFAULT_STRENGTH_PROFILEs,
        .ent_info  = l3_ip_lookup1_default_strength_profile_ent,
        .ent_num   = 1
    },
    /* PROTOCOL_PKT_FORWARD_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_PROTOCOL_PKT_FORWARD] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_PROTOCOL_PKT_FORWARD,
        .name      = PROTOCOL_PKT_FORWARD_STRENGTH_PROFILEs,
        .ent_info  = protocol_pkt_forward_strength_profile_ent,
        .ent_num   = 2
    },
    /* SYS_PORT_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_SYS_PORT] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_SYS_PORT,
        .name      = SYS_PORT_STRENGTH_PROFILEs,
        .ent_info  = sys_port_strength_profile_ent,
        .ent_num   = 2
    },
    /* VFI_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_VFI] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_VFI,
        .name      = VFI_STRENGTH_PROFILEs,
        .ent_info  = vfi_strength_profile_ent,
        .ent_num   = 2
    },
    /* VLAN_XLATE_1_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_VLAN_XLATE_1] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_VLAN_XLATE_1,
        .name      = VLAN_XLATE_1_STRENGTH_PROFILEs,
        .ent_info  = vlan_xlate_1_strength_profile_ent,
        .ent_num   = 5
    },
    /* VLAN_XLATE_2_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_VLAN_XLATE_2] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_VLAN_XLATE_2,
        .name      = VLAN_XLATE_2_STRENGTH_PROFILEs,
        .ent_info  = vlan_xlate_2_strength_profile_ent,
        .ent_num   = 5
    },
    /* VLAN_XLATE_3_STRENGTH_PROFILE. */
    [BCMI_LTSW_SBR_PTH_VLAN_XLATE_3] = {
        .shr_hdl   = BCMI_LTSW_SBR_PTH_VLAN_XLATE_1,
        .name      = VLAN_XLATE_3_STRENGTH_PROFILEs,
        .ent_info  = vlan_xlate_3_strength_profile_ent,
        .ent_num   = 5
    }
};

/* Strength fields for EGR_L2_OIF. */
static const
bcmint_sbr_fld_t egr_l2_oif_stre_flds_0[] = {
    { /* DOT1P_PTR_STR. */
        .type = BCMI_LTSW_SBR_FT_DOT1P_PTR,
        .val = 3
    },
    { /* DOT1P_PTR_STR with higher than EGR_VXLT. */
        .type = BCMI_LTSW_SBR_FT_DOT1P_PTR_HIGH,
        .val = 5
    }
};

/* Strength fields for EGR_L3_NEXT_HOP_1. */
static const
bcmint_sbr_fld_t egr_l3_next_hop_1_stre_flds_0[] = {
    { /* TOS_PTR_STR. */
        .type = BCMI_LTSW_SBR_FT_TOS_PTR,
        .val = 6
    }
};

/* Strength fields for ING_SYSTEM_DESTINATION_TABLE. */
static const
bcmint_sbr_fld_t ing_system_destination_table_stre_flds_0[] = {
    { /* L2_OIF_STRENGTH. */
        .type = BCMI_LTSW_SBR_FT_L2_OIF,
        .val = 3
    },
    { /* L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTH. */
        .type = BCMI_LTSW_SBR_FT_L2MC_L3MC_L2_OIF_SYS_DST_VALID,
        .val = 3
    }
};

/* Strength fields for ING_VLAN_XLATE_1_TABLE. */
static const
bcmint_sbr_fld_t ing_vlan_xlate_1_table_stre_flds_0[] = {
    { /* L3_IIF_STRENGTH. */
        .type = BCMI_LTSW_SBR_FT_L3_IIF,
        .val = 4
    },
    { /* VFI_STRENGTH. */
        .type = BCMI_LTSW_SBR_FT_VFI,
        .val = 4
    }
};

/* Strength fields for ING_VLAN_XLATE_2_TABLE. */
static const
bcmint_sbr_fld_t ing_vlan_xlate_2_table_stre_flds_0[] = {
    { /* L3_IIF_STRENGTH. */
        .type = BCMI_LTSW_SBR_FT_L3_IIF,
        .val = 4
    },
    { /* VFI_STRENGTH. */
        .type = BCMI_LTSW_SBR_FT_VFI,
        .val = 4
    }
};

/* Strength fields for ING_VLAN_XLATE_3_TABLE. */
static const
bcmint_sbr_fld_t ing_vlan_xlate_3_table_stre_flds_0[] = {
    { /* L3_IIF_STRENGTH. */
        .type = BCMI_LTSW_SBR_FT_L3_IIF,
        .val = 5
    },
    { /* VFI_STRENGTH. */
        .type = BCMI_LTSW_SBR_FT_VFI,
        .val = 5
    }
};

/* Strength fields for L3_IPV4_TUNNEL_TABLE. */
static const
bcmint_sbr_fld_t l3_ipv4_tunnel_table_stre_flds_0[] = {
    { /* L3_IIF_STRENGTH. */
        .type = BCMI_LTSW_SBR_FT_L3_IIF,
        .val = 6
    }
};

/* Strength fields for L3_IPV6_TUNNEL_TABLE. */
static const
bcmint_sbr_fld_t l3_ipv6_tunnel_table_stre_flds_0[] = {
    { /* L3_IIF_STRENGTH. */
        .type = BCMI_LTSW_SBR_FT_L3_IIF,
        .val = 6
    }
};

/* Strength fields for R_TUNNEL_L3_IIF_STRENGTH. */
static const
bcmint_sbr_fld_t r_tunnel_l3_iif_stre_flds_0[] = {
    { /* VALUE. */
        .type = BCMI_LTSW_SBR_FT_VALUE,
        .val = 6
    }
};

/*
 * Strength field database per table for version 0.
 * The tables should be sorted by the table hdl value from small to large.
 */
static const
bcmint_sbr_tbl_t sbr_tbl_db[BCMI_LTSW_SBR_TH_COUNT] = {
    /* EGR_L2_OIF. */
    [BCMI_LTSW_SBR_TH_EGR_L2_OIF] = {
        .fld_info  = egr_l2_oif_stre_flds_0,
        .fld_num   = 2
    },
    /* EGR_L3_NEXT_HOP_1. */
    [BCMI_LTSW_SBR_TH_EGR_L3_NEXT_HOP_1] = {
        .fld_info  = egr_l3_next_hop_1_stre_flds_0,
        .fld_num   = 1
    },
    /* ING_SYSTEM_DESTINATION_TABLE. */
    [BCMI_LTSW_SBR_TH_ING_SYSTEM_DESTINATION_TABLE] = {
        .fld_info  = ing_system_destination_table_stre_flds_0,
        .fld_num   = 2
    },
    /* ING_VLAN_XLATE_1_TABLE. */
    [BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_1_TABLE] = {
        .fld_info  = ing_vlan_xlate_1_table_stre_flds_0,
        .fld_num   = 2
    },
    /* ING_VLAN_XLATE_2_TABLE. */
    [BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_2_TABLE] = {
        .fld_info  = ing_vlan_xlate_2_table_stre_flds_0,
        .fld_num   = 2
    },
    /* ING_VLAN_XLATE_3_TABLE. */
    [BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_3_TABLE] = {
        .fld_info  = ing_vlan_xlate_3_table_stre_flds_0,
        .fld_num   = 2
    },
    /* L3_IPV4_TUNNEL_TABLE. */
    [BCMI_LTSW_SBR_TH_L3_IPV4_TUNNEL_TABLE] = {
        .fld_info  = l3_ipv4_tunnel_table_stre_flds_0,
        .fld_num   = 1
    },
    /* L3_IPV6_TUNNEL_TABLE. */
    [BCMI_LTSW_SBR_TH_L3_IPV6_TUNNEL_TABLE] = {
        .fld_info  = l3_ipv6_tunnel_table_stre_flds_0,
        .fld_num   = 1
    },
    /* R_TUNNEL_L3_IIF_STRENGTH. */
    [BCMI_LTSW_SBR_TH_R_TUNNEL_L3_IIF_STRENGTH] =  {
        .fld_info  = r_tunnel_l3_iif_stre_flds_0,
        .fld_num   = 1
    }
};

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the database of SBR.
 *
 * \param [in]  unit      Unit Number.
 * \param [out] sbr_db    Database to SBR.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
bcm56880_a0_dna_4_6_6_ltsw_sbr_db_get(int unit, bcmint_sbr_db_t *sbr_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sbr_db, SHR_E_PARAM);

    /* Allocate SBR entry index dynamically. */
    sbr_db->flags |= BCMINT_SBR_CTRL_DYNA_ENT_IDX_ALLOC;

    sbr_db->profile_tbl_db = sbr_profile_tbl_db;
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_DESTINATION_FP);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_DVP);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_L2_OIF);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_1);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_2);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_L3_TNL_0);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_L3_TNL);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_MPLS_VPN);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_VFI);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_VXLT_2);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_VXLT_DW);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_EGR_VXLT);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_ING_DVP);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_ING_ECMP_MEMBER_1);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_ING_IPMC_REMAP);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_1);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_2);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_ING_SYSTEM_DESTINATION);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L2_HOST_LOOKUP0_DEFAULT);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L2_HOST_NARROW);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L2_HOST);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L2_IPV4_MULTICAST_SG);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L2_IPV4_MULTICAST);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L2_IPV6_MULTICAST_SG);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L2_IPV6_MULTICAST);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IIF);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV4_ADDRESS_COMPRESSION);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV4_ALPM);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_1);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_2);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_DEFIP);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV6_ADDRESS_COMPRESSION);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV6_ALPM);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_1);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_2);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_DEFIP_128);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_DEFIP_64);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST_DEFIP_128);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST_DEFIP_64);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IP_LOOKUP0_DEFAULT);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_L3_IP_LOOKUP1_DEFAULT);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_PROTOCOL_PKT_FORWARD);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_SYS_PORT);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_VFI);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_VLAN_XLATE_1);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_VLAN_XLATE_2);
    SHR_BITSET
        (sbr_db->profile_tbl_bmp, BCMI_LTSW_SBR_PTH_VLAN_XLATE_3);

    sbr_db->tbl_db = sbr_tbl_db;
    SHR_BITSET
        (sbr_db->tbl_bmp, BCMI_LTSW_SBR_TH_EGR_L2_OIF);
    SHR_BITSET
        (sbr_db->tbl_bmp, BCMI_LTSW_SBR_TH_EGR_L3_NEXT_HOP_1);
    SHR_BITSET
        (sbr_db->tbl_bmp, BCMI_LTSW_SBR_TH_ING_SYSTEM_DESTINATION_TABLE);
    SHR_BITSET
        (sbr_db->tbl_bmp, BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_1_TABLE);
    SHR_BITSET
        (sbr_db->tbl_bmp, BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_2_TABLE);
    SHR_BITSET
        (sbr_db->tbl_bmp, BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_3_TABLE);
    SHR_BITSET
        (sbr_db->tbl_bmp, BCMI_LTSW_SBR_TH_L3_IPV4_TUNNEL_TABLE);
    SHR_BITSET
        (sbr_db->tbl_bmp, BCMI_LTSW_SBR_TH_L3_IPV6_TUNNEL_TABLE);
    SHR_BITSET
        (sbr_db->tbl_bmp, BCMI_LTSW_SBR_TH_R_TUNNEL_L3_IIF_STRENGTH);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief SBR driver function variable for bcm56880_a0 DNA_4_6_6 device.
 */
static mbcm_ltsw_sbr_drv_t bcm56880_a0_dna_4_6_6_ltsw_sbr_drv = {
    .sbr_db_get = bcm56880_a0_dna_4_6_6_ltsw_sbr_db_get
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_dna_4_6_6_ltsw_sbr_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_sbr_drv_set(unit, &bcm56880_a0_dna_4_6_6_ltsw_sbr_drv));

exit:
    SHR_FUNC_EXIT();
}

