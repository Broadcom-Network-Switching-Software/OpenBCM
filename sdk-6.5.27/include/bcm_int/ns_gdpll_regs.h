/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        ns_gdpll_regs.h
 * Purpose:     Internal GDPLL header that defines the registers and fields
 */


#if defined(BCM_NANOSYNC_V1_SUPPORT)

#define misc_clk_event_ctrl_r       NSYNC_MISC_CLK_EVENT_CTRLr
#define common_ctrl_r               NSYNC_COMMON_CTRLr
#define gpio_0_ctrl_r               NSYNC_TIMESYNC_GPIO_0_CTRLr
#define bs0_bs_tc_ctrl_r            NSYNC_BS_0_BS_TC_CTRLr
#define bs1_bs_tc_ctrl_r            NSYNC_BS_1_BS_TC_CTRLr
#define rx_cdr_event_ovrd_status_r  NSYNC_RX_CDR_EVENT_OVRD_STATUS_0r
#define tx_pi_event_ovrd_status_r   NSYNC_TX_PI_EVENT_OVRD_STATUS_0r
#define ia_ts_pair_available0_r     NS_GDPLL_IA_TS_PAIR_AVAILABLE_0r
#define ia_in_status0_r             NS_GDPLL_IA_IN_STATUS_0r
#define ia_in_enable0_r             NS_GDPLL_IA_IN_ENABLE_0r
#define nco_out_enable0_r           NS_GDPLL_NCO_OUT_ENABLE_0r
#define nco_out_status0_r           NS_GDPLL_NCO_OUT_STATUS_0r
#define nco_overload_status0_r      NS_GDPLL_NCO_OVERLOAD_STATUS_0r

#define debug_m7dg_rstate_r         NS_GDPLL_DEBUG_M_7_DG_RSTATEr
#define m7_core_int_status_r        NS_GDPLL_M_7_CORE_INT_STATUSr
#define m7_core_int_enable_r        NS_GDPLL_M_7_CORE_INT_ENABLEr
#define m7_error_info_r             NS_GDPLL_M_7_ERROR_INFOr
#define m7_error_info_int_r         NS_GDPLL_M_7_ERROR_INFO_INTr
#define debug_m7dg_int_status_r     NS_GDPLL_DEBUG_M_7_DG_INT_STATUSr
#define debug_m7dg_int_enable_r     NS_GDPLL_DEBUG_M_7_DG_INT_ENABLEr
#define debug_m7dg_threshold_r      NS_GDPLL_DEBUG_M_7_DG_THRESHOLDr
#define debug_m7dg_enable_r         NS_GDPLL_DEBUG_M_7_DG_ENABLEr
#define ts_int_enable0_r            NSYNC_TS_INT_ENABLE_0r
#define ts_int_enable1_r            NSYNC_TS_INT_ENABLE_1r
#define ts_int_status_r             NSYNC_TS_INT_STATUSr
#define bs0_bs_config_r             NSYNC_BS_0_BS_CONFIGr
#define bs1_bs_config_r             NSYNC_BS_1_BS_CONFIGr
#define misc_event_ovrd_status_r    NSYNC_MISC_EVENT_OVRD_STATUSr
#define misc_event_status_r         NSYNC_MISC_EVENT_STATUSr

STATIC soc_reg_t  bs_pll_reg1[2] = {
    NSYNC_BROADSYNC_0_CLK_EVENT_CTRLAr, NSYNC_BROADSYNC_1_CLK_EVENT_CTRLAr };
STATIC soc_reg_t  bs_pll_reg2[2] = {
    NSYNC_BROADSYNC_0_CLK_EVENT_CTRLBr, NSYNC_BROADSYNC_1_CLK_EVENT_CTRLBr };

/* Fields for NS_GDPLL_IA_WDOG_CONFIGr */
STATIC soc_reg_t ia_wdog_interval_f     = INTERVAL_Nf;
STATIC soc_reg_t ia_wdog_enable_f       = ENABLE_Nf;

/* Fields of NS_GDPLL_IA_UPDATE_CONFIGr */
STATIC soc_reg_t ia_update_enable_f     = ENABLE_Nf;
STATIC soc_reg_t ia_update_ref_id_f     = REFERENCE_ID_Nf;
STATIC soc_reg_t ia_update_fb_id_f      = FEEDBACK_ID_Nf;
STATIC soc_reg_t ia_update_phaseref_f   = PHASECOUNTERREF_Nf;
STATIC soc_reg_t ia_update_speed_f      = SPEED_BIN_Nf;

