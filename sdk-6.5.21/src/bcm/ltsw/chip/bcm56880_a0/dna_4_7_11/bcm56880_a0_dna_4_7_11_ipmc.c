/*! \file bcm56880_a0_dna_4_7_11_ipmc.c
 *
 * BCM56880_A0 DNA_4_7_11 L2 IPMC driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/ipmc.h>
#include <bcm_int/ltsw/xfs/ipmc.h>
#include <bcm_int/ltsw/xfs/types.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_device_constants.h>

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_IPMC

static const char *dest_type_str[] = BCMI_XFS_DEST_TYPE_STR;

/******************************************************************************
 * Private functions
 */
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

/*
 * L2_IPV4_MULTICAST_TABLE
 */
static const bcmint_ipmc_l2_fld_t
l2mc_ipv4_flds[XFS_LTSW_IPMC_L2_FLD_CNT] = {
    [XFS_LTSW_IPMC_L2_FLD_DIP_0] = {
        .fld_name = IPV4_DST_ADDRs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_TABLEt_IPV4_DST_ADDRf,
        .key = true,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_VFI] = {
        .fld_name = VFIs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_TABLEt_VFIf,
        .key = true,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_STRENGTH_PROF_IDX] = {
        .fld_name = STRENGTH_PROFILE_INDEXs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_TABLEt_STRENGTH_PROFILE_INDEXf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_DEST] = {
        .fld_name = DESTINATIONs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_TABLEt_DESTINATIONf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_DEST_TYPE] = {
        .fld_name = DESTINATION_TYPEs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_TABLEt_DESTINATION_TYPEf,
        .key = false,
        .symbol = true,
        .elements = 0,
        .symbol_to_scalar = dest_type_symbol_to_scalar,
        .scalar_to_symbol = dest_type_scalar_to_symbol
    },
    [XFS_LTSW_IPMC_L2_FLD_CLASS_ID] = {
        .fld_name = CLASS_IDs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_TABLEt_CLASS_IDf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_MISC_0] = {
        .fld_name = MISC_CTRL_0s,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_TABLEt_MISC_CTRL_0f,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_MISC_1] = {
        .fld_name = MISC_CTRL_1s,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_TABLEt_MISC_CTRL_1f,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    }
};

/*
 * L2_IPV4_MULTICAST_WIDE_TABLE
 */
static const bcmint_ipmc_l2_fld_t
l2mc_ipv4_wide_flds[XFS_LTSW_IPMC_L2_FLD_CNT] = {
    [XFS_LTSW_IPMC_L2_FLD_DIP_0] = {
        .fld_name = IPV4_DST_ADDRs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_IPV4_DST_ADDRf,
        .key = true,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_VFI] = {
        .fld_name = VFIs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_VFIf,
        .key = true,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_STRENGTH_PROF_IDX] = {
        .fld_name = STRENGTH_PROFILE_INDEXs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_STRENGTH_PROFILE_INDEXf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_DEST] = {
        .fld_name = DESTINATIONs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_DESTINATIONf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_DEST_TYPE] = {
        .fld_name = DESTINATION_TYPEs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_DESTINATION_TYPEf,
        .key = false,
        .symbol = true,
        .elements = 0,
        .symbol_to_scalar = dest_type_symbol_to_scalar,
        .scalar_to_symbol = dest_type_scalar_to_symbol
    },
    [XFS_LTSW_IPMC_L2_FLD_CLASS_ID] = {
        .fld_name = CLASS_IDs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_CLASS_IDf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_MISC_0] = {
        .fld_name = MISC_CTRL_0s,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_MISC_CTRL_0f,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_MISC_1] = {
        .fld_name = MISC_CTRL_1s,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_MISC_CTRL_1f,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_OPQ_OBJ] = {
        .fld_name = OPAQUE_OBJECTs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_OPAQUE_OBJECTf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_FLEXCTR_ACTION] = {
        .fld_name = FLEX_CTR_ACTIONs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_FLEX_CTR_ACTIONf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    }
};

/*
 * L2_IPV4_MULTICAST_SG_TABLE
 */
