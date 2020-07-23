/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : tbhmod_gen2_cfg_seq.c
 * Description: c functions implementing Tier1s for TBHMod Gen2 Serdes Driver
 *---------------------------------------------------------------------*/

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_tscbh_gen2_xgxs_defs.h>
#include <tscbh/tier1/tbhmod_enum_defines.h>
#include <tscbh/tier1/tbhmod_spd_ctrl.h>
#include <tscbh/tier1/tbhmod.h>
#include <tscbh/tier1/tbhmod_sc_lkup_table.h>
#include <tscbh/tier1/tbhPCSRegEnums.h>
#include <tscbh_gen2/tier1/tbhmod_gen2.h>

#define TBHMOD_DBG_IN_FUNC_INFO(pc) \
  PHYMOD_VDBG(TBHMOD_DBG_FUNC,pc,("-22%s: Adr:%08x Ln:%02d\n", __func__, pc->addr, pc->lane_mask))
#define TBHMOD_DBG_IN_FUNC_VIN_INFO(pc,_print_) \
  PHYMOD_VDBG(TBHMOD_DBG_FUNCVALIN,pc,_print_)
#define TBHMOD_DBG_IN_FUNC_VOUT_INFO(pc,_print_) \
  PHYMOD_VDBG(TBHMOD_DBG_FUNCVALOUT,pc,_print_)

/*!
 *  @brief tbhmod_pcs_ts_en per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param en tells enabling/disabling r_test_mode AKA scrmable_idle_en
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_gen2_pcs_ts_en(PHYMOD_ST* pc, int en, int sfd, int rpm)
{
    RX_X4_RX_TS_CTL_GEN2r_t RX_X4_RX_TS_CTL_GEN2r_reg;
    TX_X4_TX_TS_CTL_GEN2r_t TX_X4_TX_TS_CTL_GEN2r_reg;


    RX_X4_RX_TS_CTL_GEN2r_CLR(RX_X4_RX_TS_CTL_GEN2r_reg);
    TX_X4_TX_TS_CTL_GEN2r_CLR(TX_X4_TX_TS_CTL_GEN2r_reg);


    RX_X4_RX_TS_CTL_GEN2r_TS_UPDATE_ENABLEf_SET(RX_X4_RX_TS_CTL_GEN2r_reg, en);
    RX_X4_RX_TS_CTL_GEN2r_RX_SFD_TS_ENf_SET(RX_X4_RX_TS_CTL_GEN2r_reg, sfd);
    if (sfd == 0) {
        RX_X4_RX_TS_CTL_GEN2r_RX_SOP_BYTE_OFFSETf_SET(RX_X4_RX_TS_CTL_GEN2r_reg, 0);
    }
    if ((sfd == 1) && (rpm == 1)) {
        RX_X4_RX_TS_CTL_GEN2r_RX_SOP_BYTE_OFFSETf_SET(RX_X4_RX_TS_CTL_GEN2r_reg, 3);
    }
    if ((sfd == 1) && (rpm == 0)) {
        RX_X4_RX_TS_CTL_GEN2r_RX_SOP_BYTE_OFFSETf_SET(RX_X4_RX_TS_CTL_GEN2r_reg, 7);
    }
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_TS_CTL_GEN2r(pc, RX_X4_RX_TS_CTL_GEN2r_reg));

    TX_X4_TX_TS_CTL_GEN2r_TX_SFD_TS_ENf_SET(TX_X4_TX_TS_CTL_GEN2r_reg, sfd);
    if (sfd == 0) {
        TX_X4_TX_TS_CTL_GEN2r_TX_SOP_BYTE_OFFSETf_SET(TX_X4_TX_TS_CTL_GEN2r_reg, 0);
    }
    if ((sfd == 1) && (rpm == 1)) {
        TX_X4_TX_TS_CTL_GEN2r_TX_SOP_BYTE_OFFSETf_SET(TX_X4_TX_TS_CTL_GEN2r_reg, 3);
    }
    if ((sfd == 1) && (rpm == 0)) {
        TX_X4_TX_TS_CTL_GEN2r_TX_SOP_BYTE_OFFSETf_SET(TX_X4_TX_TS_CTL_GEN2r_reg, 7);
    }
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_TX_TS_CTL_GEN2r(pc, TX_X4_TX_TS_CTL_GEN2r_reg));

    return PHYMOD_E_NONE;
}

int tbhmod_gen2_pcs_rx_ts_en(PHYMOD_ST* pc, uint32_t en)
{
    RX_X4_RX_TS_CTL_GEN2r_t RX_X4_RX_TS_CTL_GEN2r_reg;

    RX_X4_RX_TS_CTL_GEN2r_CLR(RX_X4_RX_TS_CTL_GEN2r_reg);

    RX_X4_RX_TS_CTL_GEN2r_TS_UPDATE_ENABLEf_SET(RX_X4_RX_TS_CTL_GEN2r_reg, en);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_TS_CTL_GEN2r(pc, RX_X4_RX_TS_CTL_GEN2r_reg));

    return PHYMOD_E_NONE;
}

#ifdef SERDES_API_FLOATING_POINT
static
float _tbhmod_gen2_pcs_vco_to_clk_period(uint32_t vco, int os_mode, int pam4)
{
    float clk_period = 0, vco_val = 0;

    switch (vco) {
      case TBHMOD_VCO_20G:
        vco_val = 20.6250;
        break;
      case TBHMOD_VCO_25G:
        vco_val = 25.8125;
        break;
      case TBHMOD_VCO_26G:
        vco_val = 26.56250;
      default:
        break;
    }

    if (pam4) {
        switch(os_mode) {
          case 0:
              /* OSX1 */
              clk_period = ((float) 20/vco_val);
              break;
          case 1:
              /* OSX2 */
              clk_period = ((float) 40/vco_val);
              break;
          default: break;
        }
    } else {
     /* NRZ mode */
        switch(os_mode) {
          case 0:
              /* OSX1 */
              clk_period = ((float) 20/vco_val);
              break;
          case 1:
              /* OSX2 */
              clk_period = ((float) 40/vco_val);
              break;
          case 2:
              /* OSX4 */
              clk_period = ((float) 80/vco_val);
              break;
          case 3:
              /* OSX2P5 */
              clk_period = ((float) 50/vco_val);
              break;
          case 5:
              clk_period = ((float) 40/vco_val);
              break;
          case 9:
              clk_period = ((float) 40/vco_val);
              break;
          case 13:
              clk_period = ((float) 40/vco_val);
              break;
          default: break;
        }

    }

    return clk_period;
}
#endif

/*!
 *  @brief tbhmod_set_pmd_timer_offset per port.
 *  @param unit number for instance lane number for decide which lane
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details PMD Timer offset value */
int tbhmod_gen2_set_pmd_timer_offset(PHYMOD_ST* pc, int ts_clk_period)
{
#ifdef SERDES_API_FLOATING_POINT
    uint16_t timer_offset;
    PMD_X1_PM_TMR_OFFS_GEN2r_t PMD_X1_PM_TMR_OFFS_GEN2r_reg;

    /* This timer_offset has 8-bit sub-ns fields. */
    timer_offset = (uint16_t) ((float) ((3 * ts_clk_period) * 256));

    PMD_X1_PM_TMR_OFFS_GEN2r_CLR(PMD_X1_PM_TMR_OFFS_GEN2r_reg);

    PMD_X1_PM_TMR_OFFS_GEN2r_PM_OFFSET_IN_NSf_SET (PMD_X1_PM_TMR_OFFS_GEN2r_reg, (timer_offset >> 8));
    PMD_X1_PM_TMR_OFFS_GEN2r_PM_OFFSET_SUB_NSf_SET(PMD_X1_PM_TMR_OFFS_GEN2r_reg, (timer_offset & 0xff));
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_PM_TMR_OFFS_GEN2r(pc, PMD_X1_PM_TMR_OFFS_GEN2r_reg));
#endif

    return PHYMOD_E_NONE;
}

