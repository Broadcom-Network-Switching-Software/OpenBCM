/*! \file bcm56780_a0_dna_2_4_13_uft.c
 *
 * bcm56780_A0 DNA_2_4_13 UFT Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/switch.h>
#include <bcm_int/ltsw/uft_int.h>
#include <bcm_int/ltsw/mbcm/uft.h>
#include <bcm_int/ltsw/property.h>

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_UFT

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Bank IDs mapping table.
 */
static shr_enum_map_t bcm56780_a0_dna_2_4_13_ltsw_uft_bank_id_map[] = {
    { (char *)EFTA30_E2T_00_BANK0s, bcmSwitchHashBank0 },
    { (char *)EFTA30_E2T_00_BANK1s, bcmSwitchHashBank1 },
    { (char *)IFTA30_E2T_00_BANK0s, bcmSwitchHashBank2 },
    { (char *)IFTA30_E2T_00_BANK1s, bcmSwitchHashBank3 },
    { (char *)IFTA40_E2T_00_BANK0s, bcmSwitchHashBank4 },
    { (char *)IFTA40_E2T_00_BANK1s, bcmSwitchHashBank5 },
    { (char *)IFTA40_E2T_01_BANK0s, bcmSwitchHashBank6 },
    { (char *)IFTA40_E2T_01_BANK1s, bcmSwitchHashBank7 },
    { (char *)IFTA80_E2T_00_BANK0s, bcmSwitchHashBank8 },
    { (char *)IFTA80_E2T_00_BANK1s, bcmSwitchHashBank9 },
    { (char *)IFTA80_E2T_01_BANK0s, bcmSwitchHashBank10 },
    { (char *)IFTA80_E2T_01_BANK1s, bcmSwitchHashBank11 },
    { (char *)IFTA80_E2T_02_BANK0s, bcmSwitchHashBank12 },
    { (char *)IFTA80_E2T_02_BANK1s, bcmSwitchHashBank13 },
    { (char *)IFTA80_E2T_03_BANK0s, bcmSwitchHashBank14 },
    { (char *)IFTA80_E2T_03_BANK1s, bcmSwitchHashBank15 },
    { (char *)IFTA90_E2T_00_BANK0s, bcmSwitchHashBank16 },
    { (char *)IFTA90_E2T_00_BANK1s, bcmSwitchHashBank17 },
    { (char *)IFTA90_E2T_00_BANK2s, bcmSwitchHashBank18 },
    { (char *)IFTA90_E2T_00_BANK3s, bcmSwitchHashBank19 },
    { (char *)IFTA90_E2T_01_BANK0s, bcmSwitchHashBank20 },
    { (char *)IFTA90_E2T_01_BANK1s, bcmSwitchHashBank21 },
    { (char *)IFTA90_E2T_01_BANK2s, bcmSwitchHashBank22 },
    { (char *)IFTA90_E2T_01_BANK3s, bcmSwitchHashBank23 },
    { (char *)IFTA90_E2T_02_BANK0s, bcmSwitchHashBank24 },
    { (char *)IFTA90_E2T_02_BANK1s, bcmSwitchHashBank25 },
    { (char *)IFTA90_E2T_02_BANK2s, bcmSwitchHashBank26 },
    { (char *)IFTA90_E2T_02_BANK3s, bcmSwitchHashBank27 },
    { (char *)IFTA90_E2T_03_BANK0s, bcmSwitchHashBank28 },
    { (char *)IFTA90_E2T_03_BANK1s, bcmSwitchHashBank29 },
    { (char *)IFTA90_E2T_03_BANK2s, bcmSwitchHashBank30 },
    { (char *)IFTA90_E2T_03_BANK3s, bcmSwitchHashBank31 },
    { (char *)UFT_MINI_TILE_BANK0s, bcmSwitchHashBankCount},
    { (char *)UFT_MINI_TILE_BANK1s, bcmSwitchHashBankCount + 1 },
    { (char *)UFT_MINI_TILE_BANK2s, bcmSwitchHashBankCount + 2 },
    { (char *)UFT_MINI_TILE_BANK3s, bcmSwitchHashBankCount + 3 },
    { (char *)UFT_MINI_TILE_BANK4s, bcmSwitchHashBankCount + 4 },
    { (char *)UFT_MINI_TILE_BANK5s, bcmSwitchHashBankCount + 5 },
    { (char *)IFTA80_T2T_00_BANK0s, bcmSwitchHashBankCount + 6 },
    { (char *)IFTA80_T2T_00_BANK1s, bcmSwitchHashBankCount + 7 },
    { (char *)IFTA80_T2T_00_BANK2s, bcmSwitchHashBankCount + 8 },
    { (char *)IFTA80_T2T_00_BANK3s, bcmSwitchHashBankCount + 9 },
    { (char *)IFTA80_T2T_00_BANK4s, bcmSwitchHashBankCount + 10 },
    { (char *)IFTA80_T2T_00_BANK5s, bcmSwitchHashBankCount + 11 },
    { (char *)IFTA80_T2T_00_BANK6s, bcmSwitchHashBankCount + 12 },
    { (char *)IFTA80_T2T_00_BANK7s, bcmSwitchHashBankCount + 13 },
    { (char *)IFTA80_T2T_01_BANK0s, bcmSwitchHashBankCount + 14 },
    { (char *)IFTA80_T2T_01_BANK1s, bcmSwitchHashBankCount + 15 },
    { (char *)IFTA80_T2T_01_BANK2s, bcmSwitchHashBankCount + 16 },
    { (char *)IFTA80_T2T_01_BANK3s, bcmSwitchHashBankCount + 17 },
    { (char *)IFTA80_T2T_01_BANK4s, bcmSwitchHashBankCount + 18 },
    { (char *)IFTA80_T2T_01_BANK5s, bcmSwitchHashBankCount + 19 },
    { (char *)IFTA80_T2T_01_BANK6s, bcmSwitchHashBankCount + 20 },
    { (char *)IFTA80_T2T_01_BANK7s, bcmSwitchHashBankCount + 21 },
    { (char *)IFTA40_T4T_00_BANK0s, bcmSwitchHashBankCount + 22 },
    { (char *)IFTA40_T4T_00_BANK1s, bcmSwitchHashBankCount + 23 },
    { (char *)IFTA40_T4T_00_BANK2s, bcmSwitchHashBankCount + 24 },
    { (char *)IFTA40_T4T_00_BANK3s, bcmSwitchHashBankCount + 25 },
    { (char *)IFTA40_T4T_01_BANK0s, bcmSwitchHashBankCount + 26 },
    { (char *)IFTA40_T4T_01_BANK1s, bcmSwitchHashBankCount + 27 },
    { (char *)IFTA40_T4T_01_BANK2s, bcmSwitchHashBankCount + 28 },
    { (char *)IFTA40_T4T_01_BANK3s, bcmSwitchHashBankCount + 29 },
    { (char *)EFTA10_I1T_00_BANK0s, bcmSwitchHashBankCount + 30 },
    { (char *)EFTA10_I1T_01_BANK0s, bcmSwitchHashBankCount + 31 },
    { (char *)EFTA10_I1T_01_BANK1s, bcmSwitchHashBankCount + 32 },
    { (char *)EFTA10_I1T_02_BANK0s, bcmSwitchHashBankCount + 33 },
    { (char *)EFTA10_I1T_03_BANK0s, bcmSwitchHashBankCount + 34 },
    { (char *)EFTA20_I1T_00_BANK0s, bcmSwitchHashBankCount + 35 },
    { (char *)EFTA20_I1T_01_BANK0s, bcmSwitchHashBankCount + 36 },
    { (char *)EFTA20_I1T_02_BANK0s, bcmSwitchHashBankCount + 37 },
    { (char *)EFTA20_I1T_03_BANK0s, bcmSwitchHashBankCount + 38 },
    { (char *)EFTA20_I1T_04_BANK0s, bcmSwitchHashBankCount + 39 },
    { (char *)EFTA20_I1T_05_BANK0s, bcmSwitchHashBankCount + 40 },
    { (char *)IFTA130_I1T_00_BANK0s, bcmSwitchHashBankCount + 41 },
    { (char *)IFTA130_I1T_00_BANK1s, bcmSwitchHashBankCount + 42 },
    { (char *)IFTA130_I1T_01_BANK0s, bcmSwitchHashBankCount + 43 },
    { (char *)IFTA130_I1T_01_BANK1s, bcmSwitchHashBankCount + 44 },
    { (char *)IFTA130_I1T_02_BANK0s, bcmSwitchHashBankCount + 45 },
    { (char *)UFT_MTOP_ALPM_LEVEL2_BANK0s, bcmSwitchHashBankCount + 46},
    { (char *)UFT_MTOP_ALPM_LEVEL2_BANK1s, bcmSwitchHashBankCount + 47 },
    { (char *)UFT_MTOP_ALPM_LEVEL2_BANK2s, bcmSwitchHashBankCount + 48 },
    { (char *)UFT_MTOP_ALPM_LEVEL2_BANK3s, bcmSwitchHashBankCount + 49 },
    { (char *)UFT_MTOP_ALPM_LEVEL2_BANK4s, bcmSwitchHashBankCount + 50 },
    { (char *)UFT_MTOP_ALPM_LEVEL2_BANK5s, bcmSwitchHashBankCount + 51 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK0s, bcmSwitchHashBankCount + 52 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK1s, bcmSwitchHashBankCount + 53 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK2s, bcmSwitchHashBankCount + 54 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK3s, bcmSwitchHashBankCount + 55 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK4s, bcmSwitchHashBankCount + 56 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK5s, bcmSwitchHashBankCount + 57 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK6s, bcmSwitchHashBankCount + 58 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK7s, bcmSwitchHashBankCount + 59 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK8s, bcmSwitchHashBankCount + 60 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK9s, bcmSwitchHashBankCount + 61 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK10s, bcmSwitchHashBankCount + 62 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK11s, bcmSwitchHashBankCount + 63 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK12s, bcmSwitchHashBankCount + 64 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK13s, bcmSwitchHashBankCount + 65 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK14s, bcmSwitchHashBankCount + 66 },
    { (char *)UFT_MTOP_ALPM_LEVEL1_BANK15s, bcmSwitchHashBankCount + 67 },
};

