/**************************************************************************************
 **************************************************************************************
 *  File Name     :  falcon2_dino_dv_functions.c                                        *
 *  Created On    :  22/05/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  APIs for Serdes IPs ported over from DV                          *
 *  Revision      :  $Id: falcon2_dino_dv_functions.c 1356 2016-02-12 23:27:16Z cvazquez $ *
 *                                                                                    *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                              *
 *  No portions of this material may be reproduced in any form without                *
 *  the written permission of:                                                        *
 *      Broadcom Corporation                                                          *
 *      5300 California Avenue                                                        *
 *      Irvine, CA  92617                                                             *
 *                                                                                    *
 *  All information contained in this document is Broadcom Corporation                *
 *  company private proprietary, and trade secret.                                    *
 */

/** @file falcon2_dino_dv_functions.c
 * Implementation of API functions ported over from DV
 */

/******************************/
/*  TX Pattern Generator APIs */
/******************************/

/* Cofigure shared TX Pattern (Return Val = 0:PASS, 1-6:FAIL (reports 6 possible error scenarios if failed)) */
err_code_t falcon2_dino_config_shared_tx_pattern( const phymod_access_t *pa, uint8_t patt_length, const char pattern[]) {

  char       patt_final[245] = "";
  char       patt_mod[245]   = "", bin[5] = "";
  uint8_t    str_len, i, k, j = 0;
  uint8_t    offset_len, actual_patt_len = 0, hex = 0;
  uint8_t    zero_pad_len    = 0; /* suppress warnings, changed by _calc_patt_gen_mode_sel( pa, ) */
  uint16_t   patt_gen_wr_val = 0;
  uint8_t    mode_sel        = 0; /* suppress warnings, changed by _calc_patt_gen_mode_sel( pa, ) */

  FALCON2_DINO_EFUN(falcon2_dino_calc_patt_gen_mode_sel( pa, &mode_sel,&zero_pad_len,patt_length));

  /* Generating the appropriate write value to patt_gen_seq registers */
  str_len = (int8_t)USR_STRLEN(pattern);

  if ((str_len > 2) && ((USR_STRNCMP(pattern, "0x", 2)) == 0)) {
    /* Hexadecimal Pattern */
    for (i=2; i < str_len; i++) {
      FALCON2_DINO_EFUN(falcon2_dino_compute_bin( pa, pattern[i],bin));
      FALCON2_DINO_ENULL_STRCAT(patt_mod,bin);
      if (pattern[i] != '_') {
        actual_patt_len = actual_patt_len + 4;
        if (actual_patt_len > 240) {
          FALCON2_DINO_EFUN_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
          return (falcon2_dino_error(ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN));
        }
      }
    }

    offset_len = (actual_patt_len - patt_length);
    if ((offset_len > 3)  || (actual_patt_len < patt_length)) {
      FALCON2_DINO_EFUN_PRINTF(("ERROR: Pattern length provided does not match the hexadecimal pattern provided\n"));
      return (falcon2_dino_error(ERR_CODE_CFG_PATT_LEN_MISMATCH));
    }
    else if (offset_len) {
      for (i=0; i < offset_len; i++) {
        if (patt_mod[i] != '0') {
          FALCON2_DINO_EFUN_PRINTF(("ERROR: Pattern length provided does not match the hexadecimal pattern provided\n"));
          return (falcon2_dino_error(ERR_CODE_CFG_PATT_LEN_MISMATCH));
        }
      }
      for (i=offset_len; i <= actual_patt_len; i++) {
        patt_mod[i - offset_len] = patt_mod[i];
      }
    }
  }

  else {
    /* Binary Pattern */
    for (i=0; i < str_len; i++) {
      if ((pattern[i] == '0') || (pattern[i] == '1')) {
        bin[0] = pattern[i];
        bin[1] = '\0';
        FALCON2_DINO_ENULL_STRCAT(patt_mod,bin);
        actual_patt_len++;
        if (actual_patt_len > 240) {
          FALCON2_DINO_EFUN_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
          return (falcon2_dino_error(ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN));
        }
      }
      else if (pattern[i] != '_') {
        FALCON2_DINO_EFUN_PRINTF(("ERROR: Invalid input Pattern\n"));
        return (falcon2_dino_error(ERR_CODE_CFG_PATT_INVALID_PATTERN));
      }
    }

    if (actual_patt_len != patt_length) {
      FALCON2_DINO_EFUN_PRINTF(("ERROR: Pattern length provided does not match the binary pattern provided\n"));
      return (falcon2_dino_error(ERR_CODE_CFG_PATT_LEN_MISMATCH));
    }
  }

  /* Zero padding upper bits and concatinating patt_mod to form patt_final */
  for (i=0; i < zero_pad_len; i++) {
    FALCON2_DINO_ENULL_STRCAT(patt_final,"0");
    j++;
  }
  for (i=zero_pad_len; i + patt_length < 241; i = i + patt_length) {
    FALCON2_DINO_ENULL_STRCAT(patt_final,patt_mod);
    j++;
  }

  /* FALCON2_DINO_EFUN_PRINTF(("\nFinal Pattern = %s\n\n",patt_final));    */

  for (i=0; i < 15; i++) {

    for (j=0; j < 4; j++) {
      k = i*16 + j*4;
      bin[0] = patt_final[k];
      bin[1] = patt_final[k+1];
      bin[2] = patt_final[k+2];
      bin[3] = patt_final[k+3];
      bin[4] = '\0';
      FALCON2_DINO_EFUN(falcon2_dino_compute_hex( pa, bin, &hex));
      patt_gen_wr_val = ((patt_gen_wr_val << 4) | hex);
    }
    /* FALCON2_DINO_EFUN_PRINTF(("patt_gen_wr_val[%d] = 0x%x\n",(14-i),patt_gen_wr_val));    */

    /* Writing to apprpriate patt_gen_seq Registers */
    switch (i) {
      case 0:  FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_14(patt_gen_wr_val));
               break;
      case 1:  FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_13(patt_gen_wr_val));
               break;
      case 2:  FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_12(patt_gen_wr_val));
               break;
      case 3:  FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_11(patt_gen_wr_val));
               break;
      case 4:  FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_10(patt_gen_wr_val));
               break;
      case 5:  FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_9(patt_gen_wr_val));
               break;
      case 6:  FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_8(patt_gen_wr_val));
               break;
      case 7:  FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_7(patt_gen_wr_val));
               break;
      case 8:  FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_6(patt_gen_wr_val));
               break;
      case 9:  FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_5(patt_gen_wr_val));
               break;
      case 10: FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_4(patt_gen_wr_val));
               break;
      case 11: FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_3(patt_gen_wr_val));
               break;
      case 12: FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_2(patt_gen_wr_val));
               break;
      case 13: FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_1(patt_gen_wr_val));
               break;
      case 14: FALCON2_DINO_EFUN(falcon2_dino_wrc_patt_gen_seq_0(patt_gen_wr_val));
               break;
      default: FALCON2_DINO_EFUN_PRINTF(("ERROR: Invalid write to patt_gen_seq register\n"));
               return (falcon2_dino_error(ERR_CODE_CFG_PATT_INVALID_SEQ_WRITE));
               break;
    }
  }

  /* Pattern Generator Mode Select */
  /* FALCON2_DINO_EFUN(falcon2_dino_wr_patt_gen_mode_sel(mode_sel)); */
  /* FALCON2_DINO_EFUN_PRINTF(("Pattern gen Mode = %d\n",mode));    */

  /* Enable Fixed pattern Generation */
  /* FALCON2_DINO_EFUN(falcon2_dino_wr_patt_gen_en(0x1)); */
  return(ERR_CODE_NONE);
}


