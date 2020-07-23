/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  merlin16_pcieg3_config.c                                           *
 *  Created On    :  03 Nov 2015                                                   *
 *  Created By    :  Brent Roberts                                                 *
 *  Description   :  APIs for Serdes IPs                                           *
 *  Revision      :    *
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

/** @file merlin16_pcieg3_config.c
 * Implementation of API config functions
 */

#include "merlin16_pcieg3_config.h"
#include "merlin16_pcieg3_access.h"
#include "merlin16_pcieg3_common.h"
#include "merlin16_pcieg3_functions.h"
#include "merlin16_pcieg3_internal.h"
#include "merlin16_pcieg3_internal_error.h"
#include "merlin16_pcieg3_select_defns.h"
#include "merlin16_pcieg3_enum.h"
#include "merlin16_pcieg3_dependencies.h"

/* If SERDES_EVAL is defined, then is_ate_log_enabled() is queried to *\
\* know whether to log ATE.  merlin16_pcieg3_access.h provides that function.  */

/*******************************/
/*  Stop/Resume RX Adaptation  */
/*******************************/

err_code_t merlin16_pcieg3_stop_rx_adaptation(srds_access_t *sa__, uint8_t enable) {

  if (enable) {
    return(merlin16_pcieg3_pmd_uc_control(sa__, CMD_UC_CTRL_STOP_GRACEFULLY,GRACEFUL_STOP_TIME));
  }
  else {
    return(merlin16_pcieg3_pmd_uc_control(sa__, CMD_UC_CTRL_RESUME,50));
  }
}

err_code_t merlin16_pcieg3_request_stop_rx_adaptation(srds_access_t *sa__) {

  return(merlin16_pcieg3_pmd_uc_control_return_immediate(sa__, CMD_UC_CTRL_STOP_GRACEFULLY));
}

/**********************/
/*  uCode CRC Verify  */
/**********************/