/*!
 * \brief TILE Bank IDs mapping table.
 */
static shr_enum_map_t bcm56780_a0_dna_2_4_13_ltsw_uft_tile_bank_map[] = {
    { (char *)EFTA30_E2T_00s, bcmSwitchHashBank0 },
    { (char *)EFTA30_E2T_00s, bcmSwitchHashBank1 },
    { (char *)IFTA30_E2T_00s, bcmSwitchHashBank2 },
    { (char *)IFTA30_E2T_00s, bcmSwitchHashBank3 },
    { (char *)IFTA40_E2T_00s, bcmSwitchHashBank4 },
    { (char *)IFTA40_E2T_00s, bcmSwitchHashBank5 },
    { (char *)IFTA40_E2T_01s, bcmSwitchHashBank6 },
    { (char *)IFTA40_E2T_01s, bcmSwitchHashBank7 },
    { (char *)IFTA80_E2T_00s, bcmSwitchHashBank8 },
    { (char *)IFTA80_E2T_00s, bcmSwitchHashBank9 },
    { (char *)IFTA80_E2T_01s, bcmSwitchHashBank10 },
    { (char *)IFTA80_E2T_01s, bcmSwitchHashBank11 },
    { (char *)IFTA80_E2T_02s, bcmSwitchHashBank12 },
    { (char *)IFTA80_E2T_02s, bcmSwitchHashBank13 },
    { (char *)IFTA80_E2T_03s, bcmSwitchHashBank14 },
    { (char *)IFTA80_E2T_03s, bcmSwitchHashBank15 },
    { (char *)IFTA90_E2T_00s, bcmSwitchHashBank16 },
    { (char *)IFTA90_E2T_00s, bcmSwitchHashBank17 },
    { (char *)IFTA90_E2T_00s, bcmSwitchHashBank18 },
    { (char *)IFTA90_E2T_00s, bcmSwitchHashBank19 },
    { (char *)IFTA90_E2T_01s, bcmSwitchHashBank20 },
    { (char *)IFTA90_E2T_01s, bcmSwitchHashBank21 },
    { (char *)IFTA90_E2T_01s, bcmSwitchHashBank22 },
    { (char *)IFTA90_E2T_01s, bcmSwitchHashBank23 },
    { (char *)IFTA90_E2T_02s, bcmSwitchHashBank24 },
    { (char *)IFTA90_E2T_02s, bcmSwitchHashBank25 },
    { (char *)IFTA90_E2T_02s, bcmSwitchHashBank26 },
    { (char *)IFTA90_E2T_02s, bcmSwitchHashBank27 },
    { (char *)IFTA90_E2T_03s, bcmSwitchHashBank28 },
    { (char *)IFTA90_E2T_03s, bcmSwitchHashBank29 },
    { (char *)IFTA90_E2T_03s, bcmSwitchHashBank30 },
    { (char *)IFTA90_E2T_03s, bcmSwitchHashBank31 },
    { (char *)UFT_MINI_TILEs, bcmSwitchHashBankCount},
    { (char *)UFT_MINI_TILEs, bcmSwitchHashBankCount + 1 },
    { (char *)UFT_MINI_TILEs, bcmSwitchHashBankCount + 2 },
    { (char *)UFT_MINI_TILEs, bcmSwitchHashBankCount + 3 },
    { (char *)UFT_MINI_TILEs, bcmSwitchHashBankCount + 4 },
    { (char *)UFT_MINI_TILEs, bcmSwitchHashBankCount + 5 },
    { (char *)IFTA80_T2T_00s, bcmSwitchHashBankCount + 6 },
    { (char *)IFTA80_T2T_00s, bcmSwitchHashBankCount + 7 },
    { (char *)IFTA80_T2T_00s, bcmSwitchHashBankCount + 8 },
    { (char *)IFTA80_T2T_00s, bcmSwitchHashBankCount + 9 },
    { (char *)IFTA80_T2T_00s, bcmSwitchHashBankCount + 10 },
    { (char *)IFTA80_T2T_00s, bcmSwitchHashBankCount + 11 },
    { (char *)IFTA80_T2T_00s, bcmSwitchHashBankCount + 12 },
    { (char *)IFTA80_T2T_00s, bcmSwitchHashBankCount + 13 },
    { (char *)IFTA80_T2T_01s, bcmSwitchHashBankCount + 14 },
    { (char *)IFTA80_T2T_01s, bcmSwitchHashBankCount + 15 },
    { (char *)IFTA80_T2T_01s, bcmSwitchHashBankCount + 16 },
    { (char *)IFTA80_T2T_01s, bcmSwitchHashBankCount + 17 },
    { (char *)IFTA80_T2T_01s, bcmSwitchHashBankCount + 18 },
    { (char *)IFTA80_T2T_01s, bcmSwitchHashBankCount + 19 },
    { (char *)IFTA80_T2T_01s, bcmSwitchHashBankCount + 20 },
    { (char *)IFTA80_T2T_01s, bcmSwitchHashBankCount + 21 },
    { (char *)IFTA40_T4T_00s, bcmSwitchHashBankCount + 22 },
    { (char *)IFTA40_T4T_00s, bcmSwitchHashBankCount + 23 },
    { (char *)IFTA40_T4T_00s, bcmSwitchHashBankCount + 24 },
    { (char *)IFTA40_T4T_00s, bcmSwitchHashBankCount + 25 },
    { (char *)IFTA40_T4T_01s, bcmSwitchHashBankCount + 26 },
    { (char *)IFTA40_T4T_01s, bcmSwitchHashBankCount + 27 },
    { (char *)IFTA40_T4T_01s, bcmSwitchHashBankCount + 28 },
    { (char *)IFTA40_T4T_01s, bcmSwitchHashBankCount + 29 },
    { (char *)EFTA10_I1T_00s, bcmSwitchHashBankCount + 30 },
    { (char *)EFTA10_I1T_01s, bcmSwitchHashBankCount + 31 },
    { (char *)EFTA10_I1T_01s, bcmSwitchHashBankCount + 32 },
    { (char *)EFTA10_I1T_02s, bcmSwitchHashBankCount + 33 },
    { (char *)EFTA10_I1T_03s, bcmSwitchHashBankCount + 34 },
    { (char *)EFTA20_I1T_00s, bcmSwitchHashBankCount + 35 },
    { (char *)EFTA20_I1T_01s, bcmSwitchHashBankCount + 36 },
    { (char *)EFTA20_I1T_02s, bcmSwitchHashBankCount + 37 },
    { (char *)EFTA20_I1T_03s, bcmSwitchHashBankCount + 38 },
    { (char *)EFTA20_I1T_04s, bcmSwitchHashBankCount + 39 },
    { (char *)EFTA20_I1T_05s, bcmSwitchHashBankCount + 40 },
    { (char *)IFTA130_I1T_00s, bcmSwitchHashBankCount + 41 },
    { (char *)IFTA130_I1T_00s, bcmSwitchHashBankCount + 42 },
    { (char *)IFTA130_I1T_01s, bcmSwitchHashBankCount + 43 },
    { (char *)IFTA130_I1T_01s, bcmSwitchHashBankCount + 44 },
    { (char *)IFTA130_I1T_02s, bcmSwitchHashBankCount + 45 },
    { (char *)UFT_MTOP_ALPM_LEVEL2s, bcmSwitchHashBankCount + 46},
    { (char *)UFT_MTOP_ALPM_LEVEL2s, bcmSwitchHashBankCount + 47 },
    { (char *)UFT_MTOP_ALPM_LEVEL2s, bcmSwitchHashBankCount + 48 },
    { (char *)UFT_MTOP_ALPM_LEVEL2s, bcmSwitchHashBankCount + 49 },
    { (char *)UFT_MTOP_ALPM_LEVEL2s, bcmSwitchHashBankCount + 50 },
    { (char *)UFT_MTOP_ALPM_LEVEL2s, bcmSwitchHashBankCount + 51 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 52 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 53 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 54 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 55 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 56 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 57 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 58 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 59 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 60 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 61 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 62 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 63 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 64 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 65 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 66 },
    { (char *)UFT_MTOP_ALPM_LEVEL1s, bcmSwitchHashBankCount + 67 },
};

