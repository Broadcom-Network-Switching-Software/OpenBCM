/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 *  $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 * 
*/

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include "merlin7_cfg_seq.h"
#include "merlin7_tsc_functions.h"
#include <phymod/chip/bcmi_tsce7_xgxs_defs.h>
#include "merlin7_tsc_fields.h"
#include "merlin7_tsc_dependencies.h"
#include "merlin7_tsc_field_access.h"
#include "merlin7_tsc_debug_functions.h"
#include "merlin7_tsc_internal.h"
#include "merlin7_api_uc_vars_rdwr_defns.h"
#include "merlin7_tsc_access.h"

/** Macro to execute a statement with automatic return of error LESTM(Local ESTM) to fix coverity error */
#define LESTM(statement) do {err_code_t srds_err_code = ERR_CODE_NONE; statement; if (srds_err_code) return(srds_err_code); } while(0)
/** Macro to invoke a function with automatic return of error */
#define LEFUN(fun) do {err_code_t srds_err_code = (fun); if (srds_err_code) return(srds_err_code); } while(0)

err_code_t merlin7_uc_active_set(const phymod_access_t *sa__, uint32_t enable)           /* set microcontroller active or not  */
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    srds_err_code = wrc_uc_active(enable); if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_uc_active_get(const phymod_access_t *sa__, uint32_t *enable)           /* get microcontroller active or not  */
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    *enable = rdc_uc_active();
    if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_pmd_ln_h_rstb_pkill_override(const phymod_access_t *sa__, uint16_t val)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    srds_err_code = wr_pmd_ln_h_rstb_pkill(val); if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_core_soft_reset_release(const phymod_access_t *sa__, uint32_t enable)   /* release the pmd core soft reset */
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    srds_err_code = wrc_core_dp_s_rstb(enable); if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_lane_hard_soft_reset_release(const phymod_access_t *sa__, uint32_t enable)   /* release the pmd core soft reset */
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    srds_err_code = wr_ln_s_rstb(enable); if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_lane_soft_reset_release(const phymod_access_t *sa__, uint32_t enable)   /* release the pmd core soft reset */
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    srds_err_code = wr_ln_dp_s_rstb(enable); if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_lane_soft_reset_release_get(const phymod_access_t *sa__, uint32_t *enable)   /* release the pmd core soft reset */
{
    err_code_t srds_err_code;
  srds_err_code = ERR_CODE_NONE;

  *enable=rd_ln_dp_s_rstb(); if(srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_rx_lane_soft_reset_release(const phymod_access_t *sa__, uint32_t enable)   /* release the pmd core soft reset */
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    srds_err_code = wr_ln_rx_dp_s_rstb(enable); if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_tx_lane_soft_reset_release(const phymod_access_t *sa__, uint32_t enable)   /* release the pmd core soft reset */
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    srds_err_code = wr_ln_tx_dp_s_rstb(enable); if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_rx_lane_soft_reset_release_get(const phymod_access_t *sa__, uint32_t *enable)   /* release the pmd rx lane soft reset */
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;

    *enable=rd_ln_rx_dp_s_rstb(); if(srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_tx_lane_soft_reset_release_get(const phymod_access_t *sa__, uint32_t *enable)   /* release the pmd tx lane  soft reset */
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;

    *enable=rd_ln_tx_dp_s_rstb(); if(srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}


err_code_t merlin7_pmd_tx_disable_pin_dis_set(const phymod_access_t *sa__, uint32_t enable)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    srds_err_code = wr_pmd_tx_disable_pkill(enable); if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_pmd_loopback_set(const phymod_access_t *sa__, uint32_t enable)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;

    srds_err_code = merlin7_tsc_dig_lpbk(sa__, enable); if (srds_err_code) return(srds_err_code);
    srds_err_code = wr_signal_detect_frc(1);         if (srds_err_code) return(srds_err_code);
    srds_err_code = wr_signal_detect_frc_val(1);     if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_pmd_loopback_get(const phymod_access_t *sa__, uint32_t *enable)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    *enable = rd_dig_lpbk_en();
    if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_rmt_lpbk_get(const phymod_access_t *sa__, uint32_t *lpbk)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    *lpbk = rd_rmt_lpbk_en();
    if (srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_osr_mode_get(const phymod_access_t *sa__, int *osr_mode)
{
    int osr_forced;
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    osr_forced = rd_osr_mode_frc();
    if (osr_forced) {
        *osr_mode = rd_osr_mode_frc_val();
        if (srds_err_code)
            return(srds_err_code);
    } else {
        *osr_mode = rd_osr_mode_pin();
        if (srds_err_code)
            return (srds_err_code);
    }

    return ERR_CODE_NONE;
}

int merlin7_osr_mode_to_enum(int osr_mode, phymod_osr_mode_t* osr_mode_en)
{
    switch (osr_mode) {
        case 0: *osr_mode_en = phymodOversampleMode1; break;
        case 1: *osr_mode_en = phymodOversampleMode2; break;
        case 2: *osr_mode_en = phymodOversampleMode3; break;
        case 3: *osr_mode_en = phymodOversampleMode3P3; break;
        case 4: *osr_mode_en = phymodOversampleMode4; break;
        case 5: *osr_mode_en = phymodOversampleMode5; break;
        case 6: *osr_mode_en = phymodOversampleMode7P5; break;
        case 7: *osr_mode_en = phymodOversampleMode8; break;
        case 8: *osr_mode_en = phymodOversampleMode8P25; break;
        case 9: *osr_mode_en = phymodOversampleMode10; break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INTERNAL, (_PHYMOD_MSG("unsupported OS mode %d"), osr_mode));
    }
    return PHYMOD_E_NONE;
}

err_code_t merlin7_pmd_cl72_enable_get(const phymod_access_t* sa__, uint32_t* cl72_en)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    *cl72_en = rd_cl72_ieee_training_enable(); if (srds_err_code) return(srds_err_code);
    return ERR_CODE_NONE;
}

err_code_t merlin7_pmd_cl72_receiver_status(const phymod_access_t* sa__, uint32_t* status)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    *status = rd_cl72_ieee_receiver_status(); if (srds_err_code) return(srds_err_code);
    return ERR_CODE_NONE;
}

err_code_t merlin7_polarity_set(const phymod_access_t *sa__, uint32_t tx_polarity, uint32_t rx_polarity)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    srds_err_code = wr_tx_pmd_dp_invert(tx_polarity); if (srds_err_code) return(srds_err_code);
    srds_err_code = wr_rx_pmd_dp_invert(rx_polarity); if (srds_err_code) return(srds_err_code);
    return ERR_CODE_NONE;
}