static
int _tbhmod_gen2_pcs_vco_to_ui(uint32_t vco, int os_mode, int pam4)
{
    uint32_t ui_value;
    ui_value = 0;

    /* These are ideal values and when PMD is present these values are in sync with the VCos. */
    switch (vco) {
        case TBHMOD_VCO_20G:
            if (os_mode == 1) {
                ui_value = TBHMOD_GEN2_UI_20G_NRZ_OSX2; /* 2/20.625 = 0.09696.. make 32 bit 0.09696.. * 2^32 = 416481676 = 0x18d3018c */
            } else {
                ui_value = TBHMOD_GEN2_UI_20G_NRZ_OSX1; /* 1/20.625 = 0.04848.. make 32 bit 0.04848.. * 2^32 = 208240838 = 0xc6980c6 */
            }
            break;
        case TBHMOD_VCO_25G:
            if (pam4) {
                ui_value = TBHMOD_GEN2_UI_25G_PAM4;     /* 1/(2*25.78125) = 0.019393939.. make 32 bit 0.019393939.. * 2^32 = 83296335 = 0x4F7004F */

            } else {
                if (os_mode == 3) {   /* os_mode 2.5 */
                    ui_value = TBHMOD_GEN2_UI_25G_NRZ_OSX2P5; /* 2.5/25.78125 = 0.09696.. make 32 bit 0.09696.. * 2^32 = 416481676 = 0x18d3018c */
                } else {
                    ui_value = TBHMOD_GEN2_UI_25G_NRZ_OSX1; /* 1/25.78125 = 0.038787879.. make 32 bit 0.038787879.. * 2^32 = 166592670 = 0x09ee009e */
                }
            }
            break;
        case TBHMOD_VCO_26G:
            if (pam4) {
                ui_value = TBHMOD_GEN2_UI_26G_PAM4;   /* 1/(2*26.56250) = 0.018823529.. make 32 bit 0.018823529.. * 2^32 = 80846443 = 0x4D19E6B */
            } else {
                ui_value = TBHMOD_GEN2_UI_26G_NRZ;   /* 1/26.56250 = 0.037647059.. make 32 bit 0.038787879.. * 2^32 = 161692886 = 0x09a33cd6 */
            }
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    return ui_value;

}

/*!
 *  @brief tbhmod_gen2_pcs_set_1588_ui per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @param clk4sync_div the fast clk divider 0 => 6 and 1 => 8
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_gen2_pcs_set_1588_ui (PHYMOD_ST* pc, uint32_t vco, uint32_t vco1, int os_mode, int clk4sync_div, int pam4)
{
    PMD_X1_FCLK_PERIOD_GEN2r_t PMD_X1_FCLK_PERIOD_GEN2r_reg;
    PMD_X4_UI_VALUE_HI_GEN2r_t PMD_X4_UI_VALUE_HI_GEN2r_reg;
    PMD_X4_UI_VALUE_LO_GEN2r_t PMD_X4_UI_VALUE_LO_GEN2r_reg;
    int fclk_period = 0, ui_value_hi, ui_value_lo, ui_value_lo_m17_to_m23;


    ui_value_hi = _tbhmod_gen2_pcs_vco_to_ui(vco, os_mode, pam4) >> 16;
    ui_value_lo = _tbhmod_gen2_pcs_vco_to_ui(vco, os_mode, pam4) & 0xffff;

    /* ui_value_lo represent ui_value m17 to m32. For register programming,
     * we only need m17 to m23 value. So right shift 9 bits. */
    ui_value_lo_m17_to_m23 = ui_value_lo >> 9;


    switch(vco1) {
      case TBHMOD_VCO_20G:
        if (clk4sync_div) {
            fclk_period = TBHMOD_GEN2_FCLK_PERIOD_20G_DIV8;
        } else {
            fclk_period = TBHMOD_GEN2_FCLK_PERIOD_20G_DIV6;
        }
        break;
      case TBHMOD_VCO_25G:
        fclk_period = TBHMOD_GEN2_FCLK_PERIOD_25G;
        break;
      case TBHMOD_VCO_26G:
        fclk_period = TBHMOD_GEN2_FCLK_PERIOD_26G;
        break;
      default:
          return PHYMOD_E_PARAM;
    }


    PMD_X1_FCLK_PERIOD_GEN2r_CLR(PMD_X1_FCLK_PERIOD_GEN2r_reg);
    PMD_X4_UI_VALUE_HI_GEN2r_CLR(PMD_X4_UI_VALUE_HI_GEN2r_reg);
    PMD_X4_UI_VALUE_LO_GEN2r_CLR(PMD_X4_UI_VALUE_LO_GEN2r_reg);

    PMD_X1_FCLK_PERIOD_GEN2r_FCLK_FRAC_NSf_SET(PMD_X1_FCLK_PERIOD_GEN2r_reg, fclk_period);
    PMD_X4_UI_VALUE_HI_GEN2r_UI_FRAC_M1_TO_M16f_SET(PMD_X4_UI_VALUE_HI_GEN2r_reg, ui_value_hi);
    PMD_X4_UI_VALUE_LO_GEN2r_UI_FRAC_M17_TO_M23f_SET(PMD_X4_UI_VALUE_LO_GEN2r_reg, ui_value_lo_m17_to_m23);

    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X1_FCLK_PERIOD_GEN2r(pc, PMD_X1_FCLK_PERIOD_GEN2r_reg));
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_UI_VALUE_HI_GEN2r(pc, PMD_X4_UI_VALUE_HI_GEN2r_reg));
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_UI_VALUE_LO_GEN2r(pc, PMD_X4_UI_VALUE_LO_GEN2r_reg));

    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_1588_pmd_latency per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_gen2_1588_pmd_latency(PHYMOD_ST* pc, int an_en, uint32_t vco, int os_mode, int pam4)
{
#ifdef SERDES_API_FLOATING_POINT
    uint32_t tx_latency;
    uint32_t ui_hi_lo;
    float bit_ui_val, bit_ui_val1, bit_ui_val2;

    PMD_X4_TX_FIXED_LATENCY_GEN2r_t PMD_X4_TX_FIXED_LATENCY_GEN2r_reg;
    RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_t RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg;

    if (an_en) {
      PHYMOD_IF_ERR_RETURN(READ_RXCOM_OSR_MODE_STS_MC_MASK_GEN2r(pc, &RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg));
      os_mode = RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_RX_OSR_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg);
      pam4    = RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_RX_PAM4_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg);
    }

    /* ui_val with fractional bit 32 bit from _tbhmod_gen2_pcs_vco_to_ui we need 10 fractional
     * so we need 22 bit shift.
     * First shifting 8 then 14 to improve accuracy.
     */
    ui_hi_lo = _tbhmod_gen2_pcs_vco_to_ui(vco, os_mode, pam4);
    bit_ui_val1 = ((float) ui_hi_lo / 1024);
    bit_ui_val2 = ((float) bit_ui_val1 / 1024);
    bit_ui_val = ((float) bit_ui_val2 / 4096);

    /***
     * Please refer to the Blackhawk7 user spec datapath latency table
     * For PAM4 the pmd latency is based on the middle bit ie 20th bit of the 40bit interface.
     * Hence we subtract 20. Also for Pam4 each UI == 2bits hence we multiple by 2
 */
    if (pam4) {
        tx_latency = ((uint32_t)((bit_ui_val * ((TBHMOD_GEN2_PMD_TX_DP_LATENCY_PAM4 * 2) - 20)) * 1024));
    } else {
        tx_latency = ((uint32_t)((bit_ui_val * (TBHMOD_GEN2_PMD_TX_DP_LATENCY_NRZ - 20)) * 1024));  /* delay number from Blackhawk_V1.0 User Spec.pdf */
    }

      PMD_X4_TX_FIXED_LATENCY_GEN2r_CLR(PMD_X4_TX_FIXED_LATENCY_GEN2r_reg);


      PMD_X4_TX_FIXED_LATENCY_GEN2r_TX_PMD_LATENCY_IN_NSf_SET (PMD_X4_TX_FIXED_LATENCY_GEN2r_reg, (tx_latency >> 10));
      PMD_X4_TX_FIXED_LATENCY_GEN2r_TX_PMD_LATENCY_IN_FRAC_NSf_SET(PMD_X4_TX_FIXED_LATENCY_GEN2r_reg, (tx_latency & 0x3ff));

      PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_TX_FIXED_LATENCY_GEN2r(pc, PMD_X4_TX_FIXED_LATENCY_GEN2r_reg));
#endif
    return PHYMOD_E_NONE;
}

