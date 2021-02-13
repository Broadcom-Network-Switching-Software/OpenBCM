/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 ############### THIS FILE IS AUTOMATICALLY GENERATED.  ###############
 ############### DO !! NOT !! MANUALLY EDIT THIS FILE.  ###############
 *---------------------------------------------------------------------
 * Description: This file contains enums, elems and doxyten comments
 * needed for SerDes Configuration programs.
 *---------------------------------------------------------------------
 * CVS INFORMATION:
 * Please see inc/enum_desc.txt for CVS information.
 *----------------------------------------------------------------------
 */

/* This file is automatically generated. Do not modify it. Modify the
 * inc/enum_desc.txt to change enums, elems, or comments. For issues about
 * the process that creates this file contact the tefmod16 development team.
 */

#include "tefmod16_enum_defines.h"

char* e2s_tefmod16_lane_select_t [CNT_tefmod_lane_select_t] = {
  "TEFMOD16_LANE_0_0_0_0" ,
  "TEFMOD16_LANE_0_0_0_1" ,
  "TEFMOD16_LANE_0_0_1_0" ,
  "TEFMOD16_LANE_0_0_1_1" ,
  "TEFMOD16_LANE_0_1_0_0" ,
  "TEFMOD16_LANE_0_1_0_1" ,
  "TEFMOD16_LANE_0_1_1_0" ,
  "TEFMOD16_LANE_0_1_1_1" ,
  "TEFMOD16_LANE_1_0_0_0" ,
  "TEFMOD16_LANE_1_0_0_1" ,
  "TEFMOD16_LANE_1_0_1_0" ,
  "TEFMOD16_LANE_1_0_1_1" ,
  "TEFMOD16_LANE_1_1_0_0" ,
  "TEFMOD16_LANE_1_1_0_1" ,
  "TEFMOD16_LANE_1_1_1_0" ,
  "TEFMOD16_LANE_1_1_1_1" ,
  "TEFMOD16_LANE_BCST" ,
  "TEFMOD16_LANE_ILLEGAL" 
}; /* e2s_tefmod16_lane_select_t */

int e2n_tefmod16_lane_select_t [CNT_tefmod_lane_select_t] = {
  0 ,
  1 ,
  2 ,
  3 ,
  4 ,
  5 ,
  6 ,
  7 ,
  8 ,
  9 ,
  10 ,
  11 ,
  12 ,
  13 ,
  14 ,
  15 ,
  15 ,
  0 
}; /* e2n_tefmod16_lane_select_t */

