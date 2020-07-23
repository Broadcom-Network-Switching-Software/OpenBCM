/*! \file bcm56880_a0_dna_4_6_6_mpls.c
 *
 * BCM56880_A0 DNA_4_6_6 MPLS driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mpls_int.h>
#include <bcm_int/ltsw/mbcm/mpls.h>
#include <bcm_int/ltsw/xfs/types.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_MPLS

static const char
*dest_type_str[BCMI_XFS_DEST_TYPE_CNT] = {
                            [BCMI_XFS_DEST_TYPE_NO_OP]           = NO_OPs,
                            [BCMI_XFS_DEST_TYPE_L2_OIF]          = L2_OIFs,
                            [BCMI_XFS_DEST_TYPE_RSVD]            = RESERVEDs,
                            [BCMI_XFS_DEST_TYPE_VP]              = VPs,
                            [BCMI_XFS_DEST_TYPE_ECMP]            = ECMPs,
                            [BCMI_XFS_DEST_TYPE_NHOP]            = NHOPs,
                            [BCMI_XFS_DEST_TYPE_L2MC_GROUP]      = L2MC_GROUPs,
                            [BCMI_XFS_DEST_TYPE_L3MC_GROUP]      = L3MC_GROUPs,
                            [BCMI_XFS_DEST_TYPE_RSVD_1]          = RESERVED_1s,
                            [BCMI_XFS_DEST_TYPE_ECMP_MEMBER]     = ECMP_MEMBERs,
                   };

static const char
*label_action_str[bcmintLtswMplsFwdLabelActionCount] = {
                      [bcmintLtswMplsFwdLabelActionNoop]       = NOOPs,
                      [bcmintLtswMplsFwdLabelActionSwap]       = SWAPs,
                      [bcmintLtswMplsFwdLabelActionPhp]        = PHPs,
                      [bcmintLtswMplsFwdLabelActionPopRoute]  = POP_AND_ROUTEs,
                      [bcmintLtswMplsFwdLabelActionPopSwitch] = POP_AND_SWITCHs,
                    };

static const char
*cw_chk_mode_str[bcmintLtswMplsCwCheckCtrlCount] = {
                          [bcmintLtswMplsCwCheckCtrlDisable]     = DISABLEs,
                          [bcmintLtswMplsCwCheckCtrlLoose]       = LOOSEs,
                          [bcmintLtswMplsCwCheckCtrlStrict]      = STRICTs,
                          [bcmintLtswMplsCwCheckCtrlNotPresent]  = NOT_PRESENTs,
                    };

static const char
*egr_mpls_vpn_view[bcmintLtswMplsEgrMplsVpnViewCount] = {
          [bcmintLtswMplsEgrMplsVpnViewVtVcLabelTab]       = VT_VC_LABEL_TABs,
                    };

static const char
*ttl_mode[bcmintLtswMplsTtlModeCount] = {
          [bcmintLtswMplsTtlModePipe]       = PIPEs,
          [bcmintLtswMplsTtlModeUniform]    = UNIFORMs,
                    };

/*!
 * \brief Get dest type from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value dest_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dest_type_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMI_XFS_DEST_TYPE_CNT; i++) {
        if (!sal_strcmp(symbol, dest_type_str[i])) {
            break;
        }
    }

    if (i >= BCMI_XFS_DEST_TYPE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get dest type sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dest_type_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= BCMI_XFS_DEST_TYPE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = dest_type_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get label action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value dest_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
label_action_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< bcmintLtswMplsFwdLabelActionCount; i++) {
        if (!sal_strcmp(symbol, label_action_str[i])) {
            break;
        }
    }

    if (i >= bcmintLtswMplsFwdLabelActionCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get label action sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
label_action_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= bcmintLtswMplsFwdLabelActionCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = label_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get CW check control from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value dest_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cw_chk_ctrl_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< bcmintLtswMplsCwCheckCtrlCount; i++) {
        if (!sal_strcmp(symbol, cw_chk_mode_str[i])) {
            break;
        }
    }

    if (i >= bcmintLtswMplsCwCheckCtrlCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get CW check control sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cw_chk_ctrl_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= bcmintLtswMplsCwCheckCtrlCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = cw_chk_mode_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get EGR_MPLS_VPN view scalar value from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value dest_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
egr_mpls_vpn_view_symbol_to_scalar(
    int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< bcmintLtswMplsEgrMplsVpnViewCount; i++) {
        if (!sal_strcmp(symbol, egr_mpls_vpn_view[i])) {
            break;
        }
    }

    if (i >= bcmintLtswMplsEgrMplsVpnViewCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get EGR_MPLS_VPN sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
egr_mpls_vpn_view_scalar_to_symbol(
    int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= bcmintLtswMplsEgrMplsVpnViewCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = egr_mpls_vpn_view[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert TTL mode sybmol scalar value.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value dest_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ttl_mode_symbol_to_scalar(
    int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< bcmintLtswMplsTtlModeCount; i++) {
        if (!sal_strcmp(symbol, ttl_mode[i])) {
            break;
        }
    }

    if (i >= bcmintLtswMplsTtlModeCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert TTL mode scalar value to sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ttl_mode_scalar_to_symbol(
    int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= bcmintLtswMplsTtlModeCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = ttl_mode[value];

exit:
    SHR_FUNC_EXIT();
}

/* MPLS_ENTRY_1 fields. */
static const bcmint_mpls_fld_t
mpls_entry_1_flds[BCMINT_LTSW_MPLS_FLD_ME_1_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_ME_1_LABEL] = {
        LABELs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_LABEL_CONTEXT_ID] = {
        LABEL_CONTEXT_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_ENTRY_VALID] = {
        ENTRY_VALIDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_TERM_LABEL] = {
        TERM_LABELs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_OPAQUE_CTRL_0] = {
        OPAQUE_CTRL_0s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_OPAQUE_CTRL_1] = {
        OPAQUE_CTRL_1s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_EXP_TO_ECN_MAPPING_PTR] = {
        EXP_TO_ECN_MAPPING_PTRs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_EXP_QOS_CTRL_ID] = {
        EXP_QOS_CTRL_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_EXP_REMARK_CTRL_ID] = {
        EXP_REMARK_CTRL_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_PW_TERM_NUM_VALID] = {
        PW_TERM_NUM_VALIDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_PW_CC_TYPE] = {
        PW_CC_TYPEs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_CW_CHECK_CTRL] = {
        CW_CHECK_CTRLs,
        cw_chk_ctrl_symbol_to_scalar,
        cw_chk_ctrl_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_TTL_MODE] = {
        TTL_MODEs,
        ttl_mode_symbol_to_scalar,
        ttl_mode_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX] = {
        ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_DROP_DATA_ENABLE] = {
        DROP_DATA_ENABLEs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_ENTROPY_LABEL_PRESENT] = {
        ENTROPY_LABEL_PRESENTs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_POINT2POINT_FLOW] = {
        POINT2POINT_FLOWs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_LABEL_ACTION] = {
        LABEL_ACTIONs,
        label_action_symbol_to_scalar,
        label_action_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_DESTINATION_TYPE] = {
        DESTINATION_TYPEs,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_EXP_MAPPING_PTR] = {
        EXP_MAPPING_PTRs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_PW_TERM_NUM] = {
        PW_TERM_NUMs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_CLASS_ID] = {
        CLASS_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_SVP] = {
        SVPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_DESTINATION] = {
        DESTINATIONs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ME_1_VFI_L3_IIF_NHOP_2_ECMP_GROUP_INDEX_1] = {
        VFI_L3_IIF_NHOP_2_ECMP_GROUP_INDEX_1s,
        NULL,
        NULL
    },
};

