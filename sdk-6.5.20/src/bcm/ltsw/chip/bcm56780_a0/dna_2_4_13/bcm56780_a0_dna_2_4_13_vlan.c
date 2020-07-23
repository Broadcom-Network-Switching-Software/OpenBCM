/*! \file bcm56780_a0_dna_2_4_13_vlan.c
 *
 * BCM56780_A0 DNA_2_4_13 VLAN driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/vlan_int.h>
#include <bcm_int/ltsw/mbcm/vlan.h>
#include <bcm_int/ltsw/xfs/types.h>

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_str.h>
/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_VLAN

static const char *ev_view[] = {
    VT_VFI_DEFAULTs,
    VT_VFI_VSIDs
};

static const char *vlan_miss_action[] = {
    FLOODs,
    DROPs
};

static const char *pvlan_port_type[] = {
    PROMISCUOUSs,
    COMMUNITYs,
    ISOLATEDs
};

static const char *dest_type_str[] = BCMI_XFS_DEST_TYPE_STR;

/*!
 * \brief Get dest type from sybmol.
 *
 * \param [in]   unit Unit number.
 * \param [in]   symbol String.
 * \param [out]  value dest_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
dest_type_symbol_to_scalar(int unit, const char *symbol, uint64 *value)
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
 * \brief Get dest_type from value.
 *
 * \param [in]   unit Unit number.
 * \param [in]   value dest_type.
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
 * \brief Get pvlan_port_type from sybmol.
 *
 * \param [in]   unit Unit number.
 * \param [in]   symbol String.
 * \param [out] value dest_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pvlan_port_type_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< PVLAN_PORT_TYPE_CNT; i++) {
        if (!sal_strcmp(symbol, pvlan_port_type[i])) {
            break;
        }
    }

    if (i >= PVLAN_PORT_TYPE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get pvlan_port_type from value.
 *
 * \param [in]   unit Unit number.
 * \param [in]   value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pvlan_port_type_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= PVLAN_PORT_TYPE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }
    *symbol = pvlan_port_type[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egr vfi table view from sybmol.
 *
 * \param [in]   unit Unit number.
 * \param [in]   symbol String.
 * \param [out] value dest_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
egr_vfi_view_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< EGR_VFI_VIEW_CNT; i++) {
        if (!sal_strcmp(symbol, ev_view[i])) {
            break;
        }
    }

    if (i >= EGR_VFI_VIEW_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egr vfi table view from value.
 *
 * \param [in]   unit Unit number.
 * \param [in]   value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
egr_vfi_view_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= EGR_VFI_VIEW_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }
    *symbol = ev_view[value];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get l2_miss_action from sybmol.
 *
 * \param [in]   unit Unit number.
 * \param [in]   symbol String.
 * \param [out] value dest_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l2_miss_action_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i< VLAN_MISS_ACTION_TYPE_CNT; i++) {
        if (!sal_strcmp(symbol, vlan_miss_action[i])) {
            break;
        }
    }

    if (i >= VLAN_MISS_ACTION_TYPE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get l2_miss_action from value.
 *
 * \param [in]   unit Unit number.
 * \param [in]   value dest_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l2_miss_action_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (value >= VLAN_MISS_ACTION_TYPE_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }
    *symbol = vlan_miss_action[value];

exit:
    SHR_FUNC_EXIT();
}


/* ING_VLAN_XLATE_X_TABLE fields. */
static const bcmint_vlan_fld_t
ing_xlate_entry_flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_FLD_CNT] = {
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_OVID] = {
        OVIDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_IVID] = {
        IVIDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID] = {
        L2_IIF_PORT_GROUP_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_STRENGTH_PROFILE_INDEX] = {
        STRENGTH_PROFILE_INDEXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF_STRENGTH] = {
        L3_IIF_STRENGTHs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI_STRENGTH] = {
        VFI_STRENGTHs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF] = {
        L2_IIFs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF] = {
        L3_IIFs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_SVP] = {
        SVPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI] = {
        VFIs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_VLAN_TAG_PRESERVE_CTRL] = {
        VLAN_TAG_PRESERVE_CTRLs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID] = {
        OPAQUE_CTRL_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID_1] = {
        OPAQUE_CTRL_ID_1s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_XLATE_PIPE] = {
        PIPEs,
        NULL,
        NULL
    }
};