int tbhmod_gen2_update_tx_pmd_latency (PHYMOD_ST* pc, uint32_t latency_adj, int normalize_to_latest)
{
    uint32_t tx_latency, latency_ns, latency_frac_ns;

    PMD_X4_TX_FIXED_LATENCY_GEN2r_t PMD_X4_TX_FIXED_LATENCY_GEN2r_reg;

    PMD_X4_TX_FIXED_LATENCY_GEN2r_CLR(PMD_X4_TX_FIXED_LATENCY_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_PMD_X4_TX_FIXED_LATENCY_GEN2r(pc, &PMD_X4_TX_FIXED_LATENCY_GEN2r_reg));
    latency_ns =  PMD_X4_TX_FIXED_LATENCY_GEN2r_TX_PMD_LATENCY_IN_NSf_GET(PMD_X4_TX_FIXED_LATENCY_GEN2r_reg);
    latency_frac_ns = PMD_X4_TX_FIXED_LATENCY_GEN2r_TX_PMD_LATENCY_IN_FRAC_NSf_GET(PMD_X4_TX_FIXED_LATENCY_GEN2r_reg);

    if  (normalize_to_latest == 1) {
            tx_latency = ((latency_ns << 10) | latency_frac_ns) + latency_adj;
    } else {
        if  ((latency_ns >> 5) & 1)   {
            tx_latency = ((latency_ns << 10) | latency_frac_ns) + latency_adj;
        } else {
            tx_latency = ((latency_ns << 10) | latency_frac_ns) - latency_adj;
        }
    }

    PMD_X4_TX_FIXED_LATENCY_GEN2r_TX_PMD_LATENCY_IN_NSf_SET (PMD_X4_TX_FIXED_LATENCY_GEN2r_reg, (tx_latency >> 10));
    PMD_X4_TX_FIXED_LATENCY_GEN2r_TX_PMD_LATENCY_IN_FRAC_NSf_SET(PMD_X4_TX_FIXED_LATENCY_GEN2r_reg, (tx_latency & 0x3ff));
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_TX_FIXED_LATENCY_GEN2r(pc, PMD_X4_TX_FIXED_LATENCY_GEN2r_reg));

  return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_pcs_set_tx_lane_skew_capture per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_gen2_pcs_set_tx_lane_skew_capture (PHYMOD_ST* pc, int tx_skew_en)
{
    TX_X1_GLAS_TPMA_CTL_GEN2r_t   TX_X1_GLAS_TPMA_CTL_GEN2r_reg;
    PHYMOD_ST phy_copy;

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));
    TX_X1_GLAS_TPMA_CTL_GEN2r_CLR(TX_X1_GLAS_TPMA_CTL_GEN2r_reg);
    TX_X1_GLAS_TPMA_CTL_GEN2r_GLAS_TPMA_CAPTURE_ENf_SET(TX_X1_GLAS_TPMA_CTL_GEN2r_reg, tx_skew_en);
    TX_X1_GLAS_TPMA_CTL_GEN2r_GLAS_TPMA_CAPTURE_MASKf_SET(TX_X1_GLAS_TPMA_CTL_GEN2r_reg, pc->lane_mask);
    TX_X1_GLAS_TPMA_CTL_GEN2r_GLAS_TPMA_CAPTURE_LOGICALf_SET(TX_X1_GLAS_TPMA_CTL_GEN2r_reg, 1);

    phy_copy.lane_mask = 0x1;
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X1_GLAS_TPMA_CTL_GEN2r(&phy_copy, TX_X1_GLAS_TPMA_CTL_GEN2r_reg));

    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_pcs_measure_tx_lane_skew per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_gen2_pcs_measure_tx_lane_skew (PHYMOD_ST* pc, int an_en, uint32_t vco, int os_mode, int pam4, int normalize_to_latest, int *tx_max_skew)
{
#ifdef SERDES_API_FLOATING_POINT
    uint32_t latency_adj;
    uint32_t ui_hi_lo;
    float bit_ui_val, bit_ui_val1, bit_ui_val2;
    uint32_t fclk_frac_ns;
    uint32_t tx_lane_skew_bits[8], max_lane_skew_bits, min_lane_skew_bits, normalized_tx_lane_skew_bits[8];
    uint32_t tx_lane_skew_bits_ro[8], max_lane_skew_bits_ro, min_lane_skew_bits_ro;
    uint16_t glas_tpma_capture_data[8];
    uint8_t glas_tpma_capture_adj[8];
    float fclk_period, fclk_period_tmp, no_of_bits;
    int curr_lane;
    int i, start_lane, num_lane;
    PHYMOD_ST phy_copy;

    RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_t RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg;

    TX_X1_GLAS_TPMA_DATA_0_GEN2r_t TX_X1_GLAS_TPMA_DATA_0_GEN2r_reg;
    TX_X1_GLAS_TPMA_DATA_1_GEN2r_t TX_X1_GLAS_TPMA_DATA_1_GEN2r_reg;
    TX_X1_GLAS_TPMA_DATA_2_GEN2r_t TX_X1_GLAS_TPMA_DATA_2_GEN2r_reg;
    TX_X1_GLAS_TPMA_DATA_3_GEN2r_t TX_X1_GLAS_TPMA_DATA_3_GEN2r_reg;
    TX_X1_GLAS_TPMA_DATA_4_GEN2r_t TX_X1_GLAS_TPMA_DATA_4_GEN2r_reg;
    TX_X1_GLAS_TPMA_DATA_5_GEN2r_t TX_X1_GLAS_TPMA_DATA_5_GEN2r_reg;
    TX_X1_GLAS_TPMA_DATA_6_GEN2r_t TX_X1_GLAS_TPMA_DATA_6_GEN2r_reg;
    TX_X1_GLAS_TPMA_DATA_7_GEN2r_t TX_X1_GLAS_TPMA_DATA_7_GEN2r_reg;

    TX_X1_GLAS_TPMA_ADJ_0_1_GEN2r_t TX_X1_GLAS_TPMA_ADJ_0_1_GEN2r_reg;
    TX_X1_GLAS_TPMA_ADJ_2_3_GEN2r_t TX_X1_GLAS_TPMA_ADJ_2_3_GEN2r_reg;
    TX_X1_GLAS_TPMA_ADJ_4_5_GEN2r_t TX_X1_GLAS_TPMA_ADJ_4_5_GEN2r_reg;
    TX_X1_GLAS_TPMA_ADJ_6_7_GEN2r_t TX_X1_GLAS_TPMA_ADJ_6_7_GEN2r_reg;

    PMD_X1_FCLK_PERIOD_GEN2r_t PMD_X1_FCLK_PERIOD_GEN2r_reg;

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    for (i = 0; i < 8; i++) {
        tx_lane_skew_bits[i] = 0;
        normalized_tx_lane_skew_bits[i] = 0;
        glas_tpma_capture_data[i] = 0;
        glas_tpma_capture_adj[i] = 0;
    }

    max_lane_skew_bits = 0x0;
    min_lane_skew_bits = 0x2fff;
    max_lane_skew_bits_ro = 0x0;
    min_lane_skew_bits_ro = 0x2fff;
    no_of_bits = 0;


    if (an_en) {
        PHYMOD_IF_ERR_RETURN(READ_RXCOM_OSR_MODE_STS_MC_MASK_GEN2r(pc, &RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg));
        os_mode = RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_RX_OSR_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg);
        pam4    = RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_RX_PAM4_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg);
    }

    /* ui_val with fractional bit 32 bit from _tbhmod_gen2_pcs_vco_to_ui we need 10 fractional
     * so we need 22 bit shift.
     * First shifting 8 then 14 to improve accuracy.
     */
    ui_hi_lo = _tbhmod_gen2_pcs_vco_to_ui(vco, os_mode, pam4);
    bit_ui_val1 = ((float) ui_hi_lo / 1024);
    bit_ui_val2 = ((float) bit_ui_val1 / 1024);
    bit_ui_val = ((float) bit_ui_val2 / 4096);

    if (start_lane < 4) {
        phy_copy.lane_mask = 0x1;
    } else {
        phy_copy.lane_mask = 0x10;
    }
    PHYMOD_IF_ERR_RETURN(READ_PMD_X1_FCLK_PERIOD_GEN2r(&phy_copy, &PMD_X1_FCLK_PERIOD_GEN2r_reg));
    fclk_frac_ns = PMD_X1_FCLK_PERIOD_GEN2r_FCLK_FRAC_NSf_GET(PMD_X1_FCLK_PERIOD_GEN2r_reg);
    fclk_period_tmp =  ((float) fclk_frac_ns / 1024);
    fclk_period = ((float) fclk_period_tmp / 64);

    TX_X1_GLAS_TPMA_DATA_0_GEN2r_CLR(TX_X1_GLAS_TPMA_DATA_0_GEN2r_reg);
    TX_X1_GLAS_TPMA_DATA_1_GEN2r_CLR(TX_X1_GLAS_TPMA_DATA_1_GEN2r_reg);
    TX_X1_GLAS_TPMA_DATA_2_GEN2r_CLR(TX_X1_GLAS_TPMA_DATA_2_GEN2r_reg);
    TX_X1_GLAS_TPMA_DATA_3_GEN2r_CLR(TX_X1_GLAS_TPMA_DATA_3_GEN2r_reg);
    TX_X1_GLAS_TPMA_DATA_4_GEN2r_CLR(TX_X1_GLAS_TPMA_DATA_4_GEN2r_reg);
    TX_X1_GLAS_TPMA_DATA_5_GEN2r_CLR(TX_X1_GLAS_TPMA_DATA_5_GEN2r_reg);
    TX_X1_GLAS_TPMA_DATA_6_GEN2r_CLR(TX_X1_GLAS_TPMA_DATA_6_GEN2r_reg);
    TX_X1_GLAS_TPMA_DATA_7_GEN2r_CLR(TX_X1_GLAS_TPMA_DATA_7_GEN2r_reg);
    TX_X1_GLAS_TPMA_ADJ_0_1_GEN2r_CLR(TX_X1_GLAS_TPMA_ADJ_0_1_GEN2r_reg);
    TX_X1_GLAS_TPMA_ADJ_2_3_GEN2r_CLR(TX_X1_GLAS_TPMA_ADJ_2_3_GEN2r_reg);
    TX_X1_GLAS_TPMA_ADJ_4_5_GEN2r_CLR(TX_X1_GLAS_TPMA_ADJ_4_5_GEN2r_reg);
    TX_X1_GLAS_TPMA_ADJ_6_7_GEN2r_CLR(TX_X1_GLAS_TPMA_ADJ_6_7_GEN2r_reg);

    phy_copy.lane_mask = 0x1;
    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA_0_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_DATA_0_GEN2r_reg));
    glas_tpma_capture_data[0] = TX_X1_GLAS_TPMA_DATA_0_GEN2r_GLAS_TPMA_CAPTURE_DATA_0f_GET(TX_X1_GLAS_TPMA_DATA_0_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA_1_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_DATA_1_GEN2r_reg));
    glas_tpma_capture_data[1] = TX_X1_GLAS_TPMA_DATA_1_GEN2r_GLAS_TPMA_CAPTURE_DATA_1f_GET(TX_X1_GLAS_TPMA_DATA_1_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA_2_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_DATA_2_GEN2r_reg));
    glas_tpma_capture_data[2] = TX_X1_GLAS_TPMA_DATA_2_GEN2r_GLAS_TPMA_CAPTURE_DATA_2f_GET(TX_X1_GLAS_TPMA_DATA_2_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA_3_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_DATA_3_GEN2r_reg));
    glas_tpma_capture_data[3] = TX_X1_GLAS_TPMA_DATA_3_GEN2r_GLAS_TPMA_CAPTURE_DATA_3f_GET(TX_X1_GLAS_TPMA_DATA_3_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA_4_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_DATA_4_GEN2r_reg));
    glas_tpma_capture_data[4] = TX_X1_GLAS_TPMA_DATA_4_GEN2r_GLAS_TPMA_CAPTURE_DATA_4f_GET(TX_X1_GLAS_TPMA_DATA_4_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA_5_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_DATA_5_GEN2r_reg));
    glas_tpma_capture_data[5] = TX_X1_GLAS_TPMA_DATA_5_GEN2r_GLAS_TPMA_CAPTURE_DATA_5f_GET(TX_X1_GLAS_TPMA_DATA_5_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA_6_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_DATA_6_GEN2r_reg));
    glas_tpma_capture_data[6] = TX_X1_GLAS_TPMA_DATA_6_GEN2r_GLAS_TPMA_CAPTURE_DATA_6f_GET(TX_X1_GLAS_TPMA_DATA_6_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA_7_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_DATA_7_GEN2r_reg));
    glas_tpma_capture_data[7] = TX_X1_GLAS_TPMA_DATA_7_GEN2r_GLAS_TPMA_CAPTURE_DATA_7f_GET(TX_X1_GLAS_TPMA_DATA_7_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_ADJ_0_1_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_ADJ_0_1_GEN2r_reg));
    glas_tpma_capture_adj[0] = TX_X1_GLAS_TPMA_ADJ_0_1_GEN2r_GLAS_TPMA_CAPTURE_ADJ_0f_GET(TX_X1_GLAS_TPMA_ADJ_0_1_GEN2r_reg);
    glas_tpma_capture_adj[1] = TX_X1_GLAS_TPMA_ADJ_0_1_GEN2r_GLAS_TPMA_CAPTURE_ADJ_1f_GET(TX_X1_GLAS_TPMA_ADJ_0_1_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_ADJ_2_3_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_ADJ_2_3_GEN2r_reg));
    glas_tpma_capture_adj[2] = TX_X1_GLAS_TPMA_ADJ_2_3_GEN2r_GLAS_TPMA_CAPTURE_ADJ_2f_GET(TX_X1_GLAS_TPMA_ADJ_2_3_GEN2r_reg);
    glas_tpma_capture_adj[3] = TX_X1_GLAS_TPMA_ADJ_2_3_GEN2r_GLAS_TPMA_CAPTURE_ADJ_3f_GET(TX_X1_GLAS_TPMA_ADJ_2_3_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_ADJ_4_5_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_ADJ_4_5_GEN2r_reg));
    glas_tpma_capture_adj[4] = TX_X1_GLAS_TPMA_ADJ_4_5_GEN2r_GLAS_TPMA_CAPTURE_ADJ_4f_GET(TX_X1_GLAS_TPMA_ADJ_4_5_GEN2r_reg);
    glas_tpma_capture_adj[5] = TX_X1_GLAS_TPMA_ADJ_4_5_GEN2r_GLAS_TPMA_CAPTURE_ADJ_5f_GET(TX_X1_GLAS_TPMA_ADJ_4_5_GEN2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_ADJ_6_7_GEN2r(&phy_copy, &TX_X1_GLAS_TPMA_ADJ_6_7_GEN2r_reg));
    glas_tpma_capture_adj[6] = TX_X1_GLAS_TPMA_ADJ_6_7_GEN2r_GLAS_TPMA_CAPTURE_ADJ_6f_GET(TX_X1_GLAS_TPMA_ADJ_6_7_GEN2r_reg);
    glas_tpma_capture_adj[7] = TX_X1_GLAS_TPMA_ADJ_6_7_GEN2r_GLAS_TPMA_CAPTURE_ADJ_7f_GET(TX_X1_GLAS_TPMA_ADJ_6_7_GEN2r_reg);


    no_of_bits = fclk_period / bit_ui_val;

    for (i = 0; i < num_lane; i++) {
        tx_lane_skew_bits[start_lane + i] = glas_tpma_capture_data[start_lane + i] + (glas_tpma_capture_adj[start_lane + i] * no_of_bits);
        if (tx_lane_skew_bits[start_lane + i] > max_lane_skew_bits) {
            max_lane_skew_bits = tx_lane_skew_bits[start_lane + i];
        }
        if (tx_lane_skew_bits[start_lane + i] < min_lane_skew_bits) {
            min_lane_skew_bits = tx_lane_skew_bits[start_lane + i];
        }
    }


    if (normalize_to_latest == 1) {
        if ((max_lane_skew_bits - min_lane_skew_bits) > 0x1000) {
            for (i = 0; i < num_lane; i++) {
                if (max_lane_skew_bits - tx_lane_skew_bits[start_lane + i] > 0x1000) {
                    tx_lane_skew_bits_ro[start_lane + i] = 0x2000 | tx_lane_skew_bits[start_lane + i];
                } else {
                    tx_lane_skew_bits_ro[start_lane + i] = tx_lane_skew_bits[start_lane + i];
                }
                if (tx_lane_skew_bits_ro[start_lane + i] > max_lane_skew_bits_ro) {
                    max_lane_skew_bits_ro = tx_lane_skew_bits_ro[start_lane + i];
                }
                if (tx_lane_skew_bits_ro[start_lane + i] < min_lane_skew_bits_ro) {
                    min_lane_skew_bits_ro = tx_lane_skew_bits_ro[start_lane + i];
                }
            }
            for (i = 0; i < num_lane; i++) {
                normalized_tx_lane_skew_bits[start_lane + i] = tx_lane_skew_bits_ro[start_lane + i] - min_lane_skew_bits_ro;
            }
            *tx_max_skew = ((uint32_t) (max_lane_skew_bits_ro - min_lane_skew_bits_ro) * bit_ui_val * 1024);
        } else {
            for (i = 0; i < num_lane; i++) {
                normalized_tx_lane_skew_bits[start_lane + i] = tx_lane_skew_bits[start_lane + i] - min_lane_skew_bits;
            }
            *tx_max_skew = ((uint32_t) (max_lane_skew_bits - min_lane_skew_bits) * bit_ui_val * 1024);
        }
    } else { /*Normalize to earliest */
        if ((max_lane_skew_bits - min_lane_skew_bits) > 0x1000) {
            for (i = 0; i < num_lane; i++) {
                if (max_lane_skew_bits - tx_lane_skew_bits[start_lane + i] > 0x1000) {
                    tx_lane_skew_bits_ro[start_lane + i] = 0x2000 | tx_lane_skew_bits[start_lane + i];
                } else {
                    tx_lane_skew_bits_ro[start_lane + i] = tx_lane_skew_bits[start_lane + i];
                }
                if (tx_lane_skew_bits_ro[start_lane + i] > max_lane_skew_bits_ro) {
                    max_lane_skew_bits_ro = tx_lane_skew_bits_ro[start_lane + i];
                }
                if (tx_lane_skew_bits_ro[start_lane + i] < min_lane_skew_bits_ro) {
                    min_lane_skew_bits_ro = tx_lane_skew_bits_ro[start_lane + i];
                }
            }
            for (i = 0; i < num_lane; i++) {
                normalized_tx_lane_skew_bits[start_lane + i] = max_lane_skew_bits_ro - tx_lane_skew_bits_ro[start_lane + i];
            }
            *tx_max_skew = ((uint32_t) (max_lane_skew_bits_ro - min_lane_skew_bits_ro) * bit_ui_val * 1024);
        } else {
            for (i = 0; i < num_lane; i++) {
                normalized_tx_lane_skew_bits[start_lane + i] = max_lane_skew_bits - tx_lane_skew_bits[start_lane + i];
            }
            *tx_max_skew = ((uint32_t) (max_lane_skew_bits - min_lane_skew_bits) * bit_ui_val * 1024);
        }
    }

    for (i = 0; i < num_lane; i++) {
        curr_lane = start_lane + i;
        phy_copy.lane_mask = 0x1 << curr_lane;
        latency_adj = normalized_tx_lane_skew_bits[curr_lane] * bit_ui_val * 1024;
        PHYMOD_IF_ERR_RETURN
            (tbhmod_gen2_update_tx_pmd_latency(&phy_copy, latency_adj, normalize_to_latest));
    }
