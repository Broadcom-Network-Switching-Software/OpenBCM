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
 *  @brief tbhmod_fe_gen2_pcs_tx_fclk_sel_set per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param pll  pll number
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Select FCLK  in tx path */
int tbhmod_fe_gen2_pcs_tx_fclk_sel_set(PHYMOD_ST* pc, int pll)
{
    PMD_X4_FCLK_AFE_PLL_SELr_t reg_pll_sel;

    PMD_X4_FCLK_AFE_PLL_SELr_CLR(reg_pll_sel);
    PMD_X4_FCLK_AFE_PLL_SELr_FCLK_SEL_PLL_TXf_SET(reg_pll_sel, pll);
    PHYMOD_IF_ERR_RETURN
        (MODIFY_PMD_X4_FCLK_AFE_PLL_SELr(pc, reg_pll_sel));
    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_fe_gen2_pcs_tx_fclk_sel_set per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param pll  pll number
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Select FCLK  in tx path */
int tbhmod_fe_gen2_pcs_tx_fclk_sel_get(PHYMOD_ST* pc, int *pll)
{
    PMD_X4_FCLK_AFE_PLL_SELr_t reg_pll_sel;

    PHYMOD_IF_ERR_RETURN
        (READ_PMD_X4_FCLK_AFE_PLL_SELr(pc, &reg_pll_sel));
    *pll = PMD_X4_FCLK_AFE_PLL_SELr_FCLK_SEL_PLL_TXf_GET(reg_pll_sel);

    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_fe_gen2_pcs_rx_fclk_sel_set per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param pll  pll number
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Select FCLK  in rx path */
int tbhmod_fe_gen2_pcs_rx_fclk_sel_set(PHYMOD_ST* pc, int pll)
{
    PMD_X4_FCLK_AFE_PLL_SELr_t reg_pll_sel;

    PMD_X4_FCLK_AFE_PLL_SELr_CLR(reg_pll_sel);
    PMD_X4_FCLK_AFE_PLL_SELr_FCLK_SEL_PLL_RXf_SET(reg_pll_sel, pll);
    PHYMOD_IF_ERR_RETURN
        (MODIFY_PMD_X4_FCLK_AFE_PLL_SELr(pc, reg_pll_sel));

    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_fe_gen2_pcs_rx_fclk_sel_set per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param pll  pll number
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Select FCLK  in rx path */
int tbhmod_fe_gen2_pcs_rx_fclk_sel_get(PHYMOD_ST* pc, int *pll)
{
    PMD_X4_FCLK_AFE_PLL_SELr_t reg_pll_sel;

    PHYMOD_IF_ERR_RETURN
        (READ_PMD_X4_FCLK_AFE_PLL_SELr(pc, &reg_pll_sel));
    *pll = PMD_X4_FCLK_AFE_PLL_SELr_FCLK_SEL_PLL_RXf_GET(reg_pll_sel);

    return PHYMOD_E_NONE;
}

/*!
 *  @brief tbhmod_pcs_ts_config per port.
 *  @param pc  phy access
 *  @param ts_offset timestamp offset from SOP
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_fe_gen2_pcs_ts_config(PHYMOD_ST* pc, int ts_offset)
{
    RX_X4_RX_TS_CTLr_t RX_X4_RX_TS_CTLr_reg;
    TX_X4_TX_TS_CTLr_t TX_X4_TX_TS_CTLr_reg;

    RX_X4_RX_TS_CTLr_CLR(RX_X4_RX_TS_CTLr_reg);
    TX_X4_TX_TS_CTLr_CLR(TX_X4_TX_TS_CTLr_reg);

    RX_X4_RX_TS_CTLr_RX_SOP_BYTE_OFFSETf_SET(RX_X4_RX_TS_CTLr_reg, ts_offset);
    TX_X4_TX_TS_CTLr_TX_SOP_BYTE_OFFSETf_SET(TX_X4_TX_TS_CTLr_reg, ts_offset);

    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_TS_CTLr(pc, RX_X4_RX_TS_CTLr_reg));
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X4_TX_TS_CTLr(pc, TX_X4_TX_TS_CTLr_reg));

    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_pcs_rx_ts_en(PHYMOD_ST* pc, uint32_t en)
{
    RX_X4_RX_TS_CTLr_t RX_X4_RX_TS_CTLr_reg;

    RX_X4_RX_TS_CTLr_CLR(RX_X4_RX_TS_CTLr_reg);

    RX_X4_RX_TS_CTLr_TS_UPDATE_ENABLEf_SET(RX_X4_RX_TS_CTLr_reg, en);
    PHYMOD_IF_ERR_RETURN(MODIFY_RX_X4_RX_TS_CTLr(pc, RX_X4_RX_TS_CTLr_reg));

    return PHYMOD_E_NONE;
}

#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
static
float _tbhmod_fe_gen2_pcs_vco_to_clk_period(uint32_t vco, int os_mode, int pam4)
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
        break;
      case TBHMOD_VCO_25P00G:
        vco_val = 25.00;
        break;
      default:
        return clk_period;
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
          default:
              break;
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
          default:
              break;
        }

    }
    return clk_period;
}
#endif

int _tbhmod_fe_gen2_pcs_vco_to_ui(uint32_t vco, int os_mode, int pam4)
{
    uint32_t ui_value;
    ui_value = 0;

    /* These are ideal values and when PMD is present these values are in sync with the VCos. */
    switch (vco) {
        case TBHMOD_VCO_20G:
            if (os_mode == TBHMOD_FE_GEN2_OS_MODE_2) {
                /* 2/20.625 = 0.09696.. make 32 bit 0.09696.. * 2^32 = 416481676 = 0x18d3018c */
                ui_value = TBHMOD_FE_GEN2_UI_20G_NRZ_OSX2;
            } else {
                /* 1/20.625 = 0.04848.. make 32 bit 0.04848.. * 2^32 = 208240838 = 0xc6980c6 */
                ui_value = TBHMOD_FE_GEN2_UI_20G_NRZ_OSX1;
            }
            break;
        case TBHMOD_VCO_25G:
            if (pam4) {
                /* 1/(2*25.78125) = 0.019393939.. make 32 bit 0.019393939.. * 2^32 = 83296335 = 0x4F7004F */
                ui_value = TBHMOD_FE_GEN2_UI_25G_PAM4;

            } else {
                if (os_mode == TBHMOD_FE_GEN2_OS_MODE_2p5) {   /* os_mode 2.5 */
                    /* 2.5/25.78125 = 0.09696.. make 32 bit 0.09696.. * 2^32 = 416481676 = 0x18d3018c */
                    ui_value = TBHMOD_FE_GEN2_UI_25G_NRZ_OSX2P5;
                } else {
                    /* 1/25.78125 = 0.038787879.. make 32 bit 0.038787879.. * 2^32 = 166592670 = 0x09ee009e */
                    ui_value = TBHMOD_FE_GEN2_UI_25G_NRZ_OSX1;
                }
            }
            break;
        case TBHMOD_VCO_26G:
            if (pam4) {
                /* 1/(2*26.56250) = 0.018823529.. make 32 bit 0.018823529.. * 2^32 = 80846443 = 0x4D19E6B */
                ui_value = TBHMOD_FE_GEN2_UI_26G_PAM4;
            } else {
                /* 1/26.56250 = 0.037647059.. make 32 bit 0.038787879.. * 2^32 = 161692886 = 0x09a33cd6 */
                ui_value = TBHMOD_FE_GEN2_UI_26G_NRZ;
            }
            break;
        case TBHMOD_VCO_25P00G:
            if (os_mode == 1) {
                /* 2/25.00 = 0.08. make 32 bit 0.08. * 2^32 = 343597383 = 0x147ae147 */
                ui_value = TBHMOD_FE_GEN2_UI_25P00G_NRZ_OSX2;
            } else {
                /* 1/25.00 = 0.04. make 32 bit 0.04. * 2^32 = 171798691 = 0xa3d70a3 */
                ui_value = TBHMOD_FE_GEN2_UI_25P00G_NRZ_OSX1;
            }
            break;
        default:
            return PHYMOD_E_PARAM;
    }

    return ui_value;

}


/*!
 *  @brief tbhmod_fe_gen2_pcs_set_1588_ui per port.
 *  @param unit number for instance lane number for decide which lane
 *  @param vco tells which vco is used in the lane
 *  @param os_mode tells which os_mode is used in the lane
 *  @param clk4sync_div the fast clk divider 0 => 6 and 1 => 8
 *  @returns PHYMOD_E_NONE if no errors. PHYMOD_EERROR else.
 *  @details Enables ts in rx path */
int tbhmod_fe_gen2_pcs_set_1588_ui (PHYMOD_ST* pc, uint32_t vco, uint32_t fclk_vco, int os_mode, int clk4sync_div, int pam4)
{
    PMD_X4_FCLK_PERIOD_PORTr_t PMD_X4_FCLK_PERIOD_PORTr_reg;
    PMD_X4_UI_VALUE_HIr_t PMD_X4_UI_VALUE_HIr_reg;
    PMD_X4_UI_VALUE_LOr_t PMD_X4_UI_VALUE_LOr_reg;
    int fclk_period = 0, ui_value_hi, ui_value_lo, ui_value_lo_m17_to_m23;

    ui_value_hi = _tbhmod_fe_gen2_pcs_vco_to_ui(vco, os_mode, pam4) >> 16;
    ui_value_lo = _tbhmod_fe_gen2_pcs_vco_to_ui(vco, os_mode, pam4) & 0xffff;

    /* ui_value_lo represent ui_value m17 to m32. For register programming,
     * we only need m17 to m23 value. So right shift 9 bits. */
    ui_value_lo_m17_to_m23 = ui_value_lo >> 9;

    switch(fclk_vco) {
      case TBHMOD_VCO_20G:
        /*
         * Hardcoded to DIV6, matching DV setting.
         */
        fclk_period = TBHMOD_FE_GEN2_FCLK_PERIOD_20G_DIV6;
        break;
      case TBHMOD_VCO_25G:
        fclk_period = TBHMOD_FE_GEN2_FCLK_PERIOD_25G;
        break;
      case TBHMOD_VCO_26G:
        fclk_period = TBHMOD_FE_GEN2_FCLK_PERIOD_26G;
        break;
      default:
          return PHYMOD_E_PARAM;
    }


    PMD_X4_FCLK_PERIOD_PORTr_CLR(PMD_X4_FCLK_PERIOD_PORTr_reg);
    PMD_X4_UI_VALUE_HIr_CLR(PMD_X4_UI_VALUE_HIr_reg);
    PMD_X4_UI_VALUE_LOr_CLR(PMD_X4_UI_VALUE_LOr_reg);

    /* PER LANE SETTING */
    PMD_X4_FCLK_PERIOD_PORTr_FCLK_FRAC_NS_PORTf_SET(PMD_X4_FCLK_PERIOD_PORTr_reg, fclk_period);
    PMD_X4_UI_VALUE_HIr_UI_FRAC_M1_TO_M16f_SET(PMD_X4_UI_VALUE_HIr_reg, ui_value_hi);
    PMD_X4_UI_VALUE_LOr_UI_FRAC_M17_TO_M23f_SET(PMD_X4_UI_VALUE_LOr_reg, ui_value_lo_m17_to_m23);

    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_FCLK_PERIOD_PORTr(pc, PMD_X4_FCLK_PERIOD_PORTr_reg));
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_UI_VALUE_HIr(pc, PMD_X4_UI_VALUE_HIr_reg));
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_UI_VALUE_LOr(pc, PMD_X4_UI_VALUE_LOr_reg));

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
#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
    uint32_t tx_latency;
    uint32_t ui_hi_lo;
    float bit_ui_val, bit_ui_val1, bit_ui_val2;

    PMD_X4_TX_FIXED_LATENCYr_t PMD_X4_TX_FIXED_LATENCYr_reg;
    RXCOM_OSR_MODE_STS_MC_MASKr_t RXCOM_OSR_MODE_STS_MC_MASKr_reg;

    if (an_en) {
      PHYMOD_IF_ERR_RETURN(READ_RXCOM_OSR_MODE_STS_MC_MASKr(pc, &RXCOM_OSR_MODE_STS_MC_MASKr_reg));
      os_mode = RXCOM_OSR_MODE_STS_MC_MASKr_RX_OSR_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASKr_reg);
      pam4    = RXCOM_OSR_MODE_STS_MC_MASKr_RX_PAM4_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASKr_reg);
    }

    /* ui_val with fractional bit 32 bit from _tbhmod_fe_gen2_pcs_vco_to_ui we need 10 fractional
     * so we need 22 bit shift.
     * First shifting 8 then 14 to improve accuracy.
     */
    ui_hi_lo = _tbhmod_fe_gen2_pcs_vco_to_ui(vco, os_mode, pam4);
    bit_ui_val1 = ((float) ui_hi_lo / 1024);
    bit_ui_val2 = ((float) bit_ui_val1 / 1024);
    bit_ui_val = ((float) bit_ui_val2 / 4096);

    /***
     * Please refer to the Blackhawk7 user spec datapath latency table
     * For PAM4 the pmd latency is based on the middle bit ie 20th bit of the 40bit interface.
     * Hence we subtract 20. Also for Pam4 each UI == 2bits hence we multiple by 2
    *****/
    if (pam4) {
        tx_latency = ((uint32_t)((bit_ui_val * ((TBHMOD_FE_GEN2_PMD_TX_DP_LATENCY_PAM4 * 2) - 20)) * 1024));
    } else {
        tx_latency = ((uint32_t)((bit_ui_val * (TBHMOD_FE_GEN2_PMD_TX_DP_LATENCY_NRZ - 20)) * 1024));
    }

      PMD_X4_TX_FIXED_LATENCYr_CLR(PMD_X4_TX_FIXED_LATENCYr_reg);


      PMD_X4_TX_FIXED_LATENCYr_TX_PMD_LATENCY_IN_NSf_SET (PMD_X4_TX_FIXED_LATENCYr_reg, (tx_latency >> 10));
      PMD_X4_TX_FIXED_LATENCYr_TX_PMD_LATENCY_IN_FRAC_NSf_SET(PMD_X4_TX_FIXED_LATENCYr_reg, (tx_latency & 0x3ff));

      PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_TX_FIXED_LATENCYr(pc, PMD_X4_TX_FIXED_LATENCYr_reg));