err_code_t merlin16_pcieg3_ucode_crc_verify(srds_access_t *sa__, uint16_t ucode_len,uint16_t expected_crc_value) {
    uint16_t calc_crc;
    EFUN(merlin16_pcieg3_pmd_uc_cmd_with_data(sa__, CMD_CALC_CRC,0,ucode_len,200));

    ESTM(calc_crc = rd_uc_dsc_data());
    if(calc_crc != expected_crc_value) {
        EFUN_PRINTF(("Microcode CRC did not match expected=%04x : calculated=%04x\n",expected_crc_value, calc_crc));
        return(_error(ERR_CODE_UC_CRC_NOT_MATCH));
    } else {
        EFUN_PRINTF(("Microcode CRC Matched expected=0x%04X\n",expected_crc_value));
    }

    return(ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_start_ucode_crc_calc(srds_access_t *sa__, uint16_t ucode_len) {
    EFUN(merlin16_pcieg3_pmd_uc_cmd_with_data_return_immediate(sa__, CMD_CALC_CRC, 0, ucode_len)); /* Invoke Calculate CRC command and return control immediately */
    return(ERR_CODE_NONE);
}

err_code_t merlin16_pcieg3_check_ucode_crc(srds_access_t *sa__, uint16_t expected_crc_value, uint32_t timeout_ms) {
    uint16_t calc_crc;
    err_code_t err_code;

    err_code = merlin16_pcieg3_INTERNAL_poll_uc_dsc_ready_for_cmd_equals_1(sa__, timeout_ms, CMD_CALC_CRC); /* Poll for uc_dsc_ready_for_cmd = 1 to indicate merlin16_pcieg3 ready for command */
    if (err_code) {
      EFUN_PRINTF(("ERROR : DSC ready for command timed out. Previous uC command not finished yet\n"));
      return (err_code);
    }
    ESTM(calc_crc = rd_uc_dsc_data());
    if(calc_crc != expected_crc_value) {
        EFUN_PRINTF(("UC CRC did not match expected=%04x : calculated=%04x\n",expected_crc_value, calc_crc));
        return(_error(ERR_CODE_UC_CRC_NOT_MATCH));
    }

    return(ERR_CODE_NONE);
}


/*--------------------------------------------*/
/*  APIs to Enable or Disable datapath reset  */
/*--------------------------------------------*/

err_code_t merlin16_pcieg3_tx_dp_reset(srds_access_t *sa__, uint8_t enable) {
    if (enable) {
        EFUN(wr_tx_ln_dp_s_rstb(0x0));
    } else {
        EFUN(wr_tx_ln_dp_s_rstb(0x1));
        EFUN(wr_tx_uc_ack_lane_dp_reset(0x1));
        EFUN(wr_tx_uc_ack_lane_cfg_done(0x1));
    }
    return ERR_CODE_NONE;
}

err_code_t merlin16_pcieg3_rx_dp_reset(srds_access_t *sa__, uint8_t enable) {
    if (enable) {
        EFUN(wr_rx_ln_dp_s_rstb(0x0));
    } else {
        EFUN(wr_rx_ln_dp_s_rstb(0x1));
    }
    return ERR_CODE_NONE;
}

err_code_t merlin16_pcieg3_core_dp_reset(srds_access_t *sa__, uint8_t enable){
    if (enable) {
        EFUN(wrc_core_dp_s_rstb(0x0));
    } else {
        EFUN(wrc_core_dp_s_rstb(0x1));
    }
    return ERR_CODE_NONE;
}

/********************************/
/*  Loading ucode through PRAM  */
/********************************/

err_code_t merlin16_pcieg3_ucode_pram_load(srds_access_t *sa__, char const * ucode_image, uint16_t ucode_len) {
    uint16_t ucode_len_remainder = ((ucode_len + 3) & 0xFFFC) - ucode_len;

    if(!ucode_image) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if (ucode_len > UCODE_MAX_SIZE) {                       /* uCode size should be less than UCODE_MAX_SIZE  */
        return (_error(ERR_CODE_INVALID_UCODE_LEN));
    }

    EFUN(wrc_micro_master_clk_en(0x1));                     /* Enable clock to microcontroller subsystem */
    EFUN(wrc_micro_master_rstb(0x1));                       /* De-assert reset to microcontroller sybsystem */
    EFUN(wrc_micro_master_rstb(0x0));                       /* Assert reset to microcontroller sybsystem - Toggling reset*/
    EFUN(wrc_micro_master_rstb(0x1));                       /* De-assert reset to microcontroller sybsystem */

    EFUN(wrc_micro_ra_init(0x1));                           /* Set initialization command to initialize code RAM */
    EFUN(merlin16_pcieg3_INTERNAL_poll_micro_ra_initdone(sa__, 250));/* Poll for micro_ra_initdone = 1 to indicate initialization done */
    EFUN(wrc_micro_ra_init(0x0));                           /* Clear initialization command */


    EFUN(wrc_micro_pramif_ahb_wraddr_msw(0x0000));
    EFUN(wrc_micro_pramif_ahb_wraddr_lsw(0x0000));
    EFUN(wrc_micro_pram_if_rstb(1));
    EFUN(wrc_micro_pramif_en(1));

    /* Wait 8 pram clocks */
    EFUN(USR_DELAY_US(1));
    
    /* write ucode into pram */
    while (ucode_len > 0) {
        EFUN(merlin16_pcieg3_pmd_wr_pram(sa__, *ucode_image));
        --ucode_len;
        ++ucode_image;
    }
    
    /* Pad to 32 bits */
    while (ucode_len_remainder > 0) {
        EFUN(merlin16_pcieg3_pmd_wr_pram(sa__, 0));
        --ucode_len_remainder;
    }
    
    /* Wait 8 pram clocks */
    EFUN(USR_DELAY_US(1));


    EFUN(wrc_micro_pramif_en(0));
    EFUN(wrc_micro_core_clk_en(1));
   /* EFUN(wrc_micro_core_rstb(1)); */
    return(ERR_CODE_NONE);
}


/************************/
/*  Serdes API Version  */
/************************/

err_code_t merlin16_pcieg3_version(uint32_t *api_version) {

    if(!api_version) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    *api_version = 0xA10700;
    return (ERR_CODE_NONE);
}

/*****************/
/*  PMD_RX_LOCK  */
/*****************/

err_code_t merlin16_pcieg3_pmd_lock_status(srds_access_t *sa__, uint8_t *pmd_rx_lock) {
    if(!pmd_rx_lock) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    ESTM(*pmd_rx_lock = rd_pmd_rx_lock());
    return (ERR_CODE_NONE);
}

/**********************************/
/*  Serdes TX disable/RX Restart  */
/**********************************/

err_code_t merlin16_pcieg3_tx_disable(srds_access_t *sa__, uint8_t enable) {

  if (enable) {
    EFUN(wr_sdk_tx_disable(0x1));
  }
  else {
    EFUN(wr_sdk_tx_disable(0x0));
  }
  return(ERR_CODE_NONE);
}


/******************************************************/
/*  Single function to set/get all RX AFE parameters  */
/******************************************************/

err_code_t merlin16_pcieg3_write_rx_afe(srds_access_t *sa__, enum srds_rx_afe_settings_enum param, int8_t val) {
  /* Assumes the micro is not actively tuning */

    switch(param) {
    case RX_AFE_PF:
        return(merlin16_pcieg3_INTERNAL_set_rx_pf_main(sa__, val));

    case RX_AFE_PF2:
        return(merlin16_pcieg3_INTERNAL_set_rx_pf2(sa__, val));

    case RX_AFE_VGA:
      return(merlin16_pcieg3_INTERNAL_set_rx_vga(sa__, val));

    case RX_AFE_DFE1:
        return(merlin16_pcieg3_INTERNAL_set_rx_dfe1(sa__, val));

    case RX_AFE_DFE2:
        return(merlin16_pcieg3_INTERNAL_set_rx_dfe2(sa__, val));

    case RX_AFE_DFE3:
        return(merlin16_pcieg3_INTERNAL_set_rx_dfe3(sa__, val));

    case RX_AFE_DFE4:
        return(merlin16_pcieg3_INTERNAL_set_rx_dfe4(sa__, val));

    case RX_AFE_DFE5:
        return(merlin16_pcieg3_INTERNAL_set_rx_dfe5(sa__, val));

    default:
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
}


err_code_t merlin16_pcieg3_read_rx_afe(srds_access_t *sa__, enum srds_rx_afe_settings_enum param, int8_t *val) {
  /* Assumes the micro is not actively tuning */
    if(!val) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    switch(param) {
    case RX_AFE_PF:
        EFUN(merlin16_pcieg3_INTERNAL_get_rx_pf_main(sa__, val));
        break;
    case RX_AFE_PF2:
        EFUN(merlin16_pcieg3_INTERNAL_get_rx_pf2(sa__, val));
        break;
    case RX_AFE_VGA:
        EFUN(merlin16_pcieg3_INTERNAL_get_rx_vga(sa__, val));
      break;

    case RX_AFE_DFE1:
        EFUN(merlin16_pcieg3_INTERNAL_get_rx_dfe1(sa__, val));
        break;
    case RX_AFE_DFE2:
        EFUN(merlin16_pcieg3_INTERNAL_get_rx_dfe2(sa__, val));
        break;
    case RX_AFE_DFE3:
        EFUN(merlin16_pcieg3_INTERNAL_get_rx_dfe3(sa__, val));
        break;
    case RX_AFE_DFE4:
        EFUN(merlin16_pcieg3_INTERNAL_get_rx_dfe4(sa__, val));
        break;
    case RX_AFE_DFE5:
        EFUN(merlin16_pcieg3_INTERNAL_get_rx_dfe5(sa__, val));
        break;
    default:
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    return(ERR_CODE_NONE);
}


/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/



/********************************/
/*  TX_PI Fixed Frequency Mode  */
/********************************/

/* TX_PI Frequency Override (Fixed Frequency Mode) */
err_code_t merlin16_pcieg3_tx_pi_freq_override(srds_access_t *sa__, uint8_t enable, int16_t freq_override_val) {
    if (enable) {
        EFUN(wr_tx_pi_en(0x1));                              /* TX_PI enable :            0 = disabled, 1 = enabled */
        EFUN(wr_tx_pi_freq_override_en(0x1));                /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
        EFUN(wr_tx_pi_freq_override_val(freq_override_val)); /* Fixed Freq Override Value (+/-8192) */
    }
    else {
        EFUN(wr_tx_pi_freq_override_val(0));                 /* Fixed Freq Override Value to 0 */
        EFUN(wr_tx_pi_freq_override_en(0x0));                /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
        EFUN(wr_tx_pi_en(0x0));                              /* TX_PI enable :            0 = disabled, 1 = enabled */
    }
  return (ERR_CODE_NONE);
}
/*********************************/
/*  uc_active and uc_reset APIs  */
/*********************************/

/* Enable or Disable the uC reset */
err_code_t merlin16_pcieg3_uc_reset(srds_access_t *sa__, uint8_t enable) {
  if (enable) {
    /* Assert micro reset and reset all micro registers (all non-status registers written to default value) */
    EFUN(wrc_micro_core_clk_en(0x0));                     /* Disable clock to M0 core */

    EFUN(wrc_micro_master_clk_en(0x0));                   /* Disable clock to microcontroller subsystem */
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD200, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD201, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD202, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD204, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD205, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD206, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD207, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD208, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD209, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD20A, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD20B, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD20C, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD20D, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD20E, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD211, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD212, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD213, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD214, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD215, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD216, 0x0007));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD217, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD218, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD219, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD21A, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD21B, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD220, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD221, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD224, 0x0000));

    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD225, 0x8301));

    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD226, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD228, 0x0101));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD229, 0x0000));
    EFUN(merlin16_pcieg3_pmd_wr_reg(sa__, 0xD22A, 0x0000));  
  }
  else {
    /* De-assert micro reset - Start executing code */
    EFUN(wrc_micro_master_clk_en (0x1));             /* Enable clock to microcontroller subsystem */
    EFUN(wrc_micro_master_rstb   (0x1));             /* De-assert reset to microcontroller sybsystem */
    EFUN(wrc_micro_ra_init       (0x2));             /* Write command for data RAM initialization */
    EFUN(merlin16_pcieg3_INTERNAL_poll_micro_ra_initdone(sa__, 250)); /* Poll status of data RAM initialization */
    EFUN(wrc_micro_ra_init       (0x0));             /* Clear command for data RAM initialization */
    EFUN(wrc_micro_core_clk_en   (0x1));            /* Enable clock to M0 core */
    EFUN(wrc_micro_core_rstb     (0x1));
  }
 return (ERR_CODE_NONE);
}



