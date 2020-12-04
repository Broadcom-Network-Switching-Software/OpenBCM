/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>

#include <soc/defs.h>

#ifdef BCM_FIRELIGHT_SUPPORT
#include <soc/firelight.h>
#include <soc/bondoptions.h>
#include <shared/bitop.h>
#include <soc/iproc.h>

/**********************/
/*
 * Function to initialize the bondoptions cache based on the
 * Bond Info read from the relevant registers.
 */
int
soc_firelight_bond_info_init(int unit)
{

    int i, bit_val;
    uint32 val, addr;
#if 0
    int pport_offset;
#endif
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    const soc_reg_t soc_fl_bond_info_regs[] = {
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

    int reg_num = COUNTOF(soc_fl_bond_info_regs);
    SHR_BITDCL bond_info_regval[
                   _SHR_BITDCLSIZE(COUNTOF(soc_fl_bond_info_regs) * 32)] = {0};

    int feature_enables[][3] = {
        /* Bit */  /* Bool Invert?  */    /* Feature Enum */ /* Desc */
        {389, 0, socBondInfoFeatureEfp64HalfSlice  },  /* EFP_SLICE_DEPTH  */
        {387, 0, socBondInfoFeatureVfp64HalfSlice  },  /* VFP_SLICE_DEPTH  */
        {128, 1, socBondInfoFeatureNoL3            },  /* L3_ENABLE        */
        {352, 1, socBondInfoFeatureNoVxlanLite     },  /* VXLAN_LITE_ENABLE */
        {155, 0, socBondInfoFeatureNoTimeSync      },  /* TS_1588_DISABLE  */
        {147, 1, socBondInfoFeatureNoFpOam         },  /* OAM_ENABLE       */
        {152, 1, socBondInfoFeatureNoFpRouting     },  /* FP_ROUTE_ENABLE  */
        {353, 1, socBondInfoFeatureNoRRCOE         },  /* RROCE_ENABLE     */
        {355, 1, socBondInfoFeatureNoTAS           },  /* TAS_ENABLE        */
        {356, 1, socBondInfoFeatureNoHSR           },  /* HSR_ENABLE       */
        {357, 1, socBondInfoFeatureNoPRP           },  /* PRP_ENABLE       */
        {358, 1, socBondInfoFeatureNo8021CB        },  /* IEEE_802_1CB_ENABLE */
        {215, 1, socBondInfoFeatureNoMacsec        },  /* MACSEC_ENABLE */
        {-1,  0, -1                                }   /* Always last         */
    };

    for (i = 0; i < reg_num; i++) {
        addr = soc_reg_addr(unit, soc_fl_bond_info_regs[i], REG_PORT_ANY, 0);
        soc_iproc_getreg(unit, addr, &val);
        SHR_BITCOPY_RANGE(bond_info_regval, (i * 32), &val, 0, 32);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->rev_id, uint16, bond_info_regval, 0, 7);
    SOC_BOND_OPTIONS_COPY(bond_info->dev_id, uint16, bond_info_regval, 8, 23);

    /* IFP_SLICE_DEPTH */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 141, 142);
    switch (val) {
        case 0x0:
            /* 32 entries, skip */
            break;
        case 0x02:
            /* 128 entries, skip */
            break;
        case 0x01:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIfpSliceHalf);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* L2 ENTRY Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 148, 149);
    switch (val) {
        case 0x0:
            /* 2K entries, skip */
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2Entry4k);
            break;
        case 0x2:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2Entry8k);
            break;
        case 0x3:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2Entry16k);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* LPM Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 150, 150);
    switch (val) {
        case 0x0:
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureLpm64);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* NEXT_HOP and L3MC Table Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 151, 151);
    switch (val) {
        case 0x0:
            /* L3NextHop : 128 entries, skip */
            /* Ipmc : 64 entries, skip */
            break;
        case 0x1:
            /* L3NextHop : 512 entries, skip */
            /* Ipmc : 128 entries, skip */
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* L2_MC */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 384, 384);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2mc256);
            break;
        case 0x1:
            /* 1024 entries, skip */
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* SPAN_TREE_GROUPS_SIZE */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 385, 385);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureStg32);
            break;
        case 0x1:
            /* 128 entries, skip */
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* VLAN_TRANSLATION_SIZE */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 386, 386);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpVlanXlate256);
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpVlanXlate1k);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* L3_HOST_SIZE */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 388, 388);
    switch (val) {
        case 0x0:
            /* 256 entries, skip */
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3HostMax512);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* SR_FLOWID_SIZE */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 390, 390);
    switch (val) {
        case 0x0:
            /* 64 entries, skip */
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureSRFlowId256);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    SOC_BOND_OPTIONS_COPY(bond_info->ifp_slice_map, uint32,
                          bond_info_regval, 133, 140);
    SOC_BOND_OPTIONS_COPY(bond_info->efp_slice_map, uint32,
                          bond_info_regval, 143, 146);
    SOC_BOND_OPTIONS_COPY(bond_info->vfp_slice_map, uint32,
                          bond_info_regval, 129, 132);
    SOC_BOND_OPTIONS_COPY(bond_info->cbp_buffer_size, uint32,
                          bond_info_regval, 153, 154);
    SOC_BOND_OPTIONS_COPY(bond_info->max_xqs_number, uint32,
                          bond_info_regval, 359, 359);

    /* TSC related */
    /* 0~2 : PM4X10Q_DISABLE */
    SHR_BITCOPY_RANGE(bond_info->tsc_disabled, 0, bond_info_regval, 162, 3);
    /* 3~6 : PM4X25_DISABLE */
    SHR_BITCOPY_RANGE(bond_info->tsc_disabled, 3, bond_info_regval, 165, 4);

    /* 7 ~ 9 : PM4X10Q_QSGMII_ENABLE[2:0] */
    for(i = 0; i < 3; i++) {
        bit_val = SHR_BITGET(bond_info_regval, (169 + i));
        /* Boolean invert  */
        bit_val = !bit_val;

        SHR_BITCLR(bond_info->tsc_disabled, (7 + i));
        if (bit_val) {
            SHR_BITSET(bond_info->tsc_disabled, (7 + i));
        }
    }

    /* 10 ~ 12 : PM4X10Q_QXGMII_ENABLE[2:0] */
    for(i = 0; i < 3; i++) {
        bit_val = SHR_BITGET(bond_info_regval, (173 + i));
        /* Boolean invert  */
        bit_val = !bit_val;

        SHR_BITCLR(bond_info->tsc_disabled, (10 + i));
        if (bit_val) {
            SHR_BITSET(bond_info->tsc_disabled, (10 + i));
        }
    }

    /* FL_PHY_PORT_OFFSET_TSCE0 ~ FL_PHY_PORT_OFFSET_TSCE0 + 4 : PM4X10_0_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      FL_PHY_PORT_OFFSET_TSCE0, bond_info_regval, 177, 4);
    /* FL_PHY_PORT_OFFSET_TSCE1 ~ FL_PHY_PORT_OFFSET_TSCE1 + 4 : PM4X10_1_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      FL_PHY_PORT_OFFSET_TSCE1, bond_info_regval, 181, 4);
    /* FL_PHY_PORT_OFFSET_TSCE2 ~ FL_PHY_PORT_OFFSET_TSCE2 + 4 : PM4X10_2_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      FL_PHY_PORT_OFFSET_TSCE2, bond_info_regval, 185, 4);
    /* FL_PHY_PORT_OFFSET_TSCF0 ~ FL_PHY_PORT_OFFSET_TSCF0 + 4 : PM4X25_0_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      FL_PHY_PORT_OFFSET_TSCF0, bond_info_regval, 196, 4);
    /* FL_PHY_PORT_OFFSET_TSCF1 ~ FL_PHY_PORT_OFFSET_TSCF1 + 4 : PM4X25_1_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      FL_PHY_PORT_OFFSET_TSCF1, bond_info_regval, 200, 4);
    /* FL_PHY_PORT_OFFSET_TSCF2 ~ FL_PHY_PORT_OFFSET_TSCF2 + 4 : PM4X25_2_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      FL_PHY_PORT_OFFSET_TSCF2, bond_info_regval, 204, 4);
    /* FL_PHY_PORT_OFFSET_TSCF3 ~ FL_PHY_PORT_OFFSET_TSCF3 + 4 : PM4X25_3_LANE_DISABLE[3:0]  */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      FL_PHY_PORT_OFFSET_TSCF3, bond_info_regval, 208, 4);

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
#endif /* BCM_FIRELIGHT_SUPPORT */
