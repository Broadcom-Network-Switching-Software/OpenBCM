/**************************************************************************************
 **************************************************************************************
 *  File Name     :  falcon_furia_dv_functions.c                                        *
 *  Created On    :  22/05/2014                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  APIs for Serdes IPs ported over from DV                          *
 *  Revision      :  $Id: falcon_furia_dv_functions.c 869 2015-01-23 00:24:44Z kirand $ *
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

/** @file falcon_furia_dv_functions.c
 * Implementation of API functions ported over from DV
 */

/******************************/
/*  TX Pattern Generator APIs */
/******************************/

/* Cofigure shared TX Pattern (Return Val = 0:PASS, 1-6:FAIL (reports 6 possible error scenarios if failed)) */
err_code_t falcon_furia_madura_config_shared_tx_pattern( const phymod_access_t *pa, uint8_t patt_length, const char pattern[]) {
   
  char       patt_final[245] = ""; 
  char       patt_mod[245]   = "", bin[5] = "";
  uint8_t    str_len, i, k, j = 0;
  uint8_t    offset_len, actual_patt_len = 0, hex = 0;
  uint8_t    zero_pad_len;
  uint16_t   patt_gen_wr_val = 0;
  uint8_t mode_sel; 

  err_code_t err_code;

  EFUN(_calc_patt_gen_mode_sel( pa, &mode_sel,&zero_pad_len,patt_length)); 

  /* Generating the appropriate write value to patt_gen_seq registers */
  str_len = (int8_t)USR_STRLEN(pattern);
 
  if ((str_len > 2) && ((USR_STRNCMP(pattern, "0x", 2)) == 0)) {
    /* Hexadecimal Pattern */
    for (i=2; i < str_len; i++) {
      err_code = _compute_bin( pa, pattern[i],bin);
      if (err_code) {
        return (err_code);
      }
      USR_STRNCAT(patt_mod,bin,4);
      if (pattern[i] != '_') {
        actual_patt_len = actual_patt_len + 4;
        if (actual_patt_len > 240) {
          USR_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
          return (_error(ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN));
        }
      }
    }
 
    offset_len = (actual_patt_len - patt_length);
    if ((offset_len > 3)  || (actual_patt_len < patt_length)) {
      USR_PRINTF(("ERROR: Pattern length provided does not match the hexadecimal pattern provided\n"));
      return (_error(ERR_CODE_CFG_PATT_LEN_MISMATCH));
    }  
    else if (offset_len) {
      for (i=0; i < offset_len; i++) {
        if (patt_mod[i] != '0') {
          USR_PRINTF(("ERROR: Pattern length provided does not match the hexadecimal pattern provided\n"));
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
        USR_STRNCAT(patt_mod,bin,4);
        actual_patt_len++;
        if (actual_patt_len > 240) {
          USR_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
          return (_error(ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN));
        }
      }
      else if (pattern[i] != '_') {
        USR_PRINTF(("ERROR: Invalid input Pattern\n"));
        return (_error(ERR_CODE_CFG_PATT_INVALID_PATTERN));
      } 
    } 
    
    if (actual_patt_len != patt_length) {
      USR_PRINTF(("ERROR: Pattern length provided does not match the binary pattern provided\n"));
      return (_error(ERR_CODE_CFG_PATT_LEN_MISMATCH));
    } 
  }

  /* Zero padding upper bits and concatinating patt_mod to form patt_final */
  for (i=0; i < zero_pad_len; i++) {
    USR_STRNCAT(patt_final,"0",2);
    j++; 
  }
  for (i=zero_pad_len; i + patt_length < 241; i = i + patt_length) {  
    USR_STRNCAT(patt_final,patt_mod,2);
    j++; 
  }
  
  /* USR_PRINTF(("\nFinal Pattern = %s\n\n",patt_final)); */

  for (i=0; i < 15; i++) {

    for (j=0; j < 4; j++) {
      k = i*16 + j*4;
      bin[0] = patt_final[k];
      bin[1] = patt_final[k+1];
      bin[2] = patt_final[k+2];
      bin[3] = patt_final[k+3];
      bin[4] = '\0';
      err_code = _compute_hex( pa, bin, &hex);
      if (err_code) {
        return (err_code);
      }
      patt_gen_wr_val = ((patt_gen_wr_val << 4) | hex); 
    }
    /* USR_PRINTF(("patt_gen_wr_val[%d] = 0x%x\n",(14-i),patt_gen_wr_val)); */

    /* Writing to apprpriate patt_gen_seq Registers */
    switch (i) {
      case 0:  wrc_falcon_furia_madura_patt_gen_seq_14(patt_gen_wr_val);
               break;
      case 1:  wrc_falcon_furia_madura_patt_gen_seq_13(patt_gen_wr_val);
               break;
      case 2:  wrc_falcon_furia_madura_patt_gen_seq_12(patt_gen_wr_val);
               break;
      case 3:  wrc_falcon_furia_madura_patt_gen_seq_11(patt_gen_wr_val);
               break;
      case 4:  wrc_falcon_furia_madura_patt_gen_seq_10(patt_gen_wr_val);
               break;
      case 5:  wrc_falcon_furia_madura_patt_gen_seq_9(patt_gen_wr_val);
               break;
      case 6:  wrc_falcon_furia_madura_patt_gen_seq_8(patt_gen_wr_val);
               break;
      case 7:  wrc_falcon_furia_madura_patt_gen_seq_7(patt_gen_wr_val);
               break;
      case 8:  wrc_falcon_furia_madura_patt_gen_seq_6(patt_gen_wr_val);
               break;
      case 9:  wrc_falcon_furia_madura_patt_gen_seq_5(patt_gen_wr_val);
               break;
      case 10: wrc_falcon_furia_madura_patt_gen_seq_4(patt_gen_wr_val);
               break;
      case 11: wrc_falcon_furia_madura_patt_gen_seq_3(patt_gen_wr_val);
               break;
      case 12: wrc_falcon_furia_madura_patt_gen_seq_2(patt_gen_wr_val);
               break;
      case 13: wrc_falcon_furia_madura_patt_gen_seq_1(patt_gen_wr_val);
               break;
      case 14: wrc_falcon_furia_madura_patt_gen_seq_0(patt_gen_wr_val);
               break;
      default: USR_PRINTF(("ERROR: Invalid write to patt_gen_seq register\n"));
               return (_error(ERR_CODE_CFG_PATT_INVALID_SEQ_WRITE));
               break;
    }
  }

  /* Pattern Generator Mode Select */
  /* wr_falcon_furia_madura_patt_gen_mode_sel(mode_sel); */
  /* USR_PRINTF(("Pattern gen Mode = %d\n",mode)); */
 
  /* Enable Fixed pattern Generation */
  /* wr_falcon_furia_madura_patt_gen_en(0x1);  */
  return(ERR_CODE_NONE); 
}


/* Enable/Disable Shared TX pattern generator */
err_code_t falcon_furia_madura_tx_shared_patt_gen_en( const phymod_access_t *pa, uint8_t enable, uint8_t patt_length) {
  uint8_t mode_sel,zero_pad_len;

  EFUN(_calc_patt_gen_mode_sel( pa, &mode_sel,&zero_pad_len,patt_length));

  if (enable) {
      if ((mode_sel < 1) || (mode_sel > 6)) {
        return (_error(ERR_CODE_PATT_GEN_INVALID_MODE_SEL));
      }
      mode_sel = (12 - mode_sel);                     
      wr_falcon_furia_madura_patt_gen_start_pos(mode_sel);                /* Start position for pattern */
      wr_falcon_furia_madura_patt_gen_stop_pos(0x0);                      /* Stop position for pattern */
    wr_falcon_furia_madura_patt_gen_en(0x1);                              /* Enable Fixed pattern Generation  */
  }
  else {
    wr_falcon_furia_madura_patt_gen_en(0x0);                              /* Disable Fixed pattern Generation  */
  }
  return(ERR_CODE_NONE); 
}

/**************************************/
/*  PMD_RX_LOCK and CL72/CL93 Status  */
/**************************************/

err_code_t falcon_furia_madura_pmd_lock_status( const phymod_access_t *pa, uint8_t *pmd_rx_lock) {
  if(!pmd_rx_lock) {
	  return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*pmd_rx_lock = rd_falcon_furia_madura_pmd_rx_lock());
  return (ERR_CODE_NONE);
}


err_code_t falcon_furia_madura_display_cl93n72_status( const phymod_access_t *pa ) {
 
  uint8_t rddata;
  USR_PRINTF(("\n\n***************************\n"));
  USR_PRINTF(("** LANE %d CL93n72 Status **\n",falcon_furia_madura_get_lane(pa)));
  USR_PRINTF(("***************************\n"));
  ESTM(rddata = rd_falcon_furia_madura_cl93n72_training_fsm_signal_detect());
  USR_PRINTF(("cl93n72_signal_detect         = %d   (1 = CL93n72 training FSM in SEND_DATA state;  0 = CL93n72 in training state)\n", rddata));

  ESTM(rddata = rd_falcon_furia_madura_cl93n72_ieee_training_failure());
  USR_PRINTF(("cl93n72_ieee_training_failure = %d   (1 = Training failure detected;                0 = Training failure not detected)\n", rddata));

  ESTM(rddata = rd_falcon_furia_madura_cl93n72_ieee_training_status());
  USR_PRINTF(("cl93n72_ieee_training_status  = %d   (1 = Start-up protocol in progress;            0 = Start-up protocol complete)\n", rddata));

  ESTM(rddata = rd_falcon_furia_madura_cl93n72_ieee_receiver_status());
  USR_PRINTF(("cl93n72_ieee_receiver_status  = %d   (1 = Receiver trained and ready to receive;    0 = Receiver training)\n\n", rddata));

  return(ERR_CODE_NONE); 
}



/**********************************/
/*  Serdes TX disable/RX Restart  */
/**********************************/

err_code_t falcon_furia_madura_tx_disable( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    wr_falcon_furia_madura_sdk_tx_disable(0x1);
  }
  else {
    wr_falcon_furia_madura_sdk_tx_disable(0x0);
  }
  return(ERR_CODE_NONE); 
}