#endif
    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_update_tx_pmd_latency (PHYMOD_ST* pc, uint32_t latency_adj, int normalize_to_latest)
{
    uint32_t tx_latency, latency_ns, latency_frac_ns;

    PMD_X4_TX_FIXED_LATENCYr_t PMD_X4_TX_FIXED_LATENCYr_reg;

    PMD_X4_TX_FIXED_LATENCYr_CLR(PMD_X4_TX_FIXED_LATENCYr_reg);

    PHYMOD_IF_ERR_RETURN(READ_PMD_X4_TX_FIXED_LATENCYr(pc, &PMD_X4_TX_FIXED_LATENCYr_reg));
    latency_ns =  PMD_X4_TX_FIXED_LATENCYr_TX_PMD_LATENCY_IN_NSf_GET(PMD_X4_TX_FIXED_LATENCYr_reg);
    latency_frac_ns = PMD_X4_TX_FIXED_LATENCYr_TX_PMD_LATENCY_IN_FRAC_NSf_GET(PMD_X4_TX_FIXED_LATENCYr_reg);

    if  (normalize_to_latest == 1) {
            tx_latency = ((latency_ns << 10) | latency_frac_ns) + latency_adj;
    } else {
        if  ((latency_ns >> 5) & 1)   {
            tx_latency = ((latency_ns << 10) | latency_frac_ns) + latency_adj;
        } else {
            tx_latency = ((latency_ns << 10) | latency_frac_ns) - latency_adj;
        }
    }

    PMD_X4_TX_FIXED_LATENCYr_TX_PMD_LATENCY_IN_NSf_SET (PMD_X4_TX_FIXED_LATENCYr_reg, (tx_latency >> 10));
    PMD_X4_TX_FIXED_LATENCYr_TX_PMD_LATENCY_IN_FRAC_NSf_SET(PMD_X4_TX_FIXED_LATENCYr_reg, (tx_latency & 0x3ff));
    PHYMOD_IF_ERR_RETURN(MODIFY_PMD_X4_TX_FIXED_LATENCYr(pc, PMD_X4_TX_FIXED_LATENCYr_reg));

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
    TX_X1_GLAS_TPMA_CTLr_t   TX_X1_GLAS_TPMA_CTLr_reg;
    PHYMOD_ST phy_copy;

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));
    TX_X1_GLAS_TPMA_CTLr_CLR(TX_X1_GLAS_TPMA_CTLr_reg);
    TX_X1_GLAS_TPMA_CTLr_GLAS_TPMA_CAPTURE_ENf_SET(TX_X1_GLAS_TPMA_CTLr_reg, tx_skew_en);
    TX_X1_GLAS_TPMA_CTLr_GLAS_TPMA_CAPTURE_MASKf_SET(TX_X1_GLAS_TPMA_CTLr_reg, pc->lane_mask);
    TX_X1_GLAS_TPMA_CTLr_GLAS_TPMA_CAPTURE_LOGICALf_SET(TX_X1_GLAS_TPMA_CTLr_reg, 1);

    phy_copy.lane_mask = 0x1;
    PHYMOD_IF_ERR_RETURN(MODIFY_TX_X1_GLAS_TPMA_CTLr(&phy_copy, TX_X1_GLAS_TPMA_CTLr_reg));

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
#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
    uint32_t latency_adj;
    uint32_t ui_hi_lo;
    float bit_ui_val, bit_ui_val1, bit_ui_val2;
    uint32_t fclk_frac_ns;
    uint32_t tx_lane_skew_bits[TSCBH_FE_GEN2_NOF_LANES_IN_CORE];
    uint32_t tx_lane_skew_bits_ro[TSCBH_FE_GEN2_NOF_LANES_IN_CORE];
    uint16_t glas_tpma_capture_data[TSCBH_FE_GEN2_NOF_LANES_IN_CORE];
    uint8_t glas_tpma_capture_adj[TSCBH_FE_GEN2_NOF_LANES_IN_CORE];
    uint32_t normalized_tx_lane_skew_bits[TSCBH_FE_GEN2_NOF_LANES_IN_CORE];
    uint32_t max_lane_skew_bits, min_lane_skew_bits;
    uint32_t max_lane_skew_bits_ro, min_lane_skew_bits_ro;
    float fclk_period, fclk_period_tmp, no_of_bits;
    int curr_lane;
    int i, start_lane, num_lane;
    int lane;
    PHYMOD_ST phy_copy;

    RXCOM_OSR_MODE_STS_MC_MASKr_t RXCOM_OSR_MODE_STS_MC_MASKr_reg;

    TX_X1_GLAS_TPMA_DATA0r_t TX_X1_GLAS_TPMA_DATA0r_reg;
    TX_X1_GLAS_TPMA_DATA1r_t TX_X1_GLAS_TPMA_DATA1r_reg;
    TX_X1_GLAS_TPMA_DATA2r_t TX_X1_GLAS_TPMA_DATA2r_reg;
    TX_X1_GLAS_TPMA_DATA3r_t TX_X1_GLAS_TPMA_DATA3r_reg;
    TX_X1_GLAS_TPMA_DATA4r_t TX_X1_GLAS_TPMA_DATA4r_reg;
    TX_X1_GLAS_TPMA_DATA5r_t TX_X1_GLAS_TPMA_DATA5r_reg;
    TX_X1_GLAS_TPMA_DATA6r_t TX_X1_GLAS_TPMA_DATA6r_reg;
    TX_X1_GLAS_TPMA_DATA7r_t TX_X1_GLAS_TPMA_DATA7r_reg;

    TX_X1_GLAS_TPMA_ADJ_0_1r_t TX_X1_GLAS_TPMA_ADJ_0_1r_reg;
    TX_X1_GLAS_TPMA_ADJ_2_3r_t TX_X1_GLAS_TPMA_ADJ_2_3r_reg;
    TX_X1_GLAS_TPMA_ADJ_4_5r_t TX_X1_GLAS_TPMA_ADJ_4_5r_reg;
    TX_X1_GLAS_TPMA_ADJ_6_7r_t TX_X1_GLAS_TPMA_ADJ_6_7r_reg;

    PMD_X4_FCLK_PERIOD_PORTr_t PMD_X4_FCLK_PERIOD_PORTr_reg;

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    for (lane = 0; lane < TSCBH_FE_GEN2_NOF_LANES_IN_CORE; lane++) {
        tx_lane_skew_bits[lane] = 0;
        normalized_tx_lane_skew_bits[lane] = 0;
        glas_tpma_capture_data[lane] = 0;
        glas_tpma_capture_adj[lane] = 0;
    }

    max_lane_skew_bits = 0x0;
    min_lane_skew_bits = 0x2fff;
    max_lane_skew_bits_ro = 0x0;
    min_lane_skew_bits_ro = 0x2fff;
    no_of_bits = 0;


    if (an_en) {
        PHYMOD_IF_ERR_RETURN(READ_RXCOM_OSR_MODE_STS_MC_MASKr(pc, &RXCOM_OSR_MODE_STS_MC_MASKr_reg));
        os_mode = RXCOM_OSR_MODE_STS_MC_MASKr_RX_OSR_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASKr_reg);
        pam4    = RXCOM_OSR_MODE_STS_MC_MASKr_RX_PAM4_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASKr_reg);
    }

    /* ui_val with fractional bit 32 bit from _tbhmod_fe_gen2_pcs_vco_to_ui we need 10 fractional
     * so we need 22 bit shift.
     * First shifting 8 then 14 to improve accuracy.
     */
    ui_hi_lo = _tbhmod_fe_gen2_pcs_vco_to_ui(vco, os_mode, pam4);
    bit_ui_val1 = ((float) ui_hi_lo / 1024);
    bit_ui_val2 = ((float) bit_ui_val1 / 1024);
    bit_ui_val = ((float) bit_ui_val2 / 4096);


    /* ADJUST Lane Mask as it is X4 now. */
    if (start_lane < 4) {
        phy_copy.lane_mask = 0x1;
    } else {
        phy_copy.lane_mask = 0x10;
    }
    PHYMOD_IF_ERR_RETURN(READ_PMD_X4_FCLK_PERIOD_PORTr(&phy_copy, &PMD_X4_FCLK_PERIOD_PORTr_reg));
    fclk_frac_ns = PMD_X4_FCLK_PERIOD_PORTr_FCLK_FRAC_NS_PORTf_GET(PMD_X4_FCLK_PERIOD_PORTr_reg);
    fclk_period_tmp =  ((float) fclk_frac_ns / 1024);
    fclk_period = ((float) fclk_period_tmp / 64);

    TX_X1_GLAS_TPMA_DATA0r_CLR(TX_X1_GLAS_TPMA_DATA0r_reg);
    TX_X1_GLAS_TPMA_DATA1r_CLR(TX_X1_GLAS_TPMA_DATA1r_reg);
    TX_X1_GLAS_TPMA_DATA2r_CLR(TX_X1_GLAS_TPMA_DATA2r_reg);
    TX_X1_GLAS_TPMA_DATA3r_CLR(TX_X1_GLAS_TPMA_DATA3r_reg);
    TX_X1_GLAS_TPMA_DATA4r_CLR(TX_X1_GLAS_TPMA_DATA4r_reg);
    TX_X1_GLAS_TPMA_DATA5r_CLR(TX_X1_GLAS_TPMA_DATA5r_reg);
    TX_X1_GLAS_TPMA_DATA6r_CLR(TX_X1_GLAS_TPMA_DATA6r_reg);
    TX_X1_GLAS_TPMA_DATA7r_CLR(TX_X1_GLAS_TPMA_DATA7r_reg);
    TX_X1_GLAS_TPMA_ADJ_0_1r_CLR(TX_X1_GLAS_TPMA_ADJ_0_1r_reg);
    TX_X1_GLAS_TPMA_ADJ_2_3r_CLR(TX_X1_GLAS_TPMA_ADJ_2_3r_reg);
    TX_X1_GLAS_TPMA_ADJ_4_5r_CLR(TX_X1_GLAS_TPMA_ADJ_4_5r_reg);
    TX_X1_GLAS_TPMA_ADJ_6_7r_CLR(TX_X1_GLAS_TPMA_ADJ_6_7r_reg);

    phy_copy.lane_mask = 0x1;
    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA0r(&phy_copy, &TX_X1_GLAS_TPMA_DATA0r_reg));
    glas_tpma_capture_data[0] = TX_X1_GLAS_TPMA_DATA0r_GLAS_TPMA_CAPTURE_DATA_0f_GET(TX_X1_GLAS_TPMA_DATA0r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA1r(&phy_copy, &TX_X1_GLAS_TPMA_DATA1r_reg));
    glas_tpma_capture_data[1] = TX_X1_GLAS_TPMA_DATA1r_GLAS_TPMA_CAPTURE_DATA_1f_GET(TX_X1_GLAS_TPMA_DATA1r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA2r(&phy_copy, &TX_X1_GLAS_TPMA_DATA2r_reg));
    glas_tpma_capture_data[2] = TX_X1_GLAS_TPMA_DATA2r_GLAS_TPMA_CAPTURE_DATA_2f_GET(TX_X1_GLAS_TPMA_DATA2r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA3r(&phy_copy, &TX_X1_GLAS_TPMA_DATA3r_reg));
    glas_tpma_capture_data[3] = TX_X1_GLAS_TPMA_DATA3r_GLAS_TPMA_CAPTURE_DATA_3f_GET(TX_X1_GLAS_TPMA_DATA3r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA4r(&phy_copy, &TX_X1_GLAS_TPMA_DATA4r_reg));
    glas_tpma_capture_data[4] = TX_X1_GLAS_TPMA_DATA4r_GLAS_TPMA_CAPTURE_DATA_4f_GET(TX_X1_GLAS_TPMA_DATA4r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA5r(&phy_copy, &TX_X1_GLAS_TPMA_DATA5r_reg));
    glas_tpma_capture_data[5] = TX_X1_GLAS_TPMA_DATA5r_GLAS_TPMA_CAPTURE_DATA_5f_GET(TX_X1_GLAS_TPMA_DATA5r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA6r(&phy_copy, &TX_X1_GLAS_TPMA_DATA6r_reg));
    glas_tpma_capture_data[6] = TX_X1_GLAS_TPMA_DATA6r_GLAS_TPMA_CAPTURE_DATA_6f_GET(TX_X1_GLAS_TPMA_DATA6r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_DATA7r(&phy_copy, &TX_X1_GLAS_TPMA_DATA7r_reg));
    glas_tpma_capture_data[7] = TX_X1_GLAS_TPMA_DATA7r_GLAS_TPMA_CAPTURE_DATA_7f_GET(TX_X1_GLAS_TPMA_DATA7r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_ADJ_0_1r(&phy_copy, &TX_X1_GLAS_TPMA_ADJ_0_1r_reg));
    glas_tpma_capture_adj[0] = TX_X1_GLAS_TPMA_ADJ_0_1r_GLAS_TPMA_CAPTURE_ADJ_0f_GET(TX_X1_GLAS_TPMA_ADJ_0_1r_reg);
    glas_tpma_capture_adj[1] = TX_X1_GLAS_TPMA_ADJ_0_1r_GLAS_TPMA_CAPTURE_ADJ_1f_GET(TX_X1_GLAS_TPMA_ADJ_0_1r_reg);
    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_ADJ_2_3r(&phy_copy, &TX_X1_GLAS_TPMA_ADJ_2_3r_reg));
    glas_tpma_capture_adj[2] = TX_X1_GLAS_TPMA_ADJ_2_3r_GLAS_TPMA_CAPTURE_ADJ_2f_GET(TX_X1_GLAS_TPMA_ADJ_2_3r_reg);
    glas_tpma_capture_adj[3] = TX_X1_GLAS_TPMA_ADJ_2_3r_GLAS_TPMA_CAPTURE_ADJ_3f_GET(TX_X1_GLAS_TPMA_ADJ_2_3r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_ADJ_4_5r(&phy_copy, &TX_X1_GLAS_TPMA_ADJ_4_5r_reg));
    glas_tpma_capture_adj[4] = TX_X1_GLAS_TPMA_ADJ_4_5r_GLAS_TPMA_CAPTURE_ADJ_4f_GET(TX_X1_GLAS_TPMA_ADJ_4_5r_reg);
    glas_tpma_capture_adj[5] = TX_X1_GLAS_TPMA_ADJ_4_5r_GLAS_TPMA_CAPTURE_ADJ_5f_GET(TX_X1_GLAS_TPMA_ADJ_4_5r_reg);

    PHYMOD_IF_ERR_RETURN(READ_TX_X1_GLAS_TPMA_ADJ_6_7r(&phy_copy, &TX_X1_GLAS_TPMA_ADJ_6_7r_reg));
    glas_tpma_capture_adj[6] = TX_X1_GLAS_TPMA_ADJ_6_7r_GLAS_TPMA_CAPTURE_ADJ_6f_GET(TX_X1_GLAS_TPMA_ADJ_6_7r_reg);
    glas_tpma_capture_adj[7] = TX_X1_GLAS_TPMA_ADJ_6_7r_GLAS_TPMA_CAPTURE_ADJ_7f_GET(TX_X1_GLAS_TPMA_ADJ_6_7r_reg);


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
            (tbhmod_fe_gen2_update_tx_pmd_latency(&phy_copy, latency_adj, normalize_to_latest));
    }
