/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2020 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  blackhawk7_l8p2_debug_functions.c                                  *
 *  Created On    :  03 Nov 2015                                                   *
 *  Created By    :  Brent Roberts                                                 *
 *  Description   :  Debug APIs for Serdes IPs                                     *
 *  Revision      :                                                            *
 */

/** @file blackhawk7_l8p2_debug_functions.c
 * Implementation of API debug functions
 */
#include <phymod/phymod_system.h>
#include "blackhawk7_l8p2_debug_functions.h"
#include "blackhawk7_l8p2_access.h"
#include "blackhawk7_l8p2_common.h"
#include "blackhawk7_l8p2_config.h"
#include "blackhawk7_l8p2_functions.h"
#include "blackhawk7_l8p2_internal.h"
#include "blackhawk7_l8p2_internal_error.h"
#include "blackhawk7_l8p2_prbs.h"
#include "blackhawk7_l8p2_select_defns.h"
#include "blackhawk7_l8p2_reg_dump.h"




#ifndef SMALL_FOOTPRINT
static err_code_t _blackhawk7_l8p2_reg_print_no_buf(srds_access_t *sa__, uint8_t reg_section);
static err_code_t _blackhawk7_l8p2_reg_print_with_buf(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][SRDS_DUMP_BUF_SIZE], uint8_t *buf_index);
static err_code_t _blackhawk7_l8p2_reg_print(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][SRDS_DUMP_BUF_SIZE], uint8_t *buf_index);
#endif /* SMALL_FOOTPRINT */


#ifndef SMALL_FOOTPRINT

/*************************/
/*  Stop/Resume uC Lane  */
/*************************/

err_code_t blackhawk7_l8p2_stop_uc_lane(srds_access_t *sa__, uint8_t enable) {

    return(blackhawk7_l8p2_stop_rx_adaptation(sa__,enable));
}