char* e2s_tefmod16_spd_intfc_type_t [CNT_tefmod_spd_intfc_type_t] = {
  "TEFMOD16_SPD_ZERO" ,
  "TEFMOD16_SPD_10_SGMII" ,
  "TEFMOD16_SPD_100_SGMII" ,
  "TEFMOD16_SPD_1000_SGMII" ,
  "TEFMOD16_SPD_2500" ,
  "TEFMOD16_SPD_10000_XFI" ,
  "TEFMOD16_SPD_10600_XFI_HG" ,
  "TEFMOD16_SPD_20000_XFI" ,
  "TEFMOD16_SPD_21200_XFI_HG" ,
  "TEFMOD16_SPD_25000_XFI" ,
  "TEFMOD16_SPD_26500_XFI_HG" ,
  "TEFMOD16_SPD_20G_MLD_X2" ,
  "TEFMOD16_SPD_21G_MLD_HG_X2" ,
  "TEFMOD16_SPD_40G_MLD_X2" ,
  "TEFMOD16_SPD_42G_MLD_HG_X2" ,
  "TEFMOD16_SPD_40G_MLD_X4" ,
  "TEFMOD16_SPD_42G_MLD_HG_X4" ,
  "TEFMOD16_SPD_50G_MLD_X2" ,
  "TEFMOD16_SPD_53G_MLD_HG_X2" ,
  "TEFMOD16_SPD_100G_MLD_X4" ,
  "TEFMOD16_SPD_106G_MLD_HG_X4" ,
  "TEFMOD16_SPD_10000_HI" ,
  "TEFMOD16_SPD_10000" ,
  "TEFMOD16_SPD_12000_HI" ,
  "TEFMOD16_SPD_13000" ,
  "TEFMOD16_SPD_15000" ,
  "TEFMOD16_SPD_16000" ,
  "TEFMOD16_SPD_20000" ,
  "TEFMOD16_SPD_20000_SCR" ,
  "TEFMOD16_SPD_21000" ,
  "TEFMOD16_SPD_25455" ,
  "TEFMOD16_SPD_31500" ,
  "TEFMOD16_SPD_40G_X4" ,
  "TEFMOD16_SPD_42G_X4" ,
  "TEFMOD16_SPD_40G_XLAUI" ,
  "TEFMOD16_SPD_42G_XLAUI" ,
  "TEFMOD16_SPD_10000_X2" ,
  "TEFMOD16_SPD_10000_HI_DXGXS" ,
  "TEFMOD16_SPD_10000_DXGXS" ,
  "TEFMOD16_SPD_10000_HI_DXGXS_SCR" ,
  "TEFMOD16_SPD_10000_DXGXS_SCR" ,
  "TEFMOD16_SPD_10500_HI_DXGXS" ,
  "TEFMOD16_SPD_12773_HI_DXGXS" ,
  "TEFMOD16_SPD_12773_DXGXS" ,
  "TEFMOD16_SPD_15750_HI_DXGXS" ,
  "TEFMOD16_SPD_20G_MLD_DXGXS" ,
  "TEFMOD16_SPD_21G_HI_MLD_DXGXS" ,
  "TEFMOD16_SPD_20G_DXGXS" ,
  "TEFMOD16_SPD_21G_HI_DXGXS" ,
  "TEFMOD16_SPD_100G_CR10" ,
  "TEFMOD16_SPD_120G_CR12" ,
  "TEFMOD16_SPD_10000_XFI_HG2" ,
  "TEFMOD16_SPD_50G_MLD_X4" ,
  "TEFMOD16_SPD_53G_MLD_HG_X4" ,
  "TEFMOD16_SPD_10000_XFI_CR1" ,
  "TEFMOD16_SPD_10600_XFI_HG_CR1" ,
  "TEFMOD16_SPD_CL73_20G" ,
  "TEFMOD16_SPD_CL73_25G" ,
  "TEFMOD16_SPD_1G_20G" ,
  "TEFMOD16_SPD_1G_25G" ,
  "TEFMOD16_SPD_ILLEGAL" 
}; /* e2s_tefmod16_spd_intfc_type_t */

int e2n_tefmod16_spd_intfc_type_t [CNT_tefmod_spd_intfc_type_t] = {
  0 ,
  10 ,
  100 ,
  1000 ,
  2500 ,
  10000 ,
  10560 ,
  20000 ,
  21200 ,
  25000 ,
  26500 ,
  20000 ,
  21000 ,
  40000 ,
  42400 ,
  40000 ,
  42400 ,
  50000 ,
  53000 ,
  100000 ,
  106000 ,
  10000 ,
  10000 ,
  12000 ,
  13000 ,
  15000 ,
  16000 ,
  20000 ,
  20000 ,
  21000 ,
  25000 ,
  31500 ,
  40000 ,
  40000 ,
  40000 ,
  42000 ,
  10000 ,
  10000 ,
  10000 ,
  10000 ,
  10000 ,
  10500 ,
  12773 ,
  12773 ,
  15750 ,
  20000 ,
  20000 ,
  20000 ,
  21000 ,
  100000 ,
  120000 ,
  10000 ,
  50000 ,
  53000 ,
  54000 ,
  55000 ,
  56000 ,
  57000 ,
  58000 ,
  59000 ,
  0 
}; /* e2n_tefmod16_spd_intfc_type_t */

char* e2s_tefmod16_regacc_type_t [CNT_tefmod_regacc_type_t] = {
  "TEFMOD16_REGACC_CL22" ,
  "TEFMOD16_REGACC_CL45" ,
  "TEFMOD16_REGACC_TOTSC" ,
  "TEFMOD16_REGACC_SBUS_FD" ,
  "TEFMOD16_REGACC_SBUS_BD" ,
  "TEFMOD16_REGACC_ILLEGAL" 
}; /* e2s_tefmod16_regacc_type_t */

