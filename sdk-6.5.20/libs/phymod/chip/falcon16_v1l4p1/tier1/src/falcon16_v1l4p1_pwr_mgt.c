/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/**************************************************************************************
 **************************************************************************************
 *  File Name     :  falcon16_v1l4p1_pwr_mgt.c                                             *
 *  Created On    :  04 Nov 2015                                                      *
 *  Created By    :  Brent Roberts                                                    *
 *  Description   :  APIs for Serdes IPs                                              *
 *  Revision      :      *
 */

/** @file falcon16_v1l4p1_pwr_mgt.c
 * Implementation of API power management functions
 */

#include "../include/falcon16_v1l4p1_pwr_mgt.h"
#include "../include/falcon16_v1l4p1_common.h"
#include "../include/falcon16_v1l4p1_functions.h"
#include "../include/falcon16_v1l4p1_internal.h"
#include "../include/falcon16_v1l4p1_internal_error.h"
#include "../include/falcon16_v1l4p1_select_defns.h"
#include "../include/falcon16_v1l4p1_config.h"



/***************************/
/*  Configure Serdes IDDQ  */
/***************************/

err_code_t falcon16_v1l4p1_core_config_for_iddq(srds_access_t *sa__) {
  UNUSED(sa__);

  return (ERR_CODE_NONE);
}


err_code_t falcon16_v1l4p1_lane_config_for_iddq(srds_access_t *sa__) {

  /* Use frc/frc_val to force all RX and TX clk_vld signals to 0 */
  EFUN(wr_pmd_rx_clk_vld_frc_val(0x0));
  EFUN(wr_pmd_rx_clk_vld_frc(0x1));
      EFUN(wr_pmd_tx_clk_vld_frc_val(0x0));
      EFUN(wr_pmd_tx_clk_vld_frc(0x1));

  /* Use frc/frc_val to force all pmd_rx_lock signals to 0 */
  EFUN(wr_rx_dsc_lock_frc_val(0x0));
  EFUN(wr_rx_dsc_lock_frc(0x1));

  /* Switch all the lane clocks to comclk by writing to RX/TX comclk_sel registers */
  EFUN(wr_ln_rx_s_comclk_sel(0x1));
    EFUN(wr_ln_tx_s_comclk_sel(0x1));

  /* Assert all the AFE pwrdn/reset pins using frc/frc_val to make sure AFE is in lowest possible power mode */
  EFUN(wr_afe_tx_pwrdn_frc_val(0x1));
  EFUN(wr_afe_tx_pwrdn_frc(0x1));
  EFUN(wr_afe_rx_pwrdn_frc_val(0x1));
  EFUN(wr_afe_rx_pwrdn_frc(0x1));
  EFUN(wr_afe_tx_reset_frc_val(0x1));
  EFUN(wr_afe_tx_reset_frc(0x1));
  EFUN(wr_afe_rx_reset_frc_val(0x1));
  EFUN(wr_afe_rx_reset_frc(0x1));

  /* Set pmd_iddq pin to enable IDDQ */
  return (ERR_CODE_NONE);
}

/****************************************************/
/*  Serdes Powerdown, ClockGate and Deep_Powerdown  */
/****************************************************/

err_code_t falcon16_v1l4p1_core_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode) {
    switch(mode) {
    case PWR_ON:
        EFUN(falcon16_v1l4p1_INTERNAL_core_clkgate(sa__, 0));
        EFUN(wrc_ams_pll_pwrdn(0x0));
        EFUN(wrc_afe_s_pll_pwrdn(0x0));
        EFUN(falcon16_v1l4p1_core_dp_reset(sa__, 0x0));
        break;
    case PWRDN:
        EFUN(falcon16_v1l4p1_core_dp_reset(sa__, 1));
        EFUN(USR_DELAY_NS(500)); /* wait >50 comclk cycles  */
        EFUN(wrc_afe_s_pll_pwrdn(0x1));
        EFUN(USR_DELAY_NS(500)); /* wait >50 comclk cycles  */
        EFUN(wrc_ams_pll_pwrdn(0x1));
        break;
    case PWRDN_DEEP:
        EFUN(falcon16_v1l4p1_core_dp_reset(sa__, 1));
        EFUN(USR_DELAY_NS(500)); /* wait >50 comclk cycles  */
                EFUN(wrc_afe_s_pll_pwrdn(0x1));
                EFUN(USR_DELAY_NS(500)); /* wait >50 comclk cycles  */
                EFUN(wrc_ams_pll_pwrdn(0x1));
        EFUN(falcon16_v1l4p1_INTERNAL_core_clkgate(sa__, 1));
        break;
    default:
        EFUN(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;
    }
    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_lane_pwrdn(srds_access_t *sa__, enum srds_core_pwrdn_mode_enum mode) {

    switch(mode) {
    case PWR_ON:
        EFUN(wr_ln_tx_s_pwrdn(0x0));
        EFUN(wr_ln_rx_s_pwrdn(0x0));
        EFUN(falcon16_v1l4p1_INTERNAL_lane_clkgate(sa__, 0));
        break;
    case PWRDN:
        /* do the RX first, since that is what is most users care about */
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        break;
    case PWRDN_DEEP:
        /* do the RX first, since that is what is most users care about */
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        EFUN(falcon16_v1l4p1_INTERNAL_lane_clkgate(sa__, 1));
        EFUN(wr_ln_dp_s_rstb(0x0));
        break;
    case PWRDN_TX:
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        break;
    case PWRDN_RX:
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        break;
    default :
        return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
  return (ERR_CODE_NONE);
}