/* Enable/Disable Shared TX pattern generator */
err_code_t falcon2_dino_tx_shared_patt_gen_en( const phymod_access_t *pa, uint8_t enable, uint8_t patt_length) {
  uint8_t zero_pad_len = 0; /* suppress warnings, changed by _calc_patt_gen_mode_sel( pa, ) */
  uint8_t mode_sel     = 0; /* suppress warnings, changed by _calc_patt_gen_mode_sel( pa, ) */

  FALCON2_DINO_EFUN(falcon2_dino_calc_patt_gen_mode_sel( pa, &mode_sel,&zero_pad_len,patt_length));

  if (enable) {
      if ((mode_sel < 1) || (mode_sel > 6)) {
        return (falcon2_dino_error(ERR_CODE_PATT_GEN_INVALID_MODE_SEL));
      }
      mode_sel = (12 - mode_sel);
      FALCON2_DINO_EFUN(falcon2_dino_wr_patt_gen_start_pos(mode_sel));          /* Start position for pattern */
      FALCON2_DINO_EFUN(falcon2_dino_wr_patt_gen_stop_pos(0x0));                /* Stop position for pattern */
    FALCON2_DINO_EFUN(falcon2_dino_wr_patt_gen_en(0x1));                        /* Enable Fixed pattern Generation  */
  }
  else {
    FALCON2_DINO_EFUN(falcon2_dino_wr_patt_gen_en(0x0));                        /* Disable Fixed pattern Generation  */
  }
  return(ERR_CODE_NONE);
}

/**************************************/
/*  PMD_RX_LOCK and CL72/CL93 Status  */
/**************************************/

err_code_t falcon2_dino_pmd_lock_status( const phymod_access_t *pa, uint8_t *pmd_rx_lock) {
    if(!pmd_rx_lock) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    FALCON2_DINO_ESTM(*pmd_rx_lock = falcon2_dino_rd_pmd_rx_lock());
    return (ERR_CODE_NONE);
}


err_code_t falcon2_dino_display_cl93n72_status(const phymod_access_t *pa) {
  FALCON2_DINO_EFUN_PRINTF(("\n\n***************************\n"                                                                                                                                    ));
  FALCON2_DINO_ESTM_PRINTF((   "** LANE %d CL93n72 Status **\n"                                                                                           , falcon2_dino_get_lane(pa)                      ));
  FALCON2_DINO_EFUN_PRINTF((    "***************************\n"                                                                                                                                    ));
  FALCON2_DINO_ESTM_PRINTF((    "cl93n72_signal_detect         = %d   (1 = CL93n72 training FSM in SEND_DATA state;  0 = CL93n72 in training state)"  "\n", falcon2_dino_rd_cl93n72_training_fsm_signal_detect()));
  FALCON2_DINO_ESTM_PRINTF((    "cl93n72_ieee_training_failure = %d   (1 = Training failure detected;                0 = Training failure not detected)\n", falcon2_dino_rd_cl93n72_ieee_training_failure()     ));
  FALCON2_DINO_ESTM_PRINTF((    "cl93n72_ieee_training_status  = %d   (1 = Start-up protocol in progress;            0 = Start-up protocol complete)" "\n", falcon2_dino_rd_cl93n72_ieee_training_status()      ));
  FALCON2_DINO_ESTM_PRINTF((    "cl93n72_ieee_receiver_status  = %d   (1 = Receiver trained and ready to receive;    0 = Receiver training)"        "\n\n", falcon2_dino_rd_cl93n72_ieee_receiver_status()      ));
  return(ERR_CODE_NONE);
}


/**********************************/
/*  Serdes TX disable/RX Restart  */
/**********************************/

err_code_t falcon2_dino_tx_disable( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    FALCON2_DINO_EFUN(falcon2_dino_wr_sdk_tx_disable(0x1));
  }
  else {
    FALCON2_DINO_EFUN(falcon2_dino_wr_sdk_tx_disable(0x0));
  }
  return(ERR_CODE_NONE);
}


err_code_t falcon2_dino_rx_restart( const phymod_access_t *pa, uint8_t enable) {

  FALCON2_DINO_EFUN(falcon2_dino_wr_rx_restart_pmd_hold(enable));
  return(ERR_CODE_NONE);
}


/******************************************************/
/*  Single function to set/get all RX AFE parameters  */
/******************************************************/

err_code_t falcon2_dino_write_rx_afe( const phymod_access_t *pa, enum srds_rx_afe_settings_enum param, int8_t val) {
  /* Assumes the micro is not actively tuning */

    switch(param) {
    case RX_AFE_PF:
        return(falcon2_dino_set_rx_pf_main( pa, val));

    case RX_AFE_PF2:
        return(falcon2_dino_set_rx_pf2( pa, val));

    case RX_AFE_VGA:
      return(falcon2_dino_set_rx_vga( pa, val));

    case RX_AFE_DFE1:
        return(falcon2_dino_set_rx_dfe1( pa, val));

    case RX_AFE_DFE2:
        return(falcon2_dino_set_rx_dfe2( pa, val));

    case RX_AFE_DFE3:
        return(falcon2_dino_set_rx_dfe3( pa, val));

    case RX_AFE_DFE4:
        return(falcon2_dino_set_rx_dfe4( pa, val));

    case RX_AFE_DFE5:
        return(falcon2_dino_set_rx_dfe5( pa, val));
    case RX_AFE_DFE6:
        return (falcon2_dino_set_rx_dfe6( pa, val));

    case RX_AFE_DFE7:
        return (falcon2_dino_set_rx_dfe7( pa, val));

    case RX_AFE_DFE8:
        return (falcon2_dino_set_rx_dfe8( pa, val));

    case RX_AFE_DFE9:
        return (falcon2_dino_set_rx_dfe9( pa, val));

    case RX_AFE_DFE10:
        return (falcon2_dino_set_rx_dfe10( pa, val));

    case RX_AFE_DFE11:
        return (falcon2_dino_set_rx_dfe11( pa, val));

    case RX_AFE_DFE12:
        return (falcon2_dino_set_rx_dfe12( pa, val));

    case RX_AFE_DFE13:
        return (falcon2_dino_set_rx_dfe13( pa, val));

    case RX_AFE_DFE14:
        return (falcon2_dino_set_rx_dfe14( pa, val));
    default:
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
}


err_code_t falcon2_dino_read_rx_afe( const phymod_access_t *pa, enum srds_rx_afe_settings_enum param, int8_t *val) {
  /* Assumes the micro is not actively tuning */
    if(!val) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    switch(param) {
    case RX_AFE_PF:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_pf_main( pa, val));
        break;
    case RX_AFE_PF2:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_pf2( pa, val));
        break;
    case RX_AFE_VGA:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_vga( pa, val));
      break;

    case RX_AFE_DFE1:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe1( pa, val));
        break;
    case RX_AFE_DFE2:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe2( pa, val));
        break;
    case RX_AFE_DFE3:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe3( pa, val));
        break;
    case RX_AFE_DFE4:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe4( pa, val));
        break;
    case RX_AFE_DFE5:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe5( pa, val));
        break;
    case RX_AFE_DFE6:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe6( pa, val));
        break;
    case RX_AFE_DFE7:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe7( pa, val));
        break;
    case RX_AFE_DFE8:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe8( pa, val));
        break;
    case RX_AFE_DFE9:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe9( pa, val));
        break;
    case RX_AFE_DFE10:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe10( pa, val));
        break;
    case RX_AFE_DFE11:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe11( pa, val));
        break;
    case RX_AFE_DFE12:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe12( pa, val));
        break;
    case RX_AFE_DFE13:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe13( pa, val));
        break;
    case RX_AFE_DFE14:
        FALCON2_DINO_EFUN(falcon2_dino_get_rx_dfe14( pa, val));
        break;
    default:
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    return(ERR_CODE_NONE);
}


err_code_t falcon2_dino_validate_txfir_cfg( const phymod_access_t *pa, int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3) {
  err_code_t err_code = ERR_CODE_NONE;

  if ((pre   >  31) || (pre   <   0)) err_code |= ERR_CODE_TXFIR_PRE_INVALID;
  if ((main  > 112) || (main  <   0)) err_code |= ERR_CODE_TXFIR_MAIN_INVALID;
  if ((post1 >  63) || (post1 <   0)) err_code |= ERR_CODE_TXFIR_POST1_INVALID;
  if ((post2 >  15) || (post2 < -15)) err_code |= ERR_CODE_TXFIR_POST2_INVALID;
  if ((post3 >   7) || (post3 <  -7)) err_code |= ERR_CODE_TXFIR_POST3_INVALID;

  if ((int16_t)(main + 48) < (int16_t)(pre + post1 + post2 + post3 + 1))
    err_code |= ERR_CODE_TXFIR_V2_LIMIT;

  if ((int16_t)(pre + main + post1 + falcon2_dino_abs(post2) + falcon2_dino_abs(post3)) > 112)
    err_code |= ERR_CODE_TXFIR_SUM_LIMIT;

  return (falcon2_dino_error(err_code));
}


