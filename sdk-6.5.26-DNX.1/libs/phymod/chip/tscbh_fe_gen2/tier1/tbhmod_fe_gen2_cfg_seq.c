/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : tbhmod_fe_gen2_cfg_seq.c
 * Description: c functions implementing Tier1s for TSCBH FE Gen2 Serdes Driver
 *---------------------------------------------------------------------*/

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_tscbh_fe_gen2_xgxs_defs.h>
#include <tscbh/tier1/tbhmod_enum_defines.h>
#include <tscbh/tier1/tbhmod_spd_ctrl.h>
#include <tscbh/tier1/tbhmod.h>
#include <tscbh/tier1/tbhmod_sc_lkup_table.h>
#include <tscbh/tier1/tbhPCSRegEnums.h>
#include <tscbh_fe_gen2/tier1/tbhmod_fe_gen2.h>
#include <tscbh_fe_gen2/tier1/tbhmod_fe_gen2_sc_lkup_table.h>

#define TBHMOD_DBG_IN_FUNC_INFO(pc) \
  PHYMOD_VDBG(TBHMOD_DBG_FUNC,pc,("-22%s: Adr:%08x Ln:%02d\n", __func__, pc->addr, pc->lane_mask))
#define TBHMOD_DBG_IN_FUNC_VIN_INFO(pc,_print_) \
  PHYMOD_VDBG(TBHMOD_DBG_FUNCVALIN,pc,_print_)
#define TBHMOD_DBG_IN_FUNC_VOUT_INFO(pc,_print_) \
  PHYMOD_VDBG(TBHMOD_DBG_FUNCVALOUT,pc,_print_)