/*!
 * \brief Switch hash table to LT name mapping table.
 *
 * Switch hash tables need to be in ascending order.
 */
static shr_enum_map_t bcm56780_a0_dna_2_4_13_ltsw_uft_table_map[] = {
    { (char *)L2_HOST_TABLEs,              bcmHashTableL2 },
    { (char *)L3_IPV4_UNICAST_TABLEs,      bcmHashTableL3 },
    { (char *)ING_VLAN_XLATE_1_TABLEs,     bcmHashTableVlanTranslate },
    { (char *)EGR_VLAN_TRANSLATIONs,       bcmHashTableEgressVlanTranslate },
    { (char *)DT_EM_FT_ENTRYs,             bcmHashTableFlowtrackerEntry },
    { (char *)L2_TUNNEL_IPV4_DIP_TABLEs,   bcmHashTableL2Tunnel },
    { (char *)L2_TUNNEL_VN_ID_TABLEs,      bcmHashTableL2TunnelVnid },
    { (char *)L3_IPV4_TUNNEL_EM_TABLEs,    bcmHashTableL3Tunnel },
    { (char *)DT_EM_FP_ENTRYs,             bcmHashTableExactMatch },
    { (char *)L3_IPV4_MULTICAST_1_TABLEs,  bcmHashTableIpmcGroup },
    { (char *)L3_IPV4_MULTICAST_2_TABLEs,  bcmHashTableIpmcSourceGroup },
    { (char *)L2_IPV4_MULTICAST_TABLEs,    bcmHashTableL2mc },
};