err_code_t falcon2_dino_write_tx_afe( const phymod_access_t *pa, enum srds_tx_afe_settings_enum param, int8_t val) {

  switch(param) {
    case TX_AFE_PRE:
        return(falcon2_dino_set_tx_pre( pa, val));
    case TX_AFE_MAIN:
        return(falcon2_dino_set_tx_main( pa, val));
    case TX_AFE_POST1:
        return(falcon2_dino_set_tx_post1( pa, val));
    case TX_AFE_POST2:
        return(falcon2_dino_set_tx_post2( pa, val));
    case TX_AFE_POST3:
        return(falcon2_dino_set_tx_post3( pa, val));
    case TX_AFE_AMP:
        return(falcon2_dino_set_tx_amp( pa, val));
    case TX_AFE_DRIVERMODE:
        if(val == DM_NOT_SUPPORTED || val > DM_HALF_AMPLITUDE_HI_IMPED) {
            return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        } else {
            return(falcon2_dino_wr_ams_tx_driver_mode(val));
        }
    default:
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
}

err_code_t falcon2_dino_read_tx_afe( const phymod_access_t *pa, enum srds_tx_afe_settings_enum param, int8_t *val) {
    if(!val) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    switch(param) {
    case TX_AFE_PRE:
        FALCON2_DINO_EFUN(falcon2_dino_get_tx_pre( pa, val));
        break;
    case TX_AFE_MAIN:
        FALCON2_DINO_EFUN(falcon2_dino_get_tx_main( pa, val));
        break;
    case TX_AFE_POST1:
        FALCON2_DINO_EFUN(falcon2_dino_get_tx_post1( pa, val));
        break;
    case TX_AFE_POST2:
        FALCON2_DINO_EFUN(falcon2_dino_get_tx_post2( pa, val));
        break;
    case TX_AFE_POST3:
        FALCON2_DINO_EFUN(falcon2_dino_get_tx_post3( pa, val));
        break;
    case TX_AFE_AMP:
        FALCON2_DINO_EFUN(falcon2_dino_get_tx_amp( pa, val));
        break;
    default:
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_apply_txfir_cfg( const phymod_access_t *pa, int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3) {

  err_code_t failcode = falcon2_dino_validate_txfir_cfg( pa, pre, main, post1, post2, post3);

  if (!failcode) {
    failcode |= falcon2_dino_set_tx_pre( pa, pre);
    failcode |= falcon2_dino_set_tx_main( pa, main);
    failcode |= falcon2_dino_set_tx_post1( pa, post1);
    failcode |= falcon2_dino_set_tx_post2( pa, post2);
    failcode |= falcon2_dino_set_tx_post3( pa, post3);
  }

  return (falcon2_dino_error(failcode));
}





/**************************************/
/*  PRBS Generator/Checker Functions  */
/**************************************/

/* Configure PRBS Generator */
err_code_t falcon2_dino_config_tx_prbs( const phymod_access_t *pa, enum srds_prbs_polynomial_enum prbs_poly_mode, uint8_t prbs_inv) {

  FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_gen_mode_sel((uint8_t)prbs_poly_mode));  /* PRBS Generator mode sel */
  FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_gen_inv(prbs_inv));                      /* PRBS Invert Enable/Disable */
  /* To enable PRBS Generator */
  /* FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_gen_en(0x1)); */
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_tx_prbs_config( const phymod_access_t *pa, enum srds_prbs_polynomial_enum *prbs_poly_mode, uint8_t *prbs_inv) {
    uint8_t val;

  FALCON2_DINO_ESTM(val = falcon2_dino_rd_prbs_gen_mode_sel());                   /* PRBS Generator mode sel */
  *prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;
  FALCON2_DINO_ESTM(val = falcon2_dino_rd_prbs_gen_inv());                        /* PRBS Invert Enable/Disable */
  *prbs_inv = val;

  return (ERR_CODE_NONE);
}

/* PRBS Generator Enable/Disable */
err_code_t falcon2_dino_tx_prbs_en( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_gen_en(0x1));                          /* Enable PRBS Generator */
  }
  else {
    FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_gen_en(0x0));                          /* Disable PRBS Generator */
  }
  return (ERR_CODE_NONE);
}

/* Get PRBS Generator Enable/Disable */
err_code_t falcon2_dino_get_tx_prbs_en( const phymod_access_t *pa, uint8_t *enable) {

  FALCON2_DINO_ESTM(*enable = falcon2_dino_rd_prbs_gen_en());

  return (ERR_CODE_NONE);
}

/* PRBS 1-bit error injection */
err_code_t falcon2_dino_tx_prbs_err_inject( const phymod_access_t *pa, uint8_t enable) {
  /* PRBS Error Insert.
     0 to 1 transition on this signal will insert single bit error in the MSB bit of the data bus.
     Reset value is 0x0.
  */
  if(enable)
    FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_gen_err_ins(0x1));
  FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_gen_err_ins(0));
  return (ERR_CODE_NONE);
}

/* Configure PRBS Checker */
err_code_t falcon2_dino_config_rx_prbs( const phymod_access_t *pa, enum srds_prbs_polynomial_enum prbs_poly_mode, enum srds_prbs_checker_mode_enum prbs_checker_mode, uint8_t prbs_inv) {

  FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_chk_mode_sel((uint8_t)prbs_poly_mode)); /* PRBS Checker Polynomial mode sel  */
  FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_chk_mode((uint8_t)prbs_checker_mode));  /* PRBS Checker mode sel (PRBS LOCK state machine select) */
  FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_chk_inv(prbs_inv));                     /* PRBS Invert Enable/Disable */
  /* To enable PRBS Checker */
  /* FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_chk_en(0x1)); */
  return (ERR_CODE_NONE);
}

/* get PRBS Checker */
err_code_t falcon2_dino_get_rx_prbs_config( const phymod_access_t *pa, enum srds_prbs_polynomial_enum *prbs_poly_mode, enum srds_prbs_checker_mode_enum *prbs_checker_mode, uint8_t *prbs_inv) {
  uint8_t val;

  FALCON2_DINO_ESTM(val = falcon2_dino_rd_prbs_chk_mode_sel());                 /* PRBS Checker Polynomial mode sel  */
  *prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;
  FALCON2_DINO_ESTM(val = falcon2_dino_rd_prbs_chk_mode());                     /* PRBS Checker mode sel (PRBS LOCK state machine select) */
  *prbs_checker_mode = (enum srds_prbs_checker_mode_enum)val;
  FALCON2_DINO_ESTM(val = falcon2_dino_rd_prbs_chk_inv());                      /* PRBS Invert Enable/Disable */
  *prbs_inv = val;
  return (ERR_CODE_NONE);
}

/* PRBS Checker Enable/Disable */
err_code_t falcon2_dino_rx_prbs_en( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_chk_burst_err_cnt_en(0x1));            /* must be enabled before prbs_chk_en */
    FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_chk_en(0x1));                          /* Enable PRBS Checker */
  }
  else {
    FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_chk_en(0x0));                          /* Disable PRBS Checker */
    FALCON2_DINO_EFUN(falcon2_dino_wr_prbs_chk_burst_err_cnt_en(0x0));
  }
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_get_rx_prbs_en( const phymod_access_t *pa, uint8_t *enable) {

  FALCON2_DINO_ESTM(*enable = falcon2_dino_rd_prbs_chk_en());

  return (ERR_CODE_NONE);
}


/* PRBS Checker Lock State */
err_code_t falcon2_dino_prbs_chk_lock_state( const phymod_access_t *pa, uint8_t *chk_lock) {
    if(!chk_lock) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    FALCON2_DINO_ESTM(*chk_lock = falcon2_dino_rd_prbs_chk_lock());                  /* PRBS Checker Lock Indication 1 = Locked, 0 = Out of Lock */
    return (ERR_CODE_NONE);
}