/* EGR_L3_TUNNEL_0 LT fields. */
static const bcmint_mpls_fld_t
egr_l3_tunnel_0_flds[BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_EGR_L3_TUNNEL_IDX_0] = {
        EGR_L3_TUNNEL_IDX_0s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_EDIT_CTRL_ID] = {
        EDIT_CTRL_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_IPV6_SRC_ADDR] = {
        IPV6_SRC_ADDRs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_TOS_OR_EXP_REMARK_BASE_PTR] = {
        TOS_OR_EXP_REMARK_BASE_PTRs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_MPLS_PUSH_BITMAP] = {
        MPLS_PUSH_BITMAPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_TTL_OR_MPLS_CTRL] = {
        TTL_OR_MPLS_CTRLs,
        NULL,
        NULL
    }
};

/* EGR_L3_TUNNEL_1 LT fields. */
static const bcmint_mpls_fld_t
egr_l3_tunnel_1_flds[BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_EGR_L3_TUNNEL_IDX_0] = {
        EGR_L3_TUNNEL_IDX_0s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_HI_1] = {
        IPV6_DST_ADDR_HI_1s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_HI_0] = {
        IPV6_DST_ADDR_HI_0s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_LO] = {
        IPV6_DST_ADDR_LOs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_STRENGTH_PRFL_IDX] = {
        STRENGTH_PRFL_IDXs,
        NULL,
        NULL
    },
};