/*!
 * \brief Switch controls to TABLE_EM_CONTROL LT key symbol mapping table.
 *
 * Switch controls need to be in ascending order.
 */
static shr_enum_map_t bcm56780_a0_dna_2_4_13_ltsw_uft_move_depth_map[] = {
    { (char *)L2_HOST_TABLEs,              bcmSwitchHashMultiMoveDepthL2 },
    { (char *)L2_HOST_NARROW_TABLEs,       bcmSwitchHashMultiMoveDepthL2 },
    { (char *)L3_IPV4_UNICAST_TABLEs,      bcmSwitchHashMultiMoveDepthL3 },
    { (char *)L3_IPV6_UNICAST_TABLEs,      bcmSwitchHashMultiMoveDepthL3 },
    { (char *)DT_EM_FP_ENTRYs,             bcmSwitchHashMultiMoveDepthExactMatch },
    { (char *)ING_VLAN_XLATE_1_TABLEs,     bcmSwitchHashMultiMoveDepthVlanTranslate1 },
    { (char *)ING_VLAN_XLATE_2_TABLEs,     bcmSwitchHashMultiMoveDepthVlanTranslate2 },
    { (char *)EGR_VLAN_TRANSLATIONs,       bcmSwitchHashMultiMoveDepthEgressVlanTranslate1 },
    { (char *)EGR_VLAN_TRANSLATION_2s,     bcmSwitchHashMultiMoveDepthEgressVlanTranslate2 },
    { (char *)L3_IPV4_TUNNEL_EM_TABLEs,    bcmSwitchHashMultiMoveDepthL3Tunnel },
    { (char *)L3_IPV6_TUNNEL_EM_TABLEs,    bcmSwitchHashMultiMoveDepthL3Tunnel },
    { (char *)ING_VLAN_XLATE_3_TABLEs,     bcmSwitchHashMultiMoveDepthVlanTranslate3 },
    { (char *)L2_TUNNEL_IPV4_DIP_TABLEs,   bcmSwitchHashMultiMoveDepthL2Tunnel },
    { (char *)L2_TUNNEL_IPV4_SIP_TABLEs,   bcmSwitchHashMultiMoveDepthL2Tunnel },
    { (char *)L2_TUNNEL_IPV6_DIP_TABLEs,   bcmSwitchHashMultiMoveDepthL2Tunnel },
    { (char *)L2_TUNNEL_IPV6_SIP_TABLEs,   bcmSwitchHashMultiMoveDepthL2Tunnel },
    { (char *)L2_TUNNEL_VN_ID_TABLEs,      bcmSwitchHashMultiMoveDepthL2TunnelVnid },
    { (char *)DT_EM_FT_ENTRYs,             bcmSwitchHashMultiMoveDepthFlowTracker },
    { (char *)L3_IPV4_MULTICAST_1_TABLEs,  bcmSwitchHashMultiMoveDepthIpmcGroup },
    { (char *)L3_IPV6_MULTICAST_1_TABLEs,  bcmSwitchHashMultiMoveDepthIpmcGroup },
    { (char *)L3_IPV4_MULTICAST_2_TABLEs,  bcmSwitchHashMultiMoveDepthIpmcSourceGroup },
    { (char *)L3_IPV6_MULTICAST_2_TABLEs,  bcmSwitchHashMultiMoveDepthIpmcSourceGroup },
    { (char *)L2_IPV4_MULTICAST_TABLEs,    bcmSwitchHashMultiMoveDepthL2mc },
    { (char *)L2_IPV6_MULTICAST_TABLEs,    bcmSwitchHashMultiMoveDepthL2mc },
    { (char *)L2_IPV4_MULTICAST_SG_TABLEs, bcmSwitchHashMultiMoveDepthL2mc },
    { (char *)L2_IPV6_MULTICAST_SG_TABLEs, bcmSwitchHashMultiMoveDepthL2mc },
};

/*!
 * \brief Config property to logical table mapping table.
 */
