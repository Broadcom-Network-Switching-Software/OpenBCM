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
 * File       : merlin7_cfg_seq.h
 * Description: c functions implementing Tier1s for TEMod Serdes Driver
 *---------------------------------------------------------------------*/
/*
 *  $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * 
*/


#ifndef MERLIN7_CFG_SEQ_H
#define MERLIN7_CFG_SEQ_H

#include "common/srds_api_err_code.h"
#include "merlin7_tsc_enum.h"

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
} merlin7_rev_id0_t;

typedef struct {
  int8_t revid_eee;
  int8_t revid_llp;
  int8_t revid_pir;
  int8_t revid_cl72;
  int8_t revid_micro;
  int8_t revid_mdio;
  int8_t revid_multiplicity;
} merlin7_rev_id1_t;


err_code_t merlin7_uc_active_set(const phymod_access_t *pa, uint32_t enable);              /* set microcontroller active or not  */
err_code_t merlin7_uc_active_get(const phymod_access_t *pa, uint32_t *enable);             /* get microcontroller active or not  */
err_code_t merlin7_pmd_ln_h_rstb_pkill_override( const phymod_access_t *pa, uint16_t val);
err_code_t merlin7_core_soft_reset_release(const phymod_access_t *pa, uint32_t enable);    /* release the pmd core soft reset */
err_code_t merlin7_lane_soft_reset_release(const phymod_access_t *pa, uint32_t enable);    /* release the pmd lane soft reset */
err_code_t merlin7_pmd_tx_disable_pin_dis_set(const phymod_access_t *pa, uint32_t enable);
err_code_t merlin7_pmd_loopback_set(const phymod_access_t *pa, uint32_t enable);
err_code_t merlin7_pmd_loopback_get(const phymod_access_t *pa, uint32_t *enable);
err_code_t merlin7_rmt_lpbk_get(const phymod_access_t *pa, uint32_t *lpbk);
err_code_t merlin7_osr_mode_get(const phymod_access_t *pa, int *osr_mode);
int merlin7_osr_mode_to_enum(int osr_mode, phymod_osr_mode_t* osr_mode_en);
err_code_t merlin7_pmd_cl72_enable_get(const phymod_access_t* sa__, uint32_t* cl72_en);
err_code_t merlin7_pmd_cl72_receiver_status(const phymod_access_t* sa__, uint32_t* status);
err_code_t merlin7_polarity_set(const phymod_access_t *sa__, uint32_t tx_polarity, uint32_t rx_polarity);
err_code_t merlin7_polarity_get(const phymod_access_t *sa__, uint32_t* tx_polarity, uint32_t* rx_polarity);
err_code_t merlin7_ucode_pram_load_pre(const phymod_access_t *sa__);
err_code_t merlin7_ucode_pram_load_post(const phymod_access_t *sa__);
err_code_t merlin7_identify(const phymod_access_t *sa__, merlin7_rev_id0_t *rev_id0, merlin7_rev_id1_t *rev_id1);
err_code_t merlin7_rx_lane_soft_reset_release(const phymod_access_t *sa__, uint32_t enable);
err_code_t merlin7_tx_lane_soft_reset_release(const phymod_access_t *sa__, uint32_t enable);
err_code_t merlin7_rx_lane_soft_reset_release_get(const phymod_access_t *sa__, uint32_t *enable) ;
err_code_t merlin7_tx_lane_soft_reset_release_get(const phymod_access_t *sa__, uint32_t *enable) ;
err_code_t merlin7_lane_soft_reset_release_get(const phymod_access_t *sa__, uint32_t *enable);
err_code_t merlin7_pwrdn_set(const phymod_access_t *sa__, int tx_rx, int pwrdn);
err_code_t merlin7_pwrdn_get(const phymod_access_t *sa__, power_status_t *pwrdn);
err_code_t merlin7_tx_disable_get(const phymod_access_t *sa__, uint32_t* en);
err_code_t merlin7_rx_lane_control_set(const phymod_access_t *sa__, uint32_t en);
err_code_t merlin7_rx_lane_control_get(const phymod_access_t *sa__, uint32_t* en);
err_code_t merlin7_clause72_control(const phymod_access_t* sa__, int cl72_en);
err_code_t merlin7_get_pll_vco_osmode (const phymod_phy_inf_config_t* config, uint32_t *new_pll_div, int16_t *new_os_mode);
err_code_t merlin7_osmode_set(const phymod_access_t* sa__, int16_t os_mode);
err_code_t merlin7_pmd_reset_seq(const phymod_access_t *sa__, int pmd_touched);
err_code_t merlin7_refclk_set(const phymod_access_t *sa__, phymod_ref_clk_t ref_clock);
err_code_t merlin7_lane_hard_soft_reset_release(const phymod_access_t *sa__, uint32_t enable) ;
err_code_t merlin7_tsc_rx_ppm(const phymod_access_t *sa__, int16_t *rx_ppm);
err_code_t merlin7_tsc_afe_hardware_version_set(const phymod_access_t *sa__, uint16_t ver_id);
err_code_t merlin7_tsc_afe_hardware_version_get(const phymod_access_t *sa__, uint16_t *ver_id);

#endif /* MERLIN7_CFG_SEQ_H */
