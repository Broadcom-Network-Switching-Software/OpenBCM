/*! \file bcm5699x_l3_fib.c
 *
 * BCM5699x L3 FIB subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/mbcm/l3_fib.h>
#include <bcm_int/ltsw/xgs/l3_fib.h>
#include <bcm_int/ltsw/l3_fib_int.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/uft.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

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
    KEY_L3_IPV4_MCs,
    KEY_L3_IPV6_MCs
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
    ALPM_KEY_INPUT_LPM_L3MC_QUADs,
    ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs,
};

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
 * L3_IPV4_UC_ROUTE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_uc_gl_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        L3_IPV4_UC_ROUTEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        L3_IPV4_UC_ROUTEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP] = {
        ECMP_NHOPs,
        L3_IPV4_UC_ROUTEt_ECMP_NHOPf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID] = {
        ECMP_IDs,
        L3_IPV4_UC_ROUTEt_ECMP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID] = {
        NHOP_IDs,
        L3_IPV4_UC_ROUTEt_NHOP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI] = {
        REPLACE_INT_PRIs,
        L3_IPV4_UC_ROUTEt_REPLACE_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] = {
        INT_PRIs,
        L3_IPV4_UC_ROUTEt_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH] = {
        DROP_ON_DIP_MATCHs,
        L3_IPV4_UC_ROUTEt_DROP_ON_DIP_MATCHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV4_UC_ROUTEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_IPV4_UC_ROUTEt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDs,
        L3_IPV4_UC_ROUTEt_L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_IPV4_UC_ROUTEt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_ACTION_PROFILE_IDs,
        L3_IPV4_UC_ROUTEt_MON_INBAND_TELEMETRY_ACTION_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_OPAQUE_DATA_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDs,
        L3_IPV4_UC_ROUTEt_MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_IPV4_UC_ROUTE_OVERRIDE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_uc_gh_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        L3_IPV4_UC_ROUTE_OVERRIDEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP] = {
        ECMP_NHOPs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_ECMP_NHOPf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID] = {
        ECMP_IDs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_ECMP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID] = {
        NHOP_IDs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_NHOP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI] = {
        REPLACE_INT_PRIs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_REPLACE_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] = {
        INT_PRIs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH] = {
        DROP_ON_DIP_MATCHs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_DROP_ON_DIP_MATCHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_ACTION_PROFILE_IDs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_MON_INBAND_TELEMETRY_ACTION_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_OPAQUE_DATA_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDs,
        L3_IPV4_UC_ROUTE_OVERRIDEt_MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_IPV4_UC_ROUTE_VRF
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_uc_vrf_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        L3_IPV4_UC_ROUTE_VRFt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        L3_IPV4_UC_ROUTE_VRFt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        L3_IPV4_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP] = {
        ECMP_NHOPs,
        L3_IPV4_UC_ROUTE_VRFt_ECMP_NHOPf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID] = {
        ECMP_IDs,
        L3_IPV4_UC_ROUTE_VRFt_ECMP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID] = {
        NHOP_IDs,
        L3_IPV4_UC_ROUTE_VRFt_NHOP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI] = {
        REPLACE_INT_PRIs,
        L3_IPV4_UC_ROUTE_VRFt_REPLACE_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] = {
        INT_PRIs,
        L3_IPV4_UC_ROUTE_VRFt_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH] = {
        DROP_ON_DIP_MATCHs,
        L3_IPV4_UC_ROUTE_VRFt_DROP_ON_DIP_MATCHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV4_UC_ROUTE_VRFt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_IPV4_UC_ROUTE_VRFt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDs,
        L3_IPV4_UC_ROUTE_VRFt_L3_IPV4_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_IPV4_UC_ROUTE_VRFt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_ACTION_PROFILE_IDs,
        L3_IPV4_UC_ROUTE_VRFt_MON_INBAND_TELEMETRY_ACTION_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_OPAQUE_DATA_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDs,
        L3_IPV4_UC_ROUTE_VRFt_MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_IPV6_UC_ROUTE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_uc_gl_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        L3_IPV6_UC_ROUTEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        L3_IPV6_UC_ROUTEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        L3_IPV6_UC_ROUTEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        L3_IPV6_UC_ROUTEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP] = {
        ECMP_NHOPs,
        L3_IPV6_UC_ROUTEt_ECMP_NHOPf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID] = {
        ECMP_IDs,
        L3_IPV6_UC_ROUTEt_ECMP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID] = {
        NHOP_IDs,
        L3_IPV6_UC_ROUTEt_NHOP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI] = {
        REPLACE_INT_PRIs,
        L3_IPV6_UC_ROUTEt_REPLACE_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] = {
        INT_PRIs,
        L3_IPV6_UC_ROUTEt_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH] = {
        DROP_ON_DIP_MATCHs,
        L3_IPV6_UC_ROUTEt_DROP_ON_DIP_MATCHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV6_UC_ROUTEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_IPV6_UC_ROUTEt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDs,
        L3_IPV6_UC_ROUTEt_L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_IPV6_UC_ROUTEt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_ACTION_PROFILE_IDs,
        L3_IPV6_UC_ROUTEt_MON_INBAND_TELEMETRY_ACTION_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_OPAQUE_DATA_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDs,
        L3_IPV6_UC_ROUTEt_MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_IPV6_UC_ROUTE_OVERRIDE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_uc_gh_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_UPPERs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPERs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP] = {
        ECMP_NHOPs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_ECMP_NHOPf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID] = {
        ECMP_IDs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_ECMP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID] = {
        NHOP_IDs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_NHOP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI] = {
        REPLACE_INT_PRIs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_REPLACE_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] = {
        INT_PRIs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH] = {
        DROP_ON_DIP_MATCHs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_DROP_ON_DIP_MATCHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_ACTION_PROFILE_IDs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_MON_INBAND_TELEMETRY_ACTION_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_OPAQUE_DATA_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDs,
        L3_IPV6_UC_ROUTE_OVERRIDEt_MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_IPV6_UC_ROUTE_VRF
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_uc_vrf_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        L3_IPV6_UC_ROUTE_VRFt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_UPPERs,
        L3_IPV6_UC_ROUTE_VRFt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        L3_IPV6_UC_ROUTE_VRFt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPERs,
        L3_IPV6_UC_ROUTE_VRFt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        L3_IPV6_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP] = {
        ECMP_NHOPs,
        L3_IPV6_UC_ROUTE_VRFt_ECMP_NHOPf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID] = {
        ECMP_IDs,
        L3_IPV6_UC_ROUTE_VRFt_ECMP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID] = {
        NHOP_IDs,
        L3_IPV6_UC_ROUTE_VRFt_NHOP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI] = {
        REPLACE_INT_PRIs,
        L3_IPV6_UC_ROUTE_VRFt_REPLACE_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI] = {
        INT_PRIs,
        L3_IPV6_UC_ROUTE_VRFt_INT_PRIf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH] = {
        DROP_ON_DIP_MATCHs,
        L3_IPV6_UC_ROUTE_VRFt_DROP_ON_DIP_MATCHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV6_UC_ROUTE_VRFt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_IPV6_UC_ROUTE_VRFt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDs,
        L3_IPV6_UC_ROUTE_VRFt_L3_IPV6_UC_ROUTE_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_IPV6_UC_ROUTE_VRFt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_ACTION_PROFILE_IDs,
        L3_IPV6_UC_ROUTE_VRFt_MON_INBAND_TELEMETRY_ACTION_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_INT_OPAQUE_DATA_PROFILE_ID] = {
        MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDs,
        L3_IPV6_UC_ROUTE_VRFt_MON_INBAND_TELEMETRY_OPAQUE_DATA_PROFILE_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_SRC_IPV4_UC_ROUTE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv4_uc_gl_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        L3_SRC_IPV4_UC_ROUTEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        L3_SRC_IPV4_UC_ROUTEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_SRC_IPV4_UC_ROUTEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_SRC_IPV4_UC_ROUTEt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTION_IDs,
        L3_SRC_IPV4_UC_ROUTEt_L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_SRC_IPV4_UC_ROUTEt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_SRC_IPV4_UC_ROUTE_OVERRIDE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv4_uc_gh_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTION_IDs,
        L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_SRC_IPV4_UC_ROUTE_VRF
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv4_uc_vrf_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4s,
        L3_SRC_IPV4_UC_ROUTE_VRFt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        L3_SRC_IPV4_UC_ROUTE_VRFt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        L3_SRC_IPV4_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_SRC_IPV4_UC_ROUTE_VRFt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_SRC_IPV4_UC_ROUTE_VRFt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTION_IDs,
        L3_SRC_IPV4_UC_ROUTE_VRFt_L3_SRC_IPV4_UC_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_SRC_IPV4_UC_ROUTE_VRFt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_SRC_IPV6_UC_ROUTE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv6_uc_gl_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        L3_SRC_IPV6_UC_ROUTEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        L3_SRC_IPV6_UC_ROUTEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        L3_SRC_IPV6_UC_ROUTEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        L3_SRC_IPV6_UC_ROUTEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_SRC_IPV6_UC_ROUTEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_SRC_IPV6_UC_ROUTEt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTION_IDs,
        L3_SRC_IPV6_UC_ROUTEt_L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_SRC_IPV6_UC_ROUTEt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_SRC_IPV6_UC_ROUTE_OVERRIDE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv6_uc_gh_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTION_IDs,
        L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_SRC_IPV6_UC_ROUTE_VRF
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_src_ipv6_uc_vrf_flds[BCMINT_XGS_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV6_LOWERs,
        L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        L3_SRC_IPV6_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        L3_SRC_IPV6_UC_ROUTE_VRFt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_SRC_IPV6_UC_ROUTE_VRFt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID] = {
        L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTION_IDs,
        L3_SRC_IPV6_UC_ROUTE_VRFt_L3_SRC_IPV6_UC_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_UC_HIT] = {
        HITs,
        L3_SRC_IPV6_UC_ROUTE_VRFt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_IPV4_MC_ROUTE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_mc_g_flds[BCMINT_XGS_L3_FIB_TBL_MC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0] = {
        MC_GROUPs,
        L3_IPV4_MC_ROUTEt_MC_GROUPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID] = {
        L3_MC_IIF_IDs,
        L3_IPV4_MC_ROUTEt_L3_MC_IIF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_VRF] = {
        L3_IIFs,
        L3_IPV4_MC_ROUTEt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR] = {
        SRC_IPV4s,
        L3_IPV4_MC_ROUTEt_SRC_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK] = {
        SRC_IPV4_MASKs,
        L3_IPV4_MC_ROUTEt_SRC_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_GROUP_ID] = {
        TM_MC_GROUP_IDs,
        L3_IPV4_MC_ROUTEt_TM_MC_GROUP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV4_MC_ROUTEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_COPY_TO_CPU] = {
        COPY_TO_CPUs,
        L3_IPV4_MC_ROUTEt_COPY_TO_CPUf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_DROP] = {
        DROPs,
        L3_IPV4_MC_ROUTEt_DROPf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_DROP_ON_GROUP_MATCH] = {
        DROP_ON_GROUP_MATCHs,
        L3_IPV4_MC_ROUTEt_DROP_ON_GROUP_MATCHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_USE_PORT_TRUNK_ID] = {
        USE_PORT_TRUNK_IDs,
        L3_IPV4_MC_ROUTEt_USE_PORT_TRUNK_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_IS_TRUNK] = {
        IS_TRUNKs,
        L3_IPV4_MC_ROUTEt_IS_TRUNKf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_TRUNK_ID] = {
        TRUNK_IDs,
        L3_IPV4_MC_ROUTEt_TRUNK_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_PORT] = {
        MODPORTs,
        L3_IPV4_MC_ROUTEt_MODPORTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_EXPECTED_L3_IIF_ID] = {
        EXPECTED_L3_MC_IIF_IDs,
        L3_IPV4_MC_ROUTEt_EXPECTED_L3_MC_IIF_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_IPV4_MC_ROUTEt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID] = {
        L3_IPV4_MC_CTR_ING_EFLEX_ACTION_IDs,
        L3_IPV4_MC_ROUTEt_L3_IPV4_MC_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_HIT] = {
        HITs,
        L3_IPV4_MC_ROUTEt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_IPV6_MC_ROUTE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv6_mc_g_flds[BCMINT_XGS_L3_FIB_TBL_MC_FLD_CNT] = {
    [BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0] = {
        MC_GROUP_LOWERs,
        L3_IPV6_MC_ROUTEt_MC_GROUPu_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_GROUP_1] = {
        MC_GROUP_UPPERs,
        L3_IPV6_MC_ROUTEt_MC_GROUPu_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID] = {
        L3_MC_IIF_IDs,
        L3_IPV6_MC_ROUTEt_L3_MC_IIF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_VRF] = {
        VRF_IDs,
        L3_IPV6_MC_ROUTEt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR] = {
        SRC_IPV6_LOWERs,
        L3_IPV6_MC_ROUTEt_SRC_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_SIP_1_ADDR] = {
        SRC_IPV6_UPPERs,
        L3_IPV6_MC_ROUTEt_SRC_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK] = {
        SRC_IPV6_MASK_LOWERs,
        L3_IPV6_MC_ROUTEt_SRC_IPV6_MASKu_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_SIP_1_ADDR_MASK] = {
        SRC_IPV6_MASK_UPPERs,
        L3_IPV6_MC_ROUTEt_SRC_IPV6_MASKu_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_GROUP_ID] = {
        TM_MC_GROUP_IDs,
        L3_IPV6_MC_ROUTEt_TM_MC_GROUP_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV6_MC_ROUTEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_COPY_TO_CPU] = {
        COPY_TO_CPUs,
        L3_IPV6_MC_ROUTEt_COPY_TO_CPUf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_DROP] = {
        DROPs,
        L3_IPV6_MC_ROUTEt_DROPf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_DROP_ON_GROUP_MATCH] = {
        DROP_ON_GROUP_MATCHs,
        L3_IPV6_MC_ROUTEt_DROP_ON_GROUP_MATCHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_USE_PORT_TRUNK_ID] = {
        USE_PORT_TRUNK_IDs,
        L3_IPV6_MC_ROUTEt_USE_PORT_TRUNK_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_IS_TRUNK] = {
        IS_TRUNKs,
        L3_IPV6_MC_ROUTEt_IS_TRUNKf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_TRUNK_ID] = {
        TRUNK_IDs,
        L3_IPV6_MC_ROUTEt_TRUNK_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_PORT] = {
        MODPORTs,
        L3_IPV6_MC_ROUTEt_MODPORTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_EXPECTED_L3_IIF_ID] = {
        EXPECTED_L3_MC_IIF_IDs,
        L3_IPV6_MC_ROUTEt_EXPECTED_L3_MC_IIF_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_DROP_SRC_IPV6_LINK_LOCAL] = {
        DROP_SRC_IPV6_LINK_LOCALs,
        L3_IPV6_MC_ROUTEt_DROP_SRC_IPV6_LINK_LOCALf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_OBJECT] = {
        CTR_ING_EFLEX_OBJECTs,
        L3_IPV6_MC_ROUTEt_CTR_ING_EFLEX_OBJECTf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID] = {
        L3_IPV6_MC_CTR_ING_EFLEX_ACTION_IDs,
        L3_IPV6_MC_ROUTEt_L3_IPV6_MC_CTR_ING_EFLEX_ACTION_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_FIB_TBL_MC_HIT] = {
        HITs,
        L3_IPV6_MC_ROUTEt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
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
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_4] = {
        KEY_INPUT_LEVEL_1_BLOCK_4s,
        L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_4f,
        false,
        true,
        0,
        key_input_symbol_to_scalar,
        key_input_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_5] = {
        KEY_INPUT_LEVEL_1_BLOCK_5s,
        L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_5f,
        false,
        true,
        0,
        key_input_symbol_to_scalar,
        key_input_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_6] = {
        KEY_INPUT_LEVEL_1_BLOCK_6s,
        L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_6f,
        false,
        true,
        0,
        key_input_symbol_to_scalar,
        key_input_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_7] = {
        KEY_INPUT_LEVEL_1_BLOCK_7s,
        L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_7f,
        false,
        true,
        0,
        key_input_symbol_to_scalar,
        key_input_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_8] = {
        KEY_INPUT_LEVEL_1_BLOCK_8s,
        L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_8f,
        false,
        true,
        0,
        key_input_symbol_to_scalar,
        key_input_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_9] = {
        KEY_INPUT_LEVEL_1_BLOCK_9s,
        L3_ALPM_CONTROLt_KEY_INPUT_LEVEL_1_BLOCK_9f,
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
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_4] = {
        DB_LEVEL_1_BLOCK_4s,
        L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_4f,
        false,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_5] = {
        DB_LEVEL_1_BLOCK_5s,
        L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_5f,
        false,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_6] = {
        DB_LEVEL_1_BLOCK_6s,
        L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_6f,
        false,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_7] = {
        DB_LEVEL_1_BLOCK_7s,
        L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_7f,
        false,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_8] = {
        DB_LEVEL_1_BLOCK_8s,
        L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_8f,
        false,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_9] = {
        DB_LEVEL_1_BLOCK_9s,
        L3_ALPM_CONTROLt_DB_LEVEL_1_BLOCK_9f,
        false,
        true,
        0,
        db_symbol_to_scalar,
        db_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_ALPM_CTRL_TBL_HIT_MODE] = {
        HIT_MODEs,
        L3_ALPM_CONTROLt_HIT_MODEf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

static const bcmint_ltsw_l3_fib_tbl_t l3_fib_tbls[] = {
    [BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC] = {
        .name = L3_IPV4_MC_ROUTEs,
        .tbl_id = L3_IPV4_MC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_IPV4_MC_ROUTEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_VRF) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_GROUP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_COPY_TO_CPU) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_DROP) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_DROP_ON_GROUP_MATCH) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_USE_PORT_TRUNK_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_IS_TRUNK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_PORT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_TRUNK_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_EXPECTED_L3_IIF_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_HIT),
        .flds = l3_ipv4_mc_g_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128] = {
        .name = L3_IPV6_MC_ROUTEs,
        .tbl_id = L3_IPV6_MC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 2,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_IPV6_MC_ROUTEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_MC_GROUP_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_GROUP_1) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_L3_IIF_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_VRF) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_SIP_0_ADDR_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_SIP_1_ADDR) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_SIP_1_ADDR_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_GROUP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_COPY_TO_CPU) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_DROP) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_DROP_ON_GROUP_MATCH) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_USE_PORT_TRUNK_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_IS_TRUNK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_PORT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_TRUNK_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_EXPECTED_L3_IIF_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_DROP_SRC_IPV6_LINK_LOCAL) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_MC_HIT),
        .flds = l3_ipv6_mc_g_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL] = {
        .name = L3_IPV4_UC_ROUTEs,
        .tbl_id = L3_IPV4_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_IPV4_UC_ROUTEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_ipv4_uc_gl_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF] = {
        .name = L3_IPV4_UC_ROUTE_VRFs,
        .tbl_id = L3_IPV4_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_IPV4_UC_ROUTE_VRFt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_VRF) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_ipv4_uc_vrf_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH] = {
        .name = L3_IPV4_UC_ROUTE_OVERRIDEs,
        .tbl_id = L3_IPV4_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_IPV4_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_ipv4_uc_gh_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL] = {
        .name = L3_IPV6_UC_ROUTEs,
        .tbl_id = L3_IPV6_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_IPV6_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_ipv6_uc_gl_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF] = {
        .name = L3_IPV6_UC_ROUTE_VRFs,
        .tbl_id = L3_IPV6_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_IPV6_UC_ROUTE_VRFt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_VRF) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_ipv6_uc_vrf_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH] = {
        .name = L3_IPV6_UC_ROUTE_OVERRIDEs,
        .tbl_id = L3_IPV6_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_IPV6_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_NHOP) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_ECMP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_NHOP_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_REPLACE_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_PRI) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_DROP_ON_DIP_MATCH) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_INT_ACT_PROFILE_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_ipv6_uc_gh_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GL] = {
        .name = L3_SRC_IPV4_UC_ROUTEs,
        .tbl_id = L3_SRC_IPV4_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_SRC_IPV4_UC_ROUTEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_src_ipv4_uc_gl_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_VRF] = {
        .name = L3_SRC_IPV4_UC_ROUTE_VRFs,
        .tbl_id = L3_SRC_IPV4_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_SRC_IPV4_UC_ROUTE_VRFt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_VRF) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_src_ipv4_uc_vrf_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV4_UC_GH] = {
        .name = L3_SRC_IPV4_UC_ROUTE_OVERRIDEs,
        .tbl_id = L3_SRC_IPV4_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_src_ipv4_uc_gh_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GL] = {
        .name = L3_SRC_IPV6_UC_ROUTEs,
        .tbl_id = L3_SRC_IPV6_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_src_ipv6_uc_gl_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_VRF] = {
        .name = L3_SRC_IPV6_UC_ROUTE_VRFs,
        .tbl_id = L3_SRC_IPV6_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_SRC_IPV6_UC_ROUTE_VRFt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_VRF) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_src_ipv6_uc_vrf_flds
    },
    [BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH] = {
        .name = L3_SRC_IPV6_UC_ROUTE_OVERRIDEs,
        .tbl_id = L3_SRC_IPV6_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_IP_ADDR_1_MASK) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_OBJECT) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_FLEX_CTR_ACTION_ID) |
                   (1 << BCMINT_XGS_L3_FIB_TBL_UC_HIT),
        .flds = l3_src_ipv6_uc_gh_flds
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
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_4) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_5) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_6) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_7) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_0) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_1) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_2) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_3) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_4) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_5) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_6) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_7) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_HIT_MODE) |
                   (1ULL << BCMINT_LTSW_L3_ALPM_CTRL_TBL_LARGE_VRF),
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

static alpm_grp_usage_t alpm_test_db_usage_uft_4[] = {
    /* ALPM template 1 */
    {bcmL3RouteGroupPrivateV4, 1, 0, -1, 1032528, {1428, 4510, 65517}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  1, 0, -1, 1032528, {1428, 4510, 65517}, {4096, 6144, 65536}},

    /* ALPM template 2 */
    {bcmL3RouteGroupPrivateV4, 2, 0, -1, 349480, {2046, 2411, 21844}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  2, 0, -1, 349480, {2046, 2411, 21844}, {4096, 6144, 65536}},

    /* ALPM template 3 */
    {bcmL3RouteGroupPrivateV4, 3, 0, -1, 443740, {2602, 3063, 27736}, {3072, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  3, 0, -1, 443740, {2602, 3063, 27736}, {3072, 3072, 32768}},

    /* ALPM template 4 */
    {bcmL3RouteGroupPrivateV4, 4, 0, -1, 93329, {1534, 1540, 5902}, {3072, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  4, 0, -1, 93329, {1534, 1540, 5902}, {3072, 3072, 32768}},

    /* ALPM template 5 */
    {bcmL3RouteGroupPrivateV4, 5, 0, -1, 436508, {2558, 3010, 27284}, {4096, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  5, 0, -1, 436508, {2558, 3010, 27284}, {4096, 3072, 32768}},

    /* ALPM template 6 */
    {bcmL3RouteGroupPrivateV4, 6, 0, -1, 60561, {1022, 1028, 3854}, {4096, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  6, 0, -1, 19944, {1022, 1015, 1536}, {4096, 3072, 32768}},

    /* ALPM template 7 */
    {bcmL3RouteGroupPrivateV4, 7, 0, -1, 93329, {1534, 1540, 5902}, {3072, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, -1, 93329, {1534, 1540, 5902}, {3072, 2048, 16384}},

    /* ALPM template 1, V6-64 */
    {bcmL3RouteGroupPrivateV6, 1, 0, 0, 573456, {1917, 3862, 37341}, {1920, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 0, 573456, {1917, 3862, 37341}, {1920, 6144, 65536}},

    /* ALPM template 2, V6-64 */
    {bcmL3RouteGroupPrivateV6, 2, 0, 0, 73960, {893, 975, 4736}, {1920, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 0, 83688, {1021, 1111, 5352}, {1920, 6144, 65536}},

    /* ALPM template 3, V6-64 */
    {bcmL3RouteGroupPrivateV6, 3, 0, 0, 114415, {1405, 1527, 7303}, {1408, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 0, 114415, {1405, 1527, 7303}, {1408, 3072, 32768}},

    /* ALPM template 4, V6-64 */
    {bcmL3RouteGroupPrivateV6, 4, 0, 0, 19244, {637, 646, 1576}, {1408, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 0, 20200, {765, 765, 1520}, {1408, 3072, 32768}},

    /* ALPM template 5, V6-64 */
    {bcmL3RouteGroupPrivateV6, 5, 0, 0, 93416, {1149, 1247, 5968}, {1920, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 0, 93416, {1149, 1247, 5968}, {1920, 3072, 32768}},

    /* ALPM template 6, V6-64 */
    {bcmL3RouteGroupPrivateV6, 6, 0, 0, 12040, {381, 382, 947}, {1920, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 0, 0, 12744, {509, 509, 991}, {1920, 3072, 32768}},

    /* ALPM template 7, V6-64 */
    {bcmL3RouteGroupPrivateV4, 7, 0, 0, 12168, {381, 384, 955}, {1152, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, 0, 12168, {381, 384, 955}, {1152, 2048, 16384}},

    /* ALPM template 1, V6-128 */
    {bcmL3RouteGroupPrivateV6, 1, 0, 1, 210452, {1021, 2043, 13342}, {1024, 2043, 13342}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 1, 210452, {1021, 2043, 13342}, {1024, 2043, 13342}},

    /* ALPM template 2, V6-128 */
    {bcmL3RouteGroupPrivateV6, 2, 0, 1, 30600, {509, 573, 2615}, {1024, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 1, 30600, {509, 573, 2615}, {1024, 6144, 65536}},

    /* ALPM template 3, V6-128 */
    {bcmL3RouteGroupPrivateV6, 3, 0, 1, 45945, {765, 861, 3952}, {768, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 1, 45945, {765, 861, 3952}, {768, 3072, 32768}},

    /* ALPM template 4, V6-128 */
    {bcmL3RouteGroupPrivateV6, 4, 0, 1, 6120, {381, 382, 763}, {768, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 1, 5364, {381, 381, 574}, {768, 3072, 32768}},

    /* ALPM template 5, V6-128 */
    {bcmL3RouteGroupPrivateV6, 5, 0, 1, 38280, {637, 717, 3287}, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 1, 38280, {637, 717, 3287}, {1024, 3072, 32768}},

    /* ALPM template 6, V6-128 */
    {bcmL3RouteGroupPrivateV6, 6, 0, 1, 5097, {253, 254, 507}, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 0, 1, 4853, {253, 253, 446}, {1024, 3072, 32768}},

    /* ALPM template 7, V6-128 */
    {bcmL3RouteGroupPrivateV4, 7, 0, 1, 6120, {381, 382, 763}, {768, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, 1, 6120, {381, 382, 763}, {768, 2048, 16384}},

};

static alpm_grp_usage_t alpm_test_db_usage_uft_5[] = {
    /* ALPM template 1 */
    {bcmL3RouteGroupPrivateV4, 1, 0, -1, 1032528, {1428, 4510, 65517}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  1, 0, -1, 1032528, {1428, 4510, 65517}, {4096, 6144, 65536}},

    /* ALPM template 2 */
    {bcmL3RouteGroupPrivateV4, 2, 0, -1, 349480, {2046, 2411, 21844}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  2, 0, -1, 349480, {2046, 2411, 21844}, {4096, 6144, 65536}},

    /* ALPM template 3 */
    {bcmL3RouteGroupPrivateV4, 3, 0, -1, 443740, {2602, 3063, 27736}, {3072, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  3, 0, -1, 443740, {2602, 3063, 27736}, {3072, 3072, 32768}},

    /* ALPM template 4 */
    {bcmL3RouteGroupPrivateV4, 4, 0, -1, 93329, {1534, 1540, 5902}, {3072, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  4, 0, -1, 93329, {1534, 1540, 5902}, {3072, 3072, 32768}},

    /* ALPM template 5 */
    {bcmL3RouteGroupPrivateV4, 5, 0, -1, 436508, {2558, 3010, 27284}, {4096, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  5, 0, -1, 436508, {2558, 3010, 27284}, {4096, 3072, 32768}},

    /* ALPM template 6 */
    {bcmL3RouteGroupPrivateV4, 6, 0, -1, 60561, {1022, 1028, 3854}, {4096, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  6, 0, -1, 19944, {1022, 1015, 1536}, {4096, 3072, 32768}},

    /* ALPM template 7 */
    {bcmL3RouteGroupPrivateV4, 7, 0, -1, 93329, {1534, 1540, 5902}, {3072, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, -1, 93329, {1534, 1540, 5902}, {3072, 2048, 16384}},

    /* ALPM template 1, V6-64 */
    {bcmL3RouteGroupPrivateV6, 1, 0, 0, 573456, {1917, 3862, 37341}, {1920, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 0, 573456, {1917, 3862, 37341}, {1920, 6144, 65536}},

    /* ALPM template 2, V6-64 */
    {bcmL3RouteGroupPrivateV6, 2, 0, 0, 73960, {893, 975, 4736}, {1920, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 0, 83688, {1021, 1111, 5352}, {1920, 6144, 65536}},

    /* ALPM template 3, V6-64 */
    {bcmL3RouteGroupPrivateV6, 3, 0, 0, 114415, {1405, 1527, 7303}, {1408, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 0, 114415, {1405, 1527, 7303}, {1408, 3072, 32768}},

    /* ALPM template 4, V6-64 */
    {bcmL3RouteGroupPrivateV6, 4, 0, 0, 19244, {637, 646, 1576}, {1408, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 0, 20200, {765, 765, 1520}, {1408, 3072, 32768}},

    /* ALPM template 5, V6-64 */
    {bcmL3RouteGroupPrivateV6, 5, 0, 0, 93416, {1149, 1247, 5968}, {1920, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 0, 93416, {1149, 1247, 5968}, {1920, 3072, 32768}},

    /* ALPM template 6, V6-64 */
    {bcmL3RouteGroupPrivateV6, 6, 0, 0, 12040, {381, 382, 947}, {1920, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 0, 0, 12744, {509, 509, 991}, {1920, 3072, 32768}},

    /* ALPM template 7, V6-64 */
    {bcmL3RouteGroupPrivateV4, 7, 0, 0, 12168, {381, 384, 955}, {1152, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, 0, 12168, {381, 384, 955}, {1152, 2048, 16384}},

    /* ALPM template 1, V6-128 */
    {bcmL3RouteGroupPrivateV6, 1, 0, 1, 210452, {1021, 2043, 13342}, {1024, 2043, 13342}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 1, 210452, {1021, 2043, 13342}, {1024, 2043, 13342}},

    /* ALPM template 2, V6-128 */
    {bcmL3RouteGroupPrivateV6, 2, 0, 1, 30600, {509, 573, 2615}, {1024, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 1, 30600, {509, 573, 2615}, {1024, 6144, 65536}},

    /* ALPM template 3, V6-128 */
    {bcmL3RouteGroupPrivateV6, 3, 0, 1, 45945, {765, 861, 3952}, {768, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 1, 45945, {765, 861, 3952}, {768, 3072, 32768}},

    /* ALPM template 4, V6-128 */
    {bcmL3RouteGroupPrivateV6, 4, 0, 1, 6120, {381, 382, 763}, {768, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 1, 5364, {381, 381, 574}, {768, 3072, 32768}},

    /* ALPM template 5, V6-128 */
    {bcmL3RouteGroupPrivateV6, 5, 0, 1, 38280, {637, 717, 3287}, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 1, 38280, {637, 717, 3287}, {1024, 3072, 32768}},

    /* ALPM template 6, V6-128 */
    {bcmL3RouteGroupPrivateV6, 6, 0, 1, 5097, {253, 254, 507}, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 0, 1, 4853, {253, 253, 446}, {1024, 3072, 32768}},

    /* ALPM template 7, V6-128 */
    {bcmL3RouteGroupPrivateV4, 7, 0, 1, 6120, {381, 382, 763}, {768, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, 1, 6120, {381, 382, 763}, {768, 2048, 16384}},

};

static alpm_grp_usage_t alpm_test_db_usage_uft_6[] = {
    /* ALPM template 1 */
    {bcmL3RouteGroupPrivateV4, 1, 0, -1, 1032528, {1428, 4510, 65517}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  1, 0, -1, 1032528, {1428, 4510, 65517}, {4096, 6144, 65536}},

    /* ALPM template 2 */
    {bcmL3RouteGroupPrivateV4, 2, 0, -1, 349480, {2046, 2411, 21844}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  2, 0, -1, 349480, {2046, 2411, 21844}, {4096, 6144, 65536}},

    /* ALPM template 3 */
    {bcmL3RouteGroupPrivateV4, 3, 0, -1, 443740, {2602, 3063, 27736}, {3072, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  3, 0, -1, 443740, {2602, 3063, 27736}, {3072, 3072, 32768}},

    /* ALPM template 4 */
    {bcmL3RouteGroupPrivateV4, 4, 0, -1, 93329, {1534, 1540, 5902}, {3072, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  4, 0, -1, 93329, {1534, 1540, 5902}, {3072, 3072, 32768}},

    /* ALPM template 5 */
    {bcmL3RouteGroupPrivateV4, 5, 0, -1, 436508, {2558, 3010, 27284}, {4096, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  5, 0, -1, 436508, {2558, 3010, 27284}, {4096, 3072, 32768}},

    /* ALPM template 6 */
    {bcmL3RouteGroupPrivateV4, 6, 0, -1, 60561, {1022, 1028, 3854}, {4096, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  6, 0, -1, 19944, {1022, 1015, 1536}, {4096, 3072, 32768}},

    /* ALPM template 7 */
    {bcmL3RouteGroupPrivateV4, 7, 0, -1, 93329, {1534, 1540, 5902}, {3072, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, -1, 93329, {1534, 1540, 5902}, {3072, 2048, 16384}},

    /* ALPM template 1, V6-64 */
    {bcmL3RouteGroupPrivateV6, 1, 0, 0, 573456, {1917, 3862, 37341}, {1920, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 0, 573456, {1917, 3862, 37341}, {1920, 6144, 65536}},

    /* ALPM template 2, V6-64 */
    {bcmL3RouteGroupPrivateV6, 2, 0, 0, 73960, {893, 975, 4736}, {1920, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 0, 83688, {1021, 1111, 5352}, {1920, 6144, 65536}},

    /* ALPM template 3, V6-64 */
    {bcmL3RouteGroupPrivateV6, 3, 0, 0, 114415, {1405, 1527, 7303}, {1408, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 0, 114415, {1405, 1527, 7303}, {1408, 3072, 32768}},

    /* ALPM template 4, V6-64 */
    {bcmL3RouteGroupPrivateV6, 4, 0, 0, 19244, {637, 646, 1576}, {1408, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 0, 20200, {765, 765, 1520}, {1408, 3072, 32768}},

    /* ALPM template 5, V6-64 */
    {bcmL3RouteGroupPrivateV6, 5, 0, 0, 93416, {1149, 1247, 5968}, {1920, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 0, 93416, {1149, 1247, 5968}, {1920, 3072, 32768}},

    /* ALPM template 6, V6-64 */
    {bcmL3RouteGroupPrivateV6, 6, 0, 0, 12040, {381, 382, 947}, {1920, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 0, 0, 12744, {509, 509, 991}, {1920, 3072, 32768}},

    /* ALPM template 7, V6-64 */
    {bcmL3RouteGroupPrivateV4, 7, 0, 0, 12168, {381, 384, 955}, {1152, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, 0, 12168, {381, 384, 955}, {1152, 2048, 16384}},

    /* ALPM template 1, V6-128 */
    {bcmL3RouteGroupPrivateV6, 1, 0, 1, 210452, {1021, 2043, 13342}, {1024, 2043, 13342}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 1, 210452, {1021, 2043, 13342}, {1024, 2043, 13342}},

    /* ALPM template 2, V6-128 */
    {bcmL3RouteGroupPrivateV6, 2, 0, 1, 30600, {509, 573, 2615}, {1024, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 1, 30600, {509, 573, 2615}, {1024, 6144, 65536}},

    /* ALPM template 3, V6-128 */
    {bcmL3RouteGroupPrivateV6, 3, 0, 1, 45945, {765, 861, 3952}, {768, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 1, 45945, {765, 861, 3952}, {768, 3072, 32768}},

    /* ALPM template 4, V6-128 */
    {bcmL3RouteGroupPrivateV6, 4, 0, 1, 6120, {381, 382, 763}, {768, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 1, 5364, {381, 381, 574}, {768, 3072, 32768}},

    /* ALPM template 5, V6-128 */
    {bcmL3RouteGroupPrivateV6, 5, 0, 1, 38280, {637, 717, 3287}, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 1, 38280, {637, 717, 3287}, {1024, 3072, 32768}},

    /* ALPM template 6, V6-128 */
    {bcmL3RouteGroupPrivateV6, 6, 0, 1, 5097, {253, 254, 507}, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 0, 1, 4853, {253, 253, 446}, {1024, 3072, 32768}},

    /* ALPM template 7, V6-128 */
    {bcmL3RouteGroupPrivateV4, 7, 0, 1, 6120, {381, 382, 763}, {768, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, 1, 6120, {381, 382, 763}, {768, 2048, 16384}},

};

static alpm_grp_usage_t alpm_test_db_usage_uft_8[] = {
    /* ALPM template 1 */
    {bcmL3RouteGroupPrivateV4, 1, 0, -1, 1032528, {1428, 4510, 65517}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  1, 0, -1, 1032528, {1428, 4510, 65517}, {4096, 6144, 65536}},

    /* ALPM template 2 */
    {bcmL3RouteGroupPrivateV4, 2, 0, -1, 349480, {2046, 2411, 21844}, {4096, 6144, 65536}},
    {bcmL3RouteGroupGlobalV4,  2, 0, -1, 349480, {2046, 2411, 21844}, {4096, 6144, 65536}},

    /* ALPM template 3 */
    {bcmL3RouteGroupPrivateV4, 3, 0, -1, 443740, {2602, 3063, 27736}, {3072, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  3, 0, -1, 443740, {2602, 3063, 27736}, {3072, 3072, 32768}},

    /* ALPM template 4 */
    {bcmL3RouteGroupPrivateV4, 4, 0, -1, 93329, {1534, 1540, 5902}, {3072, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  4, 0, -1, 93329, {1534, 1540, 5902}, {3072, 3072, 32768}},

    /* ALPM template 5 */
    {bcmL3RouteGroupPrivateV4, 5, 0, -1, 436508, {2558, 3010, 27284}, {4096, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  5, 0, -1, 436508, {2558, 3010, 27284}, {4096, 3072, 32768}},

    /* ALPM template 6 */
    {bcmL3RouteGroupPrivateV4, 6, 0, -1, 60561, {1022, 1028, 3854}, {4096, 3072, 32768}},
    {bcmL3RouteGroupGlobalV4,  6, 0, -1, 19944, {1022, 1015, 1536}, {4096, 3072, 32768}},

    /* ALPM template 7 */
    {bcmL3RouteGroupPrivateV4, 7, 0, -1, 93329, {1534, 1540, 5902}, {3072, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, -1, 93329, {1534, 1540, 5902}, {3072, 2048, 16384}},

    /* ALPM template 1, V6-64 */
    {bcmL3RouteGroupPrivateV6, 1, 0, 0, 573456, {1917, 3862, 37341}, {1920, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 0, 573456, {1917, 3862, 37341}, {1920, 6144, 65536}},

    /* ALPM template 2, V6-64 */
    {bcmL3RouteGroupPrivateV6, 2, 0, 0, 73960, {893, 975, 4736}, {1920, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 0, 83688, {1021, 1111, 5352}, {1920, 6144, 65536}},

    /* ALPM template 3, V6-64 */
    {bcmL3RouteGroupPrivateV6, 3, 0, 0, 114415, {1405, 1527, 7303}, {1408, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 0, 114415, {1405, 1527, 7303}, {1408, 3072, 32768}},

    /* ALPM template 4, V6-64 */
    {bcmL3RouteGroupPrivateV6, 4, 0, 0, 19244, {637, 646, 1576}, {1408, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 0, 20200, {765, 765, 1520}, {1408, 3072, 32768}},

    /* ALPM template 5, V6-64 */
    {bcmL3RouteGroupPrivateV6, 5, 0, 0, 93416, {1149, 1247, 5968}, {1920, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 0, 93416, {1149, 1247, 5968}, {1920, 3072, 32768}},

    /* ALPM template 6, V6-64 */
    {bcmL3RouteGroupPrivateV6, 6, 0, 0, 12040, {381, 382, 947}, {1920, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 0, 0, 12744, {509, 509, 991}, {1920, 3072, 32768}},

    /* ALPM template 7, V6-64 */
    {bcmL3RouteGroupPrivateV4, 7, 0, 0, 12168, {381, 384, 955}, {1152, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, 0, 12168, {381, 384, 955}, {1152, 2048, 16384}},

    /* ALPM template 1, V6-128 */
    {bcmL3RouteGroupPrivateV6, 1, 0, 1, 210452, {1021, 2043, 13342}, {1024, 2043, 13342}},
    {bcmL3RouteGroupGlobalV6,  1, 0, 1, 210452, {1021, 2043, 13342}, {1024, 2043, 13342}},

    /* ALPM template 2, V6-128 */
    {bcmL3RouteGroupPrivateV6, 2, 0, 1, 30600, {509, 573, 2615}, {1024, 6144, 65536}},
    {bcmL3RouteGroupGlobalV6,  2, 0, 1, 30600, {509, 573, 2615}, {1024, 6144, 65536}},

    /* ALPM template 3, V6-128 */
    {bcmL3RouteGroupPrivateV6, 3, 0, 1, 45945, {765, 861, 3952}, {768, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  3, 0, 1, 45945, {765, 861, 3952}, {768, 3072, 32768}},

    /* ALPM template 4, V6-128 */
    {bcmL3RouteGroupPrivateV6, 4, 0, 1, 6120, {381, 382, 763}, {768, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  4, 0, 1, 5364, {381, 381, 574}, {768, 3072, 32768}},

    /* ALPM template 5, V6-128 */
    {bcmL3RouteGroupPrivateV6, 5, 0, 1, 38280, {637, 717, 3287}, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  5, 0, 1, 38280, {637, 717, 3287}, {1024, 3072, 32768}},

    /* ALPM template 6, V6-128 */
    {bcmL3RouteGroupPrivateV6, 6, 0, 1, 5097, {253, 254, 507}, {1024, 3072, 32768}},
    {bcmL3RouteGroupGlobalV6,  6, 0, 1, 4853, {253, 253, 446}, {1024, 3072, 32768}},

    /* ALPM template 7, V6-128 */
    {bcmL3RouteGroupPrivateV4, 7, 0, 1, 6120, {381, 382, 763}, {768, 2048, 16384}},
    {bcmL3RouteGroupGlobalV4,  7, 0, 1, 6120, {381, 382, 763}, {768, 2048, 16384}},

};

static alpm_grp_usage_t *alpm_test_db_usage[] = {
    NULL,                       /* 0 UFT BANKs for ALPM */
    NULL,                       /* 1 UFT BANKs for ALPM */
    NULL,                       /* 2 UFT BANKs for ALPM */
    NULL,                       /* 3 UFT BANKs for ALPM */
    alpm_test_db_usage_uft_4,   /* 4 UFT BANKs for ALPM */
    alpm_test_db_usage_uft_5,   /* 5 UFT BANKs for ALPM */
    alpm_test_db_usage_uft_6,   /* 6 UFT BANKs for ALPM */
    NULL,                       /* 7 UFT BANKs for ALPM */
    alpm_test_db_usage_uft_8,   /* 8 UFT BANKs for ALPM */
};

static int alpm_test_db_usage_tblsz[] = {
    0,                                   /* 0 UFT BANKs for ALPM */
    0,                                   /* 1 UFT BANKs for ALPM */
    0,                                   /* 2 UFT BANKs for ALPM */
    0,                                   /* 3 UFT BANKs for ALPM */
    COUNTOF(alpm_test_db_usage_uft_4),   /* 4 UFT BANKs for ALPM */
    COUNTOF(alpm_test_db_usage_uft_5),   /* 5 UFT BANKs for ALPM */
    COUNTOF(alpm_test_db_usage_uft_6),   /* 6 UFT BANKs for ALPM */
    0,                                   /* 7 UFT BANKs for ALPM */
    COUNTOF(alpm_test_db_usage_uft_8),   /* 8 UFT BANKs for ALPM */

};


int
bcm5699x_ltsw_l3_alpm_control_set(int unit,
                                  int alpm_temp,
                                  bcmi_ltsw_uft_mode_t uft_mode)
{
    int alpm_128b_en;
    int alpm_level;
    int hitbit_mode;
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
            KEY_INPUT_LEVEL_1_BLOCK_4s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 5 */
            KEY_INPUT_LEVEL_1_BLOCK_5s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 6 */
            KEY_INPUT_LEVEL_1_BLOCK_6s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 7 */
            KEY_INPUT_LEVEL_1_BLOCK_7s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 8 */
            KEY_INPUT_LEVEL_1_BLOCK_8s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 9 */
            KEY_INPUT_LEVEL_1_BLOCK_9s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 10 */
            DB_LEVEL_1_BLOCK_0s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 11 */
            DB_LEVEL_1_BLOCK_1s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 12 */
            DB_LEVEL_1_BLOCK_2s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 13 */
            DB_LEVEL_1_BLOCK_3s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 14 */
            DB_LEVEL_1_BLOCK_4s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 15 */
            DB_LEVEL_1_BLOCK_5s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 16 */
            DB_LEVEL_1_BLOCK_6s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 17 */
            DB_LEVEL_1_BLOCK_7s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 18 */
            DB_LEVEL_1_BLOCK_8s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 19 */
            DB_LEVEL_1_BLOCK_9s,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        },
        { /* 20 */
            NUM_DB_0_LEVELSs, BCMI_LT_FIELD_F_SET, 0 , {0}
        },
        { /* 21 */
            NUM_DB_1_LEVELSs, BCMI_LT_FIELD_F_SET, 0 , {0}
        },
        { /* 22 */
            NUM_DB_2_LEVELSs, BCMI_LT_FIELD_F_SET, 0 , {0}
        },
        { /* 23 */
            NUM_DB_3_LEVELSs, BCMI_LT_FIELD_F_SET, 0 , {0}
        },
        { /* 24 */
            HIT_MODEs,
            BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0 , {0}
        }
    };

    static const char *hit_mode_str[] = {
        ALPM_HIT_MODE_DISABLEs,
        ALPM_HIT_MODE_FORCE_CLEARs,
        ALPM_HIT_MODE_FORCE_SETs,
    };

    SHR_FUNC_ENTER(unit);
    hitbit_mode =
        bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_HIT_MODE, 0);
    alpm_level = 3;
    flds[24].u.sym_val = hit_mode_str[hitbit_mode];

    alpm_128b_en = bcmi_ltsw_property_get(unit,
                                          BCMI_CPN_IPV6_LPM_128B_ENABLE,
                                          1);
    switch (alpm_temp) {
        case 1: /* Combined */
            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
            }
            flds[10].u.sym_val = ALPM_DB_0s;
            flds[11].u.sym_val = ALPM_DB_0s;
            flds[12].u.sym_val = ALPM_DB_0s;
            flds[13].u.sym_val = ALPM_DB_0s;
            flds[14].u.sym_val = ALPM_DB_0s;
            flds[15].u.sym_val = ALPM_DB_0s;
            flds[16].u.sym_val = ALPM_DB_0s;
            flds[17].u.sym_val = ALPM_DB_0s;
            flds[18].u.sym_val = ALPM_DB_0s;
            flds[19].u.sym_val = ALPM_DB_0s;
            flds[20].u.val = alpm_level;
            flds[21].u.val = 0;
            flds[22].u.val = 0;
            flds[23].u.val = 0;
            break;
        case 2: /* Parallel */
            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
            }
            flds[10].u.sym_val = ALPM_DB_0s;
            flds[11].u.sym_val = ALPM_DB_0s;
            flds[12].u.sym_val = ALPM_DB_0s;
            flds[13].u.sym_val = ALPM_DB_0s;
            flds[14].u.sym_val = ALPM_DB_0s;
            flds[15].u.sym_val = ALPM_DB_1s;
            flds[16].u.sym_val = ALPM_DB_1s;
            flds[17].u.sym_val = ALPM_DB_1s;
            flds[18].u.sym_val = ALPM_DB_1s;
            flds[19].u.sym_val = ALPM_DB_1s;
            flds[20].u.val = alpm_level;
            flds[21].u.val = alpm_level;
            flds[22].u.val = 0;
            flds[23].u.val = 0;
            break;
        case 3: /* Combined + FP_COMP */
            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;
            }
            flds[10].u.sym_val = ALPM_DB_0s;
            flds[11].u.sym_val = ALPM_DB_0s;
            flds[12].u.sym_val = ALPM_DB_0s;
            flds[13].u.sym_val = ALPM_DB_0s;
            flds[14].u.sym_val = ALPM_DB_0s;
            flds[15].u.sym_val = ALPM_DB_0s;
            flds[16].u.sym_val = ALPM_DB_0s;
            flds[17].u.sym_val = ALPM_DB_0s;
            flds[18].u.sym_val = ALPM_DB_2s;
            flds[19].u.sym_val = ALPM_DB_3s;
            flds[20].u.val = alpm_level;
            flds[21].u.val = 0;
            flds[22].u.val = alpm_level;
            flds[23].u.val = alpm_level;
            break;
        case 4: /* Parallel + FP_COMP */
            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;
            }
            flds[10].u.sym_val = ALPM_DB_0s;
            flds[11].u.sym_val = ALPM_DB_0s;
            flds[12].u.sym_val = ALPM_DB_0s;
            flds[13].u.sym_val = ALPM_DB_0s;
            flds[14].u.sym_val = ALPM_DB_1s;
            flds[15].u.sym_val = ALPM_DB_1s;
            flds[16].u.sym_val = ALPM_DB_1s;
            flds[17].u.sym_val = ALPM_DB_1s;
            flds[18].u.sym_val = ALPM_DB_2s;
            flds[19].u.sym_val = ALPM_DB_3s;
            flds[20].u.val = alpm_level;
            flds[21].u.val = alpm_level;
            flds[22].u.val = alpm_level;
            flds[23].u.val = alpm_level;
            break;
        case 5: /* Combined + SIP lookup */
            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
            }
            flds[10].u.sym_val = ALPM_DB_0s;
            flds[11].u.sym_val = ALPM_DB_0s;
            flds[12].u.sym_val = ALPM_DB_0s;
            flds[13].u.sym_val = ALPM_DB_0s;
            flds[14].u.sym_val = ALPM_DB_0s;
            flds[15].u.sym_val = ALPM_DB_0s;
            flds[16].u.sym_val = ALPM_DB_2s;
            flds[17].u.sym_val = ALPM_DB_2s;
            flds[18].u.sym_val = ALPM_DB_2s;
            flds[19].u.sym_val = ALPM_DB_2s;
            flds[20].u.val = alpm_level;
            flds[21].u.val = 0;
            flds[22].u.val = alpm_level;
            flds[23].u.val = 0;
            break;
        case 6: /* Parallel + SIP lookup */
            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
            }
            flds[10].u.sym_val = ALPM_DB_0s;
            flds[11].u.sym_val = ALPM_DB_0s;
            flds[12].u.sym_val = ALPM_DB_0s;
            flds[13].u.sym_val = ALPM_DB_1s;
            flds[14].u.sym_val = ALPM_DB_1s;
            flds[15].u.sym_val = ALPM_DB_1s;
            flds[16].u.sym_val = ALPM_DB_2s;
            flds[17].u.sym_val = ALPM_DB_2s;
            flds[18].u.sym_val = ALPM_DB_3s;
            flds[19].u.sym_val = ALPM_DB_3s;
            flds[20].u.val = alpm_level;
            flds[21].u.val = alpm_level;
            flds[22].u.val = alpm_level;
            flds[23].u.val = alpm_level;
            break;
        case 7: /* Combined + FP_COMP + SIP lookup */
            if (alpm_128b_en) {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_QUADs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_QUADs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;
            } else {
                flds[0].u.sym_val = ALPM_KEY_INPUT_LPM_L3MC_DOUBLEs;
                flds[1].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[2].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[3].u.sym_val = ALPM_KEY_INPUT_LPM_DST_DOUBLEs;
                flds[4].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[5].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[6].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[7].u.sym_val = ALPM_KEY_INPUT_LPM_SRC_DOUBLEs;
                flds[8].u.sym_val = ALPM_KEY_INPUT_FP_COMP_DSTs;
                flds[9].u.sym_val = ALPM_KEY_INPUT_FP_COMP_SRCs;
            }
            flds[10].u.sym_val = ALPM_DB_0s;
            flds[11].u.sym_val = ALPM_DB_0s;
            flds[12].u.sym_val = ALPM_DB_0s;
            flds[13].u.sym_val = ALPM_DB_0s;
            flds[14].u.sym_val = ALPM_DB_1s;
            flds[15].u.sym_val = ALPM_DB_1s;
            flds[16].u.sym_val = ALPM_DB_1s;
            flds[17].u.sym_val = ALPM_DB_1s;
            flds[18].u.sym_val = ALPM_DB_2s;
            flds[19].u.sym_val = ALPM_DB_3s;
            flds[20].u.val = alpm_level;
            flds[21].u.val = alpm_level;
            flds[22].u.val = alpm_level;
            flds[23].u.val = alpm_level;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
    }

    /* Update the L3_ALPM_CONTROL */
    lt_ent.fields = flds;
    lt_ent.nfields = sizeof(flds) / sizeof(flds[0]);
    lt_ent.attr = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_update(unit, L3_ALPM_CONTROLs, &lt_ent, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm5699x_a0_ltsw_l3_fib_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_fib_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    tbl_db->tbl_bmp = (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL) |
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
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ROUTE_SRC_IPV6_UC_GH)  |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV4_MC) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_1_USAGE) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_2_USAGE) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_3_USAGE) |
                      (1 << BCMINT_LTSW_L3_FIB_TBL_ALPM_CONTROL);;

    tbl_db->tbls = l3_fib_tbls;

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
bcm5699x_a0_l3_alpm_grp_prj_db_get(int unit,
                                             bcm_l3_route_group_t grp,
                                             int *prj_cnt,
                                             int *max_usage_lvl,
                                             int *max_usage)
{
    int alpm_uft_banks = 0;
    int alpm_temp, en_128b, i, usage, usage_m, lvl_m, lvl_thres, delta;
    alpm_grp_usage_t *db;
    uint32_t l2_uft_bank_count, em_bank_count;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prj_cnt, SHR_E_PARAM);


    SHR_IF_ERR_EXIT
        (bcmi_ltsw_uft_bank_max_get(unit, bcmHashTableL2, &l2_uft_bank_count));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_uft_bank_max_get(unit, bcmHashTableExactMatch, &em_bank_count));

    alpm_uft_banks = 8 - l2_uft_bank_count - em_bank_count;

    alpm_temp = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_TEMPLATE, 1);
    en_128b = bcmi_ltsw_property_get(unit, BCMI_CPN_IPV6_LPM_128B_ENABLE, 1);

    db = alpm_test_db_usage[alpm_uft_banks];
    if (!db) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (i = 0; i < alpm_test_db_usage_tblsz[alpm_uft_banks]; i++) {
        if ((grp == db->grp) && (alpm_temp == db->alpm_temp) &&
            ((db->alpm_128b_en == -1) || (en_128b == db->alpm_128b_en))) {
            break;
        }
        db++;
    }

    if (i == alpm_test_db_usage_tblsz[alpm_uft_banks]) {
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

/*!
 * \brief L3_fib sub driver functions for bcm5699x.
 */
static mbcm_ltsw_l3_fib_drv_t bcm5699x_l3_fib_sub_drv = {
    .l3_alpm_control_set = bcm5699x_ltsw_l3_alpm_control_set,
    .l3_fib_tbl_db_get = bcm5699x_a0_ltsw_l3_fib_tbl_db_get,
    /* ALPM Projection. */
    .l3_alpm_grp_prj_db_get = bcm5699x_a0_l3_alpm_grp_prj_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_l3_fib_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_fib_drv_set(unit, &bcm5699x_l3_fib_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
