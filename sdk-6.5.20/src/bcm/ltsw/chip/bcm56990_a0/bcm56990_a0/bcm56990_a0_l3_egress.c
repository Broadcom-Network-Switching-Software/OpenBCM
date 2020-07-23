/*! \file bcm56990_a0_l3_egress.c
 *
 * BCM56990_A0 L3 egress subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/l3_egress.h>
#include <bcm_int/ltsw/xgs/l3_egress.h>
#include <bcm_int/ltsw/l3_egress_int.h>


#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

static const char *tnl_mpls_label_action_str[] = {
    NONEs,
    SWAPs,
    PRESERVEs,
    "INVALID"
};

static const char *tnl_mpls_exp_mode_str[] = {
    FIXEDs,
    MAPs,
    USE_INNER_LABELs,
    USE_SWAP_LABELs,
    "INVALID"
};

static const char *lb_mode_str[] = {
    REGULARs,
    RANDOMs,
    RESILIENTs,
    "INVALID"
};

static const char *wgt_sz_str[] = {
    WEIGHTED_SIZE_0_127s,
    WEIGHTED_SIZE_256s,
    WEIGHTED_SIZE_512s,
    WEIGHTED_SIZE_1Ks,
    WEIGHTED_SIZE_2Ks,
    WEIGHTED_SIZE_4Ks,
    "INVALID"
};

static const char *res_sz_str[] = {
    RH_SIZE_64s,
    RH_SIZE_128s,
    RH_SIZE_256s,
    RH_SIZE_512s,
    RH_SIZE_1Ks,
    RH_SIZE_2Ks,
    RH_SIZE_4Ks,
    RH_SIZE_8Ks,
    RH_SIZE_16Ks,
    RH_SIZE_32Ks,
    "INVALID"
};

static const char *wgt_mode_str[] = {
    MEMBER_REPLICATIONs,
    MEMBER_WEIGHTs,
    "INVALID"
};


/******************************************************************************
 * Private functions
 */
