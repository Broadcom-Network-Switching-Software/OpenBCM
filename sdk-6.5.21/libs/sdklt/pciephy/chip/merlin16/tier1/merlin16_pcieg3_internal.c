/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  merlin16_pcieg3_internal.c                                         *
 *  Created On    :  13/02/2014                                                    *
 *  Created By    :  Justin Gaither                                                *
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

/** @file merlin16_pcieg3_internal.c
 * Implementation of API functions
 */

#include "merlin16_pcieg3_internal.h"
#include "merlin16_pcieg3_internal_error.h"
#include "merlin16_pcieg3_access.h"
#include "merlin16_pcieg3_common.h"
#include "merlin16_pcieg3_config.h"
#include "merlin16_pcieg3_functions.h"
#include "merlin16_pcieg3_select_defns.h"
#include "merlin16_pcieg3_prbs.h"

#define SPC_LEFT(_buf) (sizeof(_buf) - USR_STRLEN(_buf) - 1)

/* If SERDES_EVAL is defined, then is_ate_log_enabled() is queried to *\
\* know whether to log ATE.  merlin16_pcieg3_access.h provides that function.  */


static merlin16_pcieg3_info_t merlin16_pcieg3_info;

merlin16_pcieg3_info_t *merlin16_pcieg3_INTERNAL_get_merlin16_pcieg3_info_ptr(void) {
    return &merlin16_pcieg3_info;
}

err_code_t merlin16_pcieg3_INTERNAL_verify_merlin16_pcieg3_info(merlin16_pcieg3_info_t const *test_info)
{
    if (test_info->signature != SRDS_INFO_SIGNATURE) {
        EFUN_PRINTF(("ERROR:  Mismatch in merlin16_pcieg3_info signature.  Expected 0x%08X, but received 0x%08X.\n",
                     SRDS_INFO_SIGNATURE, test_info->signature));
        return (_error(ERR_CODE_INFO_TABLE_ERROR));
    }
    return (ERR_CODE_NONE);
}

/* Store a cached AFE version for re-use */
err_code_t merlin16_pcieg3_INTERNAL_get_afe_hw_version(srds_access_t *sa__, uint8_t *afe_hw_version) {
  static uint8_t _cached_afe_hw_version = 255;
  
  if (!afe_hw_version)
    return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  if (_cached_afe_hw_version == 255)
    ESTM(_cached_afe_hw_version = rdcv_afe_hardware_version());
  *afe_hw_version = _cached_afe_hw_version;
  return(ERR_CODE_NONE);
}

static int merlin16_pcieg3_osr_mode_enum_to_int_x1000(uint8_t osr_mode) {
    switch(osr_mode) {
    case MERLIN16_PCIEG3_OSX1:      return 1000;
    case MERLIN16_PCIEG3_OSX2:      return 2000;
    case MERLIN16_PCIEG3_OSX4:      return 4000;
    case MERLIN16_PCIEG3_OSX3:      return 3000;
    case MERLIN16_PCIEG3_OSX3P3:    return 3300;
    case MERLIN16_PCIEG3_OSX5:      return 5000;
    case MERLIN16_PCIEG3_OSX7P5:    return 7500;
    case MERLIN16_PCIEG3_OSX8P25:   return 8250;
    case MERLIN16_PCIEG3_OSX10:     return 10000;
    case MERLIN16_PCIEG3_OSX8:      return 8000;
    default:                    return 1000;
    }
}

