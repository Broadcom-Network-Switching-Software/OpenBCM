/*! \file bcm56780_a0_dna_2_4_13_l3_fib.c
 *
 * BCM56780_A0 DNA_2_4_13 L3 FIB driver.
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
    SHR_FUNC_ENTER(unit);

    if (!sal_strcmp(symbol, WIDE_MODEs)) {
        *value = bcmL3VrfRouteDataModeFull;
    } else if (!sal_strcmp(symbol, NARROW_MODEs)) {
        *value = bcmL3VrfRouteDataModeReduced;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
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
    SHR_FUNC_ENTER(unit);

    if (value == bcmL3VrfRouteDataModeFull) {
        *symbol = WIDE_MODEs;
    } else  if (value == bcmL3VrfRouteDataModeReduced) {
        *symbol = NARROW_MODEs;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * L3_IPV4_UNICAST_TABLE
 */
static const
bcmint_ltsw_l3_fib_fld_t l3_ipv4_uc_flds[BCMINT_LTSW_L3_FIB_TBL_UC_FLD_CNT] = {
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0] = {
        IPV4_ADDRs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_IPV4_ADDRf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_NAT_IDX] = {
        NAT_INDEXs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_NAT_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1] = {
        NHOP_2_OR_ECMP_GROUP_INDEX_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_NHOP_2_OR_ECMP_GROUP_INDEX_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_NAT_CTRL] = {
        NAT_CTRLs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_NAT_CTRLf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX] = {
        ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_MISC_CTRL_1f,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt_IPV6_ADDRf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_CLASS_ID] = {
        CLASS_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_NHOP_2_OR_ECMP_GRP_1] = {
        NHOP_2_OR_ECMP_GROUP_INDEX_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt_NHOP_2_OR_ECMP_GROUP_INDEX_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_ECMP_AND_PROT_SWT_PROFILE_IDX] = {
        ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt_MISC_CTRL_1f,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_IP_HDR_DIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF] = {
        L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_L3_IIFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_STRENGTH_PROFILE_INDEXf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_IP_HDR_DIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR] = {
        IP_HDR_SIPs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_IP_HDR_SIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF] = {
        L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_L3_IIFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_STRENGTH_PROFILE_INDEXf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_IP_HDR_DIPf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF] = {
        L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_L3_IIFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_STRENGTH_PROFILE_INDEXf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_IP_HDR_DIPf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_SIP_ADDR] = {
        IP_HDR_SIPs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_IP_HDR_SIPf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_L3_IIF] = {
        L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_L3_IIFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_STRENGTH_PROFILE_INDEXf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTEt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF_MASK] = {
        VRF_ID_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt_VRF_ID_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_OVERRIDEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_OVERRIDEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_OVERRIDEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_OVERRIDEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_OVERRIDEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_OVERRIDEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_OVERRIDEt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF_MASK] = {
        VRF_ID_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_VRF_ID_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTEt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF_MASK] = {
        VRF_ID_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt_VRF_ID_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF] = {
        VRF_IDs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_VRF_IDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_VRF_MASK] = {
        VRF_ID_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_VRF_ID_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1] = {
        IPV6_UPPERs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_UC_DATA_TYPE] = {
        DATA_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_DATA_TYPEf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_IPV4_ADDRf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK] = {
        IPV4_ADDR_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_IPV4_ADDR_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK] = {
        VRF_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_VRF_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY] = {
        ENTRY_PRIORITYs,
        BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_ENTRY_PRIORITYf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_IPV6_ADDR_UPPER_64f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK] = {
        IPV6_ADDR_UPPER_64_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_IPV6_ADDR_UPPER_64_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK] = {
        VRF_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_VRF_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY] = {
        ENTRY_PRIORITYs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_ENTRY_PRIORITYf,
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
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_IPV6_ADDRf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_IP_ADDR_MASK] = {
        IPV6_ADDR_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_IPV6_ADDR_MASKf,
        true,
        false,
        2,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF] = {
        VRFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_VRF_MASK] = {
        VRF_MASKs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_VRF_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST] = {
        DESTINATIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_DESTINATIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_DEST_TYPE] = {
        DESTINATION_TYPEs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_DESTINATION_TYPEf,
        false,
        true,
        0,
        dest_type_symbol_to_scalar,
        dest_type_scalar_to_symbol
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_FLEX_CTR_ACTION] = {
        FLEX_CTR_ACTIONs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_FLEX_CTR_ACTIONf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_CLASS_ID] = {
        CLASS_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MTU_PROFILE_IDX] = {
        MTU_PROFILE_PTRs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_MTU_PROFILE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_RPA_ID] = {
        RPA_IDs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_RPA_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_EXPECTED_L3_IIF] = {
        EXPECTED_L3_IIFs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_EXPECTED_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_0] = {
        MISC_CTRL_0s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_MISC_CTRL_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_MISC_CTRL_1] = {
        MISC_CTRL_1s,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_MISC_CTRL_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_STRENGTH_PROF_IDX] = {
        STRENGTH_PROFILE_INDEXs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_STRENGTH_PROFILE_INDEXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_L3_FIB_TBL_MC_ENTRY_PRIORITY] = {
        ENTRY_PRIORITYs,
        BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_ENTRY_PRIORITYf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

static const bcmint_ltsw_l3_fib_tbl_t bcm56780_a0_dna_2_4_13_l3_fib_tbls[] = {
    [BCMINT_LTSW_L3_FIB_TBL_HOST_IPV4_UC] = {
        .name = L3_IPV4_UNICAST_TABLEs,
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV4_UNICAST_TABLEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 2,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV6_UNICAST_TABLEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_G |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 2,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_1_TABLEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SG |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 2,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_2_TABLEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_G |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_1_TABLEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SG |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_2_TABLEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_VRFt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV4_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_VRFt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV6_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_VRFt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV4_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GL |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_VRFt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_GH |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_SRC_IPV6_UC_ROUTE_OVERRIDEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 1,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_TCAM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV4_MULTICAST_DEFIP_TABLEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 2,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_TCAM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_64_TABLEt_FIELD_COUNT,
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
        .tbl_id = BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt,
        .attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_LPM_128 |
                BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET,
        .multiple = 4,
        .rm_type = BCMINT_LTSW_L3_FIB_TBL_RM_T_TCAM,
        .fld_cnt = BCM56780_A0_DNA_2_4_13_L3_IPV6_MULTICAST_DEFIP_128_TABLEt_FIELD_COUNT,
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
    }
};

/******************************************************************************
 * Private functions
 */

static int
bcm56780_a0_dna_2_4_13_l3_alpm_control_set(int unit,
                                          int alpm_temp,
                                          bcmi_ltsw_uft_mode_t uft_mode)
{
    int fp_lvl, alpm_lvl;
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
        }
    };

    SHR_FUNC_ENTER(unit);

    if ((uft_mode == bcmiUftMode3) ||
        (uft_mode == bcmiUftMode7) ||
        (uft_mode == bcmiUftMode9) ||
        (uft_mode == bcmiUftMode10) ||
        (uft_mode == bcmiUftMode12)) {
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
                (uft_mode != bcmiUftMode4)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template 1 requires uft_mode "
                                      "1/3/4.\n")));
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
                (uft_mode != bcmiUftMode4)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "l3_alpm_template 2 requires uft_mode "
                                      "1/3/4.\n")));
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
                flds[4].u.sym_val = ALPM_DB_1s;
                flds[5].u.sym_val = ALPM_DB_0s;
                flds[6].u.sym_val = ALPM_DB_0s;
                flds[7].u.sym_val = ALPM_DB_0s;
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
            SHR_IF_ERR_EXIT(SHR_E_CONFIG);
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
bcm56780_a0_dna_2_4_13_l3_fib_tbl_db_get(int unit,
                                        bcmint_ltsw_l3_fib_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    /* Bitmap of LTs that valid on bcm56780 DNA_2_4_13. */
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
                      (1 << BCMINT_LTSW_L3_FIB_TBL_DEFIP_IPV6_MC_128);

    tbl_db->tbls = bcm56780_a0_dna_2_4_13_l3_fib_tbls;

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_l3_fib_drv_t bcm56780_a0_dna_2_4_13_ltsw_l3_fib_drv = {
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
    .l3_fib_tbl_db_get = bcm56780_a0_dna_2_4_13_l3_fib_tbl_db_get,

    /* L3 UC same interface drop */
    .l3_same_intf_drop_set = xfs_ltsw_l3_same_intf_drop_set,
    .l3_same_intf_drop_get = xfs_ltsw_l3_same_intf_drop_get,

    /* Hit bit clear. */
    .l3_host_hit_clear = xfs_ltsw_l3_host_hit_clear,
    .l3_route_hit_clear = xfs_ltsw_l3_route_hit_clear,

    /* Age. */
    .l3_ipmc_age = xfs_ltsw_l3_ipmc_age,
    .l3_alpm_control_set = bcm56780_a0_dna_2_4_13_l3_alpm_control_set,

    /* Performance operation. */
    .l3_route_perf_v4_lt_op = xfs_ltsw_l3_route_perf_v4_lt_op,
    .l3_route_perf_v6_lt_op = xfs_ltsw_l3_route_perf_v6_lt_op,
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_dna_2_4_13_ltsw_l3_fib_drv_attach(int unit)
{
    return mbcm_ltsw_l3_fib_drv_set(unit,
                                    &bcm56780_a0_dna_2_4_13_ltsw_l3_fib_drv);
}

