/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  merlin7_tsc_debug_functions.c                                  *
 *  Created On    :  03 Nov 2015                                                   *
 *  Created By    :  Brent Roberts                                                 *
 *  Description   :  Debug APIs for Serdes IPs                                     *
 *  Revision      :   *
 */

/** @file merlin7_tsc_debug_functions.c
 * Implementation of API debug functions
 */
#include <phymod/phymod_system.h>
#include "merlin7_tsc_debug_functions.h"
#include "merlin7_tsc_access.h"
#include "merlin7_tsc_common.h"
#include "merlin7_tsc_config.h"
#include "merlin7_tsc_functions.h"
#include "merlin7_tsc_internal.h"
#include "merlin7_tsc_internal_error.h"
#include "merlin7_tsc_prbs.h"
#include "merlin7_tsc_select_defns.h"
#include "merlin7_tsc_reg_dump.h"



#ifndef SMALL_FOOTPRINT
static err_code_t _merlin7_tsc_reg_print_no_buf(srds_access_t *sa__, uint8_t reg_section);
static err_code_t _merlin7_tsc_reg_print_with_buf(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][SRDS_DUMP_BUF_SIZE], uint8_t *buf_index);
static err_code_t _merlin7_tsc_reg_print(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][SRDS_DUMP_BUF_SIZE], uint8_t *buf_index);
#endif /* SMALL_FOOTPRINT */

#ifndef SMALL_FOOTPRINT

/*************************/
/*  Stop/Resume uC Lane  */
/*************************/

err_code_t merlin7_tsc_stop_uc_lane(srds_access_t *sa__, uint8_t enable) {

    return(merlin7_tsc_stop_rx_adaptation(sa__,enable));
}