/* Fields of MISC_EVENT_STATUS */
STATIC soc_reg_t event_status_bs0_hb_f  = BS_0_HEARTBEATf;
STATIC soc_reg_t event_status_bs1_hb_f  = BS_1_HEARTBEATf;

/* Fields of NS_TS_INT_STATUS */
STATIC soc_reg_t int_ts0_off_update_f   = TS_0_CNT_OFFSET_UPDATEDf;
STATIC soc_reg_t int_ts1_off_update_f   = TS_1_CNT_OFFSET_UPDATEDf;
STATIC soc_reg_t int_m7_int_status_f    = M_7_CORE_INT_STATUSf;
STATIC soc_reg_t int_common_status_f    = GDPLL_COMMON_STATUSf;
STATIC soc_reg_t int_fifo1_ecc_err_f    = TS_CPU_FIFO_1_ECC_ERR_STATUSf;
STATIC soc_reg_t int_fifo2_ecc_err_f    = TS_CPU_FIFO_2_ECC_ERR_STATUSf;
STATIC soc_reg_t int_mem_ecc_err_f      = GDPLL_MEM_ECC_ERR_STATUSf;
STATIC soc_reg_t int_tcm_ecc_err_f      = M_7_TCM_ECC_ERR_STATUSf;
STATIC soc_reg_t int_ts_capture_ovrd_f  = TS_CAPTURE_OVRDf;

/* Fields of NS_GDPLL_DEBUG_M_7_DG_INT_STATUS */
STATIC soc_reg_t m7dg_int_status_ovflw_f = OVERFLOWf;
STATIC soc_reg_t m7dg_int_status_ecc_err_mem0f = ECC_ERR_MEM_0f;
STATIC soc_reg_t m7dg_int_status_ecc_err_mem1f = ECC_ERR_MEM_1f;

#elif defined(BCM_MONTEREY_SUPPORT)

#define misc_clk_event_ctrl_r       NS_MISC_CLK_EVENT_CTRLr
#define common_ctrl_r               NS_COMMON_CTRLr
#define gpio_0_ctrl_r               NS_TIMESYNC_GPIO_0_CTRLr
#define bs0_bs_tc_ctrl_r            NS_BS0_BS_TC_CTRLr
#define bs1_bs_tc_ctrl_r            NS_BS1_BS_TC_CTRLr
#define rx_cdr_event_ovrd_status_r  NS_RX_CDR_EVENT_OVRD_STATUS0r
#define tx_pi_event_ovrd_status_r   NS_TX_PI_EVENT_OVRD_STATUS0r
#define ia_ts_pair_available0_r     NS_GDPLL_IA_TS_PAIR_AVAILABLE0r
#define ia_in_status0_r             NS_GDPLL_IA_IN_STATUS0r
#define ia_in_enable0_r             NS_GDPLL_IA_IN_ENABLE0r
#define nco_out_enable0_r           NS_GDPLL_NCO_OUT_ENABLE0r
#define nco_out_status0_r           NS_GDPLL_NCO_OUT_STATUS0r
#define nco_overload_status0_r      NS_GDPLL_NCO_OVERLOAD_STATUS0r

#define debug_m7dg_rstate_r         NS_GDPLL_DEBUG_M7DG_RSTATEr
#define m7_core_int_status_r        NS_GDPLL_M7_CORE_INT_STATUSr
#define m7_core_int_enable_r        NS_GDPLL_M7_CORE_INT_ENABLEr
#define m7_error_info_r             NS_GDPLL_M7_ERROR_INFOr
#define m7_error_info_int_r         NS_GDPLL_M7_ERROR_INFO_INTr
#define debug_m7dg_int_status_r     NS_GDPLL_DEBUG_M7DG_INT_STATUSr
#define debug_m7dg_int_enable_r     NS_GDPLL_DEBUG_M7DG_INT_ENABLEr
#define debug_m7dg_threshold_r      NS_GDPLL_DEBUG_M7DG_THRESHOLDr
#define debug_m7dg_enable_r         NS_GDPLL_DEBUG_M7DG_ENABLEr
#define ts_int_enable0_r            NS_TS_INT_ENABLEr
#define ts_int_status_r             NS_TS_INT_STATUSr
#define bs0_bs_config_r             NS_BS0_BS_CONFIGr
#define bs1_bs_config_r             NS_BS1_BS_CONFIGr
#define misc_event_ovrd_status_r    NS_MISC_EVENT_OVRD_STATUSr
#define misc_event_status_r         NS_MISC_EVENT_STATUSr

STATIC soc_reg_t gdpll_ia_status[4] = {
    GDPLL_IA_0_STATUSf, GDPLL_IA_1_STATUSf,
    GDPLL_IA_2_STATUSf, GDPLL_IA_3_STATUSf, };