err_code_t blackhawk7_l8p2_stop_uc_lane_status(srds_access_t *sa__, uint8_t *uc_lane_stopped) {
    INIT_SRDS_ERR_CODE

  if(!uc_lane_stopped) {
      return(blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*uc_lane_stopped = rdv_usr_sts_micro_stopped());

  return (ERR_CODE_NONE);
}

/*******************************************************************/
/*  APIs to Write Core/Lane Config and User variables into uC RAM  */
/*******************************************************************/

err_code_t blackhawk7_l8p2_set_usr_ctrl_core_event_log_level(srds_access_t *sa__, uint8_t core_event_log_level) {
  return(wrcv_usr_ctrl_core_event_log_level(core_event_log_level));
}

err_code_t blackhawk7_l8p2_set_usr_ctrl_lane_event_log_level(srds_access_t *sa__, uint8_t lane_event_log_level) {
  return(wrv_usr_ctrl_lane_event_log_level(lane_event_log_level));
}

err_code_t blackhawk7_l8p2_set_usr_ctrl_disable_startup(srds_access_t *sa__, struct blackhawk7_l8p2_usr_ctrl_disable_functions_st set_val) {
    INIT_SRDS_ERR_CODE
  EFUN(blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_startup_functions_word(set_val.word));
}

err_code_t blackhawk7_l8p2_set_usr_ctrl_disable_startup_dfe(srds_access_t *sa__, struct blackhawk7_l8p2_usr_ctrl_disable_dfe_functions_st set_val) {
    INIT_SRDS_ERR_CODE
  EFUN(blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_startup_dfe_functions_byte(set_val.byte));
}

err_code_t blackhawk7_l8p2_set_usr_ctrl_disable_steady_state(srds_access_t *sa__, struct blackhawk7_l8p2_usr_ctrl_disable_functions_st set_val) {
    INIT_SRDS_ERR_CODE
  EFUN(blackhawk7_l8p2_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */
  EFUN(blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_steady_state_functions_word(set_val.word));
}

err_code_t blackhawk7_l8p2_set_usr_ctrl_disable_steady_state_dfe(srds_access_t *sa__, struct blackhawk7_l8p2_usr_ctrl_disable_dfe_functions_st set_val) {
    INIT_SRDS_ERR_CODE
  EFUN(blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_dfe_functions_byte(&set_val));
  return(wrv_usr_ctrl_disable_steady_state_dfe_functions_byte(set_val.byte));
}

/******************************************************************/
/*  APIs to Read Core/Lane Config and User variables from uC RAM  */
/******************************************************************/

err_code_t blackhawk7_l8p2_get_usr_ctrl_core_event_log_level(srds_access_t *sa__, uint8_t *core_event_log_level) {
    INIT_SRDS_ERR_CODE

  if(!core_event_log_level) {
     return(blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*core_event_log_level = rdcv_usr_ctrl_core_event_log_level());

  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_get_usr_ctrl_lane_event_log_level(srds_access_t *sa__, uint8_t *lane_event_log_level) {
    INIT_SRDS_ERR_CODE

  if(!lane_event_log_level) {
     return(blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*lane_event_log_level = rdv_usr_ctrl_lane_event_log_level());
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_get_usr_ctrl_disable_startup(srds_access_t *sa__, struct blackhawk7_l8p2_usr_ctrl_disable_functions_st *get_val) {
    INIT_SRDS_ERR_CODE

  if(!get_val) {
     return(blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdv_usr_ctrl_disable_startup_functions_word());
  EFUN(blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_get_usr_ctrl_disable_startup_dfe(srds_access_t *sa__, struct blackhawk7_l8p2_usr_ctrl_disable_dfe_functions_st *get_val) {
    INIT_SRDS_ERR_CODE

  if(!get_val) {
     return(blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_startup_dfe_functions_byte());
  EFUN(blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_get_usr_ctrl_disable_steady_state(srds_access_t *sa__, struct blackhawk7_l8p2_usr_ctrl_disable_functions_st *get_val) {
    INIT_SRDS_ERR_CODE

  if(!get_val) {
     return(blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->word = rdv_usr_ctrl_disable_steady_state_functions_word());
  EFUN(blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_functions_st(get_val));
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_get_usr_ctrl_disable_steady_state_dfe(srds_access_t *sa__, struct blackhawk7_l8p2_usr_ctrl_disable_dfe_functions_st *get_val) {
    INIT_SRDS_ERR_CODE

  if(!get_val) {
     return(blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(get_val->byte = rdv_usr_ctrl_disable_steady_state_dfe_functions_byte());
  EFUN(blackhawk7_l8p2_INTERNAL_update_usr_ctrl_disable_dfe_functions_st(get_val));
  return (ERR_CODE_NONE);
}

/********************************************************************************/
/* Helper print function for Serdes Register/Variable Dump using no buffer      */
/********************************************************************************/
static err_code_t _blackhawk7_l8p2_reg_print_no_buf(srds_access_t *sa__, uint8_t reg_section) {
    INIT_SRDS_ERR_CODE
    uint16_t addr, rddata = 0;

    for (addr = blackhawk7_l8p2_reg_dump_arr[reg_section][0]; addr < blackhawk7_l8p2_reg_dump_arr[reg_section][1]; addr++) {
        if (!(addr & 0x000F)) {
            EFUN_PRINTF(("\n%04x ",addr));
        }
        EFUN(blackhawk7_l8p2_acc_rdt_reg(sa__, addr,&rddata));
        EFUN_PRINTF(("%04x ",rddata));
    }
    return (ERR_CODE_NONE);
}

/********************************************************************************/
/* Helper print function for Serdes Register/Variable Dump using a buffer       */
/********************************************************************************/
static err_code_t _blackhawk7_l8p2_reg_print_with_buf(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][SRDS_DUMP_BUF_SIZE], uint8_t *buf_index) {
    INIT_SRDS_ERR_CODE
    uint16_t addr, rddata = 0;
    uint8_t first = 0;
    int32_t count = 0;

    if(reg_section == 0) {
        first = 1;
    }

    for (addr = blackhawk7_l8p2_reg_dump_arr[reg_section][0]; addr < blackhawk7_l8p2_reg_dump_arr[reg_section][1]; addr++) {
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
        EFUN(blackhawk7_l8p2_acc_rdt_reg(sa__, addr,&rddata));
        count += USR_SNPRINTF(reg_buffer[*buf_index] + count, (size_t)(SRDS_DUMP_BUF_SIZE - count - 1), "%04x ", rddata);

    }
    return (ERR_CODE_NONE);
}

/********************************************************************************/
/* Helper print function for Serdes Register/Variable Dump                      */
/********************************************************************************/
static err_code_t _blackhawk7_l8p2_reg_print(srds_access_t *sa__, uint8_t reg_section, char reg_buffer[][SRDS_DUMP_BUF_SIZE], uint8_t *buf_index) {
    INIT_SRDS_ERR_CODE
    if(reg_buffer == NULL) {
         EFUN(_blackhawk7_l8p2_reg_print_no_buf(sa__, reg_section));
    }
    else {
        EFUN(_blackhawk7_l8p2_reg_print_with_buf(sa__, reg_section, reg_buffer, buf_index));
    }
    return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_reg_dump(srds_access_t *sa__, uint8_t core, uint8_t lane, char reg_buffer[][SRDS_DUMP_BUF_SIZE]) {
    INIT_SRDS_ERR_CODE
    uint8_t reg_section = 0;
    uint8_t buf_index = 0;

    if (reg_buffer == NULL) {
        EFUN_PRINTF(("\n****  SERDES REGISTER CORE %d LANE %d DUMP    ****", core, lane));
    }
    else {
        EFUN_PRINTF(("\n****  SERDES REGISTER CORE %d LANE %d DECODED    ****\n", core, lane));
    }
    while(reg_section < BLACKHAWK7_L8P2_REG_DUMP_SECTIONS) {
        if(blackhawk7_l8p2_reg_dump_arr[reg_section][0] == BLACKHAWK7_L8P2_REG_PLL_START) {
            uint8_t pll_orig, pll_idx;
            uint8_t pll_start_section = reg_section;

            ESTM(pll_orig = blackhawk7_l8p2_acc_get_pll_idx(sa__));
            for (pll_idx = 0; pll_idx < NUM_PLLS; pll_idx++) {
                reg_section = pll_start_section;
                EFUN(blackhawk7_l8p2_acc_set_pll_idx(sa__,pll_idx));
                while( reg_section < BLACKHAWK7_L8P2_REG_DUMP_SECTIONS && blackhawk7_l8p2_reg_dump_arr[reg_section][0] != BLACKHAWK7_L8P2_REG_UC_START) {
                    EFUN(_blackhawk7_l8p2_reg_print(sa__, reg_section, reg_buffer, &buf_index));
                    reg_section++;
                }
            }
            EFUN(blackhawk7_l8p2_acc_set_pll_idx(sa__,pll_orig));
        }
        else if (blackhawk7_l8p2_reg_dump_arr[reg_section][0] == BLACKHAWK7_L8P2_REG_UC_START) {
            uint8_t micro_orig, micro_idx, num_micros;
            uint8_t uc_start_section = reg_section;

            ESTM(micro_orig = blackhawk7_l8p2_acc_get_micro_idx(sa__));
            EFUN(blackhawk7_l8p2_get_micro_num_uc_cores(sa__, &num_micros));
            for(micro_idx = 0; micro_idx < num_micros; micro_idx++) {
                reg_section = uc_start_section;
                EFUN(blackhawk7_l8p2_acc_set_micro_idx(sa__,micro_idx));
                while(reg_section < BLACKHAWK7_L8P2_REG_DUMP_SECTIONS) {
                    EFUN(_blackhawk7_l8p2_reg_print(sa__, reg_section, reg_buffer, &buf_index));
                    reg_section++;
                }
            }
            EFUN(blackhawk7_l8p2_acc_set_micro_idx(sa__,micro_orig));
        }
        else {
            EFUN(_blackhawk7_l8p2_reg_print(sa__, reg_section, reg_buffer, &buf_index));
            reg_section++;
        }
    }

    if (reg_buffer != NULL) {
        reg_buffer[++buf_index][0] = 0xA;
        reg_buffer[++buf_index][0] = 0;
    }

    return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_uc_core_var_dump(srds_access_t *sa__, uint8_t core, char ram_buffer[][SRDS_DUMP_BUF_SIZE]) {
  INIT_SRDS_ERR_CODE
  uint8_t rddata;
  uint8_t addr, core_var_ram_size;

  srds_info_t * blackhawk7_l8p2_info_ptr = blackhawk7_l8p2_INTERNAL_get_blackhawk7_l8p2_info_ptr_with_check(sa__);
  EFUN(blackhawk7_l8p2_INTERNAL_match_ucode_from_info(sa__, blackhawk7_l8p2_info_ptr));
  core_var_ram_size = (uint8_t)blackhawk7_l8p2_info_ptr->core_var_ram_size;

  if (ram_buffer == NULL) {
      EFUN_PRINTF(("\n**** SERDES UC CORE %d RAM VARIABLE DUMP ****", core));
      for (addr = 0x0; addr < core_var_ram_size; addr++) {
          if (!(addr % 26))  {
              EFUN_PRINTF(("\n%04x ",addr));
          }
          ESTM_PRINTF(("%02x ", blackhawk7_l8p2_rdbc_uc_var(sa__, __ERR, addr)));
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
          ESTM(rddata = blackhawk7_l8p2_rdbc_uc_var(sa__, __ERR, addr));
          count += USR_SNPRINTF(ram_buffer[index] + count, (size_t)(SRDS_DUMP_BUF_SIZE - count - 1), "%02x ", rddata);
      }
      ram_buffer[++index][0] = 0xA;
      ram_buffer[++index][0] = 0;
  }
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_uc_micro_var_dump(srds_access_t *sa__, uint8_t core, uint8_t lane, char ram_buffer[][SRDS_DUMP_BUF_SIZE]) {
    INIT_SRDS_ERR_CODE
  uint8_t addr, micro_var_ram_size;

  srds_info_t * blackhawk7_l8p2_info_ptr = blackhawk7_l8p2_INTERNAL_get_blackhawk7_l8p2_info_ptr_with_check(sa__);
  EFUN(blackhawk7_l8p2_INTERNAL_match_ucode_from_info(sa__, blackhawk7_l8p2_info_ptr));
  micro_var_ram_size = (uint8_t)blackhawk7_l8p2_info_ptr->micro_var_ram_size;

  if (ram_buffer == NULL) {
      EFUN_PRINTF(("\n**** SERDES UC CORE %d MICRO %d RAM VARIABLE DUMP ****", core,lane/2));

      for (addr = 0x0; addr < micro_var_ram_size; addr++) {
         if (!(addr % 26))  {
            EFUN_PRINTF(("\n%04x ",addr));
         }
         ESTM_PRINTF(("%02x ", blackhawk7_l8p2_rdbuc_uc_var(sa__, __ERR, addr)));
      }
  }
  else {
      uint8_t rddata, index=0;
      int32_t count = 0;
      EFUN_PRINTF(("\n**** SERDES UC CORE %d MICRO %d RAM VARIABLE DECODED ****\n", core,lane/2));

      for (addr = 0x0; addr < micro_var_ram_size; addr++) {
          if (!(addr % 26))  {
              if (addr != 0) {
                  index++;
                  count = 0;
            }
            count += USR_SNPRINTF(ram_buffer[index] + count, (size_t)(SRDS_DUMP_BUF_SIZE - count - 1),"%04x ", addr);
         }
         ESTM(rddata = blackhawk7_l8p2_rdbuc_uc_var(sa__, __ERR, addr));
         count += USR_SNPRINTF(ram_buffer[index] + count, (size_t)(SRDS_DUMP_BUF_SIZE - count - 1), "%02x ", rddata);
     }
     ram_buffer[++index][0] = 0xA;
     ram_buffer[++index][0] = 0;
  }

  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_uc_lane_var_dump(srds_access_t *sa__, uint8_t core, uint8_t lane, char ram_buffer[][SRDS_DUMP_BUF_SIZE]) {
  INIT_SRDS_ERR_CODE
  uint8_t     rx_lock, uc_stopped = 0;
  uint16_t    addr, lane_var_ram_size;
  srds_info_t const * const blackhawk7_l8p2_info_ptr = blackhawk7_l8p2_INTERNAL_get_blackhawk7_l8p2_info_ptr_with_check(sa__);
  EFUN(blackhawk7_l8p2_INTERNAL_match_ucode_from_info(sa__, blackhawk7_l8p2_info_ptr));
  lane_var_ram_size = (uint16_t)blackhawk7_l8p2_info_ptr->lane_var_ram_size;

  ESTM(rx_lock = rd_pmd_rx_lock());

  {
      err_code_t err_code=ERR_CODE_NONE;
      uc_stopped = blackhawk7_l8p2_INTERNAL_stop_micro(sa__,rx_lock,&err_code);
      if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
  }

  if (ram_buffer == NULL) {
      EFUN_PRINTF(("\n**** SERDES UC CORE %d LANE %d RAM VARIABLE DUMP ****", core,lane));
      for (addr = 0x0; addr < lane_var_ram_size; addr++) {
         if (!(addr % 26))  {
            EFUN_PRINTF(("\n%04x ",addr));
         }
         ESTM_PRINTF(("%02x ", blackhawk7_l8p2_rdbl_uc_var(sa__, __ERR, addr)));
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
         ESTM(rddata = blackhawk7_l8p2_rdbl_uc_var(sa__, __ERR, addr));
         count += USR_SNPRINTF(ram_buffer[index] + count,  (size_t)(SRDS_DUMP_BUF_SIZE - count - 1), "%02x ", rddata);
      }
      ram_buffer[++index][0] = 0xA;
      ram_buffer[++index][0] = 0;
  }

  if (rx_lock == 1) {
      if (!uc_stopped) {
          EFUN(blackhawk7_l8p2_stop_rx_adaptation(sa__, 0));
      }
  } else {
      EFUN(blackhawk7_l8p2_stop_rx_adaptation(sa__, 0));
  }

  return (ERR_CODE_NONE);
}

/***************************************/
/*  API Function to Read Event Logger  */
/***************************************/

err_code_t blackhawk7_l8p2_read_event_log(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    blackhawk7_l8p2_INTERNAL_event_log_dump_state_t state;
    uint8_t micro_num = 0;
    srds_info_t * blackhawk7_l8p2_info_ptr;
    ESTM(blackhawk7_l8p2_info_ptr = blackhawk7_l8p2_INTERNAL_get_blackhawk7_l8p2_info_ptr_with_check(sa__));
    for (; micro_num<blackhawk7_l8p2_info_ptr->micro_count; ++micro_num)
    {
        state.index = 0;
        state.line_start_index = 0;
        EFUN(blackhawk7_l8p2_INTERNAL_read_event_log_with_callback(sa__, micro_num, 0, &state, blackhawk7_l8p2_INTERNAL_event_log_dump_callback));
        EFUN(blackhawk7_l8p2_INTERNAL_event_log_dump_callback(sa__, &state, 0, 0));
    }
    return(ERR_CODE_NONE);
}

/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/

/* Enable/Disable internal Loopback */
err_code_t blackhawk7_l8p2_internal_lpbk(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    blackhawk7_l8p2_osr_mode_st osr_mode;
    enum blackhawk7_l8p2_rx_pam_mode_enum pam_mode = NRZ;
    uint8_t reset_state;

    ENULL_MEMSET(&osr_mode, 0, sizeof(blackhawk7_l8p2_osr_mode_st));
    EFUN(blackhawk7_l8p2_INTERNAL_get_osr_mode(sa__, &osr_mode));
    if ((osr_mode.rx != BLACKHAWK7_L8P2_OSX1) && (osr_mode.rx != BLACKHAWK7_L8P2_OSX2) && (osr_mode.rx != BLACKHAWK7_L8P2_OSX4)) {
        EFUN_PRINTF(("Error: Internal loopback only supported in OSx1, OSx2, and OSx4\n"));
        return(blackhawk7_l8p2_error(sa__, ERR_CODE_CONFLICTING_PARAMETERS));
    }

    EFUN(blackhawk7_l8p2_INTERNAL_get_rx_pam_mode(sa__, &pam_mode));
    if (pam_mode == PAM4_ER) {
        EFUN_PRINTF(("Error: Internal loopback not supported in PAM4 ER mode\n"));
        return(blackhawk7_l8p2_error(sa__, ERR_CODE_INVALID_RX_PAM_MODE));
    }

    if(enable) {
        ESTM(reset_state = rd_rx_lane_dp_reset_state());
        if(reset_state < 7) {
            EFUN_PRINTF(("ERROR: blackhawk7_l8p2_internal_lpbk(..) called without ln_dp_s_rstb=0 Lane=%d reset_state=%d\n",blackhawk7_l8p2_acc_get_lane(sa__),reset_state));
            return (blackhawk7_l8p2_error(sa__, ERR_CODE_LANE_DP_NOT_RESET));
        }

        EFUN_PRINTF((" Core: %d, Lane %d: Starting internal loopback mode...\n", blackhawk7_l8p2_acc_get_core(sa__), blackhawk7_l8p2_acc_get_lane(sa__)));
        EFUN(wr_dac4ck_check_disable(1));                                        /* Disable clock check */
        EFUN(wr_ilb_en(1));                                                      /* Enable ILB mode */
        EFUN(wr_signal_detect_frc_val(0));                                       /* Force sigdet off */
        EFUN(wr_signal_detect_frc(1));
        EFUN(wr_ams_tx_hz_dac(1));                                               /* Equivalent to turning off Tx */
        EFUN(wr_signal_detect_frc_val(1));                                       /* Force sigdet on */
    }
    else {
        EFUN(wr_ams_tx_hz_dac(0));                                               /* Equivalent to turning on Tx */
        EFUN(wr_signal_detect_frc(0));                                           /* Disable force sigdet */
        EFUN(wr_dac4ck_check_disable(0));                                        /* Enable clock check */
        EFUN(wr_ilb_en(0));                                                      /* Disable ILB mode */
        EFUN(wr_ln_s_rstb(0));                                                   /* Toggle lane soft reset */
        EFUN(wr_ln_s_rstb(1));

        EFUN_PRINTF(("WARNING: Core: %d, Lane %d: Exiting internal loopback mode by toggling ln_s_rstb, please re-configure the lane!\n", blackhawk7_l8p2_acc_get_core(sa__), blackhawk7_l8p2_acc_get_lane(sa__)));
    }
  return (ERR_CODE_NONE);
}

/* Enable/Disable Digital Loopback */
err_code_t blackhawk7_l8p2_dig_lpbk(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    /* setting/clearing prbs_chk_en_auto_mode while coming out of/going in to dig lpbk */
    EFUN(wr_prbs_chk_en_auto_mode(!enable));
    EFUN(wr_dig_lpbk_en(enable));                         /* 0 = disabled, 1 = enabled */

    /* Link Training enabled is an invalid mode in digital loopback */
    if(enable) {
        uint8_t link_training_enable = 0;
        ESTM(link_training_enable = rd_linktrn_ieee_training_enable());
        if(link_training_enable) {
            EFUN_PRINTF(("Warning: Core: %d, Lane %d: Link Training mode is on in digital loopback.\n", blackhawk7_l8p2_acc_get_core(sa__), blackhawk7_l8p2_acc_get_lane(sa__)));
        }
    }

  return (ERR_CODE_NONE);
}


/**********************************/
/*  TX_PI Jitter Generation APIs  */
/**********************************/

/* TX_PI Sinusoidal or Spread-Spectrum (SSC) Jitter Generation  */
err_code_t blackhawk7_l8p2_tx_pi_jitt_gen(srds_access_t *sa__, uint8_t enable, int16_t freq_override_val, enum srds_tx_pi_freq_jit_gen_enum jit_type, uint8_t tx_pi_jit_freq_idx, uint8_t tx_pi_jit_amp) {
    INIT_SRDS_ERR_CODE
    /* Added a limiting for the jitter amplitude index, per freq_idx */
    uint8_t max_amp_idx_r20_os1[] = {37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 48, 33, 47, 37, 33, 37, 37};

    /* Irrespective of the osr_mode, txpi runs @ os1. Thus the max amp idx values remain the same. */
    if (jit_type == TX_PI_SJ) {
        if (tx_pi_jit_amp > max_amp_idx_r20_os1[tx_pi_jit_freq_idx]) {
            tx_pi_jit_amp = max_amp_idx_r20_os1[tx_pi_jit_freq_idx];
        }
    }

    EFUN(blackhawk7_l8p2_tx_pi_freq_override(sa__, enable, freq_override_val));

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

err_code_t blackhawk7_l8p2_isolate_ctrl_pins(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    uint8_t lane, lane_orig, num_lanes;
    uint8_t pll, pll_orig;
    ESTM(pll_orig = blackhawk7_l8p2_acc_get_pll_idx(sa__));

    for(pll = 0; pll < NUM_PLLS; pll++) {
        EFUN(blackhawk7_l8p2_acc_set_pll_idx(sa__, pll));
        EFUN(blackhawk7_l8p2_isolate_core_ctrl_pins(sa__, enable));
    }
    EFUN(blackhawk7_l8p2_acc_set_pll_idx(sa__, pll_orig));

    ESTM(lane_orig = blackhawk7_l8p2_acc_get_lane(sa__));
    /* read num lanes per core directly from register */
    ESTM(num_lanes = rdc_revid_multiplicity());
    for(lane = 0; lane < num_lanes; lane++) {
        EFUN(blackhawk7_l8p2_acc_set_lane(sa__, lane));
        EFUN(blackhawk7_l8p2_isolate_lane_ctrl_pins(sa__, enable));
    }
    EFUN(blackhawk7_l8p2_acc_set_lane(sa__, lane_orig));

  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_isolate_lane_ctrl_pins(srds_access_t *sa__, uint8_t enable) {

    blackhawk7_l8p2_isolate_lane_ctrl_tx_pins(sa__, enable);
    blackhawk7_l8p2_isolate_lane_ctrl_rx_pins(sa__, enable);
    return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_isolate_lane_ctrl_tx_pins(srds_access_t *sa__, uint8_t enable){

    INIT_SRDS_ERR_CODE

    if (enable) {
        EFUN(wr_pmd_ln_tx_h_pwrdn_pkill(0x1));
        EFUN(wr_pmd_ln_dp_h_rstb_pkill(0x1));
        EFUN(wr_pmd_ln_h_rstb_pkill(0x1));
        EFUN(wr_pmd_tx_disable_pkill(0x1));
    } else {
        EFUN(wr_pmd_ln_tx_h_pwrdn_pkill(0x0));
        EFUN(wr_pmd_ln_dp_h_rstb_pkill(0x0));
        EFUN(wr_pmd_ln_h_rstb_pkill(0x0));
        EFUN(wr_pmd_tx_disable_pkill(0x0));
    }
    return (ERR_CODE_NONE);

}

err_code_t blackhawk7_l8p2_isolate_lane_ctrl_rx_pins(srds_access_t *sa__, uint8_t enable){

    INIT_SRDS_ERR_CODE

    if (enable) {
        EFUN(wr_pmd_ln_rx_h_pwrdn_pkill(0x1));
        EFUN(wr_pmd_ln_dp_h_rstb_pkill(0x1));
        EFUN(wr_pmd_ln_h_rstb_pkill(0x1));
    } else {
        EFUN(wr_pmd_ln_rx_h_pwrdn_pkill(0x0));
        EFUN(wr_pmd_ln_dp_h_rstb_pkill(0x0));
        EFUN(wr_pmd_ln_h_rstb_pkill(0x0));
    }
    return (ERR_CODE_NONE);

}


err_code_t blackhawk7_l8p2_isolate_core_ctrl_pins(srds_access_t *sa__, uint8_t enable) {
  INIT_SRDS_ERR_CODE

  if (enable) {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x1));
  }
  else {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x0));
  }
  return (ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_get_dac4ck(srds_access_t *sa__, int8_t *lms, int8_t *phase, int8_t *data) {
  INIT_SRDS_ERR_CODE
  ESTM(*lms   =(int8_t)rd_ams_rx_dac4ck_lms_i());
  ESTM(*phase = (int8_t)rd_ams_rx_dac4ck_phs_i());
  ESTM(*data  = (int8_t)rd_ams_rx_dac4ck_dat_i());
  {
    uint8_t dac4ck_msb;
    ESTM(dac4ck_msb = rd_ams_rx_dac4ck_lms_i_4());
    *lms |= (int8_t)((dac4ck_msb & 0x1) << 4);
    ESTM(dac4ck_msb = rd_ams_rx_dac4ck_phs_i_4());
    *phase |= (int8_t)((dac4ck_msb & 0x1) << 4);
    ESTM(dac4ck_msb = rd_ams_rx_dac4ck_dat_i_4());
    *data |= (int8_t)((dac4ck_msb & 0x1) << 4);
  }
  /* Convert from sign+gray to sign+bin */
  *lms   = (int8_t)((*lms   & BLACKHAWK7_L8P2_DAC4CK_MASK) | (blackhawk7_l8p2_INTERNAL_gray_to_uint8(*lms&(BLACKHAWK7_L8P2_DAC4CK_MASK-1))));
  *phase = (int8_t)((*phase & BLACKHAWK7_L8P2_DAC4CK_MASK) | (blackhawk7_l8p2_INTERNAL_gray_to_uint8(*phase&(BLACKHAWK7_L8P2_DAC4CK_MASK-1))));
  *data  = (int8_t)((*data  & BLACKHAWK7_L8P2_DAC4CK_MASK) | (blackhawk7_l8p2_INTERNAL_gray_to_uint8(*data&(BLACKHAWK7_L8P2_DAC4CK_MASK-1))));
  if (*lms>(BLACKHAWK7_L8P2_DAC4CK_MASK-1))   *lms   = (int8_t)(-(*lms&(BLACKHAWK7_L8P2_DAC4CK_MASK-1)));
  if (*phase>(BLACKHAWK7_L8P2_DAC4CK_MASK-1)) *phase = (int8_t)(-(*phase&(BLACKHAWK7_L8P2_DAC4CK_MASK-1)));
  if (*data>(BLACKHAWK7_L8P2_DAC4CK_MASK-1))  *data  = (int8_t)(-(*data&(BLACKHAWK7_L8P2_DAC4CK_MASK-1)));
  return (ERR_CODE_NONE);
}


err_code_t blackhawk7_l8p2_set_pam_eye_margin_usr_ctrl(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    struct blackhawk7_l8p2_usr_ctrl_disable_functions_st val = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},0};

    /* get current usr ctrl disable value */
    EFUN(blackhawk7_l8p2_get_usr_ctrl_disable_steady_state(sa__, &val));

    /* Set/reset pam eye margin control */
    if(enable == 0) {
        val.field.pam_eye_margin_estimation = 1;
    } else {
        val.field.pam_eye_margin_estimation = 0;
    }
    EFUN(blackhawk7_l8p2_set_usr_ctrl_disable_steady_state(sa__, val));
    return ERR_CODE_NONE;
}

err_code_t blackhawk7_l8p2_get_sc_reg_bit_test_fail_count(srds_access_t *sa__, uint16_t *fail_count) {
    INIT_SRDS_ERR_CODE
    EFUN(blackhawk7_l8p2_pmd_uc_cmd(sa__, CMD_RUN_SC_REG_BITS_TEST, 0, 100));
    ESTM(*fail_count = rd_uc_dsc_data());
    return ERR_CODE_NONE;
}


err_code_t blackhawk7_l8p2_log_full_pmd_state_noPRBS (srds_access_t *sa__, struct blackhawk7_l8p2_detailed_lane_status_st *lane_st) {
    INIT_SRDS_ERR_CODE
    int8_t   tmp;
    uint16_t reg_data1;
    uint8_t  tmp1;
    const uint8_t big_endian = blackhawk7_l8p2_INTERNAL_is_big_endian();

    if(!lane_st)
      return(blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    ESTM(lane_st->pmd_lock = rd_pmd_rx_lock());
    {
        err_code_t err_code=ERR_CODE_NONE;
        lane_st->stop_state = blackhawk7_l8p2_INTERNAL_stop_micro(sa__,lane_st->pmd_lock,&err_code);
        if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
    }

    ESTM(lane_st->reset_state = rd_lane_dp_reset_state());
    /*populate the big endian info */
    lane_st->big_endian = big_endian;
    ESTM(lane_st->dummy23 = rdv_usr_dbstopped());

  {
     enum blackhawk7_l8p2_rx_pam_mode_enum pam_mode = NRZ;
     EFUN(blackhawk7_l8p2_INTERNAL_get_rx_pam_mode(sa__, &pam_mode));
     lane_st->rx_pam_mode = (uint8_t)pam_mode;
  }
    ESTM(lane_st->pmd_tx_pll_select = rd_tx_pll_select());
    ESTM(lane_st->pmd_rx_pll_select = rd_rx_pll_select());
    ESTM(lane_st->restart_count = rdv_usr_sts_restart_counter());
    ESTM(lane_st->reset_count = rdv_usr_sts_reset_counter());
    ESTM(lane_st->pmd_lock_count = rdv_usr_sts_pmd_lock_counter());

    ESTM(lane_st->temp_idx = rdcv_temp_idx());
    ESTM(lane_st->ams_tx_ana_rescal = rdc_rescal_bg_mux());
    ESTM(lane_st->tx_pam_mode = rd_tx_pam4_mode());
    ESTM(lane_st->tx_prec_en  = rd_pam4_precoder_en());
    ESTM(lane_st->txfir_use_pam4_range = rd_txfir_nrz_tap_range_sel() ? 0 : 1);
    {
        uint8_t tap_num;
        for (tap_num=0; tap_num<6; ++tap_num) {
            EFUN(blackhawk7_l8p2_INTERNAL_get_tx_tap(sa__, tap_num, &(lane_st->txfir.tap[tap_num])));
        }
        ESTM(tap_num = rd_txfir_tap_en());
        lane_st->num_txfir_taps = (tap_num == 0) ? 3 : ((tap_num == 1) ? 6 : ((tap_num == 2) ? 9 : 12));
    }

    ESTM(lane_st->sigdet = rd_signal_detect());
    ESTM(lane_st->dsc_sm[0] = rd_dsc_state_one_hot());
    ESTM(lane_st->dsc_sm[1] = rd_dsc_state_one_hot());
    ESTM(lane_st->ppm = rd_cdr_integ_reg());
    EFUN(blackhawk7_l8p2_INTERNAL_get_rx_vga(sa__, &tmp1));
    lane_st->vga = tmp1;
    EFUN(blackhawk7_l8p2_INTERNAL_get_rx_pf_main(sa__, &tmp1)); lane_st->pf = tmp1;
    EFUN(blackhawk7_l8p2_INTERNAL_get_rx_pf2(sa__, &tmp1)); lane_st->pf2 = tmp1;

    EFUN(blackhawk7_l8p2_INTERNAL_get_rx_pf3(sa__, &tmp1)); lane_st->pf3 = tmp1;

    ESTM(lane_st->main_tap_est = rdv_usr_main_tap_est());
    {
        int8_t d23, d14, d05;
        ESTM(d23 = rd_rx_data23_status());
        ESTM(d14 = rd_rx_data14_status());
        ESTM(d05 = rd_rx_data05_status());
        lane_st->data23_thresh  = d23;
        lane_st->data14_thresh  = d14;
        lane_st->data05_thresh  = d05;
    }
    ESTM(lane_st->phase1_thresh  = rd_rx_phase1_status());
    ESTM(lane_st->phase02_thresh = rd_rx_phase02_status());
    EFUN(blackhawk7_l8p2_INTERNAL_get_lms_thresh_bin(sa__, &tmp)); lane_st->lms_thresh = tmp;
    ESTM(lane_st->rx_pi_cnt_perr     = rd_rx_pi_cnt_bin_d_perr());
    ESTM(lane_st->clk90     = rdv_usr_sts_phase_hoffset());
    ESTM(reg_data1 = reg_rd_DSC_E_RX_PI_CNT_BIN_LD());
    ESTM(lane_st->dp_hoffset = rdv_dp_hoffset_0());
    lane_st->dl_hoffset = (int8_t)((reg_data1&0xFF) - ((reg_data1>>8)&0xFF));

    EFUN(blackhawk7_l8p2_get_dac4ck(sa__, &lane_st->lms_dac4ck, &lane_st->phase_dac4ck, &lane_st->data_dac4ck));


    ESTM(lane_st->dc_offset = rd_dc_offset_bin());
    ESTM(lane_st->dfe[1][0] = rd_rxa_dfe_tap2_status());
    ESTM(lane_st->dfe[1][1] = rd_rxb_dfe_tap2_status());
    ESTM(lane_st->dfe[2][0] = rd_rxa_dfe_tap3_status());
    ESTM(lane_st->dfe[2][1] = rd_rxb_dfe_tap3_status());

    {
        /*Per interleave DCO controls , S Gray format need to be converted to binary before display*/
        /* bit 6 is the sign bit */
        uint8_t ctrl;
        ctrl = rd_ams_rx_dc_adjust_a();
        if (ctrl & 0x20) {
            lane_st->dc_adjust_a = (int8_t) -blackhawk7_l8p2_INTERNAL_gray_to_uint8(ctrl & 0x1F);
        } else {
            lane_st->dc_adjust_a = (int8_t) blackhawk7_l8p2_INTERNAL_gray_to_uint8(ctrl);
        }
        ctrl = rd_ams_rx_dc_adjust_b();
        if (ctrl & 0x20) {
            lane_st->dc_adjust_b = (int8_t) -blackhawk7_l8p2_INTERNAL_gray_to_uint8(ctrl & 0x1F);
        } else {
            lane_st->dc_adjust_b = (int8_t) blackhawk7_l8p2_INTERNAL_gray_to_uint8(ctrl);
        }
    }
    ESTM(lane_st->dfe[3][0]  = rd_rxa_dfe_tap4_status());
    ESTM(lane_st->dfe[3][1]  = rd_rxb_dfe_tap4_status());
    ESTM(lane_st->dfe[4][0]  = DFE_TAP_SGB2SG(rd_rxa_dfe_tap5()));
    ESTM(lane_st->dfe[4][1]  = DFE_TAP_SGB2SG(rd_rxb_dfe_tap5()));
    ESTM(lane_st->dfe[5][0]  = DFE_TAP_SGB2SG(rd_rxa_dfe_tap6()));
    ESTM(lane_st->dfe[5][1]  = DFE_TAP_SGB2SG(rd_rxb_dfe_tap6()));
    ESTM(lane_st->dfe[6][0]  = ((rd_rxa_dfe_tap7_mux()==0)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap7()):0));
    ESTM(lane_st->dfe[6][1]  = ((rd_rxb_dfe_tap7_mux()==0)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap7()):0));
    ESTM(lane_st->dfe[7][0]  = ((rd_rxa_dfe_tap8_mux()==0)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap8()):0));
    ESTM(lane_st->dfe[7][1]  = ((rd_rxb_dfe_tap8_mux()==0)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap8()):0));
    ESTM(lane_st->dfe[8][0]  = ((rd_rxa_dfe_tap9_mux()==0)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap9()):0));
    ESTM(lane_st->dfe[8][1]  = ((rd_rxb_dfe_tap9_mux()==0)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap9()):0));
    ESTM(lane_st->dfe[9][0]  = ((rd_rxa_dfe_tap10_mux()==0)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap10()):0));
    ESTM(lane_st->dfe[9][1]  = ((rd_rxb_dfe_tap10_mux()==0)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap10()):0));
    ESTM(lane_st->dfe[10][0] = ((rd_rxa_dfe_tap7_mux()==1)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap7()):(rd_rxa_dfe_tap11_mux()==0)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap11()):0));
    ESTM(lane_st->dfe[10][1] = ((rd_rxb_dfe_tap7_mux()==1)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap7()):(rd_rxb_dfe_tap11_mux()==0)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap11()):0));
    ESTM(lane_st->dfe[11][0] = ((rd_rxa_dfe_tap8_mux()==1)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap8()):(rd_rxa_dfe_tap12_mux()==0)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap12()):0));
    ESTM(lane_st->dfe[11][1] = ((rd_rxb_dfe_tap8_mux()==1)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap8()):(rd_rxb_dfe_tap12_mux()==0)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap12()):0));
    ESTM(lane_st->dfe[12][0] = ((rd_rxa_dfe_tap9_mux()==1)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap9()):(rd_rxa_dfe_tap13_mux()==0)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap13()):0));
    ESTM(lane_st->dfe[12][1] = ((rd_rxb_dfe_tap9_mux()==1)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap9()):(rd_rxb_dfe_tap13_mux()==0)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap13()):0));
    ESTM(lane_st->dfe[13][0] = ((rd_rxa_dfe_tap10_mux()==1)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap10()):(rd_rxa_dfe_tap14_mux()==0)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap14()):0));
    ESTM(lane_st->dfe[13][1] = ((rd_rxb_dfe_tap10_mux()==1)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap10()):(rd_rxb_dfe_tap14_mux()==0)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap14()):0));
    ESTM(lane_st->dfe[14][0] = ((rd_rxa_dfe_tap7_mux()==2)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap7()):(rd_rxa_dfe_tap11_mux()==1)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap11()):0));
    ESTM(lane_st->dfe[14][1] = ((rd_rxb_dfe_tap7_mux()==2)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap7()):(rd_rxb_dfe_tap11_mux()==1)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap11()):0));
    ESTM(lane_st->dfe[15][0] = ((rd_rxa_dfe_tap8_mux()==2)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap8()):(rd_rxa_dfe_tap12_mux()==1)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap12()):0));
    ESTM(lane_st->dfe[15][1] = ((rd_rxb_dfe_tap8_mux()==2)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap8()):(rd_rxb_dfe_tap12_mux()==1)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap12()):0));
    ESTM(lane_st->dfe[16][0] = ((rd_rxa_dfe_tap9_mux()==2)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap9()):(rd_rxa_dfe_tap13_mux()==1)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap13()):0));
    ESTM(lane_st->dfe[16][1] = ((rd_rxb_dfe_tap9_mux()==2)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap9()):(rd_rxb_dfe_tap13_mux()==1)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap13()):0));
    ESTM(lane_st->dfe[17][0] = ((rd_rxa_dfe_tap10_mux()==2)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap10()):(rd_rxa_dfe_tap14_mux()==1)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap14()):0));
    ESTM(lane_st->dfe[17][1] = ((rd_rxb_dfe_tap10_mux()==2)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap10()):(rd_rxb_dfe_tap14_mux()==1)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap14()):0));
    ESTM(lane_st->dfe[18][0] = ((rd_rxa_dfe_tap7_mux()==3)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap7()):(rd_rxa_dfe_tap11_mux()==2)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap11()):0));
    ESTM(lane_st->dfe[18][1] = ((rd_rxb_dfe_tap7_mux()==3)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap7()):(rd_rxb_dfe_tap11_mux()==2)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap11()):0));
    ESTM(lane_st->dfe[19][0] = ((rd_rxa_dfe_tap8_mux()==3)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap8()):(rd_rxa_dfe_tap12_mux()==2)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap12()):0));
    ESTM(lane_st->dfe[19][1] = ((rd_rxb_dfe_tap8_mux()==3)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap8()):(rd_rxb_dfe_tap12_mux()==2)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap12()):0));
    ESTM(lane_st->dfe[20][0] = ((rd_rxa_dfe_tap9_mux()==3)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap9()):(rd_rxa_dfe_tap13_mux()==2)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap13()):0));
    ESTM(lane_st->dfe[20][1] = ((rd_rxb_dfe_tap9_mux()==3)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap9()):(rd_rxb_dfe_tap13_mux()==2)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap13()):0));
    ESTM(lane_st->dfe[21][0] = ((rd_rxa_dfe_tap10_mux()==3)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap10()):(rd_rxa_dfe_tap14_mux()==2)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap14()):0));
    ESTM(lane_st->dfe[21][1] = ((rd_rxb_dfe_tap10_mux()==3)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap10()):(rd_rxb_dfe_tap14_mux()==2)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap14()):0));
    ESTM(lane_st->dfe[22][0] = ((rd_rxa_dfe_tap11_mux()==3)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap11()):0));
    ESTM(lane_st->dfe[22][1] = ((rd_rxb_dfe_tap11_mux()==3)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap11()):0));
    ESTM(lane_st->dfe[23][0] = ((rd_rxa_dfe_tap12_mux()==3)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap12()):0));
    ESTM(lane_st->dfe[23][1] = ((rd_rxb_dfe_tap12_mux()==3)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap12()):0));
    ESTM(lane_st->dfe[24][0] = ((rd_rxa_dfe_tap13_mux()==3)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap13()):0));
    ESTM(lane_st->dfe[24][1] = ((rd_rxb_dfe_tap13_mux()==3)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap13()):0));
    ESTM(lane_st->dfe[25][0] = ((rd_rxa_dfe_tap14_mux()==3)?DFE_TAP_SGB2SG(rd_rxa_dfe_tap14()):0));
    ESTM(lane_st->dfe[25][1] = ((rd_rxb_dfe_tap14_mux()==3)?DFE_TAP_SGB2SG(rd_rxb_dfe_tap14()):0));

    ESTM(lane_st->thctrl_d[0][0] = rd_rxa_slicer_offset_adj_cal_d0());
    ESTM(lane_st->thctrl_d[0][1] = rd_rxb_slicer_offset_adj_cal_d0());

    ESTM(lane_st->thctrl_d[1][0] = rd_rxa_slicer_offset_adj_cal_d1());
    ESTM(lane_st->thctrl_d[1][1] = rd_rxb_slicer_offset_adj_cal_d1());

    ESTM(lane_st->thctrl_d[2][0] = rd_rxa_slicer_offset_adj_cal_d2());
    ESTM(lane_st->thctrl_d[2][1] = rd_rxb_slicer_offset_adj_cal_d2());

    ESTM(lane_st->thctrl_d[3][0] = rd_rxa_slicer_offset_adj_cal_d3());
    ESTM(lane_st->thctrl_d[3][1] = rd_rxb_slicer_offset_adj_cal_d3());

    ESTM(lane_st->thctrl_d[4][0] = rd_rxa_slicer_offset_adj_cal_d4());
    ESTM(lane_st->thctrl_d[4][1] = rd_rxb_slicer_offset_adj_cal_d4());

    ESTM(lane_st->thctrl_d[5][0] = rd_rxa_slicer_offset_adj_cal_d5());
    ESTM(lane_st->thctrl_d[5][1] = rd_rxb_slicer_offset_adj_cal_d5());

    ESTM(lane_st->thctrl_p[0][0] = rd_rxa_slicer_offset_adj_cal_p0());
    ESTM(lane_st->thctrl_p[0][1] = rd_rxb_slicer_offset_adj_cal_p0());

    ESTM(lane_st->thctrl_p[1][0] = rd_rxa_slicer_offset_adj_cal_p1());
    ESTM(lane_st->thctrl_p[1][1] = rd_rxb_slicer_offset_adj_cal_p1());

    ESTM(lane_st->thctrl_p[2][0] = rd_rxa_slicer_offset_adj_cal_p2());
    ESTM(lane_st->thctrl_p[2][1] = rd_rxb_slicer_offset_adj_cal_p2());

    ESTM(lane_st->thctrl_l[0]  = rd_rxa_slicer_offset_adj_cal_lms());
    ESTM(lane_st->thctrl_l[1]  = rd_rxb_slicer_offset_adj_cal_lms());


    ESTM(lane_st->pam4_chn_loss = rdv_usr_ctrl_pam4_chn_loss());
    ESTM(lane_st->ams_rx_sd_cal_pos      = rd_ams_rx_sigdet_offset_correction_pos());
    ESTM(lane_st->ams_rx_sd_cal_neg      = rd_ams_rx_sigdet_offset_correction_neg());