static const bcmint_ipmc_l2_fld_t
l2mc_ipv4_sg_flds[XFS_LTSW_IPMC_L2_FLD_CNT] = {
    [XFS_LTSW_IPMC_L2_FLD_DIP_0] = {
        .fld_name = IPV4_DST_ADDRs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_IPV4_DST_ADDRf,
        .key = true,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_SIP_0] = {
        .fld_name = IPV4_SRC_ADDRs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_IPV4_SRC_ADDRf,
        .key = true,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_VFI] = {
        .fld_name = VFIs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_VFIf,
        .key = true,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_STRENGTH_PROF_IDX] = {
        .fld_name = STRENGTH_PROFILE_INDEXs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_STRENGTH_PROFILE_INDEXf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_DEST] = {
        .fld_name = DESTINATIONs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_DESTINATIONf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_DEST_TYPE] = {
        .fld_name = DESTINATION_TYPEs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_DESTINATION_TYPEf,
        .key = false,
        .symbol = true,
        .elements = 0,
        .symbol_to_scalar = dest_type_symbol_to_scalar,
        .scalar_to_symbol = dest_type_scalar_to_symbol
    },
    [XFS_LTSW_IPMC_L2_FLD_CLASS_ID] = {
        .fld_name = CLASS_IDs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_CLASS_IDf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_MISC_0] = {
        .fld_name = MISC_CTRL_0s,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_MISC_CTRL_0f,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_MISC_1] = {
        .fld_name = MISC_CTRL_1s,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_MISC_CTRL_1f,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_OPQ_OBJ] = {
        .fld_name = OPAQUE_OBJECTs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_OPAQUE_OBJECTf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_FLEXCTR_ACTION] = {
        .fld_name = FLEX_CTR_ACTIONs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_FLEX_CTR_ACTIONf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    }
};

/*
 * L2_IPV6_MULTICAST_TABLE
 */
static const bcmint_ipmc_l2_fld_t
l2mc_ipv6_flds[XFS_LTSW_IPMC_L2_FLD_CNT] = {
    [XFS_LTSW_IPMC_L2_FLD_DIP_0] = {
        .fld_name = IPV6_DST_ADDRs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_IPV6_DST_ADDRf,
        .key = true,
        .symbol = false,
        .elements = 2,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_VFI] = {
        .fld_name = VFIs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_VFIf,
        .key = true,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_STRENGTH_PROF_IDX] = {
        .fld_name = STRENGTH_PROFILE_INDEXs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_STRENGTH_PROFILE_INDEXf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_DEST] = {
        .fld_name = DESTINATIONs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_DESTINATIONf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_DEST_TYPE] = {
        .fld_name = DESTINATION_TYPEs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_DESTINATION_TYPEf,
        .key = false,
        .symbol = true,
        .elements = 0,
        .symbol_to_scalar = dest_type_symbol_to_scalar,
        .scalar_to_symbol = dest_type_scalar_to_symbol
    },
    [XFS_LTSW_IPMC_L2_FLD_CLASS_ID] = {
        .fld_name = CLASS_IDs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_CLASS_IDf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_MISC_0] = {
        .fld_name = MISC_CTRL_0s,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_MISC_CTRL_0f,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_MISC_1] = {
        .fld_name = MISC_CTRL_1s,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_MISC_CTRL_1f,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_OPQ_OBJ] = {
        .fld_name = OPAQUE_OBJECTs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_OPAQUE_OBJECTf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_FLEXCTR_ACTION] = {
        .fld_name = FLEX_CTR_ACTIONs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_FLEX_CTR_ACTIONf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    }
};

/*
 * L2_IPV6_MULTICAST_SG_TABLE
 */
static const bcmint_ipmc_l2_fld_t
l2mc_ipv6_sg_flds[XFS_LTSW_IPMC_L2_FLD_CNT] = {
    [XFS_LTSW_IPMC_L2_FLD_DIP_0] = {
        .fld_name = IPV6_DST_ADDRs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_IPV6_DST_ADDRf,
        .key = true,
        .symbol = false,
        .elements = 2,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_SIP_0] = {
        .fld_name = IPV6_SRC_ADDRs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_IPV6_SRC_ADDRf,
        .key = true,
        .symbol = false,
        .elements = 2,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_VFI] = {
        .fld_name = VFIs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_VFIf,
        .key = true,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_STRENGTH_PROF_IDX] = {
        .fld_name = STRENGTH_PROFILE_INDEXs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_STRENGTH_PROFILE_INDEXf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_DEST] = {
        .fld_name = DESTINATIONs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_DESTINATIONf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_DEST_TYPE] = {
        .fld_name = DESTINATION_TYPEs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_DESTINATION_TYPEf,
        .key = false,
        .symbol = true,
        .elements = 0,
        .symbol_to_scalar = dest_type_symbol_to_scalar,
        .scalar_to_symbol = dest_type_scalar_to_symbol
    },
    [XFS_LTSW_IPMC_L2_FLD_CLASS_ID] = {
        .fld_name = CLASS_IDs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_CLASS_IDf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_MISC_0] = {
        .fld_name = MISC_CTRL_0s,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_MISC_CTRL_0f,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_MISC_1] = {
        .fld_name = MISC_CTRL_1s,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_MISC_CTRL_1f,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_OPQ_OBJ] = {
        .fld_name = OPAQUE_OBJECTs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_OPAQUE_OBJECTf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    },
    [XFS_LTSW_IPMC_L2_FLD_FLEXCTR_ACTION] = {
        .fld_name = FLEX_CTR_ACTIONs,
        .fld_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_FLEX_CTR_ACTIONf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    }
};