STATIC soc_reg_t gdpll_oa_status[4] = {
    GDPLL_OA_0_STATUSf, GDPLL_OA_1_STATUSf,
    GDPLL_OA_2_STATUSf, GDPLL_OA_3_STATUSf, };

STATIC soc_reg_t  bs_pll_reg1[2] = {
    NS_BROADSYNC0_CLK_EVENT_CTRLr, NS_BROADSYNC1_CLK_EVENT_CTRLr };

STATIC soc_reg_t mapper_port_enable_regs[4] = {
    NS_TIMESYNC_MAPPER_PORT_ENABLE_0r, NS_TIMESYNC_MAPPER_PORT_ENABLE_1r,
    NS_TIMESYNC_MAPPER_PORT_ENABLE_2r, NS_TIMESYNC_MAPPER_PORT_ENABLE_3r };

STATIC soc_reg_t mapper_port_enable[32] = {
    PORT0_TS_ENABLEf,  PORT1_TS_ENABLEf,  PORT2_TS_ENABLEf,  PORT3_TS_ENABLEf,
    PORT4_TS_ENABLEf,  PORT5_TS_ENABLEf,  PORT6_TS_ENABLEf,  PORT7_TS_ENABLEf,
    PORT8_TS_ENABLEf,  PORT9_TS_ENABLEf,  PORT10_TS_ENABLEf, PORT11_TS_ENABLEf,
    PORT12_TS_ENABLEf, PORT13_TS_ENABLEf, PORT14_TS_ENABLEf, PORT15_TS_ENABLEf,
    PORT16_TS_ENABLEf, PORT17_TS_ENABLEf, PORT18_TS_ENABLEf, PORT19_TS_ENABLEf,
    PORT20_TS_ENABLEf, PORT21_TS_ENABLEf, PORT22_TS_ENABLEf, PORT23_TS_ENABLEf,
    PORT24_TS_ENABLEf, PORT25_TS_ENABLEf, PORT26_TS_ENABLEf, PORT27_TS_ENABLEf,
    PORT28_TS_ENABLEf, PORT29_TS_ENABLEf, PORT30_TS_ENABLEf, PORT31_TS_ENABLEf };

/* Fields for NS_GDPLL_IA_WDOG_CONFIGr */
STATIC soc_reg_t ia_wdog_interval_f     = INTERVALf;
STATIC soc_reg_t ia_wdog_enable_f       = ENABLEf;

/* Fields of NS_GDPLL_IA_UPDATE_CONFIGr */
STATIC soc_reg_t ia_update_enable_f     = ENABLEf;
STATIC soc_reg_t ia_update_ref_id_f     = REFERENCE_IDf;
STATIC soc_reg_t ia_update_fb_id_f      = FEEDBACK_IDf;
STATIC soc_reg_t ia_update_phaseref_f   = PHASECOUNTERREFf;
STATIC soc_reg_t ia_update_speed_f      = SPEED_BINf;

/* Fields of MISC_EVENT_STATUS */
STATIC soc_reg_t event_status_bs0_hb_f  = BS0_HEARTBEATf;
STATIC soc_reg_t event_status_bs1_hb_f  = BS1_HEARTBEATf;

/* Fields of NS_TS_INT_STATUS */
STATIC soc_reg_t int_ts0_off_update_f   = TS0_CNT_OFFSET_UPDATEDf;
STATIC soc_reg_t int_ts1_off_update_f   = TS1_CNT_OFFSET_UPDATEDf;
STATIC soc_reg_t int_m7_int_status_f    = M7_CORE_INT_STATUSf;
STATIC soc_reg_t int_common_status_f    = GDPLL_COMMON_STATUSf;
STATIC soc_reg_t int_fifo1_ecc_err_f    = TS_CPU_FIFO_ECC_ERR_STATUSf;
STATIC soc_reg_t int_mem_ecc_err_f      = GDPLL_MEM_ECC_ERR_STATUSf;
STATIC soc_reg_t int_tcm_ecc_err_f      = M7_TCM_ECC_ERR_STATUSf;
STATIC soc_reg_t int_ts_capture_ovrd_f  = TS_CAPTURE_OVRDf;

/* Fields of NS_GDPLL_DEBUG_M_7_DG_INT_STATUS */
STATIC soc_reg_t m7dg_int_status_ovflw_f = OVERFLOWf;
STATIC soc_reg_t m7dg_int_status_ecc_err_mem0f = ECC_ERR_MEM0f;
STATIC soc_reg_t m7dg_int_status_ecc_err_mem1f = ECC_ERR_MEM1f;

#endif


