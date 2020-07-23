/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  falcon16_v1l4p1_debug_functions.c                                  *
 *  Created On    :  03 Nov 2015                                                   *
 *  Created By    :  Brent Roberts                                                 *
 *  Description   :  APIs for Serdes IPs                                           *
 *  Revision      :   *
 */

/** @file falcon16_v1l4p1_debug_functions.c
 * Implementation of API debug functions
 */

#include <phymod/phymod.h>
#include "../include/falcon16_v1l4p1_debug_functions.h"
#include "../include/falcon16_v1l4p1_access.h"
#include "../include/falcon16_v1l4p1_common.h"
#include "../include/falcon16_v1l4p1_config.h"
#include "../include/falcon16_v1l4p1_functions.h"
#include "../include/falcon16_v1l4p1_internal.h"
#include "../include/falcon16_v1l4p1_internal_error.h"
#include "../include/falcon16_v1l4p1_prbs.h"
#include "../include/falcon16_v1l4p1_select_defns.h"
#include "../include/falcon16_v1l4p1_reg_dump.h"


static err_code_t _falcon16_v1l4p1_reg_print_no_buf(srds_access_t *sa__, uint8_t reg_section);
static err_code_t _falcon16_v1l4p1_reg_print_with_buf(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][250], uint8_t *buf_index);
static err_code_t _falcon16_v1l4p1_reg_print(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][250], uint8_t *buf_index);


/*************************/
/*  Stop/Resume uC Lane  */
/*************************/

err_code_t falcon16_v1l4p1_stop_uc_lane(srds_access_t *sa__, uint8_t enable) {

    return(falcon16_v1l4p1_stop_rx_adaptation(sa__,enable));
}


err_code_t falcon16_v1l4p1_stop_uc_lane_status(srds_access_t *sa__, uint8_t *uc_lane_stopped) {

  if(!uc_lane_stopped) {
      return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*uc_lane_stopped = rdv_usr_sts_micro_stopped());

  return (ERR_CODE_NONE);
}

/*******************************************************************/
/*  APIs to Write Core/Lane Config and User variables into uC RAM  */
/*******************************************************************/

err_code_t falcon16_v1l4p1_set_usr_ctrl_core_event_log_level(srds_access_t *sa__, uint8_t core_event_log_level) {
  return(wrcv_usr_ctrl_core_event_log_level(core_event_log_level));
}

err_code_t falcon16_v1l4p1_set_usr_ctrl_lane_event_log_level(srds_access_t *sa__, uint8_t lane_event_log_level) {
  return(wrv_usr_ctrl_lane_event_log_level(lane_event_log_level));
}

err_code_t falcon16_v1l4p1_set_usr_ctrl_disable_startup(srds_access_t *sa__, struct falcon16_v1l4p1_usr_ctrl_disable_functions_st set_val) {
  EFUN(falcon16_v1l4p1_INTERNAL_update_usr_ctrl_disable_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_startup_functions_word(set_val.word));
}

err_code_t falcon16_v1l4p1_set_usr_ctrl_disable_startup_dfe(srds_access_t *sa__, struct falcon16_v1l4p1_usr_ctrl_disable_dfe_functions_st set_val) {
  EFUN(falcon16_v1l4p1_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_startup_dfe_functions_byte(set_val.byte));
}