/*
 * R_L2_IPV4_MULTICAST_CONFIG
 */
static const bcmint_ipmc_l2_fld_t
l2mc_ipv4_config_flds[XFS_LTSW_IPMC_L2_FLD_CNT] = {
    [XFS_LTSW_IPMC_L2_FLD_WIDE_VIEW_EN] = {
        .fld_name = WIDE_VIEW_ENABLEs,
        .fld_id = BCM56880_A0_DNA_4_7_11_R_L2_IPV4_MULTICAST_CONFIGt_WIDE_VIEW_ENABLEf,
        .key = false,
        .symbol = false,
        .elements = 0,
        .symbol_to_scalar = NULL,
        .scalar_to_symbol = NULL
    }
};

static const bcmint_ipmc_l2_tbl_t
bcm56880_a0_dna_4_7_11_ipmc_l2_tbls[] = {
    [XFS_LTSW_IPMC_L2_TBL_IPV4] = {
        .name = L2_IPV4_MULTICAST_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_TABLEt,
        .attr = XFS_LTSW_IPMC_L2_TBL_ATTR_V4 |
                XFS_LTSW_IPMC_L2_TBL_ATTR_G,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_TABLEt_FIELD_COUNT,
        .fld_bmp = (1ULL << XFS_LTSW_IPMC_L2_FLD_DIP_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_VFI) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_STRENGTH_PROF_IDX) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_DEST) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_DEST_TYPE) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_CLASS_ID) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_MISC_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_MISC_1),
        .flds = l2mc_ipv4_flds
    },
    [XFS_LTSW_IPMC_L2_TBL_IPV4_WIDE] = {
        .name = L2_IPV4_MULTICAST_WIDE_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt,
        .attr = XFS_LTSW_IPMC_L2_TBL_ATTR_V4 |
                XFS_LTSW_IPMC_L2_TBL_ATTR_G,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_WIDE_TABLEt_FIELD_COUNT,
        .fld_bmp = (1ULL << XFS_LTSW_IPMC_L2_FLD_DIP_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_VFI) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_STRENGTH_PROF_IDX) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_DEST) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_DEST_TYPE) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_CLASS_ID) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_MISC_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_MISC_1) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_OPQ_OBJ) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_FLEXCTR_ACTION),
        .flds = l2mc_ipv4_wide_flds
    },
    [XFS_LTSW_IPMC_L2_TBL_IPV4_SG] = {
        .name = L2_IPV4_MULTICAST_SG_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt,
        .attr = XFS_LTSW_IPMC_L2_TBL_ATTR_V4 |
                XFS_LTSW_IPMC_L2_TBL_ATTR_SG,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_L2_IPV4_MULTICAST_SG_TABLEt_FIELD_COUNT,
        .fld_bmp = (1ULL << XFS_LTSW_IPMC_L2_FLD_DIP_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_SIP_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_VFI) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_STRENGTH_PROF_IDX) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_DEST) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_DEST_TYPE) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_CLASS_ID) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_MISC_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_MISC_1) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_OPQ_OBJ) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_FLEXCTR_ACTION),
        .flds = l2mc_ipv4_sg_flds
    },
    [XFS_LTSW_IPMC_L2_TBL_IPV6] = {
        .name = L2_IPV6_MULTICAST_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt,
        .attr = XFS_LTSW_IPMC_L2_TBL_ATTR_V6 |
                XFS_LTSW_IPMC_L2_TBL_ATTR_G,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_TABLEt_FIELD_COUNT,
        .fld_bmp = (1ULL << XFS_LTSW_IPMC_L2_FLD_DIP_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_VFI) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_STRENGTH_PROF_IDX) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_DEST) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_DEST_TYPE) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_CLASS_ID) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_MISC_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_MISC_1) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_OPQ_OBJ) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_FLEXCTR_ACTION),
        .flds = l2mc_ipv6_flds
    },
    [XFS_LTSW_IPMC_L2_TBL_IPV6_SG] = {
        .name = L2_IPV6_MULTICAST_SG_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt,
        .attr = XFS_LTSW_IPMC_L2_TBL_ATTR_V6 |
                XFS_LTSW_IPMC_L2_TBL_ATTR_SG,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_L2_IPV6_MULTICAST_SG_TABLEt_FIELD_COUNT,
        .fld_bmp = (1ULL << XFS_LTSW_IPMC_L2_FLD_DIP_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_SIP_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_VFI) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_STRENGTH_PROF_IDX) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_DEST) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_DEST_TYPE) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_CLASS_ID) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_MISC_0) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_MISC_1) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_OPQ_OBJ) |
                   (1ULL << XFS_LTSW_IPMC_L2_FLD_FLEXCTR_ACTION),
        .flds = l2mc_ipv6_sg_flds
    },
    [XFS_LTSW_IPMC_L2_TBL_IPV4_CONFIG] = {
        .name = R_L2_IPV4_MULTICAST_CONFIGs,
        .tbl_id = BCM56880_A0_DNA_4_7_11_R_L2_IPV4_MULTICAST_CONFIGt,
        .attr = 0,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_R_L2_IPV4_MULTICAST_CONFIGt_FIELD_COUNT,
        .fld_bmp = (1ULL << XFS_LTSW_IPMC_L2_FLD_WIDE_VIEW_EN),
        .flds = l2mc_ipv4_config_flds
    }
};