#endif

    return PHYMOD_E_NONE;
}

static
int _tbhmod_gen2_virtual_lane_count_get(int bit_mux_mode, int num_lane, int* virtual_lanes, int* num_psll_per_phyl)
{
    switch (bit_mux_mode) {
        case 0:
            *virtual_lanes = num_lane * 1;
            *num_psll_per_phyl = 1;
            break;
        case 1:
            *virtual_lanes = num_lane * 2;
            *num_psll_per_phyl = 2;
            break;
        case 2:
            *virtual_lanes = num_lane * 4;
            *num_psll_per_phyl = 4;
            break;
        case 3:
            *virtual_lanes = num_lane * 5;
            *num_psll_per_phyl = 5;
            break;
        default:
            *virtual_lanes = num_lane * 1;
            *num_psll_per_phyl = 1;
            break;
    }
    return PHYMOD_E_NONE;
}

int tbhmod_gen2_pcs_mod_rx_1588_tbl_val(PHYMOD_ST* pc, int an_en, int bit_mux_mode, uint32_t vco, int os_mode, int pam4, int tbl_ln, int normalize_to_latest, int rx_skew_adjust_enb, int *table , int *rx_max_skew) {
#ifdef SERDES_API_FLOATING_POINT
   int start_lane, num_lane;
   int i, curr_psll, num_psll_per_phyl;
   int virtual_lanes, physical_lanes;
   int32_t  table_time;

   /* per psuedo logical lane
    * the equation: {NS,sub_ns} + fclk_period * fclk_adjust + bit_offset * bit_ui_val
    * there are 4 logical lanes per mpp and 20 virtual lanes.
    */
    uint32_t psll_update[20];
    uint32_t ui_value;
    float bit_ui_val, bit_ui_val1, bit_ui_val2;
    uint16_t base_ts_ns;
    uint16_t base_ts_subns;
    uint16_t am_slip_cnt;
    uint8_t am_ts_fclk_adj_val;
    uint8_t deskew_ts_info_val;
    uint32_t fclk_period;
    uint32_t psll_val_min, psll_roval_min;
    uint32_t psll_val_max, psll_roval_max;

    int32_t  vlane_val_correction;
    uint32_t  psll_val_correction[20];
    int32_t  psll_to_vl_map[20];
    int8_t   vl_to_psll_map[20];
    int8_t   vl_to_psll_off_map[20];
    uint32_t curr_vl;
    float    rx_clk_period;
    float    ts_clk_period;
#ifdef _DV_TB_
    uint32_t tx_latency, rx_latency;
#endif
    PHYMOD_ST phy_copy;

    PMD_X1_FCLK_PERIOD_GEN2r_t PMD_X1_FCLK_PERIOD_GEN2r_reg;
    RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_t RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg;


    RX_X4_PSLL_TO_VL_MAP_0_GEN2r_t RX_X4_PSLL_TO_VL_MAP_0_GEN2r_reg;
    RX_X4_PSLL_TO_VL_MAP_1_GEN2r_t RX_X4_PSLL_TO_VL_MAP_1_GEN2r_reg;

    RX_X4_SKEW_OFFSS_0_GEN2r_t RX_X4_SKEW_OFFSS_0_GEN2r_reg;
    RX_X4_SKEW_OFFSS_1_GEN2r_t RX_X4_SKEW_OFFSS_1_GEN2r_reg;
    RX_X4_SKEW_OFFSS_2_GEN2r_t RX_X4_SKEW_OFFSS_2_GEN2r_reg;
    RX_X4_SKEW_OFFSS_3_GEN2r_t RX_X4_SKEW_OFFSS_3_GEN2r_reg;
    RX_X4_SKEW_OFFSS_4_GEN2r_t RX_X4_SKEW_OFFSS_4_GEN2r_reg;

    RX_X4_AM_TS_INFO_0_GEN2r_t RX_X4_AM_TS_INFO_0_GEN2r_reg;
    RX_X4_AM_TS_INFO_1_GEN2r_t RX_X4_AM_TS_INFO_1_GEN2r_reg;
    RX_X4_AM_TS_INFO_2_GEN2r_t RX_X4_AM_TS_INFO_2_GEN2r_reg;
    RX_X4_AM_TS_INFO_3_GEN2r_t RX_X4_AM_TS_INFO_3_GEN2r_reg;
    RX_X4_AM_TS_INFO_4_GEN2r_t RX_X4_AM_TS_INFO_4_GEN2r_reg;

    PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));

    ts_clk_period = 4.0;

    PHYMOD_IF_ERR_RETURN
      (_tbhmod_gen2_virtual_lane_count_get(bit_mux_mode, num_lane, &virtual_lanes, &num_psll_per_phyl));
    physical_lanes = num_lane;

    if (an_en) {
        PHYMOD_IF_ERR_RETURN(READ_RXCOM_OSR_MODE_STS_MC_MASK_GEN2r(pc, &RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg));
        os_mode = RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_RX_OSR_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg);
        pam4    = RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_RX_PAM4_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASK_GEN2r_reg);
    }

    ui_value = _tbhmod_gen2_pcs_vco_to_ui(vco, os_mode, pam4);
    rx_clk_period = _tbhmod_gen2_pcs_vco_to_clk_period(vco, os_mode, pam4);

    /* ui_val with fractional bit 32 bit from _tbhmod_gen2_pcs_vco_to_ui we need 10 fractional
     * so we need 22 bit shift.
     * First shifting 8 then 14 to improve accuracy.
     */
    bit_ui_val1 = ((float) ui_value / 1024);
    bit_ui_val2 = ((float) bit_ui_val1 / 1024);
    bit_ui_val = ((float) bit_ui_val2 / 4096);

