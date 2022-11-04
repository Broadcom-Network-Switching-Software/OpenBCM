/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2021 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  osprey7_v2l8p2_config.c                                           *
 *  Created On    :  03 Nov 2015                                                   *
 *  Created By    :  Brent Roberts                                                 *
 *  Description   :  API config functions Serdes IPs                               *
 *  Revision      :   *
 *                                                                                 *
 ***********************************************************************************
 ***********************************************************************************/


#include <phymod/phymod_system.h>
#include "osprey7_v2l8p2_config.h"
#include "osprey7_v2l8p2_access.h"
#include "osprey7_v2l8p2_common.h"
#include "osprey7_v2l8p2_functions.h"
#include "osprey7_v2l8p2_internal.h"
#include "osprey7_v2l8p2_internal_error.h"
#include "osprey7_v2l8p2_select_defns.h"
#include "osprey7_v2l8p2_enum.h"
#include "osprey7_v2l8p2_dependencies.h"
#include "osprey7_v2l8p2_version.h"
#include "osprey7_v2l8p2_diag.h"

/** @file
 *
 */




/* User specific implementation for creating syncronization object, used for critical section access in initialization routine below */
#ifndef NO_VARIADIC_MACROS
USR_CREATE_LOCK
#endif

#ifndef SMALL_FOOTPRINT
/*******************************/
/*  Stop/Resume RX Adaptation  */
/*******************************/

err_code_t osprey7_v2l8p2_stop_rx_adaptation(srds_access_t *sa__, uint8_t enable) {
  if (enable) {
      err_code_t err_code;
      err_code = osprey7_v2l8p2_pmd_uc_control(sa__, CMD_UC_CTRL_STOP_GRACEFULLY,GRACEFUL_STOP_TIME);
      if(err_code) {
          uint8_t temp=0;
          USR_PRINTF(("Warning Graceful stop request returned error %d; Requesting a forceful stop\n",err_code));
          temp = osprey7_v2l8p2_INTERNAL_stop_micro(sa__,0,&err_code);
          /* return value from osprey7_v2l8p2_INTERNAL_stop_micro - immediate stop is not required */
          /* void casting to avoid compiler warning. */
          UNUSED(temp);
      }
      return(err_code);
  }
  else {
    return(osprey7_v2l8p2_pmd_uc_control(sa__, CMD_UC_CTRL_RESUME,50));
  }
}

err_code_t osprey7_v2l8p2_request_stop_rx_adaptation(srds_access_t *sa__) {

  return(osprey7_v2l8p2_pmd_uc_control_return_immediate(sa__, CMD_UC_CTRL_STOP_GRACEFULLY));
}

/**********************/
/*  uCode CRC Verify  */
/**********************/

err_code_t osprey7_v2l8p2_ucode_crc_verify(srds_access_t *sa__, uint32_t ucode_len, uint16_t expected_crc_value) {
    INIT_SRDS_ERR_CODE
    uint16_t calc_crc;

    UNUSED(ucode_len);
    ESTM(calc_crc = rdc_micro_cr_crc_checksum());

    if(calc_crc != expected_crc_value) {
        EFUN_PRINTF(("Microcode CRC did not match expected=%04x : calculated=%04x\n",expected_crc_value, calc_crc));
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_UC_CRC_NOT_MATCH));
    }

    return(ERR_CODE_NONE);
}

#endif /* SMALL_FOOTPRINT */

/**************************************************************/
/*  APIs to Write Lane Config and User variables into uC RAM  */
/**************************************************************/

err_code_t osprey7_v2l8p2_set_uc_lane_cfg(srds_access_t *sa__, struct osprey7_v2l8p2_uc_lane_config_st struct_val) {
    INIT_SRDS_ERR_CODE
  uint8_t reset_state;
  ESTM(reset_state = rd_rx_lane_dp_reset_state());
  if(reset_state < 7) {
      EFUN_PRINTF(("ERROR: osprey7_v2l8p2_set_uc_lane_cfg(..) called without ln_dp_s_rstb=0\n"));
      return (osprey7_v2l8p2_error(sa__, ERR_CODE_LANE_DP_NOT_RESET));
  }
  EFUN(osprey7_v2l8p2_INTERNAL_update_uc_lane_config_word(&struct_val));
  return(reg_wr_RX_CKRST_CTRL_LANE_UC_CONFIG(struct_val.word));
}

#define SRDS_FORCE_OSCDR_BIT    (3)
err_code_t osprey7_v2l8p2_get_force_lane_cdr_mode(srds_access_t *sa__, enum osprey7_v2l8p2_force_cdr_mode_enum *mode) {
    INIT_SRDS_ERR_CODE
    uint8_t os_mode = 0;
    uint8_t br_mode = 0;

    if(mode == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }
    ESTM(os_mode = ((rdv_usr_misc_ctrl_word() & (0x1 << SRDS_FORCE_OSCDR_BIT)) ? 1 : 0));
    ESTM(br_mode = rd_lane_uc_config_force_brdfe_en());
    if(os_mode && br_mode) {
        return osprey7_v2l8p2_error(sa__, ERR_CODE_INVALID_MODE);
    }

    if(os_mode) {
        *mode = OSPREY7_V2L8P2_OSCDR_FORCE_ENABLE;
    }
    else if(br_mode) {
        *mode = OSPREY7_V2L8P2_BRCDR_FORCE_ENABLE;
    }
    else {
        *mode = OSPREY7_V2L8P2_CDR_FORCE_DISABLE;
    }
    return ERR_CODE_NONE;
}

err_code_t osprey7_v2l8p2_force_lane_cdr_mode(srds_access_t *sa__, enum osprey7_v2l8p2_force_cdr_mode_enum mode) {
    INIT_SRDS_ERR_CODE
    uint16_t misc_ctrl = 0;
    switch(mode) {
        case OSPREY7_V2L8P2_OSCDR_FORCE_ENABLE:
            ESTM(misc_ctrl = rdv_usr_misc_ctrl_word());
            EFUN(wrv_usr_misc_ctrl_word(misc_ctrl | (0x1 << SRDS_FORCE_OSCDR_BIT))); /*  (force_os_on) */
            EFUN(osprey7_v2l8p2_force_lane_cdr_mode(sa__, OSPREY7_V2L8P2_BRCDR_FORCE_DISABLE));
            break;
        case OSPREY7_V2L8P2_OSCDR_FORCE_DISABLE:
            ESTM(misc_ctrl = rdv_usr_misc_ctrl_word());
            EFUN(wrv_usr_misc_ctrl_word(misc_ctrl & (uint16_t)~(0x1 << SRDS_FORCE_OSCDR_BIT)));
            break;
        case OSPREY7_V2L8P2_BRCDR_FORCE_ENABLE:
            EFUN(wr_lane_uc_config_force_brdfe_en(0x1));
            EFUN(osprey7_v2l8p2_force_lane_cdr_mode(sa__, OSPREY7_V2L8P2_OSCDR_FORCE_DISABLE));
            break;
        case OSPREY7_V2L8P2_BRCDR_FORCE_DISABLE:
            EFUN(wr_lane_uc_config_force_brdfe_en(0x0));
            break;
        case OSPREY7_V2L8P2_CDR_FORCE_DISABLE:
            EFUN(osprey7_v2l8p2_force_lane_cdr_mode(sa__, OSPREY7_V2L8P2_OSCDR_FORCE_DISABLE));
            EFUN(osprey7_v2l8p2_force_lane_cdr_mode(sa__, OSPREY7_V2L8P2_BRCDR_FORCE_DISABLE));
            break;
        default:
            return osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }
    return ERR_CODE_NONE;
}

/******************************************************************/
/*  APIs to Read Core/Lane Config and User variables from uC RAM  */
/******************************************************************/

