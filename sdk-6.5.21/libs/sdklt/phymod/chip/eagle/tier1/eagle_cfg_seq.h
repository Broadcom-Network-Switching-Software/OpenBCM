/*----------------------------------------------------------------------
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : eagle_cfg_seq.h
 * Description: c functions implementing Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/
/*
 *  $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * 
*/


#ifndef EAGLE_CFG_SEQ_H 
#define EAGLE_CFG_SEQ_H

#include "common/srds_api_err_code.h"
#include "eagle_tsc_enum.h"

typedef struct {
  int8_t pll_pwrdn;
  int8_t tx_s_pwrdn;
  int8_t rx_s_pwrdn;
} power_status_t;

typedef struct {
  int8_t revid_model;
  int8_t revid_process;
  int8_t revid_bonding;
  int8_t revid_rev_number;
  int8_t revid_rev_letter;
} eagle_rev_id0_t;


typedef enum {
  TX = 0,
  Rx
} tx_rx_t;

typedef struct {
  int8_t revid_eee;
  int8_t revid_llp;
  int8_t revid_pir;
  int8_t revid_cl72;
  int8_t revid_micro;
  int8_t revid_mdio;
  int8_t revid_multiplicity;
} eagle_rev_id1_t;


typedef enum {
    EAGLE_PRBS_POLYNOMIAL_7 = 0,
    EAGLE_PRBS_POLYNOMIAL_9,
    EAGLE_PRBS_POLYNOMIAL_11,
    EAGLE_PRBS_POLYNOMIAL_15,
    EAGLE_PRBS_POLYNOMIAL_23,
    EAGLE_PRBS_POLYNOMIAL_31,
    EAGLE_PRBS_POLYNOMIAL_58,
    EAGLE_PRBS_POLYNOMIAL_TYPE_COUNT 
}eagle_prbs_polynomial_type_t;

#define PATTERN_MAX_SIZE 8

err_code_t eagle_tsc_identify(const phymod_access_t *pa, eagle_rev_id0_t *rev_id0, eagle_rev_id1_t *rev_id1);

err_code_t eagle_uc_active_set(const phymod_access_t *pa, uint32_t enable);           /* set microcontroller active or not  */
err_code_t eagle_uc_active_get(const phymod_access_t *pa, uint32_t *enable);          /* get microcontroller active or not  */
err_code_t eagle_uc_reset(const phymod_access_t *pa, uint32_t enable);           /* set dw8501 reset  */
err_code_t eagle_prbs_tx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data);
err_code_t eagle_prbs_rx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data);
err_code_t eagle_prbs_tx_poly_get(const phymod_access_t *pa, eagle_prbs_polynomial_type_t *prbs_poly);
err_code_t eagle_prbs_rx_poly_get(const phymod_access_t *pa, eagle_prbs_polynomial_type_t *prbs_poly);
err_code_t eagle_prbs_tx_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_prbs_rx_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_pll_mode_set(const phymod_access_t *pa, int pll_mode);   /* PLL divider set */
err_code_t eagle_core_soft_reset_release(const phymod_access_t *pa, uint32_t enable);   /* release the pmd core soft reset */
err_code_t eagle_lane_soft_reset_release(const phymod_access_t *pa, uint32_t enable);   /* release the pmd core soft reset */
err_code_t eagle_pram_firmware_enable(const phymod_access_t *pa, int enable);   /* release the pmd core soft reset */
err_code_t eagle_pmd_force_signal_detect(const phymod_access_t *pa, int enable); /*force the signal detect */   
err_code_t eagle_pmd_loopback_get(const phymod_access_t *pa, uint32_t *enable);   
err_code_t eagle_uc_init(const phymod_access_t *pa);
err_code_t eagle_pram_flop_set(const phymod_access_t *pa, int value);
err_code_t eagle_tsc_ucode_init( const phymod_access_t *pa );
err_code_t eagle_pmd_ln_h_rstb_pkill_override( const phymod_access_t *pa, uint16_t val); 
err_code_t eagle_pmd_cl72_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_pmd_cl72_receiver_status(const phymod_access_t *pa, uint32_t *status);
err_code_t eagle_tx_pi_control_get( const phymod_access_t *pa,  int16_t *value);
   

