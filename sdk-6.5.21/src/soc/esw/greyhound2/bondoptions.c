/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>

#include <soc/defs.h>

#ifdef BCM_GREYHOUND2_SUPPORT
#include <soc/greyhound2.h>
#include <soc/bondoptions.h>
#include <shared/bitop.h>
#include <soc/iproc.h>

/**********************/
/*
 * Function to initialize the bondoptions cache based on the
 * Bond Info read from the relevant registers.
 */
int
soc_greyhound2_bond_info_init(int unit)
{

    int i, bit_val;
    uint32 val, addr;
    int pport_offset;
    soc_bond_info_t *bond_info = SOC_BOND_INFO(unit);
    const soc_reg_t soc_gh2_bond_info_regs[] = {
        IPROC_WRAP_CHIP_OTP_STATUS_0_31_0r,
        IPROC_WRAP_CHIP_OTP_STATUS_0_63_32r,
        IPROC_WRAP_CHIP_OTP_STATUS_0_95_64r,
        IPROC_WRAP_CHIP_OTP_STATUS_0_127_96r,
        IPROC_WRAP_CHIP_OTP_STATUS_1_31_0r,
        IPROC_WRAP_CHIP_OTP_STATUS_1_63_32r,
        IPROC_WRAP_CHIP_OTP_STATUS_1_95_64r,
        IPROC_WRAP_CHIP_OTP_STATUS_1_127_96r,
        IPROC_WRAP_CHIP_OTP_STATUS_2_31_0r,
        IPROC_WRAP_CHIP_OTP_STATUS_2_63_32r,
        IPROC_WRAP_CHIP_OTP_STATUS_2_95_64r,
        IPROC_WRAP_CHIP_OTP_STATUS_2_127_96r,
        IPROC_WRAP_CHIP_OTP_STATUS_3_31_0r,
        IPROC_WRAP_CHIP_OTP_STATUS_3_63_32r,
        IPROC_WRAP_CHIP_OTP_STATUS_3_95_64r,
        IPROC_WRAP_CHIP_OTP_STATUS_3_127_96r
    };

    int reg_num = COUNTOF(soc_gh2_bond_info_regs);
    SHR_BITDCL bond_info_regval[
                   _SHR_BITDCLSIZE(COUNTOF(soc_gh2_bond_info_regs) * 32)] = {0};

    int feature_enables[][3] = {
        /* Bit */  /* Bool Invert?  */    /* Feature Enum */ /* Desc */
        {389, 0, socBondInfoFeatureEfp64HalfSlice  },  /* EFP_SLICE_DEPTH  */
        {387, 0, socBondInfoFeatureVfp64HalfSlice  },  /* VFP_SLICE_DEPTH  */
        {128, 1, socBondInfoFeatureNoL3            },  /* L3_ENABLE        */
        {352, 1, socBondInfoFeatureNoVxlanLite     },  /* VXLAN_LITE_ENABLE */
        {159, 0, socBondInfoFeatureNoTimeSync      },  /* TS_1588_DISABLE  */
        {151, 1, socBondInfoFeatureNoFpOam         },  /* OAM_ENABLE       */
        {156, 1, socBondInfoFeatureNoFpRouting     },  /* FP_ROUTE_ENABLE  */
        {353, 1, socBondInfoFeatureNoRRCOE         },  /* RROCE_ENABLE     */
        {354, 1, socBondInfoFeatureNoPreemption    },  /* PREEMPTION_ENABLE */
        {355, 1, socBondInfoFeatureNoTAS           },  /* TAS_ENABLE        */
        {356, 1, socBondInfoFeatureNoHSR           },  /* HSR_ENABLE       */
        {357, 1, socBondInfoFeatureNoPRP           },  /* PRP_ENABLE       */
        {358, 1, socBondInfoFeatureNo8021CB        },  /* IEEE_802_1CB_ENABLE */
        {-1,  0, -1                                }   /* Always last         */
    };

    for (i = 0; i < reg_num; i++) {
        addr = soc_reg_addr(unit, soc_gh2_bond_info_regs[i], REG_PORT_ANY, 0);
        soc_iproc_getreg(unit, addr, &val);
        SHR_BITCOPY_RANGE(bond_info_regval, (i * 32), &val, 0, 32);
    }

    SOC_BOND_OPTIONS_COPY(bond_info->rev_id, uint16, bond_info_regval, 0, 7);
    SOC_BOND_OPTIONS_COPY(bond_info->dev_id, uint16, bond_info_regval, 8, 23);

    /* IFP_SLICE_DEPTH */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 145, 146);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIfpSliceHalf);
            break;
        case 0x1:
            /* 256 entries, skip */
            break;
        case 0x2:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIfp64HalfSlice);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* L2 ENTRY Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 152, 153);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2Entry8k);
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2Entry16k);
            break;
        case 0x2:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2Entry32k);
            break;
        case 0x3:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2Entry4k);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* LPM Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 154, 154);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureLpm64);
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureLpm1k);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* NEXT_HOP and L3MC Table Size */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 155, 155);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3NextHop768);
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpmc64);
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3NextHop5k);
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpmc1k);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* L2_MC */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 384, 384);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2mc4k);
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL2mc256);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* SPAN_TREE_GROUPS_SIZE */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 385, 385);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureStg256);
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureStg32);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* VLAN_TRANSLATION_SIZE */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 386, 386);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpVlanXlate4k);
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureIpVlanXlate256);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    /* L3_HOST_SIZE */
    SOC_BOND_OPTIONS_COPY(val, uint32, bond_info_regval, 388, 388);
    switch (val) {
        case 0x0:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureL3HostMax4k);
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
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureSRFlowId4k);
            break;
        case 0x1:
            SOC_BOND_INFO_FEATURE_SET(unit, socBondInfoFeatureSRFlowId256);
            break;
        default:
            return SOC_E_INTERNAL;
    }

    SOC_BOND_OPTIONS_COPY(bond_info->ifp_slice_map, uint32,
                          bond_info_regval, 133, 144);
    SOC_BOND_OPTIONS_COPY(bond_info->efp_slice_map, uint32,
                          bond_info_regval, 147, 150);
    SOC_BOND_OPTIONS_COPY(bond_info->vfp_slice_map, uint32,
                          bond_info_regval, 129, 132);
    SOC_BOND_OPTIONS_COPY(bond_info->cbp_buffer_size, uint32,
                          bond_info_regval, 157, 158);
    SOC_BOND_OPTIONS_COPY(bond_info->max_xqs_number, uint32,
                          bond_info_regval, 359, 359);

    /* TSC related */
    /* 0~1 : QTC_DISABLE */
    SHR_BITCOPY_RANGE(bond_info->tsc_disabled, 0, bond_info_regval, 160, 2);
    /* 2~8 : PM4X10_DISABLE */
    SHR_BITCOPY_RANGE(bond_info->tsc_disabled, 2, bond_info_regval, 162, 7);
    /* 9~9 : PM4X25_DISABLE */
    SHR_BITCOPY_RANGE(bond_info->tsc_disabled, 9, bond_info_regval, 169, 1);

    /* 0~3 : QTC_0_LANE_DISABLE[3:0] */
    pport_offset = GH2_PHY_PORT_OFFSET_QTC0;
    for (i = 0; i < 4; i++) {
        if (SHR_BITGET(bond_info_regval, 170 + i)) {
            SHR_BITSET(bond_info->tsc_in_loop, pport_offset);
            SHR_BITSET(bond_info->tsc_in_loop, pport_offset + 1);
            SHR_BITSET(bond_info->tsc_in_loop, pport_offset + 2);
            SHR_BITSET(bond_info->tsc_in_loop, pport_offset + 3);
        }
        pport_offset += 4;
    }
    /* 4~7 : QTC_1_LANE_DISABLE[3:0] */
    pport_offset = GH2_PHY_PORT_OFFSET_QTC1;
    for (i = 0; i < 4; i++) {
        if (SHR_BITGET(bond_info_regval, 174 + i)) {
            SHR_BITSET(bond_info->tsc_in_loop, pport_offset);
            SHR_BITSET(bond_info->tsc_in_loop, pport_offset + 1);
            SHR_BITSET(bond_info->tsc_in_loop, pport_offset + 2);
            SHR_BITSET(bond_info->tsc_in_loop, pport_offset + 3);
        }
        pport_offset += 4;
    }
    /* 8~11 : PM4X10_0_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      GH2_PHY_PORT_OFFSET_TSCE0, bond_info_regval, 178, 4);
    /* 12~15 : PM4X10_1_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      GH2_PHY_PORT_OFFSET_TSCE1, bond_info_regval, 182, 4);
    /* 16~19 : PM4X10_2_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      GH2_PHY_PORT_OFFSET_TSCE2, bond_info_regval, 186, 4);
    /* 20~23 : PM4X10_3_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      GH2_PHY_PORT_OFFSET_TSCE3, bond_info_regval, 192, 4);
    /* 24~27 : PM4X10_4_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      GH2_PHY_PORT_OFFSET_TSCE4, bond_info_regval, 196, 4);
    /* 28~31 : PM4X10_5_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      GH2_PHY_PORT_OFFSET_TSCE5, bond_info_regval, 200, 4);
    /* 32~35 : PM4X10_6_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      GH2_PHY_PORT_OFFSET_TSCE6, bond_info_regval, 204, 4);
    /* 36~39 : PM4X25_0_LANE_DISABLE[3:0] */
    SHR_BITCOPY_RANGE(bond_info->tsc_in_loop,
                      GH2_PHY_PORT_OFFSET_TSCF0, bond_info_regval, 208, 4);

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
#endif /* BCM_GREYHOUND2_SUPPORT */