static bcmint_uft_str_map_t bcm56780_a0_dna_2_4_13_ltsw_uft_property_map[] = {
    { (char *)BCMI_CPN_IP4_MC_GROUP_TABLE_ENTRIES_MAX,
      (char *)L3_IPV4_MULTICAST_1_TABLEs
    },
    { (char *)BCMI_CPN_IP6_MC_GROUP_TABLE_ENTRIES_MAX,
      (char *)L3_IPV6_MULTICAST_1_TABLEs
    },
    { (char *)BCMI_CPN_IP4_L2MC_TABLE_ENTRIES_MAX,
      (char *)L2_IPV4_MULTICAST_TABLEs
    },
    { (char *)BCMI_CPN_IP6_L2MC_TABLE_ENTRIES_MAX,
      (char *)L2_IPV6_MULTICAST_TABLEs
    },
    { (char *)BCMI_CPN_IP4_MC_SOURCE_GROUP_TABLE_ENTRIES_MAX,
      (char *)L3_IPV4_MULTICAST_2_TABLEs
    },
    { (char *)BCMI_CPN_IP6_MC_SOURCE_GROUP_TABLE_ENTRIES_MAX,
      (char *)L3_IPV6_MULTICAST_2_TABLEs
    },
    { (char *)BCMI_CPN_IP4_UC_TABLE_ENTRIES_MAX,
      (char *)L3_IPV4_UNICAST_TABLEs
    },
    { (char *)BCMI_CPN_IP6_UC_TABLE_ENTRIES_MAX,
      (char *)L3_IPV6_UNICAST_TABLEs
    },
    { (char *)BCMI_CPN_EGR_VLAN_TRANSLATION_VFI_TAG_ACTION_TABLE_ENTRIES_MAX,
      (char *)EGR_VLAN_TRANSLATIONs
    },
    { (char *)BCMI_CPN_EGR_VLAN_TRANSLATION_VFI_VNID_TABLE_ENTRIES_MAX,
      (char *)EGR_VLAN_TRANSLATION_2s
    },
    { (char *)BCMI_CPN_ING_VLAN_XLATE_OVID_TABLE_ENTRIES_MAX,
      (char *)ING_VLAN_XLATE_1_TABLEs
    },
    { (char *)BCMI_CPN_ING_VLAN_XLATE_IVID_TABLE_ENTRIES_MAX,
      (char *)ING_VLAN_XLATE_2_TABLEs
    },
    { (char *)BCMI_CPN_ING_VLAN_XLATE_OVID_IVID_TABLE_ENTRIES_MAX,
      (char *)ING_VLAN_XLATE_3_TABLEs
    },
    { (char *)BCMI_CPN_L2_TUNNEL_VNID_TABLE_ENTRIES_MAX,
      (char *)L2_TUNNEL_VN_ID_TABLEs
    },
    { (char *)BCMI_CPN_L2_TUNNEL_IP4_DIP_TABLE_ENTRIES_MAX,
      (char *)L2_TUNNEL_IPV4_DIP_TABLEs
    },
    { (char *)BCMI_CPN_L2_TUNNEL_IP4_SIP_TABLE_ENTRIES_MAX,
      (char *)L2_TUNNEL_IPV4_SIP_TABLEs
    },
    { (char *)BCMI_CPN_L2_TUNNEL_IP6_DIP_TABLE_ENTRIES_MAX,
      (char *)L2_TUNNEL_IPV6_DIP_TABLEs
    },
    { (char *)BCMI_CPN_L2_TUNNEL_IP6_SIP_TABLE_ENTRIES_MAX,
      (char *)L2_TUNNEL_IPV6_SIP_TABLEs
    },
    { (char *)BCMI_CPN_L3_TUNNEL_IP4_EM_TABLE_ENTRIES_MAX,
      (char *)L3_IPV4_TUNNEL_EM_TABLEs
    },
    { (char *)BCMI_CPN_L3_TUNNEL_IP6_EM_TABLE_ENTRIES_MAX,
      (char *)L3_IPV6_TUNNEL_EM_TABLEs
    },
    { (char *)BCMI_CPN_L3_TUNNEL_IP4_TABLE_ENTRIES_MAX,
      (char *)L3_IPV4_TUNNEL_TABLEs
    },
    { (char *)BCMI_CPN_L3_TUNNEL_IP6_TABLE_ENTRIES_MAX,
      (char *)L3_IPV6_TUNNEL_TABLEs
    },
    { (char *)BCMI_CPN_IP4_L2MC_SOURCE_GROUP_TABLE_ENTRIES_MAX,
      (char *)L2_IPV4_MULTICAST_SG_TABLEs
    },
    { (char *)BCMI_CPN_IP6_L2MC_SOURCE_GROUP_TABLE_ENTRIES_MAX,
      (char *)L2_IPV6_MULTICAST_SG_TABLEs
    },
    { (char *)BCMI_CPN_L2_HOST_NARROW_TABLE_ENTRIES_MAX,
      (char *)L2_HOST_NARROW_TABLEs
    },
    { (char *)BCMI_CPN_L2_HOST_TABLE_ENTRIES_MAX,
      (char *)L2_HOST_TABLEs
    },
    { (char *)BCMI_CPN_EGR_VLAN_TRANSLATION_DW_TABLE_ENTRIES_MAX,
      (char *)EGR_VLAN_TRANSLATION_DWs
    },
};

/*!
 * \brief Default logical table array.
 */
static const char *bcm56780_a0_dna_2_4_13_ltsw_uft_def_lt_array[] = {
    L2_HOST_LOOKUP0_DEFAULT_TABLEs,
    L3_IP_LOOKUP0_DEFAULT_TABLEs,
    L3_IP_LOOKUP1_DEFAULT_TABLEs,
};

/*!
 * \brief Tile modes configurations for UFT mode 1.
 */
static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_1_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_1_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_1_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_1_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 1 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_1_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_1_device_em_tile_entries),
        .entry_db    = uft_mode_1_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 2.
 */
static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE1s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE1s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_2_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_2_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = COMP_DSTs,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = COMP_DSTs,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_2_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_2_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 2 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_2_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_2_device_em_tile_entries),
        .entry_db    = uft_mode_2_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 3.
 */
static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE8s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE8s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_3_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_3_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = TCAMs,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_3_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_3_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 3 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_3_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_3_device_em_tile_entries),
        .entry_db    = uft_mode_3_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 4.
 */
static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_4_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_4_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = TCAMs,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_4_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_4_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 4 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_4_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_4_device_em_tile_entries),
        .entry_db    = uft_mode_4_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 5.
 */
static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE1s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_5_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_5_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = TCAMs,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_5_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_5_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 5 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_5_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_5_device_em_tile_entries),
        .entry_db    = uft_mode_5_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 6.
 */
static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE1s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_6_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_6_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_6_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_6_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 6 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_6_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_6_device_em_tile_entries),
        .entry_db    = uft_mode_6_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 7.
 */
static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE14s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE14s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_7_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_7_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_7_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_7_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 7 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_7_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_7_device_em_tile_entries),
        .entry_db    = uft_mode_7_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 8.
 */
static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE1s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE10s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE10s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE1s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_8_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_8_device_em_tile_entries[] = {
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = COMP_DSTs,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = COMP_DSTs,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_8_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_8_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 8 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_8_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_8_device_em_tile_entries),
        .entry_db    = uft_mode_8_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 9.
 */
static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE4s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE1s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE11s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE11s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_9_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_9_device_em_tile_entries[] = {
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_9_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_9_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 9 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_9_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_9_device_em_tile_entries),
        .entry_db    = uft_mode_9_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 10.
 */
static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE14s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE14s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_10_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_10_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_10_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_10_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 10 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_10_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_10_device_em_tile_entries),
        .entry_db    = uft_mode_10_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 11.
 */
static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_11_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_11_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = COMP_DSTs,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = COMP_DSTs,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_11_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_11_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 11 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_11_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_11_device_em_tile_entries),
        .entry_db    = uft_mode_11_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 12.
 */