#endif
    return PHYMOD_E_NONE;
}
static
int _tbhmod_fe_gen2_virtual_lane_count_get(int bit_mux_mode, int num_lane, int* virtual_lanes, int* num_psll_per_phyl)
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

int tbhmod_fe_gen2_pcs_mod_rx_1588_tbl_val(PHYMOD_ST* pc, int an_en, int bit_mux_mode, uint32_t vco, int os_mode, int pam4, int tbl_ln, int normalize_to_latest, int rx_skew_adjust_enb, int *table , int *rx_max_skew) {
#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
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

    PHYMOD_ST phy_copy;

    PMD_X4_FCLK_PERIOD_PORTr_t PMD_X4_FCLK_PERIOD_PORTr_reg;
    RXCOM_OSR_MODE_STS_MC_MASKr_t RXCOM_OSR_MODE_STS_MC_MASKr_reg;


    RX_X4_PSLL_TO_VL_MAP0r_t RX_X4_PSLL_TO_VL_MAP0r_reg;
    RX_X4_PSLL_TO_VL_MAP1r_t RX_X4_PSLL_TO_VL_MAP1r_reg;

    RX_X4_SKEW_OFFSS0r_t RX_X4_SKEW_OFFSS0r_reg;
    RX_X4_SKEW_OFFSS1r_t RX_X4_SKEW_OFFSS1r_reg;
    RX_X4_SKEW_OFFSS2r_t RX_X4_SKEW_OFFSS2r_reg;
    RX_X4_SKEW_OFFSS3r_t RX_X4_SKEW_OFFSS3r_reg;
    RX_X4_SKEW_OFFSS4r_t RX_X4_SKEW_OFFSS4r_reg;

    RX_X4_AM_TS_INFO0r_t RX_X4_AM_TS_INFO0r_reg;
    RX_X4_AM_TS_INFO1r_t RX_X4_AM_TS_INFO1r_reg;
    RX_X4_AM_TS_INFO2r_t RX_X4_AM_TS_INFO2r_reg;
    RX_X4_AM_TS_INFO3r_t RX_X4_AM_TS_INFO3r_reg;
    RX_X4_AM_TS_INFO4r_t RX_X4_AM_TS_INFO4r_reg;

    PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
      (_tbhmod_fe_gen2_virtual_lane_count_get(bit_mux_mode, num_lane, &virtual_lanes, &num_psll_per_phyl));
    physical_lanes = num_lane;

    if (an_en) {
        PHYMOD_IF_ERR_RETURN(READ_RXCOM_OSR_MODE_STS_MC_MASKr(pc, &RXCOM_OSR_MODE_STS_MC_MASKr_reg));
        os_mode = RXCOM_OSR_MODE_STS_MC_MASKr_RX_OSR_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASKr_reg);
        pam4    = RXCOM_OSR_MODE_STS_MC_MASKr_RX_PAM4_MODEf_GET(RXCOM_OSR_MODE_STS_MC_MASKr_reg);
    }

    ui_value = _tbhmod_fe_gen2_pcs_vco_to_ui(vco, os_mode, pam4);

    /* ui_val with fractional bit 32 bit from _tbhmod_gen2_pcs_vco_to_ui we need 10 fractional
     * so we need 22 bit shift.
     * First shifting 8 then 14 to improve accuracy.
     */
    bit_ui_val1 = ((float) ui_value / 1024);
    bit_ui_val2 = ((float) bit_ui_val1 / 1024);
    bit_ui_val = ((float) bit_ui_val2 / 4096);

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

    /* ADJUST per lane FCLK */
    if (start_lane < 4) {
        phy_copy.lane_mask = 0x1;
    } else {
        phy_copy.lane_mask = 0x10;
    }
    PHYMOD_IF_ERR_RETURN(READ_PMD_X4_FCLK_PERIOD_PORTr(&phy_copy, &PMD_X4_FCLK_PERIOD_PORTr_reg));
    fclk_period = PMD_X4_FCLK_PERIOD_PORTr_FCLK_FRAC_NS_PORTf_GET(PMD_X4_FCLK_PERIOD_PORTr_reg);

    if (rx_skew_adjust_enb == 1) {
        if (virtual_lanes > 1) {
            for (i = 0; i < physical_lanes; i++) {
                  phy_copy.lane_mask = 0x1 << (start_lane + i);
                  PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP0r(&phy_copy, &RX_X4_PSLL_TO_VL_MAP0r_reg));
                  PHYMOD_IF_ERR_RETURN(READ_RX_X4_PSLL_TO_VL_MAP1r(&phy_copy, &RX_X4_PSLL_TO_VL_MAP1r_reg));

                  if (num_psll_per_phyl > 0) {
                      curr_vl  = RX_X4_PSLL_TO_VL_MAP0r_PSLL0_TO_VL_MAPPINGf_GET(RX_X4_PSLL_TO_VL_MAP0r_reg);
                      psll_to_vl_map[curr_vl] = curr_psll;
                      vl_to_psll_map[curr_vl] = start_lane + i;
                      vl_to_psll_off_map[curr_vl] = 0;
                      curr_psll++;
                  }
                  if (num_psll_per_phyl > 1) {
                      curr_vl = RX_X4_PSLL_TO_VL_MAP0r_PSLL1_TO_VL_MAPPINGf_GET(RX_X4_PSLL_TO_VL_MAP0r_reg);
                      psll_to_vl_map[curr_vl] = curr_psll;
                      vl_to_psll_map[curr_vl] = start_lane + i;
                      vl_to_psll_off_map[curr_vl] = 1;
                      curr_psll++;
                  }
                  if (num_psll_per_phyl > 2) {
                      curr_vl = RX_X4_PSLL_TO_VL_MAP0r_PSLL2_TO_VL_MAPPINGf_GET(RX_X4_PSLL_TO_VL_MAP0r_reg);
                      psll_to_vl_map[curr_vl] = curr_psll;
                      vl_to_psll_map[curr_vl] = start_lane + i;
                      vl_to_psll_off_map[curr_vl] = 2;
                      curr_psll++;
                  }
                  if (num_psll_per_phyl > 3) {
                      curr_vl = RX_X4_PSLL_TO_VL_MAP1r_PSLL3_TO_VL_MAPPINGf_GET(RX_X4_PSLL_TO_VL_MAP1r_reg);
                      psll_to_vl_map[curr_vl] = curr_psll;
                      vl_to_psll_map[curr_vl] = start_lane + i;
                      vl_to_psll_off_map[curr_vl] = 3;
                      curr_psll++;
                  }
                  if (num_psll_per_phyl > 4) {
                      curr_vl = RX_X4_PSLL_TO_VL_MAP1r_PSLL4_TO_VL_MAPPINGf_GET(RX_X4_PSLL_TO_VL_MAP1r_reg);
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
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS0r(&phy_copy, &RX_X4_SKEW_OFFSS0r_reg));
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_AM_TS_INFO0r(&phy_copy, &RX_X4_AM_TS_INFO0r_reg));
                    base_ts_ns = RX_X4_AM_TS_INFO0r_BASE_TS_NS_0f_GET(RX_X4_AM_TS_INFO0r_reg);
                    base_ts_subns = RX_X4_AM_TS_INFO0r_BASE_TS_SUB_NS_0f_GET(RX_X4_AM_TS_INFO0r_reg);
                    am_slip_cnt = RX_X4_SKEW_OFFSS0r_AM_SLIP_COUNT_0f_GET(RX_X4_SKEW_OFFSS0r_reg);
                    am_ts_fclk_adj_val = RX_X4_SKEW_OFFSS0r_AM_TS_FCLK_ADJUST_VALUE_0f_GET(RX_X4_SKEW_OFFSS0r_reg);
                    deskew_ts_info_val = RX_X4_SKEW_OFFSS0r_DESKEW_TS_INFO_VALID_0f_GET(RX_X4_SKEW_OFFSS0r_reg);
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
                     PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS1r(&phy_copy, &RX_X4_SKEW_OFFSS1r_reg));
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_AM_TS_INFO1r(&phy_copy, &RX_X4_AM_TS_INFO1r_reg));
                    base_ts_ns = RX_X4_AM_TS_INFO1r_BASE_TS_NS_1f_GET(RX_X4_AM_TS_INFO1r_reg);
                    base_ts_subns = RX_X4_AM_TS_INFO1r_BASE_TS_SUB_NS_1f_GET(RX_X4_AM_TS_INFO1r_reg);
                    am_slip_cnt = RX_X4_SKEW_OFFSS1r_AM_SLIP_COUNT_1f_GET(RX_X4_SKEW_OFFSS1r_reg);
                    am_ts_fclk_adj_val = RX_X4_SKEW_OFFSS1r_AM_TS_FCLK_ADJUST_VALUE_1f_GET(RX_X4_SKEW_OFFSS1r_reg);
                    deskew_ts_info_val = RX_X4_SKEW_OFFSS1r_DESKEW_TS_INFO_VALID_1f_GET(RX_X4_SKEW_OFFSS1r_reg);
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
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS2r(&phy_copy, &RX_X4_SKEW_OFFSS2r_reg));
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_AM_TS_INFO2r(&phy_copy, &RX_X4_AM_TS_INFO2r_reg));
                    base_ts_ns = RX_X4_AM_TS_INFO2r_BASE_TS_NS_2f_GET(RX_X4_AM_TS_INFO2r_reg);
                    base_ts_subns = RX_X4_AM_TS_INFO2r_BASE_TS_SUB_NS_2f_GET(RX_X4_AM_TS_INFO2r_reg);
                    am_slip_cnt = RX_X4_SKEW_OFFSS2r_AM_SLIP_COUNT_2f_GET(RX_X4_SKEW_OFFSS2r_reg);
                    am_ts_fclk_adj_val = RX_X4_SKEW_OFFSS2r_AM_TS_FCLK_ADJUST_VALUE_2f_GET(RX_X4_SKEW_OFFSS2r_reg);
                    deskew_ts_info_val = RX_X4_SKEW_OFFSS2r_DESKEW_TS_INFO_VALID_2f_GET(RX_X4_SKEW_OFFSS2r_reg);
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
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS3r(&phy_copy, &RX_X4_SKEW_OFFSS3r_reg));
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_AM_TS_INFO3r(&phy_copy, &RX_X4_AM_TS_INFO3r_reg));
                    base_ts_ns = RX_X4_AM_TS_INFO3r_BASE_TS_NS_3f_GET(RX_X4_AM_TS_INFO3r_reg);
                    base_ts_subns = RX_X4_AM_TS_INFO3r_BASE_TS_SUB_NS_3f_GET(RX_X4_AM_TS_INFO3r_reg);
                    am_slip_cnt = RX_X4_SKEW_OFFSS3r_AM_SLIP_COUNT_3f_GET(RX_X4_SKEW_OFFSS3r_reg);
                    am_ts_fclk_adj_val = RX_X4_SKEW_OFFSS3r_AM_TS_FCLK_ADJUST_VALUE_3f_GET(RX_X4_SKEW_OFFSS3r_reg);
                    deskew_ts_info_val = RX_X4_SKEW_OFFSS3r_DESKEW_TS_INFO_VALID_3f_GET(RX_X4_SKEW_OFFSS3r_reg);
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
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS4r(&phy_copy, &RX_X4_SKEW_OFFSS4r_reg));
                    PHYMOD_IF_ERR_RETURN(READ_RX_X4_AM_TS_INFO4r(&phy_copy, &RX_X4_AM_TS_INFO4r_reg));
                    base_ts_ns = RX_X4_AM_TS_INFO4r_BASE_TS_NS_4f_GET(RX_X4_AM_TS_INFO4r_reg);
                    base_ts_subns = RX_X4_AM_TS_INFO4r_BASE_TS_SUB_NS_4f_GET(RX_X4_AM_TS_INFO4r_reg);
                    am_slip_cnt = RX_X4_SKEW_OFFSS4r_AM_SLIP_COUNT_4f_GET(RX_X4_SKEW_OFFSS4r_reg);
                    am_ts_fclk_adj_val = RX_X4_SKEW_OFFSS4r_AM_TS_FCLK_ADJUST_VALUE_4f_GET(RX_X4_SKEW_OFFSS4r_reg);
                    deskew_ts_info_val = RX_X4_SKEW_OFFSS4r_DESKEW_TS_INFO_VALID_4f_GET(RX_X4_SKEW_OFFSS4r_reg);
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
            table_time = table_time +  (vlane_val_correction & 0xffff);
        } else {
            vlane_val_correction = 0x8000 - vlane_val_correction;
            table_time = table_time +  (vlane_val_correction & 0x7fff);

        }

        table[i] = (table[i] & 0xf8000) | (table_time & 0x7fff);
    }
