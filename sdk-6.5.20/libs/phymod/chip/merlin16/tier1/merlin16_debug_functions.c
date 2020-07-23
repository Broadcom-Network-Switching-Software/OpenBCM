/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  merlin16_debug_functions.c                                  *
 *  Created On    :  03 Nov 2015                                                   *
 *  Created By    :  Brent Roberts                                                 *
 *  Description   :  APIs for Serdes IPs                                           *
 *  Revision      :   *
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                           *
 *  No portions of this material may be reproduced in any form without             *
 *  the written permission of:                                                     *
 *      Broadcom Corporation                                                       *
 *      5300 California Avenue                                                     *
 *      Irvine, CA  92617                                                          *
 *                                                                                 *
 *  All information contained in this document is Broadcom Corporation             *
 *  company private proprietary, and trade secret.                                 *
 */

/** @file merlin16_debug_functions.c
 * Implementation of API debug functions
 */

#include <phymod/phymod_system.h>
#include "merlin16_debug_functions.h"
#include "merlin16_access.h"
#include "merlin16_common.h"
#include "merlin16_config.h"
#include "merlin16_functions.h"
#include "merlin16_internal.h"
#include "merlin16_internal_error.h"
#include "merlin16_prbs.h"
#include "merlin16_select_defns.h"



/*************************/
/*  Stop/Resume uC Lane  */
/*************************/

err_code_t merlin16_stop_uc_lane(srds_access_t *sa__, uint8_t enable) {

    return(merlin16_stop_rx_adaptation(sa__,enable));
}


err_code_t merlin16_stop_uc_lane_status(srds_access_t *sa__, uint8_t *uc_lane_stopped) {

  if(!uc_lane_stopped) {
      return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*uc_lane_stopped = rdv_usr_sts_micro_stopped());

  return (ERR_CODE_NONE);
}

/*******************************************************************/
/*  APIs to Write Core/Lane Config and User variables into uC RAM  */
/*******************************************************************/

err_code_t merlin16_set_usr_ctrl_core_event_log_level(srds_access_t *sa__, uint8_t core_event_log_level) {
  return(wrcv_usr_ctrl_core_event_log_level(core_event_log_level));
}

err_code_t merlin16_set_usr_ctrl_lane_event_log_level(srds_access_t *sa__, uint8_t lane_event_log_level) {
  return(wrv_usr_ctrl_lane_event_log_level(lane_event_log_level));
}

err_code_t merlin16_set_usr_ctrl_disable_startup(srds_access_t *sa__, struct merlin16_usr_ctrl_disable_functions_st set_val) {
  EFUN(merlin16_INTERNAL_update_usr_ctrl_disable_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_startup_functions_byte(set_val.byte));
}

err_code_t merlin16_set_usr_ctrl_disable_startup_dfe(srds_access_t *sa__, struct merlin16_usr_ctrl_disable_dfe_functions_st set_val) {
  EFUN(merlin16_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_startup_dfe_functions_byte(set_val.byte));
}

err_code_t merlin16_set_usr_ctrl_disable_steady_state(srds_access_t *sa__, struct merlin16_usr_ctrl_disable_functions_st set_val) {
  EFUN(merlin16_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */
  EFUN(merlin16_INTERNAL_update_usr_ctrl_disable_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_steady_state_functions_byte(set_val.byte));
}

err_code_t merlin16_set_usr_ctrl_disable_steady_state_dfe(srds_access_t *sa__, struct merlin16_usr_ctrl_disable_dfe_functions_st set_val) {
  EFUN(merlin16_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_steady_state_dfe_functions_byte(set_val.byte));
}

/******************************************************************/
/*  APIs to Read Core/Lane Config and User variables from uC RAM  */
/******************************************************************/

err_code_t merlin16_get_usr_ctrl_core_event_log_level(srds_access_t *sa__, uint8_t *core_event_log_level) {

  if(!core_event_log_level) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*core_event_log_level = rdcv_usr_ctrl_core_event_log_level());

  return (ERR_CODE_NONE);
}

