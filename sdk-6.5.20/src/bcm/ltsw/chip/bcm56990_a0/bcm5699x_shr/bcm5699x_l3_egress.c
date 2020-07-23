/*! \file bcm5699x_l3_egress.c
 *
 * BCM5699x L3 Egress subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/l3_egress.h>
#include <bcm_int/ltsw/xgs/l3_egress.h>
#include <bcm_int/ltsw/l3_egress_int.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>

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
    "INVALID",
    "INVALID",
    WEIGHTED_SIZE_0_511s,
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

static const char *l3_nhop_overlay_limit_str[] = {
    /*! Allocate no entries for overlay. */
    OVERLAY_LIMIT_0s,

    /*! Allocate 4K entries for overlay. */
    OVERLAY_LIMIT_4Ks,

    /*! Allocate 8K entries for overlay. */
    OVERLAY_LIMIT_8Ks,

    /*! Allocate 12K entries for overlay. */
    OVERLAY_LIMIT_12Ks,

    /*! Allocate 16K entries for overlay. */
    OVERLAY_LIMIT_16Ks,

    /*! Allocate 20K entries for overlay. */
    OVERLAY_LIMIT_20Ks,

    /*! Allocate 24K entries for overlay. */
    OVERLAY_LIMIT_24Ks,

    /*! Allocate 28K entries for overlay. */
    OVERLAY_LIMIT_28Ks,

    "INVALID"
};

static const char *l3_eif_overlay_limit_str[] = {
    /*! Allocate no entries for overlay. */
    OVERLAY_LIMIT_0s,

    /*! Allocate 1K entries for overlay. */
    OVERLAY_LIMIT_1Ks,

    /*! Allocate 2K entries for overlay. */
    OVERLAY_LIMIT_2Ks,

    /*! Allocate 3K entries for overlay. */
    OVERLAY_LIMIT_3Ks,

    /*! Allocate 4K entries for overlay. */
    OVERLAY_LIMIT_4Ks,

    /*! Allocate 5K entries for overlay. */
    OVERLAY_LIMIT_5Ks,

    /*! Allocate 6K entries for overlay. */
    OVERLAY_LIMIT_6Ks,

    /*! Allocate 7K entries for overlay. */
    OVERLAY_LIMIT_7Ks,

    "INVALID"
};

static const char *ecmp_member_overlay_limit_str[] = {
    /*! Allocate no entries for overlay. */
    OVERLAY_LIMIT_0s,

    /*! Allocate 4K entries for overlay. */
    OVERLAY_LIMIT_4Ks,

    /*! Allocate 8K entries for overlay. */
    OVERLAY_LIMIT_8Ks,

    /*! Allocate 12K entries for overlay. */
    OVERLAY_LIMIT_12Ks,
};

static const char *vlan_tag_present_action_str[] = {
    NO_ACTIONs,
    REPLACE_VLAN_TPIDs,
    REPLACE_VLANs,
    DELETEs,
    REPLACE_VLAN_PRI_TPIDs,
    REPLACE_VLAN_PRIs,
    REPLACE_PRIs,
    REPLACE_TPIDs,
    "INVALID"
};

static const char *vlan_tag_not_present_action_str[] = {
    NO_ACTIONs,
    ADD_VLAN_TPIDs,
    "INVALID"
};

static const char *opaque_tag_present_action_str[] = {
    NO_ACTIONs,
    DELETEs,
    REPLACEs,
    "INVALID"
};

static const char *opaque_tag_not_present_action_str[] = {
    NO_ACTIONs,
    ADDs,
    "INVALID"
};