/* PRBS Error Count and Lock Lost (bit 31 in lock lost) */
err_code_t falcon2_dino_prbs_err_count_ll( const phymod_access_t *pa, uint32_t *prbs_err_cnt) {
    uint16_t rddata;

    if(!prbs_err_cnt) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    FALCON2_DINO_EFUN(falcon2_dino_pmd_rdt_reg(pa,FALCON2_DINO_TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS, &rddata));
    *prbs_err_cnt = ((uint32_t) rddata)<<16;
    FALCON2_DINO_ESTM(*prbs_err_cnt = (*prbs_err_cnt | falcon2_dino_rd_prbs_chk_err_cnt_lsb()));
    return (ERR_CODE_NONE);
}

/* PRBS Error Count State  */
err_code_t falcon2_dino_prbs_err_count_state( const phymod_access_t *pa, uint32_t *prbs_err_cnt, uint8_t *lock_lost) {
    if(!prbs_err_cnt || !lock_lost) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    FALCON2_DINO_EFUN(falcon2_dino_prbs_err_count_ll( pa, prbs_err_cnt));
    *lock_lost    = (*prbs_err_cnt >> 31);
    *prbs_err_cnt = (*prbs_err_cnt & 0x7FFFFFFF);
    return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_display_detailed_prbs_state_hdr( const phymod_access_t *pa ) {
    FALCON2_DINO_EFUN_PRINTF(("\nPRBS Detailed Display\n"));

  FALCON2_DINO_EFUN_PRINTF((" LN TX-Mode RX-Mode Lck LL PRBS-Error-Cnt Burst-Error"));
  FALCON2_DINO_EFUN_PRINTF((" Burst-Length Max-Burst-Length"));
  FALCON2_DINO_EFUN_PRINTF(("\n"));
  return (ERR_CODE_NONE);

}

err_code_t falcon2_dino_display_detailed_prbs_state( const phymod_access_t *pa ) {
    uint32_t err_cnt=0;
    uint8_t lock_lost=0, enabled=0;

    FALCON2_DINO_ESTM_PRINTF(("  %d ",falcon2_dino_get_lane(pa)));
    FALCON2_DINO_ESTM(enabled = falcon2_dino_rd_prbs_gen_en());
    if(enabled) {
        FALCON2_DINO_ESTM_PRINTF((" PRBS%-2d",srds_return_prbs_order(falcon2_dino_rd_prbs_gen_mode_sel())));
    } else {
        FALCON2_DINO_EFUN_PRINTF(("  OFF  "));
    }
       FALCON2_DINO_ESTM(enabled = falcon2_dino_rd_prbs_chk_en());
    if(enabled) {
        FALCON2_DINO_ESTM_PRINTF(("  PRBS%-2d",srds_return_prbs_order(falcon2_dino_rd_prbs_chk_mode_sel())));
    } else {
        FALCON2_DINO_EFUN_PRINTF(("   OFF  "));
    }
    FALCON2_DINO_ESTM_PRINTF(("  %d ",falcon2_dino_rd_prbs_chk_lock()));
    FALCON2_DINO_EFUN(falcon2_dino_prbs_err_count_state( pa, &err_cnt,&lock_lost));
    FALCON2_DINO_EFUN_PRINTF(("  %d  %010d ",lock_lost,err_cnt));
    FALCON2_DINO_ESTM_PRINTF(("      %4d ",falcon2_dino_rd_prbs_chk_burst_err_cnt()));
    FALCON2_DINO_ESTM_PRINTF(("         %2d ",falcon2_dino_rd_prbs_burst_err_length_status()));
    FALCON2_DINO_ESTM_PRINTF(("           %2d ",falcon2_dino_rd_max_prbs_burst_err_length_status()));
    FALCON2_DINO_EFUN_PRINTF(("\n"));

    return (ERR_CODE_NONE);
}
/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/

/* Enable/Diasble Digital Loopback */
err_code_t falcon2_dino_dig_lpbk( const phymod_access_t *pa, uint8_t enable) {
    FALCON2_DINO_EFUN(falcon2_dino_wr_rx_pi_manual_mode(!enable));

  FALCON2_DINO_EFUN(falcon2_dino_wr_dig_lpbk_en(enable));                         /* 0 = diabled, 1 = enabled */
  return (ERR_CODE_NONE);
}

/* Enable/Diasble Digital Loopback for Repeater */
err_code_t falcon2_dino_dig_lpbk_rptr( const phymod_access_t *pa, uint8_t enable, enum srds_rptr_mode_enum mode) {
  switch(mode) {
    case DATA_IN_SIDE:
        /* DATA_IN_SIDE  - The side where data is fed into the RX serial input */
        if (enable) {
            FALCON2_DINO_EFUN(falcon2_dino_loop_timing( pa, 0));
            FALCON2_DINO_EFUN(falcon2_dino_loop_timing( pa, 1));     /* Program DATA_IN_SIDE side TX_PI for loop-timing mode to lock SYS TX to SYS TX clock freq. */
        } else {
            FALCON2_DINO_EFUN(falcon2_dino_loop_timing( pa, 0));
        }
        break;

    case DIG_LPBK_SIDE:
        /* DIG_LPBK_SIDE - The side where data is looped back using digital loopback */
        { uint8_t    osr_mode;
          FALCON2_DINO_ESTM(osr_mode = falcon2_dino_rd_osr_mode());
          if ((osr_mode == FALCON2_DINO_OSX16P5) || (osr_mode == FALCON2_DINO_OSX20P625)) {
              FALCON2_DINO_EFUN(falcon2_dino_wr_rx_pi_manual_mode(0x0));
          }
        }
        FALCON2_DINO_EFUN(falcon2_dino_tx_rptr_mode_timing( pa, 1));          /* Program DIG_LPBK_SIDE side TX_PI for repeater mode to lock LINE TX to SYS RX clock freq. */
        FALCON2_DINO_EFUN(falcon2_dino_wr_dig_lpbk_en(enable));                  /* 0 = diabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_wr_ams_tx_sel_ll(00));                /* Switch the tx data mux to ULL traffic and check for data integrity */
        break;

    default : return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;
  }

  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_ull_from_dig_lpbk( const phymod_access_t *pa, enum srds_rptr_mode_enum mode) {
   switch(mode) {
    case DATA_IN_SIDE:
        FALCON2_DINO_EFUN(falcon2_dino_dig_lpbk_rptr( pa, 0,mode));
        FALCON2_DINO_EFUN(falcon2_dino_tx_ull_config( pa, 1));
        break;
    case DIG_LPBK_SIDE:
        FALCON2_DINO_EFUN(falcon2_dino_dig_lpbk_rptr( pa, 0,mode));
        FALCON2_DINO_EFUN(falcon2_dino_rx_ull_config( pa, 1));
        break;
    default : return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;

   }
   return (ERR_CODE_NONE);

}
err_code_t falcon2_dino_nl_from_dig_lpbk( const phymod_access_t *pa, enum srds_rptr_mode_enum mode) {
    FALCON2_DINO_EFUN(falcon2_dino_dig_lpbk_rptr( pa, 0,mode));
    return (ERR_CODE_NONE);

}




/* Locks TX_PI to Loop timing */
err_code_t falcon2_dino_loop_timing( const phymod_access_t *pa, uint8_t enable) {
    uint8_t    osr_mode;

    if (enable) {
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_loop_timing_src_sel(0x1)); /* RX phase_sum_val_logic enable */
        FALCON2_DINO_ESTM(osr_mode = falcon2_dino_rd_osr_mode());
        if ((osr_mode == FALCON2_DINO_OSX16P5) || (osr_mode == FALCON2_DINO_OSX20P625)) {
            FALCON2_DINO_EFUN(falcon2_dino_wr_rx_pi_manual_mode(0x1));
        }
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_repeater_mode_en(0x0));
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_en(0x1));                  /* TX_PI enable: 0 = diabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_jitter_filter_en(0x1));    /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_delay_us(25));               /* Wait for tclk to lock to CDR */
    } else {
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_jitter_filter_en(0x0));    /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_en(0x0));                  /* TX_PI enable: 0 = diabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_repeater_mode_en(0x1));
        /* FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_loop_timing_src_sel(0x0)); Through timing is default mode in Repeater cores. So never disabled */
    }
    return (ERR_CODE_NONE);
}


