/*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
*/

/*
 *
 *
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */


#include <phymod/phymod.h>
#include <phymod/phymod_util.h>
#include "falcon16_v1l4p1_cfg_seq.h"
#include "../include/falcon16_v1l4p1_fields.h"
#include "../include/falcon16_v1l4p1_field_access.h"
#include "../include/falcon16_v1l4p1_dependencies.h"
#include "../include/falcon16_v1l4p1_interface.h"
#include "../include/falcon16_v1l4p1_internal.h"

#define FALCON16_TXFIR_POST_UCRAM_ADDR 0x18
#define FALCON16_PMD_LOCK_COUNTER_ADDR 0x10

err_code_t falcon16_v1l4p1_tx_rx_polarity_set(srds_access_t *sa__, uint32_t tx_pol, uint32_t rx_pol) 
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err = (uint32_t) wr_tx_pmd_dp_invert(tx_pol);
  if(__err) return(__err);
  __err = (uint32_t) wr_rx_pmd_dp_invert(rx_pol);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_tx_rx_polarity_get(srds_access_t *sa__, uint32_t *tx_pol, uint32_t *rx_pol) 
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *tx_pol = (uint32_t) rd_tx_pmd_dp_invert();
  if(__err) return(__err);
  *rx_pol = (uint32_t) rd_rx_pmd_dp_invert();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_uc_active_set(srds_access_t *sa__, uint32_t enable)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wrc_uc_active(enable);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_uc_active_get(srds_access_t *sa__, uint32_t *enable)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *enable = (uint32_t) rdc_uc_active();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_force_tx_set_rst (srds_access_t *sa__, uint32_t rst)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  __err = wr_afe_tx_reset_frc_val(rst);
  if(__err) return(__err);
  __err = wr_afe_tx_reset_frc(0x1);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_force_tx_get_rst (srds_access_t *sa__, uint32_t *rst)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  *rst=(uint32_t) rd_afe_tx_reset_frc_val();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_force_rx_set_rst (srds_access_t *sa__, uint32_t rst)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  __err = wr_afe_rx_reset_frc_val(rst);
  if(__err) return(__err);
  __err = wr_afe_rx_reset_frc(0x1);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_force_rx_get_rst (srds_access_t *sa__, uint32_t *rst)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *rst=(uint32_t) rd_afe_rx_reset_frc_val();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}


