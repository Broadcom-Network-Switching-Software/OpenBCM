/*! \file bcm5699x_l3_intf.c
 *
 * BCM5699x L3 Interface subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/l3_intf.h>
#include <bcm_int/ltsw/xgs/l3_intf.h>
#include <bcm_int/ltsw/l3_intf_int.h>


#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

static const char *tnl_type_str[] = {
    NONEs,
    IPV4s,
    IPV6s,
    MPLSs,
    "INVALID"
};

static const char *dscp_action_str[] = {
    NONEs,
    FIXEDs,
    MAPs,
    "INVALID"
};

static const char *l2_tag_action_str[] = {
    NONEs,
    FIXEDs,
    MAPs,
    "INVALID"
};

/*!
 * \brief Get tunnel type from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value tunnel type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
tnl_type_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< XGS_LTSW_L3_INTF_TNL_TYPE_CNT; i++) {
        if (!sal_strcmp(symbol, tnl_type_str[i])) {
            break;
        }
    }

    if (i >= XGS_LTSW_L3_INTF_TNL_TYPE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get tunnel type sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value Tunnel type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
tnl_type_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= XGS_LTSW_L3_INTF_TNL_TYPE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = tnl_type_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get DSCP action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value DSCP action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dscp_action_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< XGS_LTSW_L3_INTF_DSCP_ACTION_CNT; i++) {
        if (!sal_strcmp(symbol, dscp_action_str[i])) {
            break;
        }
    }

    if (i >= XGS_LTSW_L3_INTF_DSCP_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get DSCP action sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value DSCP action.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dscp_action_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= XGS_LTSW_L3_INTF_DSCP_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = dscp_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get L2 tag action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value L2 tag action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l2_tag_action_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< XGS_LTSW_L3_INTF_L2_TAG_ACTION_CNT; i++) {
        if (!sal_strcmp(symbol, l2_tag_action_str[i])) {
            break;
        }
    }

    if (i >= XGS_LTSW_L3_INTF_L2_TAG_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get L2 tag action sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value L2 tag action.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l2_tag_action_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= XGS_LTSW_L3_INTF_L2_TAG_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = l2_tag_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get adaptation lookup key from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value L2 tag action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
egr_adapt_lookup_key_mode_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    SHR_FUNC_ENTER(unit);

    if (!sal_strcmp(symbol, NONEs)) {
        *value = bcmVlanTranslateEgressKeyInvalid;
    } else if (!sal_strcmp(symbol, VFIs)) {
        *value = bcmVlanTranslateEgressKeyVpn;
    } else if (!sal_strcmp(symbol, VFI_AND_DVP_GRP_IDs)) {
        *value = bcmVlanTranslateEgressKeyVpnGportGroup;
    } else if (!sal_strcmp(symbol, VFI_AND_PORT_GRP_IDs)) {
        *value = bcmVlanTranslateEgressKeyVpnPortGroup;
    } else if (!sal_strcmp(symbol, SVPs)) {
        *value = bcmVlanTranslateEgressKeySrcGport;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get adaptation lookup key sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value L2 tag action.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
egr_adapt_lookup_key_mode_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    switch (value) {
        case bcmVlanTranslateEgressKeyInvalid:
            *symbol = NONEs;
            break;

        case bcmVlanTranslateEgressKeyVpn:
            *symbol = VFIs;
            break;

        case bcmVlanTranslateEgressKeyVpnGportGroup:
            *symbol = VFI_AND_DVP_GRP_IDs;
            break;
        case bcmVlanTranslateEgressKeyVpnPortGroup:
            *symbol = VFI_AND_PORT_GRP_IDs;
            break;

        case bcmVlanTranslateEgressKeySrcGport:
            *symbol = SVPs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

static const
bcmint_ltsw_l3_intf_fld_t l3_iif_flds[XGS_LTSW_L3_INTF_TBL_ING_FLD_CNT] = {
    [XGS_LTSW_L3_INTF_TBL_ING_INDEX] = {
        .fld_name = L3_IIF_IDs,
        .fld_id = L3_IIFt_L3_IIF_IDf,
        .key = true,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_VRF] = {
        .fld_name = VRF_IDs,
        .fld_id = L3_IIFt_VRF_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_L3_IIF_PROFILE_ID] = {
        .fld_name = L3_IIF_PROFILE_IDs,
        .fld_id = L3_IIFt_L3_IIF_PROFILE_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_L3_IP_OPTION_CONTROL_PROFILE_ID] = {
        .fld_name = L3_IP_OPTION_CONTROL_PROFILE_IDs,
        .fld_id = L3_IIFt_L3_IP_OPTION_CONTROL_PROFILE_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_IPV4UC] = {
        .fld_name = IPV4_UCs,
        .fld_id = L3_IIFt_IPV4_UCf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_IPV6UC] = {
        .fld_name = IPV6_UCs,
        .fld_id = L3_IIFt_IPV6_UCf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_IPV4MC] = {
        .fld_name = IPV4_MCs,
        .fld_id = L3_IIFt_IPV4_MCf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_IPV6MC] = {
        .fld_name = IPV6_MCs,
        .fld_id = L3_IIFt_IPV6_MCf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_L3_MC_IIF_ID] = {
        .fld_name = L3_MC_IIF_IDs,
        .fld_id = L3_IIFt_L3_MC_IIF_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_L3_OVERRIDE_IP_MC_DO_VLAN] = {
        .fld_name = L3_OVERRIDE_IP_MC_DO_VLANs,
        .fld_id = L3_IIFt_L3_OVERRIDE_IP_MC_DO_VLANf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_CLASS_ID] = {
        .fld_name = CLASS_IDs,
        .fld_id = L3_IIFt_CLASS_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_ECN_TNL_DECAP_ID] = {
        .fld_name = ECN_TNL_DECAP_IDs,
        .fld_id = L3_IIFt_ECN_TNL_DECAP_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION] = {
        .fld_name = CTR_ING_EFLEX_ACTIONs,
        .fld_id = L3_IIFt_CTR_ING_EFLEX_ACTIONf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_INT_IFA] = {
        .fld_name = INBAND_TELEMETRY_IFAs,
        .fld_id = L3_IIFt_INBAND_TELEMETRY_IFAf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_INT_IOAM] = {
        .fld_name = INBAND_TELEMETRY_IOAMs,
        .fld_id = L3_IIFt_INBAND_TELEMETRY_IOAMf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_ING_INT_DATAPLANE] = {
        .fld_name = INBAND_TELEMETRY_DATAPLANEs,
        .fld_id = L3_IIFt_INBAND_TELEMETRY_DATAPLANEf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
};

static const
bcmint_ltsw_l3_intf_fld_t l3_eif_flds[XGS_LTSW_L3_INTF_TBL_EGR_FLD_CNT] = {
    [XGS_LTSW_L3_INTF_TBL_EGR_INDEX] = {
        .fld_name = L3_EIF_IDs,
        .fld_id = L3_EIFt_L3_EIF_IDf,
        .key = true,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_VLAN_ID] = {
        .fld_name = VLAN_IDs,
        .fld_id = L3_EIFt_VLAN_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_MAC_SA] = {
        .fld_name = MAC_SAs,
        .fld_id = L3_EIFt_MAC_SAf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_TTL] = {
        .fld_name = TTLs,
        .fld_id = L3_EIFt_TTLf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_L2_SWITCH] = {
        .fld_name = L2_SWITCHs,
        .fld_id = L3_EIFt_L2_SWITCHf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_CLASS_ID] = {
        .fld_name = CLASS_IDs,
        .fld_id = L3_EIFt_CLASS_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV4_ENCAP_INDEX] = {
        .fld_name = TNL_IPV4_ENCAP_INDEXs,
        .fld_id = L3_EIFt_TNL_IPV4_ENCAP_INDEXf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV6_ENCAP_INDEX] = {
        .fld_name = TNL_IPV6_ENCAP_INDEXs,
        .fld_id = L3_EIFt_TNL_IPV6_ENCAP_INDEXf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_TNL_MPLS_ENCAP_INDEX] = {
        .fld_name = TNL_MPLS_ENCAP_INDEXs,
        .fld_id = L3_EIFt_TNL_MPLS_ENCAP_INDEXf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE] = {
        .fld_name = TNL_TYPEs,
        .fld_id = L3_EIFt_TNL_TYPEf,
        .key = false,
        .symbol = true,
        .symbol_to_scalar = tnl_type_symbol_to_scalar,
        .scalar_to_symbol = tnl_type_scalar_to_symbol
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION] = {
        .fld_name = PHB_EGR_DSCP_ACTIONs,
        .fld_id = L3_EIFt_PHB_EGR_DSCP_ACTIONf,
        .key = false,
        .symbol = true,
        .symbol_to_scalar = dscp_action_symbol_to_scalar,
        .scalar_to_symbol = dscp_action_scalar_to_symbol
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_DSCP] = {
        .fld_name = DSCPs,
        .fld_id = L3_EIFt_DSCPf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_IP_INT_PRI_TO_DSCP_ID] = {
        .fld_name = PHB_EGR_IP_INT_PRI_TO_DSCP_IDs,
        .fld_id = L3_EIFt_PHB_EGR_IP_INT_PRI_TO_DSCP_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_OTAG_ACTION] = {
        .fld_name = PHB_EGR_L2_OTAG_ACTIONs,
        .fld_id = L3_EIFt_PHB_EGR_L2_OTAG_ACTIONf,
        .key = false,
        .symbol = true,
        .symbol_to_scalar = l2_tag_action_symbol_to_scalar,
        .scalar_to_symbol = l2_tag_action_scalar_to_symbol
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_OPRI] = {
        .fld_name = OPRIs,
        .fld_id = L3_EIFt_OPRIf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_OCFI] = {
        .fld_name = OCFIs,
        .fld_id = L3_EIFt_OCFIf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_INT_PRI_TO_OTAG_ID] = {
        .fld_name = PHB_EGR_L2_INT_PRI_TO_OTAG_IDs,
        .fld_id = L3_EIFt_PHB_EGR_L2_INT_PRI_TO_OTAG_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION] = {
        .fld_name = CTR_EGR_EFLEX_ACTIONs,
        .fld_id = L3_EIFt_CTR_EGR_EFLEX_ACTIONf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_VFI] = {
        .fld_name = VFI_IDs,
        .fld_id = L3_EIFt_VFI_IDf,
        .key = false,
        .symbol = false,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XGS_LTSW_L3_INTF_TBL_EGR_ADAPT_LOOKUP_KEY_MODE] = {
        .fld_name = EGR_ADAPT_LOOKUP_KEY_MODEs,
        .fld_id = L3_EIFt_EGR_ADAPT_LOOKUP_KEY_MODEf,
        .key = false,
        .symbol = true,
        .symbol_to_scalar = egr_adapt_lookup_key_mode_symbol_to_scalar,
        .scalar_to_symbol = egr_adapt_lookup_key_mode_scalar_to_symbol
    },
};

static const bcmint_ltsw_l3_intf_tbl_t l3_intf_tbls[] = {
    [XGS_LTSW_L3_INTF_TBL_IIF] = {
        .name = L3_IIFs,
        .tbl_id = L3_IIFt,
        .attr = XGS_LTSW_L3_INTF_TBL_ATTR_ING,
        .fld_cnt = L3_IIFt_FIELD_COUNT,
        .fld_bmp = (1LL << XGS_LTSW_L3_INTF_TBL_ING_INDEX) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_VRF) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_L3_IIF_PROFILE_ID) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_L3_IP_OPTION_CONTROL_PROFILE_ID) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_IPV4UC) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_IPV6UC) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_IPV4MC) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_IPV6MC) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_L3_MC_IIF_ID) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_L3_OVERRIDE_IP_MC_DO_VLAN) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_CLASS_ID) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_ECN_TNL_DECAP_ID) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_FLEX_CTR_ACTION) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_INT_IFA) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_INT_IOAM) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_ING_INT_DATAPLANE),
        .flds = l3_iif_flds
    },
    [XGS_LTSW_L3_INTF_TBL_EIF] = {
        .name = L3_EIFs,
        .tbl_id = L3_EIFt,
        .attr = XGS_LTSW_L3_INTF_TBL_ATTR_EGR,
        .fld_cnt = L3_EIFt_FIELD_COUNT,
        .fld_bmp = (1LL << XGS_LTSW_L3_INTF_TBL_EGR_INDEX) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_VLAN_ID) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_MAC_SA) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_TTL) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_L2_SWITCH) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_CLASS_ID) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV4_ENCAP_INDEX) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_IPV6_ENCAP_INDEX) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_MPLS_ENCAP_INDEX) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_TNL_TYPE) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_DSCP_ACTION) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_DSCP) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_IP_INT_PRI_TO_DSCP_ID) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_OTAG_ACTION) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_OPRI) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_OCFI) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_PHB_EGR_L2_INT_PRI_TO_OTAG_ID) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_FLEX_CTR_ACTION) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_VFI) |
                   (1LL << XGS_LTSW_L3_INTF_TBL_EGR_ADAPT_LOOKUP_KEY_MODE),
        .flds = l3_eif_flds
    },
};


int
bcm5699x_ltsw_l3_intf_tbl_db_get(int unit,
                                 bcmint_ltsw_l3_intf_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    tbl_db->tbl_bmp = (1 << XGS_LTSW_L3_INTF_TBL_IIF) |
                      (1 << XGS_LTSW_L3_INTF_TBL_EIF);

    tbl_db->tbls = l3_intf_tbls;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_intf sub driver functions for bcm5699x.
 */
static mbcm_ltsw_l3_intf_drv_t bcm5699x_l3_intf_sub_drv = {
    .l3_intf_tbl_db_get = bcm5699x_ltsw_l3_intf_tbl_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_l3_intf_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_intf_drv_set(unit, &bcm5699x_l3_intf_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
