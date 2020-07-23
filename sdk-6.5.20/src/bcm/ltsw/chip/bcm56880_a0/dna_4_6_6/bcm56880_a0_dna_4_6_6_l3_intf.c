/*! \file bcm56880_a0_dna_4_6_6_l3_intf.c
 *
 * BCM56880_A0 DNA_4_6_6 L3 Intf Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/l3_intf.h>
#include <bcm_int/ltsw/xfs/l3_intf.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>


/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/* URPF mode symbol, align with bcm_l3_ingress_urpf_mode_t. */
static const char *urpfm_str[] = {
    DISABLEs,
    LOOSEs,
    STRICTs,
    ECMP_GT8s
};

/* IP Mcast mode, align with xfs_ltsw_l3_intf_ipmc_type_t. */
static const char *ipmct_str[] = {
    PIM_SMs,
    PIM_BIDIRs
};

/* EGR interface view type, align with xfs_ltsw_l3_intf_egr_view_t. */
static const char *egr_vt_str[] = {
    VT_L3_OIF_1s,
    VT_L3_OIF_1_MPLS_LERs,
    "Invalid"
};

/*!
 * \brief Get URPF mode from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value URPF mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
urpf_mode_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i, num;

    SHR_FUNC_ENTER(unit);

    num = COUNTOF(urpfm_str);

    for (i = 0; i< num; i++) {
        if (!sal_strcmp(symbol, urpfm_str[i])) {
            break;
        }
    }

    if (i >= num) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get URPF mode symbol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value URPF mode.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
urpf_mode_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= COUNTOF(urpfm_str)) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    *symbol = urpfm_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get IP mcast type from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value IP Mcast type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ipmc_type_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i, num;

    SHR_FUNC_ENTER(unit);

    num = COUNTOF(ipmct_str);

    for (i = 0; i< num; i++) {
        if (!sal_strcmp(symbol, ipmct_str[i])) {
            break;
        }
    }

    if (i >= num) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get IP Mcast type symbol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value IP mcast type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ipmc_type_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= COUNTOF(ipmct_str)) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    *symbol = ipmct_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get view type from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value View type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
egr_view_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < XFS_LTSW_L3_INTF_EGR_VT_CNT; i++) {
        if (!sal_strcmp(symbol, egr_vt_str[i])) {
            break;
        }
    }

    if (i >= XFS_LTSW_L3_INTF_EGR_VT_CNT) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get view type symbol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value View type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
egr_view_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= XFS_LTSW_L3_INTF_EGR_VT_CNT) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    *symbol = egr_vt_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*
 * ING_L3_IIF_TABLE
 */
