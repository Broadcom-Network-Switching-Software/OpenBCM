/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bondoptions.c
 * Purpose:     Populates Firebolt6 Bond Options Info.
 * Requires:    SOC Common Bond Options Initializer.
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#ifdef BCM_FIREBOLT6_SUPPORT

#include <shared/bitop.h>
#include <soc/iproc.h>
#include <soc/firebolt6.h>
#include <soc/bondoptions.h>

/* Byte swap */
#define BSWAP(value) ((value & 0xff000000) >> 24 | (value & 0x00ff0000) >> 8 | \
                      (value & 0x0000ff00) << 8 | (value & 0x000000ff) << 24)

/*
 * Function to conver to AVS encodings to
 * real voltages.
 * unit is 0.1 mV.
 */
STATIC int
_soc_fb6_bond_option_to_avs_status(int unit, int val)
{
    int vol = 0;

    switch (val) {
        case 0x76:
            vol = 8750;
            break;
        case 0x7A:
            vol = 8500;
            break;
        case 0x7E:
            vol = 8250;
            break;
        case 0x82:
            vol = 8000;
            break;
    }

    return vol;
}

/*
 * Function to conver to Core Clock frequency encodings to
 * real core clock frequencies.
 */
STATIC int
_soc_firebolt6_bond_option_to_core_clk_freq(int unit, int val)
{
    int freq;

    switch (val) {
        case 0x0:
            freq = 1250;
            break;
        case 0x1:
            freq = 1118;
            break;
        case 0x2:
            freq = 1087;
            break;
        case 0x3:
            freq = 1012;
            break;
        case 0x4:
            freq = 1012;
            break;
        default:
            freq = 1250;
            /* unknown max frequency */
            break;
    }

    return freq;
}

/*
 * Function to conver to AVS encodings to
 * real voltages.
 * unit is 0.1 mV.
 */
STATIC int
_soc_firebolt6_bond_option_to_avs_status(int unit, int val)
{
    int vol;

    switch (val) {
        case 0x76:
            vol = 8750;
            break;
        case 0x7a:
            vol = 8500;
            break;
        case 0x7e:
            vol = 8250;
            break;
        default:
            vol = 8750;
            break;
    }

    return vol;
}

/*
 * Function to initialize the bondoptions cache based on the
 * Bond Info read from the relevant registers.
 */
