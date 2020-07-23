/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bondoptions.c
 * Purpose:     Populates Tomahawk2 Bond Options Info.
 * Requires:    SOC Common Bond Options Initializer.
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#ifdef BCM_TOMAHAWK2_SUPPORT

#include <shared/bitop.h>
#include <soc/iproc.h>
#include <soc/tomahawk2.h>
#include <soc/bondoptions.h>

/*
 * Function to conver to Core Clock frequency encodings to
 * real core clock frequencies.
 */
STATIC int
_soc_tomahawk2_bond_option_to_core_clk_freq(int unit, int val)
{
    int freq;

    switch (val) {
    case 0x0:
        freq = 1700;
        break;
    case 0x1:
        freq = 1675;
        break;
    case 0x2:
        freq = 1525;
        break;
    case 0x3:
        freq = 1425;
        break;
    case 0x4:
        freq = 1325;
        break;
    case 0x5:
        freq = 1275;
        break;
    case 0x6:
        freq = 1225;
        break;
    case 0x7:
        freq = 1125;
        break;
    case 0x8:
        freq = 1050;
        break;
    case 0x9:
        freq = 950;
        break;
    case 0xa:
        freq = 850;
        break;
    default:
        freq = 1700;
        break;
    }

    return freq;
}

/*
 * Function to conver to Dpp Clk Ratio encodings to
 * real dpp clk ratio
 */
STATIC int
_soc_tomahawk2_bond_option_to_dpp_clk_ratio(int unit, int val)
{
    int ratio;

    switch (val) {
    case 0x1:
        /* 3:2 */
        ratio = 15;
        break;
    case 0x2:
        /* 2:1 */
        ratio = 20;
        break;
    case 0x3:
        /* 1:1 */
        ratio = 10;
        break;
    default:
        /* 3:2 */
        ratio = 15;
        break;
    }

    return ratio;
}

/*
 * Function to conver to AVS encodings to
 * real voltages.
 * unit is 0.1 mV.
 */
STATIC int
_soc_tomahawk2_bond_option_to_avs_status(int unit, int val)
{
    int vol;

    switch (val) {
    case 0x62:
        vol = 10000;
        break;
    case 0x66:
        vol = 9750;
        break;
    case 0x6a:
        vol = 9500;
        break;
    case 0x6e:
        vol = 9250;
        break;
    case 0x72:
        vol = 9000;
        break;
    default:
        vol = 9500;
        break;
    }

    return vol;
}


/*
 * Function to initialize the bondoptions cache based on the
 * Bond Info read from the relevant registers.
 */