/*!
 * \brief Get label action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
label_action_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_CNT; i++) {
        if (!sal_strcmp(symbol, tnl_mpls_label_action_str[i])) {
            break;
        }
    }

    if (i >= BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_CNT) {
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

    if (value >= BCMINT_LTSW_TNL_MPLS_LABEL_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = tnl_mpls_label_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get exp mode from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value exp mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
exp_mode_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMINT_LTSW_TNL_MPLS_EXP_MODE_CNT; i++) {
        if (!sal_strcmp(symbol, tnl_mpls_exp_mode_str[i])) {
            break;
        }
    }

    if (i >= BCMINT_LTSW_TNL_MPLS_EXP_MODE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Getexp mode sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
exp_mode_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= BCMINT_LTSW_TNL_MPLS_EXP_MODE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = tnl_mpls_exp_mode_str[value];

exit:
    SHR_FUNC_EXIT();
}

static const
bcmint_ltsw_l3_egress_fld_t l3_egress_uc_flds[BCMINT_XGS_L3_EGRESS_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID] = {
        NHOP_IDs,
        true,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_VLAN_ID] = {
        VLAN_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_MODPORT] = {
        MODPORTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID] = {
        TRUNK_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK] = {
        IS_TRUNKs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU] = {
        COPY_TO_CPUs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DROP] = {
        DROPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID] = {
        L3_EIF_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DST_MAC] = {
        DST_MACs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC] = {
        KEEP_DST_MACs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL] = {
        KEEP_TTLs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID] = {
        KEEP_VLAN_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC] = {
        KEEP_SRC_MACs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID] = {
        CLASS_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT] = {
        CTR_EGR_EFLEX_OBJECTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID] = {
        L3_UC_NHOP_CTR_EGR_EFLEX_ACTION_IDs,
        false,
        false,
        NULL,
        NULL
    },
};

static const
bcmint_ltsw_l3_egress_fld_t l3_egress_mc_flds[BCMINT_XGS_L3_EGRESS_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID] = {
        NHOP_IDs,
        true,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_REPLACE_DST_MAC] = {
        REPLACE_DST_MACs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID] = {
        L3_EIF_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DST_MAC] = {
        DST_MACs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC] = {
        KEEP_DST_MACs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL] = {
        KEEP_TTLs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID] = {
        KEEP_VLAN_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC] = {
        KEEP_SRC_MACs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DROP_L3] = {
        DROP_L3s,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID] = {
        CLASS_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT] = {
        CTR_EGR_EFLEX_OBJECTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID] = {
        L3_MC_NHOP_CTR_EGR_EFLEX_ACTION_IDs,
        false,
        false,
        NULL,
        NULL
    },
};

static const
bcmint_ltsw_l3_egress_fld_t l3_egress_mpls_transit_flds[BCMINT_XGS_L3_EGRESS_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID] = {
        NHOP_IDs,
        true,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_MODPORT] = {
        MODPORTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID] = {
        TRUNK_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK] = {
        IS_TRUNKs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU] = {
        COPY_TO_CPUs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DROP] = {
        DROPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID] = {
        L3_EIF_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID] = {
        TNL_MPLS_DST_MAC_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_LABEL] = {
        LABELs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_LABEL_TTL] = {
        LABEL_TTLs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION] = {
        LABEL_ACTIONs,
        false,
        true,
        label_action_symbol_to_scalar,
        label_action_scalar_to_symbol,
    },
    [BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE] = {
        EXP_MODEs,
        false,
        true,
        exp_mode_symbol_to_scalar,
        exp_mode_scalar_to_symbol,
    },
    [BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP] = {
        LABEL_EXPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID] = {
        PHB_EGR_MPLS_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_PRI] = {
        PRIs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CFI] = {
        CFIs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_ID] = {
        ECN_CNG_TO_MPLS_EXP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_PRIORITY] = {
        ECN_CNG_TO_MPLS_EXP_PRIORITYs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_ID] = {
        ECN_IP_TO_MPLS_EXP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_PRIORITY] = {
        ECN_IP_TO_MPLS_EXP_PRIORITYs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT] = {
        CTR_EGR_EFLEX_OBJECTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID] = {
        TNL_MPLS_NHOP_CTR_EGR_EFLEX_ACTION_IDs,
        false,
        false,
        NULL,
        NULL
    },
};

static const
bcmint_ltsw_l3_egress_fld_t l3_egress_mpls_encap_flds[BCMINT_XGS_L3_EGRESS_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID] = {
        NHOP_IDs,
        true,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_MODPORT] = {
        MODPORTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID] = {
        TRUNK_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK] = {
        IS_TRUNKs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU] = {
        COPY_TO_CPUs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DROP] = {
        DROPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID] = {
        L3_EIF_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID] = {
        TNL_MPLS_DST_MAC_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_VPN_LABEL] = {
        VPN_LABELs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_LABEL] = {
        LABELs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_LABEL_TTL] = {
        LABEL_TTLs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE] = {
        EXP_MODEs,
        false,
        true,
        exp_mode_symbol_to_scalar,
        exp_mode_scalar_to_symbol
    },
    [BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP] = {
        LABEL_EXPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID] = {
        PHB_EGR_MPLS_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_PRI] = {
        PRIs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CFI] = {
        CFIs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_ID] = {
        ECN_CNG_TO_MPLS_EXP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_PRIORITY] = {
        ECN_CNG_TO_MPLS_EXP_PRIORITYs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_ID] = {
        ECN_IP_TO_MPLS_EXP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_PRIORITY] = {
        ECN_IP_TO_MPLS_EXP_PRIORITYs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT] = {
        CTR_EGR_EFLEX_OBJECTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID] = {
        TNL_MPLS_NHOP_CTR_EGR_EFLEX_ACTION_IDs,
        false,
        false,
        NULL,
        NULL
    },
};

static const
bcmint_ltsw_l3_egress_fld_t l3_egress_ifp_action_flds[BCMINT_XGS_L3_EGRESS_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID] = {
        NHOP_IDs,
        true,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID] = {
        L3_EIF_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DST_MAC] = {
        DST_MACs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC] = {
        KEEP_DST_MACs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL] = {
        KEEP_TTLs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID] = {
        KEEP_VLAN_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC] = {
        KEEP_SRC_MACs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID] = {
        CLASS_IDs,
        false,
        false,
        NULL,
        NULL
    },
};

static const bcmint_ltsw_l3_egress_tbl_t l3_egress_tbls[] = {
    [BCMINT_LTSW_L3_EGRESS_TBL_UC] = {
        .name = L3_UC_NHOPs,
        .fld_cnt = L3_UC_NHOPt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_VLAN_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_MODPORT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DROP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID),
        .flds = l3_egress_uc_flds
    },
    [BCMINT_LTSW_L3_EGRESS_TBL_MC] = {
        .name = L3_MC_NHOPs,
        .fld_cnt = L3_MC_NHOPt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_REPLACE_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DROP_L3) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID),
        .flds = l3_egress_mc_flds
    },
    [BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT] = {
        .name = TNL_MPLS_TRANSIT_NHOPs,
        .fld_cnt = TNL_MPLS_TRANSIT_NHOPt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_MODPORT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DROP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_TTL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_ACTION) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_PRI) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CFI) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_PRIORITY) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_PRIORITY) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID),
        .flds = l3_egress_mpls_transit_flds
    },
    [BCMINT_LTSW_L3_EGRESS_TBL_MPLS_ENCAP] = {
        .name = TNL_MPLS_ENCAP_NHOPs,
        .fld_cnt = TNL_MPLS_ENCAP_NHOPt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_MODPORT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DROP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_TNL_MPLS_DST_MAC_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_VPN_LABEL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_TTL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_EXP_MODE) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_LABEL_EXP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_MPLS_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_PRI) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CFI) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_CNG_TO_MPLS_EXP_PRIORITY) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ECN_IP_TO_MPLS_EXP_PRIORITY) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID),
        .flds = l3_egress_mpls_encap_flds
    },
    [BCMINT_LTSW_L3_EGRESS_TBL_IFP_ACTION] = {
        .name = FP_ING_ACTION_NHOPs,
        .fld_cnt = FP_ING_ACTION_NHOPt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID),
        .flds = l3_egress_ifp_action_flds
    },
};

/*!
 * \brief Get load balance mode from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lb_mode_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMINT_LTSW_L3_ECMP_LB_MODE_CNT; i++) {
        if (!sal_strcmp(symbol, lb_mode_str[i])) {
            break;
        }
    }

    if (i >= BCMINT_LTSW_L3_ECMP_LB_MODE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get load balance mode sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value load balance mode.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lb_mode_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= BCMINT_LTSW_L3_ECMP_LB_MODE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = lb_mode_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ECMP weighted size from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
weighted_size_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMINT_LTSW_L3_ECMP_WGT_SZ_CNT; i++) {
        if (!sal_strcmp(symbol, wgt_sz_str[i])) {
            break;
        }
    }

    if (i >= BCMINT_LTSW_L3_ECMP_WGT_SZ_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ECMP weighted size sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value ECMP weighted size.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
weighted_size_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= BCMINT_LTSW_L3_ECMP_WGT_SZ_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = wgt_sz_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get resilent hash size from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rh_size_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMINT_LTSW_L3_ECMP_RES_SZ_CNT; i++) {
        if (!sal_strcmp(symbol, res_sz_str[i])) {
            break;
        }
    }

    if (i >= BCMINT_LTSW_L3_ECMP_RES_SZ_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get resilent hash size sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value ECMP weighted size.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rh_size_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= BCMINT_LTSW_L3_ECMP_RES_SZ_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = res_sz_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get weighted mode from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
weighted_mode_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHT_MODE_CNT; i++) {
        if (!sal_strcmp(symbol, wgt_mode_str[i])) {
            break;
        }
    }

    if (i >= BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHT_MODE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get weighted mode sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value ECMP weighted size.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
weighted_mode_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= BCMINT_LTSW_L3_ECMP_MEMBER_WEIGHT_MODE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = wgt_mode_str[value];

exit:
    SHR_FUNC_EXIT();
}

static const
bcmint_ltsw_l3_egress_fld_t l3_ecmp_flds[BCMINT_XGS_L3_ECMP_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_ECMP_TBL_ECMP_ID] = {
        ECMP_IDs,
        true,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_LB_MODE] = {
        LB_MODEs,
        false,
        true,
        lb_mode_symbol_to_scalar,
        lb_mode_scalar_to_symbol
    },
    [BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS] = {
        MAX_PATHSs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS] = {
        NUM_PATHSs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_NHOP_ID] = {
        NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID] = {
        RH_NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_NHOP_SORTED] = {
        NHOP_SORTEDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_RH_SIZE] = {
        RH_SIZEs,
        false,
        true,
        rh_size_symbol_to_scalar,
        rh_size_scalar_to_symbol
    },
    [BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS] = {
        RH_NUM_PATHSs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_RH_RANDOM_SEED] = {
        RH_RANDOM_SEEDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID] = {
        ECMP_CTR_ING_EFLEX_ACTION_IDs,
        false,
        false,
        NULL,
        NULL
    },
};

static const
bcmint_ltsw_l3_egress_fld_t l3_ecmp_weighted_flds[BCMINT_XGS_L3_ECMP_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_ECMP_TBL_ECMP_ID] = {
        ECMP_IDs,
        true,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_SIZE] = {
        WEIGHTED_SIZEs,
        false,
        true,
        weighted_size_symbol_to_scalar,
        weighted_size_scalar_to_symbol
    },
    [BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS] = {
        NUM_PATHSs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_NHOP_ID] = {
        NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_WEIGHT] = {
        WEIGHTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_MODE] = {
        WEIGHTED_MODEs,
        false,
        true,
        weighted_mode_symbol_to_scalar,
        weighted_mode_scalar_to_symbol
    },
    [BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT] = {
        CTR_EGR_EFLEX_OBJECTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID] = {
        ECMP_CTR_ING_EFLEX_ACTION_IDs,
        false,
        false,
        NULL,
        NULL
    },
};

static const bcmint_ltsw_l3_egress_tbl_t l3_ecmp_tbls[BCMINT_LTSW_L3_ECMP_TBL_CNT] = {
    [BCMINT_LTSW_L3_ECMP_TBL_UL] = {
        .name = ECMPs,
        .fld_cnt = ECMPt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_LB_MODE) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_SORTED) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_SIZE) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_RANDOM_SEED) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID),
        .flds = l3_ecmp_flds
    },
    [BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL] = {
        .name = ECMP_WEIGHTEDs,
        .fld_cnt = ECMP_WEIGHTEDt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_SIZE) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_MODE) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID),
        .flds = l3_ecmp_weighted_flds
    },
};

static const
bcmint_ltsw_l3_egress_fld_t l3_ecmp_flexctr_action_flds[BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID] = {
        ECMP_CTR_ING_EFLEX_ACTION_IDs,
        true,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ACTION] = {
        ACTIONs,
        false,
        false,
        NULL,
        NULL
    },
};

static const bcmint_ltsw_l3_egress_tbl_t l3_ecmp_flexctr_action_tbls[] = {
    [BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_UL] = {
        .name = ECMP_CTR_ING_EFLEX_ACTIONs,
        .fld_cnt = ECMP_CTR_ING_EFLEX_ACTIONt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ACTION),
        .flds = l3_ecmp_flexctr_action_flds
    },
};


int
bcm56990_a0_ltsw_l3_egress_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    tbl_db->tbl_bmp = (1 << BCMINT_LTSW_L3_EGRESS_TBL_UC) |
                      (1 << BCMINT_LTSW_L3_EGRESS_TBL_MC) |
                      (1 << BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT) |
                      (1 << BCMINT_LTSW_L3_EGRESS_TBL_MPLS_ENCAP) |
                      (1 << BCMINT_LTSW_L3_EGRESS_TBL_IFP_ACTION);

    tbl_db->tbls = l3_egress_tbls;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_ltsw_l3_ecmp_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    tbl_db->tbl_bmp = (1 << BCMINT_LTSW_L3_ECMP_TBL_UL) |
                      (1 << BCMINT_LTSW_L3_ECMP_TBL_OL) |
                      (1 << BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL) |
                      (1 << BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL);

    tbl_db->tbls = l3_ecmp_tbls;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_ltsw_l3_ecmp_flexctr_action_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    tbl_db->tbl_bmp = (1 << BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_UL) |
                      (1 << BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_OL);

    tbl_db->tbls = l3_ecmp_flexctr_action_tbls;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_egress sub driver functions for bcm56990_a0.
 */
static mbcm_ltsw_l3_egress_drv_t bcm56990_a0_l3_egress_sub_drv = {
    .l3_egress_tbl_db_get = bcm56990_a0_ltsw_l3_egress_tbl_db_get,
    .l3_ecmp_tbl_db_get = bcm56990_a0_ltsw_l3_ecmp_tbl_db_get,
    .l3_ecmp_flexctr_action_tbl_db_get = bcm56990_a0_ltsw_l3_ecmp_flexctr_action_tbl_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_l3_egress_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_drv_set(unit, &bcm56990_a0_l3_egress_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