err_code_t merlin7_polarity_get(const phymod_access_t *sa__, uint32_t* tx_polarity, uint32_t* rx_polarity)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;
    *tx_polarity = rd_tx_pmd_dp_invert(); if (srds_err_code) return(srds_err_code);
    *rx_polarity = rd_rx_pmd_dp_invert(); if (srds_err_code) return(srds_err_code);
    return ERR_CODE_NONE;
}

err_code_t merlin7_ucode_pram_load_pre(const phymod_access_t *sa__)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;

    srds_err_code = wrc_micro_master_clk_en(0x1); if(srds_err_code) return(srds_err_code);                        /* Enable clock to microcontroller subsystem */
    srds_err_code = wrc_micro_master_rstb(0x1); if(srds_err_code) return(srds_err_code);                          /* De-assert reset to microcontroller sybsystem */
    srds_err_code = wrc_micro_master_rstb(0x0); if(srds_err_code) return(srds_err_code);                          /* Assert reset to microcontroller sybsystem - Toggling reset*/
    srds_err_code = wrc_micro_master_rstb(0x1); if(srds_err_code) return(srds_err_code);                          /* De-assert reset to microcontroller sybsystem */

    srds_err_code = wrc_micro_ra_init(0x1); if(srds_err_code) return(srds_err_code);                              /* Set initialization command to initialize code RAM */
    srds_err_code = merlin7_tsc_INTERNAL_poll_micro_ra_initdone(sa__, 250);
    if(srds_err_code) return(srds_err_code); /* Poll for micro_ra_initdone = 1 to indicate initialization done */

    srds_err_code = wrc_micro_ra_init(0x0); if(srds_err_code) return(srds_err_code);                             /* Clear initialization command */
    srds_err_code = wrc_micro_pramif_ahb_wraddr_msw(0x0000); if(srds_err_code) return(srds_err_code);
    srds_err_code = wrc_micro_pramif_ahb_wraddr_lsw(0x0000); if(srds_err_code) return(srds_err_code);
    srds_err_code = wrc_micro_pram_if_rstb(1); if(srds_err_code) return(srds_err_code);
    srds_err_code = wrc_micro_pramif_en(1); if(srds_err_code) return(srds_err_code);

    /* Wait 8 pram clocks */
    (void)USR_DELAY_US(1);

    return ERR_CODE_NONE;
}

