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
 * File       : blackhawk_l8p2_cfg_seq.h
 * Description: c functions implementing Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/


#ifndef BLACKHAWK7_L8P2_CFG_SEQ_H
#define BLACKHAWK7_L8P2_CFG_SEQ_H

#include "common/srds_api_err_code.h"
#include <phymod/phymod_diagnostics.h>

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
} blackhawk7_l8p2_rev_id0_t;

typedef struct {
  int8_t revid_eee;
  int8_t revid_llp;
  int8_t revid_pir;
  int8_t revid_cl72;
  int8_t revid_micro;
  int8_t revid_mdio;
  int8_t revid_multiplicity;
} blackhawk7_l8p2_rev_id1_t;

typedef enum {
  TX = 0,
  Rx
} tx_rx_t;

typedef enum {
    BLACKHAWK_PRBS_POLYNOMIAL_7 = 0,
    BLACKHAWK_PRBS_POLYNOMIAL_9,
    BLACKHAWK_PRBS_POLYNOMIAL_11,
    BLACKHAWK_PRBS_POLYNOMIAL_15,
    BLACKHAWK_PRBS_POLYNOMIAL_23,
    BLACKHAWK_PRBS_POLYNOMIAL_31,
    BLACKHAWK_PRBS_POLYNOMIAL_58,
    BLACKHAWK_PRBS_POLYNOMIAL_TYPE_COUNT
}blackhawk7_l8p2_prbs_polynomial_type_t;

#define PATTERN_MAX_SIZE 8