static const
bcmint_ltsw_l3_intf_fld_t l3_iif_flds[] = {
    [XFS_LTSW_L3_INTF_TBL_ING_INDEX] = {
        .fld_name = L3_IIFs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_L3_IIFf,
        .key = true,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_URPF_DEFAULT_ROUTE_CHECK_ENABLE] = {
        .fld_name = URPF_DEFAULT_ROUTE_CHECK_ENABLEs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_URPF_DEFAULT_ROUTE_CHECK_ENABLEf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_URPF_MODE] = {
        .fld_name = URPF_MODEs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_URPF_MODEf,
        .key = false,
        .symbol = true,
        .symbol_to_scalar = urpf_mode_symbol_to_scalar,
        .scalar_to_symbol = urpf_mode_scalar_to_symbol
    },
    [XFS_LTSW_L3_INTF_TBL_ING_STRENGTH_PROFILE_INDEX] = {
        .fld_name = STRENGTH_PROFILE_INDEXs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_STRENGTH_PROFILE_INDEXf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_VRF] = {
        .fld_name = VRFs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_VRFf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION] = {
        .fld_name = FLEX_CTR_ACTIONs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_FLEX_CTR_ACTIONf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_CLASS_ID] = {
        .fld_name = CLASS_IDs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_CLASS_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_DSCP_PTR] = {
        .fld_name = DSCP_PTRs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_DSCP_PTRf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_RPA_ID_PROFILE] = {
        .fld_name = RPA_ID_PROFILEs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_RPA_ID_PROFILEf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_OPAQUE_CTRL_ID] = {
        .fld_name = OPAQUE_CTRL_IDs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_OPAQUE_CTRL_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_USE_DSCP_FOR_PHB] = {
        .fld_name = USE_DSCP_FOR_PHBs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_USE_DSCP_FOR_PHBf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_IP_MCAST_TYPE] = {
        .fld_name = IP_MCAST_TYPEs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_IP_MCAST_TYPEf,
        .key = false,
        .symbol = true,
        .symbol_to_scalar = ipmc_type_symbol_to_scalar,
        .scalar_to_symbol = ipmc_type_scalar_to_symbol
    },
    [XFS_LTSW_L3_INTF_TBL_ING_USE_L3_IIF_FOR_IPMC_LOOKUP] = {
        .fld_name = USE_L3_IIF_FOR_IPMC_LOOKUPs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_USE_L3_IIF_FOR_IPMC_LOOKUPf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_IPV4UC_ENABLE] = {
        .fld_name = IPV4UC_ENABLEs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_IPV4UC_ENABLEf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_IPV6UC_ENABLE] = {
        .fld_name = IPV6UC_ENABLEs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_IPV6UC_ENABLEf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_IPV4MC_ENABLE] = {
        .fld_name = IPV4MC_ENABLEs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_IPV4MC_ENABLEf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_ING_IPV6MC_ENABLE] = {
        .fld_name = IPV6MC_ENABLEs,
        .fld_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_IPV6MC_ENABLEf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    }
};