/* EGR_VLAN_TRANSLATION fields. */
static const bcmint_vlan_fld_t
egr_xlate_entry_flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLD_CNT] = {
    [BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VFI] = {
        VFIs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_XLATE_GROUP_ID] = {
        GROUP_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VLAN_0] = {
        VLAN_0s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VLAN_1] = {
        VLAN_1s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_XLATE_TAG_ACTION] = {
        TAG_ACTIONs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_XLATE_DOT1P_REMARK_BASE_PTR] = {
        DOT1P_REMARK_BASE_PTRs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_XLATE_STRENGTH_PRFL_IDX] = {
        STRENGTH_PRFL_IDXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_XLATE_OPAQ_CTRL] = {
        VXLT_CTRLs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_XLATE_PIPE] = {
        PIPEs,
        NULL,
        NULL
    }
};

/* ING_VFI_TABLE fields. */
static const bcmint_vlan_fld_t
ing_vfi_entry_flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_FLD_CNT] = {
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_VFI] = {
        VFIs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_MAPPED_VFI] = {
        MAPPED_VFIs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_DESTINATION] = {
        DESTINATIONs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_DESTINATION_TYPE] = {
        DESTINATION_TYPEs,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_L3_IIF] = {
        L3_IIFs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_STRENGTH_PROFILE_INDEX] = {
        STRENGTH_PROFILE_INDEXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_SPANNING_TREE_GROUP] = {
        SPANNING_TREE_GROUPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_MEMBERSHIP_PROFILE_PTR] = {
        MEMBERSHIP_PROFILE_PTRs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_BITMAP_PROFILE_INDEX] = {
        BITMAP_PROFILE_INDEXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_EXPECTED_OUTER_TPID_BITMAP] = {
        EXPECTED_OUTER_TPID_BITMAPs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_OPAQUE_CTRL_ID] = {
        OPAQUE_CTRL_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_SRC_PVLAN_PORT_TYPE] = {
        SRC_PVLAN_PORT_TYPEs,
        pvlan_port_type_symbol_to_scalar,
        pvlan_port_type_scalar_to_symbol
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_IPV4MC_L2_ENABLE] = {
        IPV4MC_L2_ENABLEs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_IPV6MC_L2_ENABLE] = {
        IPV6MC_L2_ENABLEs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_L2MC_MISS_ACTION] = {
        L2MC_MISS_ACTIONs,
        l2_miss_action_symbol_to_scalar,
        l2_miss_action_scalar_to_symbol
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_L2UC_MISS_ACTION] = {
        L2UC_MISS_ACTIONs,
        l2_miss_action_symbol_to_scalar,
        l2_miss_action_scalar_to_symbol
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_DO_NOT_LEARN] = {
        DO_NOT_LEARNs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_ING_VFI_L2_HOST_NARROW_LOOKUP_ENABLE] = {
        L2_HOST_NARROW_LOOKUP_ENABLEs,
        NULL,
        NULL
    },
};

/* EGR_VFI fields. */
static const bcmint_vlan_fld_t
egr_vfi_entry_flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_FLD_CNT] = {
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_VFI] = {
        VFIs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_VLAN_0] = {
        VLAN_0s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_VLAN_1] = {
        VLAN_1s,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_TAG_ACTION] = {
        TAG_ACTIONs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_UNTAG_PROFILE_INDEX] = {
        UNTAG_PROFILE_INDEXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_STG] = {
        STGs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_MEMBERSHIP_PROFILE_IDX] = {
        MEMBERSHIP_PROFILE_IDXs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_EFP_CTRL_ID] = {
        EFP_CTRL_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_CLASS_ID] = {
        CLASS_IDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_VIEW_T] = {
        VIEW_Ts,
        egr_vfi_view_symbol_to_scalar,
        egr_vfi_view_scalar_to_symbol
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_VSID] = {
        VSIDs,
        NULL,
        NULL
    },
    [BCMINT_LTSW_VLAN_FLD_EGR_VFI_STRENGTH_PRFL_IDX] = {
        STRENGTH_PRFL_IDXs,
        NULL,
        NULL
    },
};

/* R_VLAN_XLATE_MISS_POLICY fields. */
static const bcmint_vlan_fld_t
vlan_vt_miss_policy_flds[BCMINT_LTSW_VLAN_FLD_VT_MISS_POLICY_FLD_CNT] = {
    [BCMINT_LTSW_VLAN_FLD_VT_MISS_POLICY_VT_CTRL] = {
        OPAQUE_CTRL_IDs,
        NULL,
        NULL
    }
};