char* e2s_tefmod16_port_type_t [CNT_tefmod_port_type_t] = {
  "TEFMOD16_MULTI_PORT" ,
  "TEFMOD16_TRI1_PORT" ,
  "TEFMOD16_TRI2_PORT" ,
  "TEFMOD16_DXGXS" ,
  "TEFMOD16_SINGLE_PORT" ,
  "TEFMOD16_PORT_MODE_ILLEGAL" 
}; /* e2s_tefmod16_port_type_t */

char* e2s_tefmod16_sc_mode_type_t [CNT_tefmod_sc_mode_type_t] = {
  "TEFMOD16_SC_MODE_HT_WITH_BASIC_OVERRIDE" ,
  "TEFMOD16_SC_MODE_HT_OVERRIDE" ,
  "TEFMOD16_SC_MODE_ST" ,
  "TEFMOD16_SC_MODE_ST_OVERRIDE" ,
  "TEFMOD16_SC_MODE_AN_CL37" ,
  "TEFMOD16_SC_MODE_AN_CL73" ,
  "TEFMOD16_SC_MODE_BYPASS" ,
  "TEFMOD16_SC_MODE_ILLEGAL" 
}; /* e2s_tefmod16_sc_mode_type_t */

char* e2s_tefmod16_diag_type_t [CNT_tefmod_diag_type_t] = {
  "TEFMOD16_DIAG_GENERAL" ,
  "TEFMOD16_DIAG_TOPOLOGY" ,
  "TEFMOD16_DIAG_LINK" ,
  "TEFMOD16_DIAG_SPEED" ,
  "TEFMOD16_DIAG_ANEG" ,
  "TEFMOD16_DIAG_TFC" ,
  "TEFMOD16_DIAG_AN_TIMERS" ,
  "TEFMOD16_DIAG_STATE" ,
  "TEFMOD16_DIAG_DEBUG" ,
  "TEFMOD16_DIAG_IEEE" ,
  "TEFMOD16_DIAG_EEE" ,
  "TEFMOD16_DIAG_BER" ,
  "TEFMOD16_DIAG_CFG" ,
  "TEFMOD16_DIAG_CL72" ,
  "TEFMOD16_DIAG_DSC" ,
  "TEFMOD16_SERDES_DIAG" ,
  "TEFMOD16_DIAG_ALL" ,
  "TEFMOD16_DIAG_ILLEGAL" 
}; /* e2s_tefmod16_diag_type_t */

char* e2s_tefmod16_model_type_t [CNT_tefmod_model_type_t] = {
  "TEFMOD16_WC" ,
  "TEFMOD16_WC_A0" ,
  "TEFMOD16_WC_A1" ,
  "TEFMOD16_WC_A2" ,
  "TEFMOD16_WC_B0" ,
  "TEFMOD16_WC_B1" ,
  "TEFMOD16_WC_B2" ,
  "TEFMOD16_WC_C0" ,
  "TEFMOD16_WC_C1" ,
  "TEFMOD16_WC_C2" ,
  "TEFMOD16_WC_D0" ,
  "TEFMOD16_WC_D1" ,
  "TEFMOD16_WC_D2" ,
  "TEFMOD16_XN" ,
  "TEFMOD16_WL" ,
  "TEFMOD16_WL_A0" ,
  "TEFMOD16_QS" ,
  "TEFMOD16_QS_A0" ,
  "TEFMOD16_QS_B0" ,
  "TEFMOD16_MODEL_TYPE_ILLEGAL" 
}; /* e2s_tefmod16_model_type_t */

char* e2s_tefmod16_an_type_t [CNT_tefmod_an_type_t] = {
  "TEFMOD16_CL73",
  "TEFMOD16_CL73_BAM",
  "TEFMOD16_HPAM",
  "TEFMOD16_MSA",
  "TEFMOD16_CL73_MSA",
  "TEFMOD16_AN_TYPE_ILLEGAL" 
}; /* e2s_tefmod16_an_type_t */

