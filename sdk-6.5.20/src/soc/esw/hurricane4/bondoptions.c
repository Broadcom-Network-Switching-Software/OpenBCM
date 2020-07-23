/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bondoptions.c
 * Purpose:     Populates Hurricane4 Bond Options Info.
 * Requires:    SOC Common Bond Options Initializer.
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#ifdef BCM_HURRICANE4_SUPPORT

#include <shared/bitop.h>
#include <soc/iproc.h>
#include <soc/hurricane4.h>
#include <soc/bondoptions.h>


/*
 * Function to conver to Core Clock frequency encodings to
 * real core clock frequencies.
 */
STATIC int
_soc_hurricane4_bond_option_to_core_clk_freq(int unit, int val)
{
    int freq;

    switch (val) {
        case 0x0:
            freq = 496;
            break;
        case 0x1:
            freq = 445;
            break;
        case 0x2:
            freq = 415;
            break;
        case 0x3:
            freq = 325;
            break;
        case 0x4:
            freq = 266;
            break;
        case 0x5:
            freq = 200;
            break;
        case 0x6:
            freq = 135;
            break;
        case 0x7:
            freq = 100;
            break;

        default:
            freq = 496;
            /* unknown max frequency */
            break;
    }

    return freq;
}

/*
 * Function to initialize the bondoptions cache based on the
 * Bond Info read from the relevant registers.
 */
int
soc_hurricane4_bond_info_init(int unit)
{
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    uint32 addr, val;
    int i, bit_val;

    soc_reg_t soc_hr4_bond_info_regs[] = {
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
        DMU_PCU_OTP_CONFIG_15r,
        DMU_PCU_OTP_CONFIG_16r,
        DMU_PCU_OTP_CONFIG_17r,
        DMU_PCU_OTP_CONFIG_18r,
        DMU_PCU_OTP_CONFIG_19r,
        DMU_PCU_OTP_CONFIG_20r,
        DMU_PCU_OTP_CONFIG_21r,
        DMU_PCU_OTP_CONFIG_22r,
        DMU_PCU_OTP_CONFIG_23r,
        DMU_PCU_OTP_CONFIG_24r,
        DMU_PCU_OTP_CONFIG_25r,
        DMU_PCU_OTP_CONFIG_26r,
        DMU_PCU_OTP_CONFIG_27r,
        DMU_PCU_OTP_CONFIG_28r,
        DMU_PCU_OTP_CONFIG_29r,
        DMU_PCU_OTP_CONFIG_30r,
        DMU_PCU_OTP_CONFIG_31r
    };
    int reg_num = COUNTOF(soc_hr4_bond_info_regs);
    SHR_BITDCL bond_info_regval[_SHR_BITDCLSIZE(COUNTOF(soc_hr4_bond_info_regs) * 32)] = {0};

    int feature_enables[][3] = {
        /* Bit */  /* Bool Invert?  */    /* Feature Enum */ /* Desc */
        {389, 0, socBondInfoBskDisable}, /* BROADSCAN disable */
        {390, 0, socBondInfoBskNoPktSampling},  /* BROADSCAN packet sampling disable */
        {391, 0, socBondInfoBskNoHWExportPktBuild},  /* BROADSCAN HW export packet build disable */
        {392, 0, socBondInfoBskNoTCPBidir},  /* BROADSCAN TCP bidir processing disable */
        {393, 0, socBondInfoBskNoDoSAttackVector},  /* BROADSCAN DOS attack vector check disable */
        {394, 0, socBondInfoBskNoMicroFlowPolicer}, /* BROADSCAN micro flow policer disable */
        {395, 0, socBondInfoBskNoALU32Inst2to11},  /* BROADSCAN ALU32 inst 2_to_11 disable */
        {396, 0, socBondInfoBskNoDropAnalysis},    /* BROADSCAN2.0 Drop reason tracking disable */
        {397, 0, socBondInfoBskNoLatencyTracking}, /* BROADSCAN2.0 E2E and Chip Delay tracking disable */
        {398, 0, socBondInfoBskNoAluLatencySupport}, /* BROADSCAN2.0 ALU Delay calculation disable */
        {399, 0, socBondInfoBskNoIATIDTTracking}, /* BROADSCAN2.0 IAT & IDT Delay tracking disable */
        {400, 0, socBondInfoBskNoCongestionTracking}, /* BROADSCAN2.0 MMU congestion level delay trakcing disabled */
        {401, 0, socBondInfoBskNoLoadBalancingTracking}, /* BROADSCAN2.0 Load Balancing tracking disable */
        {313, 0, socBondInfoPM4x10QInst0Disable},
        {314, 0, socBondInfoPM4x10QInst1Disable},
        {347, 1, socBondInfoPM4x10QInst0QsgmiiDisable},
        {348, 1, socBondInfoPM4x10QInst1QsgmiiDisable},
        {349, 1, socBondInfoPM4x10QInst0UsxgmiiDisable},
        {350, 1, socBondInfoPM4x10QInst1UsxgmiiDisable},
        {418, 0, socBondInfoFeatureNoMacsec}, /* MACSEC disable */
        {-1,  0, -1 }                              /* Always last */
    };


    for (i = 0; i < reg_num; i++) {
        addr = soc_reg_addr(unit, soc_hr4_bond_info_regs[i], REG_PORT_ANY, 0);
        soc_iproc_getreg(unit, addr, &val);
        SHR_BITCOPY_RANGE(bond_info_regval, i * 32, &val, 0, 32);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->rev_id, uint16, bond_info_regval, 0, 7);
    SOC_BOND_OPTIONS_COPY(bond_info->dev_id, uint16, bond_info_regval, 8, 23);
    SOC_BOND_OPTIONS_COPY(bond_info->device_skew, uint32, bond_info_regval, 24, 27);
    SOC_BOND_OPTIONS_COPY(bond_info->mb_bank_num, uint32, bond_info_regval, 28, 33);

    /* Max Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 28, 31);
    bond_info->max_core_freq = _soc_hurricane4_bond_option_to_core_clk_freq(unit, val);

    /* Default Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 32, 35);
    bond_info->default_core_freq = _soc_hurricane4_bond_option_to_core_clk_freq(unit, val);

    /* BROADSCAN Session table depth */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 386, 387);
    if (val == 1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoBskSessionDepth3Banks);
    } else if (val == 2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoBskSessionDepth5Banks);
    } else if (val == 3) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoBskSessionDepth9Banks);
    }

    /* BROADSCAN group table depth */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 388, 388);
    if (val == 0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoBskGroupDepthHalfEntries);
    }

    /* Set the feature enable status */
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
#endif /* BCM_HURRICANE4_SUPPORT */