extern err_code_t _blackhawk7_l8p2_pmd_mwr_reg_byte(  phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint8_t val);
extern uint8_t _blackhawk7_l8p2_pmd_rde_field_byte(  phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);
extern uint16_t _blackhawk7_l8p2_pmd_rde_field(  phymod_access_t *pa, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);
err_code_t blackhawk7_l8p2_tx_pi_control_get( phymod_access_t *pa,  int16_t *value);
err_code_t blackhawk7_l8p2_tx_rx_polarity_set( phymod_access_t *pa, uint32_t tx_pol, uint32_t rx_pol);
err_code_t blackhawk7_l8p2_tx_rx_polarity_get( phymod_access_t *pa, uint32_t *tx_pol, uint32_t *rx_pol);
err_code_t blackhawk7_l8p2_lane_pll_selection_set( phymod_access_t *pa, uint32_t pll_index);
err_code_t blackhawk7_l8p2_lane_pll_selection_get( phymod_access_t *sa__, uint32_t *pll_index);
err_code_t blackhawk7_l8p2_uc_active_set( phymod_access_t *pa, uint32_t enable);
err_code_t blackhawk7_l8p2_uc_active_get( phymod_access_t *pa, uint32_t *enable);
/* int blackhawk_uc_reset( phymod_access_t *pa, uint32_t enable); */
err_code_t blackhawk7_l8p2_prbs_tx_inv_data_get( phymod_access_t *pa, uint32_t *inv_data);
err_code_t blackhawk7_l8p2_prbs_rx_inv_data_get( phymod_access_t *pa, uint32_t *inv_data);
err_code_t blackhawk7_l8p2_prbs_tx_poly_get( phymod_access_t *pa, blackhawk7_l8p2_prbs_polynomial_type_t *prbs_poly);
err_code_t blackhawk7_l8p2_prbs_rx_poly_get( phymod_access_t *pa, blackhawk7_l8p2_prbs_polynomial_type_t *prbs_poly);
err_code_t blackhawk7_l8p2_prbs_tx_enable_get( phymod_access_t *pa, uint32_t *enable);
err_code_t blackhawk7_l8p2_prbs_rx_enable_get( phymod_access_t *pa, uint32_t *enable);
err_code_t blackhawk7_l8p2_pmd_force_signal_detect( phymod_access_t *pa, uint8_t force_en, uint8_t force_val);
err_code_t blackhawk7_l8p2_pll_mode_set( phymod_access_t *pa, int pll_mode);
err_code_t blackhawk7_l8p2_pll_mode_get( phymod_access_t *pa, uint32_t *pll_mode);
err_code_t blackhawk7_l8p2_osr_mode_set( phymod_access_t *pa, int osr_mode);
err_code_t blackhawk7_l8p2_osr_mode_get( phymod_access_t *pa, int *osr_mode);
err_code_t blackhawk7_l8p2_dig_lpbk_get( phymod_access_t *pa, uint32_t *lpbk);
err_code_t blackhawk7_l8p2_rmt_lpbk_get( phymod_access_t *pa, uint32_t *lpbk);
err_code_t blackhawk7_l8p2_core_soft_reset( phymod_access_t *pa);
err_code_t blackhawk7_l8p2_core_soft_reset_release( phymod_access_t *pa, uint32_t enable);
err_code_t blackhawk7_l8p2_core_soft_reset_read( phymod_access_t *pa, uint32_t *enable);
err_code_t blackhawk7_l8p2_lane_soft_reset_read( phymod_access_t *pa, uint32_t *enable);
err_code_t blackhawk7_l8p2_pmd_tx_disable_pin_dis_set( phymod_access_t *pa, uint32_t enable);
err_code_t blackhawk7_l8p2_pmd_tx_disable_pin_dis_get( phymod_access_t *pa, uint32_t *enable);
err_code_t blackhawk7_l8p2_pwrdn_set( phymod_access_t *pa, int tx_rx, int pwrdn);
err_code_t blackhawk7_l8p2_pwrdn_get( phymod_access_t *pa, power_status_t *pwrdn);
err_code_t blackhawk7_l8p2_pcs_lane_swap_tx( phymod_access_t *pa, uint32_t tx_lane_map);
err_code_t blackhawk7_l8p2_pmd_lane_swap ( phymod_access_t *pa, uint32_t lane_map);
err_code_t blackhawk7_l8p2_pmd_lane_map_get( phymod_access_t *pa, uint32_t *tx_lane_map, uint32_t *rx_lane_map);
err_code_t blackhawk7_l8p2_pmd_loopback_get( phymod_access_t *pa, uint32_t *enable);
err_code_t blackhawk7_l8p2_identify( phymod_access_t *pa, blackhawk7_l8p2_rev_id0_t *rev_id0, blackhawk7_l8p2_rev_id1_t *rev_id1);
err_code_t blackhawk7_l8p2_pmd_ln_h_rstb_pkill_override(  phymod_access_t *pa, uint16_t val);
err_code_t blackhawk7_l8p2_lane_soft_reset( phymod_access_t *pa, uint32_t enable);   /* pmd core soft reset */
err_code_t blackhawk7_l8p2_lane_soft_reset_get( phymod_access_t *pa, uint32_t *enable);
err_code_t blackhawk7_l8p2_lane_hard_soft_reset_release( phymod_access_t *pa, uint32_t enable);
err_code_t blackhawk7_l8p2_clause72_control( phymod_access_t *pc, uint32_t cl_72_en);        /* CLAUSE_72_CONTROL */
err_code_t blackhawk7_l8p2_clause72_control_get( phymod_access_t *pc, uint32_t *cl_72_en);   /* CLAUSE_72_CONTROL */
err_code_t blackhawk7_l8p2_pmd_cl72_enable_get( phymod_access_t *pa, uint32_t *enable);
err_code_t blackhawk7_l8p2_pmd_cl72_receiver_status( phymod_access_t *pa, uint32_t *status);
err_code_t blackhawk7_l8p2_ucode_init(  phymod_access_t *pa );
err_code_t blackhawk7_l8p2_pram_firmware_enable( phymod_access_t *pa, int enable, int wait);
err_code_t blackhawk7_l8p2_pmd_reset_seq( phymod_access_t *pa, int pmd_touched);
err_code_t blackhawk7_l8p2_pll_reset_enable_set( phymod_access_t *pa, int enable);
err_code_t blackhawk7_l8p2_read_pll_range( phymod_access_t *pa, uint32_t *pll_range);
err_code_t blackhawk7_l8p2_signal_detect ( phymod_access_t *pa, uint32_t *signal_detect);
err_code_t blackhawk7_l8p2_force_tx_set_rst ( phymod_access_t *pa, uint32_t rst);
err_code_t blackhawk7_l8p2_force_tx_get_rst ( phymod_access_t *pa, uint32_t *rst);
err_code_t blackhawk7_l8p2_force_rx_set_rst ( phymod_access_t *pa, uint32_t rst);
err_code_t blackhawk7_l8p2_force_rx_get_rst ( phymod_access_t *pa, uint32_t *rst);
err_code_t blackhawk7_l8p2_ladder_setting_to_mV( phymod_access_t *pa, int8_t y, int16_t* level);
err_code_t blackhawk7_l8p2_electrical_idle_set( phymod_access_t *pa, uint8_t en);
err_code_t blackhawk7_l8p2_electrical_idle_get( phymod_access_t *pa, uint8_t *en);
err_code_t blackhawk7_l8p2_get_vco ( phymod_phy_inf_config_t* config, uint32_t *vco_rate, uint32_t *new_pll_div, int16_t *new_os_mode);
err_code_t blackhawk7_l8p2_tx_shared_patt_gen_en_get(  phymod_access_t *pa, uint8_t *enable);
err_code_t blackhawk7_l8p2_config_shared_tx_pattern_idx_set(  phymod_access_t *pa,  uint32_t *pattern_len);
err_code_t blackhawk7_l8p2_config_shared_tx_pattern_idx_get(  phymod_access_t *pa, uint32_t *pattern_len, uint32_t *pattern);
err_code_t blackhawk7_l8p2_tx_disable_get ( phymod_access_t *pa, uint8_t *enable);
err_code_t blackhawk7_l8p2_refclk_set( phymod_access_t *pa, phymod_ref_clk_t ref_clock);
err_code_t blackhawk7_l8p2_pmd_force_signal_detect_get( phymod_access_t *sa__, uint8_t *force_en, uint8_t *force_val);
err_code_t blackhawk7_l8p2_channel_loss_set( phymod_access_t *sa__, uint32_t loss_in_db);
err_code_t blackhawk7_l8p2_channel_loss_get( phymod_access_t *sa__, uint32_t *loss_in_db);
err_code_t blackhawk7_l8p2_tx_tap_mode_get( phymod_access_t *sa__, uint8_t *mode);
err_code_t blackhawk7_l8p2_pam4_tx_pattern_enable_get( phymod_access_t *sa__, phymod_PAM4_tx_pattern_t pattern_type, uint32_t* enable);
err_code_t blackhawk7_l8p2_signalling_mode_status_get( phymod_access_t *sa__, phymod_phy_signalling_method_t *mode);
err_code_t blackhawk7_l8p2_tx_nrz_mode_get(phymod_access_t *sa__, uint16_t *tx_nrz_mode);
err_code_t blackhawk7_l8p2_tx_pam4_precoder_enable_set(phymod_access_t *sa__, int enable);
err_code_t blackhawk7_l8p2_tx_pam4_precoder_enable_get(phymod_access_t *sa__, int *enable);
err_code_t blackhawk7_l8p2_micro_clk_source_select( phymod_access_t *sa__, uint32_t pll_index);
err_code_t blackhawk7_l8p2_speed_config_get(uint32_t speed, int ref_clk_is_156p25, uint32_t *pll_multiplier, uint32_t *is_pam4, uint32_t *osr_mode);