err_code_t merlin7_ucode_pram_load_post(const phymod_access_t *sa__)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;

    /* Wait 8 pram clocks */
    (void)USR_DELAY_US(1);

    srds_err_code = wrc_micro_pramif_en(0); if(srds_err_code) return(srds_err_code);
    srds_err_code = wrc_micro_core_clk_en(1); if(srds_err_code) return(srds_err_code);

    return ERR_CODE_NONE;
}

err_code_t merlin7_identify(const phymod_access_t *sa__, merlin7_rev_id0_t *rev_id0, merlin7_rev_id1_t *rev_id1)
{
  err_code_t srds_err_code;

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

  srds_err_code = ERR_CODE_NONE;

  rev_id0->revid_rev_letter =rdc_revid_rev_letter(); if(srds_err_code) return(srds_err_code);
  rev_id0->revid_rev_number =rdc_revid_rev_number(); if(srds_err_code) return(srds_err_code);
  rev_id0->revid_bonding    =rdc_revid_bonding();    if(srds_err_code) return(srds_err_code);
  rev_id0->revid_process    =rdc_revid_process();    if(srds_err_code) return(srds_err_code);
  rev_id0->revid_model      =rdc_revid_model();      if(srds_err_code) return(srds_err_code);

  rev_id1->revid_multiplicity =rdc_revid_multiplicity(); if(srds_err_code) return(srds_err_code);
  rev_id1->revid_mdio         =rdc_revid_mdio();         if(srds_err_code) return(srds_err_code);
  rev_id1->revid_micro        =rdc_revid_micro();        if(srds_err_code) return(srds_err_code);
  rev_id1->revid_cl72         =rdc_revid_cl72();         if(srds_err_code) return(srds_err_code);
  rev_id1->revid_pir          =rdc_revid_pir();          if(srds_err_code) return(srds_err_code);
  rev_id1->revid_llp          =rdc_revid_llp();          if(srds_err_code) return(srds_err_code);
  rev_id1->revid_eee          =rdc_revid_eee();          if(srds_err_code) return(srds_err_code);

  return ERR_CODE_NONE;
}

err_code_t merlin7_pwrdn_set(const phymod_access_t *sa__, int tx_rx, int pwrdn)
{
  err_code_t srds_err_code;
  srds_err_code = ERR_CODE_NONE;
  if(tx_rx) {
    srds_err_code = (uint32_t) wr_ln_tx_s_pwrdn(pwrdn);
  } else {
    srds_err_code = (uint32_t) wr_ln_rx_s_pwrdn(pwrdn);
  }
  if(srds_err_code) return(srds_err_code);

  return ERR_CODE_NONE;
}

err_code_t merlin7_pwrdn_get(const phymod_access_t *sa__, power_status_t *pwrdn)
{
  err_code_t srds_err_code;
  srds_err_code = ERR_CODE_NONE;
  pwrdn->pll_pwrdn  = 0;
  pwrdn->tx_s_pwrdn = 0;
  pwrdn->rx_s_pwrdn = 0;
  pwrdn->pll_pwrdn  = rdc_afe_s_pll_pwrdn(); if(srds_err_code) return(srds_err_code);
  pwrdn->tx_s_pwrdn = rd_ln_tx_s_pwrdn();    if(srds_err_code) return(srds_err_code);
  pwrdn->rx_s_pwrdn = rd_ln_rx_s_pwrdn();    if(srds_err_code) return(srds_err_code);

  return ERR_CODE_NONE;
}

err_code_t merlin7_tx_disable_get(const phymod_access_t *sa__, uint32_t* en)
{
  err_code_t srds_err_code;
  srds_err_code = ERR_CODE_NONE;

  *en = rd_sdk_tx_disable(); if(srds_err_code) return(srds_err_code);

  return ERR_CODE_NONE;
}

err_code_t merlin7_rx_lane_control_set(const phymod_access_t *sa__, uint32_t en)
{
  err_code_t srds_err_code;
  srds_err_code = ERR_CODE_NONE;

  if(en) {
  srds_err_code = wr_signal_detect_frc(1); if(srds_err_code) return(srds_err_code);
  srds_err_code = wr_signal_detect_frc_val(0); if(srds_err_code) return(srds_err_code);
  } else {
  srds_err_code = wr_signal_detect_frc(0); if(srds_err_code) return(srds_err_code);
  srds_err_code = wr_signal_detect_frc_val(0); if(srds_err_code) return(srds_err_code);
  }

  return ERR_CODE_NONE;
}


err_code_t merlin7_rx_lane_control_get(const phymod_access_t *sa__, uint32_t* en)
{
  err_code_t srds_err_code;
  srds_err_code = ERR_CODE_NONE;

  *en = rd_signal_detect_frc(); if(srds_err_code) return(srds_err_code);

  return ERR_CODE_NONE;
}