err_code_t merlin16_pcieg3_init_merlin16_pcieg3_info(srds_access_t *sa__) {
    merlin16_pcieg3_info_t * const merlin16_pcieg3_info_ptr = merlin16_pcieg3_INTERNAL_get_merlin16_pcieg3_info_ptr();
    uint32_t info_table[INFO_TABLE_END / sizeof(uint32_t)];
    uint32_t info_table_signature;
    uint8_t  info_table_version;

    ENULL_MEMSET(merlin16_pcieg3_info_ptr, 0, sizeof(merlin16_pcieg3_info_t));

    if (merlin16_pcieg3_info_ptr == 0) {
        EFUN_PRINTF(("ERROR:  Info table pointer is null.\n"));
        return (_error(ERR_CODE_INFO_TABLE_ERROR));
    }

    EFUN(merlin16_pcieg3_rdblk_uc_prog_ram(sa__, (uint8_t *)info_table, INFO_TABLE_RAM_BASE, INFO_TABLE_END));

    info_table_signature = info_table[INFO_TABLE_OFFS_SIGNATURE / sizeof(uint32_t)];
    info_table_version = (uint8_t)(info_table_signature >> 24);
    if (((info_table_signature & 0x00FFFFFF) != 0x666E49)
        || (!(   ((info_table_version >= 0x32) && (info_table_version <= 0x39))
              || ((info_table_version >= 0x41) && (info_table_version <= 0x5A))))) {
        return (_error(ERR_CODE_INFO_TABLE_ERROR));
    }
    
    merlin16_pcieg3_info_ptr->lane_count = info_table[INFO_TABLE_OFFS_OTHER_SIZE / sizeof(uint32_t)] & 0xFF;
    merlin16_pcieg3_info_ptr->trace_memory_descending_writes = ((info_table[INFO_TABLE_OFFS_OTHER_SIZE / sizeof(uint32_t)] & 0x1000000) != 0);

    merlin16_pcieg3_info_ptr->core_var_ram_size = (info_table[INFO_TABLE_OFFS_OTHER_SIZE / sizeof(uint32_t)] >> 8) & 0xFF;

    merlin16_pcieg3_info_ptr->lane_var_ram_size = (info_table[INFO_TABLE_OFFS_TRACE_LANE_MEM_SIZE / sizeof(uint32_t)] >> 16) & 0xFFFF;

    merlin16_pcieg3_info_ptr->diag_mem_ram_size = (info_table[INFO_TABLE_OFFS_TRACE_LANE_MEM_SIZE / sizeof(uint32_t)] & 0xFFFF) / merlin16_pcieg3_info_ptr->lane_count;
    merlin16_pcieg3_info_ptr->trace_mem_ram_size = (info_table[INFO_TABLE_OFFS_TRACE_LANE_MEM_SIZE / sizeof(uint32_t)] & 0xFFFF);
    
    merlin16_pcieg3_info_ptr->diag_mem_ram_base = info_table[INFO_TABLE_OFFS_TRACE_MEM_PTR / sizeof(uint32_t)];
    merlin16_pcieg3_info_ptr->trace_mem_ram_base = merlin16_pcieg3_info_ptr->diag_mem_ram_base;
    
    merlin16_pcieg3_info_ptr->core_var_ram_base = info_table[INFO_TABLE_OFFS_CORE_MEM_PTR / sizeof(uint32_t)];
    
    merlin16_pcieg3_info_ptr->micro_var_ram_base = info_table[INFO_TABLE_OFFS_MICRO_MEM_PTR / sizeof(uint32_t)];
    merlin16_pcieg3_info_ptr->micro_var_ram_size = (info_table[INFO_TABLE_OFFS_OTHER_SIZE_2 / sizeof(uint32_t)] >> 4) & 0xFF;

    merlin16_pcieg3_info_ptr->lane_var_ram_base = info_table[INFO_TABLE_OFFS_LANE_MEM_PTR / sizeof(uint32_t)];

    

    if (info_table_version < 0x34) {
        merlin16_pcieg3_info_ptr->micro_count = 1;
    } else {
        merlin16_pcieg3_info_ptr->micro_count = info_table[INFO_TABLE_OFFS_OTHER_SIZE_2 / sizeof(uint32_t)] & 0xF;
    }
    merlin16_pcieg3_info_ptr->grp_ram_size = 0;
    merlin16_pcieg3_info_ptr->signature = SRDS_INFO_SIGNATURE;
    return (ERR_CODE_NONE);
}