/* R_EGR_VLAN_XLATE_MISS_POLICY fields. */
static const bcmint_vlan_fld_t
vlan_egr_vt_miss_policy_flds[BCMINT_LTSW_VLAN_FLD_EGR_VT_MISS_POLICY_FLD_CNT] = {
    [BCMINT_LTSW_VLAN_FLD_EGR_VT_MISS_POLICY_VT_CTRL] = {
        VXLT_CTRLs,
        NULL,
        NULL
    }
};

/* VLAN_XLATE_TABLE_PIPE_CONFIG fields. */
static const bcmint_vlan_fld_t
vlan_xlate_pipe_config_flds[BCMINT_LTSW_VLAN_FLD_VLAN_XLATE_TABLE_PIPE_CONFIG_CNT] = {
    [BCMINT_LTSW_VLAN_FLD_VLAN_XLATE_TABLE_PIPE_CONFIG_PIPEUNIQUE] = {
        PIPEUNIQUEs,
        NULL,
        NULL
    }
};

static const bcmint_vlan_lt_t
bcm56780_a0_dna_2_4_13_vlan_lt[] = {
    [BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE] = {
        .name = ING_VLAN_XLATE_1_TABLEs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OVID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_STRENGTH_PROFILE_INDEX) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF_STRENGTH) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI_STRENGTH) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_SVP) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_VLAN_TAG_PRESERVE_CTRL) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID_1) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_PIPE),
        .flds = ing_xlate_entry_flds
    },
    [BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE] = {
        .name = ING_VLAN_XLATE_2_TABLEs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_IVID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_STRENGTH_PROFILE_INDEX) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF_STRENGTH) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI_STRENGTH) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_SVP) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_VLAN_TAG_PRESERVE_CTRL) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID_1) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_PIPE),
        .flds = ing_xlate_entry_flds
    },
    [BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE] = {
        .name = ING_VLAN_XLATE_3_TABLEs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OVID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_IVID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_STRENGTH_PROFILE_INDEX) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF_STRENGTH) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI_STRENGTH) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_SVP) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_VLAN_TAG_PRESERVE_CTRL) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID_1) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_PIPE),
        .flds = ing_xlate_entry_flds
    },

    [BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION] = {
        .name = EGR_VLAN_TRANSLATIONs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VFI) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_GROUP_ID) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VLAN_0) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VLAN_1) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_TAG_ACTION) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_DOT1P_REMARK_BASE_PTR) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_STRENGTH_PRFL_IDX) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_PIPE) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_OPAQ_CTRL),
        .flds = egr_xlate_entry_flds
    },

    [BCMINT_LTSW_VLAN_ING_VFI_TABLE] = {
        .name = ING_VFI_TABLEs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_VFI) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_MAPPED_VFI) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_DESTINATION) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_DESTINATION_TYPE) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_L3_IIF) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_FLEX_CTR_ACTION) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_STRENGTH_PROFILE_INDEX) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_SPANNING_TREE_GROUP) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_MEMBERSHIP_PROFILE_PTR) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_BITMAP_PROFILE_INDEX) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_EXPECTED_OUTER_TPID_BITMAP) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_OPAQUE_CTRL_ID) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_SRC_PVLAN_PORT_TYPE) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_IPV4MC_L2_ENABLE) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_IPV6MC_L2_ENABLE) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_L2MC_MISS_ACTION) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_L2UC_MISS_ACTION) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_DO_NOT_LEARN) |
            (1 << BCMINT_LTSW_VLAN_FLD_ING_VFI_L2_HOST_NARROW_LOOKUP_ENABLE),
        .flds = ing_vfi_entry_flds
    },

    [BCMINT_LTSW_VLAN_EGR_VFI] = {
        .name = EGR_VFIs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_VFI) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_VLAN_0) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_VLAN_1) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_TAG_ACTION) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_UNTAG_PROFILE_INDEX) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_STG) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_MEMBERSHIP_PROFILE_IDX) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_FLEX_CTR_ACTION) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_EFP_CTRL_ID) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_CLASS_ID) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_VIEW_T) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_VSID) |
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VFI_STRENGTH_PRFL_IDX),
        .flds = egr_vfi_entry_flds
    },
    [BCMINT_LTSW_VLAN_R_VLAN_XLATE_MISS_POLICY] = {
        .name = R_VLAN_XLATE_MISS_POLICYs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_VT_MISS_POLICY_VT_CTRL),
        .flds = vlan_vt_miss_policy_flds
    },
    [BCMINT_LTSW_VLAN_R_EGR_VLAN_XLATE_MISS_POLICY] = {
        .name = R_EGR_VLAN_XLATE_MISS_POLICYs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_EGR_VT_MISS_POLICY_VT_CTRL),
        .flds = vlan_egr_vt_miss_policy_flds
    },
    [BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE_PIPE_CONFIG] = {
        .name = ING_VLAN_XLATE_1_TABLE_PIPE_CONFIGs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_VLAN_XLATE_TABLE_PIPE_CONFIG_PIPEUNIQUE),
        .flds = vlan_xlate_pipe_config_flds
    },
    [BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE_PIPE_CONFIG] = {
        .name = ING_VLAN_XLATE_2_TABLE_PIPE_CONFIGs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_VLAN_XLATE_TABLE_PIPE_CONFIG_PIPEUNIQUE),
        .flds = vlan_xlate_pipe_config_flds
    },
    [BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE_PIPE_CONFIG] = {
        .name = ING_VLAN_XLATE_3_TABLE_PIPE_CONFIGs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_VLAN_XLATE_TABLE_PIPE_CONFIG_PIPEUNIQUE),
        .flds = vlan_xlate_pipe_config_flds
    },
    [BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_PIPE_CONFIG] = {
        .name = EGR_VLAN_TRANSLATION_PIPE_CONFIGs,
        .fld_bmp =
            (1 << BCMINT_LTSW_VLAN_FLD_VLAN_XLATE_TABLE_PIPE_CONFIG_PIPEUNIQUE),
        .flds = vlan_xlate_pipe_config_flds
    },
};