#define L3_NHOP_BANK_SIZE 4096
#define L3_EIF_BANK_SIZE 1024
#define ECMP_MEMBER_BANK_SIZE 4096

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Get vlan_tag_present_action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
vlan_tag_present_action_symbol_to_scalar(
    int unit,
    const char *symbol,
    uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_CNT; i++) {
        if (!sal_strcmp(symbol, vlan_tag_present_action_str[i])) {
            break;
        }
    }

    if (i >= BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get vlan_tag_present_action sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
vlan_tag_present_action_scalar_to_symbol(
    int unit,
    uint64_t value,
    const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= BCMI_LTSW_VLAN_TAG_PRESENT_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = vlan_tag_present_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get vlan_not_tag_present_action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
vlan_tag_not_present_action_symbol_to_scalar(
    int unit,
    const char *symbol,
    uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMI_LTSW_VLAN_TAG_NOT_PRESENT_ACTION_CNT; i++) {
        if (!sal_strcmp(symbol, vlan_tag_not_present_action_str[i])) {
            break;
        }
    }

    if (i >= BCMI_LTSW_VLAN_TAG_NOT_PRESENT_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get vlan_not_tag_present_action sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
vlan_tag_not_present_action_scalar_to_symbol(
    int unit,
    uint64_t value,
    const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= BCMI_LTSW_VLAN_TAG_NOT_PRESENT_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = vlan_tag_not_present_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get opaque_tag_present_action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
opaque_tag_present_action_symbol_to_scalar(
    int unit,
    const char *symbol,
    uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMI_LTSW_OPAQUE_TAG_PRESENT_ACTION_CNT; i++) {
        if (!sal_strcmp(symbol, opaque_tag_present_action_str[i])) {
            break;
        }
    }

    if (i >= BCMI_LTSW_OPAQUE_TAG_PRESENT_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get opaque_tag_present_action sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
opaque_tag_present_action_scalar_to_symbol(
    int unit,
    uint64_t value,
    const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= BCMI_LTSW_OPAQUE_TAG_PRESENT_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = opaque_tag_present_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get opaque_not_tag_present_action from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value label action.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
opaque_tag_not_present_action_symbol_to_scalar(
    int unit,
    const char *symbol,
    uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< BCMI_LTSW_OPAQUE_TAG_NOT_PRESENT_ACTION_CNT; i++) {
        if (!sal_strcmp(symbol, opaque_tag_not_present_action_str[i])) {
            break;
        }
    }

    if (i >= BCMI_LTSW_OPAQUE_TAG_NOT_PRESENT_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get opaque_not_tag_present_action sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
opaque_tag_not_present_action_scalar_to_symbol(
    int unit,
    uint64_t value,
    const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= BCMI_LTSW_OPAQUE_TAG_NOT_PRESENT_ACTION_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *symbol = opaque_tag_not_present_action_str[value];

exit:
    SHR_FUNC_EXIT();
}

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
    [BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID] = {
        L3_EIF_VALIDs,
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
    [BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH] = {
        DO_NOT_CUT_THROUGHs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DVP] = {
        DVPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID] = {
        ING_UNDERLAY_NHOP_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID] = {
        ING_UNDERLAY_NHOP_IDs,
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
    [BCMINT_XGS_L3_EGRESS_TBL_MODPORT] = {
        MODPORTs,
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
    [BCMINT_XGS_L3_EGRESS_TBL_DVP] = {
        DVPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_ID] = {
        OVERLAY_L3_EIF_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_VALID] = {
        OVERLAY_L3_EIF_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_VALID] = {
        UNDERLAY_L3_EIF_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_ID] = {
        UNDERLAY_L3_EIF_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_VALID] = {
        OVERLAY_L3_EIF_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_ID] = {
        OVERLAY_L3_EIF_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_VALID] = {
        EGR_UNDERLAY_NHOP_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_ID] = {
        EGR_UNDERLAY_NHOP_IDs,
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
    [BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID] = {
        L3_EIF_VALIDs,
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
    [BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH] = {
        DO_NOT_CUT_THROUGHs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DVP] = {
        DVPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID] = {
        ING_UNDERLAY_NHOP_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID] = {
        ING_UNDERLAY_NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_VALID] = {
        EGR_UNDERLAY_NHOP_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_ID] = {
        EGR_UNDERLAY_NHOP_IDs,
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
    [BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID] = {
        L3_EIF_VALIDs,
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
    [BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH] = {
        DO_NOT_CUT_THROUGHs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DVP] = {
        DVPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID] = {
        ING_UNDERLAY_NHOP_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID] = {
        ING_UNDERLAY_NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_VALID] = {
        EGR_UNDERLAY_NHOP_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_ID] = {
        EGR_UNDERLAY_NHOP_IDs,
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
    [BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID] = {
        L3_EIF_VALIDs,
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
    [BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH] = {
        DO_NOT_CUT_THROUGHs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DVP] = {
        DVPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_VALID] = {
        UNDERLAY_L3_EIF_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_ID] = {
        UNDERLAY_L3_EIF_IDs,
        false,
        false,
        NULL,
        NULL
    },
};

static const
bcmint_ltsw_l3_egress_fld_t l3_egress_l2_tag_flds[BCMINT_XGS_L3_EGRESS_TBL_FLD_CNT] = {
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
    [BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID] = {
        L3_EIF_VALIDs,
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
        CTR_EGR_EFLEX_ACTIONs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH] = {
        DO_NOT_CUT_THROUGHs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_DVP] = {
        DVPs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID] = {
        ING_UNDERLAY_NHOP_VALIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID] = {
        ING_UNDERLAY_NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_VLAN_TAG_PRESENT_ACTION] = {
        VLAN_TAG_PRESENT_ACTIONs,
        false,
        true,
        vlan_tag_present_action_symbol_to_scalar,
        vlan_tag_present_action_scalar_to_symbol
    },
    [BCMINT_XGS_L3_EGRESS_TBL_VLAN_TAG_NOT_PRESENT_ACTION] = {
        VLAN_TAG_NOT_PRESENT_ACTIONs,
        false,
        true,
        vlan_tag_not_present_action_symbol_to_scalar,
        vlan_tag_not_present_action_scalar_to_symbol
    },
    [BCMINT_XGS_L3_EGRESS_TBL_L2_OPAQUE_TAG_PRESENT_ACTION] = {
        L2_OPAQUE_TAG_PRESENT_ACTIONs,
        false,
        true,
        opaque_tag_present_action_symbol_to_scalar,
        opaque_tag_present_action_scalar_to_symbol
    },
    [BCMINT_XGS_L3_EGRESS_TBL_L2_OPAQUE_TAG_NOT_PRESENT_ACTION] = {
        L2_OPAQUE_TAG_NOT_PRESENT_ACTIONs,
        false,
        true,
        opaque_tag_not_present_action_symbol_to_scalar,
        opaque_tag_not_present_action_scalar_to_symbol
    },
    [BCMINT_XGS_L3_EGRESS_TBL_IS_PHB_MAP_ID] = {
        IS_PHB_MAP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_L2_INT_PRI_TO_OTAG_ID] = {
        PHB_EGR_L2_INT_PRI_TO_OTAG_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_OCFI] = {
        OCFIs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_OPRI] = {
        OPRIs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_REMARK_CFI] = {
        REMARK_CFIs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_OVID] = {
        OVIDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_VLAN_OUTER_TPID_ID] = {
        VLAN_OUTER_TPID_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_EGRESS_TBL_ES_IDENTIFIER] = {
        ES_IDENTIFIERs,
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
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DVP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID),
        .flds = l3_egress_uc_flds
    },
    [BCMINT_LTSW_L3_EGRESS_TBL_MC] = {
        .name = L3_MC_NHOPs,
        .fld_cnt = L3_MC_NHOPt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_REPLACE_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DROP_L3) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DVP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_OVERLAY_L3_EIF_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_ID),
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
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID) |
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
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DVP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_ID),
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
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID) |
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
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DVP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_EGR_UNDERLAY_NHOP_ID),
        .flds = l3_egress_mpls_encap_flds
    },
    [BCMINT_LTSW_L3_EGRESS_TBL_IFP_ACTION] = {
        .name = FP_ING_ACTION_NHOPs,
        .fld_cnt = FP_ING_ACTION_NHOPt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_DST_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_TTL) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_VLAN_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_KEEP_SRC_MAC) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DVP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_UNDERLAY_L3_EIF_ID),
        .flds = l3_egress_ifp_action_flds
    },
    [BCMINT_LTSW_L3_EGRESS_TBL_L2_TAG] = {
        .name = L2_TAG_ACTION_NHOPs,
        .fld_cnt = L2_TAG_ACTION_NHOPt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_EGRESS_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_VLAN_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_MODPORT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_TRUNK_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_IS_TRUNK) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_COPY_TO_CPU) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DROP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L3_EIF_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CLASS_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_CTR_EGR_EFLEX_ACTION_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DO_NOT_CUT_THROUGH) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_DVP) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_VALID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ING_UNDERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_VLAN_TAG_PRESENT_ACTION) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_VLAN_TAG_NOT_PRESENT_ACTION) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L2_OPAQUE_TAG_PRESENT_ACTION) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_L2_OPAQUE_TAG_NOT_PRESENT_ACTION) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_IS_PHB_MAP_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_PHB_EGR_L2_INT_PRI_TO_OTAG_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_OCFI) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_OPRI) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_REMARK_CFI) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_OVID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_VLAN_OUTER_TPID_ID) |
                   (1LL << BCMINT_XGS_L3_EGRESS_TBL_ES_IDENTIFIER),
        .flds = l3_egress_l2_tag_flds
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
bcmint_ltsw_l3_egress_fld_t l3_ecmp_ul_flds[BCMINT_XGS_L3_ECMP_TBL_FLD_CNT] = {
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
bcmint_ltsw_l3_egress_fld_t l3_ecmp_weighted_ul_flds[BCMINT_XGS_L3_ECMP_TBL_FLD_CNT] = {
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

static const
bcmint_ltsw_l3_egress_fld_t l3_ecmp_ol_flds[BCMINT_XGS_L3_ECMP_TBL_FLD_CNT] = {
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
    [BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID] = {
        ECMP_UNDERLAY_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID] = {
        RH_ECMP_UNDERLAY_IDs,
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
    [BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY] = {
        ECMP_NHOP_UNDERLAYs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID] = {
        OVERLAY_NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID] = {
        UNDERLAY_NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY] = {
        RH_ECMP_NHOP_UNDERLAYs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID] = {
        RH_OVERLAY_NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID] = {
        RH_UNDERLAY_NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT] = {
        CTR_EGR_EFLEX_OBJECTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID] = {
        ECMP_OVERLAY_CTR_ING_EFLEX_ACTION_IDs,
        false,
        false,
        NULL,
        NULL
    },
};

static const
bcmint_ltsw_l3_egress_fld_t l3_ecmp_weighted_ol_flds[BCMINT_XGS_L3_ECMP_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_ECMP_TBL_ECMP_ID] = {
        ECMP_IDs,
        true,
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
    [BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID] = {
        OVERLAY_NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY] = {
        ECMP_NHOP_UNDERLAYs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID] = {
        UNDERLAY_NHOP_IDs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID] = {
        ECMP_UNDERLAY_IDs,
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
    [BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        false,
        false,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID] = {
        ECMP_OVERLAY_CTR_ING_EFLEX_ACTION_IDs,
        false,
        false,
        NULL,
        NULL
    },
};

static const bcmint_ltsw_l3_egress_tbl_t l3_ecmp_tbls[BCMINT_LTSW_L3_ECMP_TBL_CNT] = {
    [BCMINT_LTSW_L3_ECMP_TBL_UL] = {
        .name = ECMP_UNDERLAYs,
        .fld_cnt = ECMP_UNDERLAYt_FIELD_COUNT,
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
        .flds = l3_ecmp_ul_flds
    },
    [BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_UL] = {
        .name = ECMP_WEIGHTED_UNDERLAYs,
        .fld_cnt = ECMP_WEIGHTED_UNDERLAYt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_SIZE) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHTED_MODE) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID),
        .flds = l3_ecmp_weighted_ul_flds
    },
    [BCMINT_LTSW_L3_ECMP_TBL_OL] = {
        .name = ECMP_OVERLAYs,
        .fld_cnt = ECMP_OVERLAYt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_LB_MODE) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_MAX_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_UNDERLAY_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_ECMP_NHOP_UNDERLAY) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_OVERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_UNDERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_SIZE) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_RH_RANDOM_SEED) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID),
        .flds = l3_ecmp_ol_flds
    },
    [BCMINT_LTSW_L3_ECMP_TBL_WEIGHTED_OL] = {
        .name = ECMP_WEIGHTED_OVERLAYs,
        .fld_cnt = ECMP_WEIGHTED_OVERLAYt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_NUM_PATHS) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_OVERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_NHOP_UNDERLAY) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_ECMP_UNDERLAY_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_UNDERLAY_NHOP_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_WEIGHT) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_OBJECT) |
                   (1LL << BCMINT_XGS_L3_ECMP_TBL_CTR_EGR_EFLEX_ACTION_ID),
        .flds = l3_ecmp_weighted_ol_flds
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