#ifdef _DV_TB_
    /***
     * Please refer to the Blackhawk7 user spec datapath latency table
     * For PAM4 the pmd latency is based on the middle bit ie 20th bit of the 40bit interface.
     * Hence we subtract 20. Also for Pam4 each UI == 2bits hence we multiple by 2
 */
    if (pam4) {
        rx_latency = ((uint32_t)((float) ((bit_ui_val * ((TBHMOD_GEN2_PMD_RX_DP_LATENCY_PAM4 * 2) + 20)) * 16)));
        tx_latency = ((uint32_t)((float) ((bit_ui_val * ((TBHMOD_GEN2_PMD_TX_DP_LATENCY_PAM4 * 2) - 20)) * 16)));
    } else {
        rx_latency = ((uint32_t)((float) ((bit_ui_val * (TBHMOD_GEN2_PMD_RX_DP_LATENCY_NRZ + 20)) * 16)));
        tx_latency = ((uint32_t)((float) ((bit_ui_val * (TBHMOD_GEN2_PMD_TX_DP_LATENCY_NRZ - 20)) * 16)));  /* delay number from Blackhawk_V1.0 User Spec.pdf */
    }
#endif

    psll_val_min = 0x80000;
    psll_roval_min = 0x80000;
    psll_val_max = 0;
    psll_roval_max = 0;
    curr_psll = 0;
    for (i = 0; i < 20; i++) {
        psll_val_correction[i] = 0;
        psll_to_vl_map[i] = 0;
        psll_update[i] = 0;
        vl_to_psll_map[i] = 0;
        vl_to_psll_off_map[i] = 0;
    }
    vlane_val_correction = 0;

    if (start_lane < 4) {
        phy_copy.lane_mask = 0x1;
    } else {
        phy_copy.lane_mask = 0x10;
    }
    PHYMOD_IF_ERR_RETURN(READ_PMD_X1_FCLK_PERIOD_GEN2r(&phy_copy, &PMD_X1_FCLK_PERIOD_GEN2r_reg));
    fclk_period = PMD_X1_FCLK_PERIOD_GEN2r_FCLK_FRAC_NSf_GET(PMD_X1_FCLK_PERIOD_GEN2r_reg);

    if (rx_skew_adjust_enb == 1) {
        if (virtual_lanes > 1) {
            for (i = 0; i < physical_lanes; i++) {
                  phy_copy.lane_mask = 0x1 << (start_lane + i);
                  PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP_0_GEN2r(&phy_copy, &RX_X4_PSLL_TO_VL_MAP_0_GEN2r_reg));
                  PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP_1_GEN2r(&phy_copy, &RX_X4_PSLL_TO_VL_MAP_1_GEN2r_reg));

                  if (num_psll_per_phyl > 0) {
                      curr_vl  = RX_X4_PSLL_TO_VL_MAP_0_GEN2r_PSLL0_TO_VL_MAPPINGf_GET(RX_X4_PSLL_TO_VL_MAP_0_GEN2r_reg);
                      psll_to_vl_map[curr_vl] = curr_psll;
                      vl_to_psll_map[curr_vl] = start_lane + i;
                      vl_to_psll_off_map[curr_vl] = 0;
                      curr_psll++;
                  }
                  if (num_psll_per_phyl > 1) {
                      curr_vl = RX_X4_PSLL_TO_VL_MAP_0_GEN2r_PSLL1_TO_VL_MAPPINGf_GET(RX_X4_PSLL_TO_VL_MAP_0_GEN2r_reg);
                      psll_to_vl_map[curr_vl] = curr_psll;
                      vl_to_psll_map[curr_vl] = start_lane + i;
                      vl_to_psll_off_map[curr_vl] = 1;
                      curr_psll++;
                  }
                  if (num_psll_per_phyl > 2) {
                      curr_vl = RX_X4_PSLL_TO_VL_MAP_0_GEN2r_PSLL2_TO_VL_MAPPINGf_GET(RX_X4_PSLL_TO_VL_MAP_0_GEN2r_reg);
                      psll_to_vl_map[curr_vl] = curr_psll;
                      vl_to_psll_map[curr_vl] = start_lane + i;
                      vl_to_psll_off_map[curr_vl] = 2;
                      curr_psll++;
                  }
                  if (num_psll_per_phyl > 3) {
                      curr_vl = RX_X4_PSLL_TO_VL_MAP_1_GEN2r_PSLL3_TO_VL_MAPPINGf_GET(RX_X4_PSLL_TO_VL_MAP_1_GEN2r_reg);
                      psll_to_vl_map[curr_vl] = curr_psll;
                      vl_to_psll_map[curr_vl] = start_lane + i;
                      vl_to_psll_off_map[curr_vl] = 3;
                      curr_psll++;
                  }
                  if (num_psll_per_phyl > 4) {
                      curr_vl = RX_X4_PSLL_TO_VL_MAP_1_GEN2r_PSLL4_TO_VL_MAPPINGf_GET(RX_X4_PSLL_TO_VL_MAP_1_GEN2r_reg);
                      psll_to_vl_map[curr_vl] = curr_psll;
                      vl_to_psll_map[curr_vl] = start_lane + i;
                      vl_to_psll_off_map[curr_vl] = 4;
                      curr_psll++;
                  }
            }

            for (i = 0; i < virtual_lanes; i++) {
/*
                phy_copy.lane_mask  = 0x1 << (start_lane + (i / num_psll_per_phyl));
                if (physical_lanes > 4) {
                    phy_copy.lane_mask = 0x1 << (start_lane + (i / 2));
                }
*/
                phy_copy.lane_mask  = 0x1 << vl_to_psll_map[i];
                if (vl_to_psll_off_map[i] == 0) {
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS_0_GEN2r(&phy_copy, &RX_X4_SKEW_OFFSS_0_GEN2r_reg));
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_AM_TS_INFO_0_GEN2r(&phy_copy, &RX_X4_AM_TS_INFO_0_GEN2r_reg));
                    base_ts_ns = RX_X4_AM_TS_INFO_0_GEN2r_BASE_TS_NS_0f_GET(RX_X4_AM_TS_INFO_0_GEN2r_reg);
                    base_ts_subns = RX_X4_AM_TS_INFO_0_GEN2r_BASE_TS_SUB_NS_0f_GET(RX_X4_AM_TS_INFO_0_GEN2r_reg);
                    am_slip_cnt = RX_X4_SKEW_OFFSS_0_GEN2r_AM_SLIP_COUNT_0f_GET(RX_X4_SKEW_OFFSS_0_GEN2r_reg);
                    am_ts_fclk_adj_val = RX_X4_SKEW_OFFSS_0_GEN2r_AM_TS_FCLK_ADJUST_VALUE_0f_GET(RX_X4_SKEW_OFFSS_0_GEN2r_reg);
                    deskew_ts_info_val = RX_X4_SKEW_OFFSS_0_GEN2r_DESKEW_TS_INFO_VALID_0f_GET(RX_X4_SKEW_OFFSS_0_GEN2r_reg);
                    if (deskew_ts_info_val == 1) {
                        psll_update[i] = ((base_ts_ns << 4) | (base_ts_subns & 0xf)) + ((uint32_t) ((float) (fclk_period * am_ts_fclk_adj_val * 16 / 65536))) + ((uint32_t) ((float)(am_slip_cnt * bit_ui_val * 16))) ;
                        if (psll_update[i] < psll_val_min) {
                            psll_val_min = psll_update[i];
                        }
                        if (psll_update[i] > psll_val_max) {
                            psll_val_max = psll_update[i];
                        }
                    }
                } else if (vl_to_psll_off_map[i] == 1) {
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS_1_GEN2r(&phy_copy, &RX_X4_SKEW_OFFSS_1_GEN2r_reg));
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_AM_TS_INFO_1_GEN2r(&phy_copy, &RX_X4_AM_TS_INFO_1_GEN2r_reg));
                    base_ts_ns = RX_X4_AM_TS_INFO_1_GEN2r_BASE_TS_NS_1f_GET(RX_X4_AM_TS_INFO_1_GEN2r_reg);
                    base_ts_subns = RX_X4_AM_TS_INFO_1_GEN2r_BASE_TS_SUB_NS_1f_GET(RX_X4_AM_TS_INFO_1_GEN2r_reg);
                    am_slip_cnt = RX_X4_SKEW_OFFSS_1_GEN2r_AM_SLIP_COUNT_1f_GET(RX_X4_SKEW_OFFSS_1_GEN2r_reg);
                    am_ts_fclk_adj_val = RX_X4_SKEW_OFFSS_1_GEN2r_AM_TS_FCLK_ADJUST_VALUE_1f_GET(RX_X4_SKEW_OFFSS_1_GEN2r_reg);
                    deskew_ts_info_val = RX_X4_SKEW_OFFSS_1_GEN2r_DESKEW_TS_INFO_VALID_1f_GET(RX_X4_SKEW_OFFSS_1_GEN2r_reg);
                    if (deskew_ts_info_val == 1) {
                        psll_update[i] = ((base_ts_ns << 4) | (base_ts_subns & 0xf)) + ((uint32_t) ((float) (fclk_period * am_ts_fclk_adj_val * 16 / 65536))) + ((uint32_t) ((float)(am_slip_cnt * bit_ui_val * 16))) ;
                        if (psll_update[i] < psll_val_min) {
                            psll_val_min = psll_update[i];
                        }
                        if (psll_update[i] > psll_val_max) {
                            psll_val_max = psll_update[i];
                        }
                    }
                } else if (vl_to_psll_off_map[i] == 2) {
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS_2_GEN2r(&phy_copy, &RX_X4_SKEW_OFFSS_2_GEN2r_reg));
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_AM_TS_INFO_2_GEN2r(&phy_copy, &RX_X4_AM_TS_INFO_2_GEN2r_reg));
                    base_ts_ns = RX_X4_AM_TS_INFO_2_GEN2r_BASE_TS_NS_2f_GET(RX_X4_AM_TS_INFO_2_GEN2r_reg);
                    base_ts_subns = RX_X4_AM_TS_INFO_2_GEN2r_BASE_TS_SUB_NS_2f_GET(RX_X4_AM_TS_INFO_2_GEN2r_reg);
                    am_slip_cnt = RX_X4_SKEW_OFFSS_2_GEN2r_AM_SLIP_COUNT_2f_GET(RX_X4_SKEW_OFFSS_2_GEN2r_reg);
                    am_ts_fclk_adj_val = RX_X4_SKEW_OFFSS_2_GEN2r_AM_TS_FCLK_ADJUST_VALUE_2f_GET(RX_X4_SKEW_OFFSS_2_GEN2r_reg);
                    deskew_ts_info_val = RX_X4_SKEW_OFFSS_2_GEN2r_DESKEW_TS_INFO_VALID_2f_GET(RX_X4_SKEW_OFFSS_2_GEN2r_reg);
                    if (deskew_ts_info_val == 1) {
                        psll_update[i] = ((base_ts_ns << 4) | (base_ts_subns & 0xf)) + ((uint32_t) ((float) (fclk_period * am_ts_fclk_adj_val * 16 / 65536))) + ((uint32_t) ((float)(am_slip_cnt * bit_ui_val * 16))) ;
                        if ( psll_update[i] < psll_val_min) {
                            psll_val_min = psll_update[i];
                        }
                        if ( psll_update[i] > psll_val_max) {
                            psll_val_max = psll_update[i];
                        }
                    }
                } else if (vl_to_psll_off_map[i] == 3) {
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS_3_GEN2r(&phy_copy, &RX_X4_SKEW_OFFSS_3_GEN2r_reg));
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_AM_TS_INFO_3_GEN2r(&phy_copy, &RX_X4_AM_TS_INFO_3_GEN2r_reg));
                    base_ts_ns = RX_X4_AM_TS_INFO_3_GEN2r_BASE_TS_NS_3f_GET(RX_X4_AM_TS_INFO_3_GEN2r_reg);
                    base_ts_subns = RX_X4_AM_TS_INFO_3_GEN2r_BASE_TS_SUB_NS_3f_GET(RX_X4_AM_TS_INFO_3_GEN2r_reg);
                    am_slip_cnt = RX_X4_SKEW_OFFSS_3_GEN2r_AM_SLIP_COUNT_3f_GET(RX_X4_SKEW_OFFSS_3_GEN2r_reg);
                    am_ts_fclk_adj_val = RX_X4_SKEW_OFFSS_3_GEN2r_AM_TS_FCLK_ADJUST_VALUE_3f_GET(RX_X4_SKEW_OFFSS_3_GEN2r_reg);
                    deskew_ts_info_val = RX_X4_SKEW_OFFSS_3_GEN2r_DESKEW_TS_INFO_VALID_3f_GET(RX_X4_SKEW_OFFSS_3_GEN2r_reg);
                    if (deskew_ts_info_val == 1) {
                        psll_update[i] = ((base_ts_ns << 4) | (base_ts_subns & 0xf)) + ((uint32_t) ((float) (fclk_period * am_ts_fclk_adj_val * 16 / 65536))) + ((uint32_t) ((float)(am_slip_cnt * bit_ui_val * 16))) ;
                        if ( psll_update[i] < psll_val_min) {
                            psll_val_min = psll_update[i];
                        }
                        if ( psll_update[i] > psll_val_max) {
                            psll_val_max = psll_update[i];
                        }
                    }
                } else if (vl_to_psll_off_map[i] == 4) {
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS_4_GEN2r(&phy_copy, &RX_X4_SKEW_OFFSS_4_GEN2r_reg));
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_AM_TS_INFO_4_GEN2r(&phy_copy, &RX_X4_AM_TS_INFO_4_GEN2r_reg));
                    base_ts_ns = RX_X4_AM_TS_INFO_4_GEN2r_BASE_TS_NS_4f_GET(RX_X4_AM_TS_INFO_4_GEN2r_reg);
                    base_ts_subns = RX_X4_AM_TS_INFO_4_GEN2r_BASE_TS_SUB_NS_4f_GET(RX_X4_AM_TS_INFO_4_GEN2r_reg);
                    am_slip_cnt = RX_X4_SKEW_OFFSS_4_GEN2r_AM_SLIP_COUNT_4f_GET(RX_X4_SKEW_OFFSS_4_GEN2r_reg);
                    am_ts_fclk_adj_val = RX_X4_SKEW_OFFSS_4_GEN2r_AM_TS_FCLK_ADJUST_VALUE_4f_GET(RX_X4_SKEW_OFFSS_4_GEN2r_reg);
                    deskew_ts_info_val = RX_X4_SKEW_OFFSS_4_GEN2r_DESKEW_TS_INFO_VALID_4f_GET(RX_X4_SKEW_OFFSS_4_GEN2r_reg);
                    if (deskew_ts_info_val == 1) {
                        psll_update[i] = ((base_ts_ns << 4) | (base_ts_subns & 0xf)) + ((uint32_t) ((float) (fclk_period*am_ts_fclk_adj_val * 16 / 65536))) + ((uint32_t) ((float)(am_slip_cnt * bit_ui_val * 16))) ;
                        if ( psll_update[i] < psll_val_min) {
                            psll_val_min = psll_update[i];
                        }
                        if ( psll_update[i] > psll_val_max) {
                            psll_val_max = psll_update[i];
                        }
                    }
                }
            }

            if (normalize_to_latest == 1) {
                /* Normalize to latest */
                if ((psll_val_max - psll_val_min) > 0x1000) {
                    /* Sometimes the psll_update could rollover. In case of rollover all the values need to
                     * equalized meaning account for rollover and then normalize it as usual. */
                    for (i = 0; i < virtual_lanes; i++) {
                        if ((psll_val_max - psll_update[i]) > 0x01000) {
                            psll_update[i] |= 0x10000 ;
                        }
                        if (psll_update[i] < psll_roval_min) {
                            psll_roval_min = psll_update[i];
                        }
                        if (psll_update[i] > psll_roval_max) {
                            psll_roval_max = psll_update[i];
                        }
                    }
                    for (i = 0; i < virtual_lanes; i++) {
                        psll_val_correction[i] = psll_roval_max - psll_update[i];
                    }
                    *rx_max_skew = psll_roval_max - psll_roval_min;
                } else {
                    for (i = 0; i < virtual_lanes; i++) {
                        psll_val_correction[i] = psll_val_max - psll_update[i];
                    }
                    *rx_max_skew = psll_val_max - psll_val_min;
                }
            } else { /* Normalize to earliest */
                if ((psll_val_max - psll_val_min) > 0x1000) {
                    /* Sometimes the psll_update could rollover. In case of rollover all the values need to
                     * equalized meaning account for rollover and then normalize it as usual. */
                    for (i = 0; i < virtual_lanes; i++) {
                        if ((psll_val_max - psll_update[i]) > 0x01000) {
                            psll_update[i] |= 0x10000 ;
                        }
                        if (psll_update[i] < psll_roval_min) {
                            psll_roval_min = psll_update[i];
                        }
                        if (psll_update[i] > psll_roval_max) {
                            psll_roval_max = psll_update[i];
                        }
                    }
                    for (i = 0; i < virtual_lanes; i++) {
                        psll_val_correction[i] = psll_update[i] - psll_roval_min;
                    }
                    *rx_max_skew = psll_roval_max - psll_roval_min;
                } else {
                    for (i = 0; i < virtual_lanes; i++) {
                        psll_val_correction[i] = psll_update[i] - psll_val_min;
                    }
                    *rx_max_skew = psll_val_max - psll_val_min;
                }
            }

        }
    } else if (rx_skew_adjust_enb == 0) {
      /* the skew correction is disabled */
      *rx_max_skew = 0;
    }

    for (i = 0; i < tbl_ln; i++) {
        curr_vl = table[i] >> 15;
        table_time = table[i] & 0x7fff;
        if ((table_time >> 14) & 1) {
            table_time = (table_time | 0xffff8000);
        }

        curr_psll = psll_to_vl_map[curr_vl];

        vlane_val_correction = psll_val_correction[curr_psll];

        if (normalize_to_latest == 1) {
            table_time = table_time +  (vlane_val_correction & 0xffff) + ((uint32_t) ((float)((rx_clk_period*4 + ts_clk_period * 3) * 16)));
        } else {
            vlane_val_correction = 0x8000 - vlane_val_correction;
            table_time = table_time +  (vlane_val_correction & 0x7fff) + ((uint32_t) ((float)((rx_clk_period*4 + ts_clk_period * 3) * 16)));

        }

        table[i] = (table[i] & 0xf8000) | (table_time & 0x7fff);
    }