char* e2s_tefmod16_eye_direction_t [CNT_tefmod_eye_direction_t] = {
  "TEFMOD16_EYE_VU" ,
  "TEFMOD16_EYE_VD" ,
  "TEFMOD16_EYE_HL" ,
  "TEFMOD16_EYE_HR" ,
  "TEFMOD16_EYE_ILLEGAL" 
}; /* e2s_tefmod16_eye_direction_t */


char* e2s_tefmod16_tech_ability_t [CNT_tefmod_tech_ability_t] = {
  "TEFMOD16_ABILITY_1G" ,
  "TEFMOD16_ABILITY_10G_KR" ,
  "TEFMOD16_ABILITY_40G_KR4" ,
  "TEFMOD16_ABILITY_40G_CR4" ,
  "TEFMOD16_ABILITY_100G_CR10" ,
  "TEFMOD16_ABILITY_10G_HIGIG" ,
  "TEFMOD16_ABILITY_10G_CX4" ,
  "TEFMOD16_ABILITY_20G_X4" ,
  "TEFMOD16_ABILITY_40G" ,
  "TEFMOD16_ABILITY_25P455G" ,
  "TEFMOD16_ABILITY_21G_X4" ,
  "TEFMOD16_ABILITY_20G_X4S" ,
  "TEFMOD16_ABILITY_10G_DXGXS_HIGIG" ,
  "TEFMOD16_ABILITY_10G_DXGXS" ,
  "TEFMOD16_ABILITY_10P5G_DXGXS" ,
  "TEFMOD16_ABILITY_12P5_DXGXS" ,
  "TEFMOD16_ABILITY_20G_KR2_HIGIG" ,
  "TEFMOD16_ABILITY_20G_KR2" ,
  "TEFMOD16_ABILITY_20G_CR2" ,
  "TEFMOD16_ABILITY_15P75G_R2" ,
  "TEFMOD16_ABILITY_100G_KR4" ,
  "TEFMOD16_ABILITY_100G_CR4" ,
  "TEFMOD16_ABILITY_40G_KR2" ,
  "TEFMOD16_ABILITY_40G_CR2" ,
  "TEFMOD16_ABILITY_50G_KR2" ,
  "TEFMOD16_ABILITY_50G_CR2" ,
  "TEFMOD16_ABILITY_50G_KR4" ,
  "TEFMOD16_ABILITY_50G_CR4" ,
  "TEFMOD16_ABILITY_20G_KR1" ,
  "TEFMOD16_ABILITY_20G_CR1" ,
  "TEFMOD16_ABILITY_25G_KR1" ,
  "TEFMOD16_ABILITY_25G_CR1" ,
  "TEFMOD16_ABILITY_ILLEGAL" 
}; /* e2s_tefmod16_tech_ability_t */

int e2n_tefmod16_tech_ability_t [CNT_tefmod_tech_ability_t] = {
  0x6 ,
  0x7 ,
  0x8 ,
  0x9 ,
  0xa ,
  0x103 ,
  0x104 ,
  0x10a ,
  0x305 ,
  0x308 ,
  0x309 ,
  0x400 ,
  0x401 ,
  0x402 ,
  0x403 ,
  0x404 ,
  0x405 ,
  0x406 ,
  0x406 ,
  0x407 ,
  0x408 ,
  0x409 ,
  0x40a ,
  0x500 ,
  0x501 ,
  0x502 ,
  0x503 ,
  0x504 ,
  0x505 ,
  0x506 ,
  0x507 ,
  0x508 ,
  0x999 
}; /* e2n_tefmod16_tech_ability_t */