/* EGR_MPLS_VPN LT fields. */
static const bcmint_mpls_fld_t
egr_mpls_vpn_flds[BCMINT_LTSW_MPLS_FLD_EGR_MPLS_VPN_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_EGR_MPLS_VPN_MPLS_VPN_OR_INT_INDEX] = {
        MPLS_VPN_OR_INT_INDEXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_MPLS_VPN_VIEW_T] = {
        VIEW_Ts,
        egr_mpls_vpn_view_symbol_to_scalar,
        egr_mpls_vpn_view_scalar_to_symbol
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_MPLS_VPN_VPN_LABEL] = {
        VPN_LABELs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_MPLS_VPN_PROCESS_CTRL] = {
        PROCESS_CTRLs,
        NULL,
        NULL
    }
};

/* ING_SVP_TABLE LT fields. */
static const bcmint_mpls_fld_t
ing_svp_table_flds[BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_SVP] = {
        SVPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_NETWORK_GROUP_BITMAP] = {
        NETWORK_GROUP_BITMAPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_VPN_LABEL] = {
        VPN_LABELs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_PROCESS_CTRL] = {
        PROCESS_CTRLs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_VLAN_TAG_PRESERVE_CTRL] = {
        VLAN_TAG_PRESERVE_CTRLs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_CML_FLAGS_NEW] = {
        CML_FLAGS_NEWs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_CML_FLAGS_MOVE] = {
        CML_FLAGS_MOVEs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_IPMC_REMAP_ENABLE] = {
        IPMC_REMAP_ENABLEs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_DOT1P_PTR] = {
        DOT1P_PTRs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_DSCP_PTR] = {
        DSCP_PTRs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_USE_DSCP_FOR_PHB] = {
        USE_DSCP_FOR_PHBs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_USE_OTAG_DOT1P_CFI_FOR_PHB] = {
        USE_OTAG_DOT1P_CFI_FOR_PHBs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_USE_ITAG_DOT1P_CFI_FOR_PHB] = {
        USE_ITAG_DOT1P_CFI_FOR_PHBs,
        NULL,
        NULL
    },
};

/* ING_DVP_TABLE LT fields. */
static const bcmint_mpls_fld_t
ing_dvp_table_flds[BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_DVP] = {
        DVPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_NO_CUT_THRU] = {
        NO_CUT_THRUs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_DESTINATION] = {
        DESTINATIONs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_L2MC_L3MC_L2_OIF_SYS_DST_VALID] = {
        L2MC_L3MC_L2_OIF_SYS_DST_VALIDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_STRENGTH_PROFILE_INDEX] = {
        STRENGTH_PROFILE_INDEXs,
        NULL,
        NULL
    }
};

/* EGR_DVP LT fields. */
static const bcmint_mpls_fld_t
egr_dvp_flds[BCMINT_LTSW_MPLS_FLD_EGR_DVP_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_DVP] = {
        DVPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_OBJ_TABLE_SEL_1] = {
        OBJ_TABLE_SEL_1s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_EDIT_CTRL_ID] = {
        EDIT_CTRL_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_TUNNEL_IDX_0] = {
        TUNNEL_IDX_0s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_VC_LABEL_INDEX] = {
        VC_LABEL_INDEXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_CLASS_ID] = {
        CLASS_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_SPLIT_HORIZON_CTRL] = {
        SPLIT_HORIZON_CTRLs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_STRENGTH_PRFL_IDX] = {
        STRENGTH_PRFL_IDXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_L2_MTU_VALUE] = {
        L2_MTU_VALUEs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_PROCESS_CTRL_2] = {
        PROCESS_CTRL_2s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_SEQ_NUM_PROFILE_IDX] = {
        SEQ_NUM_PROFILE_IDXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_EGR_DVP_SEQ_NUM_COUNTER_IDX] = {
        SEQ_NUM_COUNTER_IDXs,
        NULL,
        NULL
    }
};

/* ING_VFI_TABLE LT fields. */
static const bcmint_mpls_fld_t
ing_vfi_table_flds[BCMINT_LTSW_MPLS_FLD_ING_VFI_TABLE_FLD_CNT] = {
    [BCMINT_LTSW_MPLS_FLD_ING_VFI_TABLE_VFI] = {
        VFIs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_VFI_TABLE_DESTINATION] = {
        DESTINATIONs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_MPLS_FLD_ING_VFI_TABLE_DESTINATION_TYPE] = {
        DESTINATION_TYPEs,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    }
};