err_code_t osprey7_v2l8p2_get_uc_core_config(srds_access_t *sa__, struct osprey7_v2l8p2_uc_core_config_st *get_val) {
    INIT_SRDS_ERR_CODE
    
    if(!get_val) {
        return osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }
    switch (osprey7_v2l8p2_acc_get_pll_idx(sa__)) {
    case  0:
        ESTM(get_val->word = reg_rdc_CORE_PLL_COM_PLL_UC_CORE_CONFIG());
        break;
    case  1:
        ESTM(get_val->word = reg_rdc_CORE_PLL_COM_PLL_UC_CORE_CONFIG());
        break;
    default:
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    EFUN(osprey7_v2l8p2_INTERNAL_update_uc_core_config_st(get_val));
    return ERR_CODE_NONE;
}
err_code_t osprey7_v2l8p2_get_physical_tx_addr(srds_access_t *sa__, uint8_t lane, uint8_t *physical_tx_lane) {
    INIT_SRDS_ERR_CODE
    uint8_t logical_tx_lane = 0, phy_lane = 0;
    const uint8_t num_lanes = 8;
    for(phy_lane = 0; phy_lane < num_lanes; phy_lane++) {
        EFUN(osprey7_v2l8p2_INTERNAL_get_logical_tx_lane_addr(sa__, phy_lane, &logical_tx_lane));
        if(logical_tx_lane == lane) {
            *physical_tx_lane = phy_lane;
            return (ERR_CODE_NONE);
        }
    }
    *physical_tx_lane = 255;
    return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
}

err_code_t osprey7_v2l8p2_get_physical_rx_addr(srds_access_t *sa__, uint8_t lane, uint8_t *physical_rx_lane) {
    INIT_SRDS_ERR_CODE
    uint8_t logical_rx_lane = 0, phy_lane = 0;
    const uint8_t num_lanes = 8;
    for(phy_lane = 0; phy_lane < num_lanes; phy_lane++) {
        EFUN(osprey7_v2l8p2_INTERNAL_get_logical_rx_lane_addr(sa__, phy_lane, &logical_rx_lane));
        if(logical_rx_lane == lane) {
            *physical_rx_lane = phy_lane;
            return (ERR_CODE_NONE);
        }
    }
    *physical_rx_lane = 255;
    return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
}
#ifndef SMALL_FOOTPRINT
err_code_t osprey7_v2l8p2_get_uc_lane_cfg(srds_access_t *sa__, struct osprey7_v2l8p2_uc_lane_config_st *get_val) {
    INIT_SRDS_ERR_CODE

  if(!get_val) {
     return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  ESTM(get_val->word = reg_rd_RX_CKRST_CTRL_LANE_UC_CONFIG());
  EFUN(osprey7_v2l8p2_INTERNAL_update_uc_lane_config_st(get_val));
  return (ERR_CODE_NONE);
}
#endif /* SMALL_FOOTPRINT */

/*---------------------------------*/
/*  APIs to Reset Lane to Default  */
/*---------------------------------*/

err_code_t osprey7_v2l8p2_reset_tx_lane_to_default(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    EFUN(wr_tx_ln_dp_s_rstb(0x0));
    EFUN(USR_DELAY_US(1));
    EFUN(wr_tx_ln_s_rstb(0x0));
    EFUN(USR_DELAY_US(1));
    EFUN(wr_tx_ln_s_rstb(0x1));
    return ERR_CODE_NONE;
}

err_code_t osprey7_v2l8p2_reset_rx_lane_to_default(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    uint8_t ams_tx_version_id;
    ESTM(ams_tx_version_id = rd_ams_tx_version_id());
    EFUN(wr_rx_ln_dp_s_rstb(0x0));
    EFUN(USR_DELAY_US(1));
    EFUN(wr_rx_ln_s_rstb(0x0));
    EFUN(USR_DELAY_US(1));
    EFUN(wr_rx_ln_s_rstb(0x1));
    if(ams_tx_version_id == 0xA0) {
        EFUN(wr_ams_rx_ro_clkdata_enb(0x1));
        EFUN(wr_ams_rx_vdd_qual(0x1));
    }
    return ERR_CODE_NONE;
}

err_code_t osprey7_v2l8p2_reset_lane_to_default(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    uint8_t ams_tx_version_id;
    ESTM(ams_tx_version_id = rd_ams_tx_version_id());
    EFUN(wr_ln_dp_s_rstb(0x0));
    EFUN(USR_DELAY_US(1));
    EFUN(wr_ln_s_rstb(0x0));
    EFUN(USR_DELAY_US(1));
    EFUN(wr_ln_s_rstb(0x1));
    if(ams_tx_version_id == 0xA0) {
        EFUN(wr_ams_rx_ro_clkdata_enb(0x1));
        EFUN(wr_ams_rx_vdd_qual(0x1));
    }
    return (ERR_CODE_NONE);
}

/*--------------------------------------------*/
/*  APIs to Enable or Disable datapath reset  */
/*--------------------------------------------*/


err_code_t osprey7_v2l8p2_tx_dp_reset(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    if (enable) {
        EFUN(wr_tx_ln_dp_s_rstb(0x0));
    } else {
        EFUN(wr_tx_ln_dp_s_rstb(0x1));
    }
    return ERR_CODE_NONE;
}

err_code_t osprey7_v2l8p2_rx_dp_reset(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    if (enable) {
        EFUN(wr_rx_ln_dp_s_rstb(0x0));
    } else {
        EFUN(wr_rx_ln_dp_s_rstb(0x1));
    }
    return ERR_CODE_NONE;
}

err_code_t osprey7_v2l8p2_core_dp_reset(srds_access_t *sa__, uint8_t enable){
    INIT_SRDS_ERR_CODE
    if (enable) {
        EFUN(wrc_core_dp_s_rstb(0x0));
    } else {
        EFUN(wrc_core_dp_s_rstb(0x1));
    }
    return ERR_CODE_NONE;
}
err_code_t osprey7_v2l8p2_ln_dp_reset(srds_access_t *sa__, uint8_t enable){
    INIT_SRDS_ERR_CODE
    if (enable) {
        EFUN(wr_ln_dp_s_rstb(0x0));
    } else {
        EFUN(wr_ln_dp_s_rstb(0x1));
    }
    return ERR_CODE_NONE;
}

/*-----------------------------------------*/
/*  APIs for setting and getting OSR mode  */
/*-----------------------------------------*/

err_code_t osprey7_v2l8p2_set_use_osr_mode_pins_only(srds_access_t *sa__, uint8_t use_pins_only) {
    INIT_SRDS_ERR_CODE

    EFUN(osprey7_v2l8p2_set_use_tx_osr_mode_pins_only(sa__, use_pins_only));
    EFUN(osprey7_v2l8p2_set_use_rx_osr_mode_pins_only(sa__, use_pins_only));

    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_get_use_osr_mode_pins_only(srds_access_t *sa__, uint8_t *tx_use_pins_only, uint8_t *rx_use_pins_only) {
    INIT_SRDS_ERR_CODE

    if(tx_use_pins_only == NULL || rx_use_pins_only == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }

    EFUN(osprey7_v2l8p2_get_use_tx_osr_mode_pins_only(sa__, tx_use_pins_only));
    EFUN(osprey7_v2l8p2_get_use_rx_osr_mode_pins_only(sa__, rx_use_pins_only));

    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_set_use_rx_osr_mode_pins_only(srds_access_t *sa__, uint8_t rx_use_pins_only) {
    INIT_SRDS_ERR_CODE
    uint8_t reset_state;
    ESTM(reset_state = rd_rx_lane_dp_reset_state());
    if(reset_state < 7) {
        EFUN_PRINTF(("ERROR: osprey7_v2l8p2_set_use_rx_osr_mode_pins_only(..) called without ln_dp_s_rstb=0\n"));
        return (osprey7_v2l8p2_error(sa__, ERR_CODE_LANE_DP_NOT_RESET));
    }
    EFUN(wrv_usr_rx_osr_ctrl_byte((uint8_t)((rx_use_pins_only << 7) | OSPREY7_V2L8P2_OSR_UNINITIALIZED)));

    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_get_use_rx_osr_mode_pins_only(srds_access_t *sa__, uint8_t *rx_use_pins_only) {
    INIT_SRDS_ERR_CODE

    if(rx_use_pins_only == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }
    ESTM(*rx_use_pins_only = rdv_usr_rx_osr_ctrl_byte() >> 7);

    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_set_use_tx_osr_mode_pins_only(srds_access_t *sa__, uint8_t tx_use_pins_only) {
    INIT_SRDS_ERR_CODE
    uint8_t reset_state;
    ESTM(reset_state = rd_tx_lane_dp_reset_state());
    if(reset_state < 7) {
        EFUN_PRINTF(("ERROR: osprey7_v2l8p2_set_use_tx_osr_mode_pins_only(..) called without ln_dp_s_rstb=0\n"));
        return (osprey7_v2l8p2_error(sa__, ERR_CODE_LANE_DP_NOT_RESET));
    }
    EFUN(wrv_usr_tx_osr_ctrl_byte((uint8_t)((tx_use_pins_only << 7) | OSPREY7_V2L8P2_OSR_UNINITIALIZED)));

    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_get_use_tx_osr_mode_pins_only(srds_access_t *sa__, uint8_t *tx_use_pins_only) {
    INIT_SRDS_ERR_CODE

    if(tx_use_pins_only == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }
    ESTM(*tx_use_pins_only = rdv_usr_tx_osr_ctrl_byte() >> 7);

    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_set_osr_mode(srds_access_t *sa__, enum osprey7_v2l8p2_osr_mode_enum osr_mode) {
    INIT_SRDS_ERR_CODE

    EFUN(osprey7_v2l8p2_set_tx_osr_mode(sa__, osr_mode));
    EFUN(osprey7_v2l8p2_set_rx_osr_mode(sa__, osr_mode));

    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_get_osr_mode(srds_access_t *sa__, enum osprey7_v2l8p2_osr_mode_enum *tx_osr_mode,  enum osprey7_v2l8p2_osr_mode_enum *rx_osr_mode) {
    INIT_SRDS_ERR_CODE

    if(tx_osr_mode == NULL || rx_osr_mode == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }

    EFUN(osprey7_v2l8p2_get_tx_osr_mode(sa__, tx_osr_mode));
    EFUN(osprey7_v2l8p2_get_rx_osr_mode(sa__, rx_osr_mode));

    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_set_rx_osr_mode(srds_access_t *sa__, enum osprey7_v2l8p2_osr_mode_enum rx_osr_mode) {
    INIT_SRDS_ERR_CODE
    uint8_t reset_state;
    ESTM(reset_state = rd_rx_lane_dp_reset_state());
    if(reset_state < 7) {
        EFUN_PRINTF(("ERROR: osprey7_v2l8p2_set_rx_osr_mode(..) called without ln_dp_s_rstb=0\n"));
        return (osprey7_v2l8p2_error(sa__, ERR_CODE_LANE_DP_NOT_RESET));
    }

    EFUN(wrv_usr_rx_osr_ctrl_byte((uint8_t)rx_osr_mode));
    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_get_rx_osr_mode(srds_access_t *sa__, enum osprey7_v2l8p2_osr_mode_enum *rx_osr_mode) {
    INIT_SRDS_ERR_CODE
    uint8_t rx_osr_ctrl;

    if(rx_osr_mode == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }

    ESTM(rx_osr_ctrl = rdv_usr_rx_osr_ctrl_byte());
    if(!(rx_osr_ctrl & (1 << 7))) {
        *rx_osr_mode = (enum osprey7_v2l8p2_osr_mode_enum)(rx_osr_ctrl & 0x3F);
    }
    else {
        ESTM(*rx_osr_mode = (enum osprey7_v2l8p2_osr_mode_enum)rd_rx_osr_mode_pin());
    }

    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_set_tx_osr_mode(srds_access_t *sa__, enum osprey7_v2l8p2_osr_mode_enum tx_osr_mode) {
    INIT_SRDS_ERR_CODE
    uint8_t reset_state;
    ESTM(reset_state = rd_tx_lane_dp_reset_state());
    if(reset_state < 7) {
        EFUN_PRINTF(("ERROR: osprey7_v2l8p2_set_tx_osr_mode(..) called without ln_dp_s_rstb=0\n"));
        return (osprey7_v2l8p2_error(sa__, ERR_CODE_LANE_DP_NOT_RESET));
    }
    EFUN(wrv_usr_tx_osr_ctrl_byte((uint8_t)tx_osr_mode));

    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_get_tx_osr_mode(srds_access_t *sa__, enum osprey7_v2l8p2_osr_mode_enum *tx_osr_mode) {
    INIT_SRDS_ERR_CODE
    uint8_t tx_osr_ctrl;

    if(tx_osr_mode == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }

    ESTM(tx_osr_ctrl = rdv_usr_tx_osr_ctrl_byte());
    if(!(tx_osr_ctrl & (1 << 7))) {
        *tx_osr_mode = (enum osprey7_v2l8p2_osr_mode_enum)(tx_osr_ctrl & 0x3F);
    }
    else {
        ESTM(*tx_osr_mode = (enum osprey7_v2l8p2_osr_mode_enum)rd_tx_osr_mode_pin());
    }

    return(ERR_CODE_NONE);
}

/********************************/
/*  Loading ucode through PRAM  */
/********************************/
#ifndef SMALL_FOOTPRINT
err_code_t osprey7_v2l8p2_ucode_pram_load(srds_access_t *sa__, char const * ucode_image, uint32_t ucode_len) {
    INIT_SRDS_ERR_CODE
    uint16_t ucode_len_remainder = (uint16_t)(((ucode_len + 3) & 0xFFFFFFFC) - ucode_len);

    if(!ucode_image) {
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    EFUN(osprey7_v2l8p2_ucode_load_init(sa__, 1));

    /* Wait 8 pram clocks */
    EFUN(USR_DELAY_US(1));

    /* write ucode into pram */
    while (ucode_len > 0) {
        EFUN(osprey7_v2l8p2_acc_wr_pram(sa__, (uint8_t)*ucode_image));
        --ucode_len;
        ++ucode_image;
    }

    /* Pad to 32 bits */
    while (ucode_len_remainder > 0) {
        EFUN(osprey7_v2l8p2_acc_wr_pram(sa__, 0));
        --ucode_len_remainder;
    }

    /* Wait 8 pram clocks */
    EFUN(USR_DELAY_US(1));

    EFUN(osprey7_v2l8p2_ucode_load_close(sa__, 1));
    return(ERR_CODE_NONE);
}

/************************************/
/*  Accessing core_config_from_pcs  */
/************************************/

err_code_t osprey7_v2l8p2_set_core_config_from_pcs(srds_access_t *sa__, uint8_t core_cfg_from_pcs) {
    INIT_SRDS_ERR_CODE
    struct osprey7_v2l8p2_uc_core_config_st core_config = UC_CORE_CONFIG_INIT;
    EFUN(osprey7_v2l8p2_get_uc_core_config(sa__, &core_config));
    core_config.field.core_cfg_from_pcs = core_cfg_from_pcs;
    EFUN(osprey7_v2l8p2_INTERNAL_set_uc_core_config(sa__, core_config));
    return (ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_set_osr_5_en(srds_access_t *sa__, uint8_t osr_5_en) {
    INIT_SRDS_ERR_CODE
    struct osprey7_v2l8p2_uc_core_config_st core_config = UC_CORE_CONFIG_INIT;
    EFUN(osprey7_v2l8p2_get_uc_core_config(sa__, &core_config));
    core_config.field.osr_5_en = osr_5_en;
    EFUN(osprey7_v2l8p2_INTERNAL_set_uc_core_config(sa__, core_config));
    return (ERR_CODE_NONE);
}
/******************/
/*  Lane Mapping  */
/******************/

err_code_t osprey7_v2l8p2_map_lanes(srds_access_t *sa__, const uint8_t num_lanes, uint8_t const *tx_lane_map, uint8_t const *rx_lane_map) {
    INIT_SRDS_ERR_CODE
    uint8_t rd_val = 0;
    /* Verify that the core data path is held in reset. */
    ESTM(rd_val = rdc_core_dp_s_rstb());
    if (rd_val != 0) {
        EFUN_PRINTF(("ERROR: core data path reset is not asserted\n"));
        return (ERR_CODE_UC_NOT_RESET);
    }

    /* Verify that all micros are held in reset. */

    {
        uint8_t micro_orig, num_micros, micro_idx;
        ESTM(micro_orig = osprey7_v2l8p2_acc_get_micro_idx(sa__));
        EFUN(osprey7_v2l8p2_get_micro_num_uc_cores(sa__, &num_micros));
        for (micro_idx = 0; micro_idx < num_micros; micro_idx++) {
            EFUN(osprey7_v2l8p2_acc_set_micro_idx(sa__, micro_idx));
            ESTM(rd_val |= rdc_micro_core_rstb());
        }
        EFUN(osprey7_v2l8p2_acc_set_micro_idx(sa__, micro_orig));
    }
    if (rd_val != 0) {
        return (ERR_CODE_UC_NOT_RESET);
    }

    /* Verify that the num_lanes parameter is correct. */
    ESTM(rd_val = rdc_revid_multiplicity());
    if (rd_val != num_lanes) {
        return (ERR_CODE_BAD_LANE_COUNT);
    }

    /* Verify that tx_lane_map and rx_lane_map are valid. */
    {
        uint8_t index1, index2;
#ifndef PHYMOD_SUPPORT
        uint8_t lp_message_printed = 0;
#endif
        for (index1=0; index1<num_lanes; ++index1) {

            /* Verify that a lane map is not to an invalid lane. */
            if ((tx_lane_map[index1] >= num_lanes)
                || (rx_lane_map[index1] >= num_lanes)){
                return (ERR_CODE_BAD_LANE);
            }
#ifndef PHYMOD_SUPPORT
            /* Warn if an RX lane mapping is not the same as TX. */
            if ((tx_lane_map[index1] != rx_lane_map[index1])
                && !lp_message_printed) {
                ESTM_PRINTF(("Warning:  In core %d, TX lane %d is mapped to %d, while RX lane %d is mapped to %d.\n          Digital and remote loopback will not operate as expected.\n          Further warnings are suppressed.\n", osprey7_v2l8p2_acc_get_core(sa__), index1, tx_lane_map[index1], index1, rx_lane_map[index1]));
                lp_message_printed = 1;
            }
#endif
            /* Verify that a lane map is not used twice. */
            for (index2=(uint8_t)(index1+1); index2<num_lanes; ++index2) {
                if ((tx_lane_map[index1] == tx_lane_map[index2])
                    || (rx_lane_map[index1] == rx_lane_map[index2])) {
                    return (ERR_CODE_BAD_LANE);
                }
            }
        }
    }

    /* Write the map bitfields.
     * Support up to 8 lanes.
     */
    EFUN(wrc_tx_lane_addr_0(*(tx_lane_map++))); EFUN(wrc_rx_lane_addr_0(*(rx_lane_map++)));
    EFUN(wrc_tx_lane_addr_1(*(tx_lane_map++))); EFUN(wrc_rx_lane_addr_1(*(rx_lane_map++)));
    EFUN(wrc_tx_lane_addr_2(*(tx_lane_map++))); EFUN(wrc_rx_lane_addr_2(*(rx_lane_map++)));
    EFUN(wrc_tx_lane_addr_3(*(tx_lane_map++))); EFUN(wrc_rx_lane_addr_3(*(rx_lane_map++)));
    EFUN(wrc_tx_lane_addr_4(*(tx_lane_map++))); EFUN(wrc_rx_lane_addr_4(*(rx_lane_map++)));
    EFUN(wrc_tx_lane_addr_5(*(tx_lane_map++))); EFUN(wrc_rx_lane_addr_5(*(rx_lane_map++)));
    EFUN(wrc_tx_lane_addr_6(*(tx_lane_map++))); EFUN(wrc_rx_lane_addr_6(*(rx_lane_map++)));
    EFUN(wrc_tx_lane_addr_7(*(tx_lane_map++))); EFUN(wrc_rx_lane_addr_7(*(rx_lane_map++)));
    return (ERR_CODE_NONE);
}

/************************/
/*  Serdes API Version  */
/************************/

err_code_t osprey7_v2l8p2_version(srds_access_t *sa__, uint32_t *api_version) {
    if(!api_version) {
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    *api_version = ((API_MAJOR_VERSION << 8) | API_MINOR_VERSION);
    return (ERR_CODE_NONE);
}
#endif /* SMALL_FOOTPRINT */

/*****************/
/*  PMD_RX_LOCK  */
/*****************/

err_code_t osprey7_v2l8p2_pmd_lock_status(srds_access_t *sa__, uint8_t *pmd_rx_lock) {
    INIT_SRDS_ERR_CODE
    if(!pmd_rx_lock) {
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    ESTM(*pmd_rx_lock = rd_pmd_rx_lock());
    return (ERR_CODE_NONE);
}

/**********************************/
/*  Serdes TX disable/RX Restart  */
/**********************************/

err_code_t osprey7_v2l8p2_tx_disable(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE

    if (enable) {
        EFUN(wr_sdk_tx_disable(0x1));
    }
    else {
        EFUN(wr_sdk_tx_disable(0x0));
    }
    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_rx_restart(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE

    EFUN(wr_rx_restart_pmd_hold(enable));
    return(ERR_CODE_NONE);
}

#if !defined(SMALL_FOOTPRINT) 
/******************************************************/
/*  Single function to set/get all RX AFE parameters  */
/******************************************************/

err_code_t osprey7_v2l8p2_write_rx_afe(srds_access_t *sa__, enum srds_rx_afe_settings_enum param, int8_t val) {
  /* Assumes the micro is not actively tuning */

    switch(param) {
    case RX_AFE_PF:
        return(osprey7_v2l8p2_INTERNAL_set_rx_pf_main(sa__, (uint8_t)val));

    case RX_AFE_PF2:
        return(osprey7_v2l8p2_INTERNAL_set_rx_pf2(sa__, (uint8_t)val));

    case RX_AFE_VGA:
      return(osprey7_v2l8p2_INTERNAL_set_rx_vga(sa__, (uint8_t)val));

    case RX_AFE_FGA:
      return(osprey7_v2l8p2_INTERNAL_set_rx_fga(sa__, (uint8_t)val));

    case RX_AFE_DFE1:
        return(osprey7_v2l8p2_INTERNAL_set_rx_dfe1(sa__, val));

    case RX_AFE_DFE2:
        return(osprey7_v2l8p2_INTERNAL_set_rx_dfe2(sa__, val));

    case RX_AFE_DFE3:
        return(osprey7_v2l8p2_INTERNAL_set_rx_dfe3(sa__, val));

    case RX_AFE_DFE4:
        return(osprey7_v2l8p2_INTERNAL_set_rx_dfe4(sa__, val));

    case RX_AFE_DFE5:
        return(osprey7_v2l8p2_INTERNAL_set_rx_dfe5(sa__, val));

    case RX_AFE_DFE6:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe6(sa__, val));

    case RX_AFE_DFE7:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe7(sa__, val));

    case RX_AFE_DFE8:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe8(sa__, val));

    case RX_AFE_DFE9:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe9(sa__, val));

    case RX_AFE_DFE10:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe10(sa__, val));

    case RX_AFE_DFE11:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe11(sa__, val));

    case RX_AFE_DFE12:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe12(sa__, val));

    case RX_AFE_DFE13:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe13(sa__, val));

    case RX_AFE_DFE14:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe14(sa__, val));

    case RX_AFE_DFE15:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe15(sa__, val));

    case RX_AFE_DFE16:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe16(sa__, val));

    case RX_AFE_DFE17:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe17(sa__, val));

    case RX_AFE_DFE18:
        return (osprey7_v2l8p2_INTERNAL_set_rx_dfe18(sa__, val));

    case RX_AFE_FFE_PRE:
        return (osprey7_v2l8p2_INTERNAL_set_rx_ffe_pre(sa__, val));

    case RX_AFE_FFE_POST:
        return (osprey7_v2l8p2_INTERNAL_set_rx_ffe_post(sa__, val));

    case RX_AFE_PF3:
        return(osprey7_v2l8p2_INTERNAL_set_rx_pf3(sa__, (uint8_t)val));
    default:
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
}
#endif /* !SMALL_FOOTPRINT */

#ifndef SMALL_FOOTPRINT
err_code_t osprey7_v2l8p2_read_rx_afe(srds_access_t *sa__, enum srds_rx_afe_settings_enum param, int8_t *val) {
    INIT_SRDS_ERR_CODE
    /* Assumes the micro is not actively tuning */

    uint8_t rd_val;

    if(!val) {
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    switch(param) {
    case RX_AFE_PF:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_pf_main(sa__, &rd_val));
        *val = (int8_t)rd_val;
        break;
    case RX_AFE_PF2:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_pf2(sa__, &rd_val));
        *val = (int8_t)rd_val;
        break;
    case RX_AFE_VGA:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_vga(sa__, &rd_val));
        *val = (int8_t)rd_val;
      break;
    case RX_AFE_FGA:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_fga(sa__, &rd_val));
        *val = (int8_t)rd_val;
      break;
    case RX_AFE_DFE1:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe1(sa__, val));
        break;
    case RX_AFE_DFE2:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe2(sa__, val));
        break;
    case RX_AFE_DFE3:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe3(sa__, val));
        break;
    case RX_AFE_DFE4:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe4(sa__, val));
        break;
    case RX_AFE_DFE5:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe5(sa__, val));
        break;
    case RX_AFE_DFE6:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe6(sa__, val));
        break;
    case RX_AFE_DFE7:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe7(sa__, val));
        break;
    case RX_AFE_DFE8:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe8(sa__, val));
        break;
    case RX_AFE_DFE9:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe9(sa__, val));
        break;
    case RX_AFE_DFE10:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe10(sa__, val));
        break;
    case RX_AFE_DFE11:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe11(sa__, val));
        break;
    case RX_AFE_DFE12:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe12(sa__, val));
        break;
    case RX_AFE_DFE13:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe13(sa__, val));
        break;
    case RX_AFE_DFE14:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe14(sa__, val));
        break;
    case RX_AFE_DFE15:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe15(sa__, val));
        break;
    case RX_AFE_DFE16:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe16(sa__, val));
        break;
    case RX_AFE_DFE17:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe17(sa__, val));
        break;
    case RX_AFE_DFE18:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_dfe18(sa__, val));
        break;
    case RX_AFE_FFE_PRE:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_ffe_pre(sa__, val));
        break;
    case RX_AFE_FFE_POST:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_ffe_post(sa__, val));
        break;
    case RX_AFE_PF3:
        EFUN(osprey7_v2l8p2_INTERNAL_get_rx_pf3(sa__, &rd_val));
        *val = (int8_t)rd_val;
        break;
    default:
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    UNUSED(rd_val);
    return(ERR_CODE_NONE);
}
#endif /* SMALL_FOOTPRINT */

