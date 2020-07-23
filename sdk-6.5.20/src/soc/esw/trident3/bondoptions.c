/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bondoptions.c
 * Purpose:     Populates Trident3 Bond Options Info.
 * Requires:    SOC Common Bond Options Initializer.
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#ifdef BCM_TRIDENT3_SUPPORT

#include <shared/bitop.h>
#include <soc/iproc.h>
#include <soc/trident3.h>
#include <soc/bondoptions.h>

/*
 * Function to conver to AVS encodings to
 * real voltages.
 * unit is 0.1 mV.
 */
STATIC int
_soc_td3_bond_option_to_avs_status(int unit, int val)
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
soc_trident3_bond_info_init(int unit)
{
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    uint32 addr, val;
    int i;

    soc_reg_t soc_td3_bond_info_regs[] = {
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
    int reg_num = COUNTOF(soc_td3_bond_info_regs);
    SHR_BITDCL bond_info_regval[_SHR_BITDCLSIZE(COUNTOF(soc_td3_bond_info_regs) * 32)] = {0};

    for (i = 0; i < reg_num; i++) {
        addr = soc_reg_addr(unit, soc_td3_bond_info_regs[i], REG_PORT_ANY, 0);
        soc_iproc_getreg(unit, addr, &val);
        SHR_BITCOPY_RANGE(bond_info_regval, i * 32, &val, 0, 32);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->rev_id, uint16, bond_info_regval, 0, 7);
    SOC_BOND_OPTIONS_COPY(bond_info->dev_id, uint16, bond_info_regval, 8, 23);
    SOC_BOND_OPTIONS_COPY(bond_info->device_skew, uint32, bond_info_regval, 24, 27);
    SOC_BOND_OPTIONS_COPY(bond_info->mb_bank_num, uint32, bond_info_regval, 28, 33);

    /* AVS Status */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 255, 262);
    bond_info->avs_status = _soc_td3_bond_option_to_avs_status(unit, val);

    return SOC_E_NONE;
}

#endif /* BCM_TRIDENT3_SUPPORT */