err_code_t falcon16_v1l4p1_set_usr_ctrl_disable_steady_state(srds_access_t *sa__, struct falcon16_v1l4p1_usr_ctrl_disable_functions_st set_val) {
  EFUN(falcon16_v1l4p1_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */
  EFUN(falcon16_v1l4p1_INTERNAL_update_usr_ctrl_disable_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_steady_state_functions_word(set_val.word));
}

err_code_t falcon16_v1l4p1_set_usr_ctrl_disable_steady_state_dfe(srds_access_t *sa__, struct falcon16_v1l4p1_usr_ctrl_disable_dfe_functions_st set_val) {
  EFUN(falcon16_v1l4p1_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_steady_state_dfe_functions_byte(set_val.byte));
}

/******************************************************************/
/*  APIs to Read Core/Lane Config and User variables from uC RAM  */
/******************************************************************/

err_code_t falcon16_v1l4p1_get_usr_ctrl_core_event_log_level(srds_access_t *sa__, uint8_t *core_event_log_level) {

  if(!core_event_log_level) {
     return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*core_event_log_level = rdcv_usr_ctrl_core_event_log_level());

  return (ERR_CODE_NONE);
}

err_code_t falcon16_v1l4p1_get_usr_ctrl_lane_event_log_level(srds_access_t *sa__, uint8_t *lane_event_log_level) {

  if(!lane_event_log_level) {
     return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*lane_event_log_level = rdv_usr_ctrl_lane_event_log_level());
  return (ERR_CODE_NONE);
}

err_code_t falcon16_v1l4p1_get_usr_ctrl_disable_startup(srds_access_t *sa__, struct falcon16_v1l4p1_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
     return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdv_usr_ctrl_disable_startup_functions_word());
  EFUN(falcon16_v1l4p1_INTERNAL_update_usr_ctrl_disable_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t falcon16_v1l4p1_get_usr_ctrl_disable_startup_dfe(srds_access_t *sa__, struct falcon16_v1l4p1_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
     return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_startup_dfe_functions_byte());
  EFUN(falcon16_v1l4p1_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t falcon16_v1l4p1_get_usr_ctrl_disable_steady_state(srds_access_t *sa__, struct falcon16_v1l4p1_usr_ctrl_disable_functions_st *get_val) {

  if(!get_val) {
     return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdv_usr_ctrl_disable_steady_state_functions_word());
  EFUN(falcon16_v1l4p1_INTERNAL_update_usr_ctrl_disable_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t falcon16_v1l4p1_get_usr_ctrl_disable_steady_state_dfe(srds_access_t *sa__, struct falcon16_v1l4p1_usr_ctrl_disable_dfe_functions_st *get_val) {

  if(!get_val) {
     return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_steady_state_dfe_functions_byte());
  EFUN(falcon16_v1l4p1_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(get_val));
  return (ERR_CODE_NONE);
}

/********************************************************************************/
/* Helper print function for Serdes Register/Variable Dump using no buffer      */
/********************************************************************************/
static err_code_t _falcon16_v1l4p1_reg_print_no_buf(srds_access_t *sa__, uint8_t reg_section)
{
    uint16_t addr, rddata = 0;

    for (addr = falcon16_v1l4p1_reg_dump_arr[reg_section][0]; addr < falcon16_v1l4p1_reg_dump_arr[reg_section][1]; addr++) 
    {
        if (!(addr & 0x000F))  
        {
            EFUN_PRINTF(("\n%04x ",addr));
        }
        EFUN(falcon16_v1l4p1_pmd_rdt_reg(sa__, addr,&rddata));
        EFUN_PRINTF(("%04x ",rddata));
    }
    return (ERR_CODE_NONE);
}
/********************************************************************************/
/* Helper print function for Serdes Register/Variable Dump using a buffer       */
/********************************************************************************/
static err_code_t _falcon16_v1l4p1_reg_print_with_buf(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][250], uint8_t *buf_index)
{
    uint16_t addr, rddata = 0;
	uint8_t first = 0;
    char temp_buf[16];
	
	if(reg_section == 0) first = 1;

    for (addr = falcon16_v1l4p1_reg_dump_arr[reg_section][0]; addr < falcon16_v1l4p1_reg_dump_arr[reg_section][1]; addr++) 
    {
        if (!(addr & 0x000F))  
        {
            if(first) first = 0;
            else (*buf_index)++; /* don't increment buf_index for first section */
            USR_SPRINTF(temp_buf,"%04x ", addr);
            USR_STRCPY(reg_buffer[*buf_index], temp_buf);
        }
        EFUN(falcon16_v1l4p1_pmd_rdt_reg(sa__, addr,&rddata));
        USR_SPRINTF(temp_buf,"%04x ", rddata);
        USR_STRNCAT(reg_buffer[*buf_index], temp_buf, 6);
    }
    return (ERR_CODE_NONE);
}
/********************************************************************************/
/* Helper print function for Serdes Register/Variable Dump                      */
/********************************************************************************/
static err_code_t _falcon16_v1l4p1_reg_print(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][250], uint8_t *buf_index)
{
    if(reg_buffer == NULL)
    {
         EFUN(_falcon16_v1l4p1_reg_print_no_buf(sa__, reg_section));
    }
    else
    {
        EFUN(_falcon16_v1l4p1_reg_print_with_buf(sa__, reg_section, reg_buffer, buf_index));
    }
    return (ERR_CODE_NONE);
}
err_code_t falcon16_v1l4p1_reg_dump(srds_access_t *sa__, uint8_t core, uint8_t lane, char reg_buffer[][250])
{    
    uint8_t reg_section = 0;
    uint8_t buf_index = 0;

    if (reg_buffer == NULL) 
    {
        EFUN_PRINTF(("\n****  SERDES REGISTER CORE %d LANE %d DUMP    ****", core, lane));
    }
    else
    {
        EFUN_PRINTF(("\n****  SERDES REGISTER CORE %d LANE %d DECODED    ****\n", core, lane));
    }
    
    while(reg_section < FALCON16_V1L4P1_REG_DUMP_SECTIONS) 
    {
        if(falcon16_v1l4p1_reg_dump_arr[reg_section][0] == FALCON16_V1L4P1_REG_PLL_START)
        {
                while( reg_section < FALCON16_V1L4P1_REG_DUMP_SECTIONS && falcon16_v1l4p1_reg_dump_arr[reg_section][0] != FALCON16_V1L4P1_REG_UC_START)
                {
                    EFUN(_falcon16_v1l4p1_reg_print(sa__, reg_section, reg_buffer, &buf_index));
                    reg_section++;
                }
        }
        else if (falcon16_v1l4p1_reg_dump_arr[reg_section][0] == FALCON16_V1L4P1_REG_UC_START)
        {
#ifdef SERDES_MULTI_MICROS
            uint8_t micro_orig, micro_idx, num_micros;
            uint8_t uc_start_section = reg_section;

            ESTM(num_micros = rdc_micro_num_uc_cores());
            ESTM(micro_orig = falcon16_v1l4p1_get_micro_idx(sa__));
            for(micro_idx = 0; micro_idx < num_micros; micro_idx++)
            {
                reg_section = uc_start_section;
                EFUN(falcon16_v1l4p1_set_micro_idx(sa__,micro_idx));
#endif
                while(reg_section < FALCON16_V1L4P1_REG_DUMP_SECTIONS)
                {
                    EFUN(_falcon16_v1l4p1_reg_print(sa__, reg_section, reg_buffer, &buf_index));
                    reg_section++;
                }
#ifdef SERDES_MULTI_MICROS
            }
            EFUN(falcon16_v1l4p1_set_micro_idx(sa__,micro_orig));
#endif
        }
        else
        {
            EFUN(_falcon16_v1l4p1_reg_print(sa__, reg_section, reg_buffer, &buf_index));
            reg_section++;
        }
    } 

    if (reg_buffer != NULL)
    {
        reg_buffer[++buf_index][0] = 0xA;
        reg_buffer[++buf_index][0] = 0;
    }

    return (ERR_CODE_NONE);
}


err_code_t falcon16_v1l4p1_uc_core_var_dump(srds_access_t *sa__, uint8_t core, char ram_buffer[][250]) {
  uint8_t  addr, rddata, core_var_ram_size;

    core_var_ram_size = 0xFF;
    
  if (ram_buffer == NULL) {
      EFUN_PRINTF(("\n**** SERDES UC CORE %d RAM VARIABLE DUMP ****", core));
      for (addr = 0x0; addr < core_var_ram_size; addr++) {
          if (!(addr % 26))  {
              EFUN_PRINTF(("\n%04x ",addr));
          }
          ESTM_PRINTF(("%02x ", falcon16_v1l4p1_rdbc_uc_var(sa__, __ERR, addr)));
      }
  }
  else {
      uint8_t index=0;
      char tempBuf[16];

      EFUN_PRINTF(("\n**** SERDES UC CORE %d RAM VARIABLE DECODED ****\n", core));
      for (addr = 0x0; addr < core_var_ram_size; addr++) {
          if (!(addr % 26))  {
             if (addr != 0) {
                index++;
             }
             USR_SPRINTF(tempBuf,"%04x ", addr);
             USR_STRCPY(ram_buffer[index], tempBuf);
          }
          ESTM(rddata = falcon16_v1l4p1_rdbc_uc_var(sa__, __ERR, addr));
          USR_SPRINTF(tempBuf,"%02x ", rddata);
          USR_STRNCAT(ram_buffer[index], tempBuf, 6);
      }
      ram_buffer[index+1][0] = 0xA;
      ram_buffer[index+2][0] = 0;
  }
  return (ERR_CODE_NONE);
}


err_code_t falcon16_v1l4p1_uc_lane_var_dump(srds_access_t *sa__, uint8_t core, uint8_t lane, char ram_buffer[][250]) {
  uint8_t     rx_lock, uc_stopped = 0;
  uint16_t    addr, lane_var_ram_size;

  lane_var_ram_size = LANE_VAR_RAM_SIZE;

  ESTM(rx_lock = rd_pmd_rx_lock());

  {
      err_code_t err_code=ERR_CODE_NONE;
      uc_stopped = falcon16_v1l4p1_INTERNAL_stop_micro(sa__,rx_lock,&err_code);
      if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
  }

  if (ram_buffer == NULL) {
      EFUN_PRINTF(("\n**** SERDES UC CORE %d LANE %d RAM VARIABLE DUMP ****", core,lane));
      for (addr = 0x0; addr < lane_var_ram_size; addr++) {
         if (!(addr % 26))  {
            EFUN_PRINTF(("\n%04x ",addr));
         }
         ESTM_PRINTF(("%02x ", falcon16_v1l4p1_rdbl_uc_var(sa__, __ERR, addr)));
      }
  }
  else {
      uint8_t rddata, index = 0;
      char tempBuf[16];
      EFUN_PRINTF(("\n**** SERDES UC CORE %d LANE %d RAM VARIABLE DECODED ****\n", core,lane));

      for (addr = 0x0; addr < lane_var_ram_size; addr++) {
         if (!(addr % 26))  {
            if (addr != 0) {
               index++;
            }
            USR_SPRINTF(tempBuf,"%04x ", addr);
            USR_STRCPY(ram_buffer[index], tempBuf);
         }
         ESTM(rddata = falcon16_v1l4p1_rdbl_uc_var(sa__, __ERR, addr));
         USR_SPRINTF(tempBuf,"%02x ", rddata);
         USR_STRNCAT(ram_buffer[index], tempBuf, 6);
      }
      ram_buffer[index+1][0] = 0xA;
      ram_buffer[index+2][0] = 0;
  }

  if (rx_lock == 1) {
      if (!uc_stopped) {
          EFUN(falcon16_v1l4p1_stop_rx_adaptation(sa__, 0));
      }
  } else {
      EFUN(falcon16_v1l4p1_stop_rx_adaptation(sa__, 0));
  }

  return (ERR_CODE_NONE);
}

/***************************************/
/*  API Function to Read Event Logger  */
/***************************************/

err_code_t falcon16_v1l4p1_read_event_log(srds_access_t *sa__) {
    falcon16_v1l4p1_INTERNAL_event_log_dump_state_t state;
    uint8_t micro_num = 0;
    {
        state.index = 0;
        state.line_start_index = 0;
        EFUN(falcon16_v1l4p1_INTERNAL_read_event_log_with_callback(sa__, micro_num, 0, &state, falcon16_v1l4p1_INTERNAL_event_log_dump_callback));
        EFUN(falcon16_v1l4p1_INTERNAL_event_log_dump_callback(&state, 0, 0));
    }
    return(ERR_CODE_NONE);
}

/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/

/* Enable/Disable Digital Loopback */
err_code_t falcon16_v1l4p1_dig_lpbk(srds_access_t *sa__, uint8_t enable) {
    /* setting/clearing prbs_chk_en_auto_mode while coming out of/going in to dig lpbk */
    EFUN(wr_prbs_chk_en_auto_mode(!enable));
    EFUN(wr_dig_lpbk_en(enable));                         /* 0 = disabled, 1 = enabled */

    /* Disabling Link Training as it's an invalid mode in digital loopback */
    if(enable) {
        uint8_t link_training_enable = 0;
        ESTM(link_training_enable = rd_cl93n72_ieee_training_enable());
        if(link_training_enable) {
            EFUN_PRINTF(("Warning: Core: %d, Lane %d: Link Training mode is on in digital loopback.\n", falcon16_v1l4p1_get_core(sa__), falcon16_v1l4p1_get_lane(sa__)));
        }
    }
  return (ERR_CODE_NONE);
}


/**********************************/
/*  TX_PI Jitter Generation APIs  */
/**********************************/

/* TX_PI Sinusoidal or Spread-Spectrum (SSC) Jitter Generation  */
err_code_t falcon16_v1l4p1_tx_pi_jitt_gen(srds_access_t *sa__, uint8_t enable, int16_t freq_override_val, enum srds_tx_pi_freq_jit_gen_enum jit_type, uint8_t tx_pi_jit_freq_idx, uint8_t tx_pi_jit_amp) {
    /* Added a limiting for the jitter amplitude index, per freq_idx */
    uint8_t max_amp_idx_r20_os1[] = {37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 48, 33, 47, 37, 33, 37, 37};

    /* Irrespective of the osr_mode, txpi runs @ os1. Thus the max amp idx values remain the same. */
    if (jit_type == TX_PI_SJ) {
        if (tx_pi_jit_amp > max_amp_idx_r20_os1[tx_pi_jit_freq_idx]) {
            tx_pi_jit_amp = max_amp_idx_r20_os1[tx_pi_jit_freq_idx];
        }
    }

    EFUN(falcon16_v1l4p1_tx_pi_freq_override(sa__, enable, freq_override_val));

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

err_code_t falcon16_v1l4p1_isolate_ctrl_pins(srds_access_t *sa__, uint8_t enable) {
    uint8_t lane, lane_orig, num_lanes;
        EFUN(falcon16_v1l4p1_isolate_core_ctrl_pins(sa__, enable));

    ESTM(lane_orig = falcon16_v1l4p1_get_lane(sa__));
    /* read num lanes per core directly from register */
    ESTM(num_lanes = rdc_revid_multiplicity());
    for(lane = 0; lane < num_lanes; lane++) {
        EFUN(falcon16_v1l4p1_set_lane(sa__, lane));
        EFUN(falcon16_v1l4p1_isolate_lane_ctrl_pins(sa__, enable));
    }
    EFUN(falcon16_v1l4p1_set_lane(sa__, lane_orig));

  return (ERR_CODE_NONE);
}

err_code_t falcon16_v1l4p1_isolate_lane_ctrl_pins(srds_access_t *sa__, uint8_t enable) {

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

err_code_t falcon16_v1l4p1_isolate_core_ctrl_pins(srds_access_t *sa__, uint8_t enable) {

  if (enable) {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x1));
  }
  else {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x0));
  }
  return (ERR_CODE_NONE);
}



err_code_t falcon16_v1l4p1_log_full_pmd_state_noPRBS (srds_access_t *sa__, struct falcon16_v1l4p1_detailed_lane_status_st *lane_st) {
    uint16_t reg_data, reg_data1, reg_data2, reg_data3, reg_data4;
    int8_t   tmp;
    uint8_t  tmp1;
    const uint8_t big_endian = falcon16_v1l4p1_INTERNAL_is_big_endian();

    if(!lane_st)
      return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    ESTM(lane_st->pmd_lock = rd_pmd_rx_lock());
    {
        err_code_t err_code=ERR_CODE_NONE;
        lane_st->stop_state = falcon16_v1l4p1_INTERNAL_stop_micro(sa__,lane_st->pmd_lock,&err_code);
        if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
    }

    ESTM(lane_st->reset_state = rd_lane_dp_reset_state());
    /*populate the big endian info */
    lane_st->big_endian = big_endian;



    ESTM(lane_st->restart_count = rdv_usr_sts_restart_counter());
    ESTM(lane_st->reset_count = rdv_usr_sts_reset_counter());
    ESTM(lane_st->pmd_lock_count = rdv_usr_sts_pmd_lock_counter());

    ESTM(lane_st->temp_idx = rdcv_temp_idx());
    ESTM(lane_st->ams_tx_drv_hv_disable = rd_ams_tx_drv_hv_disable());
    ESTM(lane_st->ams_tx_ana_rescal = rd_ams_tx_ana_rescal());
    EFUN(falcon16_v1l4p1_read_tx_afe(sa__, TX_AFE_PRE, &tmp)); lane_st->pre_tap = (uint8_t)tmp;
    EFUN(falcon16_v1l4p1_read_tx_afe(sa__, TX_AFE_MAIN, &tmp)); lane_st->main_tap = (uint8_t)tmp;
    EFUN(falcon16_v1l4p1_read_tx_afe(sa__, TX_AFE_POST1, &lane_st->post1_tap));
    EFUN(falcon16_v1l4p1_read_tx_afe(sa__, TX_AFE_POST2, &lane_st->post2_tap));
    EFUN(falcon16_v1l4p1_read_tx_afe(sa__, TX_AFE_POST3, &lane_st->post3_tap));
    EFUN(falcon16_v1l4p1_read_tx_afe(sa__, TX_AFE_RPARA, &tmp)); lane_st->rpara = (uint8_t)tmp;
    ESTM(lane_st->sigdet = rd_signal_detect());
    ESTM(lane_st->dsc_sm[0] = rd_dsc_state_one_hot());
    ESTM(lane_st->dsc_sm[1] = rd_dsc_state_one_hot());
    ESTM(lane_st->ppm = rd_cdr_integ_reg());
    ESTM(falcon16_v1l4p1_INTERNAL_get_rx_vga(sa__, &tmp1)); lane_st->vga = tmp1;
    EFUN(falcon16_v1l4p1_INTERNAL_get_rx_pf_main(sa__, &tmp1)); lane_st->pf = tmp1;
    EFUN(falcon16_v1l4p1_INTERNAL_get_rx_pf2(sa__, &tmp1)); lane_st->pf2 = tmp1;
    ESTM(lane_st->main_tap_est = rdv_usr_main_tap_est());
    EFUN(falcon16_v1l4p1_INTERNAL_get_rx_dfe1(sa__, &lane_st->data_thresh));
    ESTM(lane_st->phase_thresh = rd_rx_phase_thresh_sel());
    ESTM(lane_st->lms_thresh   = rd_rx_lms_thresh_sel());
    ESTM(reg_data = reg_rd_DSC_E_RX_PI_CNT_BIN_D());
    ESTM(reg_data1 = reg_rd_DSC_E_RX_PI_CNT_BIN_P());
    ESTM(reg_data2 = reg_rd_DSC_E_RX_PI_CNT_BIN_L());
    ESTM(reg_data3 = reg_rd_DSC_E_RX_PI_CNT_BIN_PD());
    ESTM(reg_data4 = reg_rd_DSC_E_RX_PI_CNT_BIN_LD());
    lane_st->ddq_hoffset = (uint8_t)dist_ccw(((reg_data>>8)&0xFF),(reg_data&0xFF));
    lane_st->ppq_hoffset = (uint8_t)dist_ccw(((reg_data1>>8)&0xFF),(reg_data1&0xFF));
    lane_st->llq_hoffset = (uint8_t)dist_ccw(((reg_data2>>8)&0xFF),(reg_data2&0xFF));
    lane_st->dp_hoffset = (uint8_t)dist_cw(((reg_data3>>8)&0xFF),(reg_data3&0xFF));
    lane_st->dl_hoffset = (uint8_t)dist_cw(((reg_data4>>8)&0xFF),(reg_data4&0xFF));
    ESTM(lane_st->dc_offset = rd_dc_offset_bin());
    ESTM(lane_st->dfe[1][0] = rd_rxa_dfe_tap2());
    ESTM(lane_st->dfe[1][1] = rd_rxb_dfe_tap2());
    ESTM(lane_st->dfe[1][2] = rd_rxc_dfe_tap2());
    ESTM(lane_st->dfe[1][3] = rd_rxd_dfe_tap2());
    ESTM(lane_st->dfe[2][0] = rd_rxa_dfe_tap3());
    ESTM(lane_st->dfe[2][1] = rd_rxb_dfe_tap3());
    ESTM(lane_st->dfe[2][2] = rd_rxc_dfe_tap3());
    ESTM(lane_st->dfe[2][3] = rd_rxd_dfe_tap3());
    ESTM(lane_st->dfe[3][0] = rd_rxa_dfe_tap4());
    ESTM(lane_st->dfe[3][1] = rd_rxb_dfe_tap4());
    ESTM(lane_st->dfe[3][2] = rd_rxc_dfe_tap4());
    ESTM(lane_st->dfe[3][3] = rd_rxd_dfe_tap4());
    ESTM(lane_st->dfe[4][0] = rd_rxa_dfe_tap5());
    ESTM(lane_st->dfe[4][1] = rd_rxb_dfe_tap5());
    ESTM(lane_st->dfe[4][2] = rd_rxc_dfe_tap5());
    ESTM(lane_st->dfe[4][3] = rd_rxd_dfe_tap5());
    ESTM(lane_st->dfe[5][0] = rd_rxa_dfe_tap6());
    ESTM(lane_st->dfe[5][1] = rd_rxb_dfe_tap6());
    ESTM(lane_st->dfe[5][2] = rd_rxc_dfe_tap6());
    ESTM(lane_st->dfe[5][3] = rd_rxd_dfe_tap6());
    ESTM(lane_st->dfe[6][0] = ((rd_rxa_dfe_tap7_mux()==0)?rd_rxa_dfe_tap7():0));
    ESTM(lane_st->dfe[6][1] = ((rd_rxb_dfe_tap7_mux()==0)?rd_rxb_dfe_tap7():0));
    ESTM(lane_st->dfe[6][2] = ((rd_rxc_dfe_tap7_mux()==0)?rd_rxc_dfe_tap7():0));
    ESTM(lane_st->dfe[6][3] = ((rd_rxd_dfe_tap7_mux()==0)?rd_rxd_dfe_tap7():0));
    ESTM(lane_st->dfe[7][0] = ((rd_rxa_dfe_tap8_mux()==0)?rd_rxa_dfe_tap8():0));
    ESTM(lane_st->dfe[7][1] = ((rd_rxb_dfe_tap8_mux()==0)?rd_rxb_dfe_tap8():0));
    ESTM(lane_st->dfe[7][2] = ((rd_rxc_dfe_tap8_mux()==0)?rd_rxc_dfe_tap8():0));
    ESTM(lane_st->dfe[7][3] = ((rd_rxd_dfe_tap8_mux()==0)?rd_rxd_dfe_tap8():0));
    ESTM(lane_st->dfe[8][0] = ((rd_rxa_dfe_tap9_mux()==0)?rd_rxa_dfe_tap9():0));
    ESTM(lane_st->dfe[8][1] = ((rd_rxb_dfe_tap9_mux()==0)?rd_rxb_dfe_tap9():0));
    ESTM(lane_st->dfe[8][2] = ((rd_rxc_dfe_tap9_mux()==0)?rd_rxc_dfe_tap9():0));
    ESTM(lane_st->dfe[8][3] = ((rd_rxd_dfe_tap9_mux()==0)?rd_rxd_dfe_tap9():0));
    ESTM(lane_st->dfe[9][0] = ((rd_rxa_dfe_tap10_mux()==0)?rd_rxa_dfe_tap10():0));
    ESTM(lane_st->dfe[9][1] = ((rd_rxb_dfe_tap10_mux()==0)?rd_rxb_dfe_tap10():0));
    ESTM(lane_st->dfe[9][2] = ((rd_rxc_dfe_tap10_mux()==0)?rd_rxc_dfe_tap10():0));
    ESTM(lane_st->dfe[9][3] = ((rd_rxd_dfe_tap10_mux()==0)?rd_rxd_dfe_tap10():0));
    ESTM(lane_st->dfe[10][0] = ((rd_rxa_dfe_tap7_mux()==1)?rd_rxa_dfe_tap7():(rd_rxa_dfe_tap11_mux()==0)?rd_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[10][1] = ((rd_rxb_dfe_tap7_mux()==1)?rd_rxb_dfe_tap7():(rd_rxb_dfe_tap11_mux()==0)?rd_rxb_dfe_tap11():0));
    ESTM(lane_st->dfe[10][2] = ((rd_rxc_dfe_tap7_mux()==1)?rd_rxc_dfe_tap7():(rd_rxc_dfe_tap11_mux()==0)?rd_rxc_dfe_tap11():0));
    ESTM(lane_st->dfe[10][3] = ((rd_rxd_dfe_tap7_mux()==1)?rd_rxd_dfe_tap7():(rd_rxd_dfe_tap11_mux()==0)?rd_rxd_dfe_tap11():0));
    ESTM(lane_st->dfe[11][0] = ((rd_rxa_dfe_tap8_mux()==1)?rd_rxa_dfe_tap8():(rd_rxa_dfe_tap12_mux()==0)?rd_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[11][1] = ((rd_rxb_dfe_tap8_mux()==1)?rd_rxb_dfe_tap8():(rd_rxb_dfe_tap12_mux()==0)?rd_rxb_dfe_tap12():0));
    ESTM(lane_st->dfe[11][2] = ((rd_rxc_dfe_tap8_mux()==1)?rd_rxc_dfe_tap8():(rd_rxc_dfe_tap12_mux()==0)?rd_rxc_dfe_tap12():0));
    ESTM(lane_st->dfe[11][3] = ((rd_rxd_dfe_tap8_mux()==1)?rd_rxd_dfe_tap8():(rd_rxd_dfe_tap12_mux()==0)?rd_rxd_dfe_tap12():0));
    ESTM(lane_st->dfe[12][0] = ((rd_rxa_dfe_tap9_mux()==1)?rd_rxa_dfe_tap9():(rd_rxa_dfe_tap13_mux()==0)?rd_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[12][1] = ((rd_rxb_dfe_tap9_mux()==1)?rd_rxb_dfe_tap9():(rd_rxb_dfe_tap13_mux()==0)?rd_rxb_dfe_tap13():0));
    ESTM(lane_st->dfe[12][2] = ((rd_rxc_dfe_tap9_mux()==1)?rd_rxc_dfe_tap9():(rd_rxc_dfe_tap13_mux()==0)?rd_rxc_dfe_tap13():0));
    ESTM(lane_st->dfe[12][3] = ((rd_rxd_dfe_tap9_mux()==1)?rd_rxd_dfe_tap9():(rd_rxd_dfe_tap13_mux()==0)?rd_rxd_dfe_tap13():0));
    ESTM(lane_st->dfe[13][0] = ((rd_rxa_dfe_tap10_mux()==1)?rd_rxa_dfe_tap10():(rd_rxa_dfe_tap14_mux()==0)?rd_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[13][1] = ((rd_rxb_dfe_tap10_mux()==1)?rd_rxb_dfe_tap10():(rd_rxb_dfe_tap14_mux()==0)?rd_rxb_dfe_tap14():0));
    ESTM(lane_st->dfe[13][2] = ((rd_rxc_dfe_tap10_mux()==1)?rd_rxc_dfe_tap10():(rd_rxc_dfe_tap14_mux()==0)?rd_rxc_dfe_tap14():0));
    ESTM(lane_st->dfe[13][3] = ((rd_rxd_dfe_tap10_mux()==1)?rd_rxd_dfe_tap10():(rd_rxd_dfe_tap14_mux()==0)?rd_rxd_dfe_tap14():0));
    ESTM(lane_st->dfe[14][0] = ((rd_rxa_dfe_tap7_mux()==2)?rd_rxa_dfe_tap7():(rd_rxa_dfe_tap11_mux()==1)?rd_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[14][1] = ((rd_rxb_dfe_tap7_mux()==2)?rd_rxb_dfe_tap7():(rd_rxb_dfe_tap11_mux()==1)?rd_rxb_dfe_tap11():0));
    ESTM(lane_st->dfe[14][2] = ((rd_rxc_dfe_tap7_mux()==2)?rd_rxc_dfe_tap7():(rd_rxc_dfe_tap11_mux()==1)?rd_rxc_dfe_tap11():0));
    ESTM(lane_st->dfe[14][3] = ((rd_rxd_dfe_tap7_mux()==2)?rd_rxd_dfe_tap7():(rd_rxd_dfe_tap11_mux()==1)?rd_rxd_dfe_tap11():0));
    ESTM(lane_st->dfe[15][0] = ((rd_rxa_dfe_tap8_mux()==2)?rd_rxa_dfe_tap8():(rd_rxa_dfe_tap12_mux()==1)?rd_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[15][1] = ((rd_rxb_dfe_tap8_mux()==2)?rd_rxb_dfe_tap8():(rd_rxb_dfe_tap12_mux()==1)?rd_rxb_dfe_tap12():0));
    ESTM(lane_st->dfe[15][2] = ((rd_rxc_dfe_tap8_mux()==2)?rd_rxc_dfe_tap8():(rd_rxc_dfe_tap12_mux()==1)?rd_rxc_dfe_tap12():0));
    ESTM(lane_st->dfe[15][3] = ((rd_rxd_dfe_tap8_mux()==2)?rd_rxd_dfe_tap8():(rd_rxd_dfe_tap12_mux()==1)?rd_rxd_dfe_tap12():0));
    ESTM(lane_st->dfe[16][0] = ((rd_rxa_dfe_tap9_mux()==2)?rd_rxa_dfe_tap9():(rd_rxa_dfe_tap13_mux()==1)?rd_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[16][1] = ((rd_rxb_dfe_tap9_mux()==2)?rd_rxb_dfe_tap9():(rd_rxb_dfe_tap13_mux()==1)?rd_rxb_dfe_tap13():0));
    ESTM(lane_st->dfe[16][2] = ((rd_rxc_dfe_tap9_mux()==2)?rd_rxc_dfe_tap9():(rd_rxc_dfe_tap13_mux()==1)?rd_rxc_dfe_tap13():0));
    ESTM(lane_st->dfe[16][3] = ((rd_rxd_dfe_tap9_mux()==2)?rd_rxd_dfe_tap9():(rd_rxd_dfe_tap13_mux()==1)?rd_rxd_dfe_tap13():0));
    ESTM(lane_st->dfe[17][0] = ((rd_rxa_dfe_tap10_mux()==2)?rd_rxa_dfe_tap10():(rd_rxa_dfe_tap14_mux()==1)?rd_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[17][1] = ((rd_rxb_dfe_tap10_mux()==2)?rd_rxb_dfe_tap10():(rd_rxb_dfe_tap14_mux()==1)?rd_rxb_dfe_tap14():0));
    ESTM(lane_st->dfe[17][2] = ((rd_rxc_dfe_tap10_mux()==2)?rd_rxc_dfe_tap10():(rd_rxc_dfe_tap14_mux()==1)?rd_rxc_dfe_tap14():0));
    ESTM(lane_st->dfe[17][3] = ((rd_rxd_dfe_tap10_mux()==2)?rd_rxd_dfe_tap10():(rd_rxd_dfe_tap14_mux()==1)?rd_rxd_dfe_tap14():0));
    ESTM(lane_st->dfe[18][0] = ((rd_rxa_dfe_tap7_mux()==3)?rd_rxa_dfe_tap7():(rd_rxa_dfe_tap11_mux()==2)?rd_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[18][1] = ((rd_rxb_dfe_tap7_mux()==3)?rd_rxb_dfe_tap7():(rd_rxb_dfe_tap11_mux()==2)?rd_rxb_dfe_tap11():0));
    ESTM(lane_st->dfe[18][2] = ((rd_rxc_dfe_tap7_mux()==3)?rd_rxc_dfe_tap7():(rd_rxc_dfe_tap11_mux()==2)?rd_rxc_dfe_tap11():0));
    ESTM(lane_st->dfe[18][3] = ((rd_rxd_dfe_tap7_mux()==3)?rd_rxd_dfe_tap7():(rd_rxd_dfe_tap11_mux()==2)?rd_rxd_dfe_tap11():0));
    ESTM(lane_st->dfe[19][0] = ((rd_rxa_dfe_tap8_mux()==3)?rd_rxa_dfe_tap8():(rd_rxa_dfe_tap12_mux()==2)?rd_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[19][1] = ((rd_rxb_dfe_tap8_mux()==3)?rd_rxb_dfe_tap8():(rd_rxb_dfe_tap12_mux()==2)?rd_rxb_dfe_tap12():0));
    ESTM(lane_st->dfe[19][2] = ((rd_rxc_dfe_tap8_mux()==3)?rd_rxc_dfe_tap8():(rd_rxc_dfe_tap12_mux()==2)?rd_rxc_dfe_tap12():0));
    ESTM(lane_st->dfe[19][3] = ((rd_rxd_dfe_tap8_mux()==3)?rd_rxd_dfe_tap8():(rd_rxd_dfe_tap12_mux()==2)?rd_rxd_dfe_tap12():0));
    ESTM(lane_st->dfe[20][0] = ((rd_rxa_dfe_tap9_mux()==3)?rd_rxa_dfe_tap9():(rd_rxa_dfe_tap13_mux()==2)?rd_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[20][1] = ((rd_rxb_dfe_tap9_mux()==3)?rd_rxb_dfe_tap9():(rd_rxb_dfe_tap13_mux()==2)?rd_rxb_dfe_tap13():0));
    ESTM(lane_st->dfe[20][2] = ((rd_rxc_dfe_tap9_mux()==3)?rd_rxc_dfe_tap9():(rd_rxc_dfe_tap13_mux()==2)?rd_rxc_dfe_tap13():0));
    ESTM(lane_st->dfe[20][3] = ((rd_rxd_dfe_tap9_mux()==3)?rd_rxd_dfe_tap9():(rd_rxd_dfe_tap13_mux()==2)?rd_rxd_dfe_tap13():0));
    ESTM(lane_st->dfe[21][0] = ((rd_rxa_dfe_tap10_mux()==3)?rd_rxa_dfe_tap10():(rd_rxa_dfe_tap14_mux()==2)?rd_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[21][1] = ((rd_rxb_dfe_tap10_mux()==3)?rd_rxb_dfe_tap10():(rd_rxb_dfe_tap14_mux()==2)?rd_rxb_dfe_tap14():0));
    ESTM(lane_st->dfe[21][2] = ((rd_rxc_dfe_tap10_mux()==3)?rd_rxc_dfe_tap10():(rd_rxc_dfe_tap14_mux()==2)?rd_rxc_dfe_tap14():0));
    ESTM(lane_st->dfe[21][3] = ((rd_rxd_dfe_tap10_mux()==3)?rd_rxd_dfe_tap10():(rd_rxd_dfe_tap14_mux()==2)?rd_rxd_dfe_tap14():0));
    ESTM(lane_st->dfe[22][0] = ((rd_rxa_dfe_tap11_mux()==3)?rd_rxa_dfe_tap11():0));
    ESTM(lane_st->dfe[22][1] = ((rd_rxb_dfe_tap11_mux()==3)?rd_rxb_dfe_tap11():0));
    ESTM(lane_st->dfe[22][2] = ((rd_rxc_dfe_tap11_mux()==3)?rd_rxc_dfe_tap11():0));
    ESTM(lane_st->dfe[22][3] = ((rd_rxd_dfe_tap11_mux()==3)?rd_rxd_dfe_tap11():0));
    ESTM(lane_st->dfe[23][0] = ((rd_rxa_dfe_tap12_mux()==3)?rd_rxa_dfe_tap12():0));
    ESTM(lane_st->dfe[23][1] = ((rd_rxb_dfe_tap12_mux()==3)?rd_rxb_dfe_tap12():0));
    ESTM(lane_st->dfe[23][2] = ((rd_rxc_dfe_tap12_mux()==3)?rd_rxc_dfe_tap12():0));
    ESTM(lane_st->dfe[23][3] = ((rd_rxd_dfe_tap12_mux()==3)?rd_rxd_dfe_tap12():0));
    ESTM(lane_st->dfe[24][0] = ((rd_rxa_dfe_tap13_mux()==3)?rd_rxa_dfe_tap13():0));
    ESTM(lane_st->dfe[24][1] = ((rd_rxb_dfe_tap13_mux()==3)?rd_rxb_dfe_tap13():0));
    ESTM(lane_st->dfe[24][2] = ((rd_rxc_dfe_tap13_mux()==3)?rd_rxc_dfe_tap13():0));
    ESTM(lane_st->dfe[24][3] = ((rd_rxd_dfe_tap13_mux()==3)?rd_rxd_dfe_tap13():0));
    ESTM(lane_st->dfe[25][0] = ((rd_rxa_dfe_tap14_mux()==3)?rd_rxa_dfe_tap14():0));
    ESTM(lane_st->dfe[25][1] = ((rd_rxb_dfe_tap14_mux()==3)?rd_rxb_dfe_tap14():0));
    ESTM(lane_st->dfe[25][2] = ((rd_rxc_dfe_tap14_mux()==3)?rd_rxc_dfe_tap14():0));
    ESTM(lane_st->dfe[25][3] = ((rd_rxd_dfe_tap14_mux()==3)?rd_rxd_dfe_tap14():0));
    ESTM(lane_st->thctrl_dp[0] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxa_slicer_offset_adj_dp()));
    ESTM(lane_st->thctrl_dp[1] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxb_slicer_offset_adj_dp()));
    ESTM(lane_st->thctrl_dp[2] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxc_slicer_offset_adj_dp()));
    ESTM(lane_st->thctrl_dp[3] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxd_slicer_offset_adj_dp()));
    ESTM(lane_st->thctrl_dn[0] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxa_slicer_offset_adj_dn()));
    ESTM(lane_st->thctrl_dn[1] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxb_slicer_offset_adj_dn()));
    ESTM(lane_st->thctrl_dn[2] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxc_slicer_offset_adj_dn()));
    ESTM(lane_st->thctrl_dn[3] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxd_slicer_offset_adj_dn()));
    ESTM(lane_st->thctrl_zp[0] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxa_slicer_offset_adj_zp()));
    ESTM(lane_st->thctrl_zp[1] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxb_slicer_offset_adj_zp()));
    ESTM(lane_st->thctrl_zp[2] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxc_slicer_offset_adj_zp()));
    ESTM(lane_st->thctrl_zp[3] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxd_slicer_offset_adj_zp()));
    ESTM(lane_st->thctrl_zn[0] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxa_slicer_offset_adj_zn()));
    ESTM(lane_st->thctrl_zn[1] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxb_slicer_offset_adj_zn()));
    ESTM(lane_st->thctrl_zn[2] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxc_slicer_offset_adj_zn()));
    ESTM(lane_st->thctrl_zn[3] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxd_slicer_offset_adj_zn()));
    ESTM(lane_st->thctrl_l[0] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxa_slicer_offset_adj_lms()));
    ESTM(lane_st->thctrl_l[1] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxb_slicer_offset_adj_lms()));
    ESTM(lane_st->thctrl_l[2] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxc_slicer_offset_adj_lms()));
    ESTM(lane_st->thctrl_l[3] = falcon16_v1l4p1_INTERNAL_afe_slicer_offset_mapping(rd_rxd_slicer_offset_adj_lms()));
    EFUN(falcon16_v1l4p1_INTERNAL_get_eye_margin_est(sa__, &lane_st->heye_left, &lane_st->heye_right, &lane_st->veye_upper, &lane_st->veye_lower));
    ESTM(lane_st->link_time = (uint16_t)((((uint32_t)rdv_usr_sts_link_time())*8)/10));

    if (lane_st->pmd_lock == 1) {
      if (!lane_st->stop_state) {
        EFUN(falcon16_v1l4p1_stop_rx_adaptation(sa__, 0));
      }
    } else {
        EFUN(falcon16_v1l4p1_stop_rx_adaptation(sa__, 0));
    }

    return(ERR_CODE_NONE);
}