/*!
 *  @brief tbhmod_fe_gen2_pcs_ts_en per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param en tells enabling/disabling r_test_mode AKA scrmable_idle_en
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_fe_gen2_pcs_ts_en(PHYMOD_ST* pc, int en, int sfd, int rpm)
{
    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_pcs_rx_ts_en(PHYMOD_ST* pc, uint32_t en)
{
    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_fe_gen2_set_pmd_timer_offset per port.
 *  @param unit number for instance lane number for decide which lane
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details PMD Timer offset value */
int tbhmod_fe_gen2_set_pmd_timer_offset(PHYMOD_ST* pc, int ts_clk_period)
{
    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_fe_gen2_pcs_set_1588_ui per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @param clk4sync_div the fast clk divider 0 => 6 and 1 => 8
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_fe_gen2_pcs_set_1588_ui (PHYMOD_ST* pc, uint32_t vco, uint32_t vco1, int os_mode, int clk4sync_div, int pam4)
{
    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_fe_gen2_1588_pmd_latency per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_fe_gen2_1588_pmd_latency(PHYMOD_ST* pc, int an_en, uint32_t vco, int os_mode, int pam4)
{
    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_update_tx_pmd_latency (PHYMOD_ST* pc, uint32_t latency_adj, int normalize_to_latest)
{
  return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_fe_gen2_pcs_set_tx_lane_skew_capture per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_fe_gen2_pcs_set_tx_lane_skew_capture (PHYMOD_ST* pc, int tx_skew_en)
{
    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_fe_gen2_pcs_measure_tx_lane_skew per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_fe_gen2_pcs_measure_tx_lane_skew (PHYMOD_ST* pc, int an_en, uint32_t vco, int os_mode, int pam4, int normalize_to_latest, int *tx_max_skew)
{
    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_pcs_mod_rx_1588_tbl_val(PHYMOD_ST* pc, int an_en, int bit_mux_mode, uint32_t vco, int os_mode, int pam4, int tbl_ln, int normalize_to_latest, int rx_skew_adjust_enb, int *table , int *rx_max_skew) {
    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_pcs_ts_deskew_valid(PHYMOD_ST* pc, int bit_mux_mode, int *rx_ts_deskew_valid) {

    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_ts_offset_rx_set(PHYMOD_ST* pc, uint32_t vco, int os_mode, int pam4, int tbl_ln, uint32_t *table)
{
    return PHYMOD_E_NONE;
}

/**
 * @brief   Set Per lane OS mode set in PMD
 * @param   pc handle to current TSCBH context (#PHYMOD_ST)
 * @param   mapped_speed_id: speed entry index in sw_speed_table
 * @param   os_mode over sample rate.
 * @returns The value PHYMOD_E_NONE upon successful completion
 * @details Per Port PMD Init
 *
 * */
int tbhmod_fe_gen2_pmd_osmode_set(PHYMOD_ST* pc, int mapped_speed_id, tbhmod_refclk_t refclk)
{
  RXTXCOM_OSR_MODE_CTLr_t reg_osr_mode;
  int os_mode;

  RXTXCOM_OSR_MODE_CTLr_CLR(reg_osr_mode);

  /* 0=OS_MODE_1;    1=OS_MODE_2;
   * 2=OS_MODE_4;    4=OS_MODE_21p25;
   * 8=OS MODE_16p5; 12=OS_MODE_20p625;  */

  if (refclk == TBHMOD_REF_CLK_312P5MHZ) {
      os_mode =  tscbh_fe_gen2_sc_pmd_entry_312M_ref[mapped_speed_id].t_pma_os_mode;
  } else {
      os_mode =  tscbh_fe_gen2_sc_pmd_entry[mapped_speed_id].t_pma_os_mode;
  }

  RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRCf_SET(reg_osr_mode, 1);
  RXTXCOM_OSR_MODE_CTLr_OSR_MODE_FRC_VALf_SET(reg_osr_mode, os_mode);

  PHYMOD_IF_ERR_RETURN
     (MODIFY_RXTXCOM_OSR_MODE_CTLr(pc, reg_osr_mode));

  return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_tx_ts_info_unpack_tx_ts_tbl_entry(uint32_t *tx_ts_tbl_entry, phymod_ts_fifo_status_t *tx_ts_info)
{
    return PHYMOD_E_NONE;
}

/* Enable PCS clock block */
int tbhmod_fe_gen2_pcs_clk_blk_en(const PHYMOD_ST* pc, uint32_t en)
{
    AMS_PLL_PLL_CTL_12r_t reg_pll_ctrl;

    AMS_PLL_PLL_CTL_12r_CLR(reg_pll_ctrl);
    AMS_PLL_PLL_CTL_12r_AMS_PLL_EN_CLK4PCSf_SET(reg_pll_ctrl, en);

    PHYMOD_IF_ERR_RETURN
       (MODIFY_AMS_PLL_PLL_CTL_12r(pc, reg_pll_ctrl));

    return PHYMOD_E_NONE;
}

/**
 * @brief   fec three_cw_bad state get
 * @param   pc handle to current TSCBH context (#tbhmod_st)
 * @returns The value PHYMOD_E_NONE upon successful completion
 * @details fec three_cw_bad state get.
 * */
int tbhmod_fe_gen2_fec_three_cw_bad_state_get(PHYMOD_ST* pc, uint32_t *state)
{
  RX_X4_RX_LATCH_STSr_t rx_latch_sts;

  TBHMOD_DBG_IN_FUNC_INFO(pc);

  RX_X4_RX_LATCH_STSr_CLR(rx_latch_sts);
  PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_LATCH_STSr(pc, &rx_latch_sts));
  *state = RX_X4_RX_LATCH_STSr_DESKEW_HIS_STATEf_GET(rx_latch_sts) & 0x20;

  return PHYMOD_E_NONE;
}

/* get timestamp entry availablilty indicator */
int tbhmod_fe_gen2_1588_ts_valid_get(PHYMOD_ST* pc, uint16_t* ts_valid)
{
    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_fec_align_status_get(PHYMOD_ST* pc, uint32_t *fec_align_live)
{
    RX_X4_RS_FEC_RXP_STSr_t fec_status_reg;

    RX_X4_RS_FEC_RXP_STSr_CLR(fec_status_reg);
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_RS_FEC_RXP_STSr(pc, &fec_status_reg));

    *fec_align_live = RX_X4_RS_FEC_RXP_STSr_FEC_ALIGN_STATUS_LIVEf_GET(fec_status_reg);
    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_fec_override_set(PHYMOD_ST* pc, uint32_t enable)
{
    SC_X4_SW_SPARE1r_t SC_X4_SW_SPARE1r_reg;
    uint16_t temp_reg_value;

    SC_X4_SW_SPARE1r_CLR(SC_X4_SW_SPARE1r_reg);
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_SW_SPARE1r(pc, &SC_X4_SW_SPARE1r_reg));
    temp_reg_value = SC_X4_SW_SPARE1r_GET(SC_X4_SW_SPARE1r_reg);
    /* first clear the lowest bit */
    temp_reg_value &= ~TSCBH_FE_GEN2_FEC_OVERRIDE_MASK;
    temp_reg_value |= (enable & TSCBH_FE_GEN2_FEC_OVERRIDE_MASK);
    SC_X4_SW_SPARE1r_SET(SC_X4_SW_SPARE1r_reg, temp_reg_value);
    PHYMOD_IF_ERR_RETURN(WRITE_SC_X4_SW_SPARE1r(pc, SC_X4_SW_SPARE1r_reg));

    return PHYMOD_E_NONE;
}


int tbhmod_fe_gen2_fec_override_get(PHYMOD_ST* pc, uint32_t* enable)
{
    SC_X4_SW_SPARE1r_t SC_X4_SW_SPARE1r_reg;
    uint16_t temp_reg_value;

    SC_X4_SW_SPARE1r_CLR(SC_X4_SW_SPARE1r_reg);
    PHYMOD_IF_ERR_RETURN(READ_SC_X4_SW_SPARE1r(pc, &SC_X4_SW_SPARE1r_reg));
    temp_reg_value = SC_X4_SW_SPARE1r_GET(SC_X4_SW_SPARE1r_reg);
    *enable = temp_reg_value & TSCBH_FE_GEN2_FEC_OVERRIDE_MASK;

    return PHYMOD_E_NONE;
}

/**
 * @brief   fec corruption period set
 * @param   pc handle to current TSCBH context (#tbhmod_st)
 * @returns The value PHYMOD_E_NONE upon successful completion
 * @details fec corruption period set.
 * */
int tbhmod_fe_gen2_fec_corruption_period_set(PHYMOD_ST* pc, uint32_t period)
{
  RX_X4_RS_FEC_TMRr_t rs_fec_tmr;

  TBHMOD_DBG_IN_FUNC_INFO(pc);

  RX_X4_RS_FEC_TMRr_CLR(rs_fec_tmr);
  RX_X4_RS_FEC_TMRr_CORRUPTION_PERIODf_SET(rs_fec_tmr, period);
  PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RS_FEC_TMRr(pc, rs_fec_tmr));

  return PHYMOD_E_NONE;
}

/**
 * @brief   fec corruption period get
 * @param   pc handle to current TSCBH context (#tbhmod_st)
 * @returns The value PHYMOD_E_NONE upon successful completion
 * @details fec corruption period get.
 * */
int tbhmod_fe_gen2_fec_corruption_period_get(PHYMOD_ST* pc, uint32_t *period)
{
  RX_X4_RS_FEC_TMRr_t rs_fec_tmr;

  TBHMOD_DBG_IN_FUNC_INFO(pc);

  RX_X4_RS_FEC_TMRr_CLR(rs_fec_tmr);
  PHYMOD_IF_ERR_RETURN(READ_RX_X4_RS_FEC_TMRr(pc, &rs_fec_tmr));
  *period = RX_X4_RS_FEC_TMRr_CORRUPTION_PERIODf_GET(rs_fec_tmr);

  return PHYMOD_E_NONE;
}

/**@brief   get TX lane swap values for all lanes.
 * @param   pc handle to current TSCF context (#PHYMOD_ST)
 * @param   tx_swap   Receives the PCS lane swap value
 * @returns The value PHYMOD_E_NONE upon successful completion
 * @details gets the TX lane swap values for all lanes simultaneously.
*/
int tbhmod_fe_gen2_pcs_tx_lane_swap_get(PHYMOD_ST *pc, uint32_t *tx_swap)
{
    unsigned int pcs_map, lane;
    TX_X1_TX_LN_SWPr_t reg;
    pc->lane_mask = 0x1 << 0;
    TX_X1_TX_LN_SWPr_CLR(reg);
    PHYMOD_IF_ERR_RETURN(READ_TX_X1_TX_LN_SWPr(pc, &reg));
    pcs_map = TX_X1_TX_LN_SWPr_GET(reg);
    *tx_swap = (((pcs_map >> 0) & 0x7) << 0) |
        (((pcs_map >> 3) & 0x7) << 4) |
        (((pcs_map >> 6) & 0x7) << 8) |
        (((pcs_map >> 9) & 0x7) << 12) ;
    pc->lane_mask = 0x1 << 4;
    TX_X1_TX_LN_SWPr_CLR(reg);
    PHYMOD_IF_ERR_RETURN(READ_TX_X1_TX_LN_SWPr(pc, &reg));
    pcs_map = TX_X1_TX_LN_SWPr_GET(reg);
    lane = pcs_map >> 0 & 0x7;
    *tx_swap |= ((lane >= 4) ? (lane - 4) : (lane + 4)) << 16;
    lane = pcs_map >> 3 & 0x7;
    *tx_swap |= ((lane >= 4) ? (lane - 4) : (lane + 4)) << 20;
    lane = pcs_map >> 6 & 0x7;
    *tx_swap |= ((lane >= 4) ? (lane - 4) : (lane + 4)) << 24;
    lane = pcs_map >> 9 & 0x7;
    *tx_swap |= ((lane >= 4) ? (lane - 4) : (lane + 4)) << 28;
    return PHYMOD_E_NONE ;
}

/**@brief   get rX lane swap values for all lanes.
 * @param   pc handle to current TSCF context (#PHYMOD_ST)
 * @param   rx_swap   Receives the PCS lane swap value
 * @returns The value PHYMOD_E_NONE upon successful completion
 * @details gets the RX lane swap values for all lanes simultaneously.
*/
int tbhmod_fe_gen2_pcs_rx_lane_swap_get(PHYMOD_ST *pc, uint32_t *rx_swap)
{
    unsigned int pcs_map;
    RX_X1_RX_LN_SWPr_t reg;
    pc->lane_mask = 0x1 << 0;
    RX_X1_RX_LN_SWPr_CLR(reg);
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_RX_LN_SWPr(pc, &reg)) ;
    pcs_map = RX_X1_RX_LN_SWPr_GET(reg);
    *rx_swap = (((pcs_map >> 0) & 0x7) << 0) |
        (((pcs_map >> 3) & 0x7) << 4) |
        (((pcs_map >> 6) & 0x7) << 8) |
        (((pcs_map >> 9) & 0x7) << 12) ;
    pc->lane_mask = 0x1 << 4;
    RX_X1_RX_LN_SWPr_CLR(reg);
    PHYMOD_IF_ERR_RETURN(READ_RX_X1_RX_LN_SWPr(pc, &reg)) ;
    pcs_map = RX_X1_RX_LN_SWPr_GET(reg);
    *rx_swap |= (((pcs_map >> 0) & 0x7) << 16) |
        (((pcs_map >> 3) & 0x7) << 20) |
        (((pcs_map >> 6) & 0x7) << 24) |
        (((pcs_map >> 9) & 0x7) << 28) ;
    return PHYMOD_E_NONE ;
}

/* FEC Degraded control enable set */
int tbhmod_fe_gen2_fec_degraded_ctrl_enable_set(PHYMOD_ST *pc,
                                                uint8_t enable)
{
    RX_X1_FEC_DEGRADED_CTL0r_t reg;

    TBHMOD_DBG_IN_FUNC_INFO(pc);
    RX_X1_FEC_DEGRADED_CTL0r_CLR(reg);

    RX_X1_FEC_DEGRADED_CTL0r_FEC_DEGRADED_SER_ENABLEf_SET(reg, enable);

    PHYMOD_IF_ERR_RETURN
       (MODIFY_RX_X1_FEC_DEGRADED_CTL0r(pc, reg));

    return PHYMOD_E_NONE;
}

/* FEC Degraded control enable get */
int tbhmod_fe_gen2_fec_degraded_ctrl_enable_get(PHYMOD_ST *pc,
                                                uint8_t *enable)
{
    RX_X1_FEC_DEGRADED_CTL0r_t reg;

    TBHMOD_DBG_IN_FUNC_INFO(pc);
    RX_X1_FEC_DEGRADED_CTL0r_CLR(reg);

    PHYMOD_IF_ERR_RETURN
       (READ_RX_X1_FEC_DEGRADED_CTL0r(pc, &reg));
    *enable = RX_X1_FEC_DEGRADED_CTL0r_FEC_DEGRADED_SER_ENABLEf_GET(reg);

    return PHYMOD_E_NONE;
}

/* FEC Degraded control activate threshold set */
int tbhmod_fe_gen2_fec_degraded_ctrl_activate_threshold_set(PHYMOD_ST *pc,
                                                            uint32_t threshold)
{
    RX_X1_FEC_DEGRADED_CTL1r_t reg_lower;
    RX_X1_FEC_DEGRADED_CTL2r_t reg_upper;
    uint32_t threshold_upper = 0, threshold_lower = 0;

    TBHMOD_DBG_IN_FUNC_INFO(pc);

    /* set FEC_degraded_SER_activate_threshold_lower */
    RX_X1_FEC_DEGRADED_CTL1r_CLR(reg_lower);
    threshold_lower = threshold & 0xffff;
    RX_X1_FEC_DEGRADED_CTL1r_FEC_DEGRADED_SER_ACTIVATE_THRESHOLD_LOWERf_SET(reg_lower, threshold_lower);
    PHYMOD_IF_ERR_RETURN
       (WRITE_RX_X1_FEC_DEGRADED_CTL1r(pc, reg_lower));

    /* set FEC_degraded_SER_activate_threshold_upper */
    RX_X1_FEC_DEGRADED_CTL2r_CLR(reg_upper);
    threshold_upper = (threshold >> 16) & 0xffff;
    RX_X1_FEC_DEGRADED_CTL2r_FEC_DEGRADED_SER_ACTIVATE_THRESHOLD_UPPERf_SET(reg_upper, threshold_upper);
    PHYMOD_IF_ERR_RETURN
       (WRITE_RX_X1_FEC_DEGRADED_CTL2r(pc, reg_upper));

    return PHYMOD_E_NONE;
}

/* FEC Degraded control activate threshold get */
int tbhmod_fe_gen2_fec_degraded_ctrl_activate_threshold_get(PHYMOD_ST *pc,
                                                            uint32_t *threshold)
{
    RX_X1_FEC_DEGRADED_CTL1r_t reg_lower;
    RX_X1_FEC_DEGRADED_CTL2r_t reg_upper;
    uint32_t threshold_upper = 0, threshold_lower = 0;

    TBHMOD_DBG_IN_FUNC_INFO(pc);

    /* get FEC_degraded_SER_activate_threshold_lower */
    RX_X1_FEC_DEGRADED_CTL1r_CLR(reg_lower);
    PHYMOD_IF_ERR_RETURN
       (READ_RX_X1_FEC_DEGRADED_CTL1r(pc, &reg_lower));
    threshold_lower = RX_X1_FEC_DEGRADED_CTL1r_FEC_DEGRADED_SER_ACTIVATE_THRESHOLD_LOWERf_GET(reg_lower);

    /* get FEC_degraded_SER_activate_threshold_upper */
    RX_X1_FEC_DEGRADED_CTL2r_CLR(reg_upper);
    PHYMOD_IF_ERR_RETURN
       (READ_RX_X1_FEC_DEGRADED_CTL2r(pc, &reg_upper));
    threshold_upper = RX_X1_FEC_DEGRADED_CTL2r_FEC_DEGRADED_SER_ACTIVATE_THRESHOLD_UPPERf_GET(reg_upper);

    *threshold = ((threshold_upper << 16) & 0xffff0000) |
                  (threshold_lower & 0xffff);

    return PHYMOD_E_NONE;
}

/* FEC Degraded control deactivate threshold set */
int tbhmod_fe_gen2_fec_degraded_ctrl_deactivate_threshold_set(PHYMOD_ST *pc,
                                                              uint32_t threshold)
{
    RX_X1_FEC_DEGRADED_CTL3r_t reg_lower;
    RX_X1_FEC_DEGRADED_CTL4r_t reg_upper;
    uint32_t threshold_upper = 0, threshold_lower = 0;

    TBHMOD_DBG_IN_FUNC_INFO(pc);

    /* set FEC_degraded_SER_deactivate_threshold_lower */
    RX_X1_FEC_DEGRADED_CTL3r_CLR(reg_lower);
    threshold_lower = threshold & 0xffff;
    RX_X1_FEC_DEGRADED_CTL3r_FEC_DEGRADED_SER_DEACTIVATE_THRESHOLD_LOWERf_SET(reg_lower, threshold_lower);
    PHYMOD_IF_ERR_RETURN
       (WRITE_RX_X1_FEC_DEGRADED_CTL3r(pc, reg_lower));

    /* set FEC_degraded_SER_deactivate_threshold_upper */
    RX_X1_FEC_DEGRADED_CTL4r_CLR(reg_upper);
    threshold_upper = (threshold >> 16) & 0xffff;
    RX_X1_FEC_DEGRADED_CTL4r_FEC_DEGRADED_SER_DEACTIVATE_THRESHOLD_UPPERf_SET(reg_upper, threshold_upper);
    PHYMOD_IF_ERR_RETURN
       (WRITE_RX_X1_FEC_DEGRADED_CTL4r(pc, reg_upper));

    return PHYMOD_E_NONE;
}

/* FEC Degraded control deactivate threshold get */
int tbhmod_fe_gen2_fec_degraded_ctrl_deactivate_threshold_get(PHYMOD_ST *pc,
                                                              uint32_t *threshold)
{
    RX_X1_FEC_DEGRADED_CTL3r_t reg_lower;
    RX_X1_FEC_DEGRADED_CTL4r_t reg_upper;
    uint32_t threshold_upper = 0, threshold_lower = 0;

    TBHMOD_DBG_IN_FUNC_INFO(pc);

    /* get FEC_degraded_SER_deactivate_threshold_lower */
    RX_X1_FEC_DEGRADED_CTL3r_CLR(reg_lower);
    PHYMOD_IF_ERR_RETURN
       (READ_RX_X1_FEC_DEGRADED_CTL3r(pc, &reg_lower));
    threshold_lower = RX_X1_FEC_DEGRADED_CTL3r_FEC_DEGRADED_SER_DEACTIVATE_THRESHOLD_LOWERf_GET(reg_lower);

    /* get FEC_degraded_SER_deactivate_threshold_upper */
    RX_X1_FEC_DEGRADED_CTL4r_CLR(reg_upper);
    PHYMOD_IF_ERR_RETURN
       (READ_RX_X1_FEC_DEGRADED_CTL4r(pc, &reg_upper));
    threshold_upper = RX_X1_FEC_DEGRADED_CTL4r_FEC_DEGRADED_SER_DEACTIVATE_THRESHOLD_UPPERf_GET(reg_upper);

    *threshold = ((threshold_upper << 16) & 0xffff0000) |
                  (threshold_lower & 0xffff);

    return PHYMOD_E_NONE;
}

/* FEC Degraded control interval set */
int tbhmod_fe_gen2_fec_degraded_ctrl_interval_set(PHYMOD_ST *pc,
                                                  uint32_t interval)
{
    RX_X1_FEC_DEGRADED_CTL5r_t reg_lower;
    RX_X1_FEC_DEGRADED_CTL6r_t reg_upper;
    uint32_t interval_upper = 0, interval_lower = 0;

    TBHMOD_DBG_IN_FUNC_INFO(pc);

    /* set FEC_degraded_SER_interval_lower */
    RX_X1_FEC_DEGRADED_CTL5r_CLR(reg_lower);
    interval_lower = interval & 0xffff;
    RX_X1_FEC_DEGRADED_CTL5r_FEC_DEGRADED_SER_INTERVAL_LOWERf_SET(reg_lower, interval_lower);
    PHYMOD_IF_ERR_RETURN
       (WRITE_RX_X1_FEC_DEGRADED_CTL5r(pc, reg_lower));

    /* set FEC_degraded_SER_interval_upper */
    RX_X1_FEC_DEGRADED_CTL6r_CLR(reg_upper);
    interval_upper = (interval >> 16) & 0xffff;
    RX_X1_FEC_DEGRADED_CTL6r_FEC_DEGRADED_SER_INTERVAL_UPPERf_SET(reg_upper, interval_upper);
    PHYMOD_IF_ERR_RETURN
       (WRITE_RX_X1_FEC_DEGRADED_CTL6r(pc, reg_upper));

    return PHYMOD_E_NONE;
}

/* FEC Degraded control interval get */
int tbhmod_fe_gen2_fec_degraded_ctrl_interval_get(PHYMOD_ST *pc,
                                                  uint32_t *interval)
{
    RX_X1_FEC_DEGRADED_CTL5r_t reg_lower;
    RX_X1_FEC_DEGRADED_CTL6r_t reg_upper;
    uint32_t interval_upper = 0, interval_lower = 0;

    TBHMOD_DBG_IN_FUNC_INFO(pc);

    /* get FEC_degraded_SER_interval_lower */
    RX_X1_FEC_DEGRADED_CTL5r_CLR(reg_lower);
    PHYMOD_IF_ERR_RETURN
       (READ_RX_X1_FEC_DEGRADED_CTL5r(pc, &reg_lower));
    interval_lower = RX_X1_FEC_DEGRADED_CTL5r_FEC_DEGRADED_SER_INTERVAL_LOWERf_GET(reg_lower);

    /* get FEC_degraded_SER_interval_upper */
    RX_X1_FEC_DEGRADED_CTL6r_CLR(reg_upper);
    PHYMOD_IF_ERR_RETURN
       (READ_RX_X1_FEC_DEGRADED_CTL6r(pc, &reg_upper));
    interval_upper = RX_X1_FEC_DEGRADED_CTL6r_FEC_DEGRADED_SER_INTERVAL_UPPERf_GET(reg_upper);

    *interval = ((interval_upper << 16) & 0xffff0000) |
                 (interval_lower & 0xffff);

    return PHYMOD_E_NONE;
}

/* FEC Degraded SER status get */
int tbhmod_fe_gen2_fec_degraded_status_get(PHYMOD_ST *pc,
                                           phymod_fec_degraded_ser_status_t* status)
{
    RX_X1_STS_FEC_DEGRADED_STSr_t reg_status;

    TBHMOD_DBG_IN_FUNC_INFO(pc);
    RX_X1_STS_FEC_DEGRADED_STSr_CLR(reg_status);

    PHYMOD_IF_ERR_RETURN
       (READ_RX_X1_STS_FEC_DEGRADED_STSr(pc, &reg_status));
    status->fec_degraded_ser = RX_X1_STS_FEC_DEGRADED_STSr_FEC_DEGRADED_SERf_GET(reg_status);
    status->rx_local_degraded = RX_X1_STS_FEC_DEGRADED_STSr_FEC_RX_LOCAL_DEGRADEDf_GET(reg_status);
    status->rx_remote_degraded = RX_X1_STS_FEC_DEGRADED_STSr_FEC_RX_RM_DEGRADEDf_GET(reg_status);
    status->rx_am_status_field = RX_X1_STS_FEC_DEGRADED_STSr_FEC_RX_AM_SFf_GET(reg_status);

    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched) /* PMD_RESET_SEQ */
{
    PMD_X1_PMD0_CTLr_t reg_pmd0_x1_ctrl;
    PMD_X1_PMD1_CTLr_t reg_pmd1_x1_ctrl;
    PMD_X1_PMD2_CTLr_t reg_pmd2_x1_ctrl;
    PMD_X1_PMD3_CTLr_t reg_pmd3_x1_ctrl;


    if (pmd_touched == 0) {
        /* PMD0 reset removed */
        PMD_X1_PMD0_CTLr_CLR(reg_pmd0_x1_ctrl);
        PMD_X1_PMD0_CTLr_PMD0_POR_H_RSTBf_SET(reg_pmd0_x1_ctrl,1);
        PMD_X1_PMD0_CTLr_PMD0_CORE_DP_H_RSTBf_SET(reg_pmd0_x1_ctrl,1);
        PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_PMD0_CTLr(pc,reg_pmd0_x1_ctrl));

        /* PMD1 reset removed */
        PMD_X1_PMD1_CTLr_CLR(reg_pmd1_x1_ctrl);
        PMD_X1_PMD1_CTLr_PMD1_POR_H_RSTBf_SET(reg_pmd1_x1_ctrl,1);
        PMD_X1_PMD1_CTLr_PMD1_CORE_DP_H_RSTBf_SET(reg_pmd1_x1_ctrl,1);
        PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_PMD1_CTLr(pc,reg_pmd1_x1_ctrl));

        /* PMD2 reset removed */
        PMD_X1_PMD2_CTLr_CLR(reg_pmd2_x1_ctrl);
        PMD_X1_PMD2_CTLr_PMD2_POR_H_RSTBf_SET(reg_pmd2_x1_ctrl,1);
        PMD_X1_PMD2_CTLr_PMD2_CORE_DP_H_RSTBf_SET(reg_pmd2_x1_ctrl,1);
        PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_PMD2_CTLr(pc,reg_pmd2_x1_ctrl));

        /* PMD3 reset removed */
        PMD_X1_PMD3_CTLr_CLR(reg_pmd3_x1_ctrl);
        PMD_X1_PMD3_CTLr_PMD3_POR_H_RSTBf_SET(reg_pmd3_x1_ctrl,1);
        PMD_X1_PMD3_CTLr_PMD3_CORE_DP_H_RSTBf_SET(reg_pmd3_x1_ctrl,1);
        PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_PMD3_CTLr(pc,reg_pmd3_x1_ctrl));

        /* next change the MPP index to 1 */
        pc->lane_mask = 0x1 << 4;

        /* PMD0 reset removed */
        PMD_X1_PMD0_CTLr_CLR(reg_pmd0_x1_ctrl);
        PMD_X1_PMD0_CTLr_PMD0_POR_H_RSTBf_SET(reg_pmd0_x1_ctrl,1);
        PMD_X1_PMD0_CTLr_PMD0_CORE_DP_H_RSTBf_SET(reg_pmd0_x1_ctrl,1);
        PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_PMD0_CTLr(pc,reg_pmd0_x1_ctrl));

        /* PMD1 reset removed */
        PMD_X1_PMD1_CTLr_CLR(reg_pmd1_x1_ctrl);
        PMD_X1_PMD1_CTLr_PMD1_POR_H_RSTBf_SET(reg_pmd1_x1_ctrl,1);
        PMD_X1_PMD1_CTLr_PMD1_CORE_DP_H_RSTBf_SET(reg_pmd1_x1_ctrl,1);
        PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_PMD1_CTLr(pc,reg_pmd1_x1_ctrl));

        /* PMD2 reset removed */
        PMD_X1_PMD2_CTLr_CLR(reg_pmd2_x1_ctrl);
        PMD_X1_PMD2_CTLr_PMD2_POR_H_RSTBf_SET(reg_pmd2_x1_ctrl,1);
        PMD_X1_PMD2_CTLr_PMD2_CORE_DP_H_RSTBf_SET(reg_pmd2_x1_ctrl,1);
        PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_PMD2_CTLr(pc,reg_pmd2_x1_ctrl));

        /* PMD3 reset removed */
        PMD_X1_PMD3_CTLr_CLR(reg_pmd3_x1_ctrl);
        PMD_X1_PMD3_CTLr_PMD3_POR_H_RSTBf_SET(reg_pmd3_x1_ctrl,1);
        PMD_X1_PMD3_CTLr_PMD3_CORE_DP_H_RSTBf_SET(reg_pmd3_x1_ctrl,1);
        PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_PMD3_CTLr(pc,reg_pmd3_x1_ctrl));
    }
    return PHYMOD_E_NONE;
}