static const bcmint_mpls_lt_t
bcm56880_a0_dna_4_6_6_mpls_lt[] = {
    [BCMINT_LTSW_MPLS_LT_MPLS_ENTRY_1] = {
        .name = MPLS_ENTRY_1s,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_LABEL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_LABEL_CONTEXT_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_ENTRY_VALID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_TERM_LABEL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_OPAQUE_CTRL_0) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_OPAQUE_CTRL_1) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_TO_ECN_MAPPING_PTR) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_QOS_CTRL_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_REMARK_CTRL_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_PW_TERM_NUM_VALID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_PW_CC_TYPE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_CW_CHECK_CTRL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_TTL_MODE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_DROP_DATA_ENABLE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_ENTROPY_LABEL_PRESENT) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_POINT2POINT_FLOW) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_LABEL_ACTION) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_DESTINATION_TYPE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_MAPPING_PTR) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_PW_TERM_NUM) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_CLASS_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_SVP) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_DESTINATION) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_VFI_L3_IIF_NHOP_2_ECMP_GROUP_INDEX_1),
        .flds = mpls_entry_1_flds
    },
    [BCMINT_LTSW_MPLS_LT_MPLS_ENTRY_2] = {
        .name = MPLS_ENTRY_2s,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_LABEL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_LABEL_CONTEXT_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_ENTRY_VALID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_TERM_LABEL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_OPAQUE_CTRL_0) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_OPAQUE_CTRL_1) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_TO_ECN_MAPPING_PTR) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_QOS_CTRL_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_REMARK_CTRL_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_PW_TERM_NUM_VALID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_PW_CC_TYPE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_CW_CHECK_CTRL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_TTL_MODE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_DROP_DATA_ENABLE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_ENTROPY_LABEL_PRESENT) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_POINT2POINT_FLOW) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_LABEL_ACTION) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_DESTINATION_TYPE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_MAPPING_PTR) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_PW_TERM_NUM) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_CLASS_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_SVP) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_DESTINATION) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_VFI_L3_IIF_NHOP_2_ECMP_GROUP_INDEX_1),
        .flds = mpls_entry_1_flds
    },
    [BCMINT_LTSW_MPLS_LT_MPLS_ENTRY_3] = {
        .name = MPLS_ENTRY_3s,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_LABEL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_LABEL_CONTEXT_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_ENTRY_VALID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_TERM_LABEL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_OPAQUE_CTRL_0) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_OPAQUE_CTRL_1) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_TO_ECN_MAPPING_PTR) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_QOS_CTRL_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_REMARK_CTRL_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_PW_TERM_NUM_VALID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_PW_CC_TYPE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_CW_CHECK_CTRL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_TTL_MODE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_DROP_DATA_ENABLE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_ENTROPY_LABEL_PRESENT) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_POINT2POINT_FLOW) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_LABEL_ACTION) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_DESTINATION_TYPE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_EXP_MAPPING_PTR) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_PW_TERM_NUM) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_CLASS_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_SVP) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_DESTINATION) |
            (1 << BCMINT_LTSW_MPLS_FLD_ME_1_VFI_L3_IIF_NHOP_2_ECMP_GROUP_INDEX_1),
        .flds = mpls_entry_1_flds
    },
    [BCMINT_LTSW_MPLS_LT_EGR_L3_TUNNEL_0] = {
        .name = EGR_L3_TUNNEL_0s,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_EGR_L3_TUNNEL_IDX_0) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_EDIT_CTRL_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_IPV6_SRC_ADDR) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_TOS_OR_EXP_REMARK_BASE_PTR) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_MPLS_PUSH_BITMAP) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_0_TTL_OR_MPLS_CTRL),
        .flds = egr_l3_tunnel_0_flds
    },
    [BCMINT_LTSW_MPLS_LT_EGR_L3_TUNNEL_1] = {
        .name = EGR_L3_TUNNEL_1s,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_EGR_L3_TUNNEL_IDX_0) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_HI_1) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_HI_0) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_STRENGTH_PRFL_IDX) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_LO),
        .flds = egr_l3_tunnel_1_flds
    },
    [BCMINT_LTSW_MPLS_LT_EGR_MPLS_VPN] = {
        .name = EGR_MPLS_VPNs,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_MPLS_VPN_MPLS_VPN_OR_INT_INDEX) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_MPLS_VPN_VIEW_T) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_MPLS_VPN_VPN_LABEL) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_MPLS_VPN_PROCESS_CTRL),
        .flds = egr_mpls_vpn_flds
    },
    [BCMINT_LTSW_MPLS_LT_ING_SVP_TABLE] = {
        .name = ING_SVP_TABLEs,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_SVP) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_NETWORK_GROUP_BITMAP) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_VPN_LABEL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_VLAN_TAG_PRESERVE_CTRL) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_CML_FLAGS_NEW) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_CML_FLAGS_MOVE) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_USE_DSCP_FOR_PHB) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_USE_OTAG_DOT1P_CFI_FOR_PHB) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_USE_ITAG_DOT1P_CFI_FOR_PHB) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_SVP_TABLE_PROCESS_CTRL),
        .flds = ing_svp_table_flds
    },
    [BCMINT_LTSW_MPLS_LT_ING_DVP_TABLE] = {
        .name = ING_DVP_TABLEs,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_DVP) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_NO_CUT_THRU) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_DESTINATION) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_L2MC_L3MC_L2_OIF_SYS_DST_VALID) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_DVP_TABLE_STRENGTH_PROFILE_INDEX),
        .flds = ing_dvp_table_flds
    },
    [BCMINT_LTSW_MPLS_LT_EGR_DVP] = {
        .name = EGR_DVPs,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_DVP) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_OBJ_TABLE_SEL_1) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_EDIT_CTRL_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_TUNNEL_IDX_0) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_VC_LABEL_INDEX) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_CLASS_ID) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_SPLIT_HORIZON_CTRL) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_L2_MTU_VALUE) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_PROCESS_CTRL_2) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_SEQ_NUM_PROFILE_IDX) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_SEQ_NUM_COUNTER_IDX) |
            (1 << BCMINT_LTSW_MPLS_FLD_EGR_DVP_STRENGTH_PRFL_IDX),
        .flds = egr_dvp_flds
    },
    [BCMINT_LTSW_MPLS_LT_ING_VFI_TABLE] = {
        .name = ING_VFI_TABLEs,
        .fld_bmp =
            (1 << BCMINT_LTSW_MPLS_FLD_ING_VFI_TABLE_VFI) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_VFI_TABLE_DESTINATION) |
            (1 << BCMINT_LTSW_MPLS_FLD_ING_VFI_TABLE_DESTINATION_TYPE),
        .flds = ing_vfi_table_flds
    }
};