#endif
    return PHYMOD_E_NONE;
}

int tbhmod_gen2_pcs_ts_deskew_valid(PHYMOD_ST* pc, int bit_mux_mode, int *rx_ts_deskew_valid) {

    int i, num_lane, start_lane;
    int virtual_lanes, physical_lanes, num_psll_per_phyl;
    int ts_deskew_valid = 1;
    PHYMOD_ST phy_copy;

    RX_X4_SKEW_OFFSS_0_GEN2r_t RX_X4_SKEW_OFFSS_0_GEN2r_reg;
    RX_X4_SKEW_OFFSS_1_GEN2r_t RX_X4_SKEW_OFFSS_1_GEN2r_reg;
    RX_X4_SKEW_OFFSS_2_GEN2r_t RX_X4_SKEW_OFFSS_2_GEN2r_reg;
    RX_X4_SKEW_OFFSS_3_GEN2r_t RX_X4_SKEW_OFFSS_3_GEN2r_reg;
    RX_X4_SKEW_OFFSS_4_GEN2r_t RX_X4_SKEW_OFFSS_4_GEN2r_reg;

    PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (_tbhmod_gen2_virtual_lane_count_get(bit_mux_mode, num_lane, &virtual_lanes, &num_psll_per_phyl));
    physical_lanes = num_lane;

    if (virtual_lanes == 1) {
        *rx_ts_deskew_valid = 1;
        return PHYMOD_E_NONE;
    }

    for (i = 0; i < virtual_lanes; i++) {
        phy_copy.lane_mask = 0x1 << (start_lane + i % num_lane);
        if ((i / physical_lanes) == 0) {
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS_0_GEN2r(&phy_copy, &RX_X4_SKEW_OFFSS_0_GEN2r_reg));
            ts_deskew_valid &= RX_X4_SKEW_OFFSS_0_GEN2r_DESKEW_TS_INFO_VALID_0f_GET(RX_X4_SKEW_OFFSS_0_GEN2r_reg);
        } else if ((i / physical_lanes) == 1) {
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS_1_GEN2r(&phy_copy, &RX_X4_SKEW_OFFSS_1_GEN2r_reg));
            ts_deskew_valid &= RX_X4_SKEW_OFFSS_1_GEN2r_DESKEW_TS_INFO_VALID_1f_GET(RX_X4_SKEW_OFFSS_1_GEN2r_reg);
        } else if ((i / physical_lanes) == 2) {
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS_2_GEN2r(&phy_copy, &RX_X4_SKEW_OFFSS_2_GEN2r_reg));
            ts_deskew_valid &= RX_X4_SKEW_OFFSS_2_GEN2r_DESKEW_TS_INFO_VALID_2f_GET(RX_X4_SKEW_OFFSS_2_GEN2r_reg);
        } else if ((i / physical_lanes) == 3) {
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS_3_GEN2r(&phy_copy, &RX_X4_SKEW_OFFSS_3_GEN2r_reg));
            ts_deskew_valid &= RX_X4_SKEW_OFFSS_3_GEN2r_DESKEW_TS_INFO_VALID_3f_GET(RX_X4_SKEW_OFFSS_3_GEN2r_reg);
        } else if ((i / physical_lanes) == 4) {
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS_4_GEN2r(&phy_copy, &RX_X4_SKEW_OFFSS_4_GEN2r_reg));
            ts_deskew_valid &= RX_X4_SKEW_OFFSS_4_GEN2r_DESKEW_TS_INFO_VALID_4f_GET(RX_X4_SKEW_OFFSS_4_GEN2r_reg);
        }
    }
    *rx_ts_deskew_valid = ts_deskew_valid;

    return PHYMOD_E_NONE;
}