/******************************************************************************
 * Private functions
 */

static int
bcm56780_a0_dna_2_4_13_vlan_lt_db_get(int unit,
                                     bcmint_vlan_lt_db_t *lt_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_db, SHR_E_PARAM);

    /* Bitmap of LTs that valid on bcm56780 DNA_2_4_13. */
    lt_db->lt_bmp = (1 << BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE)  |
                    (1 << BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE)  |
                    (1 << BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE)  |
                    (1 << BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION)    |
                    (1 << BCMINT_LTSW_VLAN_ING_VFI_TABLE)           |
                    (1 << BCMINT_LTSW_VLAN_EGR_VFI)                 |
                    (1 << BCMINT_LTSW_VLAN_R_VLAN_XLATE_MISS_POLICY)           |
                    (1 << BCMINT_LTSW_VLAN_R_EGR_VLAN_XLATE_MISS_POLICY)       |
                    (1 << BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE_PIPE_CONFIG) |
                    (1 << BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE_PIPE_CONFIG) |
                    (1 << BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE_PIPE_CONFIG) |
                    (1 << BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_PIPE_CONFIG);
    lt_db->lts = bcm56780_a0_dna_2_4_13_vlan_lt;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_dna_2_4_13_vlan_xlate_table_share(int unit,
                                             vlan_xlate_table_id_t lt_id,
                                             int *share)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(share, SHR_E_PARAM);
    *share = 0;

    /* xlate_2 and xlate_3 tables share same pt resources. */
    switch (lt_id) {
        case XLATE_TABLE_ING_2:
        case XLATE_TABLE_ING_3:
            *share = 1;
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_vlan_drv_t bcm56780_a0_dna_2_4_13_ltsw_vlan_drv = {
    /* VLAN LT database */
    .vlan_lt_db_get = bcm56780_a0_dna_2_4_13_vlan_lt_db_get,

    /* VLAN xlate table share same pt resource. */
    .vlan_xlate_table_share = bcm56780_a0_dna_2_4_13_vlan_xlate_table_share
};

/******************************************************************************
 * Public functions
 */
int
bcm56780_a0_dna_2_4_13_ltsw_vlan_drv_attach(int unit)
{
    return mbcm_ltsw_vlan_drv_set(unit,
                                  &bcm56780_a0_dna_2_4_13_ltsw_vlan_drv);
}