#endif

    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_pcs_ts_deskew_valid(PHYMOD_ST* pc, int bit_mux_mode, int *rx_ts_deskew_valid) {
    int i, num_lane, start_lane;
    int virtual_lanes, physical_lanes, num_psll_per_phyl;
    int ts_deskew_valid = 1;
    PHYMOD_ST phy_copy;

    RX_X4_SKEW_OFFSS0r_t RX_X4_SKEW_OFFSS0r_reg;
    RX_X4_SKEW_OFFSS1r_t RX_X4_SKEW_OFFSS1r_reg;
    RX_X4_SKEW_OFFSS2r_t RX_X4_SKEW_OFFSS2r_reg;
    RX_X4_SKEW_OFFSS3r_t RX_X4_SKEW_OFFSS3r_reg;
    RX_X4_SKEW_OFFSS4r_t RX_X4_SKEW_OFFSS4r_reg;

    PHYMOD_IF_ERR_RETURN
      (phymod_util_lane_config_get(pc, &start_lane, &num_lane));

    PHYMOD_MEMCPY(&phy_copy, pc, sizeof(phy_copy));

    PHYMOD_IF_ERR_RETURN
        (_tbhmod_fe_gen2_virtual_lane_count_get(bit_mux_mode, num_lane, &virtual_lanes, &num_psll_per_phyl));
    physical_lanes = num_lane;

    if (virtual_lanes == 1) {
        *rx_ts_deskew_valid = 1;
        return PHYMOD_E_NONE;
    }

    for (i = 0; i < virtual_lanes; i++) {
        phy_copy.lane_mask = 0x1 << (start_lane + i % num_lane);
        if ((i / physical_lanes) == 0) {
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS0r(&phy_copy, &RX_X4_SKEW_OFFSS0r_reg));
            ts_deskew_valid &= RX_X4_SKEW_OFFSS0r_DESKEW_TS_INFO_VALID_0f_GET(RX_X4_SKEW_OFFSS0r_reg);
        } else if ((i / physical_lanes) == 1) {
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS1r(&phy_copy, &RX_X4_SKEW_OFFSS1r_reg));
            ts_deskew_valid &= RX_X4_SKEW_OFFSS1r_DESKEW_TS_INFO_VALID_1f_GET(RX_X4_SKEW_OFFSS1r_reg);
        } else if ((i / physical_lanes) == 2) {
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS2r(&phy_copy, &RX_X4_SKEW_OFFSS2r_reg));
            ts_deskew_valid &= RX_X4_SKEW_OFFSS2r_DESKEW_TS_INFO_VALID_2f_GET(RX_X4_SKEW_OFFSS2r_reg);
        } else if ((i / physical_lanes) == 3) {
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS3r(&phy_copy, &RX_X4_SKEW_OFFSS3r_reg));
            ts_deskew_valid &= RX_X4_SKEW_OFFSS3r_DESKEW_TS_INFO_VALID_3f_GET(RX_X4_SKEW_OFFSS3r_reg);
        } else if ((i / physical_lanes) == 4) {
            PHYMOD_IF_ERR_RETURN(READ_RX_X4_SKEW_OFFSS4r(&phy_copy, &RX_X4_SKEW_OFFSS4r_reg));
            ts_deskew_valid &= RX_X4_SKEW_OFFSS4r_DESKEW_TS_INFO_VALID_4f_GET(RX_X4_SKEW_OFFSS4r_reg);
        }
    }
    *rx_ts_deskew_valid = ts_deskew_valid;

    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_ts_offset_rx_set(PHYMOD_ST* pc, uint32_t vco, int os_mode, int pam4, int tbl_ln, uint32_t *table)
{
    PMD_X1_PM_TMR_OFFSr_t PMD_X1_PM_TMR_OFFSr_reg;
    uint16_t pm_offset_ns, pm_offset_sub_ns;
    int32_t pm_offset, vl_time, vl_update_time;
    int i;
#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
    float rx_clk_period;
#endif

    PMD_X1_PM_TMR_OFFSr_CLR(PMD_X1_PM_TMR_OFFSr_reg);
    PHYMOD_IF_ERR_RETURN(READ_PMD_X1_PM_TMR_OFFSr(pc, &PMD_X1_PM_TMR_OFFSr_reg));
    pm_offset_ns = PMD_X1_PM_TMR_OFFSr_PM_OFFSET_IN_NSf_GET(PMD_X1_PM_TMR_OFFSr_reg);
    pm_offset_sub_ns = PMD_X1_PM_TMR_OFFSr_PM_OFFSET_SUB_NSf_GET(PMD_X1_PM_TMR_OFFSr_reg);
    /* pm_offset:
     * bit 0-7: sub ns.
     * bit 8-30: ns.
     * bit 31: sign (always 0).
     */
    pm_offset = ((pm_offset_ns & 0xff) << 8) | (pm_offset_sub_ns & 0xff);

#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
    rx_clk_period = _tbhmod_fe_gen2_pcs_vco_to_clk_period(vco, os_mode, pam4);
#endif
    /* Each entry in the table is in below format:
     * bit 0-3: sub ns.
     * bit 4-14: ns.
     * bit 15-19: virtual lane number.
     */
    for (i = 0; i < tbl_ln; i++) {
        /* Get the time from table and extend the sub_ns field to 8 bit. */
        vl_time = ((table[i] << 4) & 0x7fff0);
        if ((vl_time >> 18) & 1) {
            /* Sign bit extension. */
            vl_time = (vl_time | 0xfff80000);
        }
#ifdef PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
        vl_update_time = vl_time + pm_offset + ((uint32_t) ((float)((rx_clk_period * 4) * 256)));
#else
        vl_update_time = vl_time + pm_offset;
#endif
        /* We do not expect overflow here so omitting the overflow check. */
        table[i] = (table[i] & 0xf8000) | ((vl_update_time >> 4) & 0x7fff);
    }

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

int tbhmod_fe_gen2_txclk32_en(const PHYMOD_ST* pc, uint32_t en)
{
    AMS_TX_TX_CTL1r_t reg_ams_tx_ctrl;

    AMS_TX_TX_CTL1r_CLR(reg_ams_tx_ctrl);
    AMS_TX_TX_CTL1r_AMS_TX_TXCLK32_ENf_SET(reg_ams_tx_ctrl, en);

    PHYMOD_IF_ERR_RETURN
       (MODIFY_AMS_TX_TX_CTL1r(pc, reg_ams_tx_ctrl));

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

int tbhmod_fe_gen2_trans_am_set(PHYMOD_ST* pc, uint32_t speed, uint32_t trans_am_en)
{
    uint32_t trans_am = 0;
    SC_X4_CTLr_t SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg;


    SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);

    PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));
    if (trans_am_en) {
        switch (speed) {
          case 25000:
              trans_am = TBHMOD_FE_GEN2_TRANSPARENT_AM_25G;
              break;
          case 50000:
              trans_am = TBHMOD_FE_GEN2_TRANSPARENT_AM_50G;
              break;
          case 100000:
              trans_am = TBHMOD_FE_GEN2_TRANSPARENT_AM_100G;
              break;
          case 200000:
              trans_am = TBHMOD_FE_GEN2_TRANSPARENT_AM_200G;
              break;
          case 400000:
              trans_am = TBHMOD_FE_GEN2_TRANSPARENT_AM_400G;
              break;
          default:
              return PHYMOD_E_PARAM;
        }
    } else {
        trans_am = 0;
    }

    SC_X4_CTLr_SC_TRANSPARENT_AM_CTRLf_SET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg, trans_am);
    PHYMOD_IF_ERR_RETURN(MODIFY_SC_X4_CTLr(pc, SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));

    return PHYMOD_E_NONE;
}

int tbhmod_fe_gen2_trans_am_get(PHYMOD_ST* pc, uint32_t speed, uint32_t* trans_am_en)
{
  uint32_t trans_am = 0;
    SC_X4_CTLr_t SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg;

    SC_X4_CTLr_CLR(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);

    PHYMOD_IF_ERR_RETURN(READ_SC_X4_CTLr(pc, &SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg));
    trans_am = SC_X4_CTLr_SC_TRANSPARENT_AM_CTRLf_GET(SC_X4_CONTROL_SC_X4_CONTROL_CONTROLr_reg);
    *trans_am_en = trans_am ? 1 : 0;

    return PHYMOD_E_NONE;
}