err_code_t merlin16_pcieg3_INTERNAL_get_num_bits_per_ms(srds_access_t *sa__, uint32_t *num_bits_per_ms) {
    uint8_t osr_mode = 0;
#if defined(rd_rx_pam4_mode)
    uint8_t pam4_mode = 0;
#endif
    struct merlin16_pcieg3_uc_core_config_st core_config = {{0}};

    ESTM(osr_mode = rd_rx_osr_mode());

#if defined(rd_rx_pam4_mode)
    ESTM(pam4_mode = rd_rx_pam4_mode());
#endif

        *num_bits_per_ms = (((uint32_t)core_config.vco_rate_in_Mhz * 100000UL) / merlin16_pcieg3_osr_mode_enum_to_int_x1000(osr_mode))*10;

#if defined(rd_rx_pam4_mode)
    if(pam4_mode > 0) *num_bits_per_ms <<= 1; 
#endif
    return(ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_display_BER(srds_access_t *sa__, uint16_t time_ms) {
    char string[10];
    EFUN(merlin16_pcieg3_INTERNAL_get_BER_string(sa__,time_ms,string));
    USR_PRINTF(("%s",string));
    return(ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_get_BER_string(srds_access_t *sa__, uint16_t time_ms, char *string) {
    char string2[3];
    struct ber_data_st ber_data = {0, 0, 0, 0};

    if(string == NULL) {
        return(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }

    EFUN(merlin16_pcieg3_INTERNAL_get_BER_data(sa__, time_ms, &ber_data));

    if(ber_data.prbs_chk_en == 0) {
        USR_STRCPY(string,"");
        return(ERR_CODE_NONE);
    }

    if((ber_data.num_errs < 3) && (ber_data.lcklost == 0)) {   /* lcklost = 0 */
        USR_STRCPY(string2, " <");
        ber_data.num_errs = 3;
    } else {
        USR_STRCPY(string2, "  ");
    }
    if(ber_data.lcklost == 1) {    /* lcklost = 1 */
        USR_STRCPY(string,"  !Lock ");
    } else {                    /* lcklost = 0 */
        uint16_t x=0,y=0,z=0,d;

        if(ber_data.num_errs >= ber_data.num_bits) {
            x = 1;y=0;z=0;
        }else {
            while(1) {
                d = (uint16_t)(((ber_data.num_errs<<1) + ber_data.num_bits)/(ber_data.num_bits<<1));
                if(d>=10) break;
                ber_data.num_errs = ber_data.num_errs*10;
                z=z+1;
            }
            if(d>=100) {
                d = d/10;
                z=z-1;
            }
            x=d/10;
            y = d - 10*x;
            z=z-1;
            USR_SPRINTF((string,"%s%d.%1de-%02d",string2,x,y,z));
        }

    }
    return(ERR_CODE_NONE);

}

err_code_t merlin16_pcieg3_INTERNAL_get_BER_data(srds_access_t *sa__, uint16_t time_ms, struct ber_data_st *ber_data) {
    uint8_t lcklost = 0;
    uint32_t err_cnt= 0;

    if(ber_data == NULL) {
        return(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }

    ESTM(ber_data->prbs_chk_en = rd_prbs_chk_en());
    if(ber_data->prbs_chk_en == 0)
        return(ERR_CODE_NONE);

    EFUN(merlin16_pcieg3_prbs_err_count_state(sa__, &err_cnt,&lcklost)); /* clear error counters */
    EFUN(USR_DELAY_MS(time_ms));
    EFUN(merlin16_pcieg3_prbs_err_count_state(sa__, &err_cnt,&lcklost)); 

    ber_data->lcklost = lcklost;

    if(ber_data->lcklost == 0) {
        uint32_t num_bits_per_ms=0;

        EFUN(merlin16_pcieg3_INTERNAL_get_num_bits_per_ms(sa__,&num_bits_per_ms));

        ber_data->num_errs = err_cnt;
        ber_data->num_bits = (uint64_t)num_bits_per_ms*(uint64_t)time_ms;
    }
    return(ERR_CODE_NONE);

}


err_code_t merlin16_pcieg3_INTERNAL_get_p1_threshold(srds_access_t *sa__, int8_t *val) {
 ESTM(*val = -rd_p1_eyediag_bin()); 
 return (ERR_CODE_NONE);
}


int16_t merlin16_pcieg3_INTERNAL_ladder_setting_to_mV(srds_access_t *sa__, int8_t ctrl, uint8_t range_250) {
    uint16_t absv;                                    /* Absolute value of ctrl */
    int16_t nlmv;                                     /* Non-linear value */
    
    /* Get absolute value */
    absv = SRDS_ABS(ctrl);

    { 
        if (range_250) {
            /* 28nm range is 250mV, but 16nm range is 200mV */
            /* 200mV range, 6.5519mV linear units */
            nlmv = absv*190/29;
        }
        else {
            /* 120mV range, 3.8854mV linear units */
            nlmv = absv*136/35;
        }
    }
    /* Add sign and return */
    return( (ctrl>=0) ? nlmv : -nlmv);
}


err_code_t merlin16_pcieg3_INTERNAL_compute_bin(char var, char bin[]) {
    if(!bin) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

  switch (var) {
    case '0':  ENULL_STRCPY(bin,"0000");
               break;
    case '1':  ENULL_STRCPY(bin,"0001");
               break;
    case '2':  ENULL_STRCPY(bin,"0010");
               break;
    case '3':  ENULL_STRCPY(bin,"0011");
               break;
    case '4':  ENULL_STRCPY(bin,"0100");
               break;
    case '5':  ENULL_STRCPY(bin,"0101");
               break;
    case '6':  ENULL_STRCPY(bin,"0110");
               break;
    case '7':  ENULL_STRCPY(bin,"0111");
               break;
    case '8':  ENULL_STRCPY(bin,"1000");
               break;
    case '9':  ENULL_STRCPY(bin,"1001");
               break;
    case 'a':
    case 'A':  ENULL_STRCPY(bin,"1010");
               break;
    case 'b':
    case 'B':  ENULL_STRCPY(bin,"1011");
               break;
    case 'c':
    case 'C':  ENULL_STRCPY(bin,"1100");
               break;
    case 'd':
    case 'D':  ENULL_STRCPY(bin,"1101");
               break;
    case 'e':
    case 'E':  ENULL_STRCPY(bin,"1110");
               break;
    case 'f':
    case 'F':  ENULL_STRCPY(bin,"1111");
               break;
    case '_':  ENULL_STRCPY(bin,"");
               break;
    default :  ENULL_STRCPY(bin,"");
               EFUN_PRINTF(("ERROR: Invalid Hexadecimal Pattern\n"));
               return (_error(ERR_CODE_CFG_PATT_INVALID_HEX));
  }
  return (ERR_CODE_NONE);
}


err_code_t merlin16_pcieg3_INTERNAL_compute_hex(char bin[],uint8_t *hex) {
  if(!hex) {
    return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
 
  if (!USR_STRCMP(bin,"0000")) {
    *hex = 0x0;
  } 
  else if (!USR_STRCMP(bin,"0001")) {
    *hex = 0x1;
  } 
  else if (!USR_STRCMP(bin,"0010")) {
    *hex = 0x2;
  } 
  else if (!USR_STRCMP(bin,"0011")) {
    *hex = 0x3;
  } 
  else if (!USR_STRCMP(bin,"0100")) {
    *hex = 0x4;
  } 
  else if (!USR_STRCMP(bin,"0101")) {
    *hex = 0x5;
  } 
  else if (!USR_STRCMP(bin,"0110")) {
    *hex = 0x6;
  } 
  else if (!USR_STRCMP(bin,"0111")) {
    *hex = 0x7;
  } 
  else if (!USR_STRCMP(bin,"1000")) {
    *hex = 0x8;
  } 
  else if (!USR_STRCMP(bin,"1001")) {
    *hex = 0x9;
  } 
  else if (!USR_STRCMP(bin,"1010")) {
    *hex = 0xA;
  } 
  else if (!USR_STRCMP(bin,"1011")) {
    *hex = 0xB;
  } 
  else if (!USR_STRCMP(bin,"1100")) {
    *hex = 0xC;
  } 
  else if (!USR_STRCMP(bin,"1101")) {
    *hex = 0xD;
  } 
  else if (!USR_STRCMP(bin,"1110")) {
    *hex = 0xE;
  } 
  else if (!USR_STRCMP(bin,"1111")) {
    *hex = 0xF;
  } 
  else {
    EFUN_PRINTF(("ERROR: Invalid Binary to Hex Conversion\n"));
    *hex = 0x0;
    return (_error(ERR_CODE_CFG_PATT_INVALID_BIN2HEX));
  } 
  return (ERR_CODE_NONE);
}

uint8_t merlin16_pcieg3_INTERNAL_stop_micro(srds_access_t *sa__, uint8_t graceful, err_code_t *err_code_p) {
   uint8_t stop_state = 0;

   if(!err_code_p) {
       return(0xFF);
   }

   /* Log current micro stop status */
   EPSTM2((stop_state = rdv_usr_sts_micro_stopped()),0xFF);

   if(graceful) {
       if (!(stop_state & 0x7F)) {
           EPFUN2((merlin16_pcieg3_stop_rx_adaptation(sa__, 1)),0xFF);
       }
   } else {
       EPFUN2((merlin16_pcieg3_pmd_uc_control(sa__, CMD_UC_CTRL_STOP_IMMEDIATE,GRACEFUL_STOP_TIME)),0xFF);
   }

   /* Return the previous micro stop status */
   return(stop_state);
}


  

/********************************************************/
/*  Global RAM access through Micro Register Interface  */
/********************************************************/
/* Micro Global RAM Byte Read */
uint8_t merlin16_pcieg3_INTERNAL_rdb_uc_var(srds_access_t *sa__, err_code_t *err_code_p, uint16_t addr) {
    uint8_t rddata;

    if(!err_code_p) {
        return(0);
    }
    EPSTM(rddata = merlin16_pcieg3_rdb_uc_ram(sa__, err_code_p, addr)); /* Use Micro register interface for reading RAM */
    return (rddata);
}

/* Micro Global RAM Word Read */
uint16_t merlin16_pcieg3_INTERNAL_rdw_uc_var(srds_access_t *sa__, err_code_t *err_code_p, uint16_t addr) {
  uint16_t rddata;

  if(!err_code_p) {
      return(0);
  }
  if (addr%2 != 0) {                                         /* Validate even address */
      *err_code_p = ERR_CODE_INVALID_RAM_ADDR;
      USR_PRINTF(("Error incorrect addr x%04x\n",addr));
      return (0);
  }
  EPSTM(rddata = merlin16_pcieg3_rdw_uc_ram(sa__, err_code_p, (addr))); /* Use Micro register interface for reading RAM */
  return (rddata);
}

/* Micro Global RAM Byte Write  */
err_code_t merlin16_pcieg3_INTERNAL_wrb_uc_var(srds_access_t *sa__, uint16_t addr, uint8_t wr_val) {

    return (merlin16_pcieg3_wrb_uc_ram(sa__, addr, wr_val));          /* Use Micro register interface for writing RAM */
}

/* Micro Global RAM Word Write  */
err_code_t merlin16_pcieg3_INTERNAL_wrw_uc_var(srds_access_t *sa__, uint16_t addr, uint16_t wr_val) {
    if (addr%2 != 0) {                                       /* Validate even address */
      USR_PRINTF(("Error incorrect addr x%04x\n",addr));
        return (_error(ERR_CODE_INVALID_RAM_ADDR));
    }
    return (merlin16_pcieg3_wrw_uc_ram(sa__, addr, wr_val));          /* Use Micro register interface for writing RAM */
}


/***********************/
/*  Event Log Display  */
/***********************/
err_code_t merlin16_pcieg3_INTERNAL_event_log_dump_callback(void *arg, uint8_t byte_count, uint16_t data) {
    merlin16_pcieg3_INTERNAL_event_log_dump_state_t * const state_ptr = (merlin16_pcieg3_INTERNAL_event_log_dump_state_t *)arg;
    const uint8_t bytes_per_row=16;

    if (byte_count == 0) {
        if (state_ptr->line_start_index != state_ptr->index) {
            EFUN_PRINTF(("%*s    %d\n", 4*(bytes_per_row - state_ptr->index + state_ptr->line_start_index), "", state_ptr->line_start_index));
        }
        return (ERR_CODE_NONE);
    }
    if (byte_count == 1) {
        /* There is a trailing byte in the event log.
         * The simplest way to handle it is to print out a whole word, but mask
         * the invalid upper byte.
         */
        data &= 0xFF;
    }
    EFUN_PRINTF(("  0x%04x", ((data & 0xFF) << 8) | (data >> 8)));
    state_ptr->index += 2;
    if (state_ptr->index % bytes_per_row == 0) {
        EFUN_PRINTF(("    %d\n", state_ptr->line_start_index));
        state_ptr->line_start_index = state_ptr->index;
    }

    return(ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_read_event_log_with_callback(srds_access_t *sa__,
                                                        uint8_t micro_num,
                                                        uint8_t bypass_micro,
                                                        void *arg,
                                                        err_code_t (*callback)(void *, uint8_t, uint16_t)) {
    merlin16_pcieg3_info_t const * const merlin16_pcieg3_info_ptr = merlin16_pcieg3_INTERNAL_get_merlin16_pcieg3_info_ptr();
    uint16_t rd_idx;
    uint8_t current_lane;

    EFUN(merlin16_pcieg3_INTERNAL_verify_merlin16_pcieg3_info(merlin16_pcieg3_info_ptr));

    if (micro_num >= merlin16_pcieg3_info_ptr->micro_count) {
        return (_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    /* Read Current lane so that it can be restored at the end of function */
    current_lane = merlin16_pcieg3_get_lane(sa__);
    EFUN_PRINTF(("\n\n********************************************\n"));
    EFUN_PRINTF((    "**** SERDES UC TRACE MEMORY DUMP ***********\n"));
    EFUN_PRINTF((    "********************************************\n"));

    if (bypass_micro) {
        ESTM(rd_idx = rdcv_trace_mem_wr_idx());
        if (merlin16_pcieg3_info_ptr->trace_memory_descending_writes) {
            ++rd_idx;
            if (rd_idx >= merlin16_pcieg3_info_ptr->trace_mem_ram_size) {
                rd_idx = 0;
            }
        } else {
            if (rd_idx == 0) {
                rd_idx = merlin16_pcieg3_info_ptr->trace_mem_ram_size;
            }
            --rd_idx;
        }
    } else {
        /* Start Read to stop uC logging and read the word at last event written by uC */
        EFUN(merlin16_pcieg3_pmd_uc_cmd(sa__, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_START, GRACEFUL_STOP_TIME));
        ESTM(rd_idx = rdcv_trace_mem_rd_idx());
    }
    
    EFUN_PRINTF(( "\n  DEBUG INFO: trace memory read index = 0x%04x\n", rd_idx));
    
    EFUN_PRINTF(("  DEBUG INFO: trace memory size = 0x%04x\n\n", merlin16_pcieg3_info_ptr->trace_mem_ram_size));

    if (merlin16_pcieg3_info_ptr->trace_memory_descending_writes) {
        /* Micro writes trace memory using descending addresses.
         * So read using ascending addresses using block read
         */
        EFUN(merlin16_pcieg3_INTERNAL_rdblk_uc_generic_ram(sa__,
                                                  merlin16_pcieg3_info_ptr->trace_mem_ram_base + merlin16_pcieg3_info_ptr->grp_ram_size*micro_num,
                                                  merlin16_pcieg3_info_ptr->trace_mem_ram_size,
                                                  rd_idx,
                                                  merlin16_pcieg3_info_ptr->trace_mem_ram_size,
                                                  arg,
                                                  callback));
    } else {
        /* Micro writes trace memory using descending addresses.
         * So read using ascending addresses using block read
         */
        EFUN(merlin16_pcieg3_INTERNAL_rdblk_uc_generic_ram_descending(sa__,
                                                             merlin16_pcieg3_info_ptr->trace_mem_ram_base + merlin16_pcieg3_info_ptr->grp_ram_size*micro_num,
                                                             merlin16_pcieg3_info_ptr->trace_mem_ram_size,
                                                             rd_idx,
                                                             merlin16_pcieg3_info_ptr->trace_mem_ram_size,
                                                             arg,
                                                             callback));
    }

    if (!bypass_micro) {
        /* Read Done to resume logging  */
        EFUN(merlin16_pcieg3_pmd_uc_cmd(sa__, CMD_EVENT_LOG_READ, CMD_EVENT_LOG_READ_DONE, 50));
    }
    EFUN(merlin16_pcieg3_set_lane(sa__,current_lane));
    return(ERR_CODE_NONE);
}

#ifdef TO_FLOATS
/* convert uint32_t to float8 */
float8_t merlin16_pcieg3_INTERNAL_int32_to_float8(uint32_t input) {
    int8_t cnt;
    uint8_t output;
    
    if(input == 0) {
      return(0);
    } else if(input == 1) {
      return(0xe0);
    } else {
      cnt = 0;
      input = input & 0x7FFFFFFF; /* get rid of MSB which may be lock indicator */
      do {
        input = input << 1;
        cnt++;
      } while ((input & 0x80000000) == 0);
    
      output = (uint8_t)((input & 0x70000000)>>23)+(31 - cnt%32);
      return(output);
    }
}
#endif

/* convert float8 to uint32_t */
uint32_t merlin16_pcieg3_INTERNAL_float8_to_int32(float8_t input) {
    uint32_t x;
    if(input == 0) return(0);
    x = (input>>5) + 8;
    if((input & 0x1F) < 3) {
      return(x>>(3-(input & 0x1f)));
    } 
    return(x<<((input & 0x1F)-3));
}

/* Convert uint8 to 8-bit gray code */
uint8_t merlin16_pcieg3_INTERNAL_uint8_to_gray(uint8_t input) {
    return input ^ (input >> 1);
}

/* Convert 8-bit gray code to uint8 */
uint8_t merlin16_pcieg3_INTERNAL_gray_to_uint8(uint8_t input) {
    input = input ^ (input >> 4);
    input = input ^ (input >> 2);
    input = input ^ (input >> 1);
    return input;
}


/* convert float12 to uint32 */
uint32_t merlin16_pcieg3_INTERNAL_float12_to_uint32(uint8_t byte, uint8_t multi) {

    return(((uint32_t)byte)<<multi);
}

#ifdef TO_FLOATS
/* convert uint32 to float12 */
uint8_t merlin16_pcieg3_INTERNAL_uint32_to_float12(uint32_t input, uint8_t *multi) {
    int8_t cnt;
    uint8_t output;
    if(!multi) {
      return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    
    if((input == 0) || (!multi)) {
      *multi = 0;
      return(0);
    } else {
      cnt = 0;
      if(input > 0x007FFFFF) input = 0x007FFFFF; /* limit to 23bits so multi is 4 bits */
      do {
        input = input << 1;
        cnt++;
      } while ((input & 0x80000000) == 0);
      
      *multi = (31 - (cnt%32));
      if(*multi < 8) {
        output = (uint8_t)((input & 0xFF000000)>>(24 + (7-*multi)));
        *multi = 0;
      } else {
        output = (uint8_t)((input & 0xFF000000)>>24);
        *multi = *multi - 7;
      }
      return(output);
    }
}
#endif

err_code_t merlin16_pcieg3_INTERNAL_set_rx_pf_main(srds_access_t *sa__, uint8_t val) {
    if (val > 15) {
      return (_error(ERR_CODE_PF_INVALID));
    }
    EFUN(WR_RX_PF_CTRL(val));
    return(ERR_CODE_NONE); 
}

err_code_t merlin16_pcieg3_INTERNAL_get_rx_pf_main(srds_access_t *sa__, int8_t *val) {
    ESTM(*val = (int8_t)RD_RX_PF_CTRL());
    return (ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_set_rx_pf2(srds_access_t *sa__, uint8_t val) {
    if (val > 7) {
      return (_error(ERR_CODE_PF_INVALID));
    }
    EFUN(WR_RX_PF2_CTRL(val));
    return(ERR_CODE_NONE); 
}

err_code_t merlin16_pcieg3_INTERNAL_get_rx_pf2(srds_access_t *sa__, int8_t *val) {
    ESTM(*val = (int8_t)RD_RX_PF2_CTRL());
    return (ERR_CODE_NONE);
}


err_code_t merlin16_pcieg3_INTERNAL_set_rx_vga(srds_access_t *sa__, uint8_t val) {

    EFUN(merlin16_pcieg3_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */

    if (val > RX_VGA_CTRL_VAL_MAX) {
      return (_error(ERR_CODE_VGA_INVALID));
    }
#if defined(RX_VGA_CTRL_VAL_MIN) && (RX_VGA_CTRL_VAL_MIN > 0)
    if (val < RX_VGA_CTRL_VAL_MIN) {
      return (_error(ERR_CODE_VGA_INVALID));
    } 
#endif
    
    {
        uint8_t rate_sel;
        ESTM(rate_sel = rd_rate_sel());
        if (rate_sel < MERLIN16_PCIEG3_GEN3) {
            if (val <= 15) {
                EFUN(wr_rx_vga1_ctrl_G1G2(val));
                EFUN(wr_rx_vga2_ctrl_G1G2(0));
                EFUN(wr_rx_vga3_ctrl_G1G2(0));
            } else if (val <= 30) {
                EFUN(wr_rx_vga1_ctrl_G1G2(15));
                EFUN(wr_rx_vga2_ctrl_G1G2(val-15));
                EFUN(wr_rx_vga3_ctrl_G1G2(0));
            }else{
                EFUN(wr_rx_vga1_ctrl_G1G2(15));
                EFUN(wr_rx_vga2_ctrl_G1G2(15));
                EFUN(wr_rx_vga3_ctrl_G1G2(val-30));
            }
        } else {
            EFUN(wr_hwtune_ovr_write_sel(0));                   /* Configure hwtune_ovr_write_sel to VGA */
            EFUN(wr_hwtune_ovr_write_val(val<<3));              /* hwtune_ovr_write_val[8:3] are used to drive the analog control port */
            EFUN(wr_hwtune_ovr_write_en(1));
        }
    }
    return(ERR_CODE_NONE); 
}

err_code_t merlin16_pcieg3_INTERNAL_get_rx_vga(srds_access_t *sa__, int8_t *val) {
    {
        uint8_t rate_sel;
        ESTM(rate_sel = rd_rate_sel());
        if (rate_sel < MERLIN16_PCIEG3_GEN3) {
            ESTM(*val = (int8_t)(rd_rx_vga1_ctrl_G1G2() + rd_rx_vga2_ctrl_G1G2() + rd_rx_vga3_ctrl_G1G2()));
        } else {
    ESTM(*val = (int8_t)rd_vga_bin());
        }
    }
    return (ERR_CODE_NONE);
}


err_code_t merlin16_pcieg3_INTERNAL_set_rx_dfe1(srds_access_t *sa__, int8_t val) {

    EFUN(merlin16_pcieg3_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */

    {
        int8_t tap_eo;

        if (val > 63) {
            return (_error(ERR_CODE_DFE1_INVALID));  
        }
        /* Compute tap1 even/odd component */
        tap_eo = 0;                                      /* Not supporting dfe_dcd values */
        EFUN(wr_hwtune_ovr_write_sel(2));                /* Write tap1_odd */
        EFUN(wr_hwtune_ovr_write_val(tap_eo));
        EFUN(wr_hwtune_ovr_write_en(1));
        EFUN(wr_hwtune_ovr_write_sel(3));                /* Write tap1_even */
        EFUN(wr_hwtune_ovr_write_val(tap_eo));
        EFUN(wr_hwtune_ovr_write_en(1));
        EFUN(wr_hwtune_ovr_write_sel(1));
        EFUN(wr_hwtune_ovr_write_val(val-tap_eo));       /* Write the common tap */
        EFUN(wr_hwtune_ovr_write_en(1));
    }
    return(ERR_CODE_NONE); 
}
err_code_t merlin16_pcieg3_INTERNAL_get_rx_dfe1(srds_access_t *sa__, int8_t *val) {
    ESTM(*val = (int8_t)(rd_dfe_1_e() + rd_dfe_1_cmn()));
    return (ERR_CODE_NONE);
}


err_code_t merlin16_pcieg3_INTERNAL_set_rx_dfe2(srds_access_t *sa__, int8_t val) {
    int8_t tap_eo;

    if ((val > 31) || (val < -31)) {
      return (_error(ERR_CODE_DFE2_INVALID));  
    }

    EFUN(merlin16_pcieg3_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */

    /* Compute tap2 odd/even component */
    tap_eo = 0;                                         /* Not supporting dfe_dcd values */
    EFUN(wr_hwtune_ovr_write_sel(5));                   /* Write tap2_odd */
    EFUN(wr_hwtune_ovr_write_val(tap_eo));
    EFUN(wr_hwtune_ovr_write_en(1));
    EFUN(wr_hwtune_ovr_write_sel(6));                   /* Write tap2_even */
    EFUN(wr_hwtune_ovr_write_val(tap_eo));
    EFUN(wr_hwtune_ovr_write_en(1));
    EFUN(wr_hwtune_ovr_write_sel(4));
    EFUN(wr_hwtune_ovr_write_val(SRDS_ABS(val)-tap_eo));/* Write the common tap */
    EFUN(wr_hwtune_ovr_write_en(1));
    EFUN(wr_hwtune_ovr_write_sel(10));                  /* Write tap2_even_sign */
    EFUN(wr_hwtune_ovr_write_val(val<0?1:0));
    EFUN(wr_hwtune_ovr_write_en(1));
    EFUN(wr_hwtune_ovr_write_sel(11));                  /* Write tap2_odd_sign */
    EFUN(wr_hwtune_ovr_write_val(val<0?1:0));
    EFUN(wr_hwtune_ovr_write_en(1));
    return(ERR_CODE_NONE); 
}

err_code_t merlin16_pcieg3_INTERNAL_get_rx_dfe2(srds_access_t *sa__, int8_t *val) {
    ESTM(*val = rd_dfe_2_se() ? -(rd_dfe_2_e()+rd_dfe_2_cmn()) : (rd_dfe_2_e()+rd_dfe_2_cmn()));
    return (ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_set_rx_dfe3(srds_access_t *sa__, int8_t val) {
    if ((val > 31) || (val < -31)) {
      return (_error(ERR_CODE_DFE3_INVALID));  
    }

    EFUN(merlin16_pcieg3_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */

    EFUN(wr_hwtune_ovr_write_sel(7));                   /* Write tap3 */
    EFUN(wr_hwtune_ovr_write_val(val));
    EFUN(wr_hwtune_ovr_write_en(1));
    return(ERR_CODE_NONE); 
}

err_code_t merlin16_pcieg3_INTERNAL_get_rx_dfe3(srds_access_t *sa__, int8_t *val) {
    ESTM(*val = rd_dfe_3_cmn());
    return (ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_set_rx_dfe4(srds_access_t *sa__, int8_t val) {
    if ((val > 15) || (val < -15)) {
      return (_error(ERR_CODE_DFE4_INVALID));  
    }

    EFUN(merlin16_pcieg3_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */
    
    EFUN(wr_hwtune_ovr_write_sel(8));                   /* Write tap4 */
    EFUN(wr_hwtune_ovr_write_val(val));
    EFUN(wr_hwtune_ovr_write_en(1));
    return(ERR_CODE_NONE); 
}

err_code_t merlin16_pcieg3_INTERNAL_get_rx_dfe4(srds_access_t *sa__, int8_t *val) {
    ESTM(*val = rd_dfe_4_cmn());
    return (ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_set_rx_dfe5(srds_access_t *sa__, int8_t val) {
    if ((val > 15) || (val < -15)) {
      return (_error(ERR_CODE_DFE5_INVALID));  
    }

    EFUN(merlin16_pcieg3_INTERNAL_check_uc_lane_stopped(sa__));  /* make sure uC is stopped to avoid race conditions */
    
    EFUN(wr_hwtune_ovr_write_sel(9));                   /* Write tap5 */
    EFUN(wr_hwtune_ovr_write_val(val));
    EFUN(wr_hwtune_ovr_write_en(1));
    return(ERR_CODE_NONE); 
}

err_code_t merlin16_pcieg3_INTERNAL_get_rx_dfe5(srds_access_t *sa__, int8_t *val) {
    ESTM(*val = rd_dfe_5_cmn());
    return (ERR_CODE_NONE);
}








err_code_t merlin16_pcieg3_INTERNAL_core_clkgate(srds_access_t *sa__, uint8_t enable) {

  if (enable) {
  }
  else {
  }
  return (ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_lane_clkgate(srds_access_t *sa__, uint8_t enable) {

  if (enable) {
    /* Use frc/frc_val to force all RX and TX clk_vld signals to 0 */
        EFUN(wr_pmd_rx_clk_vld_frc_val(0x0));
        EFUN(wr_pmd_rx_clk_vld_frc(0x1));
        EFUN(wr_pmd_tx_clk_vld_frc_val(0x0));
        EFUN(wr_pmd_tx_clk_vld_frc(0x1));

    /* Use frc_on to force clkgate */
        EFUN(wr_ln_rx_s_clkgate_frc_on(0x1));

        EFUN(wr_ln_tx_s_clkgate_frc_on(0x1));

  }
  else {
        EFUN(wr_pmd_rx_clk_vld_frc_val(0x0));
        EFUN(wr_pmd_rx_clk_vld_frc(0x0));
        EFUN(wr_pmd_tx_clk_vld_frc_val(0x0));
        EFUN(wr_pmd_tx_clk_vld_frc(0x1));

        EFUN(wr_ln_rx_s_clkgate_frc_on(0x0));

        EFUN(wr_ln_tx_s_clkgate_frc_on(0x0));
  }
  return (ERR_CODE_NONE);
}


err_code_t merlin16_pcieg3_INTERNAL_get_osr_mode(srds_access_t *sa__, merlin16_pcieg3_osr_mode_st *imode) {
    merlin16_pcieg3_osr_mode_st mode;

     ENULL_MEMSET(&mode, 0, sizeof(merlin16_pcieg3_osr_mode_st));

    if(!imode) 
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

  ESTM(mode.tx = rd_tx_osr_mode());
  ESTM(mode.rx = rd_rx_osr_mode());
  mode.tx_rx = 255;
  *imode = mode;
  return (ERR_CODE_NONE);

}


err_code_t merlin16_pcieg3_INTERNAL_pmd_lock_status(srds_access_t *sa__, uint8_t *pmd_lock, uint8_t *pmd_lock_chg) {
    uint16_t rddata;
#if defined(reg_rd_TLB_RX_RXDBG_PMD_RX_LOCK_STATUS)
    ESTM(rddata = reg_rd_TLB_RX_RXDBG_PMD_RX_LOCK_STATUS());
    ESTM(*pmd_lock = ex_TLB_RX_RXDBG_PMD_RX_LOCK_STATUS__dbg_pmd_rx_lock(rddata));
    ESTM(*pmd_lock_chg = ex_TLB_RX_RXDBG_PMD_RX_LOCK_STATUS__dbg_pmd_rx_lock_change(rddata));
#else
    ESTM(rddata = reg_rd_TLB_RX_PMD_RX_LOCK_STATUS());
    ESTM(*pmd_lock = ex_TLB_RX_PMD_RX_LOCK_STATUS__pmd_rx_lock(rddata));
    ESTM(*pmd_lock_chg = ex_TLB_RX_PMD_RX_LOCK_STATUS__pmd_rx_lock_change(rddata));
#endif
    return(ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_sigdet_status(srds_access_t *sa__, uint8_t *sig_det, uint8_t *sig_det_chg) {
    ESTM(*sig_det     = ((rd_rx_sigdet() >> 1) & 0x1));
    ESTM(*sig_det_chg = (uint8_t)rd_rg_sigdet_tggl_cnt());
    return(ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_pll_lock_status(srds_access_t *sa__, uint8_t *pll_lock, uint8_t *pll_lock_chg) {

    uint16_t rddata;
    uint8_t pll_fail;
    ESTM(rddata = reg_rdc_PLL_CAL_COM_STS_0());
    ESTM(*pll_lock = exc_PLL_CAL_COM_STS_0__pll_lock(rddata));
    ESTM(pll_fail = exc_PLL_CAL_COM_STS_0__pll_fail_stky(rddata));
    ESTM(*pll_lock_chg = ((*pll_lock ^ !exc_PLL_CAL_COM_STS_0__pll_lock_bar_stky(rddata)) | (*pll_lock ^ !pll_fail)));
    return(ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_read_lane_state(srds_access_t *sa__, merlin16_pcieg3_lane_state_st *istate) {  
       
  merlin16_pcieg3_lane_state_st state;
  uint8_t ladder_range = 0;

  ENULL_MEMSET(&state, 0, sizeof(merlin16_pcieg3_lane_state_st));

  if(!istate) 
    return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

  EFUN(merlin16_pcieg3_INTERNAL_pmd_lock_status(sa__,&state.rx_lock, &state.rx_lock_chg));
  {
      err_code_t err_code = 0;
      state.stop_state = merlin16_pcieg3_INTERNAL_stop_micro(sa__,state.rx_lock,&err_code);
      if(err_code) USR_PRINTF(("Unable to stop microcontroller,  following data is suspect\n"));
  }

  ESTM(state.rate_select = (rd_rate_sel()+1));
  ESTM(state.dsc_one_hot_0_0 = rd_dsc_state_one_hot_0());
  ESTM(state.dsc_one_hot_0_1 = rd_dsc_state_one_hot_0());
  {  merlin16_pcieg3_osr_mode_st osr_mode;
  ENULL_MEMSET(&osr_mode, 0, sizeof(merlin16_pcieg3_osr_mode_st));
  EFUN(merlin16_pcieg3_INTERNAL_get_osr_mode(sa__, &osr_mode));
  state.osr_mode = osr_mode;
  }
  ESTM(state.reset_state = rd_lane_dp_reset_state());
  EFUN(merlin16_pcieg3_INTERNAL_sigdet_status(sa__,&state.sig_det, &state.sig_det_chg));
  ESTM(state.rx_ppm = rd_cdr_integ_reg()/84);
  ESTM(state.clk90 = rd_cnt_d_minus_m1());
  ESTM(state.clkp1 = rd_cnt_d_minus_p1());
  ESTM(state.br_pd_en = rd_br_pd_en());
  /* drop the MSB, the result is actually valid modulo 128 */
  /* Also flip the sign to account for d-m1, versus m1-d */
  state.clk90 = state.clk90<<1;
  state.clk90 = -(state.clk90>>1);
  state.clkp1 = state.clkp1<<1;
  state.clkp1 = -(state.clkp1>>1);
  
  EFUN(merlin16_pcieg3_INTERNAL_get_rx_pf_main(sa__, &state.pf_main));
  ESTM(state.pf_hiz = rd_pf_hiz());
  {
      uint8_t rate_sel;
      ESTM(rate_sel = rd_rate_sel());
      if (rate_sel == MERLIN16_PCIEG3_GEN1) {
          ESTM(state.pf2_ctrl = merlin16_pcieg3_INTERNAL_gray_to_uint8(rd_pf2_lowp_ctrl_g1()));
      } else if (rate_sel == MERLIN16_PCIEG3_GEN2) {
          ESTM(state.pf2_ctrl = merlin16_pcieg3_INTERNAL_gray_to_uint8(rd_pf2_lowp_ctrl_g2()));
      } else {
          ESTM(state.pf2_ctrl = rd_pf2_lowp_ctrl());
      }
  }
  EFUN(merlin16_pcieg3_INTERNAL_get_rx_vga(sa__, &state.vga));
  ESTM(state.dc_offset = rd_dc_offset_bin());
  ESTM(ladder_range = rd_p1_thresh_sel());
  EFUN(merlin16_pcieg3_INTERNAL_get_p1_threshold(sa__, &state.p1_lvl_ctrl));         
  state.p1_lvl = merlin16_pcieg3_INTERNAL_ladder_setting_to_mV(sa__, state.p1_lvl_ctrl, ladder_range);
  state.m1_lvl = 0;

  EFUN(merlin16_pcieg3_INTERNAL_get_rx_dfe1(sa__, &state.dfe1));
  EFUN(merlin16_pcieg3_INTERNAL_get_rx_dfe2(sa__, &state.dfe2));  
  EFUN(merlin16_pcieg3_INTERNAL_get_rx_dfe3(sa__, &state.dfe3));  
  EFUN(merlin16_pcieg3_INTERNAL_get_rx_dfe4(sa__, &state.dfe4));
  EFUN(merlin16_pcieg3_INTERNAL_get_rx_dfe5(sa__, &state.dfe5));

  ESTM(state.dfe1_dcd = rd_dfe_1_e()-rd_dfe_1_o());
  ESTM(state.dfe2_dcd = (rd_dfe_2_se() ? -rd_dfe_2_e(): rd_dfe_2_e()) -(rd_dfe_2_so() ? -rd_dfe_2_o(): rd_dfe_2_o()));
  ESTM(state.pe = rd_p1_offset_evn_bin());
  ESTM(state.ze = rd_data_offset_evn_bin());
  ESTM(state.me = rd_m1_offset_evn_bin());
  
  ESTM(state.po = rd_p1_offset_odd_bin());
  ESTM(state.zo = rd_data_offset_odd_bin());
  ESTM(state.mo = rd_m1_offset_odd_bin());

  /* tx_ppm = register/10.84 */
  ESTM(state.tx_ppm = (int16_t)(((int32_t)(rd_tx_pi_integ2_reg()))*3125/32768));


  if (!state.stop_state) {
      EFUN(merlin16_pcieg3_stop_rx_adaptation(sa__, 0));
  } 

  *istate = state;
  return (ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_display_lane_state_no_newline(srds_access_t *sa__) {     
  uint16_t lane_idx;
  merlin16_pcieg3_lane_state_st state;



  ENULL_MEMSET(&state, 0, sizeof(merlin16_pcieg3_lane_state_st));

  EFUN(merlin16_pcieg3_INTERNAL_read_lane_state(sa__, &state));
 
  lane_idx = merlin16_pcieg3_get_lane(sa__); 
  EFUN_PRINTF(("%2d ", lane_idx));
  EFUN_PRINTF((" %01x ",state.stop_state));
  EFUN_PRINTF((" GEN%1d", state.rate_select));
  EFUN_PRINTF((" %04X,%04X", state.dsc_one_hot_0_0, state.dsc_one_hot_0_1));
  EFUN_PRINTF(("   %1d,%3u", state.sig_det, state.sig_det_chg));
  EFUN_PRINTF(("  %1d%s", state.rx_lock, state.rx_lock_chg ? "*" : " "));
  EFUN_PRINTF(("%4d ", state.rx_ppm));
  EFUN_PRINTF(("  %3d ", state.clk90));
  EFUN_PRINTF(("  %3d ", state.clkp1));
  EFUN_PRINTF(("   %2d,%1d ", state.pf_main, state.pf2_ctrl));
  EFUN_PRINTF(("  %2d ", state.vga));
  EFUN_PRINTF(("%3d  ", state.dc_offset));
  EFUN_PRINTF(("%3d ", state.p1_lvl));
  EFUN_PRINTF(("%3d,%3d,%3d,%3d,%3d,%3d,%3d ", state.dfe1, state.dfe2, state.dfe3, state.dfe4, state.dfe5, state.dfe1_dcd, state.dfe2_dcd));
  EFUN_PRINTF(("%3d,%3d,%3d,%3d,%3d,%3d  ", state.ze, state.zo, state.pe, state.po, state.me, state.mo));
  EFUN_PRINTF(("  %4d ", state.tx_ppm));
  
  return (ERR_CODE_NONE);
}


err_code_t merlin16_pcieg3_INTERNAL_read_core_state(srds_access_t *sa__, merlin16_pcieg3_core_state_st *istate) {  
  merlin16_pcieg3_core_state_st state;
  struct merlin16_pcieg3_uc_core_config_st core_cfg;

  ENULL_MEMSET(&state, 0, sizeof(merlin16_pcieg3_core_state_st));
  ENULL_MEMSET(&core_cfg, 0, sizeof(struct merlin16_pcieg3_uc_core_config_st));

  if(!istate) 
    return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  /* pllpon_mux is read in case the rescal value is overriden. */
  ESTM(state.rescal = rdc_pllpon_mux());
  ESTM(state.core_reset           = rdc_core_dp_reset_state());
  ESTM(state.uc_active            = rdc_uc_active());
  ESTM(state.comclk_mhz           = rdc_heartbeat_count_1us());   
  ESTM(state.ucode_version        = rdcv_common_ucode_version()); 
  ESTM(state.ucode_minor_version  = rdcv_common_ucode_minor_version());
  ESTM(state.afe_hardware_version = rdcv_afe_hardware_version());
  ESTM(state.temp_idx             = rdcv_temp_idx());
  {  int16_t                           die_temp = 0;
     EFUN(merlin16_pcieg3_read_die_temperature(sa__, &die_temp));
     state.die_temp               =    die_temp;
  }
  ESTM(state.avg_tmon             = _bin_to_degC(rdcv_avg_tmon_reg13bit()>>3));
  state.vco_rate_mhz            = (uint16_t)core_cfg.vco_rate_in_Mhz;
  ESTM(state.analog_vco_range     = rdc_pll_range());   
  EFUN(merlin16_pcieg3_INTERNAL_pll_lock_status(sa__, &state.pll_lock, &state.pll_lock_chg));
  ESTM(state.core_status          = rdcv_status_byte());
  ESTM(state.rate_select = (rd_rate_sel()+1));

  *istate = state;
  return (ERR_CODE_NONE);
}


err_code_t merlin16_pcieg3_INTERNAL_display_core_state_no_newline(srds_access_t *sa__) {
  merlin16_pcieg3_core_state_st state;
    ENULL_MEMSET(&state     , 0, sizeof(state     ));
  EFUN(merlin16_pcieg3_INTERNAL_read_core_state(sa__, &state));

	if ((state.avg_tmon<-50)||(state.avg_tmon>135)) {
          EFUN_PRINTF(("\n*** WARNING: Core die temperature (LIVE_TEMP) out of bounds -50C to 130C\n"));
	}
	if ((state.rescal < 6) || (state.rescal > 13)) {
          EFUN_PRINTF(("\n*** WARNING: RESCAL value is out of bounds 6 to 13\n"));
	}

    EFUN_PRINTF((" %02d "              ,  merlin16_pcieg3_get_core(sa__)));
    EFUN_PRINTF((" %02x  "             ,  state.core_status));
    EFUN_PRINTF(("    %1d     "        ,  state.pll_pwrdn));
    EFUN_PRINTF((" %3d.%2dMHz"         , (state.comclk_mhz/4),((state.comclk_mhz%4)*25)));    /* comclk in Mhz = heartbeat_count_1us / 4 */
    EFUN_PRINTF(("   %4X_%02X "        ,  state.ucode_version, state.ucode_minor_version));
    EFUN_PRINTF(("    0x%02x   "       ,  state.afe_hardware_version));
    EFUN_PRINTF(("   %3dC   "          ,  state.die_temp));
    EFUN_PRINTF(("   (%02d)%3dC "      ,  state.temp_idx, state.avg_tmon));
    EFUN_PRINTF(("   0x%02x  "         ,  state.rescal));
    EFUN_PRINTF(("  GEN%d "            ,  state.rate_select));
    EFUN_PRINTF(("    %03d       "     ,  state.analog_vco_range));
    EFUN_PRINTF(("     %01d%s  "       ,  state.pll_lock, state.pll_lock_chg ? "*" : " "));

    return (ERR_CODE_NONE);
  }



err_code_t merlin16_pcieg3_INTERNAL_check_uc_lane_stopped(srds_access_t *sa__) {

  uint8_t is_micro_stopped;
  ESTM(is_micro_stopped = rdv_usr_sts_micro_stopped());
  if (!is_micro_stopped) {
      return(_error(ERR_CODE_UC_NOT_STOPPED));
  } else {
      return(ERR_CODE_NONE);
  }
}

err_code_t merlin16_pcieg3_INTERNAL_calc_patt_gen_mode_sel(uint8_t *mode_sel, uint8_t *zero_pad_len, uint8_t patt_length) {

  if(!mode_sel) {
    return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  if(!zero_pad_len) {
    return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  /* Select the correct Pattern generator Mode depending on Pattern length */
  if (!(140 % patt_length)) {
    *mode_sel = 6;
    *zero_pad_len = 100;
  }
  else if (!(160 % patt_length)) {
    *mode_sel = 5;
    *zero_pad_len = 80;
  }
  else if (!(180 % patt_length)) {
    *mode_sel = 4;
    *zero_pad_len = 60;
  }
  else if (!(200 % patt_length)) {
    *mode_sel = 3;
    *zero_pad_len = 40;
  }
  else if (!(220 % patt_length)) {
    *mode_sel = 2;
    *zero_pad_len = 20;
  }
  else if (!(240 % patt_length)) {
    *mode_sel = 1;
    *zero_pad_len = 0;
  } else {
    EFUN_PRINTF(("ERROR: Unsupported Pattern Length\n"));
    return (_error(ERR_CODE_CFG_PATT_INVALID_PATT_LENGTH));
  }
  return(ERR_CODE_NONE);
}


err_code_t merlin16_pcieg3_INTERNAL_print_uc_dsc_error(srds_access_t *sa__, enum srds_pmd_uc_cmd_enum cmd) {
    uint32_t supp_info;

    ESTM(supp_info = (rd_uc_dsc_supp_info()) & 0xFF);
    switch (SUPP_INFO_TO_ERROR_CODE(supp_info)) {
      case UC_CMD_ERROR_INVALID_COMMAND:
        ESTM_PRINTF(("ERROR : UC reported invalid command %d.  (other_info = 0x%X)\n",
                     cmd, SUPP_INFO_TO_OTHER_INFO(supp_info)));
        break;
      case UC_CMD_ERROR_BUSY:
        ESTM_PRINTF(("ERROR : UC reported busy for command %d.  (other_info = 0x%X)\n",
                     cmd, SUPP_INFO_TO_OTHER_INFO(supp_info)));
        break;
      case UC_CMD_ERROR_GET_EYE_SAMPLE_ERROR:
        ESTM_PRINTF(("ERROR : UC reported error in getting eye sample.  (command %d, other_info = 0x%X)\n",
                     cmd, SUPP_INFO_TO_OTHER_INFO(supp_info)));
        break;
      case UC_CMD_ERROR_PRBS_NOT_LOCKED:
        ESTM_PRINTF(("ERROR : UC reported PRBS not locked.  (command %d, other_info = 0x%X)\n",
                     cmd, SUPP_INFO_TO_OTHER_INFO(supp_info)));
        break;
      case UC_CMD_ERROR_COMMAND_IN_PROGRESS:
        ESTM_PRINTF(("ERROR : UC reported command already in progress.  (command %d, other_info = 0x%X)\n",
                     cmd, SUPP_INFO_TO_OTHER_INFO(supp_info)));
        break;
      case UC_CMD_ERROR_INVALID_MODE:
        ESTM_PRINTF(("ERROR : UC reported invalid mode for command %d.  (other_info = 0x%X)\n",
                     cmd, SUPP_INFO_TO_OTHER_INFO(supp_info)));
        break;
      default:
        ESTM_PRINTF(("ERROR : UC reported unknown error 0x%X for command %d.  (other_info = 0x%X)\n",
                     SUPP_INFO_TO_ERROR_CODE(supp_info), cmd, SUPP_INFO_TO_OTHER_INFO(supp_info)));
    }
    /* Cleanup cmd register */
    EFUN(reg_wr_DSC_A_DSC_UC_CTRL(0x80));
    EFUN(wr_uc_dsc_data(0));

    return(ERR_CODE_NONE);
}
 
/******************************************/
/*  Serdes Register field Poll functions  */
/******************************************/

#ifndef CUSTOM_REG_POLLING

/* poll for microcontroller to populate the dsc_data register */
err_code_t merlin16_pcieg3_INTERNAL_poll_diag_done(srds_access_t *sa__, uint16_t *status, uint32_t timeout_ms) {
 uint8_t loop;

 if(!status) {
     return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

 for(loop=0;loop < 100; loop++) {
     ESTM(*status=rdv_usr_diag_status());

     if((*status & 0x8000) > 0) {
         return(ERR_CODE_NONE);
     }
     if(loop>10) {
         EFUN(USR_DELAY_US(10*timeout_ms));
     }
 }
 return(_error(ERR_CODE_DIAG_TIMEOUT));
}

/** Poll for field "uc_dsc_ready_for_cmd" = 1 [Return Val => Error_code (0 = Polling Pass)] */
err_code_t merlin16_pcieg3_INTERNAL_poll_uc_dsc_ready_for_cmd_equals_1(srds_access_t *sa__, uint32_t timeout_ms, enum srds_pmd_uc_cmd_enum cmd) {
    /* read quickly for 10 tries */
    uint16_t loop;

    for (loop = 0; loop < 100; loop++) {
        uint16_t rddata;
        ESTM(rddata = reg_rd_DSC_A_DSC_UC_CTRL());
        if (rddata & 0x0080) {    /* bit 7 is uc_dsc_ready_for_cmd */
            if (rddata & 0x0040) {  /* bit 6 is uc_dsc_error_found   */
                EFUN(merlin16_pcieg3_INTERNAL_print_uc_dsc_error(sa__, cmd));
                return(_error(ERR_CODE_UC_CMD_RETURN_ERROR));
            }
            return (ERR_CODE_NONE);
        }
        if(loop>10) {
            EFUN(USR_DELAY_US(10*timeout_ms));
        }
    }
  EFUN_PRINTF(("ERROR : DSC ready for command is not working, applying workaround and getting debug info !\n"));
  DISP(rd_uc_dsc_supp_info());
  DISP(rd_uc_dsc_gp_uc_req());
  DISP(rd_dsc_state());
  ESTM_PRINTF(("Uc Core Status Byte = 0x%x\n",rdcv_status_byte()));
  DISP(rdv_usr_sts_micro_stopped());
  /* DISP(rdv_stop_graceful_trigger()); */
  /* artifically terminate the command to re-enable the command interface */
  EFUN(wr_uc_dsc_ready_for_cmd(0x1));
  return (_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}

/* Poll for field "dsc_state" = DSC_STATE_UC_TUNE [Return Val => Error_code (0 = Polling Pass)] */
err_code_t merlin16_pcieg3_INTERNAL_poll_dsc_state_equals_uc_tune(srds_access_t *sa__, uint32_t timeout_ms) {
  uint16_t loop;
  uint16_t dsc_state;
  /* poll 10 times to avoid longer delays later */
  for (loop = 0; loop < 100; loop++) {
    ESTM(dsc_state = rd_dsc_state());
    if (dsc_state == DSC_STATE_UC_TUNE) {
      return (ERR_CODE_NONE);
    }
    if(loop>10) {
        EFUN(USR_DELAY_US(10*timeout_ms));
    }
  }
  ESTM_PRINTF(("DSC_STATE = %d\n", rd_dsc_state()));
  return (_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}




/* Poll for field "micro_ra_initdone" = 1 [Return Val => Error_code (0 = Polling Pass)] */
err_code_t merlin16_pcieg3_INTERNAL_poll_micro_ra_initdone(srds_access_t *sa__, uint32_t timeout_ms) {
  uint16_t loop;

  uint8_t result;
  for (loop = 0; loop <= 100; loop++) {
    ESTM(result = rdc_micro_ra_initdone());
    if (result) {
      return (ERR_CODE_NONE);
    }
    EFUN(USR_DELAY_US(10*timeout_ms));
  }
  return (_error(ERR_CODE_POLLING_TIMEOUT));          /* Error Code for polling timeout */
}

#endif /* CUSTOM_REG_POLLING */



err_code_t merlin16_pcieg3_INTERNAL_rdblk_callback(void *arg, uint8_t byte_count, uint16_t data) {
    merlin16_pcieg3_INTERNAL_rdblk_callback_arg_t * const cast_arg = (merlin16_pcieg3_INTERNAL_rdblk_callback_arg_t *)arg;
    *(cast_arg->mem_ptr + get_endian_offset(cast_arg->mem_ptr)) = data & 0xFF;
    cast_arg->mem_ptr++;
    if (byte_count == 2) {
        *(cast_arg->mem_ptr + get_endian_offset(cast_arg->mem_ptr)) = data >> 8;
        cast_arg->mem_ptr++;
    }
    return (ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_rdblk_uc_generic_ram(srds_access_t *sa__,
                                                uint32_t block_addr,
                                                uint16_t block_size,
                                                uint16_t start_offset,
                                                uint16_t cnt,
                                                void *arg,
                                                err_code_t (*callback)(void *, uint8_t, uint16_t)) {
    uint32_t read_val = 0;
    uint8_t defecit = 0;
    uint32_t addr = block_addr + start_offset;
    
    if (cnt == 0) {
        return (ERR_CODE_NONE);
    }

    /* Check for bad start offset and block size. */
    if (start_offset >= block_size) {
        return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }

    while (cnt > 0) {
        /* Determine how many bytes to read before wrapping back to start of block. */
        uint16_t block_cnt = SRDS_MIN(cnt, block_addr + block_size - addr);
        cnt -= block_cnt;

            /* Set up the word reads. */
            EFUN(wrc_micro_autoinc_rdaddr_en(1));
            EFUN(wrc_micro_ra_rddatasize(0x1));                     /* Select 16bit read datasize */
            EFUN(wrc_micro_ra_rdaddr_msw(addr >> 16));              /* Upper 16bits of RAM address to be read */
            EFUN(wrc_micro_ra_rdaddr_lsw(addr & 0xFFFE));           /* Lower 16bits of RAM address to be read */

            /* Read the leading byte, if starting at an odd address. */
            if ((addr & 1) == 1) {
                ESTM(read_val |= ((rdc_micro_ra_rddata_lsw() >> 8) << defecit));
                if (defecit == 8) {
                    EFUN(callback(arg, 2, (uint16_t)read_val));
                    read_val = 0;
                }
                /* We just read a byte.  This toggles the defecit from 0 to 8 or from 8 to 0. */
                defecit ^= 8;
                --block_cnt;
            }

            /* Read the whole words, and call the callback with two bytes at a time. */
            while (block_cnt >= 2) {
                ESTM(read_val |= (rdc_micro_ra_rddata_lsw() << defecit));
                EFUN(callback(arg, 2, (uint16_t)read_val));
                read_val >>= 16;
                /* We just read two bytes.  This preserves whatever defecit (8 or 0) is there. */
                block_cnt -= 2;
            }

            /* Read the trailing byte, if leftover after reading whole words. */
            if (block_cnt > 0) {
                ESTM(read_val |= ((rdc_micro_ra_rddata_lsw() & 0xFF) << defecit));
                if (defecit == 8) {
                    EFUN(callback(arg, 2, (uint16_t)read_val));
                    read_val = 0;
                }
                /* We just read a byte.  This toggles the defecit from 0 to 8 or from 8 to 0. */
                defecit ^= 8;
            }
        addr = block_addr;
    }

    /* If a final byte is left behind, then call the callback with it. */
    if (defecit > 0) {
        EFUN(callback(arg, 1, (uint16_t)read_val));
    }

    return(ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_INTERNAL_rdblk_uc_generic_ram_descending(srds_access_t *sa__,
                                                           uint32_t block_addr,
                                                           uint16_t block_size,
                                                           uint16_t start_offset,
                                                           uint16_t cnt,
                                                           void *arg,
                                                           err_code_t (*callback)(void *, uint8_t, uint16_t)) {
    uint32_t read_val = 0;
    uint8_t defecit = 0;
    uint32_t addr = block_addr + start_offset;
    uint16_t configured_addr_msw = (addr >> 16) + 1;
    
    if (cnt == 0) {
        return (ERR_CODE_NONE);
    }

    /* Check for bad start offset and block size. */
    if (start_offset >= block_size) {
        return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }

    EFUN(wrc_micro_autoinc_rdaddr_en(0));
    EFUN(wrc_micro_ra_rddatasize(0x1));                         /* Select 16bit read datasize */
    
    while (cnt > 0) {
        /* Determine how many bytes to read before wrapping back to end of block. */
        uint16_t block_cnt = SRDS_MIN(cnt, start_offset+1);
        cnt -= block_cnt;

        while (block_cnt > 0) {
            const uint16_t addr_msw = addr >> 16;
            uint16_t read_val2;
            if (addr_msw != configured_addr_msw) {
                EFUN(wrc_micro_ra_rdaddr_msw(addr_msw));        /* Upper 16bits of RAM address to be read */
                configured_addr_msw = addr_msw;
            }
            
            EFUN(wrc_micro_ra_rdaddr_lsw(addr & 0xFFFE));       /* Lower 16bits of RAM address to be read */
            ESTM(read_val2 = rdc_micro_ra_rddata_lsw());

            if (((addr & 1) == 1) && (block_cnt >= 2)) {
                /* Reading two bytes.  Since we're reading in descending address order, they
                 * will be reversed before they are sent out. */
                read_val |= ((((read_val2 & 0xFF) << 8) | (read_val2 >> 8)) << defecit);
                EFUN(callback(arg, 2, (uint16_t)read_val));
                read_val >>= 16;
                /* We just read two bytes.  This preserves whatever defecit (8 or 0) is there. */
                block_cnt -= 2;
                addr -= 2;
            } else {
                if ((addr & 1) == 1) {
                    /* Reading upper byte of word. */
                    read_val |= ((read_val2 >> 8) << defecit);
                } else {
                    /* Reading lower byte of word. */
                    read_val |= ((read_val2 & 0xFF) << defecit);
                }
                if (defecit == 8) {
                    EFUN(callback(arg, 2, (uint16_t)read_val));
                    read_val = 0;
                }
                /* We just read a byte.  This toggles the defecit from 0 to 8 or from 8 to 0. */
                defecit ^= 8;
                --block_cnt;
                --addr;
            }
        }
        
        addr = block_addr + block_size - 1;
        start_offset = block_size - 1;
    }

    /* If a final byte is left behind, then call the callback with it. */
    if (defecit > 0) {
        EFUN(callback(arg, 1, (uint16_t)read_val));
    }

    return(ERR_CODE_NONE);
}

uint8_t merlin16_pcieg3_INTERNAL_grp_idx_from_lane(uint8_t lane) {
    return(0);
}