char* e2s_tefmod16_cl37bam_ability_t [CNT_tefmod_cl37bam_ability_t] = {
  "TEFMOD16_BAM37ABL_2P5G" ,
  "TEFMOD16_BAM37ABL_5G_X4" ,
  "TEFMOD16_BAM37ABL_6G_X4" ,
  "TEFMOD16_BAM37ABL_10G_HIGIG" ,
  "TEFMOD16_BAM37ABL_10G_CX4" ,
  "TEFMOD16_BAM37ABL_12G_X4" ,
  "TEFMOD16_BAM37ABL_12P5_X4" ,
  "TEFMOD16_BAM37ABL_13G_X4" ,
  "TEFMOD16_BAM37ABL_15G_X4" ,
  "TEFMOD16_BAM37ABL_16G_X4" ,
  "TEFMOD16_BAM37ABL_20G_X4_CX4" ,
  "TEFMOD16_BAM37ABL_20G_X4" ,
  "TEFMOD16_BAM37ABL_21G_X4" ,
  "TEFMOD16_BAM37ABL_25P455G" ,
  "TEFMOD16_BAM37ABL_31P5G" ,
  "TEFMOD16_BAM37ABL_32P7G" ,
  "TEFMOD16_BAM37ABL_40G" ,
  "TEFMOD16_BAM37ABL_10G_X2_CX4" ,
  "TEFMOD16_BAM37ABL_10G_DXGXS" ,
  "TEFMOD16_BAM37ABL_10P5G_DXGXS" ,
  "TEFMOD16_BAM37ABL_12P7_DXGXS" ,
  "TEFMOD16_BAM37ABL_15P75G_R2" ,
  "TEFMOD16_BAM37ABL_20G_X2_CX4" ,
  "TEFMOD16_BAM37ABL_20G_X2" ,
  "TEFMOD16_BAM37ABL_ILLEGAL" 
}; /* e2s_tefmod16_cl37bam_ability_t */

char* e2s_tefmod16_diag_an_type_t [CNT_tefmod_diag_an_type_t] = {
  "TEFMOD16_DIAG_AN_DONE" ,
  "TEFMOD16_DIAG_AN_HCD" ,
  "TEFMOD16_DIAG_AN_TYPE_ILLEGAL" 
}; /* e2s_tefmod16_diag_an_type_t */

char* e2s_tefmod16_tier1_function_type_t [CNT_tefmod_tier1_function_type_t] = {
  "PCS_BYPASS_CTL" ,
  "CREDIT_SET" ,
  "ENCODE_SET" ,
  "DECODE_SET" ,
  "CREDIT_CONTROL" ,
  "AFE_SPEED_UP_DSC_VGA" ,
  "TX_LANE_CONTROL" ,
  "RX_LANE_CONTROL" ,
  "TX_LANE_DISABLE" ,
  "POWER_CONTROL" ,
  "AUTONEG_SET" ,
  "AUTONEG_GET" ,
  "AUTONEG_CONTROL" ,
  "AUTONEG_PAGE_SET" ,
  "REG_READ" ,
  "REG_WRITE" ,
  "PRBS_CHECK" ,
  "CJPAT_CRPAT_CONTROL" ,
  "CJPAT_CRPAT_CHECK" ,
  "TEFMOD16_DIAG" ,
  "LANE_SWAP" ,
  "PARALLEL_DETECT_CONTROL" ,
  "CLAUSE_72_CONTROL" ,
  "PLL_SEQUENCER_CONTROL" ,
  "PLL_LOCK_WAIT" ,
  "DUPLEX_CONTROL" ,
  "REVID_READ" ,
  "BYPASS_SC" ,
  "SET_POLARITY" ,
  "SET_PORT_MODE" ,
  "SET_AN_PORT_MODE" ,
  "PRBS_CONTROL" ,
  "PRBS_MODE" ,
  "SOFT_RESET" ,
  "SET_SPD_INTF" ,
  "TX_BERT_CONTROL" ,
  "RX_LOOPBACK_CONTROL" ,
  "RX_PMD_LOOPBACK_CONTROL" ,
  "TX_LOOPBACK_CONTROL" ,
  "TX_PMD_LOOPBACK_CONTROL" ,
  "CORE_RESET" ,
  "REFCLK_SET" ,
  "WAIT_PMD_LOCK" ,
  "FIRMWARE_SET" ,
  "INIT_PCS_FALCON" ,
  "DISABLE_PCS_FALCON" ,
  "INIT_PMD_FALCON" ,
  "PMD_LANE_SWAP_TX" ,
  "PMD_LANE_SWAP" ,
  "PCS_LANE_SWAP" ,
  "SET_SC_SPEED" ,
  "CHECK_SC_STATS" ,
  "SET_OVERRIDE_0" ,
  "SET_OVERRIDE_1" ,
  "PMD_RESET_REMOVE" ,
  "PMD_RESET_BYPASS" ,
  "INIT_PCS_ILKN" ,
  "TOGGLE_SW_SPEED_CHANGE" ,
  "TIER1_FUNCTION_ILLEGAL" 
}; /* e2s_tefmod16_tier1_function_type_t */