int tbhmod_gen2_tx_ts_info_unpack_tx_ts_tbl_entry(uint32_t *tx_ts_tbl_entry, phymod_ts_fifo_status_t *tx_ts_info)
{
    int valid;

    /*
     *  ts_sub_nano_field = mem_tsts_fifo_data[3:0];
     *  ts_value_lo       = mem_tsts_fifo_data[19:4];
     *  ts_value_mid      = mem_tsts_fifo_data[35:20];
     *  ts_value_hi       = mem_tsts_fifo_data[51:36];
     *  ts_sequence_id    = mem_tsts_fifo_data[67:52];
     *  ts_valid          = mem_tsts_info_data[68];
     */

    valid = (tx_ts_tbl_entry[2] >> 4) & 0x1;

    if (valid) {
        tx_ts_info->ts_in_fifo_lo = ((tx_ts_tbl_entry[1] << 28) & 0xf0000000) | ((tx_ts_tbl_entry[0] >> 4) & 0xfffffff);
        tx_ts_info->ts_in_fifo_hi = (tx_ts_tbl_entry[1] >> 4) & 0xffff;
        tx_ts_info->ts_seq_id = ((tx_ts_tbl_entry[2] << 12) & 0xf000) | ((tx_ts_tbl_entry[1] >> 20) & 0xfff);
        tx_ts_info->ts_sub_nanosec = tx_ts_tbl_entry[0] & 0xf;
    } else {
        return PHYMOD_E_EMPTY;
    }

    return PHYMOD_E_NONE;
}
/* this function needs to be called if pcs reset is set
only forced speed will be affected by this HW issue */