#if defined(rd_ams_rx_status_pon_vga2)
    ESTM(lane_st->ams_rx_vga0_rescal_mux = rd_ams_rx_status_pon_vga1());
    ESTM(lane_st->ams_rx_vga1_rescal_mux = rd_ams_rx_status_pon_vga2());
#else
    ESTM(lane_st->ams_rx_vga0_rescal_mux = rd_ams_rx_status_pon_vga0());
    ESTM(lane_st->ams_rx_vga1_rescal_mux = rd_ams_rx_status_pon_vga1());
#endif
    ESTM(lane_st->ams_rx_vga2_cm = rd_ams_rx_vga2_cm());
    ESTM(lane_st->ams_rx_eq2_hibw = rd_ams_rx_eq2_hibw());
    EFUN(blackhawk7_l8p2_INTERNAL_read_cal_error(sa__, &lane_st->lms_cal_err, &lane_st->phase_cal_err, &lane_st->data_cal_err, &lane_st->max_data_cal_err));
    EFUN(blackhawk7_l8p2_INTERNAL_read_res_error(sa__, lane_st->res_dac4ck_err, lane_st->sgn_chn_cnt));
    EFUN(blackhawk7_l8p2_INTERNAL_get_bwr(sa__, &lane_st->VGA1_bwr, &lane_st->VGA2_bwr, &lane_st->EQ1_bwr, &lane_st->EQ2_bwr));
    ESTM(lane_st->vga_dis_ind = rd_ams_rx_vga_dis_ind());
    ESTM(lane_st->dac4ck_cal_fail          = rdv_ext_status_word() & 0x0001);
    ESTM(lane_st->eye_margin_fail          = (uint8_t)((rdv_ext_status_word() & 0x0002)>>1));
    ESTM(lane_st->dac4ck_monotonicity_fail = (uint8_t)((rdv_ext_status_word() & 0x0004)>>2));
    ESTM(lane_st->hoffset_rail_out         = (uint8_t)((rdv_ext_status_word() & 0x0078)>>3));
    ESTM(lane_st->reset_calibrated_dac4ck  = (uint8_t)((rdv_ext_status_word() & 0x0080)>>7));
    ESTM(lane_st->blw_gain = rd_blw_gain());
    ESTM(lane_st->dummy4 = rdv_usr_status_eq_debug1());
    ESTM(lane_st->dummy5 = rdv_usr_status_eq_debug2());
    ESTM(lane_st->dummy6 = rdv_usr_status_eq_debug3());

    ESTM(lane_st->dummy7[0][0] = rdv_lhc_sts_0_0());
    ESTM(lane_st->dummy7[0][1] = rdv_lhc_sts_0_1());
    ESTM(lane_st->dummy7[1][0] = rdv_lhc_sts_1_0());
    ESTM(lane_st->dummy7[1][1] = rdv_lhc_sts_1_1());
    ESTM(lane_st->dummy7[2][0] = rdv_lhc_sts_2_0());
    ESTM(lane_st->dummy7[2][1] = rdv_lhc_sts_2_1());
    ESTM(lane_st->dummy7[3][0] = rdv_lhc_sts_3_0());
    ESTM(lane_st->dummy7[3][1] = rdv_lhc_sts_3_1());
    ESTM(lane_st->dummy7[4][0] = rdv_lhc_sts_4_0());
    ESTM(lane_st->dummy7[4][1] = rdv_lhc_sts_4_1());
    ESTM(lane_st->dummy7[5][0] = rdv_lhc_sts_5_0());
    ESTM(lane_st->dummy7[5][1] = rdv_lhc_sts_5_1());
    ESTM(lane_st->dummy8[0][0] = rdv_lvc_sts_0_0());
    ESTM(lane_st->dummy8[0][1] = rdv_lvc_sts_0_1());
    ESTM(lane_st->dummy8[1][0] = rdv_lvc_sts_1_0());
    ESTM(lane_st->dummy8[1][1] = rdv_lvc_sts_1_1());
    ESTM(lane_st->dummy8[2][0] = rdv_lvc_sts_2_0());
    ESTM(lane_st->dummy8[2][1] = rdv_lvc_sts_2_1());
    ESTM(lane_st->dummy8[3][0] = rdv_lvc_sts_3_0());
    ESTM(lane_st->dummy8[3][1] = rdv_lvc_sts_3_1());
    ESTM(lane_st->dummy8[4][0] = rdv_lvc_sts_4_0());
    ESTM(lane_st->dummy8[4][1] = rdv_lvc_sts_4_1());
    ESTM(lane_st->dummy8[5][0] = rdv_lvc_sts_5_0());
    ESTM(lane_st->dummy8[5][1] = rdv_lvc_sts_5_1());
    ESTM(lane_st->dummy9[0] = rdv_psv_ctl_byte());
    ESTM(lane_st->dummy9[1] = rdv_psv_sts_byte());
    ESTM(lane_st->dummy9[2] = rdv_psv_slic_sts_0_byte());
    ESTM(lane_st->dummy9[3] = rdv_psv_slic_sts_1_byte());
    ESTM(lane_st->dummy9[4] = rdv_psv_slic_sts_2_byte());
    ESTM(lane_st->dummy9[5] = rdv_psv_slic_sts_3_byte());
    ESTM(lane_st->dummy9[6] = rdv_psv_slic_sts_4_byte());
    ESTM(lane_st->dummy9[7] = rdv_psv_slic_sts_5_byte());
    ESTM(lane_st->dummy9[8] = rdv_lth_ctl_0_byte());
    ESTM(lane_st->dummy9[9] = rdv_lth_ctl_1_byte());
    ESTM(lane_st->dummy10[0][0] = rdv_lc_sts_0_0_byte());
    ESTM(lane_st->dummy10[0][1] = rdv_lc_sts_0_1_byte());
    ESTM(lane_st->dummy10[1][0] = rdv_lc_sts_1_0_byte());
    ESTM(lane_st->dummy10[1][1] = rdv_lc_sts_1_1_byte());
    ESTM(lane_st->dummy10[2][0] = rdv_lc_sts_2_0_byte());
    ESTM(lane_st->dummy10[2][1] = rdv_lc_sts_2_1_byte());
    ESTM(lane_st->dummy10[3][0] = rdv_lc_sts_3_0_byte());
    ESTM(lane_st->dummy10[3][1] = rdv_lc_sts_3_1_byte());
    ESTM(lane_st->dummy10[4][0] = rdv_lc_sts_4_0_byte());
    ESTM(lane_st->dummy10[4][1] = rdv_lc_sts_4_1_byte());
    ESTM(lane_st->dummy10[5][0] = rdv_lc_sts_5_0_byte());
    ESTM(lane_st->dummy10[5][1] = rdv_lc_sts_5_1_byte());
    ESTM(lane_st->dummy11[0][0] = rdv_lhr_sts_0_0());
    ESTM(lane_st->dummy11[0][1] = rdv_lhr_sts_0_1());
    ESTM(lane_st->dummy11[1][0] = rdv_lhr_sts_1_0());
    ESTM(lane_st->dummy11[1][1] = rdv_lhr_sts_1_1());
    ESTM(lane_st->dummy11[2][0] = rdv_lhr_sts_2_0());
    ESTM(lane_st->dummy11[2][1] = rdv_lhr_sts_2_1());
    ESTM(lane_st->dummy11[3][0] = rdv_lhr_sts_3_0());
    ESTM(lane_st->dummy11[3][1] = rdv_lhr_sts_3_1());
    ESTM(lane_st->dummy11[4][0] = rdv_lhr_sts_4_0());
    ESTM(lane_st->dummy11[4][1] = rdv_lhr_sts_4_1());
    ESTM(lane_st->dummy11[5][0] = rdv_lhr_sts_5_0());
    ESTM(lane_st->dummy11[5][1] = rdv_lhr_sts_5_1());

    ESTM(lane_st->dummy12[0][0] = rdv_lvr_sts_0_0());
    ESTM(lane_st->dummy12[0][1] = rdv_lvr_sts_0_1());
    ESTM(lane_st->dummy12[1][0] = rdv_lvr_sts_1_0());
    ESTM(lane_st->dummy12[1][1] = rdv_lvr_sts_1_1());
    ESTM(lane_st->dummy12[2][0] = rdv_lvr_sts_2_0());
    ESTM(lane_st->dummy12[2][1] = rdv_lvr_sts_2_1());
    ESTM(lane_st->dummy12[3][0] = rdv_lvr_sts_3_0());
    ESTM(lane_st->dummy12[3][1] = rdv_lvr_sts_3_1());
    ESTM(lane_st->dummy12[4][0] = rdv_lvr_sts_4_0());
    ESTM(lane_st->dummy12[4][1] = rdv_lvr_sts_4_1());
    ESTM(lane_st->dummy12[5][0] = rdv_lvr_sts_5_0());
    ESTM(lane_st->dummy12[5][1] = rdv_lvr_sts_5_1());


    ESTM(lane_st->dummy13 = rdv_usr_status_eq_debug4());

    ESTM(lane_st->dummy14    = rdv_usr_status_eq_debug10());
    ESTM(lane_st->dummy15[0] = rdv_usr_status_eq_debug9());
    ESTM(lane_st->dummy15[1] = rdv_usr_status_eq_debug8());
    ESTM(lane_st->dummy16    = rdv_usr_status_eq_debug7());
    ESTM(lane_st->dummy17[0] = rdv_usr_status_eq_debug6());
    ESTM(lane_st->dummy17[1] = rdv_usr_status_eq_debug5());
    ESTM(lane_st->dummy18[0] = rdv_usr_status_tp_metric_1());
    ESTM(lane_st->dummy18[1] = rdv_usr_status_tp_metric_2());
    ESTM(lane_st->dummy18[2] = rdv_usr_status_tp_metric_3());
    ESTM(lane_st->dummy19    = rdv_usr_status_eq_debug14());
    ESTM(lane_st->dummy20[0] = rdv_usr_status_pam4_fl_cdr_0());
    ESTM(lane_st->dummy20[1] = rdv_usr_status_pam4_fl_cdr_1());
    ESTM(lane_st->dummy21[0] = rdv_usr_status_pam4_fl_cdr_2());
    ESTM(lane_st->dummy21[1] = rdv_usr_status_pam4_fl_cdr_3());
    EFUN(blackhawk7_l8p2_INTERNAL_get_eye_margin_est(sa__, &lane_st->heye_left, &lane_st->heye_right, &lane_st->veye_upper, &lane_st->veye_lower));
    {
        uint32_t link_time32b = 0;
        EFUN(blackhawk7_l8p2_INTERNAL_get_link_time(sa__, &link_time32b));
        lane_st->link_time = (uint16_t)link_time32b;
    }

    if (lane_st->pmd_lock == 1) {
      if (!lane_st->stop_state) {
        EFUN(blackhawk7_l8p2_stop_rx_adaptation(sa__, 0));
      }
    } else {
        EFUN(blackhawk7_l8p2_stop_rx_adaptation(sa__, 0));
    }

    return(ERR_CODE_NONE);
}