/* Setup Remote Loopback mode  */
err_code_t falcon2_dino_rmt_lpbk( const phymod_access_t *pa, uint8_t enable) {
    if (enable) {
        FALCON2_DINO_EFUN(falcon2_dino_loop_timing( pa, enable));
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_ext_ctrl_en(0x1));  /* PD path enable: 0 = diabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_wr_rmt_lpbk_en(0x1));        /* Remote Loopback Enable: 0 = diabled, 1 = enable  */
        FALCON2_DINO_EFUN(falcon2_dino_delay_us(50));        /* Wait for rclk and tclk phase lock before expecing good data from rmt loopback */
    } else {                              /* Might require longer wait time for smaller values of tx_pi_ext_phase_bwsel_integ */
        FALCON2_DINO_EFUN(falcon2_dino_wr_rmt_lpbk_en(0x0));        /* Remote Loopback Enable: 0 = diabled, 1 = enable  */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_ext_ctrl_en(0x0));  /* PD path enable: 0 = diabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_loop_timing( pa, enable));
    }
    return (ERR_CODE_NONE);
}


  /* Repeater Only APIs (Not applicable to PMD) */

/* Switch to remote loopback from ULL mode in repeater */
err_code_t falcon2_dino_rmt_lpbk_from_ull( const phymod_access_t *pa ) {
    FALCON2_DINO_EFUN(falcon2_dino_rmt_lpbk( pa, 0)); /* Reset TX_PI */
    FALCON2_DINO_EFUN(falcon2_dino_rmt_lpbk( pa, 1)); /* Enable RMT LPBK */
    return (ERR_CODE_NONE);
}


/* Switch to remote loopback from NL mode in repeater */
err_code_t falcon2_dino_rmt_lpbk_from_nl( const phymod_access_t *pa ) {
    FALCON2_DINO_EFUN(falcon2_dino_rmt_lpbk( pa, 0)); /* Reset TX_PI */
    FALCON2_DINO_EFUN(falcon2_dino_rmt_lpbk( pa, 1)); /* Enable RMT LPBK */
    return (ERR_CODE_NONE);
}


/* Switch back from remote loopback to ULL mode in repeater */
err_code_t falcon2_dino_ull_from_rmt_lpbk( const phymod_access_t *pa, enum srds_rptr_mode_enum mode) {
  switch(mode) {
    case DATA_IN_SIDE:
        FALCON2_DINO_EFUN(falcon2_dino_rx_rptr_mode_timing( pa, 1));  /* Enable jitter filter enable */
        FALCON2_DINO_EFUN(falcon2_dino_ull_rx_mode_setup( pa, 1));
        break;
    case RMT_LPBK_SIDE:
        FALCON2_DINO_EFUN(falcon2_dino_rmt_lpbk( pa, 0));         /* Reset TX_PI */
        FALCON2_DINO_EFUN(falcon2_dino_tx_rptr_mode_timing( pa, 1));  /* Enable jitter filter enable */
        FALCON2_DINO_EFUN(falcon2_dino_ull_tx_mode_setup( pa, 1));
        break;
    default : return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;

    }
    return (ERR_CODE_NONE);
}


/* Switch back from remote loopback to NL mode in repeater */
err_code_t falcon2_dino_nl_from_rmt_lpbk( const phymod_access_t *pa, enum srds_rptr_mode_enum mode) {
  switch(mode) {
    case DATA_IN_SIDE:
        FALCON2_DINO_EFUN(falcon2_dino_rx_rptr_mode_timing( pa, 1));  /* Enable RX PI transfer */
        break;
    case RMT_LPBK_SIDE:
        FALCON2_DINO_EFUN(falcon2_dino_rmt_lpbk( pa, 0));                 /* Reset TX_PI */
        FALCON2_DINO_EFUN(falcon2_dino_tx_rptr_mode_timing( pa, 1));      /* Enable jitter filter enable */
        FALCON2_DINO_EFUN(falcon2_dino_wr_ams_tx_sel_ll(0x0));              /* Switch the tx data mux to PCS data */
        break;
    default : return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;

  }
    return (ERR_CODE_NONE);
}




/**********************************/
/*  TX_PI Jitter Generation APIs  */
/**********************************/

/* TX_PI Frequency Override (Fixed Frequency Mode) */
err_code_t falcon2_dino_tx_pi_freq_override( const phymod_access_t *pa, uint8_t enable, int16_t freq_override_val) {
    if (enable) {
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_en(0x1));                              /* TX_PI enable :            0 = disabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_freq_override_en(0x1));                /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_freq_override_val(freq_override_val)); /* Fixed Freq Override Value (+/-8192) */
    }
    else {
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_freq_override_val(0));                 /* Fixed Freq Override Value to 0 */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_freq_override_en(0x0));                /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_en(0x0));                              /* TX_PI enable :            0 = disabled, 1 = enabled */
    }
  return (ERR_CODE_NONE);
}


/* TX_PI Sinusoidal or Spread-Spectrum (SSC) Jitter Generation  */
err_code_t falcon2_dino_tx_pi_jitt_gen( const phymod_access_t *pa, uint8_t enable, int16_t freq_override_val, enum srds_tx_pi_freq_jit_gen_enum jit_type, uint8_t tx_pi_jit_freq_idx, uint8_t tx_pi_jit_amp) {
    /* Added a limiting for the jitter amplitude index, per freq_idx */
    uint8_t max_amp_idx_r20_os1[] = {37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 48, 33, 47, 37, 33, 37, 37};

    /* Irrespective of the osr_mode, txpi runs @ os1. Thus the max amp idx values remain the same. */
    if (jit_type == TX_PI_SJ) {
        if (tx_pi_jit_amp > max_amp_idx_r20_os1[tx_pi_jit_freq_idx]) {
            tx_pi_jit_amp = max_amp_idx_r20_os1[tx_pi_jit_freq_idx];
        }
    }

    FALCON2_DINO_EFUN(falcon2_dino_tx_pi_freq_override( pa, enable, freq_override_val));

    if (enable) {
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_jit_freq_idx(tx_pi_jit_freq_idx));
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_jit_amp(tx_pi_jit_amp));

        if (jit_type == TX_PI_SSC_HIGH_FREQ) {
            FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_jit_ssc_freq_mode(0x1));        /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
            FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_ssc_gen_en(0x1));               /* SSC jitter enable:         0 = disabled,    1 = enabled */
        }
        else if (jit_type == TX_PI_SSC_LOW_FREQ) {
            FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_jit_ssc_freq_mode(0x0));        /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
            FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_ssc_gen_en(0x1));               /* SSC jitter enable:         0 = disabled,    1 = enabled */
        }
        else if (jit_type == TX_PI_SJ) {
            FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_sj_gen_en(0x1));                /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
        }
    }
    else {
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_ssc_gen_en(0x0));                   /* SSC jitter enable:         0 = disabled,    1 = enabled */
        FALCON2_DINO_EFUN(falcon2_dino_wr_tx_pi_sj_gen_en(0x0));                    /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
    }
  return (ERR_CODE_NONE);
}


/***************************/
/*  Configure Serdes IDDQ  */
/***************************/

err_code_t falcon2_dino_core_config_for_iddq( const phymod_access_t *pa ) {

  return (ERR_CODE_NONE);
}


err_code_t falcon2_dino_lane_config_for_iddq( const phymod_access_t *pa ) {

  /* Use frc/frc_val to force all RX and TX clk_vld signals to 0 */
  FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_rx_clk_vld_frc_val(0x0));
  FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_rx_clk_vld_frc(0x1));
      FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_tx_clk_vld_frc_val(0x0));
      FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_tx_clk_vld_frc(0x1));


  /* Use frc/frc_val to force all pmd_rx_lock signals to 0 */
  FALCON2_DINO_EFUN(falcon2_dino_wr_rx_dsc_lock_frc_val(0x0));
  FALCON2_DINO_EFUN(falcon2_dino_wr_rx_dsc_lock_frc(0x1));

  /* Switch all the lane clocks to comclk by writing to RX/TX comclk_sel registers */
  FALCON2_DINO_EFUN(falcon2_dino_wr_ln_rx_s_comclk_sel(0x1));
    FALCON2_DINO_EFUN(falcon2_dino_wr_ln_tx_s_comclk_sel(0x1));

  /* Assert all the AFE pwrdn/reset pins using frc/frc_val to make sure AFE is in lowest possible power mode */
  FALCON2_DINO_EFUN(falcon2_dino_wr_afe_tx_pwrdn_frc_val(0x1));
  FALCON2_DINO_EFUN(falcon2_dino_wr_afe_tx_pwrdn_frc(0x1));
  FALCON2_DINO_EFUN(falcon2_dino_wr_afe_rx_pwrdn_frc_val(0x1));
  FALCON2_DINO_EFUN(falcon2_dino_wr_afe_rx_pwrdn_frc(0x1));
  FALCON2_DINO_EFUN(falcon2_dino_wr_afe_tx_reset_frc_val(0x1));
  FALCON2_DINO_EFUN(falcon2_dino_wr_afe_tx_reset_frc(0x1));
  FALCON2_DINO_EFUN(falcon2_dino_wr_afe_rx_reset_frc_val(0x1));
  FALCON2_DINO_EFUN(falcon2_dino_wr_afe_rx_reset_frc(0x1));

  /* Set pmd_iddq pin to enable IDDQ */
  return (ERR_CODE_NONE);
}