err_code_t eagle_tx_rx_polarity_set(const phymod_access_t *pa, uint32_t tx_pol, uint32_t rx_pol);
err_code_t eagle_tx_rx_polarity_get(const phymod_access_t *pa, uint32_t *tx_pol, uint32_t *rx_pol);
err_code_t eagle_force_tx_set_rst (const phymod_access_t *pa, uint32_t rst);
err_code_t eagle_force_tx_get_rst (const phymod_access_t *pa, uint32_t *rst);
err_code_t eagle_force_rx_set_rst (const phymod_access_t *pa, uint32_t rst);
err_code_t eagle_force_rx_get_rst (const phymod_access_t *pa, uint32_t *rst);
err_code_t eagle_pll_mode_get(const phymod_access_t *pa, uint32_t *pll_mode);
err_code_t eagle_osr_mode_set(const phymod_access_t *pa, int osr_mode);
err_code_t eagle_osr_mode_get(const phymod_access_t *pa, int *osr_mode);
err_code_t eagle_tsc_dig_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk);
err_code_t eagle_tsc_rmt_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk);
err_code_t eagle_core_soft_reset(const phymod_access_t *pa);
err_code_t eagle_tsc_pwrdn_set(const phymod_access_t *pa, int tx_rx, int pwrdn);
err_code_t eagle_tsc_pwrdn_get(const phymod_access_t *pa, power_status_t *pwrdn);
err_code_t eagle_pcs_lane_swap_tx(const phymod_access_t *pa, uint32_t tx_lane_map); 
err_code_t eagle_pmd_lane_swap (const phymod_access_t *pa, uint32_t lane_map); 
err_code_t eagle_pmd_lane_swap_tx_get (const phymod_access_t *pa, uint32_t *lane_map); 
err_code_t eagle_lane_hard_soft_reset_release(const phymod_access_t *pa, uint32_t enable); 
err_code_t eagle_clause72_control(const phymod_access_t *pa, uint32_t cl_72_en);          
err_code_t eagle_clause72_control_get(const phymod_access_t *pa, uint32_t *cl_72_en);    
err_code_t eagle_pmd_reset_seq(const phymod_access_t *pa, int pmd_touched);
err_code_t eagle_pll_reset_enable_set (const phymod_access_t *pa, int enable);
err_code_t eagle_tsc_read_pll_range (const phymod_access_t *pa, uint32_t *pll_range);
err_code_t eagle_tsc_signal_detect (const phymod_access_t *pa, uint32_t *signal_detect);
err_code_t eagle_tsc_ladder_setting_to_mV(const phymod_access_t *pa, int8_t y, int16_t* level);
err_code_t eagle_pll_config_get(const phymod_access_t *pa, enum eagle_tsc_pll_enum *pll_mode);
err_code_t eagle_tx_lane_control_set(const phymod_access_t *pa, uint32_t en);
err_code_t eagle_tx_lane_control_get(const phymod_access_t *pa, uint32_t* en);
err_code_t eagle_rx_lane_control_set(const phymod_access_t *pa, uint32_t en);
err_code_t eagle_rx_lane_control_get(const phymod_access_t *pa, uint32_t* en);
err_code_t eagle_tsc_get_vco (const phymod_phy_inf_config_t* config, uint32_t *vco_rate, enum eagle_tsc_pll_enum *new_pll_div, int16_t *new_os_mode);
err_code_t eagle_pmd_tx_disable_pin_dis_set(const phymod_access_t *pa, uint32_t enable);
err_code_t eagle_pmd_tx_disable_pin_dis_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t eagle_tsc_tx_shared_patt_gen_en_get( const phymod_access_t *pa, uint8_t *enable);
err_code_t eagle_tsc_config_shared_tx_pattern_idx_set( const phymod_access_t *pa, const uint32_t *pattern_len);
err_code_t eagle_tsc_config_shared_tx_pattern_idx_get( const phymod_access_t *pa, uint32_t *pattern_len, uint32_t *pattern);
err_code_t eagle_tsc_rx_ppm(const phymod_access_t *pa, int16_t *rx_ppm);
#endif /* PHY_TSC_IBLK_H */