static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE6s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_12_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_12_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = COMP_DSTs,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_12_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_12_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 12 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_12_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_12_device_em_tile_entries),
        .entry_db    = uft_mode_12_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 13.
 */
static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE6s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_13_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_13_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_13_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_13_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 13 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_13_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_13_device_em_tile_entries),
        .entry_db    = uft_mode_13_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 14.
 */
static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE1s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_14_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_14_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = COMP_DSTs,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_14_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_14_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 14 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_14_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_14_device_em_tile_entries),
        .entry_db    = uft_mode_14_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UFT mode 15.
 */
static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_01_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_00_MODE5s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA80_T2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA80_T2T_01_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_00_MODE3s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_01_MODE2s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_02_MODE1s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_9_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA90_E2T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA90_E2T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_10_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MINI_TILEs,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MINI_TILE_MODE0s,
    }
};

static bcmint_uft_field_info_t
uft_mode_15_device_em_tile_entry_11_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2s,
    },
    {
        .field_name = MODEs,
        .symbol     = UFT_MTOP_ALPM_LEVEL2_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uft_mode_15_device_em_tile_entries[] = {
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_0_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = DYNAMIC_FT_FPs,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_1_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_2_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_3_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_4_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_1s,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_5_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = COMP_DSTs,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_6_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_7_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_8_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_8_fields,
    },
    {
        .entry_attr  = L3UC_DSTs,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_9_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_9_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_10_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_10_fields,
    },
    {
        .entry_attr  = ALPM_LEVEL_2s,
        .field_cnt   = COUNTOF(uft_mode_15_device_em_tile_entry_11_fields),
        .field_db    = uft_mode_15_device_em_tile_entry_11_fields,
    }
};

/*!
 * \brief UFT mode 15 database.
 */
static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_15_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uft_mode_15_device_em_tile_entries),
        .entry_db    = uft_mode_15_device_em_tile_entries,
    }
};

/*!
 * \brief DNA_2_4_13 UFT mode database.
 */
static bcmint_uft_mode_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode[] = {
    {
        .mode       = bcmiUftMode1,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_1_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_1_tables,
    },
    {
        .mode       = bcmiUftMode2,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_2_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_2_tables,
    },
    {
        .mode       = bcmiUftMode3,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_3_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_3_tables,
    },
    {
        .mode       = bcmiUftMode4,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_4_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_4_tables,
    },
    {
        .mode       = bcmiUftMode5,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_5_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_5_tables,
    },
    {
        .mode       = bcmiUftMode6,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_6_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_6_tables,
    },
    {
        .mode       = bcmiUftMode7,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_7_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_7_tables,
    },
    {
        .mode       = bcmiUftMode8,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_8_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_8_tables,
    },
    {
        .mode       = bcmiUftMode9,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_9_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_9_tables,
    },
    {
        .mode       = bcmiUftMode10,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_10_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_10_tables,
    },
    {
        .mode       = bcmiUftMode11,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_11_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_11_tables,
    },
    {
        .mode       = bcmiUftMode12,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_12_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_12_tables,
    },
    {
        .mode       = bcmiUftMode13,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_13_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_13_tables,
    },
    {
        .mode       = bcmiUftMode14,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_14_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_14_tables,
    },
    {
        .mode       = bcmiUftMode15,
        .table_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_15_tables),
        .table_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode_15_tables,
    }
};

/*!
 * \brief Tile modes configurations for UAT mode 0.
 */
static bcmint_uft_field_info_t
uat_mode_0_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA30_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA30_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uat_mode_0_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA40_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA40_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uat_mode_0_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA40_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA40_E2T_01_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uat_mode_0_device_em_tile_entries[] = {
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uat_mode_0_device_em_tile_entry_0_fields),
        .field_db    = uat_mode_0_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uat_mode_0_device_em_tile_entry_1_fields),
        .field_db    = uat_mode_0_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uat_mode_0_device_em_tile_entry_2_fields),
        .field_db    = uat_mode_0_device_em_tile_entry_2_fields,
    }
};

static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uat_mode_0_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uat_mode_0_device_em_tile_entries),
        .entry_db    = uat_mode_0_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for UAT mode 1.
 */
static bcmint_uft_field_info_t
uat_mode_1_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA30_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA30_E2T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
uat_mode_1_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA40_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA40_E2T_00_MODE1s,
    }
};

static bcmint_uft_field_info_t
uat_mode_1_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA40_E2T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA40_E2T_01_MODE0s,
    }
};

static bcmint_uft_entry_info_t
uat_mode_1_device_em_tile_entries[] = {
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uat_mode_1_device_em_tile_entry_0_fields),
        .field_db    = uat_mode_1_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uat_mode_1_device_em_tile_entry_1_fields),
        .field_db    = uat_mode_1_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(uat_mode_1_device_em_tile_entry_2_fields),
        .field_db    = uat_mode_1_device_em_tile_entry_2_fields,
    }
};

static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uat_mode_1_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(uat_mode_1_device_em_tile_entries),
        .entry_db    = uat_mode_1_device_em_tile_entries,
    }
};

/*!
 * \brief DNA_2_4_13 UAT mode database.
 */
static bcmint_uft_mode_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_uat_mode[] = {
    {
        .mode        = 0,
        .table_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uat_mode_0_tables),
        .table_db    = bcm56780_a0_dna_2_4_13_ltsw_uft_uat_mode_0_tables,
    },
    {
        .mode        = 1,
        .table_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uat_mode_1_tables),
        .table_db    = bcm56780_a0_dna_2_4_13_ltsw_uft_uat_mode_1_tables,
    }
};

/*!
 * \brief Tile modes configurations for egress UAT mode 0.
 */
static bcmint_uft_field_info_t
egress_uat_mode_0_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA30_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA30_E2T_00_MODE0s,
    }
};

static bcmint_uft_entry_info_t
egress_uat_mode_0_device_em_tile_entries[] = {
    {
        .field_cnt   = COUNTOF(egress_uat_mode_0_device_em_tile_entry_0_fields),
        .field_db    = egress_uat_mode_0_device_em_tile_entry_0_fields,
    }
};

static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_egress_uat_mode_0_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(egress_uat_mode_0_device_em_tile_entries),
        .entry_db    = egress_uat_mode_0_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for egress UAT mode 1.
 */
static bcmint_uft_field_info_t
egress_uat_mode_1_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA30_E2T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA30_E2T_00_MODE1s,
    }
};