/*********************************/
/*  uc_active and uc_reset APIs  */
/*********************************/

/* Set the uC active mode */
err_code_t falcon2_dino_uc_active_enable( const phymod_access_t *pa, uint8_t enable) {
  FALCON2_DINO_EFUN(falcon2_dino_wrc_uc_active(enable));
  return (ERR_CODE_NONE);
}



/* Enable or Disable the uC reset */
err_code_t falcon2_dino_uc_reset( const phymod_access_t *pa, uint8_t enable) {
  if (enable) {
    /* Assert micro reset and reset all micro registers (all non-status registers written to default value) */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_core_clk_en(0x0));                     /* Disable clock to M0 core */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_clk_en(0x0));                   /* Disable clock to microcontroller subsystem */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_protect_fwcode(0x1));                  /* Gate-off RAM chip select and other signals to avoid ucode corruption */
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD200, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD201, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD202, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD204, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD205, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD206, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD207, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD208, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD209, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD20A, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD20B, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD20C, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD20D, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD20E, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD211, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD212, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD213, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD214, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD215, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD216, 0x0007));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD217, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD218, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD219, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD21A, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD21B, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD220, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD221, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD224, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD226, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD225, 0x8803));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD228, 0x0101));
    FALCON2_DINO_EFUN(falcon2_dino_pmd_wr_reg(pa,0xD229, 0x0000));
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_protect_fwcode(0x0));                  /* Remove Gate-off RAM chip select and other signals */
  }
  else {
    /* De-assert micro reset - Start executing code */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_clk_en(0x1));                   /* Enable clock to microcontroller subsystem */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_rstb  (0x1));                   /* De-assert reset to microcontroller sybsystem */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_init            (0x2));             /* Write command for data RAM initialization */
    FALCON2_DINO_EFUN(falcon2_dino_poll_micro_ra_initdone( pa, 250));             /* Poll   status of  data RAM initialization */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_init            (0x0));             /* Clear command for data RAM initialization */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_core_clk_en  (0x1));                   /* Enable clock to M0 core */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_core_rstb    (0x1));
  }
 return (ERR_CODE_NONE);
}

/******************/
/*  Lane Mapping  */
/******************/

err_code_t falcon2_dino_map_lanes(const phymod_access_t *pa, uint8_t num_lanes, uint8_t const *tx_lane_map, uint8_t const *rx_lane_map) {
    uint8_t rd_val = 0;

    /* Verify that the core data path is held in reset. */
    FALCON2_DINO_ESTM(rd_val = falcon2_dino_rdc_uc_active());
    if (rd_val != 0) {
        return (ERR_CODE_UC_ACTIVE);
    }

    /* Verify that the num_lanes parameter is correct. */
    FALCON2_DINO_ESTM(rd_val = falcon2_dino_rdc_revid_multiplicity());
    if (rd_val != num_lanes) {
        return (ERR_CODE_BAD_LANE_COUNT);
    }

    /* Verify that tx_lane_map and rx_lane_map are valid. */
    {
        uint8_t index1, index2;
        uint8_t lp_message_printed = 0;

        /* Avoid unused variable messages if not used for core. */
        (void)lp_message_printed;
        (void)index2;

        for (index1=0; index1<num_lanes; ++index1) {

#if defined(falcon2_dino_wrc_tx_lane_addr_0) || defined(falcon2_dino_wrc_tx_lane_map_0)

            /* Not able to configure lane mapping.  So verify that it's the default mapping. */
            if ((tx_lane_map[index1] != index1)
                || (rx_lane_map[index1] != index1)) {
                return (ERR_CODE_BAD_LANE);
            }

#else /* defined(falcon2_dino_wrc_tx_lane_addr_0) || defined(falcon2_dino_wrc_tx_lane_map_0)*/

            /* Verify that a lane map is not to an invalid lane. */
            if ((tx_lane_map[index1] >= num_lanes)
                || (rx_lane_map[index1] >= num_lanes)){
                return (ERR_CODE_BAD_LANE);
            }

            /* For this core, TX and RX lane mapping can be independent.
             * But warn if they are different.
             */
            if ((tx_lane_map[index1] != rx_lane_map[index1])
                && (!lp_message_printed)) {
                FALCON2_DINO_EFUN_PRINTF(("Warning:  In core %d, TX lane %d is mapped to %d, while RX lane %d is mapped to %d.\n          Digital and remote loopback will not operate as expected.\n          Further warnings are suppressed.\n",
                             FALCON2_DINO_EFUN(falcon2_dino_get_core()), index1, tx_lane_map[index1], index1, rx_lane_map[index1]));
                lp_message_printed = 1;
            }

            /* Verify that a lane map is not used twice. */
            for (index2=index1+1; index2<num_lanes; ++index2) {
                if ((tx_lane_map[index1] == tx_lane_map[index2])
                    || (rx_lane_map[index1] == rx_lane_map[index2])) {
                    return (ERR_CODE_BAD_LANE);
                }
            }

#endif /* defined(falcon2_dino_wrc_tx_lane_addr_0) || defined(falcon2_dino_wrc_tx_lane_map_0)*/

        }
    }

    /* Write the map bitfields.
     * Support up to 4 lanes.
     */
#if defined(falcon2_dino_wrc_tx_lane_addr_0)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_tx_lane_addr_0(*(tx_lane_map++)));
#elif defined(falcon2_dino_wrc_tx_lane_map_0)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_tx_lane_map_0(*(tx_lane_map++)));
#endif
#if defined(falcon2_dino_wrc_tx_lane_addr_1)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_tx_lane_addr_1(*(tx_lane_map++)));
#elif defined(falcon2_dino_wrc_tx_lane_map_1)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_tx_lane_map_1(*(tx_lane_map++)));
#endif
#if defined(falcon2_dino_wrc_tx_lane_addr_2)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_tx_lane_addr_2(*(tx_lane_map++)));
#elif defined(falcon2_dino_wrc_tx_lane_map_2)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_tx_lane_map_2(*(tx_lane_map++)));
#endif
#if defined(falcon2_dino_wrc_tx_lane_addr_3)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_tx_lane_addr_3(*(tx_lane_map++)));
#elif defined(falcon2_dino_wrc_tx_lane_map_3)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_tx_lane_map_3(*(tx_lane_map++)));
#endif

#if defined(falcon2_dino_wrc_rx_lane_addr_0)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_rx_lane_addr_0(*(rx_lane_map++)));
#elif defined(falcon2_dino_wrc_rx_lane_map_0)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_rx_lane_map_0(*(rx_lane_map++)));
#endif
#if defined(falcon2_dino_wrc_rx_lane_addr_1)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_rx_lane_addr_1(*(rx_lane_map++)));
#elif defined(falcon2_dino_wrc_rx_lane_map_1)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_rx_lane_map_1(*(rx_lane_map++)));
#endif
#if defined(falcon2_dino_wrc_rx_lane_addr_2)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_rx_lane_addr_2(*(rx_lane_map++)));
#elif defined(falcon2_dino_wrc_rx_lane_map_2)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_rx_lane_map_2(*(rx_lane_map++)));
#endif
#if defined(falcon2_dino_wrc_rx_lane_addr_3)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_rx_lane_addr_3(*(rx_lane_map++)));
#elif defined(falcon2_dino_wrc_rx_lane_map_3)
   FALCON2_DINO_EFUN(falcon2_dino_wrc_rx_lane_map_3(*(rx_lane_map++)));
#endif

    return (ERR_CODE_NONE);
}

