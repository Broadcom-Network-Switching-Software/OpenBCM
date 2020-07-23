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
 * File       : falcon16_cfg_seq.h
 * Description: c functions implementing Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/


#ifndef FALCON16_CFG_SEQ_H 
#define FALCON16_CFG_SEQ_H

#include "../include/common/srds_api_err_code.h"
#include "../include/falcon16_tsc_enum.h"
#include "../include/falcon16_tsc_field_access.h"
#include "../include/falcon16_tsc_functions.h"

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
} falcon16_rev_id0_t;

typedef struct {
  int8_t revid_eee;
  int8_t revid_llp; 
  int8_t revid_pir; 
  int8_t revid_cl72; 
  int8_t revid_micro; 
  int8_t revid_mdio; 
  int8_t revid_multiplicity;
} falcon16_rev_id1_t;

typedef enum {
  TX = 0,
  Rx
} tx_rx_t;

typedef enum {
    FALCON16_PRBS_POLYNOMIAL_7 = 0,
    FALCON16_PRBS_POLYNOMIAL_9,
    FALCON16_PRBS_POLYNOMIAL_11,
    FALCON16_PRBS_POLYNOMIAL_15,
    FALCON16_PRBS_POLYNOMIAL_23,
    FALCON16_PRBS_POLYNOMIAL_31,
    FALCON16_PRBS_POLYNOMIAL_58,
    FALCON16_PRBS_POLYNOMIAL_TYPE_COUNT 
}falcon16_prbs_polynomial_type_t;

#define PATTERN_MAX_SIZE 8

extern err_code_t _serdes_pmd_mwr_reg_byte( srds_access_t *sa__, uint16_t addr, uint16_t mask, uint8_t lsb, uint8_t val);
extern uint8_t _serdes_pmd_rde_field_byte( srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);
extern uint16_t _serdes_pmd_rde_field( srds_access_t *sa__, uint16_t addr, uint8_t shift_left, uint8_t shift_right, err_code_t *err_code_p);