err_code_t falcon16_v1l4p1_prbs_tx_inv_data_get(srds_access_t *sa__, uint32_t *inv_data)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *inv_data = (uint32_t) rd_prbs_gen_inv();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_prbs_rx_inv_data_get(srds_access_t *sa__, uint32_t *inv_data)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *inv_data = (uint32_t) rd_prbs_chk_inv();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_prbs_tx_poly_get(srds_access_t *sa__, falcon16_v1l4p1_prbs_polynomial_type_t *prbs_poly)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *prbs_poly = rd_prbs_gen_mode_sel();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_prbs_rx_poly_get(srds_access_t *sa__, falcon16_v1l4p1_prbs_polynomial_type_t *prbs_poly)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *prbs_poly = rd_prbs_chk_mode_sel();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pmd_force_signal_detect(srds_access_t *sa__, uint32_t enable)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  __err = wr_signal_detect_frc(enable); if(__err) return(__err);
  __err = wr_signal_detect_frc_val(enable); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pll_mode_set(srds_access_t *sa__, int pll_mode)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wrc_pll_mode(pll_mode);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pll_mode_get(srds_access_t *sa__, uint32_t *pll_mode)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *pll_mode=rdc_pll_mode();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_afe_pll_reg_set(srds_access_t *sa__, const phymod_afe_pll_t *afe_pll)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  if(afe_pll->afe_pll_change_default) {
    __err=wrc_ams_pll_iqp(afe_pll->ams_pll_iqp);
    if(__err) return(__err);
  } else {
    __err=wrc_ams_pll_iqp(0x5);
    if(__err) return(__err);
  }

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_afe_pll_reg_get(srds_access_t *sa__, phymod_afe_pll_t *afe_pll)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  afe_pll->ams_pll_iqp=rdc_ams_pll_iqp();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_osr_mode_set(srds_access_t *sa__, int osr_mode)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_osr_mode_frc_val(osr_mode);
  if(__err) return(__err);
  __err=wr_osr_mode_frc(1);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_osr_mode_get(srds_access_t *sa__, int *osr_mode)
{
  int osr_forced;
  err_code_t __err;
  __err=ERR_CODE_NONE;
  osr_forced = rd_osr_mode_frc();
  if(osr_forced) {
    *osr_mode = rd_osr_mode_frc_val();
    if(__err) return(__err);
  } else {
    *osr_mode = rd_osr_mode_pin();
    if(__err) return(__err);
  }

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_dig_lpbk_get(srds_access_t *sa__, uint32_t *lpbk)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *lpbk=rd_dig_lpbk_en();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_rmt_lpbk_get(srds_access_t *sa__, uint32_t *lpbk)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *lpbk=rd_rmt_lpbk_en();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_core_soft_reset(srds_access_t *sa__)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wrc_core_dp_s_rstb(1);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_core_soft_reset_release(srds_access_t *sa__, uint32_t enable)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wrc_core_dp_s_rstb(enable);
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_core_soft_reset_read(srds_access_t *sa__, uint32_t *enable)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *enable = rdc_core_dp_s_rstb();    if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_lane_soft_reset_read(srds_access_t *sa__, uint32_t *enable)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *enable = rd_ln_dp_s_rstb();    if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pmd_tx_disable_pin_dis_set(srds_access_t *sa__, uint32_t enable) {
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_pmd_tx_disable_pkill(enable); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pmd_tx_disable_pin_dis_get(srds_access_t *sa__, uint32_t *enable) {
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *enable = rd_pmd_tx_disable_pkill(); if(__err) return(__err);

  return ERR_CODE_NONE;
}

/* set powerdown for tx or rx */
/* tx_rx == 1 => disable (enable) power for Tx */
/* tx_rx != 0 => disable (enable) power for Rx */
/* pwrdn == 0 => enable power */
/* pwrdn == 1 => disable power */
err_code_t falcon16_v1l4p1_pwrdn_set(srds_access_t *sa__, int tx_rx, int pwrdn)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  if(tx_rx) {
    __err = (uint32_t) wr_ln_tx_s_pwrdn(pwrdn);
  } else {
    __err = (uint32_t) wr_ln_rx_s_pwrdn(pwrdn);
  }
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pwrdn_get(srds_access_t *sa__, power_status_t *pwrdn)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  pwrdn->pll_pwrdn  = 0;
  pwrdn->tx_s_pwrdn = 0;
  pwrdn->rx_s_pwrdn = 0;
  pwrdn->pll_pwrdn  = rdc_afe_s_pll_pwrdn(); if(__err) return(__err);
  pwrdn->tx_s_pwrdn = rd_ln_tx_s_pwrdn();    if(__err) return(__err);
  pwrdn->rx_s_pwrdn = rd_ln_rx_s_pwrdn();    if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pmd_loopback_get(srds_access_t *sa__, uint32_t *enable)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *enable = rd_dig_lpbk_en(); if(__err) return(__err);
  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pmd_cl72_enable_get(srds_access_t *sa__, uint32_t *enable)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *enable = rd_cl93n72_ieee_training_enable(); if(__err) return(__err);
  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pmd_cl72_receiver_status(srds_access_t *sa__, uint32_t *status)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *status = rd_cl93n72_ieee_receiver_status(); if(__err) return(__err);
  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pram_firmware_enable(srds_access_t *sa__, int enable, int wait)   /* release the pmd core soft reset */
{
  err_code_t __err;

  __err=ERR_CODE_NONE;
  if(enable == 1) {
      __err=wrc_micro_pramif_ahb_wraddr_msw(0); if(__err) return(__err);
      __err=wrc_micro_pramif_ahb_wraddr_lsw(0); if(__err) return(__err);

      __err=wrc_micro_pram_if_rstb(1); if(__err) return(__err);
      __err=wrc_micro_pramif_en(1);    if(__err) return(__err);

      if(wait) {
          /* coverity[check_return] */
          falcon16_v1l4p1_delay_us(500);
      }
  } else {
      __err=wrc_micro_pramif_en(0);     if(__err) return(__err);
      __err=wrc_micro_core_clk_en(1);   if(__err) return(__err);

  }
  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_identify(srds_access_t *sa__, falcon16_v1l4p1_rev_id0_t *rev_id0, falcon16_v1l4p1_rev_id1_t *rev_id1)
{
  err_code_t __err;

  rev_id0->revid_rev_letter =0;
  rev_id0->revid_rev_number =0;
  rev_id0->revid_bonding    =0;
  rev_id0->revid_process    =0;
  rev_id0->revid_model      =0;

  rev_id1->revid_multiplicity =0;
  rev_id1->revid_mdio         =0;
  rev_id1->revid_micro        =0;
  rev_id1->revid_cl72         =0;
  rev_id1->revid_pir          =0;
  rev_id1->revid_llp          =0;
  rev_id1->revid_eee          =0;

  __err=ERR_CODE_NONE;

  rev_id0->revid_rev_letter = rdc_revid_rev_letter(); if(__err) return(__err);
  rev_id0->revid_rev_number = rdc_revid_rev_number(); if(__err) return(__err);
  rev_id0->revid_bonding    = rdc_revid_bonding();    if(__err) return(__err);
  rev_id0->revid_process    = rdc_revid_process();    if(__err) return(__err);
  rev_id0->revid_model      = rdc_revid_model();      if(__err) return(__err);

  rev_id1->revid_multiplicity = rdc_revid_multiplicity(); if(__err) return(__err);
  rev_id1->revid_mdio         = rdc_revid_mdio();         if(__err) return(__err);
  rev_id1->revid_micro        = rdc_revid_micro();        if(__err) return(__err);
  rev_id1->revid_cl72         = rdc_revid_cl72();         if(__err) return(__err);
  rev_id1->revid_pir          = rdc_revid_pir();          if(__err) return(__err);
  rev_id1->revid_llp          = rdc_revid_llp();          if(__err) return(__err);
  rev_id1->revid_eee          = rdc_revid_eee();          if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pmd_ln_h_rstb_pkill_override( srds_access_t *sa__, uint16_t val)
{
  err_code_t __err;
    /*
    * Work around per Magesh/Justin
    * override input from PCS to allow uc_dsc_ready_for_cmd
    * reg get written by UC
    */
  __err=ERR_CODE_NONE;
  __err=wr_pmd_ln_h_rstb_pkill(val); if(__err) return(__err);
    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_lane_soft_reset_release(srds_access_t *sa__, uint32_t enable)   /* release the pmd core soft reset */
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_ln_dp_s_rstb(enable); if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_lane_soft_reset_release_get(srds_access_t *sa__, uint32_t *enable)   /* release the pmd core soft reset */
{
    err_code_t __err;
  __err=ERR_CODE_NONE;

  *enable=rd_ln_dp_s_rstb(); if(__err) return(__err);

    return ERR_CODE_NONE;
}


err_code_t falcon16_v1l4p1_lane_hard_soft_reset_release(srds_access_t *sa__, uint32_t enable)   /* release the pmd core soft reset */
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  __err=wr_ln_s_rstb(enable); if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_clause72_control(srds_access_t *sa__, uint32_t cl_72_en)                /* CLAUSE_72_CONTROL */
{
  err_code_t __err;
  uint32_t enable;

  __err=ERR_CODE_NONE;

  if (cl_72_en) {
    __err=wr_cl93n72_ieee_training_enable(1);  if(__err) return(__err);

  } else {
    __err=wr_cl93n72_ieee_training_enable(0);  if(__err) return(__err);
  }

  enable=rd_ln_dp_s_rstb(); if(__err) return(__err);
  if (enable)
  {
      __err=wr_ln_dp_s_rstb(0);                 if(__err) return(__err);
      __err=wr_ln_dp_s_rstb(1);                 if(__err) return(__err);
  }
  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_clause72_control_get(srds_access_t *sa__, uint32_t *cl_72_en)                /* CLAUSE_72_CONTROL */
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *cl_72_en = rd_cl93n72_ieee_training_enable(); if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_electrical_idle_set(srds_access_t *sa__, uint32_t en)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  __err = wr_ams_tx_elec_idle_aux(en); if(__err) return(__err);

  return ERR_CODE_NONE;
}

/***********************************************/
/*  Microcode Init into Program RAM Functions  */
/***********************************************/

/* uCode Load through Register (MDIO) Interface [Return Val = Error_Code (0 = PASS)] */
err_code_t falcon16_v1l4p1_ucode_init( srds_access_t *sa__ ) {

    err_code_t __err;
    uint8_t result;
    __err=ERR_CODE_NONE;

    __err = wrc_micro_master_clk_en(0x1); if(__err) return(__err); /* Enable clock to microcontroller subsystem */
    __err = wrc_micro_master_rstb(0x1); if(__err) return(__err);  /* De-assert reset to microcontroller sybsystem */
    __err = wrc_micro_master_rstb(0x0); if(__err) return(__err); /* Assert reset to microcontroller sybsystem - Toggling reset*/
    __err = wrc_micro_master_rstb(0x1); if(__err) return(__err); /* De-assert reset to microcontroller sybsystem */

    __err = wrc_micro_ra_init(0x1);if(__err) return(__err);/* Set initialization command to initialize code RAM */
    /* coverity[check_return] */
    falcon16_v1l4p1_delay_us(1000);

    result =  rdc_micro_ra_initdone();  /* Poll for micro_ra_initdone = 1 to indicate initialization done */
    if(__err) return(__err);
    if (!result) {                                         /* Check if initialization done within 500us time interval */
        PHYMOD_DEBUG_ERROR(("ERR_CODE_MICRO_INIT_NOT_DONE\n"));
        return (ERR_CODE_MICRO_INIT_NOT_DONE);    /* Else issue error code */
    }

    __err = wrc_micro_ra_init(0x0); if(__err) return(__err);

    return (ERR_CODE_NONE);
}

/**
@brief   Init the PMD
@param   pmd_touched If the PMD is already initialized
@returns The value ERR_CODE_NONE upon successful completion
@details Per core PMD resets (both datapath and entire core)
We only intend to use this function if the PMD has never been initialized.
*/
err_code_t falcon16_v1l4p1_pmd_reset_seq(srds_access_t *sa__, int pmd_touched)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;

   if (pmd_touched == 0) {
         __err = wrc_core_s_rstb(1); if(__err) return(__err);

        __err = wrc_core_dp_s_rstb(1); if(__err) return(__err);
  }
    return (ERR_CODE_NONE);
}


/**
@brief   Enable the pll reset bit
@param   enable Controls whether to reset PLL
@returns The value ERR_CODE_NONE upon successful completion
@details
Resets the PLL
*/
err_code_t falcon16_v1l4p1_pll_reset_enable_set (srds_access_t *sa__, int enable)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;

    __err = wrc_afe_s_pll_reset_frc_val(enable); if(__err) return(__err);

    __err = wrc_afe_s_pll_reset_frc(1); if(__err) return(__err);

    return (ERR_CODE_NONE);
}

/**
@brief   Read PLL range
*/
err_code_t falcon16_v1l4p1_read_pll_range (srds_access_t *sa__, uint32_t *pll_range)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *pll_range = rdc_ams_pll_range();

  return (ERR_CODE_NONE);
}


/**
@brief   Reag signal detect
*/
err_code_t falcon16_v1l4p1_signal_detect (srds_access_t *sa__, uint32_t *signal_detect)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *signal_detect = rd_signal_detect();

  return (ERR_CODE_NONE);
}

err_code_t falcon16_v1l4p1_get_pll_vco_osmode (const phymod_phy_inf_config_t* config, uint32_t *vco_rate, uint32_t *new_pll_div, int16_t *new_os_mode) {
    *vco_rate = 0;
    *new_pll_div=0;
    *new_os_mode =0;

    if(config->ref_clock == phymodRefClk156Mhz) {
        switch (config->data_rate) {
            case 6250 :   /* speed 6.25G     */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_160; *new_os_mode = 2; *vco_rate = 25000;
                break;

            case 10312:   /* speed 10.3125G  */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_165; *new_os_mode = 0; *vco_rate = 25781;  /* 10G @25G VCO OS2p5 */
                break;

            case 10937:   /* speed 10.9375G  */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_175; *new_os_mode = 0; *vco_rate = 27343;  /* 11G @27G VCO OS2p5 */
                break;

            case 12500:   /* speed 12.5G     */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_160; *new_os_mode = 1; *vco_rate = 25000;
                break;

            case 20625:   /* speed 20.625G   */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_132; *new_os_mode = 0; *vco_rate = 20625;
                break;

            case 21875:   /* speed 21.875G   */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_140; *new_os_mode = 0; *vco_rate = 21875;
                break;

            case 25000:   /* speed 25G       */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_160; *new_os_mode = 0; *vco_rate = 25000;
                break;

            case 25781:   /* speed 25.78125G */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_165; *new_os_mode = 0; *vco_rate = 25781;
                break;

            case 27343:   /* speed 27.3435G */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_175; *new_os_mode = 0; *vco_rate = 27343;
                 break;

            case 28125:   /* speed 28.125G */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_180; *new_os_mode = 0; *vco_rate = 28125;
                 break;

            default:
                PHYMOD_DEBUG_ERROR(("Unsupported speed :: %d :: at ref clk :: %d\n", (int)config->data_rate, config->ref_clock));
                return ERR_CODE_DIAG;
                break;
        }
    } else if(config->ref_clock == phymodRefClk125Mhz) {
        switch (config->data_rate) {
            case 5750:   /* speed 5.75G     */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_184; *new_os_mode = 2; *vco_rate = 23000;
                break;

            case 6250:   /* speed 6.25G     */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_200; *new_os_mode = 2; *vco_rate = 25000;
                break;

            case 10312:   /* speed 10.3125G  */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_165; *new_os_mode = 1; *vco_rate = 20625;
                break;

            case 11250:   /* speed 11.25G    */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_180; *new_os_mode = 1; *vco_rate = 22500;
                break;

            case 11500:   /* speed 11.5G     */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_184; *new_os_mode = 1; *vco_rate = 23000;
                break;

            case 12500:   /* speed 12.5G     */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_200; *new_os_mode = 1; *vco_rate = 25000;
                break;

            case 20625:   /* speed 20.625G   */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_165; *new_os_mode = 0; *vco_rate = 20625;
                break;

            case 22500:   /* speed 22.5G     */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_180; *new_os_mode = 0; *vco_rate = 22500;
                break;

            case 23000:   /* speed 23G       */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_184; *new_os_mode = 0; *vco_rate = 23000;
                break;

            case 25000:   /* speed 25G     */
                *new_pll_div = FALCON16_V1L4P1_PLL_DIV_200; *new_os_mode = 0; *vco_rate = 25000;
                break;

            default:
                PHYMOD_DEBUG_ERROR(("Unsupported speed :: %d :: at ref clk :: %d\n", (int)config->data_rate, config->ref_clock));
                return ERR_CODE_DIAG;
                break;
        }
    } else {
        PHYMOD_DEBUG_ERROR(("Unsupported ref clk :: %d\n", config->ref_clock));
        return ERR_CODE_DIAG;
    }
    return(ERR_CODE_NONE);
}

/* Enable 10G clock when VCO is 25G */
err_code_t falcon16_v1l4p1_10g_clock_enable(srds_access_t *sa__, int enable, int enable_27g)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;

    __err = wr_ams_tx_txclk_div2p5_pwrup(enable); if(__err) return(__err);
    __err = wr_ams_tx_txclk_div2p5(enable);       if(__err) return(__err);
    __err = wr_ams_rx_rxclk_div2p5_pwrup(enable); if(__err) return(__err);
    __err = wr_ams_rx_rxclk_div2p5(enable);       if(__err) return(__err);

    __err = wr_ams_rx_rxclk_div2p5_27g(enable_27g);   if(__err) return(__err);
    __err = wr_ams_tx_txclk_div2p5_27g(enable_27g);   if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_10g_clock_enable_get(srds_access_t *sa__, int* enable, int* enable_27g)
{
    uint8_t txclk_div2p5_pwrup,txclk_div2p5, rxclk_div2p5_pwrup, rxclk_div2p5, rxclk_div2p5_27g, txclk_div2p5_27g;
    err_code_t __err;
    __err=ERR_CODE_NONE;

    txclk_div2p5_pwrup = rd_ams_tx_txclk_div2p5_pwrup();
    txclk_div2p5       = rd_ams_tx_txclk_div2p5();
    rxclk_div2p5_pwrup = rd_ams_rx_rxclk_div2p5_pwrup();
    rxclk_div2p5       = rd_ams_rx_rxclk_div2p5();

    rxclk_div2p5_27g = rd_ams_rx_rxclk_div2p5_27g();
    txclk_div2p5_27g = rd_ams_tx_txclk_div2p5_27g();
    if (txclk_div2p5_pwrup && txclk_div2p5 && rxclk_div2p5_pwrup && rxclk_div2p5) {
        if (rxclk_div2p5_27g && txclk_div2p5_27g) {
            *enable_27g = 1;
        } else {
            *enable_27g = 0;
        }
        *enable = 1;
    } else {
        *enable = 0;
        *enable_27g = 0;
    }

    return ERR_CODE_NONE;
}

/* Get Enable/Disable Shared TX pattern generator */
err_code_t falcon16_v1l4p1_tx_shared_patt_gen_en_get( srds_access_t *sa__, uint8_t *enable) {
  err_code_t __err;
  __err=ERR_CODE_NONE;

  *enable = rd_patt_gen_en(); if(__err) return(__err);
  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_config_shared_tx_pattern_idx_set( srds_access_t *sa__, const uint32_t *pattern_len) {
  err_code_t __err;
  uint8_t mode_sel;
  __err=ERR_CODE_NONE;

  __err=ERR_CODE_NONE;

  if(*pattern_len==240) {
     mode_sel = 11;
  } else if (*pattern_len== 220 ) {
     mode_sel = 10;
  } else if (*pattern_len == 200) {
     mode_sel = 9;
  } else if (*pattern_len == 180) {
     mode_sel = 8;
  } else if (*pattern_len == 160) {
     mode_sel = 7;
  } else if (*pattern_len == 140) {
     mode_sel = 6;
  } else {
    mode_sel = 0;
    PHYMOD_DEBUG_ERROR(("Invalid length input!\n"));
    return ERR_CODE_DIAG;
  }

  if(mode_sel !=0){
    __err = wr_patt_gen_start_pos(mode_sel);
    }
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_config_shared_tx_pattern_idx_get( srds_access_t *sa__, uint32_t *pattern_len, uint32_t *pattern) {
  err_code_t __err;
  uint8_t mode_sel;
  uint32_t lsb = 0, msb = 0;
  uint16_t temp_lsb, temp_msb;
  int pattern_idx;

  __err=ERR_CODE_NONE;

  mode_sel = rd_patt_gen_start_pos(); if(__err) return(__err);

  mode_sel = 12 - mode_sel;

  if(mode_sel == 6) {
    *pattern_len = 140;
  } else if (mode_sel == 5) {
    *pattern_len = 160;
  } else if (mode_sel == 4) {
    *pattern_len = 180;
  } else if (mode_sel == 3) {
    *pattern_len = 200;
  } else if (mode_sel == 2) {
    *pattern_len = 220;
  } else if (mode_sel == 1) {
    *pattern_len = 240;
  } else {
    *pattern_len = 0;
  }

  for (pattern_idx=0;pattern_idx<PATTERN_MAX_SIZE;pattern_idx++) {
      switch (pattern_idx) {
        case 0:
          msb=rdc_patt_gen_seq_14(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_13(); if(__err) return(__err);
          break;
        case 1:
          msb=rdc_patt_gen_seq_12(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_11(); if(__err) return(__err);
          break;
        case 2:
          msb=rdc_patt_gen_seq_10(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_9(); if(__err) return(__err);
          break;
        case 3:
          msb=rdc_patt_gen_seq_8(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_7(); if(__err) return(__err);
          break;
        case 4:
          msb=rdc_patt_gen_seq_6(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_5(); if(__err) return(__err);
          break;
        case 5:
          msb=rdc_patt_gen_seq_4(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_3(); if(__err) return(__err);
          break;
        case 6:
          msb=rdc_patt_gen_seq_2(); if(__err) return(__err);
          lsb=rdc_patt_gen_seq_1(); if(__err) return(__err);
          break;
        case 7:
          msb=rdc_patt_gen_seq_0(); if(__err) return(__err);
          lsb=0;
          break;
        /* Its a dead default and cause coverity defect
        default:
              PHYMOD_DEBUG_ERROR(("Wrong index value : Should be between 0 and 7\n"));
          return ERR_CODE_DIAG;
          break;*/
      }
      phymod_swap_bit(lsb, &temp_lsb);
      phymod_swap_bit(msb, &temp_msb);

      pattern[pattern_idx] = temp_lsb << 16 | temp_msb;
  }

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_tx_pi_control_get(srds_access_t *sa__, uint16_t* value)
{
    err_code_t __err;
    uint32_t enable;

    __err=ERR_CODE_NONE;

    enable=rd_tx_pi_en(); if(__err) return(__err);
    if (enable) {
        *value=rd_tx_pi_freq_override_val();  if(__err) return(__err);
    } else {
        *value=0;
    }

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_tx_pi_enable_set(srds_access_t *sa__, uint32_t enable)
{
    err_code_t __err;

    __err=ERR_CODE_NONE;

    __err = wr_tx_pi_en(enable); if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_tx_pi_enable_get(srds_access_t *sa__, uint32_t *enable)
{
    err_code_t __err;

    __err=ERR_CODE_NONE;

    *enable = rd_tx_pi_en(); if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_tx_pi_ext_pd_select_set(srds_access_t *sa__, uint32_t ext_pd)
{
    err_code_t __err;

    __err=ERR_CODE_NONE;

    __err = wr_tx_pi_ext_pd_inc_dec_sel(ext_pd ? 1 : 0); if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_tx_pi_ext_pd_select_get(srds_access_t *sa__, uint32_t *ext_pd)
{
    err_code_t __err;

    __err=ERR_CODE_NONE;

    *ext_pd = rd_tx_pi_ext_pd_inc_dec_sel(); if(__err) return(__err);

    return ERR_CODE_NONE;
}

/**********************************/
/*  Serdes TX disable/RX Restart  */
/**********************************/
err_code_t falcon16_v1l4p1_tx_disable_get (srds_access_t *sa__, uint8_t *enable){
  err_code_t __err;

  __err=ERR_CODE_NONE;
  *enable = (uint8_t) rd_sdk_tx_disable();
  if(__err) return(__err);

  return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_rescal_val_get(srds_access_t *sa__, uint32_t *rescal_val)
{
    err_code_t __err;
    uint8_t rescal_frc = 0;

    __err=ERR_CODE_NONE;
    rescal_frc = rdc_rescal_frc();
    if (rescal_frc == 1){
        *rescal_val = rdc_rescal_frc_val();
    } else {
        *rescal_val = rd_ams_tx_ana_rescal();
    }
    if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_rescal_val_set(srds_access_t *sa__, uint8_t enable, uint32_t rescal_val)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;

    __err = wrc_rescal_frc(enable);
    if(__err) return(__err);
    if (enable){
        __err = wrc_rescal_frc_val(rescal_val & 0xf);
        if(__err) return(__err);
    }

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_get_eye_margin_est (srds_access_t *sa__, int *left_eye_mUI, int *right_eye_mUI, int *upper_eye_mV, int *lower_eye_mV){

    uint8_t ladder_range = 0;
    err_code_t __err;
    __err=ERR_CODE_NONE;

    *left_eye_mUI = falcon16_v1l4p1_INTERNAL_eye_to_mUI(sa__, rdv_usr_sts_heye_left());
    if(__err) return(__err);
    *right_eye_mUI = falcon16_v1l4p1_INTERNAL_eye_to_mUI(sa__, rdv_usr_sts_heye_right());
    if(__err) return(__err);
    *upper_eye_mV = falcon16_v1l4p1_INTERNAL_eye_to_mV(sa__, rdv_usr_sts_veye_upper(), ladder_range);
    if(__err) return(__err);
    *lower_eye_mV = falcon16_v1l4p1_INTERNAL_eye_to_mV(sa__, rdv_usr_sts_veye_lower(), ladder_range);
    if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_clk4pcs_set(srds_access_t *sa__, falcon16_v1l4p1_clk4pcs_type_t clk4pcs)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;

    __err = wrc_ams_pll_set_clk4pcs(clk4pcs & 0x3);
    if(__err) return(__err);

    __err = wrc_ams_pll_set_clk4pcs_msb((clk4pcs & 0x4) >> 2);
    if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_clk4sync_set(srds_access_t *sa__, uint32_t enable, uint32_t div)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;

    __err = wrc_ams_pll_clk4sync_en(enable);
    if(__err) return(__err);
    __err = wrc_ams_pll_clk4sync_div(div);
    if(__err) return(__err);

    return ERR_CODE_NONE;
}


err_code_t falcon16_v1l4p1_rx_ppm(srds_access_t *sa__, int16_t *rx_ppm)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *rx_ppm = rd_cdr_integ_reg() / 84;
  if(__err) return(__err);
  return (ERR_CODE_NONE);
}

err_code_t falcon16_v1l4p1_txfir_post_uc_get(srds_access_t *sa__, uint8_t* tx_post)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;

    *tx_post = falcon16_v1l4p1_rdbcs_uc_var(sa__, &__err, FALCON16_TXFIR_POST_UCRAM_ADDR);
    if(__err) return(__err);

    return ERR_CODE_NONE;
}


err_code_t falcon16_v1l4p1_txfir_post_uc_set(srds_access_t *sa__, uint8_t tx_post)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;

  __err = falcon16_v1l4p1_wrbc_uc_var(sa__, FALCON16_TXFIR_POST_UCRAM_ADDR, tx_post);
  if(__err) return(__err);

  return (ERR_CODE_NONE);
}

err_code_t falcon16_v1l4p1_rx_pmd_restart(srds_access_t *sa__)
{
    err_code_t __err;
    __err=ERR_CODE_NONE;

    __err = wr_rx_restart_pmd(1);
    if(__err) return(__err);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_rx_pmd_lock_counter_get(srds_access_t *sa__, uint32_t *value)
{
    err_code_t __err;

    __err = ERR_CODE_NONE;
    *value = falcon16_v1l4p1_rdbl_uc_var(sa__, &__err, FALCON16_PMD_LOCK_COUNTER_ADDR);
    if (__err) return (__err);

    return ERR_CODE_NONE;
}