err_code_t osprey7_v2l8p2_validate_txfir_cfg(srds_access_t *sa__, enum osprey7_v2l8p2_txfir_tap_enable_enum enable_taps, int16_t pre3, int16_t pre2, int16_t pre1, int16_t main, int16_t post1, int16_t post2) {
    INIT_SRDS_ERR_CODE
    osprey7_v2l8p2_txfir_st txfir;

    ENULL_MEMSET(&txfir, 0, sizeof(osprey7_v2l8p2_txfir_st));
    if ((enable_taps == OSPREY7_V2L8P2_NRZ_6TAP) || (enable_taps == OSPREY7_V2L8P2_PAM4_6TAP)) {
        txfir.tap[0] = pre3;
        txfir.tap[1] = pre2;
        txfir.tap[2] = pre1;
        txfir.tap[3] = main;
        txfir.tap[4] = post1;
        txfir.tap[5] = post2;
    }
    else {
        txfir.tap[0] = pre1;
        txfir.tap[1] = main;
        txfir.tap[2] = post1;
        if ((pre3 != 0) || (pre2 != 0) || (post2 != 0)) {
            EFUN_PRINTF(("ERROR: Selected 3 TAPs option, but other TAP inputs (pre3, pre2, post2) have non-zero value\n"));
            return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        }
    }

    if (main < 0  || post1 > 0 || pre1 > 0) {
        EFUN_PRINTF(("ERROR: main TAP is expected to be positive and pre1 and post1 TAPs are expected to be negative for normal use cases."));
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    if (main < SRDS_ABS(pre1) || main < pre2 || main < SRDS_ABS(pre3) || main < SRDS_ABS(post1) || main < post2) {
        EFUN_PRINTF(("ERROR: main TAP is expected to be greater than all other TAPs."));
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    if(pre3 > 0 || pre3 < -12) {
        EFUN_PRINTF(("ERROR: pre3 TAP is expected to be within [-12,0]."));
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    return osprey7_v2l8p2_INTERNAL_validate_full_txfir_cfg(sa__, enable_taps, &txfir);
}
static uint8_t const tap_num_invalid = 255;

/* Return the FIR tap index for the given parameter, or tap_num_invalid if the parameter is not an FIR tap. */
static uint8_t _osprey7_v2l8p2_get_fir_tap_index(enum osprey7_v2l8p2_tx_afe_settings_enum param) {
    switch(param) {
    case OSPREY7_V2L8P2_TX_AFE_TAP0:  return  0;
    case OSPREY7_V2L8P2_TX_AFE_TAP1:  return  1;
    case OSPREY7_V2L8P2_TX_AFE_TAP2:  return  2;
    case OSPREY7_V2L8P2_TX_AFE_TAP3:  return  3;
    case OSPREY7_V2L8P2_TX_AFE_TAP4:  return  4;
    case OSPREY7_V2L8P2_TX_AFE_TAP5:  return  5;
    default:           return tap_num_invalid;
    }
}

#ifndef SMALL_FOOTPRINT
err_code_t osprey7_v2l8p2_write_tx_afe(srds_access_t *sa__, enum osprey7_v2l8p2_tx_afe_settings_enum param, int16_t val) {
    INIT_SRDS_ERR_CODE
    uint8_t const tap_num = _osprey7_v2l8p2_get_fir_tap_index(param);
    uint8_t  in_nrz_range;

    ESTM(in_nrz_range = rd_txfir_nrz_tap_range_sel());

    if (tap_num != tap_num_invalid) {
        if (((!in_nrz_range) && ((val < TXFIR_PAM4_SW_TAP_MIN) || (val > TXFIR_PAM4_SW_TAP_MAX))) ||
            ((in_nrz_range)  && ((val <     TXFIR_NRZ_TAP_MIN) || (val >     TXFIR_NRZ_TAP_MAX)))) {
            return(osprey7_v2l8p2_error(sa__, ERR_CODE_TXFIR_MAIN_INVALID));
        }
        EFUN(osprey7_v2l8p2_INTERNAL_set_tx_tap(sa__, tap_num, val));
        EFUN(osprey7_v2l8p2_INTERNAL_load_txfir_taps(sa__));
    } else {
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    return(ERR_CODE_NONE);
}
#endif /* SMALL_FOOTPRINT */

err_code_t osprey7_v2l8p2_read_tx_afe(srds_access_t *sa__, enum osprey7_v2l8p2_tx_afe_settings_enum param, int16_t *val) {
    INIT_SRDS_ERR_CODE
    if(!val) {
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    {
        uint8_t const tap_num = _osprey7_v2l8p2_get_fir_tap_index(param);
        if (tap_num != tap_num_invalid) {
            EFUN(osprey7_v2l8p2_INTERNAL_get_tx_tap(sa__, tap_num, val));
        } else {
            return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        }
    }
    return(ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_apply_txfir_cfg(srds_access_t *sa__, enum osprey7_v2l8p2_txfir_tap_enable_enum enable_taps, int16_t pre3, int16_t pre2, int16_t pre1, int16_t main, int16_t post1, int16_t post2) {
    INIT_SRDS_ERR_CODE
    osprey7_v2l8p2_txfir_st txfir;
#if !defined(SMALL_FOOTPRINT) 
    struct osprey7_v2l8p2_event_st api_event;
#endif

    EFUN(osprey7_v2l8p2_validate_txfir_cfg(sa__, enable_taps, pre3, pre2, pre1, main, post1, post2));

    ENULL_MEMSET(&txfir, 0, sizeof(osprey7_v2l8p2_txfir_st));
    if ((enable_taps == OSPREY7_V2L8P2_NRZ_6TAP) || (enable_taps == OSPREY7_V2L8P2_PAM4_6TAP)) {
#if !defined(SMALL_FOOTPRINT) 
        api_event.data1 = 6;
#endif
        txfir.tap[0] = pre3;
        txfir.tap[1] = pre2;
        txfir.tap[2] = pre1;
        txfir.tap[3] = main;
        txfir.tap[4] = post1;
        txfir.tap[5] = post2;
    }
    else {
#if !defined(SMALL_FOOTPRINT) 
        api_event.data1 = 3;
#endif
        txfir.tap[0] = pre1;
        txfir.tap[1] = main;
        txfir.tap[2] = post1;
    }
#if !defined(SMALL_FOOTPRINT) 
    api_event.event_id = 1;
    api_event.data2 = 0;
    EFUN(osprey7_v2l8p2_log_api_event(sa__, api_event));
#endif

    return osprey7_v2l8p2_INTERNAL_apply_full_txfir_cfg(sa__, enable_taps, &txfir);
}

/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/

/* Locks TX_PI to Loop timing */
err_code_t osprey7_v2l8p2_loop_timing(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE

    if (enable) {
        EFUN(wr_tx_pi_loop_timing_src_sel(0x1)); /* RX phase_sum_val_logic enable */
        EFUN(wr_tx_pi_en(0x1));                  /* TX_PI enable: 0 = diabled, 1 = enabled */
        EFUN(wr_tx_pi_jitter_filter_en(0x1));    /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */
        EFUN(USR_DELAY_US(25));               /* Wait for tclk to lock to CDR */
    }
    else {
        EFUN(wr_tx_pi_jitter_filter_en(0x0));    /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */
        EFUN(wr_tx_pi_en(0x0));                  /* TX_PI enable: 0 = diabled, 1 = enabled */
        EFUN(wr_tx_pi_loop_timing_src_sel(0x0)); /* RX phase_sum_val_logic enable */
    }
    return (ERR_CODE_NONE);
}

/* Setup Remote Loopback mode  */
err_code_t osprey7_v2l8p2_rmt_lpbk(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    if (enable) {
        EFUN(osprey7_v2l8p2_loop_timing(sa__, enable));
        EFUN(wr_tx_pi_ext_ctrl_en(0x1));  /* PD path enable: 0 = diabled, 1 = enabled */
        EFUN(wr_rmt_lpbk_en(0x1));        /* Remote Loopback Enable: 0 = diabled, 1 = enable  */
        EFUN(USR_DELAY_US(50));        /* Wait for rclk and tclk phase lock before expecing good data from rmt loopback */
    } else {                              /* Might require longer wait time for smaller values of tx_pi_ext_phase_bwsel_integ */
        EFUN(wr_rmt_lpbk_en(0x0));        /* Remote Loopback Enable: 0 = diabled, 1 = enable  */
        EFUN(wr_tx_pi_ext_ctrl_en(0x0));  /* PD path enable: 0 = diabled, 1 = enabled */
        EFUN(osprey7_v2l8p2_loop_timing(sa__, enable));
    }
    return (ERR_CODE_NONE);
}


/********************************/
/*  TX_PI Fixed Frequency Mode  */
/********************************/
#ifndef SMALL_FOOTPRINT
/* TX_PI Frequency Override (Fixed Frequency Mode) */
err_code_t osprey7_v2l8p2_tx_pi_freq_override(srds_access_t *sa__, uint8_t enable, int16_t freq_override_val) {
    INIT_SRDS_ERR_CODE
    if (enable) {
        EFUN(wr_tx_pi_en(0x1));                              /* TX_PI enable :            0 = disabled, 1 = enabled */
        EFUN(wr_tx_pi_freq_override_val((uint16_t)freq_override_val)); /* Fixed Freq Override Value (+/-8192) */
        EFUN(wr_tx_pi_freq_override_en(0x1));                /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
    }
    else {
        EFUN(wr_tx_pi_freq_override_en(0x0));                /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
        EFUN(wr_tx_pi_freq_override_val(0));                 /* Fixed Freq Override Value to 0 */
        EFUN(wr_tx_pi_en(0x0));                              /* TX_PI enable :            0 = disabled, 1 = enabled */
    }
  return (ERR_CODE_NONE);
}
#endif /* SMALL_FOOTPRINT */

/*********************************/
/*  uc_active and uc_reset APIs  */
/*********************************/
err_code_t osprey7_v2l8p2_get_micro_num_uc_cores(srds_access_t *sa__, uint8_t *num_micros) {
    INIT_SRDS_ERR_CODE
    UNUSED(sa__);
    ESTM(*num_micros = rdc_micro_num_uc_cores());
    return (ERR_CODE_NONE);
}

/* Toggle core reset */
err_code_t osprey7_v2l8p2_core_reset(srds_access_t *sa__, ucode_info_t ucode_info) {
    INIT_SRDS_ERR_CODE
    uint8_t i, num_lanes;
    uint8_t orig_pll = osprey7_v2l8p2_acc_get_pll_idx(sa__);
    uint8_t orig_lane = osprey7_v2l8p2_acc_get_lane(sa__);
    uint8_t ams_tx_version_id;
    ESTM(ams_tx_version_id = rd_ams_tx_version_id());
    EFUN(osprey7_v2l8p2_uc_reset(sa__, 1, ucode_info));   /* Assert uC reset */
    EFUN(wrc_micro_clk_s_comclk_sel(0x1));        /* Select com_clk as micro_clk */
    EFUN(wrc_mdio_brcst_port_addr(0x1f));         /* mdio_brcst_port_addr is supposed to be reset to default 0x1f, but can't in fact. So has to be set explicitly.*/
    ESTM(num_lanes = rdc_revid_multiplicity());
    for(i = 0; i < num_lanes; i++) {
        EFUN(osprey7_v2l8p2_acc_set_lane(sa__, i));
        EFUN(osprey7_v2l8p2_ln_dp_reset(sa__, 1));
    }
    EFUN(USR_DELAY_US(1));
    for (i = 0; i < NUM_PLLS; i++) {
        EFUN(osprey7_v2l8p2_acc_set_pll_idx(sa__, i));
        EFUN(osprey7_v2l8p2_core_dp_reset(sa__, 1));
    }
    EFUN(USR_DELAY_US(1));
    EFUN(wrc_core_s_rstb(0x0));                   /* Assert core reset */
    EFUN(wrc_core_s_rstb(0x1));                   /* Deassert core reset */
    if(ams_tx_version_id == 0xA0) {
        for(i=0;i<num_lanes;i++) {
            EFUN(osprey7_v2l8p2_acc_set_lane(sa__,i));
            EFUN(wr_ams_rx_ro_clkdata_enb(0x1));
            EFUN(wr_ams_rx_vdd_qual(0x1));
        }
    }
    EFUN(osprey7_v2l8p2_acc_set_lane(sa__, orig_lane));
    EFUN(osprey7_v2l8p2_acc_set_pll_idx(sa__, orig_pll));
    return (ERR_CODE_NONE);
}

/* Enable or Disable the uC reset; Dummy function to maintain compatibility with BHK16 APIs */
err_code_t osprey7_v2l8p2_uc_reset_with_info(srds_access_t *sa__, uint8_t enable, ucode_info_t ucode_info) {
    return osprey7_v2l8p2_uc_reset(sa__, enable, ucode_info);
}

/* Enable or Disable the uC reset */
err_code_t osprey7_v2l8p2_uc_reset(srds_access_t *sa__, uint8_t enable, ucode_info_t ucode_info) {
    INIT_SRDS_ERR_CODE
  if (enable) {
    EFUN(wrc_micro_micro_s_rstb(0x0));                    /* Toggle micro_reset to reset all micro registers to default value */
    EFUN(wrc_micro_micro_s_rstb(0x1));
    EFUN(wrc_micro_cr_crc_calc_en(0x0));                  /* Default value of crc calc en is 1. Set to 0 to stop crc calculation. */

    {
      uint16_t stack_size;
      if (ucode_info.stack_size != 0) {
         stack_size = ucode_info.stack_size;
         EFUN(wrc_micro_core_stack_size(stack_size));
         EFUN(wrc_micro_core_stack_en(1));
      } else {
          EFUN_PRINTF(("ERROR:  Stack Size in ucode_info is Zero.\n"));
          return(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
      }
    }
    if(ucode_info.ucode_dr_code_size_kb != 0) {
        EFUN(wrc_micro_dr_code_size(ucode_info.ucode_dr_code_size_kb));
    }
    EFUN(wrc_uc_active(0));  /* This register is only cleared by POR */
  }
  else {
    EFUN(wrc_micro_micro_s_rstb(0x1));                    /* De-assert micro reset */
    /* De-assert micro reset - Start executing code */
    EFUN(wrc_micro_master_clk_en (0x1));                  /* Enable clock to microcontroller subsystem */
    EFUN(wrc_micro_master_rstb   (0x1));                  /* De-assert reset to microcontroller sybsystem */
    EFUN(wrc_micro_cr_access_en  (0x1));
    EFUN(wrc_micro_ra_init(0x2));                         /* Write command for data RAM initialization */
    EFUN(osprey7_v2l8p2_INTERNAL_poll_micro_ra_initdone(sa__, 5));/* Poll status of data RAM initialization */
    EFUN(wrc_micro_ra_init(0x0));                         /* Clear command for data RAM initialization */
    {
        uint8_t micro_orig, num_micros = 0;
        int8_t micro_idx;
        ESTM(micro_orig = osprey7_v2l8p2_acc_get_micro_idx(sa__));
        EFUN(osprey7_v2l8p2_get_micro_num_uc_cores(sa__, &num_micros));
        for (micro_idx = (int8_t)(num_micros-1); micro_idx >= 0; micro_idx--) {
            EFUN(osprey7_v2l8p2_acc_set_micro_idx(sa__, (uint8_t)micro_idx));
            EFUN(wrc_micro_core_clk_en(0x1));          /* Enable clock to micro core */
            EFUN(wrc_micro_core_rstb(0x1));
        }
        EFUN(osprey7_v2l8p2_acc_set_micro_idx(sa__, micro_orig));
    }
    UNUSED(ucode_info);
  }
  return (ERR_CODE_NONE);
}

/* Wait for uC to become active */
err_code_t osprey7_v2l8p2_wait_uc_active(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    uint16_t loop;
    for (loop = 0; loop < 10000; loop++) {
        uint16_t rddata;
        uint8_t core_uc_active=0;
        ESTM(core_uc_active = rdc_uc_active());
        ESTM(rddata = (reg_rdc_MICRO_B_COM_RMI_MICRO_SDK_STATUS0() & 0xF));
        if ((rddata == 0xF) && (core_uc_active == 1)) {
            return (ERR_CODE_NONE);
        }
        if (loop>10) {
            EFUN(USR_DELAY_US(1));
        }
    }
    return (osprey7_v2l8p2_error(sa__, ERR_CODE_MICRO_INIT_NOT_DONE));
}

err_code_t osprey7_v2l8p2_init_osprey7_v2l8p2_info(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    srds_info_t * const osprey7_v2l8p2_info_ptr = osprey7_v2l8p2_INTERNAL_get_osprey7_v2l8p2_info_ptr(sa__); /* Never put a check here */
    uint32_t info_table[INFO_TABLE_END / sizeof(uint32_t)];
    uint32_t info_table_signature;
    uint8_t  info_table_version;
    uint8_t status_byte;
    err_code_t err_code = ERR_CODE_NONE;

    ENULL_MEMSET(info_table, 0, sizeof(info_table));

#ifndef NO_VARIADIC_MACROS
    USR_ACQUIRE_LOCK;
#endif

    if (osprey7_v2l8p2_info_ptr == NULL) {
        EFUN_PRINTF(("ERROR:  Info table pointer is null.\n"));
        RELEASE_LOCK_AND_RETURN (osprey7_v2l8p2_error(sa__, ERR_CODE_INFO_TABLE_ERROR));
    }
#ifdef PHYMOD_SUPPORT
    if (PHYMOD_ACC_F_PHYSIM_GET(sa__)) {
        osprey7_v2l8p2_info_ptr->signature = SRDS_INFO_SIGNATURE;
        osprey7_v2l8p2_info_ptr->diag_mem_ram_base = 0x200013A8;
        osprey7_v2l8p2_info_ptr->diag_mem_ram_size = 0x100;
        osprey7_v2l8p2_info_ptr->core_var_ram_base = 0x20000080;
        osprey7_v2l8p2_info_ptr->core_var_ram_size = 0x40;
        osprey7_v2l8p2_info_ptr->lane_var_ram_base = 0x200003B4;
        osprey7_v2l8p2_info_ptr->lane_var_ram_size = 0x600;
        osprey7_v2l8p2_info_ptr->trace_mem_ram_base = 0x20000100;
        osprey7_v2l8p2_info_ptr->trace_mem_ram_size = 0x200;
        osprey7_v2l8p2_info_ptr->micro_var_ram_base = 0x20000300;
        osprey7_v2l8p2_info_ptr->lane_count = 0x2;
        osprey7_v2l8p2_info_ptr->trace_memory_descending_writes = 0x1;
        osprey7_v2l8p2_info_ptr->micro_count = 0x4;
        osprey7_v2l8p2_info_ptr->micro_var_ram_size = 0x10;
        osprey7_v2l8p2_info_ptr->grp_ram_size = 0x27C8;
        osprey7_v2l8p2_info_ptr->ucode_version = 0x0;
    } else {
#endif /* PHYMOD_SUPPORT */
    if (osprey7_v2l8p2_info_ptr->signature == SRDS_INFO_SIGNATURE) {
        /*Already initalized and so check for microcode version and exit with proper status*/
        err_code = osprey7_v2l8p2_INTERNAL_match_ucode_from_info(sa__, osprey7_v2l8p2_info_ptr);

        if (err_code == ERR_CODE_NONE) {
            /* ucode version match */
            RELEASE_LOCK_AND_RETURN (ERR_CODE_NONE);
        } else {
            /* ucode version mismatch */
            RELEASE_LOCK_AND_RETURN (osprey7_v2l8p2_error(sa__, ERR_CODE_MICRO_INIT_NOT_DONE));
        }
    }
    /*Never initialized so far, continue with initialization */

    err_code = osprey7_v2l8p2_rdblk_uc_prog_ram(sa__, (uint8_t *)info_table, INFO_TABLE_RAM_BASE, INFO_TABLE_END);
    /* Release lock before returning from the call to avoid deadlock */
    if (ERR_CODE_NONE != err_code) {
       RELEASE_LOCK_AND_RETURN(osprey7_v2l8p2_error(sa__, err_code));
    }

    info_table_signature = info_table[INFO_TABLE_OFFS_SIGNATURE / sizeof(uint32_t)];
    info_table_version = (uint8_t)(info_table_signature >> 24);
    if (((info_table_signature & 0x00FFFFFF) != 0x464E49)
        || (!(   ((info_table_version >= 0x32) && (info_table_version <= 0x39))
              || ((info_table_version >= 0x41) && (info_table_version <= 0x5A))))) {
        RELEASE_LOCK_AND_RETURN (osprey7_v2l8p2_error(sa__, ERR_CODE_INFO_TABLE_ERROR));
    }

    osprey7_v2l8p2_info_ptr->lane_count = info_table[INFO_TABLE_OFFS_OTHER_SIZE / sizeof(uint32_t)] & 0xFF;
    osprey7_v2l8p2_info_ptr->trace_memory_descending_writes = ((info_table[INFO_TABLE_OFFS_OTHER_SIZE / sizeof(uint32_t)] & 0x1000000) != 0);

    osprey7_v2l8p2_info_ptr->core_var_ram_size = (info_table[INFO_TABLE_OFFS_OTHER_SIZE / sizeof(uint32_t)] >> 8) & 0xFF;

    osprey7_v2l8p2_info_ptr->lane_var_ram_size = (info_table[INFO_TABLE_OFFS_TRACE_LANE_MEM_SIZE / sizeof(uint32_t)] >> 16) & 0xFFFF;

    osprey7_v2l8p2_info_ptr->diag_mem_ram_size = (info_table[INFO_TABLE_OFFS_DIAG_LANE_MEM_SIZE / sizeof(uint32_t)] & 0xFFFF);
    osprey7_v2l8p2_info_ptr->diag_mem_ram_base = info_table[INFO_TABLE_OFFS_DIAG_MEM_PTR / sizeof(uint32_t)];

    osprey7_v2l8p2_info_ptr->trace_mem_ram_size = (info_table[INFO_TABLE_OFFS_TRACE_LANE_MEM_SIZE / sizeof(uint32_t)] & 0xFFFF);
    osprey7_v2l8p2_info_ptr->trace_mem_ram_base = info_table[INFO_TABLE_OFFS_TRACE_MEM_PTR / sizeof(uint32_t)];

    osprey7_v2l8p2_info_ptr->core_var_ram_base = info_table[INFO_TABLE_OFFS_CORE_MEM_PTR / sizeof(uint32_t)];

    osprey7_v2l8p2_info_ptr->micro_var_ram_base = info_table[INFO_TABLE_OFFS_MICRO_MEM_PTR / sizeof(uint32_t)];
    osprey7_v2l8p2_info_ptr->micro_var_ram_size = (info_table[INFO_TABLE_OFFS_OTHER_SIZE_2 / sizeof(uint32_t)] >> 4) & 0xFF;

    osprey7_v2l8p2_info_ptr->lane_var_ram_base = info_table[INFO_TABLE_OFFS_LANE_MEM_PTR / sizeof(uint32_t)];
    if (info_table_version >= 0x41){
        osprey7_v2l8p2_info_ptr->lane_static_var_ram_base = info_table[INFO_TABLE_OFFS_LANE_STATIC_MEM_PTR  / sizeof(uint32_t)];
        osprey7_v2l8p2_info_ptr->lane_static_var_ram_size = info_table[INFO_TABLE_OFFS_LANE_STATIC_MEM_SIZE / sizeof(uint32_t)];
    }


    osprey7_v2l8p2_info_ptr->micro_count = info_table[INFO_TABLE_OFFS_OTHER_SIZE_2 / sizeof(uint32_t)] & 0xF;
    osprey7_v2l8p2_info_ptr->grp_ram_size = info_table[INFO_TABLE_OFFS_GROUP_RAM_SIZE / sizeof(uint32_t)] & 0xFFFF;
    osprey7_v2l8p2_info_ptr->ucode_version = info_table[INFO_TABLE_OFFS_UC_VERSION /sizeof(uint32_t)];
    osprey7_v2l8p2_info_ptr->signature = SRDS_INFO_SIGNATURE;
    osprey7_v2l8p2_info_ptr->info_table_version = info_table_version;
    {
        uint8_t afe_version;
        uint8_t revid2;
        ESTM(revid2 = rdc_revid2());
        ESTM(afe_version = rd_ams_tx_version_id());
        if((afe_version == OSPREY7_V2L8P2_AFE_IPVERSION_ID_DEFAULT_C1) && (revid2 == OSPREY7_V2L8P2_REVID2_DEFAULT_C1_2)) {
            osprey7_v2l8p2_info_ptr->silicon_version = OSPREY7_V2L8P2_UAPI_C1_AFE_WITH_REVID2_7;
        }
        else {
            osprey7_v2l8p2_info_ptr->silicon_version = afe_version;
        }
    }
#ifdef PHYMOD_SUPPORT
    }
#endif
    /* Check to make sure that ucode IP matches core IP */
    ESTM(status_byte = rdcv_status_byte());
    if (status_byte & 0x20) {
       EFUN_PRINTF(("Microcode IP did not match core! (status_byte = 0x%x)\n", status_byte));
       RELEASE_LOCK_AND_RETURN (osprey7_v2l8p2_error(sa__, ERR_CODE_UCODE_IP_DOES_NOT_MATCH_CORE));
    }

    RELEASE_LOCK_AND_RETURN (ERR_CODE_NONE);
}

#ifndef SMALL_FOOTPRINT
/*-----------------*/
/*  Configure PLL  */
/*-----------------*/

err_code_t osprey7_v2l8p2_configure_pll_powerdown(srds_access_t *sa__) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, OSPREY7_V2L8P2_PLL_REFCLK_UNKNOWN, OSPREY7_V2L8P2_PLL_DIV_UNKNOWN, 0, OSPREY7_V2L8P2_PLL_OPTION_POWERDOWN);
}

err_code_t osprey7_v2l8p2_configure_pll_refclk_div(srds_access_t *sa__,
                                           enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                           enum osprey7_v2l8p2_pll_div_enum srds_div) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, refclk, srds_div, 0, OSPREY7_V2L8P2_PLL_OPTION_NONE);
}

err_code_t osprey7_v2l8p2_configure_pll_refclk_vco(srds_access_t *sa__,
                                           enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, refclk, OSPREY7_V2L8P2_PLL_DIV_UNKNOWN, vco_freq_khz, OSPREY7_V2L8P2_PLL_OPTION_NONE);
}

err_code_t osprey7_v2l8p2_configure_pll_div_vco(srds_access_t *sa__,
                                        enum osprey7_v2l8p2_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, OSPREY7_V2L8P2_PLL_REFCLK_UNKNOWN, srds_div, vco_freq_khz, OSPREY7_V2L8P2_PLL_OPTION_NONE);
}

/* following routines will set the VCO rate to 53.125 GHz */
err_code_t osprey7_v2l8p2_configure_53P125G_pll_refclk(srds_access_t *sa__,
                                        enum osprey7_v2l8p2_pll_refclk_enum refclk) {
    return osprey7_v2l8p2_INTERNAL_configure_53P125_pll(sa__, refclk, OSPREY7_V2L8P2_PLL_DIV_UNKNOWN, OSPREY7_V2L8P2_PLL_OPTION_NONE);
}

err_code_t osprey7_v2l8p2_configure_53P125G_pll_div(srds_access_t *sa__,
                                        enum osprey7_v2l8p2_pll_div_enum srds_div) {
    return osprey7_v2l8p2_INTERNAL_configure_53P125_pll(sa__, OSPREY7_V2L8P2_PLL_REFCLK_UNKNOWN, srds_div, OSPREY7_V2L8P2_PLL_OPTION_NONE);
}

err_code_t osprey7_v2l8p2_configure_53P125G_lb_pll_refclk(srds_access_t *sa__,
                                        enum osprey7_v2l8p2_pll_refclk_enum refclk) {
    return osprey7_v2l8p2_INTERNAL_configure_lb_53P125_pll(sa__, refclk, OSPREY7_V2L8P2_PLL_DIV_UNKNOWN, OSPREY7_V2L8P2_PLL_OPTION_NONE);
}

err_code_t osprey7_v2l8p2_configure_53P125G_lb_pll_div(srds_access_t *sa__,
                                        enum osprey7_v2l8p2_pll_div_enum srds_div) {
    return osprey7_v2l8p2_INTERNAL_configure_lb_53P125_pll(sa__, OSPREY7_V2L8P2_PLL_REFCLK_UNKNOWN, srds_div, OSPREY7_V2L8P2_PLL_OPTION_NONE);
}

err_code_t osprey7_v2l8p2_pll_lb_range_check(srds_access_t *sa__, uint8_t *pll_out_range) {
    if(pll_out_range == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }
    return osprey7_v2l8p2_INTERNAL_pll_lb_range_check(sa__, pll_out_range);
}

err_code_t osprey7_v2l8p2_pll_hb_range_check(srds_access_t *sa__, uint8_t *pll_out_range) {
    if(pll_out_range == NULL) {
        return ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }
    return osprey7_v2l8p2_INTERNAL_pll_hb_range_check(sa__, pll_out_range);
}

/* following routines divide refclk input by 2 to prevent fracn mode */
err_code_t osprey7_v2l8p2_configure_pll_refclk_div_div2refclk(srds_access_t *sa__,
                                           enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                           enum osprey7_v2l8p2_pll_div_enum srds_div) {
   return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, refclk, srds_div, 0, OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DIV2_EN);
}

err_code_t osprey7_v2l8p2_configure_pll_refclk_vco_div2refclk(srds_access_t *sa__,
                                           enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, refclk, OSPREY7_V2L8P2_PLL_DIV_UNKNOWN, vco_freq_khz, OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DIV2_EN);
}