err_code_t blackhawk7_l8p2_log_full_pmd_state (srds_access_t *sa__, struct blackhawk7_l8p2_detailed_lane_status_st *lane_st) {
    INIT_SRDS_ERR_CODE
    enum srds_prbs_polynomial_enum prbs_poly_mode = PRBS_7;
    enum srds_prbs_checker_mode_enum prbs_checker_mode = PRBS_SELF_SYNC_HYSTERESIS;

    if(!lane_st) {
      return(blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    ENULL_MEMSET(lane_st, 0, sizeof(struct blackhawk7_l8p2_detailed_lane_status_st));
    EFUN(blackhawk7_l8p2_log_full_pmd_state_noPRBS(sa__, lane_st));

    ESTM(lane_st->prbs_chk_en = rd_prbs_chk_en());
    EFUN(blackhawk7_l8p2_get_rx_prbs_config(sa__, &prbs_poly_mode, &prbs_checker_mode, &lane_st->prbs_chk_inv));
    lane_st->prbs_chk_order = (uint8_t)prbs_poly_mode;

    EFUN(blackhawk7_l8p2_prbs_chk_lock_state(sa__, &lane_st->prbs_chk_lock));
    EFUN(blackhawk7_l8p2_prbs_err_count_ll(sa__, &lane_st->prbs_chk_errcnt));
    if (lane_st->prbs_chk_lock && (lane_st->rx_pam_mode != NRZ)) {
       EFUN(blackhawk7_l8p2_collect_dbgfb_stats(sa__, 8000, &lane_st->dbgfb_stats));
    }
    EFUN(blackhawk7_l8p2_INTERNAL_get_BER_string(sa__,100,&lane_st->ber_string[0], MEMBER_SIZE(struct blackhawk7_l8p2_detailed_lane_status_st, ber_string)));

    return(ERR_CODE_NONE);
}


err_code_t blackhawk7_l8p2_disp_full_pmd_state (srds_access_t *sa__, struct blackhawk7_l8p2_detailed_lane_status_st const * const lane_st, uint8_t num_lanes) {
    INIT_SRDS_ERR_CODE
    const uint8_t num_bytes_each_line = 26;
    uint32_t i;
    uint32_t size_of_lane_st = 0;

    const uint8_t big_endian = blackhawk7_l8p2_INTERNAL_is_big_endian();

    if(lane_st == NULL) {
        return (blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if((num_lanes != 1) && (num_lanes != 2) && (num_lanes != 4) && (num_lanes != 8)) {
        return(blackhawk7_l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT)); /* Number of lanes has to be 1, 2, 4, or 8 */
    }

      size_of_lane_st = sizeof(struct blackhawk7_l8p2_detailed_lane_status_st);

      EFUN_PRINTF(("\n**** SERDES EXTENDED LANE DIAG DATA CORE %d LANE %d DUMP ****", blackhawk7_l8p2_acc_get_core(sa__), blackhawk7_l8p2_acc_get_lane(sa__)));
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

#endif /* ! SMALL_FOOTPRINT */


/*
 * for backtrace() support
 */


void blackhawk7_l8p2_INTERNAL_print_triage_info(srds_access_t *sa__, err_code_t err_code, uint8_t print_header, uint8_t print_data, uint16_t line)
{
    /*  Note: No EFUNs or ESTMs should be used in this function as this print routine is called by _error() handler. */
#if defined(SMALL_FOOTPRINT)
    return;
#else
    blackhawk7_l8p2_triage_info info;
    INIT_SRDS_ERR_CODE
    uint16_t   ucode_version_major;
    uint8_t    ucode_version_minor, error_seen = 0;

    if (ERR_CODE_SRDS_REG_ACCESS_FAIL == err_code) {  /* Early return to prevent error handling recursion on Access Errors! */
        return;
    }

    USR_MEMSET(&info, 0, sizeof(struct blackhawk7_l8p2_triage_info_st));
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

    if (blackhawk7_l8p2_version(sa__, &info.api_ver)) {  /* Unable to read api version */
        info.api_ver = 0xFFFFFFFF;
        error_seen = 1;
    }
    CHECK_ERR(ucode_version_major = rdcv_common_ucode_version());
    CHECK_ERR(ucode_version_minor = rdcv_common_ucode_minor_version());
    info.ucode_ver = (uint32_t)((ucode_version_major << 8) | ucode_version_minor);
    CHECK_ERR(info.stop_status = rdv_usr_sts_micro_stopped());

    /* Collect exception and overflow information */
    CHECK_ERR(info.stack_overflow   = rdc_micro_status_stack_overflowed());
    CHECK_ERR(info.overflow_lane_id = rdc_micro_status_stack_overflowed_laneID());
    CHECK_ERR(info.sw_exception     = rdc_micro_status_sw_exception_occurred());
    CHECK_ERR(info.hw_exception     = rdc_micro_status_hw_exception_occurred());
    if(blackhawk7_l8p2_INTERNAL_sigdet_status(sa__, &info.sig_det, &info.sig_det_chg)) {
        error_seen = 1;
    }
    if (blackhawk7_l8p2_INTERNAL_pmd_lock_status(sa__, &info.pmd_lock, &info.pmd_lock_chg)) {
        error_seen = 1;
    }

    CHECK_ERR(info.dsc_one_hot[0] = rd_dsc_state_one_hot());
    CHECK_ERR(info.dsc_one_hot[1] = rd_dsc_state_one_hot());
    CHECK_ERR(info.cmd_info = reg_rd_DSC_A_DSC_UC_CTRL());
    info.core = blackhawk7_l8p2_acc_get_core(sa__);
    info.lane = blackhawk7_l8p2_acc_get_lane(sa__);
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
                        blackhawk7_l8p2_INTERNAL_e2s_err_code(info.error) ));
        } else {
            USR_PRINTF(("%4d, %4d,  %X_%X,     %X_%X, %s\n",
                        info.lane,
                        info.core,
                        info.api_ver>>8,
                        info.api_ver & 0xFF,
                        info.ucode_ver >> 8,
                        info.ucode_ver & 0xFF,
                        blackhawk7_l8p2_INTERNAL_e2s_err_code(info.error) ));
        }
    }
    if (error_seen) {
        USR_PRINTF(("WARNING: There were some errors seen while collecting triage info and so the debug data above may not be all accurate\n"));
    }
    return;
#endif /* SMALL_FOOTPRINT */
}