err_code_t falcon16_v1l4p1_log_full_pmd_state (srds_access_t *sa__, struct falcon16_v1l4p1_detailed_lane_status_st *lane_st) {
    enum srds_prbs_polynomial_enum prbs_poly_mode = PRBS_7;
    enum srds_prbs_checker_mode_enum prbs_checker_mode = PRBS_SELF_SYNC_HYSTERESIS;

    if(!lane_st)
      return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    ENULL_MEMSET(lane_st, 0, sizeof(struct falcon16_v1l4p1_detailed_lane_status_st));
    EFUN(falcon16_v1l4p1_log_full_pmd_state_noPRBS(sa__, lane_st));

    ESTM(lane_st->prbs_chk_en = rd_prbs_chk_en());
    EFUN(falcon16_v1l4p1_get_rx_prbs_config(sa__, &prbs_poly_mode, &prbs_checker_mode, &lane_st->prbs_chk_inv));
    lane_st->prbs_chk_order = (uint8_t)prbs_poly_mode;

    EFUN(falcon16_v1l4p1_prbs_chk_lock_state(sa__, &lane_st->prbs_chk_lock));
    EFUN(falcon16_v1l4p1_prbs_err_count_ll(sa__, &lane_st->prbs_chk_errcnt));
    EFUN(falcon16_v1l4p1_INTERNAL_get_BER_string(sa__,100,&lane_st->ber_string[0]));

    return(ERR_CODE_NONE);
}