err_code_t merlin7_tsc_stop_uc_lane_status(srds_access_t *sa__, uint8_t *uc_lane_stopped) {
    INIT_SRDS_ERR_CODE

  if(!uc_lane_stopped) {
      return(merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*uc_lane_stopped = rdv_usr_sts_micro_stopped());

  return (ERR_CODE_NONE);
}

/*******************************************************************/
/*  APIs to Write Core/Lane Config and User variables into uC RAM  */
/*******************************************************************/

err_code_t merlin7_tsc_set_usr_ctrl_core_event_log_level(srds_access_t *sa__, uint8_t core_event_log_level) {
  return(wrcv_usr_ctrl_core_event_log_level(core_event_log_level));
}

err_code_t merlin7_tsc_set_usr_ctrl_lane_event_log_level(srds_access_t *sa__, uint8_t lane_event_log_level) {
  return(wrv_usr_ctrl_lane_event_log_level(lane_event_log_level));
}

err_code_t merlin7_tsc_set_usr_ctrl_disable_startup(srds_access_t *sa__, struct merlin7_tsc_usr_ctrl_disable_functions_st set_val) {
    INIT_SRDS_ERR_CODE
  EFUN(merlin7_tsc_INTERNAL_update_usr_ctrl_disable_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_startup_functions_byte(set_val.byte));
}

err_code_t merlin7_tsc_set_usr_ctrl_disable_startup_dfe(srds_access_t *sa__, struct merlin7_tsc_usr_ctrl_disable_dfe_functions_st set_val) {
    INIT_SRDS_ERR_CODE
  EFUN(merlin7_tsc_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_startup_dfe_functions_byte(set_val.byte));
}

err_code_t merlin7_tsc_set_usr_ctrl_disable_steady_state(srds_access_t *sa__, struct merlin7_tsc_usr_ctrl_disable_functions_st set_val) {
    INIT_SRDS_ERR_CODE
  EFUN(merlin7_tsc_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */
  EFUN(merlin7_tsc_INTERNAL_update_usr_ctrl_disable_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_steady_state_functions_byte(set_val.byte));
}

err_code_t merlin7_tsc_set_usr_ctrl_disable_steady_state_dfe(srds_access_t *sa__, struct merlin7_tsc_usr_ctrl_disable_dfe_functions_st set_val) {
    INIT_SRDS_ERR_CODE
  EFUN(merlin7_tsc_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_steady_state_dfe_functions_byte(set_val.byte));
}

/******************************************************************/
/*  APIs to Read Core/Lane Config and User variables from uC RAM  */
/******************************************************************/

err_code_t merlin7_tsc_get_usr_ctrl_core_event_log_level(srds_access_t *sa__, uint8_t *core_event_log_level) {
    INIT_SRDS_ERR_CODE

  if(!core_event_log_level) {
     return(merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*core_event_log_level = rdcv_usr_ctrl_core_event_log_level());

  return (ERR_CODE_NONE);
}

err_code_t merlin7_tsc_get_usr_ctrl_lane_event_log_level(srds_access_t *sa__, uint8_t *lane_event_log_level) {
    INIT_SRDS_ERR_CODE

  if(!lane_event_log_level) {
     return(merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*lane_event_log_level = rdv_usr_ctrl_lane_event_log_level());
  return (ERR_CODE_NONE);
}

err_code_t merlin7_tsc_get_usr_ctrl_disable_startup(srds_access_t *sa__, struct merlin7_tsc_usr_ctrl_disable_functions_st *get_val) {
    INIT_SRDS_ERR_CODE

  if(!get_val) {
     return(merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_startup_functions_byte());
  EFUN(merlin7_tsc_INTERNAL_update_usr_ctrl_disable_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t merlin7_tsc_get_usr_ctrl_disable_startup_dfe(srds_access_t *sa__, struct merlin7_tsc_usr_ctrl_disable_dfe_functions_st *get_val) {
    INIT_SRDS_ERR_CODE

  if(!get_val) {
     return(merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_startup_dfe_functions_byte());
  EFUN(merlin7_tsc_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t merlin7_tsc_get_usr_ctrl_disable_steady_state(srds_access_t *sa__, struct merlin7_tsc_usr_ctrl_disable_functions_st *get_val) {
    INIT_SRDS_ERR_CODE

  if(!get_val) {
     return(merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_steady_state_functions_byte());
  EFUN(merlin7_tsc_INTERNAL_update_usr_ctrl_disable_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t merlin7_tsc_get_usr_ctrl_disable_steady_state_dfe(srds_access_t *sa__, struct merlin7_tsc_usr_ctrl_disable_dfe_functions_st *get_val) {
    INIT_SRDS_ERR_CODE

  if(!get_val) {
     return(merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_steady_state_dfe_functions_byte());
  EFUN(merlin7_tsc_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(get_val));
  return (ERR_CODE_NONE);
}

/********************************************************************************/
/* Helper print function for Serdes Register/Variable Dump using no buffer      */
/********************************************************************************/
static err_code_t _merlin7_tsc_reg_print_no_buf(srds_access_t *sa__, uint8_t reg_section) {
    INIT_SRDS_ERR_CODE
    uint16_t addr, rddata = 0;

    for (addr = merlin7_tsc_reg_dump_arr[reg_section][0]; addr < merlin7_tsc_reg_dump_arr[reg_section][1]; addr++) {
        if (!(addr & 0x000F)) {
            EFUN_PRINTF(("\n%04x ",addr));
        }
        EFUN(merlin7_tsc_pmd_rdt_reg(sa__, addr,&rddata));
        EFUN_PRINTF(("%04x ",rddata));
    }
    return (ERR_CODE_NONE);
}
/********************************************************************************/
/* Helper print function for Serdes Register/Variable Dump using a buffer       */
/********************************************************************************/
static err_code_t _merlin7_tsc_reg_print_with_buf(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][SRDS_DUMP_BUF_SIZE], uint8_t *buf_index) {
    INIT_SRDS_ERR_CODE
    uint16_t addr, rddata = 0;
	uint8_t first = 0;
    int32_t count = 0;

	if(reg_section == 0) {
        first = 1;
    }

    for (addr = merlin7_tsc_reg_dump_arr[reg_section][0]; addr < merlin7_tsc_reg_dump_arr[reg_section][1]; addr++) {
        if (!(addr & 0x000F)) {
            if(first) {
                first = 0;  /* don't increment buf_index for first section */
            }
            else {
                (*buf_index)++;
                count = 0;
            }
            count += USR_SNPRINTF(reg_buffer[*buf_index] + count, (size_t)(SRDS_DUMP_BUF_SIZE - count - 1),"%04x ", addr);
        }
        EFUN(merlin7_tsc_pmd_rdt_reg(sa__, addr,&rddata));
        count += USR_SNPRINTF(reg_buffer[*buf_index] + count, (size_t)(SRDS_DUMP_BUF_SIZE - count - 1), "%04x ", rddata);
    }
    return (ERR_CODE_NONE);
}
/********************************************************************************/
/* Helper print function for Serdes Register/Variable Dump                      */
/********************************************************************************/
static err_code_t _merlin7_tsc_reg_print(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][SRDS_DUMP_BUF_SIZE], uint8_t *buf_index) {
    INIT_SRDS_ERR_CODE
    if(reg_buffer == NULL) {
         EFUN(_merlin7_tsc_reg_print_no_buf(sa__, reg_section));
    }
    else {
        EFUN(_merlin7_tsc_reg_print_with_buf(sa__, reg_section, reg_buffer, buf_index));
    }
    return (ERR_CODE_NONE);
}
err_code_t merlin7_tsc_reg_dump(srds_access_t *sa__, uint8_t core, uint8_t lane, char reg_buffer[][SRDS_DUMP_BUF_SIZE]) {
    INIT_SRDS_ERR_CODE
    uint8_t reg_section = 0;
    uint8_t buf_index = 0;

    if (reg_buffer == NULL) {
        EFUN_PRINTF(("\n****  SERDES REGISTER CORE %d LANE %d DUMP    ****", core, lane));
    }
    else {
        EFUN_PRINTF(("\n****  SERDES REGISTER CORE %d LANE %d DECODED    ****\n", core, lane));
    }
    
    while(reg_section < MERLIN7_TSC_REG_DUMP_SECTIONS) {
        if(merlin7_tsc_reg_dump_arr[reg_section][0] == MERLIN7_TSC_REG_PLL_START) {
                while( reg_section < MERLIN7_TSC_REG_DUMP_SECTIONS && merlin7_tsc_reg_dump_arr[reg_section][0] != MERLIN7_TSC_REG_UC_START) {
                    EFUN(_merlin7_tsc_reg_print(sa__, reg_section, reg_buffer, &buf_index));
                    reg_section++;
                }
        }
        else if (merlin7_tsc_reg_dump_arr[reg_section][0] == MERLIN7_TSC_REG_UC_START) {
                while(reg_section < MERLIN7_TSC_REG_DUMP_SECTIONS) {
                    EFUN(_merlin7_tsc_reg_print(sa__, reg_section, reg_buffer, &buf_index));
                    reg_section++;
                }
        }
        else {
            EFUN(_merlin7_tsc_reg_print(sa__, reg_section, reg_buffer, &buf_index));
            reg_section++;
        }
    }

    if (reg_buffer != NULL) {
        reg_buffer[++buf_index][0] = 0xA;
        reg_buffer[++buf_index][0] = 0;
    }

    return (ERR_CODE_NONE);
}

err_code_t merlin7_tsc_uc_core_var_dump(srds_access_t *sa__, uint8_t core, char ram_buffer[][SRDS_DUMP_BUF_SIZE]) {
    INIT_SRDS_ERR_CODE
  uint8_t rddata;
  uint8_t addr, core_var_ram_size;

  srds_info_t * merlin7_tsc_info_ptr = merlin7_tsc_INTERNAL_get_merlin7_tsc_info_ptr_with_check(sa__);
  EFUN(merlin7_tsc_INTERNAL_verify_merlin7_tsc_info(sa__, merlin7_tsc_info_ptr));
  core_var_ram_size = (uint8_t)merlin7_tsc_info_ptr->core_var_ram_size;

  if (ram_buffer == NULL) {
      EFUN_PRINTF(("\n**** SERDES UC CORE %d RAM VARIABLE DUMP ****", core));
      for (addr = 0x0; addr < core_var_ram_size; addr++) {
          if (!(addr % 26))  {
              EFUN_PRINTF(("\n%04x ",addr));
          }
          ESTM_PRINTF(("%02x ", merlin7_tsc_rdbc_uc_var(sa__, __ERR, addr)));
      }
  }
  else {
      uint8_t index=0;
      int32_t count = 0;

      EFUN_PRINTF(("\n**** SERDES UC CORE RAM VARIABLE DECODED ****\n"));
      for (addr = 0x0; addr < core_var_ram_size; addr++) {
          if (!(addr % 26))  {
             if (addr != 0) {
                index++;
                count = 0;
             }
             count += USR_SNPRINTF(ram_buffer[index] + count, (size_t)(SRDS_DUMP_BUF_SIZE - count - 1), "%04x ", addr);
          }
          ESTM(rddata = merlin7_tsc_rdbc_uc_var(sa__, __ERR, addr));
          count += USR_SNPRINTF(ram_buffer[index] + count, (size_t)(SRDS_DUMP_BUF_SIZE - count - 1), "%02x ", rddata);
      }
      ram_buffer[++index][0] = 0xA;
      ram_buffer[++index][0] = 0;
  }
  return (ERR_CODE_NONE);
}


err_code_t merlin7_tsc_uc_lane_var_dump(srds_access_t *sa__, uint8_t core, uint8_t lane, char ram_buffer[][SRDS_DUMP_BUF_SIZE]) {
    INIT_SRDS_ERR_CODE
  uint8_t     rx_lock, uc_stopped = 0;
  uint16_t    addr, lane_var_ram_size;
  srds_info_t const * const merlin7_tsc_info_ptr = merlin7_tsc_INTERNAL_get_merlin7_tsc_info_ptr_with_check(sa__);
  EFUN(merlin7_tsc_INTERNAL_verify_merlin7_tsc_info(sa__, merlin7_tsc_info_ptr));
  lane_var_ram_size = (uint16_t)merlin7_tsc_info_ptr->lane_var_ram_size;

  ESTM(rx_lock = rd_pmd_rx_lock());

  {
      err_code_t err_code=ERR_CODE_NONE;
      uc_stopped = merlin7_tsc_INTERNAL_stop_micro(sa__,rx_lock,&err_code);
      if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
  }

  if (ram_buffer == NULL) {
      EFUN_PRINTF(("\n**** SERDES UC CORE %d LANE %d RAM VARIABLE DUMP ****", core,lane));
      for (addr = 0x0; addr < lane_var_ram_size; addr++) {
         if (!(addr % 26))  {
            EFUN_PRINTF(("\n%04x ",addr));
         }
         ESTM_PRINTF(("%02x ", merlin7_tsc_rdbl_uc_var(sa__, __ERR, addr)));
      }
  }
  else {
      uint8_t rddata, index = 0;
      int32_t count = 0;
      
      EFUN_PRINTF(("\n**** SERDES UC CORE %d LANE %d RAM VARIABLE DECODED ****\n", core,lane));

      for (addr = 0x0; addr < lane_var_ram_size; addr++) {
          if (!(addr % 26))  {
              if (addr != 0) {
                  index++;
                  count = 0;
            }
            count += USR_SNPRINTF(ram_buffer[index] + count, (size_t)(SRDS_DUMP_BUF_SIZE - count - 1), "%04x ", addr);
         }
         ESTM(rddata = merlin7_tsc_rdbl_uc_var(sa__, __ERR, addr));
         count += USR_SNPRINTF(ram_buffer[index] + count,  (size_t)(SRDS_DUMP_BUF_SIZE - count - 1), "%02x ", rddata);
      }
      ram_buffer[++index][0] = 0xA;
      ram_buffer[++index][0] = 0;
  }

  if (rx_lock == 1) {
      if (!uc_stopped) {
          EFUN(merlin7_tsc_stop_rx_adaptation(sa__, 0));
      }
  } else {
      EFUN(merlin7_tsc_stop_rx_adaptation(sa__, 0));
  }

  return (ERR_CODE_NONE);
}

/***************************************/
/*  API Function to Read Event Logger  */
/***************************************/

err_code_t merlin7_tsc_read_event_log(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    merlin7_tsc_INTERNAL_event_log_dump_state_t state;
    uint8_t micro_num = 0;
    {
        state.index = 0;
        state.line_start_index = 0;
        EFUN(merlin7_tsc_INTERNAL_read_event_log_with_callback(sa__, micro_num, 0, &state, merlin7_tsc_INTERNAL_event_log_dump_callback));
        EFUN(merlin7_tsc_INTERNAL_event_log_dump_callback(sa__, &state, 0, 0));
    }
    return(ERR_CODE_NONE);
}

/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/


/* Enable/Disable Digital Loopback */
err_code_t merlin7_tsc_dig_lpbk(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    /* setting/clearing prbs_chk_en_auto_mode while coming out of/going in to dig lpbk */
    EFUN(wr_prbs_chk_en_auto_mode(!enable));
    EFUN(wr_dig_lpbk_en(enable));                         /* 0 = disabled, 1 = enabled */

    /* Link Training enabled is an invalid mode in digital loopback */
    if(enable) {
        uint8_t link_training_enable = 0;
        ESTM(link_training_enable = rd_cl72_ieee_training_enable());
        if(link_training_enable) {
            EFUN_PRINTF(("Warning: Core: %d, Lane %d: Link Training mode is on in digital loopback.\n", merlin7_tsc_get_core(sa__), merlin7_tsc_get_lane(sa__)));
        }
    }

  return (ERR_CODE_NONE);
}


/**********************************/
/*  TX_PI Jitter Generation APIs  */
/**********************************/

/* TX_PI Sinusoidal or Spread-Spectrum (SSC) Jitter Generation  */
err_code_t merlin7_tsc_tx_pi_jitt_gen(srds_access_t *sa__, uint8_t enable, int16_t freq_override_val, enum srds_tx_pi_freq_jit_gen_enum jit_type, uint8_t tx_pi_jit_freq_idx, uint8_t tx_pi_jit_amp) {
    INIT_SRDS_ERR_CODE
    /* Added a limiting for the jitter amplitude index, per freq_idx */
    uint8_t max_amp_idx_r20_os1[] = {37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 48, 33, 47, 37, 33, 37, 37};

    /* Irrespective of the osr_mode, txpi runs @ os1. Thus the max amp idx values remain the same. */
    if (jit_type == TX_PI_SJ) {
        if (tx_pi_jit_amp > max_amp_idx_r20_os1[tx_pi_jit_freq_idx]) {
            tx_pi_jit_amp = max_amp_idx_r20_os1[tx_pi_jit_freq_idx];
        }
    }

    EFUN(merlin7_tsc_tx_pi_freq_override(sa__, enable, freq_override_val));

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

err_code_t merlin7_tsc_isolate_ctrl_pins(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    uint8_t lane, lane_orig, num_lanes;
        EFUN(merlin7_tsc_isolate_core_ctrl_pins(sa__, enable));

    ESTM(lane_orig = merlin7_tsc_get_lane(sa__));
    /* read num lanes per core directly from register */
    ESTM(num_lanes = rdc_revid_multiplicity());
    for(lane = 0; lane < num_lanes; lane++) {
        EFUN(merlin7_tsc_set_lane(sa__, lane));
        EFUN(merlin7_tsc_isolate_lane_ctrl_pins(sa__, enable));
    }
    EFUN(merlin7_tsc_set_lane(sa__, lane_orig));

  return (ERR_CODE_NONE);
}

err_code_t merlin7_tsc_isolate_lane_ctrl_pins(srds_access_t *sa__, uint8_t enable) {
  INIT_SRDS_ERR_CODE

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

err_code_t merlin7_tsc_isolate_core_ctrl_pins(srds_access_t *sa__, uint8_t enable) {
  INIT_SRDS_ERR_CODE

  if (enable) {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x1));
  }
  else {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x0));
  }
  return (ERR_CODE_NONE);
}




err_code_t merlin7_tsc_log_full_pmd_state_noPRBS (srds_access_t *sa__, struct merlin7_tsc_detailed_lane_status_st *lane_st) {
    INIT_SRDS_ERR_CODE
    uint8_t  tmp1;
    const uint8_t big_endian = merlin7_tsc_INTERNAL_is_big_endian();

    if(!lane_st)
      return(merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    ESTM(lane_st->pmd_lock = rd_pmd_rx_lock());
    {
        err_code_t err_code=ERR_CODE_NONE;
        lane_st->stop_state = merlin7_tsc_INTERNAL_stop_micro(sa__,lane_st->pmd_lock,&err_code);
        if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
    }

    ESTM(lane_st->reset_state = rd_lane_dp_reset_state());
    /*populate the big endian info */
    lane_st->big_endian = big_endian;
    {
        struct merlin7_tsc_uc_lane_config_st lane_cfg;
        EFUN(merlin7_tsc_get_uc_lane_cfg(sa__, &lane_cfg));
        lane_st->lane_config_word = lane_cfg.word;
    }
    ESTM(lane_st->ucv_status = rdv_status_byte());
    ESTM(lane_st->reset_state    = rd_rx_lane_dp_reset_state());
    ESTM(lane_st->tx_reset_state = rd_tx_lane_dp_reset_state());
    ESTM(lane_st->clk90 = rd_cnt_d_minus_m1());
    ESTM(lane_st->clkp1 = rd_cnt_d_minus_p1());
    /* drop the MSB, the result is actually valid modulo 128 */
    /* Also flip the sign to account for d-m1, versus m1-d */
    lane_st->clk90 = (int8_t)(lane_st->clk90<<1);
    lane_st->clk90 = (int8_t)(-(lane_st->clk90>>1));
    lane_st->clkp1 = (int8_t)(lane_st->clkp1<<1);
    lane_st->clkp1 = (int8_t)(-(lane_st->clkp1>>1));
    ESTM(lane_st->pf_hiz = rd_pf_hiz());
    EFUN(merlin7_tsc_read_rx_afe(sa__, RX_AFE_PF2, &lane_st->pf2_ctrl));
    EFUN(merlin7_tsc_read_rx_afe(sa__, RX_AFE_VGA, &lane_st->vga));
    ESTM(lane_st->dc_offset = rd_dc_offset_bin());
    {
        uint8_t ladder_range = 0;
        ESTM(ladder_range = rd_p1_thresh_sel());
        EFUN(merlin7_tsc_INTERNAL_get_p1_threshold(sa__, &lane_st->p1_lvl_ctrl));
        EFUN(merlin7_tsc_INTERNAL_ladder_setting_to_mV(sa__, lane_st->p1_lvl_ctrl, ladder_range, INVALID_SLICER, &lane_st->p1_lvl));
    }
    lane_st->m1_lvl = 0;


    EFUN(merlin7_tsc_INTERNAL_get_rx_dfe1(sa__, &lane_st->dfe1));
    EFUN(merlin7_tsc_INTERNAL_get_rx_dfe2(sa__, &lane_st->dfe2));
    EFUN(merlin7_tsc_INTERNAL_get_rx_dfe3(sa__, &lane_st->dfe3));
    EFUN(merlin7_tsc_INTERNAL_get_rx_dfe4(sa__, &lane_st->dfe4));
    EFUN(merlin7_tsc_INTERNAL_get_rx_dfe5(sa__, &lane_st->dfe5));
    {
        int8_t dfe_1_e;
        ESTM(dfe_1_e = (int8_t)rd_dfe_1_e());
        ESTM(lane_st->dfe1_dcd = (int8_t)(dfe_1_e - rd_dfe_1_o()));
    }
    EFUN(merlin7_tsc_INTERNAL_get_dfe2_dcd(sa__, &lane_st->dfe2_dcd));
    ESTM(lane_st->pe = rd_p1_offset_evn_bin());
    ESTM(lane_st->ze = rd_data_offset_evn_bin());
    ESTM(lane_st->me = rd_m1_offset_evn_bin());

    ESTM(lane_st->po = rd_p1_offset_odd_bin());
    ESTM(lane_st->zo = rd_data_offset_odd_bin());
    ESTM(lane_st->mo = rd_m1_offset_odd_bin());

    /* tx_ppm = register/10.84 */
    ESTM(lane_st->tx_ppm = (int16_t)(((int32_t)(rd_tx_pi_integ2_reg()))*3125/32768));
    EFUN(merlin7_tsc_INTERNAL_get_tx_pre(sa__,   &lane_st->txfir_pre));
    EFUN(merlin7_tsc_INTERNAL_get_tx_main(sa__,  &lane_st->txfir_main));
    EFUN(merlin7_tsc_INTERNAL_get_tx_post1(sa__, &lane_st->txfir_post1));
    EFUN(merlin7_tsc_INTERNAL_get_tx_post2(sa__, &lane_st->txfir_post2));

    EFUN(merlin7_tsc_INTERNAL_get_eye_margin_est(sa__, &lane_st->heye_left, &lane_st->heye_right, &lane_st->veye_upper, &lane_st->veye_lower));

    ESTM(lane_st->soc_pos = rd_ams_rx_sigdet_offset_correction_pos());
    ESTM(lane_st->soc_neg = rd_ams_rx_sigdet_offset_correction_neg());

    ESTM(lane_st->restart_count = rdv_usr_sts_restart_counter());
    ESTM(lane_st->reset_count = rdv_usr_sts_reset_counter());
    ESTM(lane_st->pmd_lock_count = rdv_usr_sts_pmd_lock_counter());

    ESTM(lane_st->temp_idx = rdcv_temp_idx());

    ESTM(lane_st->sigdet = rd_signal_detect());
    ESTM(lane_st->dsc_sm[0] = rd_dsc_state_one_hot());
    ESTM(lane_st->dsc_sm[1] = rd_dsc_state_one_hot());
    ESTM(lane_st->ppm = rd_cdr_integ_reg());
    EFUN(merlin7_tsc_INTERNAL_get_rx_vga(sa__, &tmp1));
    lane_st->vga = (int8_t)tmp1;
    EFUN(merlin7_tsc_INTERNAL_get_rx_pf_main(sa__, &tmp1)); lane_st->pf = tmp1;
    EFUN(merlin7_tsc_INTERNAL_get_rx_pf2(sa__, &tmp1)); lane_st->pf2 = tmp1;
    

    EFUN(merlin7_tsc_INTERNAL_get_eye_margin_est(sa__, &lane_st->heye_left, &lane_st->heye_right, &lane_st->veye_upper, &lane_st->veye_lower));
    ESTM(lane_st->link_time = (uint16_t)((((uint32_t)rdv_usr_sts_link_time())*8)/10));

    if (lane_st->pmd_lock == 1) {
      if (!lane_st->stop_state) {
        EFUN(merlin7_tsc_stop_rx_adaptation(sa__, 0));
      }
    } else {
        EFUN(merlin7_tsc_stop_rx_adaptation(sa__, 0));
    }

    return(ERR_CODE_NONE);
}

err_code_t merlin7_tsc_log_full_pmd_state (srds_access_t *sa__, struct merlin7_tsc_detailed_lane_status_st *lane_st) {
    INIT_SRDS_ERR_CODE
    enum srds_prbs_polynomial_enum prbs_poly_mode = PRBS_7;
    enum srds_prbs_checker_mode_enum prbs_checker_mode = PRBS_SELF_SYNC_HYSTERESIS;

    if(!lane_st) {
      return(merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    ENULL_MEMSET(lane_st, 0, sizeof(struct merlin7_tsc_detailed_lane_status_st));
    EFUN(merlin7_tsc_log_full_pmd_state_noPRBS(sa__, lane_st));

    ESTM(lane_st->prbs_chk_en = rd_prbs_chk_en());
    EFUN(merlin7_tsc_get_rx_prbs_config(sa__, &prbs_poly_mode, &prbs_checker_mode, &lane_st->prbs_chk_inv));
    lane_st->prbs_chk_order = (uint8_t)prbs_poly_mode;

    EFUN(merlin7_tsc_prbs_chk_lock_state(sa__, &lane_st->prbs_chk_lock));
    EFUN(merlin7_tsc_prbs_err_count_ll(sa__, &lane_st->prbs_chk_errcnt));
    EFUN(merlin7_tsc_INTERNAL_get_BER_string(sa__,100,&lane_st->ber_string[0], MEMBER_SIZE(struct merlin7_tsc_detailed_lane_status_st, ber_string)));

    return(ERR_CODE_NONE);
}


err_code_t merlin7_tsc_disp_full_pmd_state (srds_access_t *sa__, struct merlin7_tsc_detailed_lane_status_st const * const lane_st, uint8_t num_lanes) {
    INIT_SRDS_ERR_CODE
    const uint8_t num_bytes_each_line = 26;
    uint32_t i;
    uint32_t size_of_lane_st = 0;
    const uint8_t big_endian = merlin7_tsc_INTERNAL_is_big_endian();

    if(lane_st == NULL) {
        return (merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if((num_lanes != 4) && (num_lanes != 8) && (num_lanes != 1)) {
       return(merlin7_tsc_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT)); /* Number of lanes has to be 1, 4 or 8 */
    }

    size_of_lane_st = sizeof(struct merlin7_tsc_detailed_lane_status_st);

    EFUN_PRINTF(("\n**** SERDES EXTENDED LANE DIAG DATA CORE %d LANE %d DUMP ****", merlin7_tsc_get_core(sa__), merlin7_tsc_get_lane(sa__)));
    EFUN_PRINTF(("\n0000 "));
    ESTM_PRINTF(("%02x ", num_lanes));
    ESTM_PRINTF(("%02x ", big_endian));
    ESTM_PRINTF(("%02x ", (uint8_t)sizeof(struct check_platform_info_st)));
    /* For some customer builds, ESTM_PRINTF is an empty macro.
       In such cases, some compilers might generates an error of "unused variable" for big_endian.
       The void usage of big_endian down below - solves this compiler error */
    UNUSED(big_endian);
    for (i = 3; i < num_lanes*size_of_lane_st+3; i++) {
      if (!(i % num_bytes_each_line))  {
        EFUN_PRINTF(("\n%04x ", i));
      }
      ESTM_PRINTF(("%02x ", *(((uint8_t*)lane_st)+i-3)));
    }

    EFUN_PRINTF(("\n**** END OF DATA DUMP ****\n"));

    EFUN_PRINTF(("\n========== End of SERDES EXTENDED LANE DIAG DATA ==================\n"));

    return (ERR_CODE_NONE);
}

#endif /* SMALL_FOOTPRINT */


/*
 * for backtrace() support
 */


void merlin7_tsc_INTERNAL_print_triage_info(srds_access_t *sa__, err_code_t err_code, uint8_t print_header, uint8_t print_data, uint16_t line)
{
    /*  Note: No EFUNs or ESTMs should be used in this function as this print routine is called by _error() handler. */
#if defined(SMALL_FOOTPRINT)
    return;
#else
    merlin7_tsc_triage_info info;
    err_code_t error;
    INIT_SRDS_ERR_CODE
    uint16_t   ucode_version_major;
    uint8_t    ucode_version_minor, error_seen = 0;

    USR_MEMSET(&info, 0, sizeof(struct merlin7_tsc_triage_info_st));
    info.error = err_code;
    info.line = line;
    if (print_header) {
        USR_PRINTF(("Triage Info Below:\n"));
        if ((err_code == ERR_CODE_UC_CMD_POLLING_TIMEOUT) || (err_code == ERR_CODE_UC_NOT_STOPPED)) {
            USR_PRINTF(("Lane, Core,  API_VER, UCODE_VER, micro_stop_status, exception(sw,hw), stack_ovflw, cmd_info, pmd_lock, sigdet, dsc_one_hot(0,1), Error\n"));
        } else {
            USR_PRINTF(("Lane, Core,  API_VER, UCODE_VER, Error\n"));
        }
    }
    error = merlin7_tsc_version(sa__, &info.api_ver);
    if (error) {
        /* Unable to read api version */
        /*USR_PRINTF(("Unable to capture api version and so setting invalid value in the print\n"));*/
        info.api_ver = 0xFFFFFFFF;
        error_seen = 1;
    }
    CHECK_ERR(ucode_version_major = rdcv_common_ucode_version());
    CHECK_ERR(ucode_version_minor = rdcv_common_ucode_minor_version());
    info.ucode_ver = (uint32_t)((ucode_version_major << 8) | ucode_version_minor);
    CHECK_ERR(info.stop_status = rdv_usr_sts_micro_stopped());

    /* Collect exception and overflow information */
    CHECK_ERR(info.stack_overflow = rdcv_status_byte());
    if(merlin7_tsc_INTERNAL_sigdet_status(sa__, &info.sig_det, &info.sig_det_chg)) {
        error_seen = 1;
    }
    if (merlin7_tsc_INTERNAL_pmd_lock_status(sa__, &info.pmd_lock, &info.pmd_lock_chg)) {
        error_seen = 1;
    }

    CHECK_ERR(info.dsc_one_hot[0] = rd_dsc_state_one_hot());
    CHECK_ERR(info.dsc_one_hot[1] = rd_dsc_state_one_hot());
    CHECK_ERR(info.cmd_info = reg_rd_DSC_A_DSC_UC_CTRL());
    info.core = merlin7_tsc_get_core(sa__);
    info.lane = merlin7_tsc_get_lane(sa__);
    if (print_data) {
        if ((err_code == ERR_CODE_UC_CMD_POLLING_TIMEOUT) || (err_code == ERR_CODE_UC_NOT_STOPPED)) {
            USR_PRINTF(("%4d, %4d,  %X_%X,    %X_%X, %17d, %7d,%d       , %11d,   0x%04x, %8d, %6d,    0x%x,0x%x   , %s\n",
                        info.lane,
                        info.core,
                        info.api_ver>>8,
                        info.api_ver & 0xFF,
                        info.ucode_ver >> 8,
                        info.ucode_ver & 0xFF,
                        info.stop_status,
                        info.sw_exception,
                        info.hw_exception,
                        info.stack_overflow,
                        info.cmd_info,
                        info.pmd_lock,
                        info.sig_det,
                        info.dsc_one_hot[0],
                        info.dsc_one_hot[1],
                        merlin7_tsc_INTERNAL_e2s_err_code(info.error) ));
        } else {
            USR_PRINTF(("%4d, %4d,  %X_%X,     %X_%X, %s\n",
                        info.lane,
                        info.core,
                        info.api_ver>>8,
                        info.api_ver & 0xFF,
                        info.ucode_ver >> 8,
                        info.ucode_ver & 0xFF,
                        merlin7_tsc_INTERNAL_e2s_err_code(info.error) ));
        }
    }
    if (error_seen) {
        USR_PRINTF(("WARNING: There were some errors seen while collecting triage info and so the debug data above may not be all accurate\n"));
    }
    return;
#endif /* SMALL_FOOTPRINT */
}