char* e2s_tefmod16_fec_en_parm_t [CNT_tefmod_fec_en_parm_t] = {
  "TEFMOD16_CL91_TX_EN_DIS" ,
  "TEFMOD16_CL91_RX_EN_DIS" ,
  "TEFMOD16_CL91_IND_ONLY_EN_DIS" ,
  "TEFMOD16_CL91_COR_ONLY_EN_DIS" ,
  "TEFMOD16_CL74_TX_EN_DIS" ,
  "TEFMOD16_CL74_RX_EN_DIS" ,
  "TEFMOD16_CL74_CL91_EN_DIS" 
}; /* e2s_tefmod16_fec_en_parm_t */

char* e2s_tefmod16_fec_type_t [CNT_tefmod_fec_type_t] = {
  "TEFMOD16_CL74" ,
  "TEFMOD16_CL91" ,
  "TEFMOD16_CL108"
}; /* e2s_tefmod16_fec_type_t */


char* e2s_tefmod16_tx_disable_enum_t [CNT_tefmod_tx_disable_enum_t] = {
  "TEFMOD16_TX_LANE_TRAFFIC" ,
  "TEFMOD16_TX_LANE_RESET" 
}; /* e2s_tefmod16_tx_disable_enum_t */

char* e2s_tefmod16_os_mode_type [CNT_tefmod_os_mode_type] = {
  "TEFMOD16_PMA_OS_MODE_1" ,
  "TEFMOD16_PMA_OS_MODE_2" ,
  "TEFMOD16_PMA_OS_MODE_4" ,
  "TEFMOD16_PMA_OS_MODE_16_25" ,
  "TEFMOD16_PMA_OS_MODE_20_625" ,
  "TEFMOD16_PMA_OS_MODE_ILLEGAL" 
}; /* e2s_tefmod16_os_mode_type */

int e2n_tefmod16_os_mode_type [CNT_tefmod_os_mode_type] = {
  1 ,
  2 ,
  4 ,
  16 ,
  10 ,
  0 
}; /* e2n_tefmod16_os_mode_type */

char* e2s_tefmod16_scr_mode [CNT_tefmod_scr_mode] = {
  "TEFMOD16_SCR_MODE_BYPASS" ,
  "TEFMOD16_SCR_MODE_CL49" ,
  "TEFMOD16_SCR_MODE_40G_2_LANE" ,
  "TEFMOD16_SCR_MODE_100G" ,
  "TEFMOD16_SCR_MODE_20G" ,
  "TEFMOD16_SCR_MODE_40G_4_LANE" ,
  "TEFMOD16_SCR_MODE_ILLEGAL" 
}; /* e2s_tefmod16_scr_mode */

int e2n_tefmod16_scr_mode [CNT_tefmod_scr_mode] = {
  0 ,
  1 ,
  2 ,
  3 ,
  4 ,
  5 ,
  0 
}; /* e2n_tefmod16_scr_mode */

char* e2s_tefmod16_encode_mode [CNT_tefmod_encode_mode] = {
  "TEFMOD16_ENCODE_MODE_NONE" ,
  "TEFMOD16_ENCODE_MODE_CL49" ,
  "TEFMOD16_ENCODE_MODE_CL82" ,
  "TEFMOD16_ENCODE_MODE_ILLEGAL" 
}; /* e2s_tefmod16_encode_mode */

