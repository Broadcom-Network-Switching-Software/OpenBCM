/**************************************************************************************
 **************************************************************************************
 *  File Name     :  eagle_phy_1_dv_functions.c                                        *
 *  Created On    :  22/05/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  APIs for Serdes IPs ported over from DV                          *
 *  Revision      :  $Id: eagle_phy_1_dv_functions.c 1543 2017-04-07 20:52:44Z kirand $ *
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

/** @file eagle_phy_1_dv_functions.c
 * Implementation of API functions ported over from DV
 */

/******************************/
/*  TX Pattern Generator APIs */
/******************************/

/* Cofigure shared TX Pattern (Return Val = 0:PASS, 1-6:FAIL (reports 6 possible error scenarios if failed)) */
err_code_t eagle_phy_1_config_shared_tx_pattern(uint8_t patt_length, const char pattern[]) {

  char       patt_final[245] = "";
  char       patt_mod[245]   = "", bin[5] = "";
  uint8_t    str_len, i, k, j = 0;
  uint8_t    offset_len, actual_patt_len = 0, hex = 0;
  uint8_t    zero_pad_len    = 0; /* suppress warnings, changed by _calc_patt_gen_mode_sel() */
  uint16_t   patt_gen_wr_val = 0;
  uint8_t    mode_sel        = 0; /* suppress warnings, changed by _calc_patt_gen_mode_sel() */

  EFUN(_calc_patt_gen_mode_sel(&mode_sel,&zero_pad_len,patt_length));

  /* Generating the appropriate write value to patt_gen_seq registers */
  str_len = (int8_t)USR_STRLEN(pattern);

  if ((str_len > 2) && ((USR_STRNCMP(pattern, "0x", 2)) == 0)) {
    /* Hexadecimal Pattern */
    for (i=2; i < str_len; i++) {
      EFUN(_compute_bin(pattern[i],bin));
      ENULL_STRNCAT(patt_mod,bin);
      if (pattern[i] != '_') {
        actual_patt_len = actual_patt_len + 4;
        if (actual_patt_len > 240) {
          EFUN_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
          return (_error(ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN));
        }
      }
    }

    offset_len = (actual_patt_len - patt_length);
    if ((offset_len > 3)  || (actual_patt_len < patt_length)) {
      EFUN_PRINTF(("ERROR: Pattern length provided does not match the hexadecimal pattern provided\n"));
      return (_error(ERR_CODE_CFG_PATT_LEN_MISMATCH));
    }
    else if (offset_len) {
      for (i=0; i < offset_len; i++) {
        if (patt_mod[i] != '0') {
          EFUN_PRINTF(("ERROR: Pattern length provided does not match the hexadecimal pattern provided\n"));
          return (_error(ERR_CODE_CFG_PATT_LEN_MISMATCH));
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
        ENULL_STRNCAT(patt_mod,bin);
        actual_patt_len++;
        if (actual_patt_len > 240) {
          EFUN_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
          return (_error(ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN));
        }
      }
      else if (pattern[i] != '_') {
        EFUN_PRINTF(("ERROR: Invalid input Pattern\n"));
        return (_error(ERR_CODE_CFG_PATT_INVALID_PATTERN));
      }
    }

    if (actual_patt_len != patt_length) {
      EFUN_PRINTF(("ERROR: Pattern length provided does not match the binary pattern provided\n"));
      return (_error(ERR_CODE_CFG_PATT_LEN_MISMATCH));
    }
  }

  /* Zero padding upper bits and concatinating patt_mod to form patt_final */
  for (i=0; i < zero_pad_len; i++) {
    ENULL_STRNCAT(patt_final,"0");
    j++;
  }
  for (i=zero_pad_len; i + patt_length < 241; i = i + patt_length) {
    ENULL_STRNCAT(patt_final,patt_mod);
    j++;
  }

  /* EFUN_PRINTF(("\nFinal Pattern = %s\n\n",patt_final));    */

  for (i=0; i < 15; i++) {

    for (j=0; j < 4; j++) {
      k = i*16 + j*4;
      bin[0] = patt_final[k];
      bin[1] = patt_final[k+1];
      bin[2] = patt_final[k+2];
      bin[3] = patt_final[k+3];
      bin[4] = '\0';
      EFUN(_compute_hex(bin, &hex));
      patt_gen_wr_val = ((patt_gen_wr_val << 4) | hex);
    }
    /* EFUN_PRINTF(("patt_gen_wr_val[%d] = 0x%x\n",(14-i),patt_gen_wr_val));    */

    /* Writing to apprpriate patt_gen_seq Registers */
    switch (i) {
      case 0:  EFUN(wrc_patt_gen_seq_14(patt_gen_wr_val));
               break;
      case 1:  EFUN(wrc_patt_gen_seq_13(patt_gen_wr_val));
               break;
      case 2:  EFUN(wrc_patt_gen_seq_12(patt_gen_wr_val));
               break;
      case 3:  EFUN(wrc_patt_gen_seq_11(patt_gen_wr_val));
               break;
      case 4:  EFUN(wrc_patt_gen_seq_10(patt_gen_wr_val));
               break;
      case 5:  EFUN(wrc_patt_gen_seq_9(patt_gen_wr_val));
               break;
      case 6:  EFUN(wrc_patt_gen_seq_8(patt_gen_wr_val));
               break;
      case 7:  EFUN(wrc_patt_gen_seq_7(patt_gen_wr_val));
               break;
      case 8:  EFUN(wrc_patt_gen_seq_6(patt_gen_wr_val));
               break;
      case 9:  EFUN(wrc_patt_gen_seq_5(patt_gen_wr_val));
               break;
      case 10: EFUN(wrc_patt_gen_seq_4(patt_gen_wr_val));
               break;
      case 11: EFUN(wrc_patt_gen_seq_3(patt_gen_wr_val));
               break;
      case 12: EFUN(wrc_patt_gen_seq_2(patt_gen_wr_val));
               break;
      case 13: EFUN(wrc_patt_gen_seq_1(patt_gen_wr_val));
               break;
      case 14: EFUN(wrc_patt_gen_seq_0(patt_gen_wr_val));
               break;
      default: EFUN_PRINTF(("ERROR: Invalid write to patt_gen_seq register\n"));
               return (_error(ERR_CODE_CFG_PATT_INVALID_SEQ_WRITE));
               break;
    }
  }

  /* Pattern Generator Mode Select */
  /* EFUN(wr_patt_gen_mode_sel(mode_sel)); */
  /* EFUN_PRINTF(("Pattern gen Mode = %d\n",mode));    */

  /* Enable Fixed pattern Generation */
  /* EFUN(wr_patt_gen_en(0x1)); */
  return(ERR_CODE_NONE);
}


/* Enable/Disable Shared TX pattern generator */
err_code_t eagle_phy_1_tx_shared_patt_gen_en(uint8_t enable, uint8_t patt_length) {
  uint8_t zero_pad_len = 0; /* suppress warnings, changed by _calc_patt_gen_mode_sel() */
  uint8_t mode_sel     = 0; /* suppress warnings, changed by _calc_patt_gen_mode_sel() */

  EFUN(_calc_patt_gen_mode_sel(&mode_sel,&zero_pad_len,patt_length));

  if (enable) {
      if ((mode_sel < 1) || (mode_sel > 6)) {
        return (_error(ERR_CODE_PATT_GEN_INVALID_MODE_SEL));
      }
      mode_sel = (12 - mode_sel);
      EFUN(wr_patt_gen_start_pos(mode_sel));          /* Start position for pattern */
      EFUN(wr_patt_gen_stop_pos(0x0));                /* Stop position for pattern */
    EFUN(wr_patt_gen_en(0x1));                        /* Enable Fixed pattern Generation  */
  }
  else {
    EFUN(wr_patt_gen_en(0x0));                        /* Disable Fixed pattern Generation  */
  }
  return(ERR_CODE_NONE);
}

/**************************************/
/*  PMD_RX_LOCK and CL72/CL93 Status  */
/**************************************/

err_code_t eagle_phy_1_pmd_lock_status(uint8_t *pmd_rx_lock) {
    if(!pmd_rx_lock) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    ESTM(*pmd_rx_lock = rd_pmd_rx_lock());
    return (ERR_CODE_NONE);
}


err_code_t eagle_phy_1_display_cl72_status(void) {
  EFUN_PRINTF(("\n\n************************\n"                                                                                                                         ));
  ESTM_PRINTF((   "** LANE %d CL72 Status **\n"                                                                                        , eagle_phy_1_get_lane()              ));
  EFUN_PRINTF((    "************************\n"                                                                                                                         ));
  ESTM_PRINTF((    "cl72_signal_detect         = %d   (1 = CL72 training FSM in SEND_DATA state;  0 = CL72 in training state)\n"       , rd_cl72_signal_detect()        ));
  ESTM_PRINTF((    "cl72_ieee_training_failure = %d   (1 = Training failure detected;             0 = Training failure not detected)\n", rd_cl72_ieee_training_failure()));
  ESTM_PRINTF((    "cl72_ieee_training_status  = %d   (1 = Start-up protocol in progress;         0 = Start-up protocol complete)\n"   , rd_cl72_ieee_training_status() ));
  ESTM_PRINTF((    "cl72_ieee_receiver_status  = %d   (1 = Receiver trained and ready to receive; 0 = Receiver training)\n\n"          , rd_cl72_ieee_receiver_status() ));
  return(ERR_CODE_NONE);
}


/**********************************/
/*  Serdes TX disable/RX Restart  */
/**********************************/

err_code_t eagle_phy_1_tx_disable(uint8_t enable) {

  if (enable) {
    EFUN(wr_sdk_tx_disable(0x1));
  }
  else {
    EFUN(wr_sdk_tx_disable(0x0));
  }
  return(ERR_CODE_NONE);
}


err_code_t eagle_phy_1_rx_restart(uint8_t enable) {

  EFUN(wr_rx_restart_pmd_hold(enable));
  return(ERR_CODE_NONE);
}


/******************************************************/
/*  Single function to set/get all RX AFE parameters  */
/******************************************************/

err_code_t eagle_phy_1_write_rx_afe(enum srds_rx_afe_settings_enum param, int8_t val) {
  /* Assumes the micro is not actively tuning */

    switch(param) {
    case RX_AFE_PF:
        return(_set_rx_pf_main(val));

    case RX_AFE_PF2:
        return(_set_rx_pf2(val));

    case RX_AFE_VGA:
      return(_set_rx_vga(val));

    case RX_AFE_DFE1:
        return(_set_rx_dfe1(val));

    case RX_AFE_DFE2:
        return(_set_rx_dfe2(val));

    case RX_AFE_DFE3:
        return(_set_rx_dfe3(val));

    case RX_AFE_DFE4:
        return(_set_rx_dfe4(val));

    case RX_AFE_DFE5:
        return(_set_rx_dfe5(val));
    default:
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
}


err_code_t eagle_phy_1_read_rx_afe(enum srds_rx_afe_settings_enum param, int8_t *val) {
  /* Assumes the micro is not actively tuning */
    if(!val) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    switch(param) {
    case RX_AFE_PF:
        EFUN(_get_rx_pf_main(val));
        break;
    case RX_AFE_PF2:
        EFUN(_get_rx_pf2(val));
        break;
    case RX_AFE_VGA:
        EFUN(_get_rx_vga(val));
      break;

    case RX_AFE_DFE1:
        EFUN(_get_rx_dfe1(val));
        break;
    case RX_AFE_DFE2:
        EFUN(_get_rx_dfe2(val));
        break;
    case RX_AFE_DFE3:
        EFUN(_get_rx_dfe3(val));
        break;
    case RX_AFE_DFE4:
        EFUN(_get_rx_dfe4(val));
        break;
    case RX_AFE_DFE5:
        EFUN(_get_rx_dfe5(val));
        break;
    default:
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    return(ERR_CODE_NONE);
}


err_code_t eagle_phy_1_validate_txfir_cfg(int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3) {
  err_code_t err_code = ERR_CODE_NONE;

  if ((pre   >  31) || (pre   <   0)) err_code |= ERR_CODE_TXFIR_PRE_INVALID;
  if ((main  > 112) || (main  <   0)) err_code |= ERR_CODE_TXFIR_MAIN_INVALID;
  if ((post1 >  63) || (post1 <   0)) err_code |= ERR_CODE_TXFIR_POST1_INVALID;
  if ((post2 >  15) || (post2 < -15)) err_code |= ERR_CODE_TXFIR_POST2_INVALID;
  if ((post3 >   7) || (post3 <  -7)) err_code |= ERR_CODE_TXFIR_POST3_INVALID;

  if ((int16_t)(main + 48) < (int16_t)(pre + post1 + post2 + post3 + 1))
    err_code |= ERR_CODE_TXFIR_V2_LIMIT;

  if ((int16_t)(pre + main + post1 + _abs(post2) + _abs(post3)) > 112)
    err_code |= ERR_CODE_TXFIR_SUM_LIMIT;

  return (_error(err_code));
}


err_code_t eagle_phy_1_write_tx_afe(enum srds_tx_afe_settings_enum param, int8_t val) {

  switch(param) {
    case TX_AFE_PRE:
        return(_set_tx_pre(val));
    case TX_AFE_MAIN:
        return(_set_tx_main(val));
    case TX_AFE_POST1:
        return(_set_tx_post1(val));
    case TX_AFE_POST2:
        return(_set_tx_post2(val));
    case TX_AFE_POST3:
        return(_set_tx_post3(val));
    case TX_AFE_AMP:
        return(_set_tx_amp(val));
    case TX_AFE_DRIVERMODE:
        if(val == DM_NOT_SUPPORTED || val > DM_HALF_AMPLITUDE_HI_IMPED) {
            return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        } else {
            return(wr_ams_tx_drivermode(val));
        }
    default:
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
}

err_code_t eagle_phy_1_read_tx_afe(enum srds_tx_afe_settings_enum param, int8_t *val) {
    if(!val) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    switch(param) {
    case TX_AFE_PRE:
        EFUN(_get_tx_pre(val));
        break;
    case TX_AFE_MAIN:
        EFUN(_get_tx_main(val));
        break;
    case TX_AFE_POST1:
        EFUN(_get_tx_post1(val));
        break;
    case TX_AFE_POST2:
        EFUN(_get_tx_post2(val));
        break;
    case TX_AFE_POST3:
        EFUN(_get_tx_post3(val));
        break;
    case TX_AFE_AMP:
        EFUN(_get_tx_amp(val));
        break;
    case TX_AFE_DRIVERMODE: 
        EFUN(_get_tx_driver_mode(val)); 
        break; 
    default:
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    return(ERR_CODE_NONE);
}

err_code_t eagle_phy_1_apply_txfir_cfg(int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3) {

  err_code_t failcode = eagle_phy_1_validate_txfir_cfg(pre, main, post1, post2, post3);

  if (!failcode) {
    failcode |= _set_tx_pre(pre);
    failcode |= _set_tx_main(main);
    failcode |= _set_tx_post1(post1);
    failcode |= _set_tx_post2(post2);
    failcode |= _set_tx_post3(post3);
  }

  return (_error(failcode));
}





/**************************************/
/*  PRBS Generator/Checker Functions  */
/**************************************/

/* Configure PRBS Generator */
err_code_t eagle_phy_1_config_tx_prbs(enum srds_prbs_polynomial_enum prbs_poly_mode, uint8_t prbs_inv) {

  EFUN(wr_prbs_gen_mode_sel((uint8_t)prbs_poly_mode));  /* PRBS Generator mode sel */
  EFUN(wr_prbs_gen_inv(prbs_inv));                      /* PRBS Invert Enable/Disable */
  /* To enable PRBS Generator */
  /* EFUN(wr_prbs_gen_en(0x1)); */
  return (ERR_CODE_NONE);
}

err_code_t eagle_phy_1_get_tx_prbs_config(enum srds_prbs_polynomial_enum *prbs_poly_mode, uint8_t *prbs_inv) {
    uint8_t val;

  ESTM(val = rd_prbs_gen_mode_sel());                   /* PRBS Generator mode sel */
  *prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;
  ESTM(val = rd_prbs_gen_inv());                        /* PRBS Invert Enable/Disable */
  *prbs_inv = val;

  return (ERR_CODE_NONE);
}

/* PRBS Generator Enable/Disable */
err_code_t eagle_phy_1_tx_prbs_en(uint8_t enable) {

  if (enable) {
    EFUN(wr_prbs_gen_en(0x1));                          /* Enable PRBS Generator */
  }
  else {
    EFUN(wr_prbs_gen_en(0x0));                          /* Disable PRBS Generator */
  }
  return (ERR_CODE_NONE);
}

/* Get PRBS Generator Enable/Disable */
err_code_t eagle_phy_1_get_tx_prbs_en(uint8_t *enable) {

  ESTM(*enable = rd_prbs_gen_en());

  return (ERR_CODE_NONE);
}

/* PRBS 1-bit error injection */
err_code_t eagle_phy_1_tx_prbs_err_inject(uint8_t enable) {
  /* PRBS Error Insert.
     0 to 1 transition on this signal will insert single bit error in the MSB bit of the data bus.
     Reset value is 0x0.
  */
  if(enable)
    EFUN(wr_prbs_gen_err_ins(0x1));
  EFUN(wr_prbs_gen_err_ins(0));
  return (ERR_CODE_NONE);
}

/* Configure PRBS Checker */
err_code_t eagle_phy_1_config_rx_prbs(enum srds_prbs_polynomial_enum prbs_poly_mode, enum srds_prbs_checker_mode_enum prbs_checker_mode, uint8_t prbs_inv) {

  EFUN(wr_prbs_chk_mode_sel((uint8_t)prbs_poly_mode)); /* PRBS Checker Polynomial mode sel  */
  EFUN(wr_prbs_chk_mode((uint8_t)prbs_checker_mode));  /* PRBS Checker mode sel (PRBS LOCK state machine select) */
  EFUN(wr_prbs_chk_inv(prbs_inv));                     /* PRBS Invert Enable/Disable */
  /* To enable PRBS Checker */
  /* EFUN(wr_prbs_chk_en(0x1)); */
  return (ERR_CODE_NONE);
}

/* get PRBS Checker */
err_code_t eagle_phy_1_get_rx_prbs_config(enum srds_prbs_polynomial_enum *prbs_poly_mode, enum srds_prbs_checker_mode_enum *prbs_checker_mode, uint8_t *prbs_inv) {
  uint8_t val;

  ESTM(val = rd_prbs_chk_mode_sel());                 /* PRBS Checker Polynomial mode sel  */
  *prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;
  ESTM(val = rd_prbs_chk_mode());                     /* PRBS Checker mode sel (PRBS LOCK state machine select) */
  *prbs_checker_mode = (enum srds_prbs_checker_mode_enum)val;
  ESTM(val = rd_prbs_chk_inv());                      /* PRBS Invert Enable/Disable */
  *prbs_inv = val;
  return (ERR_CODE_NONE);
}

/* PRBS Checker Enable/Disable */
err_code_t eagle_phy_1_rx_prbs_en(uint8_t enable) {

  if (enable) {
    EFUN(wr_prbs_chk_en(0x1));                          /* Enable PRBS Checker */
  }
  else {
    EFUN(wr_prbs_chk_en(0x0));                          /* Disable PRBS Checker */
  }
  return (ERR_CODE_NONE);
}

err_code_t eagle_phy_1_get_rx_prbs_en(uint8_t *enable) {

  ESTM(*enable = rd_prbs_chk_en());

  return (ERR_CODE_NONE);
}


/* PRBS Checker Lock State */
err_code_t eagle_phy_1_prbs_chk_lock_state(uint8_t *chk_lock) {
    if(!chk_lock) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    ESTM(*chk_lock = rd_prbs_chk_lock());                  /* PRBS Checker Lock Indication 1 = Locked, 0 = Out of Lock */
    return (ERR_CODE_NONE);
}

/* PRBS Error Count and Lock Lost (bit 31 in lock lost) */
err_code_t eagle_phy_1_prbs_err_count_ll(uint32_t *prbs_err_cnt) {
    uint16_t rddata;

    if(!prbs_err_cnt) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    EFUN(eagle_phy_1_pmd_rdt_reg(TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS, &rddata));
    *prbs_err_cnt = ((uint32_t) rddata)<<16;
    ESTM(*prbs_err_cnt = (*prbs_err_cnt | rd_prbs_chk_err_cnt_lsb()));
    return (ERR_CODE_NONE);
}

/* PRBS Error Count State  */
err_code_t eagle_phy_1_prbs_err_count_state(uint32_t *prbs_err_cnt, uint8_t *lock_lost) {
    if(!prbs_err_cnt || !lock_lost) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    EFUN(eagle_phy_1_prbs_err_count_ll(prbs_err_cnt));
    *lock_lost    = (*prbs_err_cnt >> 31);
    *prbs_err_cnt = (*prbs_err_cnt & 0x7FFFFFFF);
    return (ERR_CODE_NONE);
}

/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/

/* Enable/Diasble Digital Loopback */
err_code_t eagle_phy_1_dig_lpbk(uint8_t enable) {

  EFUN(wr_dig_lpbk_en(enable));                         /* 0 = diabled, 1 = enabled */
  return (ERR_CODE_NONE);
}



/* Locks TX_PI to Loop timing */
err_code_t eagle_phy_1_loop_timing(uint8_t enable) {

    if (enable) {
        EFUN(wr_tx_pi_loop_timing_src_sel(0x1)); /* RX phase_sum_val_logic enable */
        EFUN(wrc_tx_pi_en(0x1));                 /* TX_PI enable: 0 = diabled, 1 = enabled */
        EFUN(wrc_tx_pi_jitter_filter_en(0x1));   /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */
        EFUN(eagle_phy_1_delay_us(25));               /* Wait for tclk to lock to CDR */
    } else {
        EFUN(wrc_tx_pi_jitter_filter_en(0x0));   /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */
        EFUN(wrc_tx_pi_en(0x0));                 /* TX_PI enable: 0 = diabled, 1 = enabled */
        EFUN(wr_tx_pi_loop_timing_src_sel(0x0)); /* RX phase_sum_val_logic enable */
    }
    return (ERR_CODE_NONE);
}


/* Setup Remote Loopback mode  */
err_code_t eagle_phy_1_rmt_lpbk(uint8_t enable) {
    if (enable) {
        EFUN(eagle_phy_1_loop_timing(enable));
        EFUN(wrc_tx_pi_ext_ctrl_en(0x1)); /* PD path enable: 0 = diabled, 1 = enabled */
        EFUN(wr_rmt_lpbk_en(0x1));        /* Remote Loopback Enable: 0 = diabled, 1 = enable  */
        EFUN(eagle_phy_1_delay_us(50));        /* Wait for rclk and tclk phase lock before expecing good data from rmt loopback */
    } else {                              /* Might require longer wait time for smaller values of tx_pi_ext_phase_bwsel_integ */
        EFUN(wr_rmt_lpbk_en(0x0));        /* Remote Loopback Enable: 0 = diabled, 1 = enable  */
        EFUN(wrc_tx_pi_ext_ctrl_en(0x0)); /* PD path enable: 0 = diabled, 1 = enabled */
        EFUN(eagle_phy_1_loop_timing(enable));
    }
    return (ERR_CODE_NONE);
}





/**********************************/
/*  TX_PI Jitter Generation APIs  */
/**********************************/

/* TX_PI Frequency Override (Fixed Frequency Mode) */
err_code_t eagle_phy_1_tx_pi_freq_override(uint8_t enable, int16_t freq_override_val) {
   if (enable) {
        EFUN(wrc_tx_pi_en(0x1));                              /* TX_PI enable :            0 = disabled, 1 = enabled */
        EFUN(wrc_tx_pi_freq_override_en(0x1));                /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
        EFUN(wrc_tx_pi_freq_override_val(freq_override_val)); /* Fixed Freq Override Value (+/-8192) */
   }
   else {
        EFUN(wrc_tx_pi_freq_override_val(0));                 /* Fixed Freq Override Value to 0 */
        EFUN(wrc_tx_pi_freq_override_en(0x0));                /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
        EFUN(wrc_tx_pi_en(0x0));                              /* TX_PI enable :            0 = disabled, 1 = enabled */
   }
  return (ERR_CODE_NONE);
}


/* TX_PI Sinusoidal or Spread-Spectrum (SSC) Jitter Generation  */
err_code_t eagle_phy_1_tx_pi_jitt_gen(uint8_t enable, int16_t freq_override_val, enum srds_tx_pi_freq_jit_gen_enum jit_type, uint8_t tx_pi_jit_freq_idx, uint8_t tx_pi_jit_amp) {
    /* Added a limiting for the jitter amplitude index, per freq_idx */
    uint8_t max_amp_idx_r20_os1[] = {37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 48, 33, 47, 37, 33, 37, 37};

    /* Irrespective of the osr_mode, txpi runs @ os1. Thus the max amp idx values remain the same. */
    if (jit_type == TX_PI_SJ) {
        if (tx_pi_jit_amp > max_amp_idx_r20_os1[tx_pi_jit_freq_idx]) {
            tx_pi_jit_amp = max_amp_idx_r20_os1[tx_pi_jit_freq_idx];
        }
    }

    EFUN(eagle_phy_1_tx_pi_freq_override(enable, freq_override_val));

    if (enable) {
        EFUN(wrc_tx_pi_jit_freq_idx(tx_pi_jit_freq_idx));
        EFUN(wrc_tx_pi_jit_amp(tx_pi_jit_amp));

        if (jit_type == TX_PI_SSC_HIGH_FREQ) {
            EFUN(wrc_tx_pi_jit_ssc_freq_mode(0x1));       /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
            EFUN(wrc_tx_pi_ssc_gen_en(0x1));              /* SSC jitter enable:         0 = disabled,    1 = enabled */
        }
        else if (jit_type == TX_PI_SSC_LOW_FREQ) {
            EFUN(wrc_tx_pi_jit_ssc_freq_mode(0x0));       /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
            EFUN(wrc_tx_pi_ssc_gen_en(0x1));              /* SSC jitter enable:         0 = disabled,    1 = enabled */
        }
        else if (jit_type == TX_PI_SJ) {
            EFUN(wrc_tx_pi_sj_gen_en(0x1));               /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
        }
    }
    else {
        EFUN(wrc_tx_pi_ssc_gen_en(0x0));                  /* SSC jitter enable:         0 = disabled,    1 = enabled */
        EFUN(wrc_tx_pi_sj_gen_en(0x0));                   /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
    }
  return (ERR_CODE_NONE);
}


/***************************/
/*  Configure Serdes IDDQ  */
/***************************/

err_code_t eagle_phy_1_core_config_for_iddq(void) {

  /* Use frc/frc_val to force TX clk_vld signals to 0 */
  EFUN(wrc_pmd_tx_clk_vld_frc_val(0x0));
  EFUN(wrc_pmd_tx_clk_vld_frc(0x1));

  /* Switch all the lane clocks to comclk by writing to RX/TX comclk_sel registers */
  EFUN(wrc_tx_s_comclk_sel(0x1));
  return (ERR_CODE_NONE);
}


err_code_t eagle_phy_1_lane_config_for_iddq(void) {

  /* Use frc/frc_val to force all RX and TX clk_vld signals to 0 */
  EFUN(wr_pmd_rx_clk_vld_frc_val(0x0));
  EFUN(wr_pmd_rx_clk_vld_frc(0x1));


  /* Use frc/frc_val to force all pmd_rx_lock signals to 0 */
  EFUN(wr_rx_dsc_lock_frc_val(0x0));
  EFUN(wr_rx_dsc_lock_frc(0x1));

  /* Switch all the lane clocks to comclk by writing to RX/TX comclk_sel registers */
  EFUN(wr_ln_rx_s_comclk_sel(0x1));

  /* Assert all the AFE pwrdn/reset pins using frc/frc_val to make sure AFE is in lowest possible power mode */
  EFUN(wr_afe_tx_pwrdn_frc_val(0x1));
  EFUN(wr_afe_tx_pwrdn_frc(0x1));
  EFUN(wr_afe_rx_pwrdn_frc_val(0x1));
  EFUN(wr_afe_rx_pwrdn_frc(0x1));
  EFUN(wr_afe_tx_reset_frc_val(0x1));
  EFUN(wr_afe_tx_reset_frc(0x1));
  EFUN(wr_afe_rx_reset_frc_val(0x1));
  EFUN(wr_afe_rx_reset_frc(0x1));

  /* Set pmd_iddq pin to enable IDDQ */
  return (ERR_CODE_NONE);
}

/*********************************/
/*  uc_active and uc_reset APIs  */
/*********************************/

/* Set the uC active mode */
err_code_t eagle_phy_1_uc_active_enable(uint8_t enable) {
  EFUN(wrc_uc_active(enable));
  return (ERR_CODE_NONE);
}


/* Enable or Disable the uC reset */
err_code_t eagle_phy_1_uc_reset(uint8_t enable) {
  if (enable) {
    /* Assert micro reset and reset all micro registers (all non-status registers written to default value) */
    EFUN(eagle_phy_1_pmd_wr_reg(0xD200, 0x0000));
    EFUN(eagle_phy_1_pmd_wr_reg(0xD201, 0x0000));
    EFUN(eagle_phy_1_pmd_wr_reg(0xD202, 0x0000));
    EFUN(eagle_phy_1_pmd_wr_reg(0xD203, 0x0000));
    EFUN(eagle_phy_1_pmd_wr_reg(0xD207, 0x0000));
    EFUN(eagle_phy_1_pmd_wr_reg(0xD208, 0x0000));
    EFUN(eagle_phy_1_pmd_wr_reg(0xD20A, 0x080f));
    EFUN(eagle_phy_1_pmd_wr_reg(0xD20C, 0x0002));
    EFUN(eagle_phy_1_pmd_wr_reg(0xD20D, 0x0000));
  }
  else {
    /* De-assert micro reset - Start executing code */
      EFUN(wrc_micro_system_clk_en(0x1));                   /* Enable clock to micro  */
      EFUN(wrc_micro_system_reset_n(0x1));                  /* De-assert reset to micro */
      EFUN(wrc_micro_mdio_dw8051_reset_n(0x1));
  }
 return (ERR_CODE_NONE);
}

/******************/
/*  Lane Mapping  */
/******************/

err_code_t eagle_phy_1_map_lanes(uint8_t num_lanes, uint8_t const *tx_lane_map, uint8_t const *rx_lane_map) {
    uint8_t rd_val = 0;

    /* Verify that the core data path is held in reset. */
    ESTM(rd_val = rdc_uc_active());
    if (rd_val != 0) {
        return (ERR_CODE_UC_ACTIVE);
    }

    /* Verify that the num_lanes parameter is correct. */
    ESTM(rd_val = rdc_revid_multiplicity());
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

#if defined(wrc_tx_lane_addr_0) || defined(wrc_tx_lane_map_0)

            /* Not able to configure lane mapping.  So verify that it's the default mapping. */
            if ((tx_lane_map[index1] != index1)
                || (rx_lane_map[index1] != index1)) {
                return (ERR_CODE_BAD_LANE);
            }

#else /* defined(wrc_tx_lane_addr_0) || defined(wrc_tx_lane_map_0)*/

            /* Verify that a lane map is not to an invalid lane. */
            if ((tx_lane_map[index1] >= num_lanes)
                || (rx_lane_map[index1] >= num_lanes)){
                return (ERR_CODE_BAD_LANE);
            }

            /* For this core, TX and RX lane mapping must be the same. */
            if (tx_lane_map[index1] != rx_lane_map[index1]) {
                return (ERR_CODE_CONFLICTING_PARAMETERS);
            }

            /* Verify that a lane map is not used twice. */
            for (index2=index1+1; index2<num_lanes; ++index2) {
                if ((tx_lane_map[index1] == tx_lane_map[index2])
                    || (rx_lane_map[index1] == rx_lane_map[index2])) {
                    return (ERR_CODE_BAD_LANE);
                }
            }

#endif /* defined(wrc_tx_lane_addr_0) || defined(wrc_tx_lane_map_0)*/

        }
    }

    /* Write the map bitfields.
     * Support up to 4 lanes.
     */
#if defined(wrc_tx_lane_addr_0)
   EFUN(wrc_tx_lane_addr_0(*(tx_lane_map++)));
#elif defined(wrc_tx_lane_map_0)
   EFUN(wrc_tx_lane_map_0(*(tx_lane_map++)));
#endif
#if defined(wrc_tx_lane_addr_1)
   EFUN(wrc_tx_lane_addr_1(*(tx_lane_map++)));
#elif defined(wrc_tx_lane_map_1)
   EFUN(wrc_tx_lane_map_1(*(tx_lane_map++)));
#endif
#if defined(wrc_tx_lane_addr_2)
   EFUN(wrc_tx_lane_addr_2(*(tx_lane_map++)));
#elif defined(wrc_tx_lane_map_2)
   EFUN(wrc_tx_lane_map_2(*(tx_lane_map++)));
#endif
#if defined(wrc_tx_lane_addr_3)
   EFUN(wrc_tx_lane_addr_3(*(tx_lane_map++)));
#elif defined(wrc_tx_lane_map_3)
   EFUN(wrc_tx_lane_map_3(*(tx_lane_map++)));
#endif

#if defined(wrc_rx_lane_addr_0)
   EFUN(wrc_rx_lane_addr_0(*(rx_lane_map++)));
#elif defined(wrc_rx_lane_map_0)
   EFUN(wrc_rx_lane_map_0(*(rx_lane_map++)));
#endif
#if defined(wrc_rx_lane_addr_1)
   EFUN(wrc_rx_lane_addr_1(*(rx_lane_map++)));
#elif defined(wrc_rx_lane_map_1)
   EFUN(wrc_rx_lane_map_1(*(rx_lane_map++)));
#endif
#if defined(wrc_rx_lane_addr_2)
   EFUN(wrc_rx_lane_addr_2(*(rx_lane_map++)));
#elif defined(wrc_rx_lane_map_2)
   EFUN(wrc_rx_lane_map_2(*(rx_lane_map++)));
#endif
#if defined(wrc_rx_lane_addr_3)
   EFUN(wrc_rx_lane_addr_3(*(rx_lane_map++)));
#elif defined(wrc_rx_lane_map_3)
   EFUN(wrc_rx_lane_map_3(*(rx_lane_map++)));
#endif

    return (ERR_CODE_NONE);
}

/****************************************************/
/*  Serdes Powerdown, ClockGate and Deep_Powerdown  */
/****************************************************/

err_code_t eagle_phy_1_core_pwrdn(enum srds_core_pwrdn_mode_enum mode) {
    switch(mode) {
    case PWR_ON:
        EFUN(_eagle_phy_1_core_clkgate(0));
        EFUN(wrc_afe_s_pll_pwrdn(0x0));
        EFUN(wrc_core_dp_s_rstb(0x1));
        break;
    case PWRDN:
        EFUN(wrc_core_dp_s_rstb(0x0));
        EFUN(eagle_phy_1_delay_ns(   500)); /* wait >50 comclk cycles  */
        EFUN(wrc_afe_s_pll_pwrdn(0x1));
        break;
    case PWRDN_DEEP:
        EFUN(wrc_core_dp_s_rstb(0x0));
        EFUN(eagle_phy_1_delay_ns(   500)); /* wait >50 comclk cycles  */
                EFUN(wrc_afe_s_pll_pwrdn(0x1));
        EFUN(_eagle_phy_1_core_clkgate(1));
        break;
    default:
        EFUN(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;
    }
    return ERR_CODE_NONE;
}

err_code_t eagle_phy_1_lane_pwrdn(enum srds_core_pwrdn_mode_enum mode) {
    switch(mode) {
    case PWR_ON:
        EFUN(wr_ln_tx_s_pwrdn(0x0));
        EFUN(wr_ln_rx_s_pwrdn(0x0));
        EFUN(_eagle_phy_1_lane_clkgate(0));
        break;
    case PWRDN:
        /* do the RX first, since that is what most users care about */
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        break;
    case PWRDN_DEEP:
        /* do the RX first, since that is what most users care about */
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        EFUN(_eagle_phy_1_lane_clkgate(1));
        EFUN(wr_ln_dp_s_rstb(0x0));
        break;
    case PWRDN_TX:
        EFUN(wr_ln_tx_s_pwrdn(0x1));
        break;
    case PWRDN_RX:
        EFUN(wr_ln_rx_s_pwrdn(0x1));
        break;
    default :
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;
    }
  return (ERR_CODE_NONE);
}


/*******************************/
/*  Isolate Serdes Input Pins  */
/*******************************/

err_code_t eagle_phy_1_isolate_ctrl_pins(uint8_t enable) {

    EFUN(eagle_phy_1_isolate_lane_ctrl_pins(enable));
    EFUN(eagle_phy_1_isolate_core_ctrl_pins(enable));

  return (ERR_CODE_NONE);
}

err_code_t eagle_phy_1_isolate_lane_ctrl_pins(uint8_t enable) {

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

err_code_t eagle_phy_1_isolate_core_ctrl_pins(uint8_t enable) {

  if (enable) {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x1));
    EFUN(wrc_pmd_mdio_trans_pkill(0x1));
  }
  else {
    EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x0));
    EFUN(wrc_pmd_mdio_trans_pkill(0x0));
  }
  return (ERR_CODE_NONE);
}




/***********************************************/
/*  Microcode Load into Program RAM Functions  */
/***********************************************/


/* uCode Load through Register (MDIO) Interface [Return Val = Error_Code (0 = PASS)] */
err_code_t eagle_phy_1_ucode_mdio_load(uint8_t *ucode_image, uint16_t ucode_len) {

    uint16_t ucode_len_padded, wr_data, count = 0;
    uint8_t  wrdata_lsb;
    uint8_t  result;

    if(!ucode_image) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if (ucode_len > UCODE_MAX_SIZE) {                     /* uCode size should be less than UCODE_MAX_SIZE  */
        return (_error(ERR_CODE_INVALID_UCODE_LEN));
    }
    EFUN(wrc_micro_mdio_dw8051_reset_n(0x0));
    EFUN(wrc_micro_system_clk_en(0x1));                   /* Enable clock to micro  */
    EFUN(wrc_micro_system_reset_n(0x1));                  /* De-assert reset to micro */
    EFUN(wrc_micro_system_reset_n(0x0));                  /* Assert reset to micro - Toggling micro reset*/
    EFUN(wrc_micro_system_reset_n(0x1));                  /* De-assert reset to micro */

    EFUN(wrc_micro_mdio_ram_access_mode(0x0));            /* Select Program Memory access mode - Program RAM load when Serdes DW8051 in reset */
    EFUN(wrc_micro_byte_mode(0));

    EFUN(wrc_micro_ram_address(0x0));                     /* RAM start address */

    EFUN(wrc_micro_init_cmd(0x0));                        /* Clear initialization command */
    EFUN(wrc_micro_init_cmd(0x1));                        /* Set initialization command */
    EFUN(wrc_micro_init_cmd(0x0));                        /* Clear initialization command */

    EFUN(eagle_phy_1_delay_us(300));                           /* Wait for Initialization to finish */
#ifndef ATE_LOG
    ESTM(result = !rdc_micro_init_done());
    if (result) {                                         /* Check if initialization done within 300us time interval */
        return (_error(ERR_CODE_MICRO_INIT_NOT_DONE));    /* Else issue error code */
    }
#endif
    ucode_len_padded = ((ucode_len + 7) & 0xFFF8);        /* Aligning ucode size to 8-byte boundary */

    /* Code to Load microcode */
    EFUN(wrc_micro_ram_count(ucode_len_padded - 1));      /* Set number of bytes of ucode to be written */
    EFUN(wrc_micro_ram_address(0x0));                     /* Start address of Program RAM where the ucode is to be loaded */
    EFUN(wrc_micro_stop(0x0));                            /* Stop Write to Program RAM */
    EFUN(wrc_micro_write(0x1));                           /* Enable Write to Program RAM  */
    EFUN(wrc_micro_run(0x1));                             /* Start Write to Program RAM */
    count = 0;
    do {                                                  /* ucode_image loaded 16bits at a time */
        wrdata_lsb = (count < ucode_len) ? ucode_image[count] : 0x0; /* wrdata_lsb read from ucode_image; zero padded to 8byte boundary */
        count++;
        wr_data    = (count < ucode_len) ? ucode_image[count] : 0x0; /* wrdata_msb read from ucode_image; zero padded to 8byte boundary */
        count++;
        wr_data = ((wr_data << 8) | wrdata_lsb);                     /* 16bit wr_data formed from 8bit msb and lsb values read from ucode_image */
        EFUN(wrc_micro_ram_wrdata(wr_data));                         /* Program RAM write data */
    }   while (count < ucode_len_padded);                 /* Loop repeated till entire image loaded (upto the 8byte boundary) */

    EFUN(wrc_micro_write(0x0));                           /* Clear Write enable to Program RAM  */
    EFUN(wrc_micro_run(0x0));                             /* Clear RUN command to Program RAM */
    EFUN(wrc_micro_stop(0x1));                            /* Stop Write to Program RAM */

#ifndef ATE_LOG
    /* Verify Microcode load */
    ESTM(result = (rdc_micro_err0() | (rdc_micro_err1()<<1)));
    if (result > 0) {            /* Look for errors in micro load FSM */
        EFUN_PRINTF(("download status =%x\n",result));
        EFUN(wrc_micro_stop(0x0));                        /* Clear stop field */
        return (_error(ERR_CODE_UCODE_LOAD_FAIL));        /* Return 1 : ERROR while loading microcode (uCode Load FAIL) */
    }
    else {
        EFUN(wrc_micro_stop(0x0));                        /* Clear stop field */
     /* EFUN(wrc_micro_mdio_dw8051_reset_n(0x1)); */      /* De-assert reset to Serdes DW8051 to start executing microcode */
    }
#else
        EFUN(wrc_micro_stop(0x0));                        /* Clear stop field */
#endif

    return (ERR_CODE_NONE);                               /* NO Errors while loading microcode (uCode Load PASS) */
}


/* Read-back uCode from Program RAM and verify against ucode_image [Return Val = Error_Code (0 = PASS)]  */
err_code_t eagle_phy_1_ucode_load_verify(uint8_t *ucode_image, uint16_t ucode_len) {
    uint16_t ucode_len_padded, rddata, ram_data, count = 0;
    uint8_t  rdata_lsb;

    if(!ucode_image) {
      return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    ucode_len_padded = ((ucode_len + 7) & 0xFFF8);        /* Aligning ucode size to 8-byte boundary */

    if (ucode_len_padded > UCODE_MAX_SIZE) {              /* uCode size should be less than UCODE_MAX_SIZE */
      return (_error(ERR_CODE_INVALID_UCODE_LEN));
    }

    EFUN(wrc_micro_byte_mode(0x0));                     /* Select Word access mode */
    EFUN(wrc_micro_mdio_ram_access_mode(0x1));          /* Select Program RAM access through MDIO Register interface mode */
    EFUN(wrc_micro_mdio_ram_read_autoinc_en(1));


    EFUN(wrc_micro_ram_address(0x0));                     /* Start address of Program RAM from where to read ucode */

    do {                                                  /* ucode_image read 16bits at a time */
        rdata_lsb = (count < ucode_len) ? ucode_image[count] : 0x0; /* rdata_lsb read from ucode_image; zero padded to 8byte boundary */
        count++;
        rddata    = (count < ucode_len) ? ucode_image[count] : 0x0; /* rdata_msb read from ucode_image; zero padded to 8byte boundary */
        count++;
        rddata = ((rddata << 8) | rdata_lsb);                       /* 16bit rddata formed from 8bit msb and lsb values read from ucode_image */
        ESTM(ram_data = rdc_micro_ram_rddata());
        if (ram_data != rddata) {                                   /* Compare Program RAM ucode to ucode_image (Read to ram_rddata reg auto-increments the ram_address) */
            EFUN_PRINTF(("Ucode_Load_Verify_FAIL: Addr = 0x%x : Read_data = 0x%x : Expected_data = 0x%x\n",(count-2),ram_data,rddata));
            return (_error(ERR_CODE_UCODE_VERIFY_FAIL));            /* Verify uCode FAIL */
        }
    }   while (count < ucode_len_padded);                 /* Loop repeated till entire image loaded (upto the 8byte boundary) */
    EFUN(wrc_micro_mdio_ram_read_autoinc_en(0));
    EFUN(wrc_micro_mdio_ram_access_mode(0x2));          /* Select Data RAM access through MDIO Register interface mode */

    return (ERR_CODE_NONE);                               /* Verify uCode PASS */
}


/*************************************************/
/*  RAM access through Micro Register Interface  */
/*************************************************/



/* Micro RAM Word Write */
err_code_t eagle_phy_1_wrw_uc_ram(uint16_t addr, uint16_t wr_val) {
    wr_val = (((wr_val & 0xFF) << 8) | (wr_val >> 8));  /* Swapping upper byte and lower byte to compensate for endianness in Serdes 8051 */

    EFUN(wrc_micro_mdio_ram_access_mode(0x2));          /* Select Data RAM access through MDIO Register interface mode */
    EFUN(wrc_micro_byte_mode(0x0));                     /* Select Word access mode */
    EFUN(wrc_micro_ram_address(addr));                  /* RAM Address to be written to */
    EFUN(eagle_phy_1_delay_ns(80));                          /* wait for 10 comclk cycles/ 80ns  */
    EFUN(wrc_micro_ram_wrdata(wr_val));                 /* RAM Write value */
    EFUN(eagle_phy_1_delay_ns(80));                          /* Wait for Data RAM to be written */
    return (ERR_CODE_NONE);
}

/* Micro RAM Byte Write */
err_code_t eagle_phy_1_wrb_uc_ram(uint16_t addr, uint8_t wr_val) {
    EFUN(wrc_micro_mdio_ram_access_mode(0x2));          /* Select Data RAM access through MDIO Register interface mode */
    EFUN(wrc_micro_byte_mode(0x1));                     /* Select Byte access mode */
    EFUN(wrc_micro_ram_address(addr));                  /* RAM Address to be written to */
    EFUN(eagle_phy_1_delay_ns(80));                          /* wait for 10 comclk cycles/ 80ns  */
    EFUN(wrc_micro_ram_wrdata(wr_val));                 /* RAM Write value */
    EFUN(eagle_phy_1_delay_ns(80));                          /* Wait for Data RAM to be written */
    return (ERR_CODE_NONE);
}

/* Micro RAM Word Read */
uint16_t eagle_phy_1_rdw_uc_ram(err_code_t *err_code_p, uint16_t addr) {
    uint16_t rddata;

    if(!err_code_p) {
        return(0);
    }
    *err_code_p = ERR_CODE_NONE;
    EPFUN(wrc_micro_mdio_ram_access_mode(0x2));         /* Select Data RAM access through MDIO Register interface mode */
    EPFUN(wrc_micro_byte_mode(0x0));                    /* Select Word access mode */
    EPFUN(wrc_micro_ram_address(addr));                 /* RAM Address to be read from */
    EPFUN(eagle_phy_1_delay_ns(80));                         /* Wait for the RAM read  */
    EPSTM(rddata = rdc_micro_ram_rddata());             /* Value read from the required Data RAM Address */
    rddata = (((rddata & 0xFF) << 8) | (rddata >> 8));  /* Swapping upper byte and lower byte to compensate for endianness in Serdes 8051 */
    return (rddata);
}

/* Micro RAM Byte Read */
uint8_t eagle_phy_1_rdb_uc_ram(err_code_t *err_code_p, uint16_t addr) {
    uint8_t rddata;

    if(!err_code_p) {
        return(0);
    }
    *err_code_p = ERR_CODE_NONE;
    EPFUN(wrc_micro_mdio_ram_access_mode(0x2));         /* Select Data RAM access through MDIO Register interface mode */
    EPFUN(wrc_micro_byte_mode(0x1));                    /* Select Byte access mode */
    EPFUN(wrc_micro_ram_address(addr));                 /* RAM Address to be read from */
    EPFUN(eagle_phy_1_delay_ns(80));                         /* Wait for the RAM read  */
    EPSTM(rddata = (uint8_t) rdc_micro_ram_rddata());   /* Value read from the required Data RAM Address */
    return (rddata);
}


/* Micro RAM Word Signed Write */
err_code_t eagle_phy_1_wrws_uc_ram(uint16_t addr, int16_t wr_val) {
    return (eagle_phy_1_wrw_uc_ram(addr, wr_val));
}

/* Micro RAM Byte Signed Write */
err_code_t eagle_phy_1_wrbs_uc_ram(uint16_t addr, int8_t wr_val) {
    return (eagle_phy_1_wrb_uc_ram(addr, wr_val));
}

/* Micro RAM Word Signed Read */
int16_t eagle_phy_1_rdws_uc_ram(err_code_t *err_code_p, uint16_t addr) {
    return ((int16_t)eagle_phy_1_rdw_uc_ram(err_code_p, addr));
}

/* Micro RAM Byte Signed Read */
int8_t eagle_phy_1_rdbs_uc_ram(err_code_t *err_code_p, uint16_t addr) {
    return ((int8_t)eagle_phy_1_rdb_uc_ram(err_code_p, addr));
}


/* Micro RAM Byte Read */
err_code_t eagle_phy_1_rdblk_uc_ram(uint8_t *mem, uint16_t addr, uint16_t cnt) {
    if(!mem) {
        return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    while (cnt--) {
        err_code_t error_code;
        uint8_t    tmp = eagle_phy_1_rdb_uc_ram(&error_code, addr++); /* Value read from the required Data RAM Address */
        if (error_code) {
            return _error(error_code);
        }
        *mem++ = tmp;
    }
    return(ERR_CODE_NONE);
}



#if defined(SERDES_ENABLE_FAST_LINK_RECOVERY)
/************************/
/*  Fast Link Recovery  */
/************************/

err_code_t eagle_phy_1_configure_fast_link_recovery(uint8_t enable, uint8_t enable_timeout, enum eagle_phy_1_flr_timeout_enum timeout_time, uint8_t enable_2ms_yield, uint8_t filter_count) {

    uint8_t const timeout_time_val = (uint8_t)timeout_time;

#ifndef ATE_LOG
    uint8_t reset_state;
    ESTM(reset_state = rd_lane_dp_reset_state());
    if(reset_state < 7) {
        EFUN_PRINTF(("ERROR: eagle_phy_1_configure_fast_link_recovery(..) called without ln_dp_s_rstb=0 Lane=%d reset_state=%d\n",eagle_phy_1_get_lane(),reset_state));
        return (_error(ERR_CODE_LANE_DP_NOT_RESET));
    }
#endif

    if (timeout_time_val > 3) {
        ESTM_PRINTF(("timeout_time value for eagle_phy_1_configure_fast_link_recovery() is too large:  %d\n", timeout_time_val));
        return (_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    if (filter_count > 7) {
        ESTM_PRINTF(("filter_count value for eagle_phy_1_configure_fast_link_recovery() is too large:  %d\n", filter_count));
        return (_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    EFUN(wrv_usr_ctrl_fast_link_recovery_byte(  (enable ? 1 : 0)
                                              | (enable_timeout ? 2 : 0)
                                              | (timeout_time_val << 2)
                                              | (enable_2ms_yield ? 16 : 0)
                                              | (filter_count << 5)));
    /* DISP(rdv_usr_ctrl_fast_link_recovery_byte()); */
    return (ERR_CODE_NONE);
}
#endif /* defined(SERDES_ENABLE_FAST_LINK_RECOVERY) */