bcmint_ltsw_l3_egress_fld_t l3_overlay_ecmp_flexctr_action_flds[BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID] = {
        ECMP_OVERLAY_CTR_ING_EFLEX_ACTION_IDs,
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

static const bcmint_ltsw_l3_egress_tbl_t l3_ecmp_flexctr_action_tbls[BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_CNT] = {
    [BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_UL] = {
        .name = ECMP_CTR_ING_EFLEX_ACTIONs,
        .fld_cnt = ECMP_CTR_ING_EFLEX_ACTIONt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ACTION),
        .flds = l3_ecmp_flexctr_action_flds
    },
    [BCMINT_LTSW_L3_ECMP_FLEXCTR_ACT_TBL_OL] = {
        .name = ECMP_OVERLAY_CTR_ING_EFLEX_ACTIONs,
        .fld_cnt = ECMP_OVERLAY_CTR_ING_EFLEX_ACTIONt_FIELD_COUNT,
        .fld_bmp = (1LL << BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ID) |
                   (1LL << BCMINT_XGS_L3_ECMP_FLEXCTR_ACT_TBL_ACTION),
        .flds = l3_overlay_ecmp_flexctr_action_flds
    },
};

int
bcm5699x_ltsw_l3_egress_tbl_db_get(int unit,
                                   bcmint_ltsw_l3_egress_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    tbl_db->tbl_bmp = (1 << BCMINT_LTSW_L3_EGRESS_TBL_UC) |
                      (1 << BCMINT_LTSW_L3_EGRESS_TBL_MC) |
                      (1 << BCMINT_LTSW_L3_EGRESS_TBL_MPLS_TRNSIT) |
                      (1 << BCMINT_LTSW_L3_EGRESS_TBL_MPLS_ENCAP) |
                      (1 << BCMINT_LTSW_L3_EGRESS_TBL_IFP_ACTION) |
                      (1 << BCMINT_LTSW_L3_EGRESS_TBL_L2_TAG);

    tbl_db->tbls = l3_egress_tbls;

exit:
    SHR_FUNC_EXIT();
}