static int
bcm56880_a0_dna_4_7_11_ipmc_l2_tbl_db_get(int unit,
                                         bcmint_ipmc_l2_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    /* Bitmap of LTs that valid on bcm56880 DNA_4_7_11. */
    tbl_db->tbl_bmp = (1 << XFS_LTSW_IPMC_L2_TBL_IPV4) |
                      (1 << XFS_LTSW_IPMC_L2_TBL_IPV4_WIDE) |
                      (1 << XFS_LTSW_IPMC_L2_TBL_IPV4_SG) |
                      (1 << XFS_LTSW_IPMC_L2_TBL_IPV6) |
                      (1 << XFS_LTSW_IPMC_L2_TBL_IPV6_SG) |
                      (1 << XFS_LTSW_IPMC_L2_TBL_IPV4_CONFIG);

    tbl_db->tbls = bcm56880_a0_dna_4_7_11_ipmc_l2_tbls;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Ipmc driver function variable for bcm56880_a0 device.
 */
static mbcm_ltsw_ipmc_drv_t bcm56880_a0_dna_4_7_11_ltsw_ipmc_drv = {
    .ipmc_init = xfs_ltsw_ipmc_init,
    .ipmc_detach = xfs_ltsw_ipmc_detach,
    .ipmc_pub_info_get = xfs_ltsw_ipmc_pub_info_get,
    .ipmc_l2_add = xfs_ltsw_ipmc_l2_add,
    .ipmc_l2_remove = xfs_ltsw_ipmc_l2_remove,
    .ipmc_l2_find = xfs_ltsw_ipmc_l2_find,
    .ipmc_l2_traverse = xfs_ltsw_ipmc_l2_traverse,
    .ipmc_l2_delete_all = xfs_ltsw_ipmc_l2_delete_all,
    .ipmc_l2_flexctr_attach = xfs_ltsw_ipmc_l2_flexctr_attach,
    .ipmc_l2_flexctr_detach = xfs_ltsw_ipmc_l2_flexctr_detach,
    .ipmc_l2_flexctr_info_get = xfs_ltsw_ipmc_l2_flexctr_info_get,
    .ipmc_l2_flexctr_object_set = xfs_ltsw_ipmc_l2_flexctr_object_set,
    .ipmc_l2_flexctr_object_get = xfs_ltsw_ipmc_l2_flexctr_object_get,

    .ipmc_l2_tbl_db_get = bcm56880_a0_dna_4_7_11_ipmc_l2_tbl_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_dna_4_7_11_ltsw_ipmc_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ipmc_drv_set(unit, &bcm56880_a0_dna_4_7_11_ltsw_ipmc_drv));

exit:
    SHR_FUNC_EXIT();
}