/******************************************************************************
 * Private functions
 */

static int
bcm56880_a0_dna_4_6_6_mpls_lt_db_get(int unit,
                                      bcmint_mpls_lt_db_t *lt_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_db, SHR_E_PARAM);

    /* Bitmap of LTs that valid on bcm56880 DNA_4_6_6. */
    lt_db->lt_bmp = (1 << BCMINT_LTSW_MPLS_LT_MPLS_ENTRY_1) |
                    (1 << BCMINT_LTSW_MPLS_LT_MPLS_ENTRY_2) |
                    (1 << BCMINT_LTSW_MPLS_LT_MPLS_ENTRY_3) |
                    (1 << BCMINT_LTSW_MPLS_LT_EGR_L3_TUNNEL_0) |
                    (1 << BCMINT_LTSW_MPLS_LT_EGR_L3_TUNNEL_1) |
                    (1 << BCMINT_LTSW_MPLS_LT_EGR_MPLS_VPN) |
                    (1 << BCMINT_LTSW_MPLS_LT_ING_SVP_TABLE) |
                    (1 << BCMINT_LTSW_MPLS_LT_ING_DVP_TABLE) |
                    (1 << BCMINT_LTSW_MPLS_LT_EGR_DVP) |
                    (1 << BCMINT_LTSW_MPLS_LT_ING_VFI_TABLE);

    lt_db->lts = bcm56880_a0_dna_4_6_6_mpls_lt;

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_mpls_drv_t bcm56880_a0_dna_4_6_6_ltsw_mpls_drv = {
    /* MPLS LT database */
    .mpls_lt_db_get = bcm56880_a0_dna_4_6_6_mpls_lt_db_get
};

/******************************************************************************
 * Public functions
 */
int
bcm56880_a0_dna_4_6_6_ltsw_mpls_drv_attach(int unit)
{
    return mbcm_ltsw_mpls_drv_set(unit,
                                  &bcm56880_a0_dna_4_6_6_ltsw_mpls_drv);
}