int
bcm5699x_ltsw_l3_ecmp_tbl_db_get(int unit,
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
bcm5699x_ltsw_l3_ecmp_flexctr_action_tbl_db_get(int unit,
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

int
bcm5699x_ltsw_l3_overlay_table_init(int unit,
                                    bcmint_ltsw_l3_ol_tbl_info_t *info)
{
    int dunit;
    int i;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    if ((info->ol_egr_obj_num >= 0) || (info->ol_eif_num >= 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_LIMIT_CONTROLs, &eh));

        if (info->ol_egr_obj_num >= 0) {
            i = info->ol_egr_obj_num / L3_NHOP_BANK_SIZE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, NHOP_OVERLAY_LIMITs,
                                              l3_nhop_overlay_limit_str[i]));
        }

        if (info->ol_eif_num >= 0) {
            i = info->ol_eif_num / L3_EIF_BANK_SIZE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, EIF_OVERLAY_LIMITs,
                                              l3_eif_overlay_limit_str[i]));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    if (info->ol_ecmp_member_num >= 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, ECMP_LIMIT_CONTROLs, &eh));

        i = info->ol_ecmp_member_num / ECMP_MEMBER_BANK_SIZE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, MEMBER_OVERLAY_LIMITs,
                                          ecmp_member_overlay_limit_str[i]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_egress sub driver functions for bcm5699x.
 */
static mbcm_ltsw_l3_egress_drv_t bcm5699x_l3_egress_sub_drv = {
    .l3_egress_tbl_db_get = bcm5699x_ltsw_l3_egress_tbl_db_get,
    .l3_ecmp_tbl_db_get = bcm5699x_ltsw_l3_ecmp_tbl_db_get,
    .l3_ecmp_flexctr_action_tbl_db_get = bcm5699x_ltsw_l3_ecmp_flexctr_action_tbl_db_get,
    .l3_overlay_table_init = bcm5699x_ltsw_l3_overlay_table_init,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_l3_egress_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_drv_set(unit, &bcm5699x_l3_egress_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