err_code_t merlin16_get_usr_ctrl_lane_event_log_level(srds_access_t *sa__, uint8_t *lane_event_log_level) {

  if(!lane_event_log_level) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*lane_event_log_level = rdv_usr_ctrl_lane_event_log_level());
  return (ERR_CODE_NONE);
}

err_code_t merlin16_get_usr_ctrl_disable_startup(srds_access_t *sa__, struct merlin16_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_startup_functions_byte());
  EFUN(merlin16_INTERNAL_update_usr_ctrl_disable_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t merlin16_get_usr_ctrl_disable_startup_dfe(srds_access_t *sa__, struct merlin16_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_startup_dfe_functions_byte());
  EFUN(merlin16_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t merlin16_get_usr_ctrl_disable_steady_state(srds_access_t *sa__, struct merlin16_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_steady_state_functions_byte());
  EFUN(merlin16_INTERNAL_update_usr_ctrl_disable_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t merlin16_get_usr_ctrl_disable_steady_state_dfe(srds_access_t *sa__, struct merlin16_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_steady_state_dfe_functions_byte());
  EFUN(merlin16_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(get_val));
  return (ERR_CODE_NONE);
}

/****************************************/
/*  Serdes Register/Variable Dump APIs  */
/****************************************/

err_code_t merlin16_reg_dump(srds_access_t *sa__) {
  uint16_t addr, rddata = 0;

  EFUN_PRINTF(("\n****  SERDES REGISTER DUMP    ****"));

  for (addr = 0x0; addr < 0x10; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(merlin16_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0x90; addr < 0xA0; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(merlin16_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0xD000; addr < 0xD1A0; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(merlin16_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }

  for (addr = 0xD200; addr < 0xD230; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(merlin16_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }
  for (addr = 0xFFD0; addr < 0xFFE0; addr++) {
    if (!(addr % 16))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    EFUN(merlin16_pmd_rdt_reg(sa__, addr,&rddata));
    EFUN_PRINTF(("%04x ",rddata));
  }
  return (ERR_CODE_NONE);
}


err_code_t merlin16_uc_core_var_dump(srds_access_t *sa__) {
  uint8_t addr;

  EFUN_PRINTF(("\n**** SERDES UC CORE RAM VARIABLE DUMP ****"));

  for (addr = 0x0; addr < 0xFF; addr++) {
    if (!(addr % 26))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    ESTM_PRINTF(("%02x ", merlin16_rdbc_uc_var(sa__, __ERR, addr)));
  }
  return (ERR_CODE_NONE);
}


err_code_t merlin16_uc_lane_var_dump(srds_access_t *sa__) {
  uint8_t     rx_lock, uc_stopped = 0;
  uint16_t    addr;
  EFUN_PRINTF(("\n**** SERDES UC LANE %d RAM VARIABLE DUMP ****",merlin16_get_lane(sa__)));

  ESTM(rx_lock = rd_pmd_rx_lock());

  {
      err_code_t err_code=ERR_CODE_NONE;
      uc_stopped = merlin16_INTERNAL_stop_micro(sa__,rx_lock,&err_code);
      if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
  }

  for (addr = 0x0; addr < LANE_VAR_RAM_SIZE; addr++) {
    if (!(addr % 26))  {
      EFUN_PRINTF(("\n%04x ",addr));
    }
    ESTM_PRINTF(("%02x ", merlin16_rdbl_uc_var(sa__, __ERR, addr)));
  }

  if (rx_lock == 1) {
      if (!uc_stopped) {
          EFUN(merlin16_stop_rx_adaptation(sa__, 0));
      }
  } else {
      EFUN(merlin16_stop_rx_adaptation(sa__, 0));
  }

  return (ERR_CODE_NONE);
}

/***************************************/
/*  API Function to Read Event Logger  */
/***************************************/

err_code_t merlin16_read_event_log(srds_access_t *sa__) {
    merlin16_INTERNAL_event_log_dump_state_t state;
    uint8_t micro_num = 0;
    {
        state.index = 0;
        state.line_start_index = 0;
        EFUN(merlin16_INTERNAL_read_event_log_with_callback(sa__, micro_num, 0, &state, merlin16_INTERNAL_event_log_dump_callback));
        EFUN(merlin16_INTERNAL_event_log_dump_callback(&state, 0, 0));
    }
    return(ERR_CODE_NONE);
}

/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/

/* Enable/Disable Digital Loopback */
err_code_t merlin16_dig_lpbk(srds_access_t *sa__, uint8_t enable) {
    /* setting/clearing prbs_chk_en_auto_mode while coming out of/going in to dig lpbk */
    EFUN(wr_prbs_chk_en_auto_mode(!enable));
    EFUN(wr_dig_lpbk_en(enable));                         /* 0 = disabled, 1 = enabled */

    /* Disabling Link Training as it's an invalid mode in digital loopback */
    if(enable) {
        uint8_t link_training_enable = 0;
        ESTM(link_training_enable = rd_cl72_ieee_training_enable());
        if(link_training_enable) {
            EFUN_PRINTF(("Warning: Core: %d, Lane %d: Link Training mode is on in digital loopback.\n", merlin16_get_core(sa__), merlin16_get_lane(sa__)));
        }
    }
  return (ERR_CODE_NONE);
}


/* Enable/Diasble Digital Loopback for Repeater */
err_code_t merlin16_dig_lpbk_rptr(srds_access_t *sa__, uint8_t enable, enum srds_rptr_mode_enum mode) {   
  switch(mode) {
    case DATA_IN_SIDE:
        /* DATA_IN_SIDE  - The side where data is fed into the RX serial input */
        if (enable) {
            EFUN(merlin16_loop_timing(sa__, 0));
            EFUN(merlin16_loop_timing(sa__, 1)); /* Program DATA_IN_SIDE side TX_PI for loop-timing mode to lock SYS TX to SYS TX clock freq. */
        } else {
            EFUN(merlin16_loop_timing(sa__, 0));
        }
        break;

    case DIG_LPBK_SIDE:
        /* DIG_LPBK_SIDE - The side where data is looped back using digital loopback */
        
        EFUN(merlin16_tx_rptr_mode_timing(sa__, 1)); /* Program DIG_LPBK_SIDE side TX_PI for repeater mode to lock LINE TX to SYS RX clock freq. */
        EFUN(wr_dig_lpbk_en(enable));              /* 0 = diabled, 1 = enabled */
        EFUN(wr_ams_tx_ll_selpath_tx(0));          /* Switch mux and enable traffic to flow through */
        break;

    default : return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  return (ERR_CODE_NONE);
}


err_code_t merlin16_nl_from_dig_lpbk(srds_access_t *sa__, enum srds_rptr_mode_enum mode) {
    EFUN(merlin16_dig_lpbk_rptr(sa__, 0,mode));
    return (ERR_CODE_NONE);

}


/**********************************/
/*  TX_PI Jitter Generation APIs  */
/**********************************/

/* TX_PI Sinusoidal or Spread-Spectrum (SSC) Jitter Generation  */
err_code_t merlin16_tx_pi_jitt_gen(srds_access_t *sa__, uint8_t enable, int16_t freq_override_val, enum srds_tx_pi_freq_jit_gen_enum jit_type, uint8_t tx_pi_jit_freq_idx, uint8_t tx_pi_jit_amp) {
    /* Added a limiting for the jitter amplitude index, per freq_idx */
    uint8_t max_amp_idx_r20_os1[] = {37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 48, 33, 47, 37, 33, 37, 37};

    /* Irrespective of the osr_mode, txpi runs @ os1. Thus the max amp idx values remain the same. */
    if (jit_type == TX_PI_SJ) {
        if (tx_pi_jit_amp > max_amp_idx_r20_os1[tx_pi_jit_freq_idx]) {
            tx_pi_jit_amp = max_amp_idx_r20_os1[tx_pi_jit_freq_idx];
        }
    }

    EFUN(merlin16_tx_pi_freq_override(sa__, enable, freq_override_val));

    if (enable) {
        EFUN(wr_tx_pi_jit_freq_idx(tx_pi_jit_freq_idx));
        EFUN(wr_tx_pi_jit_amp(tx_pi_jit_amp));

        if (jit_type == TX_PI_SSC_HIGH_FREQ) {
            EFUN(wr_tx_pi_jit_ssc_freq_mode(0x1));        /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
            EFUN(wr_tx_pi_ssc_gen_en(0x1));               /* SSC jitter enable:         0 = disabled,    1 = enabled */
        }
        else if (jit_type == TX_PI_SSC_LOW_FREQ) {
            EFUN(wr_tx_pi_jit_ssc_freq_mode(0x0));        /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
            EFUN(wr_tx_pi_ssc_gen_en(0x1));               /* SSC jitter enable:         0 = disabled,    1 = enabled */
        }
        else if (jit_type == TX_PI_SJ) {
            EFUN(wr_tx_pi_sj_gen_en(0x1));                /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
        }
    }
    else {
        EFUN(wr_tx_pi_ssc_gen_en(0x0));                   /* SSC jitter enable:         0 = disabled,    1 = enabled */
        EFUN(wr_tx_pi_sj_gen_en(0x0));                    /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
    }
  return (ERR_CODE_NONE);
}


/*******************************/
/*  Isolate Serdes Input Pins  */
/*******************************/

err_code_t merlin16_isolate_ctrl_pins(srds_access_t *sa__, uint8_t enable) {
    uint8_t lane, lane_orig, num_lanes;
    merlin16_info_t const * const merlin16_info_ptr = merlin16_INTERNAL_get_merlin16_info_ptr();
        EFUN(merlin16_isolate_core_ctrl_pins(sa__, enable));
    
    ESTM(lane_orig = merlin16_get_lane(sa__));
    num_lanes = merlin16_info_ptr->lane_count * merlin16_info_ptr->micro_count;
    for(lane = 0; lane < num_lanes; lane++) {
        EFUN(merlin16_set_lane(sa__, lane));
        EFUN(merlin16_isolate_lane_ctrl_pins(sa__, enable));
    }
    EFUN(merlin16_set_lane(sa__, lane_orig));

  return (ERR_CODE_NONE);
}

err_code_t merlin16_isolate_lane_ctrl_pins(srds_access_t *sa__, uint8_t enable) {

  if (enable) {
    EFUN(wr_pmd_ln_tx_h_pwrdn_pkill(0x1));
    EFUN(wr_pmd_ln_rx_h_pwrdn_pkill(0x1));
    EFUN(wr_pmd_ln_dp_h_rstb_pkill(0x1));
    EFUN(wr_pmd_ln_h_rstb_pkill(0x1));
    EFUN(wr_pmd_tx_disable_pkill(0x1));
  }
  else {
    EFUN(wr_pmd_ln_tx_h_pwrdn_pkill(0x0));
    EFUN(wr_pmd_ln_rx_h_pwrdn_pkill(0x0));
    EFUN(wr_pmd_ln_dp_h_rstb_pkill(0x0));
    EFUN(wr_pmd_ln_h_rstb_pkill(0x0));
    EFUN(wr_pmd_tx_disable_pkill(0x0));
  }
  return (ERR_CODE_NONE);
}

err_code_t merlin16_isolate_core_ctrl_pins(srds_access_t *sa__, uint8_t enable) {

  if (enable) {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x1));
  }
  else {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x0));
  }
  return (ERR_CODE_NONE);
}