int
soc_firebolt6_bond_info_init(int unit)
{
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    uint32 addr, val;
    int i, bit_val;
    uint32 mask, max_pm_speed, pm_speed_data,  pm_data[2] = {0};
    int shift_bits = 0;

    soc_reg_t soc_fb6_bond_info_regs[] = {
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
    int reg_num = COUNTOF(soc_fb6_bond_info_regs);
    SHR_BITDCL bond_info_regval[_SHR_BITDCLSIZE(COUNTOF(soc_fb6_bond_info_regs) * 32)] = {0};

    int feature_enables[][3] = {
        /* Bit | Bool Invert? | Feature Enum | Description */
        {436, 1, socBondInfoBskGroupDepthHalfEntries}, /* BROADSCAN2.0 Group Depth */
        {437, 0, socBondInfoBskDisable}, /* BROADSCAN Disable */
        {438, 0, socBondInfoBskNoPktSampling}, /* BROADSCAN packet sampling disable */
        {439, 0, socBondInfoBskNoHWExportPktBuild},  /* BROADSCAN HW export packet build disable */
        {440, 0, socBondInfoBskNoTCPBidir},  /* BROADSCAN TCP bidir processing disable */
        {441, 0, socBondInfoBskNoDoSAttackVector},  /* BROADSCAN DOS attack vector check disable */
        {442, 0, socBondInfoBskNoMicroFlowPolicer}, /* BROADSCAN micro flow policer disable */
        {443, 0, socBondInfoBskNoALU32Inst2to11}, /* BROADSCAN ALU32 inst 2_to_11 disable */
        {444, 0, socBondInfoBskNoDropAnalysis},    /* BROADSCAN2.0 Drop reason tracking disable */
        {445, 0, socBondInfoBskNoLatencyTracking}, /* BROADSCAN2.0 E2E and Chip Delay tracking disable */
        {446, 0, socBondInfoBskNoAluLatencySupport}, /* BROADSCAN2.0 ALU Delay calculation disable */
        {447, 0, socBondInfoBskNoIATIDTTracking}, /* BROADSCAN2.0 IAT & IDT Delay tracking disable */
        {448, 0, socBondInfoBskNoCongestionTracking}, /* BROADSCAN2.0 MMU congestion level delay trakcing disabled */
        {449, 0, socBondInfoBskNoLoadBalancingTracking}, /* BROADSCAN2.0 Load Balancing tracking disable */
        {450, 0, socBondInfoBskAggregateDisable}, /* BROADSCAN Aggregate tracking disable */
        {422, 1, socBondInfoFeatureNoL3Tunnel},  /* L3_TUNNEL_ENABLE */
        {423, 1, socBondInfoFeatureNoRiot},  /* RIOT_ENABLE */
        {424, 1, socBondInfoFeatureNoL3},  /* L3_ENABLE */
        {425, 1, socBondInfoFeatureEfp512HalfSlice },  /* EFP_SLICE_DEPTH */
        {426, 1, socBondInfoFeatureVfp512HalfSlice },  /* VFP_SLICE_DEPTH */
        {427, 1, socBondInfoFeatureNoMpls},  /* MPLS_ENABLE */
        {432, 1, socBondInfoFeatureNoPstats},     /* PSTATS_ENABLE */
        {433, 1, socBondInfoFeatureNoTimestamp},  /* TIMESTAMP_ENABLE */
        {467, 1, socBondInfoFeatureNoAlpm},  /* L3_ALPM_ENABLE */
        {468, 0, socBondInfoChannelizedSwitchingDisable}, /* Channelized switching disable */
        {-1,  0, -1 }                              /* Always last */
    };

    for (i = 0; i < reg_num; i++) {
        addr = soc_reg_addr(unit, soc_fb6_bond_info_regs[i], REG_PORT_ANY, 0);
        soc_iproc_getreg(unit, addr, &val);
        SHR_BITCOPY_RANGE(bond_info_regval, i * 32, &val, 0, 32);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->rev_id, uint16, bond_info_regval, 0, 7);
    SOC_BOND_OPTIONS_COPY(bond_info->dev_id, uint16, bond_info_regval, 8, 23);
    SOC_BOND_OPTIONS_COPY(bond_info->device_skew, uint32, bond_info_regval, 24, 27);

    /* Max Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 28, 31);
    bond_info->max_core_freq = _soc_firebolt6_bond_option_to_core_clk_freq(unit, val);

    /* Default Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 32, 35);
    bond_info->default_core_freq = _soc_firebolt6_bond_option_to_core_clk_freq(unit, val);

    /* AVS status */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 327, 334);
    bond_info->avs_status = _soc_firebolt6_bond_option_to_avs_status(unit, val);

    /* TSC Diable */
    SOC_BOND_OPTIONS_COPY(bond_info->tsc_enable, uint32, bond_info_regval, 335, 351);

    /* Force HG */
    SOC_BOND_OPTIONS_COPY(bond_info->tsc_force_hg, uint32, bond_info_regval, 352, 368);

    /* UFT Size */
#if 0
    
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 420, 421);
    if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureUft128k);
    } else if (val == 0x3) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureUft256k);
    }
#endif

    /* UTT Banks */
    SOC_BOND_OPTIONS_COPY(bond_info->utt_banks_map, uint32, bond_info_regval, 469, 484);

    /* BROADSCAN Session table depth */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 434, 435);
    if (val == 1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoBskSessionDepth3Banks);
    } else if (val == 2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoBskSessionDepth5Banks);
    }

    /* Next Hop Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 485, 485);
    if (val == 0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureNextHop64k);
    }

    /* IPMC Groups */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 428, 429);
    if (val == 0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpmcDisabled);
    } else if (val == 1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpmc8k);
    } else if (val == 2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpmc16k);
    }

    /* ICID */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 490, 521);
    bond_info->ic_id[0] = BSWAP(val);
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 522, 553);
    bond_info->ic_id[1] = BSWAP(val);

    /* PM4X25  speed limit */
    SOC_BOND_OPTIONS_COPY(pm_data[0], uint32, bond_info_regval, 369, 400);
    SOC_BOND_OPTIONS_COPY(pm_data[1], uint32, bond_info_regval, 401, 402);

    mask = 0x3;
    pm_speed_data = pm_data[0];
    for (i=0; i<FIREBOLT6_TDM_PBLKS_PER_DEV; i++) {

        /* First 2*16=32bits in one register and 17th in other. */
        if (i == 16) {
            pm_speed_data = pm_data[1];
            mask = 0x3;
            shift_bits = 0;
        }

        switch (((pm_speed_data & mask) >> shift_bits)) {
            case 1:
                max_pm_speed = 20;
            break;
            case 2:
                max_pm_speed = 10;
            break;

            case 0:
            case 3:
            default:
                max_pm_speed = -1;
            break;
        }
        /* Print speed limit for each PM */
        LOG_INFO(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 " PM (%d) is in max speed of %dG \n"), i, max_pm_speed));

        bcmi_otp_pm_max_speed[unit][i] = max_pm_speed;
        /* shift by 2 bits */
        mask = (mask << 2);
        shift_bits += 2;
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

    /* AVS Status */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 327, 334);
    bond_info->avs_status = _soc_fb6_bond_option_to_avs_status(unit, val);

    return SOC_E_NONE;
}

#endif /* BCM_FIREBOLT6_SUPPORT */