/****************************************************/
/*  Serdes Powerdown, ClockGate and Deep_Powerdown  */
/****************************************************/

err_code_t falcon2_dino_core_pwrdn( const phymod_access_t *pa, enum srds_core_pwrdn_mode_enum mode) {
    switch(mode) {
    case PWR_ON:
        FALCON2_DINO_EFUN(_falcon2_dino_core_clkgate( pa, 0));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_afe_s_pll_pwrdn(0x0));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_core_dp_s_rstb(0x1));
        break;
    case PWRDN:
        FALCON2_DINO_EFUN(falcon2_dino_wrc_core_dp_s_rstb(0x0));
        FALCON2_DINO_EFUN(falcon2_dino_delay_ns(   500)); /* wait >50 comclk cycles  */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_afe_s_pll_pwrdn(0x1));
        break;
    case PWRDN_DEEP:
        FALCON2_DINO_EFUN(falcon2_dino_wrc_core_dp_s_rstb(0x0));
        FALCON2_DINO_EFUN(falcon2_dino_delay_ns(   500)); /* wait >50 comclk cycles  */
                FALCON2_DINO_EFUN(falcon2_dino_wrc_afe_s_pll_pwrdn(0x1));
        FALCON2_DINO_EFUN(_falcon2_dino_core_clkgate( pa, 1));
        break;
    default:
        FALCON2_DINO_EFUN(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;
    }
    return ERR_CODE_NONE;
}

err_code_t falcon2_dino_lane_pwrdn( const phymod_access_t *pa, enum srds_core_pwrdn_mode_enum mode) {
    switch(mode) {
    case PWR_ON:
        FALCON2_DINO_EFUN(falcon2_dino_wr_ln_tx_s_pwrdn(0x0));
        FALCON2_DINO_EFUN(falcon2_dino_wr_ln_rx_s_pwrdn(0x0));
        FALCON2_DINO_EFUN(_falcon2_dino_lane_clkgate( pa, 0));
        break;
    case PWRDN:
        /* do the RX first, since that is what is most users care about */
        FALCON2_DINO_EFUN(falcon2_dino_wr_ln_rx_s_pwrdn(0x1));
        FALCON2_DINO_EFUN(falcon2_dino_wr_ln_tx_s_pwrdn(0x1));
        break;
    case PWRDN_DEEP:
        /* do the RX first, since that is what is most users care about */
        FALCON2_DINO_EFUN(falcon2_dino_wr_ln_rx_s_pwrdn(0x1));
        FALCON2_DINO_EFUN(falcon2_dino_wr_ln_tx_s_pwrdn(0x1));
        FALCON2_DINO_EFUN(_falcon2_dino_lane_clkgate( pa, 1));
        FALCON2_DINO_EFUN(falcon2_dino_wr_ln_dp_s_rstb(0x0));
        break;
    case PWRDN_TX:
        FALCON2_DINO_EFUN(falcon2_dino_wr_ln_tx_s_pwrdn(0x1));
        break;
    case PWRDN_RX:
        FALCON2_DINO_EFUN(falcon2_dino_wr_ln_rx_s_pwrdn(0x1));
        break;
    default :
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;
    }
  return (ERR_CODE_NONE);
}


/*******************************/
/*  Isolate Serdes Input Pins  */
/*******************************/

err_code_t falcon2_dino_isolate_ctrl_pins( const phymod_access_t *pa, uint8_t enable) {

    FALCON2_DINO_EFUN(falcon2_dino_isolate_lane_ctrl_pins( pa, enable));
    FALCON2_DINO_EFUN(falcon2_dino_isolate_core_ctrl_pins( pa, enable));

  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_isolate_lane_ctrl_pins( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_tx_h_pwrdn_pkill(0x1));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_rx_h_pwrdn_pkill(0x1));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_dp_h_rstb_pkill(0x1));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_h_rstb_pkill(0x1));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_tx_disable_pkill(0x1));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_rx_h_rstb_pkill(0x1));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_rx_dp_h_rstb_pkill(0x1));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_tx_dp_h_rstb_pkill(0x1));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_tx_h_rstb_pkill(0x1));
  }
  else {
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_tx_h_pwrdn_pkill(0x0));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_rx_h_pwrdn_pkill(0x0));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_dp_h_rstb_pkill(0x0));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_h_rstb_pkill(0x0));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_tx_disable_pkill(0x0));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_rx_h_rstb_pkill(0x0));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_rx_dp_h_rstb_pkill(0x0));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_tx_dp_h_rstb_pkill(0x0));
    FALCON2_DINO_EFUN(falcon2_dino_wr_pmd_ln_tx_h_rstb_pkill(0x0));
  }
  return (ERR_CODE_NONE);
}

err_code_t falcon2_dino_isolate_core_ctrl_pins( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    FALCON2_DINO_EFUN(falcon2_dino_wrc_pmd_core_dp_h_rstb_pkill(0x1));
    FALCON2_DINO_EFUN(falcon2_dino_wrc_pmd_mdio_trans_pkill(0x1));
  }
  else {
    FALCON2_DINO_EFUN(falcon2_dino_wrc_pmd_core_dp_h_rstb_pkill(0x0));
    FALCON2_DINO_EFUN(falcon2_dino_wrc_pmd_mdio_trans_pkill(0x0));
  }
  return (ERR_CODE_NONE);
}




/***********************************************/
/*  Microcode Load into Program RAM Functions  */
/***********************************************/


/* uCode Load through Register (MDIO) Interface [Return Val = Error_Code (0 = PASS)] */
err_code_t falcon2_dino_ucode_mdio_load( const phymod_access_t *pa, uint8_t *ucode_image, uint16_t ucode_len) {
    uint16_t   ucode_len_padded, count = 0;
    uint16_t   wrdata_lsw;
    uint8_t    wrdata_lsb;

    if(!ucode_image) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if (ucode_len > FALCON2_DINO_UCODE_MAX_SIZE) {                     /* uCode size should be less than UCODE_MAX_SIZE  */
        return (falcon2_dino_error(ERR_CODE_INVALID_UCODE_LEN));
    }

    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_protect_fwcode(0x0));                  /* Remove Gate-off RAM chip select and other signals */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_clk_en(0x1));                   /* Enable clock to microcontroller subsystem */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_rstb(0x1));                     /* De-assert reset to microcontroller sybsystem */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_rstb(0x0));                     /* Assert reset to microcontroller sybsystem - Toggling reset*/
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_master_rstb(0x1));                     /* De-assert reset to microcontroller sybsystem */

    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_init(0x1));                         /* Set initialization command to initialize code RAM */
    FALCON2_DINO_EFUN(falcon2_dino_poll_micro_ra_initdone( pa, 250));             /* Poll for micro_ra_initdone = 1 to indicate initialization done */

    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_init(0x0));                         /* Clear initialization command */

    ucode_len_padded = ((ucode_len + 3) & 0xFFFC);        /* Aligning ucode size to 4-byte boundary */

    /* Code to Load microcode */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_autoinc_wraddr_en(0x1));               /* To auto increment RAM write address */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wrdatasize(0x1));                   /* Select 16bit transfers */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wraddr_msw(0x0));                   /* Upper 16bits of start address of Program RAM where the ucode is to be loaded */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wraddr_lsw(0x0));                   /* Lower 16bits of start address of Program RAM where the ucode is to be loaded */

    do {                                                  /* ucode_image loaded 16bits at a time */
        wrdata_lsb = (count < ucode_len) ? ucode_image[count] : 0x0; /* wrdata_lsb read from ucode_image; zero padded to 4byte boundary */
        count++;
        wrdata_lsw = (count < ucode_len) ? ucode_image[count] : 0x0; /* wrdata_msb read from ucode_image; zero padded to 4byte boundary */
        count++;
        wrdata_lsw = ((wrdata_lsw << 8) | wrdata_lsb);               /* 16bit wrdata_lsw formed from 8bit msb and lsb values read from ucode_image */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wrdata_lsw(wrdata_lsw));                   /* Program RAM lower 16bits write data */
    }   while (count < ucode_len_padded);                 /* Loop repeated till entire image loaded (upto the 4byte boundary) */

    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wrdatasize(0x2));                   /* Select 32bit transfers as default */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_core_clk_en(0x1));                     /* Enable clock to M0 core */
 /* FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_core_rstb(0x1)); */                    /* De-assert reset to micro to start executing microcode */
    return (ERR_CODE_NONE);                               /* NO Errors while loading microcode (uCode Load PASS) */
  }