err_code_t osprey7_v2l8p2_configure_pll_div_vco_div2refclk(srds_access_t *sa__,
                                        enum osprey7_v2l8p2_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, OSPREY7_V2L8P2_PLL_REFCLK_UNKNOWN, srds_div, vco_freq_khz, OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DIV2_EN);
}
/* following routines divide refclk input by 2 to prevent fracn mode */
err_code_t osprey7_v2l8p2_configure_pll_refclk_div_div4refclk(srds_access_t *sa__,
                                           enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                           enum osprey7_v2l8p2_pll_div_enum srds_div) {
   return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, refclk, srds_div, 0, OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DIV4_EN);
}

err_code_t osprey7_v2l8p2_configure_pll_refclk_vco_div4refclk(srds_access_t *sa__,
                                           enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, refclk, OSPREY7_V2L8P2_PLL_DIV_UNKNOWN, vco_freq_khz, OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DIV4_EN);
}

err_code_t osprey7_v2l8p2_configure_pll_div_vco_div4refclk(srds_access_t *sa__,
                                        enum osprey7_v2l8p2_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, OSPREY7_V2L8P2_PLL_REFCLK_UNKNOWN, srds_div, vco_freq_khz, OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DIV4_EN);
}

err_code_t osprey7_v2l8p2_configure_pll_refclk_div_refclk_2x(srds_access_t *sa__,
                                           enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                           enum osprey7_v2l8p2_pll_div_enum srds_div) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, refclk, srds_div, 0, OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DOUBLER_EN);
}