int tbhmod_gen2_pcs_reset_sw_war(const PHYMOD_ST* pc)
{
    SC_X4_CTL_GEN2r_t reg_sc_ctrl;
    TXFIR_MISC_CTL0_GEN2r_t txfir_misc_ctrl;
    RX_X4_PCS_LATCH_STS_1_GEN2r_t link_status_reg;
    int start_lane, num_lane, i;
    PHYMOD_ST phy_copy;

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));


    /*first check live status, if link status is down,
    nothing is needed */
    PHYMOD_IF_ERR_RETURN(READ_RX_X4_PCS_LATCH_STS_1_GEN2r(pc, &link_status_reg));

    if (RX_X4_PCS_LATCH_STS_1_GEN2r_PCS_LINK_STATUS_LIVEf_GET(link_status_reg)) {
        SC_X4_CTL_GEN2r_CLR(reg_sc_ctrl);
        TXFIR_MISC_CTL0_GEN2r_CLR(txfir_misc_ctrl);
        /* first disable all lane tx */
        TXFIR_MISC_CTL0_GEN2r_SDK_TX_DISABLEf_SET(txfir_misc_ctrl, 1);
        for (i = 0; i < num_lane; i++) {
            phy_copy.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
                (MODIFY_TXFIR_MISC_CTL0_GEN2r(&phy_copy, txfir_misc_ctrl));
        }
        /* next  toggle speed control bit */
        SC_X4_CTL_GEN2r_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 1);
        phy_copy.lane_mask = 1 << start_lane;
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTL_GEN2r(&phy_copy, reg_sc_ctrl));
        /* then sleep for 5 micro-second */
        PHYMOD_USLEEP(5);

        SC_X4_CTL_GEN2r_SW_SPEED_CHANGEf_SET(reg_sc_ctrl, 0);
        PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTL_GEN2r(&phy_copy, reg_sc_ctrl));
        /* then sleep for 5 micro-second */
        PHYMOD_USLEEP(5);

        /* last, disable tx_disable */
        TXFIR_MISC_CTL0_GEN2r_SDK_TX_DISABLEf_SET(txfir_misc_ctrl, 0);
        for (i = 0; i < num_lane; i++) {
            phy_copy.lane_mask = 1 << (start_lane + i);
            PHYMOD_IF_ERR_RETURN
                (MODIFY_TXFIR_MISC_CTL0_GEN2r(&phy_copy, txfir_misc_ctrl));
        }
    }

    return PHYMOD_E_NONE;
}

/* Enable PCS clock block */
int tbhmod_gen2_pcs_clk_blk_en(const PHYMOD_ST* pc, uint32_t en)
{
    AMS_PLL_PLL_CTL_12_GEN2r_t reg_pll_ctrl;

    AMS_PLL_PLL_CTL_12_GEN2r_CLR(reg_pll_ctrl);
    AMS_PLL_PLL_CTL_12_GEN2r_AMS_PLL_EN_CLK4PCSf_SET(reg_pll_ctrl, en);

    PHYMOD_IF_ERR_RETURN
       (MODIFY_AMS_PLL_PLL_CTL_12_GEN2r(pc, reg_pll_ctrl));

    return PHYMOD_E_NONE;
}

/**
 * @brief   fec three_cw_bad state get
 * @param   pc handle to current TSCBH context (#tbhmod_st)
 * @returns The value PHYMOD_E_NONE upon successful completion
 * @details fec three_cw_bad state get.
 */
int tbhmod_gen2_fec_three_cw_bad_state_get(PHYMOD_ST* pc, uint32_t *state)
{
  RX_X4_RX_LATCH_STS_GEN2r_t rx_latch_sts;

  TBHMOD_DBG_IN_FUNC_INFO(pc);

  RX_X4_RX_LATCH_STS_GEN2r_CLR(rx_latch_sts);
  PHYMOD_IF_ERR_RETURN(READ_RX_X4_RX_LATCH_STS_GEN2r(pc, &rx_latch_sts));
  *state = RX_X4_RX_LATCH_STS_GEN2r_DESKEW_HIS_STATEf_GET(rx_latch_sts) & 0x20;

  return PHYMOD_E_NONE;
}
