/*! \file bcm56880_a0_dna_4_6_6_l3_fib.c
 *
 * BCM56880_A0 DNA_4_6_6 L3 FIB driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/mbcm/l3_fib.h>
#include <bcm_int/ltsw/xfs/l3_fib.h>
#include <bcm_int/ltsw/xfs/types.h>
#include <bcm_int/ltsw/uft.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

static const char *dest_type_str[] = BCMI_XFS_DEST_TYPE_STR;

static const char *key_type_str[BCMINT_LTSW_L3_ALPM_USAGE_KT_CNT] = {
    KEY_L3_IPV4_UCs,
    KEY_L3_IPV4_UC_VRFs,
    KEY_L3_IPV4_UC_OVERRIDEs,
    KEY_L3_IPV6_UC_SINGLEs,
    KEY_L3_IPV6_UC_SINGLE_VRFs,
    KEY_L3_IPV6_UC_SINGLE_OVERRIDEs,
    KEY_L3_IPV6_UC_DOUBLEs,
    KEY_L3_IPV6_UC_DOUBLE_VRFs,
    KEY_L3_IPV6_UC_DOUBLE_OVERRIDEs,
    KEY_L3_IPV6_UC_QUADs,
    KEY_L3_IPV6_UC_QUAD_VRFs,
    KEY_L3_IPV6_UC_QUAD_OVERRIDEs,
    KEY_L3_IPV4_COMPs,
    KEY_L3_IPV6_COMPs,
    NULL,
    NULL
};

static const char *db_str[] = {
    ALPM_DB_0s,
    ALPM_DB_1s,
    ALPM_DB_2s,
    ALPM_DB_3s
};

static const char *key_input_str[BCMINT_LTSW_L3_ALPM_KI_CNT] = {
    ALPM_KEY_INPUT_LPM_DST_QUADs,
    ALPM_KEY_INPUT_LPM_DST_DOUBLEs,
    ALPM_KEY_INPUT_LPM_DST_SINGLEs,
    ALPM_KEY_INPUT_LPM_SRC_QUADs,
    ALPM_KEY_INPUT_LPM_SRC_DOUBLEs,
    ALPM_KEY_INPUT_LPM_SRC_SINGLEs,
    ALPM_KEY_INPUT_FP_COMP_DSTs,
    ALPM_KEY_INPUT_FP_COMP_SRCs,
    NULL,
    NULL,
    NULL
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

    for (i = 0; i< BCMI_XFS_DEST_TYPE_CNT; i++) {
        if (!sal_strcmp(symbol, dest_type_str[i])) {
            break;
        }
    }

    if (i >= BCMI_XFS_DEST_TYPE_CNT) {
        return SHR_E_INTERNAL;
    }

    *value = i;

    return SHR_E_NONE;
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
    if (value >= BCMI_XFS_DEST_TYPE_CNT) {
        return SHR_E_INTERNAL;
    }

    *symbol = dest_type_str[value];

    return SHR_E_NONE;
}

/*!
 * \brief Get data type from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value data_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
data_type_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    if (!sal_strcmp(symbol, WIDE_MODEs)) {
        *value = bcmL3VrfRouteDataModeFull;
    } else if (!sal_strcmp(symbol, NARROW_MODEs)) {
        *value = bcmL3VrfRouteDataModeReduced;
    } else {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Get data type sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value data_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
data_type_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    if (value == bcmL3VrfRouteDataModeFull) {
        *symbol = WIDE_MODEs;
    } else  if (value == bcmL3VrfRouteDataModeReduced) {
        *symbol = NARROW_MODEs;
    } else {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Get key type from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value data_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_type_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    for (i = 0; i < BCMINT_LTSW_L3_ALPM_USAGE_KT_CNT; i++) {
        if (!key_type_str[i]) {
            continue;
        }
        if (!sal_strcmp(symbol, key_type_str[i])) {
            break;
        }
    }

    if (i >= BCMINT_LTSW_L3_ALPM_USAGE_KT_CNT) {
        return SHR_E_INTERNAL;
    }

    *value = i;

    return SHR_E_NONE;
}

/*!
 * \brief Get key type sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value data_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_type_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    if ((value < BCMINT_LTSW_L3_ALPM_USAGE_KT_CNT) && key_type_str[value]) {
        *symbol = key_type_str[value];
    } else {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Get DB from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value data_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    for (i = 0; i < BCMINT_LTSW_L3_ALPM_DB_CNT; i++) {
        if (!db_str[i]) {
            continue;
        }
        if (!sal_strcmp(symbol, db_str[i])) {
            break;
        }
    }

    if (i >= BCMINT_LTSW_L3_ALPM_DB_CNT) {
        return SHR_E_INTERNAL;
    }

    *value = i;

    return SHR_E_NONE;
}

/*!
 * \brief Get DB sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value data_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
db_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    if ((value < BCMINT_LTSW_L3_ALPM_DB_CNT) && db_str[value]) {
        *symbol = db_str[value];
    } else {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Get key input type from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value data_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_input_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    int i;

    for (i = 0; i < BCMINT_LTSW_L3_ALPM_KI_CNT; i++) {
        if (!key_input_str[i]) {
            continue;
        }
        if (!sal_strcmp(symbol, key_input_str[i])) {
            break;
        }
    }

    if (i >= BCMINT_LTSW_L3_ALPM_KI_CNT) {
        return SHR_E_INTERNAL;
    }

    *value = i;

    return SHR_E_NONE;
}

/*!
 * \brief Get key input type sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value data_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
key_input_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    if ((value < BCMINT_LTSW_L3_ALPM_KI_CNT) && key_input_str[value]) {
        *symbol = key_input_str[value];
    } else {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Get operational state from sybmol.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol String.
 * \param [out] value data_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
op_state_symbol_to_scalar(int unit, const char *symbol, uint64_t *value)
{
    if (!sal_strcmp(symbol, VALIDs)) {
        *value = BCMINT_LTSW_L3_ALPM_OP_STATE_VALID;
    } else if (!sal_strcmp(symbol, CONFLICTING_ALPM_BANKSs)) {
        *value = BCMINT_LTSW_L3_ALPM_OP_STATE_CONFILICT;
    } else if (!sal_strcmp(symbol, ALPM_IN_USEs)) {
        *value = BCMINT_LTSW_L3_ALPM_OP_STATE_INUSE;
    } else {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Get operational state sybmol from value.
 *
 * \param [in] unit Unit number.
 * \param [in] value data_type.
 * \param [out] symbol String.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
op_state_scalar_to_symbol(int unit, uint64_t value, const char **symbol)
{
    if (value == BCMINT_LTSW_L3_ALPM_OP_STATE_VALID) {
        *symbol = VALIDs;
    } else if (value == BCMINT_LTSW_L3_ALPM_OP_STATE_CONFILICT) {
        *symbol = CONFLICTING_ALPM_BANKSs;
    } else if (value == BCMINT_LTSW_L3_ALPM_OP_STATE_INUSE) {
        *symbol = ALPM_IN_USEs;
    } else {
        return SHR_E_INTERNAL;
    }

    return SHR_E_NONE;
}

/*
 * L3_IPV4_UNICAST_TABLE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_uc_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4_ADDRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_IPV4_ADDRf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_NAT_IDX] = {
        NAT_INDEXs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_NAT_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1] = {
        NHOP_2_OR_ECMP_GROUP_INDEX_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_NHOP_2_OR_ECMP_GROUP_INDEX_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_NAT_CTRL] = {
        NAT_CTRLs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_NAT_CTRLf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX] = {
        ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_IPV6_UNICAST_TABLE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_uc_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_ADDRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt_IPV6_ADDRf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1] = {
        NHOP_2_OR_ECMP_GROUP_INDEX_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt_NHOP_2_OR_ECMP_GROUP_INDEX_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX] = {
        ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_IPV4_MULTICAST_1_TABLE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_mc_g_flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR] = {
        IP_HDR_DIPs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_IP_HDR_DIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF] = {
        L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_L3_IIFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_IPV4_MULTICAST_2_TABLE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_mc_sg_flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR] = {
        IP_HDR_DIPs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_IP_HDR_DIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR] = {
        IP_HDR_SIPs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_IP_HDR_SIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF] = {
        L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_L3_IIFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_IPV6_MULTICAST_1_TABLE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_mc_g_flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR] = {
        IP_HDR_DIPs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_IP_HDR_DIPf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF] = {
        L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_L3_IIFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_IPV6_MULTICAST_2_TABLE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_mc_sg_flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR] = {
        IP_HDR_DIPs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_IP_HDR_DIPf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR] = {
        IP_HDR_SIPs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_IP_HDR_SIPf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF] = {
        L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_L3_IIFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_IPV4_UC_ROUTE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_uc_gl_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTEt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_IPV4_UC_ROUTE_VRF
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_uc_vrf_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF_MASK] = {
        VRF_ID_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt_VRF_ID_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_IPV4_UC_ROUTE_OVERRIDE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_uc_gh_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_OVERRIDEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_OVERRIDEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_OVERRIDEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_OVERRIDEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_OVERRIDEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_OVERRIDEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_OVERRIDEt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_IPV6_UC_ROUTE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_uc_gl_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_IPV6_UC_ROUTE_VRF
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_uc_vrf_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF_MASK] = {
        VRF_ID_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_VRF_ID_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_IPV6_UC_ROUTE_OVERRIDE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_uc_gh_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_SRC_IPV4_UC_ROUTE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv4_uc_gl_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTEt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_SRC_IPV4_UC_ROUTE_VRF
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv4_uc_vrf_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF_MASK] = {
        VRF_ID_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt_VRF_ID_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_SRC_IPV4_UC_ROUTE_OVERRIDE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv4_uc_gh_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_SRC_IPV6_UC_ROUTE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv6_uc_gl_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_SRC_IPV6_UC_ROUTE_VRF
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv6_uc_vrf_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF_MASK] = {
        VRF_ID_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_VRF_ID_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};

/*
 * L3_SRC_IPV6_UC_ROUTE_OVERRIDE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv6_uc_gh_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_DATA_TYPEf,
        false,
        true,
        0,
        data_type_symbol_to_scalar,
        data_type_scalar_to_symbol
    }
};
/*
 * L3_IPV4_MULTICAST_DEFIP_TABLE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_mc_defip_flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR] = {
        IPV4_ADDRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_IPV4_ADDRf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK] = {
        IPV4_ADDR_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_IPV4_ADDR_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK] = {
        VRF_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_VRF_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY] = {
        ENTRY_PRIORITYs,
        BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_ENTRY_PRIORITYf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_IPV6_MULTICAST_DEFIP_64_TABLE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_mc_defip_64_flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR] = {
        IPV6_ADDR_UPPER_64s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_IPV6_ADDR_UPPER_64f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK] = {
        IPV6_ADDR_UPPER_64_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_IPV6_ADDR_UPPER_64_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK] = {
        VRF_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_VRF_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY] = {
        ENTRY_PRIORITYs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_ENTRY_PRIORITYf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_IPV6_MULTICAST_DEFIP_128_TABLE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_mc_defip_128_flds[BCMINT_LTSW_L3_FIB_TBL_MC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR] = {
        IPV6_ADDRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_IPV6_ADDRf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK] = {
        IPV6_ADDR_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_IPV6_ADDR_MASKf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK] = {
        VRF_MASKs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_VRF_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY] = {
        ENTRY_PRIORITYs,
        BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_ENTRY_PRIORITYf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_ALPM_LEVEL_1_USAGE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_alpm_lvl_1_usage_flds[BCMINT_LTSW_L3_ALPM_USAGE_TBL_FLD_CNT] = {
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_KEY_TYPE] = {
        KEY_TYPEs,
        L3_ALPM_LEVEL_1_USAGEt_KEY_TYPEf,
        true,
        true,
        0,
        key_type_symbol_to_scalar,
        key_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_ENT] = {
        MAX_ENTRIESs,
        L3_ALPM_LEVEL_1_USAGEt_MAX_ENTRIESf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_ENT] = {
        INUSE_ENTRIESs,
        L3_ALPM_LEVEL_1_USAGEt_INUSE_ENTRIESf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_ENT_UTILIZATION] = {
        ENTRY_UTILIZATIONs,
        L3_ALPM_LEVEL_1_USAGEt_ENTRY_UTILIZATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_ALPM_LEVEL_2_USAGE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_alpm_lvl_2_usage_flds[BCMINT_LTSW_L3_ALPM_USAGE_TBL_FLD_CNT] = {
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_DB] = {
        DBs,
        L3_ALPM_LEVEL_2_USAGEt_DBf,
        true,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_ENT] = {
        MAX_ENTRIESs,
        L3_ALPM_LEVEL_2_USAGEt_MAX_ENTRIESf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_ENT] = {
        INUSE_ENTRIESs,
        L3_ALPM_LEVEL_2_USAGEt_INUSE_ENTRIESf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_ENT_UTILIZATION] = {
        ENTRY_UTILIZATIONs,
        L3_ALPM_LEVEL_2_USAGEt_ENTRY_UTILIZATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_RBKT] = {
        MAX_RAW_BUCKETSs,
        L3_ALPM_LEVEL_2_USAGEt_MAX_RAW_BUCKETSf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_RBKT] = {
        INUSE_RAW_BUCKETSs,
        L3_ALPM_LEVEL_2_USAGEt_INUSE_RAW_BUCKETSf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_RBKT_UTILIZATION] = {
        RAW_BUCKET_UTILIZATIONs,
        L3_ALPM_LEVEL_2_USAGEt_RAW_BUCKET_UTILIZATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_ALPM_LEVEL_3_USAGE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_alpm_lvl_3_usage_flds[BCMINT_LTSW_L3_ALPM_USAGE_TBL_FLD_CNT] = {
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_DB] = {
        DBs,
        L3_ALPM_LEVEL_3_USAGEt_DBf,
        true,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_ENT] = {
        MAX_ENTRIESs,
        L3_ALPM_LEVEL_3_USAGEt_MAX_ENTRIESf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_ENT] = {
        INUSE_ENTRIESs,
        L3_ALPM_LEVEL_3_USAGEt_INUSE_ENTRIESf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_ENT_UTILIZATION] = {
        ENTRY_UTILIZATIONs,
        L3_ALPM_LEVEL_3_USAGEt_ENTRY_UTILIZATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_RBKT] = {
        MAX_RAW_BUCKETSs,
        L3_ALPM_LEVEL_3_USAGEt_MAX_RAW_BUCKETSf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_RBKT] = {
        INUSE_RAW_BUCKETSs,
        L3_ALPM_LEVEL_3_USAGEt_INUSE_RAW_BUCKETSf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_USAGE_TBL_RBKT_UTILIZATION] = {
        RAW_BUCKET_UTILIZATIONs,
        L3_ALPM_LEVEL_3_USAGEt_RAW_BUCKET_UTILIZATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/*
 * L3_ALPM_CONTROL
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_alpm_control_flds[BCMINT_LTSW_L3_ALPM_CTRL_TBL_FLD_CNT] = {
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_OP_STATE] = {
        OPERATIONAL_STATEs,
        L3_ALPM_CONTROLt_OPERATIONAL_STATEf,
        false,
        true,
        0,
        op_state_symbol_to_scalar,
        op_state_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_0_LVLS] = {
        NUM_DB_0_LEVELSs,
        L3_ALPM_CONTROLt_NUM_DB_0_LEVELSf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_1_LVLS] = {
        NUM_DB_1_LEVELSs,
        L3_ALPM_CONTROLt_NUM_DB_1_LEVELSf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_2_LVLS] = {
        NUM_DB_2_LEVELSs,
        L3_ALPM_CONTROLt_NUM_DB_2_LEVELSf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_3_LVLS] = {
        NUM_DB_3_LEVELSs,
        L3_ALPM_CONTROLt_NUM_DB_3_LEVELSf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_0] = {
        KEY_INPUT_LEVEL_1_BLOCK_0s,
        L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_0f,
        false,
        true,
        0,
        key_input_symbol_to_scalar,
        key_input_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_1] = {
        KEY_INPUT_LEVEL_1_BLOCK_1s,
        L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_1f,
        false,
        true,
        0,
        key_input_symbol_to_scalar,
        key_input_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_2] = {
        KEY_INPUT_LEVEL_1_BLOCK_2s,
        L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_2f,
        false,
        true,
        0,
        key_input_symbol_to_scalar,
        key_input_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_3] = {
        KEY_INPUT_LEVEL_1_BLOCK_3s,
        L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_3f,
        false,
        true,
        0,
        key_input_symbol_to_scalar,
        key_input_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_0] = {
        DB_LEVEL_1_BLOCK_0s,
        L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_0f,
        false,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_1] = {
        DB_LEVEL_1_BLOCK_1s,
        L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_1f,
        false,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_2] = {
        DB_LEVEL_1_BLOCK_2s,
        L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_2f,
        false,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_3] = {
        DB_LEVEL_1_BLOCK_3s,
        L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_3f,
        false,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST] = {
        DESTINATIONs,
        L3_ALPM_CONTROLt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST_MASK] = {
        DESTINATION_MASKs,
        L3_ALPM_CONTROLt_DESTINATION_MASKf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST_TYPE_MATCH] = {
        DESTINATION_TYPE_MATCHs,
        L3_ALPM_CONTROLt_DESTINATION_TYPE_MATCHf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST_TYPE_NON_MATCH] = {
        DESTINATION_TYPE_NON_MATCHs,
        L3_ALPM_CONTROLt_DESTINATION_TYPE_NON_MATCHf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_UC_GL_STRENGTH_PROF_IDX] = {
        IPV4_UC_STRENGTH_PROFILE_INDEXs,
        L3_ALPM_CONTROLt_IPV4_UC_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_UC_VRF_STRENGTH_PROF_IDX] = {
        IPV4_UC_VRF_STRENGTH_PROFILE_INDEXs,
        L3_ALPM_CONTROLt_IPV4_UC_VRF_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_UC_GH_STRENGTH_PROF_IDX] = {
        IPV4_UC_OVERRIDE_STRENGTH_PROFILE_INDEXs,
        L3_ALPM_CONTROLt_IPV4_UC_OVERRIDE_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_UC_GL_STRENGTH_PROF_IDX] = {
        IPV6_UC_STRENGTH_PROFILE_INDEXs,
        L3_ALPM_CONTROLt_IPV6_UC_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_UC_VRF_STRENGTH_PROF_IDX] = {
        IPV6_UC_VRF_STRENGTH_PROFILE_INDEXs,
        L3_ALPM_CONTROLt_IPV6_UC_VRF_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_UC_GH_STRENGTH_PROF_IDX] = {
        IPV6_UC_OVERRIDE_STRENGTH_PROFILE_INDEXs,
        L3_ALPM_CONTROLt_IPV6_UC_OVERRIDE_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_COMP_STRENGTH_PROF_IDX] = {
        IPV4_COMPRESSION_STRENGTH_PROFILE_INDEXs,
        L3_ALPM_CONTROLt_IPV4_COMPRESSION_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_COMP_STRENGTH_PROF_IDX] = {
        IPV6_COMPRESSION_STRENGTH_PROFILE_INDEXs,
        L3_ALPM_CONTROLt_IPV6_COMPRESSION_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

static const bcmint_ltsw_l3_fib_tbl_t bcm56880_a0_dna_4_6_6_l3_fib_tbls[] = {
    [BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC] = {
        .name = L3_IPV4_UNICAST_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV4_UNICAST_TABLEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1),
        .flds = l3_ipv4_uc_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_UC] = {
        .name = L3_IPV6_UNICAST_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 2,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV6_UNICAST_TABLEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1),
        .flds = l3_ipv6_uc_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_G] = {
        .name = L3_IPV4_MULTICAST_1_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_G |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 2,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_1_TABLEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1),
        .flds = l3_ipv4_mc_g_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_SG] = {
        .name = L3_IPV4_MULTICAST_2_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SG |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 2,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_2_TABLEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1),
        .flds = l3_ipv4_mc_sg_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_G] = {
        .name = L3_IPV6_MULTICAST_1_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_G |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_1_TABLEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1),
        .flds = l3_ipv6_mc_g_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_SG] = {
        .name = L3_IPV6_MULTICAST_2_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SG |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_2_TABLEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1),
        .flds = l3_ipv6_mc_sg_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL] = {
        .name = L3_IPV4_UC_ROUTEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_ipv4_uc_gl_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF] = {
        .name = L3_IPV4_UC_ROUTE_VRFs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_VRFt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_ipv4_uc_vrf_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH] = {
        .name = L3_IPV4_UC_ROUTE_OVERRIDEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV4_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_ipv4_uc_gh_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL] = {
        .name = L3_IPV6_UC_ROUTEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_ipv6_uc_gl_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF] = {
        .name = L3_IPV6_UC_ROUTE_VRFs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_VRFt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_ipv6_uc_vrf_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH] = {
        .name = L3_IPV6_UC_ROUTE_OVERRIDEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV6_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_ipv6_uc_gh_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL] = {
        .name = L3_SRC_IPV4_UC_ROUTEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_src_ipv4_uc_gl_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF] = {
        .name = L3_SRC_IPV4_UC_ROUTE_VRFs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_VRFt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_src_ipv4_uc_vrf_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH] = {
        .name = L3_SRC_IPV4_UC_ROUTE_OVERRIDEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_src_ipv4_uc_gh_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL] = {
        .name = L3_SRC_IPV6_UC_ROUTEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_src_ipv6_uc_gl_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF] = {
        .name = L3_SRC_IPV6_UC_ROUTE_VRFs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_VRFt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_src_ipv6_uc_vrf_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH] = {
        .name = L3_SRC_IPV6_UC_ROUTE_OVERRIDEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE),
        .flds = l3_src_ipv6_uc_gh_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC] = {
        .name = L3_IPV4_MULTICAST_DEFIP_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_TCAM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV4_MULTICAST_DEFIP_TABLEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY),
        .flds = l3_ipv4_mc_defip_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_64] = {
        .name = L3_IPV6_MULTICAST_DEFIP_64_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 2,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_TCAM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY),
        .flds = l3_ipv6_mc_defip_64_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128] = {
        .name = L3_IPV6_MULTICAST_DEFIP_128_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_LPM_128 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_TCAM,
        .fld_cnt = BCM56880_A0_DNA_4_6_6_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_VRF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1) |
                   (1 << BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY),
        .flds = l3_ipv6_mc_defip_128_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_1_USAGE] = {
        .name = L3_ALPM_LEVEL_1_USAGEs,
        .tbl_id = L3_ALPM_LEVEL_1_USAGEt,
        .attr = 0,
        .multiple = 0,
        .rm_type = 0,
        .fld_cnt = L3_ALPM_LEVEL_1_USAGEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_KEY_TYPE) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_ENT) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_ENT) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_ENT_UTILIZATION),
        .flds = l3_alpm_lvl_1_usage_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_2_USAGE] = {
        .name = L3_ALPM_LEVEL_2_USAGEs,
        .tbl_id = L3_ALPM_LEVEL_2_USAGEt,
        .attr = 0,
        .multiple = 0,
        .rm_type = 0,
        .fld_cnt = L3_ALPM_LEVEL_2_USAGEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_DB) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_ENT) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_ENT) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_ENT_UTILIZATION) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_RBKT) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_RBKT) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_RBKT_UTILIZATION),
        .flds = l3_alpm_lvl_2_usage_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_3_USAGE] = {
        .name = L3_ALPM_LEVEL_3_USAGEs,
        .tbl_id = L3_ALPM_LEVEL_3_USAGEt,
        .attr = 0,
        .multiple = 0,
        .rm_type = 0,
        .fld_cnt = L3_ALPM_LEVEL_3_USAGEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_DB) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_ENT) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_ENT) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_ENT_UTILIZATION) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_RBKT) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_RBKT) |
                   (1 << BCMINT_LTSW_L3_ALPM_USAGE_TBL_RBKT_UTILIZATION),
        .flds = l3_alpm_lvl_3_usage_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ALPM_CONTROL] = {
        .name = L3_ALPM_CONTROLs,
        .tbl_id = L3_ALPM_CONTROLt,
        .attr = 0,
        .multiple = 0,
        .rm_type = 0,
        .fld_cnt = L3_ALPM_CONTROLt_FIELD_COUNT,
        .fld_bmp = (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_OP_STATE) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_0_LVLS) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_1_LVLS) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_2_LVLS) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_NUM_DB_3_LVLS) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_0) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_1) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_2) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_3) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_0) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_1) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_2) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_3) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST_MASK) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST_TYPE_MATCH) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DEST_TYPE_NON_MATCH) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_UC_GL_STRENGTH_PROF_IDX) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_UC_VRF_STRENGTH_PROF_IDX) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_UC_GH_STRENGTH_PROF_IDX) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_UC_GL_STRENGTH_PROF_IDX) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_UC_VRF_STRENGTH_PROF_IDX) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_UC_GH_STRENGTH_PROF_IDX) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV4_COMP_STRENGTH_PROF_IDX) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_IPV6_COMP_STRENGTH_PROF_IDX),
        .flds = l3_alpm_control_flds
    }
};

/******************************************************************************
 * Route usage result obteained from testDB (l3 route perf)
 */

