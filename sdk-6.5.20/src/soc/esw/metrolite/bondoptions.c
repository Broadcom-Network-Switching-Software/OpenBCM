/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bondoptions.c
 * Purpose:     Populates Metrolite Bond Options Info.
 * Requires:    SOC Common Bond Options Initializer.
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#ifdef BCM_METROLITE_SUPPORT

#include <shared/bitop.h>
#include <soc/iproc.h>

#include <soc/metrolite.h>
#include <soc/bondoptions.h>


/*
 * Function to initialize the bondoptions cache based on the
 * Bond Info read from the relevant registers.
 */
int
soc_metrolite_bond_info_init(int unit)
{

    int i, bit_val;
    uint32 val, addr;
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    soc_reg_t soc_metrolite_bond_info_regs[] = {
        DMU_PCU_OTP_CONFIG_0r,
        DMU_PCU_OTP_CONFIG_1r,
        DMU_PCU_OTP_CONFIG_2r,
        DMU_PCU_OTP_CONFIG_3r,
        DMU_PCU_OTP_CONFIG_4r,
        DMU_PCU_OTP_CONFIG_5r,
        DMU_PCU_OTP_CONFIG_6r,
        DMU_PCU_OTP_CONFIG_7r,
        DMU_PCU_OTP_CONFIG_8r,
        DMU_PCU_OTP_CONFIG_9r,
        DMU_PCU_OTP_CONFIG_10r,
        DMU_PCU_OTP_CONFIG_11r,
        DMU_PCU_OTP_CONFIG_12r,
        DMU_PCU_OTP_CONFIG_13r,
        DMU_PCU_OTP_CONFIG_14r,
        DMU_PCU_OTP_CONFIG_15r
    };
    int reg_num = COUNTOF(soc_metrolite_bond_info_regs);
    SHR_BITDCL bond_info_regval[_SHR_BITDCLSIZE(COUNTOF(soc_metrolite_bond_info_regs) * 32)] = {0};

    int feature_enables[][3] = {
        /* Bit */  /* Bool Invert?  */    /* Feature Enum */ /* Desc */
        {25, 1, socBondInfoFeatureNoL3          },  /* L3_ENABLE               */
        {26, 1, socBondInfoFeatureNoSat         },  /* SAT_ENABLE              */
        {27, 1, socBondInfoFeatureNoFpRouting   },  /* FP_ROUTE_ENABLE         */
        {28, 1, socBondInfoFeatureNoServiceMeter},  /* SVM_ENABLE              */
        {30, 1, socBondInfoFeatureNoMpls        },  /* MPLS_ENABLE             */
        {34, 1, socBondInfoFeatureNoMim         },  /* MIM_ENABLE              */
        {35, 1, socBondInfoFeatureNoOam         },  /* OAM_ENABLE              */
        {37, 1, socBondInfoFeatureNoSubTagCoe   },  /* SUBPORT_TAG_ENABLE      */
        {38, 1, socBondInfoFeatureNoLinkPhy     },  /* LINKPHY_ENABLE          */
        {127, 1, socBondInfoFeatureNoPTP        },  /* NTS_1588_ENABLE         */
        {141, 1, socBondInfoFeatureNoAvs        },  /* AVS_DISABLE             */

        {-1,  0, -1                              }  /* Always last             */
    };

    for (i = 0; i < reg_num; i++) {
        addr = soc_reg_addr(unit, soc_metrolite_bond_info_regs[i], REG_PORT_ANY, 0);
        soc_iproc_getreg(unit, addr, &val);
        SHR_BITCOPY_RANGE(bond_info_regval, (i * 32), &val, 0, 32);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->rev_id, uint16, bond_info_regval, 0, 7);
    SOC_BOND_OPTIONS_COPY(bond_info->dev_id, uint16, bond_info_regval, 8, 23);

    /* MMU Buffers */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 62, 62);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureMmu2Mb);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureMmu1Mb);
    } else {
        /* unknown mmu size */
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureMmu2Mb);
    }

    /* L2 ENTRY Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 24, 24);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2Entry8k);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2Entry32k);
    } else {
        /* unknown size */
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2Entry32k);
    }

    /* IP VLAN_XLATE Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 33, 33);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpVlanXlate1k);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpVlanXlate4k);
    } else {
        /* unknown size */
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpVlanXlate4k);
    }

    /* IP MPLS Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 29, 29);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureMpls1k);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureMpls2k);
    } else {
        /* unknown size */
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureMpls2k);
    }

    /* L3_ENTRY size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 31, 31);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3Entry1k);
    }

    /* L3_NEXT_HOP size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 32, 32);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3NextHop1k);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3NextHop2k);
    } else {
        /* unknown size */
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3NextHop2k);
    }

    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 45, 48);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3DefIpDisable);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->ifp_slice_map, uint32, bond_info_regval, 53, 60);
    SOC_BOND_OPTIONS_COPY(bond_info->efp_slice_map, uint32, bond_info_regval, 41, 44);
    SOC_BOND_OPTIONS_COPY(bond_info->vfp_slice_map, uint32, bond_info_regval, 49, 52);

    for(i = 0; i < COUNTOF(feature_enables); i++) {
        bit_val = SHR_BITGET(bond_info_regval, feature_enables[i][0]);
        if (feature_enables[i][1]) {
            /* Boolean invert  */
            bit_val = !bit_val;
        }

        if (bit_val) {
            SOC_BOND_INFO_FEATURE_SET(unit, feature_enables[i][2]);
        }
    }

    return SOC_E_NONE;
}

#endif /* BCM_METROLITE_SUPPORT */

