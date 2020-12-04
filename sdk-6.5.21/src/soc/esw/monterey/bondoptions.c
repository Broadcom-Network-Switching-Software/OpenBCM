/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bondoptions.c
 * Purpose:     Populates Apache Bond Options Info.
 * Requires:    SOC Common Bond Options Initializer.
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#ifdef BCM_APACHE_SUPPORT

#include <soc/apache.h>
#include <soc/bondoptions.h>

#include <shared/bitop.h>
#include <soc/iproc.h>

/*
 * Function to conver to Core Clock frequency encodings to
 * real core clock frequencies.
 */
STATIC int
_soc_monterey_bond_option_to_core_clk_freq(int unit, int val, int rev_id)
{
    int freq;

    switch (val) {
        case 0x0:
            freq = 862;
            break;
        case 0x1:
            if ((rev_id == BCM56670_B0_REV_ID) || (rev_id == BCM56670_C0_REV_ID) ) {
                freq = 817;
            } else {
                freq = 837;
            }
            break;
        case 0x2:
            if ((rev_id == BCM56670_B0_REV_ID) || (rev_id == BCM56670_C0_REV_ID)) {
                freq = 667;
            } else {
                freq = 817;
            }
            break;
        case 0x3:
            freq = 550;
            break;
        case 0x4:
            if ((rev_id == BCM56670_B0_REV_ID) || (rev_id == BCM56670_C0_REV_ID)) {
                freq = 450;
            } else {
                freq = 550;
            }
            break;
        default:
            freq = 862;
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
soc_monterey_bond_info_init(int unit)
{

    int i, bit_val;
    uint32 val, addr;
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    soc_reg_t soc_monterey_bond_info_regs[] = {
        USER_OTP_BONDOPTION_31_0_REGr,
        USER_OTP_BONDOPTION_63_32_REGr,
        USER_OTP_BONDOPTION_95_64_REGr,
        USER_OTP_BONDOPTION_127_96_REGr,
        USER_OTP_BONDOPTION_159_128_REGr,
        USER_OTP_BONDOPTION_191_160_REGr,
        USER_OTP_BONDOPTION_223_192_REGr,
        USER_OTP_BONDOPTION_255_224_REGr,
        USER_OTP_BONDOPTION_287_256_REGr,
        USER_OTP_BONDOPTION_319_288_REGr,
        USER_OTP_BONDOPTION_351_320_REGr,
        USER_OTP_BONDOPTION_383_352_REGr,
        USER_OTP_BONDOPTION_415_384_REGr,
        USER_OTP_BONDOPTION_447_416_REGr,
        USER_OTP_BONDOPTION_479_448_REGr,
        USER_OTP_BONDOPTION_511_480_REGr
    };

    int reg_num = COUNTOF(soc_monterey_bond_info_regs);
    SHR_BITDCL bond_info_regval[_SHR_BITDCLSIZE(COUNTOF(soc_monterey_bond_info_regs) * 32)] = {0};

    int feature_enables[][3] = {
        /* Bit */  /* Bool Invert?  */    /* Feature Enum */ /* Desc */
        {424, 1, socBondInfoFeatureNoL3            },  /* L3_ENABLE            */
        {425, 1, socBondInfoFeatureNoL3Tunnel      },  /* L3_TUNNEL_ENABLE     */
        {426, 1, socBondInfoFeatureNoVxlan         },  /* VXLAN_ENABLE         */
        {427, 1, socBondInfoFeatureNoMpls          },  /* MPLS_ENABLE          */
        {428, 1, socBondInfoFeatureNoMacsec        },  /* MACSEC_ENABLE        */
        {429, 1, socBondInfoFeatureNoReserveTunnel },  /* RESERVE_TUNNEL_ENABLE*/
        {430, 1, socBondInfoFeatureNoPreemption    },  /* PREEMPTION_ENABLE    */
        {433, 1, socBondInfoFeatureTs1588        },  /* TS_1588_BC_ENABLE    */
        {-1,  0, -1                                }   /* Always last          */
    };

    for (i = 0; i < reg_num; i++) {
        addr = soc_reg_addr(unit, soc_monterey_bond_info_regs[i], REG_PORT_ANY, 0);
        soc_iproc_getreg(unit, addr, &val);
        SHR_BITCOPY_RANGE(bond_info_regval, (i * 32), &val, 0, 32);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->rev_id, uint16, bond_info_regval, 0, 7);
    SOC_BOND_OPTIONS_COPY(bond_info->dev_id, uint16, bond_info_regval, 8, 23);
    SOC_BOND_OPTIONS_COPY(bond_info->device_skew, uint32, bond_info_regval, 24, 27);
    /* Max Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 34, 36);
    bond_info->max_core_freq = _soc_monterey_bond_option_to_core_clk_freq(unit, val,
                                                                  bond_info->rev_id);

    /* Default Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 37, 39);
    bond_info->default_core_freq = _soc_monterey_bond_option_to_core_clk_freq(unit, val,
                                                                    bond_info->rev_id);


    /* RTS MC Enable */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 431,432 );
    if (val & 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureUc0Active);
    }
    if (val & 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureUc1Active);
    }




    SOC_BOND_OPTIONS_COPY(bond_info->efp_slice_map, uint32, bond_info_regval,416 , 419);
    SOC_BOND_OPTIONS_COPY(bond_info->vfp_slice_map, uint32, bond_info_regval, 420, 423);
    SOC_BOND_OPTIONS_COPY(bond_info->clmac_map, uint32, bond_info_regval, 173,178);
    SOC_BOND_OPTIONS_COPY(bond_info->xlmac_map, uint32, bond_info_regval, 179, 188);
    SOC_BOND_OPTIONS_COPY(bond_info->tsc_force_hg, uint32, bond_info_regval, 157, 172);
    SOC_BOND_OPTIONS_COPY(bond_info->tsc_enable, uint32, bond_info_regval, 141, 156);
    SHR_BITCOPY_RANGE(bond_info->tsc_sr_only, 0, bond_info_regval,189 ,9 );
    SHR_BITCOPY_RANGE(bond_info->cpri_enable, 0, bond_info_regval,199 ,48 );

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

#endif /* BCM_MONTEREY_SUPPORT */

