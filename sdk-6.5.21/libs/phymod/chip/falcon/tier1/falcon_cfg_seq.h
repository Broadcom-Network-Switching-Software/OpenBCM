/*----------------------------------------------------------------------
 *  
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : falcon_cfg_seq.h
 * Description: c functions implementing Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/


#ifndef FALCON_CFG_SEQ_H 
#define FALCON_CFG_SEQ_H

#include "common/srds_api_err_code.h"
#include "falcon_tsc_enum.h"

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
} falcon_rev_id0_t;

typedef struct {
  int8_t revid_eee;
  int8_t revid_llp; 
  int8_t revid_pir; 
  int8_t revid_cl72; 
  int8_t revid_micro; 
  int8_t revid_mdio; 
  int8_t revid_multiplicity;
} falcon_rev_id1_t;

typedef enum {
  TX = 0,
  Rx
} tx_rx_t;

typedef enum {
    FALCON_PRBS_POLYNOMIAL_7 = 0,
    FALCON_PRBS_POLYNOMIAL_9,
    FALCON_PRBS_POLYNOMIAL_11,
    FALCON_PRBS_POLYNOMIAL_15,
    FALCON_PRBS_POLYNOMIAL_23,
    FALCON_PRBS_POLYNOMIAL_31,
    FALCON_PRBS_POLYNOMIAL_58,
    FALCON_PRBS_POLYNOMIAL_TYPE_COUNT 
}falcon_prbs_polynomial_type_t;

#define PATTERN_MAX_SIZE 8

extern err_code_t _falcon_pmd_mwr_reg_byte( const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint8_t val);
extern uint8_t _falcon_pmd_rde_field_byte( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);
extern uint16_t _falcon_pmd_rde_field( const phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);
err_code_t falcon_tx_pi_control_get(const phymod_access_t *pa,  int16_t *value);

err_code_t falcon_tx_rx_polarity_set(const phymod_access_t *pa, uint32_t tx_pol, uint32_t rx_pol);
err_code_t falcon_tx_rx_polarity_get(const phymod_access_t *pa, uint32_t *tx_pol, uint32_t *rx_pol);
err_code_t falcon_uc_active_set(const phymod_access_t *pa, uint32_t enable);
err_code_t falcon_uc_active_get(const phymod_access_t *pa, uint32_t *enable);
/* int falcon_uc_reset(const phymod_access_t *pa, uint32_t enable); */
err_code_t falcon_prbs_tx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data);
err_code_t falcon_prbs_rx_inv_data_get(const phymod_access_t *pa, uint32_t *inv_data);
err_code_t falcon_prbs_tx_poly_get(const phymod_access_t *pa, falcon_prbs_polynomial_type_t *prbs_poly);
err_code_t falcon_prbs_rx_poly_get(const phymod_access_t *pa, falcon_prbs_polynomial_type_t *prbs_poly);
err_code_t falcon_prbs_tx_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t falcon_prbs_rx_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t falcon_pmd_force_signal_detect(const phymod_access_t *pa, uint32_t enable);
err_code_t falcon_pll_mode_set(const phymod_access_t *pa, int pll_mode);
err_code_t falcon_pll_mode_get(const phymod_access_t *pa, uint32_t *pll_mode);
err_code_t falcon_afe_pll_reg_set(const phymod_access_t *pa, const phymod_afe_pll_t *afe_pll);
err_code_t falcon_afe_pll_reg_get(const phymod_access_t *pa, phymod_afe_pll_t *afe_pll);
err_code_t falcon_osr_mode_set(const phymod_access_t *pa, int osr_mode);
err_code_t falcon_osr_mode_get(const phymod_access_t *pa, int *osr_mode);
err_code_t falcon_tsc_dig_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk);
err_code_t falcon_tsc_rmt_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk);
err_code_t falcon_core_soft_reset(const phymod_access_t *pa);
err_code_t falcon_core_soft_reset_release(const phymod_access_t *pa, uint32_t enable);
err_code_t falcon_core_soft_reset_read(const phymod_access_t *pa, uint32_t *enable);
err_code_t falcon_lane_soft_reset_read(const phymod_access_t *pa, uint32_t *enable);
err_code_t falcon_pmd_tx_disable_pin_dis_set(const phymod_access_t *pa, uint32_t enable);
err_code_t falcon_pmd_tx_disable_pin_dis_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t falcon_tsc_pwrdn_set(const phymod_access_t *pa, int tx_rx, int pwrdn);
err_code_t falcon_tsc_pwrdn_get(const phymod_access_t *pa, power_status_t *pwrdn);
err_code_t falcon_pcs_lane_swap_tx(const phymod_access_t *pa, uint32_t tx_lane_map);
err_code_t falcon_pmd_lane_swap (const phymod_access_t *pa, uint32_t lane_map);
err_code_t falcon_pmd_lane_swap_tx_get(const phymod_access_t *pa, uint32_t *lane_map);
err_code_t falcon_pmd_loopback_get(const phymod_access_t *pa, uint32_t *enable);   
err_code_t falcon_tsc_identify(const phymod_access_t *pa, falcon_rev_id0_t *rev_id0, falcon_rev_id1_t *rev_id1);
err_code_t falcon_pmd_ln_h_rstb_pkill_override( const phymod_access_t *pa, uint16_t val); 
err_code_t falcon_lane_soft_reset_release(const phymod_access_t *pa, uint32_t enable);   /* pmd core soft reset */
err_code_t falcon_lane_soft_reset_release_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t falcon_rx_lane_soft_reset_release(const phymod_access_t *pa, uint32_t enable);
err_code_t falcon_rx_lane_soft_reset_release_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t falcon_tx_lane_soft_reset_release(const phymod_access_t *pa, uint32_t enable);
err_code_t falcon_tx_lane_soft_reset_release_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t falcon_lane_hard_soft_reset_release(const phymod_access_t *pa, uint32_t enable);
err_code_t falcon_clause72_control(const phymod_access_t *pc, uint32_t cl_72_en);        /* CLAUSE_72_CONTROL */
err_code_t falcon_clause72_control_get(const phymod_access_t *pc, uint32_t *cl_72_en);   /* CLAUSE_72_CONTROL */
err_code_t falcon_pmd_cl72_enable_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t falcon_pmd_cl72_receiver_status(const phymod_access_t *pa, uint32_t *status); 
err_code_t falcon_tsc_ucode_init( const phymod_access_t *pa );
err_code_t falcon_pram_firmware_enable(const phymod_access_t *pa, int enable, int wait);  
err_code_t falcon_pmd_reset_seq(const phymod_access_t *pa, int pmd_touched);
err_code_t falcon_pll_reset_enable_set(const phymod_access_t *pa, int enable);
err_code_t falcon_tsc_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range);
err_code_t falcon_tsc_signal_detect (const phymod_access_t *pa, uint32_t *signal_detect);
err_code_t falcon_rx_squelch_set(const phymod_access_t *pa, uint32_t enable);
err_code_t falcon_rx_squelch_get(const phymod_access_t *pa, uint32_t *val);
err_code_t falcon_force_tx_set_rst (const phymod_access_t *pa, uint32_t rst);
err_code_t falcon_force_tx_get_rst (const phymod_access_t *pa, uint32_t *rst);
err_code_t falcon_force_rx_set_rst (const phymod_access_t *pa, uint32_t rst);
err_code_t falcon_force_rx_get_rst (const phymod_access_t *pa, uint32_t *rst);
err_code_t falcon_tsc_ladder_setting_to_mV(const phymod_access_t *pa, int8_t y, int16_t* level);
err_code_t falcon_electrical_idle_set(const phymod_access_t *pa, uint32_t en);
err_code_t falcon_tsc_get_vco (const phymod_phy_inf_config_t* config, uint32_t *vco_rate, uint32_t *new_pll_div, int16_t *new_os_mode);
err_code_t falcon_tsc_tx_shared_patt_gen_en_get( const phymod_access_t *pa, uint8_t *enable);
err_code_t falcon_tsc_config_shared_tx_pattern_idx_set( const phymod_access_t *pa, const uint32_t *pattern_len);
err_code_t falcon_tsc_config_shared_tx_pattern_idx_get( const phymod_access_t *pa, uint32_t *pattern_len, uint32_t *pattern);
err_code_t falcon_tsc_tx_disable_get (const phymod_access_t *pa, uint8_t *enable);
err_code_t falcon_refclk_set(const phymod_access_t *pa, phymod_ref_clk_t ref_clock);
err_code_t falcon_tsc_rx_ppm(const phymod_access_t *pa, int16_t *rx_ppm);
err_code_t falcon_tsc_rx_pmd_restart(const phymod_access_t *pa);
err_code_t falcon_tsc_cl93n72_frc_byte_set(const phymod_access_t *pa, uint32_t data);
err_code_t falcon_tsc_cl93n72_frc_byte_get(const phymod_access_t *pa, uint32_t *data);
#endif /* PHY_TSC_IBLK_H */