/* Get the PLL powerdown status */
err_code_t blackhawk7_l8p2_pll_pwrdn_get(phymod_access_t *sa__, uint32_t *is_pwrdn);
err_code_t blackhawk7_l8p2_enable_pass_through_configuration(phymod_access_t *sa__, int enable);
/* Locks TX_PI to Loop timing, external CDR */
err_code_t blackhawk7_l8p2_ext_loop_timing(phymod_access_t *sa__, uint8_t enable);
err_code_t blackhawk7_l8p2_error_analyzer_status_clear(phymod_access_t *sa__);
err_code_t blackhawk7_l8p2_rx_ppm(phymod_access_t *sa__, int16_t *rx_ppm);
/* Set/Get clk4sync_en, clk4sync_div */
err_code_t blackhawk7_l8p2_clk4sync_enable_set(phymod_access_t *sa__, uint32_t en, uint32_t div);
err_code_t blackhawk7_l8p2_clk4sync_enable_get(phymod_access_t *sa__, uint32_t *en, uint32_t *div);
err_code_t blackhawk7_l8p2_pll_lock_get(phymod_access_t *sa__, uint32_t *pll_lock);
err_code_t blackhawk7_l8p2_afe_version_get(phymod_access_t *sa__, uint8_t *ver_id);
#endif /* PHY_TSC_IBLK_H */