/***********************************************/
/*  Microcode Load into Program RAM Functions  */
/***********************************************/

/* uCode Load through Register (MDIO) Interface [Return Val = Error_Code (0 = PASS)] */
err_code_t merlin16_pcieg3_ucode_mdio_load(srds_access_t *sa__, uint8_t *ucode_image, uint16_t ucode_len) {
    uint16_t   ucode_len_padded, count = 0;
    uint16_t   wrdata_lsw;
    uint8_t    wrdata_lsb;

    if(!ucode_image) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if (ucode_len > UCODE_MAX_SIZE) {                     /* uCode size should be less than UCODE_MAX_SIZE  */
        return (_error(ERR_CODE_INVALID_UCODE_LEN));
    }

    EFUN(wrc_micro_master_clk_en(0x1));                   /* Enable clock to microcontroller subsystem */
    EFUN(wrc_micro_master_rstb(0x1));                     /* De-assert reset to microcontroller sybsystem */
    EFUN(wrc_micro_master_rstb(0x0));                     /* Assert reset to microcontroller sybsystem - Toggling reset*/
    EFUN(wrc_micro_master_rstb(0x1));                     /* De-assert reset to microcontroller sybsystem */

    EFUN(wrc_micro_ra_init(0x1));                         /* Set initialization command to initialize code RAM */
    EFUN(merlin16_pcieg3_INTERNAL_poll_micro_ra_initdone(sa__, 250)); /* Poll for micro_ra_initdone = 1 to indicate initialization done */
    EFUN(wrc_micro_ra_init(0x0));                         /* Clear initialization command */

    ucode_len_padded = ((ucode_len + 3) & 0xFFFC);        /* Aligning ucode size to 4-byte boundary */

    /* Code to Load microcode */
    EFUN(wrc_micro_autoinc_wraddr_en(0x1));               /* To auto increment RAM write address */
    EFUN(wrc_micro_ra_wrdatasize(0x1));                   /* Select 16bit transfers */
    EFUN(wrc_micro_ra_wraddr_msw(0x0));                   /* Upper 16bits of start address of Program RAM where the ucode is to be loaded */
    EFUN(wrc_micro_ra_wraddr_lsw(0x0));                   /* Lower 16bits of start address of Program RAM where the ucode is to be loaded */

    do {                                                  /* ucode_image loaded 16bits at a time */
        wrdata_lsb = (count < ucode_len) ? ucode_image[count] : 0x0; /* wrdata_lsb read from ucode_image; zero padded to 4byte boundary */
        count++;
        wrdata_lsw = (count < ucode_len) ? ucode_image[count] : 0x0; /* wrdata_msb read from ucode_image; zero padded to 4byte boundary */
        count++;
        wrdata_lsw = ((wrdata_lsw << 8) | wrdata_lsb);               /* 16bit wrdata_lsw formed from 8bit msb and lsb values read from ucode_image */
        EFUN(wrc_micro_ra_wrdata_lsw(wrdata_lsw));                   /* Program RAM lower 16bits write data */
    }   while (count < ucode_len_padded);                 /* Loop repeated till entire image loaded (upto the 4byte boundary) */

    EFUN(wrc_micro_ra_wrdatasize(0x2));                   /* Select 32bit transfers as default */
    EFUN(wrc_micro_core_clk_en(0x1));                     /* Enable clock to M0 core */
    /* EFUN(wrc_micro_core_rstb(0x1)); */                 /* De-assert reset to micro to start executing microcode */
    return (ERR_CODE_NONE);                               /* NO Errors while loading microcode (uCode Load PASS) */
  }