err_code_t falcon_furia_madura_rx_restart( const phymod_access_t *pa, uint8_t enable) { 

  wr_falcon_furia_madura_rx_restart_pmd_hold(enable);
  return(ERR_CODE_NONE); 
}


/******************************************************/
/*  Single function to set/get all RX AFE parameters  */
/******************************************************/

err_code_t falcon_furia_madura_write_rx_afe( const phymod_access_t *pa, enum srds_rx_afe_settings_enum param, int8_t val) {
  /* Assumes the micro is not actively tuning */

  switch(param) {
    case RX_AFE_PF: 
    	return(_set_rx_pf_main( pa, val));	 
    
    case RX_AFE_PF2:
    	return(_set_rx_pf2( pa, val));
    
    case RX_AFE_VGA:
      return(_set_rx_vga( pa, val)); 
    
    case RX_AFE_DFE1:
    	return(_set_rx_dfe1( pa, val));
    
    case RX_AFE_DFE2:
    	return(_set_rx_dfe2( pa, val));
    
    case RX_AFE_DFE3:
    	return(_set_rx_dfe3( pa, val)); 
    
    case RX_AFE_DFE4:
    	return(_set_rx_dfe4( pa, val)); 
    
    case RX_AFE_DFE5:
    	return(_set_rx_dfe5( pa, val)); 
   
    case RX_AFE_DFE6:           
	return (_set_rx_dfe6( pa, val));  

    case RX_AFE_DFE7:          
	return (_set_rx_dfe7( pa, val));  

    case RX_AFE_DFE8:          
	return (_set_rx_dfe8( pa, val));  
    
    case RX_AFE_DFE9:          
	return (_set_rx_dfe9( pa, val));  
    
    case RX_AFE_DFE10:         
	return (_set_rx_dfe10( pa, val));  
    
    case RX_AFE_DFE11:     
        return (_set_rx_dfe11( pa, val));  

    case RX_AFE_DFE12: 
        return (_set_rx_dfe12( pa, val));  
    
    case RX_AFE_DFE13: 
        return (_set_rx_dfe13( pa, val));  
    
    case RX_AFE_DFE14: 
        return (_set_rx_dfe14( pa, val));  
    default:
	return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
}


err_code_t falcon_furia_madura_read_rx_afe( const phymod_access_t *pa, enum srds_rx_afe_settings_enum param, int8_t *val) {
  /* Assumes the micro is not actively tuning */
  if(!val) {
  	return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  
  switch(param) {
    case RX_AFE_PF: 
    	EFUN(_get_rx_pf_main( pa, val)); 
    	break;
    case RX_AFE_PF2:
    	EFUN(_get_rx_pf2( pa, val)); 
    	break;
    case RX_AFE_VGA:
    	EFUN(_get_rx_vga( pa, val)); 
      break;
    
    case RX_AFE_DFE1:
    	EFUN(_get_rx_dfe1( pa, val));
    	break;
    case RX_AFE_DFE2:
    	EFUN(_get_rx_dfe2( pa, val));
    	break;
    case RX_AFE_DFE3:
    	EFUN(_get_rx_dfe3( pa, val));
    	break;
    case RX_AFE_DFE4:
    	EFUN(_get_rx_dfe4( pa, val));
    	break;
    case RX_AFE_DFE5:
    	EFUN(_get_rx_dfe5( pa, val));
    	break;
    case RX_AFE_DFE6:
    	EFUN(_get_rx_dfe6( pa, val));
    	break;
    case RX_AFE_DFE7:
    	EFUN(_get_rx_dfe7( pa, val));
    	break;
    case RX_AFE_DFE8:
    	EFUN(_get_rx_dfe8( pa, val));
    	break;
    case RX_AFE_DFE9:
    	EFUN(_get_rx_dfe9( pa, val));
    	break;
    case RX_AFE_DFE10:
    	EFUN(_get_rx_dfe10( pa, val));
    	break;
    case RX_AFE_DFE11:
    	EFUN(_get_rx_dfe11( pa, val));
    	break;
    case RX_AFE_DFE12:
    	EFUN(_get_rx_dfe12( pa, val));
    	break;
    case RX_AFE_DFE13:
    	EFUN(_get_rx_dfe13( pa, val));
    	break;
    case RX_AFE_DFE14:
    	EFUN(_get_rx_dfe14( pa, val));
    	break;
    default:
    	return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  return(ERR_CODE_NONE);
}


err_code_t falcon_furia_madura_validate_txfir_cfg( const phymod_access_t *pa, int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3) {

  err_code_t err_code = ERR_CODE_NONE;

  if ((pre > 31) || (pre < 0)) 
    err_code = err_code | ERR_CODE_TXFIR_PRE_INVALID;  

  if ((main > 112) || (main < 0)) 
    err_code = err_code | ERR_CODE_TXFIR_MAIN_INVALID;  

  if ((post1 > 63) || (post1 < 0)) 
    err_code = err_code | ERR_CODE_TXFIR_POST1_INVALID;  

  if ((post2 > 15) || (post2 < -15)) 
    err_code = err_code | ERR_CODE_TXFIR_POST2_INVALID;  

  if ((post3 > 7) || (post3 < -7)) 
    err_code = err_code | ERR_CODE_TXFIR_POST3_INVALID;  
  
  if ((int16_t)(main + 48) < (int16_t)(pre + post1 + post2 + post3 + 1))
    err_code = err_code | ERR_CODE_TXFIR_V2_LIMIT;
  
  if ((int16_t)(pre + main + post1 + _abs(post2) + _abs(post3)) > 112)
    err_code = err_code | ERR_CODE_TXFIR_SUM_LIMIT;

  return (_error(err_code));  
}


err_code_t falcon_furia_madura_write_tx_afe( const phymod_access_t *pa, enum srds_tx_afe_settings_enum param, int8_t val) {

  switch(param) {
    case TX_AFE_PRE:
    	return(_set_tx_pre( pa, val));
    case TX_AFE_MAIN:
    	return(_set_tx_main( pa, val));
    case TX_AFE_POST1:
    	return(_set_tx_post1( pa, val));
    case TX_AFE_POST2:
    	return(_set_tx_post2( pa, val));
    case TX_AFE_POST3:
    	return(_set_tx_post3( pa, val));
    case TX_AFE_AMP:
    	return(_set_tx_amp( pa, val));
    case TX_AFE_DRIVERMODE:
        if(val == DM_NOT_SUPPORTED || val > DM_HALF_AMPLITUDE_HI_IMPED) {
            return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        } else {
            return(wr_falcon_furia_madura_ams_tx_drivermode(val));
        }
    default:
    	return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
}

err_code_t falcon_furia_madura_read_tx_afe( const phymod_access_t *pa, enum srds_tx_afe_settings_enum param, int8_t *val) {
  if(!val) {
  	return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  
  switch(param) {
    case TX_AFE_PRE:
    	EFUN(_get_tx_pre( pa, val));
    	break;
    case TX_AFE_MAIN:
    	EFUN(_get_tx_main( pa, val));
    	break;
    case TX_AFE_POST1:
    	EFUN(_get_tx_post1( pa, val));
    	break;
    case TX_AFE_POST2:
    	EFUN(_get_tx_post2( pa, val));
    	break;
    case TX_AFE_POST3:
    	EFUN(_get_tx_post3( pa, val));
    	break;
    case TX_AFE_AMP:
    	EFUN(_get_tx_amp( pa, val));
    	break;
    default:
    	return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  return(ERR_CODE_NONE);
}

err_code_t falcon_furia_madura_apply_txfir_cfg( const phymod_access_t *pa, int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3) {
  
  err_code_t failcode = falcon_furia_madura_validate_txfir_cfg( pa, pre, main, post1, post2, post3);

  if (!failcode) {
    failcode |= _set_tx_pre( pa, pre);
    failcode |= _set_tx_main( pa, main);    
    failcode |= _set_tx_post1( pa, post1);
    failcode |= _set_tx_post2( pa, post2);
    failcode |= _set_tx_post3( pa, post3);
  }

  return (_error(failcode));  
}





/**************************************/
/*  PRBS Generator/Checker Functions  */
/**************************************/

/* Configure PRBS Generator */
err_code_t falcon_furia_madura_config_tx_prbs( const phymod_access_t *pa, enum srds_prbs_polynomial_enum prbs_poly_mode, uint8_t prbs_inv) {

  wr_falcon_furia_madura_prbs_gen_mode_sel((uint8_t)prbs_poly_mode);        /* PRBS Generator mode sel */
  wr_falcon_furia_madura_prbs_gen_inv(prbs_inv);                            /* PRBS Invert Enable/Disable */
  /* To enable PRBS Generator */
  /* wr_falcon_furia_madura_prbs_gen_en(0x1); */
  return (ERR_CODE_NONE);
}

err_code_t falcon_furia_madura_get_tx_prbs_config( const phymod_access_t *pa, enum srds_prbs_polynomial_enum *prbs_poly_mode, uint8_t *prbs_inv) {
    uint8_t val;

  ESTM(val = rd_falcon_furia_madura_prbs_gen_mode_sel());                   /* PRBS Generator mode sel */
  *prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;
  ESTM(val = rd_falcon_furia_madura_prbs_gen_inv());                        /* PRBS Invert Enable/Disable */
  *prbs_inv = val;

  return (ERR_CODE_NONE);
}

/* PRBS Generator Enable/Disable */
err_code_t falcon_furia_madura_tx_prbs_en( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    wr_falcon_furia_madura_prbs_gen_en(0x1);                                /* Enable PRBS Generator */
  } 
  else { 
    wr_falcon_furia_madura_prbs_gen_en(0x0);                                /* Disable PRBS Generator */
  } 
  return (ERR_CODE_NONE);
}

/* Get PRBS Generator Enable/Disable */
err_code_t falcon_furia_madura_get_tx_prbs_en( const phymod_access_t *pa, uint8_t *enable) {

  ESTM(*enable = rd_falcon_furia_madura_prbs_gen_en());                                
 
  return (ERR_CODE_NONE);
}

/* PRBS 1-bit error injection */
err_code_t falcon_furia_madura_tx_prbs_err_inject( const phymod_access_t *pa, uint8_t enable) {
  /* PRBS Error Insert.
     0 to 1 transition on this signal will insert single bit error in the MSB bit of the data bus. 
     Reset value is 0x0.
  */
  if(enable)
    wr_falcon_furia_madura_prbs_gen_err_ins(0x1);
  wr_falcon_furia_madura_prbs_gen_err_ins(0);
  return (ERR_CODE_NONE);
}

/* Configure PRBS Checker */
err_code_t falcon_furia_madura_config_rx_prbs( const phymod_access_t *pa, enum srds_prbs_polynomial_enum prbs_poly_mode, enum srds_prbs_checker_mode_enum prbs_checker_mode, uint8_t prbs_inv) {

  wr_falcon_furia_madura_prbs_chk_mode_sel((uint8_t)prbs_poly_mode);       /* PRBS Checker Polynomial mode sel  */
  wr_falcon_furia_madura_prbs_chk_mode((uint8_t)prbs_checker_mode);        /* PRBS Checker mode sel (PRBS LOCK state machine select) */
  wr_falcon_furia_madura_prbs_chk_inv(prbs_inv);                           /* PRBS Invert Enable/Disable */
  /* To enable PRBS Checker */
  /* wr_falcon_furia_madura_prbs_chk_en(0x1); */
  return (ERR_CODE_NONE);
}

/* get PRBS Checker */
err_code_t falcon_furia_madura_get_rx_prbs_config( const phymod_access_t *pa, enum srds_prbs_polynomial_enum *prbs_poly_mode, enum srds_prbs_checker_mode_enum *prbs_checker_mode, uint8_t *prbs_inv) {
  uint8_t val;

  ESTM(val = rd_falcon_furia_madura_prbs_chk_mode_sel());                 /* PRBS Checker Polynomial mode sel  */
  *prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;
  ESTM(val = rd_falcon_furia_madura_prbs_chk_mode());                     /* PRBS Checker mode sel (PRBS LOCK state machine select) */
  *prbs_checker_mode = (enum srds_prbs_checker_mode_enum)val;
  ESTM(val = rd_falcon_furia_madura_prbs_chk_inv());                      /* PRBS Invert Enable/Disable */
  *prbs_inv = val;
  return (ERR_CODE_NONE);
}

/* PRBS Checker Enable/Disable */
err_code_t falcon_furia_madura_rx_prbs_en( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    wr_falcon_furia_madura_prbs_chk_en(0x1);                                /* Enable PRBS Checker */
  } 
  else { 
    wr_falcon_furia_madura_prbs_chk_en(0x0);                                /* Disable PRBS Checker */
  } 
  return (ERR_CODE_NONE);
}

err_code_t falcon_furia_madura_get_rx_prbs_en( const phymod_access_t *pa, uint8_t *enable) {

  ESTM(*enable = rd_falcon_furia_madura_prbs_chk_en());                                
 
  return (ERR_CODE_NONE);
}


/* PRBS Checker Lock State */
err_code_t falcon_furia_madura_prbs_chk_lock_state( const phymod_access_t *pa, uint8_t *chk_lock) {
  if(!chk_lock) {
	  return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  ESTM(*chk_lock = rd_falcon_furia_madura_prbs_chk_lock());                  /* PRBS Checker Lock Indication 1 = Locked, 0 = Out of Lock */
  return (ERR_CODE_NONE);
}

/* PRBS Error Count and Lock Lost (bit 31 in lock lost) */
err_code_t falcon_furia_madura_prbs_err_count_ll( const phymod_access_t *pa, uint32_t *prbs_err_cnt) {
  uint16_t rddata;
  if(!prbs_err_cnt) {
	  return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  EFUN(falcon_furia_madura_pmd_rdt_reg(pa,FALCON_FURIA_TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS, &rddata));
  *prbs_err_cnt = ((uint32_t) rddata)<<16;
  ESTM(*prbs_err_cnt = (*prbs_err_cnt | rd_falcon_furia_madura_prbs_chk_err_cnt_lsb()));
  return (ERR_CODE_NONE);
}
   
/* PRBS Error Count State  */
err_code_t falcon_furia_madura_prbs_err_count_state( const phymod_access_t *pa, uint32_t *prbs_err_cnt, uint8_t *lock_lost) {

  if(!prbs_err_cnt || !lock_lost) {
	  return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }

  EFUN(falcon_furia_madura_prbs_err_count_ll( pa, prbs_err_cnt));
  *lock_lost    = (*prbs_err_cnt >> 31);
  *prbs_err_cnt = (*prbs_err_cnt & 0x7FFFFFFF);
  return (ERR_CODE_NONE);
}


/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/

/* Enable/Diasble Digital Loopback */
err_code_t falcon_furia_madura_dig_lpbk( const phymod_access_t *pa, uint8_t enable) {
    wr_falcon_furia_madura_rx_pi_manual_mode(!enable);
  wr_falcon_furia_madura_dig_lpbk_en(enable);                               /* 0 = diabled, 1 = enabled */
  return (ERR_CODE_NONE);
}

/* Enable/Diasble Digital Loopback for Repeater */
err_code_t falcon_furia_madura_dig_lpbk_rptr( const phymod_access_t *pa, uint8_t enable, enum srds_rptr_mode_enum mode) {

  err_code_t err_code;
    uint8_t    osr_mode;

  switch(mode) {
    case DATA_IN_SIDE:
        /* DATA_IN_SIDE  - The side where data is fed into the RX serial input */
        if (enable) {
          err_code = falcon_furia_madura_loop_timing( pa, 0);
          if (err_code) {
            return(err_code);
          }
          err_code = falcon_furia_madura_loop_timing( pa, 1);     /* Program DATA_IN_SIDE side TX_PI for loop-timing mode to lock SYS TX to SYS TX clock freq. */
          if (err_code) {
            return(err_code);
          }
        }
        else {
          err_code = falcon_furia_madura_loop_timing( pa, 0);
          if (err_code) {
            return(err_code);
          }
        }
        break;

    case DIG_LPBK_SIDE:
        /* DIG_LPBK_SIDE - The side where data is looped back using digital loopback */
          ESTM(osr_mode = rd_falcon_furia_madura_osr_mode());                           
          if ((osr_mode == FALCON_FURIA_OSX16P5) || (osr_mode == FALCON_FURIA_OSX20P625)) {
            wr_falcon_furia_madura_rx_pi_manual_mode(0x0);
          }
        err_code = _rptr_mode_timing( pa );          /* Program DIG_LPBK_SIDE side TX_PI for repeater mode to lock LINE TX to SYS RX clock freq. */
        if (err_code) {
          return (err_code);
        }
        wr_falcon_furia_madura_dig_lpbk_en(enable);                        /* 0 = diabled, 1 = enabled */
        break;

    default : return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
        break;      
  }

  return (ERR_CODE_NONE);
}
                 


/* Locks TX_PI to Loop timing */
err_code_t falcon_furia_madura_loop_timing( const phymod_access_t *pa, uint8_t enable) {
  
  err_code_t err_code;
    uint8_t    osr_mode;

  if (enable) {

   
      wr_falcon_furia_madura_tx_pi_loop_timing_src_sel(0x1);                /* RX phase_sum_val_logic enable */
      ESTM(osr_mode = rd_falcon_furia_madura_osr_mode());
      if ((osr_mode == FALCON_FURIA_OSX16P5) || (osr_mode == FALCON_FURIA_OSX20P625)) {
        wr_falcon_furia_madura_rx_pi_manual_mode(0x1);
      }
      wr_falcon_furia_madura_tx_pi_repeater_mode_en(0x0);
      wr_falcon_furia_madura_tx_pi_en(0x1);                                 /* TX_PI enable: 0 = diabled, 1 = enabled */
      wr_falcon_furia_madura_tx_pi_jitter_filter_en(0x1);                   /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */



    err_code = falcon_furia_madura_delay_us(25);                     /* Wait for tclk to lock to CDR */
    if (err_code) {
      return(err_code);
    }
  } else {


      wr_falcon_furia_madura_tx_pi_jitter_filter_en(0x0);                   /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */
      wr_falcon_furia_madura_tx_pi_en(0x0);                                 /* TX_PI enable: 0 = diabled, 1 = enabled */
      wr_falcon_furia_madura_tx_pi_repeater_mode_en(0x1);
      wr_falcon_furia_madura_tx_pi_loop_timing_src_sel(0x0);                /* RX phase_sum_val_logic enable */


  }
  return (ERR_CODE_NONE);
}  


/* Setup Remote Loopback mode  */
err_code_t falcon_furia_madura_rmt_lpbk( const phymod_access_t *pa, uint8_t enable) {

  err_code_t err_code;

  if (enable) {
    err_code = falcon_furia_madura_loop_timing( pa, enable);
    if (err_code) {
      return(err_code);
    }
      wr_falcon_furia_madura_tx_pi_ext_ctrl_en(0x1);                        /* PD path enable: 0 = diabled, 1 = enabled */
    wr_falcon_furia_madura_rmt_lpbk_en(0x1);                                /* Remote Loopback Enable: 0 = diabled, 1 = enable  */
    err_code = falcon_furia_madura_delay_us(50);                     /* Wait for rclk and tclk phase lock before expecing good data from rmt loopback */
    return(err_code);                                   /* Might require longer wait time for smaller values of tx_pi_ext_phase_bwsel_integ */
  }
  else {
    wr_falcon_furia_madura_rmt_lpbk_en(0x0);                                /* Remote Loopback Enable: 0 = diabled, 1 = enable  */
      wr_falcon_furia_madura_tx_pi_ext_ctrl_en(0x0);                        /* PD path enable: 0 = diabled, 1 = enabled */
    err_code = falcon_furia_madura_loop_timing( pa, enable);
    return(err_code);
  }
}
    

  /* Repeater Only APIs (Not applicable to PMD) */

  /* Switch to remote loopback from ULL mode in repeater */
  err_code_t falcon_furia_madura_rmt_lpbk_from_ull( const phymod_access_t *pa ) {

    err_code_t err_code;

    err_code = falcon_furia_madura_rmt_lpbk( pa, 0);                      /* Reset TX_PI */
    if (err_code) {
      return (err_code);
    }
    err_code = falcon_furia_madura_rmt_lpbk( pa, 1);                      /* Enable RMT LPBK */
    return (err_code);
  }


  /* Switch to remote loopback from NL mode in repeater */
  err_code_t falcon_furia_madura_rmt_lpbk_from_nl( const phymod_access_t *pa ) {

    err_code_t err_code;

    err_code = falcon_furia_madura_rmt_lpbk( pa, 0);                      /* Reset TX_PI */
    if (err_code) {
      return (err_code);
    }
    err_code = falcon_furia_madura_rmt_lpbk( pa, 1);                      /* Enable RMT LPBK */
    return (err_code);
  }


  /* Switch back from remote loopback to ULL mode in repeater */
  err_code_t falcon_furia_madura_ull_from_rmt_lpbk( const phymod_access_t *pa ) {

    err_code_t err_code;

    err_code = falcon_furia_madura_rmt_lpbk( pa, 0);                      /* Reset TX_PI */
    if (err_code) {
      return (err_code);
    }
    err_code = _rptr_mode_timing( pa );                     /* Enable jitter filter enable */
    if (err_code) {
      return (err_code);
    }
    err_code = _ull_mode_setup( pa );                       /* Setup ULL Mode */
    return (err_code);
  }


  /* Switch back from remote loopback to NL mode in repeater */
  err_code_t falcon_furia_madura_nl_from_rmt_lpbk( const phymod_access_t *pa ) { 

    err_code_t err_code;

    err_code = falcon_furia_madura_rmt_lpbk( pa, 0);                      /* Reset TX_PI */
    if (err_code) {
      return (err_code);
    }
    err_code = _rptr_mode_timing( pa );                     /* Enable jitter filter enable */
    if (err_code) {
      return (err_code);
    }
    
      wr_falcon_furia_madura_ams_tx_sel_ll(0x0);                            /* Switch the tx data mux to PCS data */

    return (ERR_CODE_NONE);
  }




/**********************************/
/*  TX_PI Jitter Generation APIs  */
/**********************************/

/* TX_PI Frequency Override (Fixed Frequency Mode) */
err_code_t falcon_furia_madura_tx_pi_freq_override( const phymod_access_t *pa, uint8_t enable, int16_t freq_override_val) {                  

    if (enable) {
      wr_falcon_furia_madura_tx_pi_en(0x1);                                 /* TX_PI enable :            0 = disabled, 1 = enabled */
      wr_falcon_furia_madura_tx_pi_freq_override_en(0x1);                   /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
      wr_falcon_furia_madura_tx_pi_freq_override_val(freq_override_val);    /* Fixed Freq Override Value (+/-8192) */
    } 
    else {
      wr_falcon_furia_madura_tx_pi_freq_override_val(0);                    /* Fixed Freq Override Value to 0 */
      wr_falcon_furia_madura_tx_pi_freq_override_en(0x0);                   /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
      wr_falcon_furia_madura_tx_pi_en(0x0);                                 /* TX_PI enable :            0 = disabled, 1 = enabled */
    }

  return (ERR_CODE_NONE);
}


/* TX_PI Sinusoidal or Spread-Spectrum (SSC) Jitter Generation  */
err_code_t falcon_furia_madura_tx_pi_jitt_gen( const phymod_access_t *pa, uint8_t enable, int16_t freq_override_val, enum srds_tx_pi_freq_jit_gen_enum jit_type, uint8_t tx_pi_jit_freq_idx, uint8_t tx_pi_jit_amp) {

  err_code_t err_code;

  /* Added a limiting for the jitter amplitude index, per freq_idx */
  uint8_t max_amp_idx_r20_os1[] = {37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 48, 33, 47, 37, 33, 37, 37};

  /* Irrespective of the osr_mode, txpi runs @ os1. Thus the max amp idx values remain the same. */
  if (jit_type == TX_PI_SJ) {
	  if (tx_pi_jit_amp > max_amp_idx_r20_os1[tx_pi_jit_freq_idx]) {
			tx_pi_jit_amp = max_amp_idx_r20_os1[tx_pi_jit_freq_idx];
	  }
  }

  err_code = falcon_furia_madura_tx_pi_freq_override( pa, enable, freq_override_val);
  if (err_code) {
    return (err_code);
  }

    if (enable) {
      wr_falcon_furia_madura_tx_pi_jit_freq_idx(tx_pi_jit_freq_idx);                             
      wr_falcon_furia_madura_tx_pi_jit_amp(tx_pi_jit_amp);       
                      
      if (jit_type == TX_PI_SSC_HIGH_FREQ) { 
        wr_falcon_furia_madura_tx_pi_jit_ssc_freq_mode(0x1);                /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
        wr_falcon_furia_madura_tx_pi_ssc_gen_en(0x1);                       /* SSC jitter enable:         0 = disabled,    1 = enabled */
      } 
      else if (jit_type == TX_PI_SSC_LOW_FREQ) {
        wr_falcon_furia_madura_tx_pi_jit_ssc_freq_mode(0x0);                /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
        wr_falcon_furia_madura_tx_pi_ssc_gen_en(0x1);                       /* SSC jitter enable:         0 = disabled,    1 = enabled */
      }
      else if (jit_type == TX_PI_SJ) {
        wr_falcon_furia_madura_tx_pi_sj_gen_en(0x1);                        /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
      }
    } 
    else {
      wr_falcon_furia_madura_tx_pi_ssc_gen_en(0x0);                         /* SSC jitter enable:         0 = disabled,    1 = enabled */
      wr_falcon_furia_madura_tx_pi_sj_gen_en(0x0);                          /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
    }
  return (ERR_CODE_NONE);
}


/***************************/
/*  Configure Serdes IDDQ  */
/***************************/

err_code_t falcon_furia_madura_core_config_for_iddq( const phymod_access_t *pa ) {

  return (ERR_CODE_NONE);
}
 

err_code_t falcon_furia_madura_lane_config_for_iddq( const phymod_access_t *pa ) {

  /* Use frc/frc_val to force all RX and TX clk_vld signals to 0 */
  wr_falcon_furia_madura_pmd_rx_clk_vld_frc_val(0x0);
  wr_falcon_furia_madura_pmd_rx_clk_vld_frc(0x1);
      wr_falcon_furia_madura_pmd_tx_clk_vld_frc_val(0x0);
      wr_falcon_furia_madura_pmd_tx_clk_vld_frc(0x1);

 
  /* Use frc/frc_val to force all pmd_rx_lock signals to 0 */
  wr_falcon_furia_madura_rx_dsc_lock_frc_val(0x0);
  wr_falcon_furia_madura_rx_dsc_lock_frc(0x1);

  /* Switch all the lane clocks to comclk by writing to RX/TX comclk_sel registers */
  wr_falcon_furia_madura_ln_rx_s_comclk_sel(0x1);
    wr_falcon_furia_madura_ln_tx_s_comclk_sel(0x1);

  /* Assert all the AFE pwrdn/reset pins using frc/frc_val to make sure AFE is in lowest possible power mode */
  wr_falcon_furia_madura_afe_tx_pwrdn_frc_val(0x1);
  wr_falcon_furia_madura_afe_tx_pwrdn_frc(0x1);
  wr_falcon_furia_madura_afe_rx_pwrdn_frc_val(0x1);
  wr_falcon_furia_madura_afe_rx_pwrdn_frc(0x1);
  wr_falcon_furia_madura_afe_tx_reset_frc_val(0x1);
  wr_falcon_furia_madura_afe_tx_reset_frc(0x1);
  wr_falcon_furia_madura_afe_rx_reset_frc_val(0x1);
  wr_falcon_furia_madura_afe_rx_reset_frc(0x1);

  /* Set pmd_iddq pin to enable IDDQ */
  return (ERR_CODE_NONE);
} 


/*********************************/
/*  uc_active and uc_reset APIs  */
/*********************************/

/* Set the uC active mode */
err_code_t falcon_furia_madura_uc_active_enable( const phymod_access_t *pa, uint8_t enable) {
  wrc_falcon_furia_madura_uc_active(enable);
  return (ERR_CODE_NONE);
}




/****************************************************/
/*  Serdes Powerdown, ClockGate and Deep_Powerdown  */
/****************************************************/

err_code_t falcon_furia_madura_core_pwrdn( const phymod_access_t *pa, enum srds_core_pwrdn_mode_enum mode) {
  err_code_t err_code;
  switch(mode) {
    case PWR_ON:
            err_code = _falcon_furia_madura_core_clkgate( pa, 0);
            if (err_code) return (err_code);
            wrc_falcon_furia_madura_afe_s_pll_pwrdn(0x0);
            wrc_falcon_furia_madura_core_dp_s_rstb(0x1);
            break;
    case PWRDN:
            wrc_falcon_furia_madura_core_dp_s_rstb(0x0);
            err_code = falcon_furia_madura_delay_ns(500);                     /* wait for more than 50 comclk cycles  */
            wrc_falcon_furia_madura_afe_s_pll_pwrdn(0x1);
            break;
    case PWRDN_DEEP:
            wrc_falcon_furia_madura_core_dp_s_rstb(0x0);
            err_code = falcon_furia_madura_delay_ns(500);                     /* wait for more than 50 comclk cycles  */
            wrc_falcon_furia_madura_afe_s_pll_pwrdn(0x1);
            err_code = _falcon_furia_madura_core_clkgate( pa, 1);
            if (err_code) return (err_code);
            break;
    default : return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  return (ERR_CODE_NONE);
}

err_code_t falcon_furia_madura_lane_pwrdn( const phymod_access_t *pa, enum srds_core_pwrdn_mode_enum mode) {
  err_code_t err_code;


  switch(mode) {
    case PWR_ON:
            wr_falcon_furia_madura_ln_tx_s_pwrdn(0x0);
            wr_falcon_furia_madura_ln_rx_s_pwrdn(0x0);
            err_code = _falcon_furia_madura_lane_clkgate( pa, 0);
            if (err_code) return (err_code);
            break;
    case PWRDN:
            /* do the RX first, since that is what is most users care about */
            wr_falcon_furia_madura_ln_rx_s_pwrdn(0x1);
            wr_falcon_furia_madura_ln_tx_s_pwrdn(0x1);
            break;
    case PWRDN_DEEP:
            /* do the RX first, since that is what is most users care about */
            wr_falcon_furia_madura_ln_rx_s_pwrdn(0x1);
            wr_falcon_furia_madura_ln_tx_s_pwrdn(0x1);
            err_code = _falcon_furia_madura_lane_clkgate( pa, 1);
            if (err_code) return (err_code);
            wr_falcon_furia_madura_ln_dp_s_rstb(0x0);
            break;
    case PWRDN_TX:
            wr_falcon_furia_madura_ln_tx_s_pwrdn(0x1);
            break;
    case PWRDN_RX:
      wr_falcon_furia_madura_ln_rx_s_pwrdn(0x1);
            break;
    default : return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
  return (ERR_CODE_NONE);
}


/*******************************/
/*  Isolate Serdes Input Pins  */
/*******************************/

err_code_t falcon_furia_madura_isolate_ctrl_pins( const phymod_access_t *pa, uint8_t enable) {

  if (enable) {
    wr_falcon_furia_madura_pmd_ln_tx_h_pwrdn_pkill(0x1);
    wr_falcon_furia_madura_pmd_ln_rx_h_pwrdn_pkill(0x1);
    wr_falcon_furia_madura_pmd_ln_dp_h_rstb_pkill(0x1);
    wr_falcon_furia_madura_pmd_ln_h_rstb_pkill(0x1);
    wrc_falcon_furia_madura_pmd_core_dp_h_rstb_pkill(0x1);
  }
  else {
    wr_falcon_furia_madura_pmd_ln_tx_h_pwrdn_pkill(0x0);
    wr_falcon_furia_madura_pmd_ln_rx_h_pwrdn_pkill(0x0);
    wr_falcon_furia_madura_pmd_ln_dp_h_rstb_pkill(0x0);
    wr_falcon_furia_madura_pmd_ln_h_rstb_pkill(0x0);
    wrc_falcon_furia_madura_pmd_core_dp_h_rstb_pkill(0x0);
  }
  return (ERR_CODE_NONE);
}




  /* Micro RAM Byte Read */
  err_code_t falcon_furia_madura_rdblk_uc_ram( const phymod_access_t *pa, uint8_t *mem, uint16_t addr, uint16_t cnt) {      

      if(!mem) {
          return(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
      }

      falcon_furia_madura_rdb_ram(pa, mem,addr,cnt);
      return(ERR_CODE_NONE);
  } 