err_code_t osprey7_v2l8p2_configure_pll_refclk_vco_refclk_2x(srds_access_t *sa__,
                                           enum osprey7_v2l8p2_pll_refclk_enum refclk,
                                           uint32_t vco_freq_khz) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, refclk, OSPREY7_V2L8P2_PLL_DIV_UNKNOWN, vco_freq_khz, OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DOUBLER_EN);
}

err_code_t osprey7_v2l8p2_configure_pll_div_vco_refclk_2x(srds_access_t *sa__,
                                        enum osprey7_v2l8p2_pll_div_enum srds_div,
                                        uint32_t vco_freq_khz) {
    return osprey7_v2l8p2_INTERNAL_configure_pll(sa__, OSPREY7_V2L8P2_PLL_REFCLK_UNKNOWN, srds_div, vco_freq_khz, OSPREY7_V2L8P2_PLL_OPTION_REFCLK_DOUBLER_EN);
}

err_code_t osprey7_v2l8p2_get_vco_from_refclk_div(srds_access_t *sa__, uint32_t refclk_freq_hz, enum osprey7_v2l8p2_pll_div_enum srds_div, uint32_t *vco_freq_khz, enum osprey7_v2l8p2_pll_option_enum pll_option) {
    INIT_SRDS_ERR_CODE
    EFUN(osprey7_v2l8p2_INTERNAL_get_vco_from_refclk_div(sa__, refclk_freq_hz, srds_div, vco_freq_khz, pll_option));
    return (ERR_CODE_NONE);
}