static bcmint_uft_entry_info_t
egress_uat_mode_1_device_em_tile_entries[] = {
    {
        .field_cnt   = COUNTOF(egress_uat_mode_1_device_em_tile_entry_0_fields),
        .field_db    = egress_uat_mode_1_device_em_tile_entry_0_fields,
    }
};

static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_egress_uat_mode_1_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(egress_uat_mode_1_device_em_tile_entries),
        .entry_db    = egress_uat_mode_1_device_em_tile_entries,
    }
};

/*!
 * \brief DNA_2_4_13 egress UAT mode database.
 */
static bcmint_uft_mode_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_egress_uat_mode[] = {
    {
        .mode        = 0,
        .table_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_egress_uat_mode_0_tables),
        .table_db    = bcm56780_a0_dna_2_4_13_ltsw_uft_egress_uat_mode_0_tables,
    },
    {
        .mode        = 1,
        .table_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_egress_uat_mode_1_tables),
        .table_db    = bcm56780_a0_dna_2_4_13_ltsw_uft_egress_uat_mode_1_tables,
    }
};

/*!
 * \brief Tile modes configurations for tunnel mode 0.
 */
static bcmint_uft_field_info_t
tunnel_mode_0_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA10_I1T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA10_I1T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_0_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA10_I1T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA10_I1T_01_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_0_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA10_I1T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA10_I1T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_0_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_0_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_0_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_0_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_05s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_05_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_0_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA130_I1T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA130_I1T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_0_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA130_I1T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA130_I1T_01_MODE0s,
    }
};

static bcmint_uft_entry_info_t
tunnel_mode_0_device_em_tile_entries[] = {
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_0_device_em_tile_entry_0_fields),
        .field_db    = tunnel_mode_0_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_0_device_em_tile_entry_1_fields),
        .field_db    = tunnel_mode_0_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_0_device_em_tile_entry_2_fields),
        .field_db    = tunnel_mode_0_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_0_device_em_tile_entry_3_fields),
        .field_db    = tunnel_mode_0_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_0_device_em_tile_entry_4_fields),
        .field_db    = tunnel_mode_0_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_0_device_em_tile_entry_5_fields),
        .field_db    = tunnel_mode_0_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_0_device_em_tile_entry_6_fields),
        .field_db    = tunnel_mode_0_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_0_device_em_tile_entry_7_fields),
        .field_db    = tunnel_mode_0_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_0_device_em_tile_entry_8_fields),
        .field_db    = tunnel_mode_0_device_em_tile_entry_8_fields,
    }
};

static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode_0_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(tunnel_mode_0_device_em_tile_entries),
        .entry_db    = tunnel_mode_0_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for tunnel mode 1.
 */
static bcmint_uft_field_info_t
tunnel_mode_1_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA10_I1T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA10_I1T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_1_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA10_I1T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA10_I1T_01_MODE1s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_1_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA10_I1T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA10_I1T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_1_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_1_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_1_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_03_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_1_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_05s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_05_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_1_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA130_I1T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA130_I1T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_1_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA130_I1T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA130_I1T_01_MODE1s,
    }
};

static bcmint_uft_entry_info_t
tunnel_mode_1_device_em_tile_entries[] = {
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_1_device_em_tile_entry_0_fields),
        .field_db    = tunnel_mode_1_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_1_device_em_tile_entry_1_fields),
        .field_db    = tunnel_mode_1_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_1_device_em_tile_entry_2_fields),
        .field_db    = tunnel_mode_1_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_1_device_em_tile_entry_3_fields),
        .field_db    = tunnel_mode_1_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_1_device_em_tile_entry_4_fields),
        .field_db    = tunnel_mode_1_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_1_device_em_tile_entry_5_fields),
        .field_db    = tunnel_mode_1_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_1_device_em_tile_entry_6_fields),
        .field_db    = tunnel_mode_1_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_1_device_em_tile_entry_7_fields),
        .field_db    = tunnel_mode_1_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_1_device_em_tile_entry_8_fields),
        .field_db    = tunnel_mode_1_device_em_tile_entry_8_fields,
    }
};

static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode_1_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(tunnel_mode_1_device_em_tile_entries),
        .entry_db    = tunnel_mode_1_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for tunnel mode 2.
 */
static bcmint_uft_field_info_t
tunnel_mode_2_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA10_I1T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA10_I1T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_2_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA10_I1T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA10_I1T_01_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_2_device_em_tile_entry_2_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA10_I1T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA10_I1T_02_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_2_device_em_tile_entry_3_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_00_MODE1s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_2_device_em_tile_entry_4_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_02s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_02_MODE1s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_2_device_em_tile_entry_5_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_03s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_03_MODE1s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_2_device_em_tile_entry_6_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = EFTA20_I1T_05s,
    },
    {
        .field_name = MODEs,
        .symbol     = EFTA20_I1T_05_MODE1s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_2_device_em_tile_entry_7_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA130_I1T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA130_I1T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
tunnel_mode_2_device_em_tile_entry_8_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA130_I1T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA130_I1T_01_MODE0s,
    }
};

static bcmint_uft_entry_info_t
tunnel_mode_2_device_em_tile_entries[] = {
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_2_device_em_tile_entry_0_fields),
        .field_db    = tunnel_mode_2_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_2_device_em_tile_entry_1_fields),
        .field_db    = tunnel_mode_2_device_em_tile_entry_1_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_2_device_em_tile_entry_2_fields),
        .field_db    = tunnel_mode_2_device_em_tile_entry_2_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_2_device_em_tile_entry_3_fields),
        .field_db    = tunnel_mode_2_device_em_tile_entry_3_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_2_device_em_tile_entry_4_fields),
        .field_db    = tunnel_mode_2_device_em_tile_entry_4_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_2_device_em_tile_entry_5_fields),
        .field_db    = tunnel_mode_2_device_em_tile_entry_5_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_2_device_em_tile_entry_6_fields),
        .field_db    = tunnel_mode_2_device_em_tile_entry_6_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_2_device_em_tile_entry_7_fields),
        .field_db    = tunnel_mode_2_device_em_tile_entry_7_fields,
    },
    {
        .entry_attr  = HASHs,
        .field_cnt   = COUNTOF(tunnel_mode_2_device_em_tile_entry_8_fields),
        .field_db    = tunnel_mode_2_device_em_tile_entry_8_fields,
    }
};

static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode_2_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(tunnel_mode_2_device_em_tile_entries),
        .entry_db    = tunnel_mode_2_device_em_tile_entries,
    }
};

