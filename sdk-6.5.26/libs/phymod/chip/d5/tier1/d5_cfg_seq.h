/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
*/


#ifndef D5_CFG_SEQ_H
#define D5_CFG_SEQ_H

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
} d5_rev_id0_t;

typedef struct {
  int8_t revid_eee;
  int8_t revid_llp;
  int8_t revid_pir;
  int8_t revid_cl72;
  int8_t revid_micro;
  int8_t revid_mdio;
  int8_t revid_multiplicity;
} d5_rev_id1_t;

typedef enum {
  TX = 0,
  Rx
} tx_rx_t;

int d5_identify(phymod_access_t *sa__, d5_rev_id0_t *rev_id0, d5_rev_id1_t *rev_id1);
int d5_tx_rx_polarity_set(phymod_access_t *sa__, uint32_t tx_pol, uint32_t rx_pol);
int d5_tx_rx_polarity_get(phymod_access_t *sa__, uint32_t *tx_pol, uint32_t *rx_pol);
int d5_dig_lpbk_get(phymod_access_t *sa__, uint32_t *lpbk);
int d5_rmt_lpbk_get(phymod_access_t *sa__, uint32_t *lpbk);
int d5_lane_soft_reset_read(phymod_access_t *sa__, uint32_t *enable);
int d5_pwrdn_set(phymod_access_t *sa__, int tx_rx, int pwrdn);
int d5_pwrdn_get(phymod_access_t *sa__, power_status_t *pwrdn);
int d5_pmd_loopback_get(phymod_access_t *sa__, uint32_t *enable);
int d5_lane_soft_reset_release(phymod_access_t *sa__, uint32_t enable);
int d5_lane_soft_reset_release_get(phymod_access_t *sa__, uint32_t *enable);
int d5_lane_hard_soft_reset_release(phymod_access_t *sa__, uint32_t enable);
int d5_clause72_control(phymod_access_t *sa__, uint32_t cl_72_en);
int d5_clause72_control_get(phymod_access_t *sa__, uint32_t *cl_72_en);
int d5_pmd_cl72_enable_get(phymod_access_t *sa__, uint32_t *enable);
int d5_pmd_cl72_receiver_status(phymod_access_t *sa__, uint32_t *status);
int d5_pmd_reset_seq(phymod_access_t *sa__, int pmd_touched);
int d5_signal_detect (phymod_access_t *sa__, uint32_t *signal_detect);
int d5_electrical_idle_set(phymod_access_t *sa__, uint32_t en);
int d5_tx_disable_get (phymod_access_t *sa__, uint8_t *enable);
int d5_rescal_val_set(phymod_access_t *sa__, uint8_t enable, uint32_t rescal_val);
int d5_rescal_val_get(phymod_access_t *sa__, uint32_t *rescal_val);
int d5_rx_pmd_restart(phymod_access_t *sa__);
#endif /* D5_CFG_SEQ_H */