err_code_t falcon16_v1l4p1_disp_full_pmd_state (srds_access_t *sa__, struct falcon16_v1l4p1_detailed_lane_status_st const * const lane_st, uint8_t num_lanes) {
    const uint8_t num_bytes_each_line = 26;
    uint32_t i;
    uint32_t size_of_lane_st = 0;

    const uint8_t big_endian = falcon16_v1l4p1_INTERNAL_is_big_endian();


    if(lane_st == NULL) {
        return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if ((num_lanes != 4) && (num_lanes != 8) && (num_lanes != 1)) {
      return(falcon16_v1l4p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));  /* Number of lanes has to be 1, 4 or 8 */
    }

    size_of_lane_st = sizeof(struct falcon16_v1l4p1_detailed_lane_status_st);

    EFUN_PRINTF(("\n**** SERDES EXTENDED LANE DIAG DATA CORE %d LANE %d DUMP ****", falcon16_v1l4p1_get_core(sa__), falcon16_v1l4p1_get_lane(sa__)));
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

void falcon16_v1l4p1_INTERNAL_print_triage_info(srds_access_t *sa__, err_code_t err_code, uint8_t print_header, uint8_t print_data, uint16_t line)
{
/*  Note: No EFUNs or ESTMs should be used in this function as this print routine is called by _error() handler. */
    falcon16_v1l4p1_triage_info info;
    err_code_t error;
    uint8_t error_seen = 0;
    err_code_t __err;
    uint16_t   ucode_version_major;
    uint8_t    ucode_version_minor;
    UNUSED(error);
    USR_MEMSET(&info, 0, sizeof(falcon16_v1l4p1_triage_info));
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
    error = falcon16_v1l4p1_version(sa__, &info.api_ver);
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
    if(falcon16_v1l4p1_INTERNAL_sigdet_status(sa__, &info.sig_det, &info.sig_det_chg)) {
        error_seen = 1;
    }
    if (falcon16_v1l4p1_INTERNAL_pmd_lock_status(sa__, &info.pmd_lock, &info.pmd_lock_chg)) {
        error_seen = 1;
    }

    CHECK_ERR(info.dsc_one_hot[0] = rd_dsc_state_one_hot());
    CHECK_ERR(info.dsc_one_hot[1] = rd_dsc_state_one_hot());
    CHECK_ERR(info.cmd_info = reg_rd_DSC_A_DSC_UC_CTRL());
    info.core = falcon16_v1l4p1_get_core(sa__);
    info.lane = falcon16_v1l4p1_get_lane(sa__);
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
                        falcon16_v1l4p1_INTERNAL_e2s_err_code(info.error) ));
        } else {
            USR_PRINTF(("%4d, %4d,  %X_%X,    %X_%X, %s\n",
                        info.lane,
                        info.core,
                        info.api_ver>>8,
                        info.api_ver & 0xFF,
                        info.ucode_ver >> 8,
                        info.ucode_ver & 0xFF,
                        falcon16_v1l4p1_INTERNAL_e2s_err_code(info.error) ));
        }
    }
    if (error_seen) {
        USR_PRINTF(("WARNING: There were some errors seen while collecting triage info and so the debug data above may not be all accurate\n"));
    }
    return;
}