/*!
 * \brief DNA_2_4_13 tunnel mode database.
 */
static bcmint_uft_mode_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode[] = {
    {
        .mode        = 0,
        .table_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode_0_tables),
        .table_db    = bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode_0_tables,
    },
    {
        .mode        = 1,
        .table_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode_1_tables),
        .table_db    = bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode_1_tables,
    },
    {
        .mode        = 2,
        .table_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode_2_tables),
        .table_db    = bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode_2_tables,
    }
};

/*!
 * \brief Tile modes configurations for VFP mode 0.
 */
static bcmint_uft_field_info_t
vfp_mode_0_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA40_T4T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA40_T4T_00_MODE0s,
    }
};

static bcmint_uft_field_info_t
vfp_mode_0_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA40_T4T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA40_T4T_01_MODE0s,
    }
};

static bcmint_uft_entry_info_t
vfp_mode_0_device_em_tile_entries[] = {
    {
        .entry_attr  = TCAMs,
        .field_cnt   = COUNTOF(vfp_mode_0_device_em_tile_entry_0_fields),
        .field_db    = vfp_mode_0_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = TCAMs,
        .field_cnt   = COUNTOF(vfp_mode_0_device_em_tile_entry_1_fields),
        .field_db    = vfp_mode_0_device_em_tile_entry_1_fields,
    }
};

static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_vfp_mode_0_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(vfp_mode_0_device_em_tile_entries),
        .entry_db    = vfp_mode_0_device_em_tile_entries,
    }
};

/*!
 * \brief Tile modes configurations for VFP mode 1.
 */
static bcmint_uft_field_info_t
vfp_mode_1_device_em_tile_entry_0_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA40_T4T_00s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA40_T4T_00_MODE1s,
    }
};

static bcmint_uft_field_info_t
vfp_mode_1_device_em_tile_entry_1_fields[] = {
    {
        .field_name = DEVICE_EM_TILE_IDs,
        .symbol     = IFTA40_T4T_01s,
    },
    {
        .field_name = MODEs,
        .symbol     = IFTA40_T4T_01_MODE1s,
    }
};

static bcmint_uft_entry_info_t
vfp_mode_1_device_em_tile_entries[] = {
    {
        .entry_attr  = TCAMs,
        .field_cnt   = COUNTOF(vfp_mode_1_device_em_tile_entry_0_fields),
        .field_db    = vfp_mode_1_device_em_tile_entry_0_fields,
    },
    {
        .entry_attr  = TCAMs,
        .field_cnt   = COUNTOF(vfp_mode_1_device_em_tile_entry_1_fields),
        .field_db    = vfp_mode_1_device_em_tile_entry_1_fields,
    }
};

static bcmint_uft_table_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_vfp_mode_1_tables[] = {
    {
        .table_name  = DEVICE_EM_TILEs,
        .entry_cnt   = COUNTOF(vfp_mode_1_device_em_tile_entries),
        .entry_db    = vfp_mode_1_device_em_tile_entries,
    }
};

/*!
 * \brief DNA_2_4_13 VFP mode database.
 */
static bcmint_uft_mode_info_t
bcm56780_a0_dna_2_4_13_ltsw_uft_vfp_mode[] = {
    {
        .mode        = 0,
        .table_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_vfp_mode_0_tables),
        .table_db    = bcm56780_a0_dna_2_4_13_ltsw_uft_vfp_mode_0_tables,
    },
    {
        .mode        = 1,
        .table_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_vfp_mode_1_tables),
        .table_db    = bcm56780_a0_dna_2_4_13_ltsw_uft_vfp_mode_1_tables,
    }
};

/*!
 * \brief Get the variant specific database.
 *
 * \param [in] unit Unit Number.
 * \param [out] uft_db Hash database structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_dna_2_4_13_ltsw_uft_db_get(int unit, bcmint_uft_db_t *uft_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(uft_db, SHR_E_PARAM);

    uft_db->num_lt_array     = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_def_lt_array);
    uft_db->def_lt_array     = bcm56780_a0_dna_2_4_13_ltsw_uft_def_lt_array;

    uft_db->move_depth_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_move_depth_map);
    uft_db->move_depth_map   = bcm56780_a0_dna_2_4_13_ltsw_uft_move_depth_map;

    uft_db->hash_table_cnt   = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_table_map);
    uft_db->hash_table_map   = bcm56780_a0_dna_2_4_13_ltsw_uft_table_map;

    uft_db->bank_id_cnt      = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_bank_id_map);
    uft_db->bank_id_map      = bcm56780_a0_dna_2_4_13_ltsw_uft_bank_id_map;

    uft_db->tile_bank_cnt    = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_tile_bank_map);
    uft_db->tile_bank_map    = bcm56780_a0_dna_2_4_13_ltsw_uft_tile_bank_map;

    uft_db->uft_mode_cnt     = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode);
    uft_db->uft_mode_db      = bcm56780_a0_dna_2_4_13_ltsw_uft_uft_mode;

    uft_db->uat_mode_cnt     = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_uat_mode);
    uft_db->uat_mode_db      = bcm56780_a0_dna_2_4_13_ltsw_uft_uat_mode;

    uft_db->egress_uat_mode_cnt = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_egress_uat_mode);
    uft_db->egress_uat_mode_db  = bcm56780_a0_dna_2_4_13_ltsw_uft_egress_uat_mode;

    uft_db->tunnel_mode_cnt  = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode);
    uft_db->tunnel_mode_db   = bcm56780_a0_dna_2_4_13_ltsw_uft_tunnel_mode;

    uft_db->vfp_mode_cnt     = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_vfp_mode);
    uft_db->vfp_mode_db      = bcm56780_a0_dna_2_4_13_ltsw_uft_vfp_mode;

    uft_db->property_cnt     = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_uft_property_map);
    uft_db->property_map     = bcm56780_a0_dna_2_4_13_ltsw_uft_property_map;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash driver function variable for bcm56780_a0 DNA_2_4_13 device.
 */
static mbcm_ltsw_uft_drv_t bcm56780_a0_dna_2_4_13_ltsw_uft_drv = {
    .uft_db_get = bcm56780_a0_dna_2_4_13_ltsw_uft_db_get
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_dna_2_4_13_ltsw_uft_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_drv_set(unit, &bcm56780_a0_dna_2_4_13_ltsw_uft_drv));

exit:
    SHR_FUNC_EXIT();
}