/***********************************************/
/*  Microcode Load into Program RAM Functions  */
/***********************************************/
err_code_t osprey7_v2l8p2_ucode_load_init(srds_access_t *sa__, uint8_t pram) {
    INIT_SRDS_ERR_CODE
    IF_API_ID_CODES_DONT_MATCH_CORE_ID_CODES
       return (ERR_CODE_API_IP_DOES_NOT_MATCH_CORE);
    }
    EFUN(wrc_micro_master_clk_en(0x1));                     /* Enable clock to microcontroller subsystem */
    EFUN(wrc_micro_master_rstb(0x1));                       /* De-assert reset to microcontroller sybsystem */
    EFUN(wrc_micro_master_rstb(0x0));                       /* Assert reset to microcontroller sybsystem - Toggling reset*/
    EFUN(wrc_micro_master_rstb(0x1));                       /* De-assert reset to microcontroller sybsystem */
    EFUN(wrc_micro_cr_access_en(1));                        /* Allow access to Code RAM */
    EFUN(wrc_micro_ra_init(0x1));                           /* Set initialization command to initialize code RAM */
    EFUN(osprey7_v2l8p2_INTERNAL_poll_micro_ra_initdone(sa__, 5));  /* Poll for micro_ra_initdone = 1 to indicate initialization done */
    EFUN(wrc_micro_ra_init(0x0));

    EFUN(wrc_micro_cr_crc_prtsel(0));
    if(pram) {
        EFUN(wrc_micro_cr_prif_prtsel(0));
    }
    EFUN(wrc_micro_cr_crc_init(1));                         /* Initialize the HW CRC calculation */
    EFUN(wrc_micro_cr_crc_init(0));
    EFUN(wrc_micro_cr_crc_calc_en(1));
    EFUN(wrc_micro_cr_ignore_micro_code_writes(0));         /* Allow writing to program RAM */
    if(pram) {
        EFUN(wrc_micro_pramif_ahb_wraddr_msw(0x0000));
        EFUN(wrc_micro_pramif_ahb_wraddr_lsw(0x0000));
        EFUN(wrc_micro_pram_if_rstb(1));
        EFUN(wrc_micro_pramif_en(1));
    }
    else {
        /* Code to Load microcode */
        EFUN(wrc_micro_autoinc_wraddr_en(0x1));             /* To auto increment RAM write address */
        EFUN(wrc_micro_ra_wrdatasize(0x1));                 /* Select 16bit transfers */
        EFUN(wrc_micro_ra_wraddr_msw(0x0));                 /* Upper 16bits of start address of Program RAM where the ucode is to be loaded */
        EFUN(wrc_micro_ra_wraddr_lsw(0x0));                 /* Lower 16bits of start address of Program RAM where the ucode is to be loaded */
    }
    return (ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_ucode_load_write(srds_access_t *sa__, uint8_t *ucode_image, uint32_t ucode_len) {
    INIT_SRDS_ERR_CODE
    uint32_t   ucode_len_padded, count = 0;
    uint16_t   wrdata_lsw;
    uint8_t    wrdata_lsb;

    if(!ucode_image) {
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    ucode_len_padded = ((ucode_len + 3) & 0xFFFFFFFC);    /* Aligning ucode size to 4-byte boundary */

    do {                                                  /* ucode_image loaded 16bits at a time */
        wrdata_lsb = (count < ucode_len) ? ucode_image[count] : 0x0; /* wrdata_lsb read from ucode_image; zero padded to 4byte boundary */
        count++;
        wrdata_lsw = (count < ucode_len) ? ucode_image[count] : 0x0; /* wrdata_msb read from ucode_image; zero padded to 4byte boundary */
        count++;
        wrdata_lsw = (uint16_t)((wrdata_lsw << 8) | wrdata_lsb);               /* 16bit wrdata_lsw formed from 8bit msb and lsb values read from ucode_image */
        EFUN(wrc_micro_ra_wrdata_lsw(wrdata_lsw));                   /* Program RAM lower 16bits write data */
    }   while (count < ucode_len_padded);                 /* Loop repeated till entire image loaded (upto the 4byte boundary) */

    return (ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_ucode_load_close(srds_access_t *sa__, uint8_t pram) {
    INIT_SRDS_ERR_CODE

    if(!pram) {
        EFUN(wrc_micro_ra_wrdatasize(0x2));               /* Select 32bit transfers as default */
    }
    EFUN(wrc_micro_cr_ignore_micro_code_writes(1));       /* block writing to program RAM */
    EFUN(wrc_micro_cr_crc_calc_en(0));

    {
        uint8_t micro_orig, num_micros = 0, micro_idx;
        ESTM(micro_orig = osprey7_v2l8p2_acc_get_micro_idx(sa__));
        EFUN(osprey7_v2l8p2_get_micro_num_uc_cores(sa__, &num_micros));
        for (micro_idx = 0; micro_idx < num_micros; micro_idx++) {
            EFUN(osprey7_v2l8p2_acc_set_micro_idx(sa__, micro_idx));
            EFUN(wrc_micro_core_clk_en(0x1));             /* Enable clock to micro core */
        }
        EFUN(osprey7_v2l8p2_acc_set_micro_idx(sa__, micro_orig));
    }

    return (ERR_CODE_NONE);                               /* NO Errors while loading microcode (uCode Load PASS) */

}

/* uCode Load through Register (MDIO) Interface [Return Val = Error_Code (0 = PASS)] */
err_code_t osprey7_v2l8p2_ucode_load(srds_access_t *sa__, uint8_t *ucode_image, uint32_t ucode_len) {
    INIT_SRDS_ERR_CODE

    EFUN(osprey7_v2l8p2_ucode_load_init(sa__, 0));
    EFUN(osprey7_v2l8p2_ucode_load_write(sa__, ucode_image, ucode_len));
    EFUN(osprey7_v2l8p2_ucode_load_close(sa__, 0));

    return (ERR_CODE_NONE);
}


/* Read-back uCode from Program RAM and verify against ucode_image [Return Val = Error_Code (0 = PASS)]  */
err_code_t osprey7_v2l8p2_ucode_load_verify(srds_access_t *sa__, uint8_t *ucode_image, uint32_t ucode_len) {
    INIT_SRDS_ERR_CODE

    uint32_t ucode_len_padded, count = 0;
    uint16_t rddata_lsw;
    uint16_t data_lsw;
    uint8_t  rddata_lsb;

    if(!ucode_image) {
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    ucode_len_padded = ((ucode_len + 3) & 0xFFFFFFFC);    /* Aligning ucode size to 4-byte boundary */

    EFUN(wrc_micro_autoinc_rdaddr_en(0x1));               /* To auto increment RAM read address */
    EFUN(wrc_micro_ra_rddatasize(0x1));                   /* Select 16bit transfers */
    EFUN(wrc_micro_ra_rdaddr_msw(0x0));                   /* Upper 16bits of start address of Program RAM from where to read ucode */
    EFUN(wrc_micro_ra_rdaddr_lsw(0x0));                   /* Lower 16bits of start address of Program RAM from where to read ucode */

    do {                                                  /* ucode_image read 16bits at a time */
        rddata_lsb = (count < ucode_len) ? ucode_image[count] : 0x0; /* rddata_lsb read from ucode_image; zero padded to 4byte boundary */
        count++;
        rddata_lsw = (count < ucode_len) ? ucode_image[count] : 0x0; /* rddata_msb read from ucode_image; zero padded to 4byte boundary */
        count++;
        rddata_lsw = (uint16_t)((rddata_lsw << 8) | rddata_lsb);               /* 16bit rddata_lsw formed from 8bit msb and lsb values read from ucode_image */
                                                                     /* Compare Program RAM ucode to ucode_image (Read to micro_ra_rddata_lsw reg auto-increments the ram_address) */
        ESTM(data_lsw = rdc_micro_ra_rddata_lsw());
        if (data_lsw != rddata_lsw) {
            EFUN_PRINTF(("Ucode_Load_Verify_FAIL: Addr = 0x%x: Read_data = 0x%x :  Expected_data = 0x%x \n",(count-2),data_lsw,rddata_lsw));
            return (osprey7_v2l8p2_error(sa__, ERR_CODE_UCODE_VERIFY_FAIL));             /* Verify uCode FAIL */
        }

    } while (count < ucode_len_padded);                   /* Loop repeated till entire image loaded (upto the 4byte boundary) */

    EFUN(wrc_micro_ra_rddatasize(0x2));                   /* Select 32bit transfers ad default */
    return (ERR_CODE_NONE);                               /* Verify uCode PASS */
}

/******************************/
/*  API to Align TX clocks    */
/******************************/
err_code_t osprey7_v2l8p2_tx_clock_align_enable(srds_access_t *sa__, uint8_t enable, uint8_t lane_mask) {
    INIT_SRDS_ERR_CODE
    uint8_t num_lanes, current_lane, lane;

    /* read the number of lanes from register */
    ESTM(num_lanes = rdc_revid_multiplicity());

    ESTM(current_lane = osprey7_v2l8p2_acc_get_lane(sa__));

    /* Check if current lane is part of the mask, else return ERROR */
    if (!((lane_mask >> current_lane) & 0x1))
        return(osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));

    /* Start out by disabling all the lanes selected in the mask */
    for(lane = 0;lane<num_lanes;lane++) {
        if ((lane_mask >> lane) & 0x1) {
            EFUN(osprey7_v2l8p2_acc_set_lane(sa__,lane));

           EFUN(wr_ams_tx_sel_txmaster(0));         /* Disable any master lanes */
           /* Initial TX lane clock phase alignment. Program following registers only for slave lanes, master lane registers should not be programmed (can be kept in default values). */
           EFUN(wr_tx_pi_pd_bypass_vco(0));         /* for quick phase lock time */
           EFUN(wr_tx_pi_pd_bypass_flt(0));         /* for quick phase lock time */

           EFUN(wr_tx_pi_hs_fifo_phserr_sel(0));    /* selects PD from afe as input */
           EFUN(wr_tx_pi_ext_pd_sel(0));            /* selects PD path based on tx_pi_repeater_mode_en */
           EFUN(wr_tx_pi_en(0));                    /* Enable TX_PI    */
           EFUN(wr_tx_pi_jitter_filter_en(0));      /* turn off frequency lock for tx_pi as all te tclk clocks are derived from VCO and same frequency */
           EFUN(wr_tx_pi_ext_ctrl_en(0));           /* turn on PD path to TX_PI to lock the clocks */
           EFUN(wr_tx_pi_ext_phase_bwsel_integ(0)); /* 0 to 7: higher value means faster lock time */
           EFUN(wr_tx_pi_pd_bypass_vco(0));         /* disable filter bypass for more accurate lock */
        }
    }

    if (enable)
    {
        /* Enable master lane first so that when slave lanes are enabled, they start aligning to the master */
        EFUN(osprey7_v2l8p2_acc_set_lane(sa__,current_lane)); /* return lane to previous state */
        EFUN(wr_ams_tx_sel_txmaster(1));          /* Select one lane as txmaster. For all other lanes, this should be 0. */

        for(lane = 0;lane<num_lanes;lane++) {
            if ((lane_mask >> lane) & 0x1) {
                if(lane == current_lane) {                   /* Current Lane should be the master lane */
                /* Do nothing */
                } else {
                    EFUN(osprey7_v2l8p2_acc_set_lane(sa__,lane));
                    /* Initial TX lane clock phase alignment. Program following registers only for slave lanes, master lane registers should not be programmed (can be kept in default values). */
                    EFUN(wr_tx_pi_pd_bypass_vco(1));         /* for quick phase lock time */
                    EFUN(wr_tx_pi_pd_bypass_flt(1));         /* for quick phase lock time */
                    EFUN(wr_tx_pi_hs_fifo_phserr_sel(1));    /* selects PD from afe as input */
#ifdef wr_tx_pi_ext_pd_sel
                    EFUN(wr_tx_pi_ext_pd_sel(0));            /*  selects PD path based on tx_pi_repeater_mode_en */
#endif
                    EFUN(wr_tx_pi_en(1));                    /* Enable TX_PI    */
                    EFUN(wr_tx_pi_jitter_filter_en(0));      /* turn off frequency lock for tx_pi as all te tclk clocks are derived from VCO and same frequency */
                    EFUN(wr_tx_pi_ext_ctrl_en(1));           /* turn on PD path to TX_PI to lock the clocks */
                    EFUN(USR_DELAY_US(25));                  /* wait for the slave lane clocks to lock to the master lane clock */
                    EFUN(wr_tx_pi_ext_phase_bwsel_integ(7)); /* 0 to 7: higher value means faster lock time */
                    EFUN(wr_tx_pi_pd_bypass_vco(0));         /* disable filter bypass for more accurate lock */
                    EFUN(wr_tx_pi_pd_bypass_flt(0));         /* disable filter bypass for more accurate lock */
                }
            }
        }

        EFUN(USR_DELAY_US(60));                          /* wait for the slave lane clocks to lock to the master lane clock withing 1/16th of UI. */

        for(lane = 0;lane<num_lanes;lane++) {
            if ((lane_mask >> lane) & 0x1) {
                EFUN(osprey7_v2l8p2_acc_set_lane(sa__,lane));
                EFUN(wr_tx_pi_en(0));                    /* Freeze TX_PI for all enabled lanes after slave lanes have locked   */
            }
        }
    }
    EFUN(osprey7_v2l8p2_acc_set_lane(sa__,current_lane)); /* return lane to previous state */
    EFUN(wr_ams_tx_sel_txmaster(0));          /* Disable any master lanes and move on to next TCA group */
    return(ERR_CODE_NONE);
 }
#endif /* SMALL_FOOTPRINT */


/************************/
/*  Fast Link Recovery  */
/************************/
err_code_t osprey7_v2l8p2_configure_fast_link_recovery(srds_access_t *sa__, uint8_t enable, uint8_t enable_timeout, enum osprey7_v2l8p2_flr_timeout_enum timeout_time) {
    INIT_SRDS_ERR_CODE
    uint8_t const timeout_time_val = (uint8_t)timeout_time;
    uint8_t flr_support = 0;

    uint8_t reset_state;
    ESTM(reset_state = rd_rx_lane_dp_reset_state());
    if(reset_state < 7) {
        EFUN_PRINTF(("ERROR: osprey7_v2l8p2_configure_fast_link_recovery(..) called without ln_dp_s_rstb=0 Lane=%d reset_state=%d\n",osprey7_v2l8p2_acc_get_lane(sa__),reset_state));
        return (osprey7_v2l8p2_error(sa__, ERR_CODE_LANE_DP_NOT_RESET));
    }
    EFUN(osprey7_v2l8p2_INTERNAL_is_flr_supported(sa__, &flr_support));
    if(!flr_support) {
        EFUN_PRINTF(("ERROR: osprey7_v2l8p2_configure_fast_link_recovery(..) unsupported with currently loaded microcode (Core=%d Lane=%d).\n", osprey7_v2l8p2_acc_get_core(sa__), osprey7_v2l8p2_acc_get_lane(sa__)));
        return (osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if (timeout_time_val > 5) {
        ESTM_PRINTF(("timeout_time value for osprey7_v2l8p2_configure_fast_link_recovery() is too large:  %d\n", timeout_time_val));
        return (osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    {
        uint8_t disable_ss_dfe;
        uint16_t disable_ss;
        ESTM(disable_ss_dfe = rdv_usr_ctrl_disable_steady_state_dfe_functions_byte());
        ESTM(disable_ss = rdv_usr_ctrl_disable_steady_state_functions_word());
        if (enable) {
            EFUN(wrv_usr_ctrl_disable_steady_state_dfe_functions_byte(0x01 | disable_ss_dfe));
            EFUN(wrv_usr_ctrl_disable_steady_state_functions_word(0x0020 | disable_ss));
        }
        else {
            EFUN(wrv_usr_ctrl_disable_steady_state_dfe_functions_byte((uint8_t)(~0x01 & disable_ss_dfe)));
            EFUN(wrv_usr_ctrl_disable_steady_state_functions_word((uint16_t)(~0x0020 & disable_ss)));
        }
    }
    EFUN(wrv_usr_ctrl_fast_link_recovery_byte((uint8_t)((enable ? 1 : 0)
                                              | (enable_timeout ? 2 : 0)
                                              | (timeout_time_val << 2))));

    return (ERR_CODE_NONE);
}

err_code_t osprey7_v2l8p2_get_configure_fast_link_recovery(srds_access_t *sa__, uint8_t *enable, uint8_t *enable_timeout, enum osprey7_v2l8p2_flr_timeout_enum *timeout_time) {
    INIT_SRDS_ERR_CODE
    uint8_t flr_byte = 0;
    uint8_t flr_support = 0;

    EFUN(osprey7_v2l8p2_INTERNAL_is_flr_supported(sa__, &flr_support));
    if(!flr_support) {
        EFUN_PRINTF(("ERROR: osprey7_v2l8p2_get_configure_fast_link_recovery(..) unsupported with currently loaded microcode (Core=%d Lane=%d).\n", osprey7_v2l8p2_acc_get_core(sa__), osprey7_v2l8p2_acc_get_lane(sa__)));
        return (osprey7_v2l8p2_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    if((enable == NULL) || (enable_timeout == NULL) ||( timeout_time == NULL)) {
        return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }

    ESTM(flr_byte = rdv_usr_ctrl_fast_link_recovery_byte());
    *enable = flr_byte & 0x1;
    *enable_timeout = (flr_byte >> 1) & 0x1;
    *timeout_time = (enum osprey7_v2l8p2_flr_timeout_enum)((flr_byte >> 2) & 0x7);
    return (ERR_CODE_NONE);
}