int e2n_tefmod16_encode_mode [CNT_tefmod_encode_mode] = {
  0 ,
  1 ,
  2 ,
  15 
}; /* e2n_tefmod16_encode_mode */

char* e2s_tefmod16_descrambler_mode [CNT_tefmod_descrambler_mode] = {
  "TEFMOD16_R_DESCR_MODE_BYPASS" ,
  "TEFMOD16_R_DESCR_MODE_CL49" ,
  "TEFMOD16_R_DESCR_MODE_CL82" ,
  "TEFMOD16_R_DESCR_MODE_ILLEGAL" 
}; /* e2s_tefmod16_descrambler_mode */

char* e2s_tefmod16_dec_tl_mode [CNT_tefmod_dec_tl_mode] = {
  "TEFMOD16_DEC_TL_MODE_NONE" ,
  "TEFMOD16_DEC_TL_MODE_CL49" ,
  "TEFMOD16_DEC_TL_MODE_CL82" ,
  "TEFMOD16_DEC_TL_MODE_ILLEGAL" 
}; /* e2s_tefmod16_dec_tl_mode */

char* e2s_tefmod16_dec_fsm_mode [CNT_tefmod_dec_fsm_mode] = {
  "TEFMOD16_DEC_FSM_MODE_NONE" ,
  "TEFMOD16_DEC_FSM_MODE_CL49" ,
  "TEFMOD16_DEC_FSM_MODE_CL82" ,
  "TEFMOD16_DEC_FSM_MODE_ILLEGAL" 
}; /* e2s_tefmod16_dec_fsm_mode */

char* e2s_tefmod16_deskew_mode [CNT_tefmod_deskew_mode] = {
  "TEFMOD16_R_DESKEW_MODE_BYPASS" ,
  "TEFMOD16_R_DESKEW_MODE_10G" ,
  "TEFMOD16_R_DESKEW_MODE_40G_4_LANE" ,
  "TEFMOD16_R_DESKEW_MODE_40G_2_LANE" ,
  "TEFMOD16_R_DESKEW_MODE_100G" ,
  "TEFMOD16_R_DESKEW_MODE_CL49" ,
  "TEFMOD16_R_DESKEW_MODE_CL91" ,
  "TEFMOD16_R_DESKEW_MODE_ILLEGAL" 
}; /* e2s_tefmod16_deskew_mode */

char* e2s_tefmod16_t_fifo_mode [CNT_tefmod_t_fifo_mode] = {
  "TEFMOD16_T_FIFO_MODE_NONE" ,
  "TEFMOD16_T_FIFO_MODE_40G" ,
  "TEFMOD16_T_FIFO_MODE_100G" ,
  "TEFMOD16_T_FIFO_MODE_20G" ,
  "TEFMOD16_T_FIFO_MODE_ILLEGAL" 
}; /* e2s_tefmod16_t_fifo_mode */

char* e2s_tefmod16_bs_btmx_mode [CNT_tefmod_bs_btmx_mode] = {
  "TEFMOD16_BS_BTMX_MODE_NONE" ,
  "TEFMOD16_BS_BTMX_MODE_1to1" ,
  "TEFMOD16_BS_BTMX_MODE_2to1" ,
  "TEFMOD16_BS_BTMX_MODE_5to1" ,
  "TEFMOD16_BS_BTMX_MODE_ILLEGAL" 
}; /* e2s_tefmod16_bs_btmx_mode */

char* e2s_tefmod16_bs_dist_mode [CNT_tefmod_bs_dist_mode] = {
  "TEFMOD16_BS_DIST_MODE_5_LANE_TDM" ,
  "TEFMOD16_BS_DIST_MODE_2_LANE_TDM_2_VLANE" ,
  "TEFMOD16_BS_DIST_MODE_2_LANE_TDM_1_VLANE" ,
  "TEFMOD16_BS_DIST_MODE_NO_TDM" ,
  "TEFMOD16_BS_DIST_MODE_ILLEGAL" 
}; /* e2s_tefmod16_bs_dist_mode */