int
soc_tomahawk2_bond_info_init(int unit)
{
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    uint32 addr, val;
    int i, bit_val;

    soc_reg_t soc_th2_bond_info_regs[] = {
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
    int reg_num = COUNTOF(soc_th2_bond_info_regs);
    SHR_BITDCL bond_info_regval[_SHR_BITDCLSIZE(COUNTOF(soc_th2_bond_info_regs) * 32)] = {0};

    int feature_enables[][3] = {
        /* Bit */ /* Bool Invert? */ /* Feature Enum */ /* Desc */
        {276, 1, socBondInfoFeatureNoTimeSync    },  /* TS_1588_BC_ENABLE    */
        {277, 1, socBondInfoFeatureUc0Active     },  /* IPROC_CPU0_ENABLE    */
        {278, 1, socBondInfoFeatureUc1Active     },  /* IPROC_CPU1_ENABLE    */
        {279, 1, socBondInfoFeatureNoPipe_2_3    },  /* PIPE2_PIPE3_ENABLE   */
        {609, 1, socBondInfoFeatureNoCutThru     },  /* CUT_THRU_ENABLE      */
        {612, 1, socBondInfoFeatureNoExactMatch  },  /* UFT_ACL_MATCH_ENABLE */
        {617, 1, socBondInfoFeatureNoVxlan       },  /* VXLAN_ENABLE         */
        {618, 1, socBondInfoFeatureNoL3          },  /* L3_ENABLE            */
        {619, 1, socBondInfoFeatureNoEfp         },  /* EFP_ENABLE           */
        {620, 1, socBondInfoFeatureNoIfp         },  /* IFP_ENABLE           */
        {621, 1, socBondInfoFeatureNoVfp         },  /* VFP_ENABLE           */
        {622, 1, socBondInfoFeatureNoLpm         },  /* LPM_ENABLE           */
        {623, 1, socBondInfoFeatureNoUftBank0    },  /* UFT_BANK_0_ENABLE    */
        {624, 1, socBondInfoFeatureNoUftBank1    },  /* UFT_BANK_1_ENABLE    */
        {625, 1, socBondInfoFeatureNoUftBank2    },  /* UFT_BANK_2_ENABLE    */
        {626, 1, socBondInfoFeatureNoUftBank3    },  /* UFT_BANK_3_ENABLE    */
        {691, 1, socBondInfoFeatureNoMpls        },  /* MPLS_ENABLE          */
        {692, 1, socBondInfoFeatureNoMplsFrr     },  /* MPLS_FRR_ENABLE      */
        {693, 1, socBondInfoFeatureNoAlpm        },  /* L3_ALPM_ENABLE       */
        {694, 1, socBondInfoFeatureNoDynamicLoadBalancing},  /* DLB_ENABLE     */
        {695, 1, socBondInfoFeatureNoDynamicAlternatePath},  /* DYNAMIC_ALTERNATE_PATH_ENABLE */
        {696, 1, socBondInfoFeatureNoPstats},        /* PSTATS_ENABLE        */
        {697, 1, socBondInfoFeatureNoTCB},           /* TCB_ENABLE           */
        {698, 1, socBondInfoFeatureNoTimestamp   },  /* TIMESTAMP_ENABLE     */
        {-1,  0, -1                              }   /* Always last          */
    };

    for (i = 0; i < reg_num; i++) {
        addr = soc_reg_addr(unit, soc_th2_bond_info_regs[i], REG_PORT_ANY, 0);
        soc_iproc_getreg(unit, addr, &val);
        SHR_BITCOPY_RANGE(bond_info_regval, i * 32, &val, 0, 32);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->rev_id, uint16, bond_info_regval, 0, 7);
    SOC_BOND_OPTIONS_COPY(bond_info->dev_id, uint16, bond_info_regval, 8, 23);
    SOC_BOND_OPTIONS_COPY(bond_info->device_skew, uint32, bond_info_regval, 24, 27);
    SOC_BOND_OPTIONS_COPY(bond_info->mb_bank_num, uint32, bond_info_regval, 28, 31);

    /* Max Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 32, 35);
    bond_info->max_core_freq = _soc_tomahawk2_bond_option_to_core_clk_freq(unit, val);

    /* Default Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 36, 39);
    bond_info->default_core_freq = _soc_tomahawk2_bond_option_to_core_clk_freq(unit, val);

    /* Dpp Clk To Core Clk Ratio */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 40, 41);
    bond_info->dpp_clk_ratio = _soc_tomahawk2_bond_option_to_dpp_clk_ratio(unit, val);

    /* AVS Status */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 280, 287);
    bond_info->avs_status = _soc_tomahawk2_bond_option_to_avs_status(unit, val);

    /* TSC */
    /* OTP tsc_disabled may be impacted by TOP_TSC_ENABLE
    SHR_BITCOPY_RANGE(bond_info->tsc_disabled, 0, bond_info_regval, 288, 65);
    */
    SHR_BITCOPY_RANGE(bond_info->force_hg, 0, bond_info_regval, 353, 64);
    SHR_BITCOPY_RANGE(bond_info->tsc_max_speed, 0, bond_info_regval, 417, 128);
    SHR_BITCOPY_RANGE(bond_info->tsc_sr_only, 0, bond_info_regval, 545, 64);
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop, 0, bond_info_regval, 627, 64);

    /* IFP Slice Depth */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 610, 611);
    if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIfpSlice1k5);
    } else if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIfpSlice3k);
    } else if (val == 0x3) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIfpSlice6k);
    }

    /* LPM Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 613, 614);
    if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureLpm2k);
    } else if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureLpm8k);
    } else if (val == 0x3) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureLpm16k);
    }

    /* Next-hop Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 615, 616);
    if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureNextHop16k);
    } else if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureNextHop32k);
    } else if (val == 0x3) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureNextHop48k);
    }

    for (i = 0; i < (COUNTOF(feature_enables) - 1); i++) {
        bit_val = SHR_BITGET(bond_info_regval, feature_enables[i][0]);
        if (feature_enables[i][1]) {
            /* Boolean invert  */
            bit_val = !bit_val;
        }

        if (bit_val) {
            SOC_BOND_INFO_FEATURE_SET(unit, feature_enables[i][2]);
        }
    }


    if (bond_info->dev_id == 0xb972) {
        /* 56972 special setting */
        if (soc_property_get(unit, spn_BCM56972_RIOT_32X100, 0)) {
            /* 32x100G with Riot
             * Set additional 4 ports to loopback
             */
            SHR_BITSET(bond_info->tsc_in_loop, 1);
            SHR_BITSET(bond_info->tsc_in_loop, 17);
            SHR_BITSET(bond_info->tsc_in_loop, 47);
            SHR_BITSET(bond_info->tsc_in_loop, 63);
        } else {
            /* 36x100G */
            SHR_BITCOPY_RANGE(bond_info->tsc_disabled, 0,
                              bond_info->tsc_in_loop, 0, 64);
            SHR_BITCLR_RANGE(bond_info->tsc_in_loop, 0, 64);
        }
    }

    return SOC_E_NONE;
}

#endif /* BCM_TOMAHAWK2_SUPPORT */