/* Read-back uCode from Program RAM and verify against ucode_image [Return Val = Error_Code (0 = PASS)]  */
err_code_t falcon2_dino_ucode_load_verify( const phymod_access_t *pa, uint8_t *ucode_image, uint16_t ucode_len) {

    uint16_t ucode_len_padded, count = 0;
    uint16_t rddata_lsw;
    uint16_t data_lsw;
    uint8_t  rddata_lsb;

    if(!ucode_image) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    ucode_len_padded = ((ucode_len + 3) & 0xFFFC);        /* Aligning ucode size to 4-byte boundary */

    if (ucode_len_padded > FALCON2_DINO_UCODE_MAX_SIZE) {              /* uCode size should be less than UCODE_MAX_SIZE */
        return (falcon2_dino_error(ERR_CODE_INVALID_UCODE_LEN));
    }

    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_autoinc_rdaddr_en(0x1));               /* To auto increment RAM read address */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_rddatasize(0x1));                   /* Select 16bit transfers */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_rdaddr_msw(0x0));                   /* Upper 16bits of start address of Program RAM from where to read ucode */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_rdaddr_lsw(0x0));                   /* Lower 16bits of start address of Program RAM from where to read ucode */

    do {                                                  /* ucode_image read 16bits at a time */
        rddata_lsb = (count < ucode_len) ? ucode_image[count] : 0x0; /* rddata_lsb read from ucode_image; zero padded to 4byte boundary */
        count++;
        rddata_lsw = (count < ucode_len) ? ucode_image[count] : 0x0; /* rddata_msb read from ucode_image; zero padded to 4byte boundary */
        count++;
        rddata_lsw = ((rddata_lsw << 8) | rddata_lsb);               /* 16bit rddata_lsw formed from 8bit msb and lsb values read from ucode_image */
                                                                     /* Compare Program RAM ucode to ucode_image (Read to micro_ra_rddata_lsw reg auto-increments the ram_address) */
        FALCON2_DINO_ESTM(data_lsw = falcon2_dino_rdc_micro_ra_rddata_lsw());
        if (data_lsw != rddata_lsw) {
            FALCON2_DINO_EFUN_PRINTF(("Ucode_Load_Verify_FAIL: Addr = 0x%x: Read_data = 0x%x :  Expected_data = 0x%x \n",(count-2),data_lsw,rddata_lsw));
            return (falcon2_dino_error(ERR_CODE_UCODE_VERIFY_FAIL));             /* Verify uCode FAIL */
        }

    } while (count < ucode_len_padded);                   /* Loop repeated till entire image loaded (upto the 4byte boundary) */

    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_rddatasize(0x2));                   /* Select 32bit transfers ad default */
    return (ERR_CODE_NONE);                               /* Verify uCode PASS */
}



/*************************************************/
/*  RAM access through Micro Register Interface  */
/*************************************************/


/* Micro RAM Word Write */
err_code_t falcon2_dino_wrw_uc_ram( const phymod_access_t *pa, uint16_t addr, uint16_t wr_val) {

    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_autoinc_wraddr_en(0));
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wrdatasize(0x1));                 /* Select 16bit write datasize */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wraddr_msw(0x2000));              /* Upper 16bits of RAM address to be written to */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wraddr_lsw(addr));                /* Lower 16bits of RAM address to be written to */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wrdata_lsw(wr_val));              /* uC RAM lower 16bits write data */
    return (ERR_CODE_NONE);
}

/* Micro RAM Byte Write */
err_code_t falcon2_dino_wrb_uc_ram( const phymod_access_t *pa, uint16_t addr, uint8_t wr_val) {
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_autoinc_wraddr_en(0));
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wrdatasize(0x0));                 /* Select 8bit write datasize */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wraddr_msw(0x2000));              /* Upper 16bits of RAM address to be written to */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wraddr_lsw(addr));                /* Lower 16bits of RAM address to be written to */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_micro_ra_wrdata_lsw(wr_val));              /* uC RAM lower 16bits write data */
    return (ERR_CODE_NONE);
}

/* Micro RAM Word Read */
uint16_t falcon2_dino_rdw_uc_ram( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
   uint16_t rddata;
   if(!err_code_p) {
        return(0);
   }
   *err_code_p = ERR_CODE_NONE;
   FALCON2_DINO_EPFUN(falcon2_dino_wrc_micro_autoinc_rdaddr_en(0));
   FALCON2_DINO_EPFUN(falcon2_dino_wrc_micro_ra_rddatasize(0x1));                 /* Select 16bit read datasize */
   FALCON2_DINO_EPFUN(falcon2_dino_wrc_micro_ra_rdaddr_msw(0x2000));              /* Upper 16bits of RAM address to be read */
   FALCON2_DINO_EPFUN(falcon2_dino_wrc_micro_ra_rdaddr_lsw(addr));                /* Lower 16bits of RAM address to be read */
   FALCON2_DINO_EPSTM(rddata = falcon2_dino_rdc_micro_ra_rddata_lsw());           /* 16bit read data */
   return (rddata);
}

/* Micro RAM Byte Read */
uint8_t falcon2_dino_rdb_uc_ram( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
    uint8_t rddata;
    if(!err_code_p) {
        return(0);
    }
    *err_code_p = ERR_CODE_NONE;
    FALCON2_DINO_EPFUN(falcon2_dino_wrc_micro_autoinc_rdaddr_en(0));
    FALCON2_DINO_EPFUN(falcon2_dino_wrc_micro_ra_rddatasize(0x0));                 /* Select 8bit read datasize */
    FALCON2_DINO_EPFUN(falcon2_dino_wrc_micro_ra_rdaddr_msw(0x2000));              /* Upper 16bits of RAM address to be read */
    FALCON2_DINO_EPFUN(falcon2_dino_wrc_micro_ra_rdaddr_lsw(addr));                /* Lower 16bits of RAM address to be read */
    FALCON2_DINO_EPSTM(rddata = (uint8_t) falcon2_dino_rdc_micro_ra_rddata_lsw()); /* 16bit read data */
    return (rddata);
}



/* Micro RAM Word Signed Write */
err_code_t falcon2_dino_wrws_uc_ram( const phymod_access_t *pa, uint16_t addr, int16_t wr_val) {
    return (falcon2_dino_wrw_uc_ram( pa, addr, wr_val));
}

/* Micro RAM Byte Signed Write */
err_code_t falcon2_dino_wrbs_uc_ram( const phymod_access_t *pa, uint16_t addr, int8_t wr_val) {
    return (falcon2_dino_wrb_uc_ram( pa, addr, wr_val));
}

/* Micro RAM Word Signed Read */
int16_t falcon2_dino_rdws_uc_ram( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
    return ((int16_t)falcon2_dino_rdw_uc_ram( pa, err_code_p, addr));
}

/* Micro RAM Byte Signed Read */
int8_t falcon2_dino_rdbs_uc_ram( const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr) {
    return ((int8_t)falcon2_dino_rdb_uc_ram( pa, err_code_p, addr));
}


/* Micro RAM Byte Read */
err_code_t falcon2_dino_rdblk_uc_ram( const phymod_access_t *pa, uint8_t *mem, uint16_t addr, uint16_t cnt) {
    if(!mem) {
        return(falcon2_dino_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    while (cnt--) {
        err_code_t error_code;
        uint8_t    tmp = falcon2_dino_rdb_uc_ram( pa, &error_code, addr++); /* Value read from the required Data RAM Address */
        if (error_code) {
            return falcon2_dino_error(error_code);
        }
        *mem++ = tmp;
    }
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_pll_drv_hv_disable( const phymod_access_t *pa, uint8_t disable) {
  return(falcon2_dino_wrc_ams_pll_drv_hv_disable(disable&0x1));
}