err_code_t merlin7_clause72_control(const phymod_access_t* sa__, int cl72_en)                /* CLAUSE_72_CONTROL */
{
    err_code_t srds_err_code;

    srds_err_code = wr_cl72_ieee_training_enable(cl72_en); if (srds_err_code) return(srds_err_code);
    srds_err_code = wr_cl72_ieee_restart_training(cl72_en); if (srds_err_code) return(srds_err_code);

    return PHYMOD_E_NONE;
}

err_code_t merlin7_get_pll_vco_osmode (const phymod_phy_inf_config_t* config,
                                       uint32_t *new_pll_div,
                                       int16_t *new_os_mode)
{
    *new_pll_div = 0;
    *new_os_mode = 0;

    if(config->ref_clock == phymodRefClk156Mhz) {
        switch (config->data_rate) {
            case 9375 :    /*speed 9.375G*/
                *new_pll_div = MERLIN7_TSC_PLL_DIV_60; *new_os_mode = 1;
                break;

            case 10000 :   /* speed 10G     */
                *new_pll_div = MERLIN7_TSC_PLL_DIV_64; *new_os_mode = 1;
                break;

            case 10312:   /* speed 10.3125G  */
                *new_pll_div = MERLIN7_TSC_PLL_DIV_66; *new_os_mode = 1;
                break;

            case 10937:   /* speed 10.9375G  */
                *new_pll_div = MERLIN7_TSC_PLL_DIV_70; *new_os_mode = 1;
                break;

            case 11250:   /*speed 11.25G*/
                *new_pll_div = MERLIN7_TSC_PLL_DIV_72; *new_os_mode = 1;
                break;

            case 12500:   /* speed 12.5G     */
                *new_pll_div = MERLIN7_TSC_PLL_DIV_80; *new_os_mode = 1;
                break;

            default:
                PHYMOD_DEBUG_ERROR(("Unsupported speed :: %d :: at ref clk :: %d\n", (int)config->data_rate, config->ref_clock));
                return ERR_CODE_DIAG;
                break;
        }
    }
    return(ERR_CODE_NONE);
}

err_code_t merlin7_osmode_set(const phymod_access_t* sa__, int16_t os_mode)
{
    err_code_t srds_err_code;

    srds_err_code = wr_osr_mode_frc(1); if (srds_err_code) return(srds_err_code);
    srds_err_code = wr_osr_mode_frc_val(os_mode); if (srds_err_code) return(srds_err_code);

    return PHYMOD_E_NONE;
}

err_code_t merlin7_pmd_reset_seq(const phymod_access_t *sa__, int pmd_touched)
{
    err_code_t srds_err_code;
    srds_err_code = ERR_CODE_NONE;

   if (pmd_touched == 0) {
       srds_err_code = wrc_core_s_rstb(1); if(srds_err_code) return(srds_err_code);
       srds_err_code = wrc_core_dp_s_rstb(1); if(srds_err_code) return(srds_err_code);
   }
   return (ERR_CODE_NONE);
}

err_code_t merlin7_refclk_set(const phymod_access_t *sa__, phymod_ref_clk_t ref_clock)
{
    err_code_t srds_err_code = ERR_CODE_NONE;

    switch (ref_clock) {
        case phymodRefClk156Mhz:
            srds_err_code = wrc_heartbeat_count_1us(0x271);
            break;
        case phymodRefClk125Mhz:
            srds_err_code = wrc_heartbeat_count_1us(0x1f4);
            break;
        default:
            srds_err_code = wrc_heartbeat_count_1us(0x271);
            break;
    }

    return srds_err_code;
}

err_code_t merlin7_tsc_rx_ppm(const phymod_access_t *sa__, int16_t *rx_ppm)
{
  err_code_t srds_err_code;
  srds_err_code = ERR_CODE_NONE;
  *rx_ppm = rd_cdr_integ_reg() / 84;
  return (ERR_CODE_NONE);
}

/* Set device hardware version */
err_code_t merlin7_tsc_afe_hardware_version_set(const phymod_access_t *sa__, uint16_t ver_id)
{
    err_code_t srds_err_code = ERR_CODE_NONE;

    srds_err_code = wrcv_afe_hardware_version(ver_id);

    return srds_err_code;
}

/* Get device hardware version */
err_code_t merlin7_tsc_afe_hardware_version_get(const phymod_access_t *sa__, uint16_t *ver_id)
{
    err_code_t srds_err_code = ERR_CODE_NONE;

    *ver_id = rdcv_afe_hardware_version();
    if (srds_err_code) {
       return srds_err_code;
    }

    return ERR_CODE_NONE;
}