static const
bcmint_ltsw_l3_intf_fld_t l3_oif_ol_flds[] = {
    [XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = {
        .fld_name = L3_OIF_1s,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_L3_OIF_1f,
        .key = true,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF) |
                  (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_VIEW_T] = {
        .fld_name = VIEW_Ts,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_VIEW_Tf,
        .key = false,
        .symbol = true,
        .view_t = true,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF) |
                  (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = egr_view_symbol_to_scalar,
        .scalar_to_symbol = egr_view_scalar_to_symbol
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS] = {
        .fld_name = SRC_MAC_ADDRESSs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_SRC_MAC_ADDRESSf,
        .key = false,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_EDIT_CTRL_ID] = {
        .fld_name = EDIT_CTRL_IDs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_EDIT_CTRL_IDf,
        .key = false,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF) |
                  (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_VFI] = {
        .fld_name = VFIs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_VFIf,
        .key = false,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF) |
                  (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_L3_OIF_PROCESS_CTRL] = {
        .fld_name = L3_OIF_PROCESS_CTRLs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_L3_OIF_PROCESS_CTRLf,
        .key = false,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF) |
                  (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_TUNNEL_IDX] = {
        .fld_name = TUNNEL_IDX_0s,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_TUNNEL_IDX_0f,
        .key = false,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF) |
                  (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1] = {
        .fld_name = OBJ_TABLE_SEL_1s,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_OBJ_TABLE_SEL_1f,
        .key = false,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF) |
                  (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION] = {
        .fld_name = FLEX_CTR_ACTIONs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_FLEX_CTR_ACTIONf,
        .key = false,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF) |
                  (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_EFP_CTRL_ID] = {
        .fld_name = EFP_CTRL_IDs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_EFP_CTRL_IDf,
        .key = false,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF) |
                  (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX] = {
        .fld_name = STRENGTH_PRFL_IDXs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_STRENGTH_PRFL_IDXf,
        .key = false,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_L3_OIF) |
                  (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX] = {
        .fld_name = L3_VPN_INDEXs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_L3_VPN_INDEXf,
        .key = false,
        .symbol = false,
        .view_t = false,
        .vt_bmp = (1UL << XFS_LTSW_L3_INTF_EGR_VT_MPLS_LER),
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    }
};

static const
bcmint_ltsw_l3_intf_fld_t l3_oif_ul_flds[] = {
    [XFS_LTSW_L3_INTF_TBL_EGR_INDEX] = {
        .fld_name = L3_OIF_2s,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t_L3_OIF_2f,
        .key = true,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS] = {
        .fld_name = SRC_MAC_ADDRESSs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t_SRC_MAC_ADDRESSf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG] = {
        .fld_name = TNL_VLAN_TAGs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t_TNL_VLAN_TAGf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_L2_HDR_ACTION] = {
        .fld_name = L2_HDR_ACTIONs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t_L2_HDR_ACTIONf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_STG] = {
        .fld_name = STGs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t_STGf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_MEMBERSHIP_PROFILE_IDX] = {
        .fld_name = MEMBERSHIP_PROFILE_IDXs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t_MEMBERSHIP_PROFILE_IDXf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_UNTAG_PROFILE_INDEX] = {
        .fld_name = UNTAG_PROFILE_INDEXs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t_UNTAG_PROFILE_INDEXf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION] = {
        .fld_name = FLEX_CTR_ACTIONs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t_FLEX_CTR_ACTIONf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_L3_INTF_TBL_EGR_EFP_CTRL_ID] = {
        .fld_name = EFP_CTRL_IDs,
        .fld_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t_EFP_CTRL_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    }
};

static const
bcmint_ltsw_l3_intf_tbl_t bcm56880_a0_dna_4_6_6_l3_intf_tbls[] = {
    [XFS_LTSW_L3_INTF_TBL_IIF] = {
        .name = ING_L3_IIF_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt,
        .attr = XFS_LTSW_L3_INTF_TBL_ATTR_ING,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_ING_L3_IIF_TABLEt_FIELD_COUNT,
        .fld_bmp =
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_INDEX) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_URPF_DEFAULT_ROUTE_CHECK_ENABLE) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_URPF_MODE) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_STRENGTH_PROFILE_INDEX) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_VRF) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_CLASS_ID) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_DSCP_PTR) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_RPA_ID_PROFILE) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_OPAQUE_CTRL_ID) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_USE_DSCP_FOR_PHB) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IP_MCAST_TYPE) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_USE_L3_IIF_FOR_IPMC_LOOKUP) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV4UC_ENABLE) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV6UC_ENABLE) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV4MC_ENABLE) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_ING_IPV6MC_ENABLE),
        .flds = l3_iif_flds
    },
    [XFS_LTSW_L3_INTF_TBL_OIF_OL] = {
        .name = EGR_L3_OIF_1s,
        .tbl_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t,
        .attr = XFS_LTSW_L3_INTF_TBL_ATTR_EGR | XFS_LTSW_L3_INTF_TBL_ATTR_OL,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_1t_FIELD_COUNT,
        .fld_bmp =
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_INDEX) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_VIEW_T) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_EDIT_CTRL_ID) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_VFI) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_OIF_PROCESS_CTRL) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_TUNNEL_IDX) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_OBJ_TABLE_SEL_1) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_EFP_CTRL_ID) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_STRENGTH_PRFL_IDX) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L3_VPN_INDEX),
        .flds = l3_oif_ol_flds
    },
    [XFS_LTSW_L3_INTF_TBL_OIF_UL] = {
        .name = EGR_L3_OIF_2s,
        .tbl_id = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t,
        .attr = XFS_LTSW_L3_INTF_TBL_ATTR_EGR | XFS_LTSW_L3_INTF_TBL_ATTR_UL,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_EGR_L3_OIF_2t_FIELD_COUNT,
        .fld_bmp =
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_INDEX) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_SRC_MAC_ADDRESS) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_TNL_VLAN_TAG) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_L2_HDR_ACTION) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_STG) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_MEMBERSHIP_PROFILE_IDX) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_UNTAG_PROFILE_INDEX) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION) |
            (1ULL << XFS_LTSW_L3_INTF_TBL_EGR_EFP_CTRL_ID),
        .flds = l3_oif_ul_flds
    }
};