/* Read-back uCode from Program RAM and verify against ucode_image [Return Val = Error_Code (0 = PASS)]  */
err_code_t merlin16_pcieg3_ucode_load_verify(srds_access_t *sa__, uint8_t *ucode_image, uint16_t ucode_len) {

    uint16_t ucode_len_padded, count = 0;
    uint16_t rddata_lsw;
    uint16_t data_lsw;
    uint8_t  rddata_lsb;

    if(!ucode_image) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    ucode_len_padded = ((ucode_len + 3) & 0xFFFC);        /* Aligning ucode size to 4-byte boundary */

    if (ucode_len_padded > UCODE_MAX_SIZE) {              /* uCode size should be less than UCODE_MAX_SIZE */
        return (_error(ERR_CODE_INVALID_UCODE_LEN));
    }

    EFUN(wrc_micro_autoinc_rdaddr_en(0x1));               /* To auto increment RAM read address */
    EFUN(wrc_micro_ra_rddatasize(0x1));                   /* Select 16bit transfers */
    EFUN(wrc_micro_ra_rdaddr_msw(0x0));                   /* Upper 16bits of start address of Program RAM from where to read ucode */
    EFUN(wrc_micro_ra_rdaddr_lsw(0x0));                   /* Lower 16bits of start address of Program RAM from where to read ucode */

    do {                                                  /* ucode_image read 16bits at a time */
        rddata_lsb = (count < ucode_len) ? ucode_image[count] : 0x0; /* rddata_lsb read from ucode_image; zero padded to 4byte boundary */
        count++;
        rddata_lsw = (count < ucode_len) ? ucode_image[count] : 0x0; /* rddata_msb read from ucode_image; zero padded to 4byte boundary */
        count++;
        rddata_lsw = ((rddata_lsw << 8) | rddata_lsb);               /* 16bit rddata_lsw formed from 8bit msb and lsb values read from ucode_image */
                                                                     /* Compare Program RAM ucode to ucode_image (Read to micro_ra_rddata_lsw reg auto-increments the ram_address) */
        ESTM(data_lsw = rdc_micro_ra_rddata_lsw());
        if (data_lsw != rddata_lsw) {
            EFUN_PRINTF(("Ucode_Load_Verify_FAIL: Addr = 0x%x: Read_data = 0x%x :  Expected_data = 0x%x \n",(count-2),data_lsw,rddata_lsw));
            return (_error(ERR_CODE_UCODE_VERIFY_FAIL));             /* Verify uCode FAIL */
        }

    } while (count < ucode_len_padded);                   /* Loop repeated till entire image loaded (upto the 4byte boundary) */

    EFUN(wrc_micro_ra_rddatasize(0x2));                   /* Select 32bit transfers ad default */
    return (ERR_CODE_NONE);                               /* Verify uCode PASS */
}