typedef struct alpm_grp_usage_s {
    /* Key: Route group. */
    int grp;

    /* Key: ALPM Template. */
    int alpm_temp;

    /* Key: Date mode. 0 - reduce, 1 - full. */
    int data_mode;

    /* Key: IPv6_128b. 0 - disable, 1 - enable, -1 - irrelevant. */
    int alpm_128b_en;

    /* Count of route entry. */
    int route_cnt;

    /* In use number: Level 1 - entry, Level 2/3 - raw bucket. */
    int cnt_inuse[BCM_L3_ALPM_LEVELS];

    /* Maximum number: Level 1 - entry, Level 2/3 - raw bucket*/
    int cnt_total[BCM_L3_ALPM_LEVELS];
} alpm_grp_usage_t;

static alpm_grp_usage_t alpm_test_db_usage_uft_1[] = {
    /* ALPM 1, Reduce */
    {bcmL3RouteGroupPrivateV4, 1, 0, -1, 2077408, {2140, 8120, 131055}, {16384, 12288, 131072}},
    {bcmL3RouteGroupGlobalV4,  1, 0, -1, 2077408, {2140, 8120, 131055}, {16384, 12288, 131072}},

    /* ALPM 1, Full */
    {bcmL3RouteGroupPrivateV4, 1, 1, -1, 698960, {1630, 5464, 131054}, {16384, 12288, 131072}},
    {bcmL3RouteGroupGlobalV4,  1, 1, -1, 698960, {1630, 5464, 131054}, {16384, 12288, 131072}},

    /* ALPM 1, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 1, 0, 0, 1935792, {4719, 12273, 123596}, {8192,  12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 0, 1935792, {4719, 12273, 123596}, {8192,  12288, 131072}},

    /* ALPM 1, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 1, 1, 0, 698947, {4091, 8184, 131055}, {8192,  12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  1, 1, 0, 698947, {4091, 8184, 131055}, {8192,  12288, 131072}},

    /* ALPM 1, Reduce, v6-128b*/
    {bcmL3RouteGroupPrivateV6, 1, 0, 1, 840212, {4093, 8187, 53241}, {4096, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 1, 840212, {4093, 8187, 53241}, {4096, 12288, 131072}},

    /* ALPM 1, Full, v6-128b*/
    {bcmL3RouteGroupPrivateV6, 1, 1, 1, 422092, {4093, 8151, 88850}, {4096,  12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  1, 1, 1, 422092, {4093, 8151, 88850}, {4096,  12288, 131072}},

    /* ALPM 2, Reduce */
    {bcmL3RouteGroupPrivateV4, 2, 0, -1, 770684, {3822, 6135, 48579}, {8192, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  2, 0, -1, 770684, {3822, 6135, 48579}, {8192, 6144, 65536}},

    /* ALPM 2, Full */
    {bcmL3RouteGroupPrivateV4, 2, 1, -1, 349476, {5126, 5462, 65527}, {8192, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  2, 1, -1, 349476, {5126, 5462, 65527}, {8192, 6144, 65536}},

    /* ALPM 2, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 2, 0, 0, 309176, {4093, 4247, 19429}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 0, 309176, {4093, 4247, 19429}, {4096, 6144, 65536}},

    /* ALPM 2, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 2, 1, 0, 120003, {4093, 4095, 24722}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 1, 0, 120003, {4093, 4095, 24722}, {4096, 6144, 65536}},

    /* ALPM 2, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 2, 0, 1, 119161, {2045, 2285, 10368}, {2048, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 1, 119161, {2045, 2285, 10368}, {2048, 6144, 65536}},

    /* ALPM 2, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 2, 1, 1, 54205, {2045, 2046, 11761}, {2048, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 1, 1, 54205, {2045, 2046, 11761}, {2048, 6144, 65536}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_2[] = {
    /* ALPM 7, Reduce */
    {bcmL3RouteGroupPrivateV4, 7, 0, -1, 207540, {6624, 4089, 13035}, {8192, 4096, 32768}},
    {bcmL3RouteGroupGlobalV4,  7, 0, -1, 207540, {6624, 4089, 13035}, {8192, 4096, 32768}},

    /* ALPM 7, Full */
    {bcmL3RouteGroupPrivateV4, 7, 1, -1, 87912,  {6776, 4089, 21977}, {8192, 4096, 32768}},
    {bcmL3RouteGroupGlobalV4,  7, 1, -1, 87912,  {6776, 4089, 21977}, {8192, 4096, 32768}},

    /* ALPM 7, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 7, 0, 0, 106247, {4090, 2047, 7603}, {4096, 4096, 32768}},
    {bcmL3RouteGroupGlobalV6,  7, 0, 0, 106247, {4090, 2047, 7603}, {4096, 4096, 32768}},

    /* ALPM 7, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 7, 1, 0, 24051,  {4090, 2046, 6013}, {4096, 4096, 32768}},
    {bcmL3RouteGroupGlobalV6,  7, 1, 0, 24051,  {4090, 2046, 6013}, {4096, 4096, 32768}},

    /* ALPM 7, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 7, 0, 1, 20981,  {2048, 1024, 2367}, {2048, 4096, 32768}},
    {bcmL3RouteGroupGlobalV6,  7, 0, 1, 20981,  {2048, 1024, 2367}, {2048, 4096, 32768}},

    /* ALPM 7, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 7, 1, 1, 7157,   {2042, 1022, 2555},  {2048, 4096, 32768}},
    {bcmL3RouteGroupGlobalV6,  7, 1, 1, 7157,   {2042, 1022, 2555},  {2048, 4096, 32768}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_3[] = {
    /* ALPM 1, Reduce */
    {bcmL3RouteGroupPrivateV4, 1, 0, -1, 196303, {3948, 12273, 0}, {8192, 12288, 0}},
    {bcmL3RouteGroupGlobalV4,  1, 0, -1, 196303, {3948, 12273, 0}, {8192, 12288, 0}},

    /* ALPM 1, Full */
    {bcmL3RouteGroupPrivateV4, 1, 1, -1, 65474, {2684, 12275, 0}, {8192, 12288, 0}},
    {bcmL3RouteGroupGlobalV4,  1, 1, -1, 65474, {2684, 12275, 0}, {8192, 12288, 0}},

    /* ALPM 1, Reduce, v6-64b*/
    {bcmL3RouteGroupPrivateV6, 1, 0, 0, 173844, {4093, 10872, 0}, {4096, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 0, 173844, {4093, 10872, 0}, {4096, 12288, 0}},

    /* ALPM 1, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 1, 1, 0, 65436, {3466, 12273, 0}, {4096, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  1, 1, 0, 65436, {3466, 12273, 0}, {4096, 12288, 0}},

    /* ALPM 1, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 1, 0, 1, 80528, {2045, 5105, 0}, {2048, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 1, 80528, {2045, 5105, 0}, {2048, 12288, 0}},

    /* ALPM 1, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 1, 1, 1, 24040, {2045, 5245, 0}, {2048, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  1, 1, 1, 24040, {2045, 5245, 0}, {2048, 12288, 0}},

    /* ALPM 2, Reduce */
    {bcmL3RouteGroupPrivateV4, 2, 0, -1, 87972, {4094, 5500, 0}, {4096, 6144, 0}},
    {bcmL3RouteGroupGlobalV4,  2, 0, -1, 87972, {4094, 5500, 0}, {4096, 6144, 0}},

    /* ALPM 2, Full */
    {bcmL3RouteGroupPrivateV4, 2, 1, -1, 32374, {2096, 6136, 0}, {4096, 6144, 0}},
    {bcmL3RouteGroupGlobalV4,  2, 1, -1, 32374, {2096, 6136, 0}, {4096, 6144, 0}},

    /* ALPM 2, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 2, 0, 0, 30699, {2045, 2559, 0}, {2048, 6144, 0}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 0, 30699, {2045, 2559, 0}, {2048, 6144, 0}},

    /* ALPM 2, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 2, 1, 0, 25506, {2045, 5104, 0}, {2048, 6144, 0}},
    {bcmL3RouteGroupGlobalV6,  2, 1, 0, 25506, {2045, 5104, 0}, {2048, 6144, 0}},

    /* ALPM 2, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 2, 0, 1, 15239, {1021, 1405, 0}, {1024, 6144, 0}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 1, 15239, {1021, 1405, 0}, {1024, 6144, 0}},

    /* ALPM 2, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 2, 1, 1, 7158, {1024, 1535, 0}, {1024, 6144, 0}},
    {bcmL3RouteGroupGlobalV6,  2, 1, 1, 7158, {1024, 1535, 0}, {1024, 6144, 0}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_4[] = {
    /* ALPM 1, Reduce */
    {bcmL3RouteGroupPrivateV4, 1, 0, -1, 2077408, {2140, 8120, 131055}, {8192, 12288, 131072}},
    {bcmL3RouteGroupGlobalV4,  1, 0, -1, 2077408, {2140, 8120, 131055}, {8192, 12288, 131072}},

    /* ALPM 1, Full */
    {bcmL3RouteGroupPrivateV4, 1, 1, -1, 698960, {1630, 5464, 131054}, {8192, 12288, 131072}},
    {bcmL3RouteGroupGlobalV4,  1, 1, -1, 698960, {1630, 5464, 131054}, {8192, 12288, 131072}},

    /* ALPM 1, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 1, 0, 0, 1679075, {4093, 10644, 107206}, {4096, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 0, 1679075, {4093, 10644, 107206}, {4096, 12288, 131072}},

    /* ALPM 1, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 1, 1, 0, 698947, {4091, 8184, 131055}, {4096, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  1, 1, 0, 698947, {4091, 8184, 131055}, {4096, 12288, 131072}},

    /* ALPM 1, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 1, 0, 1, 419904, {2045, 4092, 26606}, {2048, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 1, 419904, {2045, 4092, 26606}, {2048, 12288, 131072}},

    /* ALPM 1, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 1, 1, 1, 211148, {2045, 4081, 44454}, {2048, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  1, 1, 1, 211148, {2045, 4081, 44454}, {2048, 12288, 131072}},

    /* ALPM 2, Reduce */
    {bcmL3RouteGroupPrivateV4, 2, 0, -1, 770684, {3822, 6135, 48579}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  2, 0, -1, 770684, {3822, 6135, 48579}, {4096, 6144, 65536}},

    /* ALPM 2, Full */
    {bcmL3RouteGroupPrivateV4, 2, 1, -1, 279288, {4094, 4365, 52367}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  2, 1, -1, 279288, {4094, 4365, 52367}, {4096, 6144, 65536}},

    /* ALPM 2, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 2, 0, 0, 157336, {2045, 2153, 9930}, {2048, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 0, 157336, {2045, 2153, 9930}, {2048, 6144, 65536}},

    /* ALPM 2, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 2, 1, 0, 61819, {2045, 2047, 12681}, {2048, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 1, 0, 61819, {2045, 2047, 12681}, {2048, 6144, 65536}},

    /* ALPM 2, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 2, 0, 1, 60352, {1021, 1144, 5208}, {1024, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 1, 60352, {1021, 1144, 5208}, {1024, 6144, 65536}},

    /* ALPM 2, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 2, 1, 1, 28408, {1021, 1022, 6095}, {1024, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 1, 1, 28408, {1021, 1022, 6095}, {1024, 6144, 65536}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_5[] = {
    /* ALPM 5, Reduce */
    {bcmL3RouteGroupPrivateV4, 5, 0, -1, 770684, {7644, 6135, 48579}, {8192, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  5, 0, -1, 770684, {7644, 6135, 48579}, {8192, 6144, 65536}},

    /* ALPM 5, Full */
    {bcmL3RouteGroupPrivateV4, 5, 1, -1, 279288, {8188, 4365, 52367}, {8192, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  5, 1, -1, 279288, {8188, 4365, 52367}, {8192, 6144, 65536}},

    /* ALPM 5, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 5, 0, 0, 157336, {4090, 2153, 9930}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 0, 157336, {4090, 2153, 9930}, {4096, 6144, 65536}},

    /* ALPM 5, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 5, 1, 0, 61819, {4090, 2047, 12681}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  5, 1, 0, 61819, {4090, 2047, 12681}, {4096, 6144, 65536}},

    /* ALPM 5, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 5, 0, 1, 60352, {2042, 1144, 5208}, {2048, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 1, 60352, {2042, 1144, 5208}, {2048, 6144, 65536}},

    /* ALPM 5, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 5, 1, 1, 28408, {2042, 1022, 6095}, {2048, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  5, 1, 1, 28408, {2042, 1022, 6095}, {2048, 6144, 65536}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_6[] = {
    /* ALPM 5, Reduce */
    {bcmL3RouteGroupPrivateV4, 5, 0, -1, 770684, {7644, 6135, 48579}, {16384, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  5, 0, -1, 770684, {7644, 6135, 48579}, {16384, 6144, 65536}},

    /* ALPM 5, Full */
    {bcmL3RouteGroupPrivateV4, 5, 1, -1, 349476, {10252, 1556, 65527}, {16384, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  5, 1, -1, 349476, {10252, 1556, 65527}, {16384, 6144, 65536}},

    /* ALPM 5, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 5, 0, 0, 309176, {8186, 4247, 19429}, {8192, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 0, 309176, {8186, 4247, 19429}, {8192, 6144, 65536}},

    /* ALPM 5, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 5, 1, 0, 120003, {8186, 4096, 24722}, {8192, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  5, 1, 0, 120003, {8186, 4096, 24722}, {8192, 6144, 65536}},

    /* ALPM 5, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 5, 0, 1, 119161, {4090, 2285, 10368}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 1, 119161, {4090, 2285, 10368}, {4096, 6144, 65536}},

    /* ALPM 5, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 5, 1, 1, 54205, {4090, 2046, 11761}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  5, 1, 1, 54205, {4090, 2046, 11761}, {4096, 6144, 65536}},

    /* ALPM 6, Reduce */
    {bcmL3RouteGroupPrivateV4, 6, 0, -1, 207540, {6624, 4089, 13035}, {8192, 4096, 32768}},
    {bcmL3RouteGroupGlobalV4,  6, 0, -1, 64794,  {4100, 2043, 4845},  {8192, 2048, 32768}},

    /* ALPM 6, Full */
    {bcmL3RouteGroupPrivateV4, 6, 1, -1, 87912, {6776, 4089, 21977}, {8192, 4096, 32768}},
    {bcmL3RouteGroupGlobalV4,  6, 1, -1, 24024, {4088, 2043, 6005},  {8192, 2048, 32768}},

    /* ALPM 6, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 6, 0, 0, 106247, {4090, 2047, 7603}, {4096, 4096, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 0, 0, 54744,  {4086, 2043, 3965}, {4096, 2048, 32768}},

    /* ALPM 6, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 6, 1, 0, 24051, {4090, 2046, 6013}, {4096, 4096, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 1, 0, 20637, {4086, 2043, 5159}, {4096, 2048, 32768}},

    /* ALPM 6, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 6, 0, 1, 20981, {2042, 1024, 2367}, {2048, 4096, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 0, 1, 12723, {2042, 1021, 1454}, {2048, 2048, 32768}},

    /* ALPM 6, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 6, 1, 1, 7157, {2042, 1022, 2555}, {2048, 4096, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 1, 1, 5755, {2042, 1021, 1854}, {2048, 2048, 32768}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_7[] = {
    /* ALPM 3, Reduce */
    {bcmL3RouteGroupPrivateV4, 3, 0, -1, 130858, {2382, 8181, 0}, {8192, 8192, 0}},
    {bcmL3RouteGroupGlobalV4,  3, 0, -1, 130858, {2382, 8181, 0}, {8192, 8192, 0}},

    /* ALPM 3, Full */
    {bcmL3RouteGroupPrivateV4, 3, 1, -1, 43640, {2730, 8183, 0}, {8192, 8192, 0}},
    {bcmL3RouteGroupGlobalV4,  3, 1, -1, 43640, {2730, 8183, 0}, {8192, 8192, 0}},

    /* ALPM 3, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 3, 0, 0, 122315, {4093, 7650, 0}, {4096, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 0, 122315, {4093, 7650, 0}, {4096, 8192, 0}},

    /* ALPM 3, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 3, 1, 0, 399507, {3242, 8181, 0}, {4096, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  3, 1, 0, 399507, {3242, 8181, 0}, {4096, 8192, 0}},

    /* ALPM 3, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 3, 0, 1, 30696, {2045, 2816, 0}, {2048, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 1, 30696, {2045, 2816, 0}, {2048, 8192, 0}},

    /* ALPM 3, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 3, 1, 1, 15863, {2045, 3328, 0}, {2048, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  3, 1, 1, 15863, {2045, 3328, 0}, {2048, 8192, 0}},

    /* ALPM 4, Reduce */
    {bcmL3RouteGroupPrivateV4, 4, 0, -1, 60600, {3356, 4089, 0}, {4096, 4096, 0}},
    {bcmL3RouteGroupGlobalV4,  4, 0, -1, 60600, {3356, 4089, 0}, {4096, 4096, 0}},

    /* ALPM 4, Full */
    {bcmL3RouteGroupPrivateV4, 4, 1, -1, 16360, {2046, 4089, 0}, {4096, 4096, 0}},
    {bcmL3RouteGroupGlobalV4,  4, 1, -1, 16360, {2046, 4089, 0}, {4096, 4096, 0}},

    /* ALPM 4, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 4, 0, 0, 32695, {2045, 2429, 0}, {2048, 4096, 0}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 0, 32695, {2045, 2429, 0}, {2048, 4096, 0}},

    /* ALPM 4, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 4, 1, 0, 13285, {2045, 3322, 0}, {2048, 4096, 0}},
    {bcmL3RouteGroupGlobalV6,  4, 1, 0, 13285, {2045, 3322, 0}, {2048, 4096, 0}},

    /* ALPM 4, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 4, 0, 1, 8488, {1021, 1107, 0}, {1024, 4096, 0}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 1, 8488, {1021, 1107, 0}, {1024, 4096, 0}},

    /* ALPM 4, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 4, 1, 1, 5115, {1021, 1534, 0}, {1024, 4096, 0}},
    {bcmL3RouteGroupGlobalV6,  4, 1, 1, 5115, {1021, 1534, 0}, {1024, 4096, 0}},

    /* ALPM 9, Reduce */
    {bcmL3RouteGroupPrivateV4, 9, 0, -1, 196303, {3948, 12273, 0}, {8192, 12288, 0}},
    {bcmL3RouteGroupGlobalV4,  9, 0, -1, 196303, {3948, 12273, 0}, {8192, 12288, 0}},

    /* ALPM 9, Full */
    {bcmL3RouteGroupPrivateV4, 9, 1, -1, 65474, {2684, 12275, 0}, {8192, 12288, 0}},
    {bcmL3RouteGroupGlobalV4,  9, 1, -1, 65474, {2684, 12275, 0}, {8192, 12288, 0}},

    /* ALPM 9, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 9, 0, 0, 173844, {4093, 10872, 0}, {4096, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  9, 0, 0, 173844, {4093, 10872, 0}, {4096, 12288, 0}},

    /* ALPM 9, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 9, 1, 0, 65436, {3466, 12273, 0}, {4096, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  9, 1, 0, 65436, {3466, 12273, 0}, {4096, 12288, 0}},

    /* ALPM 9, Reduce, v6-128b*/
    {bcmL3RouteGroupPrivateV6, 9, 0, 1, 80528, {2045, 5105, 0}, {2048, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  9, 0, 1, 80528, {2045, 5105, 0}, {2048, 12288, 0}},

    /* ALPM 9, Full, v6-128b*/
    {bcmL3RouteGroupPrivateV6, 9, 1, 1, 24040, {2045, 5245, 0}, {2048, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  9, 1, 1, 24040, {2045, 5245, 0}, {2048, 12288, 0}},

    /* ALPM 10, Reduce */
    {bcmL3RouteGroupPrivateV4, 10, 0, -1, 130858, {2832, 8181, 0}, {4096, 8192, 0}},
    {bcmL3RouteGroupGlobalV4,  10, 0, -1, 60556,  {3356, 4089, 0}, {4096, 4096, 0}},

    /* ALPM 10, Full */
    {bcmL3RouteGroupPrivateV4, 10, 1, -1, 43640, {2730, 8183, 0}, {4096, 8192, 0}},
    {bcmL3RouteGroupGlobalV4,  10, 1, -1, 16360, {2046, 4089, 0}, {4096, 4096, 0}},

    /* ALPM 10, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 10, 0, 0, 61904, {2045, 3875, 0}, {2048, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  10, 0, 0, 32695, {2045, 2429, 0}, {2048, 4096, 0}},

    /* ALPM 10, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 10, 1, 0, 25035, {2045, 5175, 0}, {2048, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  10, 1, 0, 13285, {2045, 3322, 0}, {2048, 4096, 0}},

    /* ALPM 10, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 10, 0, 1, 15336, {1021, 1408, 0}, {1024, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  10, 0, 1, 8488,  {1021, 1107, 0}, {1024, 4096, 0}},

    /* ALPM 10, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 10, 1, 1, 7159, {1021, 1536, 0}, {1024, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  10, 1, 1, 5115, {1021, 1534, 0}, {1024, 4096, 0}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_11[] = {
    /* ALPM 3, Reduce */
    {bcmL3RouteGroupPrivateV4, 3, 0, -1, 2008528, {4160, 8181, 126482}, {8192, 8192, 131072}},
    {bcmL3RouteGroupGlobalV4,  3, 0, -1, 2008528, {4160, 8181, 126482}, {8192, 8192, 131072}},

    /* ALPM 3, Full */
    {bcmL3RouteGroupPrivateV4, 3, 1, -1, 698960, {2736, 5462, 131054}, {8192, 8192, 131072}},
    {bcmL3RouteGroupGlobalV4,  3, 1, -1, 698960, {2736, 5462, 131054}, {8192, 8192, 131072}},

    /* ALPM 3, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 3, 0, 0, 1112953, {4093, 8007, 73447}, {4096, 8192, 131072}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 0, 1112953, {4093, 8007, 73447}, {4096, 8192, 131072}},

    /* ALPM 3, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 3, 1, 0, 594897, {4093, 5821, 110412}, {4096, 8192, 131072}},
    {bcmL3RouteGroupGlobalV6,  3, 1, 0, 594897, {4093, 5821, 110412}, {4096, 8192, 131072}},

    /* ALPM 3, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 3, 0, 1, 212580, {2045, 2053, 13295}, {2048, 8192, 131072}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 1, 212580, {2045, 2053, 13295}, {2048, 8192, 131072}},

    /* ALPM 3, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 3, 1, 1, 111428, {2045, 2062, 24069}, {2048, 8192, 131072}},
    {bcmL3RouteGroupGlobalV6,  3, 1, 1, 111428, {2045, 2062, 24069}, {2048, 8192, 131072}},

    /* ALPM 4, Reduce */
    {bcmL3RouteGroupPrivateV4, 4, 0, -1, 382056, {3586, 4089, 24018}, {4096, 4096, 65536}},
    {bcmL3RouteGroupGlobalV4,  4, 0, -1, 382056, {3586, 4089, 24018}, {4096, 4096, 65536}},

    /* ALPM 4, Full */
    {bcmL3RouteGroupPrivateV4, 4, 1, -1, 190888, {4090, 4089, 35792}, {4096, 4096, 65536}},
    {bcmL3RouteGroupGlobalV4,  4, 1, -1, 190888, {4090, 4089, 35792}, {4096, 4096, 65536}},

    /* ALPM 4, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 4, 0, 0, 86184, {2045, 2079, 5489}, {2048, 4096, 65536}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 0, 86184, {2045, 2079, 5489}, {2048, 4096, 65536}},

    /* ALPM 4, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 4, 1, 0, 53240, {2045, 2047, 10878}, {2048, 4096, 65536}},
    {bcmL3RouteGroupGlobalV6,  4, 1, 0, 53240, {2045, 2047, 10878}, {2048, 4096, 65536}},

    /* ALPM 4, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 4, 0, 1, 30672, {1021, 1022, 2813}, {1024, 4096, 65536}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 1, 30672, {1021, 1022, 2813}, {1024, 4096, 65536}},

    /* ALPM 4, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 4, 1, 1, 16123, {1021, 1024, 3393}, {1024, 4096, 65536}},
    {bcmL3RouteGroupGlobalV6,  4, 1, 1, 16123, {1021, 1024, 3393}, {1024, 4096, 65536}},

    /* ALPM 9, Reduce */
    {bcmL3RouteGroupPrivateV4, 9, 0, -1, 2077408, {2140, 8120, 131055}, {8192, 12288, 131072}},
    {bcmL3RouteGroupGlobalV4,  9, 0, -1, 2077408, {2140, 8120, 131055}, {8192, 12288, 131072}},

    /* ALPM 9, Full */
    {bcmL3RouteGroupPrivateV4, 9, 1, -1, 698960, {1630, 5464, 131054}, {8192, 12288, 131072}},
    {bcmL3RouteGroupGlobalV4,  9, 1, -1, 698960, {1630, 5464, 131054}, {8192, 12288, 131072}},

    /* ALPM 9, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 9, 0, 0, 1679075, {4093, 10644, 107206}, {4096, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  9, 0, 0, 1679075, {4093, 10644, 107206}, {4096, 12288, 131072}},

    /* ALPM 9, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 9, 1, 0, 698947, {4091, 8184, 131055}, {4096, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  9, 1, 0, 698947, {4091, 8184, 131055}, {4096, 12288, 131072}},

    /* ALPM 9, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 9, 0, 1, 419904, {2045, 4092, 26606}, {2048, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  9, 0, 1, 419904, {2045, 4092, 26606}, {2048, 12288, 131072}},

    /* ALPM 9, Full, v6-128b*/
    {bcmL3RouteGroupPrivateV6, 9, 1, 1, 211148, {2045, 4081, 44454}, {2048, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  9, 1, 1, 211148, {2045, 4081, 44454}, {2048, 12288, 131072}},

    /* ALPM 10, Reduce */
    {bcmL3RouteGroupPrivateV4, 10, 0, -1, 1009552, {3138, 7551, 65526}, {4096, 8192, 65536}},
    {bcmL3RouteGroupGlobalV4,  10, 0, -1, 382056,  {3586, 4089, 24018}, {4096, 4096, 65536}},

    /* ALPM 10, Full, v6-64b*/
    {bcmL3RouteGroupPrivateV4, 10, 1, -1, 349476, {2734, 5462, 65527}, {4096, 8192, 65536}},
    {bcmL3RouteGroupGlobalV4,  10, 1, -1, 190936, {4092, 4089, 35801}, {4096, 4096, 65536}},

    /* ALPM 10, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 10, 0, 0, 173080, {2045, 2234, 10853}, {2048, 8192, 65536}},
    {bcmL3RouteGroupGlobalV6,  10, 0, 0, 86184,  {2045, 2047, 7603},  {2048, 4096, 65536}},

    /* ALPM 10, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 10, 1, 0, 104963, {2045, 2218, 21684}, {2048, 8192, 65536}},
    {bcmL3RouteGroupGlobalV6,  10, 1, 0, 53240,  {2045, 2047, 10878}, {2048, 4096, 65536}},

    /* ALPM 10, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 10, 0, 1, 60616, {1021, 1023, 5584}, {1024, 8192, 65536}},
    {bcmL3RouteGroupGlobalV6,  10, 0, 1, 30672, {1021, 1022, 2813}, {1024, 4096, 65536}},

    /* ALPM 10, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 10, 1, 1, 31568, {1021, 1051, 6663}, {1024, 8192, 65536}},
    {bcmL3RouteGroupGlobalV6,  10, 1, 1, 16123, {1021, 1024, 3393}, {1024, 4096, 65536}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_12[] = {
    /* ALPM 3, Reduce */
    {bcmL3RouteGroupPrivateV4, 3, 0, -1, 130858, {2832, 8181, 0}, {4096, 8192, 0}},
    {bcmL3RouteGroupGlobalV4,  3, 0, -1, 130858, {2832, 8181, 0}, {4096, 8192, 0}},

    /* ALPM 3, Full */
    {bcmL3RouteGroupPrivateV4, 3, 1, -1, 43640, {2730, 8183, 0}, {4096, 8192, 0}},
    {bcmL3RouteGroupGlobalV4,  3, 1, -1, 43640, {2730, 8183, 0}, {4096, 8192, 0}},

    /* ALPM 3, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 3, 0, 0, 61904, {2045, 3875, 0}, {2048, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 0, 61904, {2045, 3875, 0}, {2048, 8192, 0}},

    /* ALPM 3, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 3, 1, 0, 25035, {2045, 5175, 0}, {2048, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  3, 1, 0, 25035, {2045, 5175, 0}, {2048, 8192, 0}},

    /* ALPM 3, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 3, 0, 1, 15336, {1021, 1408, 0}, {1024, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 1, 15336, {1021, 1408, 0}, {1024, 8192, 0}},

    /* ALPM 3, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 3, 1, 1, 7159, {1021, 1536, 0}, {1024, 8192, 0}},
    {bcmL3RouteGroupGlobalV6,  3, 1, 1, 7159, {1021, 1536, 0}, {1024, 8192, 0}},

    /* ALPM 9, Reduce */
    {bcmL3RouteGroupPrivateV4, 9, 0, -1, 196303, {3948, 12273, 0}, {4096, 12288, 0}},
    {bcmL3RouteGroupGlobalV4,  9, 0, -1, 196303, {3948, 12273, 0}, {4096, 12288, 0}},

    /* ALPM 9, Full */
    {bcmL3RouteGroupPrivateV4, 9, 1, -1, 65474, {2684, 12275, 0}, {4096, 12288, 0}},
    {bcmL3RouteGroupGlobalV4,  9, 1, -1, 65474, {2684, 12275, 0}, {4096, 12288, 0}},

    /* ALPM 9, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 9, 0, 0, 86932, {2045, 5440, 0}, {2048, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  9, 0, 0, 86932, {2045, 5440, 0}, {2048, 12288, 0}},

    /* ALPM 9, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 9, 1, 0, 38608, {2045, 7243, 0}, {2048, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  9, 1, 0, 38608, {2045, 7243, 0}, {2048, 12288, 0}},

    /* ALPM 9, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 9, 0, 1, 37546, {1021, 2417, 0}, {1024, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  9, 0, 1, 37546, {1021, 2417, 0}, {1024, 12288, 0}},

    /* ALPM 9, Full, v6-128b*/
    {bcmL3RouteGroupPrivateV6, 9, 1, 1, 13288, {1021, 2813, 0}, {1024, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  9, 1, 1, 13288, {1021, 2813, 0}, {1024, 12288, 0}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_13[] = {
    /* ALPM 3, Reduce */
    {bcmL3RouteGroupPrivateV4, 3, 0, -1, 1974608, {4094, 8044, 124348}, {4096, 8192, 131072}},
    {bcmL3RouteGroupGlobalV4,  3, 0, -1, 1974608, {4094, 8044, 124348}, {4096, 8192, 131072}},

    /* ALPM 3, Full */
    {bcmL3RouteGroupPrivateV4, 3, 1, -1, 698960, {2734, 5462, 131054}, {4096, 8192, 131072}},
    {bcmL3RouteGroupGlobalV4,  3, 1, -1, 698960, {2734, 5462, 131054}, {4096, 8192, 131072}},

    /* ALPM 3, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 3, 0, 0, 553648, {2045, 3981, 36515}, {2048, 8192, 131072}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 0, 553648, {2045, 3981, 36515}, {2048, 8192, 131072}},

    /* ALPM 3, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 3, 1, 0, 295952, {2045, 2901, 54943}, {2048, 8192, 131072}},
    {bcmL3RouteGroupGlobalV6,  3, 1, 0, 295952, {2045, 2901, 54943}, {2048, 8192, 131072}},

    /* ALPM 3, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 3, 0, 1, 108884, {1021, 1029, 6814}, {1024, 8192, 131072}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 1, 108884, {1021, 1029, 6814}, {1024, 8192, 131072}},

    /* ALPM 3, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 3, 1, 1, 57156, {1021, 1038, 12293}, {1024, 8192, 131072}},
    {bcmL3RouteGroupGlobalV6,  3, 1, 1, 57156, {1021, 1038, 12293}, {1024, 8192, 131072}},

    /* ALPM 9, Reduce */
    {bcmL3RouteGroupPrivateV4, 9, 0, -1, 2077408, {2140, 8120, 131055}, {4096, 12288, 131072}},
    {bcmL3RouteGroupGlobalV4,  9, 0, -1, 2077408, {2140, 8120, 131055}, {4096, 12288, 131072}},

    /* ALPM 9, Full */
    {bcmL3RouteGroupPrivateV4, 9, 1, -1, 698960, {1630, 5464, 131054}, {4096, 12288, 131072}},
    {bcmL3RouteGroupGlobalV4,  9, 1, -1, 698960, {1630, 5464, 131054}, {4096, 12288, 131072}},

    /* ALPM 9, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 9, 0, 0, 840304, {2045, 5319, 53657}, {2048, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  9, 0, 0, 840304, {2045, 5319, 53657}, {2048, 12288, 131072}},

    /* ALPM 9, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 9, 1, 0, 349715, {2045, 4091, 65575}, {2048, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  9, 1, 0, 349715, {2045, 4091, 65575}, {2048, 12288, 131072}},

    /* ALPM 9, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 9, 0, 1, 210452, {1021, 2045, 13342},  {1024, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  9, 0, 1, 210452, {1021, 2045, 13342},  {1024, 12288, 131072}},

    /* ALPM 9, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 9, 1, 1, 105708, {1021, 2046, 22261}, {1024, 12288, 131072}},
    {bcmL3RouteGroupGlobalV6,  9, 1, 1, 105708, {1021, 2046, 22261}, {1024, 12288, 131072}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_15[] = {
    /* ALPM 7, Reduce */
    {bcmL3RouteGroupPrivateV4, 7, 0, -1, 382056, {7172, 4089, 24018}, {8192, 4096, 65536}},
    {bcmL3RouteGroupGlobalV4,  7, 0, -1, 382056, {7172, 4089, 24018}, {8192, 4096, 65536}},

    /* ALPM 7, Full */
    {bcmL3RouteGroupPrivateV4, 7, 1, -1, 190936, {8184, 4089, 35801}, {8192, 4096, 65536}},
    {bcmL3RouteGroupGlobalV4,  7, 1, -1, 190936, {8184, 4089, 35801}, {8192, 4096, 65536}},

    /* ALPM 7, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 7, 0, 0, 86184, {4090, 2079, 5489}, {4096, 4096, 65536}},
    {bcmL3RouteGroupGlobalV6,  7, 0, 0, 86184, {4090, 2079, 5489}, {4096, 4096, 65536}},

    /* ALPM 7, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 7, 1, 0, 53240, {4090, 2047, 10878}, {4096, 4096, 65536}},
    {bcmL3RouteGroupGlobalV6,  7, 1, 0, 53240, {4090, 2047, 10878}, {4096, 4096, 65536}},

    /* ALPM 7, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 7, 0, 1, 30762, {2042, 1022, 2813}, {2048, 4096, 65536}},
    {bcmL3RouteGroupGlobalV6,  7, 0, 1, 30762, {2042, 1022, 2813}, {2048, 4096, 65536}},

    /* ALPM 7, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 7, 1, 1, 16123, {2042, 1024, 3393}, {2048, 4096, 65536}},
    {bcmL3RouteGroupGlobalV6,  7, 1, 1, 16123, {2042, 1024, 3393}, {2048, 4096, 65536}},

    /* ALPM 11, Reduce  */
    {bcmL3RouteGroupPrivateV4, 11, 0, -1, 382056, {4754, 2840, 24855}, {8192, 8192, 65536}},
    {bcmL3RouteGroupGlobalV4,  11, 0, -1, 382056, {4754, 2840, 24855}, {8192, 8192, 65536}},

    /* ALPM 11, Full  */
    {bcmL3RouteGroupPrivateV4, 11, 1, -1, 190936, {5288, 2986, 35801}, {8192, 8192, 65536}},
    {bcmL3RouteGroupGlobalV4,  11, 1, -1, 190936, {5288, 2986, 35801}, {8192, 8192, 65536}},

    /* ALPM 11, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 11, 0, 0, 86184, {3062, 1105, 5403}, {4096, 8192, 65536}},
    {bcmL3RouteGroupGlobalV6,  11, 0, 0, 86184, {3062, 1105, 5403}, {4096, 8192, 65536}},

    /* ALPM 11, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 11, 1, 0, 53240, {3060, 1099, 10919}, {4096, 8192, 65536}},
    {bcmL3RouteGroupGlobalV6,  11, 1, 0, 53240, {3060, 1099, 10919}, {4096, 8192, 65536}},

    /* ALPM 11, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 11, 0, 1, 30672, {1531, 513, 2812}, {2048, 8192, 65536}},
    {bcmL3RouteGroupGlobalV6,  11, 0, 1, 30672, {1531, 513, 2812}, {2048, 8192, 65536}},

    /* ALPM 11, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 11, 1, 1, 16123, {1530, 530, 3392}, {2048, 8192, 65536}},
    {bcmL3RouteGroupGlobalV6,  11, 1, 1, 16123, {1530, 530, 3392}, {2048, 8192, 65536}}
};

static alpm_grp_usage_t alpm_test_db_usage_uft_16[] = {
    /* ALPM 1, Reduce */
    {bcmL3RouteGroupPrivateV4, 1, 0, -1, 196303, {3948, 12273, 0}, {16384, 12288, 0}},
    {bcmL3RouteGroupGlobalV4,  1, 0, -1, 196303, {3948, 12273, 0}, {16384, 12288, 0}},

    /* ALPM 1, Full */
    {bcmL3RouteGroupPrivateV4, 1, 1, -1, 65474, {2684, 12275, 0}, {16384, 12288, 0}},
    {bcmL3RouteGroupGlobalV4,  1, 1, -1, 65474, {2684, 12275, 0}, {16384, 12288, 0}},

    /* ALPM 1, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 1, 0, 0, 196279, {4620, 12273, 0}, {8192,  12288, 0}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 0, 196279, {4620, 12273, 0}, {8192,  12288, 0}},

    /* ALPM 1, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 1, 1, 0, 65436, {3466, 12273, 0}, {8192,  12288, 0}},
    {bcmL3RouteGroupGlobalV6,  1, 1, 0, 65436, {3466, 12273, 0}, {8192,  12288, 0}},

    /* ALPM 1, Reduce, v6-128b*/
    {bcmL3RouteGroupPrivateV6, 1, 0, 1, 168618, {4093, 10609, 0}, {4096, 12288, 0}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 1, 168618, {4093, 10609, 0}, {4096, 12288, 0}},

    /* ALPM 1, Full, v6-128b*/
    {bcmL3RouteGroupPrivateV6, 1, 1, 1, 48104, {4093, 10493, 0}, {4096,  12288, 0}},
    {bcmL3RouteGroupGlobalV6,  1, 1, 1, 48104, {4093, 10493, 0}, {4096,  12288, 0}},

    /* ALPM 2, Reduce */
    {bcmL3RouteGroupPrivateV4, 2, 0, -1, 98132, {4600, 6135, 0}, {8192, 6144, 0}},
    {bcmL3RouteGroupGlobalV4,  2, 0, -1, 98132, {4600, 6135, 0}, {8192, 6144, 0}},

    /* ALPM 2, Full */
    {bcmL3RouteGroupPrivateV4, 2, 1, -1, 32374, {2096, 6136, 0}, {8192, 6144, 0}},
    {bcmL3RouteGroupGlobalV4,  2, 1, -1, 32374, {2096, 6136, 0}, {8192, 6144, 0}},

    /* ALPM 2, Reduce, v6-64b */
    {bcmL3RouteGroupPrivateV6, 2, 0, 0, 59264, {4093, 5049, 0}, {4096, 6144, 0}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 0, 59264, {4093, 5049, 0}, {4096, 6144, 0}},

    /* ALPM 2, Full, v6-64b */
    {bcmL3RouteGroupPrivateV6, 2, 1, 0, 30680, {2429, 6136, 0}, {4096, 6144, 0}},
    {bcmL3RouteGroupGlobalV6,  2, 1, 0, 30680, {2429, 6136, 0}, {4096, 6144, 0}},

    /* ALPM 2, Reduce, v6-128b */
    {bcmL3RouteGroupPrivateV6, 2, 0, 1, 38791, {2045, 3068, 0}, {2048, 6144, 0}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 1, 38791, {2045, 3068, 0}, {2048, 6144, 0}},

    /* ALPM 2, Full, v6-128b */
    {bcmL3RouteGroupPrivateV6, 2, 1, 1, 15091, {2045, 3199, 0}, {2048, 6144, 0}},
    {bcmL3RouteGroupGlobalV6,  2, 1, 1, 15091, {2045, 3199, 0}, {2048, 6144, 0}}
};

static alpm_grp_usage_t *alpm_test_db_usage[] = {
    NULL,                       /* UFT 0 NOT SUPPORT */
    alpm_test_db_usage_uft_1,   /* UFT 1 */
    alpm_test_db_usage_uft_2,   /* UFT 2 */
    alpm_test_db_usage_uft_3,   /* UFT 3 */
    alpm_test_db_usage_uft_4,   /* UFT 4 */
    alpm_test_db_usage_uft_5,   /* UFT 5 */
    alpm_test_db_usage_uft_6,   /* UFT 6 */
    alpm_test_db_usage_uft_7,   /* UFT 7 */
    NULL,                       /* UFT 8 No resource for ALPM route*/
    alpm_test_db_usage_uft_7,   /* UFT 9 ALPM resource is same as UFT 7. */
    alpm_test_db_usage_uft_7,   /* UFT 10 ALPM resource is same as UFT 7. */
    alpm_test_db_usage_uft_11,  /* UFT 11 */
    alpm_test_db_usage_uft_12,  /* UFT 12 */
    alpm_test_db_usage_uft_13,  /* UFT 13 */
    NULL,                       /* UFT 14 NOT SUPPORT */
    alpm_test_db_usage_uft_15,  /* UFT 15 */
    alpm_test_db_usage_uft_16,  /* UFT 16 */
    NULL,                       /* UFT 17 NOT SUPPORT */
    alpm_test_db_usage_uft_16,  /* UFT 18 ALPM resource is same as UFT 16. */
    alpm_test_db_usage_uft_1,   /* UFT 19 ALPM resource is same as UFT 1. */
    NULL
};

static int alpm_test_db_usage_tblsz[] = {
    0,                                   /* UFT 0 NOT SUPPORT */
    COUNTOF(alpm_test_db_usage_uft_1),   /* UFT 1 */
    COUNTOF(alpm_test_db_usage_uft_2),   /* UFT 2 */
    COUNTOF(alpm_test_db_usage_uft_3),   /* UFT 3 */
    COUNTOF(alpm_test_db_usage_uft_4),   /* UFT 4 */
    COUNTOF(alpm_test_db_usage_uft_5),   /* UFT 5 */
    COUNTOF(alpm_test_db_usage_uft_6),   /* UFT 6 */
    COUNTOF(alpm_test_db_usage_uft_7),   /* UFT 7 */
    0,                                   /* UFT 8 No resource for ALPM route*/
    COUNTOF(alpm_test_db_usage_uft_7),   /* UFT 9 ALPM resource is same as UFT 7. */
    COUNTOF(alpm_test_db_usage_uft_7),   /* UFT 10 ALPM resource is same as UFT 7. */
    COUNTOF(alpm_test_db_usage_uft_11),  /* UFT 11 */
    COUNTOF(alpm_test_db_usage_uft_12),  /* UFT 12 */
    COUNTOF(alpm_test_db_usage_uft_13),  /* UFT 13 */
    0,                                   /* UFT 14 NOT SUPPORT */
    COUNTOF(alpm_test_db_usage_uft_15),  /* UFT 15 */
    COUNTOF(alpm_test_db_usage_uft_16),  /* UFT 16 */
    0,                                   /* UFT 17 NOT SUPPORT */
    COUNTOF(alpm_test_db_usage_uft_16),  /* UFT 18 ALPM resource is same as UFT 16. */
    COUNTOF(alpm_test_db_usage_uft_1),   /* UFT 19 ALPM resource is same as UFT 1. */
    0
};

/******************************************************************************
 * Private functions
 */

static int
bcm56880_a0_dna_4_6_6_l3_alpm_control_set(int unit,
                                          int alpm_temp,
                                          bcmi_ltsw_uft_mode_t uft_mode)
{
    int fp_lvl, alpm_lvl, max_ecmp_grp;
    int alpm_128b_en, v6_64, v6_128;
    bcmi_lt_entry_t lt_ent;
    bcmi_lt_field_t flds[] = {
        { /* 0 */
            KEY_INPUT_LEVEL_1_BLOCK_0s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 1 */
            KEY_INPUT_LEVEL_1_BLOCK_1s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 2 */
            KEY_INPUT_LEVEL_1_BLOCK_2s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 3 */
            KEY_INPUT_LEVEL_1_BLOCK_3s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 4 */
            DB_LEVEL_1_BLOCK_0s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 5 */
            DB_LEVEL_1_BLOCK_1s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 6 */
            DB_LEVEL_1_BLOCK_2s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 7 */
            DB_LEVEL_1_BLOCK_3s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 8 */
            NUM_DB_0_LEVELSs, BCMI_LT_FIELD_F_SET, 0 , {0}
        },
        { /* 9 */
            NUM_DB_1_LEVELSs, BCMI_LT_FIELD_F_SET, 0 , {0}
        },
        { /* 10 */
            NUM_DB_2_LEVELSs, BCMI_LT_FIELD_F_SET, 0 , {0}
        },
        { /* 11 */
            NUM_DB_3_LEVELSs, BCMI_LT_FIELD_F_SET, 0 , {0}
        },
        { /* 12 */
            DESTINATION_TYPE_NON_MATCHs, BCMI_LT_FIELD_F_SYMBOL, 0, {0}
        },
        { /* 13 */
            DESTINATION_TYPE_MATCHs, BCMI_LT_FIELD_F_SYMBOL, 0, {0}
        },
        { /* 14 */
            DESTINATION_MASKs, 0, 0 , {0}
        },
        { /* 15 */
            DESTINATIONs,  0, 0 , {0}
        }
    };

    SHR_FUNC_ENTER(unit);

    if ((uft_mode == bcmiUftMode3) ||
        (uft_mode == bcmiUftMode7) ||
        (uft_mode == bcmiUftMode9) ||
        (uft_mode == bcmiUftMode10) ||
        (uft_mode == bcmiUftMode12) ||
        (uft_mode == bcmiUftMode16) ||
        (uft_mode == bcmiUftMode18)) {
        alpm_lvl = 2;
    } else {
        alpm_lvl = 3;
    }

    if (uft_mode == bcmiUftMode2) {
        fp_lvl = 3;
    } else {
        fp_lvl = 2;
    }

    alpm_128b_en = bcmi_ltsw_property_get(unit, BCMI_CPN_IPV6_LPM_128B_ENABLE,
                                          1);
    xfs_ltsw_l3_mc_defip_get(unit, &v6_64, &v6_128);

    switch (alpm_temp) {
        case 1: /* Combined */
            if ((uft_mode != bcmiUftMode1) &&
                (uft_mode != bcmiUftMode3) &&
                (uft_mode != bcmiUftMode4) &&
                (uft_mode != bcmiUftMode16) &&
                (uft_mode != bcmiUftMode18) &&
                (uft_mode != bcmiUftMode19)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template 1 requires uft_mode "
                                      "1/3/4/16/18/19.\n")));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
            }
            flds[4].u.sym_val = ALPM_DB_0s; /* Block 0 */
            flds[5].u.sym_val = ALPM_DB_0s; /* Block 1 */
            flds[6].u.sym_val = ALPM_DB_0s; /* Block 2 */
            flds[7].u.sym_val = ALPM_DB_0s; /* Block 3 */

            flds[8].u.val = alpm_lvl; /* DB0 */
            break;
        case 2: /* Parallel */
            if ((uft_mode != bcmiUftMode1) &&
                (uft_mode != bcmiUftMode3) &&
                (uft_mode != bcmiUftMode4) &&
                (uft_mode != bcmiUftMode16) &&
                (uft_mode != bcmiUftMode18) &&
                (uft_mode != bcmiUftMode19)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template 2 requires uft_mode "
                                      "1/3/4/16/18/19.\n")));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
            }

            /* Half of TCAMs are used for ALPM if MC DEFIP is enabled. */
            if (v6_64 || v6_128) {
                /* ALPM use block 2 & 3. */
                flds[4].u.sym_val = ALPM_DB_0s;
                flds[5].u.sym_val = ALPM_DB_0s;
                flds[6].u.sym_val = ALPM_DB_0s;
                flds[7].u.sym_val = ALPM_DB_1s;
            } else {
                flds[4].u.sym_val = ALPM_DB_0s; /* Block 0 */
                flds[5].u.sym_val = ALPM_DB_0s; /* Block 1 */
                flds[6].u.sym_val = ALPM_DB_1s; /* Block 2 */
                flds[7].u.sym_val = ALPM_DB_1s; /* Block 3 */
            }

            flds[8].u.val = alpm_lvl; /* DB0 */
            flds[9].u.val = alpm_lvl; /* DB1 */
            break;
        case 9: /* Combined + Less FP_COMP */
            fp_lvl = 1;
            /*FALLTROUGH*/
        case 3: /* Combined + FP_COMP */
            if ((uft_mode != bcmiUftMode7) &&
                (uft_mode != bcmiUftMode9) &&
                (uft_mode != bcmiUftMode10) &&
                (uft_mode != bcmiUftMode11) &&
                (uft_mode != bcmiUftMode12) &&
                (uft_mode != bcmiUftMode13)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template %d requires uft_mode "
                                      "7/9/10/11/12/13.\n"), alpm_temp));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            flds[0].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
            flds[1].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;
            if (alpm_128b_en) {
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
            } else {
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
            }

            flds[4].u.sym_val = ALPM_DB_2s; /* Block 0 */
            flds[5].u.sym_val = ALPM_DB_3s; /* Block 1 */
            flds[6].u.sym_val = ALPM_DB_0s; /* Block 2 */
            flds[7].u.sym_val = ALPM_DB_0s; /* Block 3 */

            flds[8].u.val = alpm_lvl; /* DB0 */
            flds[10].u.val = fp_lvl;  /* DB2 */
            flds[11].u.val = fp_lvl;  /* DB3 */
            break;
        case 10: /* Parallel + less FP_COMP */
            fp_lvl = 1;
            /*FALLTROUGH*/
        case 4: /* Parallel + FP_COMP */
            if ((uft_mode != bcmiUftMode7) &&
                (uft_mode != bcmiUftMode9) &&
                (uft_mode != bcmiUftMode10) &&
                (uft_mode != bcmiUftMode11)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template %d requires uft_mode "
                                      "7/9/10/11.\n"), alpm_temp));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            flds[0].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
            flds[1].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;
            if (alpm_128b_en) {
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
            } else {
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
            }

            flds[4].u.sym_val = ALPM_DB_2s; /* Block 0 */
            flds[5].u.sym_val = ALPM_DB_3s; /* Block 1 */
            flds[6].u.sym_val = ALPM_DB_0s; /* Block 2 */
            flds[7].u.sym_val = ALPM_DB_1s; /* Block 3 */

            flds[8].u.val = alpm_lvl; /* DB0 */
            flds[9].u.val = alpm_lvl; /* DB1 */
            flds[10].u.val = fp_lvl;  /* DB2 */
            flds[11].u.val = fp_lvl;  /* DB3 */
            break;
        case 5: /* Combined + URPF */
            if ((uft_mode != bcmiUftMode5) &&
                (uft_mode != bcmiUftMode6)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template 5 requires uft_mode "
                                      "5/6.\n")));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
            }

            /* Half of TCAMs are used for ALPM if MC DEFIP is enabled. */
            if (v6_64 || v6_128) {
                /* ALPM use block 2 & 3. */
                flds[4].u.sym_val = ALPM_DB_0s; /* Block 0 */
                flds[5].u.sym_val = ALPM_DB_0s; /* Block 1 */
                flds[6].u.sym_val = ALPM_DB_0s; /* Block 2 */
                flds[7].u.sym_val = ALPM_DB_2s; /* Block 3 */
            } else {
                flds[4].u.sym_val = ALPM_DB_0s; /* Block 0 */
                flds[5].u.sym_val = ALPM_DB_2s; /* Block 1 */
                flds[6].u.sym_val = ALPM_DB_0s; /* Block 2 */
                flds[7].u.sym_val = ALPM_DB_2s; /* Block 3 */
            }

            flds[8].u.val = alpm_lvl;  /* DB0 */
            flds[10].u.val = alpm_lvl; /* DB2 */
            break;
        case 6: /* Parallel + URPF */
            if (uft_mode != bcmiUftMode6) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template 6 requires uft_mode "
                                      "6.\n")));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
            }

            flds[4].u.sym_val = ALPM_DB_0s;
            flds[5].u.sym_val = ALPM_DB_2s;
            flds[6].u.sym_val = ALPM_DB_1s;
            flds[7].u.sym_val = ALPM_DB_3s;

            flds[8].u.val = alpm_lvl; /* DB0 */
            flds[9].u.val = alpm_lvl; /* DB1 */
            flds[10].u.val = alpm_lvl; /* DB2 */
            flds[11].u.val = alpm_lvl; /* DB3 */
            break;
        case 11: /* Combined + less FP_COMP + uRPF */
            if ((uft_mode != bcmiUftMode14) &&
                (uft_mode != bcmiUftMode15)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template 11 requires uft_mode "
                                      "14/15.\n")));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
            fp_lvl = 1;
            /*FALLTROUGH*/
        case 7: /* Combined + FP_COMP + uRPF */
            if ((uft_mode != bcmiUftMode2) &&
                (uft_mode != bcmiUftMode14) &&
                (uft_mode != bcmiUftMode15)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template 7 requires uft_mode "
                                      "2/14/15.\n")));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
            flds[0].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
            flds[1].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;
            if (alpm_128b_en) {
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
            } else {
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
            }

            flds[4].u.sym_val = ALPM_DB_2s;
            flds[5].u.sym_val = ALPM_DB_3s;
            flds[6].u.sym_val = ALPM_DB_0s;
            flds[7].u.sym_val = ALPM_DB_1s;

            flds[8].u.val = alpm_lvl; /* DB0 */
            flds[9].u.val = alpm_lvl; /* DB1 */
            flds[10].u.val = fp_lvl; /* DB2 */
            flds[11].u.val = fp_lvl; /* DB3 */
            break;
        case 8: /* FP_COMP */
            if (uft_mode != bcmiUftMode8) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template 8 requires uft_mode "
                                      "8.\n")));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
            flds[0].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
            flds[1].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;
            flds[2].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
            flds[3].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;

            flds[4].u.sym_val = ALPM_DB_2s;
            flds[5].u.sym_val = ALPM_DB_3s;
            flds[6].u.sym_val = ALPM_DB_2s;
            flds[7].u.sym_val = ALPM_DB_3s;

            flds[10].u.val = fp_lvl; /* DB2 */
            flds[11].u.val = fp_lvl; /* DB3 */
            break;
        default:
            SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    max_ecmp_grp = xfs_ltsw_l3_reduced_route_max_ecmp_group_get(unit);
    if (max_ecmp_grp) {

        flds[12].flags |= BCMI_LT_FIELD_F_SET;
        flds[12].u.sym_val = NHOPs;

        flds[13].flags |= BCMI_LT_FIELD_F_SET;
        flds[13].u.sym_val = ECMPs;

        flds[14].flags |= BCMI_LT_FIELD_F_SET;
        flds[14].u.val = 0xffff & ~max_ecmp_grp;

        flds[15].flags |= BCMI_LT_FIELD_F_SET;
        flds[15].u.val = 0;
    }

    /* Update the L3_ALPM_CONTROL */
    lt_ent.fields = flds;
    lt_ent.nfields = sizeof(flds) / sizeof(flds[0]);
    lt_ent.attr = 0;
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_update(unit, L3_ALPM_CONTROLs, &lt_ent, NULL));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_dna_4_6_6_l3_fib_tbl_db_get(int unit,
                                        bcmint_ltsw_l3_fib_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    /* Bitmap of LTs that valid on bcm56880 DNA_4_6_6. */
    tbl_db->tbl_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_UC) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_G) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_MC_SG) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_G) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_HOST_IPV6_MC_SG) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_64) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_1_USAGE) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_2_USAGE) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_3_USAGE) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ALPM_CONTROL);

    tbl_db->tbls = bcm56880_a0_dna_4_6_6_l3_fib_tbls;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Route projection based on testDB.
 *
 * \param [in] unit Unit number.
 * \param [in] grp ALPM route group.
 * \param [out] prj_cnt Uni-dimensional projected maximum number of route.
 * \param [out] max_usage_lvl The level with maximum resource usage.
 * \param [out] max_usage Maximum resource usage.
 *
 * retval SHR_E_NONE No errors.
 * retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_dna_4_6_6_l3_alpm_grp_prj_db_get(int unit,
                                             bcm_l3_route_group_t grp,
                                             int *prj_cnt,
                                             int *max_usage_lvl,
                                             int *max_usage)
{
    bcmi_ltsw_uft_mode_t uft_mode = bcmiUftModeInvalid;
    int alpm_temp, en_128b, dm, i, usage, usage_m, lvl_m, lvl_thres, delta;
    alpm_grp_usage_t *db;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prj_cnt, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_uft_mode_get(unit, &uft_mode));

    alpm_temp = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_TEMPLATE, 1);
    en_128b = bcmi_ltsw_property_get(unit, BCMI_CPN_IPV6_LPM_128B_ENABLE, 1);
    dm = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_DEFAULT_DATA_MODE, 1);

    db = alpm_test_db_usage[uft_mode];
    if (!db) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (i = 0; i < alpm_test_db_usage_tblsz[uft_mode]; i++) {
        if ((grp == db->grp) && (alpm_temp == db->alpm_temp) &&
            (dm == db->data_mode) &&
            ((db->alpm_128b_en == -1) || (en_128b == db->alpm_128b_en))) {
            break;
        }
        db++;
    }

    if (i == alpm_test_db_usage_tblsz[uft_mode]) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    lvl_m = 0;
    usage_m = 0;
    for (i = 0; i < BCM_L3_ALPM_LEVELS; i++) {
        if (db->cnt_total[i] == 0) {
            break;
        }

        usage = db->cnt_inuse[i] * 10000L / db->cnt_total[i];

        /* Give 1% preference to L3 usage. */
        delta = (lvl_m == 3) ? 1 : 0;
        if ((usage + delta) > usage_m) {
            lvl_m = i + 1;
            usage_m = usage;
        }
    }

    if (lvl_m < 3) {
        lvl_thres = 100;
    } else {
        lvl_thres = bcmi_ltsw_property_get(unit,
                                           BCMI_CPN_L3_ALPM_BNK_THRESHOLD,
                                           95);
    }

    *prj_cnt = db->route_cnt * lvl_thres * 100L / usage_m;
    if (max_usage) {
        *max_usage = lvl_thres;
    }
    if (max_usage_lvl) {
        *max_usage_lvl = lvl_m;
    }

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_l3_fib_drv_t bcm56880_a0_dna_4_6_6_ltsw_l3_fib_drv = {
    .l3_fib_init = xfs_ltsw_l3_fib_init,
    .l3_fib_deinit = xfs_ltsw_l3_fib_deinit,
    .l3_fib_vrf_max_get = xfs_ltsw_l3_fib_vrf_max_get,

    /* Host IPv4 UC */
    .l3_host_ipv4_uc_add = xfs_ltsw_l3_host_ipv4_uc_add,
    .l3_host_ipv4_uc_delete = xfs_ltsw_l3_host_ipv4_uc_delete,
    .l3_host_ipv4_uc_del_by_prefix = xfs_ltsw_l3_host_ipv4_uc_del_by_prefix,
    .l3_host_ipv4_uc_find = xfs_ltsw_l3_host_ipv4_uc_find,
    .l3_host_ipv4_uc_traverse = xfs_ltsw_l3_host_ipv4_uc_traverse,

    /* Host IPv4 MC */
    .l3_host_ipv4_mc_add = xfs_ltsw_l3_host_ipv4_mc_add,
    .l3_host_ipv4_mc_delete = xfs_ltsw_l3_host_ipv4_mc_delete,
    .l3_host_ipv4_mc_find = xfs_ltsw_l3_host_ipv4_mc_find,
    .l3_host_ipv4_mc_traverse = xfs_ltsw_l3_host_ipv4_mc_traverse,
    .l3_host_ipv4_mc_flexctr_attach = xfs_ltsw_l3_host_ipv4_mc_flexctr_attach,
    .l3_host_ipv4_mc_flexctr_detach = xfs_ltsw_l3_host_ipv4_mc_flexctr_detach,
    .l3_host_ipv4_mc_flexctr_info_get =
                                    xfs_ltsw_l3_host_ipv4_mc_flexctr_info_get,

    /* Host IPv6 UC */
    .l3_host_ipv6_uc_add = xfs_ltsw_l3_host_ipv6_uc_add,
    .l3_host_ipv6_uc_delete = xfs_ltsw_l3_host_ipv6_uc_delete,
    .l3_host_ipv6_uc_del_by_prefix = xfs_ltsw_l3_host_ipv6_uc_del_by_prefix,
    .l3_host_ipv6_uc_find = xfs_ltsw_l3_host_ipv6_uc_find,
    .l3_host_ipv6_uc_traverse = xfs_ltsw_l3_host_ipv6_uc_traverse,

    /* Host IPv6 MC */
    .l3_host_ipv6_mc_add = xfs_ltsw_l3_host_ipv6_mc_add,
    .l3_host_ipv6_mc_delete = xfs_ltsw_l3_host_ipv6_mc_delete,
    .l3_host_ipv6_mc_find = xfs_ltsw_l3_host_ipv6_mc_find,
    .l3_host_ipv6_mc_traverse = xfs_ltsw_l3_host_ipv6_mc_traverse,
    .l3_host_ipv6_mc_flexctr_attach = xfs_ltsw_l3_host_ipv6_mc_flexctr_attach,
    .l3_host_ipv6_mc_flexctr_detach = xfs_ltsw_l3_host_ipv6_mc_flexctr_detach,
    .l3_host_ipv6_mc_flexctr_info_get =
                                    xfs_ltsw_l3_host_ipv6_mc_flexctr_info_get,

    .l3_host_del_by_intf = xfs_ltsw_l3_host_del_by_intf,
    .l3_host_del_all = xfs_ltsw_l3_host_del_all,

    /* Route IPv4 UC */
    .l3_route_ipv4_uc_add = xfs_ltsw_l3_route_ipv4_uc_add,
    .l3_route_ipv4_uc_delete = xfs_ltsw_l3_route_ipv4_uc_delete,
    .l3_route_ipv4_uc_find = xfs_ltsw_l3_route_ipv4_uc_find,
    .l3_route_ipv4_uc_traverse = xfs_ltsw_l3_route_ipv4_uc_traverse,

    /* Route IPv4 MC */
    .l3_route_ipv4_mc_add = xfs_ltsw_l3_route_ipv4_mc_add,
    .l3_route_ipv4_mc_delete = xfs_ltsw_l3_route_ipv4_mc_delete,
    .l3_route_ipv4_mc_find = xfs_ltsw_l3_route_ipv4_mc_find,
    .l3_route_ipv4_mc_traverse = xfs_ltsw_l3_route_ipv4_mc_traverse,
    .l3_route_ipv4_mc_flexctr_attach = xfs_ltsw_l3_route_ipv4_mc_flexctr_attach,
    .l3_route_ipv4_mc_flexctr_detach = xfs_ltsw_l3_route_ipv4_mc_flexctr_detach,
    .l3_route_ipv4_mc_flexctr_info_get =
                                    xfs_ltsw_l3_route_ipv4_mc_flexctr_info_get,

    /* Route IPv6 UC */
    .l3_route_ipv6_uc_add = xfs_ltsw_l3_route_ipv6_uc_add,
    .l3_route_ipv6_uc_delete = xfs_ltsw_l3_route_ipv6_uc_delete,
    .l3_route_ipv6_uc_find = xfs_ltsw_l3_route_ipv6_uc_find,
    .l3_route_ipv6_uc_traverse = xfs_ltsw_l3_route_ipv6_uc_traverse,

    /* Route IPv6 MC */
    .l3_route_ipv6_mc_add = xfs_ltsw_l3_route_ipv6_mc_add,
    .l3_route_ipv6_mc_delete = xfs_ltsw_l3_route_ipv6_mc_delete,
    .l3_route_ipv6_mc_find = xfs_ltsw_l3_route_ipv6_mc_find,
    .l3_route_ipv6_mc_traverse = xfs_ltsw_l3_route_ipv6_mc_traverse,
    .l3_route_ipv6_mc_flexctr_attach = xfs_ltsw_l3_route_ipv6_mc_flexctr_attach,
    .l3_route_ipv6_mc_flexctr_detach = xfs_ltsw_l3_route_ipv6_mc_flexctr_detach,
    .l3_route_ipv6_mc_flexctr_info_get =
                                    xfs_ltsw_l3_route_ipv6_mc_flexctr_info_get,

    .l3_route_del_by_intf = xfs_ltsw_l3_route_del_by_intf,
    .l3_route_del_all = xfs_ltsw_l3_route_del_all,

    /* VRF stat */
    .l3_vrf_flexctr_attach = xfs_ltsw_l3_vrf_flexctr_attach,
    .l3_vrf_flexctr_detach = xfs_ltsw_l3_vrf_flexctr_detach,
    .l3_vrf_flexctr_info_get = xfs_ltsw_l3_vrf_flexctr_info_get,

     /* VRF route counter */
    .l3_vrf_route_cnt_get = xfs_ltsw_l3_vrf_route_cnt_get,

    /* NAT Host */
    .l3_nat_host_traverse = xfs_ltsw_l3_nat_host_traverse,

    /* FIB table database */
    .l3_fib_tbl_db_get = bcm56880_a0_dna_4_6_6_l3_fib_tbl_db_get,

    /* L3 UC same interface drop */
    .l3_same_intf_drop_set = xfs_ltsw_l3_same_intf_drop_set,
    .l3_same_intf_drop_get = xfs_ltsw_l3_same_intf_drop_get,

    /* Hit bit clear. */
    .l3_host_hit_clear = xfs_ltsw_l3_host_hit_clear,
    .l3_route_hit_clear = xfs_ltsw_l3_route_hit_clear,

    /* Age. */
    .l3_ipmc_age = xfs_ltsw_l3_ipmc_age,

    /* L3 AACL. */
    .l3_fib_aacl_add = xfs_ltsw_l3_aacl_add,
    .l3_fib_aacl_delete = xfs_ltsw_l3_aacl_delete,
    .l3_fib_aacl_delete_all = xfs_ltsw_l3_aacl_delete_all,
    .l3_fib_aacl_find = xfs_ltsw_l3_aacl_find,
    .l3_fib_aacl_traverse = xfs_ltsw_l3_aacl_traverse,
    .l3_fib_aacl_matched_traverse = xfs_ltsw_l3_aacl_matched_traverse,

    /* ALPM Projection. */
    .l3_alpm_grp_prj_db_get = bcm56880_a0_dna_4_6_6_l3_alpm_grp_prj_db_get,
    .l3_alpm_control_set = bcm56880_a0_dna_4_6_6_l3_alpm_control_set,

    /* Performance operation. */
    .l3_route_perf_v4_lt_op = xfs_ltsw_l3_route_perf_v4_lt_op,
    .l3_route_perf_v6_lt_op = xfs_ltsw_l3_route_perf_v6_lt_op,
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_dna_4_6_6_ltsw_l3_fib_drv_attach(int unit)
{
    return mbcm_ltsw_l3_fib_drv_set(unit,
                                    &bcm56880_a0_dna_4_6_6_ltsw_l3_fib_drv);
}
