/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators  
 *----------------------------------------------------------------------*/
/****************************************************************************
 * Core Enums.
 */
/****************************************************************************
 * Enums: an_x4_abilities_cl36TxEEEStates_l
 */
#define an_x4_abilities_cl36TxEEEStates_l_TX_REFRESH       8
#define an_x4_abilities_cl36TxEEEStates_l_TX_QUIET         4
#define an_x4_abilities_cl36TxEEEStates_l_TX_SLEEP         2
#define an_x4_abilities_cl36TxEEEStates_l_TX_ACTIVE        1

/****************************************************************************
 * Enums: an_x4_abilities_cl36TxEEEStates_c
 */
#define an_x4_abilities_cl36TxEEEStates_c_TX_REFRESH       3
#define an_x4_abilities_cl36TxEEEStates_c_TX_QUIET         2
#define an_x4_abilities_cl36TxEEEStates_c_TX_SLEEP         1
#define an_x4_abilities_cl36TxEEEStates_c_TX_ACTIVE        0

/****************************************************************************
 * Enums: an_x4_abilities_cl49TxEEEStates_l
 */
#define an_x4_abilities_cl49TxEEEStates_l_SCR_RESET_2      64
#define an_x4_abilities_cl49TxEEEStates_l_SCR_RESET_1      32
#define an_x4_abilities_cl49TxEEEStates_l_TX_WAKE          16
#define an_x4_abilities_cl49TxEEEStates_l_TX_REFRESH       8
#define an_x4_abilities_cl49TxEEEStates_l_TX_QUIET         4
#define an_x4_abilities_cl49TxEEEStates_l_TX_SLEEP         2
#define an_x4_abilities_cl49TxEEEStates_l_TX_ACTIVE        1

/****************************************************************************
 * Enums: an_x4_abilities_fec_req_enum
 */
#define an_x4_abilities_fec_req_enum_FEC_not_supported     0
#define an_x4_abilities_fec_req_enum_FEC_supported_but_not_requested 1
#define an_x4_abilities_fec_req_enum_invalid_setting       2
#define an_x4_abilities_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: an_x4_abilities_cl73_pause_enum
 */
#define an_x4_abilities_cl73_pause_enum_No_PAUSE_ability   0
#define an_x4_abilities_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define an_x4_abilities_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define an_x4_abilities_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: an_x4_abilities_cl49TxEEEStates_c
 */
#define an_x4_abilities_cl49TxEEEStates_c_SCR_RESET_2      6
#define an_x4_abilities_cl49TxEEEStates_c_SCR_RESET_1      5
#define an_x4_abilities_cl49TxEEEStates_c_TX_WAKE          4
#define an_x4_abilities_cl49TxEEEStates_c_TX_REFRESH       3
#define an_x4_abilities_cl49TxEEEStates_c_TX_QUIET         2
#define an_x4_abilities_cl49TxEEEStates_c_TX_SLEEP         1
#define an_x4_abilities_cl49TxEEEStates_c_TX_ACTIVE        0

/****************************************************************************
 * Enums: an_x4_abilities_cl36RxEEEStates_l
 */
#define an_x4_abilities_cl36RxEEEStates_l_RX_LINK_FAIL     32
#define an_x4_abilities_cl36RxEEEStates_l_RX_WTF           16
#define an_x4_abilities_cl36RxEEEStates_l_RX_WAKE          8
#define an_x4_abilities_cl36RxEEEStates_l_RX_QUIET         4
#define an_x4_abilities_cl36RxEEEStates_l_RX_SLEEP         2
#define an_x4_abilities_cl36RxEEEStates_l_RX_ACTIVE        1

/****************************************************************************
 * Enums: an_x4_abilities_cl36RxEEEStates_c
 */
#define an_x4_abilities_cl36RxEEEStates_c_RX_LINK_FAIL     5
#define an_x4_abilities_cl36RxEEEStates_c_RX_WTF           4
#define an_x4_abilities_cl36RxEEEStates_c_RX_WAKE          3
#define an_x4_abilities_cl36RxEEEStates_c_RX_QUIET         2
#define an_x4_abilities_cl36RxEEEStates_c_RX_SLEEP         1
#define an_x4_abilities_cl36RxEEEStates_c_RX_ACTIVE        0

/****************************************************************************
 * Enums: an_x4_abilities_cl49RxEEEStates_l
 */
#define an_x4_abilities_cl49RxEEEStates_l_RX_LINK_FAIL     32
#define an_x4_abilities_cl49RxEEEStates_l_RX_WTF           16
#define an_x4_abilities_cl49RxEEEStates_l_RX_WAKE          8
#define an_x4_abilities_cl49RxEEEStates_l_RX_QUIET         4
#define an_x4_abilities_cl49RxEEEStates_l_RX_SLEEP         2
#define an_x4_abilities_cl49RxEEEStates_l_RX_ACTIVE        1

/****************************************************************************
 * Enums: an_x4_abilities_cl49RxEEEStates_c
 */
#define an_x4_abilities_cl49RxEEEStates_c_RX_LINK_FAIL     5
#define an_x4_abilities_cl49RxEEEStates_c_RX_WTF           4
#define an_x4_abilities_cl49RxEEEStates_c_RX_WAKE          3
#define an_x4_abilities_cl49RxEEEStates_c_RX_QUIET         2
#define an_x4_abilities_cl49RxEEEStates_c_RX_SLEEP         1
#define an_x4_abilities_cl49RxEEEStates_c_RX_ACTIVE        0

/****************************************************************************
 * Enums: an_x4_abilities_cl48TxEEEStates_l
 */
#define an_x4_abilities_cl48TxEEEStates_l_TX_REFRESH       8
#define an_x4_abilities_cl48TxEEEStates_l_TX_QUIET         4
#define an_x4_abilities_cl48TxEEEStates_l_TX_SLEEP         2
#define an_x4_abilities_cl48TxEEEStates_l_TX_ACTIVE        1

/****************************************************************************
 * Enums: an_x4_abilities_cl48TxEEEStates_c
 */
#define an_x4_abilities_cl48TxEEEStates_c_TX_REFRESH       3
#define an_x4_abilities_cl48TxEEEStates_c_TX_QUIET         2
#define an_x4_abilities_cl48TxEEEStates_c_TX_SLEEP         1
#define an_x4_abilities_cl48TxEEEStates_c_TX_ACTIVE        0

/****************************************************************************
 * Enums: an_x4_abilities_cl48RxEEEStates_l
 */
#define an_x4_abilities_cl48RxEEEStates_l_RX_LINK_FAIL     32
#define an_x4_abilities_cl48RxEEEStates_l_RX_WAKE          16
#define an_x4_abilities_cl48RxEEEStates_l_RX_QUIET         8
#define an_x4_abilities_cl48RxEEEStates_l_RX_DEACT         4
#define an_x4_abilities_cl48RxEEEStates_l_RX_SLEEP         2
#define an_x4_abilities_cl48RxEEEStates_l_RX_ACTIVE        1

/****************************************************************************
 * Enums: an_x4_abilities_cl48RxEEEStates_c
 */
#define an_x4_abilities_cl48RxEEEStates_c_RX_LINK_FAIL     5
#define an_x4_abilities_cl48RxEEEStates_c_RX_WAKE          4
#define an_x4_abilities_cl48RxEEEStates_c_RX_QUIET         3
#define an_x4_abilities_cl48RxEEEStates_c_RX_DEACT         2
#define an_x4_abilities_cl48RxEEEStates_c_RX_SLEEP         1
#define an_x4_abilities_cl48RxEEEStates_c_RX_ACTIVE        0

/****************************************************************************
 * Enums: an_x4_abilities_IQP_Options
 */
#define an_x4_abilities_IQP_Options_i50uA                  0
#define an_x4_abilities_IQP_Options_i100uA                 1
#define an_x4_abilities_IQP_Options_i150uA                 2
#define an_x4_abilities_IQP_Options_i200uA                 3
#define an_x4_abilities_IQP_Options_i250uA                 4
#define an_x4_abilities_IQP_Options_i300uA                 5
#define an_x4_abilities_IQP_Options_i350uA                 6
#define an_x4_abilities_IQP_Options_i400uA                 7
#define an_x4_abilities_IQP_Options_i450uA                 8
#define an_x4_abilities_IQP_Options_i500uA                 9
#define an_x4_abilities_IQP_Options_i550uA                 10
#define an_x4_abilities_IQP_Options_i600uA                 11
#define an_x4_abilities_IQP_Options_i650uA                 12
#define an_x4_abilities_IQP_Options_i700uA                 13
#define an_x4_abilities_IQP_Options_i750uA                 14
#define an_x4_abilities_IQP_Options_i800uA                 15

/****************************************************************************
 * Enums: an_x4_abilities_IDriver_Options
 */
#define an_x4_abilities_IDriver_Options_v680mV             0
#define an_x4_abilities_IDriver_Options_v730mV             1
#define an_x4_abilities_IDriver_Options_v780mV             2
#define an_x4_abilities_IDriver_Options_v830mV             3
#define an_x4_abilities_IDriver_Options_v880mV             4
#define an_x4_abilities_IDriver_Options_v930mV             5
#define an_x4_abilities_IDriver_Options_v980mV             6
#define an_x4_abilities_IDriver_Options_v1010mV            7
#define an_x4_abilities_IDriver_Options_v1040mV            8
#define an_x4_abilities_IDriver_Options_v1060mV            9
#define an_x4_abilities_IDriver_Options_v1070mV            10
#define an_x4_abilities_IDriver_Options_v1080mV            11
#define an_x4_abilities_IDriver_Options_v1085mV            12
#define an_x4_abilities_IDriver_Options_v1090mV            13
#define an_x4_abilities_IDriver_Options_v1095mV            14
#define an_x4_abilities_IDriver_Options_v1100mV            15

/****************************************************************************
 * Enums: an_x4_abilities_operationModes
 */
#define an_x4_abilities_operationModes_XGXS                0
#define an_x4_abilities_operationModes_XGXG_nCC            1
#define an_x4_abilities_operationModes_Indlane_OS8         4
#define an_x4_abilities_operationModes_IndLane_OS5         5
#define an_x4_abilities_operationModes_PCI                 7
#define an_x4_abilities_operationModes_XGXS_nLQ            8
#define an_x4_abilities_operationModes_XGXS_nLQnCC         9
#define an_x4_abilities_operationModes_PBypass             10
#define an_x4_abilities_operationModes_PBypass_nDSK        11
#define an_x4_abilities_operationModes_ComboCoreMode       12
#define an_x4_abilities_operationModes_Clocks_off          15

/****************************************************************************
 * Enums: an_x4_abilities_actualSpeeds
 */
#define an_x4_abilities_actualSpeeds_dr_10M                0
#define an_x4_abilities_actualSpeeds_dr_100M               1
#define an_x4_abilities_actualSpeeds_dr_1G                 2
#define an_x4_abilities_actualSpeeds_dr_2p5G               3
#define an_x4_abilities_actualSpeeds_dr_5G_X4              4
#define an_x4_abilities_actualSpeeds_dr_6G_X4              5
#define an_x4_abilities_actualSpeeds_dr_10G_HiG            6
#define an_x4_abilities_actualSpeeds_dr_10G_CX4            7
#define an_x4_abilities_actualSpeeds_dr_12G_HiG            8
#define an_x4_abilities_actualSpeeds_dr_12p5G_X4           9
#define an_x4_abilities_actualSpeeds_dr_13G_X4             10
#define an_x4_abilities_actualSpeeds_dr_15G_X4             11
#define an_x4_abilities_actualSpeeds_dr_16G_X4             12
#define an_x4_abilities_actualSpeeds_dr_1G_KX              13
#define an_x4_abilities_actualSpeeds_dr_10G_KX4            14
#define an_x4_abilities_actualSpeeds_dr_10G_KR             15
#define an_x4_abilities_actualSpeeds_dr_5G                 16
#define an_x4_abilities_actualSpeeds_dr_6p4G               17
#define an_x4_abilities_actualSpeeds_dr_20G_X4             18
#define an_x4_abilities_actualSpeeds_dr_21G_X4             19
#define an_x4_abilities_actualSpeeds_dr_25G_X4             20
#define an_x4_abilities_actualSpeeds_dr_10G_HiG_DXGXS      21
#define an_x4_abilities_actualSpeeds_dr_10G_DXGXS          22
#define an_x4_abilities_actualSpeeds_dr_10p5G_HiG_DXGXS    23
#define an_x4_abilities_actualSpeeds_dr_10p5G_DXGXS        24
#define an_x4_abilities_actualSpeeds_dr_12p773G_HiG_DXGXS  25
#define an_x4_abilities_actualSpeeds_dr_12p773G_DXGXS      26
#define an_x4_abilities_actualSpeeds_dr_10G_XFI            27
#define an_x4_abilities_actualSpeeds_dr_40G                28
#define an_x4_abilities_actualSpeeds_dr_20G_HiG_DXGXS      29
#define an_x4_abilities_actualSpeeds_dr_20G_DXGXS          30
#define an_x4_abilities_actualSpeeds_dr_10G_SFI            31
#define an_x4_abilities_actualSpeeds_dr_31p5G              32
#define an_x4_abilities_actualSpeeds_dr_32p7G              33
#define an_x4_abilities_actualSpeeds_dr_20G_SCR            34
#define an_x4_abilities_actualSpeeds_dr_10G_HiG_DXGXS_SCR  35
#define an_x4_abilities_actualSpeeds_dr_10G_DXGXS_SCR      36
#define an_x4_abilities_actualSpeeds_dr_12G_R2             37
#define an_x4_abilities_actualSpeeds_dr_10G_X2             38
#define an_x4_abilities_actualSpeeds_dr_40G_KR4            39
#define an_x4_abilities_actualSpeeds_dr_40G_CR4            40
#define an_x4_abilities_actualSpeeds_dr_100G_CR10          41
#define an_x4_abilities_actualSpeeds_dr_15p75G_DXGXS       44
#define an_x4_abilities_actualSpeeds_dr_20G_KR2            57
#define an_x4_abilities_actualSpeeds_dr_20G_CR2            58

/****************************************************************************
 * Enums: an_x4_abilities_actualSpeedsMisc1
 */
#define an_x4_abilities_actualSpeedsMisc1_dr_2500BRCM_X1   16
#define an_x4_abilities_actualSpeedsMisc1_dr_5000BRCM_X4   17
#define an_x4_abilities_actualSpeedsMisc1_dr_6000BRCM_X4   18
#define an_x4_abilities_actualSpeedsMisc1_dr_10GHiGig_X4   19
#define an_x4_abilities_actualSpeedsMisc1_dr_10GBASE_CX4   20
#define an_x4_abilities_actualSpeedsMisc1_dr_12GHiGig_X4   21
#define an_x4_abilities_actualSpeedsMisc1_dr_12p5GHiGig_X4 22
#define an_x4_abilities_actualSpeedsMisc1_dr_13GHiGig_X4   23
#define an_x4_abilities_actualSpeedsMisc1_dr_15GHiGig_X4   24
#define an_x4_abilities_actualSpeedsMisc1_dr_16GHiGig_X4   25
#define an_x4_abilities_actualSpeedsMisc1_dr_5000BRCM_X1   26
#define an_x4_abilities_actualSpeedsMisc1_dr_6363BRCM_X1   27
#define an_x4_abilities_actualSpeedsMisc1_dr_20GHiGig_X4   28
#define an_x4_abilities_actualSpeedsMisc1_dr_21GHiGig_X4   29
#define an_x4_abilities_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define an_x4_abilities_actualSpeedsMisc1_dr_10G_HiG_DXGXS 31

/****************************************************************************
 * Enums: an_x4_abilities_IndLaneModes
 */
#define an_x4_abilities_IndLaneModes_SWSDR_div2            0
#define an_x4_abilities_IndLaneModes_SWSDR_div1            1
#define an_x4_abilities_IndLaneModes_DWSDR_div2            2
#define an_x4_abilities_IndLaneModes_DWSDR_div1            3

/****************************************************************************
 * Enums: an_x4_abilities_prbsSelect
 */
#define an_x4_abilities_prbsSelect_prbs7                   0
#define an_x4_abilities_prbsSelect_prbs15                  1
#define an_x4_abilities_prbsSelect_prbs23                  2
#define an_x4_abilities_prbsSelect_prbs31                  3

/****************************************************************************
 * Enums: an_x4_abilities_vcoDivider
 */
#define an_x4_abilities_vcoDivider_div32                   0
#define an_x4_abilities_vcoDivider_div36                   1
#define an_x4_abilities_vcoDivider_div40                   2
#define an_x4_abilities_vcoDivider_div42                   3
#define an_x4_abilities_vcoDivider_div48                   4
#define an_x4_abilities_vcoDivider_div50                   5
#define an_x4_abilities_vcoDivider_div52                   6
#define an_x4_abilities_vcoDivider_div54                   7
#define an_x4_abilities_vcoDivider_div60                   8
#define an_x4_abilities_vcoDivider_div64                   9
#define an_x4_abilities_vcoDivider_div66                   10
#define an_x4_abilities_vcoDivider_div68                   11
#define an_x4_abilities_vcoDivider_div70                   12
#define an_x4_abilities_vcoDivider_div80                   13
#define an_x4_abilities_vcoDivider_div92                   14
#define an_x4_abilities_vcoDivider_div100                  15

/****************************************************************************
 * Enums: an_x4_abilities_refClkSelect
 */
#define an_x4_abilities_refClkSelect_clk_25MHz             0
#define an_x4_abilities_refClkSelect_clk_100MHz            1
#define an_x4_abilities_refClkSelect_clk_125MHz            2
#define an_x4_abilities_refClkSelect_clk_156p25MHz         3
#define an_x4_abilities_refClkSelect_clk_187p5MHz          4
#define an_x4_abilities_refClkSelect_clk_161p25Mhz         5
#define an_x4_abilities_refClkSelect_clk_50Mhz             6
#define an_x4_abilities_refClkSelect_clk_106p25Mhz         7

/****************************************************************************
 * Enums: an_x4_abilities_aerMMDdevTypeSelect
 */
#define an_x4_abilities_aerMMDdevTypeSelect_combo_core     0
#define an_x4_abilities_aerMMDdevTypeSelect_PMA_PMD        1
#define an_x4_abilities_aerMMDdevTypeSelect_PCS            3
#define an_x4_abilities_aerMMDdevTypeSelect_PHY            4
#define an_x4_abilities_aerMMDdevTypeSelect_DTE            5
#define an_x4_abilities_aerMMDdevTypeSelect_CL73_AN        7

/****************************************************************************
 * Enums: an_x4_abilities_aerMMDportSelect
 */
#define an_x4_abilities_aerMMDportSelect_ln0               0
#define an_x4_abilities_aerMMDportSelect_ln1               1
#define an_x4_abilities_aerMMDportSelect_ln2               2
#define an_x4_abilities_aerMMDportSelect_ln3               3
#define an_x4_abilities_aerMMDportSelect_BCST_ln0_1_2_3    511
#define an_x4_abilities_aerMMDportSelect_BCST_ln0_1        512
#define an_x4_abilities_aerMMDportSelect_BCST_ln2_3        513

/****************************************************************************
 * Enums: an_x4_abilities_firmwareModeSelect
 */
#define an_x4_abilities_firmwareModeSelect_DEFAULT         0
#define an_x4_abilities_firmwareModeSelect_SFP_OPT_LR      1
#define an_x4_abilities_firmwareModeSelect_SFP_DAC         2
#define an_x4_abilities_firmwareModeSelect_XLAUI           3
#define an_x4_abilities_firmwareModeSelect_LONG_CH_6G      4

/****************************************************************************
 * Enums: an_x4_abilities_tempIdxSelect
 */
#define an_x4_abilities_tempIdxSelect_LTE__22p9C           15
#define an_x4_abilities_tempIdxSelect_LTE__12p6C           14
#define an_x4_abilities_tempIdxSelect_LTE__3p0C            13
#define an_x4_abilities_tempIdxSelect_LTE_6p7C             12
#define an_x4_abilities_tempIdxSelect_LTE_16p4C            11
#define an_x4_abilities_tempIdxSelect_LTE_26p6C            10
#define an_x4_abilities_tempIdxSelect_LTE_36p3C            9
#define an_x4_abilities_tempIdxSelect_LTE_46p0C            8
#define an_x4_abilities_tempIdxSelect_LTE_56p2C            7
#define an_x4_abilities_tempIdxSelect_LTE_65p9C            6
#define an_x4_abilities_tempIdxSelect_LTE_75p6C            5
#define an_x4_abilities_tempIdxSelect_LTE_85p3C            4
#define an_x4_abilities_tempIdxSelect_LTE_95p5C            3
#define an_x4_abilities_tempIdxSelect_LTE_105p2C           2
#define an_x4_abilities_tempIdxSelect_LTE_114p9C           1
#define an_x4_abilities_tempIdxSelect_LTE_125p1C           0

/****************************************************************************
 * Enums: an_x4_abilities_port_mode
 */
#define an_x4_abilities_port_mode_QUAD_PORT                0
#define an_x4_abilities_port_mode_TRI_1_PORT               1
#define an_x4_abilities_port_mode_TRI_2_PORT               2
#define an_x4_abilities_port_mode_DUAL_PORT                3
#define an_x4_abilities_port_mode_SINGLE_PORT              4

/****************************************************************************
 * Enums: an_x4_abilities_rev_letter_enum
 */
#define an_x4_abilities_rev_letter_enum_REV_A              0
#define an_x4_abilities_rev_letter_enum_REV_B              1
#define an_x4_abilities_rev_letter_enum_REV_C              2
#define an_x4_abilities_rev_letter_enum_REV_D              3

/****************************************************************************
 * Enums: an_x4_abilities_rev_number_enum
 */
#define an_x4_abilities_rev_number_enum_REV_0              0
#define an_x4_abilities_rev_number_enum_REV_1              1
#define an_x4_abilities_rev_number_enum_REV_2              2
#define an_x4_abilities_rev_number_enum_REV_3              3
#define an_x4_abilities_rev_number_enum_REV_4              4
#define an_x4_abilities_rev_number_enum_REV_5              5
#define an_x4_abilities_rev_number_enum_REV_6              6
#define an_x4_abilities_rev_number_enum_REV_7              7

/****************************************************************************
 * Enums: an_x4_abilities_bonding_enum
 */
#define an_x4_abilities_bonding_enum_WIRE_BOND             0
#define an_x4_abilities_bonding_enum_FLIP_CHIP             1

/****************************************************************************
 * Enums: an_x4_abilities_tech_process
 */
#define an_x4_abilities_tech_process_PROCESS_90NM          0
#define an_x4_abilities_tech_process_PROCESS_65NM          1
#define an_x4_abilities_tech_process_PROCESS_40NM          2
#define an_x4_abilities_tech_process_PROCESS_28NM          3

/****************************************************************************
 * Enums: an_x4_abilities_model_num
 */
#define an_x4_abilities_model_num_SERDES_CL73              0
#define an_x4_abilities_model_num_XGXS_16G                 1
#define an_x4_abilities_model_num_HYPERCORE                2
#define an_x4_abilities_model_num_HYPERLITE                3
#define an_x4_abilities_model_num_PCIE_G2_PIPE             4
#define an_x4_abilities_model_num_SERDES_1p25GBd           5
#define an_x4_abilities_model_num_SATA2                    6
#define an_x4_abilities_model_num_QSGMII                   7
#define an_x4_abilities_model_num_XGXS10G                  8
#define an_x4_abilities_model_num_WARPCORE                 9
#define an_x4_abilities_model_num_XFICORE                  10
#define an_x4_abilities_model_num_RXFI                     11
#define an_x4_abilities_model_num_WARPLITE                 12
#define an_x4_abilities_model_num_PENTACORE                13
#define an_x4_abilities_model_num_ESM                      14
#define an_x4_abilities_model_num_QUAD_SGMII               15
#define an_x4_abilities_model_num_WARPCORE_3               16
#define an_x4_abilities_model_num_TSC                      17
#define an_x4_abilities_model_num_TSC4E                    18
#define an_x4_abilities_model_num_TSC12E                   19
#define an_x4_abilities_model_num_TSC4F                    20
#define an_x4_abilities_model_num_XGXS_CL73_90NM           29
#define an_x4_abilities_model_num_SERDES_CL73_90NM         30
#define an_x4_abilities_model_num_WARPCORE3                32
#define an_x4_abilities_model_num_WARPCORE4_TSC            33
#define an_x4_abilities_model_num_RXAUI                    34

/****************************************************************************
 * Enums: an_x4_abilities_payload
 */
#define an_x4_abilities_payload_REPEAT_2_BYTES             0
#define an_x4_abilities_payload_RAMPING                    1
#define an_x4_abilities_payload_CL48_CRPAT                 2
#define an_x4_abilities_payload_CL48_CJPAT                 3
#define an_x4_abilities_payload_CL36_LONG_CRPAT            4
#define an_x4_abilities_payload_CL36_SHORT_CRPAT           5

/****************************************************************************
 * Enums: an_x4_abilities_sc
 */
#define an_x4_abilities_sc_S_10G_CR1                       0
#define an_x4_abilities_sc_S_10G_KR1                       1
#define an_x4_abilities_sc_S_10G_X1                        2
#define an_x4_abilities_sc_S_10G_HG2_CR1                   4
#define an_x4_abilities_sc_S_10G_HG2_KR1                   5
#define an_x4_abilities_sc_S_10G_HG2_X1                    6
#define an_x4_abilities_sc_S_20G_CR1                       8
#define an_x4_abilities_sc_S_20G_KR1                       9
#define an_x4_abilities_sc_S_20G_X1                        10
#define an_x4_abilities_sc_S_20G_HG2_CR1                   12
#define an_x4_abilities_sc_S_20G_HG2_KR1                   13
#define an_x4_abilities_sc_S_20G_HG2_X1                    14
#define an_x4_abilities_sc_S_25G_CR1                       16
#define an_x4_abilities_sc_S_25G_KR1                       17
#define an_x4_abilities_sc_S_25G_X1                        18
#define an_x4_abilities_sc_S_25G_HG2_CR1                   20
#define an_x4_abilities_sc_S_25G_HG2_KR1                   21
#define an_x4_abilities_sc_S_25G_HG2_X1                    22
#define an_x4_abilities_sc_S_20G_CR2                       24
#define an_x4_abilities_sc_S_20G_KR2                       25
#define an_x4_abilities_sc_S_20G_X2                        26
#define an_x4_abilities_sc_S_20G_HG2_CR2                   28
#define an_x4_abilities_sc_S_20G_HG2_KR2                   29
#define an_x4_abilities_sc_S_20G_HG2_X2                    30
#define an_x4_abilities_sc_S_40G_CR2                       32
#define an_x4_abilities_sc_S_40G_KR2                       33
#define an_x4_abilities_sc_S_40G_X2                        34
#define an_x4_abilities_sc_S_40G_HG2_CR2                   36
#define an_x4_abilities_sc_S_40G_HG2_KR2                   37
#define an_x4_abilities_sc_S_40G_HG2_X2                    38
#define an_x4_abilities_sc_S_40G_CR4                       40
#define an_x4_abilities_sc_S_40G_KR4                       41
#define an_x4_abilities_sc_S_40G_X4                        42
#define an_x4_abilities_sc_S_40G_HG2_CR4                   44
#define an_x4_abilities_sc_S_40G_HG2_KR4                   45
#define an_x4_abilities_sc_S_40G_HG2_X4                    46
#define an_x4_abilities_sc_S_50G_CR2                       48
#define an_x4_abilities_sc_S_50G_KR2                       49
#define an_x4_abilities_sc_S_50G_X2                        50
#define an_x4_abilities_sc_S_50G_HG2_CR2                   52
#define an_x4_abilities_sc_S_50G_HG2_KR2                   53
#define an_x4_abilities_sc_S_50G_HG2_X2                    54
#define an_x4_abilities_sc_S_50G_CR4                       56
#define an_x4_abilities_sc_S_50G_KR4                       57
#define an_x4_abilities_sc_S_50G_X4                        58
#define an_x4_abilities_sc_S_50G_HG2_CR4                   60
#define an_x4_abilities_sc_S_50G_HG2_KR4                   61
#define an_x4_abilities_sc_S_50G_HG2_X4                    62
#define an_x4_abilities_sc_S_100G_CR4                      64
#define an_x4_abilities_sc_S_100G_KR4                      65
#define an_x4_abilities_sc_S_100G_X4                       66
#define an_x4_abilities_sc_S_100G_HG2_CR4                  68
#define an_x4_abilities_sc_S_100G_HG2_KR4                  69
#define an_x4_abilities_sc_S_100G_HG2_X4                   70
#define an_x4_abilities_sc_S_CL73_20GVCO                   72
#define an_x4_abilities_sc_S_CL73_25GVCO                   80
#define an_x4_abilities_sc_S_CL36_20GVCO                   88
#define an_x4_abilities_sc_S_CL36_25GVCO                   96

/****************************************************************************
 * Enums: an_x4_abilities_t_fifo_modes
 */
#define an_x4_abilities_t_fifo_modes_T_FIFO_MODE_BYPASS    0
#define an_x4_abilities_t_fifo_modes_T_FIFO_MODE_40G       1
#define an_x4_abilities_t_fifo_modes_T_FIFO_MODE_100G      2
#define an_x4_abilities_t_fifo_modes_T_FIFO_MODE_20G       3

/****************************************************************************
 * Enums: an_x4_abilities_t_enc_modes
 */
#define an_x4_abilities_t_enc_modes_T_ENC_MODE_BYPASS      0
#define an_x4_abilities_t_enc_modes_T_ENC_MODE_CL49        1
#define an_x4_abilities_t_enc_modes_T_ENC_MODE_CL82        2

/****************************************************************************
 * Enums: an_x4_abilities_btmx_mode
 */
#define an_x4_abilities_btmx_mode_BS_BTMX_MODE_1to1        0
#define an_x4_abilities_btmx_mode_BS_BTMX_MODE_2to1        1
#define an_x4_abilities_btmx_mode_BS_BTMX_MODE_5to1        2

/****************************************************************************
 * Enums: an_x4_abilities_t_type_cl82
 */
#define an_x4_abilities_t_type_cl82_T_TYPE_B1              5
#define an_x4_abilities_t_type_cl82_T_TYPE_C               4
#define an_x4_abilities_t_type_cl82_T_TYPE_S               3
#define an_x4_abilities_t_type_cl82_T_TYPE_T               2
#define an_x4_abilities_t_type_cl82_T_TYPE_D               1
#define an_x4_abilities_t_type_cl82_T_TYPE_E               0

/****************************************************************************
 * Enums: an_x4_abilities_txsm_state_cl82
 */
#define an_x4_abilities_txsm_state_cl82_TX_HIG_END         6
#define an_x4_abilities_txsm_state_cl82_TX_HIG_START       5
#define an_x4_abilities_txsm_state_cl82_TX_E               4
#define an_x4_abilities_txsm_state_cl82_TX_T               3
#define an_x4_abilities_txsm_state_cl82_TX_D               2
#define an_x4_abilities_txsm_state_cl82_TX_C               1
#define an_x4_abilities_txsm_state_cl82_TX_INIT            0

/****************************************************************************
 * Enums: an_x4_abilities_ltxsm_state_cl82
 */
#define an_x4_abilities_ltxsm_state_cl82_TX_HIG_END        64
#define an_x4_abilities_ltxsm_state_cl82_TX_HIG_START      32
#define an_x4_abilities_ltxsm_state_cl82_TX_E              16
#define an_x4_abilities_ltxsm_state_cl82_TX_T              8
#define an_x4_abilities_ltxsm_state_cl82_TX_D              4
#define an_x4_abilities_ltxsm_state_cl82_TX_C              2
#define an_x4_abilities_ltxsm_state_cl82_TX_INIT           1

/****************************************************************************
 * Enums: an_x4_abilities_r_type_coded_cl82
 */
#define an_x4_abilities_r_type_coded_cl82_R_TYPE_B1        32
#define an_x4_abilities_r_type_coded_cl82_R_TYPE_C         16
#define an_x4_abilities_r_type_coded_cl82_R_TYPE_S         8
#define an_x4_abilities_r_type_coded_cl82_R_TYPE_T         4
#define an_x4_abilities_r_type_coded_cl82_R_TYPE_D         2
#define an_x4_abilities_r_type_coded_cl82_R_TYPE_E         1

/****************************************************************************
 * Enums: an_x4_abilities_rxsm_state_cl82
 */
#define an_x4_abilities_rxsm_state_cl82_RX_HIG_END         64
#define an_x4_abilities_rxsm_state_cl82_RX_HIG_START       32
#define an_x4_abilities_rxsm_state_cl82_RX_E               16
#define an_x4_abilities_rxsm_state_cl82_RX_T               8
#define an_x4_abilities_rxsm_state_cl82_RX_D               4
#define an_x4_abilities_rxsm_state_cl82_RX_C               2
#define an_x4_abilities_rxsm_state_cl82_RX_INIT            1

/****************************************************************************
 * Enums: an_x4_abilities_deskew_state
 */
#define an_x4_abilities_deskew_state_ALIGN_ACQUIRED        2
#define an_x4_abilities_deskew_state_LOSS_OF_ALIGNMENT     1

/****************************************************************************
 * Enums: an_x4_abilities_os_mode_enum
 */
#define an_x4_abilities_os_mode_enum_OS_MODE_1             0
#define an_x4_abilities_os_mode_enum_OS_MODE_2             1
#define an_x4_abilities_os_mode_enum_OS_MODE_4             2
#define an_x4_abilities_os_mode_enum_OS_MODE_16p5          8
#define an_x4_abilities_os_mode_enum_OS_MODE_20p625        12

/****************************************************************************
 * Enums: an_x4_abilities_scr_modes
 */
#define an_x4_abilities_scr_modes_T_SCR_MODE_BYPASS        0
#define an_x4_abilities_scr_modes_T_SCR_MODE_CL49          1
#define an_x4_abilities_scr_modes_T_SCR_MODE_40G_2_LANE    2
#define an_x4_abilities_scr_modes_T_SCR_MODE_100G          3
#define an_x4_abilities_scr_modes_T_SCR_MODE_20G           4
#define an_x4_abilities_scr_modes_T_SCR_MODE_40G_4_LANE    5

/****************************************************************************
 * Enums: an_x4_abilities_descr_modes
 */
#define an_x4_abilities_descr_modes_R_DESCR_MODE_BYPASS    0
#define an_x4_abilities_descr_modes_R_DESCR_MODE_CL49      1
#define an_x4_abilities_descr_modes_R_DESCR_MODE_CL82      2

/****************************************************************************
 * Enums: an_x4_abilities_r_dec_tl_mode
 */
#define an_x4_abilities_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS 0
#define an_x4_abilities_r_dec_tl_mode_R_DEC_TL_MODE_CL49   1
#define an_x4_abilities_r_dec_tl_mode_R_DEC_TL_MODE_CL82   2

/****************************************************************************
 * Enums: an_x4_abilities_r_dec_fsm_mode
 */
#define an_x4_abilities_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define an_x4_abilities_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49 1
#define an_x4_abilities_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82 2

/****************************************************************************
 * Enums: an_x4_abilities_r_deskew_mode
 */
#define an_x4_abilities_r_deskew_mode_R_DESKEW_MODE_BYPASS 0
#define an_x4_abilities_r_deskew_mode_R_DESKEW_MODE_20G    1
#define an_x4_abilities_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define an_x4_abilities_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define an_x4_abilities_r_deskew_mode_R_DESKEW_MODE_100G   4
#define an_x4_abilities_r_deskew_mode_R_DESKEW_MODE_CL49   5
#define an_x4_abilities_r_deskew_mode_R_DESKEW_MODE_CL91   6

/****************************************************************************
 * Enums: an_x4_abilities_bs_dist_modes
 */
#define an_x4_abilities_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define an_x4_abilities_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define an_x4_abilities_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define an_x4_abilities_bs_dist_modes_BS_DIST_MODE_NO_TDM  3

/****************************************************************************
 * Enums: an_x4_abilities_cl49_t_type
 */
#define an_x4_abilities_cl49_t_type_BAD_T_TYPE             15
#define an_x4_abilities_cl49_t_type_T_TYPE_B0              11
#define an_x4_abilities_cl49_t_type_T_TYPE_OB              10
#define an_x4_abilities_cl49_t_type_T_TYPE_B1              9
#define an_x4_abilities_cl49_t_type_T_TYPE_DB              8
#define an_x4_abilities_cl49_t_type_T_TYPE_FC              7
#define an_x4_abilities_cl49_t_type_T_TYPE_TB              6
#define an_x4_abilities_cl49_t_type_T_TYPE_LI              5
#define an_x4_abilities_cl49_t_type_T_TYPE_C               4
#define an_x4_abilities_cl49_t_type_T_TYPE_S               3
#define an_x4_abilities_cl49_t_type_T_TYPE_T               2
#define an_x4_abilities_cl49_t_type_T_TYPE_D               1
#define an_x4_abilities_cl49_t_type_T_TYPE_E               0

/****************************************************************************
 * Enums: an_x4_abilities_cl49_txsm_states
 */
#define an_x4_abilities_cl49_txsm_states_TX_HIG_END        7
#define an_x4_abilities_cl49_txsm_states_TX_HIG_START      6
#define an_x4_abilities_cl49_txsm_states_TX_LI             5
#define an_x4_abilities_cl49_txsm_states_TX_E              4
#define an_x4_abilities_cl49_txsm_states_TX_T              3
#define an_x4_abilities_cl49_txsm_states_TX_D              2
#define an_x4_abilities_cl49_txsm_states_TX_C              1
#define an_x4_abilities_cl49_txsm_states_TX_INIT           0

/****************************************************************************
 * Enums: an_x4_abilities_cl49_ltxsm_states
 */
#define an_x4_abilities_cl49_ltxsm_states_TX_HIG_END       128
#define an_x4_abilities_cl49_ltxsm_states_TX_HIG_START     64
#define an_x4_abilities_cl49_ltxsm_states_TX_LI            32
#define an_x4_abilities_cl49_ltxsm_states_TX_E             16
#define an_x4_abilities_cl49_ltxsm_states_TX_T             8
#define an_x4_abilities_cl49_ltxsm_states_TX_D             4
#define an_x4_abilities_cl49_ltxsm_states_TX_C             2
#define an_x4_abilities_cl49_ltxsm_states_TX_INIT          1

/****************************************************************************
 * Enums: an_x4_abilities_burst_error_mode
 */
#define an_x4_abilities_burst_error_mode_BURST_ERROR_11_BITS 0
#define an_x4_abilities_burst_error_mode_BURST_ERROR_16_BITS 1
#define an_x4_abilities_burst_error_mode_BURST_ERROR_17_BITS 2
#define an_x4_abilities_burst_error_mode_BURST_ERROR_18_BITS 3
#define an_x4_abilities_burst_error_mode_BURST_ERROR_19_BITS 4
#define an_x4_abilities_burst_error_mode_BURST_ERROR_20_BITS 5
#define an_x4_abilities_burst_error_mode_BURST_ERROR_21_BITS 6
#define an_x4_abilities_burst_error_mode_BURST_ERROR_22_BITS 7
#define an_x4_abilities_burst_error_mode_BURST_ERROR_23_BITS 8
#define an_x4_abilities_burst_error_mode_BURST_ERROR_24_BITS 9
#define an_x4_abilities_burst_error_mode_BURST_ERROR_25_BITS 10
#define an_x4_abilities_burst_error_mode_BURST_ERROR_26_BITS 11
#define an_x4_abilities_burst_error_mode_BURST_ERROR_27_BITS 12
#define an_x4_abilities_burst_error_mode_BURST_ERROR_28_BITS 13
#define an_x4_abilities_burst_error_mode_BURST_ERROR_29_BITS 14
#define an_x4_abilities_burst_error_mode_BURST_ERROR_30_BITS 15
#define an_x4_abilities_burst_error_mode_BURST_ERROR_31_BITS 16
#define an_x4_abilities_burst_error_mode_BURST_ERROR_32_BITS 17
#define an_x4_abilities_burst_error_mode_BURST_ERROR_33_BITS 18
#define an_x4_abilities_burst_error_mode_BURST_ERROR_34_BITS 19
#define an_x4_abilities_burst_error_mode_BURST_ERROR_35_BITS 20
#define an_x4_abilities_burst_error_mode_BURST_ERROR_36_BITS 21
#define an_x4_abilities_burst_error_mode_BURST_ERROR_37_BITS 22
#define an_x4_abilities_burst_error_mode_BURST_ERROR_38_BITS 23
#define an_x4_abilities_burst_error_mode_BURST_ERROR_39_BITS 24
#define an_x4_abilities_burst_error_mode_BURST_ERROR_40_BITS 25
#define an_x4_abilities_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: an_x4_abilities_bermon_state
 */
#define an_x4_abilities_bermon_state_HI_BER                4
#define an_x4_abilities_bermon_state_GOOD_BER              3
#define an_x4_abilities_bermon_state_BER_TEST_SH           2
#define an_x4_abilities_bermon_state_START_TIMER           1
#define an_x4_abilities_bermon_state_BER_MT_INIT           0

/****************************************************************************
 * Enums: an_x4_abilities_rxsm_state_cl49
 */
#define an_x4_abilities_rxsm_state_cl49_RX_HIG_END         128
#define an_x4_abilities_rxsm_state_cl49_RX_HIG_START       64
#define an_x4_abilities_rxsm_state_cl49_RX_LI              32
#define an_x4_abilities_rxsm_state_cl49_RX_E               16
#define an_x4_abilities_rxsm_state_cl49_RX_T               8
#define an_x4_abilities_rxsm_state_cl49_RX_D               4
#define an_x4_abilities_rxsm_state_cl49_RX_C               2
#define an_x4_abilities_rxsm_state_cl49_RX_INIT            1

/****************************************************************************
 * Enums: an_x4_abilities_r_type
 */
#define an_x4_abilities_r_type_BAD_R_TYPE                  15
#define an_x4_abilities_r_type_R_TYPE_B0                   11
#define an_x4_abilities_r_type_R_TYPE_OB                   10
#define an_x4_abilities_r_type_R_TYPE_B1                   9
#define an_x4_abilities_r_type_R_TYPE_DB                   8
#define an_x4_abilities_r_type_R_TYPE_FC                   7
#define an_x4_abilities_r_type_R_TYPE_TB                   6
#define an_x4_abilities_r_type_R_TYPE_LI                   5
#define an_x4_abilities_r_type_R_TYPE_C                    4
#define an_x4_abilities_r_type_R_TYPE_S                    3
#define an_x4_abilities_r_type_R_TYPE_T                    2
#define an_x4_abilities_r_type_R_TYPE_D                    1
#define an_x4_abilities_r_type_R_TYPE_E                    0

/****************************************************************************
 * Enums: an_x4_abilities_am_lock_state
 */
#define an_x4_abilities_am_lock_state_INVALID_AM           512
#define an_x4_abilities_am_lock_state_GOOD_AM              256
#define an_x4_abilities_am_lock_state_COMP_AM              128
#define an_x4_abilities_am_lock_state_TIMER_2              64
#define an_x4_abilities_am_lock_state_AM_2_GOOD            32
#define an_x4_abilities_am_lock_state_COMP_2ND             16
#define an_x4_abilities_am_lock_state_TIMER_1              8
#define an_x4_abilities_am_lock_state_FIND_1ST             4
#define an_x4_abilities_am_lock_state_AM_RESET_CNT         2
#define an_x4_abilities_am_lock_state_AM_LOCK_INIT         1

/****************************************************************************
 * Enums: an_x4_abilities_msg_selector
 */
#define an_x4_abilities_msg_selector_RESERVED              0
#define an_x4_abilities_msg_selector_VALUE_802p3           1
#define an_x4_abilities_msg_selector_VALUE_802p9           2
#define an_x4_abilities_msg_selector_VALUE_802p5           3
#define an_x4_abilities_msg_selector_VALUE_1394            4

/****************************************************************************
 * Enums: an_x4_abilities_synce_enum
 */
#define an_x4_abilities_synce_enum_SYNCE_NO_DIV            0
#define an_x4_abilities_synce_enum_SYNCE_DIV_7             1
#define an_x4_abilities_synce_enum_SYNCE_DIV_11            2

/****************************************************************************
 * Enums: an_x4_abilities_synce_enum_stage0
 */
#define an_x4_abilities_synce_enum_stage0_SYNCE_NO_DIV     0
#define an_x4_abilities_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define an_x4_abilities_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: an_x4_abilities_cl91_sync_state
 */
#define an_x4_abilities_cl91_sync_state_FIND_1ST           0
#define an_x4_abilities_cl91_sync_state_COUNT_NEXT         1
#define an_x4_abilities_cl91_sync_state_COMP_2ND           2
#define an_x4_abilities_cl91_sync_state_TWO_GOOD           3

/****************************************************************************
 * Enums: an_x4_abilities_cl91_algn_state
 */
#define an_x4_abilities_cl91_algn_state_LOSS_OF_ALIGNMENT  0
#define an_x4_abilities_cl91_algn_state_DESKEW             1
#define an_x4_abilities_cl91_algn_state_DESKEW_FAIL        2
#define an_x4_abilities_cl91_algn_state_ALIGN_ACQUIRED     3
#define an_x4_abilities_cl91_algn_state_CW_GOOD            4
#define an_x4_abilities_cl91_algn_state_CW_BAD             5
#define an_x4_abilities_cl91_algn_state_THREE_BAD          6

/****************************************************************************
 * Enums: an_x4_abilities_fec_sel
 */
#define an_x4_abilities_fec_sel_NO_FEC                     0
#define an_x4_abilities_fec_sel_FEC_CL74                   1
#define an_x4_abilities_fec_sel_FEC_CL91                   2

/****************************************************************************
 * Enums: an_x4_sw_mgmt_tla_ln_seq_status
 */
#define an_x4_sw_mgmt_tla_ln_seq_status_INIT               1
#define an_x4_sw_mgmt_tla_ln_seq_status_SET_AN_SPEED       2
#define an_x4_sw_mgmt_tla_ln_seq_status_WAIT_AN_SPEED      4
#define an_x4_sw_mgmt_tla_ln_seq_status_WAIT_AN_DONE       8
#define an_x4_sw_mgmt_tla_ln_seq_status_SET_RS_SPEED       16
#define an_x4_sw_mgmt_tla_ln_seq_status_WAIT_RS_CL72EN     32
#define an_x4_sw_mgmt_tla_ln_seq_status_WAIT_RS_SPEED      64
#define an_x4_sw_mgmt_tla_ln_seq_status_AN_IGNORE_LINK     128
#define an_x4_sw_mgmt_tla_ln_seq_status_AN_GOOD_CHECK      256
#define an_x4_sw_mgmt_tla_ln_seq_status_AN_GOOD            512
#define an_x4_sw_mgmt_tla_ln_seq_status_AN_FAIL            1024
#define an_x4_sw_mgmt_tla_ln_seq_status_AN_DEAD            2048
#define an_x4_sw_mgmt_tla_ln_seq_status_WAIT_PD_SPEED      4096
#define an_x4_sw_mgmt_tla_ln_seq_status_PD_IGNORE_LINK     8192
#define an_x4_sw_mgmt_tla_ln_seq_status_PD_GOOD_CHECK      16384
#define an_x4_sw_mgmt_tla_ln_seq_status_SW_SPEED_CHANGE    32768

/****************************************************************************
 * Enums: cl82_cl36TxEEEStates_l
 */
#define cl82_cl36TxEEEStates_l_TX_REFRESH                  8
#define cl82_cl36TxEEEStates_l_TX_QUIET                    4
#define cl82_cl36TxEEEStates_l_TX_SLEEP                    2
#define cl82_cl36TxEEEStates_l_TX_ACTIVE                   1

/****************************************************************************
 * Enums: cl82_cl36TxEEEStates_c
 */
#define cl82_cl36TxEEEStates_c_TX_REFRESH                  3
#define cl82_cl36TxEEEStates_c_TX_QUIET                    2
#define cl82_cl36TxEEEStates_c_TX_SLEEP                    1
#define cl82_cl36TxEEEStates_c_TX_ACTIVE                   0

/****************************************************************************
 * Enums: cl82_cl49TxEEEStates_l
 */
#define cl82_cl49TxEEEStates_l_SCR_RESET_2                 64
#define cl82_cl49TxEEEStates_l_SCR_RESET_1                 32
#define cl82_cl49TxEEEStates_l_TX_WAKE                     16
#define cl82_cl49TxEEEStates_l_TX_REFRESH                  8
#define cl82_cl49TxEEEStates_l_TX_QUIET                    4
#define cl82_cl49TxEEEStates_l_TX_SLEEP                    2
#define cl82_cl49TxEEEStates_l_TX_ACTIVE                   1

/****************************************************************************
 * Enums: cl82_fec_req_enum
 */
#define cl82_fec_req_enum_FEC_not_supported                0
#define cl82_fec_req_enum_FEC_supported_but_not_requested  1
#define cl82_fec_req_enum_invalid_setting                  2
#define cl82_fec_req_enum_FEC_supported_and_requested      3

/****************************************************************************
 * Enums: cl82_cl73_pause_enum
 */
#define cl82_cl73_pause_enum_No_PAUSE_ability              0
#define cl82_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define cl82_cl73_pause_enum_Symmetric_PAUSE_ability       1
#define cl82_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: cl82_cl49TxEEEStates_c
 */
#define cl82_cl49TxEEEStates_c_SCR_RESET_2                 6
#define cl82_cl49TxEEEStates_c_SCR_RESET_1                 5
#define cl82_cl49TxEEEStates_c_TX_WAKE                     4
#define cl82_cl49TxEEEStates_c_TX_REFRESH                  3
#define cl82_cl49TxEEEStates_c_TX_QUIET                    2
#define cl82_cl49TxEEEStates_c_TX_SLEEP                    1
#define cl82_cl49TxEEEStates_c_TX_ACTIVE                   0

/****************************************************************************
 * Enums: cl82_cl36RxEEEStates_l
 */
#define cl82_cl36RxEEEStates_l_RX_LINK_FAIL                32
#define cl82_cl36RxEEEStates_l_RX_WTF                      16
#define cl82_cl36RxEEEStates_l_RX_WAKE                     8
#define cl82_cl36RxEEEStates_l_RX_QUIET                    4
#define cl82_cl36RxEEEStates_l_RX_SLEEP                    2
#define cl82_cl36RxEEEStates_l_RX_ACTIVE                   1

/****************************************************************************
 * Enums: cl82_cl36RxEEEStates_c
 */
#define cl82_cl36RxEEEStates_c_RX_LINK_FAIL                5
#define cl82_cl36RxEEEStates_c_RX_WTF                      4
#define cl82_cl36RxEEEStates_c_RX_WAKE                     3
#define cl82_cl36RxEEEStates_c_RX_QUIET                    2
#define cl82_cl36RxEEEStates_c_RX_SLEEP                    1
#define cl82_cl36RxEEEStates_c_RX_ACTIVE                   0

/****************************************************************************
 * Enums: cl82_cl49RxEEEStates_l
 */
#define cl82_cl49RxEEEStates_l_RX_LINK_FAIL                32
#define cl82_cl49RxEEEStates_l_RX_WTF                      16
#define cl82_cl49RxEEEStates_l_RX_WAKE                     8
#define cl82_cl49RxEEEStates_l_RX_QUIET                    4
#define cl82_cl49RxEEEStates_l_RX_SLEEP                    2
#define cl82_cl49RxEEEStates_l_RX_ACTIVE                   1

/****************************************************************************
 * Enums: cl82_cl49RxEEEStates_c
 */
#define cl82_cl49RxEEEStates_c_RX_LINK_FAIL                5
#define cl82_cl49RxEEEStates_c_RX_WTF                      4
#define cl82_cl49RxEEEStates_c_RX_WAKE                     3
#define cl82_cl49RxEEEStates_c_RX_QUIET                    2
#define cl82_cl49RxEEEStates_c_RX_SLEEP                    1
#define cl82_cl49RxEEEStates_c_RX_ACTIVE                   0

/****************************************************************************
 * Enums: cl82_cl48TxEEEStates_l
 */
#define cl82_cl48TxEEEStates_l_TX_REFRESH                  8
#define cl82_cl48TxEEEStates_l_TX_QUIET                    4
#define cl82_cl48TxEEEStates_l_TX_SLEEP                    2
#define cl82_cl48TxEEEStates_l_TX_ACTIVE                   1

/****************************************************************************
 * Enums: cl82_cl48TxEEEStates_c
 */
#define cl82_cl48TxEEEStates_c_TX_REFRESH                  3
#define cl82_cl48TxEEEStates_c_TX_QUIET                    2
#define cl82_cl48TxEEEStates_c_TX_SLEEP                    1
#define cl82_cl48TxEEEStates_c_TX_ACTIVE                   0

/****************************************************************************
 * Enums: cl82_cl48RxEEEStates_l
 */
#define cl82_cl48RxEEEStates_l_RX_LINK_FAIL                32
#define cl82_cl48RxEEEStates_l_RX_WAKE                     16
#define cl82_cl48RxEEEStates_l_RX_QUIET                    8
#define cl82_cl48RxEEEStates_l_RX_DEACT                    4
#define cl82_cl48RxEEEStates_l_RX_SLEEP                    2
#define cl82_cl48RxEEEStates_l_RX_ACTIVE                   1

/****************************************************************************
 * Enums: cl82_cl48RxEEEStates_c
 */
#define cl82_cl48RxEEEStates_c_RX_LINK_FAIL                5
#define cl82_cl48RxEEEStates_c_RX_WAKE                     4
#define cl82_cl48RxEEEStates_c_RX_QUIET                    3
#define cl82_cl48RxEEEStates_c_RX_DEACT                    2
#define cl82_cl48RxEEEStates_c_RX_SLEEP                    1
#define cl82_cl48RxEEEStates_c_RX_ACTIVE                   0

/****************************************************************************
 * Enums: cl82_IQP_Options
 */
#define cl82_IQP_Options_i50uA                             0
#define cl82_IQP_Options_i100uA                            1
#define cl82_IQP_Options_i150uA                            2
#define cl82_IQP_Options_i200uA                            3
#define cl82_IQP_Options_i250uA                            4
#define cl82_IQP_Options_i300uA                            5
#define cl82_IQP_Options_i350uA                            6
#define cl82_IQP_Options_i400uA                            7
#define cl82_IQP_Options_i450uA                            8
#define cl82_IQP_Options_i500uA                            9
#define cl82_IQP_Options_i550uA                            10
#define cl82_IQP_Options_i600uA                            11
#define cl82_IQP_Options_i650uA                            12
#define cl82_IQP_Options_i700uA                            13
#define cl82_IQP_Options_i750uA                            14
#define cl82_IQP_Options_i800uA                            15

/****************************************************************************
 * Enums: cl82_IDriver_Options
 */
#define cl82_IDriver_Options_v680mV                        0
#define cl82_IDriver_Options_v730mV                        1
#define cl82_IDriver_Options_v780mV                        2
#define cl82_IDriver_Options_v830mV                        3
#define cl82_IDriver_Options_v880mV                        4
#define cl82_IDriver_Options_v930mV                        5
#define cl82_IDriver_Options_v980mV                        6
#define cl82_IDriver_Options_v1010mV                       7
#define cl82_IDriver_Options_v1040mV                       8
#define cl82_IDriver_Options_v1060mV                       9
#define cl82_IDriver_Options_v1070mV                       10
#define cl82_IDriver_Options_v1080mV                       11
#define cl82_IDriver_Options_v1085mV                       12
#define cl82_IDriver_Options_v1090mV                       13
#define cl82_IDriver_Options_v1095mV                       14
#define cl82_IDriver_Options_v1100mV                       15

/****************************************************************************
 * Enums: cl82_operationModes
 */
#define cl82_operationModes_XGXS                           0
#define cl82_operationModes_XGXG_nCC                       1
#define cl82_operationModes_Indlane_OS8                    4
#define cl82_operationModes_IndLane_OS5                    5
#define cl82_operationModes_PCI                            7
#define cl82_operationModes_XGXS_nLQ                       8
#define cl82_operationModes_XGXS_nLQnCC                    9
#define cl82_operationModes_PBypass                        10
#define cl82_operationModes_PBypass_nDSK                   11
#define cl82_operationModes_ComboCoreMode                  12
#define cl82_operationModes_Clocks_off                     15

/****************************************************************************
 * Enums: cl82_actualSpeeds
 */
#define cl82_actualSpeeds_dr_10M                           0
#define cl82_actualSpeeds_dr_100M                          1
#define cl82_actualSpeeds_dr_1G                            2
#define cl82_actualSpeeds_dr_2p5G                          3
#define cl82_actualSpeeds_dr_5G_X4                         4
#define cl82_actualSpeeds_dr_6G_X4                         5
#define cl82_actualSpeeds_dr_10G_HiG                       6
#define cl82_actualSpeeds_dr_10G_CX4                       7
#define cl82_actualSpeeds_dr_12G_HiG                       8
#define cl82_actualSpeeds_dr_12p5G_X4                      9
#define cl82_actualSpeeds_dr_13G_X4                        10
#define cl82_actualSpeeds_dr_15G_X4                        11
#define cl82_actualSpeeds_dr_16G_X4                        12
#define cl82_actualSpeeds_dr_1G_KX                         13
#define cl82_actualSpeeds_dr_10G_KX4                       14
#define cl82_actualSpeeds_dr_10G_KR                        15
#define cl82_actualSpeeds_dr_5G                            16
#define cl82_actualSpeeds_dr_6p4G                          17
#define cl82_actualSpeeds_dr_20G_X4                        18
#define cl82_actualSpeeds_dr_21G_X4                        19
#define cl82_actualSpeeds_dr_25G_X4                        20
#define cl82_actualSpeeds_dr_10G_HiG_DXGXS                 21
#define cl82_actualSpeeds_dr_10G_DXGXS                     22
#define cl82_actualSpeeds_dr_10p5G_HiG_DXGXS               23
#define cl82_actualSpeeds_dr_10p5G_DXGXS                   24
#define cl82_actualSpeeds_dr_12p773G_HiG_DXGXS             25
#define cl82_actualSpeeds_dr_12p773G_DXGXS                 26
#define cl82_actualSpeeds_dr_10G_XFI                       27
#define cl82_actualSpeeds_dr_40G                           28
#define cl82_actualSpeeds_dr_20G_HiG_DXGXS                 29
#define cl82_actualSpeeds_dr_20G_DXGXS                     30
#define cl82_actualSpeeds_dr_10G_SFI                       31
#define cl82_actualSpeeds_dr_31p5G                         32
#define cl82_actualSpeeds_dr_32p7G                         33
#define cl82_actualSpeeds_dr_20G_SCR                       34
#define cl82_actualSpeeds_dr_10G_HiG_DXGXS_SCR             35
#define cl82_actualSpeeds_dr_10G_DXGXS_SCR                 36
#define cl82_actualSpeeds_dr_12G_R2                        37
#define cl82_actualSpeeds_dr_10G_X2                        38
#define cl82_actualSpeeds_dr_40G_KR4                       39
#define cl82_actualSpeeds_dr_40G_CR4                       40
#define cl82_actualSpeeds_dr_100G_CR10                     41
#define cl82_actualSpeeds_dr_15p75G_DXGXS                  44
#define cl82_actualSpeeds_dr_20G_KR2                       57
#define cl82_actualSpeeds_dr_20G_CR2                       58

/****************************************************************************
 * Enums: cl82_actualSpeedsMisc1
 */
#define cl82_actualSpeedsMisc1_dr_2500BRCM_X1              16
#define cl82_actualSpeedsMisc1_dr_5000BRCM_X4              17
#define cl82_actualSpeedsMisc1_dr_6000BRCM_X4              18
#define cl82_actualSpeedsMisc1_dr_10GHiGig_X4              19
#define cl82_actualSpeedsMisc1_dr_10GBASE_CX4              20
#define cl82_actualSpeedsMisc1_dr_12GHiGig_X4              21
#define cl82_actualSpeedsMisc1_dr_12p5GHiGig_X4            22
#define cl82_actualSpeedsMisc1_dr_13GHiGig_X4              23
#define cl82_actualSpeedsMisc1_dr_15GHiGig_X4              24
#define cl82_actualSpeedsMisc1_dr_16GHiGig_X4              25
#define cl82_actualSpeedsMisc1_dr_5000BRCM_X1              26
#define cl82_actualSpeedsMisc1_dr_6363BRCM_X1              27
#define cl82_actualSpeedsMisc1_dr_20GHiGig_X4              28
#define cl82_actualSpeedsMisc1_dr_21GHiGig_X4              29
#define cl82_actualSpeedsMisc1_dr_25p45GHiGig_X4           30
#define cl82_actualSpeedsMisc1_dr_10G_HiG_DXGXS            31

/****************************************************************************
 * Enums: cl82_IndLaneModes
 */
#define cl82_IndLaneModes_SWSDR_div2                       0
#define cl82_IndLaneModes_SWSDR_div1                       1
#define cl82_IndLaneModes_DWSDR_div2                       2
#define cl82_IndLaneModes_DWSDR_div1                       3

/****************************************************************************
 * Enums: cl82_prbsSelect
 */
#define cl82_prbsSelect_prbs7                              0
#define cl82_prbsSelect_prbs15                             1
#define cl82_prbsSelect_prbs23                             2
#define cl82_prbsSelect_prbs31                             3

/****************************************************************************
 * Enums: cl82_vcoDivider
 */
#define cl82_vcoDivider_div32                              0
#define cl82_vcoDivider_div36                              1
#define cl82_vcoDivider_div40                              2
#define cl82_vcoDivider_div42                              3
#define cl82_vcoDivider_div48                              4
#define cl82_vcoDivider_div50                              5
#define cl82_vcoDivider_div52                              6
#define cl82_vcoDivider_div54                              7
#define cl82_vcoDivider_div60                              8
#define cl82_vcoDivider_div64                              9
#define cl82_vcoDivider_div66                              10
#define cl82_vcoDivider_div68                              11
#define cl82_vcoDivider_div70                              12
#define cl82_vcoDivider_div80                              13
#define cl82_vcoDivider_div92                              14
#define cl82_vcoDivider_div100                             15

/****************************************************************************
 * Enums: cl82_refClkSelect
 */
#define cl82_refClkSelect_clk_25MHz                        0
#define cl82_refClkSelect_clk_100MHz                       1
#define cl82_refClkSelect_clk_125MHz                       2
#define cl82_refClkSelect_clk_156p25MHz                    3
#define cl82_refClkSelect_clk_187p5MHz                     4
#define cl82_refClkSelect_clk_161p25Mhz                    5
#define cl82_refClkSelect_clk_50Mhz                        6
#define cl82_refClkSelect_clk_106p25Mhz                    7

/****************************************************************************
 * Enums: cl82_aerMMDdevTypeSelect
 */
#define cl82_aerMMDdevTypeSelect_combo_core                0
#define cl82_aerMMDdevTypeSelect_PMA_PMD                   1
#define cl82_aerMMDdevTypeSelect_PCS                       3
#define cl82_aerMMDdevTypeSelect_PHY                       4
#define cl82_aerMMDdevTypeSelect_DTE                       5
#define cl82_aerMMDdevTypeSelect_CL73_AN                   7

/****************************************************************************
 * Enums: cl82_aerMMDportSelect
 */
#define cl82_aerMMDportSelect_ln0                          0
#define cl82_aerMMDportSelect_ln1                          1
#define cl82_aerMMDportSelect_ln2                          2
#define cl82_aerMMDportSelect_ln3                          3
#define cl82_aerMMDportSelect_BCST_ln0_1_2_3               511
#define cl82_aerMMDportSelect_BCST_ln0_1                   512
#define cl82_aerMMDportSelect_BCST_ln2_3                   513

/****************************************************************************
 * Enums: cl82_firmwareModeSelect
 */
#define cl82_firmwareModeSelect_DEFAULT                    0
#define cl82_firmwareModeSelect_SFP_OPT_LR                 1
#define cl82_firmwareModeSelect_SFP_DAC                    2
#define cl82_firmwareModeSelect_XLAUI                      3
#define cl82_firmwareModeSelect_LONG_CH_6G                 4

/****************************************************************************
 * Enums: cl82_tempIdxSelect
 */
#define cl82_tempIdxSelect_LTE__22p9C                      15
#define cl82_tempIdxSelect_LTE__12p6C                      14
#define cl82_tempIdxSelect_LTE__3p0C                       13
#define cl82_tempIdxSelect_LTE_6p7C                        12
#define cl82_tempIdxSelect_LTE_16p4C                       11
#define cl82_tempIdxSelect_LTE_26p6C                       10
#define cl82_tempIdxSelect_LTE_36p3C                       9
#define cl82_tempIdxSelect_LTE_46p0C                       8
#define cl82_tempIdxSelect_LTE_56p2C                       7
#define cl82_tempIdxSelect_LTE_65p9C                       6
#define cl82_tempIdxSelect_LTE_75p6C                       5
#define cl82_tempIdxSelect_LTE_85p3C                       4
#define cl82_tempIdxSelect_LTE_95p5C                       3
#define cl82_tempIdxSelect_LTE_105p2C                      2
#define cl82_tempIdxSelect_LTE_114p9C                      1
#define cl82_tempIdxSelect_LTE_125p1C                      0

/****************************************************************************
 * Enums: cl82_port_mode
 */
#define cl82_port_mode_QUAD_PORT                           0
#define cl82_port_mode_TRI_1_PORT                          1
#define cl82_port_mode_TRI_2_PORT                          2
#define cl82_port_mode_DUAL_PORT                           3
#define cl82_port_mode_SINGLE_PORT                         4

/****************************************************************************
 * Enums: cl82_rev_letter_enum
 */
#define cl82_rev_letter_enum_REV_A                         0
#define cl82_rev_letter_enum_REV_B                         1
#define cl82_rev_letter_enum_REV_C                         2
#define cl82_rev_letter_enum_REV_D                         3

/****************************************************************************
 * Enums: cl82_rev_number_enum
 */
#define cl82_rev_number_enum_REV_0                         0
#define cl82_rev_number_enum_REV_1                         1
#define cl82_rev_number_enum_REV_2                         2
#define cl82_rev_number_enum_REV_3                         3
#define cl82_rev_number_enum_REV_4                         4
#define cl82_rev_number_enum_REV_5                         5
#define cl82_rev_number_enum_REV_6                         6
#define cl82_rev_number_enum_REV_7                         7

/****************************************************************************
 * Enums: cl82_bonding_enum
 */
#define cl82_bonding_enum_WIRE_BOND                        0
#define cl82_bonding_enum_FLIP_CHIP                        1

/****************************************************************************
 * Enums: cl82_tech_process
 */
#define cl82_tech_process_PROCESS_90NM                     0
#define cl82_tech_process_PROCESS_65NM                     1
#define cl82_tech_process_PROCESS_40NM                     2
#define cl82_tech_process_PROCESS_28NM                     3

/****************************************************************************
 * Enums: cl82_model_num
 */
#define cl82_model_num_SERDES_CL73                         0
#define cl82_model_num_XGXS_16G                            1
#define cl82_model_num_HYPERCORE                           2
#define cl82_model_num_HYPERLITE                           3
#define cl82_model_num_PCIE_G2_PIPE                        4
#define cl82_model_num_SERDES_1p25GBd                      5
#define cl82_model_num_SATA2                               6
#define cl82_model_num_QSGMII                              7
#define cl82_model_num_XGXS10G                             8
#define cl82_model_num_WARPCORE                            9
#define cl82_model_num_XFICORE                             10
#define cl82_model_num_RXFI                                11
#define cl82_model_num_WARPLITE                            12
#define cl82_model_num_PENTACORE                           13
#define cl82_model_num_ESM                                 14
#define cl82_model_num_QUAD_SGMII                          15
#define cl82_model_num_WARPCORE_3                          16
#define cl82_model_num_TSC                                 17
#define cl82_model_num_TSC4E                               18
#define cl82_model_num_TSC12E                              19
#define cl82_model_num_TSC4F                               20
#define cl82_model_num_XGXS_CL73_90NM                      29
#define cl82_model_num_SERDES_CL73_90NM                    30
#define cl82_model_num_WARPCORE3                           32
#define cl82_model_num_WARPCORE4_TSC                       33
#define cl82_model_num_RXAUI                               34

/****************************************************************************
 * Enums: cl82_payload
 */
#define cl82_payload_REPEAT_2_BYTES                        0
#define cl82_payload_RAMPING                               1
#define cl82_payload_CL48_CRPAT                            2
#define cl82_payload_CL48_CJPAT                            3
#define cl82_payload_CL36_LONG_CRPAT                       4
#define cl82_payload_CL36_SHORT_CRPAT                      5

/****************************************************************************
 * Enums: cl82_sc
 */
#define cl82_sc_S_10G_CR1                                  0
#define cl82_sc_S_10G_KR1                                  1
#define cl82_sc_S_10G_X1                                   2
#define cl82_sc_S_10G_HG2_CR1                              4
#define cl82_sc_S_10G_HG2_KR1                              5
#define cl82_sc_S_10G_HG2_X1                               6
#define cl82_sc_S_20G_CR1                                  8
#define cl82_sc_S_20G_KR1                                  9
#define cl82_sc_S_20G_X1                                   10
#define cl82_sc_S_20G_HG2_CR1                              12
#define cl82_sc_S_20G_HG2_KR1                              13
#define cl82_sc_S_20G_HG2_X1                               14
#define cl82_sc_S_25G_CR1                                  16
#define cl82_sc_S_25G_KR1                                  17
#define cl82_sc_S_25G_X1                                   18
#define cl82_sc_S_25G_HG2_CR1                              20
#define cl82_sc_S_25G_HG2_KR1                              21
#define cl82_sc_S_25G_HG2_X1                               22
#define cl82_sc_S_20G_CR2                                  24
#define cl82_sc_S_20G_KR2                                  25
#define cl82_sc_S_20G_X2                                   26
#define cl82_sc_S_20G_HG2_CR2                              28
#define cl82_sc_S_20G_HG2_KR2                              29
#define cl82_sc_S_20G_HG2_X2                               30
#define cl82_sc_S_40G_CR2                                  32
#define cl82_sc_S_40G_KR2                                  33
#define cl82_sc_S_40G_X2                                   34
#define cl82_sc_S_40G_HG2_CR2                              36
#define cl82_sc_S_40G_HG2_KR2                              37
#define cl82_sc_S_40G_HG2_X2                               38
#define cl82_sc_S_40G_CR4                                  40
#define cl82_sc_S_40G_KR4                                  41
#define cl82_sc_S_40G_X4                                   42
#define cl82_sc_S_40G_HG2_CR4                              44
#define cl82_sc_S_40G_HG2_KR4                              45
#define cl82_sc_S_40G_HG2_X4                               46
#define cl82_sc_S_50G_CR2                                  48
#define cl82_sc_S_50G_KR2                                  49
#define cl82_sc_S_50G_X2                                   50
#define cl82_sc_S_50G_HG2_CR2                              52
#define cl82_sc_S_50G_HG2_KR2                              53
#define cl82_sc_S_50G_HG2_X2                               54
#define cl82_sc_S_50G_CR4                                  56
#define cl82_sc_S_50G_KR4                                  57
#define cl82_sc_S_50G_X4                                   58
#define cl82_sc_S_50G_HG2_CR4                              60
#define cl82_sc_S_50G_HG2_KR4                              61
#define cl82_sc_S_50G_HG2_X4                               62
#define cl82_sc_S_100G_CR4                                 64
#define cl82_sc_S_100G_KR4                                 65
#define cl82_sc_S_100G_X4                                  66
#define cl82_sc_S_100G_HG2_CR4                             68
#define cl82_sc_S_100G_HG2_KR4                             69
#define cl82_sc_S_100G_HG2_X4                              70
#define cl82_sc_S_CL73_20GVCO                              72
#define cl82_sc_S_CL73_25GVCO                              80
#define cl82_sc_S_CL36_20GVCO                              88
#define cl82_sc_S_CL36_25GVCO                              96

/****************************************************************************
 * Enums: cl82_t_fifo_modes
 */
#define cl82_t_fifo_modes_T_FIFO_MODE_BYPASS               0
#define cl82_t_fifo_modes_T_FIFO_MODE_40G                  1
#define cl82_t_fifo_modes_T_FIFO_MODE_100G                 2
#define cl82_t_fifo_modes_T_FIFO_MODE_20G                  3

/****************************************************************************
 * Enums: cl82_t_enc_modes
 */
#define cl82_t_enc_modes_T_ENC_MODE_BYPASS                 0
#define cl82_t_enc_modes_T_ENC_MODE_CL49                   1
#define cl82_t_enc_modes_T_ENC_MODE_CL82                   2

/****************************************************************************
 * Enums: cl82_btmx_mode
 */
#define cl82_btmx_mode_BS_BTMX_MODE_1to1                   0
#define cl82_btmx_mode_BS_BTMX_MODE_2to1                   1
#define cl82_btmx_mode_BS_BTMX_MODE_5to1                   2

/****************************************************************************
 * Enums: cl82_t_type_cl82
 */
#define cl82_t_type_cl82_T_TYPE_B1                         5
#define cl82_t_type_cl82_T_TYPE_C                          4
#define cl82_t_type_cl82_T_TYPE_S                          3
#define cl82_t_type_cl82_T_TYPE_T                          2
#define cl82_t_type_cl82_T_TYPE_D                          1
#define cl82_t_type_cl82_T_TYPE_E                          0

/****************************************************************************
 * Enums: cl82_txsm_state_cl82
 */
#define cl82_txsm_state_cl82_TX_HIG_END                    6
#define cl82_txsm_state_cl82_TX_HIG_START                  5
#define cl82_txsm_state_cl82_TX_E                          4
#define cl82_txsm_state_cl82_TX_T                          3
#define cl82_txsm_state_cl82_TX_D                          2
#define cl82_txsm_state_cl82_TX_C                          1
#define cl82_txsm_state_cl82_TX_INIT                       0

/****************************************************************************
 * Enums: cl82_ltxsm_state_cl82
 */
#define cl82_ltxsm_state_cl82_TX_HIG_END                   64
#define cl82_ltxsm_state_cl82_TX_HIG_START                 32
#define cl82_ltxsm_state_cl82_TX_E                         16
#define cl82_ltxsm_state_cl82_TX_T                         8
#define cl82_ltxsm_state_cl82_TX_D                         4
#define cl82_ltxsm_state_cl82_TX_C                         2
#define cl82_ltxsm_state_cl82_TX_INIT                      1

/****************************************************************************
 * Enums: cl82_r_type_coded_cl82
 */
#define cl82_r_type_coded_cl82_R_TYPE_B1                   32
#define cl82_r_type_coded_cl82_R_TYPE_C                    16
#define cl82_r_type_coded_cl82_R_TYPE_S                    8
#define cl82_r_type_coded_cl82_R_TYPE_T                    4
#define cl82_r_type_coded_cl82_R_TYPE_D                    2
#define cl82_r_type_coded_cl82_R_TYPE_E                    1

/****************************************************************************
 * Enums: cl82_rxsm_state_cl82
 */
#define cl82_rxsm_state_cl82_RX_HIG_END                    64
#define cl82_rxsm_state_cl82_RX_HIG_START                  32
#define cl82_rxsm_state_cl82_RX_E                          16
#define cl82_rxsm_state_cl82_RX_T                          8
#define cl82_rxsm_state_cl82_RX_D                          4
#define cl82_rxsm_state_cl82_RX_C                          2
#define cl82_rxsm_state_cl82_RX_INIT                       1

/****************************************************************************
 * Enums: cl82_deskew_state
 */
#define cl82_deskew_state_ALIGN_ACQUIRED                   2
#define cl82_deskew_state_LOSS_OF_ALIGNMENT                1

/****************************************************************************
 * Enums: cl82_os_mode_enum
 */
#define cl82_os_mode_enum_OS_MODE_1                        0
#define cl82_os_mode_enum_OS_MODE_2                        1
#define cl82_os_mode_enum_OS_MODE_4                        2
#define cl82_os_mode_enum_OS_MODE_16p5                     8
#define cl82_os_mode_enum_OS_MODE_20p625                   12

/****************************************************************************
 * Enums: cl82_scr_modes
 */
#define cl82_scr_modes_T_SCR_MODE_BYPASS                   0
#define cl82_scr_modes_T_SCR_MODE_CL49                     1
#define cl82_scr_modes_T_SCR_MODE_40G_2_LANE               2
#define cl82_scr_modes_T_SCR_MODE_100G                     3
#define cl82_scr_modes_T_SCR_MODE_20G                      4
#define cl82_scr_modes_T_SCR_MODE_40G_4_LANE               5

/****************************************************************************
 * Enums: cl82_descr_modes
 */
#define cl82_descr_modes_R_DESCR_MODE_BYPASS               0
#define cl82_descr_modes_R_DESCR_MODE_CL49                 1
#define cl82_descr_modes_R_DESCR_MODE_CL82                 2

/****************************************************************************
 * Enums: cl82_r_dec_tl_mode
 */
#define cl82_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS            0
#define cl82_r_dec_tl_mode_R_DEC_TL_MODE_CL49              1
#define cl82_r_dec_tl_mode_R_DEC_TL_MODE_CL82              2

/****************************************************************************
 * Enums: cl82_r_dec_fsm_mode
 */
#define cl82_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS          0
#define cl82_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49            1
#define cl82_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82            2

/****************************************************************************
 * Enums: cl82_r_deskew_mode
 */
#define cl82_r_deskew_mode_R_DESKEW_MODE_BYPASS            0
#define cl82_r_deskew_mode_R_DESKEW_MODE_20G               1
#define cl82_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE        2
#define cl82_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE        3
#define cl82_r_deskew_mode_R_DESKEW_MODE_100G              4
#define cl82_r_deskew_mode_R_DESKEW_MODE_CL49              5
#define cl82_r_deskew_mode_R_DESKEW_MODE_CL91              6

/****************************************************************************
 * Enums: cl82_bs_dist_modes
 */
#define cl82_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM         0
#define cl82_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define cl82_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define cl82_bs_dist_modes_BS_DIST_MODE_NO_TDM             3

/****************************************************************************
 * Enums: cl82_cl49_t_type
 */
#define cl82_cl49_t_type_BAD_T_TYPE                        15
#define cl82_cl49_t_type_T_TYPE_B0                         11
#define cl82_cl49_t_type_T_TYPE_OB                         10
#define cl82_cl49_t_type_T_TYPE_B1                         9
#define cl82_cl49_t_type_T_TYPE_DB                         8
#define cl82_cl49_t_type_T_TYPE_FC                         7
#define cl82_cl49_t_type_T_TYPE_TB                         6
#define cl82_cl49_t_type_T_TYPE_LI                         5
#define cl82_cl49_t_type_T_TYPE_C                          4
#define cl82_cl49_t_type_T_TYPE_S                          3
#define cl82_cl49_t_type_T_TYPE_T                          2
#define cl82_cl49_t_type_T_TYPE_D                          1
#define cl82_cl49_t_type_T_TYPE_E                          0

/****************************************************************************
 * Enums: cl82_cl49_txsm_states
 */
#define cl82_cl49_txsm_states_TX_HIG_END                   7
#define cl82_cl49_txsm_states_TX_HIG_START                 6
#define cl82_cl49_txsm_states_TX_LI                        5
#define cl82_cl49_txsm_states_TX_E                         4
#define cl82_cl49_txsm_states_TX_T                         3
#define cl82_cl49_txsm_states_TX_D                         2
#define cl82_cl49_txsm_states_TX_C                         1
#define cl82_cl49_txsm_states_TX_INIT                      0

/****************************************************************************
 * Enums: cl82_cl49_ltxsm_states
 */
#define cl82_cl49_ltxsm_states_TX_HIG_END                  128
#define cl82_cl49_ltxsm_states_TX_HIG_START                64
#define cl82_cl49_ltxsm_states_TX_LI                       32
#define cl82_cl49_ltxsm_states_TX_E                        16
#define cl82_cl49_ltxsm_states_TX_T                        8
#define cl82_cl49_ltxsm_states_TX_D                        4
#define cl82_cl49_ltxsm_states_TX_C                        2
#define cl82_cl49_ltxsm_states_TX_INIT                     1

/****************************************************************************
 * Enums: cl82_burst_error_mode
 */
#define cl82_burst_error_mode_BURST_ERROR_11_BITS          0
#define cl82_burst_error_mode_BURST_ERROR_16_BITS          1
#define cl82_burst_error_mode_BURST_ERROR_17_BITS          2
#define cl82_burst_error_mode_BURST_ERROR_18_BITS          3
#define cl82_burst_error_mode_BURST_ERROR_19_BITS          4
#define cl82_burst_error_mode_BURST_ERROR_20_BITS          5
#define cl82_burst_error_mode_BURST_ERROR_21_BITS          6
#define cl82_burst_error_mode_BURST_ERROR_22_BITS          7
#define cl82_burst_error_mode_BURST_ERROR_23_BITS          8
#define cl82_burst_error_mode_BURST_ERROR_24_BITS          9
#define cl82_burst_error_mode_BURST_ERROR_25_BITS          10
#define cl82_burst_error_mode_BURST_ERROR_26_BITS          11
#define cl82_burst_error_mode_BURST_ERROR_27_BITS          12
#define cl82_burst_error_mode_BURST_ERROR_28_BITS          13
#define cl82_burst_error_mode_BURST_ERROR_29_BITS          14
#define cl82_burst_error_mode_BURST_ERROR_30_BITS          15
#define cl82_burst_error_mode_BURST_ERROR_31_BITS          16
#define cl82_burst_error_mode_BURST_ERROR_32_BITS          17
#define cl82_burst_error_mode_BURST_ERROR_33_BITS          18
#define cl82_burst_error_mode_BURST_ERROR_34_BITS          19
#define cl82_burst_error_mode_BURST_ERROR_35_BITS          20
#define cl82_burst_error_mode_BURST_ERROR_36_BITS          21
#define cl82_burst_error_mode_BURST_ERROR_37_BITS          22
#define cl82_burst_error_mode_BURST_ERROR_38_BITS          23
#define cl82_burst_error_mode_BURST_ERROR_39_BITS          24
#define cl82_burst_error_mode_BURST_ERROR_40_BITS          25
#define cl82_burst_error_mode_BURST_ERROR_41_BITS          26

/****************************************************************************
 * Enums: cl82_bermon_state
 */
#define cl82_bermon_state_HI_BER                           4
#define cl82_bermon_state_GOOD_BER                         3
#define cl82_bermon_state_BER_TEST_SH                      2
#define cl82_bermon_state_START_TIMER                      1
#define cl82_bermon_state_BER_MT_INIT                      0

/****************************************************************************
 * Enums: cl82_rxsm_state_cl49
 */
#define cl82_rxsm_state_cl49_RX_HIG_END                    128
#define cl82_rxsm_state_cl49_RX_HIG_START                  64
#define cl82_rxsm_state_cl49_RX_LI                         32
#define cl82_rxsm_state_cl49_RX_E                          16
#define cl82_rxsm_state_cl49_RX_T                          8
#define cl82_rxsm_state_cl49_RX_D                          4
#define cl82_rxsm_state_cl49_RX_C                          2
#define cl82_rxsm_state_cl49_RX_INIT                       1

/****************************************************************************
 * Enums: cl82_r_type
 */
#define cl82_r_type_BAD_R_TYPE                             15
#define cl82_r_type_R_TYPE_B0                              11
#define cl82_r_type_R_TYPE_OB                              10
#define cl82_r_type_R_TYPE_B1                              9
#define cl82_r_type_R_TYPE_DB                              8
#define cl82_r_type_R_TYPE_FC                              7
#define cl82_r_type_R_TYPE_TB                              6
#define cl82_r_type_R_TYPE_LI                              5
#define cl82_r_type_R_TYPE_C                               4
#define cl82_r_type_R_TYPE_S                               3
#define cl82_r_type_R_TYPE_T                               2
#define cl82_r_type_R_TYPE_D                               1
#define cl82_r_type_R_TYPE_E                               0

/****************************************************************************
 * Enums: cl82_am_lock_state
 */
#define cl82_am_lock_state_INVALID_AM                      512
#define cl82_am_lock_state_GOOD_AM                         256
#define cl82_am_lock_state_COMP_AM                         128
#define cl82_am_lock_state_TIMER_2                         64
#define cl82_am_lock_state_AM_2_GOOD                       32
#define cl82_am_lock_state_COMP_2ND                        16
#define cl82_am_lock_state_TIMER_1                         8
#define cl82_am_lock_state_FIND_1ST                        4
#define cl82_am_lock_state_AM_RESET_CNT                    2
#define cl82_am_lock_state_AM_LOCK_INIT                    1

/****************************************************************************
 * Enums: cl82_msg_selector
 */
#define cl82_msg_selector_RESERVED                         0
#define cl82_msg_selector_VALUE_802p3                      1
#define cl82_msg_selector_VALUE_802p9                      2
#define cl82_msg_selector_VALUE_802p5                      3
#define cl82_msg_selector_VALUE_1394                       4

/****************************************************************************
 * Enums: cl82_synce_enum
 */
#define cl82_synce_enum_SYNCE_NO_DIV                       0
#define cl82_synce_enum_SYNCE_DIV_7                        1
#define cl82_synce_enum_SYNCE_DIV_11                       2

/****************************************************************************
 * Enums: cl82_synce_enum_stage0
 */
#define cl82_synce_enum_stage0_SYNCE_NO_DIV                0
#define cl82_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5  1
#define cl82_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV      2

/****************************************************************************
 * Enums: cl82_cl91_sync_state
 */
#define cl82_cl91_sync_state_FIND_1ST                      0
#define cl82_cl91_sync_state_COUNT_NEXT                    1
#define cl82_cl91_sync_state_COMP_2ND                      2
#define cl82_cl91_sync_state_TWO_GOOD                      3

/****************************************************************************
 * Enums: cl82_cl91_algn_state
 */
#define cl82_cl91_algn_state_LOSS_OF_ALIGNMENT             0
#define cl82_cl91_algn_state_DESKEW                        1
#define cl82_cl91_algn_state_DESKEW_FAIL                   2
#define cl82_cl91_algn_state_ALIGN_ACQUIRED                3
#define cl82_cl91_algn_state_CW_GOOD                       4
#define cl82_cl91_algn_state_CW_BAD                        5
#define cl82_cl91_algn_state_THREE_BAD                     6

/****************************************************************************
 * Enums: cl82_fec_sel
 */
#define cl82_fec_sel_NO_FEC                                0
#define cl82_fec_sel_FEC_CL74                              1
#define cl82_fec_sel_FEC_CL91                              2

/****************************************************************************
 * Enums: main0_cl36TxEEEStates_l
 */
#define main0_cl36TxEEEStates_l_TX_REFRESH                 8
#define main0_cl36TxEEEStates_l_TX_QUIET                   4
#define main0_cl36TxEEEStates_l_TX_SLEEP                   2
#define main0_cl36TxEEEStates_l_TX_ACTIVE                  1

/****************************************************************************
 * Enums: main0_cl36TxEEEStates_c
 */
#define main0_cl36TxEEEStates_c_TX_REFRESH                 3
#define main0_cl36TxEEEStates_c_TX_QUIET                   2
#define main0_cl36TxEEEStates_c_TX_SLEEP                   1
#define main0_cl36TxEEEStates_c_TX_ACTIVE                  0

/****************************************************************************
 * Enums: main0_cl49TxEEEStates_l
 */
#define main0_cl49TxEEEStates_l_SCR_RESET_2                64
#define main0_cl49TxEEEStates_l_SCR_RESET_1                32
#define main0_cl49TxEEEStates_l_TX_WAKE                    16
#define main0_cl49TxEEEStates_l_TX_REFRESH                 8
#define main0_cl49TxEEEStates_l_TX_QUIET                   4
#define main0_cl49TxEEEStates_l_TX_SLEEP                   2
#define main0_cl49TxEEEStates_l_TX_ACTIVE                  1

/****************************************************************************
 * Enums: main0_fec_req_enum
 */
#define main0_fec_req_enum_FEC_not_supported               0
#define main0_fec_req_enum_FEC_supported_but_not_requested 1
#define main0_fec_req_enum_invalid_setting                 2
#define main0_fec_req_enum_FEC_supported_and_requested     3

/****************************************************************************
 * Enums: main0_cl73_pause_enum
 */
#define main0_cl73_pause_enum_No_PAUSE_ability             0
#define main0_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define main0_cl73_pause_enum_Symmetric_PAUSE_ability      1
#define main0_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: main0_cl49TxEEEStates_c
 */
#define main0_cl49TxEEEStates_c_SCR_RESET_2                6
#define main0_cl49TxEEEStates_c_SCR_RESET_1                5
#define main0_cl49TxEEEStates_c_TX_WAKE                    4
#define main0_cl49TxEEEStates_c_TX_REFRESH                 3
#define main0_cl49TxEEEStates_c_TX_QUIET                   2
#define main0_cl49TxEEEStates_c_TX_SLEEP                   1
#define main0_cl49TxEEEStates_c_TX_ACTIVE                  0

/****************************************************************************
 * Enums: main0_cl36RxEEEStates_l
 */
#define main0_cl36RxEEEStates_l_RX_LINK_FAIL               32
#define main0_cl36RxEEEStates_l_RX_WTF                     16
#define main0_cl36RxEEEStates_l_RX_WAKE                    8
#define main0_cl36RxEEEStates_l_RX_QUIET                   4
#define main0_cl36RxEEEStates_l_RX_SLEEP                   2
#define main0_cl36RxEEEStates_l_RX_ACTIVE                  1

/****************************************************************************
 * Enums: main0_cl36RxEEEStates_c
 */
#define main0_cl36RxEEEStates_c_RX_LINK_FAIL               5
#define main0_cl36RxEEEStates_c_RX_WTF                     4
#define main0_cl36RxEEEStates_c_RX_WAKE                    3
#define main0_cl36RxEEEStates_c_RX_QUIET                   2
#define main0_cl36RxEEEStates_c_RX_SLEEP                   1
#define main0_cl36RxEEEStates_c_RX_ACTIVE                  0

/****************************************************************************
 * Enums: main0_cl49RxEEEStates_l
 */
#define main0_cl49RxEEEStates_l_RX_LINK_FAIL               32
#define main0_cl49RxEEEStates_l_RX_WTF                     16
#define main0_cl49RxEEEStates_l_RX_WAKE                    8
#define main0_cl49RxEEEStates_l_RX_QUIET                   4
#define main0_cl49RxEEEStates_l_RX_SLEEP                   2
#define main0_cl49RxEEEStates_l_RX_ACTIVE                  1

/****************************************************************************
 * Enums: main0_cl49RxEEEStates_c
 */
#define main0_cl49RxEEEStates_c_RX_LINK_FAIL               5
#define main0_cl49RxEEEStates_c_RX_WTF                     4
#define main0_cl49RxEEEStates_c_RX_WAKE                    3
#define main0_cl49RxEEEStates_c_RX_QUIET                   2
#define main0_cl49RxEEEStates_c_RX_SLEEP                   1
#define main0_cl49RxEEEStates_c_RX_ACTIVE                  0

/****************************************************************************
 * Enums: main0_cl48TxEEEStates_l
 */
#define main0_cl48TxEEEStates_l_TX_REFRESH                 8
#define main0_cl48TxEEEStates_l_TX_QUIET                   4
#define main0_cl48TxEEEStates_l_TX_SLEEP                   2
#define main0_cl48TxEEEStates_l_TX_ACTIVE                  1

/****************************************************************************
 * Enums: main0_cl48TxEEEStates_c
 */
#define main0_cl48TxEEEStates_c_TX_REFRESH                 3
#define main0_cl48TxEEEStates_c_TX_QUIET                   2
#define main0_cl48TxEEEStates_c_TX_SLEEP                   1
#define main0_cl48TxEEEStates_c_TX_ACTIVE                  0

/****************************************************************************
 * Enums: main0_cl48RxEEEStates_l
 */
#define main0_cl48RxEEEStates_l_RX_LINK_FAIL               32
#define main0_cl48RxEEEStates_l_RX_WAKE                    16
#define main0_cl48RxEEEStates_l_RX_QUIET                   8
#define main0_cl48RxEEEStates_l_RX_DEACT                   4
#define main0_cl48RxEEEStates_l_RX_SLEEP                   2
#define main0_cl48RxEEEStates_l_RX_ACTIVE                  1

/****************************************************************************
 * Enums: main0_cl48RxEEEStates_c
 */
#define main0_cl48RxEEEStates_c_RX_LINK_FAIL               5
#define main0_cl48RxEEEStates_c_RX_WAKE                    4
#define main0_cl48RxEEEStates_c_RX_QUIET                   3
#define main0_cl48RxEEEStates_c_RX_DEACT                   2
#define main0_cl48RxEEEStates_c_RX_SLEEP                   1
#define main0_cl48RxEEEStates_c_RX_ACTIVE                  0

/****************************************************************************
 * Enums: main0_IQP_Options
 */
#define main0_IQP_Options_i50uA                            0
#define main0_IQP_Options_i100uA                           1
#define main0_IQP_Options_i150uA                           2
#define main0_IQP_Options_i200uA                           3
#define main0_IQP_Options_i250uA                           4
#define main0_IQP_Options_i300uA                           5
#define main0_IQP_Options_i350uA                           6
#define main0_IQP_Options_i400uA                           7
#define main0_IQP_Options_i450uA                           8
#define main0_IQP_Options_i500uA                           9
#define main0_IQP_Options_i550uA                           10
#define main0_IQP_Options_i600uA                           11
#define main0_IQP_Options_i650uA                           12
#define main0_IQP_Options_i700uA                           13
#define main0_IQP_Options_i750uA                           14
#define main0_IQP_Options_i800uA                           15

/****************************************************************************
 * Enums: main0_IDriver_Options
 */
#define main0_IDriver_Options_v680mV                       0
#define main0_IDriver_Options_v730mV                       1
#define main0_IDriver_Options_v780mV                       2
#define main0_IDriver_Options_v830mV                       3
#define main0_IDriver_Options_v880mV                       4
#define main0_IDriver_Options_v930mV                       5
#define main0_IDriver_Options_v980mV                       6
#define main0_IDriver_Options_v1010mV                      7
#define main0_IDriver_Options_v1040mV                      8
#define main0_IDriver_Options_v1060mV                      9
#define main0_IDriver_Options_v1070mV                      10
#define main0_IDriver_Options_v1080mV                      11
#define main0_IDriver_Options_v1085mV                      12
#define main0_IDriver_Options_v1090mV                      13
#define main0_IDriver_Options_v1095mV                      14
#define main0_IDriver_Options_v1100mV                      15

/****************************************************************************
 * Enums: main0_operationModes
 */
#define main0_operationModes_XGXS                          0
#define main0_operationModes_XGXG_nCC                      1
#define main0_operationModes_Indlane_OS8                   4
#define main0_operationModes_IndLane_OS5                   5
#define main0_operationModes_PCI                           7
#define main0_operationModes_XGXS_nLQ                      8
#define main0_operationModes_XGXS_nLQnCC                   9
#define main0_operationModes_PBypass                       10
#define main0_operationModes_PBypass_nDSK                  11
#define main0_operationModes_ComboCoreMode                 12
#define main0_operationModes_Clocks_off                    15

/****************************************************************************
 * Enums: main0_actualSpeeds
 */
#define main0_actualSpeeds_dr_10M                          0
#define main0_actualSpeeds_dr_100M                         1
#define main0_actualSpeeds_dr_1G                           2
#define main0_actualSpeeds_dr_2p5G                         3
#define main0_actualSpeeds_dr_5G_X4                        4
#define main0_actualSpeeds_dr_6G_X4                        5
#define main0_actualSpeeds_dr_10G_HiG                      6
#define main0_actualSpeeds_dr_10G_CX4                      7
#define main0_actualSpeeds_dr_12G_HiG                      8
#define main0_actualSpeeds_dr_12p5G_X4                     9
#define main0_actualSpeeds_dr_13G_X4                       10
#define main0_actualSpeeds_dr_15G_X4                       11
#define main0_actualSpeeds_dr_16G_X4                       12
#define main0_actualSpeeds_dr_1G_KX                        13
#define main0_actualSpeeds_dr_10G_KX4                      14
#define main0_actualSpeeds_dr_10G_KR                       15
#define main0_actualSpeeds_dr_5G                           16
#define main0_actualSpeeds_dr_6p4G                         17
#define main0_actualSpeeds_dr_20G_X4                       18
#define main0_actualSpeeds_dr_21G_X4                       19
#define main0_actualSpeeds_dr_25G_X4                       20
#define main0_actualSpeeds_dr_10G_HiG_DXGXS                21
#define main0_actualSpeeds_dr_10G_DXGXS                    22
#define main0_actualSpeeds_dr_10p5G_HiG_DXGXS              23
#define main0_actualSpeeds_dr_10p5G_DXGXS                  24
#define main0_actualSpeeds_dr_12p773G_HiG_DXGXS            25
#define main0_actualSpeeds_dr_12p773G_DXGXS                26
#define main0_actualSpeeds_dr_10G_XFI                      27
#define main0_actualSpeeds_dr_40G                          28
#define main0_actualSpeeds_dr_20G_HiG_DXGXS                29
#define main0_actualSpeeds_dr_20G_DXGXS                    30
#define main0_actualSpeeds_dr_10G_SFI                      31
#define main0_actualSpeeds_dr_31p5G                        32
#define main0_actualSpeeds_dr_32p7G                        33
#define main0_actualSpeeds_dr_20G_SCR                      34
#define main0_actualSpeeds_dr_10G_HiG_DXGXS_SCR            35
#define main0_actualSpeeds_dr_10G_DXGXS_SCR                36
#define main0_actualSpeeds_dr_12G_R2                       37
#define main0_actualSpeeds_dr_10G_X2                       38
#define main0_actualSpeeds_dr_40G_KR4                      39
#define main0_actualSpeeds_dr_40G_CR4                      40
#define main0_actualSpeeds_dr_100G_CR10                    41
#define main0_actualSpeeds_dr_15p75G_DXGXS                 44
#define main0_actualSpeeds_dr_20G_KR2                      57
#define main0_actualSpeeds_dr_20G_CR2                      58

/****************************************************************************
 * Enums: main0_actualSpeedsMisc1
 */
#define main0_actualSpeedsMisc1_dr_2500BRCM_X1             16
#define main0_actualSpeedsMisc1_dr_5000BRCM_X4             17
#define main0_actualSpeedsMisc1_dr_6000BRCM_X4             18
#define main0_actualSpeedsMisc1_dr_10GHiGig_X4             19
#define main0_actualSpeedsMisc1_dr_10GBASE_CX4             20
#define main0_actualSpeedsMisc1_dr_12GHiGig_X4             21
#define main0_actualSpeedsMisc1_dr_12p5GHiGig_X4           22
#define main0_actualSpeedsMisc1_dr_13GHiGig_X4             23
#define main0_actualSpeedsMisc1_dr_15GHiGig_X4             24
#define main0_actualSpeedsMisc1_dr_16GHiGig_X4             25
#define main0_actualSpeedsMisc1_dr_5000BRCM_X1             26
#define main0_actualSpeedsMisc1_dr_6363BRCM_X1             27
#define main0_actualSpeedsMisc1_dr_20GHiGig_X4             28
#define main0_actualSpeedsMisc1_dr_21GHiGig_X4             29
#define main0_actualSpeedsMisc1_dr_25p45GHiGig_X4          30
#define main0_actualSpeedsMisc1_dr_10G_HiG_DXGXS           31

/****************************************************************************
 * Enums: main0_IndLaneModes
 */
#define main0_IndLaneModes_SWSDR_div2                      0
#define main0_IndLaneModes_SWSDR_div1                      1
#define main0_IndLaneModes_DWSDR_div2                      2
#define main0_IndLaneModes_DWSDR_div1                      3

/****************************************************************************
 * Enums: main0_prbsSelect
 */
#define main0_prbsSelect_prbs7                             0
#define main0_prbsSelect_prbs15                            1
#define main0_prbsSelect_prbs23                            2
#define main0_prbsSelect_prbs31                            3

/****************************************************************************
 * Enums: main0_vcoDivider
 */
#define main0_vcoDivider_div32                             0
#define main0_vcoDivider_div36                             1
#define main0_vcoDivider_div40                             2
#define main0_vcoDivider_div42                             3
#define main0_vcoDivider_div48                             4
#define main0_vcoDivider_div50                             5
#define main0_vcoDivider_div52                             6
#define main0_vcoDivider_div54                             7
#define main0_vcoDivider_div60                             8
#define main0_vcoDivider_div64                             9
#define main0_vcoDivider_div66                             10
#define main0_vcoDivider_div68                             11
#define main0_vcoDivider_div70                             12
#define main0_vcoDivider_div80                             13
#define main0_vcoDivider_div92                             14
#define main0_vcoDivider_div100                            15

/****************************************************************************
 * Enums: main0_refClkSelect
 */
#define main0_refClkSelect_clk_25MHz                       0
#define main0_refClkSelect_clk_100MHz                      1
#define main0_refClkSelect_clk_125MHz                      2
#define main0_refClkSelect_clk_156p25MHz                   3
#define main0_refClkSelect_clk_187p5MHz                    4
#define main0_refClkSelect_clk_161p25Mhz                   5
#define main0_refClkSelect_clk_50Mhz                       6
#define main0_refClkSelect_clk_106p25Mhz                   7

/****************************************************************************
 * Enums: main0_aerMMDdevTypeSelect
 */
#define main0_aerMMDdevTypeSelect_combo_core               0
#define main0_aerMMDdevTypeSelect_PMA_PMD                  1
#define main0_aerMMDdevTypeSelect_PCS                      3
#define main0_aerMMDdevTypeSelect_PHY                      4
#define main0_aerMMDdevTypeSelect_DTE                      5
#define main0_aerMMDdevTypeSelect_CL73_AN                  7

/****************************************************************************
 * Enums: main0_aerMMDportSelect
 */
#define main0_aerMMDportSelect_ln0                         0
#define main0_aerMMDportSelect_ln1                         1
#define main0_aerMMDportSelect_ln2                         2
#define main0_aerMMDportSelect_ln3                         3
#define main0_aerMMDportSelect_BCST_ln0_1_2_3              511
#define main0_aerMMDportSelect_BCST_ln0_1                  512
#define main0_aerMMDportSelect_BCST_ln2_3                  513

/****************************************************************************
 * Enums: main0_firmwareModeSelect
 */
#define main0_firmwareModeSelect_DEFAULT                   0
#define main0_firmwareModeSelect_SFP_OPT_LR                1
#define main0_firmwareModeSelect_SFP_DAC                   2
#define main0_firmwareModeSelect_XLAUI                     3
#define main0_firmwareModeSelect_LONG_CH_6G                4

/****************************************************************************
 * Enums: main0_tempIdxSelect
 */
#define main0_tempIdxSelect_LTE__22p9C                     15
#define main0_tempIdxSelect_LTE__12p6C                     14
#define main0_tempIdxSelect_LTE__3p0C                      13
#define main0_tempIdxSelect_LTE_6p7C                       12
#define main0_tempIdxSelect_LTE_16p4C                      11
#define main0_tempIdxSelect_LTE_26p6C                      10
#define main0_tempIdxSelect_LTE_36p3C                      9
#define main0_tempIdxSelect_LTE_46p0C                      8
#define main0_tempIdxSelect_LTE_56p2C                      7
#define main0_tempIdxSelect_LTE_65p9C                      6
#define main0_tempIdxSelect_LTE_75p6C                      5
#define main0_tempIdxSelect_LTE_85p3C                      4
#define main0_tempIdxSelect_LTE_95p5C                      3
#define main0_tempIdxSelect_LTE_105p2C                     2
#define main0_tempIdxSelect_LTE_114p9C                     1
#define main0_tempIdxSelect_LTE_125p1C                     0

/****************************************************************************
 * Enums: main0_port_mode
 */
#define main0_port_mode_QUAD_PORT                          0
#define main0_port_mode_TRI_1_PORT                         1
#define main0_port_mode_TRI_2_PORT                         2
#define main0_port_mode_DUAL_PORT                          3
#define main0_port_mode_SINGLE_PORT                        4

/****************************************************************************
 * Enums: main0_rev_letter_enum
 */
#define main0_rev_letter_enum_REV_A                        0
#define main0_rev_letter_enum_REV_B                        1
#define main0_rev_letter_enum_REV_C                        2
#define main0_rev_letter_enum_REV_D                        3

/****************************************************************************
 * Enums: main0_rev_number_enum
 */
#define main0_rev_number_enum_REV_0                        0
#define main0_rev_number_enum_REV_1                        1
#define main0_rev_number_enum_REV_2                        2
#define main0_rev_number_enum_REV_3                        3
#define main0_rev_number_enum_REV_4                        4
#define main0_rev_number_enum_REV_5                        5
#define main0_rev_number_enum_REV_6                        6
#define main0_rev_number_enum_REV_7                        7

/****************************************************************************
 * Enums: main0_bonding_enum
 */
#define main0_bonding_enum_WIRE_BOND                       0
#define main0_bonding_enum_FLIP_CHIP                       1

/****************************************************************************
 * Enums: main0_tech_process
 */
#define main0_tech_process_PROCESS_90NM                    0
#define main0_tech_process_PROCESS_65NM                    1
#define main0_tech_process_PROCESS_40NM                    2
#define main0_tech_process_PROCESS_28NM                    3

/****************************************************************************
 * Enums: main0_model_num
 */
#define main0_model_num_SERDES_CL73                        0
#define main0_model_num_XGXS_16G                           1
#define main0_model_num_HYPERCORE                          2
#define main0_model_num_HYPERLITE                          3
#define main0_model_num_PCIE_G2_PIPE                       4
#define main0_model_num_SERDES_1p25GBd                     5
#define main0_model_num_SATA2                              6
#define main0_model_num_QSGMII                             7
#define main0_model_num_XGXS10G                            8
#define main0_model_num_WARPCORE                           9
#define main0_model_num_XFICORE                            10
#define main0_model_num_RXFI                               11
#define main0_model_num_WARPLITE                           12
#define main0_model_num_PENTACORE                          13
#define main0_model_num_ESM                                14
#define main0_model_num_QUAD_SGMII                         15
#define main0_model_num_WARPCORE_3                         16
#define main0_model_num_TSC                                17
#define main0_model_num_TSC4E                              18
#define main0_model_num_TSC12E                             19
#define main0_model_num_TSC4F                              20
#define main0_model_num_XGXS_CL73_90NM                     29
#define main0_model_num_SERDES_CL73_90NM                   30
#define main0_model_num_WARPCORE3                          32
#define main0_model_num_WARPCORE4_TSC                      33
#define main0_model_num_RXAUI                              34

/****************************************************************************
 * Enums: main0_payload
 */
#define main0_payload_REPEAT_2_BYTES                       0
#define main0_payload_RAMPING                              1
#define main0_payload_CL48_CRPAT                           2
#define main0_payload_CL48_CJPAT                           3
#define main0_payload_CL36_LONG_CRPAT                      4
#define main0_payload_CL36_SHORT_CRPAT                     5

/****************************************************************************
 * Enums: main0_sc
 */
#define main0_sc_S_10G_CR1                                 0
#define main0_sc_S_10G_KR1                                 1
#define main0_sc_S_10G_X1                                  2
#define main0_sc_S_10G_HG2_CR1                             4
#define main0_sc_S_10G_HG2_KR1                             5
#define main0_sc_S_10G_HG2_X1                              6
#define main0_sc_S_20G_CR1                                 8
#define main0_sc_S_20G_KR1                                 9
#define main0_sc_S_20G_X1                                  10
#define main0_sc_S_20G_HG2_CR1                             12
#define main0_sc_S_20G_HG2_KR1                             13
#define main0_sc_S_20G_HG2_X1                              14
#define main0_sc_S_25G_CR1                                 16
#define main0_sc_S_25G_KR1                                 17
#define main0_sc_S_25G_X1                                  18
#define main0_sc_S_25G_HG2_CR1                             20
#define main0_sc_S_25G_HG2_KR1                             21
#define main0_sc_S_25G_HG2_X1                              22
#define main0_sc_S_20G_CR2                                 24
#define main0_sc_S_20G_KR2                                 25
#define main0_sc_S_20G_X2                                  26
#define main0_sc_S_20G_HG2_CR2                             28
#define main0_sc_S_20G_HG2_KR2                             29
#define main0_sc_S_20G_HG2_X2                              30
#define main0_sc_S_40G_CR2                                 32
#define main0_sc_S_40G_KR2                                 33
#define main0_sc_S_40G_X2                                  34
#define main0_sc_S_40G_HG2_CR2                             36
#define main0_sc_S_40G_HG2_KR2                             37
#define main0_sc_S_40G_HG2_X2                              38
#define main0_sc_S_40G_CR4                                 40
#define main0_sc_S_40G_KR4                                 41
#define main0_sc_S_40G_X4                                  42
#define main0_sc_S_40G_HG2_CR4                             44
#define main0_sc_S_40G_HG2_KR4                             45
#define main0_sc_S_40G_HG2_X4                              46
#define main0_sc_S_50G_CR2                                 48
#define main0_sc_S_50G_KR2                                 49
#define main0_sc_S_50G_X2                                  50
#define main0_sc_S_50G_HG2_CR2                             52
#define main0_sc_S_50G_HG2_KR2                             53
#define main0_sc_S_50G_HG2_X2                              54
#define main0_sc_S_50G_CR4                                 56
#define main0_sc_S_50G_KR4                                 57
#define main0_sc_S_50G_X4                                  58
#define main0_sc_S_50G_HG2_CR4                             60
#define main0_sc_S_50G_HG2_KR4                             61
#define main0_sc_S_50G_HG2_X4                              62
#define main0_sc_S_100G_CR4                                64
#define main0_sc_S_100G_KR4                                65
#define main0_sc_S_100G_X4                                 66
#define main0_sc_S_100G_HG2_CR4                            68
#define main0_sc_S_100G_HG2_KR4                            69
#define main0_sc_S_100G_HG2_X4                             70
#define main0_sc_S_CL73_20GVCO                             72
#define main0_sc_S_CL73_25GVCO                             80
#define main0_sc_S_CL36_20GVCO                             88
#define main0_sc_S_CL36_25GVCO                             96

/****************************************************************************
 * Enums: main0_t_fifo_modes
 */
#define main0_t_fifo_modes_T_FIFO_MODE_BYPASS              0
#define main0_t_fifo_modes_T_FIFO_MODE_40G                 1
#define main0_t_fifo_modes_T_FIFO_MODE_100G                2
#define main0_t_fifo_modes_T_FIFO_MODE_20G                 3

/****************************************************************************
 * Enums: main0_t_enc_modes
 */
#define main0_t_enc_modes_T_ENC_MODE_BYPASS                0
#define main0_t_enc_modes_T_ENC_MODE_CL49                  1
#define main0_t_enc_modes_T_ENC_MODE_CL82                  2

/****************************************************************************
 * Enums: main0_btmx_mode
 */
#define main0_btmx_mode_BS_BTMX_MODE_1to1                  0
#define main0_btmx_mode_BS_BTMX_MODE_2to1                  1
#define main0_btmx_mode_BS_BTMX_MODE_5to1                  2

/****************************************************************************
 * Enums: main0_t_type_cl82
 */
#define main0_t_type_cl82_T_TYPE_B1                        5
#define main0_t_type_cl82_T_TYPE_C                         4
#define main0_t_type_cl82_T_TYPE_S                         3
#define main0_t_type_cl82_T_TYPE_T                         2
#define main0_t_type_cl82_T_TYPE_D                         1
#define main0_t_type_cl82_T_TYPE_E                         0

/****************************************************************************
 * Enums: main0_txsm_state_cl82
 */
#define main0_txsm_state_cl82_TX_HIG_END                   6
#define main0_txsm_state_cl82_TX_HIG_START                 5
#define main0_txsm_state_cl82_TX_E                         4
#define main0_txsm_state_cl82_TX_T                         3
#define main0_txsm_state_cl82_TX_D                         2
#define main0_txsm_state_cl82_TX_C                         1
#define main0_txsm_state_cl82_TX_INIT                      0

/****************************************************************************
 * Enums: main0_ltxsm_state_cl82
 */
#define main0_ltxsm_state_cl82_TX_HIG_END                  64
#define main0_ltxsm_state_cl82_TX_HIG_START                32
#define main0_ltxsm_state_cl82_TX_E                        16
#define main0_ltxsm_state_cl82_TX_T                        8
#define main0_ltxsm_state_cl82_TX_D                        4
#define main0_ltxsm_state_cl82_TX_C                        2
#define main0_ltxsm_state_cl82_TX_INIT                     1

/****************************************************************************
 * Enums: main0_r_type_coded_cl82
 */
#define main0_r_type_coded_cl82_R_TYPE_B1                  32
#define main0_r_type_coded_cl82_R_TYPE_C                   16
#define main0_r_type_coded_cl82_R_TYPE_S                   8
#define main0_r_type_coded_cl82_R_TYPE_T                   4
#define main0_r_type_coded_cl82_R_TYPE_D                   2
#define main0_r_type_coded_cl82_R_TYPE_E                   1

/****************************************************************************
 * Enums: main0_rxsm_state_cl82
 */
#define main0_rxsm_state_cl82_RX_HIG_END                   64
#define main0_rxsm_state_cl82_RX_HIG_START                 32
#define main0_rxsm_state_cl82_RX_E                         16
#define main0_rxsm_state_cl82_RX_T                         8
#define main0_rxsm_state_cl82_RX_D                         4
#define main0_rxsm_state_cl82_RX_C                         2
#define main0_rxsm_state_cl82_RX_INIT                      1

/****************************************************************************
 * Enums: main0_deskew_state
 */
#define main0_deskew_state_ALIGN_ACQUIRED                  2
#define main0_deskew_state_LOSS_OF_ALIGNMENT               1

/****************************************************************************
 * Enums: main0_os_mode_enum
 */
#define main0_os_mode_enum_OS_MODE_1                       0
#define main0_os_mode_enum_OS_MODE_2                       1
#define main0_os_mode_enum_OS_MODE_4                       2
#define main0_os_mode_enum_OS_MODE_16p5                    8
#define main0_os_mode_enum_OS_MODE_20p625                  12

/****************************************************************************
 * Enums: main0_scr_modes
 */
#define main0_scr_modes_T_SCR_MODE_BYPASS                  0
#define main0_scr_modes_T_SCR_MODE_CL49                    1
#define main0_scr_modes_T_SCR_MODE_40G_2_LANE              2
#define main0_scr_modes_T_SCR_MODE_100G                    3
#define main0_scr_modes_T_SCR_MODE_20G                     4
#define main0_scr_modes_T_SCR_MODE_40G_4_LANE              5

/****************************************************************************
 * Enums: main0_descr_modes
 */
#define main0_descr_modes_R_DESCR_MODE_BYPASS              0
#define main0_descr_modes_R_DESCR_MODE_CL49                1
#define main0_descr_modes_R_DESCR_MODE_CL82                2

/****************************************************************************
 * Enums: main0_r_dec_tl_mode
 */
#define main0_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS           0
#define main0_r_dec_tl_mode_R_DEC_TL_MODE_CL49             1
#define main0_r_dec_tl_mode_R_DEC_TL_MODE_CL82             2

/****************************************************************************
 * Enums: main0_r_dec_fsm_mode
 */
#define main0_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS         0
#define main0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49           1
#define main0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82           2

/****************************************************************************
 * Enums: main0_r_deskew_mode
 */
#define main0_r_deskew_mode_R_DESKEW_MODE_BYPASS           0
#define main0_r_deskew_mode_R_DESKEW_MODE_20G              1
#define main0_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE       2
#define main0_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE       3
#define main0_r_deskew_mode_R_DESKEW_MODE_100G             4
#define main0_r_deskew_mode_R_DESKEW_MODE_CL49             5
#define main0_r_deskew_mode_R_DESKEW_MODE_CL91             6

/****************************************************************************
 * Enums: main0_bs_dist_modes
 */
#define main0_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM        0
#define main0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define main0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define main0_bs_dist_modes_BS_DIST_MODE_NO_TDM            3

/****************************************************************************
 * Enums: main0_cl49_t_type
 */
#define main0_cl49_t_type_BAD_T_TYPE                       15
#define main0_cl49_t_type_T_TYPE_B0                        11
#define main0_cl49_t_type_T_TYPE_OB                        10
#define main0_cl49_t_type_T_TYPE_B1                        9
#define main0_cl49_t_type_T_TYPE_DB                        8
#define main0_cl49_t_type_T_TYPE_FC                        7
#define main0_cl49_t_type_T_TYPE_TB                        6
#define main0_cl49_t_type_T_TYPE_LI                        5
#define main0_cl49_t_type_T_TYPE_C                         4
#define main0_cl49_t_type_T_TYPE_S                         3
#define main0_cl49_t_type_T_TYPE_T                         2
#define main0_cl49_t_type_T_TYPE_D                         1
#define main0_cl49_t_type_T_TYPE_E                         0

/****************************************************************************
 * Enums: main0_cl49_txsm_states
 */
#define main0_cl49_txsm_states_TX_HIG_END                  7
#define main0_cl49_txsm_states_TX_HIG_START                6
#define main0_cl49_txsm_states_TX_LI                       5
#define main0_cl49_txsm_states_TX_E                        4
#define main0_cl49_txsm_states_TX_T                        3
#define main0_cl49_txsm_states_TX_D                        2
#define main0_cl49_txsm_states_TX_C                        1
#define main0_cl49_txsm_states_TX_INIT                     0

/****************************************************************************
 * Enums: main0_cl49_ltxsm_states
 */
#define main0_cl49_ltxsm_states_TX_HIG_END                 128
#define main0_cl49_ltxsm_states_TX_HIG_START               64
#define main0_cl49_ltxsm_states_TX_LI                      32
#define main0_cl49_ltxsm_states_TX_E                       16
#define main0_cl49_ltxsm_states_TX_T                       8
#define main0_cl49_ltxsm_states_TX_D                       4
#define main0_cl49_ltxsm_states_TX_C                       2
#define main0_cl49_ltxsm_states_TX_INIT                    1

/****************************************************************************
 * Enums: main0_burst_error_mode
 */
#define main0_burst_error_mode_BURST_ERROR_11_BITS         0
#define main0_burst_error_mode_BURST_ERROR_16_BITS         1
#define main0_burst_error_mode_BURST_ERROR_17_BITS         2
#define main0_burst_error_mode_BURST_ERROR_18_BITS         3
#define main0_burst_error_mode_BURST_ERROR_19_BITS         4
#define main0_burst_error_mode_BURST_ERROR_20_BITS         5
#define main0_burst_error_mode_BURST_ERROR_21_BITS         6
#define main0_burst_error_mode_BURST_ERROR_22_BITS         7
#define main0_burst_error_mode_BURST_ERROR_23_BITS         8
#define main0_burst_error_mode_BURST_ERROR_24_BITS         9
#define main0_burst_error_mode_BURST_ERROR_25_BITS         10
#define main0_burst_error_mode_BURST_ERROR_26_BITS         11
#define main0_burst_error_mode_BURST_ERROR_27_BITS         12
#define main0_burst_error_mode_BURST_ERROR_28_BITS         13
#define main0_burst_error_mode_BURST_ERROR_29_BITS         14
#define main0_burst_error_mode_BURST_ERROR_30_BITS         15
#define main0_burst_error_mode_BURST_ERROR_31_BITS         16
#define main0_burst_error_mode_BURST_ERROR_32_BITS         17
#define main0_burst_error_mode_BURST_ERROR_33_BITS         18
#define main0_burst_error_mode_BURST_ERROR_34_BITS         19
#define main0_burst_error_mode_BURST_ERROR_35_BITS         20
#define main0_burst_error_mode_BURST_ERROR_36_BITS         21
#define main0_burst_error_mode_BURST_ERROR_37_BITS         22
#define main0_burst_error_mode_BURST_ERROR_38_BITS         23
#define main0_burst_error_mode_BURST_ERROR_39_BITS         24
#define main0_burst_error_mode_BURST_ERROR_40_BITS         25
#define main0_burst_error_mode_BURST_ERROR_41_BITS         26

/****************************************************************************
 * Enums: main0_bermon_state
 */
#define main0_bermon_state_HI_BER                          4
#define main0_bermon_state_GOOD_BER                        3
#define main0_bermon_state_BER_TEST_SH                     2
#define main0_bermon_state_START_TIMER                     1
#define main0_bermon_state_BER_MT_INIT                     0

/****************************************************************************
 * Enums: main0_rxsm_state_cl49
 */
#define main0_rxsm_state_cl49_RX_HIG_END                   128
#define main0_rxsm_state_cl49_RX_HIG_START                 64
#define main0_rxsm_state_cl49_RX_LI                        32
#define main0_rxsm_state_cl49_RX_E                         16
#define main0_rxsm_state_cl49_RX_T                         8
#define main0_rxsm_state_cl49_RX_D                         4
#define main0_rxsm_state_cl49_RX_C                         2
#define main0_rxsm_state_cl49_RX_INIT                      1

/****************************************************************************
 * Enums: main0_r_type
 */
#define main0_r_type_BAD_R_TYPE                            15
#define main0_r_type_R_TYPE_B0                             11
#define main0_r_type_R_TYPE_OB                             10
#define main0_r_type_R_TYPE_B1                             9
#define main0_r_type_R_TYPE_DB                             8
#define main0_r_type_R_TYPE_FC                             7
#define main0_r_type_R_TYPE_TB                             6
#define main0_r_type_R_TYPE_LI                             5
#define main0_r_type_R_TYPE_C                              4
#define main0_r_type_R_TYPE_S                              3
#define main0_r_type_R_TYPE_T                              2
#define main0_r_type_R_TYPE_D                              1
#define main0_r_type_R_TYPE_E                              0

/****************************************************************************
 * Enums: main0_am_lock_state
 */
#define main0_am_lock_state_INVALID_AM                     512
#define main0_am_lock_state_GOOD_AM                        256
#define main0_am_lock_state_COMP_AM                        128
#define main0_am_lock_state_TIMER_2                        64
#define main0_am_lock_state_AM_2_GOOD                      32
#define main0_am_lock_state_COMP_2ND                       16
#define main0_am_lock_state_TIMER_1                        8
#define main0_am_lock_state_FIND_1ST                       4
#define main0_am_lock_state_AM_RESET_CNT                   2
#define main0_am_lock_state_AM_LOCK_INIT                   1

/****************************************************************************
 * Enums: main0_msg_selector
 */
#define main0_msg_selector_RESERVED                        0
#define main0_msg_selector_VALUE_802p3                     1
#define main0_msg_selector_VALUE_802p9                     2
#define main0_msg_selector_VALUE_802p5                     3
#define main0_msg_selector_VALUE_1394                      4

/****************************************************************************
 * Enums: main0_synce_enum
 */
#define main0_synce_enum_SYNCE_NO_DIV                      0
#define main0_synce_enum_SYNCE_DIV_7                       1
#define main0_synce_enum_SYNCE_DIV_11                      2

/****************************************************************************
 * Enums: main0_synce_enum_stage0
 */
#define main0_synce_enum_stage0_SYNCE_NO_DIV               0
#define main0_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define main0_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV     2

/****************************************************************************
 * Enums: main0_cl91_sync_state
 */
#define main0_cl91_sync_state_FIND_1ST                     0
#define main0_cl91_sync_state_COUNT_NEXT                   1
#define main0_cl91_sync_state_COMP_2ND                     2
#define main0_cl91_sync_state_TWO_GOOD                     3

/****************************************************************************
 * Enums: main0_cl91_algn_state
 */
#define main0_cl91_algn_state_LOSS_OF_ALIGNMENT            0
#define main0_cl91_algn_state_DESKEW                       1
#define main0_cl91_algn_state_DESKEW_FAIL                  2
#define main0_cl91_algn_state_ALIGN_ACQUIRED               3
#define main0_cl91_algn_state_CW_GOOD                      4
#define main0_cl91_algn_state_CW_BAD                       5
#define main0_cl91_algn_state_THREE_BAD                    6

/****************************************************************************
 * Enums: main0_fec_sel
 */
#define main0_fec_sel_NO_FEC                               0
#define main0_fec_sel_FEC_CL74                             1
#define main0_fec_sel_FEC_CL91                             2

/****************************************************************************
 * Enums: rx_x1_status0_cl36TxEEEStates_l
 */
#define rx_x1_status0_cl36TxEEEStates_l_TX_REFRESH         8
#define rx_x1_status0_cl36TxEEEStates_l_TX_QUIET           4
#define rx_x1_status0_cl36TxEEEStates_l_TX_SLEEP           2
#define rx_x1_status0_cl36TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x1_status0_cl36TxEEEStates_c
 */
#define rx_x1_status0_cl36TxEEEStates_c_TX_REFRESH         3
#define rx_x1_status0_cl36TxEEEStates_c_TX_QUIET           2
#define rx_x1_status0_cl36TxEEEStates_c_TX_SLEEP           1
#define rx_x1_status0_cl36TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x1_status0_cl49TxEEEStates_l
 */
#define rx_x1_status0_cl49TxEEEStates_l_SCR_RESET_2        64
#define rx_x1_status0_cl49TxEEEStates_l_SCR_RESET_1        32
#define rx_x1_status0_cl49TxEEEStates_l_TX_WAKE            16
#define rx_x1_status0_cl49TxEEEStates_l_TX_REFRESH         8
#define rx_x1_status0_cl49TxEEEStates_l_TX_QUIET           4
#define rx_x1_status0_cl49TxEEEStates_l_TX_SLEEP           2
#define rx_x1_status0_cl49TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x1_status0_fec_req_enum
 */
#define rx_x1_status0_fec_req_enum_FEC_not_supported       0
#define rx_x1_status0_fec_req_enum_FEC_supported_but_not_requested 1
#define rx_x1_status0_fec_req_enum_invalid_setting         2
#define rx_x1_status0_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: rx_x1_status0_cl73_pause_enum
 */
#define rx_x1_status0_cl73_pause_enum_No_PAUSE_ability     0
#define rx_x1_status0_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define rx_x1_status0_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define rx_x1_status0_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: rx_x1_status0_cl49TxEEEStates_c
 */
#define rx_x1_status0_cl49TxEEEStates_c_SCR_RESET_2        6
#define rx_x1_status0_cl49TxEEEStates_c_SCR_RESET_1        5
#define rx_x1_status0_cl49TxEEEStates_c_TX_WAKE            4
#define rx_x1_status0_cl49TxEEEStates_c_TX_REFRESH         3
#define rx_x1_status0_cl49TxEEEStates_c_TX_QUIET           2
#define rx_x1_status0_cl49TxEEEStates_c_TX_SLEEP           1
#define rx_x1_status0_cl49TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x1_status0_cl36RxEEEStates_l
 */
#define rx_x1_status0_cl36RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x1_status0_cl36RxEEEStates_l_RX_WTF             16
#define rx_x1_status0_cl36RxEEEStates_l_RX_WAKE            8
#define rx_x1_status0_cl36RxEEEStates_l_RX_QUIET           4
#define rx_x1_status0_cl36RxEEEStates_l_RX_SLEEP           2
#define rx_x1_status0_cl36RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x1_status0_cl36RxEEEStates_c
 */
#define rx_x1_status0_cl36RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x1_status0_cl36RxEEEStates_c_RX_WTF             4
#define rx_x1_status0_cl36RxEEEStates_c_RX_WAKE            3
#define rx_x1_status0_cl36RxEEEStates_c_RX_QUIET           2
#define rx_x1_status0_cl36RxEEEStates_c_RX_SLEEP           1
#define rx_x1_status0_cl36RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x1_status0_cl49RxEEEStates_l
 */
#define rx_x1_status0_cl49RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x1_status0_cl49RxEEEStates_l_RX_WTF             16
#define rx_x1_status0_cl49RxEEEStates_l_RX_WAKE            8
#define rx_x1_status0_cl49RxEEEStates_l_RX_QUIET           4
#define rx_x1_status0_cl49RxEEEStates_l_RX_SLEEP           2
#define rx_x1_status0_cl49RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x1_status0_cl49RxEEEStates_c
 */
#define rx_x1_status0_cl49RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x1_status0_cl49RxEEEStates_c_RX_WTF             4
#define rx_x1_status0_cl49RxEEEStates_c_RX_WAKE            3
#define rx_x1_status0_cl49RxEEEStates_c_RX_QUIET           2
#define rx_x1_status0_cl49RxEEEStates_c_RX_SLEEP           1
#define rx_x1_status0_cl49RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x1_status0_cl48TxEEEStates_l
 */
#define rx_x1_status0_cl48TxEEEStates_l_TX_REFRESH         8
#define rx_x1_status0_cl48TxEEEStates_l_TX_QUIET           4
#define rx_x1_status0_cl48TxEEEStates_l_TX_SLEEP           2
#define rx_x1_status0_cl48TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x1_status0_cl48TxEEEStates_c
 */
#define rx_x1_status0_cl48TxEEEStates_c_TX_REFRESH         3
#define rx_x1_status0_cl48TxEEEStates_c_TX_QUIET           2
#define rx_x1_status0_cl48TxEEEStates_c_TX_SLEEP           1
#define rx_x1_status0_cl48TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x1_status0_cl48RxEEEStates_l
 */
#define rx_x1_status0_cl48RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x1_status0_cl48RxEEEStates_l_RX_WAKE            16
#define rx_x1_status0_cl48RxEEEStates_l_RX_QUIET           8
#define rx_x1_status0_cl48RxEEEStates_l_RX_DEACT           4
#define rx_x1_status0_cl48RxEEEStates_l_RX_SLEEP           2
#define rx_x1_status0_cl48RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x1_status0_cl48RxEEEStates_c
 */
#define rx_x1_status0_cl48RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x1_status0_cl48RxEEEStates_c_RX_WAKE            4
#define rx_x1_status0_cl48RxEEEStates_c_RX_QUIET           3
#define rx_x1_status0_cl48RxEEEStates_c_RX_DEACT           2
#define rx_x1_status0_cl48RxEEEStates_c_RX_SLEEP           1
#define rx_x1_status0_cl48RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x1_status0_IQP_Options
 */
#define rx_x1_status0_IQP_Options_i50uA                    0
#define rx_x1_status0_IQP_Options_i100uA                   1
#define rx_x1_status0_IQP_Options_i150uA                   2
#define rx_x1_status0_IQP_Options_i200uA                   3
#define rx_x1_status0_IQP_Options_i250uA                   4
#define rx_x1_status0_IQP_Options_i300uA                   5
#define rx_x1_status0_IQP_Options_i350uA                   6
#define rx_x1_status0_IQP_Options_i400uA                   7
#define rx_x1_status0_IQP_Options_i450uA                   8
#define rx_x1_status0_IQP_Options_i500uA                   9
#define rx_x1_status0_IQP_Options_i550uA                   10
#define rx_x1_status0_IQP_Options_i600uA                   11
#define rx_x1_status0_IQP_Options_i650uA                   12
#define rx_x1_status0_IQP_Options_i700uA                   13
#define rx_x1_status0_IQP_Options_i750uA                   14
#define rx_x1_status0_IQP_Options_i800uA                   15

/****************************************************************************
 * Enums: rx_x1_status0_IDriver_Options
 */
#define rx_x1_status0_IDriver_Options_v680mV               0
#define rx_x1_status0_IDriver_Options_v730mV               1
#define rx_x1_status0_IDriver_Options_v780mV               2
#define rx_x1_status0_IDriver_Options_v830mV               3
#define rx_x1_status0_IDriver_Options_v880mV               4
#define rx_x1_status0_IDriver_Options_v930mV               5
#define rx_x1_status0_IDriver_Options_v980mV               6
#define rx_x1_status0_IDriver_Options_v1010mV              7
#define rx_x1_status0_IDriver_Options_v1040mV              8
#define rx_x1_status0_IDriver_Options_v1060mV              9
#define rx_x1_status0_IDriver_Options_v1070mV              10
#define rx_x1_status0_IDriver_Options_v1080mV              11
#define rx_x1_status0_IDriver_Options_v1085mV              12
#define rx_x1_status0_IDriver_Options_v1090mV              13
#define rx_x1_status0_IDriver_Options_v1095mV              14
#define rx_x1_status0_IDriver_Options_v1100mV              15

/****************************************************************************
 * Enums: rx_x1_status0_operationModes
 */
#define rx_x1_status0_operationModes_XGXS                  0
#define rx_x1_status0_operationModes_XGXG_nCC              1
#define rx_x1_status0_operationModes_Indlane_OS8           4
#define rx_x1_status0_operationModes_IndLane_OS5           5
#define rx_x1_status0_operationModes_PCI                   7
#define rx_x1_status0_operationModes_XGXS_nLQ              8
#define rx_x1_status0_operationModes_XGXS_nLQnCC           9
#define rx_x1_status0_operationModes_PBypass               10
#define rx_x1_status0_operationModes_PBypass_nDSK          11
#define rx_x1_status0_operationModes_ComboCoreMode         12
#define rx_x1_status0_operationModes_Clocks_off            15

/****************************************************************************
 * Enums: rx_x1_status0_actualSpeeds
 */
#define rx_x1_status0_actualSpeeds_dr_10M                  0
#define rx_x1_status0_actualSpeeds_dr_100M                 1
#define rx_x1_status0_actualSpeeds_dr_1G                   2
#define rx_x1_status0_actualSpeeds_dr_2p5G                 3
#define rx_x1_status0_actualSpeeds_dr_5G_X4                4
#define rx_x1_status0_actualSpeeds_dr_6G_X4                5
#define rx_x1_status0_actualSpeeds_dr_10G_HiG              6
#define rx_x1_status0_actualSpeeds_dr_10G_CX4              7
#define rx_x1_status0_actualSpeeds_dr_12G_HiG              8
#define rx_x1_status0_actualSpeeds_dr_12p5G_X4             9
#define rx_x1_status0_actualSpeeds_dr_13G_X4               10
#define rx_x1_status0_actualSpeeds_dr_15G_X4               11
#define rx_x1_status0_actualSpeeds_dr_16G_X4               12
#define rx_x1_status0_actualSpeeds_dr_1G_KX                13
#define rx_x1_status0_actualSpeeds_dr_10G_KX4              14
#define rx_x1_status0_actualSpeeds_dr_10G_KR               15
#define rx_x1_status0_actualSpeeds_dr_5G                   16
#define rx_x1_status0_actualSpeeds_dr_6p4G                 17
#define rx_x1_status0_actualSpeeds_dr_20G_X4               18
#define rx_x1_status0_actualSpeeds_dr_21G_X4               19
#define rx_x1_status0_actualSpeeds_dr_25G_X4               20
#define rx_x1_status0_actualSpeeds_dr_10G_HiG_DXGXS        21
#define rx_x1_status0_actualSpeeds_dr_10G_DXGXS            22
#define rx_x1_status0_actualSpeeds_dr_10p5G_HiG_DXGXS      23
#define rx_x1_status0_actualSpeeds_dr_10p5G_DXGXS          24
#define rx_x1_status0_actualSpeeds_dr_12p773G_HiG_DXGXS    25
#define rx_x1_status0_actualSpeeds_dr_12p773G_DXGXS        26
#define rx_x1_status0_actualSpeeds_dr_10G_XFI              27
#define rx_x1_status0_actualSpeeds_dr_40G                  28
#define rx_x1_status0_actualSpeeds_dr_20G_HiG_DXGXS        29
#define rx_x1_status0_actualSpeeds_dr_20G_DXGXS            30
#define rx_x1_status0_actualSpeeds_dr_10G_SFI              31
#define rx_x1_status0_actualSpeeds_dr_31p5G                32
#define rx_x1_status0_actualSpeeds_dr_32p7G                33
#define rx_x1_status0_actualSpeeds_dr_20G_SCR              34
#define rx_x1_status0_actualSpeeds_dr_10G_HiG_DXGXS_SCR    35
#define rx_x1_status0_actualSpeeds_dr_10G_DXGXS_SCR        36
#define rx_x1_status0_actualSpeeds_dr_12G_R2               37
#define rx_x1_status0_actualSpeeds_dr_10G_X2               38
#define rx_x1_status0_actualSpeeds_dr_40G_KR4              39
#define rx_x1_status0_actualSpeeds_dr_40G_CR4              40
#define rx_x1_status0_actualSpeeds_dr_100G_CR10            41
#define rx_x1_status0_actualSpeeds_dr_15p75G_DXGXS         44
#define rx_x1_status0_actualSpeeds_dr_20G_KR2              57
#define rx_x1_status0_actualSpeeds_dr_20G_CR2              58

/****************************************************************************
 * Enums: rx_x1_status0_actualSpeedsMisc1
 */
#define rx_x1_status0_actualSpeedsMisc1_dr_2500BRCM_X1     16
#define rx_x1_status0_actualSpeedsMisc1_dr_5000BRCM_X4     17
#define rx_x1_status0_actualSpeedsMisc1_dr_6000BRCM_X4     18
#define rx_x1_status0_actualSpeedsMisc1_dr_10GHiGig_X4     19
#define rx_x1_status0_actualSpeedsMisc1_dr_10GBASE_CX4     20
#define rx_x1_status0_actualSpeedsMisc1_dr_12GHiGig_X4     21
#define rx_x1_status0_actualSpeedsMisc1_dr_12p5GHiGig_X4   22
#define rx_x1_status0_actualSpeedsMisc1_dr_13GHiGig_X4     23
#define rx_x1_status0_actualSpeedsMisc1_dr_15GHiGig_X4     24
#define rx_x1_status0_actualSpeedsMisc1_dr_16GHiGig_X4     25
#define rx_x1_status0_actualSpeedsMisc1_dr_5000BRCM_X1     26
#define rx_x1_status0_actualSpeedsMisc1_dr_6363BRCM_X1     27
#define rx_x1_status0_actualSpeedsMisc1_dr_20GHiGig_X4     28
#define rx_x1_status0_actualSpeedsMisc1_dr_21GHiGig_X4     29
#define rx_x1_status0_actualSpeedsMisc1_dr_25p45GHiGig_X4  30
#define rx_x1_status0_actualSpeedsMisc1_dr_10G_HiG_DXGXS   31

/****************************************************************************
 * Enums: rx_x1_status0_IndLaneModes
 */
#define rx_x1_status0_IndLaneModes_SWSDR_div2              0
#define rx_x1_status0_IndLaneModes_SWSDR_div1              1
#define rx_x1_status0_IndLaneModes_DWSDR_div2              2
#define rx_x1_status0_IndLaneModes_DWSDR_div1              3

/****************************************************************************
 * Enums: rx_x1_status0_prbsSelect
 */
#define rx_x1_status0_prbsSelect_prbs7                     0
#define rx_x1_status0_prbsSelect_prbs15                    1
#define rx_x1_status0_prbsSelect_prbs23                    2
#define rx_x1_status0_prbsSelect_prbs31                    3

/****************************************************************************
 * Enums: rx_x1_status0_vcoDivider
 */
#define rx_x1_status0_vcoDivider_div32                     0
#define rx_x1_status0_vcoDivider_div36                     1
#define rx_x1_status0_vcoDivider_div40                     2
#define rx_x1_status0_vcoDivider_div42                     3
#define rx_x1_status0_vcoDivider_div48                     4
#define rx_x1_status0_vcoDivider_div50                     5
#define rx_x1_status0_vcoDivider_div52                     6
#define rx_x1_status0_vcoDivider_div54                     7
#define rx_x1_status0_vcoDivider_div60                     8
#define rx_x1_status0_vcoDivider_div64                     9
#define rx_x1_status0_vcoDivider_div66                     10
#define rx_x1_status0_vcoDivider_div68                     11
#define rx_x1_status0_vcoDivider_div70                     12
#define rx_x1_status0_vcoDivider_div80                     13
#define rx_x1_status0_vcoDivider_div92                     14
#define rx_x1_status0_vcoDivider_div100                    15

/****************************************************************************
 * Enums: rx_x1_status0_refClkSelect
 */
#define rx_x1_status0_refClkSelect_clk_25MHz               0
#define rx_x1_status0_refClkSelect_clk_100MHz              1
#define rx_x1_status0_refClkSelect_clk_125MHz              2
#define rx_x1_status0_refClkSelect_clk_156p25MHz           3
#define rx_x1_status0_refClkSelect_clk_187p5MHz            4
#define rx_x1_status0_refClkSelect_clk_161p25Mhz           5
#define rx_x1_status0_refClkSelect_clk_50Mhz               6
#define rx_x1_status0_refClkSelect_clk_106p25Mhz           7

/****************************************************************************
 * Enums: rx_x1_status0_aerMMDdevTypeSelect
 */
#define rx_x1_status0_aerMMDdevTypeSelect_combo_core       0
#define rx_x1_status0_aerMMDdevTypeSelect_PMA_PMD          1
#define rx_x1_status0_aerMMDdevTypeSelect_PCS              3
#define rx_x1_status0_aerMMDdevTypeSelect_PHY              4
#define rx_x1_status0_aerMMDdevTypeSelect_DTE              5
#define rx_x1_status0_aerMMDdevTypeSelect_CL73_AN          7

/****************************************************************************
 * Enums: rx_x1_status0_aerMMDportSelect
 */
#define rx_x1_status0_aerMMDportSelect_ln0                 0
#define rx_x1_status0_aerMMDportSelect_ln1                 1
#define rx_x1_status0_aerMMDportSelect_ln2                 2
#define rx_x1_status0_aerMMDportSelect_ln3                 3
#define rx_x1_status0_aerMMDportSelect_BCST_ln0_1_2_3      511
#define rx_x1_status0_aerMMDportSelect_BCST_ln0_1          512
#define rx_x1_status0_aerMMDportSelect_BCST_ln2_3          513

/****************************************************************************
 * Enums: rx_x1_status0_firmwareModeSelect
 */
#define rx_x1_status0_firmwareModeSelect_DEFAULT           0
#define rx_x1_status0_firmwareModeSelect_SFP_OPT_LR        1
#define rx_x1_status0_firmwareModeSelect_SFP_DAC           2
#define rx_x1_status0_firmwareModeSelect_XLAUI             3
#define rx_x1_status0_firmwareModeSelect_LONG_CH_6G        4

/****************************************************************************
 * Enums: rx_x1_status0_tempIdxSelect
 */
#define rx_x1_status0_tempIdxSelect_LTE__22p9C             15
#define rx_x1_status0_tempIdxSelect_LTE__12p6C             14
#define rx_x1_status0_tempIdxSelect_LTE__3p0C              13
#define rx_x1_status0_tempIdxSelect_LTE_6p7C               12
#define rx_x1_status0_tempIdxSelect_LTE_16p4C              11
#define rx_x1_status0_tempIdxSelect_LTE_26p6C              10
#define rx_x1_status0_tempIdxSelect_LTE_36p3C              9
#define rx_x1_status0_tempIdxSelect_LTE_46p0C              8
#define rx_x1_status0_tempIdxSelect_LTE_56p2C              7
#define rx_x1_status0_tempIdxSelect_LTE_65p9C              6
#define rx_x1_status0_tempIdxSelect_LTE_75p6C              5
#define rx_x1_status0_tempIdxSelect_LTE_85p3C              4
#define rx_x1_status0_tempIdxSelect_LTE_95p5C              3
#define rx_x1_status0_tempIdxSelect_LTE_105p2C             2
#define rx_x1_status0_tempIdxSelect_LTE_114p9C             1
#define rx_x1_status0_tempIdxSelect_LTE_125p1C             0

/****************************************************************************
 * Enums: rx_x1_status0_port_mode
 */
#define rx_x1_status0_port_mode_QUAD_PORT                  0
#define rx_x1_status0_port_mode_TRI_1_PORT                 1
#define rx_x1_status0_port_mode_TRI_2_PORT                 2
#define rx_x1_status0_port_mode_DUAL_PORT                  3
#define rx_x1_status0_port_mode_SINGLE_PORT                4

/****************************************************************************
 * Enums: rx_x1_status0_rev_letter_enum
 */
#define rx_x1_status0_rev_letter_enum_REV_A                0
#define rx_x1_status0_rev_letter_enum_REV_B                1
#define rx_x1_status0_rev_letter_enum_REV_C                2
#define rx_x1_status0_rev_letter_enum_REV_D                3

/****************************************************************************
 * Enums: rx_x1_status0_rev_number_enum
 */
#define rx_x1_status0_rev_number_enum_REV_0                0
#define rx_x1_status0_rev_number_enum_REV_1                1
#define rx_x1_status0_rev_number_enum_REV_2                2
#define rx_x1_status0_rev_number_enum_REV_3                3
#define rx_x1_status0_rev_number_enum_REV_4                4
#define rx_x1_status0_rev_number_enum_REV_5                5
#define rx_x1_status0_rev_number_enum_REV_6                6
#define rx_x1_status0_rev_number_enum_REV_7                7

/****************************************************************************
 * Enums: rx_x1_status0_bonding_enum
 */
#define rx_x1_status0_bonding_enum_WIRE_BOND               0
#define rx_x1_status0_bonding_enum_FLIP_CHIP               1

/****************************************************************************
 * Enums: rx_x1_status0_tech_process
 */
#define rx_x1_status0_tech_process_PROCESS_90NM            0
#define rx_x1_status0_tech_process_PROCESS_65NM            1
#define rx_x1_status0_tech_process_PROCESS_40NM            2
#define rx_x1_status0_tech_process_PROCESS_28NM            3

/****************************************************************************
 * Enums: rx_x1_status0_model_num
 */
#define rx_x1_status0_model_num_SERDES_CL73                0
#define rx_x1_status0_model_num_XGXS_16G                   1
#define rx_x1_status0_model_num_HYPERCORE                  2
#define rx_x1_status0_model_num_HYPERLITE                  3
#define rx_x1_status0_model_num_PCIE_G2_PIPE               4
#define rx_x1_status0_model_num_SERDES_1p25GBd             5
#define rx_x1_status0_model_num_SATA2                      6
#define rx_x1_status0_model_num_QSGMII                     7
#define rx_x1_status0_model_num_XGXS10G                    8
#define rx_x1_status0_model_num_WARPCORE                   9
#define rx_x1_status0_model_num_XFICORE                    10
#define rx_x1_status0_model_num_RXFI                       11
#define rx_x1_status0_model_num_WARPLITE                   12
#define rx_x1_status0_model_num_PENTACORE                  13
#define rx_x1_status0_model_num_ESM                        14
#define rx_x1_status0_model_num_QUAD_SGMII                 15
#define rx_x1_status0_model_num_WARPCORE_3                 16
#define rx_x1_status0_model_num_TSC                        17
#define rx_x1_status0_model_num_TSC4E                      18
#define rx_x1_status0_model_num_TSC12E                     19
#define rx_x1_status0_model_num_TSC4F                      20
#define rx_x1_status0_model_num_XGXS_CL73_90NM             29
#define rx_x1_status0_model_num_SERDES_CL73_90NM           30
#define rx_x1_status0_model_num_WARPCORE3                  32
#define rx_x1_status0_model_num_WARPCORE4_TSC              33
#define rx_x1_status0_model_num_RXAUI                      34

/****************************************************************************
 * Enums: rx_x1_status0_payload
 */
#define rx_x1_status0_payload_REPEAT_2_BYTES               0
#define rx_x1_status0_payload_RAMPING                      1
#define rx_x1_status0_payload_CL48_CRPAT                   2
#define rx_x1_status0_payload_CL48_CJPAT                   3
#define rx_x1_status0_payload_CL36_LONG_CRPAT              4
#define rx_x1_status0_payload_CL36_SHORT_CRPAT             5

/****************************************************************************
 * Enums: rx_x1_status0_sc
 */
#define rx_x1_status0_sc_S_10G_CR1                         0
#define rx_x1_status0_sc_S_10G_KR1                         1
#define rx_x1_status0_sc_S_10G_X1                          2
#define rx_x1_status0_sc_S_10G_HG2_CR1                     4
#define rx_x1_status0_sc_S_10G_HG2_KR1                     5
#define rx_x1_status0_sc_S_10G_HG2_X1                      6
#define rx_x1_status0_sc_S_20G_CR1                         8
#define rx_x1_status0_sc_S_20G_KR1                         9
#define rx_x1_status0_sc_S_20G_X1                          10
#define rx_x1_status0_sc_S_20G_HG2_CR1                     12
#define rx_x1_status0_sc_S_20G_HG2_KR1                     13
#define rx_x1_status0_sc_S_20G_HG2_X1                      14
#define rx_x1_status0_sc_S_25G_CR1                         16
#define rx_x1_status0_sc_S_25G_KR1                         17
#define rx_x1_status0_sc_S_25G_X1                          18
#define rx_x1_status0_sc_S_25G_HG2_CR1                     20
#define rx_x1_status0_sc_S_25G_HG2_KR1                     21
#define rx_x1_status0_sc_S_25G_HG2_X1                      22
#define rx_x1_status0_sc_S_20G_CR2                         24
#define rx_x1_status0_sc_S_20G_KR2                         25
#define rx_x1_status0_sc_S_20G_X2                          26
#define rx_x1_status0_sc_S_20G_HG2_CR2                     28
#define rx_x1_status0_sc_S_20G_HG2_KR2                     29
#define rx_x1_status0_sc_S_20G_HG2_X2                      30
#define rx_x1_status0_sc_S_40G_CR2                         32
#define rx_x1_status0_sc_S_40G_KR2                         33
#define rx_x1_status0_sc_S_40G_X2                          34
#define rx_x1_status0_sc_S_40G_HG2_CR2                     36
#define rx_x1_status0_sc_S_40G_HG2_KR2                     37
#define rx_x1_status0_sc_S_40G_HG2_X2                      38
#define rx_x1_status0_sc_S_40G_CR4                         40
#define rx_x1_status0_sc_S_40G_KR4                         41
#define rx_x1_status0_sc_S_40G_X4                          42
#define rx_x1_status0_sc_S_40G_HG2_CR4                     44
#define rx_x1_status0_sc_S_40G_HG2_KR4                     45
#define rx_x1_status0_sc_S_40G_HG2_X4                      46
#define rx_x1_status0_sc_S_50G_CR2                         48
#define rx_x1_status0_sc_S_50G_KR2                         49
#define rx_x1_status0_sc_S_50G_X2                          50
#define rx_x1_status0_sc_S_50G_HG2_CR2                     52
#define rx_x1_status0_sc_S_50G_HG2_KR2                     53
#define rx_x1_status0_sc_S_50G_HG2_X2                      54
#define rx_x1_status0_sc_S_50G_CR4                         56
#define rx_x1_status0_sc_S_50G_KR4                         57
#define rx_x1_status0_sc_S_50G_X4                          58
#define rx_x1_status0_sc_S_50G_HG2_CR4                     60
#define rx_x1_status0_sc_S_50G_HG2_KR4                     61
#define rx_x1_status0_sc_S_50G_HG2_X4                      62
#define rx_x1_status0_sc_S_100G_CR4                        64
#define rx_x1_status0_sc_S_100G_KR4                        65
#define rx_x1_status0_sc_S_100G_X4                         66
#define rx_x1_status0_sc_S_100G_HG2_CR4                    68
#define rx_x1_status0_sc_S_100G_HG2_KR4                    69
#define rx_x1_status0_sc_S_100G_HG2_X4                     70
#define rx_x1_status0_sc_S_CL73_20GVCO                     72
#define rx_x1_status0_sc_S_CL73_25GVCO                     80
#define rx_x1_status0_sc_S_CL36_20GVCO                     88
#define rx_x1_status0_sc_S_CL36_25GVCO                     96

/****************************************************************************
 * Enums: rx_x1_status0_t_fifo_modes
 */
#define rx_x1_status0_t_fifo_modes_T_FIFO_MODE_BYPASS      0
#define rx_x1_status0_t_fifo_modes_T_FIFO_MODE_40G         1
#define rx_x1_status0_t_fifo_modes_T_FIFO_MODE_100G        2
#define rx_x1_status0_t_fifo_modes_T_FIFO_MODE_20G         3

/****************************************************************************
 * Enums: rx_x1_status0_t_enc_modes
 */
#define rx_x1_status0_t_enc_modes_T_ENC_MODE_BYPASS        0
#define rx_x1_status0_t_enc_modes_T_ENC_MODE_CL49          1
#define rx_x1_status0_t_enc_modes_T_ENC_MODE_CL82          2

/****************************************************************************
 * Enums: rx_x1_status0_btmx_mode
 */
#define rx_x1_status0_btmx_mode_BS_BTMX_MODE_1to1          0
#define rx_x1_status0_btmx_mode_BS_BTMX_MODE_2to1          1
#define rx_x1_status0_btmx_mode_BS_BTMX_MODE_5to1          2

/****************************************************************************
 * Enums: rx_x1_status0_t_type_cl82
 */
#define rx_x1_status0_t_type_cl82_T_TYPE_B1                5
#define rx_x1_status0_t_type_cl82_T_TYPE_C                 4
#define rx_x1_status0_t_type_cl82_T_TYPE_S                 3
#define rx_x1_status0_t_type_cl82_T_TYPE_T                 2
#define rx_x1_status0_t_type_cl82_T_TYPE_D                 1
#define rx_x1_status0_t_type_cl82_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x1_status0_txsm_state_cl82
 */
#define rx_x1_status0_txsm_state_cl82_TX_HIG_END           6
#define rx_x1_status0_txsm_state_cl82_TX_HIG_START         5
#define rx_x1_status0_txsm_state_cl82_TX_E                 4
#define rx_x1_status0_txsm_state_cl82_TX_T                 3
#define rx_x1_status0_txsm_state_cl82_TX_D                 2
#define rx_x1_status0_txsm_state_cl82_TX_C                 1
#define rx_x1_status0_txsm_state_cl82_TX_INIT              0

/****************************************************************************
 * Enums: rx_x1_status0_ltxsm_state_cl82
 */
#define rx_x1_status0_ltxsm_state_cl82_TX_HIG_END          64
#define rx_x1_status0_ltxsm_state_cl82_TX_HIG_START        32
#define rx_x1_status0_ltxsm_state_cl82_TX_E                16
#define rx_x1_status0_ltxsm_state_cl82_TX_T                8
#define rx_x1_status0_ltxsm_state_cl82_TX_D                4
#define rx_x1_status0_ltxsm_state_cl82_TX_C                2
#define rx_x1_status0_ltxsm_state_cl82_TX_INIT             1

/****************************************************************************
 * Enums: rx_x1_status0_r_type_coded_cl82
 */
#define rx_x1_status0_r_type_coded_cl82_R_TYPE_B1          32
#define rx_x1_status0_r_type_coded_cl82_R_TYPE_C           16
#define rx_x1_status0_r_type_coded_cl82_R_TYPE_S           8
#define rx_x1_status0_r_type_coded_cl82_R_TYPE_T           4
#define rx_x1_status0_r_type_coded_cl82_R_TYPE_D           2
#define rx_x1_status0_r_type_coded_cl82_R_TYPE_E           1

/****************************************************************************
 * Enums: rx_x1_status0_rxsm_state_cl82
 */
#define rx_x1_status0_rxsm_state_cl82_RX_HIG_END           64
#define rx_x1_status0_rxsm_state_cl82_RX_HIG_START         32
#define rx_x1_status0_rxsm_state_cl82_RX_E                 16
#define rx_x1_status0_rxsm_state_cl82_RX_T                 8
#define rx_x1_status0_rxsm_state_cl82_RX_D                 4
#define rx_x1_status0_rxsm_state_cl82_RX_C                 2
#define rx_x1_status0_rxsm_state_cl82_RX_INIT              1

/****************************************************************************
 * Enums: rx_x1_status0_deskew_state
 */
#define rx_x1_status0_deskew_state_ALIGN_ACQUIRED          2
#define rx_x1_status0_deskew_state_LOSS_OF_ALIGNMENT       1

/****************************************************************************
 * Enums: rx_x1_status0_os_mode_enum
 */
#define rx_x1_status0_os_mode_enum_OS_MODE_1               0
#define rx_x1_status0_os_mode_enum_OS_MODE_2               1
#define rx_x1_status0_os_mode_enum_OS_MODE_4               2
#define rx_x1_status0_os_mode_enum_OS_MODE_16p5            8
#define rx_x1_status0_os_mode_enum_OS_MODE_20p625          12

/****************************************************************************
 * Enums: rx_x1_status0_scr_modes
 */
#define rx_x1_status0_scr_modes_T_SCR_MODE_BYPASS          0
#define rx_x1_status0_scr_modes_T_SCR_MODE_CL49            1
#define rx_x1_status0_scr_modes_T_SCR_MODE_40G_2_LANE      2
#define rx_x1_status0_scr_modes_T_SCR_MODE_100G            3
#define rx_x1_status0_scr_modes_T_SCR_MODE_20G             4
#define rx_x1_status0_scr_modes_T_SCR_MODE_40G_4_LANE      5

/****************************************************************************
 * Enums: rx_x1_status0_descr_modes
 */
#define rx_x1_status0_descr_modes_R_DESCR_MODE_BYPASS      0
#define rx_x1_status0_descr_modes_R_DESCR_MODE_CL49        1
#define rx_x1_status0_descr_modes_R_DESCR_MODE_CL82        2

/****************************************************************************
 * Enums: rx_x1_status0_r_dec_tl_mode
 */
#define rx_x1_status0_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS   0
#define rx_x1_status0_r_dec_tl_mode_R_DEC_TL_MODE_CL49     1
#define rx_x1_status0_r_dec_tl_mode_R_DEC_TL_MODE_CL82     2

/****************************************************************************
 * Enums: rx_x1_status0_r_dec_fsm_mode
 */
#define rx_x1_status0_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define rx_x1_status0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49   1
#define rx_x1_status0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82   2

/****************************************************************************
 * Enums: rx_x1_status0_r_deskew_mode
 */
#define rx_x1_status0_r_deskew_mode_R_DESKEW_MODE_BYPASS   0
#define rx_x1_status0_r_deskew_mode_R_DESKEW_MODE_20G      1
#define rx_x1_status0_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define rx_x1_status0_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define rx_x1_status0_r_deskew_mode_R_DESKEW_MODE_100G     4
#define rx_x1_status0_r_deskew_mode_R_DESKEW_MODE_CL49     5
#define rx_x1_status0_r_deskew_mode_R_DESKEW_MODE_CL91     6

/****************************************************************************
 * Enums: rx_x1_status0_bs_dist_modes
 */
#define rx_x1_status0_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define rx_x1_status0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define rx_x1_status0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define rx_x1_status0_bs_dist_modes_BS_DIST_MODE_NO_TDM    3

/****************************************************************************
 * Enums: rx_x1_status0_cl49_t_type
 */
#define rx_x1_status0_cl49_t_type_BAD_T_TYPE               15
#define rx_x1_status0_cl49_t_type_T_TYPE_B0                11
#define rx_x1_status0_cl49_t_type_T_TYPE_OB                10
#define rx_x1_status0_cl49_t_type_T_TYPE_B1                9
#define rx_x1_status0_cl49_t_type_T_TYPE_DB                8
#define rx_x1_status0_cl49_t_type_T_TYPE_FC                7
#define rx_x1_status0_cl49_t_type_T_TYPE_TB                6
#define rx_x1_status0_cl49_t_type_T_TYPE_LI                5
#define rx_x1_status0_cl49_t_type_T_TYPE_C                 4
#define rx_x1_status0_cl49_t_type_T_TYPE_S                 3
#define rx_x1_status0_cl49_t_type_T_TYPE_T                 2
#define rx_x1_status0_cl49_t_type_T_TYPE_D                 1
#define rx_x1_status0_cl49_t_type_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x1_status0_cl49_txsm_states
 */
#define rx_x1_status0_cl49_txsm_states_TX_HIG_END          7
#define rx_x1_status0_cl49_txsm_states_TX_HIG_START        6
#define rx_x1_status0_cl49_txsm_states_TX_LI               5
#define rx_x1_status0_cl49_txsm_states_TX_E                4
#define rx_x1_status0_cl49_txsm_states_TX_T                3
#define rx_x1_status0_cl49_txsm_states_TX_D                2
#define rx_x1_status0_cl49_txsm_states_TX_C                1
#define rx_x1_status0_cl49_txsm_states_TX_INIT             0

/****************************************************************************
 * Enums: rx_x1_status0_cl49_ltxsm_states
 */
#define rx_x1_status0_cl49_ltxsm_states_TX_HIG_END         128
#define rx_x1_status0_cl49_ltxsm_states_TX_HIG_START       64
#define rx_x1_status0_cl49_ltxsm_states_TX_LI              32
#define rx_x1_status0_cl49_ltxsm_states_TX_E               16
#define rx_x1_status0_cl49_ltxsm_states_TX_T               8
#define rx_x1_status0_cl49_ltxsm_states_TX_D               4
#define rx_x1_status0_cl49_ltxsm_states_TX_C               2
#define rx_x1_status0_cl49_ltxsm_states_TX_INIT            1

/****************************************************************************
 * Enums: rx_x1_status0_burst_error_mode
 */
#define rx_x1_status0_burst_error_mode_BURST_ERROR_11_BITS 0
#define rx_x1_status0_burst_error_mode_BURST_ERROR_16_BITS 1
#define rx_x1_status0_burst_error_mode_BURST_ERROR_17_BITS 2
#define rx_x1_status0_burst_error_mode_BURST_ERROR_18_BITS 3
#define rx_x1_status0_burst_error_mode_BURST_ERROR_19_BITS 4
#define rx_x1_status0_burst_error_mode_BURST_ERROR_20_BITS 5
#define rx_x1_status0_burst_error_mode_BURST_ERROR_21_BITS 6
#define rx_x1_status0_burst_error_mode_BURST_ERROR_22_BITS 7
#define rx_x1_status0_burst_error_mode_BURST_ERROR_23_BITS 8
#define rx_x1_status0_burst_error_mode_BURST_ERROR_24_BITS 9
#define rx_x1_status0_burst_error_mode_BURST_ERROR_25_BITS 10
#define rx_x1_status0_burst_error_mode_BURST_ERROR_26_BITS 11
#define rx_x1_status0_burst_error_mode_BURST_ERROR_27_BITS 12
#define rx_x1_status0_burst_error_mode_BURST_ERROR_28_BITS 13
#define rx_x1_status0_burst_error_mode_BURST_ERROR_29_BITS 14
#define rx_x1_status0_burst_error_mode_BURST_ERROR_30_BITS 15
#define rx_x1_status0_burst_error_mode_BURST_ERROR_31_BITS 16
#define rx_x1_status0_burst_error_mode_BURST_ERROR_32_BITS 17
#define rx_x1_status0_burst_error_mode_BURST_ERROR_33_BITS 18
#define rx_x1_status0_burst_error_mode_BURST_ERROR_34_BITS 19
#define rx_x1_status0_burst_error_mode_BURST_ERROR_35_BITS 20
#define rx_x1_status0_burst_error_mode_BURST_ERROR_36_BITS 21
#define rx_x1_status0_burst_error_mode_BURST_ERROR_37_BITS 22
#define rx_x1_status0_burst_error_mode_BURST_ERROR_38_BITS 23
#define rx_x1_status0_burst_error_mode_BURST_ERROR_39_BITS 24
#define rx_x1_status0_burst_error_mode_BURST_ERROR_40_BITS 25
#define rx_x1_status0_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: rx_x1_status0_bermon_state
 */
#define rx_x1_status0_bermon_state_HI_BER                  4
#define rx_x1_status0_bermon_state_GOOD_BER                3
#define rx_x1_status0_bermon_state_BER_TEST_SH             2
#define rx_x1_status0_bermon_state_START_TIMER             1
#define rx_x1_status0_bermon_state_BER_MT_INIT             0

/****************************************************************************
 * Enums: rx_x1_status0_rxsm_state_cl49
 */
#define rx_x1_status0_rxsm_state_cl49_RX_HIG_END           128
#define rx_x1_status0_rxsm_state_cl49_RX_HIG_START         64
#define rx_x1_status0_rxsm_state_cl49_RX_LI                32
#define rx_x1_status0_rxsm_state_cl49_RX_E                 16
#define rx_x1_status0_rxsm_state_cl49_RX_T                 8
#define rx_x1_status0_rxsm_state_cl49_RX_D                 4
#define rx_x1_status0_rxsm_state_cl49_RX_C                 2
#define rx_x1_status0_rxsm_state_cl49_RX_INIT              1

/****************************************************************************
 * Enums: rx_x1_status0_r_type
 */
#define rx_x1_status0_r_type_BAD_R_TYPE                    15
#define rx_x1_status0_r_type_R_TYPE_B0                     11
#define rx_x1_status0_r_type_R_TYPE_OB                     10
#define rx_x1_status0_r_type_R_TYPE_B1                     9
#define rx_x1_status0_r_type_R_TYPE_DB                     8
#define rx_x1_status0_r_type_R_TYPE_FC                     7
#define rx_x1_status0_r_type_R_TYPE_TB                     6
#define rx_x1_status0_r_type_R_TYPE_LI                     5
#define rx_x1_status0_r_type_R_TYPE_C                      4
#define rx_x1_status0_r_type_R_TYPE_S                      3
#define rx_x1_status0_r_type_R_TYPE_T                      2
#define rx_x1_status0_r_type_R_TYPE_D                      1
#define rx_x1_status0_r_type_R_TYPE_E                      0

/****************************************************************************
 * Enums: rx_x1_status0_am_lock_state
 */
#define rx_x1_status0_am_lock_state_INVALID_AM             512
#define rx_x1_status0_am_lock_state_GOOD_AM                256
#define rx_x1_status0_am_lock_state_COMP_AM                128
#define rx_x1_status0_am_lock_state_TIMER_2                64
#define rx_x1_status0_am_lock_state_AM_2_GOOD              32
#define rx_x1_status0_am_lock_state_COMP_2ND               16
#define rx_x1_status0_am_lock_state_TIMER_1                8
#define rx_x1_status0_am_lock_state_FIND_1ST               4
#define rx_x1_status0_am_lock_state_AM_RESET_CNT           2
#define rx_x1_status0_am_lock_state_AM_LOCK_INIT           1

/****************************************************************************
 * Enums: rx_x1_status0_msg_selector
 */
#define rx_x1_status0_msg_selector_RESERVED                0
#define rx_x1_status0_msg_selector_VALUE_802p3             1
#define rx_x1_status0_msg_selector_VALUE_802p9             2
#define rx_x1_status0_msg_selector_VALUE_802p5             3
#define rx_x1_status0_msg_selector_VALUE_1394              4

/****************************************************************************
 * Enums: rx_x1_status0_synce_enum
 */
#define rx_x1_status0_synce_enum_SYNCE_NO_DIV              0
#define rx_x1_status0_synce_enum_SYNCE_DIV_7               1
#define rx_x1_status0_synce_enum_SYNCE_DIV_11              2

/****************************************************************************
 * Enums: rx_x1_status0_synce_enum_stage0
 */
#define rx_x1_status0_synce_enum_stage0_SYNCE_NO_DIV       0
#define rx_x1_status0_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define rx_x1_status0_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: rx_x1_status0_cl91_sync_state
 */
#define rx_x1_status0_cl91_sync_state_FIND_1ST             0
#define rx_x1_status0_cl91_sync_state_COUNT_NEXT           1
#define rx_x1_status0_cl91_sync_state_COMP_2ND             2
#define rx_x1_status0_cl91_sync_state_TWO_GOOD             3

/****************************************************************************
 * Enums: rx_x1_status0_cl91_algn_state
 */
#define rx_x1_status0_cl91_algn_state_LOSS_OF_ALIGNMENT    0
#define rx_x1_status0_cl91_algn_state_DESKEW               1
#define rx_x1_status0_cl91_algn_state_DESKEW_FAIL          2
#define rx_x1_status0_cl91_algn_state_ALIGN_ACQUIRED       3
#define rx_x1_status0_cl91_algn_state_CW_GOOD              4
#define rx_x1_status0_cl91_algn_state_CW_BAD               5
#define rx_x1_status0_cl91_algn_state_THREE_BAD            6

/****************************************************************************
 * Enums: rx_x1_status0_fec_sel
 */
#define rx_x1_status0_fec_sel_NO_FEC                       0
#define rx_x1_status0_fec_sel_FEC_CL74                     1
#define rx_x1_status0_fec_sel_FEC_CL91                     2

/****************************************************************************
 * Enums: rx_x4_control0_cl36TxEEEStates_l
 */
#define rx_x4_control0_cl36TxEEEStates_l_TX_REFRESH        8
#define rx_x4_control0_cl36TxEEEStates_l_TX_QUIET          4
#define rx_x4_control0_cl36TxEEEStates_l_TX_SLEEP          2
#define rx_x4_control0_cl36TxEEEStates_l_TX_ACTIVE         1

/****************************************************************************
 * Enums: rx_x4_control0_cl36TxEEEStates_c
 */
#define rx_x4_control0_cl36TxEEEStates_c_TX_REFRESH        3
#define rx_x4_control0_cl36TxEEEStates_c_TX_QUIET          2
#define rx_x4_control0_cl36TxEEEStates_c_TX_SLEEP          1
#define rx_x4_control0_cl36TxEEEStates_c_TX_ACTIVE         0

/****************************************************************************
 * Enums: rx_x4_control0_cl49TxEEEStates_l
 */
#define rx_x4_control0_cl49TxEEEStates_l_SCR_RESET_2       64
#define rx_x4_control0_cl49TxEEEStates_l_SCR_RESET_1       32
#define rx_x4_control0_cl49TxEEEStates_l_TX_WAKE           16
#define rx_x4_control0_cl49TxEEEStates_l_TX_REFRESH        8
#define rx_x4_control0_cl49TxEEEStates_l_TX_QUIET          4
#define rx_x4_control0_cl49TxEEEStates_l_TX_SLEEP          2
#define rx_x4_control0_cl49TxEEEStates_l_TX_ACTIVE         1

/****************************************************************************
 * Enums: rx_x4_control0_fec_req_enum
 */
#define rx_x4_control0_fec_req_enum_FEC_not_supported      0
#define rx_x4_control0_fec_req_enum_FEC_supported_but_not_requested 1
#define rx_x4_control0_fec_req_enum_invalid_setting        2
#define rx_x4_control0_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: rx_x4_control0_cl73_pause_enum
 */
#define rx_x4_control0_cl73_pause_enum_No_PAUSE_ability    0
#define rx_x4_control0_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define rx_x4_control0_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define rx_x4_control0_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: rx_x4_control0_cl49TxEEEStates_c
 */
#define rx_x4_control0_cl49TxEEEStates_c_SCR_RESET_2       6
#define rx_x4_control0_cl49TxEEEStates_c_SCR_RESET_1       5
#define rx_x4_control0_cl49TxEEEStates_c_TX_WAKE           4
#define rx_x4_control0_cl49TxEEEStates_c_TX_REFRESH        3
#define rx_x4_control0_cl49TxEEEStates_c_TX_QUIET          2
#define rx_x4_control0_cl49TxEEEStates_c_TX_SLEEP          1
#define rx_x4_control0_cl49TxEEEStates_c_TX_ACTIVE         0

/****************************************************************************
 * Enums: rx_x4_control0_cl36RxEEEStates_l
 */
#define rx_x4_control0_cl36RxEEEStates_l_RX_LINK_FAIL      32
#define rx_x4_control0_cl36RxEEEStates_l_RX_WTF            16
#define rx_x4_control0_cl36RxEEEStates_l_RX_WAKE           8
#define rx_x4_control0_cl36RxEEEStates_l_RX_QUIET          4
#define rx_x4_control0_cl36RxEEEStates_l_RX_SLEEP          2
#define rx_x4_control0_cl36RxEEEStates_l_RX_ACTIVE         1

/****************************************************************************
 * Enums: rx_x4_control0_cl36RxEEEStates_c
 */
#define rx_x4_control0_cl36RxEEEStates_c_RX_LINK_FAIL      5
#define rx_x4_control0_cl36RxEEEStates_c_RX_WTF            4
#define rx_x4_control0_cl36RxEEEStates_c_RX_WAKE           3
#define rx_x4_control0_cl36RxEEEStates_c_RX_QUIET          2
#define rx_x4_control0_cl36RxEEEStates_c_RX_SLEEP          1
#define rx_x4_control0_cl36RxEEEStates_c_RX_ACTIVE         0

/****************************************************************************
 * Enums: rx_x4_control0_cl49RxEEEStates_l
 */
#define rx_x4_control0_cl49RxEEEStates_l_RX_LINK_FAIL      32
#define rx_x4_control0_cl49RxEEEStates_l_RX_WTF            16
#define rx_x4_control0_cl49RxEEEStates_l_RX_WAKE           8
#define rx_x4_control0_cl49RxEEEStates_l_RX_QUIET          4
#define rx_x4_control0_cl49RxEEEStates_l_RX_SLEEP          2
#define rx_x4_control0_cl49RxEEEStates_l_RX_ACTIVE         1

/****************************************************************************
 * Enums: rx_x4_control0_cl49RxEEEStates_c
 */
#define rx_x4_control0_cl49RxEEEStates_c_RX_LINK_FAIL      5
#define rx_x4_control0_cl49RxEEEStates_c_RX_WTF            4
#define rx_x4_control0_cl49RxEEEStates_c_RX_WAKE           3
#define rx_x4_control0_cl49RxEEEStates_c_RX_QUIET          2
#define rx_x4_control0_cl49RxEEEStates_c_RX_SLEEP          1
#define rx_x4_control0_cl49RxEEEStates_c_RX_ACTIVE         0

/****************************************************************************
 * Enums: rx_x4_control0_cl48TxEEEStates_l
 */
#define rx_x4_control0_cl48TxEEEStates_l_TX_REFRESH        8
#define rx_x4_control0_cl48TxEEEStates_l_TX_QUIET          4
#define rx_x4_control0_cl48TxEEEStates_l_TX_SLEEP          2
#define rx_x4_control0_cl48TxEEEStates_l_TX_ACTIVE         1

/****************************************************************************
 * Enums: rx_x4_control0_cl48TxEEEStates_c
 */
#define rx_x4_control0_cl48TxEEEStates_c_TX_REFRESH        3
#define rx_x4_control0_cl48TxEEEStates_c_TX_QUIET          2
#define rx_x4_control0_cl48TxEEEStates_c_TX_SLEEP          1
#define rx_x4_control0_cl48TxEEEStates_c_TX_ACTIVE         0

/****************************************************************************
 * Enums: rx_x4_control0_cl48RxEEEStates_l
 */
#define rx_x4_control0_cl48RxEEEStates_l_RX_LINK_FAIL      32
#define rx_x4_control0_cl48RxEEEStates_l_RX_WAKE           16
#define rx_x4_control0_cl48RxEEEStates_l_RX_QUIET          8
#define rx_x4_control0_cl48RxEEEStates_l_RX_DEACT          4
#define rx_x4_control0_cl48RxEEEStates_l_RX_SLEEP          2
#define rx_x4_control0_cl48RxEEEStates_l_RX_ACTIVE         1

/****************************************************************************
 * Enums: rx_x4_control0_cl48RxEEEStates_c
 */
#define rx_x4_control0_cl48RxEEEStates_c_RX_LINK_FAIL      5
#define rx_x4_control0_cl48RxEEEStates_c_RX_WAKE           4
#define rx_x4_control0_cl48RxEEEStates_c_RX_QUIET          3
#define rx_x4_control0_cl48RxEEEStates_c_RX_DEACT          2
#define rx_x4_control0_cl48RxEEEStates_c_RX_SLEEP          1
#define rx_x4_control0_cl48RxEEEStates_c_RX_ACTIVE         0

/****************************************************************************
 * Enums: rx_x4_control0_IQP_Options
 */
#define rx_x4_control0_IQP_Options_i50uA                   0
#define rx_x4_control0_IQP_Options_i100uA                  1
#define rx_x4_control0_IQP_Options_i150uA                  2
#define rx_x4_control0_IQP_Options_i200uA                  3
#define rx_x4_control0_IQP_Options_i250uA                  4
#define rx_x4_control0_IQP_Options_i300uA                  5
#define rx_x4_control0_IQP_Options_i350uA                  6
#define rx_x4_control0_IQP_Options_i400uA                  7
#define rx_x4_control0_IQP_Options_i450uA                  8
#define rx_x4_control0_IQP_Options_i500uA                  9
#define rx_x4_control0_IQP_Options_i550uA                  10
#define rx_x4_control0_IQP_Options_i600uA                  11
#define rx_x4_control0_IQP_Options_i650uA                  12
#define rx_x4_control0_IQP_Options_i700uA                  13
#define rx_x4_control0_IQP_Options_i750uA                  14
#define rx_x4_control0_IQP_Options_i800uA                  15

/****************************************************************************
 * Enums: rx_x4_control0_IDriver_Options
 */
#define rx_x4_control0_IDriver_Options_v680mV              0
#define rx_x4_control0_IDriver_Options_v730mV              1
#define rx_x4_control0_IDriver_Options_v780mV              2
#define rx_x4_control0_IDriver_Options_v830mV              3
#define rx_x4_control0_IDriver_Options_v880mV              4
#define rx_x4_control0_IDriver_Options_v930mV              5
#define rx_x4_control0_IDriver_Options_v980mV              6
#define rx_x4_control0_IDriver_Options_v1010mV             7
#define rx_x4_control0_IDriver_Options_v1040mV             8
#define rx_x4_control0_IDriver_Options_v1060mV             9
#define rx_x4_control0_IDriver_Options_v1070mV             10
#define rx_x4_control0_IDriver_Options_v1080mV             11
#define rx_x4_control0_IDriver_Options_v1085mV             12
#define rx_x4_control0_IDriver_Options_v1090mV             13
#define rx_x4_control0_IDriver_Options_v1095mV             14
#define rx_x4_control0_IDriver_Options_v1100mV             15

/****************************************************************************
 * Enums: rx_x4_control0_operationModes
 */
#define rx_x4_control0_operationModes_XGXS                 0
#define rx_x4_control0_operationModes_XGXG_nCC             1
#define rx_x4_control0_operationModes_Indlane_OS8          4
#define rx_x4_control0_operationModes_IndLane_OS5          5
#define rx_x4_control0_operationModes_PCI                  7
#define rx_x4_control0_operationModes_XGXS_nLQ             8
#define rx_x4_control0_operationModes_XGXS_nLQnCC          9
#define rx_x4_control0_operationModes_PBypass              10
#define rx_x4_control0_operationModes_PBypass_nDSK         11
#define rx_x4_control0_operationModes_ComboCoreMode        12
#define rx_x4_control0_operationModes_Clocks_off           15

/****************************************************************************
 * Enums: rx_x4_control0_actualSpeeds
 */
#define rx_x4_control0_actualSpeeds_dr_10M                 0
#define rx_x4_control0_actualSpeeds_dr_100M                1
#define rx_x4_control0_actualSpeeds_dr_1G                  2
#define rx_x4_control0_actualSpeeds_dr_2p5G                3
#define rx_x4_control0_actualSpeeds_dr_5G_X4               4
#define rx_x4_control0_actualSpeeds_dr_6G_X4               5
#define rx_x4_control0_actualSpeeds_dr_10G_HiG             6
#define rx_x4_control0_actualSpeeds_dr_10G_CX4             7
#define rx_x4_control0_actualSpeeds_dr_12G_HiG             8
#define rx_x4_control0_actualSpeeds_dr_12p5G_X4            9
#define rx_x4_control0_actualSpeeds_dr_13G_X4              10
#define rx_x4_control0_actualSpeeds_dr_15G_X4              11
#define rx_x4_control0_actualSpeeds_dr_16G_X4              12
#define rx_x4_control0_actualSpeeds_dr_1G_KX               13
#define rx_x4_control0_actualSpeeds_dr_10G_KX4             14
#define rx_x4_control0_actualSpeeds_dr_10G_KR              15
#define rx_x4_control0_actualSpeeds_dr_5G                  16
#define rx_x4_control0_actualSpeeds_dr_6p4G                17
#define rx_x4_control0_actualSpeeds_dr_20G_X4              18
#define rx_x4_control0_actualSpeeds_dr_21G_X4              19
#define rx_x4_control0_actualSpeeds_dr_25G_X4              20
#define rx_x4_control0_actualSpeeds_dr_10G_HiG_DXGXS       21
#define rx_x4_control0_actualSpeeds_dr_10G_DXGXS           22
#define rx_x4_control0_actualSpeeds_dr_10p5G_HiG_DXGXS     23
#define rx_x4_control0_actualSpeeds_dr_10p5G_DXGXS         24
#define rx_x4_control0_actualSpeeds_dr_12p773G_HiG_DXGXS   25
#define rx_x4_control0_actualSpeeds_dr_12p773G_DXGXS       26
#define rx_x4_control0_actualSpeeds_dr_10G_XFI             27
#define rx_x4_control0_actualSpeeds_dr_40G                 28
#define rx_x4_control0_actualSpeeds_dr_20G_HiG_DXGXS       29
#define rx_x4_control0_actualSpeeds_dr_20G_DXGXS           30
#define rx_x4_control0_actualSpeeds_dr_10G_SFI             31
#define rx_x4_control0_actualSpeeds_dr_31p5G               32
#define rx_x4_control0_actualSpeeds_dr_32p7G               33
#define rx_x4_control0_actualSpeeds_dr_20G_SCR             34
#define rx_x4_control0_actualSpeeds_dr_10G_HiG_DXGXS_SCR   35
#define rx_x4_control0_actualSpeeds_dr_10G_DXGXS_SCR       36
#define rx_x4_control0_actualSpeeds_dr_12G_R2              37
#define rx_x4_control0_actualSpeeds_dr_10G_X2              38
#define rx_x4_control0_actualSpeeds_dr_40G_KR4             39
#define rx_x4_control0_actualSpeeds_dr_40G_CR4             40
#define rx_x4_control0_actualSpeeds_dr_100G_CR10           41
#define rx_x4_control0_actualSpeeds_dr_15p75G_DXGXS        44
#define rx_x4_control0_actualSpeeds_dr_20G_KR2             57
#define rx_x4_control0_actualSpeeds_dr_20G_CR2             58

/****************************************************************************
 * Enums: rx_x4_control0_actualSpeedsMisc1
 */
#define rx_x4_control0_actualSpeedsMisc1_dr_2500BRCM_X1    16
#define rx_x4_control0_actualSpeedsMisc1_dr_5000BRCM_X4    17
#define rx_x4_control0_actualSpeedsMisc1_dr_6000BRCM_X4    18
#define rx_x4_control0_actualSpeedsMisc1_dr_10GHiGig_X4    19
#define rx_x4_control0_actualSpeedsMisc1_dr_10GBASE_CX4    20
#define rx_x4_control0_actualSpeedsMisc1_dr_12GHiGig_X4    21
#define rx_x4_control0_actualSpeedsMisc1_dr_12p5GHiGig_X4  22
#define rx_x4_control0_actualSpeedsMisc1_dr_13GHiGig_X4    23
#define rx_x4_control0_actualSpeedsMisc1_dr_15GHiGig_X4    24
#define rx_x4_control0_actualSpeedsMisc1_dr_16GHiGig_X4    25
#define rx_x4_control0_actualSpeedsMisc1_dr_5000BRCM_X1    26
#define rx_x4_control0_actualSpeedsMisc1_dr_6363BRCM_X1    27
#define rx_x4_control0_actualSpeedsMisc1_dr_20GHiGig_X4    28
#define rx_x4_control0_actualSpeedsMisc1_dr_21GHiGig_X4    29
#define rx_x4_control0_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define rx_x4_control0_actualSpeedsMisc1_dr_10G_HiG_DXGXS  31

/****************************************************************************
 * Enums: rx_x4_control0_IndLaneModes
 */
#define rx_x4_control0_IndLaneModes_SWSDR_div2             0
#define rx_x4_control0_IndLaneModes_SWSDR_div1             1
#define rx_x4_control0_IndLaneModes_DWSDR_div2             2
#define rx_x4_control0_IndLaneModes_DWSDR_div1             3

/****************************************************************************
 * Enums: rx_x4_control0_prbsSelect
 */
#define rx_x4_control0_prbsSelect_prbs7                    0
#define rx_x4_control0_prbsSelect_prbs15                   1
#define rx_x4_control0_prbsSelect_prbs23                   2
#define rx_x4_control0_prbsSelect_prbs31                   3

/****************************************************************************
 * Enums: rx_x4_control0_vcoDivider
 */
#define rx_x4_control0_vcoDivider_div32                    0
#define rx_x4_control0_vcoDivider_div36                    1
#define rx_x4_control0_vcoDivider_div40                    2
#define rx_x4_control0_vcoDivider_div42                    3
#define rx_x4_control0_vcoDivider_div48                    4
#define rx_x4_control0_vcoDivider_div50                    5
#define rx_x4_control0_vcoDivider_div52                    6
#define rx_x4_control0_vcoDivider_div54                    7
#define rx_x4_control0_vcoDivider_div60                    8
#define rx_x4_control0_vcoDivider_div64                    9
#define rx_x4_control0_vcoDivider_div66                    10
#define rx_x4_control0_vcoDivider_div68                    11
#define rx_x4_control0_vcoDivider_div70                    12
#define rx_x4_control0_vcoDivider_div80                    13
#define rx_x4_control0_vcoDivider_div92                    14
#define rx_x4_control0_vcoDivider_div100                   15

/****************************************************************************
 * Enums: rx_x4_control0_refClkSelect
 */
#define rx_x4_control0_refClkSelect_clk_25MHz              0
#define rx_x4_control0_refClkSelect_clk_100MHz             1
#define rx_x4_control0_refClkSelect_clk_125MHz             2
#define rx_x4_control0_refClkSelect_clk_156p25MHz          3
#define rx_x4_control0_refClkSelect_clk_187p5MHz           4
#define rx_x4_control0_refClkSelect_clk_161p25Mhz          5
#define rx_x4_control0_refClkSelect_clk_50Mhz              6
#define rx_x4_control0_refClkSelect_clk_106p25Mhz          7

/****************************************************************************
 * Enums: rx_x4_control0_aerMMDdevTypeSelect
 */
#define rx_x4_control0_aerMMDdevTypeSelect_combo_core      0
#define rx_x4_control0_aerMMDdevTypeSelect_PMA_PMD         1
#define rx_x4_control0_aerMMDdevTypeSelect_PCS             3
#define rx_x4_control0_aerMMDdevTypeSelect_PHY             4
#define rx_x4_control0_aerMMDdevTypeSelect_DTE             5
#define rx_x4_control0_aerMMDdevTypeSelect_CL73_AN         7

/****************************************************************************
 * Enums: rx_x4_control0_aerMMDportSelect
 */
#define rx_x4_control0_aerMMDportSelect_ln0                0
#define rx_x4_control0_aerMMDportSelect_ln1                1
#define rx_x4_control0_aerMMDportSelect_ln2                2
#define rx_x4_control0_aerMMDportSelect_ln3                3
#define rx_x4_control0_aerMMDportSelect_BCST_ln0_1_2_3     511
#define rx_x4_control0_aerMMDportSelect_BCST_ln0_1         512
#define rx_x4_control0_aerMMDportSelect_BCST_ln2_3         513

/****************************************************************************
 * Enums: rx_x4_control0_firmwareModeSelect
 */
#define rx_x4_control0_firmwareModeSelect_DEFAULT          0
#define rx_x4_control0_firmwareModeSelect_SFP_OPT_LR       1
#define rx_x4_control0_firmwareModeSelect_SFP_DAC          2
#define rx_x4_control0_firmwareModeSelect_XLAUI            3
#define rx_x4_control0_firmwareModeSelect_LONG_CH_6G       4

/****************************************************************************
 * Enums: rx_x4_control0_tempIdxSelect
 */
#define rx_x4_control0_tempIdxSelect_LTE__22p9C            15
#define rx_x4_control0_tempIdxSelect_LTE__12p6C            14
#define rx_x4_control0_tempIdxSelect_LTE__3p0C             13
#define rx_x4_control0_tempIdxSelect_LTE_6p7C              12
#define rx_x4_control0_tempIdxSelect_LTE_16p4C             11
#define rx_x4_control0_tempIdxSelect_LTE_26p6C             10
#define rx_x4_control0_tempIdxSelect_LTE_36p3C             9
#define rx_x4_control0_tempIdxSelect_LTE_46p0C             8
#define rx_x4_control0_tempIdxSelect_LTE_56p2C             7
#define rx_x4_control0_tempIdxSelect_LTE_65p9C             6
#define rx_x4_control0_tempIdxSelect_LTE_75p6C             5
#define rx_x4_control0_tempIdxSelect_LTE_85p3C             4
#define rx_x4_control0_tempIdxSelect_LTE_95p5C             3
#define rx_x4_control0_tempIdxSelect_LTE_105p2C            2
#define rx_x4_control0_tempIdxSelect_LTE_114p9C            1
#define rx_x4_control0_tempIdxSelect_LTE_125p1C            0

/****************************************************************************
 * Enums: rx_x4_control0_port_mode
 */
#define rx_x4_control0_port_mode_QUAD_PORT                 0
#define rx_x4_control0_port_mode_TRI_1_PORT                1
#define rx_x4_control0_port_mode_TRI_2_PORT                2
#define rx_x4_control0_port_mode_DUAL_PORT                 3
#define rx_x4_control0_port_mode_SINGLE_PORT               4

/****************************************************************************
 * Enums: rx_x4_control0_rev_letter_enum
 */
#define rx_x4_control0_rev_letter_enum_REV_A               0
#define rx_x4_control0_rev_letter_enum_REV_B               1
#define rx_x4_control0_rev_letter_enum_REV_C               2
#define rx_x4_control0_rev_letter_enum_REV_D               3

/****************************************************************************
 * Enums: rx_x4_control0_rev_number_enum
 */
#define rx_x4_control0_rev_number_enum_REV_0               0
#define rx_x4_control0_rev_number_enum_REV_1               1
#define rx_x4_control0_rev_number_enum_REV_2               2
#define rx_x4_control0_rev_number_enum_REV_3               3
#define rx_x4_control0_rev_number_enum_REV_4               4
#define rx_x4_control0_rev_number_enum_REV_5               5
#define rx_x4_control0_rev_number_enum_REV_6               6
#define rx_x4_control0_rev_number_enum_REV_7               7

/****************************************************************************
 * Enums: rx_x4_control0_bonding_enum
 */
#define rx_x4_control0_bonding_enum_WIRE_BOND              0
#define rx_x4_control0_bonding_enum_FLIP_CHIP              1

/****************************************************************************
 * Enums: rx_x4_control0_tech_process
 */
#define rx_x4_control0_tech_process_PROCESS_90NM           0
#define rx_x4_control0_tech_process_PROCESS_65NM           1
#define rx_x4_control0_tech_process_PROCESS_40NM           2
#define rx_x4_control0_tech_process_PROCESS_28NM           3

/****************************************************************************
 * Enums: rx_x4_control0_model_num
 */
#define rx_x4_control0_model_num_SERDES_CL73               0
#define rx_x4_control0_model_num_XGXS_16G                  1
#define rx_x4_control0_model_num_HYPERCORE                 2
#define rx_x4_control0_model_num_HYPERLITE                 3
#define rx_x4_control0_model_num_PCIE_G2_PIPE              4
#define rx_x4_control0_model_num_SERDES_1p25GBd            5
#define rx_x4_control0_model_num_SATA2                     6
#define rx_x4_control0_model_num_QSGMII                    7
#define rx_x4_control0_model_num_XGXS10G                   8
#define rx_x4_control0_model_num_WARPCORE                  9
#define rx_x4_control0_model_num_XFICORE                   10
#define rx_x4_control0_model_num_RXFI                      11
#define rx_x4_control0_model_num_WARPLITE                  12
#define rx_x4_control0_model_num_PENTACORE                 13
#define rx_x4_control0_model_num_ESM                       14
#define rx_x4_control0_model_num_QUAD_SGMII                15
#define rx_x4_control0_model_num_WARPCORE_3                16
#define rx_x4_control0_model_num_TSC                       17
#define rx_x4_control0_model_num_TSC4E                     18
#define rx_x4_control0_model_num_TSC12E                    19
#define rx_x4_control0_model_num_TSC4F                     20
#define rx_x4_control0_model_num_XGXS_CL73_90NM            29
#define rx_x4_control0_model_num_SERDES_CL73_90NM          30
#define rx_x4_control0_model_num_WARPCORE3                 32
#define rx_x4_control0_model_num_WARPCORE4_TSC             33
#define rx_x4_control0_model_num_RXAUI                     34

/****************************************************************************
 * Enums: rx_x4_control0_payload
 */
#define rx_x4_control0_payload_REPEAT_2_BYTES              0
#define rx_x4_control0_payload_RAMPING                     1
#define rx_x4_control0_payload_CL48_CRPAT                  2
#define rx_x4_control0_payload_CL48_CJPAT                  3
#define rx_x4_control0_payload_CL36_LONG_CRPAT             4
#define rx_x4_control0_payload_CL36_SHORT_CRPAT            5

/****************************************************************************
 * Enums: rx_x4_control0_sc
 */
#define rx_x4_control0_sc_S_10G_CR1                        0
#define rx_x4_control0_sc_S_10G_KR1                        1
#define rx_x4_control0_sc_S_10G_X1                         2
#define rx_x4_control0_sc_S_10G_HG2_CR1                    4
#define rx_x4_control0_sc_S_10G_HG2_KR1                    5
#define rx_x4_control0_sc_S_10G_HG2_X1                     6
#define rx_x4_control0_sc_S_20G_CR1                        8
#define rx_x4_control0_sc_S_20G_KR1                        9
#define rx_x4_control0_sc_S_20G_X1                         10
#define rx_x4_control0_sc_S_20G_HG2_CR1                    12
#define rx_x4_control0_sc_S_20G_HG2_KR1                    13
#define rx_x4_control0_sc_S_20G_HG2_X1                     14
#define rx_x4_control0_sc_S_25G_CR1                        16
#define rx_x4_control0_sc_S_25G_KR1                        17
#define rx_x4_control0_sc_S_25G_X1                         18
#define rx_x4_control0_sc_S_25G_HG2_CR1                    20
#define rx_x4_control0_sc_S_25G_HG2_KR1                    21
#define rx_x4_control0_sc_S_25G_HG2_X1                     22
#define rx_x4_control0_sc_S_20G_CR2                        24
#define rx_x4_control0_sc_S_20G_KR2                        25
#define rx_x4_control0_sc_S_20G_X2                         26
#define rx_x4_control0_sc_S_20G_HG2_CR2                    28
#define rx_x4_control0_sc_S_20G_HG2_KR2                    29
#define rx_x4_control0_sc_S_20G_HG2_X2                     30
#define rx_x4_control0_sc_S_40G_CR2                        32
#define rx_x4_control0_sc_S_40G_KR2                        33
#define rx_x4_control0_sc_S_40G_X2                         34
#define rx_x4_control0_sc_S_40G_HG2_CR2                    36
#define rx_x4_control0_sc_S_40G_HG2_KR2                    37
#define rx_x4_control0_sc_S_40G_HG2_X2                     38
#define rx_x4_control0_sc_S_40G_CR4                        40
#define rx_x4_control0_sc_S_40G_KR4                        41
#define rx_x4_control0_sc_S_40G_X4                         42
#define rx_x4_control0_sc_S_40G_HG2_CR4                    44
#define rx_x4_control0_sc_S_40G_HG2_KR4                    45
#define rx_x4_control0_sc_S_40G_HG2_X4                     46
#define rx_x4_control0_sc_S_50G_CR2                        48
#define rx_x4_control0_sc_S_50G_KR2                        49
#define rx_x4_control0_sc_S_50G_X2                         50
#define rx_x4_control0_sc_S_50G_HG2_CR2                    52
#define rx_x4_control0_sc_S_50G_HG2_KR2                    53
#define rx_x4_control0_sc_S_50G_HG2_X2                     54
#define rx_x4_control0_sc_S_50G_CR4                        56
#define rx_x4_control0_sc_S_50G_KR4                        57
#define rx_x4_control0_sc_S_50G_X4                         58
#define rx_x4_control0_sc_S_50G_HG2_CR4                    60
#define rx_x4_control0_sc_S_50G_HG2_KR4                    61
#define rx_x4_control0_sc_S_50G_HG2_X4                     62
#define rx_x4_control0_sc_S_100G_CR4                       64
#define rx_x4_control0_sc_S_100G_KR4                       65
#define rx_x4_control0_sc_S_100G_X4                        66
#define rx_x4_control0_sc_S_100G_HG2_CR4                   68
#define rx_x4_control0_sc_S_100G_HG2_KR4                   69
#define rx_x4_control0_sc_S_100G_HG2_X4                    70
#define rx_x4_control0_sc_S_CL73_20GVCO                    72
#define rx_x4_control0_sc_S_CL73_25GVCO                    80
#define rx_x4_control0_sc_S_CL36_20GVCO                    88
#define rx_x4_control0_sc_S_CL36_25GVCO                    96

/****************************************************************************
 * Enums: rx_x4_control0_t_fifo_modes
 */
#define rx_x4_control0_t_fifo_modes_T_FIFO_MODE_BYPASS     0
#define rx_x4_control0_t_fifo_modes_T_FIFO_MODE_40G        1
#define rx_x4_control0_t_fifo_modes_T_FIFO_MODE_100G       2
#define rx_x4_control0_t_fifo_modes_T_FIFO_MODE_20G        3

/****************************************************************************
 * Enums: rx_x4_control0_t_enc_modes
 */
#define rx_x4_control0_t_enc_modes_T_ENC_MODE_BYPASS       0
#define rx_x4_control0_t_enc_modes_T_ENC_MODE_CL49         1
#define rx_x4_control0_t_enc_modes_T_ENC_MODE_CL82         2

/****************************************************************************
 * Enums: rx_x4_control0_btmx_mode
 */
#define rx_x4_control0_btmx_mode_BS_BTMX_MODE_1to1         0
#define rx_x4_control0_btmx_mode_BS_BTMX_MODE_2to1         1
#define rx_x4_control0_btmx_mode_BS_BTMX_MODE_5to1         2

/****************************************************************************
 * Enums: rx_x4_control0_t_type_cl82
 */
#define rx_x4_control0_t_type_cl82_T_TYPE_B1               5
#define rx_x4_control0_t_type_cl82_T_TYPE_C                4
#define rx_x4_control0_t_type_cl82_T_TYPE_S                3
#define rx_x4_control0_t_type_cl82_T_TYPE_T                2
#define rx_x4_control0_t_type_cl82_T_TYPE_D                1
#define rx_x4_control0_t_type_cl82_T_TYPE_E                0

/****************************************************************************
 * Enums: rx_x4_control0_txsm_state_cl82
 */
#define rx_x4_control0_txsm_state_cl82_TX_HIG_END          6
#define rx_x4_control0_txsm_state_cl82_TX_HIG_START        5
#define rx_x4_control0_txsm_state_cl82_TX_E                4
#define rx_x4_control0_txsm_state_cl82_TX_T                3
#define rx_x4_control0_txsm_state_cl82_TX_D                2
#define rx_x4_control0_txsm_state_cl82_TX_C                1
#define rx_x4_control0_txsm_state_cl82_TX_INIT             0

/****************************************************************************
 * Enums: rx_x4_control0_ltxsm_state_cl82
 */
#define rx_x4_control0_ltxsm_state_cl82_TX_HIG_END         64
#define rx_x4_control0_ltxsm_state_cl82_TX_HIG_START       32
#define rx_x4_control0_ltxsm_state_cl82_TX_E               16
#define rx_x4_control0_ltxsm_state_cl82_TX_T               8
#define rx_x4_control0_ltxsm_state_cl82_TX_D               4
#define rx_x4_control0_ltxsm_state_cl82_TX_C               2
#define rx_x4_control0_ltxsm_state_cl82_TX_INIT            1

/****************************************************************************
 * Enums: rx_x4_control0_r_type_coded_cl82
 */
#define rx_x4_control0_r_type_coded_cl82_R_TYPE_B1         32
#define rx_x4_control0_r_type_coded_cl82_R_TYPE_C          16
#define rx_x4_control0_r_type_coded_cl82_R_TYPE_S          8
#define rx_x4_control0_r_type_coded_cl82_R_TYPE_T          4
#define rx_x4_control0_r_type_coded_cl82_R_TYPE_D          2
#define rx_x4_control0_r_type_coded_cl82_R_TYPE_E          1

/****************************************************************************
 * Enums: rx_x4_control0_rxsm_state_cl82
 */
#define rx_x4_control0_rxsm_state_cl82_RX_HIG_END          64
#define rx_x4_control0_rxsm_state_cl82_RX_HIG_START        32
#define rx_x4_control0_rxsm_state_cl82_RX_E                16
#define rx_x4_control0_rxsm_state_cl82_RX_T                8
#define rx_x4_control0_rxsm_state_cl82_RX_D                4
#define rx_x4_control0_rxsm_state_cl82_RX_C                2
#define rx_x4_control0_rxsm_state_cl82_RX_INIT             1

/****************************************************************************
 * Enums: rx_x4_control0_deskew_state
 */
#define rx_x4_control0_deskew_state_ALIGN_ACQUIRED         2
#define rx_x4_control0_deskew_state_LOSS_OF_ALIGNMENT      1

/****************************************************************************
 * Enums: rx_x4_control0_os_mode_enum
 */
#define rx_x4_control0_os_mode_enum_OS_MODE_1              0
#define rx_x4_control0_os_mode_enum_OS_MODE_2              1
#define rx_x4_control0_os_mode_enum_OS_MODE_4              2
#define rx_x4_control0_os_mode_enum_OS_MODE_16p5           8
#define rx_x4_control0_os_mode_enum_OS_MODE_20p625         12

/****************************************************************************
 * Enums: rx_x4_control0_scr_modes
 */
#define rx_x4_control0_scr_modes_T_SCR_MODE_BYPASS         0
#define rx_x4_control0_scr_modes_T_SCR_MODE_CL49           1
#define rx_x4_control0_scr_modes_T_SCR_MODE_40G_2_LANE     2
#define rx_x4_control0_scr_modes_T_SCR_MODE_100G           3
#define rx_x4_control0_scr_modes_T_SCR_MODE_20G            4
#define rx_x4_control0_scr_modes_T_SCR_MODE_40G_4_LANE     5

/****************************************************************************
 * Enums: rx_x4_control0_descr_modes
 */
#define rx_x4_control0_descr_modes_R_DESCR_MODE_BYPASS     0
#define rx_x4_control0_descr_modes_R_DESCR_MODE_CL49       1
#define rx_x4_control0_descr_modes_R_DESCR_MODE_CL82       2

/****************************************************************************
 * Enums: rx_x4_control0_r_dec_tl_mode
 */
#define rx_x4_control0_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS  0
#define rx_x4_control0_r_dec_tl_mode_R_DEC_TL_MODE_CL49    1
#define rx_x4_control0_r_dec_tl_mode_R_DEC_TL_MODE_CL82    2

/****************************************************************************
 * Enums: rx_x4_control0_r_dec_fsm_mode
 */
#define rx_x4_control0_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define rx_x4_control0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49  1
#define rx_x4_control0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82  2

/****************************************************************************
 * Enums: rx_x4_control0_r_deskew_mode
 */
#define rx_x4_control0_r_deskew_mode_R_DESKEW_MODE_BYPASS  0
#define rx_x4_control0_r_deskew_mode_R_DESKEW_MODE_20G     1
#define rx_x4_control0_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define rx_x4_control0_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define rx_x4_control0_r_deskew_mode_R_DESKEW_MODE_100G    4
#define rx_x4_control0_r_deskew_mode_R_DESKEW_MODE_CL49    5
#define rx_x4_control0_r_deskew_mode_R_DESKEW_MODE_CL91    6

/****************************************************************************
 * Enums: rx_x4_control0_bs_dist_modes
 */
#define rx_x4_control0_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define rx_x4_control0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define rx_x4_control0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define rx_x4_control0_bs_dist_modes_BS_DIST_MODE_NO_TDM   3

/****************************************************************************
 * Enums: rx_x4_control0_cl49_t_type
 */
#define rx_x4_control0_cl49_t_type_BAD_T_TYPE              15
#define rx_x4_control0_cl49_t_type_T_TYPE_B0               11
#define rx_x4_control0_cl49_t_type_T_TYPE_OB               10
#define rx_x4_control0_cl49_t_type_T_TYPE_B1               9
#define rx_x4_control0_cl49_t_type_T_TYPE_DB               8
#define rx_x4_control0_cl49_t_type_T_TYPE_FC               7
#define rx_x4_control0_cl49_t_type_T_TYPE_TB               6
#define rx_x4_control0_cl49_t_type_T_TYPE_LI               5
#define rx_x4_control0_cl49_t_type_T_TYPE_C                4
#define rx_x4_control0_cl49_t_type_T_TYPE_S                3
#define rx_x4_control0_cl49_t_type_T_TYPE_T                2
#define rx_x4_control0_cl49_t_type_T_TYPE_D                1
#define rx_x4_control0_cl49_t_type_T_TYPE_E                0

/****************************************************************************
 * Enums: rx_x4_control0_cl49_txsm_states
 */
#define rx_x4_control0_cl49_txsm_states_TX_HIG_END         7
#define rx_x4_control0_cl49_txsm_states_TX_HIG_START       6
#define rx_x4_control0_cl49_txsm_states_TX_LI              5
#define rx_x4_control0_cl49_txsm_states_TX_E               4
#define rx_x4_control0_cl49_txsm_states_TX_T               3
#define rx_x4_control0_cl49_txsm_states_TX_D               2
#define rx_x4_control0_cl49_txsm_states_TX_C               1
#define rx_x4_control0_cl49_txsm_states_TX_INIT            0

/****************************************************************************
 * Enums: rx_x4_control0_cl49_ltxsm_states
 */
#define rx_x4_control0_cl49_ltxsm_states_TX_HIG_END        128
#define rx_x4_control0_cl49_ltxsm_states_TX_HIG_START      64
#define rx_x4_control0_cl49_ltxsm_states_TX_LI             32
#define rx_x4_control0_cl49_ltxsm_states_TX_E              16
#define rx_x4_control0_cl49_ltxsm_states_TX_T              8
#define rx_x4_control0_cl49_ltxsm_states_TX_D              4
#define rx_x4_control0_cl49_ltxsm_states_TX_C              2
#define rx_x4_control0_cl49_ltxsm_states_TX_INIT           1

/****************************************************************************
 * Enums: rx_x4_control0_burst_error_mode
 */
#define rx_x4_control0_burst_error_mode_BURST_ERROR_11_BITS 0
#define rx_x4_control0_burst_error_mode_BURST_ERROR_16_BITS 1
#define rx_x4_control0_burst_error_mode_BURST_ERROR_17_BITS 2
#define rx_x4_control0_burst_error_mode_BURST_ERROR_18_BITS 3
#define rx_x4_control0_burst_error_mode_BURST_ERROR_19_BITS 4
#define rx_x4_control0_burst_error_mode_BURST_ERROR_20_BITS 5
#define rx_x4_control0_burst_error_mode_BURST_ERROR_21_BITS 6
#define rx_x4_control0_burst_error_mode_BURST_ERROR_22_BITS 7
#define rx_x4_control0_burst_error_mode_BURST_ERROR_23_BITS 8
#define rx_x4_control0_burst_error_mode_BURST_ERROR_24_BITS 9
#define rx_x4_control0_burst_error_mode_BURST_ERROR_25_BITS 10
#define rx_x4_control0_burst_error_mode_BURST_ERROR_26_BITS 11
#define rx_x4_control0_burst_error_mode_BURST_ERROR_27_BITS 12
#define rx_x4_control0_burst_error_mode_BURST_ERROR_28_BITS 13
#define rx_x4_control0_burst_error_mode_BURST_ERROR_29_BITS 14
#define rx_x4_control0_burst_error_mode_BURST_ERROR_30_BITS 15
#define rx_x4_control0_burst_error_mode_BURST_ERROR_31_BITS 16
#define rx_x4_control0_burst_error_mode_BURST_ERROR_32_BITS 17
#define rx_x4_control0_burst_error_mode_BURST_ERROR_33_BITS 18
#define rx_x4_control0_burst_error_mode_BURST_ERROR_34_BITS 19
#define rx_x4_control0_burst_error_mode_BURST_ERROR_35_BITS 20
#define rx_x4_control0_burst_error_mode_BURST_ERROR_36_BITS 21
#define rx_x4_control0_burst_error_mode_BURST_ERROR_37_BITS 22
#define rx_x4_control0_burst_error_mode_BURST_ERROR_38_BITS 23
#define rx_x4_control0_burst_error_mode_BURST_ERROR_39_BITS 24
#define rx_x4_control0_burst_error_mode_BURST_ERROR_40_BITS 25
#define rx_x4_control0_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: rx_x4_control0_bermon_state
 */
#define rx_x4_control0_bermon_state_HI_BER                 4
#define rx_x4_control0_bermon_state_GOOD_BER               3
#define rx_x4_control0_bermon_state_BER_TEST_SH            2
#define rx_x4_control0_bermon_state_START_TIMER            1
#define rx_x4_control0_bermon_state_BER_MT_INIT            0

/****************************************************************************
 * Enums: rx_x4_control0_rxsm_state_cl49
 */
#define rx_x4_control0_rxsm_state_cl49_RX_HIG_END          128
#define rx_x4_control0_rxsm_state_cl49_RX_HIG_START        64
#define rx_x4_control0_rxsm_state_cl49_RX_LI               32
#define rx_x4_control0_rxsm_state_cl49_RX_E                16
#define rx_x4_control0_rxsm_state_cl49_RX_T                8
#define rx_x4_control0_rxsm_state_cl49_RX_D                4
#define rx_x4_control0_rxsm_state_cl49_RX_C                2
#define rx_x4_control0_rxsm_state_cl49_RX_INIT             1

/****************************************************************************
 * Enums: rx_x4_control0_r_type
 */
#define rx_x4_control0_r_type_BAD_R_TYPE                   15
#define rx_x4_control0_r_type_R_TYPE_B0                    11
#define rx_x4_control0_r_type_R_TYPE_OB                    10
#define rx_x4_control0_r_type_R_TYPE_B1                    9
#define rx_x4_control0_r_type_R_TYPE_DB                    8
#define rx_x4_control0_r_type_R_TYPE_FC                    7
#define rx_x4_control0_r_type_R_TYPE_TB                    6
#define rx_x4_control0_r_type_R_TYPE_LI                    5
#define rx_x4_control0_r_type_R_TYPE_C                     4
#define rx_x4_control0_r_type_R_TYPE_S                     3
#define rx_x4_control0_r_type_R_TYPE_T                     2
#define rx_x4_control0_r_type_R_TYPE_D                     1
#define rx_x4_control0_r_type_R_TYPE_E                     0

/****************************************************************************
 * Enums: rx_x4_control0_am_lock_state
 */
#define rx_x4_control0_am_lock_state_INVALID_AM            512
#define rx_x4_control0_am_lock_state_GOOD_AM               256
#define rx_x4_control0_am_lock_state_COMP_AM               128
#define rx_x4_control0_am_lock_state_TIMER_2               64
#define rx_x4_control0_am_lock_state_AM_2_GOOD             32
#define rx_x4_control0_am_lock_state_COMP_2ND              16
#define rx_x4_control0_am_lock_state_TIMER_1               8
#define rx_x4_control0_am_lock_state_FIND_1ST              4
#define rx_x4_control0_am_lock_state_AM_RESET_CNT          2
#define rx_x4_control0_am_lock_state_AM_LOCK_INIT          1

/****************************************************************************
 * Enums: rx_x4_control0_msg_selector
 */
#define rx_x4_control0_msg_selector_RESERVED               0
#define rx_x4_control0_msg_selector_VALUE_802p3            1
#define rx_x4_control0_msg_selector_VALUE_802p9            2
#define rx_x4_control0_msg_selector_VALUE_802p5            3
#define rx_x4_control0_msg_selector_VALUE_1394             4

/****************************************************************************
 * Enums: rx_x4_control0_synce_enum
 */
#define rx_x4_control0_synce_enum_SYNCE_NO_DIV             0
#define rx_x4_control0_synce_enum_SYNCE_DIV_7              1
#define rx_x4_control0_synce_enum_SYNCE_DIV_11             2

/****************************************************************************
 * Enums: rx_x4_control0_synce_enum_stage0
 */
#define rx_x4_control0_synce_enum_stage0_SYNCE_NO_DIV      0
#define rx_x4_control0_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define rx_x4_control0_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: rx_x4_control0_cl91_sync_state
 */
#define rx_x4_control0_cl91_sync_state_FIND_1ST            0
#define rx_x4_control0_cl91_sync_state_COUNT_NEXT          1
#define rx_x4_control0_cl91_sync_state_COMP_2ND            2
#define rx_x4_control0_cl91_sync_state_TWO_GOOD            3

/****************************************************************************
 * Enums: rx_x4_control0_cl91_algn_state
 */
#define rx_x4_control0_cl91_algn_state_LOSS_OF_ALIGNMENT   0
#define rx_x4_control0_cl91_algn_state_DESKEW              1
#define rx_x4_control0_cl91_algn_state_DESKEW_FAIL         2
#define rx_x4_control0_cl91_algn_state_ALIGN_ACQUIRED      3
#define rx_x4_control0_cl91_algn_state_CW_GOOD             4
#define rx_x4_control0_cl91_algn_state_CW_BAD              5
#define rx_x4_control0_cl91_algn_state_THREE_BAD           6

/****************************************************************************
 * Enums: rx_x4_control0_fec_sel
 */
#define rx_x4_control0_fec_sel_NO_FEC                      0
#define rx_x4_control0_fec_sel_FEC_CL74                    1
#define rx_x4_control0_fec_sel_FEC_CL91                    2

/****************************************************************************
 * Enums: rx_x4_fec_control_cl36TxEEEStates_l
 */
#define rx_x4_fec_control_cl36TxEEEStates_l_TX_REFRESH     8
#define rx_x4_fec_control_cl36TxEEEStates_l_TX_QUIET       4
#define rx_x4_fec_control_cl36TxEEEStates_l_TX_SLEEP       2
#define rx_x4_fec_control_cl36TxEEEStates_l_TX_ACTIVE      1

/****************************************************************************
 * Enums: rx_x4_fec_control_cl36TxEEEStates_c
 */
#define rx_x4_fec_control_cl36TxEEEStates_c_TX_REFRESH     3
#define rx_x4_fec_control_cl36TxEEEStates_c_TX_QUIET       2
#define rx_x4_fec_control_cl36TxEEEStates_c_TX_SLEEP       1
#define rx_x4_fec_control_cl36TxEEEStates_c_TX_ACTIVE      0

/****************************************************************************
 * Enums: rx_x4_fec_control_cl49TxEEEStates_l
 */
#define rx_x4_fec_control_cl49TxEEEStates_l_SCR_RESET_2    64
#define rx_x4_fec_control_cl49TxEEEStates_l_SCR_RESET_1    32
#define rx_x4_fec_control_cl49TxEEEStates_l_TX_WAKE        16
#define rx_x4_fec_control_cl49TxEEEStates_l_TX_REFRESH     8
#define rx_x4_fec_control_cl49TxEEEStates_l_TX_QUIET       4
#define rx_x4_fec_control_cl49TxEEEStates_l_TX_SLEEP       2
#define rx_x4_fec_control_cl49TxEEEStates_l_TX_ACTIVE      1

/****************************************************************************
 * Enums: rx_x4_fec_control_fec_req_enum
 */
#define rx_x4_fec_control_fec_req_enum_FEC_not_supported   0
#define rx_x4_fec_control_fec_req_enum_FEC_supported_but_not_requested 1
#define rx_x4_fec_control_fec_req_enum_invalid_setting     2
#define rx_x4_fec_control_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: rx_x4_fec_control_cl73_pause_enum
 */
#define rx_x4_fec_control_cl73_pause_enum_No_PAUSE_ability 0
#define rx_x4_fec_control_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define rx_x4_fec_control_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define rx_x4_fec_control_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: rx_x4_fec_control_cl49TxEEEStates_c
 */
#define rx_x4_fec_control_cl49TxEEEStates_c_SCR_RESET_2    6
#define rx_x4_fec_control_cl49TxEEEStates_c_SCR_RESET_1    5
#define rx_x4_fec_control_cl49TxEEEStates_c_TX_WAKE        4
#define rx_x4_fec_control_cl49TxEEEStates_c_TX_REFRESH     3
#define rx_x4_fec_control_cl49TxEEEStates_c_TX_QUIET       2
#define rx_x4_fec_control_cl49TxEEEStates_c_TX_SLEEP       1
#define rx_x4_fec_control_cl49TxEEEStates_c_TX_ACTIVE      0

/****************************************************************************
 * Enums: rx_x4_fec_control_cl36RxEEEStates_l
 */
#define rx_x4_fec_control_cl36RxEEEStates_l_RX_LINK_FAIL   32
#define rx_x4_fec_control_cl36RxEEEStates_l_RX_WTF         16
#define rx_x4_fec_control_cl36RxEEEStates_l_RX_WAKE        8
#define rx_x4_fec_control_cl36RxEEEStates_l_RX_QUIET       4
#define rx_x4_fec_control_cl36RxEEEStates_l_RX_SLEEP       2
#define rx_x4_fec_control_cl36RxEEEStates_l_RX_ACTIVE      1

/****************************************************************************
 * Enums: rx_x4_fec_control_cl36RxEEEStates_c
 */
#define rx_x4_fec_control_cl36RxEEEStates_c_RX_LINK_FAIL   5
#define rx_x4_fec_control_cl36RxEEEStates_c_RX_WTF         4
#define rx_x4_fec_control_cl36RxEEEStates_c_RX_WAKE        3
#define rx_x4_fec_control_cl36RxEEEStates_c_RX_QUIET       2
#define rx_x4_fec_control_cl36RxEEEStates_c_RX_SLEEP       1
#define rx_x4_fec_control_cl36RxEEEStates_c_RX_ACTIVE      0

/****************************************************************************
 * Enums: rx_x4_fec_control_cl49RxEEEStates_l
 */
#define rx_x4_fec_control_cl49RxEEEStates_l_RX_LINK_FAIL   32
#define rx_x4_fec_control_cl49RxEEEStates_l_RX_WTF         16
#define rx_x4_fec_control_cl49RxEEEStates_l_RX_WAKE        8
#define rx_x4_fec_control_cl49RxEEEStates_l_RX_QUIET       4
#define rx_x4_fec_control_cl49RxEEEStates_l_RX_SLEEP       2
#define rx_x4_fec_control_cl49RxEEEStates_l_RX_ACTIVE      1

/****************************************************************************
 * Enums: rx_x4_fec_control_cl49RxEEEStates_c
 */
#define rx_x4_fec_control_cl49RxEEEStates_c_RX_LINK_FAIL   5
#define rx_x4_fec_control_cl49RxEEEStates_c_RX_WTF         4
#define rx_x4_fec_control_cl49RxEEEStates_c_RX_WAKE        3
#define rx_x4_fec_control_cl49RxEEEStates_c_RX_QUIET       2
#define rx_x4_fec_control_cl49RxEEEStates_c_RX_SLEEP       1
#define rx_x4_fec_control_cl49RxEEEStates_c_RX_ACTIVE      0

/****************************************************************************
 * Enums: rx_x4_fec_control_cl48TxEEEStates_l
 */
#define rx_x4_fec_control_cl48TxEEEStates_l_TX_REFRESH     8
#define rx_x4_fec_control_cl48TxEEEStates_l_TX_QUIET       4
#define rx_x4_fec_control_cl48TxEEEStates_l_TX_SLEEP       2
#define rx_x4_fec_control_cl48TxEEEStates_l_TX_ACTIVE      1

/****************************************************************************
 * Enums: rx_x4_fec_control_cl48TxEEEStates_c
 */
#define rx_x4_fec_control_cl48TxEEEStates_c_TX_REFRESH     3
#define rx_x4_fec_control_cl48TxEEEStates_c_TX_QUIET       2
#define rx_x4_fec_control_cl48TxEEEStates_c_TX_SLEEP       1
#define rx_x4_fec_control_cl48TxEEEStates_c_TX_ACTIVE      0

/****************************************************************************
 * Enums: rx_x4_fec_control_cl48RxEEEStates_l
 */
#define rx_x4_fec_control_cl48RxEEEStates_l_RX_LINK_FAIL   32
#define rx_x4_fec_control_cl48RxEEEStates_l_RX_WAKE        16
#define rx_x4_fec_control_cl48RxEEEStates_l_RX_QUIET       8
#define rx_x4_fec_control_cl48RxEEEStates_l_RX_DEACT       4
#define rx_x4_fec_control_cl48RxEEEStates_l_RX_SLEEP       2
#define rx_x4_fec_control_cl48RxEEEStates_l_RX_ACTIVE      1

/****************************************************************************
 * Enums: rx_x4_fec_control_cl48RxEEEStates_c
 */
#define rx_x4_fec_control_cl48RxEEEStates_c_RX_LINK_FAIL   5
#define rx_x4_fec_control_cl48RxEEEStates_c_RX_WAKE        4
#define rx_x4_fec_control_cl48RxEEEStates_c_RX_QUIET       3
#define rx_x4_fec_control_cl48RxEEEStates_c_RX_DEACT       2
#define rx_x4_fec_control_cl48RxEEEStates_c_RX_SLEEP       1
#define rx_x4_fec_control_cl48RxEEEStates_c_RX_ACTIVE      0

/****************************************************************************
 * Enums: rx_x4_fec_control_IQP_Options
 */
#define rx_x4_fec_control_IQP_Options_i50uA                0
#define rx_x4_fec_control_IQP_Options_i100uA               1
#define rx_x4_fec_control_IQP_Options_i150uA               2
#define rx_x4_fec_control_IQP_Options_i200uA               3
#define rx_x4_fec_control_IQP_Options_i250uA               4
#define rx_x4_fec_control_IQP_Options_i300uA               5
#define rx_x4_fec_control_IQP_Options_i350uA               6
#define rx_x4_fec_control_IQP_Options_i400uA               7
#define rx_x4_fec_control_IQP_Options_i450uA               8
#define rx_x4_fec_control_IQP_Options_i500uA               9
#define rx_x4_fec_control_IQP_Options_i550uA               10
#define rx_x4_fec_control_IQP_Options_i600uA               11
#define rx_x4_fec_control_IQP_Options_i650uA               12
#define rx_x4_fec_control_IQP_Options_i700uA               13
#define rx_x4_fec_control_IQP_Options_i750uA               14
#define rx_x4_fec_control_IQP_Options_i800uA               15

/****************************************************************************
 * Enums: rx_x4_fec_control_IDriver_Options
 */
#define rx_x4_fec_control_IDriver_Options_v680mV           0
#define rx_x4_fec_control_IDriver_Options_v730mV           1
#define rx_x4_fec_control_IDriver_Options_v780mV           2
#define rx_x4_fec_control_IDriver_Options_v830mV           3
#define rx_x4_fec_control_IDriver_Options_v880mV           4
#define rx_x4_fec_control_IDriver_Options_v930mV           5
#define rx_x4_fec_control_IDriver_Options_v980mV           6
#define rx_x4_fec_control_IDriver_Options_v1010mV          7
#define rx_x4_fec_control_IDriver_Options_v1040mV          8
#define rx_x4_fec_control_IDriver_Options_v1060mV          9
#define rx_x4_fec_control_IDriver_Options_v1070mV          10
#define rx_x4_fec_control_IDriver_Options_v1080mV          11
#define rx_x4_fec_control_IDriver_Options_v1085mV          12
#define rx_x4_fec_control_IDriver_Options_v1090mV          13
#define rx_x4_fec_control_IDriver_Options_v1095mV          14
#define rx_x4_fec_control_IDriver_Options_v1100mV          15

/****************************************************************************
 * Enums: rx_x4_fec_control_operationModes
 */
#define rx_x4_fec_control_operationModes_XGXS              0
#define rx_x4_fec_control_operationModes_XGXG_nCC          1
#define rx_x4_fec_control_operationModes_Indlane_OS8       4
#define rx_x4_fec_control_operationModes_IndLane_OS5       5
#define rx_x4_fec_control_operationModes_PCI               7
#define rx_x4_fec_control_operationModes_XGXS_nLQ          8
#define rx_x4_fec_control_operationModes_XGXS_nLQnCC       9
#define rx_x4_fec_control_operationModes_PBypass           10
#define rx_x4_fec_control_operationModes_PBypass_nDSK      11
#define rx_x4_fec_control_operationModes_ComboCoreMode     12
#define rx_x4_fec_control_operationModes_Clocks_off        15

/****************************************************************************
 * Enums: rx_x4_fec_control_actualSpeeds
 */
#define rx_x4_fec_control_actualSpeeds_dr_10M              0
#define rx_x4_fec_control_actualSpeeds_dr_100M             1
#define rx_x4_fec_control_actualSpeeds_dr_1G               2
#define rx_x4_fec_control_actualSpeeds_dr_2p5G             3
#define rx_x4_fec_control_actualSpeeds_dr_5G_X4            4
#define rx_x4_fec_control_actualSpeeds_dr_6G_X4            5
#define rx_x4_fec_control_actualSpeeds_dr_10G_HiG          6
#define rx_x4_fec_control_actualSpeeds_dr_10G_CX4          7
#define rx_x4_fec_control_actualSpeeds_dr_12G_HiG          8
#define rx_x4_fec_control_actualSpeeds_dr_12p5G_X4         9
#define rx_x4_fec_control_actualSpeeds_dr_13G_X4           10
#define rx_x4_fec_control_actualSpeeds_dr_15G_X4           11
#define rx_x4_fec_control_actualSpeeds_dr_16G_X4           12
#define rx_x4_fec_control_actualSpeeds_dr_1G_KX            13
#define rx_x4_fec_control_actualSpeeds_dr_10G_KX4          14
#define rx_x4_fec_control_actualSpeeds_dr_10G_KR           15
#define rx_x4_fec_control_actualSpeeds_dr_5G               16
#define rx_x4_fec_control_actualSpeeds_dr_6p4G             17
#define rx_x4_fec_control_actualSpeeds_dr_20G_X4           18
#define rx_x4_fec_control_actualSpeeds_dr_21G_X4           19
#define rx_x4_fec_control_actualSpeeds_dr_25G_X4           20
#define rx_x4_fec_control_actualSpeeds_dr_10G_HiG_DXGXS    21
#define rx_x4_fec_control_actualSpeeds_dr_10G_DXGXS        22
#define rx_x4_fec_control_actualSpeeds_dr_10p5G_HiG_DXGXS  23
#define rx_x4_fec_control_actualSpeeds_dr_10p5G_DXGXS      24
#define rx_x4_fec_control_actualSpeeds_dr_12p773G_HiG_DXGXS 25
#define rx_x4_fec_control_actualSpeeds_dr_12p773G_DXGXS    26
#define rx_x4_fec_control_actualSpeeds_dr_10G_XFI          27
#define rx_x4_fec_control_actualSpeeds_dr_40G              28
#define rx_x4_fec_control_actualSpeeds_dr_20G_HiG_DXGXS    29
#define rx_x4_fec_control_actualSpeeds_dr_20G_DXGXS        30
#define rx_x4_fec_control_actualSpeeds_dr_10G_SFI          31
#define rx_x4_fec_control_actualSpeeds_dr_31p5G            32
#define rx_x4_fec_control_actualSpeeds_dr_32p7G            33
#define rx_x4_fec_control_actualSpeeds_dr_20G_SCR          34
#define rx_x4_fec_control_actualSpeeds_dr_10G_HiG_DXGXS_SCR 35
#define rx_x4_fec_control_actualSpeeds_dr_10G_DXGXS_SCR    36
#define rx_x4_fec_control_actualSpeeds_dr_12G_R2           37
#define rx_x4_fec_control_actualSpeeds_dr_10G_X2           38
#define rx_x4_fec_control_actualSpeeds_dr_40G_KR4          39
#define rx_x4_fec_control_actualSpeeds_dr_40G_CR4          40
#define rx_x4_fec_control_actualSpeeds_dr_100G_CR10        41
#define rx_x4_fec_control_actualSpeeds_dr_15p75G_DXGXS     44
#define rx_x4_fec_control_actualSpeeds_dr_20G_KR2          57
#define rx_x4_fec_control_actualSpeeds_dr_20G_CR2          58

/****************************************************************************
 * Enums: rx_x4_fec_control_actualSpeedsMisc1
 */
#define rx_x4_fec_control_actualSpeedsMisc1_dr_2500BRCM_X1 16
#define rx_x4_fec_control_actualSpeedsMisc1_dr_5000BRCM_X4 17
#define rx_x4_fec_control_actualSpeedsMisc1_dr_6000BRCM_X4 18
#define rx_x4_fec_control_actualSpeedsMisc1_dr_10GHiGig_X4 19
#define rx_x4_fec_control_actualSpeedsMisc1_dr_10GBASE_CX4 20
#define rx_x4_fec_control_actualSpeedsMisc1_dr_12GHiGig_X4 21
#define rx_x4_fec_control_actualSpeedsMisc1_dr_12p5GHiGig_X4 22
#define rx_x4_fec_control_actualSpeedsMisc1_dr_13GHiGig_X4 23
#define rx_x4_fec_control_actualSpeedsMisc1_dr_15GHiGig_X4 24
#define rx_x4_fec_control_actualSpeedsMisc1_dr_16GHiGig_X4 25
#define rx_x4_fec_control_actualSpeedsMisc1_dr_5000BRCM_X1 26
#define rx_x4_fec_control_actualSpeedsMisc1_dr_6363BRCM_X1 27
#define rx_x4_fec_control_actualSpeedsMisc1_dr_20GHiGig_X4 28
#define rx_x4_fec_control_actualSpeedsMisc1_dr_21GHiGig_X4 29
#define rx_x4_fec_control_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define rx_x4_fec_control_actualSpeedsMisc1_dr_10G_HiG_DXGXS 31

/****************************************************************************
 * Enums: rx_x4_fec_control_IndLaneModes
 */
#define rx_x4_fec_control_IndLaneModes_SWSDR_div2          0
#define rx_x4_fec_control_IndLaneModes_SWSDR_div1          1
#define rx_x4_fec_control_IndLaneModes_DWSDR_div2          2
#define rx_x4_fec_control_IndLaneModes_DWSDR_div1          3

/****************************************************************************
 * Enums: rx_x4_fec_control_prbsSelect
 */
#define rx_x4_fec_control_prbsSelect_prbs7                 0
#define rx_x4_fec_control_prbsSelect_prbs15                1
#define rx_x4_fec_control_prbsSelect_prbs23                2
#define rx_x4_fec_control_prbsSelect_prbs31                3

/****************************************************************************
 * Enums: rx_x4_fec_control_vcoDivider
 */
#define rx_x4_fec_control_vcoDivider_div32                 0
#define rx_x4_fec_control_vcoDivider_div36                 1
#define rx_x4_fec_control_vcoDivider_div40                 2
#define rx_x4_fec_control_vcoDivider_div42                 3
#define rx_x4_fec_control_vcoDivider_div48                 4
#define rx_x4_fec_control_vcoDivider_div50                 5
#define rx_x4_fec_control_vcoDivider_div52                 6
#define rx_x4_fec_control_vcoDivider_div54                 7
#define rx_x4_fec_control_vcoDivider_div60                 8
#define rx_x4_fec_control_vcoDivider_div64                 9
#define rx_x4_fec_control_vcoDivider_div66                 10
#define rx_x4_fec_control_vcoDivider_div68                 11
#define rx_x4_fec_control_vcoDivider_div70                 12
#define rx_x4_fec_control_vcoDivider_div80                 13
#define rx_x4_fec_control_vcoDivider_div92                 14
#define rx_x4_fec_control_vcoDivider_div100                15

/****************************************************************************
 * Enums: rx_x4_fec_control_refClkSelect
 */
#define rx_x4_fec_control_refClkSelect_clk_25MHz           0
#define rx_x4_fec_control_refClkSelect_clk_100MHz          1
#define rx_x4_fec_control_refClkSelect_clk_125MHz          2
#define rx_x4_fec_control_refClkSelect_clk_156p25MHz       3
#define rx_x4_fec_control_refClkSelect_clk_187p5MHz        4
#define rx_x4_fec_control_refClkSelect_clk_161p25Mhz       5
#define rx_x4_fec_control_refClkSelect_clk_50Mhz           6
#define rx_x4_fec_control_refClkSelect_clk_106p25Mhz       7

/****************************************************************************
 * Enums: rx_x4_fec_control_aerMMDdevTypeSelect
 */
#define rx_x4_fec_control_aerMMDdevTypeSelect_combo_core   0
#define rx_x4_fec_control_aerMMDdevTypeSelect_PMA_PMD      1
#define rx_x4_fec_control_aerMMDdevTypeSelect_PCS          3
#define rx_x4_fec_control_aerMMDdevTypeSelect_PHY          4
#define rx_x4_fec_control_aerMMDdevTypeSelect_DTE          5
#define rx_x4_fec_control_aerMMDdevTypeSelect_CL73_AN      7

/****************************************************************************
 * Enums: rx_x4_fec_control_aerMMDportSelect
 */
#define rx_x4_fec_control_aerMMDportSelect_ln0             0
#define rx_x4_fec_control_aerMMDportSelect_ln1             1
#define rx_x4_fec_control_aerMMDportSelect_ln2             2
#define rx_x4_fec_control_aerMMDportSelect_ln3             3
#define rx_x4_fec_control_aerMMDportSelect_BCST_ln0_1_2_3  511
#define rx_x4_fec_control_aerMMDportSelect_BCST_ln0_1      512
#define rx_x4_fec_control_aerMMDportSelect_BCST_ln2_3      513

/****************************************************************************
 * Enums: rx_x4_fec_control_firmwareModeSelect
 */
#define rx_x4_fec_control_firmwareModeSelect_DEFAULT       0
#define rx_x4_fec_control_firmwareModeSelect_SFP_OPT_LR    1
#define rx_x4_fec_control_firmwareModeSelect_SFP_DAC       2
#define rx_x4_fec_control_firmwareModeSelect_XLAUI         3
#define rx_x4_fec_control_firmwareModeSelect_LONG_CH_6G    4

/****************************************************************************
 * Enums: rx_x4_fec_control_tempIdxSelect
 */
#define rx_x4_fec_control_tempIdxSelect_LTE__22p9C         15
#define rx_x4_fec_control_tempIdxSelect_LTE__12p6C         14
#define rx_x4_fec_control_tempIdxSelect_LTE__3p0C          13
#define rx_x4_fec_control_tempIdxSelect_LTE_6p7C           12
#define rx_x4_fec_control_tempIdxSelect_LTE_16p4C          11
#define rx_x4_fec_control_tempIdxSelect_LTE_26p6C          10
#define rx_x4_fec_control_tempIdxSelect_LTE_36p3C          9
#define rx_x4_fec_control_tempIdxSelect_LTE_46p0C          8
#define rx_x4_fec_control_tempIdxSelect_LTE_56p2C          7
#define rx_x4_fec_control_tempIdxSelect_LTE_65p9C          6
#define rx_x4_fec_control_tempIdxSelect_LTE_75p6C          5
#define rx_x4_fec_control_tempIdxSelect_LTE_85p3C          4
#define rx_x4_fec_control_tempIdxSelect_LTE_95p5C          3
#define rx_x4_fec_control_tempIdxSelect_LTE_105p2C         2
#define rx_x4_fec_control_tempIdxSelect_LTE_114p9C         1
#define rx_x4_fec_control_tempIdxSelect_LTE_125p1C         0

/****************************************************************************
 * Enums: rx_x4_fec_control_port_mode
 */
#define rx_x4_fec_control_port_mode_QUAD_PORT              0
#define rx_x4_fec_control_port_mode_TRI_1_PORT             1
#define rx_x4_fec_control_port_mode_TRI_2_PORT             2
#define rx_x4_fec_control_port_mode_DUAL_PORT              3
#define rx_x4_fec_control_port_mode_SINGLE_PORT            4

/****************************************************************************
 * Enums: rx_x4_fec_control_rev_letter_enum
 */
#define rx_x4_fec_control_rev_letter_enum_REV_A            0
#define rx_x4_fec_control_rev_letter_enum_REV_B            1
#define rx_x4_fec_control_rev_letter_enum_REV_C            2
#define rx_x4_fec_control_rev_letter_enum_REV_D            3

/****************************************************************************
 * Enums: rx_x4_fec_control_rev_number_enum
 */
#define rx_x4_fec_control_rev_number_enum_REV_0            0
#define rx_x4_fec_control_rev_number_enum_REV_1            1
#define rx_x4_fec_control_rev_number_enum_REV_2            2
#define rx_x4_fec_control_rev_number_enum_REV_3            3
#define rx_x4_fec_control_rev_number_enum_REV_4            4
#define rx_x4_fec_control_rev_number_enum_REV_5            5
#define rx_x4_fec_control_rev_number_enum_REV_6            6
#define rx_x4_fec_control_rev_number_enum_REV_7            7

/****************************************************************************
 * Enums: rx_x4_fec_control_bonding_enum
 */
#define rx_x4_fec_control_bonding_enum_WIRE_BOND           0
#define rx_x4_fec_control_bonding_enum_FLIP_CHIP           1

/****************************************************************************
 * Enums: rx_x4_fec_control_tech_process
 */
#define rx_x4_fec_control_tech_process_PROCESS_90NM        0
#define rx_x4_fec_control_tech_process_PROCESS_65NM        1
#define rx_x4_fec_control_tech_process_PROCESS_40NM        2
#define rx_x4_fec_control_tech_process_PROCESS_28NM        3

/****************************************************************************
 * Enums: rx_x4_fec_control_model_num
 */
#define rx_x4_fec_control_model_num_SERDES_CL73            0
#define rx_x4_fec_control_model_num_XGXS_16G               1
#define rx_x4_fec_control_model_num_HYPERCORE              2
#define rx_x4_fec_control_model_num_HYPERLITE              3
#define rx_x4_fec_control_model_num_PCIE_G2_PIPE           4
#define rx_x4_fec_control_model_num_SERDES_1p25GBd         5
#define rx_x4_fec_control_model_num_SATA2                  6
#define rx_x4_fec_control_model_num_QSGMII                 7
#define rx_x4_fec_control_model_num_XGXS10G                8
#define rx_x4_fec_control_model_num_WARPCORE               9
#define rx_x4_fec_control_model_num_XFICORE                10
#define rx_x4_fec_control_model_num_RXFI                   11
#define rx_x4_fec_control_model_num_WARPLITE               12
#define rx_x4_fec_control_model_num_PENTACORE              13
#define rx_x4_fec_control_model_num_ESM                    14
#define rx_x4_fec_control_model_num_QUAD_SGMII             15
#define rx_x4_fec_control_model_num_WARPCORE_3             16
#define rx_x4_fec_control_model_num_TSC                    17
#define rx_x4_fec_control_model_num_TSC4E                  18
#define rx_x4_fec_control_model_num_TSC12E                 19
#define rx_x4_fec_control_model_num_TSC4F                  20
#define rx_x4_fec_control_model_num_XGXS_CL73_90NM         29
#define rx_x4_fec_control_model_num_SERDES_CL73_90NM       30
#define rx_x4_fec_control_model_num_WARPCORE3              32
#define rx_x4_fec_control_model_num_WARPCORE4_TSC          33
#define rx_x4_fec_control_model_num_RXAUI                  34

/****************************************************************************
 * Enums: rx_x4_fec_control_payload
 */
#define rx_x4_fec_control_payload_REPEAT_2_BYTES           0
#define rx_x4_fec_control_payload_RAMPING                  1
#define rx_x4_fec_control_payload_CL48_CRPAT               2
#define rx_x4_fec_control_payload_CL48_CJPAT               3
#define rx_x4_fec_control_payload_CL36_LONG_CRPAT          4
#define rx_x4_fec_control_payload_CL36_SHORT_CRPAT         5

/****************************************************************************
 * Enums: rx_x4_fec_control_sc
 */
#define rx_x4_fec_control_sc_S_10G_CR1                     0
#define rx_x4_fec_control_sc_S_10G_KR1                     1
#define rx_x4_fec_control_sc_S_10G_X1                      2
#define rx_x4_fec_control_sc_S_10G_HG2_CR1                 4
#define rx_x4_fec_control_sc_S_10G_HG2_KR1                 5
#define rx_x4_fec_control_sc_S_10G_HG2_X1                  6
#define rx_x4_fec_control_sc_S_20G_CR1                     8
#define rx_x4_fec_control_sc_S_20G_KR1                     9
#define rx_x4_fec_control_sc_S_20G_X1                      10
#define rx_x4_fec_control_sc_S_20G_HG2_CR1                 12
#define rx_x4_fec_control_sc_S_20G_HG2_KR1                 13
#define rx_x4_fec_control_sc_S_20G_HG2_X1                  14
#define rx_x4_fec_control_sc_S_25G_CR1                     16
#define rx_x4_fec_control_sc_S_25G_KR1                     17
#define rx_x4_fec_control_sc_S_25G_X1                      18
#define rx_x4_fec_control_sc_S_25G_HG2_CR1                 20
#define rx_x4_fec_control_sc_S_25G_HG2_KR1                 21
#define rx_x4_fec_control_sc_S_25G_HG2_X1                  22
#define rx_x4_fec_control_sc_S_20G_CR2                     24
#define rx_x4_fec_control_sc_S_20G_KR2                     25
#define rx_x4_fec_control_sc_S_20G_X2                      26
#define rx_x4_fec_control_sc_S_20G_HG2_CR2                 28
#define rx_x4_fec_control_sc_S_20G_HG2_KR2                 29
#define rx_x4_fec_control_sc_S_20G_HG2_X2                  30
#define rx_x4_fec_control_sc_S_40G_CR2                     32
#define rx_x4_fec_control_sc_S_40G_KR2                     33
#define rx_x4_fec_control_sc_S_40G_X2                      34
#define rx_x4_fec_control_sc_S_40G_HG2_CR2                 36
#define rx_x4_fec_control_sc_S_40G_HG2_KR2                 37
#define rx_x4_fec_control_sc_S_40G_HG2_X2                  38
#define rx_x4_fec_control_sc_S_40G_CR4                     40
#define rx_x4_fec_control_sc_S_40G_KR4                     41
#define rx_x4_fec_control_sc_S_40G_X4                      42
#define rx_x4_fec_control_sc_S_40G_HG2_CR4                 44
#define rx_x4_fec_control_sc_S_40G_HG2_KR4                 45
#define rx_x4_fec_control_sc_S_40G_HG2_X4                  46
#define rx_x4_fec_control_sc_S_50G_CR2                     48
#define rx_x4_fec_control_sc_S_50G_KR2                     49
#define rx_x4_fec_control_sc_S_50G_X2                      50
#define rx_x4_fec_control_sc_S_50G_HG2_CR2                 52
#define rx_x4_fec_control_sc_S_50G_HG2_KR2                 53
#define rx_x4_fec_control_sc_S_50G_HG2_X2                  54
#define rx_x4_fec_control_sc_S_50G_CR4                     56
#define rx_x4_fec_control_sc_S_50G_KR4                     57
#define rx_x4_fec_control_sc_S_50G_X4                      58
#define rx_x4_fec_control_sc_S_50G_HG2_CR4                 60
#define rx_x4_fec_control_sc_S_50G_HG2_KR4                 61
#define rx_x4_fec_control_sc_S_50G_HG2_X4                  62
#define rx_x4_fec_control_sc_S_100G_CR4                    64
#define rx_x4_fec_control_sc_S_100G_KR4                    65
#define rx_x4_fec_control_sc_S_100G_X4                     66
#define rx_x4_fec_control_sc_S_100G_HG2_CR4                68
#define rx_x4_fec_control_sc_S_100G_HG2_KR4                69
#define rx_x4_fec_control_sc_S_100G_HG2_X4                 70
#define rx_x4_fec_control_sc_S_CL73_20GVCO                 72
#define rx_x4_fec_control_sc_S_CL73_25GVCO                 80
#define rx_x4_fec_control_sc_S_CL36_20GVCO                 88
#define rx_x4_fec_control_sc_S_CL36_25GVCO                 96

/****************************************************************************
 * Enums: rx_x4_fec_control_t_fifo_modes
 */
#define rx_x4_fec_control_t_fifo_modes_T_FIFO_MODE_BYPASS  0
#define rx_x4_fec_control_t_fifo_modes_T_FIFO_MODE_40G     1
#define rx_x4_fec_control_t_fifo_modes_T_FIFO_MODE_100G    2
#define rx_x4_fec_control_t_fifo_modes_T_FIFO_MODE_20G     3

/****************************************************************************
 * Enums: rx_x4_fec_control_t_enc_modes
 */
#define rx_x4_fec_control_t_enc_modes_T_ENC_MODE_BYPASS    0
#define rx_x4_fec_control_t_enc_modes_T_ENC_MODE_CL49      1
#define rx_x4_fec_control_t_enc_modes_T_ENC_MODE_CL82      2

/****************************************************************************
 * Enums: rx_x4_fec_control_btmx_mode
 */
#define rx_x4_fec_control_btmx_mode_BS_BTMX_MODE_1to1      0
#define rx_x4_fec_control_btmx_mode_BS_BTMX_MODE_2to1      1
#define rx_x4_fec_control_btmx_mode_BS_BTMX_MODE_5to1      2

/****************************************************************************
 * Enums: rx_x4_fec_control_t_type_cl82
 */
#define rx_x4_fec_control_t_type_cl82_T_TYPE_B1            5
#define rx_x4_fec_control_t_type_cl82_T_TYPE_C             4
#define rx_x4_fec_control_t_type_cl82_T_TYPE_S             3
#define rx_x4_fec_control_t_type_cl82_T_TYPE_T             2
#define rx_x4_fec_control_t_type_cl82_T_TYPE_D             1
#define rx_x4_fec_control_t_type_cl82_T_TYPE_E             0

/****************************************************************************
 * Enums: rx_x4_fec_control_txsm_state_cl82
 */
#define rx_x4_fec_control_txsm_state_cl82_TX_HIG_END       6
#define rx_x4_fec_control_txsm_state_cl82_TX_HIG_START     5
#define rx_x4_fec_control_txsm_state_cl82_TX_E             4
#define rx_x4_fec_control_txsm_state_cl82_TX_T             3
#define rx_x4_fec_control_txsm_state_cl82_TX_D             2
#define rx_x4_fec_control_txsm_state_cl82_TX_C             1
#define rx_x4_fec_control_txsm_state_cl82_TX_INIT          0

/****************************************************************************
 * Enums: rx_x4_fec_control_ltxsm_state_cl82
 */
#define rx_x4_fec_control_ltxsm_state_cl82_TX_HIG_END      64
#define rx_x4_fec_control_ltxsm_state_cl82_TX_HIG_START    32
#define rx_x4_fec_control_ltxsm_state_cl82_TX_E            16
#define rx_x4_fec_control_ltxsm_state_cl82_TX_T            8
#define rx_x4_fec_control_ltxsm_state_cl82_TX_D            4
#define rx_x4_fec_control_ltxsm_state_cl82_TX_C            2
#define rx_x4_fec_control_ltxsm_state_cl82_TX_INIT         1

/****************************************************************************
 * Enums: rx_x4_fec_control_r_type_coded_cl82
 */
#define rx_x4_fec_control_r_type_coded_cl82_R_TYPE_B1      32
#define rx_x4_fec_control_r_type_coded_cl82_R_TYPE_C       16
#define rx_x4_fec_control_r_type_coded_cl82_R_TYPE_S       8
#define rx_x4_fec_control_r_type_coded_cl82_R_TYPE_T       4
#define rx_x4_fec_control_r_type_coded_cl82_R_TYPE_D       2
#define rx_x4_fec_control_r_type_coded_cl82_R_TYPE_E       1

/****************************************************************************
 * Enums: rx_x4_fec_control_rxsm_state_cl82
 */
#define rx_x4_fec_control_rxsm_state_cl82_RX_HIG_END       64
#define rx_x4_fec_control_rxsm_state_cl82_RX_HIG_START     32
#define rx_x4_fec_control_rxsm_state_cl82_RX_E             16
#define rx_x4_fec_control_rxsm_state_cl82_RX_T             8
#define rx_x4_fec_control_rxsm_state_cl82_RX_D             4
#define rx_x4_fec_control_rxsm_state_cl82_RX_C             2
#define rx_x4_fec_control_rxsm_state_cl82_RX_INIT          1

/****************************************************************************
 * Enums: rx_x4_fec_control_deskew_state
 */
#define rx_x4_fec_control_deskew_state_ALIGN_ACQUIRED      2
#define rx_x4_fec_control_deskew_state_LOSS_OF_ALIGNMENT   1

/****************************************************************************
 * Enums: rx_x4_fec_control_os_mode_enum
 */
#define rx_x4_fec_control_os_mode_enum_OS_MODE_1           0
#define rx_x4_fec_control_os_mode_enum_OS_MODE_2           1
#define rx_x4_fec_control_os_mode_enum_OS_MODE_4           2
#define rx_x4_fec_control_os_mode_enum_OS_MODE_16p5        8
#define rx_x4_fec_control_os_mode_enum_OS_MODE_20p625      12

/****************************************************************************
 * Enums: rx_x4_fec_control_scr_modes
 */
#define rx_x4_fec_control_scr_modes_T_SCR_MODE_BYPASS      0
#define rx_x4_fec_control_scr_modes_T_SCR_MODE_CL49        1
#define rx_x4_fec_control_scr_modes_T_SCR_MODE_40G_2_LANE  2
#define rx_x4_fec_control_scr_modes_T_SCR_MODE_100G        3
#define rx_x4_fec_control_scr_modes_T_SCR_MODE_20G         4
#define rx_x4_fec_control_scr_modes_T_SCR_MODE_40G_4_LANE  5

/****************************************************************************
 * Enums: rx_x4_fec_control_descr_modes
 */
#define rx_x4_fec_control_descr_modes_R_DESCR_MODE_BYPASS  0
#define rx_x4_fec_control_descr_modes_R_DESCR_MODE_CL49    1
#define rx_x4_fec_control_descr_modes_R_DESCR_MODE_CL82    2

/****************************************************************************
 * Enums: rx_x4_fec_control_r_dec_tl_mode
 */
#define rx_x4_fec_control_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS 0
#define rx_x4_fec_control_r_dec_tl_mode_R_DEC_TL_MODE_CL49 1
#define rx_x4_fec_control_r_dec_tl_mode_R_DEC_TL_MODE_CL82 2

/****************************************************************************
 * Enums: rx_x4_fec_control_r_dec_fsm_mode
 */
#define rx_x4_fec_control_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define rx_x4_fec_control_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49 1
#define rx_x4_fec_control_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82 2

/****************************************************************************
 * Enums: rx_x4_fec_control_r_deskew_mode
 */
#define rx_x4_fec_control_r_deskew_mode_R_DESKEW_MODE_BYPASS 0
#define rx_x4_fec_control_r_deskew_mode_R_DESKEW_MODE_20G  1
#define rx_x4_fec_control_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define rx_x4_fec_control_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define rx_x4_fec_control_r_deskew_mode_R_DESKEW_MODE_100G 4
#define rx_x4_fec_control_r_deskew_mode_R_DESKEW_MODE_CL49 5
#define rx_x4_fec_control_r_deskew_mode_R_DESKEW_MODE_CL91 6

/****************************************************************************
 * Enums: rx_x4_fec_control_bs_dist_modes
 */
#define rx_x4_fec_control_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define rx_x4_fec_control_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define rx_x4_fec_control_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define rx_x4_fec_control_bs_dist_modes_BS_DIST_MODE_NO_TDM 3

/****************************************************************************
 * Enums: rx_x4_fec_control_cl49_t_type
 */
#define rx_x4_fec_control_cl49_t_type_BAD_T_TYPE           15
#define rx_x4_fec_control_cl49_t_type_T_TYPE_B0            11
#define rx_x4_fec_control_cl49_t_type_T_TYPE_OB            10
#define rx_x4_fec_control_cl49_t_type_T_TYPE_B1            9
#define rx_x4_fec_control_cl49_t_type_T_TYPE_DB            8
#define rx_x4_fec_control_cl49_t_type_T_TYPE_FC            7
#define rx_x4_fec_control_cl49_t_type_T_TYPE_TB            6
#define rx_x4_fec_control_cl49_t_type_T_TYPE_LI            5
#define rx_x4_fec_control_cl49_t_type_T_TYPE_C             4
#define rx_x4_fec_control_cl49_t_type_T_TYPE_S             3
#define rx_x4_fec_control_cl49_t_type_T_TYPE_T             2
#define rx_x4_fec_control_cl49_t_type_T_TYPE_D             1
#define rx_x4_fec_control_cl49_t_type_T_TYPE_E             0

/****************************************************************************
 * Enums: rx_x4_fec_control_cl49_txsm_states
 */
#define rx_x4_fec_control_cl49_txsm_states_TX_HIG_END      7
#define rx_x4_fec_control_cl49_txsm_states_TX_HIG_START    6
#define rx_x4_fec_control_cl49_txsm_states_TX_LI           5
#define rx_x4_fec_control_cl49_txsm_states_TX_E            4
#define rx_x4_fec_control_cl49_txsm_states_TX_T            3
#define rx_x4_fec_control_cl49_txsm_states_TX_D            2
#define rx_x4_fec_control_cl49_txsm_states_TX_C            1
#define rx_x4_fec_control_cl49_txsm_states_TX_INIT         0

/****************************************************************************
 * Enums: rx_x4_fec_control_cl49_ltxsm_states
 */
#define rx_x4_fec_control_cl49_ltxsm_states_TX_HIG_END     128
#define rx_x4_fec_control_cl49_ltxsm_states_TX_HIG_START   64
#define rx_x4_fec_control_cl49_ltxsm_states_TX_LI          32
#define rx_x4_fec_control_cl49_ltxsm_states_TX_E           16
#define rx_x4_fec_control_cl49_ltxsm_states_TX_T           8
#define rx_x4_fec_control_cl49_ltxsm_states_TX_D           4
#define rx_x4_fec_control_cl49_ltxsm_states_TX_C           2
#define rx_x4_fec_control_cl49_ltxsm_states_TX_INIT        1

/****************************************************************************
 * Enums: rx_x4_fec_control_burst_error_mode
 */
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_11_BITS 0
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_16_BITS 1
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_17_BITS 2
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_18_BITS 3
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_19_BITS 4
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_20_BITS 5
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_21_BITS 6
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_22_BITS 7
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_23_BITS 8
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_24_BITS 9
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_25_BITS 10
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_26_BITS 11
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_27_BITS 12
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_28_BITS 13
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_29_BITS 14
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_30_BITS 15
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_31_BITS 16
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_32_BITS 17
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_33_BITS 18
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_34_BITS 19
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_35_BITS 20
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_36_BITS 21
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_37_BITS 22
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_38_BITS 23
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_39_BITS 24
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_40_BITS 25
#define rx_x4_fec_control_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: rx_x4_fec_control_bermon_state
 */
#define rx_x4_fec_control_bermon_state_HI_BER              4
#define rx_x4_fec_control_bermon_state_GOOD_BER            3
#define rx_x4_fec_control_bermon_state_BER_TEST_SH         2
#define rx_x4_fec_control_bermon_state_START_TIMER         1
#define rx_x4_fec_control_bermon_state_BER_MT_INIT         0

/****************************************************************************
 * Enums: rx_x4_fec_control_rxsm_state_cl49
 */
#define rx_x4_fec_control_rxsm_state_cl49_RX_HIG_END       128
#define rx_x4_fec_control_rxsm_state_cl49_RX_HIG_START     64
#define rx_x4_fec_control_rxsm_state_cl49_RX_LI            32
#define rx_x4_fec_control_rxsm_state_cl49_RX_E             16
#define rx_x4_fec_control_rxsm_state_cl49_RX_T             8
#define rx_x4_fec_control_rxsm_state_cl49_RX_D             4
#define rx_x4_fec_control_rxsm_state_cl49_RX_C             2
#define rx_x4_fec_control_rxsm_state_cl49_RX_INIT          1

/****************************************************************************
 * Enums: rx_x4_fec_control_r_type
 */
#define rx_x4_fec_control_r_type_BAD_R_TYPE                15
#define rx_x4_fec_control_r_type_R_TYPE_B0                 11
#define rx_x4_fec_control_r_type_R_TYPE_OB                 10
#define rx_x4_fec_control_r_type_R_TYPE_B1                 9
#define rx_x4_fec_control_r_type_R_TYPE_DB                 8
#define rx_x4_fec_control_r_type_R_TYPE_FC                 7
#define rx_x4_fec_control_r_type_R_TYPE_TB                 6
#define rx_x4_fec_control_r_type_R_TYPE_LI                 5
#define rx_x4_fec_control_r_type_R_TYPE_C                  4
#define rx_x4_fec_control_r_type_R_TYPE_S                  3
#define rx_x4_fec_control_r_type_R_TYPE_T                  2
#define rx_x4_fec_control_r_type_R_TYPE_D                  1
#define rx_x4_fec_control_r_type_R_TYPE_E                  0

/****************************************************************************
 * Enums: rx_x4_fec_control_am_lock_state
 */
#define rx_x4_fec_control_am_lock_state_INVALID_AM         512
#define rx_x4_fec_control_am_lock_state_GOOD_AM            256
#define rx_x4_fec_control_am_lock_state_COMP_AM            128
#define rx_x4_fec_control_am_lock_state_TIMER_2            64
#define rx_x4_fec_control_am_lock_state_AM_2_GOOD          32
#define rx_x4_fec_control_am_lock_state_COMP_2ND           16
#define rx_x4_fec_control_am_lock_state_TIMER_1            8
#define rx_x4_fec_control_am_lock_state_FIND_1ST           4
#define rx_x4_fec_control_am_lock_state_AM_RESET_CNT       2
#define rx_x4_fec_control_am_lock_state_AM_LOCK_INIT       1

/****************************************************************************
 * Enums: rx_x4_fec_control_msg_selector
 */
#define rx_x4_fec_control_msg_selector_RESERVED            0
#define rx_x4_fec_control_msg_selector_VALUE_802p3         1
#define rx_x4_fec_control_msg_selector_VALUE_802p9         2
#define rx_x4_fec_control_msg_selector_VALUE_802p5         3
#define rx_x4_fec_control_msg_selector_VALUE_1394          4

/****************************************************************************
 * Enums: rx_x4_fec_control_synce_enum
 */
#define rx_x4_fec_control_synce_enum_SYNCE_NO_DIV          0
#define rx_x4_fec_control_synce_enum_SYNCE_DIV_7           1
#define rx_x4_fec_control_synce_enum_SYNCE_DIV_11          2

/****************************************************************************
 * Enums: rx_x4_fec_control_synce_enum_stage0
 */
#define rx_x4_fec_control_synce_enum_stage0_SYNCE_NO_DIV   0
#define rx_x4_fec_control_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define rx_x4_fec_control_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: rx_x4_fec_control_cl91_sync_state
 */
#define rx_x4_fec_control_cl91_sync_state_FIND_1ST         0
#define rx_x4_fec_control_cl91_sync_state_COUNT_NEXT       1
#define rx_x4_fec_control_cl91_sync_state_COMP_2ND         2
#define rx_x4_fec_control_cl91_sync_state_TWO_GOOD         3

/****************************************************************************
 * Enums: rx_x4_fec_control_cl91_algn_state
 */
#define rx_x4_fec_control_cl91_algn_state_LOSS_OF_ALIGNMENT 0
#define rx_x4_fec_control_cl91_algn_state_DESKEW           1
#define rx_x4_fec_control_cl91_algn_state_DESKEW_FAIL      2
#define rx_x4_fec_control_cl91_algn_state_ALIGN_ACQUIRED   3
#define rx_x4_fec_control_cl91_algn_state_CW_GOOD          4
#define rx_x4_fec_control_cl91_algn_state_CW_BAD           5
#define rx_x4_fec_control_cl91_algn_state_THREE_BAD        6

/****************************************************************************
 * Enums: rx_x4_fec_control_fec_sel
 */
#define rx_x4_fec_control_fec_sel_NO_FEC                   0
#define rx_x4_fec_control_fec_sel_FEC_CL74                 1
#define rx_x4_fec_control_fec_sel_FEC_CL91                 2

/****************************************************************************
 * Enums: rx_x4_status1_cl36TxEEEStates_l
 */
#define rx_x4_status1_cl36TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status1_cl36TxEEEStates_l_TX_QUIET           4
#define rx_x4_status1_cl36TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status1_cl36TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status1_cl36TxEEEStates_c
 */
#define rx_x4_status1_cl36TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status1_cl36TxEEEStates_c_TX_QUIET           2
#define rx_x4_status1_cl36TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status1_cl36TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status1_cl49TxEEEStates_l
 */
#define rx_x4_status1_cl49TxEEEStates_l_SCR_RESET_2        64
#define rx_x4_status1_cl49TxEEEStates_l_SCR_RESET_1        32
#define rx_x4_status1_cl49TxEEEStates_l_TX_WAKE            16
#define rx_x4_status1_cl49TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status1_cl49TxEEEStates_l_TX_QUIET           4
#define rx_x4_status1_cl49TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status1_cl49TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status1_fec_req_enum
 */
#define rx_x4_status1_fec_req_enum_FEC_not_supported       0
#define rx_x4_status1_fec_req_enum_FEC_supported_but_not_requested 1
#define rx_x4_status1_fec_req_enum_invalid_setting         2
#define rx_x4_status1_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: rx_x4_status1_cl73_pause_enum
 */
#define rx_x4_status1_cl73_pause_enum_No_PAUSE_ability     0
#define rx_x4_status1_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define rx_x4_status1_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define rx_x4_status1_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: rx_x4_status1_cl49TxEEEStates_c
 */
#define rx_x4_status1_cl49TxEEEStates_c_SCR_RESET_2        6
#define rx_x4_status1_cl49TxEEEStates_c_SCR_RESET_1        5
#define rx_x4_status1_cl49TxEEEStates_c_TX_WAKE            4
#define rx_x4_status1_cl49TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status1_cl49TxEEEStates_c_TX_QUIET           2
#define rx_x4_status1_cl49TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status1_cl49TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status1_cl36RxEEEStates_l
 */
#define rx_x4_status1_cl36RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status1_cl36RxEEEStates_l_RX_WTF             16
#define rx_x4_status1_cl36RxEEEStates_l_RX_WAKE            8
#define rx_x4_status1_cl36RxEEEStates_l_RX_QUIET           4
#define rx_x4_status1_cl36RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status1_cl36RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status1_cl36RxEEEStates_c
 */
#define rx_x4_status1_cl36RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status1_cl36RxEEEStates_c_RX_WTF             4
#define rx_x4_status1_cl36RxEEEStates_c_RX_WAKE            3
#define rx_x4_status1_cl36RxEEEStates_c_RX_QUIET           2
#define rx_x4_status1_cl36RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status1_cl36RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status1_cl49RxEEEStates_l
 */
#define rx_x4_status1_cl49RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status1_cl49RxEEEStates_l_RX_WTF             16
#define rx_x4_status1_cl49RxEEEStates_l_RX_WAKE            8
#define rx_x4_status1_cl49RxEEEStates_l_RX_QUIET           4
#define rx_x4_status1_cl49RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status1_cl49RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status1_cl49RxEEEStates_c
 */
#define rx_x4_status1_cl49RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status1_cl49RxEEEStates_c_RX_WTF             4
#define rx_x4_status1_cl49RxEEEStates_c_RX_WAKE            3
#define rx_x4_status1_cl49RxEEEStates_c_RX_QUIET           2
#define rx_x4_status1_cl49RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status1_cl49RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status1_cl48TxEEEStates_l
 */
#define rx_x4_status1_cl48TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status1_cl48TxEEEStates_l_TX_QUIET           4
#define rx_x4_status1_cl48TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status1_cl48TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status1_cl48TxEEEStates_c
 */
#define rx_x4_status1_cl48TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status1_cl48TxEEEStates_c_TX_QUIET           2
#define rx_x4_status1_cl48TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status1_cl48TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status1_cl48RxEEEStates_l
 */
#define rx_x4_status1_cl48RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status1_cl48RxEEEStates_l_RX_WAKE            16
#define rx_x4_status1_cl48RxEEEStates_l_RX_QUIET           8
#define rx_x4_status1_cl48RxEEEStates_l_RX_DEACT           4
#define rx_x4_status1_cl48RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status1_cl48RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status1_cl48RxEEEStates_c
 */
#define rx_x4_status1_cl48RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status1_cl48RxEEEStates_c_RX_WAKE            4
#define rx_x4_status1_cl48RxEEEStates_c_RX_QUIET           3
#define rx_x4_status1_cl48RxEEEStates_c_RX_DEACT           2
#define rx_x4_status1_cl48RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status1_cl48RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status1_IQP_Options
 */
#define rx_x4_status1_IQP_Options_i50uA                    0
#define rx_x4_status1_IQP_Options_i100uA                   1
#define rx_x4_status1_IQP_Options_i150uA                   2
#define rx_x4_status1_IQP_Options_i200uA                   3
#define rx_x4_status1_IQP_Options_i250uA                   4
#define rx_x4_status1_IQP_Options_i300uA                   5
#define rx_x4_status1_IQP_Options_i350uA                   6
#define rx_x4_status1_IQP_Options_i400uA                   7
#define rx_x4_status1_IQP_Options_i450uA                   8
#define rx_x4_status1_IQP_Options_i500uA                   9
#define rx_x4_status1_IQP_Options_i550uA                   10
#define rx_x4_status1_IQP_Options_i600uA                   11
#define rx_x4_status1_IQP_Options_i650uA                   12
#define rx_x4_status1_IQP_Options_i700uA                   13
#define rx_x4_status1_IQP_Options_i750uA                   14
#define rx_x4_status1_IQP_Options_i800uA                   15

/****************************************************************************
 * Enums: rx_x4_status1_IDriver_Options
 */
#define rx_x4_status1_IDriver_Options_v680mV               0
#define rx_x4_status1_IDriver_Options_v730mV               1
#define rx_x4_status1_IDriver_Options_v780mV               2
#define rx_x4_status1_IDriver_Options_v830mV               3
#define rx_x4_status1_IDriver_Options_v880mV               4
#define rx_x4_status1_IDriver_Options_v930mV               5
#define rx_x4_status1_IDriver_Options_v980mV               6
#define rx_x4_status1_IDriver_Options_v1010mV              7
#define rx_x4_status1_IDriver_Options_v1040mV              8
#define rx_x4_status1_IDriver_Options_v1060mV              9
#define rx_x4_status1_IDriver_Options_v1070mV              10
#define rx_x4_status1_IDriver_Options_v1080mV              11
#define rx_x4_status1_IDriver_Options_v1085mV              12
#define rx_x4_status1_IDriver_Options_v1090mV              13
#define rx_x4_status1_IDriver_Options_v1095mV              14
#define rx_x4_status1_IDriver_Options_v1100mV              15

/****************************************************************************
 * Enums: rx_x4_status1_operationModes
 */
#define rx_x4_status1_operationModes_XGXS                  0
#define rx_x4_status1_operationModes_XGXG_nCC              1
#define rx_x4_status1_operationModes_Indlane_OS8           4
#define rx_x4_status1_operationModes_IndLane_OS5           5
#define rx_x4_status1_operationModes_PCI                   7
#define rx_x4_status1_operationModes_XGXS_nLQ              8
#define rx_x4_status1_operationModes_XGXS_nLQnCC           9
#define rx_x4_status1_operationModes_PBypass               10
#define rx_x4_status1_operationModes_PBypass_nDSK          11
#define rx_x4_status1_operationModes_ComboCoreMode         12
#define rx_x4_status1_operationModes_Clocks_off            15

/****************************************************************************
 * Enums: rx_x4_status1_actualSpeeds
 */
#define rx_x4_status1_actualSpeeds_dr_10M                  0
#define rx_x4_status1_actualSpeeds_dr_100M                 1
#define rx_x4_status1_actualSpeeds_dr_1G                   2
#define rx_x4_status1_actualSpeeds_dr_2p5G                 3
#define rx_x4_status1_actualSpeeds_dr_5G_X4                4
#define rx_x4_status1_actualSpeeds_dr_6G_X4                5
#define rx_x4_status1_actualSpeeds_dr_10G_HiG              6
#define rx_x4_status1_actualSpeeds_dr_10G_CX4              7
#define rx_x4_status1_actualSpeeds_dr_12G_HiG              8
#define rx_x4_status1_actualSpeeds_dr_12p5G_X4             9
#define rx_x4_status1_actualSpeeds_dr_13G_X4               10
#define rx_x4_status1_actualSpeeds_dr_15G_X4               11
#define rx_x4_status1_actualSpeeds_dr_16G_X4               12
#define rx_x4_status1_actualSpeeds_dr_1G_KX                13
#define rx_x4_status1_actualSpeeds_dr_10G_KX4              14
#define rx_x4_status1_actualSpeeds_dr_10G_KR               15
#define rx_x4_status1_actualSpeeds_dr_5G                   16
#define rx_x4_status1_actualSpeeds_dr_6p4G                 17
#define rx_x4_status1_actualSpeeds_dr_20G_X4               18
#define rx_x4_status1_actualSpeeds_dr_21G_X4               19
#define rx_x4_status1_actualSpeeds_dr_25G_X4               20
#define rx_x4_status1_actualSpeeds_dr_10G_HiG_DXGXS        21
#define rx_x4_status1_actualSpeeds_dr_10G_DXGXS            22
#define rx_x4_status1_actualSpeeds_dr_10p5G_HiG_DXGXS      23
#define rx_x4_status1_actualSpeeds_dr_10p5G_DXGXS          24
#define rx_x4_status1_actualSpeeds_dr_12p773G_HiG_DXGXS    25
#define rx_x4_status1_actualSpeeds_dr_12p773G_DXGXS        26
#define rx_x4_status1_actualSpeeds_dr_10G_XFI              27
#define rx_x4_status1_actualSpeeds_dr_40G                  28
#define rx_x4_status1_actualSpeeds_dr_20G_HiG_DXGXS        29
#define rx_x4_status1_actualSpeeds_dr_20G_DXGXS            30
#define rx_x4_status1_actualSpeeds_dr_10G_SFI              31
#define rx_x4_status1_actualSpeeds_dr_31p5G                32
#define rx_x4_status1_actualSpeeds_dr_32p7G                33
#define rx_x4_status1_actualSpeeds_dr_20G_SCR              34
#define rx_x4_status1_actualSpeeds_dr_10G_HiG_DXGXS_SCR    35
#define rx_x4_status1_actualSpeeds_dr_10G_DXGXS_SCR        36
#define rx_x4_status1_actualSpeeds_dr_12G_R2               37
#define rx_x4_status1_actualSpeeds_dr_10G_X2               38
#define rx_x4_status1_actualSpeeds_dr_40G_KR4              39
#define rx_x4_status1_actualSpeeds_dr_40G_CR4              40
#define rx_x4_status1_actualSpeeds_dr_100G_CR10            41
#define rx_x4_status1_actualSpeeds_dr_15p75G_DXGXS         44
#define rx_x4_status1_actualSpeeds_dr_20G_KR2              57
#define rx_x4_status1_actualSpeeds_dr_20G_CR2              58

/****************************************************************************
 * Enums: rx_x4_status1_actualSpeedsMisc1
 */
#define rx_x4_status1_actualSpeedsMisc1_dr_2500BRCM_X1     16
#define rx_x4_status1_actualSpeedsMisc1_dr_5000BRCM_X4     17
#define rx_x4_status1_actualSpeedsMisc1_dr_6000BRCM_X4     18
#define rx_x4_status1_actualSpeedsMisc1_dr_10GHiGig_X4     19
#define rx_x4_status1_actualSpeedsMisc1_dr_10GBASE_CX4     20
#define rx_x4_status1_actualSpeedsMisc1_dr_12GHiGig_X4     21
#define rx_x4_status1_actualSpeedsMisc1_dr_12p5GHiGig_X4   22
#define rx_x4_status1_actualSpeedsMisc1_dr_13GHiGig_X4     23
#define rx_x4_status1_actualSpeedsMisc1_dr_15GHiGig_X4     24
#define rx_x4_status1_actualSpeedsMisc1_dr_16GHiGig_X4     25
#define rx_x4_status1_actualSpeedsMisc1_dr_5000BRCM_X1     26
#define rx_x4_status1_actualSpeedsMisc1_dr_6363BRCM_X1     27
#define rx_x4_status1_actualSpeedsMisc1_dr_20GHiGig_X4     28
#define rx_x4_status1_actualSpeedsMisc1_dr_21GHiGig_X4     29
#define rx_x4_status1_actualSpeedsMisc1_dr_25p45GHiGig_X4  30
#define rx_x4_status1_actualSpeedsMisc1_dr_10G_HiG_DXGXS   31

/****************************************************************************
 * Enums: rx_x4_status1_IndLaneModes
 */
#define rx_x4_status1_IndLaneModes_SWSDR_div2              0
#define rx_x4_status1_IndLaneModes_SWSDR_div1              1
#define rx_x4_status1_IndLaneModes_DWSDR_div2              2
#define rx_x4_status1_IndLaneModes_DWSDR_div1              3

/****************************************************************************
 * Enums: rx_x4_status1_prbsSelect
 */
#define rx_x4_status1_prbsSelect_prbs7                     0
#define rx_x4_status1_prbsSelect_prbs15                    1
#define rx_x4_status1_prbsSelect_prbs23                    2
#define rx_x4_status1_prbsSelect_prbs31                    3

/****************************************************************************
 * Enums: rx_x4_status1_vcoDivider
 */
#define rx_x4_status1_vcoDivider_div32                     0
#define rx_x4_status1_vcoDivider_div36                     1
#define rx_x4_status1_vcoDivider_div40                     2
#define rx_x4_status1_vcoDivider_div42                     3
#define rx_x4_status1_vcoDivider_div48                     4
#define rx_x4_status1_vcoDivider_div50                     5
#define rx_x4_status1_vcoDivider_div52                     6
#define rx_x4_status1_vcoDivider_div54                     7
#define rx_x4_status1_vcoDivider_div60                     8
#define rx_x4_status1_vcoDivider_div64                     9
#define rx_x4_status1_vcoDivider_div66                     10
#define rx_x4_status1_vcoDivider_div68                     11
#define rx_x4_status1_vcoDivider_div70                     12
#define rx_x4_status1_vcoDivider_div80                     13
#define rx_x4_status1_vcoDivider_div92                     14
#define rx_x4_status1_vcoDivider_div100                    15

/****************************************************************************
 * Enums: rx_x4_status1_refClkSelect
 */
#define rx_x4_status1_refClkSelect_clk_25MHz               0
#define rx_x4_status1_refClkSelect_clk_100MHz              1
#define rx_x4_status1_refClkSelect_clk_125MHz              2
#define rx_x4_status1_refClkSelect_clk_156p25MHz           3
#define rx_x4_status1_refClkSelect_clk_187p5MHz            4
#define rx_x4_status1_refClkSelect_clk_161p25Mhz           5
#define rx_x4_status1_refClkSelect_clk_50Mhz               6
#define rx_x4_status1_refClkSelect_clk_106p25Mhz           7

/****************************************************************************
 * Enums: rx_x4_status1_aerMMDdevTypeSelect
 */
#define rx_x4_status1_aerMMDdevTypeSelect_combo_core       0
#define rx_x4_status1_aerMMDdevTypeSelect_PMA_PMD          1
#define rx_x4_status1_aerMMDdevTypeSelect_PCS              3
#define rx_x4_status1_aerMMDdevTypeSelect_PHY              4
#define rx_x4_status1_aerMMDdevTypeSelect_DTE              5
#define rx_x4_status1_aerMMDdevTypeSelect_CL73_AN          7

/****************************************************************************
 * Enums: rx_x4_status1_aerMMDportSelect
 */
#define rx_x4_status1_aerMMDportSelect_ln0                 0
#define rx_x4_status1_aerMMDportSelect_ln1                 1
#define rx_x4_status1_aerMMDportSelect_ln2                 2
#define rx_x4_status1_aerMMDportSelect_ln3                 3
#define rx_x4_status1_aerMMDportSelect_BCST_ln0_1_2_3      511
#define rx_x4_status1_aerMMDportSelect_BCST_ln0_1          512
#define rx_x4_status1_aerMMDportSelect_BCST_ln2_3          513

/****************************************************************************
 * Enums: rx_x4_status1_firmwareModeSelect
 */
#define rx_x4_status1_firmwareModeSelect_DEFAULT           0
#define rx_x4_status1_firmwareModeSelect_SFP_OPT_LR        1
#define rx_x4_status1_firmwareModeSelect_SFP_DAC           2
#define rx_x4_status1_firmwareModeSelect_XLAUI             3
#define rx_x4_status1_firmwareModeSelect_LONG_CH_6G        4

/****************************************************************************
 * Enums: rx_x4_status1_tempIdxSelect
 */
#define rx_x4_status1_tempIdxSelect_LTE__22p9C             15
#define rx_x4_status1_tempIdxSelect_LTE__12p6C             14
#define rx_x4_status1_tempIdxSelect_LTE__3p0C              13
#define rx_x4_status1_tempIdxSelect_LTE_6p7C               12
#define rx_x4_status1_tempIdxSelect_LTE_16p4C              11
#define rx_x4_status1_tempIdxSelect_LTE_26p6C              10
#define rx_x4_status1_tempIdxSelect_LTE_36p3C              9
#define rx_x4_status1_tempIdxSelect_LTE_46p0C              8
#define rx_x4_status1_tempIdxSelect_LTE_56p2C              7
#define rx_x4_status1_tempIdxSelect_LTE_65p9C              6
#define rx_x4_status1_tempIdxSelect_LTE_75p6C              5
#define rx_x4_status1_tempIdxSelect_LTE_85p3C              4
#define rx_x4_status1_tempIdxSelect_LTE_95p5C              3
#define rx_x4_status1_tempIdxSelect_LTE_105p2C             2
#define rx_x4_status1_tempIdxSelect_LTE_114p9C             1
#define rx_x4_status1_tempIdxSelect_LTE_125p1C             0

/****************************************************************************
 * Enums: rx_x4_status1_port_mode
 */
#define rx_x4_status1_port_mode_QUAD_PORT                  0
#define rx_x4_status1_port_mode_TRI_1_PORT                 1
#define rx_x4_status1_port_mode_TRI_2_PORT                 2
#define rx_x4_status1_port_mode_DUAL_PORT                  3
#define rx_x4_status1_port_mode_SINGLE_PORT                4

/****************************************************************************
 * Enums: rx_x4_status1_rev_letter_enum
 */
#define rx_x4_status1_rev_letter_enum_REV_A                0
#define rx_x4_status1_rev_letter_enum_REV_B                1
#define rx_x4_status1_rev_letter_enum_REV_C                2
#define rx_x4_status1_rev_letter_enum_REV_D                3

/****************************************************************************
 * Enums: rx_x4_status1_rev_number_enum
 */
#define rx_x4_status1_rev_number_enum_REV_0                0
#define rx_x4_status1_rev_number_enum_REV_1                1
#define rx_x4_status1_rev_number_enum_REV_2                2
#define rx_x4_status1_rev_number_enum_REV_3                3
#define rx_x4_status1_rev_number_enum_REV_4                4
#define rx_x4_status1_rev_number_enum_REV_5                5
#define rx_x4_status1_rev_number_enum_REV_6                6
#define rx_x4_status1_rev_number_enum_REV_7                7

/****************************************************************************
 * Enums: rx_x4_status1_bonding_enum
 */
#define rx_x4_status1_bonding_enum_WIRE_BOND               0
#define rx_x4_status1_bonding_enum_FLIP_CHIP               1

/****************************************************************************
 * Enums: rx_x4_status1_tech_process
 */
#define rx_x4_status1_tech_process_PROCESS_90NM            0
#define rx_x4_status1_tech_process_PROCESS_65NM            1
#define rx_x4_status1_tech_process_PROCESS_40NM            2
#define rx_x4_status1_tech_process_PROCESS_28NM            3

/****************************************************************************
 * Enums: rx_x4_status1_model_num
 */
#define rx_x4_status1_model_num_SERDES_CL73                0
#define rx_x4_status1_model_num_XGXS_16G                   1
#define rx_x4_status1_model_num_HYPERCORE                  2
#define rx_x4_status1_model_num_HYPERLITE                  3
#define rx_x4_status1_model_num_PCIE_G2_PIPE               4
#define rx_x4_status1_model_num_SERDES_1p25GBd             5
#define rx_x4_status1_model_num_SATA2                      6
#define rx_x4_status1_model_num_QSGMII                     7
#define rx_x4_status1_model_num_XGXS10G                    8
#define rx_x4_status1_model_num_WARPCORE                   9
#define rx_x4_status1_model_num_XFICORE                    10
#define rx_x4_status1_model_num_RXFI                       11
#define rx_x4_status1_model_num_WARPLITE                   12
#define rx_x4_status1_model_num_PENTACORE                  13
#define rx_x4_status1_model_num_ESM                        14
#define rx_x4_status1_model_num_QUAD_SGMII                 15
#define rx_x4_status1_model_num_WARPCORE_3                 16
#define rx_x4_status1_model_num_TSC                        17
#define rx_x4_status1_model_num_TSC4E                      18
#define rx_x4_status1_model_num_TSC12E                     19
#define rx_x4_status1_model_num_TSC4F                      20
#define rx_x4_status1_model_num_XGXS_CL73_90NM             29
#define rx_x4_status1_model_num_SERDES_CL73_90NM           30
#define rx_x4_status1_model_num_WARPCORE3                  32
#define rx_x4_status1_model_num_WARPCORE4_TSC              33
#define rx_x4_status1_model_num_RXAUI                      34

/****************************************************************************
 * Enums: rx_x4_status1_payload
 */
#define rx_x4_status1_payload_REPEAT_2_BYTES               0
#define rx_x4_status1_payload_RAMPING                      1
#define rx_x4_status1_payload_CL48_CRPAT                   2
#define rx_x4_status1_payload_CL48_CJPAT                   3
#define rx_x4_status1_payload_CL36_LONG_CRPAT              4
#define rx_x4_status1_payload_CL36_SHORT_CRPAT             5

/****************************************************************************
 * Enums: rx_x4_status1_sc
 */
#define rx_x4_status1_sc_S_10G_CR1                         0
#define rx_x4_status1_sc_S_10G_KR1                         1
#define rx_x4_status1_sc_S_10G_X1                          2
#define rx_x4_status1_sc_S_10G_HG2_CR1                     4
#define rx_x4_status1_sc_S_10G_HG2_KR1                     5
#define rx_x4_status1_sc_S_10G_HG2_X1                      6
#define rx_x4_status1_sc_S_20G_CR1                         8
#define rx_x4_status1_sc_S_20G_KR1                         9
#define rx_x4_status1_sc_S_20G_X1                          10
#define rx_x4_status1_sc_S_20G_HG2_CR1                     12
#define rx_x4_status1_sc_S_20G_HG2_KR1                     13
#define rx_x4_status1_sc_S_20G_HG2_X1                      14
#define rx_x4_status1_sc_S_25G_CR1                         16
#define rx_x4_status1_sc_S_25G_KR1                         17
#define rx_x4_status1_sc_S_25G_X1                          18
#define rx_x4_status1_sc_S_25G_HG2_CR1                     20
#define rx_x4_status1_sc_S_25G_HG2_KR1                     21
#define rx_x4_status1_sc_S_25G_HG2_X1                      22
#define rx_x4_status1_sc_S_20G_CR2                         24
#define rx_x4_status1_sc_S_20G_KR2                         25
#define rx_x4_status1_sc_S_20G_X2                          26
#define rx_x4_status1_sc_S_20G_HG2_CR2                     28
#define rx_x4_status1_sc_S_20G_HG2_KR2                     29
#define rx_x4_status1_sc_S_20G_HG2_X2                      30
#define rx_x4_status1_sc_S_40G_CR2                         32
#define rx_x4_status1_sc_S_40G_KR2                         33
#define rx_x4_status1_sc_S_40G_X2                          34
#define rx_x4_status1_sc_S_40G_HG2_CR2                     36
#define rx_x4_status1_sc_S_40G_HG2_KR2                     37
#define rx_x4_status1_sc_S_40G_HG2_X2                      38
#define rx_x4_status1_sc_S_40G_CR4                         40
#define rx_x4_status1_sc_S_40G_KR4                         41
#define rx_x4_status1_sc_S_40G_X4                          42
#define rx_x4_status1_sc_S_40G_HG2_CR4                     44
#define rx_x4_status1_sc_S_40G_HG2_KR4                     45
#define rx_x4_status1_sc_S_40G_HG2_X4                      46
#define rx_x4_status1_sc_S_50G_CR2                         48
#define rx_x4_status1_sc_S_50G_KR2                         49
#define rx_x4_status1_sc_S_50G_X2                          50
#define rx_x4_status1_sc_S_50G_HG2_CR2                     52
#define rx_x4_status1_sc_S_50G_HG2_KR2                     53
#define rx_x4_status1_sc_S_50G_HG2_X2                      54
#define rx_x4_status1_sc_S_50G_CR4                         56
#define rx_x4_status1_sc_S_50G_KR4                         57
#define rx_x4_status1_sc_S_50G_X4                          58
#define rx_x4_status1_sc_S_50G_HG2_CR4                     60
#define rx_x4_status1_sc_S_50G_HG2_KR4                     61
#define rx_x4_status1_sc_S_50G_HG2_X4                      62
#define rx_x4_status1_sc_S_100G_CR4                        64
#define rx_x4_status1_sc_S_100G_KR4                        65
#define rx_x4_status1_sc_S_100G_X4                         66
#define rx_x4_status1_sc_S_100G_HG2_CR4                    68
#define rx_x4_status1_sc_S_100G_HG2_KR4                    69
#define rx_x4_status1_sc_S_100G_HG2_X4                     70
#define rx_x4_status1_sc_S_CL73_20GVCO                     72
#define rx_x4_status1_sc_S_CL73_25GVCO                     80
#define rx_x4_status1_sc_S_CL36_20GVCO                     88
#define rx_x4_status1_sc_S_CL36_25GVCO                     96

/****************************************************************************
 * Enums: rx_x4_status1_t_fifo_modes
 */
#define rx_x4_status1_t_fifo_modes_T_FIFO_MODE_BYPASS      0
#define rx_x4_status1_t_fifo_modes_T_FIFO_MODE_40G         1
#define rx_x4_status1_t_fifo_modes_T_FIFO_MODE_100G        2
#define rx_x4_status1_t_fifo_modes_T_FIFO_MODE_20G         3

/****************************************************************************
 * Enums: rx_x4_status1_t_enc_modes
 */
#define rx_x4_status1_t_enc_modes_T_ENC_MODE_BYPASS        0
#define rx_x4_status1_t_enc_modes_T_ENC_MODE_CL49          1
#define rx_x4_status1_t_enc_modes_T_ENC_MODE_CL82          2

/****************************************************************************
 * Enums: rx_x4_status1_btmx_mode
 */
#define rx_x4_status1_btmx_mode_BS_BTMX_MODE_1to1          0
#define rx_x4_status1_btmx_mode_BS_BTMX_MODE_2to1          1
#define rx_x4_status1_btmx_mode_BS_BTMX_MODE_5to1          2

/****************************************************************************
 * Enums: rx_x4_status1_t_type_cl82
 */
#define rx_x4_status1_t_type_cl82_T_TYPE_B1                5
#define rx_x4_status1_t_type_cl82_T_TYPE_C                 4
#define rx_x4_status1_t_type_cl82_T_TYPE_S                 3
#define rx_x4_status1_t_type_cl82_T_TYPE_T                 2
#define rx_x4_status1_t_type_cl82_T_TYPE_D                 1
#define rx_x4_status1_t_type_cl82_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x4_status1_txsm_state_cl82
 */
#define rx_x4_status1_txsm_state_cl82_TX_HIG_END           6
#define rx_x4_status1_txsm_state_cl82_TX_HIG_START         5
#define rx_x4_status1_txsm_state_cl82_TX_E                 4
#define rx_x4_status1_txsm_state_cl82_TX_T                 3
#define rx_x4_status1_txsm_state_cl82_TX_D                 2
#define rx_x4_status1_txsm_state_cl82_TX_C                 1
#define rx_x4_status1_txsm_state_cl82_TX_INIT              0

/****************************************************************************
 * Enums: rx_x4_status1_ltxsm_state_cl82
 */
#define rx_x4_status1_ltxsm_state_cl82_TX_HIG_END          64
#define rx_x4_status1_ltxsm_state_cl82_TX_HIG_START        32
#define rx_x4_status1_ltxsm_state_cl82_TX_E                16
#define rx_x4_status1_ltxsm_state_cl82_TX_T                8
#define rx_x4_status1_ltxsm_state_cl82_TX_D                4
#define rx_x4_status1_ltxsm_state_cl82_TX_C                2
#define rx_x4_status1_ltxsm_state_cl82_TX_INIT             1

/****************************************************************************
 * Enums: rx_x4_status1_r_type_coded_cl82
 */
#define rx_x4_status1_r_type_coded_cl82_R_TYPE_B1          32
#define rx_x4_status1_r_type_coded_cl82_R_TYPE_C           16
#define rx_x4_status1_r_type_coded_cl82_R_TYPE_S           8
#define rx_x4_status1_r_type_coded_cl82_R_TYPE_T           4
#define rx_x4_status1_r_type_coded_cl82_R_TYPE_D           2
#define rx_x4_status1_r_type_coded_cl82_R_TYPE_E           1

/****************************************************************************
 * Enums: rx_x4_status1_rxsm_state_cl82
 */
#define rx_x4_status1_rxsm_state_cl82_RX_HIG_END           64
#define rx_x4_status1_rxsm_state_cl82_RX_HIG_START         32
#define rx_x4_status1_rxsm_state_cl82_RX_E                 16
#define rx_x4_status1_rxsm_state_cl82_RX_T                 8
#define rx_x4_status1_rxsm_state_cl82_RX_D                 4
#define rx_x4_status1_rxsm_state_cl82_RX_C                 2
#define rx_x4_status1_rxsm_state_cl82_RX_INIT              1

/****************************************************************************
 * Enums: rx_x4_status1_deskew_state
 */
#define rx_x4_status1_deskew_state_ALIGN_ACQUIRED          2
#define rx_x4_status1_deskew_state_LOSS_OF_ALIGNMENT       1

/****************************************************************************
 * Enums: rx_x4_status1_os_mode_enum
 */
#define rx_x4_status1_os_mode_enum_OS_MODE_1               0
#define rx_x4_status1_os_mode_enum_OS_MODE_2               1
#define rx_x4_status1_os_mode_enum_OS_MODE_4               2
#define rx_x4_status1_os_mode_enum_OS_MODE_16p5            8
#define rx_x4_status1_os_mode_enum_OS_MODE_20p625          12

/****************************************************************************
 * Enums: rx_x4_status1_scr_modes
 */
#define rx_x4_status1_scr_modes_T_SCR_MODE_BYPASS          0
#define rx_x4_status1_scr_modes_T_SCR_MODE_CL49            1
#define rx_x4_status1_scr_modes_T_SCR_MODE_40G_2_LANE      2
#define rx_x4_status1_scr_modes_T_SCR_MODE_100G            3
#define rx_x4_status1_scr_modes_T_SCR_MODE_20G             4
#define rx_x4_status1_scr_modes_T_SCR_MODE_40G_4_LANE      5

/****************************************************************************
 * Enums: rx_x4_status1_descr_modes
 */
#define rx_x4_status1_descr_modes_R_DESCR_MODE_BYPASS      0
#define rx_x4_status1_descr_modes_R_DESCR_MODE_CL49        1
#define rx_x4_status1_descr_modes_R_DESCR_MODE_CL82        2

/****************************************************************************
 * Enums: rx_x4_status1_r_dec_tl_mode
 */
#define rx_x4_status1_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS   0
#define rx_x4_status1_r_dec_tl_mode_R_DEC_TL_MODE_CL49     1
#define rx_x4_status1_r_dec_tl_mode_R_DEC_TL_MODE_CL82     2

/****************************************************************************
 * Enums: rx_x4_status1_r_dec_fsm_mode
 */
#define rx_x4_status1_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define rx_x4_status1_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49   1
#define rx_x4_status1_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82   2

/****************************************************************************
 * Enums: rx_x4_status1_r_deskew_mode
 */
#define rx_x4_status1_r_deskew_mode_R_DESKEW_MODE_BYPASS   0
#define rx_x4_status1_r_deskew_mode_R_DESKEW_MODE_20G      1
#define rx_x4_status1_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define rx_x4_status1_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define rx_x4_status1_r_deskew_mode_R_DESKEW_MODE_100G     4
#define rx_x4_status1_r_deskew_mode_R_DESKEW_MODE_CL49     5
#define rx_x4_status1_r_deskew_mode_R_DESKEW_MODE_CL91     6

/****************************************************************************
 * Enums: rx_x4_status1_bs_dist_modes
 */
#define rx_x4_status1_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define rx_x4_status1_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define rx_x4_status1_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define rx_x4_status1_bs_dist_modes_BS_DIST_MODE_NO_TDM    3

/****************************************************************************
 * Enums: rx_x4_status1_cl49_t_type
 */
#define rx_x4_status1_cl49_t_type_BAD_T_TYPE               15
#define rx_x4_status1_cl49_t_type_T_TYPE_B0                11
#define rx_x4_status1_cl49_t_type_T_TYPE_OB                10
#define rx_x4_status1_cl49_t_type_T_TYPE_B1                9
#define rx_x4_status1_cl49_t_type_T_TYPE_DB                8
#define rx_x4_status1_cl49_t_type_T_TYPE_FC                7
#define rx_x4_status1_cl49_t_type_T_TYPE_TB                6
#define rx_x4_status1_cl49_t_type_T_TYPE_LI                5
#define rx_x4_status1_cl49_t_type_T_TYPE_C                 4
#define rx_x4_status1_cl49_t_type_T_TYPE_S                 3
#define rx_x4_status1_cl49_t_type_T_TYPE_T                 2
#define rx_x4_status1_cl49_t_type_T_TYPE_D                 1
#define rx_x4_status1_cl49_t_type_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x4_status1_cl49_txsm_states
 */
#define rx_x4_status1_cl49_txsm_states_TX_HIG_END          7
#define rx_x4_status1_cl49_txsm_states_TX_HIG_START        6
#define rx_x4_status1_cl49_txsm_states_TX_LI               5
#define rx_x4_status1_cl49_txsm_states_TX_E                4
#define rx_x4_status1_cl49_txsm_states_TX_T                3
#define rx_x4_status1_cl49_txsm_states_TX_D                2
#define rx_x4_status1_cl49_txsm_states_TX_C                1
#define rx_x4_status1_cl49_txsm_states_TX_INIT             0

/****************************************************************************
 * Enums: rx_x4_status1_cl49_ltxsm_states
 */
#define rx_x4_status1_cl49_ltxsm_states_TX_HIG_END         128
#define rx_x4_status1_cl49_ltxsm_states_TX_HIG_START       64
#define rx_x4_status1_cl49_ltxsm_states_TX_LI              32
#define rx_x4_status1_cl49_ltxsm_states_TX_E               16
#define rx_x4_status1_cl49_ltxsm_states_TX_T               8
#define rx_x4_status1_cl49_ltxsm_states_TX_D               4
#define rx_x4_status1_cl49_ltxsm_states_TX_C               2
#define rx_x4_status1_cl49_ltxsm_states_TX_INIT            1

/****************************************************************************
 * Enums: rx_x4_status1_burst_error_mode
 */
#define rx_x4_status1_burst_error_mode_BURST_ERROR_11_BITS 0
#define rx_x4_status1_burst_error_mode_BURST_ERROR_16_BITS 1
#define rx_x4_status1_burst_error_mode_BURST_ERROR_17_BITS 2
#define rx_x4_status1_burst_error_mode_BURST_ERROR_18_BITS 3
#define rx_x4_status1_burst_error_mode_BURST_ERROR_19_BITS 4
#define rx_x4_status1_burst_error_mode_BURST_ERROR_20_BITS 5
#define rx_x4_status1_burst_error_mode_BURST_ERROR_21_BITS 6
#define rx_x4_status1_burst_error_mode_BURST_ERROR_22_BITS 7
#define rx_x4_status1_burst_error_mode_BURST_ERROR_23_BITS 8
#define rx_x4_status1_burst_error_mode_BURST_ERROR_24_BITS 9
#define rx_x4_status1_burst_error_mode_BURST_ERROR_25_BITS 10
#define rx_x4_status1_burst_error_mode_BURST_ERROR_26_BITS 11
#define rx_x4_status1_burst_error_mode_BURST_ERROR_27_BITS 12
#define rx_x4_status1_burst_error_mode_BURST_ERROR_28_BITS 13
#define rx_x4_status1_burst_error_mode_BURST_ERROR_29_BITS 14
#define rx_x4_status1_burst_error_mode_BURST_ERROR_30_BITS 15
#define rx_x4_status1_burst_error_mode_BURST_ERROR_31_BITS 16
#define rx_x4_status1_burst_error_mode_BURST_ERROR_32_BITS 17
#define rx_x4_status1_burst_error_mode_BURST_ERROR_33_BITS 18
#define rx_x4_status1_burst_error_mode_BURST_ERROR_34_BITS 19
#define rx_x4_status1_burst_error_mode_BURST_ERROR_35_BITS 20
#define rx_x4_status1_burst_error_mode_BURST_ERROR_36_BITS 21
#define rx_x4_status1_burst_error_mode_BURST_ERROR_37_BITS 22
#define rx_x4_status1_burst_error_mode_BURST_ERROR_38_BITS 23
#define rx_x4_status1_burst_error_mode_BURST_ERROR_39_BITS 24
#define rx_x4_status1_burst_error_mode_BURST_ERROR_40_BITS 25
#define rx_x4_status1_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: rx_x4_status1_bermon_state
 */
#define rx_x4_status1_bermon_state_HI_BER                  4
#define rx_x4_status1_bermon_state_GOOD_BER                3
#define rx_x4_status1_bermon_state_BER_TEST_SH             2
#define rx_x4_status1_bermon_state_START_TIMER             1
#define rx_x4_status1_bermon_state_BER_MT_INIT             0

/****************************************************************************
 * Enums: rx_x4_status1_rxsm_state_cl49
 */
#define rx_x4_status1_rxsm_state_cl49_RX_HIG_END           128
#define rx_x4_status1_rxsm_state_cl49_RX_HIG_START         64
#define rx_x4_status1_rxsm_state_cl49_RX_LI                32
#define rx_x4_status1_rxsm_state_cl49_RX_E                 16
#define rx_x4_status1_rxsm_state_cl49_RX_T                 8
#define rx_x4_status1_rxsm_state_cl49_RX_D                 4
#define rx_x4_status1_rxsm_state_cl49_RX_C                 2
#define rx_x4_status1_rxsm_state_cl49_RX_INIT              1

/****************************************************************************
 * Enums: rx_x4_status1_r_type
 */
#define rx_x4_status1_r_type_BAD_R_TYPE                    15
#define rx_x4_status1_r_type_R_TYPE_B0                     11
#define rx_x4_status1_r_type_R_TYPE_OB                     10
#define rx_x4_status1_r_type_R_TYPE_B1                     9
#define rx_x4_status1_r_type_R_TYPE_DB                     8
#define rx_x4_status1_r_type_R_TYPE_FC                     7
#define rx_x4_status1_r_type_R_TYPE_TB                     6
#define rx_x4_status1_r_type_R_TYPE_LI                     5
#define rx_x4_status1_r_type_R_TYPE_C                      4
#define rx_x4_status1_r_type_R_TYPE_S                      3
#define rx_x4_status1_r_type_R_TYPE_T                      2
#define rx_x4_status1_r_type_R_TYPE_D                      1
#define rx_x4_status1_r_type_R_TYPE_E                      0

/****************************************************************************
 * Enums: rx_x4_status1_am_lock_state
 */
#define rx_x4_status1_am_lock_state_INVALID_AM             512
#define rx_x4_status1_am_lock_state_GOOD_AM                256
#define rx_x4_status1_am_lock_state_COMP_AM                128
#define rx_x4_status1_am_lock_state_TIMER_2                64
#define rx_x4_status1_am_lock_state_AM_2_GOOD              32
#define rx_x4_status1_am_lock_state_COMP_2ND               16
#define rx_x4_status1_am_lock_state_TIMER_1                8
#define rx_x4_status1_am_lock_state_FIND_1ST               4
#define rx_x4_status1_am_lock_state_AM_RESET_CNT           2
#define rx_x4_status1_am_lock_state_AM_LOCK_INIT           1

/****************************************************************************
 * Enums: rx_x4_status1_msg_selector
 */
#define rx_x4_status1_msg_selector_RESERVED                0
#define rx_x4_status1_msg_selector_VALUE_802p3             1
#define rx_x4_status1_msg_selector_VALUE_802p9             2
#define rx_x4_status1_msg_selector_VALUE_802p5             3
#define rx_x4_status1_msg_selector_VALUE_1394              4

/****************************************************************************
 * Enums: rx_x4_status1_synce_enum
 */
#define rx_x4_status1_synce_enum_SYNCE_NO_DIV              0
#define rx_x4_status1_synce_enum_SYNCE_DIV_7               1
#define rx_x4_status1_synce_enum_SYNCE_DIV_11              2

/****************************************************************************
 * Enums: rx_x4_status1_synce_enum_stage0
 */
#define rx_x4_status1_synce_enum_stage0_SYNCE_NO_DIV       0
#define rx_x4_status1_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define rx_x4_status1_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: rx_x4_status1_cl91_sync_state
 */
#define rx_x4_status1_cl91_sync_state_FIND_1ST             0
#define rx_x4_status1_cl91_sync_state_COUNT_NEXT           1
#define rx_x4_status1_cl91_sync_state_COMP_2ND             2
#define rx_x4_status1_cl91_sync_state_TWO_GOOD             3

/****************************************************************************
 * Enums: rx_x4_status1_cl91_algn_state
 */
#define rx_x4_status1_cl91_algn_state_LOSS_OF_ALIGNMENT    0
#define rx_x4_status1_cl91_algn_state_DESKEW               1
#define rx_x4_status1_cl91_algn_state_DESKEW_FAIL          2
#define rx_x4_status1_cl91_algn_state_ALIGN_ACQUIRED       3
#define rx_x4_status1_cl91_algn_state_CW_GOOD              4
#define rx_x4_status1_cl91_algn_state_CW_BAD               5
#define rx_x4_status1_cl91_algn_state_THREE_BAD            6

/****************************************************************************
 * Enums: rx_x4_status1_fec_sel
 */
#define rx_x4_status1_fec_sel_NO_FEC                       0
#define rx_x4_status1_fec_sel_FEC_CL74                     1
#define rx_x4_status1_fec_sel_FEC_CL91                     2

/****************************************************************************
 * Enums: rx_x4_status2_cl36TxEEEStates_l
 */
#define rx_x4_status2_cl36TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status2_cl36TxEEEStates_l_TX_QUIET           4
#define rx_x4_status2_cl36TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status2_cl36TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status2_cl36TxEEEStates_c
 */
#define rx_x4_status2_cl36TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status2_cl36TxEEEStates_c_TX_QUIET           2
#define rx_x4_status2_cl36TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status2_cl36TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status2_cl49TxEEEStates_l
 */
#define rx_x4_status2_cl49TxEEEStates_l_SCR_RESET_2        64
#define rx_x4_status2_cl49TxEEEStates_l_SCR_RESET_1        32
#define rx_x4_status2_cl49TxEEEStates_l_TX_WAKE            16
#define rx_x4_status2_cl49TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status2_cl49TxEEEStates_l_TX_QUIET           4
#define rx_x4_status2_cl49TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status2_cl49TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status2_fec_req_enum
 */
#define rx_x4_status2_fec_req_enum_FEC_not_supported       0
#define rx_x4_status2_fec_req_enum_FEC_supported_but_not_requested 1
#define rx_x4_status2_fec_req_enum_invalid_setting         2
#define rx_x4_status2_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: rx_x4_status2_cl73_pause_enum
 */
#define rx_x4_status2_cl73_pause_enum_No_PAUSE_ability     0
#define rx_x4_status2_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define rx_x4_status2_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define rx_x4_status2_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: rx_x4_status2_cl49TxEEEStates_c
 */
#define rx_x4_status2_cl49TxEEEStates_c_SCR_RESET_2        6
#define rx_x4_status2_cl49TxEEEStates_c_SCR_RESET_1        5
#define rx_x4_status2_cl49TxEEEStates_c_TX_WAKE            4
#define rx_x4_status2_cl49TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status2_cl49TxEEEStates_c_TX_QUIET           2
#define rx_x4_status2_cl49TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status2_cl49TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status2_cl36RxEEEStates_l
 */
#define rx_x4_status2_cl36RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status2_cl36RxEEEStates_l_RX_WTF             16
#define rx_x4_status2_cl36RxEEEStates_l_RX_WAKE            8
#define rx_x4_status2_cl36RxEEEStates_l_RX_QUIET           4
#define rx_x4_status2_cl36RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status2_cl36RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status2_cl36RxEEEStates_c
 */
#define rx_x4_status2_cl36RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status2_cl36RxEEEStates_c_RX_WTF             4
#define rx_x4_status2_cl36RxEEEStates_c_RX_WAKE            3
#define rx_x4_status2_cl36RxEEEStates_c_RX_QUIET           2
#define rx_x4_status2_cl36RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status2_cl36RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status2_cl49RxEEEStates_l
 */
#define rx_x4_status2_cl49RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status2_cl49RxEEEStates_l_RX_WTF             16
#define rx_x4_status2_cl49RxEEEStates_l_RX_WAKE            8
#define rx_x4_status2_cl49RxEEEStates_l_RX_QUIET           4
#define rx_x4_status2_cl49RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status2_cl49RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status2_cl49RxEEEStates_c
 */
#define rx_x4_status2_cl49RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status2_cl49RxEEEStates_c_RX_WTF             4
#define rx_x4_status2_cl49RxEEEStates_c_RX_WAKE            3
#define rx_x4_status2_cl49RxEEEStates_c_RX_QUIET           2
#define rx_x4_status2_cl49RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status2_cl49RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status2_cl48TxEEEStates_l
 */
#define rx_x4_status2_cl48TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status2_cl48TxEEEStates_l_TX_QUIET           4
#define rx_x4_status2_cl48TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status2_cl48TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status2_cl48TxEEEStates_c
 */
#define rx_x4_status2_cl48TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status2_cl48TxEEEStates_c_TX_QUIET           2
#define rx_x4_status2_cl48TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status2_cl48TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status2_cl48RxEEEStates_l
 */
#define rx_x4_status2_cl48RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status2_cl48RxEEEStates_l_RX_WAKE            16
#define rx_x4_status2_cl48RxEEEStates_l_RX_QUIET           8
#define rx_x4_status2_cl48RxEEEStates_l_RX_DEACT           4
#define rx_x4_status2_cl48RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status2_cl48RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status2_cl48RxEEEStates_c
 */
#define rx_x4_status2_cl48RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status2_cl48RxEEEStates_c_RX_WAKE            4
#define rx_x4_status2_cl48RxEEEStates_c_RX_QUIET           3
#define rx_x4_status2_cl48RxEEEStates_c_RX_DEACT           2
#define rx_x4_status2_cl48RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status2_cl48RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status2_IQP_Options
 */
#define rx_x4_status2_IQP_Options_i50uA                    0
#define rx_x4_status2_IQP_Options_i100uA                   1
#define rx_x4_status2_IQP_Options_i150uA                   2
#define rx_x4_status2_IQP_Options_i200uA                   3
#define rx_x4_status2_IQP_Options_i250uA                   4
#define rx_x4_status2_IQP_Options_i300uA                   5
#define rx_x4_status2_IQP_Options_i350uA                   6
#define rx_x4_status2_IQP_Options_i400uA                   7
#define rx_x4_status2_IQP_Options_i450uA                   8
#define rx_x4_status2_IQP_Options_i500uA                   9
#define rx_x4_status2_IQP_Options_i550uA                   10
#define rx_x4_status2_IQP_Options_i600uA                   11
#define rx_x4_status2_IQP_Options_i650uA                   12
#define rx_x4_status2_IQP_Options_i700uA                   13
#define rx_x4_status2_IQP_Options_i750uA                   14
#define rx_x4_status2_IQP_Options_i800uA                   15

/****************************************************************************
 * Enums: rx_x4_status2_IDriver_Options
 */
#define rx_x4_status2_IDriver_Options_v680mV               0
#define rx_x4_status2_IDriver_Options_v730mV               1
#define rx_x4_status2_IDriver_Options_v780mV               2
#define rx_x4_status2_IDriver_Options_v830mV               3
#define rx_x4_status2_IDriver_Options_v880mV               4
#define rx_x4_status2_IDriver_Options_v930mV               5
#define rx_x4_status2_IDriver_Options_v980mV               6
#define rx_x4_status2_IDriver_Options_v1010mV              7
#define rx_x4_status2_IDriver_Options_v1040mV              8
#define rx_x4_status2_IDriver_Options_v1060mV              9
#define rx_x4_status2_IDriver_Options_v1070mV              10
#define rx_x4_status2_IDriver_Options_v1080mV              11
#define rx_x4_status2_IDriver_Options_v1085mV              12
#define rx_x4_status2_IDriver_Options_v1090mV              13
#define rx_x4_status2_IDriver_Options_v1095mV              14
#define rx_x4_status2_IDriver_Options_v1100mV              15

/****************************************************************************
 * Enums: rx_x4_status2_operationModes
 */
#define rx_x4_status2_operationModes_XGXS                  0
#define rx_x4_status2_operationModes_XGXG_nCC              1
#define rx_x4_status2_operationModes_Indlane_OS8           4
#define rx_x4_status2_operationModes_IndLane_OS5           5
#define rx_x4_status2_operationModes_PCI                   7
#define rx_x4_status2_operationModes_XGXS_nLQ              8
#define rx_x4_status2_operationModes_XGXS_nLQnCC           9
#define rx_x4_status2_operationModes_PBypass               10
#define rx_x4_status2_operationModes_PBypass_nDSK          11
#define rx_x4_status2_operationModes_ComboCoreMode         12
#define rx_x4_status2_operationModes_Clocks_off            15

/****************************************************************************
 * Enums: rx_x4_status2_actualSpeeds
 */
#define rx_x4_status2_actualSpeeds_dr_10M                  0
#define rx_x4_status2_actualSpeeds_dr_100M                 1
#define rx_x4_status2_actualSpeeds_dr_1G                   2
#define rx_x4_status2_actualSpeeds_dr_2p5G                 3
#define rx_x4_status2_actualSpeeds_dr_5G_X4                4
#define rx_x4_status2_actualSpeeds_dr_6G_X4                5
#define rx_x4_status2_actualSpeeds_dr_10G_HiG              6
#define rx_x4_status2_actualSpeeds_dr_10G_CX4              7
#define rx_x4_status2_actualSpeeds_dr_12G_HiG              8
#define rx_x4_status2_actualSpeeds_dr_12p5G_X4             9
#define rx_x4_status2_actualSpeeds_dr_13G_X4               10
#define rx_x4_status2_actualSpeeds_dr_15G_X4               11
#define rx_x4_status2_actualSpeeds_dr_16G_X4               12
#define rx_x4_status2_actualSpeeds_dr_1G_KX                13
#define rx_x4_status2_actualSpeeds_dr_10G_KX4              14
#define rx_x4_status2_actualSpeeds_dr_10G_KR               15
#define rx_x4_status2_actualSpeeds_dr_5G                   16
#define rx_x4_status2_actualSpeeds_dr_6p4G                 17
#define rx_x4_status2_actualSpeeds_dr_20G_X4               18
#define rx_x4_status2_actualSpeeds_dr_21G_X4               19
#define rx_x4_status2_actualSpeeds_dr_25G_X4               20
#define rx_x4_status2_actualSpeeds_dr_10G_HiG_DXGXS        21
#define rx_x4_status2_actualSpeeds_dr_10G_DXGXS            22
#define rx_x4_status2_actualSpeeds_dr_10p5G_HiG_DXGXS      23
#define rx_x4_status2_actualSpeeds_dr_10p5G_DXGXS          24
#define rx_x4_status2_actualSpeeds_dr_12p773G_HiG_DXGXS    25
#define rx_x4_status2_actualSpeeds_dr_12p773G_DXGXS        26
#define rx_x4_status2_actualSpeeds_dr_10G_XFI              27
#define rx_x4_status2_actualSpeeds_dr_40G                  28
#define rx_x4_status2_actualSpeeds_dr_20G_HiG_DXGXS        29
#define rx_x4_status2_actualSpeeds_dr_20G_DXGXS            30
#define rx_x4_status2_actualSpeeds_dr_10G_SFI              31
#define rx_x4_status2_actualSpeeds_dr_31p5G                32
#define rx_x4_status2_actualSpeeds_dr_32p7G                33
#define rx_x4_status2_actualSpeeds_dr_20G_SCR              34
#define rx_x4_status2_actualSpeeds_dr_10G_HiG_DXGXS_SCR    35
#define rx_x4_status2_actualSpeeds_dr_10G_DXGXS_SCR        36
#define rx_x4_status2_actualSpeeds_dr_12G_R2               37
#define rx_x4_status2_actualSpeeds_dr_10G_X2               38
#define rx_x4_status2_actualSpeeds_dr_40G_KR4              39
#define rx_x4_status2_actualSpeeds_dr_40G_CR4              40
#define rx_x4_status2_actualSpeeds_dr_100G_CR10            41
#define rx_x4_status2_actualSpeeds_dr_15p75G_DXGXS         44
#define rx_x4_status2_actualSpeeds_dr_20G_KR2              57
#define rx_x4_status2_actualSpeeds_dr_20G_CR2              58

/****************************************************************************
 * Enums: rx_x4_status2_actualSpeedsMisc1
 */
#define rx_x4_status2_actualSpeedsMisc1_dr_2500BRCM_X1     16
#define rx_x4_status2_actualSpeedsMisc1_dr_5000BRCM_X4     17
#define rx_x4_status2_actualSpeedsMisc1_dr_6000BRCM_X4     18
#define rx_x4_status2_actualSpeedsMisc1_dr_10GHiGig_X4     19
#define rx_x4_status2_actualSpeedsMisc1_dr_10GBASE_CX4     20
#define rx_x4_status2_actualSpeedsMisc1_dr_12GHiGig_X4     21
#define rx_x4_status2_actualSpeedsMisc1_dr_12p5GHiGig_X4   22
#define rx_x4_status2_actualSpeedsMisc1_dr_13GHiGig_X4     23
#define rx_x4_status2_actualSpeedsMisc1_dr_15GHiGig_X4     24
#define rx_x4_status2_actualSpeedsMisc1_dr_16GHiGig_X4     25
#define rx_x4_status2_actualSpeedsMisc1_dr_5000BRCM_X1     26
#define rx_x4_status2_actualSpeedsMisc1_dr_6363BRCM_X1     27
#define rx_x4_status2_actualSpeedsMisc1_dr_20GHiGig_X4     28
#define rx_x4_status2_actualSpeedsMisc1_dr_21GHiGig_X4     29
#define rx_x4_status2_actualSpeedsMisc1_dr_25p45GHiGig_X4  30
#define rx_x4_status2_actualSpeedsMisc1_dr_10G_HiG_DXGXS   31

/****************************************************************************
 * Enums: rx_x4_status2_IndLaneModes
 */
#define rx_x4_status2_IndLaneModes_SWSDR_div2              0
#define rx_x4_status2_IndLaneModes_SWSDR_div1              1
#define rx_x4_status2_IndLaneModes_DWSDR_div2              2
#define rx_x4_status2_IndLaneModes_DWSDR_div1              3

/****************************************************************************
 * Enums: rx_x4_status2_prbsSelect
 */
#define rx_x4_status2_prbsSelect_prbs7                     0
#define rx_x4_status2_prbsSelect_prbs15                    1
#define rx_x4_status2_prbsSelect_prbs23                    2
#define rx_x4_status2_prbsSelect_prbs31                    3

/****************************************************************************
 * Enums: rx_x4_status2_vcoDivider
 */
#define rx_x4_status2_vcoDivider_div32                     0
#define rx_x4_status2_vcoDivider_div36                     1
#define rx_x4_status2_vcoDivider_div40                     2
#define rx_x4_status2_vcoDivider_div42                     3
#define rx_x4_status2_vcoDivider_div48                     4
#define rx_x4_status2_vcoDivider_div50                     5
#define rx_x4_status2_vcoDivider_div52                     6
#define rx_x4_status2_vcoDivider_div54                     7
#define rx_x4_status2_vcoDivider_div60                     8
#define rx_x4_status2_vcoDivider_div64                     9
#define rx_x4_status2_vcoDivider_div66                     10
#define rx_x4_status2_vcoDivider_div68                     11
#define rx_x4_status2_vcoDivider_div70                     12
#define rx_x4_status2_vcoDivider_div80                     13
#define rx_x4_status2_vcoDivider_div92                     14
#define rx_x4_status2_vcoDivider_div100                    15

/****************************************************************************
 * Enums: rx_x4_status2_refClkSelect
 */
#define rx_x4_status2_refClkSelect_clk_25MHz               0
#define rx_x4_status2_refClkSelect_clk_100MHz              1
#define rx_x4_status2_refClkSelect_clk_125MHz              2
#define rx_x4_status2_refClkSelect_clk_156p25MHz           3
#define rx_x4_status2_refClkSelect_clk_187p5MHz            4
#define rx_x4_status2_refClkSelect_clk_161p25Mhz           5
#define rx_x4_status2_refClkSelect_clk_50Mhz               6
#define rx_x4_status2_refClkSelect_clk_106p25Mhz           7

/****************************************************************************
 * Enums: rx_x4_status2_aerMMDdevTypeSelect
 */
#define rx_x4_status2_aerMMDdevTypeSelect_combo_core       0
#define rx_x4_status2_aerMMDdevTypeSelect_PMA_PMD          1
#define rx_x4_status2_aerMMDdevTypeSelect_PCS              3
#define rx_x4_status2_aerMMDdevTypeSelect_PHY              4
#define rx_x4_status2_aerMMDdevTypeSelect_DTE              5
#define rx_x4_status2_aerMMDdevTypeSelect_CL73_AN          7

/****************************************************************************
 * Enums: rx_x4_status2_aerMMDportSelect
 */
#define rx_x4_status2_aerMMDportSelect_ln0                 0
#define rx_x4_status2_aerMMDportSelect_ln1                 1
#define rx_x4_status2_aerMMDportSelect_ln2                 2
#define rx_x4_status2_aerMMDportSelect_ln3                 3
#define rx_x4_status2_aerMMDportSelect_BCST_ln0_1_2_3      511
#define rx_x4_status2_aerMMDportSelect_BCST_ln0_1          512
#define rx_x4_status2_aerMMDportSelect_BCST_ln2_3          513

/****************************************************************************
 * Enums: rx_x4_status2_firmwareModeSelect
 */
#define rx_x4_status2_firmwareModeSelect_DEFAULT           0
#define rx_x4_status2_firmwareModeSelect_SFP_OPT_LR        1
#define rx_x4_status2_firmwareModeSelect_SFP_DAC           2
#define rx_x4_status2_firmwareModeSelect_XLAUI             3
#define rx_x4_status2_firmwareModeSelect_LONG_CH_6G        4

/****************************************************************************
 * Enums: rx_x4_status2_tempIdxSelect
 */
#define rx_x4_status2_tempIdxSelect_LTE__22p9C             15
#define rx_x4_status2_tempIdxSelect_LTE__12p6C             14
#define rx_x4_status2_tempIdxSelect_LTE__3p0C              13
#define rx_x4_status2_tempIdxSelect_LTE_6p7C               12
#define rx_x4_status2_tempIdxSelect_LTE_16p4C              11
#define rx_x4_status2_tempIdxSelect_LTE_26p6C              10
#define rx_x4_status2_tempIdxSelect_LTE_36p3C              9
#define rx_x4_status2_tempIdxSelect_LTE_46p0C              8
#define rx_x4_status2_tempIdxSelect_LTE_56p2C              7
#define rx_x4_status2_tempIdxSelect_LTE_65p9C              6
#define rx_x4_status2_tempIdxSelect_LTE_75p6C              5
#define rx_x4_status2_tempIdxSelect_LTE_85p3C              4
#define rx_x4_status2_tempIdxSelect_LTE_95p5C              3
#define rx_x4_status2_tempIdxSelect_LTE_105p2C             2
#define rx_x4_status2_tempIdxSelect_LTE_114p9C             1
#define rx_x4_status2_tempIdxSelect_LTE_125p1C             0

/****************************************************************************
 * Enums: rx_x4_status2_port_mode
 */
#define rx_x4_status2_port_mode_QUAD_PORT                  0
#define rx_x4_status2_port_mode_TRI_1_PORT                 1
#define rx_x4_status2_port_mode_TRI_2_PORT                 2
#define rx_x4_status2_port_mode_DUAL_PORT                  3
#define rx_x4_status2_port_mode_SINGLE_PORT                4

/****************************************************************************
 * Enums: rx_x4_status2_rev_letter_enum
 */
#define rx_x4_status2_rev_letter_enum_REV_A                0
#define rx_x4_status2_rev_letter_enum_REV_B                1
#define rx_x4_status2_rev_letter_enum_REV_C                2
#define rx_x4_status2_rev_letter_enum_REV_D                3

/****************************************************************************
 * Enums: rx_x4_status2_rev_number_enum
 */
#define rx_x4_status2_rev_number_enum_REV_0                0
#define rx_x4_status2_rev_number_enum_REV_1                1
#define rx_x4_status2_rev_number_enum_REV_2                2
#define rx_x4_status2_rev_number_enum_REV_3                3
#define rx_x4_status2_rev_number_enum_REV_4                4
#define rx_x4_status2_rev_number_enum_REV_5                5
#define rx_x4_status2_rev_number_enum_REV_6                6
#define rx_x4_status2_rev_number_enum_REV_7                7

/****************************************************************************
 * Enums: rx_x4_status2_bonding_enum
 */
#define rx_x4_status2_bonding_enum_WIRE_BOND               0
#define rx_x4_status2_bonding_enum_FLIP_CHIP               1

/****************************************************************************
 * Enums: rx_x4_status2_tech_process
 */
#define rx_x4_status2_tech_process_PROCESS_90NM            0
#define rx_x4_status2_tech_process_PROCESS_65NM            1
#define rx_x4_status2_tech_process_PROCESS_40NM            2
#define rx_x4_status2_tech_process_PROCESS_28NM            3

/****************************************************************************
 * Enums: rx_x4_status2_model_num
 */
#define rx_x4_status2_model_num_SERDES_CL73                0
#define rx_x4_status2_model_num_XGXS_16G                   1
#define rx_x4_status2_model_num_HYPERCORE                  2
#define rx_x4_status2_model_num_HYPERLITE                  3
#define rx_x4_status2_model_num_PCIE_G2_PIPE               4
#define rx_x4_status2_model_num_SERDES_1p25GBd             5
#define rx_x4_status2_model_num_SATA2                      6
#define rx_x4_status2_model_num_QSGMII                     7
#define rx_x4_status2_model_num_XGXS10G                    8
#define rx_x4_status2_model_num_WARPCORE                   9
#define rx_x4_status2_model_num_XFICORE                    10
#define rx_x4_status2_model_num_RXFI                       11
#define rx_x4_status2_model_num_WARPLITE                   12
#define rx_x4_status2_model_num_PENTACORE                  13
#define rx_x4_status2_model_num_ESM                        14
#define rx_x4_status2_model_num_QUAD_SGMII                 15
#define rx_x4_status2_model_num_WARPCORE_3                 16
#define rx_x4_status2_model_num_TSC                        17
#define rx_x4_status2_model_num_TSC4E                      18
#define rx_x4_status2_model_num_TSC12E                     19
#define rx_x4_status2_model_num_TSC4F                      20
#define rx_x4_status2_model_num_XGXS_CL73_90NM             29
#define rx_x4_status2_model_num_SERDES_CL73_90NM           30
#define rx_x4_status2_model_num_WARPCORE3                  32
#define rx_x4_status2_model_num_WARPCORE4_TSC              33
#define rx_x4_status2_model_num_RXAUI                      34

/****************************************************************************
 * Enums: rx_x4_status2_payload
 */
#define rx_x4_status2_payload_REPEAT_2_BYTES               0
#define rx_x4_status2_payload_RAMPING                      1
#define rx_x4_status2_payload_CL48_CRPAT                   2
#define rx_x4_status2_payload_CL48_CJPAT                   3
#define rx_x4_status2_payload_CL36_LONG_CRPAT              4
#define rx_x4_status2_payload_CL36_SHORT_CRPAT             5

/****************************************************************************
 * Enums: rx_x4_status2_sc
 */
#define rx_x4_status2_sc_S_10G_CR1                         0
#define rx_x4_status2_sc_S_10G_KR1                         1
#define rx_x4_status2_sc_S_10G_X1                          2
#define rx_x4_status2_sc_S_10G_HG2_CR1                     4
#define rx_x4_status2_sc_S_10G_HG2_KR1                     5
#define rx_x4_status2_sc_S_10G_HG2_X1                      6
#define rx_x4_status2_sc_S_20G_CR1                         8
#define rx_x4_status2_sc_S_20G_KR1                         9
#define rx_x4_status2_sc_S_20G_X1                          10
#define rx_x4_status2_sc_S_20G_HG2_CR1                     12
#define rx_x4_status2_sc_S_20G_HG2_KR1                     13
#define rx_x4_status2_sc_S_20G_HG2_X1                      14
#define rx_x4_status2_sc_S_25G_CR1                         16
#define rx_x4_status2_sc_S_25G_KR1                         17
#define rx_x4_status2_sc_S_25G_X1                          18
#define rx_x4_status2_sc_S_25G_HG2_CR1                     20
#define rx_x4_status2_sc_S_25G_HG2_KR1                     21
#define rx_x4_status2_sc_S_25G_HG2_X1                      22
#define rx_x4_status2_sc_S_20G_CR2                         24
#define rx_x4_status2_sc_S_20G_KR2                         25
#define rx_x4_status2_sc_S_20G_X2                          26
#define rx_x4_status2_sc_S_20G_HG2_CR2                     28
#define rx_x4_status2_sc_S_20G_HG2_KR2                     29
#define rx_x4_status2_sc_S_20G_HG2_X2                      30
#define rx_x4_status2_sc_S_40G_CR2                         32
#define rx_x4_status2_sc_S_40G_KR2                         33
#define rx_x4_status2_sc_S_40G_X2                          34
#define rx_x4_status2_sc_S_40G_HG2_CR2                     36
#define rx_x4_status2_sc_S_40G_HG2_KR2                     37
#define rx_x4_status2_sc_S_40G_HG2_X2                      38
#define rx_x4_status2_sc_S_40G_CR4                         40
#define rx_x4_status2_sc_S_40G_KR4                         41
#define rx_x4_status2_sc_S_40G_X4                          42
#define rx_x4_status2_sc_S_40G_HG2_CR4                     44
#define rx_x4_status2_sc_S_40G_HG2_KR4                     45
#define rx_x4_status2_sc_S_40G_HG2_X4                      46
#define rx_x4_status2_sc_S_50G_CR2                         48
#define rx_x4_status2_sc_S_50G_KR2                         49
#define rx_x4_status2_sc_S_50G_X2                          50
#define rx_x4_status2_sc_S_50G_HG2_CR2                     52
#define rx_x4_status2_sc_S_50G_HG2_KR2                     53
#define rx_x4_status2_sc_S_50G_HG2_X2                      54
#define rx_x4_status2_sc_S_50G_CR4                         56
#define rx_x4_status2_sc_S_50G_KR4                         57
#define rx_x4_status2_sc_S_50G_X4                          58
#define rx_x4_status2_sc_S_50G_HG2_CR4                     60
#define rx_x4_status2_sc_S_50G_HG2_KR4                     61
#define rx_x4_status2_sc_S_50G_HG2_X4                      62
#define rx_x4_status2_sc_S_100G_CR4                        64
#define rx_x4_status2_sc_S_100G_KR4                        65
#define rx_x4_status2_sc_S_100G_X4                         66
#define rx_x4_status2_sc_S_100G_HG2_CR4                    68
#define rx_x4_status2_sc_S_100G_HG2_KR4                    69
#define rx_x4_status2_sc_S_100G_HG2_X4                     70
#define rx_x4_status2_sc_S_CL73_20GVCO                     72
#define rx_x4_status2_sc_S_CL73_25GVCO                     80
#define rx_x4_status2_sc_S_CL36_20GVCO                     88
#define rx_x4_status2_sc_S_CL36_25GVCO                     96

/****************************************************************************
 * Enums: rx_x4_status2_t_fifo_modes
 */
#define rx_x4_status2_t_fifo_modes_T_FIFO_MODE_BYPASS      0
#define rx_x4_status2_t_fifo_modes_T_FIFO_MODE_40G         1
#define rx_x4_status2_t_fifo_modes_T_FIFO_MODE_100G        2
#define rx_x4_status2_t_fifo_modes_T_FIFO_MODE_20G         3

/****************************************************************************
 * Enums: rx_x4_status2_t_enc_modes
 */
#define rx_x4_status2_t_enc_modes_T_ENC_MODE_BYPASS        0
#define rx_x4_status2_t_enc_modes_T_ENC_MODE_CL49          1
#define rx_x4_status2_t_enc_modes_T_ENC_MODE_CL82          2

/****************************************************************************
 * Enums: rx_x4_status2_btmx_mode
 */
#define rx_x4_status2_btmx_mode_BS_BTMX_MODE_1to1          0
#define rx_x4_status2_btmx_mode_BS_BTMX_MODE_2to1          1
#define rx_x4_status2_btmx_mode_BS_BTMX_MODE_5to1          2

/****************************************************************************
 * Enums: rx_x4_status2_t_type_cl82
 */
#define rx_x4_status2_t_type_cl82_T_TYPE_B1                5
#define rx_x4_status2_t_type_cl82_T_TYPE_C                 4
#define rx_x4_status2_t_type_cl82_T_TYPE_S                 3
#define rx_x4_status2_t_type_cl82_T_TYPE_T                 2
#define rx_x4_status2_t_type_cl82_T_TYPE_D                 1
#define rx_x4_status2_t_type_cl82_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x4_status2_txsm_state_cl82
 */
#define rx_x4_status2_txsm_state_cl82_TX_HIG_END           6
#define rx_x4_status2_txsm_state_cl82_TX_HIG_START         5
#define rx_x4_status2_txsm_state_cl82_TX_E                 4
#define rx_x4_status2_txsm_state_cl82_TX_T                 3
#define rx_x4_status2_txsm_state_cl82_TX_D                 2
#define rx_x4_status2_txsm_state_cl82_TX_C                 1
#define rx_x4_status2_txsm_state_cl82_TX_INIT              0

/****************************************************************************
 * Enums: rx_x4_status2_ltxsm_state_cl82
 */
#define rx_x4_status2_ltxsm_state_cl82_TX_HIG_END          64
#define rx_x4_status2_ltxsm_state_cl82_TX_HIG_START        32
#define rx_x4_status2_ltxsm_state_cl82_TX_E                16
#define rx_x4_status2_ltxsm_state_cl82_TX_T                8
#define rx_x4_status2_ltxsm_state_cl82_TX_D                4
#define rx_x4_status2_ltxsm_state_cl82_TX_C                2
#define rx_x4_status2_ltxsm_state_cl82_TX_INIT             1

/****************************************************************************
 * Enums: rx_x4_status2_r_type_coded_cl82
 */
#define rx_x4_status2_r_type_coded_cl82_R_TYPE_B1          32
#define rx_x4_status2_r_type_coded_cl82_R_TYPE_C           16
#define rx_x4_status2_r_type_coded_cl82_R_TYPE_S           8
#define rx_x4_status2_r_type_coded_cl82_R_TYPE_T           4
#define rx_x4_status2_r_type_coded_cl82_R_TYPE_D           2
#define rx_x4_status2_r_type_coded_cl82_R_TYPE_E           1

/****************************************************************************
 * Enums: rx_x4_status2_rxsm_state_cl82
 */
#define rx_x4_status2_rxsm_state_cl82_RX_HIG_END           64
#define rx_x4_status2_rxsm_state_cl82_RX_HIG_START         32
#define rx_x4_status2_rxsm_state_cl82_RX_E                 16
#define rx_x4_status2_rxsm_state_cl82_RX_T                 8
#define rx_x4_status2_rxsm_state_cl82_RX_D                 4
#define rx_x4_status2_rxsm_state_cl82_RX_C                 2
#define rx_x4_status2_rxsm_state_cl82_RX_INIT              1

/****************************************************************************
 * Enums: rx_x4_status2_deskew_state
 */
#define rx_x4_status2_deskew_state_ALIGN_ACQUIRED          2
#define rx_x4_status2_deskew_state_LOSS_OF_ALIGNMENT       1

/****************************************************************************
 * Enums: rx_x4_status2_os_mode_enum
 */
#define rx_x4_status2_os_mode_enum_OS_MODE_1               0
#define rx_x4_status2_os_mode_enum_OS_MODE_2               1
#define rx_x4_status2_os_mode_enum_OS_MODE_4               2
#define rx_x4_status2_os_mode_enum_OS_MODE_16p5            8
#define rx_x4_status2_os_mode_enum_OS_MODE_20p625          12

/****************************************************************************
 * Enums: rx_x4_status2_scr_modes
 */
#define rx_x4_status2_scr_modes_T_SCR_MODE_BYPASS          0
#define rx_x4_status2_scr_modes_T_SCR_MODE_CL49            1
#define rx_x4_status2_scr_modes_T_SCR_MODE_40G_2_LANE      2
#define rx_x4_status2_scr_modes_T_SCR_MODE_100G            3
#define rx_x4_status2_scr_modes_T_SCR_MODE_20G             4
#define rx_x4_status2_scr_modes_T_SCR_MODE_40G_4_LANE      5

/****************************************************************************
 * Enums: rx_x4_status2_descr_modes
 */
#define rx_x4_status2_descr_modes_R_DESCR_MODE_BYPASS      0
#define rx_x4_status2_descr_modes_R_DESCR_MODE_CL49        1
#define rx_x4_status2_descr_modes_R_DESCR_MODE_CL82        2

/****************************************************************************
 * Enums: rx_x4_status2_r_dec_tl_mode
 */
#define rx_x4_status2_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS   0
#define rx_x4_status2_r_dec_tl_mode_R_DEC_TL_MODE_CL49     1
#define rx_x4_status2_r_dec_tl_mode_R_DEC_TL_MODE_CL82     2

/****************************************************************************
 * Enums: rx_x4_status2_r_dec_fsm_mode
 */
#define rx_x4_status2_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define rx_x4_status2_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49   1
#define rx_x4_status2_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82   2

/****************************************************************************
 * Enums: rx_x4_status2_r_deskew_mode
 */
#define rx_x4_status2_r_deskew_mode_R_DESKEW_MODE_BYPASS   0
#define rx_x4_status2_r_deskew_mode_R_DESKEW_MODE_20G      1
#define rx_x4_status2_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define rx_x4_status2_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define rx_x4_status2_r_deskew_mode_R_DESKEW_MODE_100G     4
#define rx_x4_status2_r_deskew_mode_R_DESKEW_MODE_CL49     5
#define rx_x4_status2_r_deskew_mode_R_DESKEW_MODE_CL91     6

/****************************************************************************
 * Enums: rx_x4_status2_bs_dist_modes
 */
#define rx_x4_status2_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define rx_x4_status2_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define rx_x4_status2_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define rx_x4_status2_bs_dist_modes_BS_DIST_MODE_NO_TDM    3

/****************************************************************************
 * Enums: rx_x4_status2_cl49_t_type
 */
#define rx_x4_status2_cl49_t_type_BAD_T_TYPE               15
#define rx_x4_status2_cl49_t_type_T_TYPE_B0                11
#define rx_x4_status2_cl49_t_type_T_TYPE_OB                10
#define rx_x4_status2_cl49_t_type_T_TYPE_B1                9
#define rx_x4_status2_cl49_t_type_T_TYPE_DB                8
#define rx_x4_status2_cl49_t_type_T_TYPE_FC                7
#define rx_x4_status2_cl49_t_type_T_TYPE_TB                6
#define rx_x4_status2_cl49_t_type_T_TYPE_LI                5
#define rx_x4_status2_cl49_t_type_T_TYPE_C                 4
#define rx_x4_status2_cl49_t_type_T_TYPE_S                 3
#define rx_x4_status2_cl49_t_type_T_TYPE_T                 2
#define rx_x4_status2_cl49_t_type_T_TYPE_D                 1
#define rx_x4_status2_cl49_t_type_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x4_status2_cl49_txsm_states
 */
#define rx_x4_status2_cl49_txsm_states_TX_HIG_END          7
#define rx_x4_status2_cl49_txsm_states_TX_HIG_START        6
#define rx_x4_status2_cl49_txsm_states_TX_LI               5
#define rx_x4_status2_cl49_txsm_states_TX_E                4
#define rx_x4_status2_cl49_txsm_states_TX_T                3
#define rx_x4_status2_cl49_txsm_states_TX_D                2
#define rx_x4_status2_cl49_txsm_states_TX_C                1
#define rx_x4_status2_cl49_txsm_states_TX_INIT             0

/****************************************************************************
 * Enums: rx_x4_status2_cl49_ltxsm_states
 */
#define rx_x4_status2_cl49_ltxsm_states_TX_HIG_END         128
#define rx_x4_status2_cl49_ltxsm_states_TX_HIG_START       64
#define rx_x4_status2_cl49_ltxsm_states_TX_LI              32
#define rx_x4_status2_cl49_ltxsm_states_TX_E               16
#define rx_x4_status2_cl49_ltxsm_states_TX_T               8
#define rx_x4_status2_cl49_ltxsm_states_TX_D               4
#define rx_x4_status2_cl49_ltxsm_states_TX_C               2
#define rx_x4_status2_cl49_ltxsm_states_TX_INIT            1

/****************************************************************************
 * Enums: rx_x4_status2_burst_error_mode
 */
#define rx_x4_status2_burst_error_mode_BURST_ERROR_11_BITS 0
#define rx_x4_status2_burst_error_mode_BURST_ERROR_16_BITS 1
#define rx_x4_status2_burst_error_mode_BURST_ERROR_17_BITS 2
#define rx_x4_status2_burst_error_mode_BURST_ERROR_18_BITS 3
#define rx_x4_status2_burst_error_mode_BURST_ERROR_19_BITS 4
#define rx_x4_status2_burst_error_mode_BURST_ERROR_20_BITS 5
#define rx_x4_status2_burst_error_mode_BURST_ERROR_21_BITS 6
#define rx_x4_status2_burst_error_mode_BURST_ERROR_22_BITS 7
#define rx_x4_status2_burst_error_mode_BURST_ERROR_23_BITS 8
#define rx_x4_status2_burst_error_mode_BURST_ERROR_24_BITS 9
#define rx_x4_status2_burst_error_mode_BURST_ERROR_25_BITS 10
#define rx_x4_status2_burst_error_mode_BURST_ERROR_26_BITS 11
#define rx_x4_status2_burst_error_mode_BURST_ERROR_27_BITS 12
#define rx_x4_status2_burst_error_mode_BURST_ERROR_28_BITS 13
#define rx_x4_status2_burst_error_mode_BURST_ERROR_29_BITS 14
#define rx_x4_status2_burst_error_mode_BURST_ERROR_30_BITS 15
#define rx_x4_status2_burst_error_mode_BURST_ERROR_31_BITS 16
#define rx_x4_status2_burst_error_mode_BURST_ERROR_32_BITS 17
#define rx_x4_status2_burst_error_mode_BURST_ERROR_33_BITS 18
#define rx_x4_status2_burst_error_mode_BURST_ERROR_34_BITS 19
#define rx_x4_status2_burst_error_mode_BURST_ERROR_35_BITS 20
#define rx_x4_status2_burst_error_mode_BURST_ERROR_36_BITS 21
#define rx_x4_status2_burst_error_mode_BURST_ERROR_37_BITS 22
#define rx_x4_status2_burst_error_mode_BURST_ERROR_38_BITS 23
#define rx_x4_status2_burst_error_mode_BURST_ERROR_39_BITS 24
#define rx_x4_status2_burst_error_mode_BURST_ERROR_40_BITS 25
#define rx_x4_status2_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: rx_x4_status2_bermon_state
 */
#define rx_x4_status2_bermon_state_HI_BER                  4
#define rx_x4_status2_bermon_state_GOOD_BER                3
#define rx_x4_status2_bermon_state_BER_TEST_SH             2
#define rx_x4_status2_bermon_state_START_TIMER             1
#define rx_x4_status2_bermon_state_BER_MT_INIT             0

/****************************************************************************
 * Enums: rx_x4_status2_rxsm_state_cl49
 */
#define rx_x4_status2_rxsm_state_cl49_RX_HIG_END           128
#define rx_x4_status2_rxsm_state_cl49_RX_HIG_START         64
#define rx_x4_status2_rxsm_state_cl49_RX_LI                32
#define rx_x4_status2_rxsm_state_cl49_RX_E                 16
#define rx_x4_status2_rxsm_state_cl49_RX_T                 8
#define rx_x4_status2_rxsm_state_cl49_RX_D                 4
#define rx_x4_status2_rxsm_state_cl49_RX_C                 2
#define rx_x4_status2_rxsm_state_cl49_RX_INIT              1

/****************************************************************************
 * Enums: rx_x4_status2_r_type
 */
#define rx_x4_status2_r_type_BAD_R_TYPE                    15
#define rx_x4_status2_r_type_R_TYPE_B0                     11
#define rx_x4_status2_r_type_R_TYPE_OB                     10
#define rx_x4_status2_r_type_R_TYPE_B1                     9
#define rx_x4_status2_r_type_R_TYPE_DB                     8
#define rx_x4_status2_r_type_R_TYPE_FC                     7
#define rx_x4_status2_r_type_R_TYPE_TB                     6
#define rx_x4_status2_r_type_R_TYPE_LI                     5
#define rx_x4_status2_r_type_R_TYPE_C                      4
#define rx_x4_status2_r_type_R_TYPE_S                      3
#define rx_x4_status2_r_type_R_TYPE_T                      2
#define rx_x4_status2_r_type_R_TYPE_D                      1
#define rx_x4_status2_r_type_R_TYPE_E                      0

/****************************************************************************
 * Enums: rx_x4_status2_am_lock_state
 */
#define rx_x4_status2_am_lock_state_INVALID_AM             512
#define rx_x4_status2_am_lock_state_GOOD_AM                256
#define rx_x4_status2_am_lock_state_COMP_AM                128
#define rx_x4_status2_am_lock_state_TIMER_2                64
#define rx_x4_status2_am_lock_state_AM_2_GOOD              32
#define rx_x4_status2_am_lock_state_COMP_2ND               16
#define rx_x4_status2_am_lock_state_TIMER_1                8
#define rx_x4_status2_am_lock_state_FIND_1ST               4
#define rx_x4_status2_am_lock_state_AM_RESET_CNT           2
#define rx_x4_status2_am_lock_state_AM_LOCK_INIT           1

/****************************************************************************
 * Enums: rx_x4_status2_msg_selector
 */
#define rx_x4_status2_msg_selector_RESERVED                0
#define rx_x4_status2_msg_selector_VALUE_802p3             1
#define rx_x4_status2_msg_selector_VALUE_802p9             2
#define rx_x4_status2_msg_selector_VALUE_802p5             3
#define rx_x4_status2_msg_selector_VALUE_1394              4

/****************************************************************************
 * Enums: rx_x4_status2_synce_enum
 */
#define rx_x4_status2_synce_enum_SYNCE_NO_DIV              0
#define rx_x4_status2_synce_enum_SYNCE_DIV_7               1
#define rx_x4_status2_synce_enum_SYNCE_DIV_11              2

/****************************************************************************
 * Enums: rx_x4_status2_synce_enum_stage0
 */
#define rx_x4_status2_synce_enum_stage0_SYNCE_NO_DIV       0
#define rx_x4_status2_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define rx_x4_status2_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: rx_x4_status2_cl91_sync_state
 */
#define rx_x4_status2_cl91_sync_state_FIND_1ST             0
#define rx_x4_status2_cl91_sync_state_COUNT_NEXT           1
#define rx_x4_status2_cl91_sync_state_COMP_2ND             2
#define rx_x4_status2_cl91_sync_state_TWO_GOOD             3

/****************************************************************************
 * Enums: rx_x4_status2_cl91_algn_state
 */
#define rx_x4_status2_cl91_algn_state_LOSS_OF_ALIGNMENT    0
#define rx_x4_status2_cl91_algn_state_DESKEW               1
#define rx_x4_status2_cl91_algn_state_DESKEW_FAIL          2
#define rx_x4_status2_cl91_algn_state_ALIGN_ACQUIRED       3
#define rx_x4_status2_cl91_algn_state_CW_GOOD              4
#define rx_x4_status2_cl91_algn_state_CW_BAD               5
#define rx_x4_status2_cl91_algn_state_THREE_BAD            6

/****************************************************************************
 * Enums: rx_x4_status2_fec_sel
 */
#define rx_x4_status2_fec_sel_NO_FEC                       0
#define rx_x4_status2_fec_sel_FEC_CL74                     1
#define rx_x4_status2_fec_sel_FEC_CL91                     2

/****************************************************************************
 * Enums: rx_x4_status3_cl36TxEEEStates_l
 */
#define rx_x4_status3_cl36TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status3_cl36TxEEEStates_l_TX_QUIET           4
#define rx_x4_status3_cl36TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status3_cl36TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status3_cl36TxEEEStates_c
 */
#define rx_x4_status3_cl36TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status3_cl36TxEEEStates_c_TX_QUIET           2
#define rx_x4_status3_cl36TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status3_cl36TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status3_cl49TxEEEStates_l
 */
#define rx_x4_status3_cl49TxEEEStates_l_SCR_RESET_2        64
#define rx_x4_status3_cl49TxEEEStates_l_SCR_RESET_1        32
#define rx_x4_status3_cl49TxEEEStates_l_TX_WAKE            16
#define rx_x4_status3_cl49TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status3_cl49TxEEEStates_l_TX_QUIET           4
#define rx_x4_status3_cl49TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status3_cl49TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status3_fec_req_enum
 */
#define rx_x4_status3_fec_req_enum_FEC_not_supported       0
#define rx_x4_status3_fec_req_enum_FEC_supported_but_not_requested 1
#define rx_x4_status3_fec_req_enum_invalid_setting         2
#define rx_x4_status3_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: rx_x4_status3_cl73_pause_enum
 */
#define rx_x4_status3_cl73_pause_enum_No_PAUSE_ability     0
#define rx_x4_status3_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define rx_x4_status3_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define rx_x4_status3_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: rx_x4_status3_cl49TxEEEStates_c
 */
#define rx_x4_status3_cl49TxEEEStates_c_SCR_RESET_2        6
#define rx_x4_status3_cl49TxEEEStates_c_SCR_RESET_1        5
#define rx_x4_status3_cl49TxEEEStates_c_TX_WAKE            4
#define rx_x4_status3_cl49TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status3_cl49TxEEEStates_c_TX_QUIET           2
#define rx_x4_status3_cl49TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status3_cl49TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status3_cl36RxEEEStates_l
 */
#define rx_x4_status3_cl36RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status3_cl36RxEEEStates_l_RX_WTF             16
#define rx_x4_status3_cl36RxEEEStates_l_RX_WAKE            8
#define rx_x4_status3_cl36RxEEEStates_l_RX_QUIET           4
#define rx_x4_status3_cl36RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status3_cl36RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status3_cl36RxEEEStates_c
 */
#define rx_x4_status3_cl36RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status3_cl36RxEEEStates_c_RX_WTF             4
#define rx_x4_status3_cl36RxEEEStates_c_RX_WAKE            3
#define rx_x4_status3_cl36RxEEEStates_c_RX_QUIET           2
#define rx_x4_status3_cl36RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status3_cl36RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status3_cl49RxEEEStates_l
 */
#define rx_x4_status3_cl49RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status3_cl49RxEEEStates_l_RX_WTF             16
#define rx_x4_status3_cl49RxEEEStates_l_RX_WAKE            8
#define rx_x4_status3_cl49RxEEEStates_l_RX_QUIET           4
#define rx_x4_status3_cl49RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status3_cl49RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status3_cl49RxEEEStates_c
 */
#define rx_x4_status3_cl49RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status3_cl49RxEEEStates_c_RX_WTF             4
#define rx_x4_status3_cl49RxEEEStates_c_RX_WAKE            3
#define rx_x4_status3_cl49RxEEEStates_c_RX_QUIET           2
#define rx_x4_status3_cl49RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status3_cl49RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status3_cl48TxEEEStates_l
 */
#define rx_x4_status3_cl48TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status3_cl48TxEEEStates_l_TX_QUIET           4
#define rx_x4_status3_cl48TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status3_cl48TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status3_cl48TxEEEStates_c
 */
#define rx_x4_status3_cl48TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status3_cl48TxEEEStates_c_TX_QUIET           2
#define rx_x4_status3_cl48TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status3_cl48TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status3_cl48RxEEEStates_l
 */
#define rx_x4_status3_cl48RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status3_cl48RxEEEStates_l_RX_WAKE            16
#define rx_x4_status3_cl48RxEEEStates_l_RX_QUIET           8
#define rx_x4_status3_cl48RxEEEStates_l_RX_DEACT           4
#define rx_x4_status3_cl48RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status3_cl48RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status3_cl48RxEEEStates_c
 */
#define rx_x4_status3_cl48RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status3_cl48RxEEEStates_c_RX_WAKE            4
#define rx_x4_status3_cl48RxEEEStates_c_RX_QUIET           3
#define rx_x4_status3_cl48RxEEEStates_c_RX_DEACT           2
#define rx_x4_status3_cl48RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status3_cl48RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status3_IQP_Options
 */
#define rx_x4_status3_IQP_Options_i50uA                    0
#define rx_x4_status3_IQP_Options_i100uA                   1
#define rx_x4_status3_IQP_Options_i150uA                   2
#define rx_x4_status3_IQP_Options_i200uA                   3
#define rx_x4_status3_IQP_Options_i250uA                   4
#define rx_x4_status3_IQP_Options_i300uA                   5
#define rx_x4_status3_IQP_Options_i350uA                   6
#define rx_x4_status3_IQP_Options_i400uA                   7
#define rx_x4_status3_IQP_Options_i450uA                   8
#define rx_x4_status3_IQP_Options_i500uA                   9
#define rx_x4_status3_IQP_Options_i550uA                   10
#define rx_x4_status3_IQP_Options_i600uA                   11
#define rx_x4_status3_IQP_Options_i650uA                   12
#define rx_x4_status3_IQP_Options_i700uA                   13
#define rx_x4_status3_IQP_Options_i750uA                   14
#define rx_x4_status3_IQP_Options_i800uA                   15

/****************************************************************************
 * Enums: rx_x4_status3_IDriver_Options
 */
#define rx_x4_status3_IDriver_Options_v680mV               0
#define rx_x4_status3_IDriver_Options_v730mV               1
#define rx_x4_status3_IDriver_Options_v780mV               2
#define rx_x4_status3_IDriver_Options_v830mV               3
#define rx_x4_status3_IDriver_Options_v880mV               4
#define rx_x4_status3_IDriver_Options_v930mV               5
#define rx_x4_status3_IDriver_Options_v980mV               6
#define rx_x4_status3_IDriver_Options_v1010mV              7
#define rx_x4_status3_IDriver_Options_v1040mV              8
#define rx_x4_status3_IDriver_Options_v1060mV              9
#define rx_x4_status3_IDriver_Options_v1070mV              10
#define rx_x4_status3_IDriver_Options_v1080mV              11
#define rx_x4_status3_IDriver_Options_v1085mV              12
#define rx_x4_status3_IDriver_Options_v1090mV              13
#define rx_x4_status3_IDriver_Options_v1095mV              14
#define rx_x4_status3_IDriver_Options_v1100mV              15

/****************************************************************************
 * Enums: rx_x4_status3_operationModes
 */
#define rx_x4_status3_operationModes_XGXS                  0
#define rx_x4_status3_operationModes_XGXG_nCC              1
#define rx_x4_status3_operationModes_Indlane_OS8           4
#define rx_x4_status3_operationModes_IndLane_OS5           5
#define rx_x4_status3_operationModes_PCI                   7
#define rx_x4_status3_operationModes_XGXS_nLQ              8
#define rx_x4_status3_operationModes_XGXS_nLQnCC           9
#define rx_x4_status3_operationModes_PBypass               10
#define rx_x4_status3_operationModes_PBypass_nDSK          11
#define rx_x4_status3_operationModes_ComboCoreMode         12
#define rx_x4_status3_operationModes_Clocks_off            15

/****************************************************************************
 * Enums: rx_x4_status3_actualSpeeds
 */
#define rx_x4_status3_actualSpeeds_dr_10M                  0
#define rx_x4_status3_actualSpeeds_dr_100M                 1
#define rx_x4_status3_actualSpeeds_dr_1G                   2
#define rx_x4_status3_actualSpeeds_dr_2p5G                 3
#define rx_x4_status3_actualSpeeds_dr_5G_X4                4
#define rx_x4_status3_actualSpeeds_dr_6G_X4                5
#define rx_x4_status3_actualSpeeds_dr_10G_HiG              6
#define rx_x4_status3_actualSpeeds_dr_10G_CX4              7
#define rx_x4_status3_actualSpeeds_dr_12G_HiG              8
#define rx_x4_status3_actualSpeeds_dr_12p5G_X4             9
#define rx_x4_status3_actualSpeeds_dr_13G_X4               10
#define rx_x4_status3_actualSpeeds_dr_15G_X4               11
#define rx_x4_status3_actualSpeeds_dr_16G_X4               12
#define rx_x4_status3_actualSpeeds_dr_1G_KX                13
#define rx_x4_status3_actualSpeeds_dr_10G_KX4              14
#define rx_x4_status3_actualSpeeds_dr_10G_KR               15
#define rx_x4_status3_actualSpeeds_dr_5G                   16
#define rx_x4_status3_actualSpeeds_dr_6p4G                 17
#define rx_x4_status3_actualSpeeds_dr_20G_X4               18
#define rx_x4_status3_actualSpeeds_dr_21G_X4               19
#define rx_x4_status3_actualSpeeds_dr_25G_X4               20
#define rx_x4_status3_actualSpeeds_dr_10G_HiG_DXGXS        21
#define rx_x4_status3_actualSpeeds_dr_10G_DXGXS            22
#define rx_x4_status3_actualSpeeds_dr_10p5G_HiG_DXGXS      23
#define rx_x4_status3_actualSpeeds_dr_10p5G_DXGXS          24
#define rx_x4_status3_actualSpeeds_dr_12p773G_HiG_DXGXS    25
#define rx_x4_status3_actualSpeeds_dr_12p773G_DXGXS        26
#define rx_x4_status3_actualSpeeds_dr_10G_XFI              27
#define rx_x4_status3_actualSpeeds_dr_40G                  28
#define rx_x4_status3_actualSpeeds_dr_20G_HiG_DXGXS        29
#define rx_x4_status3_actualSpeeds_dr_20G_DXGXS            30
#define rx_x4_status3_actualSpeeds_dr_10G_SFI              31
#define rx_x4_status3_actualSpeeds_dr_31p5G                32
#define rx_x4_status3_actualSpeeds_dr_32p7G                33
#define rx_x4_status3_actualSpeeds_dr_20G_SCR              34
#define rx_x4_status3_actualSpeeds_dr_10G_HiG_DXGXS_SCR    35
#define rx_x4_status3_actualSpeeds_dr_10G_DXGXS_SCR        36
#define rx_x4_status3_actualSpeeds_dr_12G_R2               37
#define rx_x4_status3_actualSpeeds_dr_10G_X2               38
#define rx_x4_status3_actualSpeeds_dr_40G_KR4              39
#define rx_x4_status3_actualSpeeds_dr_40G_CR4              40
#define rx_x4_status3_actualSpeeds_dr_100G_CR10            41
#define rx_x4_status3_actualSpeeds_dr_15p75G_DXGXS         44
#define rx_x4_status3_actualSpeeds_dr_20G_KR2              57
#define rx_x4_status3_actualSpeeds_dr_20G_CR2              58

/****************************************************************************
 * Enums: rx_x4_status3_actualSpeedsMisc1
 */
#define rx_x4_status3_actualSpeedsMisc1_dr_2500BRCM_X1     16
#define rx_x4_status3_actualSpeedsMisc1_dr_5000BRCM_X4     17
#define rx_x4_status3_actualSpeedsMisc1_dr_6000BRCM_X4     18
#define rx_x4_status3_actualSpeedsMisc1_dr_10GHiGig_X4     19
#define rx_x4_status3_actualSpeedsMisc1_dr_10GBASE_CX4     20
#define rx_x4_status3_actualSpeedsMisc1_dr_12GHiGig_X4     21
#define rx_x4_status3_actualSpeedsMisc1_dr_12p5GHiGig_X4   22
#define rx_x4_status3_actualSpeedsMisc1_dr_13GHiGig_X4     23
#define rx_x4_status3_actualSpeedsMisc1_dr_15GHiGig_X4     24
#define rx_x4_status3_actualSpeedsMisc1_dr_16GHiGig_X4     25
#define rx_x4_status3_actualSpeedsMisc1_dr_5000BRCM_X1     26
#define rx_x4_status3_actualSpeedsMisc1_dr_6363BRCM_X1     27
#define rx_x4_status3_actualSpeedsMisc1_dr_20GHiGig_X4     28
#define rx_x4_status3_actualSpeedsMisc1_dr_21GHiGig_X4     29
#define rx_x4_status3_actualSpeedsMisc1_dr_25p45GHiGig_X4  30
#define rx_x4_status3_actualSpeedsMisc1_dr_10G_HiG_DXGXS   31

/****************************************************************************
 * Enums: rx_x4_status3_IndLaneModes
 */
#define rx_x4_status3_IndLaneModes_SWSDR_div2              0
#define rx_x4_status3_IndLaneModes_SWSDR_div1              1
#define rx_x4_status3_IndLaneModes_DWSDR_div2              2
#define rx_x4_status3_IndLaneModes_DWSDR_div1              3

/****************************************************************************
 * Enums: rx_x4_status3_prbsSelect
 */
#define rx_x4_status3_prbsSelect_prbs7                     0
#define rx_x4_status3_prbsSelect_prbs15                    1
#define rx_x4_status3_prbsSelect_prbs23                    2
#define rx_x4_status3_prbsSelect_prbs31                    3

/****************************************************************************
 * Enums: rx_x4_status3_vcoDivider
 */
#define rx_x4_status3_vcoDivider_div32                     0
#define rx_x4_status3_vcoDivider_div36                     1
#define rx_x4_status3_vcoDivider_div40                     2
#define rx_x4_status3_vcoDivider_div42                     3
#define rx_x4_status3_vcoDivider_div48                     4
#define rx_x4_status3_vcoDivider_div50                     5
#define rx_x4_status3_vcoDivider_div52                     6
#define rx_x4_status3_vcoDivider_div54                     7
#define rx_x4_status3_vcoDivider_div60                     8
#define rx_x4_status3_vcoDivider_div64                     9
#define rx_x4_status3_vcoDivider_div66                     10
#define rx_x4_status3_vcoDivider_div68                     11
#define rx_x4_status3_vcoDivider_div70                     12
#define rx_x4_status3_vcoDivider_div80                     13
#define rx_x4_status3_vcoDivider_div92                     14
#define rx_x4_status3_vcoDivider_div100                    15

/****************************************************************************
 * Enums: rx_x4_status3_refClkSelect
 */
#define rx_x4_status3_refClkSelect_clk_25MHz               0
#define rx_x4_status3_refClkSelect_clk_100MHz              1
#define rx_x4_status3_refClkSelect_clk_125MHz              2
#define rx_x4_status3_refClkSelect_clk_156p25MHz           3
#define rx_x4_status3_refClkSelect_clk_187p5MHz            4
#define rx_x4_status3_refClkSelect_clk_161p25Mhz           5
#define rx_x4_status3_refClkSelect_clk_50Mhz               6
#define rx_x4_status3_refClkSelect_clk_106p25Mhz           7

/****************************************************************************
 * Enums: rx_x4_status3_aerMMDdevTypeSelect
 */
#define rx_x4_status3_aerMMDdevTypeSelect_combo_core       0
#define rx_x4_status3_aerMMDdevTypeSelect_PMA_PMD          1
#define rx_x4_status3_aerMMDdevTypeSelect_PCS              3
#define rx_x4_status3_aerMMDdevTypeSelect_PHY              4
#define rx_x4_status3_aerMMDdevTypeSelect_DTE              5
#define rx_x4_status3_aerMMDdevTypeSelect_CL73_AN          7

/****************************************************************************
 * Enums: rx_x4_status3_aerMMDportSelect
 */
#define rx_x4_status3_aerMMDportSelect_ln0                 0
#define rx_x4_status3_aerMMDportSelect_ln1                 1
#define rx_x4_status3_aerMMDportSelect_ln2                 2
#define rx_x4_status3_aerMMDportSelect_ln3                 3
#define rx_x4_status3_aerMMDportSelect_BCST_ln0_1_2_3      511
#define rx_x4_status3_aerMMDportSelect_BCST_ln0_1          512
#define rx_x4_status3_aerMMDportSelect_BCST_ln2_3          513

/****************************************************************************
 * Enums: rx_x4_status3_firmwareModeSelect
 */
#define rx_x4_status3_firmwareModeSelect_DEFAULT           0
#define rx_x4_status3_firmwareModeSelect_SFP_OPT_LR        1
#define rx_x4_status3_firmwareModeSelect_SFP_DAC           2
#define rx_x4_status3_firmwareModeSelect_XLAUI             3
#define rx_x4_status3_firmwareModeSelect_LONG_CH_6G        4

/****************************************************************************
 * Enums: rx_x4_status3_tempIdxSelect
 */
#define rx_x4_status3_tempIdxSelect_LTE__22p9C             15
#define rx_x4_status3_tempIdxSelect_LTE__12p6C             14
#define rx_x4_status3_tempIdxSelect_LTE__3p0C              13
#define rx_x4_status3_tempIdxSelect_LTE_6p7C               12
#define rx_x4_status3_tempIdxSelect_LTE_16p4C              11
#define rx_x4_status3_tempIdxSelect_LTE_26p6C              10
#define rx_x4_status3_tempIdxSelect_LTE_36p3C              9
#define rx_x4_status3_tempIdxSelect_LTE_46p0C              8
#define rx_x4_status3_tempIdxSelect_LTE_56p2C              7
#define rx_x4_status3_tempIdxSelect_LTE_65p9C              6
#define rx_x4_status3_tempIdxSelect_LTE_75p6C              5
#define rx_x4_status3_tempIdxSelect_LTE_85p3C              4
#define rx_x4_status3_tempIdxSelect_LTE_95p5C              3
#define rx_x4_status3_tempIdxSelect_LTE_105p2C             2
#define rx_x4_status3_tempIdxSelect_LTE_114p9C             1
#define rx_x4_status3_tempIdxSelect_LTE_125p1C             0

/****************************************************************************
 * Enums: rx_x4_status3_port_mode
 */
#define rx_x4_status3_port_mode_QUAD_PORT                  0
#define rx_x4_status3_port_mode_TRI_1_PORT                 1
#define rx_x4_status3_port_mode_TRI_2_PORT                 2
#define rx_x4_status3_port_mode_DUAL_PORT                  3
#define rx_x4_status3_port_mode_SINGLE_PORT                4

/****************************************************************************
 * Enums: rx_x4_status3_rev_letter_enum
 */
#define rx_x4_status3_rev_letter_enum_REV_A                0
#define rx_x4_status3_rev_letter_enum_REV_B                1
#define rx_x4_status3_rev_letter_enum_REV_C                2
#define rx_x4_status3_rev_letter_enum_REV_D                3

/****************************************************************************
 * Enums: rx_x4_status3_rev_number_enum
 */
#define rx_x4_status3_rev_number_enum_REV_0                0
#define rx_x4_status3_rev_number_enum_REV_1                1
#define rx_x4_status3_rev_number_enum_REV_2                2
#define rx_x4_status3_rev_number_enum_REV_3                3
#define rx_x4_status3_rev_number_enum_REV_4                4
#define rx_x4_status3_rev_number_enum_REV_5                5
#define rx_x4_status3_rev_number_enum_REV_6                6
#define rx_x4_status3_rev_number_enum_REV_7                7

/****************************************************************************
 * Enums: rx_x4_status3_bonding_enum
 */
#define rx_x4_status3_bonding_enum_WIRE_BOND               0
#define rx_x4_status3_bonding_enum_FLIP_CHIP               1

/****************************************************************************
 * Enums: rx_x4_status3_tech_process
 */
#define rx_x4_status3_tech_process_PROCESS_90NM            0
#define rx_x4_status3_tech_process_PROCESS_65NM            1
#define rx_x4_status3_tech_process_PROCESS_40NM            2
#define rx_x4_status3_tech_process_PROCESS_28NM            3

/****************************************************************************
 * Enums: rx_x4_status3_model_num
 */
#define rx_x4_status3_model_num_SERDES_CL73                0
#define rx_x4_status3_model_num_XGXS_16G                   1
#define rx_x4_status3_model_num_HYPERCORE                  2
#define rx_x4_status3_model_num_HYPERLITE                  3
#define rx_x4_status3_model_num_PCIE_G2_PIPE               4
#define rx_x4_status3_model_num_SERDES_1p25GBd             5
#define rx_x4_status3_model_num_SATA2                      6
#define rx_x4_status3_model_num_QSGMII                     7
#define rx_x4_status3_model_num_XGXS10G                    8
#define rx_x4_status3_model_num_WARPCORE                   9
#define rx_x4_status3_model_num_XFICORE                    10
#define rx_x4_status3_model_num_RXFI                       11
#define rx_x4_status3_model_num_WARPLITE                   12
#define rx_x4_status3_model_num_PENTACORE                  13
#define rx_x4_status3_model_num_ESM                        14
#define rx_x4_status3_model_num_QUAD_SGMII                 15
#define rx_x4_status3_model_num_WARPCORE_3                 16
#define rx_x4_status3_model_num_TSC                        17
#define rx_x4_status3_model_num_TSC4E                      18
#define rx_x4_status3_model_num_TSC12E                     19
#define rx_x4_status3_model_num_TSC4F                      20
#define rx_x4_status3_model_num_XGXS_CL73_90NM             29
#define rx_x4_status3_model_num_SERDES_CL73_90NM           30
#define rx_x4_status3_model_num_WARPCORE3                  32
#define rx_x4_status3_model_num_WARPCORE4_TSC              33
#define rx_x4_status3_model_num_RXAUI                      34

/****************************************************************************
 * Enums: rx_x4_status3_payload
 */
#define rx_x4_status3_payload_REPEAT_2_BYTES               0
#define rx_x4_status3_payload_RAMPING                      1
#define rx_x4_status3_payload_CL48_CRPAT                   2
#define rx_x4_status3_payload_CL48_CJPAT                   3
#define rx_x4_status3_payload_CL36_LONG_CRPAT              4
#define rx_x4_status3_payload_CL36_SHORT_CRPAT             5

/****************************************************************************
 * Enums: rx_x4_status3_sc
 */
#define rx_x4_status3_sc_S_10G_CR1                         0
#define rx_x4_status3_sc_S_10G_KR1                         1
#define rx_x4_status3_sc_S_10G_X1                          2
#define rx_x4_status3_sc_S_10G_HG2_CR1                     4
#define rx_x4_status3_sc_S_10G_HG2_KR1                     5
#define rx_x4_status3_sc_S_10G_HG2_X1                      6
#define rx_x4_status3_sc_S_20G_CR1                         8
#define rx_x4_status3_sc_S_20G_KR1                         9
#define rx_x4_status3_sc_S_20G_X1                          10
#define rx_x4_status3_sc_S_20G_HG2_CR1                     12
#define rx_x4_status3_sc_S_20G_HG2_KR1                     13
#define rx_x4_status3_sc_S_20G_HG2_X1                      14
#define rx_x4_status3_sc_S_25G_CR1                         16
#define rx_x4_status3_sc_S_25G_KR1                         17
#define rx_x4_status3_sc_S_25G_X1                          18
#define rx_x4_status3_sc_S_25G_HG2_CR1                     20
#define rx_x4_status3_sc_S_25G_HG2_KR1                     21
#define rx_x4_status3_sc_S_25G_HG2_X1                      22
#define rx_x4_status3_sc_S_20G_CR2                         24
#define rx_x4_status3_sc_S_20G_KR2                         25
#define rx_x4_status3_sc_S_20G_X2                          26
#define rx_x4_status3_sc_S_20G_HG2_CR2                     28
#define rx_x4_status3_sc_S_20G_HG2_KR2                     29
#define rx_x4_status3_sc_S_20G_HG2_X2                      30
#define rx_x4_status3_sc_S_40G_CR2                         32
#define rx_x4_status3_sc_S_40G_KR2                         33
#define rx_x4_status3_sc_S_40G_X2                          34
#define rx_x4_status3_sc_S_40G_HG2_CR2                     36
#define rx_x4_status3_sc_S_40G_HG2_KR2                     37
#define rx_x4_status3_sc_S_40G_HG2_X2                      38
#define rx_x4_status3_sc_S_40G_CR4                         40
#define rx_x4_status3_sc_S_40G_KR4                         41
#define rx_x4_status3_sc_S_40G_X4                          42
#define rx_x4_status3_sc_S_40G_HG2_CR4                     44
#define rx_x4_status3_sc_S_40G_HG2_KR4                     45
#define rx_x4_status3_sc_S_40G_HG2_X4                      46
#define rx_x4_status3_sc_S_50G_CR2                         48
#define rx_x4_status3_sc_S_50G_KR2                         49
#define rx_x4_status3_sc_S_50G_X2                          50
#define rx_x4_status3_sc_S_50G_HG2_CR2                     52
#define rx_x4_status3_sc_S_50G_HG2_KR2                     53
#define rx_x4_status3_sc_S_50G_HG2_X2                      54
#define rx_x4_status3_sc_S_50G_CR4                         56
#define rx_x4_status3_sc_S_50G_KR4                         57
#define rx_x4_status3_sc_S_50G_X4                          58
#define rx_x4_status3_sc_S_50G_HG2_CR4                     60
#define rx_x4_status3_sc_S_50G_HG2_KR4                     61
#define rx_x4_status3_sc_S_50G_HG2_X4                      62
#define rx_x4_status3_sc_S_100G_CR4                        64
#define rx_x4_status3_sc_S_100G_KR4                        65
#define rx_x4_status3_sc_S_100G_X4                         66
#define rx_x4_status3_sc_S_100G_HG2_CR4                    68
#define rx_x4_status3_sc_S_100G_HG2_KR4                    69
#define rx_x4_status3_sc_S_100G_HG2_X4                     70
#define rx_x4_status3_sc_S_CL73_20GVCO                     72
#define rx_x4_status3_sc_S_CL73_25GVCO                     80
#define rx_x4_status3_sc_S_CL36_20GVCO                     88
#define rx_x4_status3_sc_S_CL36_25GVCO                     96

/****************************************************************************
 * Enums: rx_x4_status3_t_fifo_modes
 */
#define rx_x4_status3_t_fifo_modes_T_FIFO_MODE_BYPASS      0
#define rx_x4_status3_t_fifo_modes_T_FIFO_MODE_40G         1
#define rx_x4_status3_t_fifo_modes_T_FIFO_MODE_100G        2
#define rx_x4_status3_t_fifo_modes_T_FIFO_MODE_20G         3

/****************************************************************************
 * Enums: rx_x4_status3_t_enc_modes
 */
#define rx_x4_status3_t_enc_modes_T_ENC_MODE_BYPASS        0
#define rx_x4_status3_t_enc_modes_T_ENC_MODE_CL49          1
#define rx_x4_status3_t_enc_modes_T_ENC_MODE_CL82          2

/****************************************************************************
 * Enums: rx_x4_status3_btmx_mode
 */
#define rx_x4_status3_btmx_mode_BS_BTMX_MODE_1to1          0
#define rx_x4_status3_btmx_mode_BS_BTMX_MODE_2to1          1
#define rx_x4_status3_btmx_mode_BS_BTMX_MODE_5to1          2

/****************************************************************************
 * Enums: rx_x4_status3_t_type_cl82
 */
#define rx_x4_status3_t_type_cl82_T_TYPE_B1                5
#define rx_x4_status3_t_type_cl82_T_TYPE_C                 4
#define rx_x4_status3_t_type_cl82_T_TYPE_S                 3
#define rx_x4_status3_t_type_cl82_T_TYPE_T                 2
#define rx_x4_status3_t_type_cl82_T_TYPE_D                 1
#define rx_x4_status3_t_type_cl82_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x4_status3_txsm_state_cl82
 */
#define rx_x4_status3_txsm_state_cl82_TX_HIG_END           6
#define rx_x4_status3_txsm_state_cl82_TX_HIG_START         5
#define rx_x4_status3_txsm_state_cl82_TX_E                 4
#define rx_x4_status3_txsm_state_cl82_TX_T                 3
#define rx_x4_status3_txsm_state_cl82_TX_D                 2
#define rx_x4_status3_txsm_state_cl82_TX_C                 1
#define rx_x4_status3_txsm_state_cl82_TX_INIT              0

/****************************************************************************
 * Enums: rx_x4_status3_ltxsm_state_cl82
 */
#define rx_x4_status3_ltxsm_state_cl82_TX_HIG_END          64
#define rx_x4_status3_ltxsm_state_cl82_TX_HIG_START        32
#define rx_x4_status3_ltxsm_state_cl82_TX_E                16
#define rx_x4_status3_ltxsm_state_cl82_TX_T                8
#define rx_x4_status3_ltxsm_state_cl82_TX_D                4
#define rx_x4_status3_ltxsm_state_cl82_TX_C                2
#define rx_x4_status3_ltxsm_state_cl82_TX_INIT             1

/****************************************************************************
 * Enums: rx_x4_status3_r_type_coded_cl82
 */
#define rx_x4_status3_r_type_coded_cl82_R_TYPE_B1          32
#define rx_x4_status3_r_type_coded_cl82_R_TYPE_C           16
#define rx_x4_status3_r_type_coded_cl82_R_TYPE_S           8
#define rx_x4_status3_r_type_coded_cl82_R_TYPE_T           4
#define rx_x4_status3_r_type_coded_cl82_R_TYPE_D           2
#define rx_x4_status3_r_type_coded_cl82_R_TYPE_E           1

/****************************************************************************
 * Enums: rx_x4_status3_rxsm_state_cl82
 */
#define rx_x4_status3_rxsm_state_cl82_RX_HIG_END           64
#define rx_x4_status3_rxsm_state_cl82_RX_HIG_START         32
#define rx_x4_status3_rxsm_state_cl82_RX_E                 16
#define rx_x4_status3_rxsm_state_cl82_RX_T                 8
#define rx_x4_status3_rxsm_state_cl82_RX_D                 4
#define rx_x4_status3_rxsm_state_cl82_RX_C                 2
#define rx_x4_status3_rxsm_state_cl82_RX_INIT              1

/****************************************************************************
 * Enums: rx_x4_status3_deskew_state
 */
#define rx_x4_status3_deskew_state_ALIGN_ACQUIRED          2
#define rx_x4_status3_deskew_state_LOSS_OF_ALIGNMENT       1

/****************************************************************************
 * Enums: rx_x4_status3_os_mode_enum
 */
#define rx_x4_status3_os_mode_enum_OS_MODE_1               0
#define rx_x4_status3_os_mode_enum_OS_MODE_2               1
#define rx_x4_status3_os_mode_enum_OS_MODE_4               2
#define rx_x4_status3_os_mode_enum_OS_MODE_16p5            8
#define rx_x4_status3_os_mode_enum_OS_MODE_20p625          12

/****************************************************************************
 * Enums: rx_x4_status3_scr_modes
 */
#define rx_x4_status3_scr_modes_T_SCR_MODE_BYPASS          0
#define rx_x4_status3_scr_modes_T_SCR_MODE_CL49            1
#define rx_x4_status3_scr_modes_T_SCR_MODE_40G_2_LANE      2
#define rx_x4_status3_scr_modes_T_SCR_MODE_100G            3
#define rx_x4_status3_scr_modes_T_SCR_MODE_20G             4
#define rx_x4_status3_scr_modes_T_SCR_MODE_40G_4_LANE      5

/****************************************************************************
 * Enums: rx_x4_status3_descr_modes
 */
#define rx_x4_status3_descr_modes_R_DESCR_MODE_BYPASS      0
#define rx_x4_status3_descr_modes_R_DESCR_MODE_CL49        1
#define rx_x4_status3_descr_modes_R_DESCR_MODE_CL82        2

/****************************************************************************
 * Enums: rx_x4_status3_r_dec_tl_mode
 */
#define rx_x4_status3_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS   0
#define rx_x4_status3_r_dec_tl_mode_R_DEC_TL_MODE_CL49     1
#define rx_x4_status3_r_dec_tl_mode_R_DEC_TL_MODE_CL82     2

/****************************************************************************
 * Enums: rx_x4_status3_r_dec_fsm_mode
 */
#define rx_x4_status3_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define rx_x4_status3_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49   1
#define rx_x4_status3_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82   2

/****************************************************************************
 * Enums: rx_x4_status3_r_deskew_mode
 */
#define rx_x4_status3_r_deskew_mode_R_DESKEW_MODE_BYPASS   0
#define rx_x4_status3_r_deskew_mode_R_DESKEW_MODE_20G      1
#define rx_x4_status3_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define rx_x4_status3_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define rx_x4_status3_r_deskew_mode_R_DESKEW_MODE_100G     4
#define rx_x4_status3_r_deskew_mode_R_DESKEW_MODE_CL49     5
#define rx_x4_status3_r_deskew_mode_R_DESKEW_MODE_CL91     6

/****************************************************************************
 * Enums: rx_x4_status3_bs_dist_modes
 */
#define rx_x4_status3_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define rx_x4_status3_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define rx_x4_status3_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define rx_x4_status3_bs_dist_modes_BS_DIST_MODE_NO_TDM    3

/****************************************************************************
 * Enums: rx_x4_status3_cl49_t_type
 */
#define rx_x4_status3_cl49_t_type_BAD_T_TYPE               15
#define rx_x4_status3_cl49_t_type_T_TYPE_B0                11
#define rx_x4_status3_cl49_t_type_T_TYPE_OB                10
#define rx_x4_status3_cl49_t_type_T_TYPE_B1                9
#define rx_x4_status3_cl49_t_type_T_TYPE_DB                8
#define rx_x4_status3_cl49_t_type_T_TYPE_FC                7
#define rx_x4_status3_cl49_t_type_T_TYPE_TB                6
#define rx_x4_status3_cl49_t_type_T_TYPE_LI                5
#define rx_x4_status3_cl49_t_type_T_TYPE_C                 4
#define rx_x4_status3_cl49_t_type_T_TYPE_S                 3
#define rx_x4_status3_cl49_t_type_T_TYPE_T                 2
#define rx_x4_status3_cl49_t_type_T_TYPE_D                 1
#define rx_x4_status3_cl49_t_type_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x4_status3_cl49_txsm_states
 */
#define rx_x4_status3_cl49_txsm_states_TX_HIG_END          7
#define rx_x4_status3_cl49_txsm_states_TX_HIG_START        6
#define rx_x4_status3_cl49_txsm_states_TX_LI               5
#define rx_x4_status3_cl49_txsm_states_TX_E                4
#define rx_x4_status3_cl49_txsm_states_TX_T                3
#define rx_x4_status3_cl49_txsm_states_TX_D                2
#define rx_x4_status3_cl49_txsm_states_TX_C                1
#define rx_x4_status3_cl49_txsm_states_TX_INIT             0

/****************************************************************************
 * Enums: rx_x4_status3_cl49_ltxsm_states
 */
#define rx_x4_status3_cl49_ltxsm_states_TX_HIG_END         128
#define rx_x4_status3_cl49_ltxsm_states_TX_HIG_START       64
#define rx_x4_status3_cl49_ltxsm_states_TX_LI              32
#define rx_x4_status3_cl49_ltxsm_states_TX_E               16
#define rx_x4_status3_cl49_ltxsm_states_TX_T               8
#define rx_x4_status3_cl49_ltxsm_states_TX_D               4
#define rx_x4_status3_cl49_ltxsm_states_TX_C               2
#define rx_x4_status3_cl49_ltxsm_states_TX_INIT            1

/****************************************************************************
 * Enums: rx_x4_status3_burst_error_mode
 */
#define rx_x4_status3_burst_error_mode_BURST_ERROR_11_BITS 0
#define rx_x4_status3_burst_error_mode_BURST_ERROR_16_BITS 1
#define rx_x4_status3_burst_error_mode_BURST_ERROR_17_BITS 2
#define rx_x4_status3_burst_error_mode_BURST_ERROR_18_BITS 3
#define rx_x4_status3_burst_error_mode_BURST_ERROR_19_BITS 4
#define rx_x4_status3_burst_error_mode_BURST_ERROR_20_BITS 5
#define rx_x4_status3_burst_error_mode_BURST_ERROR_21_BITS 6
#define rx_x4_status3_burst_error_mode_BURST_ERROR_22_BITS 7
#define rx_x4_status3_burst_error_mode_BURST_ERROR_23_BITS 8
#define rx_x4_status3_burst_error_mode_BURST_ERROR_24_BITS 9
#define rx_x4_status3_burst_error_mode_BURST_ERROR_25_BITS 10
#define rx_x4_status3_burst_error_mode_BURST_ERROR_26_BITS 11
#define rx_x4_status3_burst_error_mode_BURST_ERROR_27_BITS 12
#define rx_x4_status3_burst_error_mode_BURST_ERROR_28_BITS 13
#define rx_x4_status3_burst_error_mode_BURST_ERROR_29_BITS 14
#define rx_x4_status3_burst_error_mode_BURST_ERROR_30_BITS 15
#define rx_x4_status3_burst_error_mode_BURST_ERROR_31_BITS 16
#define rx_x4_status3_burst_error_mode_BURST_ERROR_32_BITS 17
#define rx_x4_status3_burst_error_mode_BURST_ERROR_33_BITS 18
#define rx_x4_status3_burst_error_mode_BURST_ERROR_34_BITS 19
#define rx_x4_status3_burst_error_mode_BURST_ERROR_35_BITS 20
#define rx_x4_status3_burst_error_mode_BURST_ERROR_36_BITS 21
#define rx_x4_status3_burst_error_mode_BURST_ERROR_37_BITS 22
#define rx_x4_status3_burst_error_mode_BURST_ERROR_38_BITS 23
#define rx_x4_status3_burst_error_mode_BURST_ERROR_39_BITS 24
#define rx_x4_status3_burst_error_mode_BURST_ERROR_40_BITS 25
#define rx_x4_status3_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: rx_x4_status3_bermon_state
 */
#define rx_x4_status3_bermon_state_HI_BER                  4
#define rx_x4_status3_bermon_state_GOOD_BER                3
#define rx_x4_status3_bermon_state_BER_TEST_SH             2
#define rx_x4_status3_bermon_state_START_TIMER             1
#define rx_x4_status3_bermon_state_BER_MT_INIT             0

/****************************************************************************
 * Enums: rx_x4_status3_rxsm_state_cl49
 */
#define rx_x4_status3_rxsm_state_cl49_RX_HIG_END           128
#define rx_x4_status3_rxsm_state_cl49_RX_HIG_START         64
#define rx_x4_status3_rxsm_state_cl49_RX_LI                32
#define rx_x4_status3_rxsm_state_cl49_RX_E                 16
#define rx_x4_status3_rxsm_state_cl49_RX_T                 8
#define rx_x4_status3_rxsm_state_cl49_RX_D                 4
#define rx_x4_status3_rxsm_state_cl49_RX_C                 2
#define rx_x4_status3_rxsm_state_cl49_RX_INIT              1

/****************************************************************************
 * Enums: rx_x4_status3_r_type
 */
#define rx_x4_status3_r_type_BAD_R_TYPE                    15
#define rx_x4_status3_r_type_R_TYPE_B0                     11
#define rx_x4_status3_r_type_R_TYPE_OB                     10
#define rx_x4_status3_r_type_R_TYPE_B1                     9
#define rx_x4_status3_r_type_R_TYPE_DB                     8
#define rx_x4_status3_r_type_R_TYPE_FC                     7
#define rx_x4_status3_r_type_R_TYPE_TB                     6
#define rx_x4_status3_r_type_R_TYPE_LI                     5
#define rx_x4_status3_r_type_R_TYPE_C                      4
#define rx_x4_status3_r_type_R_TYPE_S                      3
#define rx_x4_status3_r_type_R_TYPE_T                      2
#define rx_x4_status3_r_type_R_TYPE_D                      1
#define rx_x4_status3_r_type_R_TYPE_E                      0

/****************************************************************************
 * Enums: rx_x4_status3_am_lock_state
 */
#define rx_x4_status3_am_lock_state_INVALID_AM             512
#define rx_x4_status3_am_lock_state_GOOD_AM                256
#define rx_x4_status3_am_lock_state_COMP_AM                128
#define rx_x4_status3_am_lock_state_TIMER_2                64
#define rx_x4_status3_am_lock_state_AM_2_GOOD              32
#define rx_x4_status3_am_lock_state_COMP_2ND               16
#define rx_x4_status3_am_lock_state_TIMER_1                8
#define rx_x4_status3_am_lock_state_FIND_1ST               4
#define rx_x4_status3_am_lock_state_AM_RESET_CNT           2
#define rx_x4_status3_am_lock_state_AM_LOCK_INIT           1

/****************************************************************************
 * Enums: rx_x4_status3_msg_selector
 */
#define rx_x4_status3_msg_selector_RESERVED                0
#define rx_x4_status3_msg_selector_VALUE_802p3             1
#define rx_x4_status3_msg_selector_VALUE_802p9             2
#define rx_x4_status3_msg_selector_VALUE_802p5             3
#define rx_x4_status3_msg_selector_VALUE_1394              4

/****************************************************************************
 * Enums: rx_x4_status3_synce_enum
 */
#define rx_x4_status3_synce_enum_SYNCE_NO_DIV              0
#define rx_x4_status3_synce_enum_SYNCE_DIV_7               1
#define rx_x4_status3_synce_enum_SYNCE_DIV_11              2

/****************************************************************************
 * Enums: rx_x4_status3_synce_enum_stage0
 */
#define rx_x4_status3_synce_enum_stage0_SYNCE_NO_DIV       0
#define rx_x4_status3_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define rx_x4_status3_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: rx_x4_status3_cl91_sync_state
 */
#define rx_x4_status3_cl91_sync_state_FIND_1ST             0
#define rx_x4_status3_cl91_sync_state_COUNT_NEXT           1
#define rx_x4_status3_cl91_sync_state_COMP_2ND             2
#define rx_x4_status3_cl91_sync_state_TWO_GOOD             3

/****************************************************************************
 * Enums: rx_x4_status3_cl91_algn_state
 */
#define rx_x4_status3_cl91_algn_state_LOSS_OF_ALIGNMENT    0
#define rx_x4_status3_cl91_algn_state_DESKEW               1
#define rx_x4_status3_cl91_algn_state_DESKEW_FAIL          2
#define rx_x4_status3_cl91_algn_state_ALIGN_ACQUIRED       3
#define rx_x4_status3_cl91_algn_state_CW_GOOD              4
#define rx_x4_status3_cl91_algn_state_CW_BAD               5
#define rx_x4_status3_cl91_algn_state_THREE_BAD            6

/****************************************************************************
 * Enums: rx_x4_status3_fec_sel
 */
#define rx_x4_status3_fec_sel_NO_FEC                       0
#define rx_x4_status3_fec_sel_FEC_CL74                     1
#define rx_x4_status3_fec_sel_FEC_CL91                     2

/****************************************************************************
 * Enums: rx_x4_status4_cl36TxEEEStates_l
 */
#define rx_x4_status4_cl36TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status4_cl36TxEEEStates_l_TX_QUIET           4
#define rx_x4_status4_cl36TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status4_cl36TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status4_cl36TxEEEStates_c
 */
#define rx_x4_status4_cl36TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status4_cl36TxEEEStates_c_TX_QUIET           2
#define rx_x4_status4_cl36TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status4_cl36TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status4_cl49TxEEEStates_l
 */
#define rx_x4_status4_cl49TxEEEStates_l_SCR_RESET_2        64
#define rx_x4_status4_cl49TxEEEStates_l_SCR_RESET_1        32
#define rx_x4_status4_cl49TxEEEStates_l_TX_WAKE            16
#define rx_x4_status4_cl49TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status4_cl49TxEEEStates_l_TX_QUIET           4
#define rx_x4_status4_cl49TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status4_cl49TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status4_fec_req_enum
 */
#define rx_x4_status4_fec_req_enum_FEC_not_supported       0
#define rx_x4_status4_fec_req_enum_FEC_supported_but_not_requested 1
#define rx_x4_status4_fec_req_enum_invalid_setting         2
#define rx_x4_status4_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: rx_x4_status4_cl73_pause_enum
 */
#define rx_x4_status4_cl73_pause_enum_No_PAUSE_ability     0
#define rx_x4_status4_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define rx_x4_status4_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define rx_x4_status4_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: rx_x4_status4_cl49TxEEEStates_c
 */
#define rx_x4_status4_cl49TxEEEStates_c_SCR_RESET_2        6
#define rx_x4_status4_cl49TxEEEStates_c_SCR_RESET_1        5
#define rx_x4_status4_cl49TxEEEStates_c_TX_WAKE            4
#define rx_x4_status4_cl49TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status4_cl49TxEEEStates_c_TX_QUIET           2
#define rx_x4_status4_cl49TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status4_cl49TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status4_cl36RxEEEStates_l
 */
#define rx_x4_status4_cl36RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status4_cl36RxEEEStates_l_RX_WTF             16
#define rx_x4_status4_cl36RxEEEStates_l_RX_WAKE            8
#define rx_x4_status4_cl36RxEEEStates_l_RX_QUIET           4
#define rx_x4_status4_cl36RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status4_cl36RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status4_cl36RxEEEStates_c
 */
#define rx_x4_status4_cl36RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status4_cl36RxEEEStates_c_RX_WTF             4
#define rx_x4_status4_cl36RxEEEStates_c_RX_WAKE            3
#define rx_x4_status4_cl36RxEEEStates_c_RX_QUIET           2
#define rx_x4_status4_cl36RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status4_cl36RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status4_cl49RxEEEStates_l
 */
#define rx_x4_status4_cl49RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status4_cl49RxEEEStates_l_RX_WTF             16
#define rx_x4_status4_cl49RxEEEStates_l_RX_WAKE            8
#define rx_x4_status4_cl49RxEEEStates_l_RX_QUIET           4
#define rx_x4_status4_cl49RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status4_cl49RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status4_cl49RxEEEStates_c
 */
#define rx_x4_status4_cl49RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status4_cl49RxEEEStates_c_RX_WTF             4
#define rx_x4_status4_cl49RxEEEStates_c_RX_WAKE            3
#define rx_x4_status4_cl49RxEEEStates_c_RX_QUIET           2
#define rx_x4_status4_cl49RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status4_cl49RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status4_cl48TxEEEStates_l
 */
#define rx_x4_status4_cl48TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status4_cl48TxEEEStates_l_TX_QUIET           4
#define rx_x4_status4_cl48TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status4_cl48TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status4_cl48TxEEEStates_c
 */
#define rx_x4_status4_cl48TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status4_cl48TxEEEStates_c_TX_QUIET           2
#define rx_x4_status4_cl48TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status4_cl48TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status4_cl48RxEEEStates_l
 */
#define rx_x4_status4_cl48RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status4_cl48RxEEEStates_l_RX_WAKE            16
#define rx_x4_status4_cl48RxEEEStates_l_RX_QUIET           8
#define rx_x4_status4_cl48RxEEEStates_l_RX_DEACT           4
#define rx_x4_status4_cl48RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status4_cl48RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status4_cl48RxEEEStates_c
 */
#define rx_x4_status4_cl48RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status4_cl48RxEEEStates_c_RX_WAKE            4
#define rx_x4_status4_cl48RxEEEStates_c_RX_QUIET           3
#define rx_x4_status4_cl48RxEEEStates_c_RX_DEACT           2
#define rx_x4_status4_cl48RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status4_cl48RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status4_IQP_Options
 */
#define rx_x4_status4_IQP_Options_i50uA                    0
#define rx_x4_status4_IQP_Options_i100uA                   1
#define rx_x4_status4_IQP_Options_i150uA                   2
#define rx_x4_status4_IQP_Options_i200uA                   3
#define rx_x4_status4_IQP_Options_i250uA                   4
#define rx_x4_status4_IQP_Options_i300uA                   5
#define rx_x4_status4_IQP_Options_i350uA                   6
#define rx_x4_status4_IQP_Options_i400uA                   7
#define rx_x4_status4_IQP_Options_i450uA                   8
#define rx_x4_status4_IQP_Options_i500uA                   9
#define rx_x4_status4_IQP_Options_i550uA                   10
#define rx_x4_status4_IQP_Options_i600uA                   11
#define rx_x4_status4_IQP_Options_i650uA                   12
#define rx_x4_status4_IQP_Options_i700uA                   13
#define rx_x4_status4_IQP_Options_i750uA                   14
#define rx_x4_status4_IQP_Options_i800uA                   15

/****************************************************************************
 * Enums: rx_x4_status4_IDriver_Options
 */
#define rx_x4_status4_IDriver_Options_v680mV               0
#define rx_x4_status4_IDriver_Options_v730mV               1
#define rx_x4_status4_IDriver_Options_v780mV               2
#define rx_x4_status4_IDriver_Options_v830mV               3
#define rx_x4_status4_IDriver_Options_v880mV               4
#define rx_x4_status4_IDriver_Options_v930mV               5
#define rx_x4_status4_IDriver_Options_v980mV               6
#define rx_x4_status4_IDriver_Options_v1010mV              7
#define rx_x4_status4_IDriver_Options_v1040mV              8
#define rx_x4_status4_IDriver_Options_v1060mV              9
#define rx_x4_status4_IDriver_Options_v1070mV              10
#define rx_x4_status4_IDriver_Options_v1080mV              11
#define rx_x4_status4_IDriver_Options_v1085mV              12
#define rx_x4_status4_IDriver_Options_v1090mV              13
#define rx_x4_status4_IDriver_Options_v1095mV              14
#define rx_x4_status4_IDriver_Options_v1100mV              15

/****************************************************************************
 * Enums: rx_x4_status4_operationModes
 */
#define rx_x4_status4_operationModes_XGXS                  0
#define rx_x4_status4_operationModes_XGXG_nCC              1
#define rx_x4_status4_operationModes_Indlane_OS8           4
#define rx_x4_status4_operationModes_IndLane_OS5           5
#define rx_x4_status4_operationModes_PCI                   7
#define rx_x4_status4_operationModes_XGXS_nLQ              8
#define rx_x4_status4_operationModes_XGXS_nLQnCC           9
#define rx_x4_status4_operationModes_PBypass               10
#define rx_x4_status4_operationModes_PBypass_nDSK          11
#define rx_x4_status4_operationModes_ComboCoreMode         12
#define rx_x4_status4_operationModes_Clocks_off            15

/****************************************************************************
 * Enums: rx_x4_status4_actualSpeeds
 */
#define rx_x4_status4_actualSpeeds_dr_10M                  0
#define rx_x4_status4_actualSpeeds_dr_100M                 1
#define rx_x4_status4_actualSpeeds_dr_1G                   2
#define rx_x4_status4_actualSpeeds_dr_2p5G                 3
#define rx_x4_status4_actualSpeeds_dr_5G_X4                4
#define rx_x4_status4_actualSpeeds_dr_6G_X4                5
#define rx_x4_status4_actualSpeeds_dr_10G_HiG              6
#define rx_x4_status4_actualSpeeds_dr_10G_CX4              7
#define rx_x4_status4_actualSpeeds_dr_12G_HiG              8
#define rx_x4_status4_actualSpeeds_dr_12p5G_X4             9
#define rx_x4_status4_actualSpeeds_dr_13G_X4               10
#define rx_x4_status4_actualSpeeds_dr_15G_X4               11
#define rx_x4_status4_actualSpeeds_dr_16G_X4               12
#define rx_x4_status4_actualSpeeds_dr_1G_KX                13
#define rx_x4_status4_actualSpeeds_dr_10G_KX4              14
#define rx_x4_status4_actualSpeeds_dr_10G_KR               15
#define rx_x4_status4_actualSpeeds_dr_5G                   16
#define rx_x4_status4_actualSpeeds_dr_6p4G                 17
#define rx_x4_status4_actualSpeeds_dr_20G_X4               18
#define rx_x4_status4_actualSpeeds_dr_21G_X4               19
#define rx_x4_status4_actualSpeeds_dr_25G_X4               20
#define rx_x4_status4_actualSpeeds_dr_10G_HiG_DXGXS        21
#define rx_x4_status4_actualSpeeds_dr_10G_DXGXS            22
#define rx_x4_status4_actualSpeeds_dr_10p5G_HiG_DXGXS      23
#define rx_x4_status4_actualSpeeds_dr_10p5G_DXGXS          24
#define rx_x4_status4_actualSpeeds_dr_12p773G_HiG_DXGXS    25
#define rx_x4_status4_actualSpeeds_dr_12p773G_DXGXS        26
#define rx_x4_status4_actualSpeeds_dr_10G_XFI              27
#define rx_x4_status4_actualSpeeds_dr_40G                  28
#define rx_x4_status4_actualSpeeds_dr_20G_HiG_DXGXS        29
#define rx_x4_status4_actualSpeeds_dr_20G_DXGXS            30
#define rx_x4_status4_actualSpeeds_dr_10G_SFI              31
#define rx_x4_status4_actualSpeeds_dr_31p5G                32
#define rx_x4_status4_actualSpeeds_dr_32p7G                33
#define rx_x4_status4_actualSpeeds_dr_20G_SCR              34
#define rx_x4_status4_actualSpeeds_dr_10G_HiG_DXGXS_SCR    35
#define rx_x4_status4_actualSpeeds_dr_10G_DXGXS_SCR        36
#define rx_x4_status4_actualSpeeds_dr_12G_R2               37
#define rx_x4_status4_actualSpeeds_dr_10G_X2               38
#define rx_x4_status4_actualSpeeds_dr_40G_KR4              39
#define rx_x4_status4_actualSpeeds_dr_40G_CR4              40
#define rx_x4_status4_actualSpeeds_dr_100G_CR10            41
#define rx_x4_status4_actualSpeeds_dr_15p75G_DXGXS         44
#define rx_x4_status4_actualSpeeds_dr_20G_KR2              57
#define rx_x4_status4_actualSpeeds_dr_20G_CR2              58

/****************************************************************************
 * Enums: rx_x4_status4_actualSpeedsMisc1
 */
#define rx_x4_status4_actualSpeedsMisc1_dr_2500BRCM_X1     16
#define rx_x4_status4_actualSpeedsMisc1_dr_5000BRCM_X4     17
#define rx_x4_status4_actualSpeedsMisc1_dr_6000BRCM_X4     18
#define rx_x4_status4_actualSpeedsMisc1_dr_10GHiGig_X4     19
#define rx_x4_status4_actualSpeedsMisc1_dr_10GBASE_CX4     20
#define rx_x4_status4_actualSpeedsMisc1_dr_12GHiGig_X4     21
#define rx_x4_status4_actualSpeedsMisc1_dr_12p5GHiGig_X4   22
#define rx_x4_status4_actualSpeedsMisc1_dr_13GHiGig_X4     23
#define rx_x4_status4_actualSpeedsMisc1_dr_15GHiGig_X4     24
#define rx_x4_status4_actualSpeedsMisc1_dr_16GHiGig_X4     25
#define rx_x4_status4_actualSpeedsMisc1_dr_5000BRCM_X1     26
#define rx_x4_status4_actualSpeedsMisc1_dr_6363BRCM_X1     27
#define rx_x4_status4_actualSpeedsMisc1_dr_20GHiGig_X4     28
#define rx_x4_status4_actualSpeedsMisc1_dr_21GHiGig_X4     29
#define rx_x4_status4_actualSpeedsMisc1_dr_25p45GHiGig_X4  30
#define rx_x4_status4_actualSpeedsMisc1_dr_10G_HiG_DXGXS   31

/****************************************************************************
 * Enums: rx_x4_status4_IndLaneModes
 */
#define rx_x4_status4_IndLaneModes_SWSDR_div2              0
#define rx_x4_status4_IndLaneModes_SWSDR_div1              1
#define rx_x4_status4_IndLaneModes_DWSDR_div2              2
#define rx_x4_status4_IndLaneModes_DWSDR_div1              3

/****************************************************************************
 * Enums: rx_x4_status4_prbsSelect
 */
#define rx_x4_status4_prbsSelect_prbs7                     0
#define rx_x4_status4_prbsSelect_prbs15                    1
#define rx_x4_status4_prbsSelect_prbs23                    2
#define rx_x4_status4_prbsSelect_prbs31                    3

/****************************************************************************
 * Enums: rx_x4_status4_vcoDivider
 */
#define rx_x4_status4_vcoDivider_div32                     0
#define rx_x4_status4_vcoDivider_div36                     1
#define rx_x4_status4_vcoDivider_div40                     2
#define rx_x4_status4_vcoDivider_div42                     3
#define rx_x4_status4_vcoDivider_div48                     4
#define rx_x4_status4_vcoDivider_div50                     5
#define rx_x4_status4_vcoDivider_div52                     6
#define rx_x4_status4_vcoDivider_div54                     7
#define rx_x4_status4_vcoDivider_div60                     8
#define rx_x4_status4_vcoDivider_div64                     9
#define rx_x4_status4_vcoDivider_div66                     10
#define rx_x4_status4_vcoDivider_div68                     11
#define rx_x4_status4_vcoDivider_div70                     12
#define rx_x4_status4_vcoDivider_div80                     13
#define rx_x4_status4_vcoDivider_div92                     14
#define rx_x4_status4_vcoDivider_div100                    15

/****************************************************************************
 * Enums: rx_x4_status4_refClkSelect
 */
#define rx_x4_status4_refClkSelect_clk_25MHz               0
#define rx_x4_status4_refClkSelect_clk_100MHz              1
#define rx_x4_status4_refClkSelect_clk_125MHz              2
#define rx_x4_status4_refClkSelect_clk_156p25MHz           3
#define rx_x4_status4_refClkSelect_clk_187p5MHz            4
#define rx_x4_status4_refClkSelect_clk_161p25Mhz           5
#define rx_x4_status4_refClkSelect_clk_50Mhz               6
#define rx_x4_status4_refClkSelect_clk_106p25Mhz           7

/****************************************************************************
 * Enums: rx_x4_status4_aerMMDdevTypeSelect
 */
#define rx_x4_status4_aerMMDdevTypeSelect_combo_core       0
#define rx_x4_status4_aerMMDdevTypeSelect_PMA_PMD          1
#define rx_x4_status4_aerMMDdevTypeSelect_PCS              3
#define rx_x4_status4_aerMMDdevTypeSelect_PHY              4
#define rx_x4_status4_aerMMDdevTypeSelect_DTE              5
#define rx_x4_status4_aerMMDdevTypeSelect_CL73_AN          7

/****************************************************************************
 * Enums: rx_x4_status4_aerMMDportSelect
 */
#define rx_x4_status4_aerMMDportSelect_ln0                 0
#define rx_x4_status4_aerMMDportSelect_ln1                 1
#define rx_x4_status4_aerMMDportSelect_ln2                 2
#define rx_x4_status4_aerMMDportSelect_ln3                 3
#define rx_x4_status4_aerMMDportSelect_BCST_ln0_1_2_3      511
#define rx_x4_status4_aerMMDportSelect_BCST_ln0_1          512
#define rx_x4_status4_aerMMDportSelect_BCST_ln2_3          513

/****************************************************************************
 * Enums: rx_x4_status4_firmwareModeSelect
 */
#define rx_x4_status4_firmwareModeSelect_DEFAULT           0
#define rx_x4_status4_firmwareModeSelect_SFP_OPT_LR        1
#define rx_x4_status4_firmwareModeSelect_SFP_DAC           2
#define rx_x4_status4_firmwareModeSelect_XLAUI             3
#define rx_x4_status4_firmwareModeSelect_LONG_CH_6G        4

/****************************************************************************
 * Enums: rx_x4_status4_tempIdxSelect
 */
#define rx_x4_status4_tempIdxSelect_LTE__22p9C             15
#define rx_x4_status4_tempIdxSelect_LTE__12p6C             14
#define rx_x4_status4_tempIdxSelect_LTE__3p0C              13
#define rx_x4_status4_tempIdxSelect_LTE_6p7C               12
#define rx_x4_status4_tempIdxSelect_LTE_16p4C              11
#define rx_x4_status4_tempIdxSelect_LTE_26p6C              10
#define rx_x4_status4_tempIdxSelect_LTE_36p3C              9
#define rx_x4_status4_tempIdxSelect_LTE_46p0C              8
#define rx_x4_status4_tempIdxSelect_LTE_56p2C              7
#define rx_x4_status4_tempIdxSelect_LTE_65p9C              6
#define rx_x4_status4_tempIdxSelect_LTE_75p6C              5
#define rx_x4_status4_tempIdxSelect_LTE_85p3C              4
#define rx_x4_status4_tempIdxSelect_LTE_95p5C              3
#define rx_x4_status4_tempIdxSelect_LTE_105p2C             2
#define rx_x4_status4_tempIdxSelect_LTE_114p9C             1
#define rx_x4_status4_tempIdxSelect_LTE_125p1C             0

/****************************************************************************
 * Enums: rx_x4_status4_port_mode
 */
#define rx_x4_status4_port_mode_QUAD_PORT                  0
#define rx_x4_status4_port_mode_TRI_1_PORT                 1
#define rx_x4_status4_port_mode_TRI_2_PORT                 2
#define rx_x4_status4_port_mode_DUAL_PORT                  3
#define rx_x4_status4_port_mode_SINGLE_PORT                4

/****************************************************************************
 * Enums: rx_x4_status4_rev_letter_enum
 */
#define rx_x4_status4_rev_letter_enum_REV_A                0
#define rx_x4_status4_rev_letter_enum_REV_B                1
#define rx_x4_status4_rev_letter_enum_REV_C                2
#define rx_x4_status4_rev_letter_enum_REV_D                3

/****************************************************************************
 * Enums: rx_x4_status4_rev_number_enum
 */
#define rx_x4_status4_rev_number_enum_REV_0                0
#define rx_x4_status4_rev_number_enum_REV_1                1
#define rx_x4_status4_rev_number_enum_REV_2                2
#define rx_x4_status4_rev_number_enum_REV_3                3
#define rx_x4_status4_rev_number_enum_REV_4                4
#define rx_x4_status4_rev_number_enum_REV_5                5
#define rx_x4_status4_rev_number_enum_REV_6                6
#define rx_x4_status4_rev_number_enum_REV_7                7

/****************************************************************************
 * Enums: rx_x4_status4_bonding_enum
 */
#define rx_x4_status4_bonding_enum_WIRE_BOND               0
#define rx_x4_status4_bonding_enum_FLIP_CHIP               1

/****************************************************************************
 * Enums: rx_x4_status4_tech_process
 */
#define rx_x4_status4_tech_process_PROCESS_90NM            0
#define rx_x4_status4_tech_process_PROCESS_65NM            1
#define rx_x4_status4_tech_process_PROCESS_40NM            2
#define rx_x4_status4_tech_process_PROCESS_28NM            3

/****************************************************************************
 * Enums: rx_x4_status4_model_num
 */
#define rx_x4_status4_model_num_SERDES_CL73                0
#define rx_x4_status4_model_num_XGXS_16G                   1
#define rx_x4_status4_model_num_HYPERCORE                  2
#define rx_x4_status4_model_num_HYPERLITE                  3
#define rx_x4_status4_model_num_PCIE_G2_PIPE               4
#define rx_x4_status4_model_num_SERDES_1p25GBd             5
#define rx_x4_status4_model_num_SATA2                      6
#define rx_x4_status4_model_num_QSGMII                     7
#define rx_x4_status4_model_num_XGXS10G                    8
#define rx_x4_status4_model_num_WARPCORE                   9
#define rx_x4_status4_model_num_XFICORE                    10
#define rx_x4_status4_model_num_RXFI                       11
#define rx_x4_status4_model_num_WARPLITE                   12
#define rx_x4_status4_model_num_PENTACORE                  13
#define rx_x4_status4_model_num_ESM                        14
#define rx_x4_status4_model_num_QUAD_SGMII                 15
#define rx_x4_status4_model_num_WARPCORE_3                 16
#define rx_x4_status4_model_num_TSC                        17
#define rx_x4_status4_model_num_TSC4E                      18
#define rx_x4_status4_model_num_TSC12E                     19
#define rx_x4_status4_model_num_TSC4F                      20
#define rx_x4_status4_model_num_XGXS_CL73_90NM             29
#define rx_x4_status4_model_num_SERDES_CL73_90NM           30
#define rx_x4_status4_model_num_WARPCORE3                  32
#define rx_x4_status4_model_num_WARPCORE4_TSC              33
#define rx_x4_status4_model_num_RXAUI                      34

/****************************************************************************
 * Enums: rx_x4_status4_payload
 */
#define rx_x4_status4_payload_REPEAT_2_BYTES               0
#define rx_x4_status4_payload_RAMPING                      1
#define rx_x4_status4_payload_CL48_CRPAT                   2
#define rx_x4_status4_payload_CL48_CJPAT                   3
#define rx_x4_status4_payload_CL36_LONG_CRPAT              4
#define rx_x4_status4_payload_CL36_SHORT_CRPAT             5

/****************************************************************************
 * Enums: rx_x4_status4_sc
 */
#define rx_x4_status4_sc_S_10G_CR1                         0
#define rx_x4_status4_sc_S_10G_KR1                         1
#define rx_x4_status4_sc_S_10G_X1                          2
#define rx_x4_status4_sc_S_10G_HG2_CR1                     4
#define rx_x4_status4_sc_S_10G_HG2_KR1                     5
#define rx_x4_status4_sc_S_10G_HG2_X1                      6
#define rx_x4_status4_sc_S_20G_CR1                         8
#define rx_x4_status4_sc_S_20G_KR1                         9
#define rx_x4_status4_sc_S_20G_X1                          10
#define rx_x4_status4_sc_S_20G_HG2_CR1                     12
#define rx_x4_status4_sc_S_20G_HG2_KR1                     13
#define rx_x4_status4_sc_S_20G_HG2_X1                      14
#define rx_x4_status4_sc_S_25G_CR1                         16
#define rx_x4_status4_sc_S_25G_KR1                         17
#define rx_x4_status4_sc_S_25G_X1                          18
#define rx_x4_status4_sc_S_25G_HG2_CR1                     20
#define rx_x4_status4_sc_S_25G_HG2_KR1                     21
#define rx_x4_status4_sc_S_25G_HG2_X1                      22
#define rx_x4_status4_sc_S_20G_CR2                         24
#define rx_x4_status4_sc_S_20G_KR2                         25
#define rx_x4_status4_sc_S_20G_X2                          26
#define rx_x4_status4_sc_S_20G_HG2_CR2                     28
#define rx_x4_status4_sc_S_20G_HG2_KR2                     29
#define rx_x4_status4_sc_S_20G_HG2_X2                      30
#define rx_x4_status4_sc_S_40G_CR2                         32
#define rx_x4_status4_sc_S_40G_KR2                         33
#define rx_x4_status4_sc_S_40G_X2                          34
#define rx_x4_status4_sc_S_40G_HG2_CR2                     36
#define rx_x4_status4_sc_S_40G_HG2_KR2                     37
#define rx_x4_status4_sc_S_40G_HG2_X2                      38
#define rx_x4_status4_sc_S_40G_CR4                         40
#define rx_x4_status4_sc_S_40G_KR4                         41
#define rx_x4_status4_sc_S_40G_X4                          42
#define rx_x4_status4_sc_S_40G_HG2_CR4                     44
#define rx_x4_status4_sc_S_40G_HG2_KR4                     45
#define rx_x4_status4_sc_S_40G_HG2_X4                      46
#define rx_x4_status4_sc_S_50G_CR2                         48
#define rx_x4_status4_sc_S_50G_KR2                         49
#define rx_x4_status4_sc_S_50G_X2                          50
#define rx_x4_status4_sc_S_50G_HG2_CR2                     52
#define rx_x4_status4_sc_S_50G_HG2_KR2                     53
#define rx_x4_status4_sc_S_50G_HG2_X2                      54
#define rx_x4_status4_sc_S_50G_CR4                         56
#define rx_x4_status4_sc_S_50G_KR4                         57
#define rx_x4_status4_sc_S_50G_X4                          58
#define rx_x4_status4_sc_S_50G_HG2_CR4                     60
#define rx_x4_status4_sc_S_50G_HG2_KR4                     61
#define rx_x4_status4_sc_S_50G_HG2_X4                      62
#define rx_x4_status4_sc_S_100G_CR4                        64
#define rx_x4_status4_sc_S_100G_KR4                        65
#define rx_x4_status4_sc_S_100G_X4                         66
#define rx_x4_status4_sc_S_100G_HG2_CR4                    68
#define rx_x4_status4_sc_S_100G_HG2_KR4                    69
#define rx_x4_status4_sc_S_100G_HG2_X4                     70
#define rx_x4_status4_sc_S_CL73_20GVCO                     72
#define rx_x4_status4_sc_S_CL73_25GVCO                     80
#define rx_x4_status4_sc_S_CL36_20GVCO                     88
#define rx_x4_status4_sc_S_CL36_25GVCO                     96

/****************************************************************************
 * Enums: rx_x4_status4_t_fifo_modes
 */
#define rx_x4_status4_t_fifo_modes_T_FIFO_MODE_BYPASS      0
#define rx_x4_status4_t_fifo_modes_T_FIFO_MODE_40G         1
#define rx_x4_status4_t_fifo_modes_T_FIFO_MODE_100G        2
#define rx_x4_status4_t_fifo_modes_T_FIFO_MODE_20G         3

/****************************************************************************
 * Enums: rx_x4_status4_t_enc_modes
 */
#define rx_x4_status4_t_enc_modes_T_ENC_MODE_BYPASS        0
#define rx_x4_status4_t_enc_modes_T_ENC_MODE_CL49          1
#define rx_x4_status4_t_enc_modes_T_ENC_MODE_CL82          2

/****************************************************************************
 * Enums: rx_x4_status4_btmx_mode
 */
#define rx_x4_status4_btmx_mode_BS_BTMX_MODE_1to1          0
#define rx_x4_status4_btmx_mode_BS_BTMX_MODE_2to1          1
#define rx_x4_status4_btmx_mode_BS_BTMX_MODE_5to1          2

/****************************************************************************
 * Enums: rx_x4_status4_t_type_cl82
 */
#define rx_x4_status4_t_type_cl82_T_TYPE_B1                5
#define rx_x4_status4_t_type_cl82_T_TYPE_C                 4
#define rx_x4_status4_t_type_cl82_T_TYPE_S                 3
#define rx_x4_status4_t_type_cl82_T_TYPE_T                 2
#define rx_x4_status4_t_type_cl82_T_TYPE_D                 1
#define rx_x4_status4_t_type_cl82_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x4_status4_txsm_state_cl82
 */
#define rx_x4_status4_txsm_state_cl82_TX_HIG_END           6
#define rx_x4_status4_txsm_state_cl82_TX_HIG_START         5
#define rx_x4_status4_txsm_state_cl82_TX_E                 4
#define rx_x4_status4_txsm_state_cl82_TX_T                 3
#define rx_x4_status4_txsm_state_cl82_TX_D                 2
#define rx_x4_status4_txsm_state_cl82_TX_C                 1
#define rx_x4_status4_txsm_state_cl82_TX_INIT              0

/****************************************************************************
 * Enums: rx_x4_status4_ltxsm_state_cl82
 */
#define rx_x4_status4_ltxsm_state_cl82_TX_HIG_END          64
#define rx_x4_status4_ltxsm_state_cl82_TX_HIG_START        32
#define rx_x4_status4_ltxsm_state_cl82_TX_E                16
#define rx_x4_status4_ltxsm_state_cl82_TX_T                8
#define rx_x4_status4_ltxsm_state_cl82_TX_D                4
#define rx_x4_status4_ltxsm_state_cl82_TX_C                2
#define rx_x4_status4_ltxsm_state_cl82_TX_INIT             1

/****************************************************************************
 * Enums: rx_x4_status4_r_type_coded_cl82
 */
#define rx_x4_status4_r_type_coded_cl82_R_TYPE_B1          32
#define rx_x4_status4_r_type_coded_cl82_R_TYPE_C           16
#define rx_x4_status4_r_type_coded_cl82_R_TYPE_S           8
#define rx_x4_status4_r_type_coded_cl82_R_TYPE_T           4
#define rx_x4_status4_r_type_coded_cl82_R_TYPE_D           2
#define rx_x4_status4_r_type_coded_cl82_R_TYPE_E           1

/****************************************************************************
 * Enums: rx_x4_status4_rxsm_state_cl82
 */
#define rx_x4_status4_rxsm_state_cl82_RX_HIG_END           64
#define rx_x4_status4_rxsm_state_cl82_RX_HIG_START         32
#define rx_x4_status4_rxsm_state_cl82_RX_E                 16
#define rx_x4_status4_rxsm_state_cl82_RX_T                 8
#define rx_x4_status4_rxsm_state_cl82_RX_D                 4
#define rx_x4_status4_rxsm_state_cl82_RX_C                 2
#define rx_x4_status4_rxsm_state_cl82_RX_INIT              1

/****************************************************************************
 * Enums: rx_x4_status4_deskew_state
 */
#define rx_x4_status4_deskew_state_ALIGN_ACQUIRED          2
#define rx_x4_status4_deskew_state_LOSS_OF_ALIGNMENT       1

/****************************************************************************
 * Enums: rx_x4_status4_os_mode_enum
 */
#define rx_x4_status4_os_mode_enum_OS_MODE_1               0
#define rx_x4_status4_os_mode_enum_OS_MODE_2               1
#define rx_x4_status4_os_mode_enum_OS_MODE_4               2
#define rx_x4_status4_os_mode_enum_OS_MODE_16p5            8
#define rx_x4_status4_os_mode_enum_OS_MODE_20p625          12

/****************************************************************************
 * Enums: rx_x4_status4_scr_modes
 */
#define rx_x4_status4_scr_modes_T_SCR_MODE_BYPASS          0
#define rx_x4_status4_scr_modes_T_SCR_MODE_CL49            1
#define rx_x4_status4_scr_modes_T_SCR_MODE_40G_2_LANE      2
#define rx_x4_status4_scr_modes_T_SCR_MODE_100G            3
#define rx_x4_status4_scr_modes_T_SCR_MODE_20G             4
#define rx_x4_status4_scr_modes_T_SCR_MODE_40G_4_LANE      5

/****************************************************************************
 * Enums: rx_x4_status4_descr_modes
 */
#define rx_x4_status4_descr_modes_R_DESCR_MODE_BYPASS      0
#define rx_x4_status4_descr_modes_R_DESCR_MODE_CL49        1
#define rx_x4_status4_descr_modes_R_DESCR_MODE_CL82        2

/****************************************************************************
 * Enums: rx_x4_status4_r_dec_tl_mode
 */
#define rx_x4_status4_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS   0
#define rx_x4_status4_r_dec_tl_mode_R_DEC_TL_MODE_CL49     1
#define rx_x4_status4_r_dec_tl_mode_R_DEC_TL_MODE_CL82     2

/****************************************************************************
 * Enums: rx_x4_status4_r_dec_fsm_mode
 */
#define rx_x4_status4_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define rx_x4_status4_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49   1
#define rx_x4_status4_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82   2

/****************************************************************************
 * Enums: rx_x4_status4_r_deskew_mode
 */
#define rx_x4_status4_r_deskew_mode_R_DESKEW_MODE_BYPASS   0
#define rx_x4_status4_r_deskew_mode_R_DESKEW_MODE_20G      1
#define rx_x4_status4_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define rx_x4_status4_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define rx_x4_status4_r_deskew_mode_R_DESKEW_MODE_100G     4
#define rx_x4_status4_r_deskew_mode_R_DESKEW_MODE_CL49     5
#define rx_x4_status4_r_deskew_mode_R_DESKEW_MODE_CL91     6

/****************************************************************************
 * Enums: rx_x4_status4_bs_dist_modes
 */
#define rx_x4_status4_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define rx_x4_status4_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define rx_x4_status4_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define rx_x4_status4_bs_dist_modes_BS_DIST_MODE_NO_TDM    3

/****************************************************************************
 * Enums: rx_x4_status4_cl49_t_type
 */
#define rx_x4_status4_cl49_t_type_BAD_T_TYPE               15
#define rx_x4_status4_cl49_t_type_T_TYPE_B0                11
#define rx_x4_status4_cl49_t_type_T_TYPE_OB                10
#define rx_x4_status4_cl49_t_type_T_TYPE_B1                9
#define rx_x4_status4_cl49_t_type_T_TYPE_DB                8
#define rx_x4_status4_cl49_t_type_T_TYPE_FC                7
#define rx_x4_status4_cl49_t_type_T_TYPE_TB                6
#define rx_x4_status4_cl49_t_type_T_TYPE_LI                5
#define rx_x4_status4_cl49_t_type_T_TYPE_C                 4
#define rx_x4_status4_cl49_t_type_T_TYPE_S                 3
#define rx_x4_status4_cl49_t_type_T_TYPE_T                 2
#define rx_x4_status4_cl49_t_type_T_TYPE_D                 1
#define rx_x4_status4_cl49_t_type_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x4_status4_cl49_txsm_states
 */
#define rx_x4_status4_cl49_txsm_states_TX_HIG_END          7
#define rx_x4_status4_cl49_txsm_states_TX_HIG_START        6
#define rx_x4_status4_cl49_txsm_states_TX_LI               5
#define rx_x4_status4_cl49_txsm_states_TX_E                4
#define rx_x4_status4_cl49_txsm_states_TX_T                3
#define rx_x4_status4_cl49_txsm_states_TX_D                2
#define rx_x4_status4_cl49_txsm_states_TX_C                1
#define rx_x4_status4_cl49_txsm_states_TX_INIT             0

/****************************************************************************
 * Enums: rx_x4_status4_cl49_ltxsm_states
 */
#define rx_x4_status4_cl49_ltxsm_states_TX_HIG_END         128
#define rx_x4_status4_cl49_ltxsm_states_TX_HIG_START       64
#define rx_x4_status4_cl49_ltxsm_states_TX_LI              32
#define rx_x4_status4_cl49_ltxsm_states_TX_E               16
#define rx_x4_status4_cl49_ltxsm_states_TX_T               8
#define rx_x4_status4_cl49_ltxsm_states_TX_D               4
#define rx_x4_status4_cl49_ltxsm_states_TX_C               2
#define rx_x4_status4_cl49_ltxsm_states_TX_INIT            1

/****************************************************************************
 * Enums: rx_x4_status4_burst_error_mode
 */
#define rx_x4_status4_burst_error_mode_BURST_ERROR_11_BITS 0
#define rx_x4_status4_burst_error_mode_BURST_ERROR_16_BITS 1
#define rx_x4_status4_burst_error_mode_BURST_ERROR_17_BITS 2
#define rx_x4_status4_burst_error_mode_BURST_ERROR_18_BITS 3
#define rx_x4_status4_burst_error_mode_BURST_ERROR_19_BITS 4
#define rx_x4_status4_burst_error_mode_BURST_ERROR_20_BITS 5
#define rx_x4_status4_burst_error_mode_BURST_ERROR_21_BITS 6
#define rx_x4_status4_burst_error_mode_BURST_ERROR_22_BITS 7
#define rx_x4_status4_burst_error_mode_BURST_ERROR_23_BITS 8
#define rx_x4_status4_burst_error_mode_BURST_ERROR_24_BITS 9
#define rx_x4_status4_burst_error_mode_BURST_ERROR_25_BITS 10
#define rx_x4_status4_burst_error_mode_BURST_ERROR_26_BITS 11
#define rx_x4_status4_burst_error_mode_BURST_ERROR_27_BITS 12
#define rx_x4_status4_burst_error_mode_BURST_ERROR_28_BITS 13
#define rx_x4_status4_burst_error_mode_BURST_ERROR_29_BITS 14
#define rx_x4_status4_burst_error_mode_BURST_ERROR_30_BITS 15
#define rx_x4_status4_burst_error_mode_BURST_ERROR_31_BITS 16
#define rx_x4_status4_burst_error_mode_BURST_ERROR_32_BITS 17
#define rx_x4_status4_burst_error_mode_BURST_ERROR_33_BITS 18
#define rx_x4_status4_burst_error_mode_BURST_ERROR_34_BITS 19
#define rx_x4_status4_burst_error_mode_BURST_ERROR_35_BITS 20
#define rx_x4_status4_burst_error_mode_BURST_ERROR_36_BITS 21
#define rx_x4_status4_burst_error_mode_BURST_ERROR_37_BITS 22
#define rx_x4_status4_burst_error_mode_BURST_ERROR_38_BITS 23
#define rx_x4_status4_burst_error_mode_BURST_ERROR_39_BITS 24
#define rx_x4_status4_burst_error_mode_BURST_ERROR_40_BITS 25
#define rx_x4_status4_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: rx_x4_status4_bermon_state
 */
#define rx_x4_status4_bermon_state_HI_BER                  4
#define rx_x4_status4_bermon_state_GOOD_BER                3
#define rx_x4_status4_bermon_state_BER_TEST_SH             2
#define rx_x4_status4_bermon_state_START_TIMER             1
#define rx_x4_status4_bermon_state_BER_MT_INIT             0

/****************************************************************************
 * Enums: rx_x4_status4_rxsm_state_cl49
 */
#define rx_x4_status4_rxsm_state_cl49_RX_HIG_END           128
#define rx_x4_status4_rxsm_state_cl49_RX_HIG_START         64
#define rx_x4_status4_rxsm_state_cl49_RX_LI                32
#define rx_x4_status4_rxsm_state_cl49_RX_E                 16
#define rx_x4_status4_rxsm_state_cl49_RX_T                 8
#define rx_x4_status4_rxsm_state_cl49_RX_D                 4
#define rx_x4_status4_rxsm_state_cl49_RX_C                 2
#define rx_x4_status4_rxsm_state_cl49_RX_INIT              1

/****************************************************************************
 * Enums: rx_x4_status4_r_type
 */
#define rx_x4_status4_r_type_BAD_R_TYPE                    15
#define rx_x4_status4_r_type_R_TYPE_B0                     11
#define rx_x4_status4_r_type_R_TYPE_OB                     10
#define rx_x4_status4_r_type_R_TYPE_B1                     9
#define rx_x4_status4_r_type_R_TYPE_DB                     8
#define rx_x4_status4_r_type_R_TYPE_FC                     7
#define rx_x4_status4_r_type_R_TYPE_TB                     6
#define rx_x4_status4_r_type_R_TYPE_LI                     5
#define rx_x4_status4_r_type_R_TYPE_C                      4
#define rx_x4_status4_r_type_R_TYPE_S                      3
#define rx_x4_status4_r_type_R_TYPE_T                      2
#define rx_x4_status4_r_type_R_TYPE_D                      1
#define rx_x4_status4_r_type_R_TYPE_E                      0

/****************************************************************************
 * Enums: rx_x4_status4_am_lock_state
 */
#define rx_x4_status4_am_lock_state_INVALID_AM             512
#define rx_x4_status4_am_lock_state_GOOD_AM                256
#define rx_x4_status4_am_lock_state_COMP_AM                128
#define rx_x4_status4_am_lock_state_TIMER_2                64
#define rx_x4_status4_am_lock_state_AM_2_GOOD              32
#define rx_x4_status4_am_lock_state_COMP_2ND               16
#define rx_x4_status4_am_lock_state_TIMER_1                8
#define rx_x4_status4_am_lock_state_FIND_1ST               4
#define rx_x4_status4_am_lock_state_AM_RESET_CNT           2
#define rx_x4_status4_am_lock_state_AM_LOCK_INIT           1

/****************************************************************************
 * Enums: rx_x4_status4_msg_selector
 */
#define rx_x4_status4_msg_selector_RESERVED                0
#define rx_x4_status4_msg_selector_VALUE_802p3             1
#define rx_x4_status4_msg_selector_VALUE_802p9             2
#define rx_x4_status4_msg_selector_VALUE_802p5             3
#define rx_x4_status4_msg_selector_VALUE_1394              4

/****************************************************************************
 * Enums: rx_x4_status4_synce_enum
 */
#define rx_x4_status4_synce_enum_SYNCE_NO_DIV              0
#define rx_x4_status4_synce_enum_SYNCE_DIV_7               1
#define rx_x4_status4_synce_enum_SYNCE_DIV_11              2

/****************************************************************************
 * Enums: rx_x4_status4_synce_enum_stage0
 */
#define rx_x4_status4_synce_enum_stage0_SYNCE_NO_DIV       0
#define rx_x4_status4_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define rx_x4_status4_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: rx_x4_status4_cl91_sync_state
 */
#define rx_x4_status4_cl91_sync_state_FIND_1ST             0
#define rx_x4_status4_cl91_sync_state_COUNT_NEXT           1
#define rx_x4_status4_cl91_sync_state_COMP_2ND             2
#define rx_x4_status4_cl91_sync_state_TWO_GOOD             3

/****************************************************************************
 * Enums: rx_x4_status4_cl91_algn_state
 */
#define rx_x4_status4_cl91_algn_state_LOSS_OF_ALIGNMENT    0
#define rx_x4_status4_cl91_algn_state_DESKEW               1
#define rx_x4_status4_cl91_algn_state_DESKEW_FAIL          2
#define rx_x4_status4_cl91_algn_state_ALIGN_ACQUIRED       3
#define rx_x4_status4_cl91_algn_state_CW_GOOD              4
#define rx_x4_status4_cl91_algn_state_CW_BAD               5
#define rx_x4_status4_cl91_algn_state_THREE_BAD            6

/****************************************************************************
 * Enums: rx_x4_status4_fec_sel
 */
#define rx_x4_status4_fec_sel_NO_FEC                       0
#define rx_x4_status4_fec_sel_FEC_CL74                     1
#define rx_x4_status4_fec_sel_FEC_CL91                     2

/****************************************************************************
 * Enums: rx_x4_status5_cl36TxEEEStates_l
 */
#define rx_x4_status5_cl36TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status5_cl36TxEEEStates_l_TX_QUIET           4
#define rx_x4_status5_cl36TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status5_cl36TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status5_cl36TxEEEStates_c
 */
#define rx_x4_status5_cl36TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status5_cl36TxEEEStates_c_TX_QUIET           2
#define rx_x4_status5_cl36TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status5_cl36TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status5_cl49TxEEEStates_l
 */
#define rx_x4_status5_cl49TxEEEStates_l_SCR_RESET_2        64
#define rx_x4_status5_cl49TxEEEStates_l_SCR_RESET_1        32
#define rx_x4_status5_cl49TxEEEStates_l_TX_WAKE            16
#define rx_x4_status5_cl49TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status5_cl49TxEEEStates_l_TX_QUIET           4
#define rx_x4_status5_cl49TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status5_cl49TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status5_fec_req_enum
 */
#define rx_x4_status5_fec_req_enum_FEC_not_supported       0
#define rx_x4_status5_fec_req_enum_FEC_supported_but_not_requested 1
#define rx_x4_status5_fec_req_enum_invalid_setting         2
#define rx_x4_status5_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: rx_x4_status5_cl73_pause_enum
 */
#define rx_x4_status5_cl73_pause_enum_No_PAUSE_ability     0
#define rx_x4_status5_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define rx_x4_status5_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define rx_x4_status5_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: rx_x4_status5_cl49TxEEEStates_c
 */
#define rx_x4_status5_cl49TxEEEStates_c_SCR_RESET_2        6
#define rx_x4_status5_cl49TxEEEStates_c_SCR_RESET_1        5
#define rx_x4_status5_cl49TxEEEStates_c_TX_WAKE            4
#define rx_x4_status5_cl49TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status5_cl49TxEEEStates_c_TX_QUIET           2
#define rx_x4_status5_cl49TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status5_cl49TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status5_cl36RxEEEStates_l
 */
#define rx_x4_status5_cl36RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status5_cl36RxEEEStates_l_RX_WTF             16
#define rx_x4_status5_cl36RxEEEStates_l_RX_WAKE            8
#define rx_x4_status5_cl36RxEEEStates_l_RX_QUIET           4
#define rx_x4_status5_cl36RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status5_cl36RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status5_cl36RxEEEStates_c
 */
#define rx_x4_status5_cl36RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status5_cl36RxEEEStates_c_RX_WTF             4
#define rx_x4_status5_cl36RxEEEStates_c_RX_WAKE            3
#define rx_x4_status5_cl36RxEEEStates_c_RX_QUIET           2
#define rx_x4_status5_cl36RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status5_cl36RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status5_cl49RxEEEStates_l
 */
#define rx_x4_status5_cl49RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status5_cl49RxEEEStates_l_RX_WTF             16
#define rx_x4_status5_cl49RxEEEStates_l_RX_WAKE            8
#define rx_x4_status5_cl49RxEEEStates_l_RX_QUIET           4
#define rx_x4_status5_cl49RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status5_cl49RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status5_cl49RxEEEStates_c
 */
#define rx_x4_status5_cl49RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status5_cl49RxEEEStates_c_RX_WTF             4
#define rx_x4_status5_cl49RxEEEStates_c_RX_WAKE            3
#define rx_x4_status5_cl49RxEEEStates_c_RX_QUIET           2
#define rx_x4_status5_cl49RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status5_cl49RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status5_cl48TxEEEStates_l
 */
#define rx_x4_status5_cl48TxEEEStates_l_TX_REFRESH         8
#define rx_x4_status5_cl48TxEEEStates_l_TX_QUIET           4
#define rx_x4_status5_cl48TxEEEStates_l_TX_SLEEP           2
#define rx_x4_status5_cl48TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status5_cl48TxEEEStates_c
 */
#define rx_x4_status5_cl48TxEEEStates_c_TX_REFRESH         3
#define rx_x4_status5_cl48TxEEEStates_c_TX_QUIET           2
#define rx_x4_status5_cl48TxEEEStates_c_TX_SLEEP           1
#define rx_x4_status5_cl48TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status5_cl48RxEEEStates_l
 */
#define rx_x4_status5_cl48RxEEEStates_l_RX_LINK_FAIL       32
#define rx_x4_status5_cl48RxEEEStates_l_RX_WAKE            16
#define rx_x4_status5_cl48RxEEEStates_l_RX_QUIET           8
#define rx_x4_status5_cl48RxEEEStates_l_RX_DEACT           4
#define rx_x4_status5_cl48RxEEEStates_l_RX_SLEEP           2
#define rx_x4_status5_cl48RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: rx_x4_status5_cl48RxEEEStates_c
 */
#define rx_x4_status5_cl48RxEEEStates_c_RX_LINK_FAIL       5
#define rx_x4_status5_cl48RxEEEStates_c_RX_WAKE            4
#define rx_x4_status5_cl48RxEEEStates_c_RX_QUIET           3
#define rx_x4_status5_cl48RxEEEStates_c_RX_DEACT           2
#define rx_x4_status5_cl48RxEEEStates_c_RX_SLEEP           1
#define rx_x4_status5_cl48RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: rx_x4_status5_IQP_Options
 */
#define rx_x4_status5_IQP_Options_i50uA                    0
#define rx_x4_status5_IQP_Options_i100uA                   1
#define rx_x4_status5_IQP_Options_i150uA                   2
#define rx_x4_status5_IQP_Options_i200uA                   3
#define rx_x4_status5_IQP_Options_i250uA                   4
#define rx_x4_status5_IQP_Options_i300uA                   5
#define rx_x4_status5_IQP_Options_i350uA                   6
#define rx_x4_status5_IQP_Options_i400uA                   7
#define rx_x4_status5_IQP_Options_i450uA                   8
#define rx_x4_status5_IQP_Options_i500uA                   9
#define rx_x4_status5_IQP_Options_i550uA                   10
#define rx_x4_status5_IQP_Options_i600uA                   11
#define rx_x4_status5_IQP_Options_i650uA                   12
#define rx_x4_status5_IQP_Options_i700uA                   13
#define rx_x4_status5_IQP_Options_i750uA                   14
#define rx_x4_status5_IQP_Options_i800uA                   15

/****************************************************************************
 * Enums: rx_x4_status5_IDriver_Options
 */
#define rx_x4_status5_IDriver_Options_v680mV               0
#define rx_x4_status5_IDriver_Options_v730mV               1
#define rx_x4_status5_IDriver_Options_v780mV               2
#define rx_x4_status5_IDriver_Options_v830mV               3
#define rx_x4_status5_IDriver_Options_v880mV               4
#define rx_x4_status5_IDriver_Options_v930mV               5
#define rx_x4_status5_IDriver_Options_v980mV               6
#define rx_x4_status5_IDriver_Options_v1010mV              7
#define rx_x4_status5_IDriver_Options_v1040mV              8
#define rx_x4_status5_IDriver_Options_v1060mV              9
#define rx_x4_status5_IDriver_Options_v1070mV              10
#define rx_x4_status5_IDriver_Options_v1080mV              11
#define rx_x4_status5_IDriver_Options_v1085mV              12
#define rx_x4_status5_IDriver_Options_v1090mV              13
#define rx_x4_status5_IDriver_Options_v1095mV              14
#define rx_x4_status5_IDriver_Options_v1100mV              15

/****************************************************************************
 * Enums: rx_x4_status5_operationModes
 */
#define rx_x4_status5_operationModes_XGXS                  0
#define rx_x4_status5_operationModes_XGXG_nCC              1
#define rx_x4_status5_operationModes_Indlane_OS8           4
#define rx_x4_status5_operationModes_IndLane_OS5           5
#define rx_x4_status5_operationModes_PCI                   7
#define rx_x4_status5_operationModes_XGXS_nLQ              8
#define rx_x4_status5_operationModes_XGXS_nLQnCC           9
#define rx_x4_status5_operationModes_PBypass               10
#define rx_x4_status5_operationModes_PBypass_nDSK          11
#define rx_x4_status5_operationModes_ComboCoreMode         12
#define rx_x4_status5_operationModes_Clocks_off            15

/****************************************************************************
 * Enums: rx_x4_status5_actualSpeeds
 */
#define rx_x4_status5_actualSpeeds_dr_10M                  0
#define rx_x4_status5_actualSpeeds_dr_100M                 1
#define rx_x4_status5_actualSpeeds_dr_1G                   2
#define rx_x4_status5_actualSpeeds_dr_2p5G                 3
#define rx_x4_status5_actualSpeeds_dr_5G_X4                4
#define rx_x4_status5_actualSpeeds_dr_6G_X4                5
#define rx_x4_status5_actualSpeeds_dr_10G_HiG              6
#define rx_x4_status5_actualSpeeds_dr_10G_CX4              7
#define rx_x4_status5_actualSpeeds_dr_12G_HiG              8
#define rx_x4_status5_actualSpeeds_dr_12p5G_X4             9
#define rx_x4_status5_actualSpeeds_dr_13G_X4               10
#define rx_x4_status5_actualSpeeds_dr_15G_X4               11
#define rx_x4_status5_actualSpeeds_dr_16G_X4               12
#define rx_x4_status5_actualSpeeds_dr_1G_KX                13
#define rx_x4_status5_actualSpeeds_dr_10G_KX4              14
#define rx_x4_status5_actualSpeeds_dr_10G_KR               15
#define rx_x4_status5_actualSpeeds_dr_5G                   16
#define rx_x4_status5_actualSpeeds_dr_6p4G                 17
#define rx_x4_status5_actualSpeeds_dr_20G_X4               18
#define rx_x4_status5_actualSpeeds_dr_21G_X4               19
#define rx_x4_status5_actualSpeeds_dr_25G_X4               20
#define rx_x4_status5_actualSpeeds_dr_10G_HiG_DXGXS        21
#define rx_x4_status5_actualSpeeds_dr_10G_DXGXS            22
#define rx_x4_status5_actualSpeeds_dr_10p5G_HiG_DXGXS      23
#define rx_x4_status5_actualSpeeds_dr_10p5G_DXGXS          24
#define rx_x4_status5_actualSpeeds_dr_12p773G_HiG_DXGXS    25
#define rx_x4_status5_actualSpeeds_dr_12p773G_DXGXS        26
#define rx_x4_status5_actualSpeeds_dr_10G_XFI              27
#define rx_x4_status5_actualSpeeds_dr_40G                  28
#define rx_x4_status5_actualSpeeds_dr_20G_HiG_DXGXS        29
#define rx_x4_status5_actualSpeeds_dr_20G_DXGXS            30
#define rx_x4_status5_actualSpeeds_dr_10G_SFI              31
#define rx_x4_status5_actualSpeeds_dr_31p5G                32
#define rx_x4_status5_actualSpeeds_dr_32p7G                33
#define rx_x4_status5_actualSpeeds_dr_20G_SCR              34
#define rx_x4_status5_actualSpeeds_dr_10G_HiG_DXGXS_SCR    35
#define rx_x4_status5_actualSpeeds_dr_10G_DXGXS_SCR        36
#define rx_x4_status5_actualSpeeds_dr_12G_R2               37
#define rx_x4_status5_actualSpeeds_dr_10G_X2               38
#define rx_x4_status5_actualSpeeds_dr_40G_KR4              39
#define rx_x4_status5_actualSpeeds_dr_40G_CR4              40
#define rx_x4_status5_actualSpeeds_dr_100G_CR10            41
#define rx_x4_status5_actualSpeeds_dr_15p75G_DXGXS         44
#define rx_x4_status5_actualSpeeds_dr_20G_KR2              57
#define rx_x4_status5_actualSpeeds_dr_20G_CR2              58

/****************************************************************************
 * Enums: rx_x4_status5_actualSpeedsMisc1
 */
#define rx_x4_status5_actualSpeedsMisc1_dr_2500BRCM_X1     16
#define rx_x4_status5_actualSpeedsMisc1_dr_5000BRCM_X4     17
#define rx_x4_status5_actualSpeedsMisc1_dr_6000BRCM_X4     18
#define rx_x4_status5_actualSpeedsMisc1_dr_10GHiGig_X4     19
#define rx_x4_status5_actualSpeedsMisc1_dr_10GBASE_CX4     20
#define rx_x4_status5_actualSpeedsMisc1_dr_12GHiGig_X4     21
#define rx_x4_status5_actualSpeedsMisc1_dr_12p5GHiGig_X4   22
#define rx_x4_status5_actualSpeedsMisc1_dr_13GHiGig_X4     23
#define rx_x4_status5_actualSpeedsMisc1_dr_15GHiGig_X4     24
#define rx_x4_status5_actualSpeedsMisc1_dr_16GHiGig_X4     25
#define rx_x4_status5_actualSpeedsMisc1_dr_5000BRCM_X1     26
#define rx_x4_status5_actualSpeedsMisc1_dr_6363BRCM_X1     27
#define rx_x4_status5_actualSpeedsMisc1_dr_20GHiGig_X4     28
#define rx_x4_status5_actualSpeedsMisc1_dr_21GHiGig_X4     29
#define rx_x4_status5_actualSpeedsMisc1_dr_25p45GHiGig_X4  30
#define rx_x4_status5_actualSpeedsMisc1_dr_10G_HiG_DXGXS   31

/****************************************************************************
 * Enums: rx_x4_status5_IndLaneModes
 */
#define rx_x4_status5_IndLaneModes_SWSDR_div2              0
#define rx_x4_status5_IndLaneModes_SWSDR_div1              1
#define rx_x4_status5_IndLaneModes_DWSDR_div2              2
#define rx_x4_status5_IndLaneModes_DWSDR_div1              3

/****************************************************************************
 * Enums: rx_x4_status5_prbsSelect
 */
#define rx_x4_status5_prbsSelect_prbs7                     0
#define rx_x4_status5_prbsSelect_prbs15                    1
#define rx_x4_status5_prbsSelect_prbs23                    2
#define rx_x4_status5_prbsSelect_prbs31                    3

/****************************************************************************
 * Enums: rx_x4_status5_vcoDivider
 */
#define rx_x4_status5_vcoDivider_div32                     0
#define rx_x4_status5_vcoDivider_div36                     1
#define rx_x4_status5_vcoDivider_div40                     2
#define rx_x4_status5_vcoDivider_div42                     3
#define rx_x4_status5_vcoDivider_div48                     4
#define rx_x4_status5_vcoDivider_div50                     5
#define rx_x4_status5_vcoDivider_div52                     6
#define rx_x4_status5_vcoDivider_div54                     7
#define rx_x4_status5_vcoDivider_div60                     8
#define rx_x4_status5_vcoDivider_div64                     9
#define rx_x4_status5_vcoDivider_div66                     10
#define rx_x4_status5_vcoDivider_div68                     11
#define rx_x4_status5_vcoDivider_div70                     12
#define rx_x4_status5_vcoDivider_div80                     13
#define rx_x4_status5_vcoDivider_div92                     14
#define rx_x4_status5_vcoDivider_div100                    15

/****************************************************************************
 * Enums: rx_x4_status5_refClkSelect
 */
#define rx_x4_status5_refClkSelect_clk_25MHz               0
#define rx_x4_status5_refClkSelect_clk_100MHz              1
#define rx_x4_status5_refClkSelect_clk_125MHz              2
#define rx_x4_status5_refClkSelect_clk_156p25MHz           3
#define rx_x4_status5_refClkSelect_clk_187p5MHz            4
#define rx_x4_status5_refClkSelect_clk_161p25Mhz           5
#define rx_x4_status5_refClkSelect_clk_50Mhz               6
#define rx_x4_status5_refClkSelect_clk_106p25Mhz           7

/****************************************************************************
 * Enums: rx_x4_status5_aerMMDdevTypeSelect
 */
#define rx_x4_status5_aerMMDdevTypeSelect_combo_core       0
#define rx_x4_status5_aerMMDdevTypeSelect_PMA_PMD          1
#define rx_x4_status5_aerMMDdevTypeSelect_PCS              3
#define rx_x4_status5_aerMMDdevTypeSelect_PHY              4
#define rx_x4_status5_aerMMDdevTypeSelect_DTE              5
#define rx_x4_status5_aerMMDdevTypeSelect_CL73_AN          7

/****************************************************************************
 * Enums: rx_x4_status5_aerMMDportSelect
 */
#define rx_x4_status5_aerMMDportSelect_ln0                 0
#define rx_x4_status5_aerMMDportSelect_ln1                 1
#define rx_x4_status5_aerMMDportSelect_ln2                 2
#define rx_x4_status5_aerMMDportSelect_ln3                 3
#define rx_x4_status5_aerMMDportSelect_BCST_ln0_1_2_3      511
#define rx_x4_status5_aerMMDportSelect_BCST_ln0_1          512
#define rx_x4_status5_aerMMDportSelect_BCST_ln2_3          513

/****************************************************************************
 * Enums: rx_x4_status5_firmwareModeSelect
 */
#define rx_x4_status5_firmwareModeSelect_DEFAULT           0
#define rx_x4_status5_firmwareModeSelect_SFP_OPT_LR        1
#define rx_x4_status5_firmwareModeSelect_SFP_DAC           2
#define rx_x4_status5_firmwareModeSelect_XLAUI             3
#define rx_x4_status5_firmwareModeSelect_LONG_CH_6G        4

/****************************************************************************
 * Enums: rx_x4_status5_tempIdxSelect
 */
#define rx_x4_status5_tempIdxSelect_LTE__22p9C             15
#define rx_x4_status5_tempIdxSelect_LTE__12p6C             14
#define rx_x4_status5_tempIdxSelect_LTE__3p0C              13
#define rx_x4_status5_tempIdxSelect_LTE_6p7C               12
#define rx_x4_status5_tempIdxSelect_LTE_16p4C              11
#define rx_x4_status5_tempIdxSelect_LTE_26p6C              10
#define rx_x4_status5_tempIdxSelect_LTE_36p3C              9
#define rx_x4_status5_tempIdxSelect_LTE_46p0C              8
#define rx_x4_status5_tempIdxSelect_LTE_56p2C              7
#define rx_x4_status5_tempIdxSelect_LTE_65p9C              6
#define rx_x4_status5_tempIdxSelect_LTE_75p6C              5
#define rx_x4_status5_tempIdxSelect_LTE_85p3C              4
#define rx_x4_status5_tempIdxSelect_LTE_95p5C              3
#define rx_x4_status5_tempIdxSelect_LTE_105p2C             2
#define rx_x4_status5_tempIdxSelect_LTE_114p9C             1
#define rx_x4_status5_tempIdxSelect_LTE_125p1C             0

/****************************************************************************
 * Enums: rx_x4_status5_port_mode
 */
#define rx_x4_status5_port_mode_QUAD_PORT                  0
#define rx_x4_status5_port_mode_TRI_1_PORT                 1
#define rx_x4_status5_port_mode_TRI_2_PORT                 2
#define rx_x4_status5_port_mode_DUAL_PORT                  3
#define rx_x4_status5_port_mode_SINGLE_PORT                4

/****************************************************************************
 * Enums: rx_x4_status5_rev_letter_enum
 */
#define rx_x4_status5_rev_letter_enum_REV_A                0
#define rx_x4_status5_rev_letter_enum_REV_B                1
#define rx_x4_status5_rev_letter_enum_REV_C                2
#define rx_x4_status5_rev_letter_enum_REV_D                3

/****************************************************************************
 * Enums: rx_x4_status5_rev_number_enum
 */
#define rx_x4_status5_rev_number_enum_REV_0                0
#define rx_x4_status5_rev_number_enum_REV_1                1
#define rx_x4_status5_rev_number_enum_REV_2                2
#define rx_x4_status5_rev_number_enum_REV_3                3
#define rx_x4_status5_rev_number_enum_REV_4                4
#define rx_x4_status5_rev_number_enum_REV_5                5
#define rx_x4_status5_rev_number_enum_REV_6                6
#define rx_x4_status5_rev_number_enum_REV_7                7

/****************************************************************************
 * Enums: rx_x4_status5_bonding_enum
 */
#define rx_x4_status5_bonding_enum_WIRE_BOND               0
#define rx_x4_status5_bonding_enum_FLIP_CHIP               1

/****************************************************************************
 * Enums: rx_x4_status5_tech_process
 */
#define rx_x4_status5_tech_process_PROCESS_90NM            0
#define rx_x4_status5_tech_process_PROCESS_65NM            1
#define rx_x4_status5_tech_process_PROCESS_40NM            2
#define rx_x4_status5_tech_process_PROCESS_28NM            3

/****************************************************************************
 * Enums: rx_x4_status5_model_num
 */
#define rx_x4_status5_model_num_SERDES_CL73                0
#define rx_x4_status5_model_num_XGXS_16G                   1
#define rx_x4_status5_model_num_HYPERCORE                  2
#define rx_x4_status5_model_num_HYPERLITE                  3
#define rx_x4_status5_model_num_PCIE_G2_PIPE               4
#define rx_x4_status5_model_num_SERDES_1p25GBd             5
#define rx_x4_status5_model_num_SATA2                      6
#define rx_x4_status5_model_num_QSGMII                     7
#define rx_x4_status5_model_num_XGXS10G                    8
#define rx_x4_status5_model_num_WARPCORE                   9
#define rx_x4_status5_model_num_XFICORE                    10
#define rx_x4_status5_model_num_RXFI                       11
#define rx_x4_status5_model_num_WARPLITE                   12
#define rx_x4_status5_model_num_PENTACORE                  13
#define rx_x4_status5_model_num_ESM                        14
#define rx_x4_status5_model_num_QUAD_SGMII                 15
#define rx_x4_status5_model_num_WARPCORE_3                 16
#define rx_x4_status5_model_num_TSC                        17
#define rx_x4_status5_model_num_TSC4E                      18
#define rx_x4_status5_model_num_TSC12E                     19
#define rx_x4_status5_model_num_TSC4F                      20
#define rx_x4_status5_model_num_XGXS_CL73_90NM             29
#define rx_x4_status5_model_num_SERDES_CL73_90NM           30
#define rx_x4_status5_model_num_WARPCORE3                  32
#define rx_x4_status5_model_num_WARPCORE4_TSC              33
#define rx_x4_status5_model_num_RXAUI                      34

/****************************************************************************
 * Enums: rx_x4_status5_payload
 */
#define rx_x4_status5_payload_REPEAT_2_BYTES               0
#define rx_x4_status5_payload_RAMPING                      1
#define rx_x4_status5_payload_CL48_CRPAT                   2
#define rx_x4_status5_payload_CL48_CJPAT                   3
#define rx_x4_status5_payload_CL36_LONG_CRPAT              4
#define rx_x4_status5_payload_CL36_SHORT_CRPAT             5

/****************************************************************************
 * Enums: rx_x4_status5_sc
 */
#define rx_x4_status5_sc_S_10G_CR1                         0
#define rx_x4_status5_sc_S_10G_KR1                         1
#define rx_x4_status5_sc_S_10G_X1                          2
#define rx_x4_status5_sc_S_10G_HG2_CR1                     4
#define rx_x4_status5_sc_S_10G_HG2_KR1                     5
#define rx_x4_status5_sc_S_10G_HG2_X1                      6
#define rx_x4_status5_sc_S_20G_CR1                         8
#define rx_x4_status5_sc_S_20G_KR1                         9
#define rx_x4_status5_sc_S_20G_X1                          10
#define rx_x4_status5_sc_S_20G_HG2_CR1                     12
#define rx_x4_status5_sc_S_20G_HG2_KR1                     13
#define rx_x4_status5_sc_S_20G_HG2_X1                      14
#define rx_x4_status5_sc_S_25G_CR1                         16
#define rx_x4_status5_sc_S_25G_KR1                         17
#define rx_x4_status5_sc_S_25G_X1                          18
#define rx_x4_status5_sc_S_25G_HG2_CR1                     20
#define rx_x4_status5_sc_S_25G_HG2_KR1                     21
#define rx_x4_status5_sc_S_25G_HG2_X1                      22
#define rx_x4_status5_sc_S_20G_CR2                         24
#define rx_x4_status5_sc_S_20G_KR2                         25
#define rx_x4_status5_sc_S_20G_X2                          26
#define rx_x4_status5_sc_S_20G_HG2_CR2                     28
#define rx_x4_status5_sc_S_20G_HG2_KR2                     29
#define rx_x4_status5_sc_S_20G_HG2_X2                      30
#define rx_x4_status5_sc_S_40G_CR2                         32
#define rx_x4_status5_sc_S_40G_KR2                         33
#define rx_x4_status5_sc_S_40G_X2                          34
#define rx_x4_status5_sc_S_40G_HG2_CR2                     36
#define rx_x4_status5_sc_S_40G_HG2_KR2                     37
#define rx_x4_status5_sc_S_40G_HG2_X2                      38
#define rx_x4_status5_sc_S_40G_CR4                         40
#define rx_x4_status5_sc_S_40G_KR4                         41
#define rx_x4_status5_sc_S_40G_X4                          42
#define rx_x4_status5_sc_S_40G_HG2_CR4                     44
#define rx_x4_status5_sc_S_40G_HG2_KR4                     45
#define rx_x4_status5_sc_S_40G_HG2_X4                      46
#define rx_x4_status5_sc_S_50G_CR2                         48
#define rx_x4_status5_sc_S_50G_KR2                         49
#define rx_x4_status5_sc_S_50G_X2                          50
#define rx_x4_status5_sc_S_50G_HG2_CR2                     52
#define rx_x4_status5_sc_S_50G_HG2_KR2                     53
#define rx_x4_status5_sc_S_50G_HG2_X2                      54
#define rx_x4_status5_sc_S_50G_CR4                         56
#define rx_x4_status5_sc_S_50G_KR4                         57
#define rx_x4_status5_sc_S_50G_X4                          58
#define rx_x4_status5_sc_S_50G_HG2_CR4                     60
#define rx_x4_status5_sc_S_50G_HG2_KR4                     61
#define rx_x4_status5_sc_S_50G_HG2_X4                      62
#define rx_x4_status5_sc_S_100G_CR4                        64
#define rx_x4_status5_sc_S_100G_KR4                        65
#define rx_x4_status5_sc_S_100G_X4                         66
#define rx_x4_status5_sc_S_100G_HG2_CR4                    68
#define rx_x4_status5_sc_S_100G_HG2_KR4                    69
#define rx_x4_status5_sc_S_100G_HG2_X4                     70
#define rx_x4_status5_sc_S_CL73_20GVCO                     72
#define rx_x4_status5_sc_S_CL73_25GVCO                     80
#define rx_x4_status5_sc_S_CL36_20GVCO                     88
#define rx_x4_status5_sc_S_CL36_25GVCO                     96

/****************************************************************************
 * Enums: rx_x4_status5_t_fifo_modes
 */
#define rx_x4_status5_t_fifo_modes_T_FIFO_MODE_BYPASS      0
#define rx_x4_status5_t_fifo_modes_T_FIFO_MODE_40G         1
#define rx_x4_status5_t_fifo_modes_T_FIFO_MODE_100G        2
#define rx_x4_status5_t_fifo_modes_T_FIFO_MODE_20G         3

/****************************************************************************
 * Enums: rx_x4_status5_t_enc_modes
 */
#define rx_x4_status5_t_enc_modes_T_ENC_MODE_BYPASS        0
#define rx_x4_status5_t_enc_modes_T_ENC_MODE_CL49          1
#define rx_x4_status5_t_enc_modes_T_ENC_MODE_CL82          2

/****************************************************************************
 * Enums: rx_x4_status5_btmx_mode
 */
#define rx_x4_status5_btmx_mode_BS_BTMX_MODE_1to1          0
#define rx_x4_status5_btmx_mode_BS_BTMX_MODE_2to1          1
#define rx_x4_status5_btmx_mode_BS_BTMX_MODE_5to1          2

/****************************************************************************
 * Enums: rx_x4_status5_t_type_cl82
 */
#define rx_x4_status5_t_type_cl82_T_TYPE_B1                5
#define rx_x4_status5_t_type_cl82_T_TYPE_C                 4
#define rx_x4_status5_t_type_cl82_T_TYPE_S                 3
#define rx_x4_status5_t_type_cl82_T_TYPE_T                 2
#define rx_x4_status5_t_type_cl82_T_TYPE_D                 1
#define rx_x4_status5_t_type_cl82_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x4_status5_txsm_state_cl82
 */
#define rx_x4_status5_txsm_state_cl82_TX_HIG_END           6
#define rx_x4_status5_txsm_state_cl82_TX_HIG_START         5
#define rx_x4_status5_txsm_state_cl82_TX_E                 4
#define rx_x4_status5_txsm_state_cl82_TX_T                 3
#define rx_x4_status5_txsm_state_cl82_TX_D                 2
#define rx_x4_status5_txsm_state_cl82_TX_C                 1
#define rx_x4_status5_txsm_state_cl82_TX_INIT              0

/****************************************************************************
 * Enums: rx_x4_status5_ltxsm_state_cl82
 */
#define rx_x4_status5_ltxsm_state_cl82_TX_HIG_END          64
#define rx_x4_status5_ltxsm_state_cl82_TX_HIG_START        32
#define rx_x4_status5_ltxsm_state_cl82_TX_E                16
#define rx_x4_status5_ltxsm_state_cl82_TX_T                8
#define rx_x4_status5_ltxsm_state_cl82_TX_D                4
#define rx_x4_status5_ltxsm_state_cl82_TX_C                2
#define rx_x4_status5_ltxsm_state_cl82_TX_INIT             1

/****************************************************************************
 * Enums: rx_x4_status5_r_type_coded_cl82
 */
#define rx_x4_status5_r_type_coded_cl82_R_TYPE_B1          32
#define rx_x4_status5_r_type_coded_cl82_R_TYPE_C           16
#define rx_x4_status5_r_type_coded_cl82_R_TYPE_S           8
#define rx_x4_status5_r_type_coded_cl82_R_TYPE_T           4
#define rx_x4_status5_r_type_coded_cl82_R_TYPE_D           2
#define rx_x4_status5_r_type_coded_cl82_R_TYPE_E           1

/****************************************************************************
 * Enums: rx_x4_status5_rxsm_state_cl82
 */
#define rx_x4_status5_rxsm_state_cl82_RX_HIG_END           64
#define rx_x4_status5_rxsm_state_cl82_RX_HIG_START         32
#define rx_x4_status5_rxsm_state_cl82_RX_E                 16
#define rx_x4_status5_rxsm_state_cl82_RX_T                 8
#define rx_x4_status5_rxsm_state_cl82_RX_D                 4
#define rx_x4_status5_rxsm_state_cl82_RX_C                 2
#define rx_x4_status5_rxsm_state_cl82_RX_INIT              1

/****************************************************************************
 * Enums: rx_x4_status5_deskew_state
 */
#define rx_x4_status5_deskew_state_ALIGN_ACQUIRED          2
#define rx_x4_status5_deskew_state_LOSS_OF_ALIGNMENT       1

/****************************************************************************
 * Enums: rx_x4_status5_os_mode_enum
 */
#define rx_x4_status5_os_mode_enum_OS_MODE_1               0
#define rx_x4_status5_os_mode_enum_OS_MODE_2               1
#define rx_x4_status5_os_mode_enum_OS_MODE_4               2
#define rx_x4_status5_os_mode_enum_OS_MODE_16p5            8
#define rx_x4_status5_os_mode_enum_OS_MODE_20p625          12

/****************************************************************************
 * Enums: rx_x4_status5_scr_modes
 */
#define rx_x4_status5_scr_modes_T_SCR_MODE_BYPASS          0
#define rx_x4_status5_scr_modes_T_SCR_MODE_CL49            1
#define rx_x4_status5_scr_modes_T_SCR_MODE_40G_2_LANE      2
#define rx_x4_status5_scr_modes_T_SCR_MODE_100G            3
#define rx_x4_status5_scr_modes_T_SCR_MODE_20G             4
#define rx_x4_status5_scr_modes_T_SCR_MODE_40G_4_LANE      5

/****************************************************************************
 * Enums: rx_x4_status5_descr_modes
 */
#define rx_x4_status5_descr_modes_R_DESCR_MODE_BYPASS      0
#define rx_x4_status5_descr_modes_R_DESCR_MODE_CL49        1
#define rx_x4_status5_descr_modes_R_DESCR_MODE_CL82        2

/****************************************************************************
 * Enums: rx_x4_status5_r_dec_tl_mode
 */
#define rx_x4_status5_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS   0
#define rx_x4_status5_r_dec_tl_mode_R_DEC_TL_MODE_CL49     1
#define rx_x4_status5_r_dec_tl_mode_R_DEC_TL_MODE_CL82     2

/****************************************************************************
 * Enums: rx_x4_status5_r_dec_fsm_mode
 */
#define rx_x4_status5_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define rx_x4_status5_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49   1
#define rx_x4_status5_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82   2

/****************************************************************************
 * Enums: rx_x4_status5_r_deskew_mode
 */
#define rx_x4_status5_r_deskew_mode_R_DESKEW_MODE_BYPASS   0
#define rx_x4_status5_r_deskew_mode_R_DESKEW_MODE_20G      1
#define rx_x4_status5_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define rx_x4_status5_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define rx_x4_status5_r_deskew_mode_R_DESKEW_MODE_100G     4
#define rx_x4_status5_r_deskew_mode_R_DESKEW_MODE_CL49     5
#define rx_x4_status5_r_deskew_mode_R_DESKEW_MODE_CL91     6

/****************************************************************************
 * Enums: rx_x4_status5_bs_dist_modes
 */
#define rx_x4_status5_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define rx_x4_status5_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define rx_x4_status5_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define rx_x4_status5_bs_dist_modes_BS_DIST_MODE_NO_TDM    3

/****************************************************************************
 * Enums: rx_x4_status5_cl49_t_type
 */
#define rx_x4_status5_cl49_t_type_BAD_T_TYPE               15
#define rx_x4_status5_cl49_t_type_T_TYPE_B0                11
#define rx_x4_status5_cl49_t_type_T_TYPE_OB                10
#define rx_x4_status5_cl49_t_type_T_TYPE_B1                9
#define rx_x4_status5_cl49_t_type_T_TYPE_DB                8
#define rx_x4_status5_cl49_t_type_T_TYPE_FC                7
#define rx_x4_status5_cl49_t_type_T_TYPE_TB                6
#define rx_x4_status5_cl49_t_type_T_TYPE_LI                5
#define rx_x4_status5_cl49_t_type_T_TYPE_C                 4
#define rx_x4_status5_cl49_t_type_T_TYPE_S                 3
#define rx_x4_status5_cl49_t_type_T_TYPE_T                 2
#define rx_x4_status5_cl49_t_type_T_TYPE_D                 1
#define rx_x4_status5_cl49_t_type_T_TYPE_E                 0

/****************************************************************************
 * Enums: rx_x4_status5_cl49_txsm_states
 */
#define rx_x4_status5_cl49_txsm_states_TX_HIG_END          7
#define rx_x4_status5_cl49_txsm_states_TX_HIG_START        6
#define rx_x4_status5_cl49_txsm_states_TX_LI               5
#define rx_x4_status5_cl49_txsm_states_TX_E                4
#define rx_x4_status5_cl49_txsm_states_TX_T                3
#define rx_x4_status5_cl49_txsm_states_TX_D                2
#define rx_x4_status5_cl49_txsm_states_TX_C                1
#define rx_x4_status5_cl49_txsm_states_TX_INIT             0

/****************************************************************************
 * Enums: rx_x4_status5_cl49_ltxsm_states
 */
#define rx_x4_status5_cl49_ltxsm_states_TX_HIG_END         128
#define rx_x4_status5_cl49_ltxsm_states_TX_HIG_START       64
#define rx_x4_status5_cl49_ltxsm_states_TX_LI              32
#define rx_x4_status5_cl49_ltxsm_states_TX_E               16
#define rx_x4_status5_cl49_ltxsm_states_TX_T               8
#define rx_x4_status5_cl49_ltxsm_states_TX_D               4
#define rx_x4_status5_cl49_ltxsm_states_TX_C               2
#define rx_x4_status5_cl49_ltxsm_states_TX_INIT            1

/****************************************************************************
 * Enums: rx_x4_status5_burst_error_mode
 */
#define rx_x4_status5_burst_error_mode_BURST_ERROR_11_BITS 0
#define rx_x4_status5_burst_error_mode_BURST_ERROR_16_BITS 1
#define rx_x4_status5_burst_error_mode_BURST_ERROR_17_BITS 2
#define rx_x4_status5_burst_error_mode_BURST_ERROR_18_BITS 3
#define rx_x4_status5_burst_error_mode_BURST_ERROR_19_BITS 4
#define rx_x4_status5_burst_error_mode_BURST_ERROR_20_BITS 5
#define rx_x4_status5_burst_error_mode_BURST_ERROR_21_BITS 6
#define rx_x4_status5_burst_error_mode_BURST_ERROR_22_BITS 7
#define rx_x4_status5_burst_error_mode_BURST_ERROR_23_BITS 8
#define rx_x4_status5_burst_error_mode_BURST_ERROR_24_BITS 9
#define rx_x4_status5_burst_error_mode_BURST_ERROR_25_BITS 10
#define rx_x4_status5_burst_error_mode_BURST_ERROR_26_BITS 11
#define rx_x4_status5_burst_error_mode_BURST_ERROR_27_BITS 12
#define rx_x4_status5_burst_error_mode_BURST_ERROR_28_BITS 13
#define rx_x4_status5_burst_error_mode_BURST_ERROR_29_BITS 14
#define rx_x4_status5_burst_error_mode_BURST_ERROR_30_BITS 15
#define rx_x4_status5_burst_error_mode_BURST_ERROR_31_BITS 16
#define rx_x4_status5_burst_error_mode_BURST_ERROR_32_BITS 17
#define rx_x4_status5_burst_error_mode_BURST_ERROR_33_BITS 18
#define rx_x4_status5_burst_error_mode_BURST_ERROR_34_BITS 19
#define rx_x4_status5_burst_error_mode_BURST_ERROR_35_BITS 20
#define rx_x4_status5_burst_error_mode_BURST_ERROR_36_BITS 21
#define rx_x4_status5_burst_error_mode_BURST_ERROR_37_BITS 22
#define rx_x4_status5_burst_error_mode_BURST_ERROR_38_BITS 23
#define rx_x4_status5_burst_error_mode_BURST_ERROR_39_BITS 24
#define rx_x4_status5_burst_error_mode_BURST_ERROR_40_BITS 25
#define rx_x4_status5_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: rx_x4_status5_bermon_state
 */
#define rx_x4_status5_bermon_state_HI_BER                  4
#define rx_x4_status5_bermon_state_GOOD_BER                3
#define rx_x4_status5_bermon_state_BER_TEST_SH             2
#define rx_x4_status5_bermon_state_START_TIMER             1
#define rx_x4_status5_bermon_state_BER_MT_INIT             0

/****************************************************************************
 * Enums: rx_x4_status5_rxsm_state_cl49
 */
#define rx_x4_status5_rxsm_state_cl49_RX_HIG_END           128
#define rx_x4_status5_rxsm_state_cl49_RX_HIG_START         64
#define rx_x4_status5_rxsm_state_cl49_RX_LI                32
#define rx_x4_status5_rxsm_state_cl49_RX_E                 16
#define rx_x4_status5_rxsm_state_cl49_RX_T                 8
#define rx_x4_status5_rxsm_state_cl49_RX_D                 4
#define rx_x4_status5_rxsm_state_cl49_RX_C                 2
#define rx_x4_status5_rxsm_state_cl49_RX_INIT              1

/****************************************************************************
 * Enums: rx_x4_status5_r_type
 */
#define rx_x4_status5_r_type_BAD_R_TYPE                    15
#define rx_x4_status5_r_type_R_TYPE_B0                     11
#define rx_x4_status5_r_type_R_TYPE_OB                     10
#define rx_x4_status5_r_type_R_TYPE_B1                     9
#define rx_x4_status5_r_type_R_TYPE_DB                     8
#define rx_x4_status5_r_type_R_TYPE_FC                     7
#define rx_x4_status5_r_type_R_TYPE_TB                     6
#define rx_x4_status5_r_type_R_TYPE_LI                     5
#define rx_x4_status5_r_type_R_TYPE_C                      4
#define rx_x4_status5_r_type_R_TYPE_S                      3
#define rx_x4_status5_r_type_R_TYPE_T                      2
#define rx_x4_status5_r_type_R_TYPE_D                      1
#define rx_x4_status5_r_type_R_TYPE_E                      0

/****************************************************************************
 * Enums: rx_x4_status5_am_lock_state
 */
#define rx_x4_status5_am_lock_state_INVALID_AM             512
#define rx_x4_status5_am_lock_state_GOOD_AM                256
#define rx_x4_status5_am_lock_state_COMP_AM                128
#define rx_x4_status5_am_lock_state_TIMER_2                64
#define rx_x4_status5_am_lock_state_AM_2_GOOD              32
#define rx_x4_status5_am_lock_state_COMP_2ND               16
#define rx_x4_status5_am_lock_state_TIMER_1                8
#define rx_x4_status5_am_lock_state_FIND_1ST               4
#define rx_x4_status5_am_lock_state_AM_RESET_CNT           2
#define rx_x4_status5_am_lock_state_AM_LOCK_INIT           1

/****************************************************************************
 * Enums: rx_x4_status5_msg_selector
 */
#define rx_x4_status5_msg_selector_RESERVED                0
#define rx_x4_status5_msg_selector_VALUE_802p3             1
#define rx_x4_status5_msg_selector_VALUE_802p9             2
#define rx_x4_status5_msg_selector_VALUE_802p5             3
#define rx_x4_status5_msg_selector_VALUE_1394              4

/****************************************************************************
 * Enums: rx_x4_status5_synce_enum
 */
#define rx_x4_status5_synce_enum_SYNCE_NO_DIV              0
#define rx_x4_status5_synce_enum_SYNCE_DIV_7               1
#define rx_x4_status5_synce_enum_SYNCE_DIV_11              2

/****************************************************************************
 * Enums: rx_x4_status5_synce_enum_stage0
 */
#define rx_x4_status5_synce_enum_stage0_SYNCE_NO_DIV       0
#define rx_x4_status5_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define rx_x4_status5_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: rx_x4_status5_cl91_sync_state
 */
#define rx_x4_status5_cl91_sync_state_FIND_1ST             0
#define rx_x4_status5_cl91_sync_state_COUNT_NEXT           1
#define rx_x4_status5_cl91_sync_state_COMP_2ND             2
#define rx_x4_status5_cl91_sync_state_TWO_GOOD             3

/****************************************************************************
 * Enums: rx_x4_status5_cl91_algn_state
 */
#define rx_x4_status5_cl91_algn_state_LOSS_OF_ALIGNMENT    0
#define rx_x4_status5_cl91_algn_state_DESKEW               1
#define rx_x4_status5_cl91_algn_state_DESKEW_FAIL          2
#define rx_x4_status5_cl91_algn_state_ALIGN_ACQUIRED       3
#define rx_x4_status5_cl91_algn_state_CW_GOOD              4
#define rx_x4_status5_cl91_algn_state_CW_BAD               5
#define rx_x4_status5_cl91_algn_state_THREE_BAD            6

/****************************************************************************
 * Enums: rx_x4_status5_fec_sel
 */
#define rx_x4_status5_fec_sel_NO_FEC                       0
#define rx_x4_status5_fec_sel_FEC_CL74                     1
#define rx_x4_status5_fec_sel_FEC_CL91                     2

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl36TxEEEStates_l
 */
#define sc_x1_speed_override0_cl36TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override0_cl36TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override0_cl36TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override0_cl36TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl36TxEEEStates_c
 */
#define sc_x1_speed_override0_cl36TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override0_cl36TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override0_cl36TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override0_cl36TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl49TxEEEStates_l
 */
#define sc_x1_speed_override0_cl49TxEEEStates_l_SCR_RESET_2 64
#define sc_x1_speed_override0_cl49TxEEEStates_l_SCR_RESET_1 32
#define sc_x1_speed_override0_cl49TxEEEStates_l_TX_WAKE    16
#define sc_x1_speed_override0_cl49TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override0_cl49TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override0_cl49TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override0_cl49TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override0_fec_req_enum
 */
#define sc_x1_speed_override0_fec_req_enum_FEC_not_supported 0
#define sc_x1_speed_override0_fec_req_enum_FEC_supported_but_not_requested 1
#define sc_x1_speed_override0_fec_req_enum_invalid_setting 2
#define sc_x1_speed_override0_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl73_pause_enum
 */
#define sc_x1_speed_override0_cl73_pause_enum_No_PAUSE_ability 0
#define sc_x1_speed_override0_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define sc_x1_speed_override0_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define sc_x1_speed_override0_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl49TxEEEStates_c
 */
#define sc_x1_speed_override0_cl49TxEEEStates_c_SCR_RESET_2 6
#define sc_x1_speed_override0_cl49TxEEEStates_c_SCR_RESET_1 5
#define sc_x1_speed_override0_cl49TxEEEStates_c_TX_WAKE    4
#define sc_x1_speed_override0_cl49TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override0_cl49TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override0_cl49TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override0_cl49TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl36RxEEEStates_l
 */
#define sc_x1_speed_override0_cl36RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override0_cl36RxEEEStates_l_RX_WTF     16
#define sc_x1_speed_override0_cl36RxEEEStates_l_RX_WAKE    8
#define sc_x1_speed_override0_cl36RxEEEStates_l_RX_QUIET   4
#define sc_x1_speed_override0_cl36RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override0_cl36RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl36RxEEEStates_c
 */
#define sc_x1_speed_override0_cl36RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override0_cl36RxEEEStates_c_RX_WTF     4
#define sc_x1_speed_override0_cl36RxEEEStates_c_RX_WAKE    3
#define sc_x1_speed_override0_cl36RxEEEStates_c_RX_QUIET   2
#define sc_x1_speed_override0_cl36RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override0_cl36RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl49RxEEEStates_l
 */
#define sc_x1_speed_override0_cl49RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override0_cl49RxEEEStates_l_RX_WTF     16
#define sc_x1_speed_override0_cl49RxEEEStates_l_RX_WAKE    8
#define sc_x1_speed_override0_cl49RxEEEStates_l_RX_QUIET   4
#define sc_x1_speed_override0_cl49RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override0_cl49RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl49RxEEEStates_c
 */
#define sc_x1_speed_override0_cl49RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override0_cl49RxEEEStates_c_RX_WTF     4
#define sc_x1_speed_override0_cl49RxEEEStates_c_RX_WAKE    3
#define sc_x1_speed_override0_cl49RxEEEStates_c_RX_QUIET   2
#define sc_x1_speed_override0_cl49RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override0_cl49RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl48TxEEEStates_l
 */
#define sc_x1_speed_override0_cl48TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override0_cl48TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override0_cl48TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override0_cl48TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl48TxEEEStates_c
 */
#define sc_x1_speed_override0_cl48TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override0_cl48TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override0_cl48TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override0_cl48TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl48RxEEEStates_l
 */
#define sc_x1_speed_override0_cl48RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override0_cl48RxEEEStates_l_RX_WAKE    16
#define sc_x1_speed_override0_cl48RxEEEStates_l_RX_QUIET   8
#define sc_x1_speed_override0_cl48RxEEEStates_l_RX_DEACT   4
#define sc_x1_speed_override0_cl48RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override0_cl48RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl48RxEEEStates_c
 */
#define sc_x1_speed_override0_cl48RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override0_cl48RxEEEStates_c_RX_WAKE    4
#define sc_x1_speed_override0_cl48RxEEEStates_c_RX_QUIET   3
#define sc_x1_speed_override0_cl48RxEEEStates_c_RX_DEACT   2
#define sc_x1_speed_override0_cl48RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override0_cl48RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override0_IQP_Options
 */
#define sc_x1_speed_override0_IQP_Options_i50uA            0
#define sc_x1_speed_override0_IQP_Options_i100uA           1
#define sc_x1_speed_override0_IQP_Options_i150uA           2
#define sc_x1_speed_override0_IQP_Options_i200uA           3
#define sc_x1_speed_override0_IQP_Options_i250uA           4
#define sc_x1_speed_override0_IQP_Options_i300uA           5
#define sc_x1_speed_override0_IQP_Options_i350uA           6
#define sc_x1_speed_override0_IQP_Options_i400uA           7
#define sc_x1_speed_override0_IQP_Options_i450uA           8
#define sc_x1_speed_override0_IQP_Options_i500uA           9
#define sc_x1_speed_override0_IQP_Options_i550uA           10
#define sc_x1_speed_override0_IQP_Options_i600uA           11
#define sc_x1_speed_override0_IQP_Options_i650uA           12
#define sc_x1_speed_override0_IQP_Options_i700uA           13
#define sc_x1_speed_override0_IQP_Options_i750uA           14
#define sc_x1_speed_override0_IQP_Options_i800uA           15

/****************************************************************************
 * Enums: sc_x1_speed_override0_IDriver_Options
 */
#define sc_x1_speed_override0_IDriver_Options_v680mV       0
#define sc_x1_speed_override0_IDriver_Options_v730mV       1
#define sc_x1_speed_override0_IDriver_Options_v780mV       2
#define sc_x1_speed_override0_IDriver_Options_v830mV       3
#define sc_x1_speed_override0_IDriver_Options_v880mV       4
#define sc_x1_speed_override0_IDriver_Options_v930mV       5
#define sc_x1_speed_override0_IDriver_Options_v980mV       6
#define sc_x1_speed_override0_IDriver_Options_v1010mV      7
#define sc_x1_speed_override0_IDriver_Options_v1040mV      8
#define sc_x1_speed_override0_IDriver_Options_v1060mV      9
#define sc_x1_speed_override0_IDriver_Options_v1070mV      10
#define sc_x1_speed_override0_IDriver_Options_v1080mV      11
#define sc_x1_speed_override0_IDriver_Options_v1085mV      12
#define sc_x1_speed_override0_IDriver_Options_v1090mV      13
#define sc_x1_speed_override0_IDriver_Options_v1095mV      14
#define sc_x1_speed_override0_IDriver_Options_v1100mV      15

/****************************************************************************
 * Enums: sc_x1_speed_override0_operationModes
 */
#define sc_x1_speed_override0_operationModes_XGXS          0
#define sc_x1_speed_override0_operationModes_XGXG_nCC      1
#define sc_x1_speed_override0_operationModes_Indlane_OS8   4
#define sc_x1_speed_override0_operationModes_IndLane_OS5   5
#define sc_x1_speed_override0_operationModes_PCI           7
#define sc_x1_speed_override0_operationModes_XGXS_nLQ      8
#define sc_x1_speed_override0_operationModes_XGXS_nLQnCC   9
#define sc_x1_speed_override0_operationModes_PBypass       10
#define sc_x1_speed_override0_operationModes_PBypass_nDSK  11
#define sc_x1_speed_override0_operationModes_ComboCoreMode 12
#define sc_x1_speed_override0_operationModes_Clocks_off    15

/****************************************************************************
 * Enums: sc_x1_speed_override0_actualSpeeds
 */
#define sc_x1_speed_override0_actualSpeeds_dr_10M          0
#define sc_x1_speed_override0_actualSpeeds_dr_100M         1
#define sc_x1_speed_override0_actualSpeeds_dr_1G           2
#define sc_x1_speed_override0_actualSpeeds_dr_2p5G         3
#define sc_x1_speed_override0_actualSpeeds_dr_5G_X4        4
#define sc_x1_speed_override0_actualSpeeds_dr_6G_X4        5
#define sc_x1_speed_override0_actualSpeeds_dr_10G_HiG      6
#define sc_x1_speed_override0_actualSpeeds_dr_10G_CX4      7
#define sc_x1_speed_override0_actualSpeeds_dr_12G_HiG      8
#define sc_x1_speed_override0_actualSpeeds_dr_12p5G_X4     9
#define sc_x1_speed_override0_actualSpeeds_dr_13G_X4       10
#define sc_x1_speed_override0_actualSpeeds_dr_15G_X4       11
#define sc_x1_speed_override0_actualSpeeds_dr_16G_X4       12
#define sc_x1_speed_override0_actualSpeeds_dr_1G_KX        13
#define sc_x1_speed_override0_actualSpeeds_dr_10G_KX4      14
#define sc_x1_speed_override0_actualSpeeds_dr_10G_KR       15
#define sc_x1_speed_override0_actualSpeeds_dr_5G           16
#define sc_x1_speed_override0_actualSpeeds_dr_6p4G         17
#define sc_x1_speed_override0_actualSpeeds_dr_20G_X4       18
#define sc_x1_speed_override0_actualSpeeds_dr_21G_X4       19
#define sc_x1_speed_override0_actualSpeeds_dr_25G_X4       20
#define sc_x1_speed_override0_actualSpeeds_dr_10G_HiG_DXGXS 21
#define sc_x1_speed_override0_actualSpeeds_dr_10G_DXGXS    22
#define sc_x1_speed_override0_actualSpeeds_dr_10p5G_HiG_DXGXS 23
#define sc_x1_speed_override0_actualSpeeds_dr_10p5G_DXGXS  24
#define sc_x1_speed_override0_actualSpeeds_dr_12p773G_HiG_DXGXS 25
#define sc_x1_speed_override0_actualSpeeds_dr_12p773G_DXGXS 26
#define sc_x1_speed_override0_actualSpeeds_dr_10G_XFI      27
#define sc_x1_speed_override0_actualSpeeds_dr_40G          28
#define sc_x1_speed_override0_actualSpeeds_dr_20G_HiG_DXGXS 29
#define sc_x1_speed_override0_actualSpeeds_dr_20G_DXGXS    30
#define sc_x1_speed_override0_actualSpeeds_dr_10G_SFI      31
#define sc_x1_speed_override0_actualSpeeds_dr_31p5G        32
#define sc_x1_speed_override0_actualSpeeds_dr_32p7G        33
#define sc_x1_speed_override0_actualSpeeds_dr_20G_SCR      34
#define sc_x1_speed_override0_actualSpeeds_dr_10G_HiG_DXGXS_SCR 35
#define sc_x1_speed_override0_actualSpeeds_dr_10G_DXGXS_SCR 36
#define sc_x1_speed_override0_actualSpeeds_dr_12G_R2       37
#define sc_x1_speed_override0_actualSpeeds_dr_10G_X2       38
#define sc_x1_speed_override0_actualSpeeds_dr_40G_KR4      39
#define sc_x1_speed_override0_actualSpeeds_dr_40G_CR4      40
#define sc_x1_speed_override0_actualSpeeds_dr_100G_CR10    41
#define sc_x1_speed_override0_actualSpeeds_dr_15p75G_DXGXS 44
#define sc_x1_speed_override0_actualSpeeds_dr_20G_KR2      57
#define sc_x1_speed_override0_actualSpeeds_dr_20G_CR2      58

/****************************************************************************
 * Enums: sc_x1_speed_override0_actualSpeedsMisc1
 */
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_2500BRCM_X1 16
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_5000BRCM_X4 17
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_6000BRCM_X4 18
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_10GHiGig_X4 19
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_10GBASE_CX4 20
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_12GHiGig_X4 21
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_12p5GHiGig_X4 22
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_13GHiGig_X4 23
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_15GHiGig_X4 24
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_16GHiGig_X4 25
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_5000BRCM_X1 26
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_6363BRCM_X1 27
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_20GHiGig_X4 28
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_21GHiGig_X4 29
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define sc_x1_speed_override0_actualSpeedsMisc1_dr_10G_HiG_DXGXS 31

/****************************************************************************
 * Enums: sc_x1_speed_override0_IndLaneModes
 */
#define sc_x1_speed_override0_IndLaneModes_SWSDR_div2      0
#define sc_x1_speed_override0_IndLaneModes_SWSDR_div1      1
#define sc_x1_speed_override0_IndLaneModes_DWSDR_div2      2
#define sc_x1_speed_override0_IndLaneModes_DWSDR_div1      3

/****************************************************************************
 * Enums: sc_x1_speed_override0_prbsSelect
 */
#define sc_x1_speed_override0_prbsSelect_prbs7             0
#define sc_x1_speed_override0_prbsSelect_prbs15            1
#define sc_x1_speed_override0_prbsSelect_prbs23            2
#define sc_x1_speed_override0_prbsSelect_prbs31            3

/****************************************************************************
 * Enums: sc_x1_speed_override0_vcoDivider
 */
#define sc_x1_speed_override0_vcoDivider_div32             0
#define sc_x1_speed_override0_vcoDivider_div36             1
#define sc_x1_speed_override0_vcoDivider_div40             2
#define sc_x1_speed_override0_vcoDivider_div42             3
#define sc_x1_speed_override0_vcoDivider_div48             4
#define sc_x1_speed_override0_vcoDivider_div50             5
#define sc_x1_speed_override0_vcoDivider_div52             6
#define sc_x1_speed_override0_vcoDivider_div54             7
#define sc_x1_speed_override0_vcoDivider_div60             8
#define sc_x1_speed_override0_vcoDivider_div64             9
#define sc_x1_speed_override0_vcoDivider_div66             10
#define sc_x1_speed_override0_vcoDivider_div68             11
#define sc_x1_speed_override0_vcoDivider_div70             12
#define sc_x1_speed_override0_vcoDivider_div80             13
#define sc_x1_speed_override0_vcoDivider_div92             14
#define sc_x1_speed_override0_vcoDivider_div100            15

/****************************************************************************
 * Enums: sc_x1_speed_override0_refClkSelect
 */
#define sc_x1_speed_override0_refClkSelect_clk_25MHz       0
#define sc_x1_speed_override0_refClkSelect_clk_100MHz      1
#define sc_x1_speed_override0_refClkSelect_clk_125MHz      2
#define sc_x1_speed_override0_refClkSelect_clk_156p25MHz   3
#define sc_x1_speed_override0_refClkSelect_clk_187p5MHz    4
#define sc_x1_speed_override0_refClkSelect_clk_161p25Mhz   5
#define sc_x1_speed_override0_refClkSelect_clk_50Mhz       6
#define sc_x1_speed_override0_refClkSelect_clk_106p25Mhz   7

/****************************************************************************
 * Enums: sc_x1_speed_override0_aerMMDdevTypeSelect
 */
#define sc_x1_speed_override0_aerMMDdevTypeSelect_combo_core 0
#define sc_x1_speed_override0_aerMMDdevTypeSelect_PMA_PMD  1
#define sc_x1_speed_override0_aerMMDdevTypeSelect_PCS      3
#define sc_x1_speed_override0_aerMMDdevTypeSelect_PHY      4
#define sc_x1_speed_override0_aerMMDdevTypeSelect_DTE      5
#define sc_x1_speed_override0_aerMMDdevTypeSelect_CL73_AN  7

/****************************************************************************
 * Enums: sc_x1_speed_override0_aerMMDportSelect
 */
#define sc_x1_speed_override0_aerMMDportSelect_ln0         0
#define sc_x1_speed_override0_aerMMDportSelect_ln1         1
#define sc_x1_speed_override0_aerMMDportSelect_ln2         2
#define sc_x1_speed_override0_aerMMDportSelect_ln3         3
#define sc_x1_speed_override0_aerMMDportSelect_BCST_ln0_1_2_3 511
#define sc_x1_speed_override0_aerMMDportSelect_BCST_ln0_1  512
#define sc_x1_speed_override0_aerMMDportSelect_BCST_ln2_3  513

/****************************************************************************
 * Enums: sc_x1_speed_override0_firmwareModeSelect
 */
#define sc_x1_speed_override0_firmwareModeSelect_DEFAULT   0
#define sc_x1_speed_override0_firmwareModeSelect_SFP_OPT_LR 1
#define sc_x1_speed_override0_firmwareModeSelect_SFP_DAC   2
#define sc_x1_speed_override0_firmwareModeSelect_XLAUI     3
#define sc_x1_speed_override0_firmwareModeSelect_LONG_CH_6G 4

/****************************************************************************
 * Enums: sc_x1_speed_override0_tempIdxSelect
 */
#define sc_x1_speed_override0_tempIdxSelect_LTE__22p9C     15
#define sc_x1_speed_override0_tempIdxSelect_LTE__12p6C     14
#define sc_x1_speed_override0_tempIdxSelect_LTE__3p0C      13
#define sc_x1_speed_override0_tempIdxSelect_LTE_6p7C       12
#define sc_x1_speed_override0_tempIdxSelect_LTE_16p4C      11
#define sc_x1_speed_override0_tempIdxSelect_LTE_26p6C      10
#define sc_x1_speed_override0_tempIdxSelect_LTE_36p3C      9
#define sc_x1_speed_override0_tempIdxSelect_LTE_46p0C      8
#define sc_x1_speed_override0_tempIdxSelect_LTE_56p2C      7
#define sc_x1_speed_override0_tempIdxSelect_LTE_65p9C      6
#define sc_x1_speed_override0_tempIdxSelect_LTE_75p6C      5
#define sc_x1_speed_override0_tempIdxSelect_LTE_85p3C      4
#define sc_x1_speed_override0_tempIdxSelect_LTE_95p5C      3
#define sc_x1_speed_override0_tempIdxSelect_LTE_105p2C     2
#define sc_x1_speed_override0_tempIdxSelect_LTE_114p9C     1
#define sc_x1_speed_override0_tempIdxSelect_LTE_125p1C     0

/****************************************************************************
 * Enums: sc_x1_speed_override0_port_mode
 */
#define sc_x1_speed_override0_port_mode_QUAD_PORT          0
#define sc_x1_speed_override0_port_mode_TRI_1_PORT         1
#define sc_x1_speed_override0_port_mode_TRI_2_PORT         2
#define sc_x1_speed_override0_port_mode_DUAL_PORT          3
#define sc_x1_speed_override0_port_mode_SINGLE_PORT        4

/****************************************************************************
 * Enums: sc_x1_speed_override0_rev_letter_enum
 */
#define sc_x1_speed_override0_rev_letter_enum_REV_A        0
#define sc_x1_speed_override0_rev_letter_enum_REV_B        1
#define sc_x1_speed_override0_rev_letter_enum_REV_C        2
#define sc_x1_speed_override0_rev_letter_enum_REV_D        3

/****************************************************************************
 * Enums: sc_x1_speed_override0_rev_number_enum
 */
#define sc_x1_speed_override0_rev_number_enum_REV_0        0
#define sc_x1_speed_override0_rev_number_enum_REV_1        1
#define sc_x1_speed_override0_rev_number_enum_REV_2        2
#define sc_x1_speed_override0_rev_number_enum_REV_3        3
#define sc_x1_speed_override0_rev_number_enum_REV_4        4
#define sc_x1_speed_override0_rev_number_enum_REV_5        5
#define sc_x1_speed_override0_rev_number_enum_REV_6        6
#define sc_x1_speed_override0_rev_number_enum_REV_7        7

/****************************************************************************
 * Enums: sc_x1_speed_override0_bonding_enum
 */
#define sc_x1_speed_override0_bonding_enum_WIRE_BOND       0
#define sc_x1_speed_override0_bonding_enum_FLIP_CHIP       1

/****************************************************************************
 * Enums: sc_x1_speed_override0_tech_process
 */
#define sc_x1_speed_override0_tech_process_PROCESS_90NM    0
#define sc_x1_speed_override0_tech_process_PROCESS_65NM    1
#define sc_x1_speed_override0_tech_process_PROCESS_40NM    2
#define sc_x1_speed_override0_tech_process_PROCESS_28NM    3

/****************************************************************************
 * Enums: sc_x1_speed_override0_model_num
 */
#define sc_x1_speed_override0_model_num_SERDES_CL73        0
#define sc_x1_speed_override0_model_num_XGXS_16G           1
#define sc_x1_speed_override0_model_num_HYPERCORE          2
#define sc_x1_speed_override0_model_num_HYPERLITE          3
#define sc_x1_speed_override0_model_num_PCIE_G2_PIPE       4
#define sc_x1_speed_override0_model_num_SERDES_1p25GBd     5
#define sc_x1_speed_override0_model_num_SATA2              6
#define sc_x1_speed_override0_model_num_QSGMII             7
#define sc_x1_speed_override0_model_num_XGXS10G            8
#define sc_x1_speed_override0_model_num_WARPCORE           9
#define sc_x1_speed_override0_model_num_XFICORE            10
#define sc_x1_speed_override0_model_num_RXFI               11
#define sc_x1_speed_override0_model_num_WARPLITE           12
#define sc_x1_speed_override0_model_num_PENTACORE          13
#define sc_x1_speed_override0_model_num_ESM                14
#define sc_x1_speed_override0_model_num_QUAD_SGMII         15
#define sc_x1_speed_override0_model_num_WARPCORE_3         16
#define sc_x1_speed_override0_model_num_TSC                17
#define sc_x1_speed_override0_model_num_TSC4E              18
#define sc_x1_speed_override0_model_num_TSC12E             19
#define sc_x1_speed_override0_model_num_TSC4F              20
#define sc_x1_speed_override0_model_num_XGXS_CL73_90NM     29
#define sc_x1_speed_override0_model_num_SERDES_CL73_90NM   30
#define sc_x1_speed_override0_model_num_WARPCORE3          32
#define sc_x1_speed_override0_model_num_WARPCORE4_TSC      33
#define sc_x1_speed_override0_model_num_RXAUI              34

/****************************************************************************
 * Enums: sc_x1_speed_override0_payload
 */
#define sc_x1_speed_override0_payload_REPEAT_2_BYTES       0
#define sc_x1_speed_override0_payload_RAMPING              1
#define sc_x1_speed_override0_payload_CL48_CRPAT           2
#define sc_x1_speed_override0_payload_CL48_CJPAT           3
#define sc_x1_speed_override0_payload_CL36_LONG_CRPAT      4
#define sc_x1_speed_override0_payload_CL36_SHORT_CRPAT     5

/****************************************************************************
 * Enums: sc_x1_speed_override0_sc
 */
#define sc_x1_speed_override0_sc_S_10G_CR1                 0
#define sc_x1_speed_override0_sc_S_10G_KR1                 1
#define sc_x1_speed_override0_sc_S_10G_X1                  2
#define sc_x1_speed_override0_sc_S_10G_HG2_CR1             4
#define sc_x1_speed_override0_sc_S_10G_HG2_KR1             5
#define sc_x1_speed_override0_sc_S_10G_HG2_X1              6
#define sc_x1_speed_override0_sc_S_20G_CR1                 8
#define sc_x1_speed_override0_sc_S_20G_KR1                 9
#define sc_x1_speed_override0_sc_S_20G_X1                  10
#define sc_x1_speed_override0_sc_S_20G_HG2_CR1             12
#define sc_x1_speed_override0_sc_S_20G_HG2_KR1             13
#define sc_x1_speed_override0_sc_S_20G_HG2_X1              14
#define sc_x1_speed_override0_sc_S_25G_CR1                 16
#define sc_x1_speed_override0_sc_S_25G_KR1                 17
#define sc_x1_speed_override0_sc_S_25G_X1                  18
#define sc_x1_speed_override0_sc_S_25G_HG2_CR1             20
#define sc_x1_speed_override0_sc_S_25G_HG2_KR1             21
#define sc_x1_speed_override0_sc_S_25G_HG2_X1              22
#define sc_x1_speed_override0_sc_S_20G_CR2                 24
#define sc_x1_speed_override0_sc_S_20G_KR2                 25
#define sc_x1_speed_override0_sc_S_20G_X2                  26
#define sc_x1_speed_override0_sc_S_20G_HG2_CR2             28
#define sc_x1_speed_override0_sc_S_20G_HG2_KR2             29
#define sc_x1_speed_override0_sc_S_20G_HG2_X2              30
#define sc_x1_speed_override0_sc_S_40G_CR2                 32
#define sc_x1_speed_override0_sc_S_40G_KR2                 33
#define sc_x1_speed_override0_sc_S_40G_X2                  34
#define sc_x1_speed_override0_sc_S_40G_HG2_CR2             36
#define sc_x1_speed_override0_sc_S_40G_HG2_KR2             37
#define sc_x1_speed_override0_sc_S_40G_HG2_X2              38
#define sc_x1_speed_override0_sc_S_40G_CR4                 40
#define sc_x1_speed_override0_sc_S_40G_KR4                 41
#define sc_x1_speed_override0_sc_S_40G_X4                  42
#define sc_x1_speed_override0_sc_S_40G_HG2_CR4             44
#define sc_x1_speed_override0_sc_S_40G_HG2_KR4             45
#define sc_x1_speed_override0_sc_S_40G_HG2_X4              46
#define sc_x1_speed_override0_sc_S_50G_CR2                 48
#define sc_x1_speed_override0_sc_S_50G_KR2                 49
#define sc_x1_speed_override0_sc_S_50G_X2                  50
#define sc_x1_speed_override0_sc_S_50G_HG2_CR2             52
#define sc_x1_speed_override0_sc_S_50G_HG2_KR2             53
#define sc_x1_speed_override0_sc_S_50G_HG2_X2              54
#define sc_x1_speed_override0_sc_S_50G_CR4                 56
#define sc_x1_speed_override0_sc_S_50G_KR4                 57
#define sc_x1_speed_override0_sc_S_50G_X4                  58
#define sc_x1_speed_override0_sc_S_50G_HG2_CR4             60
#define sc_x1_speed_override0_sc_S_50G_HG2_KR4             61
#define sc_x1_speed_override0_sc_S_50G_HG2_X4              62
#define sc_x1_speed_override0_sc_S_100G_CR4                64
#define sc_x1_speed_override0_sc_S_100G_KR4                65
#define sc_x1_speed_override0_sc_S_100G_X4                 66
#define sc_x1_speed_override0_sc_S_100G_HG2_CR4            68
#define sc_x1_speed_override0_sc_S_100G_HG2_KR4            69
#define sc_x1_speed_override0_sc_S_100G_HG2_X4             70
#define sc_x1_speed_override0_sc_S_CL73_20GVCO             72
#define sc_x1_speed_override0_sc_S_CL73_25GVCO             80
#define sc_x1_speed_override0_sc_S_CL36_20GVCO             88
#define sc_x1_speed_override0_sc_S_CL36_25GVCO             96

/****************************************************************************
 * Enums: sc_x1_speed_override0_t_fifo_modes
 */
#define sc_x1_speed_override0_t_fifo_modes_T_FIFO_MODE_BYPASS 0
#define sc_x1_speed_override0_t_fifo_modes_T_FIFO_MODE_40G 1
#define sc_x1_speed_override0_t_fifo_modes_T_FIFO_MODE_100G 2
#define sc_x1_speed_override0_t_fifo_modes_T_FIFO_MODE_20G 3

/****************************************************************************
 * Enums: sc_x1_speed_override0_t_enc_modes
 */
#define sc_x1_speed_override0_t_enc_modes_T_ENC_MODE_BYPASS 0
#define sc_x1_speed_override0_t_enc_modes_T_ENC_MODE_CL49  1
#define sc_x1_speed_override0_t_enc_modes_T_ENC_MODE_CL82  2

/****************************************************************************
 * Enums: sc_x1_speed_override0_btmx_mode
 */
#define sc_x1_speed_override0_btmx_mode_BS_BTMX_MODE_1to1  0
#define sc_x1_speed_override0_btmx_mode_BS_BTMX_MODE_2to1  1
#define sc_x1_speed_override0_btmx_mode_BS_BTMX_MODE_5to1  2

/****************************************************************************
 * Enums: sc_x1_speed_override0_t_type_cl82
 */
#define sc_x1_speed_override0_t_type_cl82_T_TYPE_B1        5
#define sc_x1_speed_override0_t_type_cl82_T_TYPE_C         4
#define sc_x1_speed_override0_t_type_cl82_T_TYPE_S         3
#define sc_x1_speed_override0_t_type_cl82_T_TYPE_T         2
#define sc_x1_speed_override0_t_type_cl82_T_TYPE_D         1
#define sc_x1_speed_override0_t_type_cl82_T_TYPE_E         0

/****************************************************************************
 * Enums: sc_x1_speed_override0_txsm_state_cl82
 */
#define sc_x1_speed_override0_txsm_state_cl82_TX_HIG_END   6
#define sc_x1_speed_override0_txsm_state_cl82_TX_HIG_START 5
#define sc_x1_speed_override0_txsm_state_cl82_TX_E         4
#define sc_x1_speed_override0_txsm_state_cl82_TX_T         3
#define sc_x1_speed_override0_txsm_state_cl82_TX_D         2
#define sc_x1_speed_override0_txsm_state_cl82_TX_C         1
#define sc_x1_speed_override0_txsm_state_cl82_TX_INIT      0

/****************************************************************************
 * Enums: sc_x1_speed_override0_ltxsm_state_cl82
 */
#define sc_x1_speed_override0_ltxsm_state_cl82_TX_HIG_END  64
#define sc_x1_speed_override0_ltxsm_state_cl82_TX_HIG_START 32
#define sc_x1_speed_override0_ltxsm_state_cl82_TX_E        16
#define sc_x1_speed_override0_ltxsm_state_cl82_TX_T        8
#define sc_x1_speed_override0_ltxsm_state_cl82_TX_D        4
#define sc_x1_speed_override0_ltxsm_state_cl82_TX_C        2
#define sc_x1_speed_override0_ltxsm_state_cl82_TX_INIT     1

/****************************************************************************
 * Enums: sc_x1_speed_override0_r_type_coded_cl82
 */
#define sc_x1_speed_override0_r_type_coded_cl82_R_TYPE_B1  32
#define sc_x1_speed_override0_r_type_coded_cl82_R_TYPE_C   16
#define sc_x1_speed_override0_r_type_coded_cl82_R_TYPE_S   8
#define sc_x1_speed_override0_r_type_coded_cl82_R_TYPE_T   4
#define sc_x1_speed_override0_r_type_coded_cl82_R_TYPE_D   2
#define sc_x1_speed_override0_r_type_coded_cl82_R_TYPE_E   1

/****************************************************************************
 * Enums: sc_x1_speed_override0_rxsm_state_cl82
 */
#define sc_x1_speed_override0_rxsm_state_cl82_RX_HIG_END   64
#define sc_x1_speed_override0_rxsm_state_cl82_RX_HIG_START 32
#define sc_x1_speed_override0_rxsm_state_cl82_RX_E         16
#define sc_x1_speed_override0_rxsm_state_cl82_RX_T         8
#define sc_x1_speed_override0_rxsm_state_cl82_RX_D         4
#define sc_x1_speed_override0_rxsm_state_cl82_RX_C         2
#define sc_x1_speed_override0_rxsm_state_cl82_RX_INIT      1

/****************************************************************************
 * Enums: sc_x1_speed_override0_deskew_state
 */
#define sc_x1_speed_override0_deskew_state_ALIGN_ACQUIRED  2
#define sc_x1_speed_override0_deskew_state_LOSS_OF_ALIGNMENT 1

/****************************************************************************
 * Enums: sc_x1_speed_override0_os_mode_enum
 */
#define sc_x1_speed_override0_os_mode_enum_OS_MODE_1       0
#define sc_x1_speed_override0_os_mode_enum_OS_MODE_2       1
#define sc_x1_speed_override0_os_mode_enum_OS_MODE_4       2
#define sc_x1_speed_override0_os_mode_enum_OS_MODE_16p5    8
#define sc_x1_speed_override0_os_mode_enum_OS_MODE_20p625  12

/****************************************************************************
 * Enums: sc_x1_speed_override0_scr_modes
 */
#define sc_x1_speed_override0_scr_modes_T_SCR_MODE_BYPASS  0
#define sc_x1_speed_override0_scr_modes_T_SCR_MODE_CL49    1
#define sc_x1_speed_override0_scr_modes_T_SCR_MODE_40G_2_LANE 2
#define sc_x1_speed_override0_scr_modes_T_SCR_MODE_100G    3
#define sc_x1_speed_override0_scr_modes_T_SCR_MODE_20G     4
#define sc_x1_speed_override0_scr_modes_T_SCR_MODE_40G_4_LANE 5

/****************************************************************************
 * Enums: sc_x1_speed_override0_descr_modes
 */
#define sc_x1_speed_override0_descr_modes_R_DESCR_MODE_BYPASS 0
#define sc_x1_speed_override0_descr_modes_R_DESCR_MODE_CL49 1
#define sc_x1_speed_override0_descr_modes_R_DESCR_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override0_r_dec_tl_mode
 */
#define sc_x1_speed_override0_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS 0
#define sc_x1_speed_override0_r_dec_tl_mode_R_DEC_TL_MODE_CL49 1
#define sc_x1_speed_override0_r_dec_tl_mode_R_DEC_TL_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override0_r_dec_fsm_mode
 */
#define sc_x1_speed_override0_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define sc_x1_speed_override0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49 1
#define sc_x1_speed_override0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override0_r_deskew_mode
 */
#define sc_x1_speed_override0_r_deskew_mode_R_DESKEW_MODE_BYPASS 0
#define sc_x1_speed_override0_r_deskew_mode_R_DESKEW_MODE_20G 1
#define sc_x1_speed_override0_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define sc_x1_speed_override0_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define sc_x1_speed_override0_r_deskew_mode_R_DESKEW_MODE_100G 4
#define sc_x1_speed_override0_r_deskew_mode_R_DESKEW_MODE_CL49 5
#define sc_x1_speed_override0_r_deskew_mode_R_DESKEW_MODE_CL91 6

/****************************************************************************
 * Enums: sc_x1_speed_override0_bs_dist_modes
 */
#define sc_x1_speed_override0_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define sc_x1_speed_override0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define sc_x1_speed_override0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define sc_x1_speed_override0_bs_dist_modes_BS_DIST_MODE_NO_TDM 3

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl49_t_type
 */
#define sc_x1_speed_override0_cl49_t_type_BAD_T_TYPE       15
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_B0        11
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_OB        10
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_B1        9
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_DB        8
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_FC        7
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_TB        6
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_LI        5
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_C         4
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_S         3
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_T         2
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_D         1
#define sc_x1_speed_override0_cl49_t_type_T_TYPE_E         0

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl49_txsm_states
 */
#define sc_x1_speed_override0_cl49_txsm_states_TX_HIG_END  7
#define sc_x1_speed_override0_cl49_txsm_states_TX_HIG_START 6
#define sc_x1_speed_override0_cl49_txsm_states_TX_LI       5
#define sc_x1_speed_override0_cl49_txsm_states_TX_E        4
#define sc_x1_speed_override0_cl49_txsm_states_TX_T        3
#define sc_x1_speed_override0_cl49_txsm_states_TX_D        2
#define sc_x1_speed_override0_cl49_txsm_states_TX_C        1
#define sc_x1_speed_override0_cl49_txsm_states_TX_INIT     0

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl49_ltxsm_states
 */
#define sc_x1_speed_override0_cl49_ltxsm_states_TX_HIG_END 128
#define sc_x1_speed_override0_cl49_ltxsm_states_TX_HIG_START 64
#define sc_x1_speed_override0_cl49_ltxsm_states_TX_LI      32
#define sc_x1_speed_override0_cl49_ltxsm_states_TX_E       16
#define sc_x1_speed_override0_cl49_ltxsm_states_TX_T       8
#define sc_x1_speed_override0_cl49_ltxsm_states_TX_D       4
#define sc_x1_speed_override0_cl49_ltxsm_states_TX_C       2
#define sc_x1_speed_override0_cl49_ltxsm_states_TX_INIT    1

/****************************************************************************
 * Enums: sc_x1_speed_override0_burst_error_mode
 */
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_11_BITS 0
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_16_BITS 1
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_17_BITS 2
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_18_BITS 3
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_19_BITS 4
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_20_BITS 5
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_21_BITS 6
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_22_BITS 7
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_23_BITS 8
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_24_BITS 9
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_25_BITS 10
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_26_BITS 11
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_27_BITS 12
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_28_BITS 13
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_29_BITS 14
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_30_BITS 15
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_31_BITS 16
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_32_BITS 17
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_33_BITS 18
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_34_BITS 19
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_35_BITS 20
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_36_BITS 21
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_37_BITS 22
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_38_BITS 23
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_39_BITS 24
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_40_BITS 25
#define sc_x1_speed_override0_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: sc_x1_speed_override0_bermon_state
 */
#define sc_x1_speed_override0_bermon_state_HI_BER          4
#define sc_x1_speed_override0_bermon_state_GOOD_BER        3
#define sc_x1_speed_override0_bermon_state_BER_TEST_SH     2
#define sc_x1_speed_override0_bermon_state_START_TIMER     1
#define sc_x1_speed_override0_bermon_state_BER_MT_INIT     0

/****************************************************************************
 * Enums: sc_x1_speed_override0_rxsm_state_cl49
 */
#define sc_x1_speed_override0_rxsm_state_cl49_RX_HIG_END   128
#define sc_x1_speed_override0_rxsm_state_cl49_RX_HIG_START 64
#define sc_x1_speed_override0_rxsm_state_cl49_RX_LI        32
#define sc_x1_speed_override0_rxsm_state_cl49_RX_E         16
#define sc_x1_speed_override0_rxsm_state_cl49_RX_T         8
#define sc_x1_speed_override0_rxsm_state_cl49_RX_D         4
#define sc_x1_speed_override0_rxsm_state_cl49_RX_C         2
#define sc_x1_speed_override0_rxsm_state_cl49_RX_INIT      1

/****************************************************************************
 * Enums: sc_x1_speed_override0_r_type
 */
#define sc_x1_speed_override0_r_type_BAD_R_TYPE            15
#define sc_x1_speed_override0_r_type_R_TYPE_B0             11
#define sc_x1_speed_override0_r_type_R_TYPE_OB             10
#define sc_x1_speed_override0_r_type_R_TYPE_B1             9
#define sc_x1_speed_override0_r_type_R_TYPE_DB             8
#define sc_x1_speed_override0_r_type_R_TYPE_FC             7
#define sc_x1_speed_override0_r_type_R_TYPE_TB             6
#define sc_x1_speed_override0_r_type_R_TYPE_LI             5
#define sc_x1_speed_override0_r_type_R_TYPE_C              4
#define sc_x1_speed_override0_r_type_R_TYPE_S              3
#define sc_x1_speed_override0_r_type_R_TYPE_T              2
#define sc_x1_speed_override0_r_type_R_TYPE_D              1
#define sc_x1_speed_override0_r_type_R_TYPE_E              0

/****************************************************************************
 * Enums: sc_x1_speed_override0_am_lock_state
 */
#define sc_x1_speed_override0_am_lock_state_INVALID_AM     512
#define sc_x1_speed_override0_am_lock_state_GOOD_AM        256
#define sc_x1_speed_override0_am_lock_state_COMP_AM        128
#define sc_x1_speed_override0_am_lock_state_TIMER_2        64
#define sc_x1_speed_override0_am_lock_state_AM_2_GOOD      32
#define sc_x1_speed_override0_am_lock_state_COMP_2ND       16
#define sc_x1_speed_override0_am_lock_state_TIMER_1        8
#define sc_x1_speed_override0_am_lock_state_FIND_1ST       4
#define sc_x1_speed_override0_am_lock_state_AM_RESET_CNT   2
#define sc_x1_speed_override0_am_lock_state_AM_LOCK_INIT   1

/****************************************************************************
 * Enums: sc_x1_speed_override0_msg_selector
 */
#define sc_x1_speed_override0_msg_selector_RESERVED        0
#define sc_x1_speed_override0_msg_selector_VALUE_802p3     1
#define sc_x1_speed_override0_msg_selector_VALUE_802p9     2
#define sc_x1_speed_override0_msg_selector_VALUE_802p5     3
#define sc_x1_speed_override0_msg_selector_VALUE_1394      4

/****************************************************************************
 * Enums: sc_x1_speed_override0_synce_enum
 */
#define sc_x1_speed_override0_synce_enum_SYNCE_NO_DIV      0
#define sc_x1_speed_override0_synce_enum_SYNCE_DIV_7       1
#define sc_x1_speed_override0_synce_enum_SYNCE_DIV_11      2

/****************************************************************************
 * Enums: sc_x1_speed_override0_synce_enum_stage0
 */
#define sc_x1_speed_override0_synce_enum_stage0_SYNCE_NO_DIV 0
#define sc_x1_speed_override0_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define sc_x1_speed_override0_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl91_sync_state
 */
#define sc_x1_speed_override0_cl91_sync_state_FIND_1ST     0
#define sc_x1_speed_override0_cl91_sync_state_COUNT_NEXT   1
#define sc_x1_speed_override0_cl91_sync_state_COMP_2ND     2
#define sc_x1_speed_override0_cl91_sync_state_TWO_GOOD     3

/****************************************************************************
 * Enums: sc_x1_speed_override0_cl91_algn_state
 */
#define sc_x1_speed_override0_cl91_algn_state_LOSS_OF_ALIGNMENT 0
#define sc_x1_speed_override0_cl91_algn_state_DESKEW       1
#define sc_x1_speed_override0_cl91_algn_state_DESKEW_FAIL  2
#define sc_x1_speed_override0_cl91_algn_state_ALIGN_ACQUIRED 3
#define sc_x1_speed_override0_cl91_algn_state_CW_GOOD      4
#define sc_x1_speed_override0_cl91_algn_state_CW_BAD       5
#define sc_x1_speed_override0_cl91_algn_state_THREE_BAD    6

/****************************************************************************
 * Enums: sc_x1_speed_override0_fec_sel
 */
#define sc_x1_speed_override0_fec_sel_NO_FEC               0
#define sc_x1_speed_override0_fec_sel_FEC_CL74             1
#define sc_x1_speed_override0_fec_sel_FEC_CL91             2

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl36TxEEEStates_l
 */
#define sc_x1_speed_override1_cl36TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override1_cl36TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override1_cl36TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override1_cl36TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl36TxEEEStates_c
 */
#define sc_x1_speed_override1_cl36TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override1_cl36TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override1_cl36TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override1_cl36TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl49TxEEEStates_l
 */
#define sc_x1_speed_override1_cl49TxEEEStates_l_SCR_RESET_2 64
#define sc_x1_speed_override1_cl49TxEEEStates_l_SCR_RESET_1 32
#define sc_x1_speed_override1_cl49TxEEEStates_l_TX_WAKE    16
#define sc_x1_speed_override1_cl49TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override1_cl49TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override1_cl49TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override1_cl49TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override1_fec_req_enum
 */
#define sc_x1_speed_override1_fec_req_enum_FEC_not_supported 0
#define sc_x1_speed_override1_fec_req_enum_FEC_supported_but_not_requested 1
#define sc_x1_speed_override1_fec_req_enum_invalid_setting 2
#define sc_x1_speed_override1_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl73_pause_enum
 */
#define sc_x1_speed_override1_cl73_pause_enum_No_PAUSE_ability 0
#define sc_x1_speed_override1_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define sc_x1_speed_override1_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define sc_x1_speed_override1_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl49TxEEEStates_c
 */
#define sc_x1_speed_override1_cl49TxEEEStates_c_SCR_RESET_2 6
#define sc_x1_speed_override1_cl49TxEEEStates_c_SCR_RESET_1 5
#define sc_x1_speed_override1_cl49TxEEEStates_c_TX_WAKE    4
#define sc_x1_speed_override1_cl49TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override1_cl49TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override1_cl49TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override1_cl49TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl36RxEEEStates_l
 */
#define sc_x1_speed_override1_cl36RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override1_cl36RxEEEStates_l_RX_WTF     16
#define sc_x1_speed_override1_cl36RxEEEStates_l_RX_WAKE    8
#define sc_x1_speed_override1_cl36RxEEEStates_l_RX_QUIET   4
#define sc_x1_speed_override1_cl36RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override1_cl36RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl36RxEEEStates_c
 */
#define sc_x1_speed_override1_cl36RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override1_cl36RxEEEStates_c_RX_WTF     4
#define sc_x1_speed_override1_cl36RxEEEStates_c_RX_WAKE    3
#define sc_x1_speed_override1_cl36RxEEEStates_c_RX_QUIET   2
#define sc_x1_speed_override1_cl36RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override1_cl36RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl49RxEEEStates_l
 */
#define sc_x1_speed_override1_cl49RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override1_cl49RxEEEStates_l_RX_WTF     16
#define sc_x1_speed_override1_cl49RxEEEStates_l_RX_WAKE    8
#define sc_x1_speed_override1_cl49RxEEEStates_l_RX_QUIET   4
#define sc_x1_speed_override1_cl49RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override1_cl49RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl49RxEEEStates_c
 */
#define sc_x1_speed_override1_cl49RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override1_cl49RxEEEStates_c_RX_WTF     4
#define sc_x1_speed_override1_cl49RxEEEStates_c_RX_WAKE    3
#define sc_x1_speed_override1_cl49RxEEEStates_c_RX_QUIET   2
#define sc_x1_speed_override1_cl49RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override1_cl49RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl48TxEEEStates_l
 */
#define sc_x1_speed_override1_cl48TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override1_cl48TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override1_cl48TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override1_cl48TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl48TxEEEStates_c
 */
#define sc_x1_speed_override1_cl48TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override1_cl48TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override1_cl48TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override1_cl48TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl48RxEEEStates_l
 */
#define sc_x1_speed_override1_cl48RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override1_cl48RxEEEStates_l_RX_WAKE    16
#define sc_x1_speed_override1_cl48RxEEEStates_l_RX_QUIET   8
#define sc_x1_speed_override1_cl48RxEEEStates_l_RX_DEACT   4
#define sc_x1_speed_override1_cl48RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override1_cl48RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl48RxEEEStates_c
 */
#define sc_x1_speed_override1_cl48RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override1_cl48RxEEEStates_c_RX_WAKE    4
#define sc_x1_speed_override1_cl48RxEEEStates_c_RX_QUIET   3
#define sc_x1_speed_override1_cl48RxEEEStates_c_RX_DEACT   2
#define sc_x1_speed_override1_cl48RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override1_cl48RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override1_IQP_Options
 */
#define sc_x1_speed_override1_IQP_Options_i50uA            0
#define sc_x1_speed_override1_IQP_Options_i100uA           1
#define sc_x1_speed_override1_IQP_Options_i150uA           2
#define sc_x1_speed_override1_IQP_Options_i200uA           3
#define sc_x1_speed_override1_IQP_Options_i250uA           4
#define sc_x1_speed_override1_IQP_Options_i300uA           5
#define sc_x1_speed_override1_IQP_Options_i350uA           6
#define sc_x1_speed_override1_IQP_Options_i400uA           7
#define sc_x1_speed_override1_IQP_Options_i450uA           8
#define sc_x1_speed_override1_IQP_Options_i500uA           9
#define sc_x1_speed_override1_IQP_Options_i550uA           10
#define sc_x1_speed_override1_IQP_Options_i600uA           11
#define sc_x1_speed_override1_IQP_Options_i650uA           12
#define sc_x1_speed_override1_IQP_Options_i700uA           13
#define sc_x1_speed_override1_IQP_Options_i750uA           14
#define sc_x1_speed_override1_IQP_Options_i800uA           15

/****************************************************************************
 * Enums: sc_x1_speed_override1_IDriver_Options
 */
#define sc_x1_speed_override1_IDriver_Options_v680mV       0
#define sc_x1_speed_override1_IDriver_Options_v730mV       1
#define sc_x1_speed_override1_IDriver_Options_v780mV       2
#define sc_x1_speed_override1_IDriver_Options_v830mV       3
#define sc_x1_speed_override1_IDriver_Options_v880mV       4
#define sc_x1_speed_override1_IDriver_Options_v930mV       5
#define sc_x1_speed_override1_IDriver_Options_v980mV       6
#define sc_x1_speed_override1_IDriver_Options_v1010mV      7
#define sc_x1_speed_override1_IDriver_Options_v1040mV      8
#define sc_x1_speed_override1_IDriver_Options_v1060mV      9
#define sc_x1_speed_override1_IDriver_Options_v1070mV      10
#define sc_x1_speed_override1_IDriver_Options_v1080mV      11
#define sc_x1_speed_override1_IDriver_Options_v1085mV      12
#define sc_x1_speed_override1_IDriver_Options_v1090mV      13
#define sc_x1_speed_override1_IDriver_Options_v1095mV      14
#define sc_x1_speed_override1_IDriver_Options_v1100mV      15

/****************************************************************************
 * Enums: sc_x1_speed_override1_operationModes
 */
#define sc_x1_speed_override1_operationModes_XGXS          0
#define sc_x1_speed_override1_operationModes_XGXG_nCC      1
#define sc_x1_speed_override1_operationModes_Indlane_OS8   4
#define sc_x1_speed_override1_operationModes_IndLane_OS5   5
#define sc_x1_speed_override1_operationModes_PCI           7
#define sc_x1_speed_override1_operationModes_XGXS_nLQ      8
#define sc_x1_speed_override1_operationModes_XGXS_nLQnCC   9
#define sc_x1_speed_override1_operationModes_PBypass       10
#define sc_x1_speed_override1_operationModes_PBypass_nDSK  11
#define sc_x1_speed_override1_operationModes_ComboCoreMode 12
#define sc_x1_speed_override1_operationModes_Clocks_off    15

/****************************************************************************
 * Enums: sc_x1_speed_override1_actualSpeeds
 */
#define sc_x1_speed_override1_actualSpeeds_dr_10M          0
#define sc_x1_speed_override1_actualSpeeds_dr_100M         1
#define sc_x1_speed_override1_actualSpeeds_dr_1G           2
#define sc_x1_speed_override1_actualSpeeds_dr_2p5G         3
#define sc_x1_speed_override1_actualSpeeds_dr_5G_X4        4
#define sc_x1_speed_override1_actualSpeeds_dr_6G_X4        5
#define sc_x1_speed_override1_actualSpeeds_dr_10G_HiG      6
#define sc_x1_speed_override1_actualSpeeds_dr_10G_CX4      7
#define sc_x1_speed_override1_actualSpeeds_dr_12G_HiG      8
#define sc_x1_speed_override1_actualSpeeds_dr_12p5G_X4     9
#define sc_x1_speed_override1_actualSpeeds_dr_13G_X4       10
#define sc_x1_speed_override1_actualSpeeds_dr_15G_X4       11
#define sc_x1_speed_override1_actualSpeeds_dr_16G_X4       12
#define sc_x1_speed_override1_actualSpeeds_dr_1G_KX        13
#define sc_x1_speed_override1_actualSpeeds_dr_10G_KX4      14
#define sc_x1_speed_override1_actualSpeeds_dr_10G_KR       15
#define sc_x1_speed_override1_actualSpeeds_dr_5G           16
#define sc_x1_speed_override1_actualSpeeds_dr_6p4G         17
#define sc_x1_speed_override1_actualSpeeds_dr_20G_X4       18
#define sc_x1_speed_override1_actualSpeeds_dr_21G_X4       19
#define sc_x1_speed_override1_actualSpeeds_dr_25G_X4       20
#define sc_x1_speed_override1_actualSpeeds_dr_10G_HiG_DXGXS 21
#define sc_x1_speed_override1_actualSpeeds_dr_10G_DXGXS    22
#define sc_x1_speed_override1_actualSpeeds_dr_10p5G_HiG_DXGXS 23
#define sc_x1_speed_override1_actualSpeeds_dr_10p5G_DXGXS  24
#define sc_x1_speed_override1_actualSpeeds_dr_12p773G_HiG_DXGXS 25
#define sc_x1_speed_override1_actualSpeeds_dr_12p773G_DXGXS 26
#define sc_x1_speed_override1_actualSpeeds_dr_10G_XFI      27
#define sc_x1_speed_override1_actualSpeeds_dr_40G          28
#define sc_x1_speed_override1_actualSpeeds_dr_20G_HiG_DXGXS 29
#define sc_x1_speed_override1_actualSpeeds_dr_20G_DXGXS    30
#define sc_x1_speed_override1_actualSpeeds_dr_10G_SFI      31
#define sc_x1_speed_override1_actualSpeeds_dr_31p5G        32
#define sc_x1_speed_override1_actualSpeeds_dr_32p7G        33
#define sc_x1_speed_override1_actualSpeeds_dr_20G_SCR      34
#define sc_x1_speed_override1_actualSpeeds_dr_10G_HiG_DXGXS_SCR 35
#define sc_x1_speed_override1_actualSpeeds_dr_10G_DXGXS_SCR 36
#define sc_x1_speed_override1_actualSpeeds_dr_12G_R2       37
#define sc_x1_speed_override1_actualSpeeds_dr_10G_X2       38
#define sc_x1_speed_override1_actualSpeeds_dr_40G_KR4      39
#define sc_x1_speed_override1_actualSpeeds_dr_40G_CR4      40
#define sc_x1_speed_override1_actualSpeeds_dr_100G_CR10    41
#define sc_x1_speed_override1_actualSpeeds_dr_15p75G_DXGXS 44
#define sc_x1_speed_override1_actualSpeeds_dr_20G_KR2      57
#define sc_x1_speed_override1_actualSpeeds_dr_20G_CR2      58

/****************************************************************************
 * Enums: sc_x1_speed_override1_actualSpeedsMisc1
 */
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_2500BRCM_X1 16
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_5000BRCM_X4 17
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_6000BRCM_X4 18
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_10GHiGig_X4 19
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_10GBASE_CX4 20
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_12GHiGig_X4 21
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_12p5GHiGig_X4 22
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_13GHiGig_X4 23
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_15GHiGig_X4 24
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_16GHiGig_X4 25
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_5000BRCM_X1 26
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_6363BRCM_X1 27
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_20GHiGig_X4 28
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_21GHiGig_X4 29
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define sc_x1_speed_override1_actualSpeedsMisc1_dr_10G_HiG_DXGXS 31

/****************************************************************************
 * Enums: sc_x1_speed_override1_IndLaneModes
 */
#define sc_x1_speed_override1_IndLaneModes_SWSDR_div2      0
#define sc_x1_speed_override1_IndLaneModes_SWSDR_div1      1
#define sc_x1_speed_override1_IndLaneModes_DWSDR_div2      2
#define sc_x1_speed_override1_IndLaneModes_DWSDR_div1      3

/****************************************************************************
 * Enums: sc_x1_speed_override1_prbsSelect
 */
#define sc_x1_speed_override1_prbsSelect_prbs7             0
#define sc_x1_speed_override1_prbsSelect_prbs15            1
#define sc_x1_speed_override1_prbsSelect_prbs23            2
#define sc_x1_speed_override1_prbsSelect_prbs31            3

/****************************************************************************
 * Enums: sc_x1_speed_override1_vcoDivider
 */
#define sc_x1_speed_override1_vcoDivider_div32             0
#define sc_x1_speed_override1_vcoDivider_div36             1
#define sc_x1_speed_override1_vcoDivider_div40             2
#define sc_x1_speed_override1_vcoDivider_div42             3
#define sc_x1_speed_override1_vcoDivider_div48             4
#define sc_x1_speed_override1_vcoDivider_div50             5
#define sc_x1_speed_override1_vcoDivider_div52             6
#define sc_x1_speed_override1_vcoDivider_div54             7
#define sc_x1_speed_override1_vcoDivider_div60             8
#define sc_x1_speed_override1_vcoDivider_div64             9
#define sc_x1_speed_override1_vcoDivider_div66             10
#define sc_x1_speed_override1_vcoDivider_div68             11
#define sc_x1_speed_override1_vcoDivider_div70             12
#define sc_x1_speed_override1_vcoDivider_div80             13
#define sc_x1_speed_override1_vcoDivider_div92             14
#define sc_x1_speed_override1_vcoDivider_div100            15

/****************************************************************************
 * Enums: sc_x1_speed_override1_refClkSelect
 */
#define sc_x1_speed_override1_refClkSelect_clk_25MHz       0
#define sc_x1_speed_override1_refClkSelect_clk_100MHz      1
#define sc_x1_speed_override1_refClkSelect_clk_125MHz      2
#define sc_x1_speed_override1_refClkSelect_clk_156p25MHz   3
#define sc_x1_speed_override1_refClkSelect_clk_187p5MHz    4
#define sc_x1_speed_override1_refClkSelect_clk_161p25Mhz   5
#define sc_x1_speed_override1_refClkSelect_clk_50Mhz       6
#define sc_x1_speed_override1_refClkSelect_clk_106p25Mhz   7

/****************************************************************************
 * Enums: sc_x1_speed_override1_aerMMDdevTypeSelect
 */
#define sc_x1_speed_override1_aerMMDdevTypeSelect_combo_core 0
#define sc_x1_speed_override1_aerMMDdevTypeSelect_PMA_PMD  1
#define sc_x1_speed_override1_aerMMDdevTypeSelect_PCS      3
#define sc_x1_speed_override1_aerMMDdevTypeSelect_PHY      4
#define sc_x1_speed_override1_aerMMDdevTypeSelect_DTE      5
#define sc_x1_speed_override1_aerMMDdevTypeSelect_CL73_AN  7

/****************************************************************************
 * Enums: sc_x1_speed_override1_aerMMDportSelect
 */
#define sc_x1_speed_override1_aerMMDportSelect_ln0         0
#define sc_x1_speed_override1_aerMMDportSelect_ln1         1
#define sc_x1_speed_override1_aerMMDportSelect_ln2         2
#define sc_x1_speed_override1_aerMMDportSelect_ln3         3
#define sc_x1_speed_override1_aerMMDportSelect_BCST_ln0_1_2_3 511
#define sc_x1_speed_override1_aerMMDportSelect_BCST_ln0_1  512
#define sc_x1_speed_override1_aerMMDportSelect_BCST_ln2_3  513

/****************************************************************************
 * Enums: sc_x1_speed_override1_firmwareModeSelect
 */
#define sc_x1_speed_override1_firmwareModeSelect_DEFAULT   0
#define sc_x1_speed_override1_firmwareModeSelect_SFP_OPT_LR 1
#define sc_x1_speed_override1_firmwareModeSelect_SFP_DAC   2
#define sc_x1_speed_override1_firmwareModeSelect_XLAUI     3
#define sc_x1_speed_override1_firmwareModeSelect_LONG_CH_6G 4

/****************************************************************************
 * Enums: sc_x1_speed_override1_tempIdxSelect
 */
#define sc_x1_speed_override1_tempIdxSelect_LTE__22p9C     15
#define sc_x1_speed_override1_tempIdxSelect_LTE__12p6C     14
#define sc_x1_speed_override1_tempIdxSelect_LTE__3p0C      13
#define sc_x1_speed_override1_tempIdxSelect_LTE_6p7C       12
#define sc_x1_speed_override1_tempIdxSelect_LTE_16p4C      11
#define sc_x1_speed_override1_tempIdxSelect_LTE_26p6C      10
#define sc_x1_speed_override1_tempIdxSelect_LTE_36p3C      9
#define sc_x1_speed_override1_tempIdxSelect_LTE_46p0C      8
#define sc_x1_speed_override1_tempIdxSelect_LTE_56p2C      7
#define sc_x1_speed_override1_tempIdxSelect_LTE_65p9C      6
#define sc_x1_speed_override1_tempIdxSelect_LTE_75p6C      5
#define sc_x1_speed_override1_tempIdxSelect_LTE_85p3C      4
#define sc_x1_speed_override1_tempIdxSelect_LTE_95p5C      3
#define sc_x1_speed_override1_tempIdxSelect_LTE_105p2C     2
#define sc_x1_speed_override1_tempIdxSelect_LTE_114p9C     1
#define sc_x1_speed_override1_tempIdxSelect_LTE_125p1C     0

/****************************************************************************
 * Enums: sc_x1_speed_override1_port_mode
 */
#define sc_x1_speed_override1_port_mode_QUAD_PORT          0
#define sc_x1_speed_override1_port_mode_TRI_1_PORT         1
#define sc_x1_speed_override1_port_mode_TRI_2_PORT         2
#define sc_x1_speed_override1_port_mode_DUAL_PORT          3
#define sc_x1_speed_override1_port_mode_SINGLE_PORT        4

/****************************************************************************
 * Enums: sc_x1_speed_override1_rev_letter_enum
 */
#define sc_x1_speed_override1_rev_letter_enum_REV_A        0
#define sc_x1_speed_override1_rev_letter_enum_REV_B        1
#define sc_x1_speed_override1_rev_letter_enum_REV_C        2
#define sc_x1_speed_override1_rev_letter_enum_REV_D        3

/****************************************************************************
 * Enums: sc_x1_speed_override1_rev_number_enum
 */
#define sc_x1_speed_override1_rev_number_enum_REV_0        0
#define sc_x1_speed_override1_rev_number_enum_REV_1        1
#define sc_x1_speed_override1_rev_number_enum_REV_2        2
#define sc_x1_speed_override1_rev_number_enum_REV_3        3
#define sc_x1_speed_override1_rev_number_enum_REV_4        4
#define sc_x1_speed_override1_rev_number_enum_REV_5        5
#define sc_x1_speed_override1_rev_number_enum_REV_6        6
#define sc_x1_speed_override1_rev_number_enum_REV_7        7

/****************************************************************************
 * Enums: sc_x1_speed_override1_bonding_enum
 */
#define sc_x1_speed_override1_bonding_enum_WIRE_BOND       0
#define sc_x1_speed_override1_bonding_enum_FLIP_CHIP       1

/****************************************************************************
 * Enums: sc_x1_speed_override1_tech_process
 */
#define sc_x1_speed_override1_tech_process_PROCESS_90NM    0
#define sc_x1_speed_override1_tech_process_PROCESS_65NM    1
#define sc_x1_speed_override1_tech_process_PROCESS_40NM    2
#define sc_x1_speed_override1_tech_process_PROCESS_28NM    3

/****************************************************************************
 * Enums: sc_x1_speed_override1_model_num
 */
#define sc_x1_speed_override1_model_num_SERDES_CL73        0
#define sc_x1_speed_override1_model_num_XGXS_16G           1
#define sc_x1_speed_override1_model_num_HYPERCORE          2
#define sc_x1_speed_override1_model_num_HYPERLITE          3
#define sc_x1_speed_override1_model_num_PCIE_G2_PIPE       4
#define sc_x1_speed_override1_model_num_SERDES_1p25GBd     5
#define sc_x1_speed_override1_model_num_SATA2              6
#define sc_x1_speed_override1_model_num_QSGMII             7
#define sc_x1_speed_override1_model_num_XGXS10G            8
#define sc_x1_speed_override1_model_num_WARPCORE           9
#define sc_x1_speed_override1_model_num_XFICORE            10
#define sc_x1_speed_override1_model_num_RXFI               11
#define sc_x1_speed_override1_model_num_WARPLITE           12
#define sc_x1_speed_override1_model_num_PENTACORE          13
#define sc_x1_speed_override1_model_num_ESM                14
#define sc_x1_speed_override1_model_num_QUAD_SGMII         15
#define sc_x1_speed_override1_model_num_WARPCORE_3         16
#define sc_x1_speed_override1_model_num_TSC                17
#define sc_x1_speed_override1_model_num_TSC4E              18
#define sc_x1_speed_override1_model_num_TSC12E             19
#define sc_x1_speed_override1_model_num_TSC4F              20
#define sc_x1_speed_override1_model_num_XGXS_CL73_90NM     29
#define sc_x1_speed_override1_model_num_SERDES_CL73_90NM   30
#define sc_x1_speed_override1_model_num_WARPCORE3          32
#define sc_x1_speed_override1_model_num_WARPCORE4_TSC      33
#define sc_x1_speed_override1_model_num_RXAUI              34

/****************************************************************************
 * Enums: sc_x1_speed_override1_payload
 */
#define sc_x1_speed_override1_payload_REPEAT_2_BYTES       0
#define sc_x1_speed_override1_payload_RAMPING              1
#define sc_x1_speed_override1_payload_CL48_CRPAT           2
#define sc_x1_speed_override1_payload_CL48_CJPAT           3
#define sc_x1_speed_override1_payload_CL36_LONG_CRPAT      4
#define sc_x1_speed_override1_payload_CL36_SHORT_CRPAT     5

/****************************************************************************
 * Enums: sc_x1_speed_override1_sc
 */
#define sc_x1_speed_override1_sc_S_10G_CR1                 0
#define sc_x1_speed_override1_sc_S_10G_KR1                 1
#define sc_x1_speed_override1_sc_S_10G_X1                  2
#define sc_x1_speed_override1_sc_S_10G_HG2_CR1             4
#define sc_x1_speed_override1_sc_S_10G_HG2_KR1             5
#define sc_x1_speed_override1_sc_S_10G_HG2_X1              6
#define sc_x1_speed_override1_sc_S_20G_CR1                 8
#define sc_x1_speed_override1_sc_S_20G_KR1                 9
#define sc_x1_speed_override1_sc_S_20G_X1                  10
#define sc_x1_speed_override1_sc_S_20G_HG2_CR1             12
#define sc_x1_speed_override1_sc_S_20G_HG2_KR1             13
#define sc_x1_speed_override1_sc_S_20G_HG2_X1              14
#define sc_x1_speed_override1_sc_S_25G_CR1                 16
#define sc_x1_speed_override1_sc_S_25G_KR1                 17
#define sc_x1_speed_override1_sc_S_25G_X1                  18
#define sc_x1_speed_override1_sc_S_25G_HG2_CR1             20
#define sc_x1_speed_override1_sc_S_25G_HG2_KR1             21
#define sc_x1_speed_override1_sc_S_25G_HG2_X1              22
#define sc_x1_speed_override1_sc_S_20G_CR2                 24
#define sc_x1_speed_override1_sc_S_20G_KR2                 25
#define sc_x1_speed_override1_sc_S_20G_X2                  26
#define sc_x1_speed_override1_sc_S_20G_HG2_CR2             28
#define sc_x1_speed_override1_sc_S_20G_HG2_KR2             29
#define sc_x1_speed_override1_sc_S_20G_HG2_X2              30
#define sc_x1_speed_override1_sc_S_40G_CR2                 32
#define sc_x1_speed_override1_sc_S_40G_KR2                 33
#define sc_x1_speed_override1_sc_S_40G_X2                  34
#define sc_x1_speed_override1_sc_S_40G_HG2_CR2             36
#define sc_x1_speed_override1_sc_S_40G_HG2_KR2             37
#define sc_x1_speed_override1_sc_S_40G_HG2_X2              38
#define sc_x1_speed_override1_sc_S_40G_CR4                 40
#define sc_x1_speed_override1_sc_S_40G_KR4                 41
#define sc_x1_speed_override1_sc_S_40G_X4                  42
#define sc_x1_speed_override1_sc_S_40G_HG2_CR4             44
#define sc_x1_speed_override1_sc_S_40G_HG2_KR4             45
#define sc_x1_speed_override1_sc_S_40G_HG2_X4              46
#define sc_x1_speed_override1_sc_S_50G_CR2                 48
#define sc_x1_speed_override1_sc_S_50G_KR2                 49
#define sc_x1_speed_override1_sc_S_50G_X2                  50
#define sc_x1_speed_override1_sc_S_50G_HG2_CR2             52
#define sc_x1_speed_override1_sc_S_50G_HG2_KR2             53
#define sc_x1_speed_override1_sc_S_50G_HG2_X2              54
#define sc_x1_speed_override1_sc_S_50G_CR4                 56
#define sc_x1_speed_override1_sc_S_50G_KR4                 57
#define sc_x1_speed_override1_sc_S_50G_X4                  58
#define sc_x1_speed_override1_sc_S_50G_HG2_CR4             60
#define sc_x1_speed_override1_sc_S_50G_HG2_KR4             61
#define sc_x1_speed_override1_sc_S_50G_HG2_X4              62
#define sc_x1_speed_override1_sc_S_100G_CR4                64
#define sc_x1_speed_override1_sc_S_100G_KR4                65
#define sc_x1_speed_override1_sc_S_100G_X4                 66
#define sc_x1_speed_override1_sc_S_100G_HG2_CR4            68
#define sc_x1_speed_override1_sc_S_100G_HG2_KR4            69
#define sc_x1_speed_override1_sc_S_100G_HG2_X4             70
#define sc_x1_speed_override1_sc_S_CL73_20GVCO             72
#define sc_x1_speed_override1_sc_S_CL73_25GVCO             80
#define sc_x1_speed_override1_sc_S_CL36_20GVCO             88
#define sc_x1_speed_override1_sc_S_CL36_25GVCO             96

/****************************************************************************
 * Enums: sc_x1_speed_override1_t_fifo_modes
 */
#define sc_x1_speed_override1_t_fifo_modes_T_FIFO_MODE_BYPASS 0
#define sc_x1_speed_override1_t_fifo_modes_T_FIFO_MODE_40G 1
#define sc_x1_speed_override1_t_fifo_modes_T_FIFO_MODE_100G 2
#define sc_x1_speed_override1_t_fifo_modes_T_FIFO_MODE_20G 3

/****************************************************************************
 * Enums: sc_x1_speed_override1_t_enc_modes
 */
#define sc_x1_speed_override1_t_enc_modes_T_ENC_MODE_BYPASS 0
#define sc_x1_speed_override1_t_enc_modes_T_ENC_MODE_CL49  1
#define sc_x1_speed_override1_t_enc_modes_T_ENC_MODE_CL82  2

/****************************************************************************
 * Enums: sc_x1_speed_override1_btmx_mode
 */
#define sc_x1_speed_override1_btmx_mode_BS_BTMX_MODE_1to1  0
#define sc_x1_speed_override1_btmx_mode_BS_BTMX_MODE_2to1  1
#define sc_x1_speed_override1_btmx_mode_BS_BTMX_MODE_5to1  2

/****************************************************************************
 * Enums: sc_x1_speed_override1_t_type_cl82
 */
#define sc_x1_speed_override1_t_type_cl82_T_TYPE_B1        5
#define sc_x1_speed_override1_t_type_cl82_T_TYPE_C         4
#define sc_x1_speed_override1_t_type_cl82_T_TYPE_S         3
#define sc_x1_speed_override1_t_type_cl82_T_TYPE_T         2
#define sc_x1_speed_override1_t_type_cl82_T_TYPE_D         1
#define sc_x1_speed_override1_t_type_cl82_T_TYPE_E         0

/****************************************************************************
 * Enums: sc_x1_speed_override1_txsm_state_cl82
 */
#define sc_x1_speed_override1_txsm_state_cl82_TX_HIG_END   6
#define sc_x1_speed_override1_txsm_state_cl82_TX_HIG_START 5
#define sc_x1_speed_override1_txsm_state_cl82_TX_E         4
#define sc_x1_speed_override1_txsm_state_cl82_TX_T         3
#define sc_x1_speed_override1_txsm_state_cl82_TX_D         2
#define sc_x1_speed_override1_txsm_state_cl82_TX_C         1
#define sc_x1_speed_override1_txsm_state_cl82_TX_INIT      0

/****************************************************************************
 * Enums: sc_x1_speed_override1_ltxsm_state_cl82
 */
#define sc_x1_speed_override1_ltxsm_state_cl82_TX_HIG_END  64
#define sc_x1_speed_override1_ltxsm_state_cl82_TX_HIG_START 32
#define sc_x1_speed_override1_ltxsm_state_cl82_TX_E        16
#define sc_x1_speed_override1_ltxsm_state_cl82_TX_T        8
#define sc_x1_speed_override1_ltxsm_state_cl82_TX_D        4
#define sc_x1_speed_override1_ltxsm_state_cl82_TX_C        2
#define sc_x1_speed_override1_ltxsm_state_cl82_TX_INIT     1

/****************************************************************************
 * Enums: sc_x1_speed_override1_r_type_coded_cl82
 */
#define sc_x1_speed_override1_r_type_coded_cl82_R_TYPE_B1  32
#define sc_x1_speed_override1_r_type_coded_cl82_R_TYPE_C   16
#define sc_x1_speed_override1_r_type_coded_cl82_R_TYPE_S   8
#define sc_x1_speed_override1_r_type_coded_cl82_R_TYPE_T   4
#define sc_x1_speed_override1_r_type_coded_cl82_R_TYPE_D   2
#define sc_x1_speed_override1_r_type_coded_cl82_R_TYPE_E   1

/****************************************************************************
 * Enums: sc_x1_speed_override1_rxsm_state_cl82
 */
#define sc_x1_speed_override1_rxsm_state_cl82_RX_HIG_END   64
#define sc_x1_speed_override1_rxsm_state_cl82_RX_HIG_START 32
#define sc_x1_speed_override1_rxsm_state_cl82_RX_E         16
#define sc_x1_speed_override1_rxsm_state_cl82_RX_T         8
#define sc_x1_speed_override1_rxsm_state_cl82_RX_D         4
#define sc_x1_speed_override1_rxsm_state_cl82_RX_C         2
#define sc_x1_speed_override1_rxsm_state_cl82_RX_INIT      1

/****************************************************************************
 * Enums: sc_x1_speed_override1_deskew_state
 */
#define sc_x1_speed_override1_deskew_state_ALIGN_ACQUIRED  2
#define sc_x1_speed_override1_deskew_state_LOSS_OF_ALIGNMENT 1

/****************************************************************************
 * Enums: sc_x1_speed_override1_os_mode_enum
 */
#define sc_x1_speed_override1_os_mode_enum_OS_MODE_1       0
#define sc_x1_speed_override1_os_mode_enum_OS_MODE_2       1
#define sc_x1_speed_override1_os_mode_enum_OS_MODE_4       2
#define sc_x1_speed_override1_os_mode_enum_OS_MODE_16p5    8
#define sc_x1_speed_override1_os_mode_enum_OS_MODE_20p625  12

/****************************************************************************
 * Enums: sc_x1_speed_override1_scr_modes
 */
#define sc_x1_speed_override1_scr_modes_T_SCR_MODE_BYPASS  0
#define sc_x1_speed_override1_scr_modes_T_SCR_MODE_CL49    1
#define sc_x1_speed_override1_scr_modes_T_SCR_MODE_40G_2_LANE 2
#define sc_x1_speed_override1_scr_modes_T_SCR_MODE_100G    3
#define sc_x1_speed_override1_scr_modes_T_SCR_MODE_20G     4
#define sc_x1_speed_override1_scr_modes_T_SCR_MODE_40G_4_LANE 5

/****************************************************************************
 * Enums: sc_x1_speed_override1_descr_modes
 */
#define sc_x1_speed_override1_descr_modes_R_DESCR_MODE_BYPASS 0
#define sc_x1_speed_override1_descr_modes_R_DESCR_MODE_CL49 1
#define sc_x1_speed_override1_descr_modes_R_DESCR_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override1_r_dec_tl_mode
 */
#define sc_x1_speed_override1_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS 0
#define sc_x1_speed_override1_r_dec_tl_mode_R_DEC_TL_MODE_CL49 1
#define sc_x1_speed_override1_r_dec_tl_mode_R_DEC_TL_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override1_r_dec_fsm_mode
 */
#define sc_x1_speed_override1_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define sc_x1_speed_override1_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49 1
#define sc_x1_speed_override1_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override1_r_deskew_mode
 */
#define sc_x1_speed_override1_r_deskew_mode_R_DESKEW_MODE_BYPASS 0
#define sc_x1_speed_override1_r_deskew_mode_R_DESKEW_MODE_20G 1
#define sc_x1_speed_override1_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define sc_x1_speed_override1_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define sc_x1_speed_override1_r_deskew_mode_R_DESKEW_MODE_100G 4
#define sc_x1_speed_override1_r_deskew_mode_R_DESKEW_MODE_CL49 5
#define sc_x1_speed_override1_r_deskew_mode_R_DESKEW_MODE_CL91 6

/****************************************************************************
 * Enums: sc_x1_speed_override1_bs_dist_modes
 */
#define sc_x1_speed_override1_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define sc_x1_speed_override1_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define sc_x1_speed_override1_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define sc_x1_speed_override1_bs_dist_modes_BS_DIST_MODE_NO_TDM 3

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl49_t_type
 */
#define sc_x1_speed_override1_cl49_t_type_BAD_T_TYPE       15
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_B0        11
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_OB        10
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_B1        9
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_DB        8
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_FC        7
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_TB        6
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_LI        5
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_C         4
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_S         3
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_T         2
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_D         1
#define sc_x1_speed_override1_cl49_t_type_T_TYPE_E         0

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl49_txsm_states
 */
#define sc_x1_speed_override1_cl49_txsm_states_TX_HIG_END  7
#define sc_x1_speed_override1_cl49_txsm_states_TX_HIG_START 6
#define sc_x1_speed_override1_cl49_txsm_states_TX_LI       5
#define sc_x1_speed_override1_cl49_txsm_states_TX_E        4
#define sc_x1_speed_override1_cl49_txsm_states_TX_T        3
#define sc_x1_speed_override1_cl49_txsm_states_TX_D        2
#define sc_x1_speed_override1_cl49_txsm_states_TX_C        1
#define sc_x1_speed_override1_cl49_txsm_states_TX_INIT     0

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl49_ltxsm_states
 */
#define sc_x1_speed_override1_cl49_ltxsm_states_TX_HIG_END 128
#define sc_x1_speed_override1_cl49_ltxsm_states_TX_HIG_START 64
#define sc_x1_speed_override1_cl49_ltxsm_states_TX_LI      32
#define sc_x1_speed_override1_cl49_ltxsm_states_TX_E       16
#define sc_x1_speed_override1_cl49_ltxsm_states_TX_T       8
#define sc_x1_speed_override1_cl49_ltxsm_states_TX_D       4
#define sc_x1_speed_override1_cl49_ltxsm_states_TX_C       2
#define sc_x1_speed_override1_cl49_ltxsm_states_TX_INIT    1

/****************************************************************************
 * Enums: sc_x1_speed_override1_burst_error_mode
 */
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_11_BITS 0
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_16_BITS 1
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_17_BITS 2
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_18_BITS 3
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_19_BITS 4
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_20_BITS 5
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_21_BITS 6
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_22_BITS 7
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_23_BITS 8
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_24_BITS 9
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_25_BITS 10
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_26_BITS 11
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_27_BITS 12
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_28_BITS 13
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_29_BITS 14
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_30_BITS 15
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_31_BITS 16
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_32_BITS 17
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_33_BITS 18
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_34_BITS 19
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_35_BITS 20
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_36_BITS 21
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_37_BITS 22
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_38_BITS 23
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_39_BITS 24
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_40_BITS 25
#define sc_x1_speed_override1_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: sc_x1_speed_override1_bermon_state
 */
#define sc_x1_speed_override1_bermon_state_HI_BER          4
#define sc_x1_speed_override1_bermon_state_GOOD_BER        3
#define sc_x1_speed_override1_bermon_state_BER_TEST_SH     2
#define sc_x1_speed_override1_bermon_state_START_TIMER     1
#define sc_x1_speed_override1_bermon_state_BER_MT_INIT     0

/****************************************************************************
 * Enums: sc_x1_speed_override1_rxsm_state_cl49
 */
#define sc_x1_speed_override1_rxsm_state_cl49_RX_HIG_END   128
#define sc_x1_speed_override1_rxsm_state_cl49_RX_HIG_START 64
#define sc_x1_speed_override1_rxsm_state_cl49_RX_LI        32
#define sc_x1_speed_override1_rxsm_state_cl49_RX_E         16
#define sc_x1_speed_override1_rxsm_state_cl49_RX_T         8
#define sc_x1_speed_override1_rxsm_state_cl49_RX_D         4
#define sc_x1_speed_override1_rxsm_state_cl49_RX_C         2
#define sc_x1_speed_override1_rxsm_state_cl49_RX_INIT      1

/****************************************************************************
 * Enums: sc_x1_speed_override1_r_type
 */
#define sc_x1_speed_override1_r_type_BAD_R_TYPE            15
#define sc_x1_speed_override1_r_type_R_TYPE_B0             11
#define sc_x1_speed_override1_r_type_R_TYPE_OB             10
#define sc_x1_speed_override1_r_type_R_TYPE_B1             9
#define sc_x1_speed_override1_r_type_R_TYPE_DB             8
#define sc_x1_speed_override1_r_type_R_TYPE_FC             7
#define sc_x1_speed_override1_r_type_R_TYPE_TB             6
#define sc_x1_speed_override1_r_type_R_TYPE_LI             5
#define sc_x1_speed_override1_r_type_R_TYPE_C              4
#define sc_x1_speed_override1_r_type_R_TYPE_S              3
#define sc_x1_speed_override1_r_type_R_TYPE_T              2
#define sc_x1_speed_override1_r_type_R_TYPE_D              1
#define sc_x1_speed_override1_r_type_R_TYPE_E              0

/****************************************************************************
 * Enums: sc_x1_speed_override1_am_lock_state
 */
#define sc_x1_speed_override1_am_lock_state_INVALID_AM     512
#define sc_x1_speed_override1_am_lock_state_GOOD_AM        256
#define sc_x1_speed_override1_am_lock_state_COMP_AM        128
#define sc_x1_speed_override1_am_lock_state_TIMER_2        64
#define sc_x1_speed_override1_am_lock_state_AM_2_GOOD      32
#define sc_x1_speed_override1_am_lock_state_COMP_2ND       16
#define sc_x1_speed_override1_am_lock_state_TIMER_1        8
#define sc_x1_speed_override1_am_lock_state_FIND_1ST       4
#define sc_x1_speed_override1_am_lock_state_AM_RESET_CNT   2
#define sc_x1_speed_override1_am_lock_state_AM_LOCK_INIT   1

/****************************************************************************
 * Enums: sc_x1_speed_override1_msg_selector
 */
#define sc_x1_speed_override1_msg_selector_RESERVED        0
#define sc_x1_speed_override1_msg_selector_VALUE_802p3     1
#define sc_x1_speed_override1_msg_selector_VALUE_802p9     2
#define sc_x1_speed_override1_msg_selector_VALUE_802p5     3
#define sc_x1_speed_override1_msg_selector_VALUE_1394      4

/****************************************************************************
 * Enums: sc_x1_speed_override1_synce_enum
 */
#define sc_x1_speed_override1_synce_enum_SYNCE_NO_DIV      0
#define sc_x1_speed_override1_synce_enum_SYNCE_DIV_7       1
#define sc_x1_speed_override1_synce_enum_SYNCE_DIV_11      2

/****************************************************************************
 * Enums: sc_x1_speed_override1_synce_enum_stage0
 */
#define sc_x1_speed_override1_synce_enum_stage0_SYNCE_NO_DIV 0
#define sc_x1_speed_override1_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define sc_x1_speed_override1_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl91_sync_state
 */
#define sc_x1_speed_override1_cl91_sync_state_FIND_1ST     0
#define sc_x1_speed_override1_cl91_sync_state_COUNT_NEXT   1
#define sc_x1_speed_override1_cl91_sync_state_COMP_2ND     2
#define sc_x1_speed_override1_cl91_sync_state_TWO_GOOD     3

/****************************************************************************
 * Enums: sc_x1_speed_override1_cl91_algn_state
 */
#define sc_x1_speed_override1_cl91_algn_state_LOSS_OF_ALIGNMENT 0
#define sc_x1_speed_override1_cl91_algn_state_DESKEW       1
#define sc_x1_speed_override1_cl91_algn_state_DESKEW_FAIL  2
#define sc_x1_speed_override1_cl91_algn_state_ALIGN_ACQUIRED 3
#define sc_x1_speed_override1_cl91_algn_state_CW_GOOD      4
#define sc_x1_speed_override1_cl91_algn_state_CW_BAD       5
#define sc_x1_speed_override1_cl91_algn_state_THREE_BAD    6

/****************************************************************************
 * Enums: sc_x1_speed_override1_fec_sel
 */
#define sc_x1_speed_override1_fec_sel_NO_FEC               0
#define sc_x1_speed_override1_fec_sel_FEC_CL74             1
#define sc_x1_speed_override1_fec_sel_FEC_CL91             2

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl36TxEEEStates_l
 */
#define sc_x1_speed_override2_cl36TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override2_cl36TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override2_cl36TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override2_cl36TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl36TxEEEStates_c
 */
#define sc_x1_speed_override2_cl36TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override2_cl36TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override2_cl36TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override2_cl36TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl49TxEEEStates_l
 */
#define sc_x1_speed_override2_cl49TxEEEStates_l_SCR_RESET_2 64
#define sc_x1_speed_override2_cl49TxEEEStates_l_SCR_RESET_1 32
#define sc_x1_speed_override2_cl49TxEEEStates_l_TX_WAKE    16
#define sc_x1_speed_override2_cl49TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override2_cl49TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override2_cl49TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override2_cl49TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override2_fec_req_enum
 */
#define sc_x1_speed_override2_fec_req_enum_FEC_not_supported 0
#define sc_x1_speed_override2_fec_req_enum_FEC_supported_but_not_requested 1
#define sc_x1_speed_override2_fec_req_enum_invalid_setting 2
#define sc_x1_speed_override2_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl73_pause_enum
 */
#define sc_x1_speed_override2_cl73_pause_enum_No_PAUSE_ability 0
#define sc_x1_speed_override2_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define sc_x1_speed_override2_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define sc_x1_speed_override2_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl49TxEEEStates_c
 */
#define sc_x1_speed_override2_cl49TxEEEStates_c_SCR_RESET_2 6
#define sc_x1_speed_override2_cl49TxEEEStates_c_SCR_RESET_1 5
#define sc_x1_speed_override2_cl49TxEEEStates_c_TX_WAKE    4
#define sc_x1_speed_override2_cl49TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override2_cl49TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override2_cl49TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override2_cl49TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl36RxEEEStates_l
 */
#define sc_x1_speed_override2_cl36RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override2_cl36RxEEEStates_l_RX_WTF     16
#define sc_x1_speed_override2_cl36RxEEEStates_l_RX_WAKE    8
#define sc_x1_speed_override2_cl36RxEEEStates_l_RX_QUIET   4
#define sc_x1_speed_override2_cl36RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override2_cl36RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl36RxEEEStates_c
 */
#define sc_x1_speed_override2_cl36RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override2_cl36RxEEEStates_c_RX_WTF     4
#define sc_x1_speed_override2_cl36RxEEEStates_c_RX_WAKE    3
#define sc_x1_speed_override2_cl36RxEEEStates_c_RX_QUIET   2
#define sc_x1_speed_override2_cl36RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override2_cl36RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl49RxEEEStates_l
 */
#define sc_x1_speed_override2_cl49RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override2_cl49RxEEEStates_l_RX_WTF     16
#define sc_x1_speed_override2_cl49RxEEEStates_l_RX_WAKE    8
#define sc_x1_speed_override2_cl49RxEEEStates_l_RX_QUIET   4
#define sc_x1_speed_override2_cl49RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override2_cl49RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl49RxEEEStates_c
 */
#define sc_x1_speed_override2_cl49RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override2_cl49RxEEEStates_c_RX_WTF     4
#define sc_x1_speed_override2_cl49RxEEEStates_c_RX_WAKE    3
#define sc_x1_speed_override2_cl49RxEEEStates_c_RX_QUIET   2
#define sc_x1_speed_override2_cl49RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override2_cl49RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl48TxEEEStates_l
 */
#define sc_x1_speed_override2_cl48TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override2_cl48TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override2_cl48TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override2_cl48TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl48TxEEEStates_c
 */
#define sc_x1_speed_override2_cl48TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override2_cl48TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override2_cl48TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override2_cl48TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl48RxEEEStates_l
 */
#define sc_x1_speed_override2_cl48RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override2_cl48RxEEEStates_l_RX_WAKE    16
#define sc_x1_speed_override2_cl48RxEEEStates_l_RX_QUIET   8
#define sc_x1_speed_override2_cl48RxEEEStates_l_RX_DEACT   4
#define sc_x1_speed_override2_cl48RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override2_cl48RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl48RxEEEStates_c
 */
#define sc_x1_speed_override2_cl48RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override2_cl48RxEEEStates_c_RX_WAKE    4
#define sc_x1_speed_override2_cl48RxEEEStates_c_RX_QUIET   3
#define sc_x1_speed_override2_cl48RxEEEStates_c_RX_DEACT   2
#define sc_x1_speed_override2_cl48RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override2_cl48RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override2_IQP_Options
 */
#define sc_x1_speed_override2_IQP_Options_i50uA            0
#define sc_x1_speed_override2_IQP_Options_i100uA           1
#define sc_x1_speed_override2_IQP_Options_i150uA           2
#define sc_x1_speed_override2_IQP_Options_i200uA           3
#define sc_x1_speed_override2_IQP_Options_i250uA           4
#define sc_x1_speed_override2_IQP_Options_i300uA           5
#define sc_x1_speed_override2_IQP_Options_i350uA           6
#define sc_x1_speed_override2_IQP_Options_i400uA           7
#define sc_x1_speed_override2_IQP_Options_i450uA           8
#define sc_x1_speed_override2_IQP_Options_i500uA           9
#define sc_x1_speed_override2_IQP_Options_i550uA           10
#define sc_x1_speed_override2_IQP_Options_i600uA           11
#define sc_x1_speed_override2_IQP_Options_i650uA           12
#define sc_x1_speed_override2_IQP_Options_i700uA           13
#define sc_x1_speed_override2_IQP_Options_i750uA           14
#define sc_x1_speed_override2_IQP_Options_i800uA           15

/****************************************************************************
 * Enums: sc_x1_speed_override2_IDriver_Options
 */
#define sc_x1_speed_override2_IDriver_Options_v680mV       0
#define sc_x1_speed_override2_IDriver_Options_v730mV       1
#define sc_x1_speed_override2_IDriver_Options_v780mV       2
#define sc_x1_speed_override2_IDriver_Options_v830mV       3
#define sc_x1_speed_override2_IDriver_Options_v880mV       4
#define sc_x1_speed_override2_IDriver_Options_v930mV       5
#define sc_x1_speed_override2_IDriver_Options_v980mV       6
#define sc_x1_speed_override2_IDriver_Options_v1010mV      7
#define sc_x1_speed_override2_IDriver_Options_v1040mV      8
#define sc_x1_speed_override2_IDriver_Options_v1060mV      9
#define sc_x1_speed_override2_IDriver_Options_v1070mV      10
#define sc_x1_speed_override2_IDriver_Options_v1080mV      11
#define sc_x1_speed_override2_IDriver_Options_v1085mV      12
#define sc_x1_speed_override2_IDriver_Options_v1090mV      13
#define sc_x1_speed_override2_IDriver_Options_v1095mV      14
#define sc_x1_speed_override2_IDriver_Options_v1100mV      15

/****************************************************************************
 * Enums: sc_x1_speed_override2_operationModes
 */
#define sc_x1_speed_override2_operationModes_XGXS          0
#define sc_x1_speed_override2_operationModes_XGXG_nCC      1
#define sc_x1_speed_override2_operationModes_Indlane_OS8   4
#define sc_x1_speed_override2_operationModes_IndLane_OS5   5
#define sc_x1_speed_override2_operationModes_PCI           7
#define sc_x1_speed_override2_operationModes_XGXS_nLQ      8
#define sc_x1_speed_override2_operationModes_XGXS_nLQnCC   9
#define sc_x1_speed_override2_operationModes_PBypass       10
#define sc_x1_speed_override2_operationModes_PBypass_nDSK  11
#define sc_x1_speed_override2_operationModes_ComboCoreMode 12
#define sc_x1_speed_override2_operationModes_Clocks_off    15

/****************************************************************************
 * Enums: sc_x1_speed_override2_actualSpeeds
 */
#define sc_x1_speed_override2_actualSpeeds_dr_10M          0
#define sc_x1_speed_override2_actualSpeeds_dr_100M         1
#define sc_x1_speed_override2_actualSpeeds_dr_1G           2
#define sc_x1_speed_override2_actualSpeeds_dr_2p5G         3
#define sc_x1_speed_override2_actualSpeeds_dr_5G_X4        4
#define sc_x1_speed_override2_actualSpeeds_dr_6G_X4        5
#define sc_x1_speed_override2_actualSpeeds_dr_10G_HiG      6
#define sc_x1_speed_override2_actualSpeeds_dr_10G_CX4      7
#define sc_x1_speed_override2_actualSpeeds_dr_12G_HiG      8
#define sc_x1_speed_override2_actualSpeeds_dr_12p5G_X4     9
#define sc_x1_speed_override2_actualSpeeds_dr_13G_X4       10
#define sc_x1_speed_override2_actualSpeeds_dr_15G_X4       11
#define sc_x1_speed_override2_actualSpeeds_dr_16G_X4       12
#define sc_x1_speed_override2_actualSpeeds_dr_1G_KX        13
#define sc_x1_speed_override2_actualSpeeds_dr_10G_KX4      14
#define sc_x1_speed_override2_actualSpeeds_dr_10G_KR       15
#define sc_x1_speed_override2_actualSpeeds_dr_5G           16
#define sc_x1_speed_override2_actualSpeeds_dr_6p4G         17
#define sc_x1_speed_override2_actualSpeeds_dr_20G_X4       18
#define sc_x1_speed_override2_actualSpeeds_dr_21G_X4       19
#define sc_x1_speed_override2_actualSpeeds_dr_25G_X4       20
#define sc_x1_speed_override2_actualSpeeds_dr_10G_HiG_DXGXS 21
#define sc_x1_speed_override2_actualSpeeds_dr_10G_DXGXS    22
#define sc_x1_speed_override2_actualSpeeds_dr_10p5G_HiG_DXGXS 23
#define sc_x1_speed_override2_actualSpeeds_dr_10p5G_DXGXS  24
#define sc_x1_speed_override2_actualSpeeds_dr_12p773G_HiG_DXGXS 25
#define sc_x1_speed_override2_actualSpeeds_dr_12p773G_DXGXS 26
#define sc_x1_speed_override2_actualSpeeds_dr_10G_XFI      27
#define sc_x1_speed_override2_actualSpeeds_dr_40G          28
#define sc_x1_speed_override2_actualSpeeds_dr_20G_HiG_DXGXS 29
#define sc_x1_speed_override2_actualSpeeds_dr_20G_DXGXS    30
#define sc_x1_speed_override2_actualSpeeds_dr_10G_SFI      31
#define sc_x1_speed_override2_actualSpeeds_dr_31p5G        32
#define sc_x1_speed_override2_actualSpeeds_dr_32p7G        33
#define sc_x1_speed_override2_actualSpeeds_dr_20G_SCR      34
#define sc_x1_speed_override2_actualSpeeds_dr_10G_HiG_DXGXS_SCR 35
#define sc_x1_speed_override2_actualSpeeds_dr_10G_DXGXS_SCR 36
#define sc_x1_speed_override2_actualSpeeds_dr_12G_R2       37
#define sc_x1_speed_override2_actualSpeeds_dr_10G_X2       38
#define sc_x1_speed_override2_actualSpeeds_dr_40G_KR4      39
#define sc_x1_speed_override2_actualSpeeds_dr_40G_CR4      40
#define sc_x1_speed_override2_actualSpeeds_dr_100G_CR10    41
#define sc_x1_speed_override2_actualSpeeds_dr_15p75G_DXGXS 44
#define sc_x1_speed_override2_actualSpeeds_dr_20G_KR2      57
#define sc_x1_speed_override2_actualSpeeds_dr_20G_CR2      58

/****************************************************************************
 * Enums: sc_x1_speed_override2_actualSpeedsMisc1
 */
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_2500BRCM_X1 16
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_5000BRCM_X4 17
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_6000BRCM_X4 18
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_10GHiGig_X4 19
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_10GBASE_CX4 20
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_12GHiGig_X4 21
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_12p5GHiGig_X4 22
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_13GHiGig_X4 23
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_15GHiGig_X4 24
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_16GHiGig_X4 25
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_5000BRCM_X1 26
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_6363BRCM_X1 27
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_20GHiGig_X4 28
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_21GHiGig_X4 29
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define sc_x1_speed_override2_actualSpeedsMisc1_dr_10G_HiG_DXGXS 31

/****************************************************************************
 * Enums: sc_x1_speed_override2_IndLaneModes
 */
#define sc_x1_speed_override2_IndLaneModes_SWSDR_div2      0
#define sc_x1_speed_override2_IndLaneModes_SWSDR_div1      1
#define sc_x1_speed_override2_IndLaneModes_DWSDR_div2      2
#define sc_x1_speed_override2_IndLaneModes_DWSDR_div1      3

/****************************************************************************
 * Enums: sc_x1_speed_override2_prbsSelect
 */
#define sc_x1_speed_override2_prbsSelect_prbs7             0
#define sc_x1_speed_override2_prbsSelect_prbs15            1
#define sc_x1_speed_override2_prbsSelect_prbs23            2
#define sc_x1_speed_override2_prbsSelect_prbs31            3

/****************************************************************************
 * Enums: sc_x1_speed_override2_vcoDivider
 */
#define sc_x1_speed_override2_vcoDivider_div32             0
#define sc_x1_speed_override2_vcoDivider_div36             1
#define sc_x1_speed_override2_vcoDivider_div40             2
#define sc_x1_speed_override2_vcoDivider_div42             3
#define sc_x1_speed_override2_vcoDivider_div48             4
#define sc_x1_speed_override2_vcoDivider_div50             5
#define sc_x1_speed_override2_vcoDivider_div52             6
#define sc_x1_speed_override2_vcoDivider_div54             7
#define sc_x1_speed_override2_vcoDivider_div60             8
#define sc_x1_speed_override2_vcoDivider_div64             9
#define sc_x1_speed_override2_vcoDivider_div66             10
#define sc_x1_speed_override2_vcoDivider_div68             11
#define sc_x1_speed_override2_vcoDivider_div70             12
#define sc_x1_speed_override2_vcoDivider_div80             13
#define sc_x1_speed_override2_vcoDivider_div92             14
#define sc_x1_speed_override2_vcoDivider_div100            15

/****************************************************************************
 * Enums: sc_x1_speed_override2_refClkSelect
 */
#define sc_x1_speed_override2_refClkSelect_clk_25MHz       0
#define sc_x1_speed_override2_refClkSelect_clk_100MHz      1
#define sc_x1_speed_override2_refClkSelect_clk_125MHz      2
#define sc_x1_speed_override2_refClkSelect_clk_156p25MHz   3
#define sc_x1_speed_override2_refClkSelect_clk_187p5MHz    4
#define sc_x1_speed_override2_refClkSelect_clk_161p25Mhz   5
#define sc_x1_speed_override2_refClkSelect_clk_50Mhz       6
#define sc_x1_speed_override2_refClkSelect_clk_106p25Mhz   7

/****************************************************************************
 * Enums: sc_x1_speed_override2_aerMMDdevTypeSelect
 */
#define sc_x1_speed_override2_aerMMDdevTypeSelect_combo_core 0
#define sc_x1_speed_override2_aerMMDdevTypeSelect_PMA_PMD  1
#define sc_x1_speed_override2_aerMMDdevTypeSelect_PCS      3
#define sc_x1_speed_override2_aerMMDdevTypeSelect_PHY      4
#define sc_x1_speed_override2_aerMMDdevTypeSelect_DTE      5
#define sc_x1_speed_override2_aerMMDdevTypeSelect_CL73_AN  7

/****************************************************************************
 * Enums: sc_x1_speed_override2_aerMMDportSelect
 */
#define sc_x1_speed_override2_aerMMDportSelect_ln0         0
#define sc_x1_speed_override2_aerMMDportSelect_ln1         1
#define sc_x1_speed_override2_aerMMDportSelect_ln2         2
#define sc_x1_speed_override2_aerMMDportSelect_ln3         3
#define sc_x1_speed_override2_aerMMDportSelect_BCST_ln0_1_2_3 511
#define sc_x1_speed_override2_aerMMDportSelect_BCST_ln0_1  512
#define sc_x1_speed_override2_aerMMDportSelect_BCST_ln2_3  513

/****************************************************************************
 * Enums: sc_x1_speed_override2_firmwareModeSelect
 */
#define sc_x1_speed_override2_firmwareModeSelect_DEFAULT   0
#define sc_x1_speed_override2_firmwareModeSelect_SFP_OPT_LR 1
#define sc_x1_speed_override2_firmwareModeSelect_SFP_DAC   2
#define sc_x1_speed_override2_firmwareModeSelect_XLAUI     3
#define sc_x1_speed_override2_firmwareModeSelect_LONG_CH_6G 4

/****************************************************************************
 * Enums: sc_x1_speed_override2_tempIdxSelect
 */
#define sc_x1_speed_override2_tempIdxSelect_LTE__22p9C     15
#define sc_x1_speed_override2_tempIdxSelect_LTE__12p6C     14
#define sc_x1_speed_override2_tempIdxSelect_LTE__3p0C      13
#define sc_x1_speed_override2_tempIdxSelect_LTE_6p7C       12
#define sc_x1_speed_override2_tempIdxSelect_LTE_16p4C      11
#define sc_x1_speed_override2_tempIdxSelect_LTE_26p6C      10
#define sc_x1_speed_override2_tempIdxSelect_LTE_36p3C      9
#define sc_x1_speed_override2_tempIdxSelect_LTE_46p0C      8
#define sc_x1_speed_override2_tempIdxSelect_LTE_56p2C      7
#define sc_x1_speed_override2_tempIdxSelect_LTE_65p9C      6
#define sc_x1_speed_override2_tempIdxSelect_LTE_75p6C      5
#define sc_x1_speed_override2_tempIdxSelect_LTE_85p3C      4
#define sc_x1_speed_override2_tempIdxSelect_LTE_95p5C      3
#define sc_x1_speed_override2_tempIdxSelect_LTE_105p2C     2
#define sc_x1_speed_override2_tempIdxSelect_LTE_114p9C     1
#define sc_x1_speed_override2_tempIdxSelect_LTE_125p1C     0

/****************************************************************************
 * Enums: sc_x1_speed_override2_port_mode
 */
#define sc_x1_speed_override2_port_mode_QUAD_PORT          0
#define sc_x1_speed_override2_port_mode_TRI_1_PORT         1
#define sc_x1_speed_override2_port_mode_TRI_2_PORT         2
#define sc_x1_speed_override2_port_mode_DUAL_PORT          3
#define sc_x1_speed_override2_port_mode_SINGLE_PORT        4

/****************************************************************************
 * Enums: sc_x1_speed_override2_rev_letter_enum
 */
#define sc_x1_speed_override2_rev_letter_enum_REV_A        0
#define sc_x1_speed_override2_rev_letter_enum_REV_B        1
#define sc_x1_speed_override2_rev_letter_enum_REV_C        2
#define sc_x1_speed_override2_rev_letter_enum_REV_D        3

/****************************************************************************
 * Enums: sc_x1_speed_override2_rev_number_enum
 */
#define sc_x1_speed_override2_rev_number_enum_REV_0        0
#define sc_x1_speed_override2_rev_number_enum_REV_1        1
#define sc_x1_speed_override2_rev_number_enum_REV_2        2
#define sc_x1_speed_override2_rev_number_enum_REV_3        3
#define sc_x1_speed_override2_rev_number_enum_REV_4        4
#define sc_x1_speed_override2_rev_number_enum_REV_5        5
#define sc_x1_speed_override2_rev_number_enum_REV_6        6
#define sc_x1_speed_override2_rev_number_enum_REV_7        7

/****************************************************************************
 * Enums: sc_x1_speed_override2_bonding_enum
 */
#define sc_x1_speed_override2_bonding_enum_WIRE_BOND       0
#define sc_x1_speed_override2_bonding_enum_FLIP_CHIP       1

/****************************************************************************
 * Enums: sc_x1_speed_override2_tech_process
 */
#define sc_x1_speed_override2_tech_process_PROCESS_90NM    0
#define sc_x1_speed_override2_tech_process_PROCESS_65NM    1
#define sc_x1_speed_override2_tech_process_PROCESS_40NM    2
#define sc_x1_speed_override2_tech_process_PROCESS_28NM    3

/****************************************************************************
 * Enums: sc_x1_speed_override2_model_num
 */
#define sc_x1_speed_override2_model_num_SERDES_CL73        0
#define sc_x1_speed_override2_model_num_XGXS_16G           1
#define sc_x1_speed_override2_model_num_HYPERCORE          2
#define sc_x1_speed_override2_model_num_HYPERLITE          3
#define sc_x1_speed_override2_model_num_PCIE_G2_PIPE       4
#define sc_x1_speed_override2_model_num_SERDES_1p25GBd     5
#define sc_x1_speed_override2_model_num_SATA2              6
#define sc_x1_speed_override2_model_num_QSGMII             7
#define sc_x1_speed_override2_model_num_XGXS10G            8
#define sc_x1_speed_override2_model_num_WARPCORE           9
#define sc_x1_speed_override2_model_num_XFICORE            10
#define sc_x1_speed_override2_model_num_RXFI               11
#define sc_x1_speed_override2_model_num_WARPLITE           12
#define sc_x1_speed_override2_model_num_PENTACORE          13
#define sc_x1_speed_override2_model_num_ESM                14
#define sc_x1_speed_override2_model_num_QUAD_SGMII         15
#define sc_x1_speed_override2_model_num_WARPCORE_3         16
#define sc_x1_speed_override2_model_num_TSC                17
#define sc_x1_speed_override2_model_num_TSC4E              18
#define sc_x1_speed_override2_model_num_TSC12E             19
#define sc_x1_speed_override2_model_num_TSC4F              20
#define sc_x1_speed_override2_model_num_XGXS_CL73_90NM     29
#define sc_x1_speed_override2_model_num_SERDES_CL73_90NM   30
#define sc_x1_speed_override2_model_num_WARPCORE3          32
#define sc_x1_speed_override2_model_num_WARPCORE4_TSC      33
#define sc_x1_speed_override2_model_num_RXAUI              34

/****************************************************************************
 * Enums: sc_x1_speed_override2_payload
 */
#define sc_x1_speed_override2_payload_REPEAT_2_BYTES       0
#define sc_x1_speed_override2_payload_RAMPING              1
#define sc_x1_speed_override2_payload_CL48_CRPAT           2
#define sc_x1_speed_override2_payload_CL48_CJPAT           3
#define sc_x1_speed_override2_payload_CL36_LONG_CRPAT      4
#define sc_x1_speed_override2_payload_CL36_SHORT_CRPAT     5

/****************************************************************************
 * Enums: sc_x1_speed_override2_sc
 */
#define sc_x1_speed_override2_sc_S_10G_CR1                 0
#define sc_x1_speed_override2_sc_S_10G_KR1                 1
#define sc_x1_speed_override2_sc_S_10G_X1                  2
#define sc_x1_speed_override2_sc_S_10G_HG2_CR1             4
#define sc_x1_speed_override2_sc_S_10G_HG2_KR1             5
#define sc_x1_speed_override2_sc_S_10G_HG2_X1              6
#define sc_x1_speed_override2_sc_S_20G_CR1                 8
#define sc_x1_speed_override2_sc_S_20G_KR1                 9
#define sc_x1_speed_override2_sc_S_20G_X1                  10
#define sc_x1_speed_override2_sc_S_20G_HG2_CR1             12
#define sc_x1_speed_override2_sc_S_20G_HG2_KR1             13
#define sc_x1_speed_override2_sc_S_20G_HG2_X1              14
#define sc_x1_speed_override2_sc_S_25G_CR1                 16
#define sc_x1_speed_override2_sc_S_25G_KR1                 17
#define sc_x1_speed_override2_sc_S_25G_X1                  18
#define sc_x1_speed_override2_sc_S_25G_HG2_CR1             20
#define sc_x1_speed_override2_sc_S_25G_HG2_KR1             21
#define sc_x1_speed_override2_sc_S_25G_HG2_X1              22
#define sc_x1_speed_override2_sc_S_20G_CR2                 24
#define sc_x1_speed_override2_sc_S_20G_KR2                 25
#define sc_x1_speed_override2_sc_S_20G_X2                  26
#define sc_x1_speed_override2_sc_S_20G_HG2_CR2             28
#define sc_x1_speed_override2_sc_S_20G_HG2_KR2             29
#define sc_x1_speed_override2_sc_S_20G_HG2_X2              30
#define sc_x1_speed_override2_sc_S_40G_CR2                 32
#define sc_x1_speed_override2_sc_S_40G_KR2                 33
#define sc_x1_speed_override2_sc_S_40G_X2                  34
#define sc_x1_speed_override2_sc_S_40G_HG2_CR2             36
#define sc_x1_speed_override2_sc_S_40G_HG2_KR2             37
#define sc_x1_speed_override2_sc_S_40G_HG2_X2              38
#define sc_x1_speed_override2_sc_S_40G_CR4                 40
#define sc_x1_speed_override2_sc_S_40G_KR4                 41
#define sc_x1_speed_override2_sc_S_40G_X4                  42
#define sc_x1_speed_override2_sc_S_40G_HG2_CR4             44
#define sc_x1_speed_override2_sc_S_40G_HG2_KR4             45
#define sc_x1_speed_override2_sc_S_40G_HG2_X4              46
#define sc_x1_speed_override2_sc_S_50G_CR2                 48
#define sc_x1_speed_override2_sc_S_50G_KR2                 49
#define sc_x1_speed_override2_sc_S_50G_X2                  50
#define sc_x1_speed_override2_sc_S_50G_HG2_CR2             52
#define sc_x1_speed_override2_sc_S_50G_HG2_KR2             53
#define sc_x1_speed_override2_sc_S_50G_HG2_X2              54
#define sc_x1_speed_override2_sc_S_50G_CR4                 56
#define sc_x1_speed_override2_sc_S_50G_KR4                 57
#define sc_x1_speed_override2_sc_S_50G_X4                  58
#define sc_x1_speed_override2_sc_S_50G_HG2_CR4             60
#define sc_x1_speed_override2_sc_S_50G_HG2_KR4             61
#define sc_x1_speed_override2_sc_S_50G_HG2_X4              62
#define sc_x1_speed_override2_sc_S_100G_CR4                64
#define sc_x1_speed_override2_sc_S_100G_KR4                65
#define sc_x1_speed_override2_sc_S_100G_X4                 66
#define sc_x1_speed_override2_sc_S_100G_HG2_CR4            68
#define sc_x1_speed_override2_sc_S_100G_HG2_KR4            69
#define sc_x1_speed_override2_sc_S_100G_HG2_X4             70
#define sc_x1_speed_override2_sc_S_CL73_20GVCO             72
#define sc_x1_speed_override2_sc_S_CL73_25GVCO             80
#define sc_x1_speed_override2_sc_S_CL36_20GVCO             88
#define sc_x1_speed_override2_sc_S_CL36_25GVCO             96

/****************************************************************************
 * Enums: sc_x1_speed_override2_t_fifo_modes
 */
#define sc_x1_speed_override2_t_fifo_modes_T_FIFO_MODE_BYPASS 0
#define sc_x1_speed_override2_t_fifo_modes_T_FIFO_MODE_40G 1
#define sc_x1_speed_override2_t_fifo_modes_T_FIFO_MODE_100G 2
#define sc_x1_speed_override2_t_fifo_modes_T_FIFO_MODE_20G 3

/****************************************************************************
 * Enums: sc_x1_speed_override2_t_enc_modes
 */
#define sc_x1_speed_override2_t_enc_modes_T_ENC_MODE_BYPASS 0
#define sc_x1_speed_override2_t_enc_modes_T_ENC_MODE_CL49  1
#define sc_x1_speed_override2_t_enc_modes_T_ENC_MODE_CL82  2

/****************************************************************************
 * Enums: sc_x1_speed_override2_btmx_mode
 */
#define sc_x1_speed_override2_btmx_mode_BS_BTMX_MODE_1to1  0
#define sc_x1_speed_override2_btmx_mode_BS_BTMX_MODE_2to1  1
#define sc_x1_speed_override2_btmx_mode_BS_BTMX_MODE_5to1  2

/****************************************************************************
 * Enums: sc_x1_speed_override2_t_type_cl82
 */
#define sc_x1_speed_override2_t_type_cl82_T_TYPE_B1        5
#define sc_x1_speed_override2_t_type_cl82_T_TYPE_C         4
#define sc_x1_speed_override2_t_type_cl82_T_TYPE_S         3
#define sc_x1_speed_override2_t_type_cl82_T_TYPE_T         2
#define sc_x1_speed_override2_t_type_cl82_T_TYPE_D         1
#define sc_x1_speed_override2_t_type_cl82_T_TYPE_E         0

/****************************************************************************
 * Enums: sc_x1_speed_override2_txsm_state_cl82
 */
#define sc_x1_speed_override2_txsm_state_cl82_TX_HIG_END   6
#define sc_x1_speed_override2_txsm_state_cl82_TX_HIG_START 5
#define sc_x1_speed_override2_txsm_state_cl82_TX_E         4
#define sc_x1_speed_override2_txsm_state_cl82_TX_T         3
#define sc_x1_speed_override2_txsm_state_cl82_TX_D         2
#define sc_x1_speed_override2_txsm_state_cl82_TX_C         1
#define sc_x1_speed_override2_txsm_state_cl82_TX_INIT      0

/****************************************************************************
 * Enums: sc_x1_speed_override2_ltxsm_state_cl82
 */
#define sc_x1_speed_override2_ltxsm_state_cl82_TX_HIG_END  64
#define sc_x1_speed_override2_ltxsm_state_cl82_TX_HIG_START 32
#define sc_x1_speed_override2_ltxsm_state_cl82_TX_E        16
#define sc_x1_speed_override2_ltxsm_state_cl82_TX_T        8
#define sc_x1_speed_override2_ltxsm_state_cl82_TX_D        4
#define sc_x1_speed_override2_ltxsm_state_cl82_TX_C        2
#define sc_x1_speed_override2_ltxsm_state_cl82_TX_INIT     1

/****************************************************************************
 * Enums: sc_x1_speed_override2_r_type_coded_cl82
 */
#define sc_x1_speed_override2_r_type_coded_cl82_R_TYPE_B1  32
#define sc_x1_speed_override2_r_type_coded_cl82_R_TYPE_C   16
#define sc_x1_speed_override2_r_type_coded_cl82_R_TYPE_S   8
#define sc_x1_speed_override2_r_type_coded_cl82_R_TYPE_T   4
#define sc_x1_speed_override2_r_type_coded_cl82_R_TYPE_D   2
#define sc_x1_speed_override2_r_type_coded_cl82_R_TYPE_E   1

/****************************************************************************
 * Enums: sc_x1_speed_override2_rxsm_state_cl82
 */
#define sc_x1_speed_override2_rxsm_state_cl82_RX_HIG_END   64
#define sc_x1_speed_override2_rxsm_state_cl82_RX_HIG_START 32
#define sc_x1_speed_override2_rxsm_state_cl82_RX_E         16
#define sc_x1_speed_override2_rxsm_state_cl82_RX_T         8
#define sc_x1_speed_override2_rxsm_state_cl82_RX_D         4
#define sc_x1_speed_override2_rxsm_state_cl82_RX_C         2
#define sc_x1_speed_override2_rxsm_state_cl82_RX_INIT      1

/****************************************************************************
 * Enums: sc_x1_speed_override2_deskew_state
 */
#define sc_x1_speed_override2_deskew_state_ALIGN_ACQUIRED  2
#define sc_x1_speed_override2_deskew_state_LOSS_OF_ALIGNMENT 1

/****************************************************************************
 * Enums: sc_x1_speed_override2_os_mode_enum
 */
#define sc_x1_speed_override2_os_mode_enum_OS_MODE_1       0
#define sc_x1_speed_override2_os_mode_enum_OS_MODE_2       1
#define sc_x1_speed_override2_os_mode_enum_OS_MODE_4       2
#define sc_x1_speed_override2_os_mode_enum_OS_MODE_16p5    8
#define sc_x1_speed_override2_os_mode_enum_OS_MODE_20p625  12

/****************************************************************************
 * Enums: sc_x1_speed_override2_scr_modes
 */
#define sc_x1_speed_override2_scr_modes_T_SCR_MODE_BYPASS  0
#define sc_x1_speed_override2_scr_modes_T_SCR_MODE_CL49    1
#define sc_x1_speed_override2_scr_modes_T_SCR_MODE_40G_2_LANE 2
#define sc_x1_speed_override2_scr_modes_T_SCR_MODE_100G    3
#define sc_x1_speed_override2_scr_modes_T_SCR_MODE_20G     4
#define sc_x1_speed_override2_scr_modes_T_SCR_MODE_40G_4_LANE 5

/****************************************************************************
 * Enums: sc_x1_speed_override2_descr_modes
 */
#define sc_x1_speed_override2_descr_modes_R_DESCR_MODE_BYPASS 0
#define sc_x1_speed_override2_descr_modes_R_DESCR_MODE_CL49 1
#define sc_x1_speed_override2_descr_modes_R_DESCR_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override2_r_dec_tl_mode
 */
#define sc_x1_speed_override2_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS 0
#define sc_x1_speed_override2_r_dec_tl_mode_R_DEC_TL_MODE_CL49 1
#define sc_x1_speed_override2_r_dec_tl_mode_R_DEC_TL_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override2_r_dec_fsm_mode
 */
#define sc_x1_speed_override2_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define sc_x1_speed_override2_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49 1
#define sc_x1_speed_override2_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override2_r_deskew_mode
 */
#define sc_x1_speed_override2_r_deskew_mode_R_DESKEW_MODE_BYPASS 0
#define sc_x1_speed_override2_r_deskew_mode_R_DESKEW_MODE_20G 1
#define sc_x1_speed_override2_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define sc_x1_speed_override2_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define sc_x1_speed_override2_r_deskew_mode_R_DESKEW_MODE_100G 4
#define sc_x1_speed_override2_r_deskew_mode_R_DESKEW_MODE_CL49 5
#define sc_x1_speed_override2_r_deskew_mode_R_DESKEW_MODE_CL91 6

/****************************************************************************
 * Enums: sc_x1_speed_override2_bs_dist_modes
 */
#define sc_x1_speed_override2_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define sc_x1_speed_override2_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define sc_x1_speed_override2_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define sc_x1_speed_override2_bs_dist_modes_BS_DIST_MODE_NO_TDM 3

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl49_t_type
 */
#define sc_x1_speed_override2_cl49_t_type_BAD_T_TYPE       15
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_B0        11
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_OB        10
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_B1        9
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_DB        8
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_FC        7
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_TB        6
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_LI        5
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_C         4
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_S         3
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_T         2
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_D         1
#define sc_x1_speed_override2_cl49_t_type_T_TYPE_E         0

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl49_txsm_states
 */
#define sc_x1_speed_override2_cl49_txsm_states_TX_HIG_END  7
#define sc_x1_speed_override2_cl49_txsm_states_TX_HIG_START 6
#define sc_x1_speed_override2_cl49_txsm_states_TX_LI       5
#define sc_x1_speed_override2_cl49_txsm_states_TX_E        4
#define sc_x1_speed_override2_cl49_txsm_states_TX_T        3
#define sc_x1_speed_override2_cl49_txsm_states_TX_D        2
#define sc_x1_speed_override2_cl49_txsm_states_TX_C        1
#define sc_x1_speed_override2_cl49_txsm_states_TX_INIT     0

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl49_ltxsm_states
 */
#define sc_x1_speed_override2_cl49_ltxsm_states_TX_HIG_END 128
#define sc_x1_speed_override2_cl49_ltxsm_states_TX_HIG_START 64
#define sc_x1_speed_override2_cl49_ltxsm_states_TX_LI      32
#define sc_x1_speed_override2_cl49_ltxsm_states_TX_E       16
#define sc_x1_speed_override2_cl49_ltxsm_states_TX_T       8
#define sc_x1_speed_override2_cl49_ltxsm_states_TX_D       4
#define sc_x1_speed_override2_cl49_ltxsm_states_TX_C       2
#define sc_x1_speed_override2_cl49_ltxsm_states_TX_INIT    1

/****************************************************************************
 * Enums: sc_x1_speed_override2_burst_error_mode
 */
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_11_BITS 0
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_16_BITS 1
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_17_BITS 2
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_18_BITS 3
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_19_BITS 4
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_20_BITS 5
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_21_BITS 6
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_22_BITS 7
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_23_BITS 8
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_24_BITS 9
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_25_BITS 10
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_26_BITS 11
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_27_BITS 12
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_28_BITS 13
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_29_BITS 14
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_30_BITS 15
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_31_BITS 16
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_32_BITS 17
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_33_BITS 18
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_34_BITS 19
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_35_BITS 20
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_36_BITS 21
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_37_BITS 22
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_38_BITS 23
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_39_BITS 24
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_40_BITS 25
#define sc_x1_speed_override2_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: sc_x1_speed_override2_bermon_state
 */
#define sc_x1_speed_override2_bermon_state_HI_BER          4
#define sc_x1_speed_override2_bermon_state_GOOD_BER        3
#define sc_x1_speed_override2_bermon_state_BER_TEST_SH     2
#define sc_x1_speed_override2_bermon_state_START_TIMER     1
#define sc_x1_speed_override2_bermon_state_BER_MT_INIT     0

/****************************************************************************
 * Enums: sc_x1_speed_override2_rxsm_state_cl49
 */
#define sc_x1_speed_override2_rxsm_state_cl49_RX_HIG_END   128
#define sc_x1_speed_override2_rxsm_state_cl49_RX_HIG_START 64
#define sc_x1_speed_override2_rxsm_state_cl49_RX_LI        32
#define sc_x1_speed_override2_rxsm_state_cl49_RX_E         16
#define sc_x1_speed_override2_rxsm_state_cl49_RX_T         8
#define sc_x1_speed_override2_rxsm_state_cl49_RX_D         4
#define sc_x1_speed_override2_rxsm_state_cl49_RX_C         2
#define sc_x1_speed_override2_rxsm_state_cl49_RX_INIT      1

/****************************************************************************
 * Enums: sc_x1_speed_override2_r_type
 */
#define sc_x1_speed_override2_r_type_BAD_R_TYPE            15
#define sc_x1_speed_override2_r_type_R_TYPE_B0             11
#define sc_x1_speed_override2_r_type_R_TYPE_OB             10
#define sc_x1_speed_override2_r_type_R_TYPE_B1             9
#define sc_x1_speed_override2_r_type_R_TYPE_DB             8
#define sc_x1_speed_override2_r_type_R_TYPE_FC             7
#define sc_x1_speed_override2_r_type_R_TYPE_TB             6
#define sc_x1_speed_override2_r_type_R_TYPE_LI             5
#define sc_x1_speed_override2_r_type_R_TYPE_C              4
#define sc_x1_speed_override2_r_type_R_TYPE_S              3
#define sc_x1_speed_override2_r_type_R_TYPE_T              2
#define sc_x1_speed_override2_r_type_R_TYPE_D              1
#define sc_x1_speed_override2_r_type_R_TYPE_E              0

/****************************************************************************
 * Enums: sc_x1_speed_override2_am_lock_state
 */
#define sc_x1_speed_override2_am_lock_state_INVALID_AM     512
#define sc_x1_speed_override2_am_lock_state_GOOD_AM        256
#define sc_x1_speed_override2_am_lock_state_COMP_AM        128
#define sc_x1_speed_override2_am_lock_state_TIMER_2        64
#define sc_x1_speed_override2_am_lock_state_AM_2_GOOD      32
#define sc_x1_speed_override2_am_lock_state_COMP_2ND       16
#define sc_x1_speed_override2_am_lock_state_TIMER_1        8
#define sc_x1_speed_override2_am_lock_state_FIND_1ST       4
#define sc_x1_speed_override2_am_lock_state_AM_RESET_CNT   2
#define sc_x1_speed_override2_am_lock_state_AM_LOCK_INIT   1

/****************************************************************************
 * Enums: sc_x1_speed_override2_msg_selector
 */
#define sc_x1_speed_override2_msg_selector_RESERVED        0
#define sc_x1_speed_override2_msg_selector_VALUE_802p3     1
#define sc_x1_speed_override2_msg_selector_VALUE_802p9     2
#define sc_x1_speed_override2_msg_selector_VALUE_802p5     3
#define sc_x1_speed_override2_msg_selector_VALUE_1394      4

/****************************************************************************
 * Enums: sc_x1_speed_override2_synce_enum
 */
#define sc_x1_speed_override2_synce_enum_SYNCE_NO_DIV      0
#define sc_x1_speed_override2_synce_enum_SYNCE_DIV_7       1
#define sc_x1_speed_override2_synce_enum_SYNCE_DIV_11      2

/****************************************************************************
 * Enums: sc_x1_speed_override2_synce_enum_stage0
 */
#define sc_x1_speed_override2_synce_enum_stage0_SYNCE_NO_DIV 0
#define sc_x1_speed_override2_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define sc_x1_speed_override2_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl91_sync_state
 */
#define sc_x1_speed_override2_cl91_sync_state_FIND_1ST     0
#define sc_x1_speed_override2_cl91_sync_state_COUNT_NEXT   1
#define sc_x1_speed_override2_cl91_sync_state_COMP_2ND     2
#define sc_x1_speed_override2_cl91_sync_state_TWO_GOOD     3

/****************************************************************************
 * Enums: sc_x1_speed_override2_cl91_algn_state
 */
#define sc_x1_speed_override2_cl91_algn_state_LOSS_OF_ALIGNMENT 0
#define sc_x1_speed_override2_cl91_algn_state_DESKEW       1
#define sc_x1_speed_override2_cl91_algn_state_DESKEW_FAIL  2
#define sc_x1_speed_override2_cl91_algn_state_ALIGN_ACQUIRED 3
#define sc_x1_speed_override2_cl91_algn_state_CW_GOOD      4
#define sc_x1_speed_override2_cl91_algn_state_CW_BAD       5
#define sc_x1_speed_override2_cl91_algn_state_THREE_BAD    6

/****************************************************************************
 * Enums: sc_x1_speed_override2_fec_sel
 */
#define sc_x1_speed_override2_fec_sel_NO_FEC               0
#define sc_x1_speed_override2_fec_sel_FEC_CL74             1
#define sc_x1_speed_override2_fec_sel_FEC_CL91             2

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl36TxEEEStates_l
 */
#define sc_x1_speed_override3_cl36TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override3_cl36TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override3_cl36TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override3_cl36TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl36TxEEEStates_c
 */
#define sc_x1_speed_override3_cl36TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override3_cl36TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override3_cl36TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override3_cl36TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl49TxEEEStates_l
 */
#define sc_x1_speed_override3_cl49TxEEEStates_l_SCR_RESET_2 64
#define sc_x1_speed_override3_cl49TxEEEStates_l_SCR_RESET_1 32
#define sc_x1_speed_override3_cl49TxEEEStates_l_TX_WAKE    16
#define sc_x1_speed_override3_cl49TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override3_cl49TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override3_cl49TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override3_cl49TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override3_fec_req_enum
 */
#define sc_x1_speed_override3_fec_req_enum_FEC_not_supported 0
#define sc_x1_speed_override3_fec_req_enum_FEC_supported_but_not_requested 1
#define sc_x1_speed_override3_fec_req_enum_invalid_setting 2
#define sc_x1_speed_override3_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl73_pause_enum
 */
#define sc_x1_speed_override3_cl73_pause_enum_No_PAUSE_ability 0
#define sc_x1_speed_override3_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define sc_x1_speed_override3_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define sc_x1_speed_override3_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl49TxEEEStates_c
 */
#define sc_x1_speed_override3_cl49TxEEEStates_c_SCR_RESET_2 6
#define sc_x1_speed_override3_cl49TxEEEStates_c_SCR_RESET_1 5
#define sc_x1_speed_override3_cl49TxEEEStates_c_TX_WAKE    4
#define sc_x1_speed_override3_cl49TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override3_cl49TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override3_cl49TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override3_cl49TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl36RxEEEStates_l
 */
#define sc_x1_speed_override3_cl36RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override3_cl36RxEEEStates_l_RX_WTF     16
#define sc_x1_speed_override3_cl36RxEEEStates_l_RX_WAKE    8
#define sc_x1_speed_override3_cl36RxEEEStates_l_RX_QUIET   4
#define sc_x1_speed_override3_cl36RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override3_cl36RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl36RxEEEStates_c
 */
#define sc_x1_speed_override3_cl36RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override3_cl36RxEEEStates_c_RX_WTF     4
#define sc_x1_speed_override3_cl36RxEEEStates_c_RX_WAKE    3
#define sc_x1_speed_override3_cl36RxEEEStates_c_RX_QUIET   2
#define sc_x1_speed_override3_cl36RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override3_cl36RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl49RxEEEStates_l
 */
#define sc_x1_speed_override3_cl49RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override3_cl49RxEEEStates_l_RX_WTF     16
#define sc_x1_speed_override3_cl49RxEEEStates_l_RX_WAKE    8
#define sc_x1_speed_override3_cl49RxEEEStates_l_RX_QUIET   4
#define sc_x1_speed_override3_cl49RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override3_cl49RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl49RxEEEStates_c
 */
#define sc_x1_speed_override3_cl49RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override3_cl49RxEEEStates_c_RX_WTF     4
#define sc_x1_speed_override3_cl49RxEEEStates_c_RX_WAKE    3
#define sc_x1_speed_override3_cl49RxEEEStates_c_RX_QUIET   2
#define sc_x1_speed_override3_cl49RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override3_cl49RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl48TxEEEStates_l
 */
#define sc_x1_speed_override3_cl48TxEEEStates_l_TX_REFRESH 8
#define sc_x1_speed_override3_cl48TxEEEStates_l_TX_QUIET   4
#define sc_x1_speed_override3_cl48TxEEEStates_l_TX_SLEEP   2
#define sc_x1_speed_override3_cl48TxEEEStates_l_TX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl48TxEEEStates_c
 */
#define sc_x1_speed_override3_cl48TxEEEStates_c_TX_REFRESH 3
#define sc_x1_speed_override3_cl48TxEEEStates_c_TX_QUIET   2
#define sc_x1_speed_override3_cl48TxEEEStates_c_TX_SLEEP   1
#define sc_x1_speed_override3_cl48TxEEEStates_c_TX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl48RxEEEStates_l
 */
#define sc_x1_speed_override3_cl48RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x1_speed_override3_cl48RxEEEStates_l_RX_WAKE    16
#define sc_x1_speed_override3_cl48RxEEEStates_l_RX_QUIET   8
#define sc_x1_speed_override3_cl48RxEEEStates_l_RX_DEACT   4
#define sc_x1_speed_override3_cl48RxEEEStates_l_RX_SLEEP   2
#define sc_x1_speed_override3_cl48RxEEEStates_l_RX_ACTIVE  1

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl48RxEEEStates_c
 */
#define sc_x1_speed_override3_cl48RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x1_speed_override3_cl48RxEEEStates_c_RX_WAKE    4
#define sc_x1_speed_override3_cl48RxEEEStates_c_RX_QUIET   3
#define sc_x1_speed_override3_cl48RxEEEStates_c_RX_DEACT   2
#define sc_x1_speed_override3_cl48RxEEEStates_c_RX_SLEEP   1
#define sc_x1_speed_override3_cl48RxEEEStates_c_RX_ACTIVE  0

/****************************************************************************
 * Enums: sc_x1_speed_override3_IQP_Options
 */
#define sc_x1_speed_override3_IQP_Options_i50uA            0
#define sc_x1_speed_override3_IQP_Options_i100uA           1
#define sc_x1_speed_override3_IQP_Options_i150uA           2
#define sc_x1_speed_override3_IQP_Options_i200uA           3
#define sc_x1_speed_override3_IQP_Options_i250uA           4
#define sc_x1_speed_override3_IQP_Options_i300uA           5
#define sc_x1_speed_override3_IQP_Options_i350uA           6
#define sc_x1_speed_override3_IQP_Options_i400uA           7
#define sc_x1_speed_override3_IQP_Options_i450uA           8
#define sc_x1_speed_override3_IQP_Options_i500uA           9
#define sc_x1_speed_override3_IQP_Options_i550uA           10
#define sc_x1_speed_override3_IQP_Options_i600uA           11
#define sc_x1_speed_override3_IQP_Options_i650uA           12
#define sc_x1_speed_override3_IQP_Options_i700uA           13
#define sc_x1_speed_override3_IQP_Options_i750uA           14
#define sc_x1_speed_override3_IQP_Options_i800uA           15

/****************************************************************************
 * Enums: sc_x1_speed_override3_IDriver_Options
 */
#define sc_x1_speed_override3_IDriver_Options_v680mV       0
#define sc_x1_speed_override3_IDriver_Options_v730mV       1
#define sc_x1_speed_override3_IDriver_Options_v780mV       2
#define sc_x1_speed_override3_IDriver_Options_v830mV       3
#define sc_x1_speed_override3_IDriver_Options_v880mV       4
#define sc_x1_speed_override3_IDriver_Options_v930mV       5
#define sc_x1_speed_override3_IDriver_Options_v980mV       6
#define sc_x1_speed_override3_IDriver_Options_v1010mV      7
#define sc_x1_speed_override3_IDriver_Options_v1040mV      8
#define sc_x1_speed_override3_IDriver_Options_v1060mV      9
#define sc_x1_speed_override3_IDriver_Options_v1070mV      10
#define sc_x1_speed_override3_IDriver_Options_v1080mV      11
#define sc_x1_speed_override3_IDriver_Options_v1085mV      12
#define sc_x1_speed_override3_IDriver_Options_v1090mV      13
#define sc_x1_speed_override3_IDriver_Options_v1095mV      14
#define sc_x1_speed_override3_IDriver_Options_v1100mV      15

/****************************************************************************
 * Enums: sc_x1_speed_override3_operationModes
 */
#define sc_x1_speed_override3_operationModes_XGXS          0
#define sc_x1_speed_override3_operationModes_XGXG_nCC      1
#define sc_x1_speed_override3_operationModes_Indlane_OS8   4
#define sc_x1_speed_override3_operationModes_IndLane_OS5   5
#define sc_x1_speed_override3_operationModes_PCI           7
#define sc_x1_speed_override3_operationModes_XGXS_nLQ      8
#define sc_x1_speed_override3_operationModes_XGXS_nLQnCC   9
#define sc_x1_speed_override3_operationModes_PBypass       10
#define sc_x1_speed_override3_operationModes_PBypass_nDSK  11
#define sc_x1_speed_override3_operationModes_ComboCoreMode 12
#define sc_x1_speed_override3_operationModes_Clocks_off    15

/****************************************************************************
 * Enums: sc_x1_speed_override3_actualSpeeds
 */
#define sc_x1_speed_override3_actualSpeeds_dr_10M          0
#define sc_x1_speed_override3_actualSpeeds_dr_100M         1
#define sc_x1_speed_override3_actualSpeeds_dr_1G           2
#define sc_x1_speed_override3_actualSpeeds_dr_2p5G         3
#define sc_x1_speed_override3_actualSpeeds_dr_5G_X4        4
#define sc_x1_speed_override3_actualSpeeds_dr_6G_X4        5
#define sc_x1_speed_override3_actualSpeeds_dr_10G_HiG      6
#define sc_x1_speed_override3_actualSpeeds_dr_10G_CX4      7
#define sc_x1_speed_override3_actualSpeeds_dr_12G_HiG      8
#define sc_x1_speed_override3_actualSpeeds_dr_12p5G_X4     9
#define sc_x1_speed_override3_actualSpeeds_dr_13G_X4       10
#define sc_x1_speed_override3_actualSpeeds_dr_15G_X4       11
#define sc_x1_speed_override3_actualSpeeds_dr_16G_X4       12
#define sc_x1_speed_override3_actualSpeeds_dr_1G_KX        13
#define sc_x1_speed_override3_actualSpeeds_dr_10G_KX4      14
#define sc_x1_speed_override3_actualSpeeds_dr_10G_KR       15
#define sc_x1_speed_override3_actualSpeeds_dr_5G           16
#define sc_x1_speed_override3_actualSpeeds_dr_6p4G         17
#define sc_x1_speed_override3_actualSpeeds_dr_20G_X4       18
#define sc_x1_speed_override3_actualSpeeds_dr_21G_X4       19
#define sc_x1_speed_override3_actualSpeeds_dr_25G_X4       20
#define sc_x1_speed_override3_actualSpeeds_dr_10G_HiG_DXGXS 21
#define sc_x1_speed_override3_actualSpeeds_dr_10G_DXGXS    22
#define sc_x1_speed_override3_actualSpeeds_dr_10p5G_HiG_DXGXS 23
#define sc_x1_speed_override3_actualSpeeds_dr_10p5G_DXGXS  24
#define sc_x1_speed_override3_actualSpeeds_dr_12p773G_HiG_DXGXS 25
#define sc_x1_speed_override3_actualSpeeds_dr_12p773G_DXGXS 26
#define sc_x1_speed_override3_actualSpeeds_dr_10G_XFI      27
#define sc_x1_speed_override3_actualSpeeds_dr_40G          28
#define sc_x1_speed_override3_actualSpeeds_dr_20G_HiG_DXGXS 29
#define sc_x1_speed_override3_actualSpeeds_dr_20G_DXGXS    30
#define sc_x1_speed_override3_actualSpeeds_dr_10G_SFI      31
#define sc_x1_speed_override3_actualSpeeds_dr_31p5G        32
#define sc_x1_speed_override3_actualSpeeds_dr_32p7G        33
#define sc_x1_speed_override3_actualSpeeds_dr_20G_SCR      34
#define sc_x1_speed_override3_actualSpeeds_dr_10G_HiG_DXGXS_SCR 35
#define sc_x1_speed_override3_actualSpeeds_dr_10G_DXGXS_SCR 36
#define sc_x1_speed_override3_actualSpeeds_dr_12G_R2       37
#define sc_x1_speed_override3_actualSpeeds_dr_10G_X2       38
#define sc_x1_speed_override3_actualSpeeds_dr_40G_KR4      39
#define sc_x1_speed_override3_actualSpeeds_dr_40G_CR4      40
#define sc_x1_speed_override3_actualSpeeds_dr_100G_CR10    41
#define sc_x1_speed_override3_actualSpeeds_dr_15p75G_DXGXS 44
#define sc_x1_speed_override3_actualSpeeds_dr_20G_KR2      57
#define sc_x1_speed_override3_actualSpeeds_dr_20G_CR2      58

/****************************************************************************
 * Enums: sc_x1_speed_override3_actualSpeedsMisc1
 */
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_2500BRCM_X1 16
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_5000BRCM_X4 17
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_6000BRCM_X4 18
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_10GHiGig_X4 19
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_10GBASE_CX4 20
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_12GHiGig_X4 21
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_12p5GHiGig_X4 22
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_13GHiGig_X4 23
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_15GHiGig_X4 24
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_16GHiGig_X4 25
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_5000BRCM_X1 26
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_6363BRCM_X1 27
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_20GHiGig_X4 28
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_21GHiGig_X4 29
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define sc_x1_speed_override3_actualSpeedsMisc1_dr_10G_HiG_DXGXS 31

/****************************************************************************
 * Enums: sc_x1_speed_override3_IndLaneModes
 */
#define sc_x1_speed_override3_IndLaneModes_SWSDR_div2      0
#define sc_x1_speed_override3_IndLaneModes_SWSDR_div1      1
#define sc_x1_speed_override3_IndLaneModes_DWSDR_div2      2
#define sc_x1_speed_override3_IndLaneModes_DWSDR_div1      3

/****************************************************************************
 * Enums: sc_x1_speed_override3_prbsSelect
 */
#define sc_x1_speed_override3_prbsSelect_prbs7             0
#define sc_x1_speed_override3_prbsSelect_prbs15            1
#define sc_x1_speed_override3_prbsSelect_prbs23            2
#define sc_x1_speed_override3_prbsSelect_prbs31            3

/****************************************************************************
 * Enums: sc_x1_speed_override3_vcoDivider
 */
#define sc_x1_speed_override3_vcoDivider_div32             0
#define sc_x1_speed_override3_vcoDivider_div36             1
#define sc_x1_speed_override3_vcoDivider_div40             2
#define sc_x1_speed_override3_vcoDivider_div42             3
#define sc_x1_speed_override3_vcoDivider_div48             4
#define sc_x1_speed_override3_vcoDivider_div50             5
#define sc_x1_speed_override3_vcoDivider_div52             6
#define sc_x1_speed_override3_vcoDivider_div54             7
#define sc_x1_speed_override3_vcoDivider_div60             8
#define sc_x1_speed_override3_vcoDivider_div64             9
#define sc_x1_speed_override3_vcoDivider_div66             10
#define sc_x1_speed_override3_vcoDivider_div68             11
#define sc_x1_speed_override3_vcoDivider_div70             12
#define sc_x1_speed_override3_vcoDivider_div80             13
#define sc_x1_speed_override3_vcoDivider_div92             14
#define sc_x1_speed_override3_vcoDivider_div100            15

/****************************************************************************
 * Enums: sc_x1_speed_override3_refClkSelect
 */
#define sc_x1_speed_override3_refClkSelect_clk_25MHz       0
#define sc_x1_speed_override3_refClkSelect_clk_100MHz      1
#define sc_x1_speed_override3_refClkSelect_clk_125MHz      2
#define sc_x1_speed_override3_refClkSelect_clk_156p25MHz   3
#define sc_x1_speed_override3_refClkSelect_clk_187p5MHz    4
#define sc_x1_speed_override3_refClkSelect_clk_161p25Mhz   5
#define sc_x1_speed_override3_refClkSelect_clk_50Mhz       6
#define sc_x1_speed_override3_refClkSelect_clk_106p25Mhz   7

/****************************************************************************
 * Enums: sc_x1_speed_override3_aerMMDdevTypeSelect
 */
#define sc_x1_speed_override3_aerMMDdevTypeSelect_combo_core 0
#define sc_x1_speed_override3_aerMMDdevTypeSelect_PMA_PMD  1
#define sc_x1_speed_override3_aerMMDdevTypeSelect_PCS      3
#define sc_x1_speed_override3_aerMMDdevTypeSelect_PHY      4
#define sc_x1_speed_override3_aerMMDdevTypeSelect_DTE      5
#define sc_x1_speed_override3_aerMMDdevTypeSelect_CL73_AN  7

/****************************************************************************
 * Enums: sc_x1_speed_override3_aerMMDportSelect
 */
#define sc_x1_speed_override3_aerMMDportSelect_ln0         0
#define sc_x1_speed_override3_aerMMDportSelect_ln1         1
#define sc_x1_speed_override3_aerMMDportSelect_ln2         2
#define sc_x1_speed_override3_aerMMDportSelect_ln3         3
#define sc_x1_speed_override3_aerMMDportSelect_BCST_ln0_1_2_3 511
#define sc_x1_speed_override3_aerMMDportSelect_BCST_ln0_1  512
#define sc_x1_speed_override3_aerMMDportSelect_BCST_ln2_3  513

/****************************************************************************
 * Enums: sc_x1_speed_override3_firmwareModeSelect
 */
#define sc_x1_speed_override3_firmwareModeSelect_DEFAULT   0
#define sc_x1_speed_override3_firmwareModeSelect_SFP_OPT_LR 1
#define sc_x1_speed_override3_firmwareModeSelect_SFP_DAC   2
#define sc_x1_speed_override3_firmwareModeSelect_XLAUI     3
#define sc_x1_speed_override3_firmwareModeSelect_LONG_CH_6G 4

/****************************************************************************
 * Enums: sc_x1_speed_override3_tempIdxSelect
 */
#define sc_x1_speed_override3_tempIdxSelect_LTE__22p9C     15
#define sc_x1_speed_override3_tempIdxSelect_LTE__12p6C     14
#define sc_x1_speed_override3_tempIdxSelect_LTE__3p0C      13
#define sc_x1_speed_override3_tempIdxSelect_LTE_6p7C       12
#define sc_x1_speed_override3_tempIdxSelect_LTE_16p4C      11
#define sc_x1_speed_override3_tempIdxSelect_LTE_26p6C      10
#define sc_x1_speed_override3_tempIdxSelect_LTE_36p3C      9
#define sc_x1_speed_override3_tempIdxSelect_LTE_46p0C      8
#define sc_x1_speed_override3_tempIdxSelect_LTE_56p2C      7
#define sc_x1_speed_override3_tempIdxSelect_LTE_65p9C      6
#define sc_x1_speed_override3_tempIdxSelect_LTE_75p6C      5
#define sc_x1_speed_override3_tempIdxSelect_LTE_85p3C      4
#define sc_x1_speed_override3_tempIdxSelect_LTE_95p5C      3
#define sc_x1_speed_override3_tempIdxSelect_LTE_105p2C     2
#define sc_x1_speed_override3_tempIdxSelect_LTE_114p9C     1
#define sc_x1_speed_override3_tempIdxSelect_LTE_125p1C     0

/****************************************************************************
 * Enums: sc_x1_speed_override3_port_mode
 */
#define sc_x1_speed_override3_port_mode_QUAD_PORT          0
#define sc_x1_speed_override3_port_mode_TRI_1_PORT         1
#define sc_x1_speed_override3_port_mode_TRI_2_PORT         2
#define sc_x1_speed_override3_port_mode_DUAL_PORT          3
#define sc_x1_speed_override3_port_mode_SINGLE_PORT        4

/****************************************************************************
 * Enums: sc_x1_speed_override3_rev_letter_enum
 */
#define sc_x1_speed_override3_rev_letter_enum_REV_A        0
#define sc_x1_speed_override3_rev_letter_enum_REV_B        1
#define sc_x1_speed_override3_rev_letter_enum_REV_C        2
#define sc_x1_speed_override3_rev_letter_enum_REV_D        3

/****************************************************************************
 * Enums: sc_x1_speed_override3_rev_number_enum
 */
#define sc_x1_speed_override3_rev_number_enum_REV_0        0
#define sc_x1_speed_override3_rev_number_enum_REV_1        1
#define sc_x1_speed_override3_rev_number_enum_REV_2        2
#define sc_x1_speed_override3_rev_number_enum_REV_3        3
#define sc_x1_speed_override3_rev_number_enum_REV_4        4
#define sc_x1_speed_override3_rev_number_enum_REV_5        5
#define sc_x1_speed_override3_rev_number_enum_REV_6        6
#define sc_x1_speed_override3_rev_number_enum_REV_7        7

/****************************************************************************
 * Enums: sc_x1_speed_override3_bonding_enum
 */
#define sc_x1_speed_override3_bonding_enum_WIRE_BOND       0
#define sc_x1_speed_override3_bonding_enum_FLIP_CHIP       1

/****************************************************************************
 * Enums: sc_x1_speed_override3_tech_process
 */
#define sc_x1_speed_override3_tech_process_PROCESS_90NM    0
#define sc_x1_speed_override3_tech_process_PROCESS_65NM    1
#define sc_x1_speed_override3_tech_process_PROCESS_40NM    2
#define sc_x1_speed_override3_tech_process_PROCESS_28NM    3

/****************************************************************************
 * Enums: sc_x1_speed_override3_model_num
 */
#define sc_x1_speed_override3_model_num_SERDES_CL73        0
#define sc_x1_speed_override3_model_num_XGXS_16G           1
#define sc_x1_speed_override3_model_num_HYPERCORE          2
#define sc_x1_speed_override3_model_num_HYPERLITE          3
#define sc_x1_speed_override3_model_num_PCIE_G2_PIPE       4
#define sc_x1_speed_override3_model_num_SERDES_1p25GBd     5
#define sc_x1_speed_override3_model_num_SATA2              6
#define sc_x1_speed_override3_model_num_QSGMII             7
#define sc_x1_speed_override3_model_num_XGXS10G            8
#define sc_x1_speed_override3_model_num_WARPCORE           9
#define sc_x1_speed_override3_model_num_XFICORE            10
#define sc_x1_speed_override3_model_num_RXFI               11
#define sc_x1_speed_override3_model_num_WARPLITE           12
#define sc_x1_speed_override3_model_num_PENTACORE          13
#define sc_x1_speed_override3_model_num_ESM                14
#define sc_x1_speed_override3_model_num_QUAD_SGMII         15
#define sc_x1_speed_override3_model_num_WARPCORE_3         16
#define sc_x1_speed_override3_model_num_TSC                17
#define sc_x1_speed_override3_model_num_TSC4E              18
#define sc_x1_speed_override3_model_num_TSC12E             19
#define sc_x1_speed_override3_model_num_TSC4F              20
#define sc_x1_speed_override3_model_num_XGXS_CL73_90NM     29
#define sc_x1_speed_override3_model_num_SERDES_CL73_90NM   30
#define sc_x1_speed_override3_model_num_WARPCORE3          32
#define sc_x1_speed_override3_model_num_WARPCORE4_TSC      33
#define sc_x1_speed_override3_model_num_RXAUI              34

/****************************************************************************
 * Enums: sc_x1_speed_override3_payload
 */
#define sc_x1_speed_override3_payload_REPEAT_2_BYTES       0
#define sc_x1_speed_override3_payload_RAMPING              1
#define sc_x1_speed_override3_payload_CL48_CRPAT           2
#define sc_x1_speed_override3_payload_CL48_CJPAT           3
#define sc_x1_speed_override3_payload_CL36_LONG_CRPAT      4
#define sc_x1_speed_override3_payload_CL36_SHORT_CRPAT     5

/****************************************************************************
 * Enums: sc_x1_speed_override3_sc
 */
#define sc_x1_speed_override3_sc_S_10G_CR1                 0
#define sc_x1_speed_override3_sc_S_10G_KR1                 1
#define sc_x1_speed_override3_sc_S_10G_X1                  2
#define sc_x1_speed_override3_sc_S_10G_HG2_CR1             4
#define sc_x1_speed_override3_sc_S_10G_HG2_KR1             5
#define sc_x1_speed_override3_sc_S_10G_HG2_X1              6
#define sc_x1_speed_override3_sc_S_20G_CR1                 8
#define sc_x1_speed_override3_sc_S_20G_KR1                 9
#define sc_x1_speed_override3_sc_S_20G_X1                  10
#define sc_x1_speed_override3_sc_S_20G_HG2_CR1             12
#define sc_x1_speed_override3_sc_S_20G_HG2_KR1             13
#define sc_x1_speed_override3_sc_S_20G_HG2_X1              14
#define sc_x1_speed_override3_sc_S_25G_CR1                 16
#define sc_x1_speed_override3_sc_S_25G_KR1                 17
#define sc_x1_speed_override3_sc_S_25G_X1                  18
#define sc_x1_speed_override3_sc_S_25G_HG2_CR1             20
#define sc_x1_speed_override3_sc_S_25G_HG2_KR1             21
#define sc_x1_speed_override3_sc_S_25G_HG2_X1              22
#define sc_x1_speed_override3_sc_S_20G_CR2                 24
#define sc_x1_speed_override3_sc_S_20G_KR2                 25
#define sc_x1_speed_override3_sc_S_20G_X2                  26
#define sc_x1_speed_override3_sc_S_20G_HG2_CR2             28
#define sc_x1_speed_override3_sc_S_20G_HG2_KR2             29
#define sc_x1_speed_override3_sc_S_20G_HG2_X2              30
#define sc_x1_speed_override3_sc_S_40G_CR2                 32
#define sc_x1_speed_override3_sc_S_40G_KR2                 33
#define sc_x1_speed_override3_sc_S_40G_X2                  34
#define sc_x1_speed_override3_sc_S_40G_HG2_CR2             36
#define sc_x1_speed_override3_sc_S_40G_HG2_KR2             37
#define sc_x1_speed_override3_sc_S_40G_HG2_X2              38
#define sc_x1_speed_override3_sc_S_40G_CR4                 40
#define sc_x1_speed_override3_sc_S_40G_KR4                 41
#define sc_x1_speed_override3_sc_S_40G_X4                  42
#define sc_x1_speed_override3_sc_S_40G_HG2_CR4             44
#define sc_x1_speed_override3_sc_S_40G_HG2_KR4             45
#define sc_x1_speed_override3_sc_S_40G_HG2_X4              46
#define sc_x1_speed_override3_sc_S_50G_CR2                 48
#define sc_x1_speed_override3_sc_S_50G_KR2                 49
#define sc_x1_speed_override3_sc_S_50G_X2                  50
#define sc_x1_speed_override3_sc_S_50G_HG2_CR2             52
#define sc_x1_speed_override3_sc_S_50G_HG2_KR2             53
#define sc_x1_speed_override3_sc_S_50G_HG2_X2              54
#define sc_x1_speed_override3_sc_S_50G_CR4                 56
#define sc_x1_speed_override3_sc_S_50G_KR4                 57
#define sc_x1_speed_override3_sc_S_50G_X4                  58
#define sc_x1_speed_override3_sc_S_50G_HG2_CR4             60
#define sc_x1_speed_override3_sc_S_50G_HG2_KR4             61
#define sc_x1_speed_override3_sc_S_50G_HG2_X4              62
#define sc_x1_speed_override3_sc_S_100G_CR4                64
#define sc_x1_speed_override3_sc_S_100G_KR4                65
#define sc_x1_speed_override3_sc_S_100G_X4                 66
#define sc_x1_speed_override3_sc_S_100G_HG2_CR4            68
#define sc_x1_speed_override3_sc_S_100G_HG2_KR4            69
#define sc_x1_speed_override3_sc_S_100G_HG2_X4             70
#define sc_x1_speed_override3_sc_S_CL73_20GVCO             72
#define sc_x1_speed_override3_sc_S_CL73_25GVCO             80
#define sc_x1_speed_override3_sc_S_CL36_20GVCO             88
#define sc_x1_speed_override3_sc_S_CL36_25GVCO             96

/****************************************************************************
 * Enums: sc_x1_speed_override3_t_fifo_modes
 */
#define sc_x1_speed_override3_t_fifo_modes_T_FIFO_MODE_BYPASS 0
#define sc_x1_speed_override3_t_fifo_modes_T_FIFO_MODE_40G 1
#define sc_x1_speed_override3_t_fifo_modes_T_FIFO_MODE_100G 2
#define sc_x1_speed_override3_t_fifo_modes_T_FIFO_MODE_20G 3

/****************************************************************************
 * Enums: sc_x1_speed_override3_t_enc_modes
 */
#define sc_x1_speed_override3_t_enc_modes_T_ENC_MODE_BYPASS 0
#define sc_x1_speed_override3_t_enc_modes_T_ENC_MODE_CL49  1
#define sc_x1_speed_override3_t_enc_modes_T_ENC_MODE_CL82  2

/****************************************************************************
 * Enums: sc_x1_speed_override3_btmx_mode
 */
#define sc_x1_speed_override3_btmx_mode_BS_BTMX_MODE_1to1  0
#define sc_x1_speed_override3_btmx_mode_BS_BTMX_MODE_2to1  1
#define sc_x1_speed_override3_btmx_mode_BS_BTMX_MODE_5to1  2

/****************************************************************************
 * Enums: sc_x1_speed_override3_t_type_cl82
 */
#define sc_x1_speed_override3_t_type_cl82_T_TYPE_B1        5
#define sc_x1_speed_override3_t_type_cl82_T_TYPE_C         4
#define sc_x1_speed_override3_t_type_cl82_T_TYPE_S         3
#define sc_x1_speed_override3_t_type_cl82_T_TYPE_T         2
#define sc_x1_speed_override3_t_type_cl82_T_TYPE_D         1
#define sc_x1_speed_override3_t_type_cl82_T_TYPE_E         0

/****************************************************************************
 * Enums: sc_x1_speed_override3_txsm_state_cl82
 */
#define sc_x1_speed_override3_txsm_state_cl82_TX_HIG_END   6
#define sc_x1_speed_override3_txsm_state_cl82_TX_HIG_START 5
#define sc_x1_speed_override3_txsm_state_cl82_TX_E         4
#define sc_x1_speed_override3_txsm_state_cl82_TX_T         3
#define sc_x1_speed_override3_txsm_state_cl82_TX_D         2
#define sc_x1_speed_override3_txsm_state_cl82_TX_C         1
#define sc_x1_speed_override3_txsm_state_cl82_TX_INIT      0

/****************************************************************************
 * Enums: sc_x1_speed_override3_ltxsm_state_cl82
 */
#define sc_x1_speed_override3_ltxsm_state_cl82_TX_HIG_END  64
#define sc_x1_speed_override3_ltxsm_state_cl82_TX_HIG_START 32
#define sc_x1_speed_override3_ltxsm_state_cl82_TX_E        16
#define sc_x1_speed_override3_ltxsm_state_cl82_TX_T        8
#define sc_x1_speed_override3_ltxsm_state_cl82_TX_D        4
#define sc_x1_speed_override3_ltxsm_state_cl82_TX_C        2
#define sc_x1_speed_override3_ltxsm_state_cl82_TX_INIT     1

/****************************************************************************
 * Enums: sc_x1_speed_override3_r_type_coded_cl82
 */
#define sc_x1_speed_override3_r_type_coded_cl82_R_TYPE_B1  32
#define sc_x1_speed_override3_r_type_coded_cl82_R_TYPE_C   16
#define sc_x1_speed_override3_r_type_coded_cl82_R_TYPE_S   8
#define sc_x1_speed_override3_r_type_coded_cl82_R_TYPE_T   4
#define sc_x1_speed_override3_r_type_coded_cl82_R_TYPE_D   2
#define sc_x1_speed_override3_r_type_coded_cl82_R_TYPE_E   1

/****************************************************************************
 * Enums: sc_x1_speed_override3_rxsm_state_cl82
 */
#define sc_x1_speed_override3_rxsm_state_cl82_RX_HIG_END   64
#define sc_x1_speed_override3_rxsm_state_cl82_RX_HIG_START 32
#define sc_x1_speed_override3_rxsm_state_cl82_RX_E         16
#define sc_x1_speed_override3_rxsm_state_cl82_RX_T         8
#define sc_x1_speed_override3_rxsm_state_cl82_RX_D         4
#define sc_x1_speed_override3_rxsm_state_cl82_RX_C         2
#define sc_x1_speed_override3_rxsm_state_cl82_RX_INIT      1

/****************************************************************************
 * Enums: sc_x1_speed_override3_deskew_state
 */
#define sc_x1_speed_override3_deskew_state_ALIGN_ACQUIRED  2
#define sc_x1_speed_override3_deskew_state_LOSS_OF_ALIGNMENT 1

/****************************************************************************
 * Enums: sc_x1_speed_override3_os_mode_enum
 */
#define sc_x1_speed_override3_os_mode_enum_OS_MODE_1       0
#define sc_x1_speed_override3_os_mode_enum_OS_MODE_2       1
#define sc_x1_speed_override3_os_mode_enum_OS_MODE_4       2
#define sc_x1_speed_override3_os_mode_enum_OS_MODE_16p5    8
#define sc_x1_speed_override3_os_mode_enum_OS_MODE_20p625  12

/****************************************************************************
 * Enums: sc_x1_speed_override3_scr_modes
 */
#define sc_x1_speed_override3_scr_modes_T_SCR_MODE_BYPASS  0
#define sc_x1_speed_override3_scr_modes_T_SCR_MODE_CL49    1
#define sc_x1_speed_override3_scr_modes_T_SCR_MODE_40G_2_LANE 2
#define sc_x1_speed_override3_scr_modes_T_SCR_MODE_100G    3
#define sc_x1_speed_override3_scr_modes_T_SCR_MODE_20G     4
#define sc_x1_speed_override3_scr_modes_T_SCR_MODE_40G_4_LANE 5

/****************************************************************************
 * Enums: sc_x1_speed_override3_descr_modes
 */
#define sc_x1_speed_override3_descr_modes_R_DESCR_MODE_BYPASS 0
#define sc_x1_speed_override3_descr_modes_R_DESCR_MODE_CL49 1
#define sc_x1_speed_override3_descr_modes_R_DESCR_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override3_r_dec_tl_mode
 */
#define sc_x1_speed_override3_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS 0
#define sc_x1_speed_override3_r_dec_tl_mode_R_DEC_TL_MODE_CL49 1
#define sc_x1_speed_override3_r_dec_tl_mode_R_DEC_TL_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override3_r_dec_fsm_mode
 */
#define sc_x1_speed_override3_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define sc_x1_speed_override3_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49 1
#define sc_x1_speed_override3_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x1_speed_override3_r_deskew_mode
 */
#define sc_x1_speed_override3_r_deskew_mode_R_DESKEW_MODE_BYPASS 0
#define sc_x1_speed_override3_r_deskew_mode_R_DESKEW_MODE_20G 1
#define sc_x1_speed_override3_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define sc_x1_speed_override3_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define sc_x1_speed_override3_r_deskew_mode_R_DESKEW_MODE_100G 4
#define sc_x1_speed_override3_r_deskew_mode_R_DESKEW_MODE_CL49 5
#define sc_x1_speed_override3_r_deskew_mode_R_DESKEW_MODE_CL91 6

/****************************************************************************
 * Enums: sc_x1_speed_override3_bs_dist_modes
 */
#define sc_x1_speed_override3_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define sc_x1_speed_override3_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define sc_x1_speed_override3_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define sc_x1_speed_override3_bs_dist_modes_BS_DIST_MODE_NO_TDM 3

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl49_t_type
 */
#define sc_x1_speed_override3_cl49_t_type_BAD_T_TYPE       15
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_B0        11
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_OB        10
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_B1        9
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_DB        8
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_FC        7
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_TB        6
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_LI        5
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_C         4
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_S         3
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_T         2
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_D         1
#define sc_x1_speed_override3_cl49_t_type_T_TYPE_E         0

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl49_txsm_states
 */
#define sc_x1_speed_override3_cl49_txsm_states_TX_HIG_END  7
#define sc_x1_speed_override3_cl49_txsm_states_TX_HIG_START 6
#define sc_x1_speed_override3_cl49_txsm_states_TX_LI       5
#define sc_x1_speed_override3_cl49_txsm_states_TX_E        4
#define sc_x1_speed_override3_cl49_txsm_states_TX_T        3
#define sc_x1_speed_override3_cl49_txsm_states_TX_D        2
#define sc_x1_speed_override3_cl49_txsm_states_TX_C        1
#define sc_x1_speed_override3_cl49_txsm_states_TX_INIT     0

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl49_ltxsm_states
 */
#define sc_x1_speed_override3_cl49_ltxsm_states_TX_HIG_END 128
#define sc_x1_speed_override3_cl49_ltxsm_states_TX_HIG_START 64
#define sc_x1_speed_override3_cl49_ltxsm_states_TX_LI      32
#define sc_x1_speed_override3_cl49_ltxsm_states_TX_E       16
#define sc_x1_speed_override3_cl49_ltxsm_states_TX_T       8
#define sc_x1_speed_override3_cl49_ltxsm_states_TX_D       4
#define sc_x1_speed_override3_cl49_ltxsm_states_TX_C       2
#define sc_x1_speed_override3_cl49_ltxsm_states_TX_INIT    1

/****************************************************************************
 * Enums: sc_x1_speed_override3_burst_error_mode
 */
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_11_BITS 0
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_16_BITS 1
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_17_BITS 2
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_18_BITS 3
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_19_BITS 4
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_20_BITS 5
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_21_BITS 6
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_22_BITS 7
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_23_BITS 8
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_24_BITS 9
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_25_BITS 10
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_26_BITS 11
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_27_BITS 12
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_28_BITS 13
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_29_BITS 14
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_30_BITS 15
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_31_BITS 16
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_32_BITS 17
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_33_BITS 18
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_34_BITS 19
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_35_BITS 20
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_36_BITS 21
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_37_BITS 22
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_38_BITS 23
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_39_BITS 24
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_40_BITS 25
#define sc_x1_speed_override3_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: sc_x1_speed_override3_bermon_state
 */
#define sc_x1_speed_override3_bermon_state_HI_BER          4
#define sc_x1_speed_override3_bermon_state_GOOD_BER        3
#define sc_x1_speed_override3_bermon_state_BER_TEST_SH     2
#define sc_x1_speed_override3_bermon_state_START_TIMER     1
#define sc_x1_speed_override3_bermon_state_BER_MT_INIT     0

/****************************************************************************
 * Enums: sc_x1_speed_override3_rxsm_state_cl49
 */
#define sc_x1_speed_override3_rxsm_state_cl49_RX_HIG_END   128
#define sc_x1_speed_override3_rxsm_state_cl49_RX_HIG_START 64
#define sc_x1_speed_override3_rxsm_state_cl49_RX_LI        32
#define sc_x1_speed_override3_rxsm_state_cl49_RX_E         16
#define sc_x1_speed_override3_rxsm_state_cl49_RX_T         8
#define sc_x1_speed_override3_rxsm_state_cl49_RX_D         4
#define sc_x1_speed_override3_rxsm_state_cl49_RX_C         2
#define sc_x1_speed_override3_rxsm_state_cl49_RX_INIT      1

/****************************************************************************
 * Enums: sc_x1_speed_override3_r_type
 */
#define sc_x1_speed_override3_r_type_BAD_R_TYPE            15
#define sc_x1_speed_override3_r_type_R_TYPE_B0             11
#define sc_x1_speed_override3_r_type_R_TYPE_OB             10
#define sc_x1_speed_override3_r_type_R_TYPE_B1             9
#define sc_x1_speed_override3_r_type_R_TYPE_DB             8
#define sc_x1_speed_override3_r_type_R_TYPE_FC             7
#define sc_x1_speed_override3_r_type_R_TYPE_TB             6
#define sc_x1_speed_override3_r_type_R_TYPE_LI             5
#define sc_x1_speed_override3_r_type_R_TYPE_C              4
#define sc_x1_speed_override3_r_type_R_TYPE_S              3
#define sc_x1_speed_override3_r_type_R_TYPE_T              2
#define sc_x1_speed_override3_r_type_R_TYPE_D              1
#define sc_x1_speed_override3_r_type_R_TYPE_E              0

/****************************************************************************
 * Enums: sc_x1_speed_override3_am_lock_state
 */
#define sc_x1_speed_override3_am_lock_state_INVALID_AM     512
#define sc_x1_speed_override3_am_lock_state_GOOD_AM        256
#define sc_x1_speed_override3_am_lock_state_COMP_AM        128
#define sc_x1_speed_override3_am_lock_state_TIMER_2        64
#define sc_x1_speed_override3_am_lock_state_AM_2_GOOD      32
#define sc_x1_speed_override3_am_lock_state_COMP_2ND       16
#define sc_x1_speed_override3_am_lock_state_TIMER_1        8
#define sc_x1_speed_override3_am_lock_state_FIND_1ST       4
#define sc_x1_speed_override3_am_lock_state_AM_RESET_CNT   2
#define sc_x1_speed_override3_am_lock_state_AM_LOCK_INIT   1

/****************************************************************************
 * Enums: sc_x1_speed_override3_msg_selector
 */
#define sc_x1_speed_override3_msg_selector_RESERVED        0
#define sc_x1_speed_override3_msg_selector_VALUE_802p3     1
#define sc_x1_speed_override3_msg_selector_VALUE_802p9     2
#define sc_x1_speed_override3_msg_selector_VALUE_802p5     3
#define sc_x1_speed_override3_msg_selector_VALUE_1394      4

/****************************************************************************
 * Enums: sc_x1_speed_override3_synce_enum
 */
#define sc_x1_speed_override3_synce_enum_SYNCE_NO_DIV      0
#define sc_x1_speed_override3_synce_enum_SYNCE_DIV_7       1
#define sc_x1_speed_override3_synce_enum_SYNCE_DIV_11      2

/****************************************************************************
 * Enums: sc_x1_speed_override3_synce_enum_stage0
 */
#define sc_x1_speed_override3_synce_enum_stage0_SYNCE_NO_DIV 0
#define sc_x1_speed_override3_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define sc_x1_speed_override3_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl91_sync_state
 */
#define sc_x1_speed_override3_cl91_sync_state_FIND_1ST     0
#define sc_x1_speed_override3_cl91_sync_state_COUNT_NEXT   1
#define sc_x1_speed_override3_cl91_sync_state_COMP_2ND     2
#define sc_x1_speed_override3_cl91_sync_state_TWO_GOOD     3

/****************************************************************************
 * Enums: sc_x1_speed_override3_cl91_algn_state
 */
#define sc_x1_speed_override3_cl91_algn_state_LOSS_OF_ALIGNMENT 0
#define sc_x1_speed_override3_cl91_algn_state_DESKEW       1
#define sc_x1_speed_override3_cl91_algn_state_DESKEW_FAIL  2
#define sc_x1_speed_override3_cl91_algn_state_ALIGN_ACQUIRED 3
#define sc_x1_speed_override3_cl91_algn_state_CW_GOOD      4
#define sc_x1_speed_override3_cl91_algn_state_CW_BAD       5
#define sc_x1_speed_override3_cl91_algn_state_THREE_BAD    6

/****************************************************************************
 * Enums: sc_x1_speed_override3_fec_sel
 */
#define sc_x1_speed_override3_fec_sel_NO_FEC               0
#define sc_x1_speed_override3_fec_sel_FEC_CL74             1
#define sc_x1_speed_override3_fec_sel_FEC_CL91             2

/****************************************************************************
 * Enums: sc_x4_control_cl36TxEEEStates_l
 */
#define sc_x4_control_cl36TxEEEStates_l_TX_REFRESH         8
#define sc_x4_control_cl36TxEEEStates_l_TX_QUIET           4
#define sc_x4_control_cl36TxEEEStates_l_TX_SLEEP           2
#define sc_x4_control_cl36TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: sc_x4_control_cl36TxEEEStates_c
 */
#define sc_x4_control_cl36TxEEEStates_c_TX_REFRESH         3
#define sc_x4_control_cl36TxEEEStates_c_TX_QUIET           2
#define sc_x4_control_cl36TxEEEStates_c_TX_SLEEP           1
#define sc_x4_control_cl36TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: sc_x4_control_cl49TxEEEStates_l
 */
#define sc_x4_control_cl49TxEEEStates_l_SCR_RESET_2        64
#define sc_x4_control_cl49TxEEEStates_l_SCR_RESET_1        32
#define sc_x4_control_cl49TxEEEStates_l_TX_WAKE            16
#define sc_x4_control_cl49TxEEEStates_l_TX_REFRESH         8
#define sc_x4_control_cl49TxEEEStates_l_TX_QUIET           4
#define sc_x4_control_cl49TxEEEStates_l_TX_SLEEP           2
#define sc_x4_control_cl49TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: sc_x4_control_fec_req_enum
 */
#define sc_x4_control_fec_req_enum_FEC_not_supported       0
#define sc_x4_control_fec_req_enum_FEC_supported_but_not_requested 1
#define sc_x4_control_fec_req_enum_invalid_setting         2
#define sc_x4_control_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: sc_x4_control_cl73_pause_enum
 */
#define sc_x4_control_cl73_pause_enum_No_PAUSE_ability     0
#define sc_x4_control_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define sc_x4_control_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define sc_x4_control_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: sc_x4_control_cl49TxEEEStates_c
 */
#define sc_x4_control_cl49TxEEEStates_c_SCR_RESET_2        6
#define sc_x4_control_cl49TxEEEStates_c_SCR_RESET_1        5
#define sc_x4_control_cl49TxEEEStates_c_TX_WAKE            4
#define sc_x4_control_cl49TxEEEStates_c_TX_REFRESH         3
#define sc_x4_control_cl49TxEEEStates_c_TX_QUIET           2
#define sc_x4_control_cl49TxEEEStates_c_TX_SLEEP           1
#define sc_x4_control_cl49TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: sc_x4_control_cl36RxEEEStates_l
 */
#define sc_x4_control_cl36RxEEEStates_l_RX_LINK_FAIL       32
#define sc_x4_control_cl36RxEEEStates_l_RX_WTF             16
#define sc_x4_control_cl36RxEEEStates_l_RX_WAKE            8
#define sc_x4_control_cl36RxEEEStates_l_RX_QUIET           4
#define sc_x4_control_cl36RxEEEStates_l_RX_SLEEP           2
#define sc_x4_control_cl36RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: sc_x4_control_cl36RxEEEStates_c
 */
#define sc_x4_control_cl36RxEEEStates_c_RX_LINK_FAIL       5
#define sc_x4_control_cl36RxEEEStates_c_RX_WTF             4
#define sc_x4_control_cl36RxEEEStates_c_RX_WAKE            3
#define sc_x4_control_cl36RxEEEStates_c_RX_QUIET           2
#define sc_x4_control_cl36RxEEEStates_c_RX_SLEEP           1
#define sc_x4_control_cl36RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: sc_x4_control_cl49RxEEEStates_l
 */
#define sc_x4_control_cl49RxEEEStates_l_RX_LINK_FAIL       32
#define sc_x4_control_cl49RxEEEStates_l_RX_WTF             16
#define sc_x4_control_cl49RxEEEStates_l_RX_WAKE            8
#define sc_x4_control_cl49RxEEEStates_l_RX_QUIET           4
#define sc_x4_control_cl49RxEEEStates_l_RX_SLEEP           2
#define sc_x4_control_cl49RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: sc_x4_control_cl49RxEEEStates_c
 */
#define sc_x4_control_cl49RxEEEStates_c_RX_LINK_FAIL       5
#define sc_x4_control_cl49RxEEEStates_c_RX_WTF             4
#define sc_x4_control_cl49RxEEEStates_c_RX_WAKE            3
#define sc_x4_control_cl49RxEEEStates_c_RX_QUIET           2
#define sc_x4_control_cl49RxEEEStates_c_RX_SLEEP           1
#define sc_x4_control_cl49RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: sc_x4_control_cl48TxEEEStates_l
 */
#define sc_x4_control_cl48TxEEEStates_l_TX_REFRESH         8
#define sc_x4_control_cl48TxEEEStates_l_TX_QUIET           4
#define sc_x4_control_cl48TxEEEStates_l_TX_SLEEP           2
#define sc_x4_control_cl48TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: sc_x4_control_cl48TxEEEStates_c
 */
#define sc_x4_control_cl48TxEEEStates_c_TX_REFRESH         3
#define sc_x4_control_cl48TxEEEStates_c_TX_QUIET           2
#define sc_x4_control_cl48TxEEEStates_c_TX_SLEEP           1
#define sc_x4_control_cl48TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: sc_x4_control_cl48RxEEEStates_l
 */
#define sc_x4_control_cl48RxEEEStates_l_RX_LINK_FAIL       32
#define sc_x4_control_cl48RxEEEStates_l_RX_WAKE            16
#define sc_x4_control_cl48RxEEEStates_l_RX_QUIET           8
#define sc_x4_control_cl48RxEEEStates_l_RX_DEACT           4
#define sc_x4_control_cl48RxEEEStates_l_RX_SLEEP           2
#define sc_x4_control_cl48RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: sc_x4_control_cl48RxEEEStates_c
 */
#define sc_x4_control_cl48RxEEEStates_c_RX_LINK_FAIL       5
#define sc_x4_control_cl48RxEEEStates_c_RX_WAKE            4
#define sc_x4_control_cl48RxEEEStates_c_RX_QUIET           3
#define sc_x4_control_cl48RxEEEStates_c_RX_DEACT           2
#define sc_x4_control_cl48RxEEEStates_c_RX_SLEEP           1
#define sc_x4_control_cl48RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: sc_x4_control_IQP_Options
 */
#define sc_x4_control_IQP_Options_i50uA                    0
#define sc_x4_control_IQP_Options_i100uA                   1
#define sc_x4_control_IQP_Options_i150uA                   2
#define sc_x4_control_IQP_Options_i200uA                   3
#define sc_x4_control_IQP_Options_i250uA                   4
#define sc_x4_control_IQP_Options_i300uA                   5
#define sc_x4_control_IQP_Options_i350uA                   6
#define sc_x4_control_IQP_Options_i400uA                   7
#define sc_x4_control_IQP_Options_i450uA                   8
#define sc_x4_control_IQP_Options_i500uA                   9
#define sc_x4_control_IQP_Options_i550uA                   10
#define sc_x4_control_IQP_Options_i600uA                   11
#define sc_x4_control_IQP_Options_i650uA                   12
#define sc_x4_control_IQP_Options_i700uA                   13
#define sc_x4_control_IQP_Options_i750uA                   14
#define sc_x4_control_IQP_Options_i800uA                   15

/****************************************************************************
 * Enums: sc_x4_control_IDriver_Options
 */
#define sc_x4_control_IDriver_Options_v680mV               0
#define sc_x4_control_IDriver_Options_v730mV               1
#define sc_x4_control_IDriver_Options_v780mV               2
#define sc_x4_control_IDriver_Options_v830mV               3
#define sc_x4_control_IDriver_Options_v880mV               4
#define sc_x4_control_IDriver_Options_v930mV               5
#define sc_x4_control_IDriver_Options_v980mV               6
#define sc_x4_control_IDriver_Options_v1010mV              7
#define sc_x4_control_IDriver_Options_v1040mV              8
#define sc_x4_control_IDriver_Options_v1060mV              9
#define sc_x4_control_IDriver_Options_v1070mV              10
#define sc_x4_control_IDriver_Options_v1080mV              11
#define sc_x4_control_IDriver_Options_v1085mV              12
#define sc_x4_control_IDriver_Options_v1090mV              13
#define sc_x4_control_IDriver_Options_v1095mV              14
#define sc_x4_control_IDriver_Options_v1100mV              15

/****************************************************************************
 * Enums: sc_x4_control_operationModes
 */
#define sc_x4_control_operationModes_XGXS                  0
#define sc_x4_control_operationModes_XGXG_nCC              1
#define sc_x4_control_operationModes_Indlane_OS8           4
#define sc_x4_control_operationModes_IndLane_OS5           5
#define sc_x4_control_operationModes_PCI                   7
#define sc_x4_control_operationModes_XGXS_nLQ              8
#define sc_x4_control_operationModes_XGXS_nLQnCC           9
#define sc_x4_control_operationModes_PBypass               10
#define sc_x4_control_operationModes_PBypass_nDSK          11
#define sc_x4_control_operationModes_ComboCoreMode         12
#define sc_x4_control_operationModes_Clocks_off            15

/****************************************************************************
 * Enums: sc_x4_control_actualSpeeds
 */
#define sc_x4_control_actualSpeeds_dr_10M                  0
#define sc_x4_control_actualSpeeds_dr_100M                 1
#define sc_x4_control_actualSpeeds_dr_1G                   2
#define sc_x4_control_actualSpeeds_dr_2p5G                 3
#define sc_x4_control_actualSpeeds_dr_5G_X4                4
#define sc_x4_control_actualSpeeds_dr_6G_X4                5
#define sc_x4_control_actualSpeeds_dr_10G_HiG              6
#define sc_x4_control_actualSpeeds_dr_10G_CX4              7
#define sc_x4_control_actualSpeeds_dr_12G_HiG              8
#define sc_x4_control_actualSpeeds_dr_12p5G_X4             9
#define sc_x4_control_actualSpeeds_dr_13G_X4               10
#define sc_x4_control_actualSpeeds_dr_15G_X4               11
#define sc_x4_control_actualSpeeds_dr_16G_X4               12
#define sc_x4_control_actualSpeeds_dr_1G_KX                13
#define sc_x4_control_actualSpeeds_dr_10G_KX4              14
#define sc_x4_control_actualSpeeds_dr_10G_KR               15
#define sc_x4_control_actualSpeeds_dr_5G                   16
#define sc_x4_control_actualSpeeds_dr_6p4G                 17
#define sc_x4_control_actualSpeeds_dr_20G_X4               18
#define sc_x4_control_actualSpeeds_dr_21G_X4               19
#define sc_x4_control_actualSpeeds_dr_25G_X4               20
#define sc_x4_control_actualSpeeds_dr_10G_HiG_DXGXS        21
#define sc_x4_control_actualSpeeds_dr_10G_DXGXS            22
#define sc_x4_control_actualSpeeds_dr_10p5G_HiG_DXGXS      23
#define sc_x4_control_actualSpeeds_dr_10p5G_DXGXS          24
#define sc_x4_control_actualSpeeds_dr_12p773G_HiG_DXGXS    25
#define sc_x4_control_actualSpeeds_dr_12p773G_DXGXS        26
#define sc_x4_control_actualSpeeds_dr_10G_XFI              27
#define sc_x4_control_actualSpeeds_dr_40G                  28
#define sc_x4_control_actualSpeeds_dr_20G_HiG_DXGXS        29
#define sc_x4_control_actualSpeeds_dr_20G_DXGXS            30
#define sc_x4_control_actualSpeeds_dr_10G_SFI              31
#define sc_x4_control_actualSpeeds_dr_31p5G                32
#define sc_x4_control_actualSpeeds_dr_32p7G                33
#define sc_x4_control_actualSpeeds_dr_20G_SCR              34
#define sc_x4_control_actualSpeeds_dr_10G_HiG_DXGXS_SCR    35
#define sc_x4_control_actualSpeeds_dr_10G_DXGXS_SCR        36
#define sc_x4_control_actualSpeeds_dr_12G_R2               37
#define sc_x4_control_actualSpeeds_dr_10G_X2               38
#define sc_x4_control_actualSpeeds_dr_40G_KR4              39
#define sc_x4_control_actualSpeeds_dr_40G_CR4              40
#define sc_x4_control_actualSpeeds_dr_100G_CR10            41
#define sc_x4_control_actualSpeeds_dr_15p75G_DXGXS         44
#define sc_x4_control_actualSpeeds_dr_20G_KR2              57
#define sc_x4_control_actualSpeeds_dr_20G_CR2              58

/****************************************************************************
 * Enums: sc_x4_control_actualSpeedsMisc1
 */
#define sc_x4_control_actualSpeedsMisc1_dr_2500BRCM_X1     16
#define sc_x4_control_actualSpeedsMisc1_dr_5000BRCM_X4     17
#define sc_x4_control_actualSpeedsMisc1_dr_6000BRCM_X4     18
#define sc_x4_control_actualSpeedsMisc1_dr_10GHiGig_X4     19
#define sc_x4_control_actualSpeedsMisc1_dr_10GBASE_CX4     20
#define sc_x4_control_actualSpeedsMisc1_dr_12GHiGig_X4     21
#define sc_x4_control_actualSpeedsMisc1_dr_12p5GHiGig_X4   22
#define sc_x4_control_actualSpeedsMisc1_dr_13GHiGig_X4     23
#define sc_x4_control_actualSpeedsMisc1_dr_15GHiGig_X4     24
#define sc_x4_control_actualSpeedsMisc1_dr_16GHiGig_X4     25
#define sc_x4_control_actualSpeedsMisc1_dr_5000BRCM_X1     26
#define sc_x4_control_actualSpeedsMisc1_dr_6363BRCM_X1     27
#define sc_x4_control_actualSpeedsMisc1_dr_20GHiGig_X4     28
#define sc_x4_control_actualSpeedsMisc1_dr_21GHiGig_X4     29
#define sc_x4_control_actualSpeedsMisc1_dr_25p45GHiGig_X4  30
#define sc_x4_control_actualSpeedsMisc1_dr_10G_HiG_DXGXS   31

/****************************************************************************
 * Enums: sc_x4_control_IndLaneModes
 */
#define sc_x4_control_IndLaneModes_SWSDR_div2              0
#define sc_x4_control_IndLaneModes_SWSDR_div1              1
#define sc_x4_control_IndLaneModes_DWSDR_div2              2
#define sc_x4_control_IndLaneModes_DWSDR_div1              3

/****************************************************************************
 * Enums: sc_x4_control_prbsSelect
 */
#define sc_x4_control_prbsSelect_prbs7                     0
#define sc_x4_control_prbsSelect_prbs15                    1
#define sc_x4_control_prbsSelect_prbs23                    2
#define sc_x4_control_prbsSelect_prbs31                    3

/****************************************************************************
 * Enums: sc_x4_control_vcoDivider
 */
#define sc_x4_control_vcoDivider_div32                     0
#define sc_x4_control_vcoDivider_div36                     1
#define sc_x4_control_vcoDivider_div40                     2
#define sc_x4_control_vcoDivider_div42                     3
#define sc_x4_control_vcoDivider_div48                     4
#define sc_x4_control_vcoDivider_div50                     5
#define sc_x4_control_vcoDivider_div52                     6
#define sc_x4_control_vcoDivider_div54                     7
#define sc_x4_control_vcoDivider_div60                     8
#define sc_x4_control_vcoDivider_div64                     9
#define sc_x4_control_vcoDivider_div66                     10
#define sc_x4_control_vcoDivider_div68                     11
#define sc_x4_control_vcoDivider_div70                     12
#define sc_x4_control_vcoDivider_div80                     13
#define sc_x4_control_vcoDivider_div92                     14
#define sc_x4_control_vcoDivider_div100                    15

/****************************************************************************
 * Enums: sc_x4_control_refClkSelect
 */
#define sc_x4_control_refClkSelect_clk_25MHz               0
#define sc_x4_control_refClkSelect_clk_100MHz              1
#define sc_x4_control_refClkSelect_clk_125MHz              2
#define sc_x4_control_refClkSelect_clk_156p25MHz           3
#define sc_x4_control_refClkSelect_clk_187p5MHz            4
#define sc_x4_control_refClkSelect_clk_161p25Mhz           5
#define sc_x4_control_refClkSelect_clk_50Mhz               6
#define sc_x4_control_refClkSelect_clk_106p25Mhz           7

/****************************************************************************
 * Enums: sc_x4_control_aerMMDdevTypeSelect
 */
#define sc_x4_control_aerMMDdevTypeSelect_combo_core       0
#define sc_x4_control_aerMMDdevTypeSelect_PMA_PMD          1
#define sc_x4_control_aerMMDdevTypeSelect_PCS              3
#define sc_x4_control_aerMMDdevTypeSelect_PHY              4
#define sc_x4_control_aerMMDdevTypeSelect_DTE              5
#define sc_x4_control_aerMMDdevTypeSelect_CL73_AN          7

/****************************************************************************
 * Enums: sc_x4_control_aerMMDportSelect
 */
#define sc_x4_control_aerMMDportSelect_ln0                 0
#define sc_x4_control_aerMMDportSelect_ln1                 1
#define sc_x4_control_aerMMDportSelect_ln2                 2
#define sc_x4_control_aerMMDportSelect_ln3                 3
#define sc_x4_control_aerMMDportSelect_BCST_ln0_1_2_3      511
#define sc_x4_control_aerMMDportSelect_BCST_ln0_1          512
#define sc_x4_control_aerMMDportSelect_BCST_ln2_3          513

/****************************************************************************
 * Enums: sc_x4_control_firmwareModeSelect
 */
#define sc_x4_control_firmwareModeSelect_DEFAULT           0
#define sc_x4_control_firmwareModeSelect_SFP_OPT_LR        1
#define sc_x4_control_firmwareModeSelect_SFP_DAC           2
#define sc_x4_control_firmwareModeSelect_XLAUI             3
#define sc_x4_control_firmwareModeSelect_LONG_CH_6G        4

/****************************************************************************
 * Enums: sc_x4_control_tempIdxSelect
 */
#define sc_x4_control_tempIdxSelect_LTE__22p9C             15
#define sc_x4_control_tempIdxSelect_LTE__12p6C             14
#define sc_x4_control_tempIdxSelect_LTE__3p0C              13
#define sc_x4_control_tempIdxSelect_LTE_6p7C               12
#define sc_x4_control_tempIdxSelect_LTE_16p4C              11
#define sc_x4_control_tempIdxSelect_LTE_26p6C              10
#define sc_x4_control_tempIdxSelect_LTE_36p3C              9
#define sc_x4_control_tempIdxSelect_LTE_46p0C              8
#define sc_x4_control_tempIdxSelect_LTE_56p2C              7
#define sc_x4_control_tempIdxSelect_LTE_65p9C              6
#define sc_x4_control_tempIdxSelect_LTE_75p6C              5
#define sc_x4_control_tempIdxSelect_LTE_85p3C              4
#define sc_x4_control_tempIdxSelect_LTE_95p5C              3
#define sc_x4_control_tempIdxSelect_LTE_105p2C             2
#define sc_x4_control_tempIdxSelect_LTE_114p9C             1
#define sc_x4_control_tempIdxSelect_LTE_125p1C             0

/****************************************************************************
 * Enums: sc_x4_control_port_mode
 */
#define sc_x4_control_port_mode_QUAD_PORT                  0
#define sc_x4_control_port_mode_TRI_1_PORT                 1
#define sc_x4_control_port_mode_TRI_2_PORT                 2
#define sc_x4_control_port_mode_DUAL_PORT                  3
#define sc_x4_control_port_mode_SINGLE_PORT                4

/****************************************************************************
 * Enums: sc_x4_control_rev_letter_enum
 */
#define sc_x4_control_rev_letter_enum_REV_A                0
#define sc_x4_control_rev_letter_enum_REV_B                1
#define sc_x4_control_rev_letter_enum_REV_C                2
#define sc_x4_control_rev_letter_enum_REV_D                3

/****************************************************************************
 * Enums: sc_x4_control_rev_number_enum
 */
#define sc_x4_control_rev_number_enum_REV_0                0
#define sc_x4_control_rev_number_enum_REV_1                1
#define sc_x4_control_rev_number_enum_REV_2                2
#define sc_x4_control_rev_number_enum_REV_3                3
#define sc_x4_control_rev_number_enum_REV_4                4
#define sc_x4_control_rev_number_enum_REV_5                5
#define sc_x4_control_rev_number_enum_REV_6                6
#define sc_x4_control_rev_number_enum_REV_7                7

/****************************************************************************
 * Enums: sc_x4_control_bonding_enum
 */
#define sc_x4_control_bonding_enum_WIRE_BOND               0
#define sc_x4_control_bonding_enum_FLIP_CHIP               1

/****************************************************************************
 * Enums: sc_x4_control_tech_process
 */
#define sc_x4_control_tech_process_PROCESS_90NM            0
#define sc_x4_control_tech_process_PROCESS_65NM            1
#define sc_x4_control_tech_process_PROCESS_40NM            2
#define sc_x4_control_tech_process_PROCESS_28NM            3

/****************************************************************************
 * Enums: sc_x4_control_model_num
 */
#define sc_x4_control_model_num_SERDES_CL73                0
#define sc_x4_control_model_num_XGXS_16G                   1
#define sc_x4_control_model_num_HYPERCORE                  2
#define sc_x4_control_model_num_HYPERLITE                  3
#define sc_x4_control_model_num_PCIE_G2_PIPE               4
#define sc_x4_control_model_num_SERDES_1p25GBd             5
#define sc_x4_control_model_num_SATA2                      6
#define sc_x4_control_model_num_QSGMII                     7
#define sc_x4_control_model_num_XGXS10G                    8
#define sc_x4_control_model_num_WARPCORE                   9
#define sc_x4_control_model_num_XFICORE                    10
#define sc_x4_control_model_num_RXFI                       11
#define sc_x4_control_model_num_WARPLITE                   12
#define sc_x4_control_model_num_PENTACORE                  13
#define sc_x4_control_model_num_ESM                        14
#define sc_x4_control_model_num_QUAD_SGMII                 15
#define sc_x4_control_model_num_WARPCORE_3                 16
#define sc_x4_control_model_num_TSC                        17
#define sc_x4_control_model_num_TSC4E                      18
#define sc_x4_control_model_num_TSC12E                     19
#define sc_x4_control_model_num_TSC4F                      20
#define sc_x4_control_model_num_XGXS_CL73_90NM             29
#define sc_x4_control_model_num_SERDES_CL73_90NM           30
#define sc_x4_control_model_num_WARPCORE3                  32
#define sc_x4_control_model_num_WARPCORE4_TSC              33
#define sc_x4_control_model_num_RXAUI                      34

/****************************************************************************
 * Enums: sc_x4_control_payload
 */
#define sc_x4_control_payload_REPEAT_2_BYTES               0
#define sc_x4_control_payload_RAMPING                      1
#define sc_x4_control_payload_CL48_CRPAT                   2
#define sc_x4_control_payload_CL48_CJPAT                   3
#define sc_x4_control_payload_CL36_LONG_CRPAT              4
#define sc_x4_control_payload_CL36_SHORT_CRPAT             5

/****************************************************************************
 * Enums: sc_x4_control_sc
 */
#define sc_x4_control_sc_S_10G_CR1                         0
#define sc_x4_control_sc_S_10G_KR1                         1
#define sc_x4_control_sc_S_10G_X1                          2
#define sc_x4_control_sc_S_10G_HG2_CR1                     4
#define sc_x4_control_sc_S_10G_HG2_KR1                     5
#define sc_x4_control_sc_S_10G_HG2_X1                      6
#define sc_x4_control_sc_S_20G_CR1                         8
#define sc_x4_control_sc_S_20G_KR1                         9
#define sc_x4_control_sc_S_20G_X1                          10
#define sc_x4_control_sc_S_20G_HG2_CR1                     12
#define sc_x4_control_sc_S_20G_HG2_KR1                     13
#define sc_x4_control_sc_S_20G_HG2_X1                      14
#define sc_x4_control_sc_S_25G_CR1                         16
#define sc_x4_control_sc_S_25G_KR1                         17
#define sc_x4_control_sc_S_25G_X1                          18
#define sc_x4_control_sc_S_25G_HG2_CR1                     20
#define sc_x4_control_sc_S_25G_HG2_KR1                     21
#define sc_x4_control_sc_S_25G_HG2_X1                      22
#define sc_x4_control_sc_S_20G_CR2                         24
#define sc_x4_control_sc_S_20G_KR2                         25
#define sc_x4_control_sc_S_20G_X2                          26
#define sc_x4_control_sc_S_20G_HG2_CR2                     28
#define sc_x4_control_sc_S_20G_HG2_KR2                     29
#define sc_x4_control_sc_S_20G_HG2_X2                      30
#define sc_x4_control_sc_S_40G_CR2                         32
#define sc_x4_control_sc_S_40G_KR2                         33
#define sc_x4_control_sc_S_40G_X2                          34
#define sc_x4_control_sc_S_40G_HG2_CR2                     36
#define sc_x4_control_sc_S_40G_HG2_KR2                     37
#define sc_x4_control_sc_S_40G_HG2_X2                      38
#define sc_x4_control_sc_S_40G_CR4                         40
#define sc_x4_control_sc_S_40G_KR4                         41
#define sc_x4_control_sc_S_40G_X4                          42
#define sc_x4_control_sc_S_40G_HG2_CR4                     44
#define sc_x4_control_sc_S_40G_HG2_KR4                     45
#define sc_x4_control_sc_S_40G_HG2_X4                      46
#define sc_x4_control_sc_S_50G_CR2                         48
#define sc_x4_control_sc_S_50G_KR2                         49
#define sc_x4_control_sc_S_50G_X2                          50
#define sc_x4_control_sc_S_50G_HG2_CR2                     52
#define sc_x4_control_sc_S_50G_HG2_KR2                     53
#define sc_x4_control_sc_S_50G_HG2_X2                      54
#define sc_x4_control_sc_S_50G_CR4                         56
#define sc_x4_control_sc_S_50G_KR4                         57
#define sc_x4_control_sc_S_50G_X4                          58
#define sc_x4_control_sc_S_50G_HG2_CR4                     60
#define sc_x4_control_sc_S_50G_HG2_KR4                     61
#define sc_x4_control_sc_S_50G_HG2_X4                      62
#define sc_x4_control_sc_S_100G_CR4                        64
#define sc_x4_control_sc_S_100G_KR4                        65
#define sc_x4_control_sc_S_100G_X4                         66
#define sc_x4_control_sc_S_100G_HG2_CR4                    68
#define sc_x4_control_sc_S_100G_HG2_KR4                    69
#define sc_x4_control_sc_S_100G_HG2_X4                     70
#define sc_x4_control_sc_S_CL73_20GVCO                     72
#define sc_x4_control_sc_S_CL73_25GVCO                     80
#define sc_x4_control_sc_S_CL36_20GVCO                     88
#define sc_x4_control_sc_S_CL36_25GVCO                     96
#define sc_x4_control_sc_S_10G_25GVCO                      98
#define sc_x4_control_sc_S_25G_CR_IEEE                     112
#define sc_x4_control_sc_S_25G_CRS_IEEE                    113
#define sc_x4_control_sc_S_25G_KR_IEEE                     114
#define sc_x4_control_sc_S_25G_KRS_IEEE                    115



/****************************************************************************
 * Enums: sc_x4_control_t_fifo_modes
 */
#define sc_x4_control_t_fifo_modes_T_FIFO_MODE_BYPASS      0
#define sc_x4_control_t_fifo_modes_T_FIFO_MODE_40G         1
#define sc_x4_control_t_fifo_modes_T_FIFO_MODE_100G        2
#define sc_x4_control_t_fifo_modes_T_FIFO_MODE_20G         3

/****************************************************************************
 * Enums: sc_x4_control_t_enc_modes
 */
#define sc_x4_control_t_enc_modes_T_ENC_MODE_BYPASS        0
#define sc_x4_control_t_enc_modes_T_ENC_MODE_CL49          1
#define sc_x4_control_t_enc_modes_T_ENC_MODE_CL82          2

/****************************************************************************
 * Enums: sc_x4_control_btmx_mode
 */
#define sc_x4_control_btmx_mode_BS_BTMX_MODE_1to1          0
#define sc_x4_control_btmx_mode_BS_BTMX_MODE_2to1          1
#define sc_x4_control_btmx_mode_BS_BTMX_MODE_5to1          2

/****************************************************************************
 * Enums: sc_x4_control_t_type_cl82
 */
#define sc_x4_control_t_type_cl82_T_TYPE_B1                5
#define sc_x4_control_t_type_cl82_T_TYPE_C                 4
#define sc_x4_control_t_type_cl82_T_TYPE_S                 3
#define sc_x4_control_t_type_cl82_T_TYPE_T                 2
#define sc_x4_control_t_type_cl82_T_TYPE_D                 1
#define sc_x4_control_t_type_cl82_T_TYPE_E                 0

/****************************************************************************
 * Enums: sc_x4_control_txsm_state_cl82
 */
#define sc_x4_control_txsm_state_cl82_TX_HIG_END           6
#define sc_x4_control_txsm_state_cl82_TX_HIG_START         5
#define sc_x4_control_txsm_state_cl82_TX_E                 4
#define sc_x4_control_txsm_state_cl82_TX_T                 3
#define sc_x4_control_txsm_state_cl82_TX_D                 2
#define sc_x4_control_txsm_state_cl82_TX_C                 1
#define sc_x4_control_txsm_state_cl82_TX_INIT              0

/****************************************************************************
 * Enums: sc_x4_control_ltxsm_state_cl82
 */
#define sc_x4_control_ltxsm_state_cl82_TX_HIG_END          64
#define sc_x4_control_ltxsm_state_cl82_TX_HIG_START        32
#define sc_x4_control_ltxsm_state_cl82_TX_E                16
#define sc_x4_control_ltxsm_state_cl82_TX_T                8
#define sc_x4_control_ltxsm_state_cl82_TX_D                4
#define sc_x4_control_ltxsm_state_cl82_TX_C                2
#define sc_x4_control_ltxsm_state_cl82_TX_INIT             1

/****************************************************************************
 * Enums: sc_x4_control_r_type_coded_cl82
 */
#define sc_x4_control_r_type_coded_cl82_R_TYPE_B1          32
#define sc_x4_control_r_type_coded_cl82_R_TYPE_C           16
#define sc_x4_control_r_type_coded_cl82_R_TYPE_S           8
#define sc_x4_control_r_type_coded_cl82_R_TYPE_T           4
#define sc_x4_control_r_type_coded_cl82_R_TYPE_D           2
#define sc_x4_control_r_type_coded_cl82_R_TYPE_E           1

/****************************************************************************
 * Enums: sc_x4_control_rxsm_state_cl82
 */
#define sc_x4_control_rxsm_state_cl82_RX_HIG_END           64
#define sc_x4_control_rxsm_state_cl82_RX_HIG_START         32
#define sc_x4_control_rxsm_state_cl82_RX_E                 16
#define sc_x4_control_rxsm_state_cl82_RX_T                 8
#define sc_x4_control_rxsm_state_cl82_RX_D                 4
#define sc_x4_control_rxsm_state_cl82_RX_C                 2
#define sc_x4_control_rxsm_state_cl82_RX_INIT              1

/****************************************************************************
 * Enums: sc_x4_control_deskew_state
 */
#define sc_x4_control_deskew_state_ALIGN_ACQUIRED          2
#define sc_x4_control_deskew_state_LOSS_OF_ALIGNMENT       1

/****************************************************************************
 * Enums: sc_x4_control_os_mode_enum
 */
#define sc_x4_control_os_mode_enum_OS_MODE_1               0
#define sc_x4_control_os_mode_enum_OS_MODE_2               1
#define sc_x4_control_os_mode_enum_OS_MODE_4               2
#define sc_x4_control_os_mode_enum_OS_MODE_16p5            8
#define sc_x4_control_os_mode_enum_OS_MODE_20p625          12

/****************************************************************************
 * Enums: sc_x4_control_scr_modes
 */
#define sc_x4_control_scr_modes_T_SCR_MODE_BYPASS          0
#define sc_x4_control_scr_modes_T_SCR_MODE_CL49            1
#define sc_x4_control_scr_modes_T_SCR_MODE_40G_2_LANE      2
#define sc_x4_control_scr_modes_T_SCR_MODE_100G            3
#define sc_x4_control_scr_modes_T_SCR_MODE_20G             4
#define sc_x4_control_scr_modes_T_SCR_MODE_40G_4_LANE      5

/****************************************************************************
 * Enums: sc_x4_control_descr_modes
 */
#define sc_x4_control_descr_modes_R_DESCR_MODE_BYPASS      0
#define sc_x4_control_descr_modes_R_DESCR_MODE_CL49        1
#define sc_x4_control_descr_modes_R_DESCR_MODE_CL82        2

/****************************************************************************
 * Enums: sc_x4_control_r_dec_tl_mode
 */
#define sc_x4_control_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS   0
#define sc_x4_control_r_dec_tl_mode_R_DEC_TL_MODE_CL49     1
#define sc_x4_control_r_dec_tl_mode_R_DEC_TL_MODE_CL82     2

/****************************************************************************
 * Enums: sc_x4_control_r_dec_fsm_mode
 */
#define sc_x4_control_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define sc_x4_control_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49   1
#define sc_x4_control_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82   2

/****************************************************************************
 * Enums: sc_x4_control_r_deskew_mode
 */
#define sc_x4_control_r_deskew_mode_R_DESKEW_MODE_BYPASS   0
#define sc_x4_control_r_deskew_mode_R_DESKEW_MODE_20G      1
#define sc_x4_control_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define sc_x4_control_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define sc_x4_control_r_deskew_mode_R_DESKEW_MODE_100G     4
#define sc_x4_control_r_deskew_mode_R_DESKEW_MODE_CL49     5
#define sc_x4_control_r_deskew_mode_R_DESKEW_MODE_CL91     6

/****************************************************************************
 * Enums: sc_x4_control_bs_dist_modes
 */
#define sc_x4_control_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define sc_x4_control_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define sc_x4_control_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define sc_x4_control_bs_dist_modes_BS_DIST_MODE_NO_TDM    3

/****************************************************************************
 * Enums: sc_x4_control_cl49_t_type
 */
#define sc_x4_control_cl49_t_type_BAD_T_TYPE               15
#define sc_x4_control_cl49_t_type_T_TYPE_B0                11
#define sc_x4_control_cl49_t_type_T_TYPE_OB                10
#define sc_x4_control_cl49_t_type_T_TYPE_B1                9
#define sc_x4_control_cl49_t_type_T_TYPE_DB                8
#define sc_x4_control_cl49_t_type_T_TYPE_FC                7
#define sc_x4_control_cl49_t_type_T_TYPE_TB                6
#define sc_x4_control_cl49_t_type_T_TYPE_LI                5
#define sc_x4_control_cl49_t_type_T_TYPE_C                 4
#define sc_x4_control_cl49_t_type_T_TYPE_S                 3
#define sc_x4_control_cl49_t_type_T_TYPE_T                 2
#define sc_x4_control_cl49_t_type_T_TYPE_D                 1
#define sc_x4_control_cl49_t_type_T_TYPE_E                 0

/****************************************************************************
 * Enums: sc_x4_control_cl49_txsm_states
 */
#define sc_x4_control_cl49_txsm_states_TX_HIG_END          7
#define sc_x4_control_cl49_txsm_states_TX_HIG_START        6
#define sc_x4_control_cl49_txsm_states_TX_LI               5
#define sc_x4_control_cl49_txsm_states_TX_E                4
#define sc_x4_control_cl49_txsm_states_TX_T                3
#define sc_x4_control_cl49_txsm_states_TX_D                2
#define sc_x4_control_cl49_txsm_states_TX_C                1
#define sc_x4_control_cl49_txsm_states_TX_INIT             0

/****************************************************************************
 * Enums: sc_x4_control_cl49_ltxsm_states
 */
#define sc_x4_control_cl49_ltxsm_states_TX_HIG_END         128
#define sc_x4_control_cl49_ltxsm_states_TX_HIG_START       64
#define sc_x4_control_cl49_ltxsm_states_TX_LI              32
#define sc_x4_control_cl49_ltxsm_states_TX_E               16
#define sc_x4_control_cl49_ltxsm_states_TX_T               8
#define sc_x4_control_cl49_ltxsm_states_TX_D               4
#define sc_x4_control_cl49_ltxsm_states_TX_C               2
#define sc_x4_control_cl49_ltxsm_states_TX_INIT            1

/****************************************************************************
 * Enums: sc_x4_control_burst_error_mode
 */
#define sc_x4_control_burst_error_mode_BURST_ERROR_11_BITS 0
#define sc_x4_control_burst_error_mode_BURST_ERROR_16_BITS 1
#define sc_x4_control_burst_error_mode_BURST_ERROR_17_BITS 2
#define sc_x4_control_burst_error_mode_BURST_ERROR_18_BITS 3
#define sc_x4_control_burst_error_mode_BURST_ERROR_19_BITS 4
#define sc_x4_control_burst_error_mode_BURST_ERROR_20_BITS 5
#define sc_x4_control_burst_error_mode_BURST_ERROR_21_BITS 6
#define sc_x4_control_burst_error_mode_BURST_ERROR_22_BITS 7
#define sc_x4_control_burst_error_mode_BURST_ERROR_23_BITS 8
#define sc_x4_control_burst_error_mode_BURST_ERROR_24_BITS 9
#define sc_x4_control_burst_error_mode_BURST_ERROR_25_BITS 10
#define sc_x4_control_burst_error_mode_BURST_ERROR_26_BITS 11
#define sc_x4_control_burst_error_mode_BURST_ERROR_27_BITS 12
#define sc_x4_control_burst_error_mode_BURST_ERROR_28_BITS 13
#define sc_x4_control_burst_error_mode_BURST_ERROR_29_BITS 14
#define sc_x4_control_burst_error_mode_BURST_ERROR_30_BITS 15
#define sc_x4_control_burst_error_mode_BURST_ERROR_31_BITS 16
#define sc_x4_control_burst_error_mode_BURST_ERROR_32_BITS 17
#define sc_x4_control_burst_error_mode_BURST_ERROR_33_BITS 18
#define sc_x4_control_burst_error_mode_BURST_ERROR_34_BITS 19
#define sc_x4_control_burst_error_mode_BURST_ERROR_35_BITS 20
#define sc_x4_control_burst_error_mode_BURST_ERROR_36_BITS 21
#define sc_x4_control_burst_error_mode_BURST_ERROR_37_BITS 22
#define sc_x4_control_burst_error_mode_BURST_ERROR_38_BITS 23
#define sc_x4_control_burst_error_mode_BURST_ERROR_39_BITS 24
#define sc_x4_control_burst_error_mode_BURST_ERROR_40_BITS 25
#define sc_x4_control_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: sc_x4_control_bermon_state
 */
#define sc_x4_control_bermon_state_HI_BER                  4
#define sc_x4_control_bermon_state_GOOD_BER                3
#define sc_x4_control_bermon_state_BER_TEST_SH             2
#define sc_x4_control_bermon_state_START_TIMER             1
#define sc_x4_control_bermon_state_BER_MT_INIT             0

/****************************************************************************
 * Enums: sc_x4_control_rxsm_state_cl49
 */
#define sc_x4_control_rxsm_state_cl49_RX_HIG_END           128
#define sc_x4_control_rxsm_state_cl49_RX_HIG_START         64
#define sc_x4_control_rxsm_state_cl49_RX_LI                32
#define sc_x4_control_rxsm_state_cl49_RX_E                 16
#define sc_x4_control_rxsm_state_cl49_RX_T                 8
#define sc_x4_control_rxsm_state_cl49_RX_D                 4
#define sc_x4_control_rxsm_state_cl49_RX_C                 2
#define sc_x4_control_rxsm_state_cl49_RX_INIT              1

/****************************************************************************
 * Enums: sc_x4_control_r_type
 */
#define sc_x4_control_r_type_BAD_R_TYPE                    15
#define sc_x4_control_r_type_R_TYPE_B0                     11
#define sc_x4_control_r_type_R_TYPE_OB                     10
#define sc_x4_control_r_type_R_TYPE_B1                     9
#define sc_x4_control_r_type_R_TYPE_DB                     8
#define sc_x4_control_r_type_R_TYPE_FC                     7
#define sc_x4_control_r_type_R_TYPE_TB                     6
#define sc_x4_control_r_type_R_TYPE_LI                     5
#define sc_x4_control_r_type_R_TYPE_C                      4
#define sc_x4_control_r_type_R_TYPE_S                      3
#define sc_x4_control_r_type_R_TYPE_T                      2
#define sc_x4_control_r_type_R_TYPE_D                      1
#define sc_x4_control_r_type_R_TYPE_E                      0

/****************************************************************************
 * Enums: sc_x4_control_am_lock_state
 */
#define sc_x4_control_am_lock_state_INVALID_AM             512
#define sc_x4_control_am_lock_state_GOOD_AM                256
#define sc_x4_control_am_lock_state_COMP_AM                128
#define sc_x4_control_am_lock_state_TIMER_2                64
#define sc_x4_control_am_lock_state_AM_2_GOOD              32
#define sc_x4_control_am_lock_state_COMP_2ND               16
#define sc_x4_control_am_lock_state_TIMER_1                8
#define sc_x4_control_am_lock_state_FIND_1ST               4
#define sc_x4_control_am_lock_state_AM_RESET_CNT           2
#define sc_x4_control_am_lock_state_AM_LOCK_INIT           1

/****************************************************************************
 * Enums: sc_x4_control_msg_selector
 */
#define sc_x4_control_msg_selector_RESERVED                0
#define sc_x4_control_msg_selector_VALUE_802p3             1
#define sc_x4_control_msg_selector_VALUE_802p9             2
#define sc_x4_control_msg_selector_VALUE_802p5             3
#define sc_x4_control_msg_selector_VALUE_1394              4

/****************************************************************************
 * Enums: sc_x4_control_synce_enum
 */
#define sc_x4_control_synce_enum_SYNCE_NO_DIV              0
#define sc_x4_control_synce_enum_SYNCE_DIV_7               1
#define sc_x4_control_synce_enum_SYNCE_DIV_11              2

/****************************************************************************
 * Enums: sc_x4_control_synce_enum_stage0
 */
#define sc_x4_control_synce_enum_stage0_SYNCE_NO_DIV       0
#define sc_x4_control_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define sc_x4_control_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: sc_x4_control_cl91_sync_state
 */
#define sc_x4_control_cl91_sync_state_FIND_1ST             0
#define sc_x4_control_cl91_sync_state_COUNT_NEXT           1
#define sc_x4_control_cl91_sync_state_COMP_2ND             2
#define sc_x4_control_cl91_sync_state_TWO_GOOD             3

/****************************************************************************
 * Enums: sc_x4_control_cl91_algn_state
 */
#define sc_x4_control_cl91_algn_state_LOSS_OF_ALIGNMENT    0
#define sc_x4_control_cl91_algn_state_DESKEW               1
#define sc_x4_control_cl91_algn_state_DESKEW_FAIL          2
#define sc_x4_control_cl91_algn_state_ALIGN_ACQUIRED       3
#define sc_x4_control_cl91_algn_state_CW_GOOD              4
#define sc_x4_control_cl91_algn_state_CW_BAD               5
#define sc_x4_control_cl91_algn_state_THREE_BAD            6

/****************************************************************************
 * Enums: sc_x4_control_fec_sel
 */
#define sc_x4_control_fec_sel_NO_FEC                       0
#define sc_x4_control_fec_sel_FEC_CL74                     1
#define sc_x4_control_fec_sel_FEC_CL91                     2

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl36TxEEEStates_l
 */
#define sc_x4_final_config_status_cl36TxEEEStates_l_TX_REFRESH 8
#define sc_x4_final_config_status_cl36TxEEEStates_l_TX_QUIET 4
#define sc_x4_final_config_status_cl36TxEEEStates_l_TX_SLEEP 2
#define sc_x4_final_config_status_cl36TxEEEStates_l_TX_ACTIVE 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl36TxEEEStates_c
 */
#define sc_x4_final_config_status_cl36TxEEEStates_c_TX_REFRESH 3
#define sc_x4_final_config_status_cl36TxEEEStates_c_TX_QUIET 2
#define sc_x4_final_config_status_cl36TxEEEStates_c_TX_SLEEP 1
#define sc_x4_final_config_status_cl36TxEEEStates_c_TX_ACTIVE 0

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl49TxEEEStates_l
 */
#define sc_x4_final_config_status_cl49TxEEEStates_l_SCR_RESET_2 64
#define sc_x4_final_config_status_cl49TxEEEStates_l_SCR_RESET_1 32
#define sc_x4_final_config_status_cl49TxEEEStates_l_TX_WAKE 16
#define sc_x4_final_config_status_cl49TxEEEStates_l_TX_REFRESH 8
#define sc_x4_final_config_status_cl49TxEEEStates_l_TX_QUIET 4
#define sc_x4_final_config_status_cl49TxEEEStates_l_TX_SLEEP 2
#define sc_x4_final_config_status_cl49TxEEEStates_l_TX_ACTIVE 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_fec_req_enum
 */
#define sc_x4_final_config_status_fec_req_enum_FEC_not_supported 0
#define sc_x4_final_config_status_fec_req_enum_FEC_supported_but_not_requested 1
#define sc_x4_final_config_status_fec_req_enum_invalid_setting 2
#define sc_x4_final_config_status_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl73_pause_enum
 */
#define sc_x4_final_config_status_cl73_pause_enum_No_PAUSE_ability 0
#define sc_x4_final_config_status_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define sc_x4_final_config_status_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define sc_x4_final_config_status_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl49TxEEEStates_c
 */
#define sc_x4_final_config_status_cl49TxEEEStates_c_SCR_RESET_2 6
#define sc_x4_final_config_status_cl49TxEEEStates_c_SCR_RESET_1 5
#define sc_x4_final_config_status_cl49TxEEEStates_c_TX_WAKE 4
#define sc_x4_final_config_status_cl49TxEEEStates_c_TX_REFRESH 3
#define sc_x4_final_config_status_cl49TxEEEStates_c_TX_QUIET 2
#define sc_x4_final_config_status_cl49TxEEEStates_c_TX_SLEEP 1
#define sc_x4_final_config_status_cl49TxEEEStates_c_TX_ACTIVE 0

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl36RxEEEStates_l
 */
#define sc_x4_final_config_status_cl36RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x4_final_config_status_cl36RxEEEStates_l_RX_WTF 16
#define sc_x4_final_config_status_cl36RxEEEStates_l_RX_WAKE 8
#define sc_x4_final_config_status_cl36RxEEEStates_l_RX_QUIET 4
#define sc_x4_final_config_status_cl36RxEEEStates_l_RX_SLEEP 2
#define sc_x4_final_config_status_cl36RxEEEStates_l_RX_ACTIVE 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl36RxEEEStates_c
 */
#define sc_x4_final_config_status_cl36RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x4_final_config_status_cl36RxEEEStates_c_RX_WTF 4
#define sc_x4_final_config_status_cl36RxEEEStates_c_RX_WAKE 3
#define sc_x4_final_config_status_cl36RxEEEStates_c_RX_QUIET 2
#define sc_x4_final_config_status_cl36RxEEEStates_c_RX_SLEEP 1
#define sc_x4_final_config_status_cl36RxEEEStates_c_RX_ACTIVE 0

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl49RxEEEStates_l
 */
#define sc_x4_final_config_status_cl49RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x4_final_config_status_cl49RxEEEStates_l_RX_WTF 16
#define sc_x4_final_config_status_cl49RxEEEStates_l_RX_WAKE 8
#define sc_x4_final_config_status_cl49RxEEEStates_l_RX_QUIET 4
#define sc_x4_final_config_status_cl49RxEEEStates_l_RX_SLEEP 2
#define sc_x4_final_config_status_cl49RxEEEStates_l_RX_ACTIVE 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl49RxEEEStates_c
 */
#define sc_x4_final_config_status_cl49RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x4_final_config_status_cl49RxEEEStates_c_RX_WTF 4
#define sc_x4_final_config_status_cl49RxEEEStates_c_RX_WAKE 3
#define sc_x4_final_config_status_cl49RxEEEStates_c_RX_QUIET 2
#define sc_x4_final_config_status_cl49RxEEEStates_c_RX_SLEEP 1
#define sc_x4_final_config_status_cl49RxEEEStates_c_RX_ACTIVE 0

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl48TxEEEStates_l
 */
#define sc_x4_final_config_status_cl48TxEEEStates_l_TX_REFRESH 8
#define sc_x4_final_config_status_cl48TxEEEStates_l_TX_QUIET 4
#define sc_x4_final_config_status_cl48TxEEEStates_l_TX_SLEEP 2
#define sc_x4_final_config_status_cl48TxEEEStates_l_TX_ACTIVE 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl48TxEEEStates_c
 */
#define sc_x4_final_config_status_cl48TxEEEStates_c_TX_REFRESH 3
#define sc_x4_final_config_status_cl48TxEEEStates_c_TX_QUIET 2
#define sc_x4_final_config_status_cl48TxEEEStates_c_TX_SLEEP 1
#define sc_x4_final_config_status_cl48TxEEEStates_c_TX_ACTIVE 0

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl48RxEEEStates_l
 */
#define sc_x4_final_config_status_cl48RxEEEStates_l_RX_LINK_FAIL 32
#define sc_x4_final_config_status_cl48RxEEEStates_l_RX_WAKE 16
#define sc_x4_final_config_status_cl48RxEEEStates_l_RX_QUIET 8
#define sc_x4_final_config_status_cl48RxEEEStates_l_RX_DEACT 4
#define sc_x4_final_config_status_cl48RxEEEStates_l_RX_SLEEP 2
#define sc_x4_final_config_status_cl48RxEEEStates_l_RX_ACTIVE 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl48RxEEEStates_c
 */
#define sc_x4_final_config_status_cl48RxEEEStates_c_RX_LINK_FAIL 5
#define sc_x4_final_config_status_cl48RxEEEStates_c_RX_WAKE 4
#define sc_x4_final_config_status_cl48RxEEEStates_c_RX_QUIET 3
#define sc_x4_final_config_status_cl48RxEEEStates_c_RX_DEACT 2
#define sc_x4_final_config_status_cl48RxEEEStates_c_RX_SLEEP 1
#define sc_x4_final_config_status_cl48RxEEEStates_c_RX_ACTIVE 0

/****************************************************************************
 * Enums: sc_x4_final_config_status_IQP_Options
 */
#define sc_x4_final_config_status_IQP_Options_i50uA        0
#define sc_x4_final_config_status_IQP_Options_i100uA       1
#define sc_x4_final_config_status_IQP_Options_i150uA       2
#define sc_x4_final_config_status_IQP_Options_i200uA       3
#define sc_x4_final_config_status_IQP_Options_i250uA       4
#define sc_x4_final_config_status_IQP_Options_i300uA       5
#define sc_x4_final_config_status_IQP_Options_i350uA       6
#define sc_x4_final_config_status_IQP_Options_i400uA       7
#define sc_x4_final_config_status_IQP_Options_i450uA       8
#define sc_x4_final_config_status_IQP_Options_i500uA       9
#define sc_x4_final_config_status_IQP_Options_i550uA       10
#define sc_x4_final_config_status_IQP_Options_i600uA       11
#define sc_x4_final_config_status_IQP_Options_i650uA       12
#define sc_x4_final_config_status_IQP_Options_i700uA       13
#define sc_x4_final_config_status_IQP_Options_i750uA       14
#define sc_x4_final_config_status_IQP_Options_i800uA       15

/****************************************************************************
 * Enums: sc_x4_final_config_status_IDriver_Options
 */
#define sc_x4_final_config_status_IDriver_Options_v680mV   0
#define sc_x4_final_config_status_IDriver_Options_v730mV   1
#define sc_x4_final_config_status_IDriver_Options_v780mV   2
#define sc_x4_final_config_status_IDriver_Options_v830mV   3
#define sc_x4_final_config_status_IDriver_Options_v880mV   4
#define sc_x4_final_config_status_IDriver_Options_v930mV   5
#define sc_x4_final_config_status_IDriver_Options_v980mV   6
#define sc_x4_final_config_status_IDriver_Options_v1010mV  7
#define sc_x4_final_config_status_IDriver_Options_v1040mV  8
#define sc_x4_final_config_status_IDriver_Options_v1060mV  9
#define sc_x4_final_config_status_IDriver_Options_v1070mV  10
#define sc_x4_final_config_status_IDriver_Options_v1080mV  11
#define sc_x4_final_config_status_IDriver_Options_v1085mV  12
#define sc_x4_final_config_status_IDriver_Options_v1090mV  13
#define sc_x4_final_config_status_IDriver_Options_v1095mV  14
#define sc_x4_final_config_status_IDriver_Options_v1100mV  15

/****************************************************************************
 * Enums: sc_x4_final_config_status_operationModes
 */
#define sc_x4_final_config_status_operationModes_XGXS      0
#define sc_x4_final_config_status_operationModes_XGXG_nCC  1
#define sc_x4_final_config_status_operationModes_Indlane_OS8 4
#define sc_x4_final_config_status_operationModes_IndLane_OS5 5
#define sc_x4_final_config_status_operationModes_PCI       7
#define sc_x4_final_config_status_operationModes_XGXS_nLQ  8
#define sc_x4_final_config_status_operationModes_XGXS_nLQnCC 9
#define sc_x4_final_config_status_operationModes_PBypass   10
#define sc_x4_final_config_status_operationModes_PBypass_nDSK 11
#define sc_x4_final_config_status_operationModes_ComboCoreMode 12
#define sc_x4_final_config_status_operationModes_Clocks_off 15

/****************************************************************************
 * Enums: sc_x4_final_config_status_actualSpeeds
 */
#define sc_x4_final_config_status_actualSpeeds_dr_10M      0
#define sc_x4_final_config_status_actualSpeeds_dr_100M     1
#define sc_x4_final_config_status_actualSpeeds_dr_1G       2
#define sc_x4_final_config_status_actualSpeeds_dr_2p5G     3
#define sc_x4_final_config_status_actualSpeeds_dr_5G_X4    4
#define sc_x4_final_config_status_actualSpeeds_dr_6G_X4    5
#define sc_x4_final_config_status_actualSpeeds_dr_10G_HiG  6
#define sc_x4_final_config_status_actualSpeeds_dr_10G_CX4  7
#define sc_x4_final_config_status_actualSpeeds_dr_12G_HiG  8
#define sc_x4_final_config_status_actualSpeeds_dr_12p5G_X4 9
#define sc_x4_final_config_status_actualSpeeds_dr_13G_X4   10
#define sc_x4_final_config_status_actualSpeeds_dr_15G_X4   11
#define sc_x4_final_config_status_actualSpeeds_dr_16G_X4   12
#define sc_x4_final_config_status_actualSpeeds_dr_1G_KX    13
#define sc_x4_final_config_status_actualSpeeds_dr_10G_KX4  14
#define sc_x4_final_config_status_actualSpeeds_dr_10G_KR   15
#define sc_x4_final_config_status_actualSpeeds_dr_5G       16
#define sc_x4_final_config_status_actualSpeeds_dr_6p4G     17
#define sc_x4_final_config_status_actualSpeeds_dr_20G_X4   18
#define sc_x4_final_config_status_actualSpeeds_dr_21G_X4   19
#define sc_x4_final_config_status_actualSpeeds_dr_25G_X4   20
#define sc_x4_final_config_status_actualSpeeds_dr_10G_HiG_DXGXS 21
#define sc_x4_final_config_status_actualSpeeds_dr_10G_DXGXS 22
#define sc_x4_final_config_status_actualSpeeds_dr_10p5G_HiG_DXGXS 23
#define sc_x4_final_config_status_actualSpeeds_dr_10p5G_DXGXS 24
#define sc_x4_final_config_status_actualSpeeds_dr_12p773G_HiG_DXGXS 25
#define sc_x4_final_config_status_actualSpeeds_dr_12p773G_DXGXS 26
#define sc_x4_final_config_status_actualSpeeds_dr_10G_XFI  27
#define sc_x4_final_config_status_actualSpeeds_dr_40G      28
#define sc_x4_final_config_status_actualSpeeds_dr_20G_HiG_DXGXS 29
#define sc_x4_final_config_status_actualSpeeds_dr_20G_DXGXS 30
#define sc_x4_final_config_status_actualSpeeds_dr_10G_SFI  31
#define sc_x4_final_config_status_actualSpeeds_dr_31p5G    32
#define sc_x4_final_config_status_actualSpeeds_dr_32p7G    33
#define sc_x4_final_config_status_actualSpeeds_dr_20G_SCR  34
#define sc_x4_final_config_status_actualSpeeds_dr_10G_HiG_DXGXS_SCR 35
#define sc_x4_final_config_status_actualSpeeds_dr_10G_DXGXS_SCR 36
#define sc_x4_final_config_status_actualSpeeds_dr_12G_R2   37
#define sc_x4_final_config_status_actualSpeeds_dr_10G_X2   38
#define sc_x4_final_config_status_actualSpeeds_dr_40G_KR4  39
#define sc_x4_final_config_status_actualSpeeds_dr_40G_CR4  40
#define sc_x4_final_config_status_actualSpeeds_dr_100G_CR10 41
#define sc_x4_final_config_status_actualSpeeds_dr_15p75G_DXGXS 44
#define sc_x4_final_config_status_actualSpeeds_dr_20G_KR2  57
#define sc_x4_final_config_status_actualSpeeds_dr_20G_CR2  58

/****************************************************************************
 * Enums: sc_x4_final_config_status_actualSpeedsMisc1
 */
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_2500BRCM_X1 16
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_5000BRCM_X4 17
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_6000BRCM_X4 18
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_10GHiGig_X4 19
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_10GBASE_CX4 20
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_12GHiGig_X4 21
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_12p5GHiGig_X4 22
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_13GHiGig_X4 23
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_15GHiGig_X4 24
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_16GHiGig_X4 25
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_5000BRCM_X1 26
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_6363BRCM_X1 27
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_20GHiGig_X4 28
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_21GHiGig_X4 29
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define sc_x4_final_config_status_actualSpeedsMisc1_dr_10G_HiG_DXGXS 31

/****************************************************************************
 * Enums: sc_x4_final_config_status_IndLaneModes
 */
#define sc_x4_final_config_status_IndLaneModes_SWSDR_div2  0
#define sc_x4_final_config_status_IndLaneModes_SWSDR_div1  1
#define sc_x4_final_config_status_IndLaneModes_DWSDR_div2  2
#define sc_x4_final_config_status_IndLaneModes_DWSDR_div1  3

/****************************************************************************
 * Enums: sc_x4_final_config_status_prbsSelect
 */
#define sc_x4_final_config_status_prbsSelect_prbs7         0
#define sc_x4_final_config_status_prbsSelect_prbs15        1
#define sc_x4_final_config_status_prbsSelect_prbs23        2
#define sc_x4_final_config_status_prbsSelect_prbs31        3

/****************************************************************************
 * Enums: sc_x4_final_config_status_vcoDivider
 */
#define sc_x4_final_config_status_vcoDivider_div32         0
#define sc_x4_final_config_status_vcoDivider_div36         1
#define sc_x4_final_config_status_vcoDivider_div40         2
#define sc_x4_final_config_status_vcoDivider_div42         3
#define sc_x4_final_config_status_vcoDivider_div48         4
#define sc_x4_final_config_status_vcoDivider_div50         5
#define sc_x4_final_config_status_vcoDivider_div52         6
#define sc_x4_final_config_status_vcoDivider_div54         7
#define sc_x4_final_config_status_vcoDivider_div60         8
#define sc_x4_final_config_status_vcoDivider_div64         9
#define sc_x4_final_config_status_vcoDivider_div66         10
#define sc_x4_final_config_status_vcoDivider_div68         11
#define sc_x4_final_config_status_vcoDivider_div70         12
#define sc_x4_final_config_status_vcoDivider_div80         13
#define sc_x4_final_config_status_vcoDivider_div92         14
#define sc_x4_final_config_status_vcoDivider_div100        15

/****************************************************************************
 * Enums: sc_x4_final_config_status_refClkSelect
 */
#define sc_x4_final_config_status_refClkSelect_clk_25MHz   0
#define sc_x4_final_config_status_refClkSelect_clk_100MHz  1
#define sc_x4_final_config_status_refClkSelect_clk_125MHz  2
#define sc_x4_final_config_status_refClkSelect_clk_156p25MHz 3
#define sc_x4_final_config_status_refClkSelect_clk_187p5MHz 4
#define sc_x4_final_config_status_refClkSelect_clk_161p25Mhz 5
#define sc_x4_final_config_status_refClkSelect_clk_50Mhz   6
#define sc_x4_final_config_status_refClkSelect_clk_106p25Mhz 7

/****************************************************************************
 * Enums: sc_x4_final_config_status_aerMMDdevTypeSelect
 */
#define sc_x4_final_config_status_aerMMDdevTypeSelect_combo_core 0
#define sc_x4_final_config_status_aerMMDdevTypeSelect_PMA_PMD 1
#define sc_x4_final_config_status_aerMMDdevTypeSelect_PCS  3
#define sc_x4_final_config_status_aerMMDdevTypeSelect_PHY  4
#define sc_x4_final_config_status_aerMMDdevTypeSelect_DTE  5
#define sc_x4_final_config_status_aerMMDdevTypeSelect_CL73_AN 7

/****************************************************************************
 * Enums: sc_x4_final_config_status_aerMMDportSelect
 */
#define sc_x4_final_config_status_aerMMDportSelect_ln0     0
#define sc_x4_final_config_status_aerMMDportSelect_ln1     1
#define sc_x4_final_config_status_aerMMDportSelect_ln2     2
#define sc_x4_final_config_status_aerMMDportSelect_ln3     3
#define sc_x4_final_config_status_aerMMDportSelect_BCST_ln0_1_2_3 511
#define sc_x4_final_config_status_aerMMDportSelect_BCST_ln0_1 512
#define sc_x4_final_config_status_aerMMDportSelect_BCST_ln2_3 513

/****************************************************************************
 * Enums: sc_x4_final_config_status_firmwareModeSelect
 */
#define sc_x4_final_config_status_firmwareModeSelect_DEFAULT 0
#define sc_x4_final_config_status_firmwareModeSelect_SFP_OPT_LR 1
#define sc_x4_final_config_status_firmwareModeSelect_SFP_DAC 2
#define sc_x4_final_config_status_firmwareModeSelect_XLAUI 3
#define sc_x4_final_config_status_firmwareModeSelect_LONG_CH_6G 4

/****************************************************************************
 * Enums: sc_x4_final_config_status_tempIdxSelect
 */
#define sc_x4_final_config_status_tempIdxSelect_LTE__22p9C 15
#define sc_x4_final_config_status_tempIdxSelect_LTE__12p6C 14
#define sc_x4_final_config_status_tempIdxSelect_LTE__3p0C  13
#define sc_x4_final_config_status_tempIdxSelect_LTE_6p7C   12
#define sc_x4_final_config_status_tempIdxSelect_LTE_16p4C  11
#define sc_x4_final_config_status_tempIdxSelect_LTE_26p6C  10
#define sc_x4_final_config_status_tempIdxSelect_LTE_36p3C  9
#define sc_x4_final_config_status_tempIdxSelect_LTE_46p0C  8
#define sc_x4_final_config_status_tempIdxSelect_LTE_56p2C  7
#define sc_x4_final_config_status_tempIdxSelect_LTE_65p9C  6
#define sc_x4_final_config_status_tempIdxSelect_LTE_75p6C  5
#define sc_x4_final_config_status_tempIdxSelect_LTE_85p3C  4
#define sc_x4_final_config_status_tempIdxSelect_LTE_95p5C  3
#define sc_x4_final_config_status_tempIdxSelect_LTE_105p2C 2
#define sc_x4_final_config_status_tempIdxSelect_LTE_114p9C 1
#define sc_x4_final_config_status_tempIdxSelect_LTE_125p1C 0

/****************************************************************************
 * Enums: sc_x4_final_config_status_port_mode
 */
#define sc_x4_final_config_status_port_mode_QUAD_PORT      0
#define sc_x4_final_config_status_port_mode_TRI_1_PORT     1
#define sc_x4_final_config_status_port_mode_TRI_2_PORT     2
#define sc_x4_final_config_status_port_mode_DUAL_PORT      3
#define sc_x4_final_config_status_port_mode_SINGLE_PORT    4

/****************************************************************************
 * Enums: sc_x4_final_config_status_rev_letter_enum
 */
#define sc_x4_final_config_status_rev_letter_enum_REV_A    0
#define sc_x4_final_config_status_rev_letter_enum_REV_B    1
#define sc_x4_final_config_status_rev_letter_enum_REV_C    2
#define sc_x4_final_config_status_rev_letter_enum_REV_D    3

/****************************************************************************
 * Enums: sc_x4_final_config_status_rev_number_enum
 */
#define sc_x4_final_config_status_rev_number_enum_REV_0    0
#define sc_x4_final_config_status_rev_number_enum_REV_1    1
#define sc_x4_final_config_status_rev_number_enum_REV_2    2
#define sc_x4_final_config_status_rev_number_enum_REV_3    3
#define sc_x4_final_config_status_rev_number_enum_REV_4    4
#define sc_x4_final_config_status_rev_number_enum_REV_5    5
#define sc_x4_final_config_status_rev_number_enum_REV_6    6
#define sc_x4_final_config_status_rev_number_enum_REV_7    7

/****************************************************************************
 * Enums: sc_x4_final_config_status_bonding_enum
 */
#define sc_x4_final_config_status_bonding_enum_WIRE_BOND   0
#define sc_x4_final_config_status_bonding_enum_FLIP_CHIP   1

/****************************************************************************
 * Enums: sc_x4_final_config_status_tech_process
 */
#define sc_x4_final_config_status_tech_process_PROCESS_90NM 0
#define sc_x4_final_config_status_tech_process_PROCESS_65NM 1
#define sc_x4_final_config_status_tech_process_PROCESS_40NM 2
#define sc_x4_final_config_status_tech_process_PROCESS_28NM 3

/****************************************************************************
 * Enums: sc_x4_final_config_status_model_num
 */
#define sc_x4_final_config_status_model_num_SERDES_CL73    0
#define sc_x4_final_config_status_model_num_XGXS_16G       1
#define sc_x4_final_config_status_model_num_HYPERCORE      2
#define sc_x4_final_config_status_model_num_HYPERLITE      3
#define sc_x4_final_config_status_model_num_PCIE_G2_PIPE   4
#define sc_x4_final_config_status_model_num_SERDES_1p25GBd 5
#define sc_x4_final_config_status_model_num_SATA2          6
#define sc_x4_final_config_status_model_num_QSGMII         7
#define sc_x4_final_config_status_model_num_XGXS10G        8
#define sc_x4_final_config_status_model_num_WARPCORE       9
#define sc_x4_final_config_status_model_num_XFICORE        10
#define sc_x4_final_config_status_model_num_RXFI           11
#define sc_x4_final_config_status_model_num_WARPLITE       12
#define sc_x4_final_config_status_model_num_PENTACORE      13
#define sc_x4_final_config_status_model_num_ESM            14
#define sc_x4_final_config_status_model_num_QUAD_SGMII     15
#define sc_x4_final_config_status_model_num_WARPCORE_3     16
#define sc_x4_final_config_status_model_num_TSC            17
#define sc_x4_final_config_status_model_num_TSC4E          18
#define sc_x4_final_config_status_model_num_TSC12E         19
#define sc_x4_final_config_status_model_num_TSC4F          20
#define sc_x4_final_config_status_model_num_XGXS_CL73_90NM 29
#define sc_x4_final_config_status_model_num_SERDES_CL73_90NM 30
#define sc_x4_final_config_status_model_num_WARPCORE3      32
#define sc_x4_final_config_status_model_num_WARPCORE4_TSC  33
#define sc_x4_final_config_status_model_num_RXAUI          34

/****************************************************************************
 * Enums: sc_x4_final_config_status_payload
 */
#define sc_x4_final_config_status_payload_REPEAT_2_BYTES   0
#define sc_x4_final_config_status_payload_RAMPING          1
#define sc_x4_final_config_status_payload_CL48_CRPAT       2
#define sc_x4_final_config_status_payload_CL48_CJPAT       3
#define sc_x4_final_config_status_payload_CL36_LONG_CRPAT  4
#define sc_x4_final_config_status_payload_CL36_SHORT_CRPAT 5

/****************************************************************************
 * Enums: sc_x4_final_config_status_sc
 */
#define sc_x4_final_config_status_sc_S_10G_CR1             0
#define sc_x4_final_config_status_sc_S_10G_KR1             1
#define sc_x4_final_config_status_sc_S_10G_X1              2
#define sc_x4_final_config_status_sc_S_10G_HG2_CR1         4
#define sc_x4_final_config_status_sc_S_10G_HG2_KR1         5
#define sc_x4_final_config_status_sc_S_10G_HG2_X1          6
#define sc_x4_final_config_status_sc_S_20G_CR1             8
#define sc_x4_final_config_status_sc_S_20G_KR1             9
#define sc_x4_final_config_status_sc_S_20G_X1              10
#define sc_x4_final_config_status_sc_S_20G_HG2_CR1         12
#define sc_x4_final_config_status_sc_S_20G_HG2_KR1         13
#define sc_x4_final_config_status_sc_S_20G_HG2_X1          14
#define sc_x4_final_config_status_sc_S_25G_CR1             16
#define sc_x4_final_config_status_sc_S_25G_KR1             17
#define sc_x4_final_config_status_sc_S_25G_X1              18
#define sc_x4_final_config_status_sc_S_25G_HG2_CR1         20
#define sc_x4_final_config_status_sc_S_25G_HG2_KR1         21
#define sc_x4_final_config_status_sc_S_25G_HG2_X1          22
#define sc_x4_final_config_status_sc_S_20G_CR2             24
#define sc_x4_final_config_status_sc_S_20G_KR2             25
#define sc_x4_final_config_status_sc_S_20G_X2              26
#define sc_x4_final_config_status_sc_S_20G_HG2_CR2         28
#define sc_x4_final_config_status_sc_S_20G_HG2_KR2         29
#define sc_x4_final_config_status_sc_S_20G_HG2_X2          30
#define sc_x4_final_config_status_sc_S_40G_CR2             32
#define sc_x4_final_config_status_sc_S_40G_KR2             33
#define sc_x4_final_config_status_sc_S_40G_X2              34
#define sc_x4_final_config_status_sc_S_40G_HG2_CR2         36
#define sc_x4_final_config_status_sc_S_40G_HG2_KR2         37
#define sc_x4_final_config_status_sc_S_40G_HG2_X2          38
#define sc_x4_final_config_status_sc_S_40G_CR4             40
#define sc_x4_final_config_status_sc_S_40G_KR4             41
#define sc_x4_final_config_status_sc_S_40G_X4              42
#define sc_x4_final_config_status_sc_S_40G_HG2_CR4         44
#define sc_x4_final_config_status_sc_S_40G_HG2_KR4         45
#define sc_x4_final_config_status_sc_S_40G_HG2_X4          46
#define sc_x4_final_config_status_sc_S_50G_CR2             48
#define sc_x4_final_config_status_sc_S_50G_KR2             49
#define sc_x4_final_config_status_sc_S_50G_X2              50
#define sc_x4_final_config_status_sc_S_50G_HG2_CR2         52
#define sc_x4_final_config_status_sc_S_50G_HG2_KR2         53
#define sc_x4_final_config_status_sc_S_50G_HG2_X2          54
#define sc_x4_final_config_status_sc_S_50G_CR4             56
#define sc_x4_final_config_status_sc_S_50G_KR4             57
#define sc_x4_final_config_status_sc_S_50G_X4              58
#define sc_x4_final_config_status_sc_S_50G_HG2_CR4         60
#define sc_x4_final_config_status_sc_S_50G_HG2_KR4         61
#define sc_x4_final_config_status_sc_S_50G_HG2_X4          62
#define sc_x4_final_config_status_sc_S_100G_CR4            64
#define sc_x4_final_config_status_sc_S_100G_KR4            65
#define sc_x4_final_config_status_sc_S_100G_X4             66
#define sc_x4_final_config_status_sc_S_100G_HG2_CR4        68
#define sc_x4_final_config_status_sc_S_100G_HG2_KR4        69
#define sc_x4_final_config_status_sc_S_100G_HG2_X4         70
#define sc_x4_final_config_status_sc_S_CL73_20GVCO         72
#define sc_x4_final_config_status_sc_S_CL73_25GVCO         80
#define sc_x4_final_config_status_sc_S_CL36_20GVCO         88
#define sc_x4_final_config_status_sc_S_CL36_25GVCO         96

/****************************************************************************
 * Enums: sc_x4_final_config_status_t_fifo_modes
 */
#define sc_x4_final_config_status_t_fifo_modes_T_FIFO_MODE_BYPASS 0
#define sc_x4_final_config_status_t_fifo_modes_T_FIFO_MODE_40G 1
#define sc_x4_final_config_status_t_fifo_modes_T_FIFO_MODE_100G 2
#define sc_x4_final_config_status_t_fifo_modes_T_FIFO_MODE_20G 3

/****************************************************************************
 * Enums: sc_x4_final_config_status_t_enc_modes
 */
#define sc_x4_final_config_status_t_enc_modes_T_ENC_MODE_BYPASS 0
#define sc_x4_final_config_status_t_enc_modes_T_ENC_MODE_CL49 1
#define sc_x4_final_config_status_t_enc_modes_T_ENC_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x4_final_config_status_btmx_mode
 */
#define sc_x4_final_config_status_btmx_mode_BS_BTMX_MODE_1to1 0
#define sc_x4_final_config_status_btmx_mode_BS_BTMX_MODE_2to1 1
#define sc_x4_final_config_status_btmx_mode_BS_BTMX_MODE_5to1 2

/****************************************************************************
 * Enums: sc_x4_final_config_status_t_type_cl82
 */
#define sc_x4_final_config_status_t_type_cl82_T_TYPE_B1    5
#define sc_x4_final_config_status_t_type_cl82_T_TYPE_C     4
#define sc_x4_final_config_status_t_type_cl82_T_TYPE_S     3
#define sc_x4_final_config_status_t_type_cl82_T_TYPE_T     2
#define sc_x4_final_config_status_t_type_cl82_T_TYPE_D     1
#define sc_x4_final_config_status_t_type_cl82_T_TYPE_E     0

/****************************************************************************
 * Enums: sc_x4_final_config_status_txsm_state_cl82
 */
#define sc_x4_final_config_status_txsm_state_cl82_TX_HIG_END 6
#define sc_x4_final_config_status_txsm_state_cl82_TX_HIG_START 5
#define sc_x4_final_config_status_txsm_state_cl82_TX_E     4
#define sc_x4_final_config_status_txsm_state_cl82_TX_T     3
#define sc_x4_final_config_status_txsm_state_cl82_TX_D     2
#define sc_x4_final_config_status_txsm_state_cl82_TX_C     1
#define sc_x4_final_config_status_txsm_state_cl82_TX_INIT  0

/****************************************************************************
 * Enums: sc_x4_final_config_status_ltxsm_state_cl82
 */
#define sc_x4_final_config_status_ltxsm_state_cl82_TX_HIG_END 64
#define sc_x4_final_config_status_ltxsm_state_cl82_TX_HIG_START 32
#define sc_x4_final_config_status_ltxsm_state_cl82_TX_E    16
#define sc_x4_final_config_status_ltxsm_state_cl82_TX_T    8
#define sc_x4_final_config_status_ltxsm_state_cl82_TX_D    4
#define sc_x4_final_config_status_ltxsm_state_cl82_TX_C    2
#define sc_x4_final_config_status_ltxsm_state_cl82_TX_INIT 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_r_type_coded_cl82
 */
#define sc_x4_final_config_status_r_type_coded_cl82_R_TYPE_B1 32
#define sc_x4_final_config_status_r_type_coded_cl82_R_TYPE_C 16
#define sc_x4_final_config_status_r_type_coded_cl82_R_TYPE_S 8
#define sc_x4_final_config_status_r_type_coded_cl82_R_TYPE_T 4
#define sc_x4_final_config_status_r_type_coded_cl82_R_TYPE_D 2
#define sc_x4_final_config_status_r_type_coded_cl82_R_TYPE_E 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_rxsm_state_cl82
 */
#define sc_x4_final_config_status_rxsm_state_cl82_RX_HIG_END 64
#define sc_x4_final_config_status_rxsm_state_cl82_RX_HIG_START 32
#define sc_x4_final_config_status_rxsm_state_cl82_RX_E     16
#define sc_x4_final_config_status_rxsm_state_cl82_RX_T     8
#define sc_x4_final_config_status_rxsm_state_cl82_RX_D     4
#define sc_x4_final_config_status_rxsm_state_cl82_RX_C     2
#define sc_x4_final_config_status_rxsm_state_cl82_RX_INIT  1

/****************************************************************************
 * Enums: sc_x4_final_config_status_deskew_state
 */
#define sc_x4_final_config_status_deskew_state_ALIGN_ACQUIRED 2
#define sc_x4_final_config_status_deskew_state_LOSS_OF_ALIGNMENT 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_os_mode_enum
 */
#define sc_x4_final_config_status_os_mode_enum_OS_MODE_1   0
#define sc_x4_final_config_status_os_mode_enum_OS_MODE_2   1
#define sc_x4_final_config_status_os_mode_enum_OS_MODE_4   2
#define sc_x4_final_config_status_os_mode_enum_OS_MODE_16p5 8
#define sc_x4_final_config_status_os_mode_enum_OS_MODE_20p625 12

/****************************************************************************
 * Enums: sc_x4_final_config_status_scr_modes
 */
#define sc_x4_final_config_status_scr_modes_T_SCR_MODE_BYPASS 0
#define sc_x4_final_config_status_scr_modes_T_SCR_MODE_CL49 1
#define sc_x4_final_config_status_scr_modes_T_SCR_MODE_40G_2_LANE 2
#define sc_x4_final_config_status_scr_modes_T_SCR_MODE_100G 3
#define sc_x4_final_config_status_scr_modes_T_SCR_MODE_20G 4
#define sc_x4_final_config_status_scr_modes_T_SCR_MODE_40G_4_LANE 5

/****************************************************************************
 * Enums: sc_x4_final_config_status_descr_modes
 */
#define sc_x4_final_config_status_descr_modes_R_DESCR_MODE_BYPASS 0
#define sc_x4_final_config_status_descr_modes_R_DESCR_MODE_CL49 1
#define sc_x4_final_config_status_descr_modes_R_DESCR_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x4_final_config_status_r_dec_tl_mode
 */
#define sc_x4_final_config_status_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS 0
#define sc_x4_final_config_status_r_dec_tl_mode_R_DEC_TL_MODE_CL49 1
#define sc_x4_final_config_status_r_dec_tl_mode_R_DEC_TL_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x4_final_config_status_r_dec_fsm_mode
 */
#define sc_x4_final_config_status_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define sc_x4_final_config_status_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49 1
#define sc_x4_final_config_status_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82 2

/****************************************************************************
 * Enums: sc_x4_final_config_status_r_deskew_mode
 */
#define sc_x4_final_config_status_r_deskew_mode_R_DESKEW_MODE_BYPASS 0
#define sc_x4_final_config_status_r_deskew_mode_R_DESKEW_MODE_20G 1
#define sc_x4_final_config_status_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define sc_x4_final_config_status_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define sc_x4_final_config_status_r_deskew_mode_R_DESKEW_MODE_100G 4
#define sc_x4_final_config_status_r_deskew_mode_R_DESKEW_MODE_CL49 5
#define sc_x4_final_config_status_r_deskew_mode_R_DESKEW_MODE_CL91 6

/****************************************************************************
 * Enums: sc_x4_final_config_status_bs_dist_modes
 */
#define sc_x4_final_config_status_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define sc_x4_final_config_status_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define sc_x4_final_config_status_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define sc_x4_final_config_status_bs_dist_modes_BS_DIST_MODE_NO_TDM 3

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl49_t_type
 */
#define sc_x4_final_config_status_cl49_t_type_BAD_T_TYPE   15
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_B0    11
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_OB    10
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_B1    9
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_DB    8
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_FC    7
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_TB    6
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_LI    5
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_C     4
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_S     3
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_T     2
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_D     1
#define sc_x4_final_config_status_cl49_t_type_T_TYPE_E     0

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl49_txsm_states
 */
#define sc_x4_final_config_status_cl49_txsm_states_TX_HIG_END 7
#define sc_x4_final_config_status_cl49_txsm_states_TX_HIG_START 6
#define sc_x4_final_config_status_cl49_txsm_states_TX_LI   5
#define sc_x4_final_config_status_cl49_txsm_states_TX_E    4
#define sc_x4_final_config_status_cl49_txsm_states_TX_T    3
#define sc_x4_final_config_status_cl49_txsm_states_TX_D    2
#define sc_x4_final_config_status_cl49_txsm_states_TX_C    1
#define sc_x4_final_config_status_cl49_txsm_states_TX_INIT 0

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl49_ltxsm_states
 */
#define sc_x4_final_config_status_cl49_ltxsm_states_TX_HIG_END 128
#define sc_x4_final_config_status_cl49_ltxsm_states_TX_HIG_START 64
#define sc_x4_final_config_status_cl49_ltxsm_states_TX_LI  32
#define sc_x4_final_config_status_cl49_ltxsm_states_TX_E   16
#define sc_x4_final_config_status_cl49_ltxsm_states_TX_T   8
#define sc_x4_final_config_status_cl49_ltxsm_states_TX_D   4
#define sc_x4_final_config_status_cl49_ltxsm_states_TX_C   2
#define sc_x4_final_config_status_cl49_ltxsm_states_TX_INIT 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_burst_error_mode
 */
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_11_BITS 0
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_16_BITS 1
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_17_BITS 2
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_18_BITS 3
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_19_BITS 4
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_20_BITS 5
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_21_BITS 6
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_22_BITS 7
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_23_BITS 8
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_24_BITS 9
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_25_BITS 10
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_26_BITS 11
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_27_BITS 12
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_28_BITS 13
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_29_BITS 14
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_30_BITS 15
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_31_BITS 16
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_32_BITS 17
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_33_BITS 18
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_34_BITS 19
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_35_BITS 20
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_36_BITS 21
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_37_BITS 22
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_38_BITS 23
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_39_BITS 24
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_40_BITS 25
#define sc_x4_final_config_status_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: sc_x4_final_config_status_bermon_state
 */
#define sc_x4_final_config_status_bermon_state_HI_BER      4
#define sc_x4_final_config_status_bermon_state_GOOD_BER    3
#define sc_x4_final_config_status_bermon_state_BER_TEST_SH 2
#define sc_x4_final_config_status_bermon_state_START_TIMER 1
#define sc_x4_final_config_status_bermon_state_BER_MT_INIT 0

/****************************************************************************
 * Enums: sc_x4_final_config_status_rxsm_state_cl49
 */
#define sc_x4_final_config_status_rxsm_state_cl49_RX_HIG_END 128
#define sc_x4_final_config_status_rxsm_state_cl49_RX_HIG_START 64
#define sc_x4_final_config_status_rxsm_state_cl49_RX_LI    32
#define sc_x4_final_config_status_rxsm_state_cl49_RX_E     16
#define sc_x4_final_config_status_rxsm_state_cl49_RX_T     8
#define sc_x4_final_config_status_rxsm_state_cl49_RX_D     4
#define sc_x4_final_config_status_rxsm_state_cl49_RX_C     2
#define sc_x4_final_config_status_rxsm_state_cl49_RX_INIT  1

/****************************************************************************
 * Enums: sc_x4_final_config_status_r_type
 */
#define sc_x4_final_config_status_r_type_BAD_R_TYPE        15
#define sc_x4_final_config_status_r_type_R_TYPE_B0         11
#define sc_x4_final_config_status_r_type_R_TYPE_OB         10
#define sc_x4_final_config_status_r_type_R_TYPE_B1         9
#define sc_x4_final_config_status_r_type_R_TYPE_DB         8
#define sc_x4_final_config_status_r_type_R_TYPE_FC         7
#define sc_x4_final_config_status_r_type_R_TYPE_TB         6
#define sc_x4_final_config_status_r_type_R_TYPE_LI         5
#define sc_x4_final_config_status_r_type_R_TYPE_C          4
#define sc_x4_final_config_status_r_type_R_TYPE_S          3
#define sc_x4_final_config_status_r_type_R_TYPE_T          2
#define sc_x4_final_config_status_r_type_R_TYPE_D          1
#define sc_x4_final_config_status_r_type_R_TYPE_E          0

/****************************************************************************
 * Enums: sc_x4_final_config_status_am_lock_state
 */
#define sc_x4_final_config_status_am_lock_state_INVALID_AM 512
#define sc_x4_final_config_status_am_lock_state_GOOD_AM    256
#define sc_x4_final_config_status_am_lock_state_COMP_AM    128
#define sc_x4_final_config_status_am_lock_state_TIMER_2    64
#define sc_x4_final_config_status_am_lock_state_AM_2_GOOD  32
#define sc_x4_final_config_status_am_lock_state_COMP_2ND   16
#define sc_x4_final_config_status_am_lock_state_TIMER_1    8
#define sc_x4_final_config_status_am_lock_state_FIND_1ST   4
#define sc_x4_final_config_status_am_lock_state_AM_RESET_CNT 2
#define sc_x4_final_config_status_am_lock_state_AM_LOCK_INIT 1

/****************************************************************************
 * Enums: sc_x4_final_config_status_msg_selector
 */
#define sc_x4_final_config_status_msg_selector_RESERVED    0
#define sc_x4_final_config_status_msg_selector_VALUE_802p3 1
#define sc_x4_final_config_status_msg_selector_VALUE_802p9 2
#define sc_x4_final_config_status_msg_selector_VALUE_802p5 3
#define sc_x4_final_config_status_msg_selector_VALUE_1394  4

/****************************************************************************
 * Enums: sc_x4_final_config_status_synce_enum
 */
#define sc_x4_final_config_status_synce_enum_SYNCE_NO_DIV  0
#define sc_x4_final_config_status_synce_enum_SYNCE_DIV_7   1
#define sc_x4_final_config_status_synce_enum_SYNCE_DIV_11  2

/****************************************************************************
 * Enums: sc_x4_final_config_status_synce_enum_stage0
 */
#define sc_x4_final_config_status_synce_enum_stage0_SYNCE_NO_DIV 0
#define sc_x4_final_config_status_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define sc_x4_final_config_status_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl91_sync_state
 */
#define sc_x4_final_config_status_cl91_sync_state_FIND_1ST 0
#define sc_x4_final_config_status_cl91_sync_state_COUNT_NEXT 1
#define sc_x4_final_config_status_cl91_sync_state_COMP_2ND 2
#define sc_x4_final_config_status_cl91_sync_state_TWO_GOOD 3

/****************************************************************************
 * Enums: sc_x4_final_config_status_cl91_algn_state
 */
#define sc_x4_final_config_status_cl91_algn_state_LOSS_OF_ALIGNMENT 0
#define sc_x4_final_config_status_cl91_algn_state_DESKEW   1
#define sc_x4_final_config_status_cl91_algn_state_DESKEW_FAIL 2
#define sc_x4_final_config_status_cl91_algn_state_ALIGN_ACQUIRED 3
#define sc_x4_final_config_status_cl91_algn_state_CW_GOOD  4
#define sc_x4_final_config_status_cl91_algn_state_CW_BAD   5
#define sc_x4_final_config_status_cl91_algn_state_THREE_BAD 6

/****************************************************************************
 * Enums: sc_x4_final_config_status_fec_sel
 */
#define sc_x4_final_config_status_fec_sel_NO_FEC           0
#define sc_x4_final_config_status_fec_sel_FEC_CL74         1
#define sc_x4_final_config_status_fec_sel_FEC_CL91         2

/****************************************************************************
 * Enums: testshared0_cl36TxEEEStates_l
 */
#define testshared0_cl36TxEEEStates_l_TX_REFRESH           8
#define testshared0_cl36TxEEEStates_l_TX_QUIET             4
#define testshared0_cl36TxEEEStates_l_TX_SLEEP             2
#define testshared0_cl36TxEEEStates_l_TX_ACTIVE            1

/****************************************************************************
 * Enums: testshared0_cl36TxEEEStates_c
 */
#define testshared0_cl36TxEEEStates_c_TX_REFRESH           3
#define testshared0_cl36TxEEEStates_c_TX_QUIET             2
#define testshared0_cl36TxEEEStates_c_TX_SLEEP             1
#define testshared0_cl36TxEEEStates_c_TX_ACTIVE            0

/****************************************************************************
 * Enums: testshared0_cl49TxEEEStates_l
 */
#define testshared0_cl49TxEEEStates_l_SCR_RESET_2          64
#define testshared0_cl49TxEEEStates_l_SCR_RESET_1          32
#define testshared0_cl49TxEEEStates_l_TX_WAKE              16
#define testshared0_cl49TxEEEStates_l_TX_REFRESH           8
#define testshared0_cl49TxEEEStates_l_TX_QUIET             4
#define testshared0_cl49TxEEEStates_l_TX_SLEEP             2
#define testshared0_cl49TxEEEStates_l_TX_ACTIVE            1

/****************************************************************************
 * Enums: testshared0_fec_req_enum
 */
#define testshared0_fec_req_enum_FEC_not_supported         0
#define testshared0_fec_req_enum_FEC_supported_but_not_requested 1
#define testshared0_fec_req_enum_invalid_setting           2
#define testshared0_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: testshared0_cl73_pause_enum
 */
#define testshared0_cl73_pause_enum_No_PAUSE_ability       0
#define testshared0_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define testshared0_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define testshared0_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: testshared0_cl49TxEEEStates_c
 */
#define testshared0_cl49TxEEEStates_c_SCR_RESET_2          6
#define testshared0_cl49TxEEEStates_c_SCR_RESET_1          5
#define testshared0_cl49TxEEEStates_c_TX_WAKE              4
#define testshared0_cl49TxEEEStates_c_TX_REFRESH           3
#define testshared0_cl49TxEEEStates_c_TX_QUIET             2
#define testshared0_cl49TxEEEStates_c_TX_SLEEP             1
#define testshared0_cl49TxEEEStates_c_TX_ACTIVE            0

/****************************************************************************
 * Enums: testshared0_cl36RxEEEStates_l
 */
#define testshared0_cl36RxEEEStates_l_RX_LINK_FAIL         32
#define testshared0_cl36RxEEEStates_l_RX_WTF               16
#define testshared0_cl36RxEEEStates_l_RX_WAKE              8
#define testshared0_cl36RxEEEStates_l_RX_QUIET             4
#define testshared0_cl36RxEEEStates_l_RX_SLEEP             2
#define testshared0_cl36RxEEEStates_l_RX_ACTIVE            1

/****************************************************************************
 * Enums: testshared0_cl36RxEEEStates_c
 */
#define testshared0_cl36RxEEEStates_c_RX_LINK_FAIL         5
#define testshared0_cl36RxEEEStates_c_RX_WTF               4
#define testshared0_cl36RxEEEStates_c_RX_WAKE              3
#define testshared0_cl36RxEEEStates_c_RX_QUIET             2
#define testshared0_cl36RxEEEStates_c_RX_SLEEP             1
#define testshared0_cl36RxEEEStates_c_RX_ACTIVE            0

/****************************************************************************
 * Enums: testshared0_cl49RxEEEStates_l
 */
#define testshared0_cl49RxEEEStates_l_RX_LINK_FAIL         32
#define testshared0_cl49RxEEEStates_l_RX_WTF               16
#define testshared0_cl49RxEEEStates_l_RX_WAKE              8
#define testshared0_cl49RxEEEStates_l_RX_QUIET             4
#define testshared0_cl49RxEEEStates_l_RX_SLEEP             2
#define testshared0_cl49RxEEEStates_l_RX_ACTIVE            1

/****************************************************************************
 * Enums: testshared0_cl49RxEEEStates_c
 */
#define testshared0_cl49RxEEEStates_c_RX_LINK_FAIL         5
#define testshared0_cl49RxEEEStates_c_RX_WTF               4
#define testshared0_cl49RxEEEStates_c_RX_WAKE              3
#define testshared0_cl49RxEEEStates_c_RX_QUIET             2
#define testshared0_cl49RxEEEStates_c_RX_SLEEP             1
#define testshared0_cl49RxEEEStates_c_RX_ACTIVE            0

/****************************************************************************
 * Enums: testshared0_cl48TxEEEStates_l
 */
#define testshared0_cl48TxEEEStates_l_TX_REFRESH           8
#define testshared0_cl48TxEEEStates_l_TX_QUIET             4
#define testshared0_cl48TxEEEStates_l_TX_SLEEP             2
#define testshared0_cl48TxEEEStates_l_TX_ACTIVE            1

/****************************************************************************
 * Enums: testshared0_cl48TxEEEStates_c
 */
#define testshared0_cl48TxEEEStates_c_TX_REFRESH           3
#define testshared0_cl48TxEEEStates_c_TX_QUIET             2
#define testshared0_cl48TxEEEStates_c_TX_SLEEP             1
#define testshared0_cl48TxEEEStates_c_TX_ACTIVE            0

/****************************************************************************
 * Enums: testshared0_cl48RxEEEStates_l
 */
#define testshared0_cl48RxEEEStates_l_RX_LINK_FAIL         32
#define testshared0_cl48RxEEEStates_l_RX_WAKE              16
#define testshared0_cl48RxEEEStates_l_RX_QUIET             8
#define testshared0_cl48RxEEEStates_l_RX_DEACT             4
#define testshared0_cl48RxEEEStates_l_RX_SLEEP             2
#define testshared0_cl48RxEEEStates_l_RX_ACTIVE            1

/****************************************************************************
 * Enums: testshared0_cl48RxEEEStates_c
 */
#define testshared0_cl48RxEEEStates_c_RX_LINK_FAIL         5
#define testshared0_cl48RxEEEStates_c_RX_WAKE              4
#define testshared0_cl48RxEEEStates_c_RX_QUIET             3
#define testshared0_cl48RxEEEStates_c_RX_DEACT             2
#define testshared0_cl48RxEEEStates_c_RX_SLEEP             1
#define testshared0_cl48RxEEEStates_c_RX_ACTIVE            0

/****************************************************************************
 * Enums: testshared0_IQP_Options
 */
#define testshared0_IQP_Options_i50uA                      0
#define testshared0_IQP_Options_i100uA                     1
#define testshared0_IQP_Options_i150uA                     2
#define testshared0_IQP_Options_i200uA                     3
#define testshared0_IQP_Options_i250uA                     4
#define testshared0_IQP_Options_i300uA                     5
#define testshared0_IQP_Options_i350uA                     6
#define testshared0_IQP_Options_i400uA                     7
#define testshared0_IQP_Options_i450uA                     8
#define testshared0_IQP_Options_i500uA                     9
#define testshared0_IQP_Options_i550uA                     10
#define testshared0_IQP_Options_i600uA                     11
#define testshared0_IQP_Options_i650uA                     12
#define testshared0_IQP_Options_i700uA                     13
#define testshared0_IQP_Options_i750uA                     14
#define testshared0_IQP_Options_i800uA                     15

/****************************************************************************
 * Enums: testshared0_IDriver_Options
 */
#define testshared0_IDriver_Options_v680mV                 0
#define testshared0_IDriver_Options_v730mV                 1
#define testshared0_IDriver_Options_v780mV                 2
#define testshared0_IDriver_Options_v830mV                 3
#define testshared0_IDriver_Options_v880mV                 4
#define testshared0_IDriver_Options_v930mV                 5
#define testshared0_IDriver_Options_v980mV                 6
#define testshared0_IDriver_Options_v1010mV                7
#define testshared0_IDriver_Options_v1040mV                8
#define testshared0_IDriver_Options_v1060mV                9
#define testshared0_IDriver_Options_v1070mV                10
#define testshared0_IDriver_Options_v1080mV                11
#define testshared0_IDriver_Options_v1085mV                12
#define testshared0_IDriver_Options_v1090mV                13
#define testshared0_IDriver_Options_v1095mV                14
#define testshared0_IDriver_Options_v1100mV                15

/****************************************************************************
 * Enums: testshared0_operationModes
 */
#define testshared0_operationModes_XGXS                    0
#define testshared0_operationModes_XGXG_nCC                1
#define testshared0_operationModes_Indlane_OS8             4
#define testshared0_operationModes_IndLane_OS5             5
#define testshared0_operationModes_PCI                     7
#define testshared0_operationModes_XGXS_nLQ                8
#define testshared0_operationModes_XGXS_nLQnCC             9
#define testshared0_operationModes_PBypass                 10
#define testshared0_operationModes_PBypass_nDSK            11
#define testshared0_operationModes_ComboCoreMode           12
#define testshared0_operationModes_Clocks_off              15

/****************************************************************************
 * Enums: testshared0_actualSpeeds
 */
#define testshared0_actualSpeeds_dr_10M                    0
#define testshared0_actualSpeeds_dr_100M                   1
#define testshared0_actualSpeeds_dr_1G                     2
#define testshared0_actualSpeeds_dr_2p5G                   3
#define testshared0_actualSpeeds_dr_5G_X4                  4
#define testshared0_actualSpeeds_dr_6G_X4                  5
#define testshared0_actualSpeeds_dr_10G_HiG                6
#define testshared0_actualSpeeds_dr_10G_CX4                7
#define testshared0_actualSpeeds_dr_12G_HiG                8
#define testshared0_actualSpeeds_dr_12p5G_X4               9
#define testshared0_actualSpeeds_dr_13G_X4                 10
#define testshared0_actualSpeeds_dr_15G_X4                 11
#define testshared0_actualSpeeds_dr_16G_X4                 12
#define testshared0_actualSpeeds_dr_1G_KX                  13
#define testshared0_actualSpeeds_dr_10G_KX4                14
#define testshared0_actualSpeeds_dr_10G_KR                 15
#define testshared0_actualSpeeds_dr_5G                     16
#define testshared0_actualSpeeds_dr_6p4G                   17
#define testshared0_actualSpeeds_dr_20G_X4                 18
#define testshared0_actualSpeeds_dr_21G_X4                 19
#define testshared0_actualSpeeds_dr_25G_X4                 20
#define testshared0_actualSpeeds_dr_10G_HiG_DXGXS          21
#define testshared0_actualSpeeds_dr_10G_DXGXS              22
#define testshared0_actualSpeeds_dr_10p5G_HiG_DXGXS        23
#define testshared0_actualSpeeds_dr_10p5G_DXGXS            24
#define testshared0_actualSpeeds_dr_12p773G_HiG_DXGXS      25
#define testshared0_actualSpeeds_dr_12p773G_DXGXS          26
#define testshared0_actualSpeeds_dr_10G_XFI                27
#define testshared0_actualSpeeds_dr_40G                    28
#define testshared0_actualSpeeds_dr_20G_HiG_DXGXS          29
#define testshared0_actualSpeeds_dr_20G_DXGXS              30
#define testshared0_actualSpeeds_dr_10G_SFI                31
#define testshared0_actualSpeeds_dr_31p5G                  32
#define testshared0_actualSpeeds_dr_32p7G                  33
#define testshared0_actualSpeeds_dr_20G_SCR                34
#define testshared0_actualSpeeds_dr_10G_HiG_DXGXS_SCR      35
#define testshared0_actualSpeeds_dr_10G_DXGXS_SCR          36
#define testshared0_actualSpeeds_dr_12G_R2                 37
#define testshared0_actualSpeeds_dr_10G_X2                 38
#define testshared0_actualSpeeds_dr_40G_KR4                39
#define testshared0_actualSpeeds_dr_40G_CR4                40
#define testshared0_actualSpeeds_dr_100G_CR10              41
#define testshared0_actualSpeeds_dr_15p75G_DXGXS           44
#define testshared0_actualSpeeds_dr_20G_KR2                57
#define testshared0_actualSpeeds_dr_20G_CR2                58

/****************************************************************************
 * Enums: testshared0_actualSpeedsMisc1
 */
#define testshared0_actualSpeedsMisc1_dr_2500BRCM_X1       16
#define testshared0_actualSpeedsMisc1_dr_5000BRCM_X4       17
#define testshared0_actualSpeedsMisc1_dr_6000BRCM_X4       18
#define testshared0_actualSpeedsMisc1_dr_10GHiGig_X4       19
#define testshared0_actualSpeedsMisc1_dr_10GBASE_CX4       20
#define testshared0_actualSpeedsMisc1_dr_12GHiGig_X4       21
#define testshared0_actualSpeedsMisc1_dr_12p5GHiGig_X4     22
#define testshared0_actualSpeedsMisc1_dr_13GHiGig_X4       23
#define testshared0_actualSpeedsMisc1_dr_15GHiGig_X4       24
#define testshared0_actualSpeedsMisc1_dr_16GHiGig_X4       25
#define testshared0_actualSpeedsMisc1_dr_5000BRCM_X1       26
#define testshared0_actualSpeedsMisc1_dr_6363BRCM_X1       27
#define testshared0_actualSpeedsMisc1_dr_20GHiGig_X4       28
#define testshared0_actualSpeedsMisc1_dr_21GHiGig_X4       29
#define testshared0_actualSpeedsMisc1_dr_25p45GHiGig_X4    30
#define testshared0_actualSpeedsMisc1_dr_10G_HiG_DXGXS     31

/****************************************************************************
 * Enums: testshared0_IndLaneModes
 */
#define testshared0_IndLaneModes_SWSDR_div2                0
#define testshared0_IndLaneModes_SWSDR_div1                1
#define testshared0_IndLaneModes_DWSDR_div2                2
#define testshared0_IndLaneModes_DWSDR_div1                3

/****************************************************************************
 * Enums: testshared0_prbsSelect
 */
#define testshared0_prbsSelect_prbs7                       0
#define testshared0_prbsSelect_prbs15                      1
#define testshared0_prbsSelect_prbs23                      2
#define testshared0_prbsSelect_prbs31                      3

/****************************************************************************
 * Enums: testshared0_vcoDivider
 */
#define testshared0_vcoDivider_div32                       0
#define testshared0_vcoDivider_div36                       1
#define testshared0_vcoDivider_div40                       2
#define testshared0_vcoDivider_div42                       3
#define testshared0_vcoDivider_div48                       4
#define testshared0_vcoDivider_div50                       5
#define testshared0_vcoDivider_div52                       6
#define testshared0_vcoDivider_div54                       7
#define testshared0_vcoDivider_div60                       8
#define testshared0_vcoDivider_div64                       9
#define testshared0_vcoDivider_div66                       10
#define testshared0_vcoDivider_div68                       11
#define testshared0_vcoDivider_div70                       12
#define testshared0_vcoDivider_div80                       13
#define testshared0_vcoDivider_div92                       14
#define testshared0_vcoDivider_div100                      15

/****************************************************************************
 * Enums: testshared0_refClkSelect
 */
#define testshared0_refClkSelect_clk_25MHz                 0
#define testshared0_refClkSelect_clk_100MHz                1
#define testshared0_refClkSelect_clk_125MHz                2
#define testshared0_refClkSelect_clk_156p25MHz             3
#define testshared0_refClkSelect_clk_187p5MHz              4
#define testshared0_refClkSelect_clk_161p25Mhz             5
#define testshared0_refClkSelect_clk_50Mhz                 6
#define testshared0_refClkSelect_clk_106p25Mhz             7

/****************************************************************************
 * Enums: testshared0_aerMMDdevTypeSelect
 */
#define testshared0_aerMMDdevTypeSelect_combo_core         0
#define testshared0_aerMMDdevTypeSelect_PMA_PMD            1
#define testshared0_aerMMDdevTypeSelect_PCS                3
#define testshared0_aerMMDdevTypeSelect_PHY                4
#define testshared0_aerMMDdevTypeSelect_DTE                5
#define testshared0_aerMMDdevTypeSelect_CL73_AN            7

/****************************************************************************
 * Enums: testshared0_aerMMDportSelect
 */
#define testshared0_aerMMDportSelect_ln0                   0
#define testshared0_aerMMDportSelect_ln1                   1
#define testshared0_aerMMDportSelect_ln2                   2
#define testshared0_aerMMDportSelect_ln3                   3
#define testshared0_aerMMDportSelect_BCST_ln0_1_2_3        511
#define testshared0_aerMMDportSelect_BCST_ln0_1            512
#define testshared0_aerMMDportSelect_BCST_ln2_3            513

/****************************************************************************
 * Enums: testshared0_firmwareModeSelect
 */
#define testshared0_firmwareModeSelect_DEFAULT             0
#define testshared0_firmwareModeSelect_SFP_OPT_LR          1
#define testshared0_firmwareModeSelect_SFP_DAC             2
#define testshared0_firmwareModeSelect_XLAUI               3
#define testshared0_firmwareModeSelect_LONG_CH_6G          4

/****************************************************************************
 * Enums: testshared0_tempIdxSelect
 */
#define testshared0_tempIdxSelect_LTE__22p9C               15
#define testshared0_tempIdxSelect_LTE__12p6C               14
#define testshared0_tempIdxSelect_LTE__3p0C                13
#define testshared0_tempIdxSelect_LTE_6p7C                 12
#define testshared0_tempIdxSelect_LTE_16p4C                11
#define testshared0_tempIdxSelect_LTE_26p6C                10
#define testshared0_tempIdxSelect_LTE_36p3C                9
#define testshared0_tempIdxSelect_LTE_46p0C                8
#define testshared0_tempIdxSelect_LTE_56p2C                7
#define testshared0_tempIdxSelect_LTE_65p9C                6
#define testshared0_tempIdxSelect_LTE_75p6C                5
#define testshared0_tempIdxSelect_LTE_85p3C                4
#define testshared0_tempIdxSelect_LTE_95p5C                3
#define testshared0_tempIdxSelect_LTE_105p2C               2
#define testshared0_tempIdxSelect_LTE_114p9C               1
#define testshared0_tempIdxSelect_LTE_125p1C               0

/****************************************************************************
 * Enums: testshared0_port_mode
 */
#define testshared0_port_mode_QUAD_PORT                    0
#define testshared0_port_mode_TRI_1_PORT                   1
#define testshared0_port_mode_TRI_2_PORT                   2
#define testshared0_port_mode_DUAL_PORT                    3
#define testshared0_port_mode_SINGLE_PORT                  4

/****************************************************************************
 * Enums: testshared0_rev_letter_enum
 */
#define testshared0_rev_letter_enum_REV_A                  0
#define testshared0_rev_letter_enum_REV_B                  1
#define testshared0_rev_letter_enum_REV_C                  2
#define testshared0_rev_letter_enum_REV_D                  3

/****************************************************************************
 * Enums: testshared0_rev_number_enum
 */
#define testshared0_rev_number_enum_REV_0                  0
#define testshared0_rev_number_enum_REV_1                  1
#define testshared0_rev_number_enum_REV_2                  2
#define testshared0_rev_number_enum_REV_3                  3
#define testshared0_rev_number_enum_REV_4                  4
#define testshared0_rev_number_enum_REV_5                  5
#define testshared0_rev_number_enum_REV_6                  6
#define testshared0_rev_number_enum_REV_7                  7

/****************************************************************************
 * Enums: testshared0_bonding_enum
 */
#define testshared0_bonding_enum_WIRE_BOND                 0
#define testshared0_bonding_enum_FLIP_CHIP                 1

/****************************************************************************
 * Enums: testshared0_tech_process
 */
#define testshared0_tech_process_PROCESS_90NM              0
#define testshared0_tech_process_PROCESS_65NM              1
#define testshared0_tech_process_PROCESS_40NM              2
#define testshared0_tech_process_PROCESS_28NM              3

/****************************************************************************
 * Enums: testshared0_model_num
 */
#define testshared0_model_num_SERDES_CL73                  0
#define testshared0_model_num_XGXS_16G                     1
#define testshared0_model_num_HYPERCORE                    2
#define testshared0_model_num_HYPERLITE                    3
#define testshared0_model_num_PCIE_G2_PIPE                 4
#define testshared0_model_num_SERDES_1p25GBd               5
#define testshared0_model_num_SATA2                        6
#define testshared0_model_num_QSGMII                       7
#define testshared0_model_num_XGXS10G                      8
#define testshared0_model_num_WARPCORE                     9
#define testshared0_model_num_XFICORE                      10
#define testshared0_model_num_RXFI                         11
#define testshared0_model_num_WARPLITE                     12
#define testshared0_model_num_PENTACORE                    13
#define testshared0_model_num_ESM                          14
#define testshared0_model_num_QUAD_SGMII                   15
#define testshared0_model_num_WARPCORE_3                   16
#define testshared0_model_num_TSC                          17
#define testshared0_model_num_TSC4E                        18
#define testshared0_model_num_TSC12E                       19
#define testshared0_model_num_TSC4F                        20
#define testshared0_model_num_XGXS_CL73_90NM               29
#define testshared0_model_num_SERDES_CL73_90NM             30
#define testshared0_model_num_WARPCORE3                    32
#define testshared0_model_num_WARPCORE4_TSC                33
#define testshared0_model_num_RXAUI                        34

/****************************************************************************
 * Enums: testshared0_payload
 */
#define testshared0_payload_REPEAT_2_BYTES                 0
#define testshared0_payload_RAMPING                        1
#define testshared0_payload_CL48_CRPAT                     2
#define testshared0_payload_CL48_CJPAT                     3
#define testshared0_payload_CL36_LONG_CRPAT                4
#define testshared0_payload_CL36_SHORT_CRPAT               5

/****************************************************************************
 * Enums: testshared0_sc
 */
#define testshared0_sc_S_10G_CR1                           0
#define testshared0_sc_S_10G_KR1                           1
#define testshared0_sc_S_10G_X1                            2
#define testshared0_sc_S_10G_HG2_CR1                       4
#define testshared0_sc_S_10G_HG2_KR1                       5
#define testshared0_sc_S_10G_HG2_X1                        6
#define testshared0_sc_S_20G_CR1                           8
#define testshared0_sc_S_20G_KR1                           9
#define testshared0_sc_S_20G_X1                            10
#define testshared0_sc_S_20G_HG2_CR1                       12
#define testshared0_sc_S_20G_HG2_KR1                       13
#define testshared0_sc_S_20G_HG2_X1                        14
#define testshared0_sc_S_25G_CR1                           16
#define testshared0_sc_S_25G_KR1                           17
#define testshared0_sc_S_25G_X1                            18
#define testshared0_sc_S_25G_HG2_CR1                       20
#define testshared0_sc_S_25G_HG2_KR1                       21
#define testshared0_sc_S_25G_HG2_X1                        22
#define testshared0_sc_S_20G_CR2                           24
#define testshared0_sc_S_20G_KR2                           25
#define testshared0_sc_S_20G_X2                            26
#define testshared0_sc_S_20G_HG2_CR2                       28
#define testshared0_sc_S_20G_HG2_KR2                       29
#define testshared0_sc_S_20G_HG2_X2                        30
#define testshared0_sc_S_40G_CR2                           32
#define testshared0_sc_S_40G_KR2                           33
#define testshared0_sc_S_40G_X2                            34
#define testshared0_sc_S_40G_HG2_CR2                       36
#define testshared0_sc_S_40G_HG2_KR2                       37
#define testshared0_sc_S_40G_HG2_X2                        38
#define testshared0_sc_S_40G_CR4                           40
#define testshared0_sc_S_40G_KR4                           41
#define testshared0_sc_S_40G_X4                            42
#define testshared0_sc_S_40G_HG2_CR4                       44
#define testshared0_sc_S_40G_HG2_KR4                       45
#define testshared0_sc_S_40G_HG2_X4                        46
#define testshared0_sc_S_50G_CR2                           48
#define testshared0_sc_S_50G_KR2                           49
#define testshared0_sc_S_50G_X2                            50
#define testshared0_sc_S_50G_HG2_CR2                       52
#define testshared0_sc_S_50G_HG2_KR2                       53
#define testshared0_sc_S_50G_HG2_X2                        54
#define testshared0_sc_S_50G_CR4                           56
#define testshared0_sc_S_50G_KR4                           57
#define testshared0_sc_S_50G_X4                            58
#define testshared0_sc_S_50G_HG2_CR4                       60
#define testshared0_sc_S_50G_HG2_KR4                       61
#define testshared0_sc_S_50G_HG2_X4                        62
#define testshared0_sc_S_100G_CR4                          64
#define testshared0_sc_S_100G_KR4                          65
#define testshared0_sc_S_100G_X4                           66
#define testshared0_sc_S_100G_HG2_CR4                      68
#define testshared0_sc_S_100G_HG2_KR4                      69
#define testshared0_sc_S_100G_HG2_X4                       70
#define testshared0_sc_S_CL73_20GVCO                       72
#define testshared0_sc_S_CL73_25GVCO                       80
#define testshared0_sc_S_CL36_20GVCO                       88
#define testshared0_sc_S_CL36_25GVCO                       96

/****************************************************************************
 * Enums: testshared0_t_fifo_modes
 */
#define testshared0_t_fifo_modes_T_FIFO_MODE_BYPASS        0
#define testshared0_t_fifo_modes_T_FIFO_MODE_40G           1
#define testshared0_t_fifo_modes_T_FIFO_MODE_100G          2
#define testshared0_t_fifo_modes_T_FIFO_MODE_20G           3

/****************************************************************************
 * Enums: testshared0_t_enc_modes
 */
#define testshared0_t_enc_modes_T_ENC_MODE_BYPASS          0
#define testshared0_t_enc_modes_T_ENC_MODE_CL49            1
#define testshared0_t_enc_modes_T_ENC_MODE_CL82            2

/****************************************************************************
 * Enums: testshared0_btmx_mode
 */
#define testshared0_btmx_mode_BS_BTMX_MODE_1to1            0
#define testshared0_btmx_mode_BS_BTMX_MODE_2to1            1
#define testshared0_btmx_mode_BS_BTMX_MODE_5to1            2

/****************************************************************************
 * Enums: testshared0_t_type_cl82
 */
#define testshared0_t_type_cl82_T_TYPE_B1                  5
#define testshared0_t_type_cl82_T_TYPE_C                   4
#define testshared0_t_type_cl82_T_TYPE_S                   3
#define testshared0_t_type_cl82_T_TYPE_T                   2
#define testshared0_t_type_cl82_T_TYPE_D                   1
#define testshared0_t_type_cl82_T_TYPE_E                   0

/****************************************************************************
 * Enums: testshared0_txsm_state_cl82
 */
#define testshared0_txsm_state_cl82_TX_HIG_END             6
#define testshared0_txsm_state_cl82_TX_HIG_START           5
#define testshared0_txsm_state_cl82_TX_E                   4
#define testshared0_txsm_state_cl82_TX_T                   3
#define testshared0_txsm_state_cl82_TX_D                   2
#define testshared0_txsm_state_cl82_TX_C                   1
#define testshared0_txsm_state_cl82_TX_INIT                0

/****************************************************************************
 * Enums: testshared0_ltxsm_state_cl82
 */
#define testshared0_ltxsm_state_cl82_TX_HIG_END            64
#define testshared0_ltxsm_state_cl82_TX_HIG_START          32
#define testshared0_ltxsm_state_cl82_TX_E                  16
#define testshared0_ltxsm_state_cl82_TX_T                  8
#define testshared0_ltxsm_state_cl82_TX_D                  4
#define testshared0_ltxsm_state_cl82_TX_C                  2
#define testshared0_ltxsm_state_cl82_TX_INIT               1

/****************************************************************************
 * Enums: testshared0_r_type_coded_cl82
 */
#define testshared0_r_type_coded_cl82_R_TYPE_B1            32
#define testshared0_r_type_coded_cl82_R_TYPE_C             16
#define testshared0_r_type_coded_cl82_R_TYPE_S             8
#define testshared0_r_type_coded_cl82_R_TYPE_T             4
#define testshared0_r_type_coded_cl82_R_TYPE_D             2
#define testshared0_r_type_coded_cl82_R_TYPE_E             1

/****************************************************************************
 * Enums: testshared0_rxsm_state_cl82
 */
#define testshared0_rxsm_state_cl82_RX_HIG_END             64
#define testshared0_rxsm_state_cl82_RX_HIG_START           32
#define testshared0_rxsm_state_cl82_RX_E                   16
#define testshared0_rxsm_state_cl82_RX_T                   8
#define testshared0_rxsm_state_cl82_RX_D                   4
#define testshared0_rxsm_state_cl82_RX_C                   2
#define testshared0_rxsm_state_cl82_RX_INIT                1

/****************************************************************************
 * Enums: testshared0_deskew_state
 */
#define testshared0_deskew_state_ALIGN_ACQUIRED            2
#define testshared0_deskew_state_LOSS_OF_ALIGNMENT         1

/****************************************************************************
 * Enums: testshared0_os_mode_enum
 */
#define testshared0_os_mode_enum_OS_MODE_1                 0
#define testshared0_os_mode_enum_OS_MODE_2                 1
#define testshared0_os_mode_enum_OS_MODE_4                 2
#define testshared0_os_mode_enum_OS_MODE_16p5              8
#define testshared0_os_mode_enum_OS_MODE_20p625            12

/****************************************************************************
 * Enums: testshared0_scr_modes
 */
#define testshared0_scr_modes_T_SCR_MODE_BYPASS            0
#define testshared0_scr_modes_T_SCR_MODE_CL49              1
#define testshared0_scr_modes_T_SCR_MODE_40G_2_LANE        2
#define testshared0_scr_modes_T_SCR_MODE_100G              3
#define testshared0_scr_modes_T_SCR_MODE_20G               4
#define testshared0_scr_modes_T_SCR_MODE_40G_4_LANE        5

/****************************************************************************
 * Enums: testshared0_descr_modes
 */
#define testshared0_descr_modes_R_DESCR_MODE_BYPASS        0
#define testshared0_descr_modes_R_DESCR_MODE_CL49          1
#define testshared0_descr_modes_R_DESCR_MODE_CL82          2

/****************************************************************************
 * Enums: testshared0_r_dec_tl_mode
 */
#define testshared0_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS     0
#define testshared0_r_dec_tl_mode_R_DEC_TL_MODE_CL49       1
#define testshared0_r_dec_tl_mode_R_DEC_TL_MODE_CL82       2

/****************************************************************************
 * Enums: testshared0_r_dec_fsm_mode
 */
#define testshared0_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS   0
#define testshared0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49     1
#define testshared0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82     2

/****************************************************************************
 * Enums: testshared0_r_deskew_mode
 */
#define testshared0_r_deskew_mode_R_DESKEW_MODE_BYPASS     0
#define testshared0_r_deskew_mode_R_DESKEW_MODE_20G        1
#define testshared0_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define testshared0_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define testshared0_r_deskew_mode_R_DESKEW_MODE_100G       4
#define testshared0_r_deskew_mode_R_DESKEW_MODE_CL49       5
#define testshared0_r_deskew_mode_R_DESKEW_MODE_CL91       6

/****************************************************************************
 * Enums: testshared0_bs_dist_modes
 */
#define testshared0_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM  0
#define testshared0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define testshared0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define testshared0_bs_dist_modes_BS_DIST_MODE_NO_TDM      3

/****************************************************************************
 * Enums: testshared0_cl49_t_type
 */
#define testshared0_cl49_t_type_BAD_T_TYPE                 15
#define testshared0_cl49_t_type_T_TYPE_B0                  11
#define testshared0_cl49_t_type_T_TYPE_OB                  10
#define testshared0_cl49_t_type_T_TYPE_B1                  9
#define testshared0_cl49_t_type_T_TYPE_DB                  8
#define testshared0_cl49_t_type_T_TYPE_FC                  7
#define testshared0_cl49_t_type_T_TYPE_TB                  6
#define testshared0_cl49_t_type_T_TYPE_LI                  5
#define testshared0_cl49_t_type_T_TYPE_C                   4
#define testshared0_cl49_t_type_T_TYPE_S                   3
#define testshared0_cl49_t_type_T_TYPE_T                   2
#define testshared0_cl49_t_type_T_TYPE_D                   1
#define testshared0_cl49_t_type_T_TYPE_E                   0

/****************************************************************************
 * Enums: testshared0_cl49_txsm_states
 */
#define testshared0_cl49_txsm_states_TX_HIG_END            7
#define testshared0_cl49_txsm_states_TX_HIG_START          6
#define testshared0_cl49_txsm_states_TX_LI                 5
#define testshared0_cl49_txsm_states_TX_E                  4
#define testshared0_cl49_txsm_states_TX_T                  3
#define testshared0_cl49_txsm_states_TX_D                  2
#define testshared0_cl49_txsm_states_TX_C                  1
#define testshared0_cl49_txsm_states_TX_INIT               0

/****************************************************************************
 * Enums: testshared0_cl49_ltxsm_states
 */
#define testshared0_cl49_ltxsm_states_TX_HIG_END           128
#define testshared0_cl49_ltxsm_states_TX_HIG_START         64
#define testshared0_cl49_ltxsm_states_TX_LI                32
#define testshared0_cl49_ltxsm_states_TX_E                 16
#define testshared0_cl49_ltxsm_states_TX_T                 8
#define testshared0_cl49_ltxsm_states_TX_D                 4
#define testshared0_cl49_ltxsm_states_TX_C                 2
#define testshared0_cl49_ltxsm_states_TX_INIT              1

/****************************************************************************
 * Enums: testshared0_burst_error_mode
 */
#define testshared0_burst_error_mode_BURST_ERROR_11_BITS   0
#define testshared0_burst_error_mode_BURST_ERROR_16_BITS   1
#define testshared0_burst_error_mode_BURST_ERROR_17_BITS   2
#define testshared0_burst_error_mode_BURST_ERROR_18_BITS   3
#define testshared0_burst_error_mode_BURST_ERROR_19_BITS   4
#define testshared0_burst_error_mode_BURST_ERROR_20_BITS   5
#define testshared0_burst_error_mode_BURST_ERROR_21_BITS   6
#define testshared0_burst_error_mode_BURST_ERROR_22_BITS   7
#define testshared0_burst_error_mode_BURST_ERROR_23_BITS   8
#define testshared0_burst_error_mode_BURST_ERROR_24_BITS   9
#define testshared0_burst_error_mode_BURST_ERROR_25_BITS   10
#define testshared0_burst_error_mode_BURST_ERROR_26_BITS   11
#define testshared0_burst_error_mode_BURST_ERROR_27_BITS   12
#define testshared0_burst_error_mode_BURST_ERROR_28_BITS   13
#define testshared0_burst_error_mode_BURST_ERROR_29_BITS   14
#define testshared0_burst_error_mode_BURST_ERROR_30_BITS   15
#define testshared0_burst_error_mode_BURST_ERROR_31_BITS   16
#define testshared0_burst_error_mode_BURST_ERROR_32_BITS   17
#define testshared0_burst_error_mode_BURST_ERROR_33_BITS   18
#define testshared0_burst_error_mode_BURST_ERROR_34_BITS   19
#define testshared0_burst_error_mode_BURST_ERROR_35_BITS   20
#define testshared0_burst_error_mode_BURST_ERROR_36_BITS   21
#define testshared0_burst_error_mode_BURST_ERROR_37_BITS   22
#define testshared0_burst_error_mode_BURST_ERROR_38_BITS   23
#define testshared0_burst_error_mode_BURST_ERROR_39_BITS   24
#define testshared0_burst_error_mode_BURST_ERROR_40_BITS   25
#define testshared0_burst_error_mode_BURST_ERROR_41_BITS   26

/****************************************************************************
 * Enums: testshared0_bermon_state
 */
#define testshared0_bermon_state_HI_BER                    4
#define testshared0_bermon_state_GOOD_BER                  3
#define testshared0_bermon_state_BER_TEST_SH               2
#define testshared0_bermon_state_START_TIMER               1
#define testshared0_bermon_state_BER_MT_INIT               0

/****************************************************************************
 * Enums: testshared0_rxsm_state_cl49
 */
#define testshared0_rxsm_state_cl49_RX_HIG_END             128
#define testshared0_rxsm_state_cl49_RX_HIG_START           64
#define testshared0_rxsm_state_cl49_RX_LI                  32
#define testshared0_rxsm_state_cl49_RX_E                   16
#define testshared0_rxsm_state_cl49_RX_T                   8
#define testshared0_rxsm_state_cl49_RX_D                   4
#define testshared0_rxsm_state_cl49_RX_C                   2
#define testshared0_rxsm_state_cl49_RX_INIT                1

/****************************************************************************
 * Enums: testshared0_r_type
 */
#define testshared0_r_type_BAD_R_TYPE                      15
#define testshared0_r_type_R_TYPE_B0                       11
#define testshared0_r_type_R_TYPE_OB                       10
#define testshared0_r_type_R_TYPE_B1                       9
#define testshared0_r_type_R_TYPE_DB                       8
#define testshared0_r_type_R_TYPE_FC                       7
#define testshared0_r_type_R_TYPE_TB                       6
#define testshared0_r_type_R_TYPE_LI                       5
#define testshared0_r_type_R_TYPE_C                        4
#define testshared0_r_type_R_TYPE_S                        3
#define testshared0_r_type_R_TYPE_T                        2
#define testshared0_r_type_R_TYPE_D                        1
#define testshared0_r_type_R_TYPE_E                        0

/****************************************************************************
 * Enums: testshared0_am_lock_state
 */
#define testshared0_am_lock_state_INVALID_AM               512
#define testshared0_am_lock_state_GOOD_AM                  256
#define testshared0_am_lock_state_COMP_AM                  128
#define testshared0_am_lock_state_TIMER_2                  64
#define testshared0_am_lock_state_AM_2_GOOD                32
#define testshared0_am_lock_state_COMP_2ND                 16
#define testshared0_am_lock_state_TIMER_1                  8
#define testshared0_am_lock_state_FIND_1ST                 4
#define testshared0_am_lock_state_AM_RESET_CNT             2
#define testshared0_am_lock_state_AM_LOCK_INIT             1

/****************************************************************************
 * Enums: testshared0_msg_selector
 */
#define testshared0_msg_selector_RESERVED                  0
#define testshared0_msg_selector_VALUE_802p3               1
#define testshared0_msg_selector_VALUE_802p9               2
#define testshared0_msg_selector_VALUE_802p5               3
#define testshared0_msg_selector_VALUE_1394                4

/****************************************************************************
 * Enums: testshared0_synce_enum
 */
#define testshared0_synce_enum_SYNCE_NO_DIV                0
#define testshared0_synce_enum_SYNCE_DIV_7                 1
#define testshared0_synce_enum_SYNCE_DIV_11                2

/****************************************************************************
 * Enums: testshared0_synce_enum_stage0
 */
#define testshared0_synce_enum_stage0_SYNCE_NO_DIV         0
#define testshared0_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define testshared0_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: testshared0_cl91_sync_state
 */
#define testshared0_cl91_sync_state_FIND_1ST               0
#define testshared0_cl91_sync_state_COUNT_NEXT             1
#define testshared0_cl91_sync_state_COMP_2ND               2
#define testshared0_cl91_sync_state_TWO_GOOD               3

/****************************************************************************
 * Enums: testshared0_cl91_algn_state
 */
#define testshared0_cl91_algn_state_LOSS_OF_ALIGNMENT      0
#define testshared0_cl91_algn_state_DESKEW                 1
#define testshared0_cl91_algn_state_DESKEW_FAIL            2
#define testshared0_cl91_algn_state_ALIGN_ACQUIRED         3
#define testshared0_cl91_algn_state_CW_GOOD                4
#define testshared0_cl91_algn_state_CW_BAD                 5
#define testshared0_cl91_algn_state_THREE_BAD              6

/****************************************************************************
 * Enums: testshared0_fec_sel
 */
#define testshared0_fec_sel_NO_FEC                         0
#define testshared0_fec_sel_FEC_CL74                       1
#define testshared0_fec_sel_FEC_CL91                       2

/****************************************************************************
 * Enums: tx_x2_control0_cl36TxEEEStates_l
 */
#define tx_x2_control0_cl36TxEEEStates_l_TX_REFRESH        8
#define tx_x2_control0_cl36TxEEEStates_l_TX_QUIET          4
#define tx_x2_control0_cl36TxEEEStates_l_TX_SLEEP          2
#define tx_x2_control0_cl36TxEEEStates_l_TX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x2_control0_cl36TxEEEStates_c
 */
#define tx_x2_control0_cl36TxEEEStates_c_TX_REFRESH        3
#define tx_x2_control0_cl36TxEEEStates_c_TX_QUIET          2
#define tx_x2_control0_cl36TxEEEStates_c_TX_SLEEP          1
#define tx_x2_control0_cl36TxEEEStates_c_TX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x2_control0_cl49TxEEEStates_l
 */
#define tx_x2_control0_cl49TxEEEStates_l_SCR_RESET_2       64
#define tx_x2_control0_cl49TxEEEStates_l_SCR_RESET_1       32
#define tx_x2_control0_cl49TxEEEStates_l_TX_WAKE           16
#define tx_x2_control0_cl49TxEEEStates_l_TX_REFRESH        8
#define tx_x2_control0_cl49TxEEEStates_l_TX_QUIET          4
#define tx_x2_control0_cl49TxEEEStates_l_TX_SLEEP          2
#define tx_x2_control0_cl49TxEEEStates_l_TX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x2_control0_fec_req_enum
 */
#define tx_x2_control0_fec_req_enum_FEC_not_supported      0
#define tx_x2_control0_fec_req_enum_FEC_supported_but_not_requested 1
#define tx_x2_control0_fec_req_enum_invalid_setting        2
#define tx_x2_control0_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: tx_x2_control0_cl73_pause_enum
 */
#define tx_x2_control0_cl73_pause_enum_No_PAUSE_ability    0
#define tx_x2_control0_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define tx_x2_control0_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define tx_x2_control0_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: tx_x2_control0_cl49TxEEEStates_c
 */
#define tx_x2_control0_cl49TxEEEStates_c_SCR_RESET_2       6
#define tx_x2_control0_cl49TxEEEStates_c_SCR_RESET_1       5
#define tx_x2_control0_cl49TxEEEStates_c_TX_WAKE           4
#define tx_x2_control0_cl49TxEEEStates_c_TX_REFRESH        3
#define tx_x2_control0_cl49TxEEEStates_c_TX_QUIET          2
#define tx_x2_control0_cl49TxEEEStates_c_TX_SLEEP          1
#define tx_x2_control0_cl49TxEEEStates_c_TX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x2_control0_cl36RxEEEStates_l
 */
#define tx_x2_control0_cl36RxEEEStates_l_RX_LINK_FAIL      32
#define tx_x2_control0_cl36RxEEEStates_l_RX_WTF            16
#define tx_x2_control0_cl36RxEEEStates_l_RX_WAKE           8
#define tx_x2_control0_cl36RxEEEStates_l_RX_QUIET          4
#define tx_x2_control0_cl36RxEEEStates_l_RX_SLEEP          2
#define tx_x2_control0_cl36RxEEEStates_l_RX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x2_control0_cl36RxEEEStates_c
 */
#define tx_x2_control0_cl36RxEEEStates_c_RX_LINK_FAIL      5
#define tx_x2_control0_cl36RxEEEStates_c_RX_WTF            4
#define tx_x2_control0_cl36RxEEEStates_c_RX_WAKE           3
#define tx_x2_control0_cl36RxEEEStates_c_RX_QUIET          2
#define tx_x2_control0_cl36RxEEEStates_c_RX_SLEEP          1
#define tx_x2_control0_cl36RxEEEStates_c_RX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x2_control0_cl49RxEEEStates_l
 */
#define tx_x2_control0_cl49RxEEEStates_l_RX_LINK_FAIL      32
#define tx_x2_control0_cl49RxEEEStates_l_RX_WTF            16
#define tx_x2_control0_cl49RxEEEStates_l_RX_WAKE           8
#define tx_x2_control0_cl49RxEEEStates_l_RX_QUIET          4
#define tx_x2_control0_cl49RxEEEStates_l_RX_SLEEP          2
#define tx_x2_control0_cl49RxEEEStates_l_RX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x2_control0_cl49RxEEEStates_c
 */
#define tx_x2_control0_cl49RxEEEStates_c_RX_LINK_FAIL      5
#define tx_x2_control0_cl49RxEEEStates_c_RX_WTF            4
#define tx_x2_control0_cl49RxEEEStates_c_RX_WAKE           3
#define tx_x2_control0_cl49RxEEEStates_c_RX_QUIET          2
#define tx_x2_control0_cl49RxEEEStates_c_RX_SLEEP          1
#define tx_x2_control0_cl49RxEEEStates_c_RX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x2_control0_cl48TxEEEStates_l
 */
#define tx_x2_control0_cl48TxEEEStates_l_TX_REFRESH        8
#define tx_x2_control0_cl48TxEEEStates_l_TX_QUIET          4
#define tx_x2_control0_cl48TxEEEStates_l_TX_SLEEP          2
#define tx_x2_control0_cl48TxEEEStates_l_TX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x2_control0_cl48TxEEEStates_c
 */
#define tx_x2_control0_cl48TxEEEStates_c_TX_REFRESH        3
#define tx_x2_control0_cl48TxEEEStates_c_TX_QUIET          2
#define tx_x2_control0_cl48TxEEEStates_c_TX_SLEEP          1
#define tx_x2_control0_cl48TxEEEStates_c_TX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x2_control0_cl48RxEEEStates_l
 */
#define tx_x2_control0_cl48RxEEEStates_l_RX_LINK_FAIL      32
#define tx_x2_control0_cl48RxEEEStates_l_RX_WAKE           16
#define tx_x2_control0_cl48RxEEEStates_l_RX_QUIET          8
#define tx_x2_control0_cl48RxEEEStates_l_RX_DEACT          4
#define tx_x2_control0_cl48RxEEEStates_l_RX_SLEEP          2
#define tx_x2_control0_cl48RxEEEStates_l_RX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x2_control0_cl48RxEEEStates_c
 */
#define tx_x2_control0_cl48RxEEEStates_c_RX_LINK_FAIL      5
#define tx_x2_control0_cl48RxEEEStates_c_RX_WAKE           4
#define tx_x2_control0_cl48RxEEEStates_c_RX_QUIET          3
#define tx_x2_control0_cl48RxEEEStates_c_RX_DEACT          2
#define tx_x2_control0_cl48RxEEEStates_c_RX_SLEEP          1
#define tx_x2_control0_cl48RxEEEStates_c_RX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x2_control0_IQP_Options
 */
#define tx_x2_control0_IQP_Options_i50uA                   0
#define tx_x2_control0_IQP_Options_i100uA                  1
#define tx_x2_control0_IQP_Options_i150uA                  2
#define tx_x2_control0_IQP_Options_i200uA                  3
#define tx_x2_control0_IQP_Options_i250uA                  4
#define tx_x2_control0_IQP_Options_i300uA                  5
#define tx_x2_control0_IQP_Options_i350uA                  6
#define tx_x2_control0_IQP_Options_i400uA                  7
#define tx_x2_control0_IQP_Options_i450uA                  8
#define tx_x2_control0_IQP_Options_i500uA                  9
#define tx_x2_control0_IQP_Options_i550uA                  10
#define tx_x2_control0_IQP_Options_i600uA                  11
#define tx_x2_control0_IQP_Options_i650uA                  12
#define tx_x2_control0_IQP_Options_i700uA                  13
#define tx_x2_control0_IQP_Options_i750uA                  14
#define tx_x2_control0_IQP_Options_i800uA                  15

/****************************************************************************
 * Enums: tx_x2_control0_IDriver_Options
 */
#define tx_x2_control0_IDriver_Options_v680mV              0
#define tx_x2_control0_IDriver_Options_v730mV              1
#define tx_x2_control0_IDriver_Options_v780mV              2
#define tx_x2_control0_IDriver_Options_v830mV              3
#define tx_x2_control0_IDriver_Options_v880mV              4
#define tx_x2_control0_IDriver_Options_v930mV              5
#define tx_x2_control0_IDriver_Options_v980mV              6
#define tx_x2_control0_IDriver_Options_v1010mV             7
#define tx_x2_control0_IDriver_Options_v1040mV             8
#define tx_x2_control0_IDriver_Options_v1060mV             9
#define tx_x2_control0_IDriver_Options_v1070mV             10
#define tx_x2_control0_IDriver_Options_v1080mV             11
#define tx_x2_control0_IDriver_Options_v1085mV             12
#define tx_x2_control0_IDriver_Options_v1090mV             13
#define tx_x2_control0_IDriver_Options_v1095mV             14
#define tx_x2_control0_IDriver_Options_v1100mV             15

/****************************************************************************
 * Enums: tx_x2_control0_operationModes
 */
#define tx_x2_control0_operationModes_XGXS                 0
#define tx_x2_control0_operationModes_XGXG_nCC             1
#define tx_x2_control0_operationModes_Indlane_OS8          4
#define tx_x2_control0_operationModes_IndLane_OS5          5
#define tx_x2_control0_operationModes_PCI                  7
#define tx_x2_control0_operationModes_XGXS_nLQ             8
#define tx_x2_control0_operationModes_XGXS_nLQnCC          9
#define tx_x2_control0_operationModes_PBypass              10
#define tx_x2_control0_operationModes_PBypass_nDSK         11
#define tx_x2_control0_operationModes_ComboCoreMode        12
#define tx_x2_control0_operationModes_Clocks_off           15

/****************************************************************************
 * Enums: tx_x2_control0_actualSpeeds
 */
#define tx_x2_control0_actualSpeeds_dr_10M                 0
#define tx_x2_control0_actualSpeeds_dr_100M                1
#define tx_x2_control0_actualSpeeds_dr_1G                  2
#define tx_x2_control0_actualSpeeds_dr_2p5G                3
#define tx_x2_control0_actualSpeeds_dr_5G_X4               4
#define tx_x2_control0_actualSpeeds_dr_6G_X4               5
#define tx_x2_control0_actualSpeeds_dr_10G_HiG             6
#define tx_x2_control0_actualSpeeds_dr_10G_CX4             7
#define tx_x2_control0_actualSpeeds_dr_12G_HiG             8
#define tx_x2_control0_actualSpeeds_dr_12p5G_X4            9
#define tx_x2_control0_actualSpeeds_dr_13G_X4              10
#define tx_x2_control0_actualSpeeds_dr_15G_X4              11
#define tx_x2_control0_actualSpeeds_dr_16G_X4              12
#define tx_x2_control0_actualSpeeds_dr_1G_KX               13
#define tx_x2_control0_actualSpeeds_dr_10G_KX4             14
#define tx_x2_control0_actualSpeeds_dr_10G_KR              15
#define tx_x2_control0_actualSpeeds_dr_5G                  16
#define tx_x2_control0_actualSpeeds_dr_6p4G                17
#define tx_x2_control0_actualSpeeds_dr_20G_X4              18
#define tx_x2_control0_actualSpeeds_dr_21G_X4              19
#define tx_x2_control0_actualSpeeds_dr_25G_X4              20
#define tx_x2_control0_actualSpeeds_dr_10G_HiG_DXGXS       21
#define tx_x2_control0_actualSpeeds_dr_10G_DXGXS           22
#define tx_x2_control0_actualSpeeds_dr_10p5G_HiG_DXGXS     23
#define tx_x2_control0_actualSpeeds_dr_10p5G_DXGXS         24
#define tx_x2_control0_actualSpeeds_dr_12p773G_HiG_DXGXS   25
#define tx_x2_control0_actualSpeeds_dr_12p773G_DXGXS       26
#define tx_x2_control0_actualSpeeds_dr_10G_XFI             27
#define tx_x2_control0_actualSpeeds_dr_40G                 28
#define tx_x2_control0_actualSpeeds_dr_20G_HiG_DXGXS       29
#define tx_x2_control0_actualSpeeds_dr_20G_DXGXS           30
#define tx_x2_control0_actualSpeeds_dr_10G_SFI             31
#define tx_x2_control0_actualSpeeds_dr_31p5G               32
#define tx_x2_control0_actualSpeeds_dr_32p7G               33
#define tx_x2_control0_actualSpeeds_dr_20G_SCR             34
#define tx_x2_control0_actualSpeeds_dr_10G_HiG_DXGXS_SCR   35
#define tx_x2_control0_actualSpeeds_dr_10G_DXGXS_SCR       36
#define tx_x2_control0_actualSpeeds_dr_12G_R2              37
#define tx_x2_control0_actualSpeeds_dr_10G_X2              38
#define tx_x2_control0_actualSpeeds_dr_40G_KR4             39
#define tx_x2_control0_actualSpeeds_dr_40G_CR4             40
#define tx_x2_control0_actualSpeeds_dr_100G_CR10           41
#define tx_x2_control0_actualSpeeds_dr_15p75G_DXGXS        44
#define tx_x2_control0_actualSpeeds_dr_20G_KR2             57
#define tx_x2_control0_actualSpeeds_dr_20G_CR2             58

/****************************************************************************
 * Enums: tx_x2_control0_actualSpeedsMisc1
 */
#define tx_x2_control0_actualSpeedsMisc1_dr_2500BRCM_X1    16
#define tx_x2_control0_actualSpeedsMisc1_dr_5000BRCM_X4    17
#define tx_x2_control0_actualSpeedsMisc1_dr_6000BRCM_X4    18
#define tx_x2_control0_actualSpeedsMisc1_dr_10GHiGig_X4    19
#define tx_x2_control0_actualSpeedsMisc1_dr_10GBASE_CX4    20
#define tx_x2_control0_actualSpeedsMisc1_dr_12GHiGig_X4    21
#define tx_x2_control0_actualSpeedsMisc1_dr_12p5GHiGig_X4  22
#define tx_x2_control0_actualSpeedsMisc1_dr_13GHiGig_X4    23
#define tx_x2_control0_actualSpeedsMisc1_dr_15GHiGig_X4    24
#define tx_x2_control0_actualSpeedsMisc1_dr_16GHiGig_X4    25
#define tx_x2_control0_actualSpeedsMisc1_dr_5000BRCM_X1    26
#define tx_x2_control0_actualSpeedsMisc1_dr_6363BRCM_X1    27
#define tx_x2_control0_actualSpeedsMisc1_dr_20GHiGig_X4    28
#define tx_x2_control0_actualSpeedsMisc1_dr_21GHiGig_X4    29
#define tx_x2_control0_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define tx_x2_control0_actualSpeedsMisc1_dr_10G_HiG_DXGXS  31

/****************************************************************************
 * Enums: tx_x2_control0_IndLaneModes
 */
#define tx_x2_control0_IndLaneModes_SWSDR_div2             0
#define tx_x2_control0_IndLaneModes_SWSDR_div1             1
#define tx_x2_control0_IndLaneModes_DWSDR_div2             2
#define tx_x2_control0_IndLaneModes_DWSDR_div1             3

/****************************************************************************
 * Enums: tx_x2_control0_prbsSelect
 */
#define tx_x2_control0_prbsSelect_prbs7                    0
#define tx_x2_control0_prbsSelect_prbs15                   1
#define tx_x2_control0_prbsSelect_prbs23                   2
#define tx_x2_control0_prbsSelect_prbs31                   3

/****************************************************************************
 * Enums: tx_x2_control0_vcoDivider
 */
#define tx_x2_control0_vcoDivider_div32                    0
#define tx_x2_control0_vcoDivider_div36                    1
#define tx_x2_control0_vcoDivider_div40                    2
#define tx_x2_control0_vcoDivider_div42                    3
#define tx_x2_control0_vcoDivider_div48                    4
#define tx_x2_control0_vcoDivider_div50                    5
#define tx_x2_control0_vcoDivider_div52                    6
#define tx_x2_control0_vcoDivider_div54                    7
#define tx_x2_control0_vcoDivider_div60                    8
#define tx_x2_control0_vcoDivider_div64                    9
#define tx_x2_control0_vcoDivider_div66                    10
#define tx_x2_control0_vcoDivider_div68                    11
#define tx_x2_control0_vcoDivider_div70                    12
#define tx_x2_control0_vcoDivider_div80                    13
#define tx_x2_control0_vcoDivider_div92                    14
#define tx_x2_control0_vcoDivider_div100                   15

/****************************************************************************
 * Enums: tx_x2_control0_refClkSelect
 */
#define tx_x2_control0_refClkSelect_clk_25MHz              0
#define tx_x2_control0_refClkSelect_clk_100MHz             1
#define tx_x2_control0_refClkSelect_clk_125MHz             2
#define tx_x2_control0_refClkSelect_clk_156p25MHz          3
#define tx_x2_control0_refClkSelect_clk_187p5MHz           4
#define tx_x2_control0_refClkSelect_clk_161p25Mhz          5
#define tx_x2_control0_refClkSelect_clk_50Mhz              6
#define tx_x2_control0_refClkSelect_clk_106p25Mhz          7

/****************************************************************************
 * Enums: tx_x2_control0_aerMMDdevTypeSelect
 */
#define tx_x2_control0_aerMMDdevTypeSelect_combo_core      0
#define tx_x2_control0_aerMMDdevTypeSelect_PMA_PMD         1
#define tx_x2_control0_aerMMDdevTypeSelect_PCS             3
#define tx_x2_control0_aerMMDdevTypeSelect_PHY             4
#define tx_x2_control0_aerMMDdevTypeSelect_DTE             5
#define tx_x2_control0_aerMMDdevTypeSelect_CL73_AN         7

/****************************************************************************
 * Enums: tx_x2_control0_aerMMDportSelect
 */
#define tx_x2_control0_aerMMDportSelect_ln0                0
#define tx_x2_control0_aerMMDportSelect_ln1                1
#define tx_x2_control0_aerMMDportSelect_ln2                2
#define tx_x2_control0_aerMMDportSelect_ln3                3
#define tx_x2_control0_aerMMDportSelect_BCST_ln0_1_2_3     511
#define tx_x2_control0_aerMMDportSelect_BCST_ln0_1         512
#define tx_x2_control0_aerMMDportSelect_BCST_ln2_3         513

/****************************************************************************
 * Enums: tx_x2_control0_firmwareModeSelect
 */
#define tx_x2_control0_firmwareModeSelect_DEFAULT          0
#define tx_x2_control0_firmwareModeSelect_SFP_OPT_LR       1
#define tx_x2_control0_firmwareModeSelect_SFP_DAC          2
#define tx_x2_control0_firmwareModeSelect_XLAUI            3
#define tx_x2_control0_firmwareModeSelect_LONG_CH_6G       4

/****************************************************************************
 * Enums: tx_x2_control0_tempIdxSelect
 */
#define tx_x2_control0_tempIdxSelect_LTE__22p9C            15
#define tx_x2_control0_tempIdxSelect_LTE__12p6C            14
#define tx_x2_control0_tempIdxSelect_LTE__3p0C             13
#define tx_x2_control0_tempIdxSelect_LTE_6p7C              12
#define tx_x2_control0_tempIdxSelect_LTE_16p4C             11
#define tx_x2_control0_tempIdxSelect_LTE_26p6C             10
#define tx_x2_control0_tempIdxSelect_LTE_36p3C             9
#define tx_x2_control0_tempIdxSelect_LTE_46p0C             8
#define tx_x2_control0_tempIdxSelect_LTE_56p2C             7
#define tx_x2_control0_tempIdxSelect_LTE_65p9C             6
#define tx_x2_control0_tempIdxSelect_LTE_75p6C             5
#define tx_x2_control0_tempIdxSelect_LTE_85p3C             4
#define tx_x2_control0_tempIdxSelect_LTE_95p5C             3
#define tx_x2_control0_tempIdxSelect_LTE_105p2C            2
#define tx_x2_control0_tempIdxSelect_LTE_114p9C            1
#define tx_x2_control0_tempIdxSelect_LTE_125p1C            0

/****************************************************************************
 * Enums: tx_x2_control0_port_mode
 */
#define tx_x2_control0_port_mode_QUAD_PORT                 0
#define tx_x2_control0_port_mode_TRI_1_PORT                1
#define tx_x2_control0_port_mode_TRI_2_PORT                2
#define tx_x2_control0_port_mode_DUAL_PORT                 3
#define tx_x2_control0_port_mode_SINGLE_PORT               4

/****************************************************************************
 * Enums: tx_x2_control0_rev_letter_enum
 */
#define tx_x2_control0_rev_letter_enum_REV_A               0
#define tx_x2_control0_rev_letter_enum_REV_B               1
#define tx_x2_control0_rev_letter_enum_REV_C               2
#define tx_x2_control0_rev_letter_enum_REV_D               3

/****************************************************************************
 * Enums: tx_x2_control0_rev_number_enum
 */
#define tx_x2_control0_rev_number_enum_REV_0               0
#define tx_x2_control0_rev_number_enum_REV_1               1
#define tx_x2_control0_rev_number_enum_REV_2               2
#define tx_x2_control0_rev_number_enum_REV_3               3
#define tx_x2_control0_rev_number_enum_REV_4               4
#define tx_x2_control0_rev_number_enum_REV_5               5
#define tx_x2_control0_rev_number_enum_REV_6               6
#define tx_x2_control0_rev_number_enum_REV_7               7

/****************************************************************************
 * Enums: tx_x2_control0_bonding_enum
 */
#define tx_x2_control0_bonding_enum_WIRE_BOND              0
#define tx_x2_control0_bonding_enum_FLIP_CHIP              1

/****************************************************************************
 * Enums: tx_x2_control0_tech_process
 */
#define tx_x2_control0_tech_process_PROCESS_90NM           0
#define tx_x2_control0_tech_process_PROCESS_65NM           1
#define tx_x2_control0_tech_process_PROCESS_40NM           2
#define tx_x2_control0_tech_process_PROCESS_28NM           3

/****************************************************************************
 * Enums: tx_x2_control0_model_num
 */
#define tx_x2_control0_model_num_SERDES_CL73               0
#define tx_x2_control0_model_num_XGXS_16G                  1
#define tx_x2_control0_model_num_HYPERCORE                 2
#define tx_x2_control0_model_num_HYPERLITE                 3
#define tx_x2_control0_model_num_PCIE_G2_PIPE              4
#define tx_x2_control0_model_num_SERDES_1p25GBd            5
#define tx_x2_control0_model_num_SATA2                     6
#define tx_x2_control0_model_num_QSGMII                    7
#define tx_x2_control0_model_num_XGXS10G                   8
#define tx_x2_control0_model_num_WARPCORE                  9
#define tx_x2_control0_model_num_XFICORE                   10
#define tx_x2_control0_model_num_RXFI                      11
#define tx_x2_control0_model_num_WARPLITE                  12
#define tx_x2_control0_model_num_PENTACORE                 13
#define tx_x2_control0_model_num_ESM                       14
#define tx_x2_control0_model_num_QUAD_SGMII                15
#define tx_x2_control0_model_num_WARPCORE_3                16
#define tx_x2_control0_model_num_TSC                       17
#define tx_x2_control0_model_num_TSC4E                     18
#define tx_x2_control0_model_num_TSC12E                    19
#define tx_x2_control0_model_num_TSC4F                     20
#define tx_x2_control0_model_num_XGXS_CL73_90NM            29
#define tx_x2_control0_model_num_SERDES_CL73_90NM          30
#define tx_x2_control0_model_num_WARPCORE3                 32
#define tx_x2_control0_model_num_WARPCORE4_TSC             33
#define tx_x2_control0_model_num_RXAUI                     34

/****************************************************************************
 * Enums: tx_x2_control0_payload
 */
#define tx_x2_control0_payload_REPEAT_2_BYTES              0
#define tx_x2_control0_payload_RAMPING                     1
#define tx_x2_control0_payload_CL48_CRPAT                  2
#define tx_x2_control0_payload_CL48_CJPAT                  3
#define tx_x2_control0_payload_CL36_LONG_CRPAT             4
#define tx_x2_control0_payload_CL36_SHORT_CRPAT            5

/****************************************************************************
 * Enums: tx_x2_control0_sc
 */
#define tx_x2_control0_sc_S_10G_CR1                        0
#define tx_x2_control0_sc_S_10G_KR1                        1
#define tx_x2_control0_sc_S_10G_X1                         2
#define tx_x2_control0_sc_S_10G_HG2_CR1                    4
#define tx_x2_control0_sc_S_10G_HG2_KR1                    5
#define tx_x2_control0_sc_S_10G_HG2_X1                     6
#define tx_x2_control0_sc_S_20G_CR1                        8
#define tx_x2_control0_sc_S_20G_KR1                        9
#define tx_x2_control0_sc_S_20G_X1                         10
#define tx_x2_control0_sc_S_20G_HG2_CR1                    12
#define tx_x2_control0_sc_S_20G_HG2_KR1                    13
#define tx_x2_control0_sc_S_20G_HG2_X1                     14
#define tx_x2_control0_sc_S_25G_CR1                        16
#define tx_x2_control0_sc_S_25G_KR1                        17
#define tx_x2_control0_sc_S_25G_X1                         18
#define tx_x2_control0_sc_S_25G_HG2_CR1                    20
#define tx_x2_control0_sc_S_25G_HG2_KR1                    21
#define tx_x2_control0_sc_S_25G_HG2_X1                     22
#define tx_x2_control0_sc_S_20G_CR2                        24
#define tx_x2_control0_sc_S_20G_KR2                        25
#define tx_x2_control0_sc_S_20G_X2                         26
#define tx_x2_control0_sc_S_20G_HG2_CR2                    28
#define tx_x2_control0_sc_S_20G_HG2_KR2                    29
#define tx_x2_control0_sc_S_20G_HG2_X2                     30
#define tx_x2_control0_sc_S_40G_CR2                        32
#define tx_x2_control0_sc_S_40G_KR2                        33
#define tx_x2_control0_sc_S_40G_X2                         34
#define tx_x2_control0_sc_S_40G_HG2_CR2                    36
#define tx_x2_control0_sc_S_40G_HG2_KR2                    37
#define tx_x2_control0_sc_S_40G_HG2_X2                     38
#define tx_x2_control0_sc_S_40G_CR4                        40
#define tx_x2_control0_sc_S_40G_KR4                        41
#define tx_x2_control0_sc_S_40G_X4                         42
#define tx_x2_control0_sc_S_40G_HG2_CR4                    44
#define tx_x2_control0_sc_S_40G_HG2_KR4                    45
#define tx_x2_control0_sc_S_40G_HG2_X4                     46
#define tx_x2_control0_sc_S_50G_CR2                        48
#define tx_x2_control0_sc_S_50G_KR2                        49
#define tx_x2_control0_sc_S_50G_X2                         50
#define tx_x2_control0_sc_S_50G_HG2_CR2                    52
#define tx_x2_control0_sc_S_50G_HG2_KR2                    53
#define tx_x2_control0_sc_S_50G_HG2_X2                     54
#define tx_x2_control0_sc_S_50G_CR4                        56
#define tx_x2_control0_sc_S_50G_KR4                        57
#define tx_x2_control0_sc_S_50G_X4                         58
#define tx_x2_control0_sc_S_50G_HG2_CR4                    60
#define tx_x2_control0_sc_S_50G_HG2_KR4                    61
#define tx_x2_control0_sc_S_50G_HG2_X4                     62
#define tx_x2_control0_sc_S_100G_CR4                       64
#define tx_x2_control0_sc_S_100G_KR4                       65
#define tx_x2_control0_sc_S_100G_X4                        66
#define tx_x2_control0_sc_S_100G_HG2_CR4                   68
#define tx_x2_control0_sc_S_100G_HG2_KR4                   69
#define tx_x2_control0_sc_S_100G_HG2_X4                    70
#define tx_x2_control0_sc_S_CL73_20GVCO                    72
#define tx_x2_control0_sc_S_CL73_25GVCO                    80
#define tx_x2_control0_sc_S_CL36_20GVCO                    88
#define tx_x2_control0_sc_S_CL36_25GVCO                    96

/****************************************************************************
 * Enums: tx_x2_control0_t_fifo_modes
 */
#define tx_x2_control0_t_fifo_modes_T_FIFO_MODE_BYPASS     0
#define tx_x2_control0_t_fifo_modes_T_FIFO_MODE_40G        1
#define tx_x2_control0_t_fifo_modes_T_FIFO_MODE_100G       2
#define tx_x2_control0_t_fifo_modes_T_FIFO_MODE_20G        3

/****************************************************************************
 * Enums: tx_x2_control0_t_enc_modes
 */
#define tx_x2_control0_t_enc_modes_T_ENC_MODE_BYPASS       0
#define tx_x2_control0_t_enc_modes_T_ENC_MODE_CL49         1
#define tx_x2_control0_t_enc_modes_T_ENC_MODE_CL82         2

/****************************************************************************
 * Enums: tx_x2_control0_btmx_mode
 */
#define tx_x2_control0_btmx_mode_BS_BTMX_MODE_1to1         0
#define tx_x2_control0_btmx_mode_BS_BTMX_MODE_2to1         1
#define tx_x2_control0_btmx_mode_BS_BTMX_MODE_5to1         2

/****************************************************************************
 * Enums: tx_x2_control0_t_type_cl82
 */
#define tx_x2_control0_t_type_cl82_T_TYPE_B1               5
#define tx_x2_control0_t_type_cl82_T_TYPE_C                4
#define tx_x2_control0_t_type_cl82_T_TYPE_S                3
#define tx_x2_control0_t_type_cl82_T_TYPE_T                2
#define tx_x2_control0_t_type_cl82_T_TYPE_D                1
#define tx_x2_control0_t_type_cl82_T_TYPE_E                0

/****************************************************************************
 * Enums: tx_x2_control0_txsm_state_cl82
 */
#define tx_x2_control0_txsm_state_cl82_TX_HIG_END          6
#define tx_x2_control0_txsm_state_cl82_TX_HIG_START        5
#define tx_x2_control0_txsm_state_cl82_TX_E                4
#define tx_x2_control0_txsm_state_cl82_TX_T                3
#define tx_x2_control0_txsm_state_cl82_TX_D                2
#define tx_x2_control0_txsm_state_cl82_TX_C                1
#define tx_x2_control0_txsm_state_cl82_TX_INIT             0

/****************************************************************************
 * Enums: tx_x2_control0_ltxsm_state_cl82
 */
#define tx_x2_control0_ltxsm_state_cl82_TX_HIG_END         64
#define tx_x2_control0_ltxsm_state_cl82_TX_HIG_START       32
#define tx_x2_control0_ltxsm_state_cl82_TX_E               16
#define tx_x2_control0_ltxsm_state_cl82_TX_T               8
#define tx_x2_control0_ltxsm_state_cl82_TX_D               4
#define tx_x2_control0_ltxsm_state_cl82_TX_C               2
#define tx_x2_control0_ltxsm_state_cl82_TX_INIT            1

/****************************************************************************
 * Enums: tx_x2_control0_r_type_coded_cl82
 */
#define tx_x2_control0_r_type_coded_cl82_R_TYPE_B1         32
#define tx_x2_control0_r_type_coded_cl82_R_TYPE_C          16
#define tx_x2_control0_r_type_coded_cl82_R_TYPE_S          8
#define tx_x2_control0_r_type_coded_cl82_R_TYPE_T          4
#define tx_x2_control0_r_type_coded_cl82_R_TYPE_D          2
#define tx_x2_control0_r_type_coded_cl82_R_TYPE_E          1

/****************************************************************************
 * Enums: tx_x2_control0_rxsm_state_cl82
 */
#define tx_x2_control0_rxsm_state_cl82_RX_HIG_END          64
#define tx_x2_control0_rxsm_state_cl82_RX_HIG_START        32
#define tx_x2_control0_rxsm_state_cl82_RX_E                16
#define tx_x2_control0_rxsm_state_cl82_RX_T                8
#define tx_x2_control0_rxsm_state_cl82_RX_D                4
#define tx_x2_control0_rxsm_state_cl82_RX_C                2
#define tx_x2_control0_rxsm_state_cl82_RX_INIT             1

/****************************************************************************
 * Enums: tx_x2_control0_deskew_state
 */
#define tx_x2_control0_deskew_state_ALIGN_ACQUIRED         2
#define tx_x2_control0_deskew_state_LOSS_OF_ALIGNMENT      1

/****************************************************************************
 * Enums: tx_x2_control0_os_mode_enum
 */
#define tx_x2_control0_os_mode_enum_OS_MODE_1              0
#define tx_x2_control0_os_mode_enum_OS_MODE_2              1
#define tx_x2_control0_os_mode_enum_OS_MODE_4              2
#define tx_x2_control0_os_mode_enum_OS_MODE_16p5           8
#define tx_x2_control0_os_mode_enum_OS_MODE_20p625         12

/****************************************************************************
 * Enums: tx_x2_control0_scr_modes
 */
#define tx_x2_control0_scr_modes_T_SCR_MODE_BYPASS         0
#define tx_x2_control0_scr_modes_T_SCR_MODE_CL49           1
#define tx_x2_control0_scr_modes_T_SCR_MODE_40G_2_LANE     2
#define tx_x2_control0_scr_modes_T_SCR_MODE_100G           3
#define tx_x2_control0_scr_modes_T_SCR_MODE_20G            4
#define tx_x2_control0_scr_modes_T_SCR_MODE_40G_4_LANE     5

/****************************************************************************
 * Enums: tx_x2_control0_descr_modes
 */
#define tx_x2_control0_descr_modes_R_DESCR_MODE_BYPASS     0
#define tx_x2_control0_descr_modes_R_DESCR_MODE_CL49       1
#define tx_x2_control0_descr_modes_R_DESCR_MODE_CL82       2

/****************************************************************************
 * Enums: tx_x2_control0_r_dec_tl_mode
 */
#define tx_x2_control0_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS  0
#define tx_x2_control0_r_dec_tl_mode_R_DEC_TL_MODE_CL49    1
#define tx_x2_control0_r_dec_tl_mode_R_DEC_TL_MODE_CL82    2

/****************************************************************************
 * Enums: tx_x2_control0_r_dec_fsm_mode
 */
#define tx_x2_control0_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define tx_x2_control0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49  1
#define tx_x2_control0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82  2

/****************************************************************************
 * Enums: tx_x2_control0_r_deskew_mode
 */
#define tx_x2_control0_r_deskew_mode_R_DESKEW_MODE_BYPASS  0
#define tx_x2_control0_r_deskew_mode_R_DESKEW_MODE_20G     1
#define tx_x2_control0_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define tx_x2_control0_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define tx_x2_control0_r_deskew_mode_R_DESKEW_MODE_100G    4
#define tx_x2_control0_r_deskew_mode_R_DESKEW_MODE_CL49    5
#define tx_x2_control0_r_deskew_mode_R_DESKEW_MODE_CL91    6

/****************************************************************************
 * Enums: tx_x2_control0_bs_dist_modes
 */
#define tx_x2_control0_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define tx_x2_control0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define tx_x2_control0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define tx_x2_control0_bs_dist_modes_BS_DIST_MODE_NO_TDM   3

/****************************************************************************
 * Enums: tx_x2_control0_cl49_t_type
 */
#define tx_x2_control0_cl49_t_type_BAD_T_TYPE              15
#define tx_x2_control0_cl49_t_type_T_TYPE_B0               11
#define tx_x2_control0_cl49_t_type_T_TYPE_OB               10
#define tx_x2_control0_cl49_t_type_T_TYPE_B1               9
#define tx_x2_control0_cl49_t_type_T_TYPE_DB               8
#define tx_x2_control0_cl49_t_type_T_TYPE_FC               7
#define tx_x2_control0_cl49_t_type_T_TYPE_TB               6
#define tx_x2_control0_cl49_t_type_T_TYPE_LI               5
#define tx_x2_control0_cl49_t_type_T_TYPE_C                4
#define tx_x2_control0_cl49_t_type_T_TYPE_S                3
#define tx_x2_control0_cl49_t_type_T_TYPE_T                2
#define tx_x2_control0_cl49_t_type_T_TYPE_D                1
#define tx_x2_control0_cl49_t_type_T_TYPE_E                0

/****************************************************************************
 * Enums: tx_x2_control0_cl49_txsm_states
 */
#define tx_x2_control0_cl49_txsm_states_TX_HIG_END         7
#define tx_x2_control0_cl49_txsm_states_TX_HIG_START       6
#define tx_x2_control0_cl49_txsm_states_TX_LI              5
#define tx_x2_control0_cl49_txsm_states_TX_E               4
#define tx_x2_control0_cl49_txsm_states_TX_T               3
#define tx_x2_control0_cl49_txsm_states_TX_D               2
#define tx_x2_control0_cl49_txsm_states_TX_C               1
#define tx_x2_control0_cl49_txsm_states_TX_INIT            0

/****************************************************************************
 * Enums: tx_x2_control0_cl49_ltxsm_states
 */
#define tx_x2_control0_cl49_ltxsm_states_TX_HIG_END        128
#define tx_x2_control0_cl49_ltxsm_states_TX_HIG_START      64
#define tx_x2_control0_cl49_ltxsm_states_TX_LI             32
#define tx_x2_control0_cl49_ltxsm_states_TX_E              16
#define tx_x2_control0_cl49_ltxsm_states_TX_T              8
#define tx_x2_control0_cl49_ltxsm_states_TX_D              4
#define tx_x2_control0_cl49_ltxsm_states_TX_C              2
#define tx_x2_control0_cl49_ltxsm_states_TX_INIT           1

/****************************************************************************
 * Enums: tx_x2_control0_burst_error_mode
 */
#define tx_x2_control0_burst_error_mode_BURST_ERROR_11_BITS 0
#define tx_x2_control0_burst_error_mode_BURST_ERROR_16_BITS 1
#define tx_x2_control0_burst_error_mode_BURST_ERROR_17_BITS 2
#define tx_x2_control0_burst_error_mode_BURST_ERROR_18_BITS 3
#define tx_x2_control0_burst_error_mode_BURST_ERROR_19_BITS 4
#define tx_x2_control0_burst_error_mode_BURST_ERROR_20_BITS 5
#define tx_x2_control0_burst_error_mode_BURST_ERROR_21_BITS 6
#define tx_x2_control0_burst_error_mode_BURST_ERROR_22_BITS 7
#define tx_x2_control0_burst_error_mode_BURST_ERROR_23_BITS 8
#define tx_x2_control0_burst_error_mode_BURST_ERROR_24_BITS 9
#define tx_x2_control0_burst_error_mode_BURST_ERROR_25_BITS 10
#define tx_x2_control0_burst_error_mode_BURST_ERROR_26_BITS 11
#define tx_x2_control0_burst_error_mode_BURST_ERROR_27_BITS 12
#define tx_x2_control0_burst_error_mode_BURST_ERROR_28_BITS 13
#define tx_x2_control0_burst_error_mode_BURST_ERROR_29_BITS 14
#define tx_x2_control0_burst_error_mode_BURST_ERROR_30_BITS 15
#define tx_x2_control0_burst_error_mode_BURST_ERROR_31_BITS 16
#define tx_x2_control0_burst_error_mode_BURST_ERROR_32_BITS 17
#define tx_x2_control0_burst_error_mode_BURST_ERROR_33_BITS 18
#define tx_x2_control0_burst_error_mode_BURST_ERROR_34_BITS 19
#define tx_x2_control0_burst_error_mode_BURST_ERROR_35_BITS 20
#define tx_x2_control0_burst_error_mode_BURST_ERROR_36_BITS 21
#define tx_x2_control0_burst_error_mode_BURST_ERROR_37_BITS 22
#define tx_x2_control0_burst_error_mode_BURST_ERROR_38_BITS 23
#define tx_x2_control0_burst_error_mode_BURST_ERROR_39_BITS 24
#define tx_x2_control0_burst_error_mode_BURST_ERROR_40_BITS 25
#define tx_x2_control0_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: tx_x2_control0_bermon_state
 */
#define tx_x2_control0_bermon_state_HI_BER                 4
#define tx_x2_control0_bermon_state_GOOD_BER               3
#define tx_x2_control0_bermon_state_BER_TEST_SH            2
#define tx_x2_control0_bermon_state_START_TIMER            1
#define tx_x2_control0_bermon_state_BER_MT_INIT            0

/****************************************************************************
 * Enums: tx_x2_control0_rxsm_state_cl49
 */
#define tx_x2_control0_rxsm_state_cl49_RX_HIG_END          128
#define tx_x2_control0_rxsm_state_cl49_RX_HIG_START        64
#define tx_x2_control0_rxsm_state_cl49_RX_LI               32
#define tx_x2_control0_rxsm_state_cl49_RX_E                16
#define tx_x2_control0_rxsm_state_cl49_RX_T                8
#define tx_x2_control0_rxsm_state_cl49_RX_D                4
#define tx_x2_control0_rxsm_state_cl49_RX_C                2
#define tx_x2_control0_rxsm_state_cl49_RX_INIT             1

/****************************************************************************
 * Enums: tx_x2_control0_r_type
 */
#define tx_x2_control0_r_type_BAD_R_TYPE                   15
#define tx_x2_control0_r_type_R_TYPE_B0                    11
#define tx_x2_control0_r_type_R_TYPE_OB                    10
#define tx_x2_control0_r_type_R_TYPE_B1                    9
#define tx_x2_control0_r_type_R_TYPE_DB                    8
#define tx_x2_control0_r_type_R_TYPE_FC                    7
#define tx_x2_control0_r_type_R_TYPE_TB                    6
#define tx_x2_control0_r_type_R_TYPE_LI                    5
#define tx_x2_control0_r_type_R_TYPE_C                     4
#define tx_x2_control0_r_type_R_TYPE_S                     3
#define tx_x2_control0_r_type_R_TYPE_T                     2
#define tx_x2_control0_r_type_R_TYPE_D                     1
#define tx_x2_control0_r_type_R_TYPE_E                     0

/****************************************************************************
 * Enums: tx_x2_control0_am_lock_state
 */
#define tx_x2_control0_am_lock_state_INVALID_AM            512
#define tx_x2_control0_am_lock_state_GOOD_AM               256
#define tx_x2_control0_am_lock_state_COMP_AM               128
#define tx_x2_control0_am_lock_state_TIMER_2               64
#define tx_x2_control0_am_lock_state_AM_2_GOOD             32
#define tx_x2_control0_am_lock_state_COMP_2ND              16
#define tx_x2_control0_am_lock_state_TIMER_1               8
#define tx_x2_control0_am_lock_state_FIND_1ST              4
#define tx_x2_control0_am_lock_state_AM_RESET_CNT          2
#define tx_x2_control0_am_lock_state_AM_LOCK_INIT          1

/****************************************************************************
 * Enums: tx_x2_control0_msg_selector
 */
#define tx_x2_control0_msg_selector_RESERVED               0
#define tx_x2_control0_msg_selector_VALUE_802p3            1
#define tx_x2_control0_msg_selector_VALUE_802p9            2
#define tx_x2_control0_msg_selector_VALUE_802p5            3
#define tx_x2_control0_msg_selector_VALUE_1394             4

/****************************************************************************
 * Enums: tx_x2_control0_synce_enum
 */
#define tx_x2_control0_synce_enum_SYNCE_NO_DIV             0
#define tx_x2_control0_synce_enum_SYNCE_DIV_7              1
#define tx_x2_control0_synce_enum_SYNCE_DIV_11             2

/****************************************************************************
 * Enums: tx_x2_control0_synce_enum_stage0
 */
#define tx_x2_control0_synce_enum_stage0_SYNCE_NO_DIV      0
#define tx_x2_control0_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define tx_x2_control0_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: tx_x2_control0_cl91_sync_state
 */
#define tx_x2_control0_cl91_sync_state_FIND_1ST            0
#define tx_x2_control0_cl91_sync_state_COUNT_NEXT          1
#define tx_x2_control0_cl91_sync_state_COMP_2ND            2
#define tx_x2_control0_cl91_sync_state_TWO_GOOD            3

/****************************************************************************
 * Enums: tx_x2_control0_cl91_algn_state
 */
#define tx_x2_control0_cl91_algn_state_LOSS_OF_ALIGNMENT   0
#define tx_x2_control0_cl91_algn_state_DESKEW              1
#define tx_x2_control0_cl91_algn_state_DESKEW_FAIL         2
#define tx_x2_control0_cl91_algn_state_ALIGN_ACQUIRED      3
#define tx_x2_control0_cl91_algn_state_CW_GOOD             4
#define tx_x2_control0_cl91_algn_state_CW_BAD              5
#define tx_x2_control0_cl91_algn_state_THREE_BAD           6

/****************************************************************************
 * Enums: tx_x2_control0_fec_sel
 */
#define tx_x2_control0_fec_sel_NO_FEC                      0
#define tx_x2_control0_fec_sel_FEC_CL74                    1
#define tx_x2_control0_fec_sel_FEC_CL91                    2

/****************************************************************************
 * Enums: tx_x2_status0_cl36TxEEEStates_l
 */
#define tx_x2_status0_cl36TxEEEStates_l_TX_REFRESH         8
#define tx_x2_status0_cl36TxEEEStates_l_TX_QUIET           4
#define tx_x2_status0_cl36TxEEEStates_l_TX_SLEEP           2
#define tx_x2_status0_cl36TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x2_status0_cl36TxEEEStates_c
 */
#define tx_x2_status0_cl36TxEEEStates_c_TX_REFRESH         3
#define tx_x2_status0_cl36TxEEEStates_c_TX_QUIET           2
#define tx_x2_status0_cl36TxEEEStates_c_TX_SLEEP           1
#define tx_x2_status0_cl36TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x2_status0_cl49TxEEEStates_l
 */
#define tx_x2_status0_cl49TxEEEStates_l_SCR_RESET_2        64
#define tx_x2_status0_cl49TxEEEStates_l_SCR_RESET_1        32
#define tx_x2_status0_cl49TxEEEStates_l_TX_WAKE            16
#define tx_x2_status0_cl49TxEEEStates_l_TX_REFRESH         8
#define tx_x2_status0_cl49TxEEEStates_l_TX_QUIET           4
#define tx_x2_status0_cl49TxEEEStates_l_TX_SLEEP           2
#define tx_x2_status0_cl49TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x2_status0_fec_req_enum
 */
#define tx_x2_status0_fec_req_enum_FEC_not_supported       0
#define tx_x2_status0_fec_req_enum_FEC_supported_but_not_requested 1
#define tx_x2_status0_fec_req_enum_invalid_setting         2
#define tx_x2_status0_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: tx_x2_status0_cl73_pause_enum
 */
#define tx_x2_status0_cl73_pause_enum_No_PAUSE_ability     0
#define tx_x2_status0_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define tx_x2_status0_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define tx_x2_status0_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: tx_x2_status0_cl49TxEEEStates_c
 */
#define tx_x2_status0_cl49TxEEEStates_c_SCR_RESET_2        6
#define tx_x2_status0_cl49TxEEEStates_c_SCR_RESET_1        5
#define tx_x2_status0_cl49TxEEEStates_c_TX_WAKE            4
#define tx_x2_status0_cl49TxEEEStates_c_TX_REFRESH         3
#define tx_x2_status0_cl49TxEEEStates_c_TX_QUIET           2
#define tx_x2_status0_cl49TxEEEStates_c_TX_SLEEP           1
#define tx_x2_status0_cl49TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x2_status0_cl36RxEEEStates_l
 */
#define tx_x2_status0_cl36RxEEEStates_l_RX_LINK_FAIL       32
#define tx_x2_status0_cl36RxEEEStates_l_RX_WTF             16
#define tx_x2_status0_cl36RxEEEStates_l_RX_WAKE            8
#define tx_x2_status0_cl36RxEEEStates_l_RX_QUIET           4
#define tx_x2_status0_cl36RxEEEStates_l_RX_SLEEP           2
#define tx_x2_status0_cl36RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x2_status0_cl36RxEEEStates_c
 */
#define tx_x2_status0_cl36RxEEEStates_c_RX_LINK_FAIL       5
#define tx_x2_status0_cl36RxEEEStates_c_RX_WTF             4
#define tx_x2_status0_cl36RxEEEStates_c_RX_WAKE            3
#define tx_x2_status0_cl36RxEEEStates_c_RX_QUIET           2
#define tx_x2_status0_cl36RxEEEStates_c_RX_SLEEP           1
#define tx_x2_status0_cl36RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x2_status0_cl49RxEEEStates_l
 */
#define tx_x2_status0_cl49RxEEEStates_l_RX_LINK_FAIL       32
#define tx_x2_status0_cl49RxEEEStates_l_RX_WTF             16
#define tx_x2_status0_cl49RxEEEStates_l_RX_WAKE            8
#define tx_x2_status0_cl49RxEEEStates_l_RX_QUIET           4
#define tx_x2_status0_cl49RxEEEStates_l_RX_SLEEP           2
#define tx_x2_status0_cl49RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x2_status0_cl49RxEEEStates_c
 */
#define tx_x2_status0_cl49RxEEEStates_c_RX_LINK_FAIL       5
#define tx_x2_status0_cl49RxEEEStates_c_RX_WTF             4
#define tx_x2_status0_cl49RxEEEStates_c_RX_WAKE            3
#define tx_x2_status0_cl49RxEEEStates_c_RX_QUIET           2
#define tx_x2_status0_cl49RxEEEStates_c_RX_SLEEP           1
#define tx_x2_status0_cl49RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x2_status0_cl48TxEEEStates_l
 */
#define tx_x2_status0_cl48TxEEEStates_l_TX_REFRESH         8
#define tx_x2_status0_cl48TxEEEStates_l_TX_QUIET           4
#define tx_x2_status0_cl48TxEEEStates_l_TX_SLEEP           2
#define tx_x2_status0_cl48TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x2_status0_cl48TxEEEStates_c
 */
#define tx_x2_status0_cl48TxEEEStates_c_TX_REFRESH         3
#define tx_x2_status0_cl48TxEEEStates_c_TX_QUIET           2
#define tx_x2_status0_cl48TxEEEStates_c_TX_SLEEP           1
#define tx_x2_status0_cl48TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x2_status0_cl48RxEEEStates_l
 */
#define tx_x2_status0_cl48RxEEEStates_l_RX_LINK_FAIL       32
#define tx_x2_status0_cl48RxEEEStates_l_RX_WAKE            16
#define tx_x2_status0_cl48RxEEEStates_l_RX_QUIET           8
#define tx_x2_status0_cl48RxEEEStates_l_RX_DEACT           4
#define tx_x2_status0_cl48RxEEEStates_l_RX_SLEEP           2
#define tx_x2_status0_cl48RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x2_status0_cl48RxEEEStates_c
 */
#define tx_x2_status0_cl48RxEEEStates_c_RX_LINK_FAIL       5
#define tx_x2_status0_cl48RxEEEStates_c_RX_WAKE            4
#define tx_x2_status0_cl48RxEEEStates_c_RX_QUIET           3
#define tx_x2_status0_cl48RxEEEStates_c_RX_DEACT           2
#define tx_x2_status0_cl48RxEEEStates_c_RX_SLEEP           1
#define tx_x2_status0_cl48RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x2_status0_IQP_Options
 */
#define tx_x2_status0_IQP_Options_i50uA                    0
#define tx_x2_status0_IQP_Options_i100uA                   1
#define tx_x2_status0_IQP_Options_i150uA                   2
#define tx_x2_status0_IQP_Options_i200uA                   3
#define tx_x2_status0_IQP_Options_i250uA                   4
#define tx_x2_status0_IQP_Options_i300uA                   5
#define tx_x2_status0_IQP_Options_i350uA                   6
#define tx_x2_status0_IQP_Options_i400uA                   7
#define tx_x2_status0_IQP_Options_i450uA                   8
#define tx_x2_status0_IQP_Options_i500uA                   9
#define tx_x2_status0_IQP_Options_i550uA                   10
#define tx_x2_status0_IQP_Options_i600uA                   11
#define tx_x2_status0_IQP_Options_i650uA                   12
#define tx_x2_status0_IQP_Options_i700uA                   13
#define tx_x2_status0_IQP_Options_i750uA                   14
#define tx_x2_status0_IQP_Options_i800uA                   15

/****************************************************************************
 * Enums: tx_x2_status0_IDriver_Options
 */
#define tx_x2_status0_IDriver_Options_v680mV               0
#define tx_x2_status0_IDriver_Options_v730mV               1
#define tx_x2_status0_IDriver_Options_v780mV               2
#define tx_x2_status0_IDriver_Options_v830mV               3
#define tx_x2_status0_IDriver_Options_v880mV               4
#define tx_x2_status0_IDriver_Options_v930mV               5
#define tx_x2_status0_IDriver_Options_v980mV               6
#define tx_x2_status0_IDriver_Options_v1010mV              7
#define tx_x2_status0_IDriver_Options_v1040mV              8
#define tx_x2_status0_IDriver_Options_v1060mV              9
#define tx_x2_status0_IDriver_Options_v1070mV              10
#define tx_x2_status0_IDriver_Options_v1080mV              11
#define tx_x2_status0_IDriver_Options_v1085mV              12
#define tx_x2_status0_IDriver_Options_v1090mV              13
#define tx_x2_status0_IDriver_Options_v1095mV              14
#define tx_x2_status0_IDriver_Options_v1100mV              15

/****************************************************************************
 * Enums: tx_x2_status0_operationModes
 */
#define tx_x2_status0_operationModes_XGXS                  0
#define tx_x2_status0_operationModes_XGXG_nCC              1
#define tx_x2_status0_operationModes_Indlane_OS8           4
#define tx_x2_status0_operationModes_IndLane_OS5           5
#define tx_x2_status0_operationModes_PCI                   7
#define tx_x2_status0_operationModes_XGXS_nLQ              8
#define tx_x2_status0_operationModes_XGXS_nLQnCC           9
#define tx_x2_status0_operationModes_PBypass               10
#define tx_x2_status0_operationModes_PBypass_nDSK          11
#define tx_x2_status0_operationModes_ComboCoreMode         12
#define tx_x2_status0_operationModes_Clocks_off            15

/****************************************************************************
 * Enums: tx_x2_status0_actualSpeeds
 */
#define tx_x2_status0_actualSpeeds_dr_10M                  0
#define tx_x2_status0_actualSpeeds_dr_100M                 1
#define tx_x2_status0_actualSpeeds_dr_1G                   2
#define tx_x2_status0_actualSpeeds_dr_2p5G                 3
#define tx_x2_status0_actualSpeeds_dr_5G_X4                4
#define tx_x2_status0_actualSpeeds_dr_6G_X4                5
#define tx_x2_status0_actualSpeeds_dr_10G_HiG              6
#define tx_x2_status0_actualSpeeds_dr_10G_CX4              7
#define tx_x2_status0_actualSpeeds_dr_12G_HiG              8
#define tx_x2_status0_actualSpeeds_dr_12p5G_X4             9
#define tx_x2_status0_actualSpeeds_dr_13G_X4               10
#define tx_x2_status0_actualSpeeds_dr_15G_X4               11
#define tx_x2_status0_actualSpeeds_dr_16G_X4               12
#define tx_x2_status0_actualSpeeds_dr_1G_KX                13
#define tx_x2_status0_actualSpeeds_dr_10G_KX4              14
#define tx_x2_status0_actualSpeeds_dr_10G_KR               15
#define tx_x2_status0_actualSpeeds_dr_5G                   16
#define tx_x2_status0_actualSpeeds_dr_6p4G                 17
#define tx_x2_status0_actualSpeeds_dr_20G_X4               18
#define tx_x2_status0_actualSpeeds_dr_21G_X4               19
#define tx_x2_status0_actualSpeeds_dr_25G_X4               20
#define tx_x2_status0_actualSpeeds_dr_10G_HiG_DXGXS        21
#define tx_x2_status0_actualSpeeds_dr_10G_DXGXS            22
#define tx_x2_status0_actualSpeeds_dr_10p5G_HiG_DXGXS      23
#define tx_x2_status0_actualSpeeds_dr_10p5G_DXGXS          24
#define tx_x2_status0_actualSpeeds_dr_12p773G_HiG_DXGXS    25
#define tx_x2_status0_actualSpeeds_dr_12p773G_DXGXS        26
#define tx_x2_status0_actualSpeeds_dr_10G_XFI              27
#define tx_x2_status0_actualSpeeds_dr_40G                  28
#define tx_x2_status0_actualSpeeds_dr_20G_HiG_DXGXS        29
#define tx_x2_status0_actualSpeeds_dr_20G_DXGXS            30
#define tx_x2_status0_actualSpeeds_dr_10G_SFI              31
#define tx_x2_status0_actualSpeeds_dr_31p5G                32
#define tx_x2_status0_actualSpeeds_dr_32p7G                33
#define tx_x2_status0_actualSpeeds_dr_20G_SCR              34
#define tx_x2_status0_actualSpeeds_dr_10G_HiG_DXGXS_SCR    35
#define tx_x2_status0_actualSpeeds_dr_10G_DXGXS_SCR        36
#define tx_x2_status0_actualSpeeds_dr_12G_R2               37
#define tx_x2_status0_actualSpeeds_dr_10G_X2               38
#define tx_x2_status0_actualSpeeds_dr_40G_KR4              39
#define tx_x2_status0_actualSpeeds_dr_40G_CR4              40
#define tx_x2_status0_actualSpeeds_dr_100G_CR10            41
#define tx_x2_status0_actualSpeeds_dr_15p75G_DXGXS         44
#define tx_x2_status0_actualSpeeds_dr_20G_KR2              57
#define tx_x2_status0_actualSpeeds_dr_20G_CR2              58

/****************************************************************************
 * Enums: tx_x2_status0_actualSpeedsMisc1
 */
#define tx_x2_status0_actualSpeedsMisc1_dr_2500BRCM_X1     16
#define tx_x2_status0_actualSpeedsMisc1_dr_5000BRCM_X4     17
#define tx_x2_status0_actualSpeedsMisc1_dr_6000BRCM_X4     18
#define tx_x2_status0_actualSpeedsMisc1_dr_10GHiGig_X4     19
#define tx_x2_status0_actualSpeedsMisc1_dr_10GBASE_CX4     20
#define tx_x2_status0_actualSpeedsMisc1_dr_12GHiGig_X4     21
#define tx_x2_status0_actualSpeedsMisc1_dr_12p5GHiGig_X4   22
#define tx_x2_status0_actualSpeedsMisc1_dr_13GHiGig_X4     23
#define tx_x2_status0_actualSpeedsMisc1_dr_15GHiGig_X4     24
#define tx_x2_status0_actualSpeedsMisc1_dr_16GHiGig_X4     25
#define tx_x2_status0_actualSpeedsMisc1_dr_5000BRCM_X1     26
#define tx_x2_status0_actualSpeedsMisc1_dr_6363BRCM_X1     27
#define tx_x2_status0_actualSpeedsMisc1_dr_20GHiGig_X4     28
#define tx_x2_status0_actualSpeedsMisc1_dr_21GHiGig_X4     29
#define tx_x2_status0_actualSpeedsMisc1_dr_25p45GHiGig_X4  30
#define tx_x2_status0_actualSpeedsMisc1_dr_10G_HiG_DXGXS   31

/****************************************************************************
 * Enums: tx_x2_status0_IndLaneModes
 */
#define tx_x2_status0_IndLaneModes_SWSDR_div2              0
#define tx_x2_status0_IndLaneModes_SWSDR_div1              1
#define tx_x2_status0_IndLaneModes_DWSDR_div2              2
#define tx_x2_status0_IndLaneModes_DWSDR_div1              3

/****************************************************************************
 * Enums: tx_x2_status0_prbsSelect
 */
#define tx_x2_status0_prbsSelect_prbs7                     0
#define tx_x2_status0_prbsSelect_prbs15                    1
#define tx_x2_status0_prbsSelect_prbs23                    2
#define tx_x2_status0_prbsSelect_prbs31                    3

/****************************************************************************
 * Enums: tx_x2_status0_vcoDivider
 */
#define tx_x2_status0_vcoDivider_div32                     0
#define tx_x2_status0_vcoDivider_div36                     1
#define tx_x2_status0_vcoDivider_div40                     2
#define tx_x2_status0_vcoDivider_div42                     3
#define tx_x2_status0_vcoDivider_div48                     4
#define tx_x2_status0_vcoDivider_div50                     5
#define tx_x2_status0_vcoDivider_div52                     6
#define tx_x2_status0_vcoDivider_div54                     7
#define tx_x2_status0_vcoDivider_div60                     8
#define tx_x2_status0_vcoDivider_div64                     9
#define tx_x2_status0_vcoDivider_div66                     10
#define tx_x2_status0_vcoDivider_div68                     11
#define tx_x2_status0_vcoDivider_div70                     12
#define tx_x2_status0_vcoDivider_div80                     13
#define tx_x2_status0_vcoDivider_div92                     14
#define tx_x2_status0_vcoDivider_div100                    15

/****************************************************************************
 * Enums: tx_x2_status0_refClkSelect
 */
#define tx_x2_status0_refClkSelect_clk_25MHz               0
#define tx_x2_status0_refClkSelect_clk_100MHz              1
#define tx_x2_status0_refClkSelect_clk_125MHz              2
#define tx_x2_status0_refClkSelect_clk_156p25MHz           3
#define tx_x2_status0_refClkSelect_clk_187p5MHz            4
#define tx_x2_status0_refClkSelect_clk_161p25Mhz           5
#define tx_x2_status0_refClkSelect_clk_50Mhz               6
#define tx_x2_status0_refClkSelect_clk_106p25Mhz           7

/****************************************************************************
 * Enums: tx_x2_status0_aerMMDdevTypeSelect
 */
#define tx_x2_status0_aerMMDdevTypeSelect_combo_core       0
#define tx_x2_status0_aerMMDdevTypeSelect_PMA_PMD          1
#define tx_x2_status0_aerMMDdevTypeSelect_PCS              3
#define tx_x2_status0_aerMMDdevTypeSelect_PHY              4
#define tx_x2_status0_aerMMDdevTypeSelect_DTE              5
#define tx_x2_status0_aerMMDdevTypeSelect_CL73_AN          7

/****************************************************************************
 * Enums: tx_x2_status0_aerMMDportSelect
 */
#define tx_x2_status0_aerMMDportSelect_ln0                 0
#define tx_x2_status0_aerMMDportSelect_ln1                 1
#define tx_x2_status0_aerMMDportSelect_ln2                 2
#define tx_x2_status0_aerMMDportSelect_ln3                 3
#define tx_x2_status0_aerMMDportSelect_BCST_ln0_1_2_3      511
#define tx_x2_status0_aerMMDportSelect_BCST_ln0_1          512
#define tx_x2_status0_aerMMDportSelect_BCST_ln2_3          513

/****************************************************************************
 * Enums: tx_x2_status0_firmwareModeSelect
 */
#define tx_x2_status0_firmwareModeSelect_DEFAULT           0
#define tx_x2_status0_firmwareModeSelect_SFP_OPT_LR        1
#define tx_x2_status0_firmwareModeSelect_SFP_DAC           2
#define tx_x2_status0_firmwareModeSelect_XLAUI             3
#define tx_x2_status0_firmwareModeSelect_LONG_CH_6G        4

/****************************************************************************
 * Enums: tx_x2_status0_tempIdxSelect
 */
#define tx_x2_status0_tempIdxSelect_LTE__22p9C             15
#define tx_x2_status0_tempIdxSelect_LTE__12p6C             14
#define tx_x2_status0_tempIdxSelect_LTE__3p0C              13
#define tx_x2_status0_tempIdxSelect_LTE_6p7C               12
#define tx_x2_status0_tempIdxSelect_LTE_16p4C              11
#define tx_x2_status0_tempIdxSelect_LTE_26p6C              10
#define tx_x2_status0_tempIdxSelect_LTE_36p3C              9
#define tx_x2_status0_tempIdxSelect_LTE_46p0C              8
#define tx_x2_status0_tempIdxSelect_LTE_56p2C              7
#define tx_x2_status0_tempIdxSelect_LTE_65p9C              6
#define tx_x2_status0_tempIdxSelect_LTE_75p6C              5
#define tx_x2_status0_tempIdxSelect_LTE_85p3C              4
#define tx_x2_status0_tempIdxSelect_LTE_95p5C              3
#define tx_x2_status0_tempIdxSelect_LTE_105p2C             2
#define tx_x2_status0_tempIdxSelect_LTE_114p9C             1
#define tx_x2_status0_tempIdxSelect_LTE_125p1C             0

/****************************************************************************
 * Enums: tx_x2_status0_port_mode
 */
#define tx_x2_status0_port_mode_QUAD_PORT                  0
#define tx_x2_status0_port_mode_TRI_1_PORT                 1
#define tx_x2_status0_port_mode_TRI_2_PORT                 2
#define tx_x2_status0_port_mode_DUAL_PORT                  3
#define tx_x2_status0_port_mode_SINGLE_PORT                4

/****************************************************************************
 * Enums: tx_x2_status0_rev_letter_enum
 */
#define tx_x2_status0_rev_letter_enum_REV_A                0
#define tx_x2_status0_rev_letter_enum_REV_B                1
#define tx_x2_status0_rev_letter_enum_REV_C                2
#define tx_x2_status0_rev_letter_enum_REV_D                3

/****************************************************************************
 * Enums: tx_x2_status0_rev_number_enum
 */
#define tx_x2_status0_rev_number_enum_REV_0                0
#define tx_x2_status0_rev_number_enum_REV_1                1
#define tx_x2_status0_rev_number_enum_REV_2                2
#define tx_x2_status0_rev_number_enum_REV_3                3
#define tx_x2_status0_rev_number_enum_REV_4                4
#define tx_x2_status0_rev_number_enum_REV_5                5
#define tx_x2_status0_rev_number_enum_REV_6                6
#define tx_x2_status0_rev_number_enum_REV_7                7

/****************************************************************************
 * Enums: tx_x2_status0_bonding_enum
 */
#define tx_x2_status0_bonding_enum_WIRE_BOND               0
#define tx_x2_status0_bonding_enum_FLIP_CHIP               1

/****************************************************************************
 * Enums: tx_x2_status0_tech_process
 */
#define tx_x2_status0_tech_process_PROCESS_90NM            0
#define tx_x2_status0_tech_process_PROCESS_65NM            1
#define tx_x2_status0_tech_process_PROCESS_40NM            2
#define tx_x2_status0_tech_process_PROCESS_28NM            3

/****************************************************************************
 * Enums: tx_x2_status0_model_num
 */
#define tx_x2_status0_model_num_SERDES_CL73                0
#define tx_x2_status0_model_num_XGXS_16G                   1
#define tx_x2_status0_model_num_HYPERCORE                  2
#define tx_x2_status0_model_num_HYPERLITE                  3
#define tx_x2_status0_model_num_PCIE_G2_PIPE               4
#define tx_x2_status0_model_num_SERDES_1p25GBd             5
#define tx_x2_status0_model_num_SATA2                      6
#define tx_x2_status0_model_num_QSGMII                     7
#define tx_x2_status0_model_num_XGXS10G                    8
#define tx_x2_status0_model_num_WARPCORE                   9
#define tx_x2_status0_model_num_XFICORE                    10
#define tx_x2_status0_model_num_RXFI                       11
#define tx_x2_status0_model_num_WARPLITE                   12
#define tx_x2_status0_model_num_PENTACORE                  13
#define tx_x2_status0_model_num_ESM                        14
#define tx_x2_status0_model_num_QUAD_SGMII                 15
#define tx_x2_status0_model_num_WARPCORE_3                 16
#define tx_x2_status0_model_num_TSC                        17
#define tx_x2_status0_model_num_TSC4E                      18
#define tx_x2_status0_model_num_TSC12E                     19
#define tx_x2_status0_model_num_TSC4F                      20
#define tx_x2_status0_model_num_XGXS_CL73_90NM             29
#define tx_x2_status0_model_num_SERDES_CL73_90NM           30
#define tx_x2_status0_model_num_WARPCORE3                  32
#define tx_x2_status0_model_num_WARPCORE4_TSC              33
#define tx_x2_status0_model_num_RXAUI                      34

/****************************************************************************
 * Enums: tx_x2_status0_payload
 */
#define tx_x2_status0_payload_REPEAT_2_BYTES               0
#define tx_x2_status0_payload_RAMPING                      1
#define tx_x2_status0_payload_CL48_CRPAT                   2
#define tx_x2_status0_payload_CL48_CJPAT                   3
#define tx_x2_status0_payload_CL36_LONG_CRPAT              4
#define tx_x2_status0_payload_CL36_SHORT_CRPAT             5

/****************************************************************************
 * Enums: tx_x2_status0_sc
 */
#define tx_x2_status0_sc_S_10G_CR1                         0
#define tx_x2_status0_sc_S_10G_KR1                         1
#define tx_x2_status0_sc_S_10G_X1                          2
#define tx_x2_status0_sc_S_10G_HG2_CR1                     4
#define tx_x2_status0_sc_S_10G_HG2_KR1                     5
#define tx_x2_status0_sc_S_10G_HG2_X1                      6
#define tx_x2_status0_sc_S_20G_CR1                         8
#define tx_x2_status0_sc_S_20G_KR1                         9
#define tx_x2_status0_sc_S_20G_X1                          10
#define tx_x2_status0_sc_S_20G_HG2_CR1                     12
#define tx_x2_status0_sc_S_20G_HG2_KR1                     13
#define tx_x2_status0_sc_S_20G_HG2_X1                      14
#define tx_x2_status0_sc_S_25G_CR1                         16
#define tx_x2_status0_sc_S_25G_KR1                         17
#define tx_x2_status0_sc_S_25G_X1                          18
#define tx_x2_status0_sc_S_25G_HG2_CR1                     20
#define tx_x2_status0_sc_S_25G_HG2_KR1                     21
#define tx_x2_status0_sc_S_25G_HG2_X1                      22
#define tx_x2_status0_sc_S_20G_CR2                         24
#define tx_x2_status0_sc_S_20G_KR2                         25
#define tx_x2_status0_sc_S_20G_X2                          26
#define tx_x2_status0_sc_S_20G_HG2_CR2                     28
#define tx_x2_status0_sc_S_20G_HG2_KR2                     29
#define tx_x2_status0_sc_S_20G_HG2_X2                      30
#define tx_x2_status0_sc_S_40G_CR2                         32
#define tx_x2_status0_sc_S_40G_KR2                         33
#define tx_x2_status0_sc_S_40G_X2                          34
#define tx_x2_status0_sc_S_40G_HG2_CR2                     36
#define tx_x2_status0_sc_S_40G_HG2_KR2                     37
#define tx_x2_status0_sc_S_40G_HG2_X2                      38
#define tx_x2_status0_sc_S_40G_CR4                         40
#define tx_x2_status0_sc_S_40G_KR4                         41
#define tx_x2_status0_sc_S_40G_X4                          42
#define tx_x2_status0_sc_S_40G_HG2_CR4                     44
#define tx_x2_status0_sc_S_40G_HG2_KR4                     45
#define tx_x2_status0_sc_S_40G_HG2_X4                      46
#define tx_x2_status0_sc_S_50G_CR2                         48
#define tx_x2_status0_sc_S_50G_KR2                         49
#define tx_x2_status0_sc_S_50G_X2                          50
#define tx_x2_status0_sc_S_50G_HG2_CR2                     52
#define tx_x2_status0_sc_S_50G_HG2_KR2                     53
#define tx_x2_status0_sc_S_50G_HG2_X2                      54
#define tx_x2_status0_sc_S_50G_CR4                         56
#define tx_x2_status0_sc_S_50G_KR4                         57
#define tx_x2_status0_sc_S_50G_X4                          58
#define tx_x2_status0_sc_S_50G_HG2_CR4                     60
#define tx_x2_status0_sc_S_50G_HG2_KR4                     61
#define tx_x2_status0_sc_S_50G_HG2_X4                      62
#define tx_x2_status0_sc_S_100G_CR4                        64
#define tx_x2_status0_sc_S_100G_KR4                        65
#define tx_x2_status0_sc_S_100G_X4                         66
#define tx_x2_status0_sc_S_100G_HG2_CR4                    68
#define tx_x2_status0_sc_S_100G_HG2_KR4                    69
#define tx_x2_status0_sc_S_100G_HG2_X4                     70
#define tx_x2_status0_sc_S_CL73_20GVCO                     72
#define tx_x2_status0_sc_S_CL73_25GVCO                     80
#define tx_x2_status0_sc_S_CL36_20GVCO                     88
#define tx_x2_status0_sc_S_CL36_25GVCO                     96

/****************************************************************************
 * Enums: tx_x2_status0_t_fifo_modes
 */
#define tx_x2_status0_t_fifo_modes_T_FIFO_MODE_BYPASS      0
#define tx_x2_status0_t_fifo_modes_T_FIFO_MODE_40G         1
#define tx_x2_status0_t_fifo_modes_T_FIFO_MODE_100G        2
#define tx_x2_status0_t_fifo_modes_T_FIFO_MODE_20G         3

/****************************************************************************
 * Enums: tx_x2_status0_t_enc_modes
 */
#define tx_x2_status0_t_enc_modes_T_ENC_MODE_BYPASS        0
#define tx_x2_status0_t_enc_modes_T_ENC_MODE_CL49          1
#define tx_x2_status0_t_enc_modes_T_ENC_MODE_CL82          2

/****************************************************************************
 * Enums: tx_x2_status0_btmx_mode
 */
#define tx_x2_status0_btmx_mode_BS_BTMX_MODE_1to1          0
#define tx_x2_status0_btmx_mode_BS_BTMX_MODE_2to1          1
#define tx_x2_status0_btmx_mode_BS_BTMX_MODE_5to1          2

/****************************************************************************
 * Enums: tx_x2_status0_t_type_cl82
 */
#define tx_x2_status0_t_type_cl82_T_TYPE_B1                5
#define tx_x2_status0_t_type_cl82_T_TYPE_C                 4
#define tx_x2_status0_t_type_cl82_T_TYPE_S                 3
#define tx_x2_status0_t_type_cl82_T_TYPE_T                 2
#define tx_x2_status0_t_type_cl82_T_TYPE_D                 1
#define tx_x2_status0_t_type_cl82_T_TYPE_E                 0

/****************************************************************************
 * Enums: tx_x2_status0_txsm_state_cl82
 */
#define tx_x2_status0_txsm_state_cl82_TX_HIG_END           6
#define tx_x2_status0_txsm_state_cl82_TX_HIG_START         5
#define tx_x2_status0_txsm_state_cl82_TX_E                 4
#define tx_x2_status0_txsm_state_cl82_TX_T                 3
#define tx_x2_status0_txsm_state_cl82_TX_D                 2
#define tx_x2_status0_txsm_state_cl82_TX_C                 1
#define tx_x2_status0_txsm_state_cl82_TX_INIT              0

/****************************************************************************
 * Enums: tx_x2_status0_ltxsm_state_cl82
 */
#define tx_x2_status0_ltxsm_state_cl82_TX_HIG_END          64
#define tx_x2_status0_ltxsm_state_cl82_TX_HIG_START        32
#define tx_x2_status0_ltxsm_state_cl82_TX_E                16
#define tx_x2_status0_ltxsm_state_cl82_TX_T                8
#define tx_x2_status0_ltxsm_state_cl82_TX_D                4
#define tx_x2_status0_ltxsm_state_cl82_TX_C                2
#define tx_x2_status0_ltxsm_state_cl82_TX_INIT             1

/****************************************************************************
 * Enums: tx_x2_status0_r_type_coded_cl82
 */
#define tx_x2_status0_r_type_coded_cl82_R_TYPE_B1          32
#define tx_x2_status0_r_type_coded_cl82_R_TYPE_C           16
#define tx_x2_status0_r_type_coded_cl82_R_TYPE_S           8
#define tx_x2_status0_r_type_coded_cl82_R_TYPE_T           4
#define tx_x2_status0_r_type_coded_cl82_R_TYPE_D           2
#define tx_x2_status0_r_type_coded_cl82_R_TYPE_E           1

/****************************************************************************
 * Enums: tx_x2_status0_rxsm_state_cl82
 */
#define tx_x2_status0_rxsm_state_cl82_RX_HIG_END           64
#define tx_x2_status0_rxsm_state_cl82_RX_HIG_START         32
#define tx_x2_status0_rxsm_state_cl82_RX_E                 16
#define tx_x2_status0_rxsm_state_cl82_RX_T                 8
#define tx_x2_status0_rxsm_state_cl82_RX_D                 4
#define tx_x2_status0_rxsm_state_cl82_RX_C                 2
#define tx_x2_status0_rxsm_state_cl82_RX_INIT              1

/****************************************************************************
 * Enums: tx_x2_status0_deskew_state
 */
#define tx_x2_status0_deskew_state_ALIGN_ACQUIRED          2
#define tx_x2_status0_deskew_state_LOSS_OF_ALIGNMENT       1

/****************************************************************************
 * Enums: tx_x2_status0_os_mode_enum
 */
#define tx_x2_status0_os_mode_enum_OS_MODE_1               0
#define tx_x2_status0_os_mode_enum_OS_MODE_2               1
#define tx_x2_status0_os_mode_enum_OS_MODE_4               2
#define tx_x2_status0_os_mode_enum_OS_MODE_16p5            8
#define tx_x2_status0_os_mode_enum_OS_MODE_20p625          12

/****************************************************************************
 * Enums: tx_x2_status0_scr_modes
 */
#define tx_x2_status0_scr_modes_T_SCR_MODE_BYPASS          0
#define tx_x2_status0_scr_modes_T_SCR_MODE_CL49            1
#define tx_x2_status0_scr_modes_T_SCR_MODE_40G_2_LANE      2
#define tx_x2_status0_scr_modes_T_SCR_MODE_100G            3
#define tx_x2_status0_scr_modes_T_SCR_MODE_20G             4
#define tx_x2_status0_scr_modes_T_SCR_MODE_40G_4_LANE      5

/****************************************************************************
 * Enums: tx_x2_status0_descr_modes
 */
#define tx_x2_status0_descr_modes_R_DESCR_MODE_BYPASS      0
#define tx_x2_status0_descr_modes_R_DESCR_MODE_CL49        1
#define tx_x2_status0_descr_modes_R_DESCR_MODE_CL82        2

/****************************************************************************
 * Enums: tx_x2_status0_r_dec_tl_mode
 */
#define tx_x2_status0_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS   0
#define tx_x2_status0_r_dec_tl_mode_R_DEC_TL_MODE_CL49     1
#define tx_x2_status0_r_dec_tl_mode_R_DEC_TL_MODE_CL82     2

/****************************************************************************
 * Enums: tx_x2_status0_r_dec_fsm_mode
 */
#define tx_x2_status0_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define tx_x2_status0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49   1
#define tx_x2_status0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82   2

/****************************************************************************
 * Enums: tx_x2_status0_r_deskew_mode
 */
#define tx_x2_status0_r_deskew_mode_R_DESKEW_MODE_BYPASS   0
#define tx_x2_status0_r_deskew_mode_R_DESKEW_MODE_20G      1
#define tx_x2_status0_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define tx_x2_status0_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define tx_x2_status0_r_deskew_mode_R_DESKEW_MODE_100G     4
#define tx_x2_status0_r_deskew_mode_R_DESKEW_MODE_CL49     5
#define tx_x2_status0_r_deskew_mode_R_DESKEW_MODE_CL91     6

/****************************************************************************
 * Enums: tx_x2_status0_bs_dist_modes
 */
#define tx_x2_status0_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define tx_x2_status0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define tx_x2_status0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define tx_x2_status0_bs_dist_modes_BS_DIST_MODE_NO_TDM    3

/****************************************************************************
 * Enums: tx_x2_status0_cl49_t_type
 */
#define tx_x2_status0_cl49_t_type_BAD_T_TYPE               15
#define tx_x2_status0_cl49_t_type_T_TYPE_B0                11
#define tx_x2_status0_cl49_t_type_T_TYPE_OB                10
#define tx_x2_status0_cl49_t_type_T_TYPE_B1                9
#define tx_x2_status0_cl49_t_type_T_TYPE_DB                8
#define tx_x2_status0_cl49_t_type_T_TYPE_FC                7
#define tx_x2_status0_cl49_t_type_T_TYPE_TB                6
#define tx_x2_status0_cl49_t_type_T_TYPE_LI                5
#define tx_x2_status0_cl49_t_type_T_TYPE_C                 4
#define tx_x2_status0_cl49_t_type_T_TYPE_S                 3
#define tx_x2_status0_cl49_t_type_T_TYPE_T                 2
#define tx_x2_status0_cl49_t_type_T_TYPE_D                 1
#define tx_x2_status0_cl49_t_type_T_TYPE_E                 0

/****************************************************************************
 * Enums: tx_x2_status0_cl49_txsm_states
 */
#define tx_x2_status0_cl49_txsm_states_TX_HIG_END          7
#define tx_x2_status0_cl49_txsm_states_TX_HIG_START        6
#define tx_x2_status0_cl49_txsm_states_TX_LI               5
#define tx_x2_status0_cl49_txsm_states_TX_E                4
#define tx_x2_status0_cl49_txsm_states_TX_T                3
#define tx_x2_status0_cl49_txsm_states_TX_D                2
#define tx_x2_status0_cl49_txsm_states_TX_C                1
#define tx_x2_status0_cl49_txsm_states_TX_INIT             0

/****************************************************************************
 * Enums: tx_x2_status0_cl49_ltxsm_states
 */
#define tx_x2_status0_cl49_ltxsm_states_TX_HIG_END         128
#define tx_x2_status0_cl49_ltxsm_states_TX_HIG_START       64
#define tx_x2_status0_cl49_ltxsm_states_TX_LI              32
#define tx_x2_status0_cl49_ltxsm_states_TX_E               16
#define tx_x2_status0_cl49_ltxsm_states_TX_T               8
#define tx_x2_status0_cl49_ltxsm_states_TX_D               4
#define tx_x2_status0_cl49_ltxsm_states_TX_C               2
#define tx_x2_status0_cl49_ltxsm_states_TX_INIT            1

/****************************************************************************
 * Enums: tx_x2_status0_burst_error_mode
 */
#define tx_x2_status0_burst_error_mode_BURST_ERROR_11_BITS 0
#define tx_x2_status0_burst_error_mode_BURST_ERROR_16_BITS 1
#define tx_x2_status0_burst_error_mode_BURST_ERROR_17_BITS 2
#define tx_x2_status0_burst_error_mode_BURST_ERROR_18_BITS 3
#define tx_x2_status0_burst_error_mode_BURST_ERROR_19_BITS 4
#define tx_x2_status0_burst_error_mode_BURST_ERROR_20_BITS 5
#define tx_x2_status0_burst_error_mode_BURST_ERROR_21_BITS 6
#define tx_x2_status0_burst_error_mode_BURST_ERROR_22_BITS 7
#define tx_x2_status0_burst_error_mode_BURST_ERROR_23_BITS 8
#define tx_x2_status0_burst_error_mode_BURST_ERROR_24_BITS 9
#define tx_x2_status0_burst_error_mode_BURST_ERROR_25_BITS 10
#define tx_x2_status0_burst_error_mode_BURST_ERROR_26_BITS 11
#define tx_x2_status0_burst_error_mode_BURST_ERROR_27_BITS 12
#define tx_x2_status0_burst_error_mode_BURST_ERROR_28_BITS 13
#define tx_x2_status0_burst_error_mode_BURST_ERROR_29_BITS 14
#define tx_x2_status0_burst_error_mode_BURST_ERROR_30_BITS 15
#define tx_x2_status0_burst_error_mode_BURST_ERROR_31_BITS 16
#define tx_x2_status0_burst_error_mode_BURST_ERROR_32_BITS 17
#define tx_x2_status0_burst_error_mode_BURST_ERROR_33_BITS 18
#define tx_x2_status0_burst_error_mode_BURST_ERROR_34_BITS 19
#define tx_x2_status0_burst_error_mode_BURST_ERROR_35_BITS 20
#define tx_x2_status0_burst_error_mode_BURST_ERROR_36_BITS 21
#define tx_x2_status0_burst_error_mode_BURST_ERROR_37_BITS 22
#define tx_x2_status0_burst_error_mode_BURST_ERROR_38_BITS 23
#define tx_x2_status0_burst_error_mode_BURST_ERROR_39_BITS 24
#define tx_x2_status0_burst_error_mode_BURST_ERROR_40_BITS 25
#define tx_x2_status0_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: tx_x2_status0_bermon_state
 */
#define tx_x2_status0_bermon_state_HI_BER                  4
#define tx_x2_status0_bermon_state_GOOD_BER                3
#define tx_x2_status0_bermon_state_BER_TEST_SH             2
#define tx_x2_status0_bermon_state_START_TIMER             1
#define tx_x2_status0_bermon_state_BER_MT_INIT             0

/****************************************************************************
 * Enums: tx_x2_status0_rxsm_state_cl49
 */
#define tx_x2_status0_rxsm_state_cl49_RX_HIG_END           128
#define tx_x2_status0_rxsm_state_cl49_RX_HIG_START         64
#define tx_x2_status0_rxsm_state_cl49_RX_LI                32
#define tx_x2_status0_rxsm_state_cl49_RX_E                 16
#define tx_x2_status0_rxsm_state_cl49_RX_T                 8
#define tx_x2_status0_rxsm_state_cl49_RX_D                 4
#define tx_x2_status0_rxsm_state_cl49_RX_C                 2
#define tx_x2_status0_rxsm_state_cl49_RX_INIT              1

/****************************************************************************
 * Enums: tx_x2_status0_r_type
 */
#define tx_x2_status0_r_type_BAD_R_TYPE                    15
#define tx_x2_status0_r_type_R_TYPE_B0                     11
#define tx_x2_status0_r_type_R_TYPE_OB                     10
#define tx_x2_status0_r_type_R_TYPE_B1                     9
#define tx_x2_status0_r_type_R_TYPE_DB                     8
#define tx_x2_status0_r_type_R_TYPE_FC                     7
#define tx_x2_status0_r_type_R_TYPE_TB                     6
#define tx_x2_status0_r_type_R_TYPE_LI                     5
#define tx_x2_status0_r_type_R_TYPE_C                      4
#define tx_x2_status0_r_type_R_TYPE_S                      3
#define tx_x2_status0_r_type_R_TYPE_T                      2
#define tx_x2_status0_r_type_R_TYPE_D                      1
#define tx_x2_status0_r_type_R_TYPE_E                      0

/****************************************************************************
 * Enums: tx_x2_status0_am_lock_state
 */
#define tx_x2_status0_am_lock_state_INVALID_AM             512
#define tx_x2_status0_am_lock_state_GOOD_AM                256
#define tx_x2_status0_am_lock_state_COMP_AM                128
#define tx_x2_status0_am_lock_state_TIMER_2                64
#define tx_x2_status0_am_lock_state_AM_2_GOOD              32
#define tx_x2_status0_am_lock_state_COMP_2ND               16
#define tx_x2_status0_am_lock_state_TIMER_1                8
#define tx_x2_status0_am_lock_state_FIND_1ST               4
#define tx_x2_status0_am_lock_state_AM_RESET_CNT           2
#define tx_x2_status0_am_lock_state_AM_LOCK_INIT           1

/****************************************************************************
 * Enums: tx_x2_status0_msg_selector
 */
#define tx_x2_status0_msg_selector_RESERVED                0
#define tx_x2_status0_msg_selector_VALUE_802p3             1
#define tx_x2_status0_msg_selector_VALUE_802p9             2
#define tx_x2_status0_msg_selector_VALUE_802p5             3
#define tx_x2_status0_msg_selector_VALUE_1394              4

/****************************************************************************
 * Enums: tx_x2_status0_synce_enum
 */
#define tx_x2_status0_synce_enum_SYNCE_NO_DIV              0
#define tx_x2_status0_synce_enum_SYNCE_DIV_7               1
#define tx_x2_status0_synce_enum_SYNCE_DIV_11              2

/****************************************************************************
 * Enums: tx_x2_status0_synce_enum_stage0
 */
#define tx_x2_status0_synce_enum_stage0_SYNCE_NO_DIV       0
#define tx_x2_status0_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define tx_x2_status0_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: tx_x2_status0_cl91_sync_state
 */
#define tx_x2_status0_cl91_sync_state_FIND_1ST             0
#define tx_x2_status0_cl91_sync_state_COUNT_NEXT           1
#define tx_x2_status0_cl91_sync_state_COMP_2ND             2
#define tx_x2_status0_cl91_sync_state_TWO_GOOD             3

/****************************************************************************
 * Enums: tx_x2_status0_cl91_algn_state
 */
#define tx_x2_status0_cl91_algn_state_LOSS_OF_ALIGNMENT    0
#define tx_x2_status0_cl91_algn_state_DESKEW               1
#define tx_x2_status0_cl91_algn_state_DESKEW_FAIL          2
#define tx_x2_status0_cl91_algn_state_ALIGN_ACQUIRED       3
#define tx_x2_status0_cl91_algn_state_CW_GOOD              4
#define tx_x2_status0_cl91_algn_state_CW_BAD               5
#define tx_x2_status0_cl91_algn_state_THREE_BAD            6

/****************************************************************************
 * Enums: tx_x2_status0_fec_sel
 */
#define tx_x2_status0_fec_sel_NO_FEC                       0
#define tx_x2_status0_fec_sel_FEC_CL74                     1
#define tx_x2_status0_fec_sel_FEC_CL91                     2

/****************************************************************************
 * Enums: tx_x4_control0_cl36TxEEEStates_l
 */
#define tx_x4_control0_cl36TxEEEStates_l_TX_REFRESH        8
#define tx_x4_control0_cl36TxEEEStates_l_TX_QUIET          4
#define tx_x4_control0_cl36TxEEEStates_l_TX_SLEEP          2
#define tx_x4_control0_cl36TxEEEStates_l_TX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x4_control0_cl36TxEEEStates_c
 */
#define tx_x4_control0_cl36TxEEEStates_c_TX_REFRESH        3
#define tx_x4_control0_cl36TxEEEStates_c_TX_QUIET          2
#define tx_x4_control0_cl36TxEEEStates_c_TX_SLEEP          1
#define tx_x4_control0_cl36TxEEEStates_c_TX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x4_control0_cl49TxEEEStates_l
 */
#define tx_x4_control0_cl49TxEEEStates_l_SCR_RESET_2       64
#define tx_x4_control0_cl49TxEEEStates_l_SCR_RESET_1       32
#define tx_x4_control0_cl49TxEEEStates_l_TX_WAKE           16
#define tx_x4_control0_cl49TxEEEStates_l_TX_REFRESH        8
#define tx_x4_control0_cl49TxEEEStates_l_TX_QUIET          4
#define tx_x4_control0_cl49TxEEEStates_l_TX_SLEEP          2
#define tx_x4_control0_cl49TxEEEStates_l_TX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x4_control0_fec_req_enum
 */
#define tx_x4_control0_fec_req_enum_FEC_not_supported      0
#define tx_x4_control0_fec_req_enum_FEC_supported_but_not_requested 1
#define tx_x4_control0_fec_req_enum_invalid_setting        2
#define tx_x4_control0_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: tx_x4_control0_cl73_pause_enum
 */
#define tx_x4_control0_cl73_pause_enum_No_PAUSE_ability    0
#define tx_x4_control0_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define tx_x4_control0_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define tx_x4_control0_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: tx_x4_control0_cl49TxEEEStates_c
 */
#define tx_x4_control0_cl49TxEEEStates_c_SCR_RESET_2       6
#define tx_x4_control0_cl49TxEEEStates_c_SCR_RESET_1       5
#define tx_x4_control0_cl49TxEEEStates_c_TX_WAKE           4
#define tx_x4_control0_cl49TxEEEStates_c_TX_REFRESH        3
#define tx_x4_control0_cl49TxEEEStates_c_TX_QUIET          2
#define tx_x4_control0_cl49TxEEEStates_c_TX_SLEEP          1
#define tx_x4_control0_cl49TxEEEStates_c_TX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x4_control0_cl36RxEEEStates_l
 */
#define tx_x4_control0_cl36RxEEEStates_l_RX_LINK_FAIL      32
#define tx_x4_control0_cl36RxEEEStates_l_RX_WTF            16
#define tx_x4_control0_cl36RxEEEStates_l_RX_WAKE           8
#define tx_x4_control0_cl36RxEEEStates_l_RX_QUIET          4
#define tx_x4_control0_cl36RxEEEStates_l_RX_SLEEP          2
#define tx_x4_control0_cl36RxEEEStates_l_RX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x4_control0_cl36RxEEEStates_c
 */
#define tx_x4_control0_cl36RxEEEStates_c_RX_LINK_FAIL      5
#define tx_x4_control0_cl36RxEEEStates_c_RX_WTF            4
#define tx_x4_control0_cl36RxEEEStates_c_RX_WAKE           3
#define tx_x4_control0_cl36RxEEEStates_c_RX_QUIET          2
#define tx_x4_control0_cl36RxEEEStates_c_RX_SLEEP          1
#define tx_x4_control0_cl36RxEEEStates_c_RX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x4_control0_cl49RxEEEStates_l
 */
#define tx_x4_control0_cl49RxEEEStates_l_RX_LINK_FAIL      32
#define tx_x4_control0_cl49RxEEEStates_l_RX_WTF            16
#define tx_x4_control0_cl49RxEEEStates_l_RX_WAKE           8
#define tx_x4_control0_cl49RxEEEStates_l_RX_QUIET          4
#define tx_x4_control0_cl49RxEEEStates_l_RX_SLEEP          2
#define tx_x4_control0_cl49RxEEEStates_l_RX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x4_control0_cl49RxEEEStates_c
 */
#define tx_x4_control0_cl49RxEEEStates_c_RX_LINK_FAIL      5
#define tx_x4_control0_cl49RxEEEStates_c_RX_WTF            4
#define tx_x4_control0_cl49RxEEEStates_c_RX_WAKE           3
#define tx_x4_control0_cl49RxEEEStates_c_RX_QUIET          2
#define tx_x4_control0_cl49RxEEEStates_c_RX_SLEEP          1
#define tx_x4_control0_cl49RxEEEStates_c_RX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x4_control0_cl48TxEEEStates_l
 */
#define tx_x4_control0_cl48TxEEEStates_l_TX_REFRESH        8
#define tx_x4_control0_cl48TxEEEStates_l_TX_QUIET          4
#define tx_x4_control0_cl48TxEEEStates_l_TX_SLEEP          2
#define tx_x4_control0_cl48TxEEEStates_l_TX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x4_control0_cl48TxEEEStates_c
 */
#define tx_x4_control0_cl48TxEEEStates_c_TX_REFRESH        3
#define tx_x4_control0_cl48TxEEEStates_c_TX_QUIET          2
#define tx_x4_control0_cl48TxEEEStates_c_TX_SLEEP          1
#define tx_x4_control0_cl48TxEEEStates_c_TX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x4_control0_cl48RxEEEStates_l
 */
#define tx_x4_control0_cl48RxEEEStates_l_RX_LINK_FAIL      32
#define tx_x4_control0_cl48RxEEEStates_l_RX_WAKE           16
#define tx_x4_control0_cl48RxEEEStates_l_RX_QUIET          8
#define tx_x4_control0_cl48RxEEEStates_l_RX_DEACT          4
#define tx_x4_control0_cl48RxEEEStates_l_RX_SLEEP          2
#define tx_x4_control0_cl48RxEEEStates_l_RX_ACTIVE         1

/****************************************************************************
 * Enums: tx_x4_control0_cl48RxEEEStates_c
 */
#define tx_x4_control0_cl48RxEEEStates_c_RX_LINK_FAIL      5
#define tx_x4_control0_cl48RxEEEStates_c_RX_WAKE           4
#define tx_x4_control0_cl48RxEEEStates_c_RX_QUIET          3
#define tx_x4_control0_cl48RxEEEStates_c_RX_DEACT          2
#define tx_x4_control0_cl48RxEEEStates_c_RX_SLEEP          1
#define tx_x4_control0_cl48RxEEEStates_c_RX_ACTIVE         0

/****************************************************************************
 * Enums: tx_x4_control0_IQP_Options
 */
#define tx_x4_control0_IQP_Options_i50uA                   0
#define tx_x4_control0_IQP_Options_i100uA                  1
#define tx_x4_control0_IQP_Options_i150uA                  2
#define tx_x4_control0_IQP_Options_i200uA                  3
#define tx_x4_control0_IQP_Options_i250uA                  4
#define tx_x4_control0_IQP_Options_i300uA                  5
#define tx_x4_control0_IQP_Options_i350uA                  6
#define tx_x4_control0_IQP_Options_i400uA                  7
#define tx_x4_control0_IQP_Options_i450uA                  8
#define tx_x4_control0_IQP_Options_i500uA                  9
#define tx_x4_control0_IQP_Options_i550uA                  10
#define tx_x4_control0_IQP_Options_i600uA                  11
#define tx_x4_control0_IQP_Options_i650uA                  12
#define tx_x4_control0_IQP_Options_i700uA                  13
#define tx_x4_control0_IQP_Options_i750uA                  14
#define tx_x4_control0_IQP_Options_i800uA                  15

/****************************************************************************
 * Enums: tx_x4_control0_IDriver_Options
 */
#define tx_x4_control0_IDriver_Options_v680mV              0
#define tx_x4_control0_IDriver_Options_v730mV              1
#define tx_x4_control0_IDriver_Options_v780mV              2
#define tx_x4_control0_IDriver_Options_v830mV              3
#define tx_x4_control0_IDriver_Options_v880mV              4
#define tx_x4_control0_IDriver_Options_v930mV              5
#define tx_x4_control0_IDriver_Options_v980mV              6
#define tx_x4_control0_IDriver_Options_v1010mV             7
#define tx_x4_control0_IDriver_Options_v1040mV             8
#define tx_x4_control0_IDriver_Options_v1060mV             9
#define tx_x4_control0_IDriver_Options_v1070mV             10
#define tx_x4_control0_IDriver_Options_v1080mV             11
#define tx_x4_control0_IDriver_Options_v1085mV             12
#define tx_x4_control0_IDriver_Options_v1090mV             13
#define tx_x4_control0_IDriver_Options_v1095mV             14
#define tx_x4_control0_IDriver_Options_v1100mV             15

/****************************************************************************
 * Enums: tx_x4_control0_operationModes
 */
#define tx_x4_control0_operationModes_XGXS                 0
#define tx_x4_control0_operationModes_XGXG_nCC             1
#define tx_x4_control0_operationModes_Indlane_OS8          4
#define tx_x4_control0_operationModes_IndLane_OS5          5
#define tx_x4_control0_operationModes_PCI                  7
#define tx_x4_control0_operationModes_XGXS_nLQ             8
#define tx_x4_control0_operationModes_XGXS_nLQnCC          9
#define tx_x4_control0_operationModes_PBypass              10
#define tx_x4_control0_operationModes_PBypass_nDSK         11
#define tx_x4_control0_operationModes_ComboCoreMode        12
#define tx_x4_control0_operationModes_Clocks_off           15

/****************************************************************************
 * Enums: tx_x4_control0_actualSpeeds
 */
#define tx_x4_control0_actualSpeeds_dr_10M                 0
#define tx_x4_control0_actualSpeeds_dr_100M                1
#define tx_x4_control0_actualSpeeds_dr_1G                  2
#define tx_x4_control0_actualSpeeds_dr_2p5G                3
#define tx_x4_control0_actualSpeeds_dr_5G_X4               4
#define tx_x4_control0_actualSpeeds_dr_6G_X4               5
#define tx_x4_control0_actualSpeeds_dr_10G_HiG             6
#define tx_x4_control0_actualSpeeds_dr_10G_CX4             7
#define tx_x4_control0_actualSpeeds_dr_12G_HiG             8
#define tx_x4_control0_actualSpeeds_dr_12p5G_X4            9
#define tx_x4_control0_actualSpeeds_dr_13G_X4              10
#define tx_x4_control0_actualSpeeds_dr_15G_X4              11
#define tx_x4_control0_actualSpeeds_dr_16G_X4              12
#define tx_x4_control0_actualSpeeds_dr_1G_KX               13
#define tx_x4_control0_actualSpeeds_dr_10G_KX4             14
#define tx_x4_control0_actualSpeeds_dr_10G_KR              15
#define tx_x4_control0_actualSpeeds_dr_5G                  16
#define tx_x4_control0_actualSpeeds_dr_6p4G                17
#define tx_x4_control0_actualSpeeds_dr_20G_X4              18
#define tx_x4_control0_actualSpeeds_dr_21G_X4              19
#define tx_x4_control0_actualSpeeds_dr_25G_X4              20
#define tx_x4_control0_actualSpeeds_dr_10G_HiG_DXGXS       21
#define tx_x4_control0_actualSpeeds_dr_10G_DXGXS           22
#define tx_x4_control0_actualSpeeds_dr_10p5G_HiG_DXGXS     23
#define tx_x4_control0_actualSpeeds_dr_10p5G_DXGXS         24
#define tx_x4_control0_actualSpeeds_dr_12p773G_HiG_DXGXS   25
#define tx_x4_control0_actualSpeeds_dr_12p773G_DXGXS       26
#define tx_x4_control0_actualSpeeds_dr_10G_XFI             27
#define tx_x4_control0_actualSpeeds_dr_40G                 28
#define tx_x4_control0_actualSpeeds_dr_20G_HiG_DXGXS       29
#define tx_x4_control0_actualSpeeds_dr_20G_DXGXS           30
#define tx_x4_control0_actualSpeeds_dr_10G_SFI             31
#define tx_x4_control0_actualSpeeds_dr_31p5G               32
#define tx_x4_control0_actualSpeeds_dr_32p7G               33
#define tx_x4_control0_actualSpeeds_dr_20G_SCR             34
#define tx_x4_control0_actualSpeeds_dr_10G_HiG_DXGXS_SCR   35
#define tx_x4_control0_actualSpeeds_dr_10G_DXGXS_SCR       36
#define tx_x4_control0_actualSpeeds_dr_12G_R2              37
#define tx_x4_control0_actualSpeeds_dr_10G_X2              38
#define tx_x4_control0_actualSpeeds_dr_40G_KR4             39
#define tx_x4_control0_actualSpeeds_dr_40G_CR4             40
#define tx_x4_control0_actualSpeeds_dr_100G_CR10           41
#define tx_x4_control0_actualSpeeds_dr_15p75G_DXGXS        44
#define tx_x4_control0_actualSpeeds_dr_20G_KR2             57
#define tx_x4_control0_actualSpeeds_dr_20G_CR2             58

/****************************************************************************
 * Enums: tx_x4_control0_actualSpeedsMisc1
 */
#define tx_x4_control0_actualSpeedsMisc1_dr_2500BRCM_X1    16
#define tx_x4_control0_actualSpeedsMisc1_dr_5000BRCM_X4    17
#define tx_x4_control0_actualSpeedsMisc1_dr_6000BRCM_X4    18
#define tx_x4_control0_actualSpeedsMisc1_dr_10GHiGig_X4    19
#define tx_x4_control0_actualSpeedsMisc1_dr_10GBASE_CX4    20
#define tx_x4_control0_actualSpeedsMisc1_dr_12GHiGig_X4    21
#define tx_x4_control0_actualSpeedsMisc1_dr_12p5GHiGig_X4  22
#define tx_x4_control0_actualSpeedsMisc1_dr_13GHiGig_X4    23
#define tx_x4_control0_actualSpeedsMisc1_dr_15GHiGig_X4    24
#define tx_x4_control0_actualSpeedsMisc1_dr_16GHiGig_X4    25
#define tx_x4_control0_actualSpeedsMisc1_dr_5000BRCM_X1    26
#define tx_x4_control0_actualSpeedsMisc1_dr_6363BRCM_X1    27
#define tx_x4_control0_actualSpeedsMisc1_dr_20GHiGig_X4    28
#define tx_x4_control0_actualSpeedsMisc1_dr_21GHiGig_X4    29
#define tx_x4_control0_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define tx_x4_control0_actualSpeedsMisc1_dr_10G_HiG_DXGXS  31

/****************************************************************************
 * Enums: tx_x4_control0_IndLaneModes
 */
#define tx_x4_control0_IndLaneModes_SWSDR_div2             0
#define tx_x4_control0_IndLaneModes_SWSDR_div1             1
#define tx_x4_control0_IndLaneModes_DWSDR_div2             2
#define tx_x4_control0_IndLaneModes_DWSDR_div1             3

/****************************************************************************
 * Enums: tx_x4_control0_prbsSelect
 */
#define tx_x4_control0_prbsSelect_prbs7                    0
#define tx_x4_control0_prbsSelect_prbs15                   1
#define tx_x4_control0_prbsSelect_prbs23                   2
#define tx_x4_control0_prbsSelect_prbs31                   3

/****************************************************************************
 * Enums: tx_x4_control0_vcoDivider
 */
#define tx_x4_control0_vcoDivider_div32                    0
#define tx_x4_control0_vcoDivider_div36                    1
#define tx_x4_control0_vcoDivider_div40                    2
#define tx_x4_control0_vcoDivider_div42                    3
#define tx_x4_control0_vcoDivider_div48                    4
#define tx_x4_control0_vcoDivider_div50                    5
#define tx_x4_control0_vcoDivider_div52                    6
#define tx_x4_control0_vcoDivider_div54                    7
#define tx_x4_control0_vcoDivider_div60                    8
#define tx_x4_control0_vcoDivider_div64                    9
#define tx_x4_control0_vcoDivider_div66                    10
#define tx_x4_control0_vcoDivider_div68                    11
#define tx_x4_control0_vcoDivider_div70                    12
#define tx_x4_control0_vcoDivider_div80                    13
#define tx_x4_control0_vcoDivider_div92                    14
#define tx_x4_control0_vcoDivider_div100                   15

/****************************************************************************
 * Enums: tx_x4_control0_refClkSelect
 */
#define tx_x4_control0_refClkSelect_clk_25MHz              0
#define tx_x4_control0_refClkSelect_clk_100MHz             1
#define tx_x4_control0_refClkSelect_clk_125MHz             2
#define tx_x4_control0_refClkSelect_clk_156p25MHz          3
#define tx_x4_control0_refClkSelect_clk_187p5MHz           4
#define tx_x4_control0_refClkSelect_clk_161p25Mhz          5
#define tx_x4_control0_refClkSelect_clk_50Mhz              6
#define tx_x4_control0_refClkSelect_clk_106p25Mhz          7

/****************************************************************************
 * Enums: tx_x4_control0_aerMMDdevTypeSelect
 */
#define tx_x4_control0_aerMMDdevTypeSelect_combo_core      0
#define tx_x4_control0_aerMMDdevTypeSelect_PMA_PMD         1
#define tx_x4_control0_aerMMDdevTypeSelect_PCS             3
#define tx_x4_control0_aerMMDdevTypeSelect_PHY             4
#define tx_x4_control0_aerMMDdevTypeSelect_DTE             5
#define tx_x4_control0_aerMMDdevTypeSelect_CL73_AN         7

/****************************************************************************
 * Enums: tx_x4_control0_aerMMDportSelect
 */
#define tx_x4_control0_aerMMDportSelect_ln0                0
#define tx_x4_control0_aerMMDportSelect_ln1                1
#define tx_x4_control0_aerMMDportSelect_ln2                2
#define tx_x4_control0_aerMMDportSelect_ln3                3
#define tx_x4_control0_aerMMDportSelect_BCST_ln0_1_2_3     511
#define tx_x4_control0_aerMMDportSelect_BCST_ln0_1         512
#define tx_x4_control0_aerMMDportSelect_BCST_ln2_3         513

/****************************************************************************
 * Enums: tx_x4_control0_firmwareModeSelect
 */
#define tx_x4_control0_firmwareModeSelect_DEFAULT          0
#define tx_x4_control0_firmwareModeSelect_SFP_OPT_LR       1
#define tx_x4_control0_firmwareModeSelect_SFP_DAC          2
#define tx_x4_control0_firmwareModeSelect_XLAUI            3
#define tx_x4_control0_firmwareModeSelect_LONG_CH_6G       4

/****************************************************************************
 * Enums: tx_x4_control0_tempIdxSelect
 */
#define tx_x4_control0_tempIdxSelect_LTE__22p9C            15
#define tx_x4_control0_tempIdxSelect_LTE__12p6C            14
#define tx_x4_control0_tempIdxSelect_LTE__3p0C             13
#define tx_x4_control0_tempIdxSelect_LTE_6p7C              12
#define tx_x4_control0_tempIdxSelect_LTE_16p4C             11
#define tx_x4_control0_tempIdxSelect_LTE_26p6C             10
#define tx_x4_control0_tempIdxSelect_LTE_36p3C             9
#define tx_x4_control0_tempIdxSelect_LTE_46p0C             8
#define tx_x4_control0_tempIdxSelect_LTE_56p2C             7
#define tx_x4_control0_tempIdxSelect_LTE_65p9C             6
#define tx_x4_control0_tempIdxSelect_LTE_75p6C             5
#define tx_x4_control0_tempIdxSelect_LTE_85p3C             4
#define tx_x4_control0_tempIdxSelect_LTE_95p5C             3
#define tx_x4_control0_tempIdxSelect_LTE_105p2C            2
#define tx_x4_control0_tempIdxSelect_LTE_114p9C            1
#define tx_x4_control0_tempIdxSelect_LTE_125p1C            0

/****************************************************************************
 * Enums: tx_x4_control0_port_mode
 */
#define tx_x4_control0_port_mode_QUAD_PORT                 0
#define tx_x4_control0_port_mode_TRI_1_PORT                1
#define tx_x4_control0_port_mode_TRI_2_PORT                2
#define tx_x4_control0_port_mode_DUAL_PORT                 3
#define tx_x4_control0_port_mode_SINGLE_PORT               4

/****************************************************************************
 * Enums: tx_x4_control0_rev_letter_enum
 */
#define tx_x4_control0_rev_letter_enum_REV_A               0
#define tx_x4_control0_rev_letter_enum_REV_B               1
#define tx_x4_control0_rev_letter_enum_REV_C               2
#define tx_x4_control0_rev_letter_enum_REV_D               3

/****************************************************************************
 * Enums: tx_x4_control0_rev_number_enum
 */
#define tx_x4_control0_rev_number_enum_REV_0               0
#define tx_x4_control0_rev_number_enum_REV_1               1
#define tx_x4_control0_rev_number_enum_REV_2               2
#define tx_x4_control0_rev_number_enum_REV_3               3
#define tx_x4_control0_rev_number_enum_REV_4               4
#define tx_x4_control0_rev_number_enum_REV_5               5
#define tx_x4_control0_rev_number_enum_REV_6               6
#define tx_x4_control0_rev_number_enum_REV_7               7

/****************************************************************************
 * Enums: tx_x4_control0_bonding_enum
 */
#define tx_x4_control0_bonding_enum_WIRE_BOND              0
#define tx_x4_control0_bonding_enum_FLIP_CHIP              1

/****************************************************************************
 * Enums: tx_x4_control0_tech_process
 */
#define tx_x4_control0_tech_process_PROCESS_90NM           0
#define tx_x4_control0_tech_process_PROCESS_65NM           1
#define tx_x4_control0_tech_process_PROCESS_40NM           2
#define tx_x4_control0_tech_process_PROCESS_28NM           3

/****************************************************************************
 * Enums: tx_x4_control0_model_num
 */
#define tx_x4_control0_model_num_SERDES_CL73               0
#define tx_x4_control0_model_num_XGXS_16G                  1
#define tx_x4_control0_model_num_HYPERCORE                 2
#define tx_x4_control0_model_num_HYPERLITE                 3
#define tx_x4_control0_model_num_PCIE_G2_PIPE              4
#define tx_x4_control0_model_num_SERDES_1p25GBd            5
#define tx_x4_control0_model_num_SATA2                     6
#define tx_x4_control0_model_num_QSGMII                    7
#define tx_x4_control0_model_num_XGXS10G                   8
#define tx_x4_control0_model_num_WARPCORE                  9
#define tx_x4_control0_model_num_XFICORE                   10
#define tx_x4_control0_model_num_RXFI                      11
#define tx_x4_control0_model_num_WARPLITE                  12
#define tx_x4_control0_model_num_PENTACORE                 13
#define tx_x4_control0_model_num_ESM                       14
#define tx_x4_control0_model_num_QUAD_SGMII                15
#define tx_x4_control0_model_num_WARPCORE_3                16
#define tx_x4_control0_model_num_TSC                       17
#define tx_x4_control0_model_num_TSC4E                     18
#define tx_x4_control0_model_num_TSC12E                    19
#define tx_x4_control0_model_num_TSC4F                     20
#define tx_x4_control0_model_num_XGXS_CL73_90NM            29
#define tx_x4_control0_model_num_SERDES_CL73_90NM          30
#define tx_x4_control0_model_num_WARPCORE3                 32
#define tx_x4_control0_model_num_WARPCORE4_TSC             33
#define tx_x4_control0_model_num_RXAUI                     34

/****************************************************************************
 * Enums: tx_x4_control0_payload
 */
#define tx_x4_control0_payload_REPEAT_2_BYTES              0
#define tx_x4_control0_payload_RAMPING                     1
#define tx_x4_control0_payload_CL48_CRPAT                  2
#define tx_x4_control0_payload_CL48_CJPAT                  3
#define tx_x4_control0_payload_CL36_LONG_CRPAT             4
#define tx_x4_control0_payload_CL36_SHORT_CRPAT            5

/****************************************************************************
 * Enums: tx_x4_control0_sc
 */
#define tx_x4_control0_sc_S_10G_CR1                        0
#define tx_x4_control0_sc_S_10G_KR1                        1
#define tx_x4_control0_sc_S_10G_X1                         2
#define tx_x4_control0_sc_S_10G_HG2_CR1                    4
#define tx_x4_control0_sc_S_10G_HG2_KR1                    5
#define tx_x4_control0_sc_S_10G_HG2_X1                     6
#define tx_x4_control0_sc_S_20G_CR1                        8
#define tx_x4_control0_sc_S_20G_KR1                        9
#define tx_x4_control0_sc_S_20G_X1                         10
#define tx_x4_control0_sc_S_20G_HG2_CR1                    12
#define tx_x4_control0_sc_S_20G_HG2_KR1                    13
#define tx_x4_control0_sc_S_20G_HG2_X1                     14
#define tx_x4_control0_sc_S_25G_CR1                        16
#define tx_x4_control0_sc_S_25G_KR1                        17
#define tx_x4_control0_sc_S_25G_X1                         18
#define tx_x4_control0_sc_S_25G_HG2_CR1                    20
#define tx_x4_control0_sc_S_25G_HG2_KR1                    21
#define tx_x4_control0_sc_S_25G_HG2_X1                     22
#define tx_x4_control0_sc_S_20G_CR2                        24
#define tx_x4_control0_sc_S_20G_KR2                        25
#define tx_x4_control0_sc_S_20G_X2                         26
#define tx_x4_control0_sc_S_20G_HG2_CR2                    28
#define tx_x4_control0_sc_S_20G_HG2_KR2                    29
#define tx_x4_control0_sc_S_20G_HG2_X2                     30
#define tx_x4_control0_sc_S_40G_CR2                        32
#define tx_x4_control0_sc_S_40G_KR2                        33
#define tx_x4_control0_sc_S_40G_X2                         34
#define tx_x4_control0_sc_S_40G_HG2_CR2                    36
#define tx_x4_control0_sc_S_40G_HG2_KR2                    37
#define tx_x4_control0_sc_S_40G_HG2_X2                     38
#define tx_x4_control0_sc_S_40G_CR4                        40
#define tx_x4_control0_sc_S_40G_KR4                        41
#define tx_x4_control0_sc_S_40G_X4                         42
#define tx_x4_control0_sc_S_40G_HG2_CR4                    44
#define tx_x4_control0_sc_S_40G_HG2_KR4                    45
#define tx_x4_control0_sc_S_40G_HG2_X4                     46
#define tx_x4_control0_sc_S_50G_CR2                        48
#define tx_x4_control0_sc_S_50G_KR2                        49
#define tx_x4_control0_sc_S_50G_X2                         50
#define tx_x4_control0_sc_S_50G_HG2_CR2                    52
#define tx_x4_control0_sc_S_50G_HG2_KR2                    53
#define tx_x4_control0_sc_S_50G_HG2_X2                     54
#define tx_x4_control0_sc_S_50G_CR4                        56
#define tx_x4_control0_sc_S_50G_KR4                        57
#define tx_x4_control0_sc_S_50G_X4                         58
#define tx_x4_control0_sc_S_50G_HG2_CR4                    60
#define tx_x4_control0_sc_S_50G_HG2_KR4                    61
#define tx_x4_control0_sc_S_50G_HG2_X4                     62
#define tx_x4_control0_sc_S_100G_CR4                       64
#define tx_x4_control0_sc_S_100G_KR4                       65
#define tx_x4_control0_sc_S_100G_X4                        66
#define tx_x4_control0_sc_S_100G_HG2_CR4                   68
#define tx_x4_control0_sc_S_100G_HG2_KR4                   69
#define tx_x4_control0_sc_S_100G_HG2_X4                    70
#define tx_x4_control0_sc_S_CL73_20GVCO                    72
#define tx_x4_control0_sc_S_CL73_25GVCO                    80
#define tx_x4_control0_sc_S_CL36_20GVCO                    88
#define tx_x4_control0_sc_S_CL36_25GVCO                    96

/****************************************************************************
 * Enums: tx_x4_control0_t_fifo_modes
 */
#define tx_x4_control0_t_fifo_modes_T_FIFO_MODE_BYPASS     0
#define tx_x4_control0_t_fifo_modes_T_FIFO_MODE_40G        1
#define tx_x4_control0_t_fifo_modes_T_FIFO_MODE_100G       2
#define tx_x4_control0_t_fifo_modes_T_FIFO_MODE_20G        3

/****************************************************************************
 * Enums: tx_x4_control0_t_enc_modes
 */
#define tx_x4_control0_t_enc_modes_T_ENC_MODE_BYPASS       0
#define tx_x4_control0_t_enc_modes_T_ENC_MODE_CL49         1
#define tx_x4_control0_t_enc_modes_T_ENC_MODE_CL82         2

/****************************************************************************
 * Enums: tx_x4_control0_btmx_mode
 */
#define tx_x4_control0_btmx_mode_BS_BTMX_MODE_1to1         0
#define tx_x4_control0_btmx_mode_BS_BTMX_MODE_2to1         1
#define tx_x4_control0_btmx_mode_BS_BTMX_MODE_5to1         2

/****************************************************************************
 * Enums: tx_x4_control0_t_type_cl82
 */
#define tx_x4_control0_t_type_cl82_T_TYPE_B1               5
#define tx_x4_control0_t_type_cl82_T_TYPE_C                4
#define tx_x4_control0_t_type_cl82_T_TYPE_S                3
#define tx_x4_control0_t_type_cl82_T_TYPE_T                2
#define tx_x4_control0_t_type_cl82_T_TYPE_D                1
#define tx_x4_control0_t_type_cl82_T_TYPE_E                0

/****************************************************************************
 * Enums: tx_x4_control0_txsm_state_cl82
 */
#define tx_x4_control0_txsm_state_cl82_TX_HIG_END          6
#define tx_x4_control0_txsm_state_cl82_TX_HIG_START        5
#define tx_x4_control0_txsm_state_cl82_TX_E                4
#define tx_x4_control0_txsm_state_cl82_TX_T                3
#define tx_x4_control0_txsm_state_cl82_TX_D                2
#define tx_x4_control0_txsm_state_cl82_TX_C                1
#define tx_x4_control0_txsm_state_cl82_TX_INIT             0

/****************************************************************************
 * Enums: tx_x4_control0_ltxsm_state_cl82
 */
#define tx_x4_control0_ltxsm_state_cl82_TX_HIG_END         64
#define tx_x4_control0_ltxsm_state_cl82_TX_HIG_START       32
#define tx_x4_control0_ltxsm_state_cl82_TX_E               16
#define tx_x4_control0_ltxsm_state_cl82_TX_T               8
#define tx_x4_control0_ltxsm_state_cl82_TX_D               4
#define tx_x4_control0_ltxsm_state_cl82_TX_C               2
#define tx_x4_control0_ltxsm_state_cl82_TX_INIT            1

/****************************************************************************
 * Enums: tx_x4_control0_r_type_coded_cl82
 */
#define tx_x4_control0_r_type_coded_cl82_R_TYPE_B1         32
#define tx_x4_control0_r_type_coded_cl82_R_TYPE_C          16
#define tx_x4_control0_r_type_coded_cl82_R_TYPE_S          8
#define tx_x4_control0_r_type_coded_cl82_R_TYPE_T          4
#define tx_x4_control0_r_type_coded_cl82_R_TYPE_D          2
#define tx_x4_control0_r_type_coded_cl82_R_TYPE_E          1

/****************************************************************************
 * Enums: tx_x4_control0_rxsm_state_cl82
 */
#define tx_x4_control0_rxsm_state_cl82_RX_HIG_END          64
#define tx_x4_control0_rxsm_state_cl82_RX_HIG_START        32
#define tx_x4_control0_rxsm_state_cl82_RX_E                16
#define tx_x4_control0_rxsm_state_cl82_RX_T                8
#define tx_x4_control0_rxsm_state_cl82_RX_D                4
#define tx_x4_control0_rxsm_state_cl82_RX_C                2
#define tx_x4_control0_rxsm_state_cl82_RX_INIT             1

/****************************************************************************
 * Enums: tx_x4_control0_deskew_state
 */
#define tx_x4_control0_deskew_state_ALIGN_ACQUIRED         2
#define tx_x4_control0_deskew_state_LOSS_OF_ALIGNMENT      1

/****************************************************************************
 * Enums: tx_x4_control0_os_mode_enum
 */
#define tx_x4_control0_os_mode_enum_OS_MODE_1              0
#define tx_x4_control0_os_mode_enum_OS_MODE_2              1
#define tx_x4_control0_os_mode_enum_OS_MODE_4              2
#define tx_x4_control0_os_mode_enum_OS_MODE_16p5           8
#define tx_x4_control0_os_mode_enum_OS_MODE_20p625         12

/****************************************************************************
 * Enums: tx_x4_control0_scr_modes
 */
#define tx_x4_control0_scr_modes_T_SCR_MODE_BYPASS         0
#define tx_x4_control0_scr_modes_T_SCR_MODE_CL49           1
#define tx_x4_control0_scr_modes_T_SCR_MODE_40G_2_LANE     2
#define tx_x4_control0_scr_modes_T_SCR_MODE_100G           3
#define tx_x4_control0_scr_modes_T_SCR_MODE_20G            4
#define tx_x4_control0_scr_modes_T_SCR_MODE_40G_4_LANE     5

/****************************************************************************
 * Enums: tx_x4_control0_descr_modes
 */
#define tx_x4_control0_descr_modes_R_DESCR_MODE_BYPASS     0
#define tx_x4_control0_descr_modes_R_DESCR_MODE_CL49       1
#define tx_x4_control0_descr_modes_R_DESCR_MODE_CL82       2

/****************************************************************************
 * Enums: tx_x4_control0_r_dec_tl_mode
 */
#define tx_x4_control0_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS  0
#define tx_x4_control0_r_dec_tl_mode_R_DEC_TL_MODE_CL49    1
#define tx_x4_control0_r_dec_tl_mode_R_DEC_TL_MODE_CL82    2

/****************************************************************************
 * Enums: tx_x4_control0_r_dec_fsm_mode
 */
#define tx_x4_control0_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define tx_x4_control0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49  1
#define tx_x4_control0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82  2

/****************************************************************************
 * Enums: tx_x4_control0_r_deskew_mode
 */
#define tx_x4_control0_r_deskew_mode_R_DESKEW_MODE_BYPASS  0
#define tx_x4_control0_r_deskew_mode_R_DESKEW_MODE_20G     1
#define tx_x4_control0_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define tx_x4_control0_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define tx_x4_control0_r_deskew_mode_R_DESKEW_MODE_100G    4
#define tx_x4_control0_r_deskew_mode_R_DESKEW_MODE_CL49    5
#define tx_x4_control0_r_deskew_mode_R_DESKEW_MODE_CL91    6

/****************************************************************************
 * Enums: tx_x4_control0_bs_dist_modes
 */
#define tx_x4_control0_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define tx_x4_control0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define tx_x4_control0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define tx_x4_control0_bs_dist_modes_BS_DIST_MODE_NO_TDM   3

/****************************************************************************
 * Enums: tx_x4_control0_cl49_t_type
 */
#define tx_x4_control0_cl49_t_type_BAD_T_TYPE              15
#define tx_x4_control0_cl49_t_type_T_TYPE_B0               11
#define tx_x4_control0_cl49_t_type_T_TYPE_OB               10
#define tx_x4_control0_cl49_t_type_T_TYPE_B1               9
#define tx_x4_control0_cl49_t_type_T_TYPE_DB               8
#define tx_x4_control0_cl49_t_type_T_TYPE_FC               7
#define tx_x4_control0_cl49_t_type_T_TYPE_TB               6
#define tx_x4_control0_cl49_t_type_T_TYPE_LI               5
#define tx_x4_control0_cl49_t_type_T_TYPE_C                4
#define tx_x4_control0_cl49_t_type_T_TYPE_S                3
#define tx_x4_control0_cl49_t_type_T_TYPE_T                2
#define tx_x4_control0_cl49_t_type_T_TYPE_D                1
#define tx_x4_control0_cl49_t_type_T_TYPE_E                0

/****************************************************************************
 * Enums: tx_x4_control0_cl49_txsm_states
 */
#define tx_x4_control0_cl49_txsm_states_TX_HIG_END         7
#define tx_x4_control0_cl49_txsm_states_TX_HIG_START       6
#define tx_x4_control0_cl49_txsm_states_TX_LI              5
#define tx_x4_control0_cl49_txsm_states_TX_E               4
#define tx_x4_control0_cl49_txsm_states_TX_T               3
#define tx_x4_control0_cl49_txsm_states_TX_D               2
#define tx_x4_control0_cl49_txsm_states_TX_C               1
#define tx_x4_control0_cl49_txsm_states_TX_INIT            0

/****************************************************************************
 * Enums: tx_x4_control0_cl49_ltxsm_states
 */
#define tx_x4_control0_cl49_ltxsm_states_TX_HIG_END        128
#define tx_x4_control0_cl49_ltxsm_states_TX_HIG_START      64
#define tx_x4_control0_cl49_ltxsm_states_TX_LI             32
#define tx_x4_control0_cl49_ltxsm_states_TX_E              16
#define tx_x4_control0_cl49_ltxsm_states_TX_T              8
#define tx_x4_control0_cl49_ltxsm_states_TX_D              4
#define tx_x4_control0_cl49_ltxsm_states_TX_C              2
#define tx_x4_control0_cl49_ltxsm_states_TX_INIT           1

/****************************************************************************
 * Enums: tx_x4_control0_burst_error_mode
 */
#define tx_x4_control0_burst_error_mode_BURST_ERROR_11_BITS 0
#define tx_x4_control0_burst_error_mode_BURST_ERROR_16_BITS 1
#define tx_x4_control0_burst_error_mode_BURST_ERROR_17_BITS 2
#define tx_x4_control0_burst_error_mode_BURST_ERROR_18_BITS 3
#define tx_x4_control0_burst_error_mode_BURST_ERROR_19_BITS 4
#define tx_x4_control0_burst_error_mode_BURST_ERROR_20_BITS 5
#define tx_x4_control0_burst_error_mode_BURST_ERROR_21_BITS 6
#define tx_x4_control0_burst_error_mode_BURST_ERROR_22_BITS 7
#define tx_x4_control0_burst_error_mode_BURST_ERROR_23_BITS 8
#define tx_x4_control0_burst_error_mode_BURST_ERROR_24_BITS 9
#define tx_x4_control0_burst_error_mode_BURST_ERROR_25_BITS 10
#define tx_x4_control0_burst_error_mode_BURST_ERROR_26_BITS 11
#define tx_x4_control0_burst_error_mode_BURST_ERROR_27_BITS 12
#define tx_x4_control0_burst_error_mode_BURST_ERROR_28_BITS 13
#define tx_x4_control0_burst_error_mode_BURST_ERROR_29_BITS 14
#define tx_x4_control0_burst_error_mode_BURST_ERROR_30_BITS 15
#define tx_x4_control0_burst_error_mode_BURST_ERROR_31_BITS 16
#define tx_x4_control0_burst_error_mode_BURST_ERROR_32_BITS 17
#define tx_x4_control0_burst_error_mode_BURST_ERROR_33_BITS 18
#define tx_x4_control0_burst_error_mode_BURST_ERROR_34_BITS 19
#define tx_x4_control0_burst_error_mode_BURST_ERROR_35_BITS 20
#define tx_x4_control0_burst_error_mode_BURST_ERROR_36_BITS 21
#define tx_x4_control0_burst_error_mode_BURST_ERROR_37_BITS 22
#define tx_x4_control0_burst_error_mode_BURST_ERROR_38_BITS 23
#define tx_x4_control0_burst_error_mode_BURST_ERROR_39_BITS 24
#define tx_x4_control0_burst_error_mode_BURST_ERROR_40_BITS 25
#define tx_x4_control0_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: tx_x4_control0_bermon_state
 */
#define tx_x4_control0_bermon_state_HI_BER                 4
#define tx_x4_control0_bermon_state_GOOD_BER               3
#define tx_x4_control0_bermon_state_BER_TEST_SH            2
#define tx_x4_control0_bermon_state_START_TIMER            1
#define tx_x4_control0_bermon_state_BER_MT_INIT            0

/****************************************************************************
 * Enums: tx_x4_control0_rxsm_state_cl49
 */
#define tx_x4_control0_rxsm_state_cl49_RX_HIG_END          128
#define tx_x4_control0_rxsm_state_cl49_RX_HIG_START        64
#define tx_x4_control0_rxsm_state_cl49_RX_LI               32
#define tx_x4_control0_rxsm_state_cl49_RX_E                16
#define tx_x4_control0_rxsm_state_cl49_RX_T                8
#define tx_x4_control0_rxsm_state_cl49_RX_D                4
#define tx_x4_control0_rxsm_state_cl49_RX_C                2
#define tx_x4_control0_rxsm_state_cl49_RX_INIT             1

/****************************************************************************
 * Enums: tx_x4_control0_r_type
 */
#define tx_x4_control0_r_type_BAD_R_TYPE                   15
#define tx_x4_control0_r_type_R_TYPE_B0                    11
#define tx_x4_control0_r_type_R_TYPE_OB                    10
#define tx_x4_control0_r_type_R_TYPE_B1                    9
#define tx_x4_control0_r_type_R_TYPE_DB                    8
#define tx_x4_control0_r_type_R_TYPE_FC                    7
#define tx_x4_control0_r_type_R_TYPE_TB                    6
#define tx_x4_control0_r_type_R_TYPE_LI                    5
#define tx_x4_control0_r_type_R_TYPE_C                     4
#define tx_x4_control0_r_type_R_TYPE_S                     3
#define tx_x4_control0_r_type_R_TYPE_T                     2
#define tx_x4_control0_r_type_R_TYPE_D                     1
#define tx_x4_control0_r_type_R_TYPE_E                     0

/****************************************************************************
 * Enums: tx_x4_control0_am_lock_state
 */
#define tx_x4_control0_am_lock_state_INVALID_AM            512
#define tx_x4_control0_am_lock_state_GOOD_AM               256
#define tx_x4_control0_am_lock_state_COMP_AM               128
#define tx_x4_control0_am_lock_state_TIMER_2               64
#define tx_x4_control0_am_lock_state_AM_2_GOOD             32
#define tx_x4_control0_am_lock_state_COMP_2ND              16
#define tx_x4_control0_am_lock_state_TIMER_1               8
#define tx_x4_control0_am_lock_state_FIND_1ST              4
#define tx_x4_control0_am_lock_state_AM_RESET_CNT          2
#define tx_x4_control0_am_lock_state_AM_LOCK_INIT          1

/****************************************************************************
 * Enums: tx_x4_control0_msg_selector
 */
#define tx_x4_control0_msg_selector_RESERVED               0
#define tx_x4_control0_msg_selector_VALUE_802p3            1
#define tx_x4_control0_msg_selector_VALUE_802p9            2
#define tx_x4_control0_msg_selector_VALUE_802p5            3
#define tx_x4_control0_msg_selector_VALUE_1394             4

/****************************************************************************
 * Enums: tx_x4_control0_synce_enum
 */
#define tx_x4_control0_synce_enum_SYNCE_NO_DIV             0
#define tx_x4_control0_synce_enum_SYNCE_DIV_7              1
#define tx_x4_control0_synce_enum_SYNCE_DIV_11             2

/****************************************************************************
 * Enums: tx_x4_control0_synce_enum_stage0
 */
#define tx_x4_control0_synce_enum_stage0_SYNCE_NO_DIV      0
#define tx_x4_control0_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define tx_x4_control0_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: tx_x4_control0_cl91_sync_state
 */
#define tx_x4_control0_cl91_sync_state_FIND_1ST            0
#define tx_x4_control0_cl91_sync_state_COUNT_NEXT          1
#define tx_x4_control0_cl91_sync_state_COMP_2ND            2
#define tx_x4_control0_cl91_sync_state_TWO_GOOD            3

/****************************************************************************
 * Enums: tx_x4_control0_cl91_algn_state
 */
#define tx_x4_control0_cl91_algn_state_LOSS_OF_ALIGNMENT   0
#define tx_x4_control0_cl91_algn_state_DESKEW              1
#define tx_x4_control0_cl91_algn_state_DESKEW_FAIL         2
#define tx_x4_control0_cl91_algn_state_ALIGN_ACQUIRED      3
#define tx_x4_control0_cl91_algn_state_CW_GOOD             4
#define tx_x4_control0_cl91_algn_state_CW_BAD              5
#define tx_x4_control0_cl91_algn_state_THREE_BAD           6

/****************************************************************************
 * Enums: tx_x4_control0_fec_sel
 */
#define tx_x4_control0_fec_sel_NO_FEC                      0
#define tx_x4_control0_fec_sel_FEC_CL74                    1
#define tx_x4_control0_fec_sel_FEC_CL91                    2

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl36TxEEEStates_l
 */
#define tx_x4_credit_gen0_cl36TxEEEStates_l_TX_REFRESH     8
#define tx_x4_credit_gen0_cl36TxEEEStates_l_TX_QUIET       4
#define tx_x4_credit_gen0_cl36TxEEEStates_l_TX_SLEEP       2
#define tx_x4_credit_gen0_cl36TxEEEStates_l_TX_ACTIVE      1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl36TxEEEStates_c
 */
#define tx_x4_credit_gen0_cl36TxEEEStates_c_TX_REFRESH     3
#define tx_x4_credit_gen0_cl36TxEEEStates_c_TX_QUIET       2
#define tx_x4_credit_gen0_cl36TxEEEStates_c_TX_SLEEP       1
#define tx_x4_credit_gen0_cl36TxEEEStates_c_TX_ACTIVE      0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl49TxEEEStates_l
 */
#define tx_x4_credit_gen0_cl49TxEEEStates_l_SCR_RESET_2    64
#define tx_x4_credit_gen0_cl49TxEEEStates_l_SCR_RESET_1    32
#define tx_x4_credit_gen0_cl49TxEEEStates_l_TX_WAKE        16
#define tx_x4_credit_gen0_cl49TxEEEStates_l_TX_REFRESH     8
#define tx_x4_credit_gen0_cl49TxEEEStates_l_TX_QUIET       4
#define tx_x4_credit_gen0_cl49TxEEEStates_l_TX_SLEEP       2
#define tx_x4_credit_gen0_cl49TxEEEStates_l_TX_ACTIVE      1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_fec_req_enum
 */
#define tx_x4_credit_gen0_fec_req_enum_FEC_not_supported   0
#define tx_x4_credit_gen0_fec_req_enum_FEC_supported_but_not_requested 1
#define tx_x4_credit_gen0_fec_req_enum_invalid_setting     2
#define tx_x4_credit_gen0_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl73_pause_enum
 */
#define tx_x4_credit_gen0_cl73_pause_enum_No_PAUSE_ability 0
#define tx_x4_credit_gen0_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define tx_x4_credit_gen0_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define tx_x4_credit_gen0_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl49TxEEEStates_c
 */
#define tx_x4_credit_gen0_cl49TxEEEStates_c_SCR_RESET_2    6
#define tx_x4_credit_gen0_cl49TxEEEStates_c_SCR_RESET_1    5
#define tx_x4_credit_gen0_cl49TxEEEStates_c_TX_WAKE        4
#define tx_x4_credit_gen0_cl49TxEEEStates_c_TX_REFRESH     3
#define tx_x4_credit_gen0_cl49TxEEEStates_c_TX_QUIET       2
#define tx_x4_credit_gen0_cl49TxEEEStates_c_TX_SLEEP       1
#define tx_x4_credit_gen0_cl49TxEEEStates_c_TX_ACTIVE      0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl36RxEEEStates_l
 */
#define tx_x4_credit_gen0_cl36RxEEEStates_l_RX_LINK_FAIL   32
#define tx_x4_credit_gen0_cl36RxEEEStates_l_RX_WTF         16
#define tx_x4_credit_gen0_cl36RxEEEStates_l_RX_WAKE        8
#define tx_x4_credit_gen0_cl36RxEEEStates_l_RX_QUIET       4
#define tx_x4_credit_gen0_cl36RxEEEStates_l_RX_SLEEP       2
#define tx_x4_credit_gen0_cl36RxEEEStates_l_RX_ACTIVE      1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl36RxEEEStates_c
 */
#define tx_x4_credit_gen0_cl36RxEEEStates_c_RX_LINK_FAIL   5
#define tx_x4_credit_gen0_cl36RxEEEStates_c_RX_WTF         4
#define tx_x4_credit_gen0_cl36RxEEEStates_c_RX_WAKE        3
#define tx_x4_credit_gen0_cl36RxEEEStates_c_RX_QUIET       2
#define tx_x4_credit_gen0_cl36RxEEEStates_c_RX_SLEEP       1
#define tx_x4_credit_gen0_cl36RxEEEStates_c_RX_ACTIVE      0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl49RxEEEStates_l
 */
#define tx_x4_credit_gen0_cl49RxEEEStates_l_RX_LINK_FAIL   32
#define tx_x4_credit_gen0_cl49RxEEEStates_l_RX_WTF         16
#define tx_x4_credit_gen0_cl49RxEEEStates_l_RX_WAKE        8
#define tx_x4_credit_gen0_cl49RxEEEStates_l_RX_QUIET       4
#define tx_x4_credit_gen0_cl49RxEEEStates_l_RX_SLEEP       2
#define tx_x4_credit_gen0_cl49RxEEEStates_l_RX_ACTIVE      1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl49RxEEEStates_c
 */
#define tx_x4_credit_gen0_cl49RxEEEStates_c_RX_LINK_FAIL   5
#define tx_x4_credit_gen0_cl49RxEEEStates_c_RX_WTF         4
#define tx_x4_credit_gen0_cl49RxEEEStates_c_RX_WAKE        3
#define tx_x4_credit_gen0_cl49RxEEEStates_c_RX_QUIET       2
#define tx_x4_credit_gen0_cl49RxEEEStates_c_RX_SLEEP       1
#define tx_x4_credit_gen0_cl49RxEEEStates_c_RX_ACTIVE      0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl48TxEEEStates_l
 */
#define tx_x4_credit_gen0_cl48TxEEEStates_l_TX_REFRESH     8
#define tx_x4_credit_gen0_cl48TxEEEStates_l_TX_QUIET       4
#define tx_x4_credit_gen0_cl48TxEEEStates_l_TX_SLEEP       2
#define tx_x4_credit_gen0_cl48TxEEEStates_l_TX_ACTIVE      1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl48TxEEEStates_c
 */
#define tx_x4_credit_gen0_cl48TxEEEStates_c_TX_REFRESH     3
#define tx_x4_credit_gen0_cl48TxEEEStates_c_TX_QUIET       2
#define tx_x4_credit_gen0_cl48TxEEEStates_c_TX_SLEEP       1
#define tx_x4_credit_gen0_cl48TxEEEStates_c_TX_ACTIVE      0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl48RxEEEStates_l
 */
#define tx_x4_credit_gen0_cl48RxEEEStates_l_RX_LINK_FAIL   32
#define tx_x4_credit_gen0_cl48RxEEEStates_l_RX_WAKE        16
#define tx_x4_credit_gen0_cl48RxEEEStates_l_RX_QUIET       8
#define tx_x4_credit_gen0_cl48RxEEEStates_l_RX_DEACT       4
#define tx_x4_credit_gen0_cl48RxEEEStates_l_RX_SLEEP       2
#define tx_x4_credit_gen0_cl48RxEEEStates_l_RX_ACTIVE      1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl48RxEEEStates_c
 */
#define tx_x4_credit_gen0_cl48RxEEEStates_c_RX_LINK_FAIL   5
#define tx_x4_credit_gen0_cl48RxEEEStates_c_RX_WAKE        4
#define tx_x4_credit_gen0_cl48RxEEEStates_c_RX_QUIET       3
#define tx_x4_credit_gen0_cl48RxEEEStates_c_RX_DEACT       2
#define tx_x4_credit_gen0_cl48RxEEEStates_c_RX_SLEEP       1
#define tx_x4_credit_gen0_cl48RxEEEStates_c_RX_ACTIVE      0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_IQP_Options
 */
#define tx_x4_credit_gen0_IQP_Options_i50uA                0
#define tx_x4_credit_gen0_IQP_Options_i100uA               1
#define tx_x4_credit_gen0_IQP_Options_i150uA               2
#define tx_x4_credit_gen0_IQP_Options_i200uA               3
#define tx_x4_credit_gen0_IQP_Options_i250uA               4
#define tx_x4_credit_gen0_IQP_Options_i300uA               5
#define tx_x4_credit_gen0_IQP_Options_i350uA               6
#define tx_x4_credit_gen0_IQP_Options_i400uA               7
#define tx_x4_credit_gen0_IQP_Options_i450uA               8
#define tx_x4_credit_gen0_IQP_Options_i500uA               9
#define tx_x4_credit_gen0_IQP_Options_i550uA               10
#define tx_x4_credit_gen0_IQP_Options_i600uA               11
#define tx_x4_credit_gen0_IQP_Options_i650uA               12
#define tx_x4_credit_gen0_IQP_Options_i700uA               13
#define tx_x4_credit_gen0_IQP_Options_i750uA               14
#define tx_x4_credit_gen0_IQP_Options_i800uA               15

/****************************************************************************
 * Enums: tx_x4_credit_gen0_IDriver_Options
 */
#define tx_x4_credit_gen0_IDriver_Options_v680mV           0
#define tx_x4_credit_gen0_IDriver_Options_v730mV           1
#define tx_x4_credit_gen0_IDriver_Options_v780mV           2
#define tx_x4_credit_gen0_IDriver_Options_v830mV           3
#define tx_x4_credit_gen0_IDriver_Options_v880mV           4
#define tx_x4_credit_gen0_IDriver_Options_v930mV           5
#define tx_x4_credit_gen0_IDriver_Options_v980mV           6
#define tx_x4_credit_gen0_IDriver_Options_v1010mV          7
#define tx_x4_credit_gen0_IDriver_Options_v1040mV          8
#define tx_x4_credit_gen0_IDriver_Options_v1060mV          9
#define tx_x4_credit_gen0_IDriver_Options_v1070mV          10
#define tx_x4_credit_gen0_IDriver_Options_v1080mV          11
#define tx_x4_credit_gen0_IDriver_Options_v1085mV          12
#define tx_x4_credit_gen0_IDriver_Options_v1090mV          13
#define tx_x4_credit_gen0_IDriver_Options_v1095mV          14
#define tx_x4_credit_gen0_IDriver_Options_v1100mV          15

/****************************************************************************
 * Enums: tx_x4_credit_gen0_operationModes
 */
#define tx_x4_credit_gen0_operationModes_XGXS              0
#define tx_x4_credit_gen0_operationModes_XGXG_nCC          1
#define tx_x4_credit_gen0_operationModes_Indlane_OS8       4
#define tx_x4_credit_gen0_operationModes_IndLane_OS5       5
#define tx_x4_credit_gen0_operationModes_PCI               7
#define tx_x4_credit_gen0_operationModes_XGXS_nLQ          8
#define tx_x4_credit_gen0_operationModes_XGXS_nLQnCC       9
#define tx_x4_credit_gen0_operationModes_PBypass           10
#define tx_x4_credit_gen0_operationModes_PBypass_nDSK      11
#define tx_x4_credit_gen0_operationModes_ComboCoreMode     12
#define tx_x4_credit_gen0_operationModes_Clocks_off        15

/****************************************************************************
 * Enums: tx_x4_credit_gen0_actualSpeeds
 */
#define tx_x4_credit_gen0_actualSpeeds_dr_10M              0
#define tx_x4_credit_gen0_actualSpeeds_dr_100M             1
#define tx_x4_credit_gen0_actualSpeeds_dr_1G               2
#define tx_x4_credit_gen0_actualSpeeds_dr_2p5G             3
#define tx_x4_credit_gen0_actualSpeeds_dr_5G_X4            4
#define tx_x4_credit_gen0_actualSpeeds_dr_6G_X4            5
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_HiG          6
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_CX4          7
#define tx_x4_credit_gen0_actualSpeeds_dr_12G_HiG          8
#define tx_x4_credit_gen0_actualSpeeds_dr_12p5G_X4         9
#define tx_x4_credit_gen0_actualSpeeds_dr_13G_X4           10
#define tx_x4_credit_gen0_actualSpeeds_dr_15G_X4           11
#define tx_x4_credit_gen0_actualSpeeds_dr_16G_X4           12
#define tx_x4_credit_gen0_actualSpeeds_dr_1G_KX            13
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_KX4          14
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_KR           15
#define tx_x4_credit_gen0_actualSpeeds_dr_5G               16
#define tx_x4_credit_gen0_actualSpeeds_dr_6p4G             17
#define tx_x4_credit_gen0_actualSpeeds_dr_20G_X4           18
#define tx_x4_credit_gen0_actualSpeeds_dr_21G_X4           19
#define tx_x4_credit_gen0_actualSpeeds_dr_25G_X4           20
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_HiG_DXGXS    21
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_DXGXS        22
#define tx_x4_credit_gen0_actualSpeeds_dr_10p5G_HiG_DXGXS  23
#define tx_x4_credit_gen0_actualSpeeds_dr_10p5G_DXGXS      24
#define tx_x4_credit_gen0_actualSpeeds_dr_12p773G_HiG_DXGXS 25
#define tx_x4_credit_gen0_actualSpeeds_dr_12p773G_DXGXS    26
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_XFI          27
#define tx_x4_credit_gen0_actualSpeeds_dr_40G              28
#define tx_x4_credit_gen0_actualSpeeds_dr_20G_HiG_DXGXS    29
#define tx_x4_credit_gen0_actualSpeeds_dr_20G_DXGXS        30
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_SFI          31
#define tx_x4_credit_gen0_actualSpeeds_dr_31p5G            32
#define tx_x4_credit_gen0_actualSpeeds_dr_32p7G            33
#define tx_x4_credit_gen0_actualSpeeds_dr_20G_SCR          34
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_HiG_DXGXS_SCR 35
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_DXGXS_SCR    36
#define tx_x4_credit_gen0_actualSpeeds_dr_12G_R2           37
#define tx_x4_credit_gen0_actualSpeeds_dr_10G_X2           38
#define tx_x4_credit_gen0_actualSpeeds_dr_40G_KR4          39
#define tx_x4_credit_gen0_actualSpeeds_dr_40G_CR4          40
#define tx_x4_credit_gen0_actualSpeeds_dr_100G_CR10        41
#define tx_x4_credit_gen0_actualSpeeds_dr_15p75G_DXGXS     44
#define tx_x4_credit_gen0_actualSpeeds_dr_20G_KR2          57
#define tx_x4_credit_gen0_actualSpeeds_dr_20G_CR2          58

/****************************************************************************
 * Enums: tx_x4_credit_gen0_actualSpeedsMisc1
 */
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_2500BRCM_X1 16
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_5000BRCM_X4 17
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_6000BRCM_X4 18
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_10GHiGig_X4 19
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_10GBASE_CX4 20
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_12GHiGig_X4 21
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_12p5GHiGig_X4 22
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_13GHiGig_X4 23
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_15GHiGig_X4 24
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_16GHiGig_X4 25
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_5000BRCM_X1 26
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_6363BRCM_X1 27
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_20GHiGig_X4 28
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_21GHiGig_X4 29
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_25p45GHiGig_X4 30
#define tx_x4_credit_gen0_actualSpeedsMisc1_dr_10G_HiG_DXGXS 31

/****************************************************************************
 * Enums: tx_x4_credit_gen0_IndLaneModes
 */
#define tx_x4_credit_gen0_IndLaneModes_SWSDR_div2          0
#define tx_x4_credit_gen0_IndLaneModes_SWSDR_div1          1
#define tx_x4_credit_gen0_IndLaneModes_DWSDR_div2          2
#define tx_x4_credit_gen0_IndLaneModes_DWSDR_div1          3

/****************************************************************************
 * Enums: tx_x4_credit_gen0_prbsSelect
 */
#define tx_x4_credit_gen0_prbsSelect_prbs7                 0
#define tx_x4_credit_gen0_prbsSelect_prbs15                1
#define tx_x4_credit_gen0_prbsSelect_prbs23                2
#define tx_x4_credit_gen0_prbsSelect_prbs31                3

/****************************************************************************
 * Enums: tx_x4_credit_gen0_vcoDivider
 */
#define tx_x4_credit_gen0_vcoDivider_div32                 0
#define tx_x4_credit_gen0_vcoDivider_div36                 1
#define tx_x4_credit_gen0_vcoDivider_div40                 2
#define tx_x4_credit_gen0_vcoDivider_div42                 3
#define tx_x4_credit_gen0_vcoDivider_div48                 4
#define tx_x4_credit_gen0_vcoDivider_div50                 5
#define tx_x4_credit_gen0_vcoDivider_div52                 6
#define tx_x4_credit_gen0_vcoDivider_div54                 7
#define tx_x4_credit_gen0_vcoDivider_div60                 8
#define tx_x4_credit_gen0_vcoDivider_div64                 9
#define tx_x4_credit_gen0_vcoDivider_div66                 10
#define tx_x4_credit_gen0_vcoDivider_div68                 11
#define tx_x4_credit_gen0_vcoDivider_div70                 12
#define tx_x4_credit_gen0_vcoDivider_div80                 13
#define tx_x4_credit_gen0_vcoDivider_div92                 14
#define tx_x4_credit_gen0_vcoDivider_div100                15

/****************************************************************************
 * Enums: tx_x4_credit_gen0_refClkSelect
 */
#define tx_x4_credit_gen0_refClkSelect_clk_25MHz           0
#define tx_x4_credit_gen0_refClkSelect_clk_100MHz          1
#define tx_x4_credit_gen0_refClkSelect_clk_125MHz          2
#define tx_x4_credit_gen0_refClkSelect_clk_156p25MHz       3
#define tx_x4_credit_gen0_refClkSelect_clk_187p5MHz        4
#define tx_x4_credit_gen0_refClkSelect_clk_161p25Mhz       5
#define tx_x4_credit_gen0_refClkSelect_clk_50Mhz           6
#define tx_x4_credit_gen0_refClkSelect_clk_106p25Mhz       7

/****************************************************************************
 * Enums: tx_x4_credit_gen0_aerMMDdevTypeSelect
 */
#define tx_x4_credit_gen0_aerMMDdevTypeSelect_combo_core   0
#define tx_x4_credit_gen0_aerMMDdevTypeSelect_PMA_PMD      1
#define tx_x4_credit_gen0_aerMMDdevTypeSelect_PCS          3
#define tx_x4_credit_gen0_aerMMDdevTypeSelect_PHY          4
#define tx_x4_credit_gen0_aerMMDdevTypeSelect_DTE          5
#define tx_x4_credit_gen0_aerMMDdevTypeSelect_CL73_AN      7

/****************************************************************************
 * Enums: tx_x4_credit_gen0_aerMMDportSelect
 */
#define tx_x4_credit_gen0_aerMMDportSelect_ln0             0
#define tx_x4_credit_gen0_aerMMDportSelect_ln1             1
#define tx_x4_credit_gen0_aerMMDportSelect_ln2             2
#define tx_x4_credit_gen0_aerMMDportSelect_ln3             3
#define tx_x4_credit_gen0_aerMMDportSelect_BCST_ln0_1_2_3  511
#define tx_x4_credit_gen0_aerMMDportSelect_BCST_ln0_1      512
#define tx_x4_credit_gen0_aerMMDportSelect_BCST_ln2_3      513

/****************************************************************************
 * Enums: tx_x4_credit_gen0_firmwareModeSelect
 */
#define tx_x4_credit_gen0_firmwareModeSelect_DEFAULT       0
#define tx_x4_credit_gen0_firmwareModeSelect_SFP_OPT_LR    1
#define tx_x4_credit_gen0_firmwareModeSelect_SFP_DAC       2
#define tx_x4_credit_gen0_firmwareModeSelect_XLAUI         3
#define tx_x4_credit_gen0_firmwareModeSelect_LONG_CH_6G    4

/****************************************************************************
 * Enums: tx_x4_credit_gen0_tempIdxSelect
 */
#define tx_x4_credit_gen0_tempIdxSelect_LTE__22p9C         15
#define tx_x4_credit_gen0_tempIdxSelect_LTE__12p6C         14
#define tx_x4_credit_gen0_tempIdxSelect_LTE__3p0C          13
#define tx_x4_credit_gen0_tempIdxSelect_LTE_6p7C           12
#define tx_x4_credit_gen0_tempIdxSelect_LTE_16p4C          11
#define tx_x4_credit_gen0_tempIdxSelect_LTE_26p6C          10
#define tx_x4_credit_gen0_tempIdxSelect_LTE_36p3C          9
#define tx_x4_credit_gen0_tempIdxSelect_LTE_46p0C          8
#define tx_x4_credit_gen0_tempIdxSelect_LTE_56p2C          7
#define tx_x4_credit_gen0_tempIdxSelect_LTE_65p9C          6
#define tx_x4_credit_gen0_tempIdxSelect_LTE_75p6C          5
#define tx_x4_credit_gen0_tempIdxSelect_LTE_85p3C          4
#define tx_x4_credit_gen0_tempIdxSelect_LTE_95p5C          3
#define tx_x4_credit_gen0_tempIdxSelect_LTE_105p2C         2
#define tx_x4_credit_gen0_tempIdxSelect_LTE_114p9C         1
#define tx_x4_credit_gen0_tempIdxSelect_LTE_125p1C         0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_port_mode
 */
#define tx_x4_credit_gen0_port_mode_QUAD_PORT              0
#define tx_x4_credit_gen0_port_mode_TRI_1_PORT             1
#define tx_x4_credit_gen0_port_mode_TRI_2_PORT             2
#define tx_x4_credit_gen0_port_mode_DUAL_PORT              3
#define tx_x4_credit_gen0_port_mode_SINGLE_PORT            4

/****************************************************************************
 * Enums: tx_x4_credit_gen0_rev_letter_enum
 */
#define tx_x4_credit_gen0_rev_letter_enum_REV_A            0
#define tx_x4_credit_gen0_rev_letter_enum_REV_B            1
#define tx_x4_credit_gen0_rev_letter_enum_REV_C            2
#define tx_x4_credit_gen0_rev_letter_enum_REV_D            3

/****************************************************************************
 * Enums: tx_x4_credit_gen0_rev_number_enum
 */
#define tx_x4_credit_gen0_rev_number_enum_REV_0            0
#define tx_x4_credit_gen0_rev_number_enum_REV_1            1
#define tx_x4_credit_gen0_rev_number_enum_REV_2            2
#define tx_x4_credit_gen0_rev_number_enum_REV_3            3
#define tx_x4_credit_gen0_rev_number_enum_REV_4            4
#define tx_x4_credit_gen0_rev_number_enum_REV_5            5
#define tx_x4_credit_gen0_rev_number_enum_REV_6            6
#define tx_x4_credit_gen0_rev_number_enum_REV_7            7

/****************************************************************************
 * Enums: tx_x4_credit_gen0_bonding_enum
 */
#define tx_x4_credit_gen0_bonding_enum_WIRE_BOND           0
#define tx_x4_credit_gen0_bonding_enum_FLIP_CHIP           1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_tech_process
 */
#define tx_x4_credit_gen0_tech_process_PROCESS_90NM        0
#define tx_x4_credit_gen0_tech_process_PROCESS_65NM        1
#define tx_x4_credit_gen0_tech_process_PROCESS_40NM        2
#define tx_x4_credit_gen0_tech_process_PROCESS_28NM        3

/****************************************************************************
 * Enums: tx_x4_credit_gen0_model_num
 */
#define tx_x4_credit_gen0_model_num_SERDES_CL73            0
#define tx_x4_credit_gen0_model_num_XGXS_16G               1
#define tx_x4_credit_gen0_model_num_HYPERCORE              2
#define tx_x4_credit_gen0_model_num_HYPERLITE              3
#define tx_x4_credit_gen0_model_num_PCIE_G2_PIPE           4
#define tx_x4_credit_gen0_model_num_SERDES_1p25GBd         5
#define tx_x4_credit_gen0_model_num_SATA2                  6
#define tx_x4_credit_gen0_model_num_QSGMII                 7
#define tx_x4_credit_gen0_model_num_XGXS10G                8
#define tx_x4_credit_gen0_model_num_WARPCORE               9
#define tx_x4_credit_gen0_model_num_XFICORE                10
#define tx_x4_credit_gen0_model_num_RXFI                   11
#define tx_x4_credit_gen0_model_num_WARPLITE               12
#define tx_x4_credit_gen0_model_num_PENTACORE              13
#define tx_x4_credit_gen0_model_num_ESM                    14
#define tx_x4_credit_gen0_model_num_QUAD_SGMII             15
#define tx_x4_credit_gen0_model_num_WARPCORE_3             16
#define tx_x4_credit_gen0_model_num_TSC                    17
#define tx_x4_credit_gen0_model_num_TSC4E                  18
#define tx_x4_credit_gen0_model_num_TSC12E                 19
#define tx_x4_credit_gen0_model_num_TSC4F                  20
#define tx_x4_credit_gen0_model_num_XGXS_CL73_90NM         29
#define tx_x4_credit_gen0_model_num_SERDES_CL73_90NM       30
#define tx_x4_credit_gen0_model_num_WARPCORE3              32
#define tx_x4_credit_gen0_model_num_WARPCORE4_TSC          33
#define tx_x4_credit_gen0_model_num_RXAUI                  34

/****************************************************************************
 * Enums: tx_x4_credit_gen0_payload
 */
#define tx_x4_credit_gen0_payload_REPEAT_2_BYTES           0
#define tx_x4_credit_gen0_payload_RAMPING                  1
#define tx_x4_credit_gen0_payload_CL48_CRPAT               2
#define tx_x4_credit_gen0_payload_CL48_CJPAT               3
#define tx_x4_credit_gen0_payload_CL36_LONG_CRPAT          4
#define tx_x4_credit_gen0_payload_CL36_SHORT_CRPAT         5

/****************************************************************************
 * Enums: tx_x4_credit_gen0_sc
 */
#define tx_x4_credit_gen0_sc_S_10G_CR1                     0
#define tx_x4_credit_gen0_sc_S_10G_KR1                     1
#define tx_x4_credit_gen0_sc_S_10G_X1                      2
#define tx_x4_credit_gen0_sc_S_10G_HG2_CR1                 4
#define tx_x4_credit_gen0_sc_S_10G_HG2_KR1                 5
#define tx_x4_credit_gen0_sc_S_10G_HG2_X1                  6
#define tx_x4_credit_gen0_sc_S_20G_CR1                     8
#define tx_x4_credit_gen0_sc_S_20G_KR1                     9
#define tx_x4_credit_gen0_sc_S_20G_X1                      10
#define tx_x4_credit_gen0_sc_S_20G_HG2_CR1                 12
#define tx_x4_credit_gen0_sc_S_20G_HG2_KR1                 13
#define tx_x4_credit_gen0_sc_S_20G_HG2_X1                  14
#define tx_x4_credit_gen0_sc_S_25G_CR1                     16
#define tx_x4_credit_gen0_sc_S_25G_KR1                     17
#define tx_x4_credit_gen0_sc_S_25G_X1                      18
#define tx_x4_credit_gen0_sc_S_25G_HG2_CR1                 20
#define tx_x4_credit_gen0_sc_S_25G_HG2_KR1                 21
#define tx_x4_credit_gen0_sc_S_25G_HG2_X1                  22
#define tx_x4_credit_gen0_sc_S_20G_CR2                     24
#define tx_x4_credit_gen0_sc_S_20G_KR2                     25
#define tx_x4_credit_gen0_sc_S_20G_X2                      26
#define tx_x4_credit_gen0_sc_S_20G_HG2_CR2                 28
#define tx_x4_credit_gen0_sc_S_20G_HG2_KR2                 29
#define tx_x4_credit_gen0_sc_S_20G_HG2_X2                  30
#define tx_x4_credit_gen0_sc_S_40G_CR2                     32
#define tx_x4_credit_gen0_sc_S_40G_KR2                     33
#define tx_x4_credit_gen0_sc_S_40G_X2                      34
#define tx_x4_credit_gen0_sc_S_40G_HG2_CR2                 36
#define tx_x4_credit_gen0_sc_S_40G_HG2_KR2                 37
#define tx_x4_credit_gen0_sc_S_40G_HG2_X2                  38
#define tx_x4_credit_gen0_sc_S_40G_CR4                     40
#define tx_x4_credit_gen0_sc_S_40G_KR4                     41
#define tx_x4_credit_gen0_sc_S_40G_X4                      42
#define tx_x4_credit_gen0_sc_S_40G_HG2_CR4                 44
#define tx_x4_credit_gen0_sc_S_40G_HG2_KR4                 45
#define tx_x4_credit_gen0_sc_S_40G_HG2_X4                  46
#define tx_x4_credit_gen0_sc_S_50G_CR2                     48
#define tx_x4_credit_gen0_sc_S_50G_KR2                     49
#define tx_x4_credit_gen0_sc_S_50G_X2                      50
#define tx_x4_credit_gen0_sc_S_50G_HG2_CR2                 52
#define tx_x4_credit_gen0_sc_S_50G_HG2_KR2                 53
#define tx_x4_credit_gen0_sc_S_50G_HG2_X2                  54
#define tx_x4_credit_gen0_sc_S_50G_CR4                     56
#define tx_x4_credit_gen0_sc_S_50G_KR4                     57
#define tx_x4_credit_gen0_sc_S_50G_X4                      58
#define tx_x4_credit_gen0_sc_S_50G_HG2_CR4                 60
#define tx_x4_credit_gen0_sc_S_50G_HG2_KR4                 61
#define tx_x4_credit_gen0_sc_S_50G_HG2_X4                  62
#define tx_x4_credit_gen0_sc_S_100G_CR4                    64
#define tx_x4_credit_gen0_sc_S_100G_KR4                    65
#define tx_x4_credit_gen0_sc_S_100G_X4                     66
#define tx_x4_credit_gen0_sc_S_100G_HG2_CR4                68
#define tx_x4_credit_gen0_sc_S_100G_HG2_KR4                69
#define tx_x4_credit_gen0_sc_S_100G_HG2_X4                 70
#define tx_x4_credit_gen0_sc_S_CL73_20GVCO                 72
#define tx_x4_credit_gen0_sc_S_CL73_25GVCO                 80
#define tx_x4_credit_gen0_sc_S_CL36_20GVCO                 88
#define tx_x4_credit_gen0_sc_S_CL36_25GVCO                 96

/****************************************************************************
 * Enums: tx_x4_credit_gen0_t_fifo_modes
 */
#define tx_x4_credit_gen0_t_fifo_modes_T_FIFO_MODE_BYPASS  0
#define tx_x4_credit_gen0_t_fifo_modes_T_FIFO_MODE_40G     1
#define tx_x4_credit_gen0_t_fifo_modes_T_FIFO_MODE_100G    2
#define tx_x4_credit_gen0_t_fifo_modes_T_FIFO_MODE_20G     3

/****************************************************************************
 * Enums: tx_x4_credit_gen0_t_enc_modes
 */
#define tx_x4_credit_gen0_t_enc_modes_T_ENC_MODE_BYPASS    0
#define tx_x4_credit_gen0_t_enc_modes_T_ENC_MODE_CL49      1
#define tx_x4_credit_gen0_t_enc_modes_T_ENC_MODE_CL82      2

/****************************************************************************
 * Enums: tx_x4_credit_gen0_btmx_mode
 */
#define tx_x4_credit_gen0_btmx_mode_BS_BTMX_MODE_1to1      0
#define tx_x4_credit_gen0_btmx_mode_BS_BTMX_MODE_2to1      1
#define tx_x4_credit_gen0_btmx_mode_BS_BTMX_MODE_5to1      2

/****************************************************************************
 * Enums: tx_x4_credit_gen0_t_type_cl82
 */
#define tx_x4_credit_gen0_t_type_cl82_T_TYPE_B1            5
#define tx_x4_credit_gen0_t_type_cl82_T_TYPE_C             4
#define tx_x4_credit_gen0_t_type_cl82_T_TYPE_S             3
#define tx_x4_credit_gen0_t_type_cl82_T_TYPE_T             2
#define tx_x4_credit_gen0_t_type_cl82_T_TYPE_D             1
#define tx_x4_credit_gen0_t_type_cl82_T_TYPE_E             0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_txsm_state_cl82
 */
#define tx_x4_credit_gen0_txsm_state_cl82_TX_HIG_END       6
#define tx_x4_credit_gen0_txsm_state_cl82_TX_HIG_START     5
#define tx_x4_credit_gen0_txsm_state_cl82_TX_E             4
#define tx_x4_credit_gen0_txsm_state_cl82_TX_T             3
#define tx_x4_credit_gen0_txsm_state_cl82_TX_D             2
#define tx_x4_credit_gen0_txsm_state_cl82_TX_C             1
#define tx_x4_credit_gen0_txsm_state_cl82_TX_INIT          0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_ltxsm_state_cl82
 */
#define tx_x4_credit_gen0_ltxsm_state_cl82_TX_HIG_END      64
#define tx_x4_credit_gen0_ltxsm_state_cl82_TX_HIG_START    32
#define tx_x4_credit_gen0_ltxsm_state_cl82_TX_E            16
#define tx_x4_credit_gen0_ltxsm_state_cl82_TX_T            8
#define tx_x4_credit_gen0_ltxsm_state_cl82_TX_D            4
#define tx_x4_credit_gen0_ltxsm_state_cl82_TX_C            2
#define tx_x4_credit_gen0_ltxsm_state_cl82_TX_INIT         1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_r_type_coded_cl82
 */
#define tx_x4_credit_gen0_r_type_coded_cl82_R_TYPE_B1      32
#define tx_x4_credit_gen0_r_type_coded_cl82_R_TYPE_C       16
#define tx_x4_credit_gen0_r_type_coded_cl82_R_TYPE_S       8
#define tx_x4_credit_gen0_r_type_coded_cl82_R_TYPE_T       4
#define tx_x4_credit_gen0_r_type_coded_cl82_R_TYPE_D       2
#define tx_x4_credit_gen0_r_type_coded_cl82_R_TYPE_E       1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_rxsm_state_cl82
 */
#define tx_x4_credit_gen0_rxsm_state_cl82_RX_HIG_END       64
#define tx_x4_credit_gen0_rxsm_state_cl82_RX_HIG_START     32
#define tx_x4_credit_gen0_rxsm_state_cl82_RX_E             16
#define tx_x4_credit_gen0_rxsm_state_cl82_RX_T             8
#define tx_x4_credit_gen0_rxsm_state_cl82_RX_D             4
#define tx_x4_credit_gen0_rxsm_state_cl82_RX_C             2
#define tx_x4_credit_gen0_rxsm_state_cl82_RX_INIT          1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_deskew_state
 */
#define tx_x4_credit_gen0_deskew_state_ALIGN_ACQUIRED      2
#define tx_x4_credit_gen0_deskew_state_LOSS_OF_ALIGNMENT   1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_os_mode_enum
 */
#define tx_x4_credit_gen0_os_mode_enum_OS_MODE_1           0
#define tx_x4_credit_gen0_os_mode_enum_OS_MODE_2           1
#define tx_x4_credit_gen0_os_mode_enum_OS_MODE_4           2
#define tx_x4_credit_gen0_os_mode_enum_OS_MODE_16p5        8
#define tx_x4_credit_gen0_os_mode_enum_OS_MODE_20p625      12

/****************************************************************************
 * Enums: tx_x4_credit_gen0_scr_modes
 */
#define tx_x4_credit_gen0_scr_modes_T_SCR_MODE_BYPASS      0
#define tx_x4_credit_gen0_scr_modes_T_SCR_MODE_CL49        1
#define tx_x4_credit_gen0_scr_modes_T_SCR_MODE_40G_2_LANE  2
#define tx_x4_credit_gen0_scr_modes_T_SCR_MODE_100G        3
#define tx_x4_credit_gen0_scr_modes_T_SCR_MODE_20G         4
#define tx_x4_credit_gen0_scr_modes_T_SCR_MODE_40G_4_LANE  5

/****************************************************************************
 * Enums: tx_x4_credit_gen0_descr_modes
 */
#define tx_x4_credit_gen0_descr_modes_R_DESCR_MODE_BYPASS  0
#define tx_x4_credit_gen0_descr_modes_R_DESCR_MODE_CL49    1
#define tx_x4_credit_gen0_descr_modes_R_DESCR_MODE_CL82    2

/****************************************************************************
 * Enums: tx_x4_credit_gen0_r_dec_tl_mode
 */
#define tx_x4_credit_gen0_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS 0
#define tx_x4_credit_gen0_r_dec_tl_mode_R_DEC_TL_MODE_CL49 1
#define tx_x4_credit_gen0_r_dec_tl_mode_R_DEC_TL_MODE_CL82 2

/****************************************************************************
 * Enums: tx_x4_credit_gen0_r_dec_fsm_mode
 */
#define tx_x4_credit_gen0_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define tx_x4_credit_gen0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49 1
#define tx_x4_credit_gen0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82 2

/****************************************************************************
 * Enums: tx_x4_credit_gen0_r_deskew_mode
 */
#define tx_x4_credit_gen0_r_deskew_mode_R_DESKEW_MODE_BYPASS 0
#define tx_x4_credit_gen0_r_deskew_mode_R_DESKEW_MODE_20G  1
#define tx_x4_credit_gen0_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define tx_x4_credit_gen0_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define tx_x4_credit_gen0_r_deskew_mode_R_DESKEW_MODE_100G 4
#define tx_x4_credit_gen0_r_deskew_mode_R_DESKEW_MODE_CL49 5
#define tx_x4_credit_gen0_r_deskew_mode_R_DESKEW_MODE_CL91 6

/****************************************************************************
 * Enums: tx_x4_credit_gen0_bs_dist_modes
 */
#define tx_x4_credit_gen0_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define tx_x4_credit_gen0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define tx_x4_credit_gen0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define tx_x4_credit_gen0_bs_dist_modes_BS_DIST_MODE_NO_TDM 3

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl49_t_type
 */
#define tx_x4_credit_gen0_cl49_t_type_BAD_T_TYPE           15
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_B0            11
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_OB            10
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_B1            9
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_DB            8
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_FC            7
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_TB            6
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_LI            5
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_C             4
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_S             3
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_T             2
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_D             1
#define tx_x4_credit_gen0_cl49_t_type_T_TYPE_E             0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl49_txsm_states
 */
#define tx_x4_credit_gen0_cl49_txsm_states_TX_HIG_END      7
#define tx_x4_credit_gen0_cl49_txsm_states_TX_HIG_START    6
#define tx_x4_credit_gen0_cl49_txsm_states_TX_LI           5
#define tx_x4_credit_gen0_cl49_txsm_states_TX_E            4
#define tx_x4_credit_gen0_cl49_txsm_states_TX_T            3
#define tx_x4_credit_gen0_cl49_txsm_states_TX_D            2
#define tx_x4_credit_gen0_cl49_txsm_states_TX_C            1
#define tx_x4_credit_gen0_cl49_txsm_states_TX_INIT         0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl49_ltxsm_states
 */
#define tx_x4_credit_gen0_cl49_ltxsm_states_TX_HIG_END     128
#define tx_x4_credit_gen0_cl49_ltxsm_states_TX_HIG_START   64
#define tx_x4_credit_gen0_cl49_ltxsm_states_TX_LI          32
#define tx_x4_credit_gen0_cl49_ltxsm_states_TX_E           16
#define tx_x4_credit_gen0_cl49_ltxsm_states_TX_T           8
#define tx_x4_credit_gen0_cl49_ltxsm_states_TX_D           4
#define tx_x4_credit_gen0_cl49_ltxsm_states_TX_C           2
#define tx_x4_credit_gen0_cl49_ltxsm_states_TX_INIT        1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_burst_error_mode
 */
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_11_BITS 0
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_16_BITS 1
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_17_BITS 2
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_18_BITS 3
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_19_BITS 4
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_20_BITS 5
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_21_BITS 6
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_22_BITS 7
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_23_BITS 8
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_24_BITS 9
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_25_BITS 10
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_26_BITS 11
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_27_BITS 12
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_28_BITS 13
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_29_BITS 14
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_30_BITS 15
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_31_BITS 16
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_32_BITS 17
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_33_BITS 18
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_34_BITS 19
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_35_BITS 20
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_36_BITS 21
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_37_BITS 22
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_38_BITS 23
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_39_BITS 24
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_40_BITS 25
#define tx_x4_credit_gen0_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: tx_x4_credit_gen0_bermon_state
 */
#define tx_x4_credit_gen0_bermon_state_HI_BER              4
#define tx_x4_credit_gen0_bermon_state_GOOD_BER            3
#define tx_x4_credit_gen0_bermon_state_BER_TEST_SH         2
#define tx_x4_credit_gen0_bermon_state_START_TIMER         1
#define tx_x4_credit_gen0_bermon_state_BER_MT_INIT         0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_rxsm_state_cl49
 */
#define tx_x4_credit_gen0_rxsm_state_cl49_RX_HIG_END       128
#define tx_x4_credit_gen0_rxsm_state_cl49_RX_HIG_START     64
#define tx_x4_credit_gen0_rxsm_state_cl49_RX_LI            32
#define tx_x4_credit_gen0_rxsm_state_cl49_RX_E             16
#define tx_x4_credit_gen0_rxsm_state_cl49_RX_T             8
#define tx_x4_credit_gen0_rxsm_state_cl49_RX_D             4
#define tx_x4_credit_gen0_rxsm_state_cl49_RX_C             2
#define tx_x4_credit_gen0_rxsm_state_cl49_RX_INIT          1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_r_type
 */
#define tx_x4_credit_gen0_r_type_BAD_R_TYPE                15
#define tx_x4_credit_gen0_r_type_R_TYPE_B0                 11
#define tx_x4_credit_gen0_r_type_R_TYPE_OB                 10
#define tx_x4_credit_gen0_r_type_R_TYPE_B1                 9
#define tx_x4_credit_gen0_r_type_R_TYPE_DB                 8
#define tx_x4_credit_gen0_r_type_R_TYPE_FC                 7
#define tx_x4_credit_gen0_r_type_R_TYPE_TB                 6
#define tx_x4_credit_gen0_r_type_R_TYPE_LI                 5
#define tx_x4_credit_gen0_r_type_R_TYPE_C                  4
#define tx_x4_credit_gen0_r_type_R_TYPE_S                  3
#define tx_x4_credit_gen0_r_type_R_TYPE_T                  2
#define tx_x4_credit_gen0_r_type_R_TYPE_D                  1
#define tx_x4_credit_gen0_r_type_R_TYPE_E                  0

/****************************************************************************
 * Enums: tx_x4_credit_gen0_am_lock_state
 */
#define tx_x4_credit_gen0_am_lock_state_INVALID_AM         512
#define tx_x4_credit_gen0_am_lock_state_GOOD_AM            256
#define tx_x4_credit_gen0_am_lock_state_COMP_AM            128
#define tx_x4_credit_gen0_am_lock_state_TIMER_2            64
#define tx_x4_credit_gen0_am_lock_state_AM_2_GOOD          32
#define tx_x4_credit_gen0_am_lock_state_COMP_2ND           16
#define tx_x4_credit_gen0_am_lock_state_TIMER_1            8
#define tx_x4_credit_gen0_am_lock_state_FIND_1ST           4
#define tx_x4_credit_gen0_am_lock_state_AM_RESET_CNT       2
#define tx_x4_credit_gen0_am_lock_state_AM_LOCK_INIT       1

/****************************************************************************
 * Enums: tx_x4_credit_gen0_msg_selector
 */
#define tx_x4_credit_gen0_msg_selector_RESERVED            0
#define tx_x4_credit_gen0_msg_selector_VALUE_802p3         1
#define tx_x4_credit_gen0_msg_selector_VALUE_802p9         2
#define tx_x4_credit_gen0_msg_selector_VALUE_802p5         3
#define tx_x4_credit_gen0_msg_selector_VALUE_1394          4

/****************************************************************************
 * Enums: tx_x4_credit_gen0_synce_enum
 */
#define tx_x4_credit_gen0_synce_enum_SYNCE_NO_DIV          0
#define tx_x4_credit_gen0_synce_enum_SYNCE_DIV_7           1
#define tx_x4_credit_gen0_synce_enum_SYNCE_DIV_11          2

/****************************************************************************
 * Enums: tx_x4_credit_gen0_synce_enum_stage0
 */
#define tx_x4_credit_gen0_synce_enum_stage0_SYNCE_NO_DIV   0
#define tx_x4_credit_gen0_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define tx_x4_credit_gen0_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl91_sync_state
 */
#define tx_x4_credit_gen0_cl91_sync_state_FIND_1ST         0
#define tx_x4_credit_gen0_cl91_sync_state_COUNT_NEXT       1
#define tx_x4_credit_gen0_cl91_sync_state_COMP_2ND         2
#define tx_x4_credit_gen0_cl91_sync_state_TWO_GOOD         3

/****************************************************************************
 * Enums: tx_x4_credit_gen0_cl91_algn_state
 */
#define tx_x4_credit_gen0_cl91_algn_state_LOSS_OF_ALIGNMENT 0
#define tx_x4_credit_gen0_cl91_algn_state_DESKEW           1
#define tx_x4_credit_gen0_cl91_algn_state_DESKEW_FAIL      2
#define tx_x4_credit_gen0_cl91_algn_state_ALIGN_ACQUIRED   3
#define tx_x4_credit_gen0_cl91_algn_state_CW_GOOD          4
#define tx_x4_credit_gen0_cl91_algn_state_CW_BAD           5
#define tx_x4_credit_gen0_cl91_algn_state_THREE_BAD        6

/****************************************************************************
 * Enums: tx_x4_credit_gen0_fec_sel
 */
#define tx_x4_credit_gen0_fec_sel_NO_FEC                   0
#define tx_x4_credit_gen0_fec_sel_FEC_CL74                 1
#define tx_x4_credit_gen0_fec_sel_FEC_CL91                 2

/****************************************************************************
 * Enums: tx_x4_status0_cl36TxEEEStates_l
 */
#define tx_x4_status0_cl36TxEEEStates_l_TX_REFRESH         8
#define tx_x4_status0_cl36TxEEEStates_l_TX_QUIET           4
#define tx_x4_status0_cl36TxEEEStates_l_TX_SLEEP           2
#define tx_x4_status0_cl36TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x4_status0_cl36TxEEEStates_c
 */
#define tx_x4_status0_cl36TxEEEStates_c_TX_REFRESH         3
#define tx_x4_status0_cl36TxEEEStates_c_TX_QUIET           2
#define tx_x4_status0_cl36TxEEEStates_c_TX_SLEEP           1
#define tx_x4_status0_cl36TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x4_status0_cl49TxEEEStates_l
 */
#define tx_x4_status0_cl49TxEEEStates_l_SCR_RESET_2        64
#define tx_x4_status0_cl49TxEEEStates_l_SCR_RESET_1        32
#define tx_x4_status0_cl49TxEEEStates_l_TX_WAKE            16
#define tx_x4_status0_cl49TxEEEStates_l_TX_REFRESH         8
#define tx_x4_status0_cl49TxEEEStates_l_TX_QUIET           4
#define tx_x4_status0_cl49TxEEEStates_l_TX_SLEEP           2
#define tx_x4_status0_cl49TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x4_status0_fec_req_enum
 */
#define tx_x4_status0_fec_req_enum_FEC_not_supported       0
#define tx_x4_status0_fec_req_enum_FEC_supported_but_not_requested 1
#define tx_x4_status0_fec_req_enum_invalid_setting         2
#define tx_x4_status0_fec_req_enum_FEC_supported_and_requested 3

/****************************************************************************
 * Enums: tx_x4_status0_cl73_pause_enum
 */
#define tx_x4_status0_cl73_pause_enum_No_PAUSE_ability     0
#define tx_x4_status0_cl73_pause_enum_Asymmetric_toward_link_partner_PAUSE_ability 2
#define tx_x4_status0_cl73_pause_enum_Symmetric_PAUSE_ability 1
#define tx_x4_status0_cl73_pause_enum_Both_symmetric_and_asymmetric_toward_local_device_PAUSE_ability 3

/****************************************************************************
 * Enums: tx_x4_status0_cl49TxEEEStates_c
 */
#define tx_x4_status0_cl49TxEEEStates_c_SCR_RESET_2        6
#define tx_x4_status0_cl49TxEEEStates_c_SCR_RESET_1        5
#define tx_x4_status0_cl49TxEEEStates_c_TX_WAKE            4
#define tx_x4_status0_cl49TxEEEStates_c_TX_REFRESH         3
#define tx_x4_status0_cl49TxEEEStates_c_TX_QUIET           2
#define tx_x4_status0_cl49TxEEEStates_c_TX_SLEEP           1
#define tx_x4_status0_cl49TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x4_status0_cl36RxEEEStates_l
 */
#define tx_x4_status0_cl36RxEEEStates_l_RX_LINK_FAIL       32
#define tx_x4_status0_cl36RxEEEStates_l_RX_WTF             16
#define tx_x4_status0_cl36RxEEEStates_l_RX_WAKE            8
#define tx_x4_status0_cl36RxEEEStates_l_RX_QUIET           4
#define tx_x4_status0_cl36RxEEEStates_l_RX_SLEEP           2
#define tx_x4_status0_cl36RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x4_status0_cl36RxEEEStates_c
 */
#define tx_x4_status0_cl36RxEEEStates_c_RX_LINK_FAIL       5
#define tx_x4_status0_cl36RxEEEStates_c_RX_WTF             4
#define tx_x4_status0_cl36RxEEEStates_c_RX_WAKE            3
#define tx_x4_status0_cl36RxEEEStates_c_RX_QUIET           2
#define tx_x4_status0_cl36RxEEEStates_c_RX_SLEEP           1
#define tx_x4_status0_cl36RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x4_status0_cl49RxEEEStates_l
 */
#define tx_x4_status0_cl49RxEEEStates_l_RX_LINK_FAIL       32
#define tx_x4_status0_cl49RxEEEStates_l_RX_WTF             16
#define tx_x4_status0_cl49RxEEEStates_l_RX_WAKE            8
#define tx_x4_status0_cl49RxEEEStates_l_RX_QUIET           4
#define tx_x4_status0_cl49RxEEEStates_l_RX_SLEEP           2
#define tx_x4_status0_cl49RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x4_status0_cl49RxEEEStates_c
 */
#define tx_x4_status0_cl49RxEEEStates_c_RX_LINK_FAIL       5
#define tx_x4_status0_cl49RxEEEStates_c_RX_WTF             4
#define tx_x4_status0_cl49RxEEEStates_c_RX_WAKE            3
#define tx_x4_status0_cl49RxEEEStates_c_RX_QUIET           2
#define tx_x4_status0_cl49RxEEEStates_c_RX_SLEEP           1
#define tx_x4_status0_cl49RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x4_status0_cl48TxEEEStates_l
 */
#define tx_x4_status0_cl48TxEEEStates_l_TX_REFRESH         8
#define tx_x4_status0_cl48TxEEEStates_l_TX_QUIET           4
#define tx_x4_status0_cl48TxEEEStates_l_TX_SLEEP           2
#define tx_x4_status0_cl48TxEEEStates_l_TX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x4_status0_cl48TxEEEStates_c
 */
#define tx_x4_status0_cl48TxEEEStates_c_TX_REFRESH         3
#define tx_x4_status0_cl48TxEEEStates_c_TX_QUIET           2
#define tx_x4_status0_cl48TxEEEStates_c_TX_SLEEP           1
#define tx_x4_status0_cl48TxEEEStates_c_TX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x4_status0_cl48RxEEEStates_l
 */
#define tx_x4_status0_cl48RxEEEStates_l_RX_LINK_FAIL       32
#define tx_x4_status0_cl48RxEEEStates_l_RX_WAKE            16
#define tx_x4_status0_cl48RxEEEStates_l_RX_QUIET           8
#define tx_x4_status0_cl48RxEEEStates_l_RX_DEACT           4
#define tx_x4_status0_cl48RxEEEStates_l_RX_SLEEP           2
#define tx_x4_status0_cl48RxEEEStates_l_RX_ACTIVE          1

/****************************************************************************
 * Enums: tx_x4_status0_cl48RxEEEStates_c
 */
#define tx_x4_status0_cl48RxEEEStates_c_RX_LINK_FAIL       5
#define tx_x4_status0_cl48RxEEEStates_c_RX_WAKE            4
#define tx_x4_status0_cl48RxEEEStates_c_RX_QUIET           3
#define tx_x4_status0_cl48RxEEEStates_c_RX_DEACT           2
#define tx_x4_status0_cl48RxEEEStates_c_RX_SLEEP           1
#define tx_x4_status0_cl48RxEEEStates_c_RX_ACTIVE          0

/****************************************************************************
 * Enums: tx_x4_status0_IQP_Options
 */
#define tx_x4_status0_IQP_Options_i50uA                    0
#define tx_x4_status0_IQP_Options_i100uA                   1
#define tx_x4_status0_IQP_Options_i150uA                   2
#define tx_x4_status0_IQP_Options_i200uA                   3
#define tx_x4_status0_IQP_Options_i250uA                   4
#define tx_x4_status0_IQP_Options_i300uA                   5
#define tx_x4_status0_IQP_Options_i350uA                   6
#define tx_x4_status0_IQP_Options_i400uA                   7
#define tx_x4_status0_IQP_Options_i450uA                   8
#define tx_x4_status0_IQP_Options_i500uA                   9
#define tx_x4_status0_IQP_Options_i550uA                   10
#define tx_x4_status0_IQP_Options_i600uA                   11
#define tx_x4_status0_IQP_Options_i650uA                   12
#define tx_x4_status0_IQP_Options_i700uA                   13
#define tx_x4_status0_IQP_Options_i750uA                   14
#define tx_x4_status0_IQP_Options_i800uA                   15

/****************************************************************************
 * Enums: tx_x4_status0_IDriver_Options
 */
#define tx_x4_status0_IDriver_Options_v680mV               0
#define tx_x4_status0_IDriver_Options_v730mV               1
#define tx_x4_status0_IDriver_Options_v780mV               2
#define tx_x4_status0_IDriver_Options_v830mV               3
#define tx_x4_status0_IDriver_Options_v880mV               4
#define tx_x4_status0_IDriver_Options_v930mV               5
#define tx_x4_status0_IDriver_Options_v980mV               6
#define tx_x4_status0_IDriver_Options_v1010mV              7
#define tx_x4_status0_IDriver_Options_v1040mV              8
#define tx_x4_status0_IDriver_Options_v1060mV              9
#define tx_x4_status0_IDriver_Options_v1070mV              10
#define tx_x4_status0_IDriver_Options_v1080mV              11
#define tx_x4_status0_IDriver_Options_v1085mV              12
#define tx_x4_status0_IDriver_Options_v1090mV              13
#define tx_x4_status0_IDriver_Options_v1095mV              14
#define tx_x4_status0_IDriver_Options_v1100mV              15

/****************************************************************************
 * Enums: tx_x4_status0_operationModes
 */
#define tx_x4_status0_operationModes_XGXS                  0
#define tx_x4_status0_operationModes_XGXG_nCC              1
#define tx_x4_status0_operationModes_Indlane_OS8           4
#define tx_x4_status0_operationModes_IndLane_OS5           5
#define tx_x4_status0_operationModes_PCI                   7
#define tx_x4_status0_operationModes_XGXS_nLQ              8
#define tx_x4_status0_operationModes_XGXS_nLQnCC           9
#define tx_x4_status0_operationModes_PBypass               10
#define tx_x4_status0_operationModes_PBypass_nDSK          11
#define tx_x4_status0_operationModes_ComboCoreMode         12
#define tx_x4_status0_operationModes_Clocks_off            15

/****************************************************************************
 * Enums: tx_x4_status0_actualSpeeds
 */
#define tx_x4_status0_actualSpeeds_dr_10M                  0
#define tx_x4_status0_actualSpeeds_dr_100M                 1
#define tx_x4_status0_actualSpeeds_dr_1G                   2
#define tx_x4_status0_actualSpeeds_dr_2p5G                 3
#define tx_x4_status0_actualSpeeds_dr_5G_X4                4
#define tx_x4_status0_actualSpeeds_dr_6G_X4                5
#define tx_x4_status0_actualSpeeds_dr_10G_HiG              6
#define tx_x4_status0_actualSpeeds_dr_10G_CX4              7
#define tx_x4_status0_actualSpeeds_dr_12G_HiG              8
#define tx_x4_status0_actualSpeeds_dr_12p5G_X4             9
#define tx_x4_status0_actualSpeeds_dr_13G_X4               10
#define tx_x4_status0_actualSpeeds_dr_15G_X4               11
#define tx_x4_status0_actualSpeeds_dr_16G_X4               12
#define tx_x4_status0_actualSpeeds_dr_1G_KX                13
#define tx_x4_status0_actualSpeeds_dr_10G_KX4              14
#define tx_x4_status0_actualSpeeds_dr_10G_KR               15
#define tx_x4_status0_actualSpeeds_dr_5G                   16
#define tx_x4_status0_actualSpeeds_dr_6p4G                 17
#define tx_x4_status0_actualSpeeds_dr_20G_X4               18
#define tx_x4_status0_actualSpeeds_dr_21G_X4               19
#define tx_x4_status0_actualSpeeds_dr_25G_X4               20
#define tx_x4_status0_actualSpeeds_dr_10G_HiG_DXGXS        21
#define tx_x4_status0_actualSpeeds_dr_10G_DXGXS            22
#define tx_x4_status0_actualSpeeds_dr_10p5G_HiG_DXGXS      23
#define tx_x4_status0_actualSpeeds_dr_10p5G_DXGXS          24
#define tx_x4_status0_actualSpeeds_dr_12p773G_HiG_DXGXS    25
#define tx_x4_status0_actualSpeeds_dr_12p773G_DXGXS        26
#define tx_x4_status0_actualSpeeds_dr_10G_XFI              27
#define tx_x4_status0_actualSpeeds_dr_40G                  28
#define tx_x4_status0_actualSpeeds_dr_20G_HiG_DXGXS        29
#define tx_x4_status0_actualSpeeds_dr_20G_DXGXS            30
#define tx_x4_status0_actualSpeeds_dr_10G_SFI              31
#define tx_x4_status0_actualSpeeds_dr_31p5G                32
#define tx_x4_status0_actualSpeeds_dr_32p7G                33
#define tx_x4_status0_actualSpeeds_dr_20G_SCR              34
#define tx_x4_status0_actualSpeeds_dr_10G_HiG_DXGXS_SCR    35
#define tx_x4_status0_actualSpeeds_dr_10G_DXGXS_SCR        36
#define tx_x4_status0_actualSpeeds_dr_12G_R2               37
#define tx_x4_status0_actualSpeeds_dr_10G_X2               38
#define tx_x4_status0_actualSpeeds_dr_40G_KR4              39
#define tx_x4_status0_actualSpeeds_dr_40G_CR4              40
#define tx_x4_status0_actualSpeeds_dr_100G_CR10            41
#define tx_x4_status0_actualSpeeds_dr_15p75G_DXGXS         44
#define tx_x4_status0_actualSpeeds_dr_20G_KR2              57
#define tx_x4_status0_actualSpeeds_dr_20G_CR2              58

/****************************************************************************
 * Enums: tx_x4_status0_actualSpeedsMisc1
 */
#define tx_x4_status0_actualSpeedsMisc1_dr_2500BRCM_X1     16
#define tx_x4_status0_actualSpeedsMisc1_dr_5000BRCM_X4     17
#define tx_x4_status0_actualSpeedsMisc1_dr_6000BRCM_X4     18
#define tx_x4_status0_actualSpeedsMisc1_dr_10GHiGig_X4     19
#define tx_x4_status0_actualSpeedsMisc1_dr_10GBASE_CX4     20
#define tx_x4_status0_actualSpeedsMisc1_dr_12GHiGig_X4     21
#define tx_x4_status0_actualSpeedsMisc1_dr_12p5GHiGig_X4   22
#define tx_x4_status0_actualSpeedsMisc1_dr_13GHiGig_X4     23
#define tx_x4_status0_actualSpeedsMisc1_dr_15GHiGig_X4     24
#define tx_x4_status0_actualSpeedsMisc1_dr_16GHiGig_X4     25
#define tx_x4_status0_actualSpeedsMisc1_dr_5000BRCM_X1     26
#define tx_x4_status0_actualSpeedsMisc1_dr_6363BRCM_X1     27
#define tx_x4_status0_actualSpeedsMisc1_dr_20GHiGig_X4     28
#define tx_x4_status0_actualSpeedsMisc1_dr_21GHiGig_X4     29
#define tx_x4_status0_actualSpeedsMisc1_dr_25p45GHiGig_X4  30
#define tx_x4_status0_actualSpeedsMisc1_dr_10G_HiG_DXGXS   31

/****************************************************************************
 * Enums: tx_x4_status0_IndLaneModes
 */
#define tx_x4_status0_IndLaneModes_SWSDR_div2              0
#define tx_x4_status0_IndLaneModes_SWSDR_div1              1
#define tx_x4_status0_IndLaneModes_DWSDR_div2              2
#define tx_x4_status0_IndLaneModes_DWSDR_div1              3

/****************************************************************************
 * Enums: tx_x4_status0_prbsSelect
 */
#define tx_x4_status0_prbsSelect_prbs7                     0
#define tx_x4_status0_prbsSelect_prbs15                    1
#define tx_x4_status0_prbsSelect_prbs23                    2
#define tx_x4_status0_prbsSelect_prbs31                    3

/****************************************************************************
 * Enums: tx_x4_status0_vcoDivider
 */
#define tx_x4_status0_vcoDivider_div32                     0
#define tx_x4_status0_vcoDivider_div36                     1
#define tx_x4_status0_vcoDivider_div40                     2
#define tx_x4_status0_vcoDivider_div42                     3
#define tx_x4_status0_vcoDivider_div48                     4
#define tx_x4_status0_vcoDivider_div50                     5
#define tx_x4_status0_vcoDivider_div52                     6
#define tx_x4_status0_vcoDivider_div54                     7
#define tx_x4_status0_vcoDivider_div60                     8
#define tx_x4_status0_vcoDivider_div64                     9
#define tx_x4_status0_vcoDivider_div66                     10
#define tx_x4_status0_vcoDivider_div68                     11
#define tx_x4_status0_vcoDivider_div70                     12
#define tx_x4_status0_vcoDivider_div80                     13
#define tx_x4_status0_vcoDivider_div92                     14
#define tx_x4_status0_vcoDivider_div100                    15

/****************************************************************************
 * Enums: tx_x4_status0_refClkSelect
 */
#define tx_x4_status0_refClkSelect_clk_25MHz               0
#define tx_x4_status0_refClkSelect_clk_100MHz              1
#define tx_x4_status0_refClkSelect_clk_125MHz              2
#define tx_x4_status0_refClkSelect_clk_156p25MHz           3
#define tx_x4_status0_refClkSelect_clk_187p5MHz            4
#define tx_x4_status0_refClkSelect_clk_161p25Mhz           5
#define tx_x4_status0_refClkSelect_clk_50Mhz               6
#define tx_x4_status0_refClkSelect_clk_106p25Mhz           7

/****************************************************************************
 * Enums: tx_x4_status0_aerMMDdevTypeSelect
 */
#define tx_x4_status0_aerMMDdevTypeSelect_combo_core       0
#define tx_x4_status0_aerMMDdevTypeSelect_PMA_PMD          1
#define tx_x4_status0_aerMMDdevTypeSelect_PCS              3
#define tx_x4_status0_aerMMDdevTypeSelect_PHY              4
#define tx_x4_status0_aerMMDdevTypeSelect_DTE              5
#define tx_x4_status0_aerMMDdevTypeSelect_CL73_AN          7

/****************************************************************************
 * Enums: tx_x4_status0_aerMMDportSelect
 */
#define tx_x4_status0_aerMMDportSelect_ln0                 0
#define tx_x4_status0_aerMMDportSelect_ln1                 1
#define tx_x4_status0_aerMMDportSelect_ln2                 2
#define tx_x4_status0_aerMMDportSelect_ln3                 3
#define tx_x4_status0_aerMMDportSelect_BCST_ln0_1_2_3      511
#define tx_x4_status0_aerMMDportSelect_BCST_ln0_1          512
#define tx_x4_status0_aerMMDportSelect_BCST_ln2_3          513

/****************************************************************************
 * Enums: tx_x4_status0_firmwareModeSelect
 */
#define tx_x4_status0_firmwareModeSelect_DEFAULT           0
#define tx_x4_status0_firmwareModeSelect_SFP_OPT_LR        1
#define tx_x4_status0_firmwareModeSelect_SFP_DAC           2
#define tx_x4_status0_firmwareModeSelect_XLAUI             3
#define tx_x4_status0_firmwareModeSelect_LONG_CH_6G        4

/****************************************************************************
 * Enums: tx_x4_status0_tempIdxSelect
 */
#define tx_x4_status0_tempIdxSelect_LTE__22p9C             15
#define tx_x4_status0_tempIdxSelect_LTE__12p6C             14
#define tx_x4_status0_tempIdxSelect_LTE__3p0C              13
#define tx_x4_status0_tempIdxSelect_LTE_6p7C               12
#define tx_x4_status0_tempIdxSelect_LTE_16p4C              11
#define tx_x4_status0_tempIdxSelect_LTE_26p6C              10
#define tx_x4_status0_tempIdxSelect_LTE_36p3C              9
#define tx_x4_status0_tempIdxSelect_LTE_46p0C              8
#define tx_x4_status0_tempIdxSelect_LTE_56p2C              7
#define tx_x4_status0_tempIdxSelect_LTE_65p9C              6
#define tx_x4_status0_tempIdxSelect_LTE_75p6C              5
#define tx_x4_status0_tempIdxSelect_LTE_85p3C              4
#define tx_x4_status0_tempIdxSelect_LTE_95p5C              3
#define tx_x4_status0_tempIdxSelect_LTE_105p2C             2
#define tx_x4_status0_tempIdxSelect_LTE_114p9C             1
#define tx_x4_status0_tempIdxSelect_LTE_125p1C             0

/****************************************************************************
 * Enums: tx_x4_status0_port_mode
 */
#define tx_x4_status0_port_mode_QUAD_PORT                  0
#define tx_x4_status0_port_mode_TRI_1_PORT                 1
#define tx_x4_status0_port_mode_TRI_2_PORT                 2
#define tx_x4_status0_port_mode_DUAL_PORT                  3
#define tx_x4_status0_port_mode_SINGLE_PORT                4

/****************************************************************************
 * Enums: tx_x4_status0_rev_letter_enum
 */
#define tx_x4_status0_rev_letter_enum_REV_A                0
#define tx_x4_status0_rev_letter_enum_REV_B                1
#define tx_x4_status0_rev_letter_enum_REV_C                2
#define tx_x4_status0_rev_letter_enum_REV_D                3

/****************************************************************************
 * Enums: tx_x4_status0_rev_number_enum
 */
#define tx_x4_status0_rev_number_enum_REV_0                0
#define tx_x4_status0_rev_number_enum_REV_1                1
#define tx_x4_status0_rev_number_enum_REV_2                2
#define tx_x4_status0_rev_number_enum_REV_3                3
#define tx_x4_status0_rev_number_enum_REV_4                4
#define tx_x4_status0_rev_number_enum_REV_5                5
#define tx_x4_status0_rev_number_enum_REV_6                6
#define tx_x4_status0_rev_number_enum_REV_7                7

/****************************************************************************
 * Enums: tx_x4_status0_bonding_enum
 */
#define tx_x4_status0_bonding_enum_WIRE_BOND               0
#define tx_x4_status0_bonding_enum_FLIP_CHIP               1

/****************************************************************************
 * Enums: tx_x4_status0_tech_process
 */
#define tx_x4_status0_tech_process_PROCESS_90NM            0
#define tx_x4_status0_tech_process_PROCESS_65NM            1
#define tx_x4_status0_tech_process_PROCESS_40NM            2
#define tx_x4_status0_tech_process_PROCESS_28NM            3

/****************************************************************************
 * Enums: tx_x4_status0_model_num
 */
#define tx_x4_status0_model_num_SERDES_CL73                0
#define tx_x4_status0_model_num_XGXS_16G                   1
#define tx_x4_status0_model_num_HYPERCORE                  2
#define tx_x4_status0_model_num_HYPERLITE                  3
#define tx_x4_status0_model_num_PCIE_G2_PIPE               4
#define tx_x4_status0_model_num_SERDES_1p25GBd             5
#define tx_x4_status0_model_num_SATA2                      6
#define tx_x4_status0_model_num_QSGMII                     7
#define tx_x4_status0_model_num_XGXS10G                    8
#define tx_x4_status0_model_num_WARPCORE                   9
#define tx_x4_status0_model_num_XFICORE                    10
#define tx_x4_status0_model_num_RXFI                       11
#define tx_x4_status0_model_num_WARPLITE                   12
#define tx_x4_status0_model_num_PENTACORE                  13
#define tx_x4_status0_model_num_ESM                        14
#define tx_x4_status0_model_num_QUAD_SGMII                 15
#define tx_x4_status0_model_num_WARPCORE_3                 16
#define tx_x4_status0_model_num_TSC                        17
#define tx_x4_status0_model_num_TSC4E                      18
#define tx_x4_status0_model_num_TSC12E                     19
#define tx_x4_status0_model_num_TSC4F                      20
#define tx_x4_status0_model_num_XGXS_CL73_90NM             29
#define tx_x4_status0_model_num_SERDES_CL73_90NM           30
#define tx_x4_status0_model_num_WARPCORE3                  32
#define tx_x4_status0_model_num_WARPCORE4_TSC              33
#define tx_x4_status0_model_num_RXAUI                      34

/****************************************************************************
 * Enums: tx_x4_status0_payload
 */
#define tx_x4_status0_payload_REPEAT_2_BYTES               0
#define tx_x4_status0_payload_RAMPING                      1
#define tx_x4_status0_payload_CL48_CRPAT                   2
#define tx_x4_status0_payload_CL48_CJPAT                   3
#define tx_x4_status0_payload_CL36_LONG_CRPAT              4
#define tx_x4_status0_payload_CL36_SHORT_CRPAT             5

/****************************************************************************
 * Enums: tx_x4_status0_sc
 */
#define tx_x4_status0_sc_S_10G_CR1                         0
#define tx_x4_status0_sc_S_10G_KR1                         1
#define tx_x4_status0_sc_S_10G_X1                          2
#define tx_x4_status0_sc_S_10G_HG2_CR1                     4
#define tx_x4_status0_sc_S_10G_HG2_KR1                     5
#define tx_x4_status0_sc_S_10G_HG2_X1                      6
#define tx_x4_status0_sc_S_20G_CR1                         8
#define tx_x4_status0_sc_S_20G_KR1                         9
#define tx_x4_status0_sc_S_20G_X1                          10
#define tx_x4_status0_sc_S_20G_HG2_CR1                     12
#define tx_x4_status0_sc_S_20G_HG2_KR1                     13
#define tx_x4_status0_sc_S_20G_HG2_X1                      14
#define tx_x4_status0_sc_S_25G_CR1                         16
#define tx_x4_status0_sc_S_25G_KR1                         17
#define tx_x4_status0_sc_S_25G_X1                          18
#define tx_x4_status0_sc_S_25G_HG2_CR1                     20
#define tx_x4_status0_sc_S_25G_HG2_KR1                     21
#define tx_x4_status0_sc_S_25G_HG2_X1                      22
#define tx_x4_status0_sc_S_20G_CR2                         24
#define tx_x4_status0_sc_S_20G_KR2                         25
#define tx_x4_status0_sc_S_20G_X2                          26
#define tx_x4_status0_sc_S_20G_HG2_CR2                     28
#define tx_x4_status0_sc_S_20G_HG2_KR2                     29
#define tx_x4_status0_sc_S_20G_HG2_X2                      30
#define tx_x4_status0_sc_S_40G_CR2                         32
#define tx_x4_status0_sc_S_40G_KR2                         33
#define tx_x4_status0_sc_S_40G_X2                          34
#define tx_x4_status0_sc_S_40G_HG2_CR2                     36
#define tx_x4_status0_sc_S_40G_HG2_KR2                     37
#define tx_x4_status0_sc_S_40G_HG2_X2                      38
#define tx_x4_status0_sc_S_40G_CR4                         40
#define tx_x4_status0_sc_S_40G_KR4                         41
#define tx_x4_status0_sc_S_40G_X4                          42
#define tx_x4_status0_sc_S_40G_HG2_CR4                     44
#define tx_x4_status0_sc_S_40G_HG2_KR4                     45
#define tx_x4_status0_sc_S_40G_HG2_X4                      46
#define tx_x4_status0_sc_S_50G_CR2                         48
#define tx_x4_status0_sc_S_50G_KR2                         49
#define tx_x4_status0_sc_S_50G_X2                          50
#define tx_x4_status0_sc_S_50G_HG2_CR2                     52
#define tx_x4_status0_sc_S_50G_HG2_KR2                     53
#define tx_x4_status0_sc_S_50G_HG2_X2                      54
#define tx_x4_status0_sc_S_50G_CR4                         56
#define tx_x4_status0_sc_S_50G_KR4                         57
#define tx_x4_status0_sc_S_50G_X4                          58
#define tx_x4_status0_sc_S_50G_HG2_CR4                     60
#define tx_x4_status0_sc_S_50G_HG2_KR4                     61
#define tx_x4_status0_sc_S_50G_HG2_X4                      62
#define tx_x4_status0_sc_S_100G_CR4                        64
#define tx_x4_status0_sc_S_100G_KR4                        65
#define tx_x4_status0_sc_S_100G_X4                         66
#define tx_x4_status0_sc_S_100G_HG2_CR4                    68
#define tx_x4_status0_sc_S_100G_HG2_KR4                    69
#define tx_x4_status0_sc_S_100G_HG2_X4                     70
#define tx_x4_status0_sc_S_CL73_20GVCO                     72
#define tx_x4_status0_sc_S_CL73_25GVCO                     80
#define tx_x4_status0_sc_S_CL36_20GVCO                     88
#define tx_x4_status0_sc_S_CL36_25GVCO                     96

/****************************************************************************
 * Enums: tx_x4_status0_t_fifo_modes
 */
#define tx_x4_status0_t_fifo_modes_T_FIFO_MODE_BYPASS      0
#define tx_x4_status0_t_fifo_modes_T_FIFO_MODE_40G         1
#define tx_x4_status0_t_fifo_modes_T_FIFO_MODE_100G        2
#define tx_x4_status0_t_fifo_modes_T_FIFO_MODE_20G         3

/****************************************************************************
 * Enums: tx_x4_status0_t_enc_modes
 */
#define tx_x4_status0_t_enc_modes_T_ENC_MODE_BYPASS        0
#define tx_x4_status0_t_enc_modes_T_ENC_MODE_CL49          1
#define tx_x4_status0_t_enc_modes_T_ENC_MODE_CL82          2

/****************************************************************************
 * Enums: tx_x4_status0_btmx_mode
 */
#define tx_x4_status0_btmx_mode_BS_BTMX_MODE_1to1          0
#define tx_x4_status0_btmx_mode_BS_BTMX_MODE_2to1          1
#define tx_x4_status0_btmx_mode_BS_BTMX_MODE_5to1          2

/****************************************************************************
 * Enums: tx_x4_status0_t_type_cl82
 */
#define tx_x4_status0_t_type_cl82_T_TYPE_B1                5
#define tx_x4_status0_t_type_cl82_T_TYPE_C                 4
#define tx_x4_status0_t_type_cl82_T_TYPE_S                 3
#define tx_x4_status0_t_type_cl82_T_TYPE_T                 2
#define tx_x4_status0_t_type_cl82_T_TYPE_D                 1
#define tx_x4_status0_t_type_cl82_T_TYPE_E                 0

/****************************************************************************
 * Enums: tx_x4_status0_txsm_state_cl82
 */
#define tx_x4_status0_txsm_state_cl82_TX_HIG_END           6
#define tx_x4_status0_txsm_state_cl82_TX_HIG_START         5
#define tx_x4_status0_txsm_state_cl82_TX_E                 4
#define tx_x4_status0_txsm_state_cl82_TX_T                 3
#define tx_x4_status0_txsm_state_cl82_TX_D                 2
#define tx_x4_status0_txsm_state_cl82_TX_C                 1
#define tx_x4_status0_txsm_state_cl82_TX_INIT              0

/****************************************************************************
 * Enums: tx_x4_status0_ltxsm_state_cl82
 */
#define tx_x4_status0_ltxsm_state_cl82_TX_HIG_END          64
#define tx_x4_status0_ltxsm_state_cl82_TX_HIG_START        32
#define tx_x4_status0_ltxsm_state_cl82_TX_E                16
#define tx_x4_status0_ltxsm_state_cl82_TX_T                8
#define tx_x4_status0_ltxsm_state_cl82_TX_D                4
#define tx_x4_status0_ltxsm_state_cl82_TX_C                2
#define tx_x4_status0_ltxsm_state_cl82_TX_INIT             1

/****************************************************************************
 * Enums: tx_x4_status0_r_type_coded_cl82
 */
#define tx_x4_status0_r_type_coded_cl82_R_TYPE_B1          32
#define tx_x4_status0_r_type_coded_cl82_R_TYPE_C           16
#define tx_x4_status0_r_type_coded_cl82_R_TYPE_S           8
#define tx_x4_status0_r_type_coded_cl82_R_TYPE_T           4
#define tx_x4_status0_r_type_coded_cl82_R_TYPE_D           2
#define tx_x4_status0_r_type_coded_cl82_R_TYPE_E           1

/****************************************************************************
 * Enums: tx_x4_status0_rxsm_state_cl82
 */
#define tx_x4_status0_rxsm_state_cl82_RX_HIG_END           64
#define tx_x4_status0_rxsm_state_cl82_RX_HIG_START         32
#define tx_x4_status0_rxsm_state_cl82_RX_E                 16
#define tx_x4_status0_rxsm_state_cl82_RX_T                 8
#define tx_x4_status0_rxsm_state_cl82_RX_D                 4
#define tx_x4_status0_rxsm_state_cl82_RX_C                 2
#define tx_x4_status0_rxsm_state_cl82_RX_INIT              1

/****************************************************************************
 * Enums: tx_x4_status0_deskew_state
 */
#define tx_x4_status0_deskew_state_ALIGN_ACQUIRED          2
#define tx_x4_status0_deskew_state_LOSS_OF_ALIGNMENT       1

/****************************************************************************
 * Enums: tx_x4_status0_os_mode_enum
 */
#define tx_x4_status0_os_mode_enum_OS_MODE_1               0
#define tx_x4_status0_os_mode_enum_OS_MODE_2               1
#define tx_x4_status0_os_mode_enum_OS_MODE_4               2
#define tx_x4_status0_os_mode_enum_OS_MODE_16p5            8
#define tx_x4_status0_os_mode_enum_OS_MODE_20p625          12

/****************************************************************************
 * Enums: tx_x4_status0_scr_modes
 */
#define tx_x4_status0_scr_modes_T_SCR_MODE_BYPASS          0
#define tx_x4_status0_scr_modes_T_SCR_MODE_CL49            1
#define tx_x4_status0_scr_modes_T_SCR_MODE_40G_2_LANE      2
#define tx_x4_status0_scr_modes_T_SCR_MODE_100G            3
#define tx_x4_status0_scr_modes_T_SCR_MODE_20G             4
#define tx_x4_status0_scr_modes_T_SCR_MODE_40G_4_LANE      5

/****************************************************************************
 * Enums: tx_x4_status0_descr_modes
 */
#define tx_x4_status0_descr_modes_R_DESCR_MODE_BYPASS      0
#define tx_x4_status0_descr_modes_R_DESCR_MODE_CL49        1
#define tx_x4_status0_descr_modes_R_DESCR_MODE_CL82        2

/****************************************************************************
 * Enums: tx_x4_status0_r_dec_tl_mode
 */
#define tx_x4_status0_r_dec_tl_mode_R_DEC_TL_MODE_BYPASS   0
#define tx_x4_status0_r_dec_tl_mode_R_DEC_TL_MODE_CL49     1
#define tx_x4_status0_r_dec_tl_mode_R_DEC_TL_MODE_CL82     2

/****************************************************************************
 * Enums: tx_x4_status0_r_dec_fsm_mode
 */
#define tx_x4_status0_r_dec_fsm_mode_R_DEC_FSM_MODE_BYPASS 0
#define tx_x4_status0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL49   1
#define tx_x4_status0_r_dec_fsm_mode_R_DEC_FSM_MODE_CL82   2

/****************************************************************************
 * Enums: tx_x4_status0_r_deskew_mode
 */
#define tx_x4_status0_r_deskew_mode_R_DESKEW_MODE_BYPASS   0
#define tx_x4_status0_r_deskew_mode_R_DESKEW_MODE_20G      1
#define tx_x4_status0_r_deskew_mode_R_DESKEW_MODE_40G_4_LANE 2
#define tx_x4_status0_r_deskew_mode_R_DESKEW_MODE_40G_2_LANE 3
#define tx_x4_status0_r_deskew_mode_R_DESKEW_MODE_100G     4
#define tx_x4_status0_r_deskew_mode_R_DESKEW_MODE_CL49     5
#define tx_x4_status0_r_deskew_mode_R_DESKEW_MODE_CL91     6

/****************************************************************************
 * Enums: tx_x4_status0_bs_dist_modes
 */
#define tx_x4_status0_bs_dist_modes_BS_DIST_MODE_5_LANE_TDM 0
#define tx_x4_status0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_2_VLANE 1
#define tx_x4_status0_bs_dist_modes_BS_DIST_MODE_2_LANE_TDM_1_VLANE 2
#define tx_x4_status0_bs_dist_modes_BS_DIST_MODE_NO_TDM    3

/****************************************************************************
 * Enums: tx_x4_status0_cl49_t_type
 */
#define tx_x4_status0_cl49_t_type_BAD_T_TYPE               15
#define tx_x4_status0_cl49_t_type_T_TYPE_B0                11
#define tx_x4_status0_cl49_t_type_T_TYPE_OB                10
#define tx_x4_status0_cl49_t_type_T_TYPE_B1                9
#define tx_x4_status0_cl49_t_type_T_TYPE_DB                8
#define tx_x4_status0_cl49_t_type_T_TYPE_FC                7
#define tx_x4_status0_cl49_t_type_T_TYPE_TB                6
#define tx_x4_status0_cl49_t_type_T_TYPE_LI                5
#define tx_x4_status0_cl49_t_type_T_TYPE_C                 4
#define tx_x4_status0_cl49_t_type_T_TYPE_S                 3
#define tx_x4_status0_cl49_t_type_T_TYPE_T                 2
#define tx_x4_status0_cl49_t_type_T_TYPE_D                 1
#define tx_x4_status0_cl49_t_type_T_TYPE_E                 0

/****************************************************************************
 * Enums: tx_x4_status0_cl49_txsm_states
 */
#define tx_x4_status0_cl49_txsm_states_TX_HIG_END          7
#define tx_x4_status0_cl49_txsm_states_TX_HIG_START        6
#define tx_x4_status0_cl49_txsm_states_TX_LI               5
#define tx_x4_status0_cl49_txsm_states_TX_E                4
#define tx_x4_status0_cl49_txsm_states_TX_T                3
#define tx_x4_status0_cl49_txsm_states_TX_D                2
#define tx_x4_status0_cl49_txsm_states_TX_C                1
#define tx_x4_status0_cl49_txsm_states_TX_INIT             0

/****************************************************************************
 * Enums: tx_x4_status0_cl49_ltxsm_states
 */
#define tx_x4_status0_cl49_ltxsm_states_TX_HIG_END         128
#define tx_x4_status0_cl49_ltxsm_states_TX_HIG_START       64
#define tx_x4_status0_cl49_ltxsm_states_TX_LI              32
#define tx_x4_status0_cl49_ltxsm_states_TX_E               16
#define tx_x4_status0_cl49_ltxsm_states_TX_T               8
#define tx_x4_status0_cl49_ltxsm_states_TX_D               4
#define tx_x4_status0_cl49_ltxsm_states_TX_C               2
#define tx_x4_status0_cl49_ltxsm_states_TX_INIT            1

/****************************************************************************
 * Enums: tx_x4_status0_burst_error_mode
 */
#define tx_x4_status0_burst_error_mode_BURST_ERROR_11_BITS 0
#define tx_x4_status0_burst_error_mode_BURST_ERROR_16_BITS 1
#define tx_x4_status0_burst_error_mode_BURST_ERROR_17_BITS 2
#define tx_x4_status0_burst_error_mode_BURST_ERROR_18_BITS 3
#define tx_x4_status0_burst_error_mode_BURST_ERROR_19_BITS 4
#define tx_x4_status0_burst_error_mode_BURST_ERROR_20_BITS 5
#define tx_x4_status0_burst_error_mode_BURST_ERROR_21_BITS 6
#define tx_x4_status0_burst_error_mode_BURST_ERROR_22_BITS 7
#define tx_x4_status0_burst_error_mode_BURST_ERROR_23_BITS 8
#define tx_x4_status0_burst_error_mode_BURST_ERROR_24_BITS 9
#define tx_x4_status0_burst_error_mode_BURST_ERROR_25_BITS 10
#define tx_x4_status0_burst_error_mode_BURST_ERROR_26_BITS 11
#define tx_x4_status0_burst_error_mode_BURST_ERROR_27_BITS 12
#define tx_x4_status0_burst_error_mode_BURST_ERROR_28_BITS 13
#define tx_x4_status0_burst_error_mode_BURST_ERROR_29_BITS 14
#define tx_x4_status0_burst_error_mode_BURST_ERROR_30_BITS 15
#define tx_x4_status0_burst_error_mode_BURST_ERROR_31_BITS 16
#define tx_x4_status0_burst_error_mode_BURST_ERROR_32_BITS 17
#define tx_x4_status0_burst_error_mode_BURST_ERROR_33_BITS 18
#define tx_x4_status0_burst_error_mode_BURST_ERROR_34_BITS 19
#define tx_x4_status0_burst_error_mode_BURST_ERROR_35_BITS 20
#define tx_x4_status0_burst_error_mode_BURST_ERROR_36_BITS 21
#define tx_x4_status0_burst_error_mode_BURST_ERROR_37_BITS 22
#define tx_x4_status0_burst_error_mode_BURST_ERROR_38_BITS 23
#define tx_x4_status0_burst_error_mode_BURST_ERROR_39_BITS 24
#define tx_x4_status0_burst_error_mode_BURST_ERROR_40_BITS 25
#define tx_x4_status0_burst_error_mode_BURST_ERROR_41_BITS 26

/****************************************************************************
 * Enums: tx_x4_status0_bermon_state
 */
#define tx_x4_status0_bermon_state_HI_BER                  4
#define tx_x4_status0_bermon_state_GOOD_BER                3
#define tx_x4_status0_bermon_state_BER_TEST_SH             2
#define tx_x4_status0_bermon_state_START_TIMER             1
#define tx_x4_status0_bermon_state_BER_MT_INIT             0

/****************************************************************************
 * Enums: tx_x4_status0_rxsm_state_cl49
 */
#define tx_x4_status0_rxsm_state_cl49_RX_HIG_END           128
#define tx_x4_status0_rxsm_state_cl49_RX_HIG_START         64
#define tx_x4_status0_rxsm_state_cl49_RX_LI                32
#define tx_x4_status0_rxsm_state_cl49_RX_E                 16
#define tx_x4_status0_rxsm_state_cl49_RX_T                 8
#define tx_x4_status0_rxsm_state_cl49_RX_D                 4
#define tx_x4_status0_rxsm_state_cl49_RX_C                 2
#define tx_x4_status0_rxsm_state_cl49_RX_INIT              1

/****************************************************************************
 * Enums: tx_x4_status0_r_type
 */
#define tx_x4_status0_r_type_BAD_R_TYPE                    15
#define tx_x4_status0_r_type_R_TYPE_B0                     11
#define tx_x4_status0_r_type_R_TYPE_OB                     10
#define tx_x4_status0_r_type_R_TYPE_B1                     9
#define tx_x4_status0_r_type_R_TYPE_DB                     8
#define tx_x4_status0_r_type_R_TYPE_FC                     7
#define tx_x4_status0_r_type_R_TYPE_TB                     6
#define tx_x4_status0_r_type_R_TYPE_LI                     5
#define tx_x4_status0_r_type_R_TYPE_C                      4
#define tx_x4_status0_r_type_R_TYPE_S                      3
#define tx_x4_status0_r_type_R_TYPE_T                      2
#define tx_x4_status0_r_type_R_TYPE_D                      1
#define tx_x4_status0_r_type_R_TYPE_E                      0

/****************************************************************************
 * Enums: tx_x4_status0_am_lock_state
 */
#define tx_x4_status0_am_lock_state_INVALID_AM             512
#define tx_x4_status0_am_lock_state_GOOD_AM                256
#define tx_x4_status0_am_lock_state_COMP_AM                128
#define tx_x4_status0_am_lock_state_TIMER_2                64
#define tx_x4_status0_am_lock_state_AM_2_GOOD              32
#define tx_x4_status0_am_lock_state_COMP_2ND               16
#define tx_x4_status0_am_lock_state_TIMER_1                8
#define tx_x4_status0_am_lock_state_FIND_1ST               4
#define tx_x4_status0_am_lock_state_AM_RESET_CNT           2
#define tx_x4_status0_am_lock_state_AM_LOCK_INIT           1

/****************************************************************************
 * Enums: tx_x4_status0_msg_selector
 */
#define tx_x4_status0_msg_selector_RESERVED                0
#define tx_x4_status0_msg_selector_VALUE_802p3             1
#define tx_x4_status0_msg_selector_VALUE_802p9             2
#define tx_x4_status0_msg_selector_VALUE_802p5             3
#define tx_x4_status0_msg_selector_VALUE_1394              4

/****************************************************************************
 * Enums: tx_x4_status0_synce_enum
 */
#define tx_x4_status0_synce_enum_SYNCE_NO_DIV              0
#define tx_x4_status0_synce_enum_SYNCE_DIV_7               1
#define tx_x4_status0_synce_enum_SYNCE_DIV_11              2

/****************************************************************************
 * Enums: tx_x4_status0_synce_enum_stage0
 */
#define tx_x4_status0_synce_enum_stage0_SYNCE_NO_DIV       0
#define tx_x4_status0_synce_enum_stage0_SYNCE_DIV_GAP_CLK_4_OVER_5 1
#define tx_x4_status0_synce_enum_stage0_SYNCE_DIV_SDM_FRAC_DIV 2

/****************************************************************************
 * Enums: tx_x4_status0_cl91_sync_state
 */
#define tx_x4_status0_cl91_sync_state_FIND_1ST             0
#define tx_x4_status0_cl91_sync_state_COUNT_NEXT           1
#define tx_x4_status0_cl91_sync_state_COMP_2ND             2
#define tx_x4_status0_cl91_sync_state_TWO_GOOD             3

/****************************************************************************
 * Enums: tx_x4_status0_cl91_algn_state
 */
#define tx_x4_status0_cl91_algn_state_LOSS_OF_ALIGNMENT    0
#define tx_x4_status0_cl91_algn_state_DESKEW               1
#define tx_x4_status0_cl91_algn_state_DESKEW_FAIL          2
#define tx_x4_status0_cl91_algn_state_ALIGN_ACQUIRED       3
#define tx_x4_status0_cl91_algn_state_CW_GOOD              4
#define tx_x4_status0_cl91_algn_state_CW_BAD               5
#define tx_x4_status0_cl91_algn_state_THREE_BAD            6

/****************************************************************************
 * Enums: tx_x4_status0_fec_sel
 */
#define tx_x4_status0_fec_sel_NO_FEC                       0
#define tx_x4_status0_fec_sel_FEC_CL74                     1
#define tx_x4_status0_fec_sel_FEC_CL91                     2

/****************************************************************************
 * End of RDB defined Enums
 */