/******************************************************************/
/*  APIs to Align TX clocks                                       */
/******************************************************************/
err_code_t merlin16_pcieg3_tx_clock_align(srds_access_t *sa__, int num_lanes, int enable) {
    int current_lane, lane;
    
    ESTM(current_lane = merlin16_pcieg3_get_lane(sa__));

    for(lane = 0;lane<num_lanes;++lane) {
        EFUN(merlin16_pcieg3_set_lane(sa__,lane));
        if(enable) {
            if(lane == current_lane) {              /* Current Lane should be the master lane */
                EFUN(wr_ams_tx_sel_txmaster(1));    /* Select one lane as txmaster. For all other lanes, this should be 0. */
                EFUN(wr_ams_tx_pd_phasedet(0));     /* 1 - TCA PD powerdown, 0 - enable PD  (enable only for the slave lanes) */
            } else {
           /* Initial TX lane clock phase alignment. Program following registers only for slave lanes, master lane registers should not be programmed (can be kept in default values). */
                EFUN(wr_tx_pi_pd_bypass_vco(1));        /* for quick phase lock time */
                EFUN(wr_tx_pi_pd_bypass_flt(1));      /*  for quick phase lock time */
                EFUN(wr_tx_pi_ext_pd_sel(0));         /*  selects PD path based on tx_pi_repeater_mode_en */
                EFUN(wr_tx_pi_repeater_mode_en(1));   /*  selects external PD from afe as input to tx_pi */
                EFUN(wr_tx_pi_en(1));                 /*  Enable TX_PI    */
                EFUN(wr_tx_pi_jitter_filter_en(0));   /*  turn off frequency lock for tx_pi as all te tclk clocks are derived from VCO and same frequency */
                EFUN(wr_tx_pi_ext_ctrl_en(1));        /*  turn on PD path to TX_PI to lock the clocks */
                EFUN(USR_DELAY_US(25));                   /*  wait for the slave lane clocks to lock to the master lane clock */
                EFUN(wr_tx_pi_ext_phase_bwsel_integ(3)); /*  0 to 7: higher value means faster lock time */
                EFUN(wr_tx_pi_pd_bypass_vco(0));         /*  disable filter bypass for more accurate lock */
                EFUN(wr_tx_pi_pd_bypass_flt(0));         /*  disable filter bypass for more accurate lock */
                EFUN(USR_DELAY_US(60));                   /*  wait for the slave lane clocks to lock to the master lane clock withing 1/16th of UI. */
                /* that all TX lane clocks are correct frequency and aligned together within 1/16th of UI. */
            }
        
        } else {
            if(lane == current_lane) {              /* Current Lane should be the master lane */
                EFUN(wr_ams_tx_sel_txmaster(0));    /* Select one lane as txmaster. For all other lanes, this should be 0. */
                EFUN(wr_ams_tx_pd_phasedet(1));     /* 1 - TCA PD powerdown, 0 - enable PD  (enable only for the slave lanes) */
            } else {
           /* Initial TX lane clock phase alignment. Program following registers only for slave lanes, master lane registers should not be programmed (can be kept in default values). */
                EFUN(wr_tx_pi_pd_bypass_vco(0));         /* for quick phase lock time */
                EFUN(wr_tx_pi_pd_bypass_flt(0));      /*  for quick phase lock time */
                EFUN(wr_tx_pi_ext_pd_sel(0));         /*  selects PD path based on tx_pi_repeater_mode_en */
                EFUN(wr_tx_pi_repeater_mode_en(1));   /*  selects external PD from afe as input to tx_pi */
                EFUN(wr_tx_pi_en(0));                 /*  Enable TX_PI    */
                EFUN(wr_tx_pi_jitter_filter_en(0));   /*  turn off frequency lock for tx_pi as all te tclk clocks are derived from VCO and same frequency */
                EFUN(wr_tx_pi_ext_ctrl_en(0));        /*  turn on PD path to TX_PI to lock the clocks */
                EFUN(wr_tx_pi_ext_phase_bwsel_integ(0)); /*  0 to 7: higher value means faster lock time */
                EFUN(wr_tx_pi_pd_bypass_vco(0));         /*  disable filter bypass for more accurate lock */
                /* that all TX lane clocks are correct frequency and aligned together within 1/16th of UI. */
            }

        }
    }
    EFUN(merlin16_pcieg3_set_lane(sa__,current_lane)); /* return lane to previous state */
    return(ERR_CODE_NONE);
 }