err_code_t falcon16_tx_rx_polarity_set(srds_access_t *sa__, uint32_t tx_pol, uint32_t rx_pol);
err_code_t falcon16_tx_rx_polarity_get(srds_access_t *sa__, uint32_t *tx_pol, uint32_t *rx_pol);
err_code_t falcon16_uc_active_set(srds_access_t *sa__, uint32_t enable);
err_code_t falcon16_uc_active_get(srds_access_t *sa__, uint32_t *enable);
/* int falcon16_uc_reset(srds_access_t *sa__, uint32_t enable); */
err_code_t falcon16_prbs_tx_inv_data_get(srds_access_t *sa__, uint32_t *inv_data);
err_code_t falcon16_prbs_rx_inv_data_get(srds_access_t *sa__, uint32_t *inv_data);
err_code_t falcon16_prbs_tx_poly_get(srds_access_t *sa__, falcon16_prbs_polynomial_type_t *prbs_poly);
err_code_t falcon16_prbs_rx_poly_get(srds_access_t *sa__, falcon16_prbs_polynomial_type_t *prbs_poly);
err_code_t falcon16_pmd_force_signal_detect(srds_access_t *sa__, uint32_t enable);
err_code_t falcon16_pll_mode_set(srds_access_t *sa__, int pll_mode);
err_code_t falcon16_pll_mode_get(srds_access_t *sa__, uint32_t *pll_mode);
err_code_t falcon16_afe_pll_reg_set(srds_access_t *sa__, const phymod_afe_pll_t *afe_pll);
err_code_t falcon16_afe_pll_reg_get(srds_access_t *sa__, phymod_afe_pll_t *afe_pll);
err_code_t falcon16_osr_mode_set(srds_access_t *sa__, int osr_mode);
err_code_t falcon16_osr_mode_get(srds_access_t *sa__, int *osr_mode);
err_code_t falcon16_tsc_dig_lpbk_get(srds_access_t *sa__, uint32_t *lpbk);
err_code_t falcon16_tsc_rmt_lpbk_get(srds_access_t *sa__, uint32_t *lpbk);
err_code_t falcon16_core_soft_reset(srds_access_t *sa__);
err_code_t falcon16_core_soft_reset_release(srds_access_t *sa__, uint32_t enable);
err_code_t falcon16_core_soft_reset_read(srds_access_t *sa__, uint32_t *enable);
err_code_t falcon16_lane_soft_reset_read(srds_access_t *sa__, uint32_t *enable);
err_code_t falcon16_pmd_tx_disable_pin_dis_set(srds_access_t *sa__, uint32_t enable);
err_code_t falcon16_pmd_tx_disable_pin_dis_get(srds_access_t *sa__, uint32_t *enable);
err_code_t falcon16_tsc_pwrdn_set(srds_access_t *sa__, int tx_rx, int pwrdn);
err_code_t falcon16_tsc_pwrdn_get(srds_access_t *sa__, power_status_t *pwrdn);
err_code_t falcon16_pcs_lane_swap_tx(srds_access_t *sa__, uint32_t tx_lane_map);
err_code_t falcon16_pmd_lane_swap (srds_access_t *sa__, uint32_t lane_map);
err_code_t falcon16_pmd_lane_swap_tx_get(srds_access_t *sa__, uint32_t *lane_map);
err_code_t falcon16_pmd_loopback_get(srds_access_t *sa__, uint32_t *enable);   
err_code_t falcon16_tsc_identify(srds_access_t *sa__, falcon16_rev_id0_t *rev_id0, falcon16_rev_id1_t *rev_id1);
err_code_t falcon16_pmd_ln_h_rstb_pkill_override( srds_access_t *sa__, uint16_t val); 
err_code_t falcon16_lane_soft_reset_release(srds_access_t *sa__, uint32_t enable);   /* pmd core soft reset */
err_code_t falcon16_lane_soft_reset_release_get(srds_access_t *sa__, uint32_t *enable);
err_code_t falcon16_lane_hard_soft_reset_release(srds_access_t *sa__, uint32_t enable);
err_code_t falcon16_clause72_control(const phymod_access_t *pc, uint32_t cl_72_en);        /* CLAUSE_72_CONTROL */
err_code_t falcon16_clause72_control_get(const phymod_access_t *pc, uint32_t *cl_72_en);   /* CLAUSE_72_CONTROL */
err_code_t falcon16_pmd_cl72_enable_get(srds_access_t *sa__, uint32_t *enable);
err_code_t falcon16_pmd_cl72_receiver_status(srds_access_t *sa__, uint32_t *status); 
err_code_t falcon16_tsc_ucode_init( srds_access_t *sa__ );
err_code_t falcon16_pram_firmware_enable(srds_access_t *sa__, int enable, int wait);  
err_code_t falcon16_pmd_reset_seq(srds_access_t *sa__, int pmd_touched);
err_code_t falcon16_pll_reset_enable_set(srds_access_t *sa__, int enable);
err_code_t falcon16_tsc_read_pll_range(srds_access_t *sa__, uint32_t *pll_range);
err_code_t falcon16_tsc_signal_detect (srds_access_t *sa__, uint32_t *signal_detect);
err_code_t falcon16_force_tx_set_rst (srds_access_t *sa__, uint32_t rst);
err_code_t falcon16_force_tx_get_rst (srds_access_t *sa__, uint32_t *rst);
err_code_t falcon16_force_rx_set_rst (srds_access_t *sa__, uint32_t rst);
err_code_t falcon16_force_rx_get_rst (srds_access_t *sa__, uint32_t *rst);
err_code_t falcon16_tsc_ladder_setting_to_mV(srds_access_t *sa__, int8_t y, int16_t* level);
err_code_t falcon16_electrical_idle_set(srds_access_t *sa__, uint32_t en);
err_code_t falcon16_tsc_get_pll_vco_osmode(const phymod_phy_inf_config_t* config, uint32_t *vco_rate, uint32_t *new_pll_div, int16_t *new_os_mode);
err_code_t falcon16_tsc_tx_shared_patt_gen_en_get( srds_access_t *sa__, uint8_t *enable);
err_code_t falcon16_tsc_config_shared_tx_pattern_idx_set( srds_access_t *sa__, const uint32_t *pattern_len);
err_code_t falcon16_tsc_config_shared_tx_pattern_idx_get( srds_access_t *sa__, uint32_t *pattern_len, uint32_t *pattern);
err_code_t falcon16_tsc_tx_disable_get (srds_access_t *sa__, uint8_t *enable);
err_code_t falcon16_tsc_tx_pi_control_get(srds_access_t *sa__, uint16_t* value);
err_code_t falcon16_rescal_val_set(srds_access_t *sa__, uint8_t enable, uint32_t rescal_val);
err_code_t falcon16_rescal_val_get(srds_access_t *sa__, uint32_t *rescal_val);
err_code_t falcon16_tsc_get_eye_margin_est (srds_access_t *sa__, int *left_eye_mUI, int *right_eye_mUI, int *upper_eye_mV, int *lower_eye_mV);
err_code_t falcon16_clk4pcs_3bit_40t_config(srds_access_t *sa__);
err_code_t falcon16_tsc_rx_ppm(srds_access_t *sa__, int16_t *rx_ppm);
err_code_t falcon16_tsc_txfir_post_uc_set(srds_access_t *sa__, uint8_t tx_post);
err_code_t falcon16_tsc_txfir_post_uc_get(srds_access_t *sa__, uint8_t* tx_post);
err_code_t falcon16_tsc_rx_pmd_restart(srds_access_t *sa__);
#endif /* FALCON16_CFG_SEQ_H */