/******************************************************************************
 * Private functions
 */

static int
bcm56880_a0_dna_4_6_6_l3_intf_tbl_db_get(int unit,
                                          bcmint_ltsw_l3_intf_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    /* Bitmap of LTs that valid on bcm56880 DNA_4_6_6. */
    tbl_db->tbl_bmp = (1 << XFS_LTSW_L3_INTF_TBL_IIF) |
                      (1 << XFS_LTSW_L3_INTF_TBL_OIF_OL) |
                      (1 << XFS_LTSW_L3_INTF_TBL_OIF_UL);
    tbl_db->tbls = bcm56880_a0_dna_4_6_6_l3_intf_tbls;

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_l3_intf_drv_t bcm56880_a0_dna_4_6_6_ltsw_l3_intf_drv = {
    .l3_intf_tbl_db_get = bcm56880_a0_dna_4_6_6_l3_intf_tbl_db_get,

    .l3_intf_init = xfs_ltsw_l3_intf_init,
    .l3_intf_deinit = xfs_ltsw_l3_intf_deinit,

    /* Ingress interface. */
    .l3_ing_intf_id_range_get = xfs_ltsw_l3_ing_intf_id_range_get,
    .l3_ing_class_id_range_get = xfs_ltsw_l3_ing_class_id_range_get,
    .l3_ing_intf_create = xfs_ltsw_l3_ing_intf_create,
    .l3_ing_intf_destroy = xfs_ltsw_l3_ing_intf_destroy,
    .l3_ing_intf_get = xfs_ltsw_l3_ing_intf_get,
    .l3_ing_intf_find = xfs_ltsw_l3_ing_intf_find,
    .l3_ing_intf_traverse = xfs_ltsw_l3_ing_intf_traverse,
    .l3_ing_intf_rp_op = xfs_ltsw_l3_ing_intf_rp_op,
    .l3_ing_intf_rp_get = xfs_ltsw_l3_ing_intf_rp_get,
    .l3_ing_intf_flexctr_attach = xfs_ltsw_l3_ing_intf_flexctr_attach,
    .l3_ing_intf_flexctr_detach = xfs_ltsw_l3_ing_intf_flexctr_detach,
    .l3_ing_intf_flexctr_info_get = xfs_ltsw_l3_ing_intf_flexctr_info_get,

    /* Egress interface. */
    .l3_egr_l2cpu_intf_id_get = xfs_ltsw_l3_egr_l2cpu_intf_id_get,
    .l3_egr_intf_id_range_get = xfs_ltsw_l3_egr_intf_id_range_get,
    .l3_egr_intf_to_index = xfs_ltsw_l3_egr_intf_to_index,
    .l3_egr_index_to_intf = xfs_ltsw_l3_egr_index_to_intf,
    .l3_egr_intf_create = xfs_ltsw_l3_egr_intf_create,
    .l3_egr_intf_get = xfs_ltsw_l3_egr_intf_get,
    .l3_egr_intf_find = xfs_ltsw_l3_egr_intf_find,
    .l3_egr_intf_delete = xfs_ltsw_l3_egr_intf_delete,
    .l3_egr_intf_delete_all = xfs_ltsw_l3_egr_intf_delete_all,
    .l3_intf_tnl_init_get = xfs_ltsw_l3_intf_tnl_init_get,
    .l3_intf_vfi_update = xfs_ltsw_l3_intf_vfi_update,
    .l3_intf_mpls_recover_all = xfs_ltsw_l3_intf_mpls_recover_all
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_dna_4_6_6_ltsw_l3_intf_drv_attach(int unit)
{
    return mbcm_ltsw_l3_intf_drv_set(unit,
                                     &bcm56880_a0_dna_4_6_6_ltsw_l3_intf_drv);
}
