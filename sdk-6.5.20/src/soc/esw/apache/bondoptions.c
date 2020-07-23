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
_soc_apache_bond_option_to_core_clk_freq(int unit, int val)
{
    int freq;

    switch (val) {
        case 0x0:
            freq = 841;
            break;
        case 0x1:
            freq = 793;
            break;
        case 0x2:
            freq = 575;
            break;
        case 0x3:
            freq = 510;
            break;
        case 0x4:
            freq = 435;
            break;
        default:
            freq = 793;
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
soc_apache_bond_info_init(int unit)
{

    int i, bit_val;
    uint32 val, addr;
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    soc_reg_t soc_apache_bond_info_regs[] = {
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

    int reg_num = COUNTOF(soc_apache_bond_info_regs);
    SHR_BITDCL bond_info_regval[_SHR_BITDCLSIZE(COUNTOF(soc_apache_bond_info_regs) * 32)] = {0};

    int feature_enables[][3] = {
        /* Bit */  /* Bool Invert?  */    /* Feature Enum */ /* Desc */
        {154, 1, socBondInfoFeatureEfp512HalfSlice },  /* EFP_SLICE_DEPTH      */
        {159, 1, socBondInfoFeatureVfp512HalfSlice },  /* VFP_SLICE_DEPTH      */
        {164, 1, socBondInfoFeatureIfpSliceHalf    },  /* FP_DEPTH             */
        {177, 1, socBondInfoFeatureNoL3            },  /* L3_ENABLE            */
        {178, 1, socBondInfoFeatureNoAlpm          },  /* ALPM_ENABLE          */
        {179, 1, socBondInfoFeatureNoL3Tunnel      },  /* L3_TUNNEL_ENABLE     */
        {180, 1, socBondInfoFeatureNoStaticNat     },  /* STATIC_NAT_ENABLE    */
        {181, 1, socBondInfoFeatureNoVxlan         },  /* VXLAN_ENABLE         */
        {182, 1, socBondInfoFeatureNoL2Gre         },  /* L2_GRE_ENABLE        */
        {183, 1, socBondInfoFeatureNoFcoe          },  /* FCOE_ENABLE          */
        {190, 1, socBondInfoFeatureNoOsubCT        },  /* OVERSUB_CT_ENABLE    */
        {191, 1, socBondInfoFeatureNoTimeSync      },  /* TS_1588_BC_ENABLE    */
        {256, 0, socBondInfoFeatureSramPwrDwn      },  /* SRAM_PWRDWN          */
        {351, 0, socBondInfoFeatureNoCutThru       },  /* MMU_DISABLE_CT       */
        {354, 1, socBondInfoFeatureNoRiot          },  /* RIOT_ENABLE          */
        {355, 1, socBondInfoFeatureNoTrill         },  /* TRILL_ENABLE         */
        {356, 1, socBondInfoFeatureNoMim           },  /* MIM_ENABLE           */
        {357, 1, socBondInfoFeatureNoEcmp          },  /* ECMP_ENABLE          */
        {359, 1, socBondInfoFeatureNoMpls          },  /* MPLS_ENABLE          */
        {372, 1, socBondInfoFeatureNoHgProxyCoe    },  /* COE_ENABLE           */
        {372, 1, socBondInfoFeatureNoCoeVlanPause  },  /* COE_VLAN_PAUSE_ENABLE*/
        {373, 1, socBondInfoFeatureNoCoeSubtag8100 },  /* SUBPORT_8100_ENABLE  */
        {374, 1, socBondInfoFeatureNoFpOam         },  /* OAM_ENABLE           */
        {377, 1, socBondInfoFeatureNoEtherTm       },  /* ETM_ENABLE           */

        {215, 0, socBondInfoFeatureNoServiceMeter  },  /* SVM                  */
        {216, 0, socBondInfoFeatureNoOamTrueUpMep  },  /* OAM TRUE UPMEP       */
        {218, 0, socBondInfoFeatureNoEpRedirectV2  },  /* EP REDIRECT V2.0     */
        {219, 0, socBondInfoFeatureNoMultiLevelProt},  /* MULTI LEVEL PROT     */
        {220, 0, socBondInfoFeatureNoFpSat         },  /* FP-SAT (SAT Hooks)   */
        {221, 0, socBondInfoFeatureNoSegmentRouting},  /* Segment Routing      */
        {222, 0, socBondInfoFeatureNoCAUI10Support },  /* CAUI10 Support       */
        {223, 0, socBondInfoFeatureHqos3Levels     },  /* HQOS 3 levels Support*/
        {224, 0, socBondInfoFeatureNoQCN           },  /* QCN                  */
        {225, 0, socBondInfoFeatureNoECN           },  /* ECN                  */
        {226, 0, socBondInfoFeatureNoDMVoq         },  /* DMVOQ                */
        {227, 0, socBondInfoFeatureNoDPVoq         },  /* DPVOQ                */

        {-1,  0, -1                                }   /* Always last          */
    };

    for (i = 0; i < reg_num; i++) {
        addr = soc_reg_addr(unit, soc_apache_bond_info_regs[i], REG_PORT_ANY, 0);
        soc_iproc_getreg(unit, addr, &val);
        SHR_BITCOPY_RANGE(bond_info_regval, (i * 32), &val, 0, 32);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->rev_id, uint16, bond_info_regval, 0, 7);
    SOC_BOND_OPTIONS_COPY(bond_info->dev_id, uint16, bond_info_regval, 8, 23);
    SOC_BOND_OPTIONS_COPY(bond_info->device_skew, uint32, bond_info_regval, 24, 27);

    /* Max Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 33, 35);
    bond_info->max_core_freq = _soc_apache_bond_option_to_core_clk_freq(unit, val);

    /* Default Core Clock Frequency */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 36, 38);
    bond_info->default_core_freq = _soc_apache_bond_option_to_core_clk_freq(unit, val);

    /* SPI Code Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 297, 298);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureSpiCodeSize32KB);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureSpiCodeSize64KB);
    } else if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureSpiCodeSize128KB);
    } else {
        /* unknown spi code size */
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureSpiCodeSize64KB);
    }

    /* MMU Buffers */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 31, 32);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureMmu12Mb);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureMmu9Mb);
    } else if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureMmu6Mb);
    } else {
        /* unknown mmu size */
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureMmu12Mb);
    }

    /* MMU IPMC Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 352, 353);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpmcDisabled);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpmc8k);
    } else if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpmc4k);
    } else {
        /* unknown mmu size */
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpmc8k);
    }

    /* VRF Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 360, 361);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureVrf1k);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureVrf2k);
    } else if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureVrf4k);
    } else {
        /* unknown vrf size */
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureVrf4k);
    }


    /* L3_IIF Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 362, 362);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3Iif8k);
    } else {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3Iif12k);
    }


    /* UFT Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 363, 363);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureUft128k);
    } else {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureUft256k);
    }

    /* RTS MC Enable */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 375, 376);
    if (val & 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureUc0Active);
    }
    if (val & 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureUc1Active);
    }

    /* PKG_TYPE */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 396, 397);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureApachePkg42p5mm);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureApachePkg50mmTDpCompat);
    } else if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureApachePkg50mmTD2pCompat);
    } else {
        /* unknown package type */
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureApachePkg42p5mm);
    }




    /* ALPM (2) */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 217, 217);
    if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_CLEAR(unit, socBondInfoFeatureNoAlpm);
    }

    /* L3_EIF Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 231, 232);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3Intf12k);
    } else {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3Intf8k);
    }

    /* L3_NEXTHOP Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 233, 234);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3NHop32k);
    } else {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3NHop8k);
    }

    /* L3_VRF Size (2) */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 235, 236);
    if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureVrf16);
    }

    /* L3_HOST */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 237, 238);
    if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3HostMax4k);
    }

    /* L2_IF (SVP) */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 239, 240);
    if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureSVP4k);
    }

    /* L2_MC */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 241, 242);
    if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2mc4k);
    }

    /* COS Profile */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 243, 244);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureApacheCoSProfile0);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureApacheCoSProfile1);
    } else if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureApacheCoSProfile2);
    }

    /* Family */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 245, 246);
    if (val == 0x0) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureFamilyMaverick);
    } else if (val == 0x1) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureFamilyFirebolt5);
    } else if (val == 0x2) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureFamilyApache);
    } else if (val == 0x3) {
        SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureFamilyRanger2Plus);
    }




    SOC_BOND_OPTIONS_COPY(bond_info->ifp_slice_map, uint32, bond_info_regval, 165, 176);
    SOC_BOND_OPTIONS_COPY(bond_info->efp_slice_map, uint32, bond_info_regval, 155, 158);
    SOC_BOND_OPTIONS_COPY(bond_info->vfp_slice_map, uint32, bond_info_regval, 160, 163);
    SOC_BOND_OPTIONS_COPY(bond_info->clmac_map, uint32, bond_info_regval, 184, 189);
    SOC_BOND_OPTIONS_COPY(bond_info->xlmac_map, uint32, bond_info_regval, 275, 292);
    SOC_BOND_OPTIONS_COPY(bond_info->tsce_max_2p5g, uint32, bond_info_regval, 378, 391);
    SOC_BOND_OPTIONS_COPY(bond_info->tscf_max_11g, uint32, bond_info_regval, 392, 395);
    SOC_BOND_OPTIONS_COPY(bond_info->tsc_force_hg, uint32, bond_info_regval, 192, 209);
    SOC_BOND_OPTIONS_COPY(bond_info->tsc_enable, uint32, bond_info_regval, 257, 274);
    SOC_BOND_OPTIONS_COPY(bond_info->defip_cam_map, uint32, bond_info_regval, 364, 371);

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

#endif /* BCM_APACHE_SUPPORT */

