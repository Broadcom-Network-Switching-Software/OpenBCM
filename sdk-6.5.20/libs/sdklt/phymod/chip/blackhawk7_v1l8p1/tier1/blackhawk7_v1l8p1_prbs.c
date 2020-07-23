/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/***********************************************************************************
 ***********************************************************************************
 *  File Name     :  blackhawk7_v1l8p1_prbs.c                                             *
 *  Created On    :  04 Nov 2015                                                   *
 *  Created By    :  Brent Roberts                                                 *
 *  Description   :  Serdes PRBS test APIs for Serdes IPs                          *
 *  Revision      :     *
 */

/** @file blackhawk7_v1l8p1_prbs.c
 * Implementation of API PRBS functions
 */
#ifdef SERDES_API_FLOATING_POINT
#include <math.h>
#endif

#include <phymod/phymod_system.h>
#include "blackhawk7_v1l8p1_prbs.h"
#include "blackhawk7_v1l8p1_common.h"
#include "blackhawk7_v1l8p1_config.h"
#include "blackhawk7_v1l8p1_dependencies.h"
#include "blackhawk7_v1l8p1_functions.h"
#include "blackhawk7_v1l8p1_internal.h"
#include "blackhawk7_v1l8p1_internal_error.h"
#include "blackhawk7_v1l8p1_select_defns.h"
#include "blackhawk7_v1l8p1_access.h"

#ifndef SMALL_FOOTPRINT

/******************************/
/*  TX Pattern Generator APIs */
/******************************/

/* Cofigure shared TX Pattern (Return Val = 0:PASS, 1-6:FAIL (reports 6 possible error scenarios if failed)) */
err_code_t blackhawk7_v1l8p1_config_shared_tx_pattern(srds_access_t *sa__, uint8_t patt_length_bits, const char pattern[]) {
    INIT_SRDS_ERR_CODE

  char       patt_final[246] = "";
  char       patt_mod[246]   = "", bin[5] = "";
  uint8_t    str_len, i, k, j = 0;
  uint8_t    offset_len, actual_patt_len = 0, hex = 0;
  uint8_t    zero_pad_len    = 0; /* suppress warnings, changed by blackhawk7_v1l8p1_INTERNAL_calc_patt_gen_mode_sel() */
  uint16_t   patt_gen_wr_val = 0;
  uint8_t    mode_sel        = 0; /* suppress warnings, changed by blackhawk7_v1l8p1_INTERNAL_calc_patt_gen_mode_sel() */

  EFUN(blackhawk7_v1l8p1_INTERNAL_calc_patt_gen_mode_sel(sa__, &mode_sel,&zero_pad_len,patt_length_bits));

  /* Generating the appropriate write value to patt_gen_seq registers */
  str_len = (uint8_t)USR_STRLEN(pattern);

  if ((str_len > 2) && ((USR_STRNCMP(pattern, "0x", 2)) == 0)) {
    /* Hexadecimal Pattern */
    for (i=2; i < str_len; i++) {
      if (pattern[i] != '_') {
        EFUN(blackhawk7_v1l8p1_INTERNAL_compute_bin(sa__, pattern[i],bin));
        ENULL_STRNCAT(patt_mod, bin, SPC_LEFT(patt_mod) - 1);
        actual_patt_len = (uint8_t)(actual_patt_len + 4);
        if (actual_patt_len > 240) {
          EFUN_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
          return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN));
        }
      }
    }

    offset_len = (uint8_t)(actual_patt_len - patt_length_bits);
    if ((offset_len > 3)  || (actual_patt_len < patt_length_bits)) {
      EFUN_PRINTF(("ERROR: Pattern length provided does not match the hexadecimal pattern provided\n"));
      return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CFG_PATT_LEN_MISMATCH));
    }
    else if (offset_len) {
      for (i=0; i < offset_len; i++) {
        if (patt_mod[i] != '0') {
          EFUN_PRINTF(("ERROR: Pattern length provided does not match the hexadecimal pattern provided\n"));
          return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CFG_PATT_LEN_MISMATCH));
        }
      }
      for (i=offset_len; i <= actual_patt_len; i++) {
        patt_mod[i - offset_len] = patt_mod[i];
      }
    }
  }
  else if ((str_len > 1) && ((USR_STRNCMP(pattern, "p", 1)) == 0)) {
    /* PAM4 Pattern */
    for (i=1; i < str_len; i++) {
      if (pattern[i] != '_') {
        EFUN(blackhawk7_v1l8p1_INTERNAL_pam4_to_bin(sa__, pattern[i],bin));
        ENULL_STRNCAT(patt_mod, bin, SPC_LEFT(patt_mod) - 1);
        actual_patt_len = (uint8_t)(actual_patt_len + 2);
        if (actual_patt_len > 240) {
          EFUN_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
          return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN));
        }
      }
    }

    offset_len = (uint8_t)(actual_patt_len - patt_length_bits);
    if ((offset_len > 3)  || (actual_patt_len < patt_length_bits)) {
      EFUN_PRINTF(("ERROR: Pattern length provided does not match the PAM4 pattern provided\n"));
      return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CFG_PATT_LEN_MISMATCH));
    }
    else if (offset_len) {
      for (i=0; i < offset_len; i++) {
        if (patt_mod[i] != '0') {
          EFUN_PRINTF(("ERROR: Pattern length provided does not match the PAM4 pattern provided\n"));
          return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CFG_PATT_LEN_MISMATCH));
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
        ENULL_STRNCAT(patt_mod, bin, SPC_LEFT(patt_mod) - 1);
        actual_patt_len++;
        if (actual_patt_len > 240) {
          EFUN_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
          return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN));
        }
      }
      else if (pattern[i] != '_') {
        EFUN_PRINTF(("ERROR: Invalid input Pattern\n"));
        return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CFG_PATT_INVALID_PATTERN));
      }
    }

    if (actual_patt_len != patt_length_bits) {
      EFUN_PRINTF(("ERROR: Pattern length provided does not match the binary pattern provided\n"));
      return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CFG_PATT_LEN_MISMATCH));
    }
  }

  /* Zero padding upper bits and concatinating patt_mod to form patt_final */
  for (i=0; i < zero_pad_len; i++) {
    ENULL_STRNCAT(patt_final, "0", SPC_LEFT(patt_final) - 1);
    j++;
  }
  for (i=zero_pad_len; i + patt_length_bits < 241; i = (uint8_t)(i + patt_length_bits)) {
    ENULL_STRNCAT(patt_final, patt_mod, SPC_LEFT(patt_final) - 1);
    j++;
  }

  /* EFUN_PRINTF(("\nFinal Pattern = %s\n\n",patt_final));    */

  for (i=0; i < 15; i++) {

    for (j=0; j < 4; j++) {
      k = (uint8_t)(i*16 + j*4);
      bin[0] = patt_final[k];
      bin[1] = patt_final[k+1];
      bin[2] = patt_final[k+2];
      bin[3] = patt_final[k+3];
      bin[4] = '\0';
      EFUN(blackhawk7_v1l8p1_INTERNAL_compute_hex(sa__,bin, &hex));
      patt_gen_wr_val = (uint16_t)((patt_gen_wr_val << 4) | hex);
    }
    /* EFUN_PRINTF(("patt_gen_wr_val[%d] = 0x%x\n",(14-i),patt_gen_wr_val)); */

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
               return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_CFG_PATT_INVALID_SEQ_WRITE));
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
err_code_t blackhawk7_v1l8p1_tx_shared_patt_gen_en(srds_access_t *sa__, uint8_t enable, uint8_t patt_length_bits) {
    INIT_SRDS_ERR_CODE
  uint8_t zero_pad_len = 0; /* suppress warnings, changed by blackhawk7_v1l8p1_INTERNAL_calc_patt_gen_mode_sel() */
  uint8_t mode_sel     = 0; /* suppress warnings, changed by blackhawk7_v1l8p1_INTERNAL_calc_patt_gen_mode_sel() */

  EFUN(blackhawk7_v1l8p1_INTERNAL_calc_patt_gen_mode_sel(sa__, &mode_sel,&zero_pad_len,patt_length_bits));

  if (enable) {
    if ((mode_sel < 1) || (mode_sel > 6)) {
      return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_PATT_GEN_INVALID_MODE_SEL));
    }
    mode_sel = (uint8_t)(12 - mode_sel);
    EFUN(wr_patt_gen_start_pos(mode_sel));            /* Start position for pattern */
    EFUN(wr_patt_gen_stop_pos(0x0));                  /* Stop position for pattern */
    EFUN(wr_patt_gen_en(0x1));                        /* Enable Fixed pattern Generation  */
  }
  else {
    EFUN(wr_patt_gen_en(0x0));                        /* Disable Fixed pattern Generation  */
  }
  return(ERR_CODE_NONE);
}

/* Configure JP03B Pattern */
err_code_t blackhawk7_v1l8p1_config_tx_jp03b_pattern(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE

  if (enable) {
    /* JP03B Pattern - "p0330" => 8'b_0011_1100 or 8'h3C w/o  Gray coding */
    /*                         => 8'b_0010_1000 or 8'h28 with Gray coding */
    EFUN(wrc_patt_gen_seq_1(0x2800));
    EFUN(wr_pam4_precoder_en(0x0));
    EFUN(wr_pam4_gray_enc_en(0x0));
    EFUN(wr_pam4_tx_jp03b_patt_en(0x1));
    EFUN(wr_patt_gen_en(0x1));
  }
  else {
    EFUN(wr_patt_gen_en(0x0));
    EFUN(wr_pam4_tx_jp03b_patt_en(0x0));
  }
  return(ERR_CODE_NONE);
}


/* Configure TX Linearity Pattern */
err_code_t blackhawk7_v1l8p1_config_tx_linearity_pattern(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE

  if (enable) {
    /* Linearity Pattern - "p0123_0303_21" => 20'b_0001_1011_0011_0011_1001 or 20'h1B339 w/o  Gray coding */
    /*                                     => 20'b_0001_1110_0010_0010_1101 or 20'h1E22D with Gray coding */
    EFUN(wrc_patt_gen_seq_0(0xE22D));
    EFUN(wrc_patt_gen_seq_1(0x0001));
    EFUN(wr_pam4_precoder_en(0x0));
    EFUN(wr_pam4_gray_enc_en(0x0));
    EFUN(wr_pam4_tx_jp03b_patt_en(0x0));
    EFUN(wr_pam4_tx_linearity_patt_en(0x1));
    EFUN(wr_patt_gen_en(0x1));
  }
  else {
    EFUN(wr_patt_gen_en(0x0));
    EFUN(wr_pam4_tx_linearity_patt_en(0x0));
  }
  return(ERR_CODE_NONE);
}

/**************************************/
/*  PRBS Generator/Checker Functions  */
/**************************************/

/* Configure PRBS Generator */
err_code_t blackhawk7_v1l8p1_config_tx_prbs(srds_access_t *sa__, enum srds_prbs_polynomial_enum prbs_poly_mode, uint8_t prbs_inv) {
    INIT_SRDS_ERR_CODE
    if(prbs_poly_mode >= PCS_PRBS_7) {
      return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }
    {
      uint8_t poly_mode_lsb;

      poly_mode_lsb = prbs_poly_mode & 0x7;

      EFUN(wr_prbs_gen_mode_sel((uint8_t)poly_mode_lsb));   /* PRBS Generator mode sel */
    }
    EFUN(wr_prbs_gen_mode_sel_msb(prbs_poly_mode > 7));   /* Set MSB for new Poly mode selections */
    EFUN(wr_prbs_gen_inv(prbs_inv));                      /* PRBS Invert Enable/Disable */
    /* To enable PRBS Generator */
    /* EFUN(wr_prbs_gen_en(0x1)); */
    return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p1_get_tx_prbs_config(srds_access_t *sa__, enum srds_prbs_polynomial_enum *prbs_poly_mode, uint8_t *prbs_inv) {
    INIT_SRDS_ERR_CODE
    uint8_t val;

    ESTM(val = rd_prbs_gen_mode_sel());                   /* PRBS Generator mode sel */
    ESTM(val = (uint8_t)(val + (rd_prbs_gen_mode_sel_msb() ? PRBS_10 : 0)));  /* Add the MSB for mode_sel. PRBS10 is the offset to the 2nd group of PRBS pattern */
    *prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;
    ESTM(val = rd_prbs_gen_inv());                        /* PRBS Invert Enable/Disable */
    *prbs_inv = val;
  return (ERR_CODE_NONE);
}

/* PRBS Generator Enable/Disable */
err_code_t blackhawk7_v1l8p1_tx_prbs_en(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    if (enable) {
        EFUN(wr_prbs_gen_en(0x1));                          /* Enable PRBS Generator */
    }
    else {
        EFUN(wr_prbs_gen_en(0x0));                          /* Disable PRBS Generator */
    }
    return (ERR_CODE_NONE);
}
/* Get PRBS Generator Enable/Disable */
err_code_t blackhawk7_v1l8p1_get_tx_prbs_en(srds_access_t *sa__, uint8_t *enable) {
    INIT_SRDS_ERR_CODE
    if(!enable) {
        return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    ESTM(*enable = rd_prbs_gen_en());
    return (ERR_CODE_NONE);
}

/* PRBS 1-bit error injection */
err_code_t blackhawk7_v1l8p1_tx_prbs_err_inject(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
  /* PRBS Error Insert.
     0 to 1 transition on this signal will insert single bit error in the MSB bit of the data bus.
     Reset value is 0x0.
  */
    if(enable) {
      EFUN(wr_prbs_gen_err_ins(0x1));
    }
    EFUN(wr_prbs_gen_err_ins(0));
  return (ERR_CODE_NONE);
}

/* Configure PRBS Checker */
err_code_t blackhawk7_v1l8p1_config_rx_prbs(srds_access_t *sa__, enum srds_prbs_polynomial_enum prbs_poly_mode, enum srds_prbs_checker_mode_enum prbs_checker_mode, uint8_t prbs_inv) {
    INIT_SRDS_ERR_CODE
  {
    uint8_t dig_lpbk = 0;
  if(prbs_poly_mode >= PCS_PRBS_7) {
    return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
  }
  if (prbs_poly_mode == PRBS_AUTO_DETECT) {
    EFUN(wr_prbs_chk_auto_detect_en(0x1));                 /* Enable PRBS checker Auto-Detect */
    EFUN(wr_prbs_chk_mode(PRBS_INITIAL_SEED_HYSTERESIS));  /* PRBS Checker mode sel (PRBS LOCK state machine select) */
  }
  else if (prbs_poly_mode == USER_40_BIT_REPEAT) {
    EFUN(wr_prbs_chk_mode_sel_msb(0x1));                  /* PRBS Checker Polynomial mode sel MSB  */
    EFUN(wr_prbs_chk_mode_sel(USER_40_BIT_REPEAT & 0x7)); /* PRBS Checker Polynomial mode sel  */
    EFUN(wr_prbs_chk_mode((uint8_t)prbs_checker_mode));  /* PRBS Checker mode sel (PRBS LOCK state machine select) */
  }
    else {
      {
        uint8_t poly_mode_lsb;

        poly_mode_lsb = prbs_poly_mode & 0x7;
        EFUN(wr_prbs_chk_mode_sel((uint8_t)poly_mode_lsb));  /* PRBS Checker Polynomial mode sel  */
      }
      EFUN(wr_prbs_chk_mode_sel_msb(prbs_poly_mode > 7));  /* Set MSB for new Poly mode selections */
      EFUN(wr_prbs_chk_mode((uint8_t)prbs_checker_mode));  /* PRBS Checker mode sel (PRBS LOCK state machine select) */
      ESTM(dig_lpbk = rd_dig_lpbk_en());
      if(dig_lpbk == 0) {
        /* Only enable auto mode in non-digital loop-back mode */
        EFUN(wr_prbs_chk_en_auto_mode(0x1));             /* PRBS Checker enable control - rx_dsc_lock & prbs_chk_en */
      }
      EFUN(wr_prbs_chk_inv(prbs_inv));                     /* PRBS Invert Enable/Disable */
      EFUN(wr_prbs_chk_auto_detect_en(0x0));               /* Disable PRBS checker Auto-Detect */
    }
  }
  /* To enable PRBS Checker */
  /* EFUN(wr_prbs_chk_en(0x1)); */
  return (ERR_CODE_NONE);
}

/* get PRBS Checker */
err_code_t blackhawk7_v1l8p1_get_rx_prbs_config(srds_access_t *sa__, enum srds_prbs_polynomial_enum *prbs_poly_mode, enum srds_prbs_checker_mode_enum *prbs_checker_mode, uint8_t *prbs_inv) {
    INIT_SRDS_ERR_CODE
    uint8_t val;

    uint8_t auto_det_en;
    ESTM(auto_det_en = rd_prbs_chk_auto_detect_en());       /* PRBS checker Auto-Detect Enable */
    if (auto_det_en) {
      uint8_t auto_det_lock;
      ESTM(auto_det_lock = rd_prbs_chk_auto_detect_lock()); /* PRBS checker Auto-Detect Lock */
      if (auto_det_lock) {
        ESTM(val = rd_prbs_chk_mode_sel_auto_detect());     /* PRBS Auto-Detect Checker Polynomial mode sel  */
        ESTM(val = (uint8_t)(val + (rd_prbs_chk_mode_sel_msb_auto_detect() ? PRBS_10 : 0)));  /* Add the MSB for Auto-Detect mode_sel. PRBS10 is the offset to the 2nd group of PRBS pattern */
        *prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;

        ESTM(val = rd_prbs_chk_mode());                     /* PRBS Checker mode sel (PRBS LOCK state machine select) */
        *prbs_checker_mode = (enum srds_prbs_checker_mode_enum)val;
        ESTM(val = rd_prbs_chk_inv_auto_detect());          /* PRBS Invert Enable/Disable */
        *prbs_inv = val;
      }
      else {
        *prbs_poly_mode = PRBS_UNKNOWN;
        *prbs_inv = 0;
        ESTM(val = rd_prbs_chk_mode());                     /* PRBS Checker mode sel (PRBS LOCK state machine select) */
        *prbs_checker_mode = (enum srds_prbs_checker_mode_enum)val;
      }
    }
    else {
      ESTM(val = rd_prbs_chk_mode_sel());                 /* PRBS Checker Polynomial mode sel  */
      ESTM(val = (uint8_t)(val + (rd_prbs_chk_mode_sel_msb() ? PRBS_10 : 0)));  /* Add the MSB for mode_sel. PRBS10 is the offset to the 2nd group of PRBS pattern */
      *prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;

      ESTM(val = rd_prbs_chk_mode());                     /* PRBS Checker mode sel (PRBS LOCK state machine select) */
      *prbs_checker_mode = (enum srds_prbs_checker_mode_enum)val;
      ESTM(val = rd_prbs_chk_inv());                      /* PRBS Invert Enable/Disable */
      *prbs_inv = val;
    }
  return (ERR_CODE_NONE);
}

/* PRBS Checker Enable/Disable */
err_code_t blackhawk7_v1l8p1_rx_prbs_en(srds_access_t *sa__, uint8_t enable) {
    INIT_SRDS_ERR_CODE
    if (enable) {
        EFUN(wr_prbs_chk_en(0x1));                          /* Enable PRBS Checker */
    }
    else {
        EFUN(wr_prbs_chk_en(0x0));                          /* Disable PRBS Checker */
    }
    return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p1_get_rx_prbs_en(srds_access_t *sa__, uint8_t *enable) {
    INIT_SRDS_ERR_CODE
    if(!enable) {
        return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }
    ESTM(*enable = rd_prbs_chk_en());
    return (ERR_CODE_NONE);
}


/* PRBS Checker Lock State */
err_code_t blackhawk7_v1l8p1_prbs_chk_lock_state(srds_access_t *sa__, uint8_t *chk_lock) {
    INIT_SRDS_ERR_CODE
  if(!chk_lock) {
    return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
    ESTM(*chk_lock = rd_prbs_chk_lock());                  /* PRBS Checker Lock Indication 1 = Locked, 0 = Out of Lock */
  return (ERR_CODE_NONE);
}

/* PRBS Error Count and Lock Lost (bit 31 in lock lost) */
err_code_t blackhawk7_v1l8p1_prbs_err_count_ll(srds_access_t *sa__, uint32_t *prbs_err_cnt) {
    INIT_SRDS_ERR_CODE
  uint16_t rddata;

  if(!prbs_err_cnt) {
    return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
    ESTM(rddata = REG_RD_TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS());
    *prbs_err_cnt = ((uint32_t) rddata)<<16;
    ESTM(*prbs_err_cnt = (*prbs_err_cnt | rd_prbs_chk_err_cnt_lsb()));
  return (ERR_CODE_NONE);
}

/* PRBS Error Count State  */
err_code_t blackhawk7_v1l8p1_prbs_err_count_state(srds_access_t *sa__, uint32_t *prbs_err_cnt, uint8_t *lock_lost) {
    INIT_SRDS_ERR_CODE
  uint8_t dig_lpbk_enable = 0;
  uint8_t link_training_enable = 0;
  uint8_t hw_timer_mode = 0;
  if(!prbs_err_cnt || !lock_lost) {
    return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
  }
    EFUN(blackhawk7_v1l8p1_prbs_err_count_ll(sa__, prbs_err_cnt));
    ESTM(hw_timer_mode = rd_prbs_chk_en_timer_unit_sel());
    if(hw_timer_mode != 0) {
      *lock_lost = 0;
    } else
    {
      *lock_lost = (uint8_t)(*prbs_err_cnt >> 31);
    }
    *prbs_err_cnt = (*prbs_err_cnt & 0x7FFFFFFF);

    /* Check if Digital Loopback and LinkTrainig both are enabled */
    ESTM(dig_lpbk_enable = rd_dig_lpbk_en());
    ESTM(link_training_enable = rd_linktrn_ieee_training_enable());

    if(dig_lpbk_enable && link_training_enable) {
      EFUN_PRINTF(("WARNING: PRBS Check Lock - Digital Loopback and Link Training both are enabled\n"));
    }
  return (ERR_CODE_NONE);
}

/* Save prbs hardware timer config registers */
err_code_t blackhawk7_v1l8p1_get_prbs_chk_hw_timer_ctrl(srds_access_t *sa__, struct prbs_chk_hw_timer_ctrl_st * const prbs_chk_hw_timer_ctrl_bak) {
    INIT_SRDS_ERR_CODE
    ESTM(prbs_chk_hw_timer_ctrl_bak->prbs_chk_burst_err_cnt_en = rd_prbs_chk_burst_err_cnt_en());
    ESTM(prbs_chk_hw_timer_ctrl_bak->prbs_chk_en_timer_unit_sel = rd_prbs_chk_en_timer_unit_sel());
    ESTM(prbs_chk_hw_timer_ctrl_bak->prbs_chk_en_timer_unit_timeout = rd_prbs_chk_en_timer_unit_timeout());
    ESTM(prbs_chk_hw_timer_ctrl_bak->prbs_chk_new_timer_mode = rd_prbs_chk_new_timer_mode());
    return ERR_CODE_NONE;
}

/* Restore prbs hardware timer config registers */
err_code_t blackhawk7_v1l8p1_set_prbs_chk_hw_timer_ctrl(srds_access_t *sa__, struct prbs_chk_hw_timer_ctrl_st const * const prbs_chk_hw_timer_ctrl_bak) {
    INIT_SRDS_ERR_CODE
    EFUN(wr_prbs_chk_burst_err_cnt_en(prbs_chk_hw_timer_ctrl_bak->prbs_chk_burst_err_cnt_en));
    EFUN(wr_prbs_chk_en_timer_unit_sel(prbs_chk_hw_timer_ctrl_bak->prbs_chk_en_timer_unit_sel));
    EFUN(wr_prbs_chk_en_timer_unit_timeout(prbs_chk_hw_timer_ctrl_bak->prbs_chk_en_timer_unit_timeout));
    EFUN(wr_prbs_chk_new_timer_mode(prbs_chk_hw_timer_ctrl_bak->prbs_chk_new_timer_mode));
    return ERR_CODE_NONE;
}

/* Configure hardware timer to count prbs errors */
err_code_t blackhawk7_v1l8p1_config_prbs_chk_hw_timer(srds_access_t *sa__, uint16_t time_ms, uint16_t *time_ms_adjusted) {
    INIT_SRDS_ERR_CODE
    struct prbs_chk_hw_timer_ctrl_st prbs_chk_hw_timer_ctrl;
    ENULL_MEMSET(&prbs_chk_hw_timer_ctrl, 0, sizeof(struct prbs_chk_hw_timer_ctrl_st));

    EFUN(wr_prbs_chk_burst_err_cnt_en(0));

    /* Figure out timer mode and timer count */
    EFUN(blackhawk7_v1l8p1_INTERNAL_get_prbs_timeout_count_from_time(time_ms, time_ms_adjusted, &prbs_chk_hw_timer_ctrl));
    EFUN(wr_prbs_chk_en_timer_unit_sel(prbs_chk_hw_timer_ctrl.prbs_chk_en_timer_unit_sel));
    EFUN(wr_prbs_chk_en_timer_unit_timeout(prbs_chk_hw_timer_ctrl.prbs_chk_en_timer_unit_timeout));
    EFUN(wr_prbs_chk_new_timer_mode(1));
    return ERR_CODE_NONE;
}

err_code_t blackhawk7_v1l8p1_display_detailed_prbs_state_hdr(srds_access_t *sa__) {
    EFUN_PRINTF(("\nPRBS DETAILED DISPLAY :\n"));

/* Comment out display of PRBS burst error if BLACKHAWK7 & OSPREY7 */
    EFUN_PRINTF((" LN TX-Mode TX-PRBS-Inv TX-PMD-Inv RX-Mode RX-PRBS-Inv RX-PMD-Inv Lck LL PRBS-Err-Cnt"));
    EFUN_PRINTF(("     BER\n"));
    return (ERR_CODE_NONE);
}

const char* blackhawk7_v1l8p1_e2s_prbs_mode_enum[22] = {
    " PRBS_7",
    " PRBS_9",
    "PRBS_11",
    "PRBS_15",
    "PRBS_23",
    "PRBS_31",
    "PRBS_58",
    "PRBS_49",
    "PRBS_10",
    "PRBS_20",
    "PRBS_13",
    "  ERR  ",
    "  ERR  ",
    "  ERR  ",
    "  ERR  ",
    "USER_40",
    "  ERR  ",
    "  ERR  ",
    "  ERR  ",
    "  ERR  ",
    "AUTO_DT",
    "UNKNOWN"
  };
err_code_t blackhawk7_v1l8p1_display_detailed_prbs_state(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    uint32_t err_cnt = 0;
    uint8_t lock_lost = 0;
    uint8_t enabled;

    ESTM_PRINTF(("  %d ",blackhawk7_v1l8p1_acc_get_lane(sa__)));

    ESTM(enabled = rd_prbs_gen_en());
    if(enabled) {
        enum srds_prbs_polynomial_enum prbs_poly_mode = PRBS_7;
        uint8_t prbs_inv = 0;
        const char *prbs_string;
        EFUN(blackhawk7_v1l8p1_get_tx_prbs_config(sa__, &prbs_poly_mode, &prbs_inv));
        if(prbs_poly_mode == PRBS_UNKNOWN) {
            prbs_string = "UNKNOWN";
        }
        else {
            prbs_string =  (char *)blackhawk7_v1l8p1_e2s_prbs_mode_enum[prbs_poly_mode];
        }
        ESTM_PRINTF(("%s",prbs_string));
        ESTM_PRINTF(("      %1d     ",prbs_inv));
    } else {
        EFUN_PRINTF(("  OFF  "));
        ESTM_PRINTF(("      -     "));
    }
    ESTM_PRINTF(("     %1d     ",rd_tx_pmd_dp_invert()));

    ESTM(enabled = rd_prbs_chk_en());
    if(enabled) {
        enum srds_prbs_polynomial_enum prbs_poly_mode = PRBS_7;
        enum srds_prbs_checker_mode_enum prbs_checker_mode;
        uint8_t prbs_inv = 0;
        const char *prbs_string;
        EFUN(blackhawk7_v1l8p1_get_rx_prbs_config(sa__, &prbs_poly_mode, &prbs_checker_mode, &prbs_inv));
        if(prbs_poly_mode == PRBS_UNKNOWN) {
            prbs_string = "UNKNOWN";
        }
        else {
            prbs_string =  (char *)blackhawk7_v1l8p1_e2s_prbs_mode_enum[prbs_poly_mode];
        }
        ESTM_PRINTF((" %s",prbs_string));
        ESTM_PRINTF(("      %1d     ",prbs_inv));
    } else {
        EFUN_PRINTF(("   OFF  "));
        ESTM_PRINTF(("      -     "));
    }

    ESTM_PRINTF(("     %1d     ",rd_rx_pmd_dp_invert()));
    ESTM_PRINTF(("  %d ",rd_prbs_chk_lock()));
    EFUN(blackhawk7_v1l8p1_prbs_err_count_state(sa__,&err_cnt,&lock_lost));
    EFUN_PRINTF(("  %d  %010d ",lock_lost,err_cnt));
    EFUN(blackhawk7_v1l8p1_INTERNAL_display_BER(sa__,100));
    EFUN_PRINTF(("\n"));

    return (ERR_CODE_NONE);
}


/***********************************/
/*  PRBS Error Analyzer Functions  */
/***********************************/
/* Reset PRBS Error Analyzer */
err_code_t blackhawk7_v1l8p1_prbs_error_analyzer_reset(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE

    EFUN(wrc_tlb_err_analyze_en(0x0)); /* Disables error counting into the memory */
    EFUN(blackhawk7_v1l8p1_clear_dac_pattern_memory(sa__));  /* Clear DAC pattern memory */
    return (ERR_CODE_NONE);
}

/* Configure PRBS Error Analyzer */
err_code_t blackhawk7_v1l8p1_prbs_error_analyzer_config(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st *err_analyzer) {
    INIT_SRDS_ERR_CODE
    uint8_t  fec_size;
    uint16_t fec_size_bits;

    if(!err_analyzer) {
        return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if ((err_analyzer->prbs_err_fec_size < BLACKHAWK7_V1L8P1_PRBS_ERR_ANALYZER_FEC_SIZE_MIN) || (err_analyzer->prbs_err_fec_size > BLACKHAWK7_V1L8P1_PRBS_ERR_ANALYZER_FEC_SIZE_MAX)) {
        return (blackhawk7_v1l8p1_error(sa__, ERR_CODE_INVALID_PRBS_ERR_ANALYZER_FEC_SIZE));
    }

    fec_size = (uint8_t)((err_analyzer->prbs_err_fec_size + BLACKHAWK7_V1L8P1_PRBS_FEC_FRAME_SIZE_ROUND_UP) / BLACKHAWK7_V1L8P1_PRBS_FEC_FRAME_SIZE_MULTIPLIER);

    fec_size_bits = (uint16_t)(fec_size * BLACKHAWK7_V1L8P1_PRBS_FEC_FRAME_SIZE_MULTIPLIER);
    if (err_analyzer->prbs_err_fec_size != fec_size_bits) {
        err_analyzer->prbs_err_fec_size = fec_size_bits;
        EFUN_PRINTF(("\n << WARNING: FEC Frame size of %d bits NOT programmable. Instead setting frame size to %d bits >>\n", err_analyzer->prbs_err_fec_size, fec_size_bits));
    }

    /* Stop Error Analyzer counters */
    EFUN(wr_tlb_err_start_error_analyzer(0x0));
    EFUN(wrc_tlb_err_aggr_start_error_aggregation(0x0));

    EFUN(wr_tlb_err_analyze_lanes_active(0x1)); /* Enable lane */
    EFUN(wr_tlb_err_fec_size(fec_size));

    switch (err_analyzer->prbs_err_aggregate_mode) {
        case SRDS_IEEE_50GE: /* 50G w/ Bit Muxing 2:1 and No Code Word Interleaving 50GE/100GE 56G BHK */
           err_analyzer->num_of_aggregate_lanes = 1;
           err_analyzer->fec_size_frac = BLACKHAWK7_V1L8P1_PRBS_FEC_SIZE_FRAC_1_8;
           EFUN(wr_tlb_err_symbol_msb_lsb_group(0x1));
           EFUN(wr_tlb_err_fec_msb_en(0x1));
           EFUN(wr_tlb_err_fec_lsb_en(0x1));
           EFUN(wr_tlb_err_fec_l_en(0x1));
           EFUN(wr_tlb_err_fec_r_en(0x1));
           EFUN(wr_tlb_err_fec_rscode_swap_en(0x0));
           break;
        case SRDS_BHK_200GE:              /* 200G w/ Bit Muxing 2:1 and 2 Code Word Interleaving 200GE/400GE 56G BHK */
           err_analyzer->num_of_aggregate_lanes = 4;
           err_analyzer->fec_size_frac = BLACKHAWK7_V1L8P1_PRBS_FEC_SIZE_FRAC_1_4;
           EFUN(wr_tlb_err_symbol_msb_lsb_group(0x1));
           EFUN(wr_tlb_err_fec_msb_en(0x1));
           EFUN(wr_tlb_err_fec_lsb_en(0x0));
           EFUN(wr_tlb_err_fec_l_en(0x1));
           EFUN(wr_tlb_err_fec_r_en(0x1));
           EFUN(wr_tlb_err_fec_rscode_swap_en(0x1));
           break;
        case SRDS_CUSTOM:
           break;
        default:
           EFUN_PRINTF(("\n << WARNING: prbs_err_aggregate_mode needs to be a valid number\n"));
           return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    EFUN(wr_tlb_err_fec_size_frac(err_analyzer->fec_size_frac));    /* Set fraction to appropriate value */

    EFUN(wrc_tlb_err_analyze_counter_more_than_15_en(0x1)); /* Enable counters >15 errors */
    EFUN(wrc_tlb_err_analyze_counter_en(0xffff)); /* Enable all error counters */

    return (ERR_CODE_NONE);
}

/* Start each lane PRBS Error Analyzer */
err_code_t blackhawk7_v1l8p1_prbs_error_analyzer_start(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE

    EFUN(wrc_tlb_err_analyze_wclk_switch(0x1)); /* This switches the memory write clock to memory read clock */
    EFUN(wrc_tlb_err_analyze_en(0x1)); /* Enables error counting into the memory */
    /* Start Error Analyzer counters */
    EFUN(wr_tlb_err_start_error_analyzer(0x1));
    return (ERR_CODE_NONE);
}


/* Get PRBS Error Analyzer Config */
err_code_t blackhawk7_v1l8p1_get_prbs_error_analyzer_config(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st *err_analyzer) {
    INIT_SRDS_ERR_CODE

    uint16_t prbs_err_fec_size, prbs_err_fec_size_frac;

    if(!err_analyzer) {
        return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    ESTM(prbs_err_fec_size  = rd_tlb_err_fec_size());
    ESTM(prbs_err_fec_size_frac  = rd_tlb_err_fec_size_frac());
    err_analyzer->prbs_err_fec_size = (uint16_t)(prbs_err_fec_size * BLACKHAWK7_V1L8P1_PRBS_FEC_FRAME_SIZE_MULTIPLIER);
    err_analyzer->fec_size_frac = (uint8_t) prbs_err_fec_size_frac;
    return (ERR_CODE_NONE);
}

/* Display PRBS Error Analyzer Config */
err_code_t blackhawk7_v1l8p1_display_prbs_error_analyzer_config(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st *err_analyzer, uint32_t timeout_s) {
#if defined(SERDES_EVAL) & !defined(DIAG_STANDALONE)
    INIT_SRDS_ERR_CODE
#endif
    if(!err_analyzer) {
        return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    if (err_analyzer->diag_mode)
    {
        EFUN_PRINTF(("%u %hu %hhu %u %hhu %hhu %hhu %08x %08x\n",   timeout_s, err_analyzer->prbs_err_fec_size, err_analyzer->fec_size_frac, err_analyzer->prbs_err_aggregate_mode, err_analyzer->lanes_active, err_analyzer->diag_mode, blackhawk7_v1l8p1_get_lane(sa__), err_analyzer->pp_field2, err_analyzer->pp_field1));
    }
    else
    {
        EFUN_PRINTF(("\n ****************************************************************************** \n"));
        EFUN_PRINTF(("  PRBS Error Analyzer Config:\n"));
        EFUN_PRINTF((" ------------------------------------------------------------- \n"));
        EFUN_PRINTF(("    FEC Frame Size               = %4d bits\n",err_analyzer->prbs_err_fec_size));
        EFUN_PRINTF(("    Time Duration of Analysis    = %4d seconds\n",timeout_s));
#if defined(SERDES_EVAL) & !defined(DIAG_STANDALONE)
         /* DIAG_STANDALONE is defined for all daig_data_logfile_decoder topos.
            Since there is no availability real hardware while decoding memory dumps
            we need to exclude below register reads.
          */
        {
            uint16_t val;
            ESTM(val  = rd_tlb_err_fec_size());
            EFUN_PRINTF(("    tlb_err_fec_size             = %4d\n",val));
            ESTM(val  = rd_tlb_err_fec_size_frac());
            EFUN_PRINTF(("    tlb_err_fec_size_frac        = %4d\n",val));
            ESTM(val  = rd_tlb_err_symbol_msb_lsb_group());
            EFUN_PRINTF(("    tlb_err_symbol_msb_lsb_group = %4d\n",val));
            ESTM(val  = rd_tlb_err_fec_msb_en());
            EFUN_PRINTF(("    tlb_err_fec_msb_en           = %4d\n",val));
            ESTM(val  = rd_tlb_err_fec_lsb_en());
            EFUN_PRINTF(("    tlb_err_fec_lsb_en           = %4d\n",val));
            ESTM(val  = rd_tlb_err_fec_l_en());
            EFUN_PRINTF(("    tlb_err_fec_l_en             = %4d\n",val));
            ESTM(val  = rd_tlb_err_fec_r_en());
            EFUN_PRINTF(("    tlb_err_fec_r_en             = %4d\n",val));
            ESTM(val  = rd_tlb_err_fec_rscode_swap_en());
            EFUN_PRINTF(("    tlb_err_fec_rscode_swap_en   = %4d\n",val));
        }
#endif
        EFUN_PRINTF((" -------------------------------------------------------------\n"));
    }

    return (ERR_CODE_NONE);
}

/* Clear DAC pattern memory */
err_code_t blackhawk7_v1l8p1_clear_dac_pattern_memory(srds_access_t *sa__) {
    INIT_SRDS_ERR_CODE
    uint8_t i;

    /* Stop Error Analyzer counters */
    EFUN(wr_tlb_err_start_error_analyzer(0x0));
    EFUN(wrc_tlb_err_aggr_start_error_aggregation(0x0));
    EFUN(wrc_tlb_err_analyze_en(0x0)); /* Disables error counting into the memory */

    EFUN(wrc_tlb_err_analyze_wclk_switch(0x0));

    /* Set clock to comclk when writing to SC bit dac_test_mem_write_pulse.
       The following is added for Osprey7 and new Blackhawk7 cores. */
    EFUN(blackhawk7_v1l8p1_INTERNAL_clk4sync_div2_sequence(sa__, 0));

    EFUN(wrc_dac_test_mem_write_pulse(0));
    EFUN(wrc_dac_test_readback_en(0));
    EFUN(wrc_dac_test_read_en(0));
    EFUN(wrc_dac_test_test_en(0));
    EFUN(wrc_dac_test_lane_sel_en(0));

    EFUN(wrc_dac_test_lane_sel_en(1)); /* Enables tclk0 to be used by the memory for rclk and wclk. Need to set this clock to be the fastest tclk */
    EFUN(wrc_dac_test_test_en(1));

    /* Clear the registers */
    EFUN(wrc_patt_gen_seq_0(0));
    EFUN(wrc_patt_gen_seq_1(0));
    EFUN(wrc_patt_gen_seq_2(0));
    EFUN(wrc_patt_gen_seq_3(0));
    EFUN(wrc_patt_gen_seq_4(0));
    EFUN(wrc_patt_gen_seq_5(0));
    EFUN(wrc_patt_gen_seq_6(0));
    EFUN(wrc_patt_gen_seq_7(0));
    EFUN(wrc_patt_gen_seq_8(0));

    /* Write the data into the memory */
    for (i = 0; i < 50; i++) {
        EFUN(wrc_dac_test_mem_write_pulse(1));
    }


    /* Set clock to fast clock(clk4sync_div2) for running PRBS error analyzer.
       This is added for Osprey7 and new Blackhawk7 cores. */
    EFUN(blackhawk7_v1l8p1_INTERNAL_clk4sync_div2_sequence(sa__, 1));

    return (ERR_CODE_NONE);
}

/* Read DAC pattern memory into a 32xN memory */
err_code_t blackhawk7_v1l8p1_read_dac_pattern_memory(srds_access_t *sa__, uint32_t *dac_mem, uint8_t read_addr, uint8_t num_of_lines) {
    INIT_SRDS_ERR_CODE

     uint8_t i;
     uint16_t temp16 = 0;

     /* Disable error analyzer access to DAC memory */
     EFUN(wrc_tlb_err_analyze_en(0));
     EFUN(wrc_dac_test_test_en(0));

     /* Set up DAC memory for test */
     EFUN(wrc_tlb_err_analyze_wclk_switch(0x0));
     EFUN(wrc_dac_test_lane_sel_en(0));
     EFUN(wrc_dac_test_read_en(1));
     EFUN(wrc_dac_test_readback_en(1));
     EFUN(wrc_dac_test_test_en(1));

     /* Continuously read readback_data_8 until you reach the appropriate address */
     for (i = 0; i < read_addr; i++) {
       ESTM(temp16 = rdc_dac_test_readback_data_8());
     }

    /* Return the data from the desired location */
    for (i = 0; i < num_of_lines; i++) {
      ESTM(dac_mem[i*9]   = (uint32_t) rdc_dac_test_readback_data_0());
      ESTM(dac_mem[i*9+1] = (uint32_t) rdc_dac_test_readback_data_1());
      ESTM(dac_mem[i*9+2] = (uint32_t) rdc_dac_test_readback_data_2());
      ESTM(dac_mem[i*9+3] = (uint32_t) rdc_dac_test_readback_data_3());
      ESTM(dac_mem[i*9+4] = (uint32_t) rdc_dac_test_readback_data_4());
      ESTM(dac_mem[i*9+5] = (uint32_t) rdc_dac_test_readback_data_5());
      ESTM(dac_mem[i*9+6] = (uint32_t) rdc_dac_test_readback_data_6());
      ESTM(dac_mem[i*9+7] = (uint32_t) rdc_dac_test_readback_data_7());
      ESTM(dac_mem[i*9+8] = (uint32_t) rdc_dac_test_readback_data_8());

      if (PRBS_VERBOSE > 2)
         EFUN_PRINTF(("\t Line  %2d Data = 0x%x%08x%08x%08x%08x%08x%08x%08x%08x temp=%4x\n",i,dac_mem[i*9+8],dac_mem[i*9+7],dac_mem[i*9+6],dac_mem[i*9+5],dac_mem[i*9+4],dac_mem[i*9+3],dac_mem[i*9+2],dac_mem[i*9+1],dac_mem[i*9],temp16));
    }

     /* Reset DAC memory access */
     EFUN(wrc_dac_test_read_en(0));
     EFUN(wrc_dac_test_readback_en(0));
     EFUN(wrc_dac_test_test_en(0));

    return (ERR_CODE_NONE);
}

/* Get PRBS Error Analyzer Error Count from aggregate lane or lane n, where n can be 0..num_of_lanes */
/* (eg. for 8 lanes, you can do 0..8 where lanes 0..7 are lane-specific and lane 8 gives aggregate err count) */
err_code_t blackhawk7_v1l8p1_get_prbs_error_analyzer_lane_err_count(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st *err_analyzer, uint8_t lane) {
    INIT_SRDS_ERR_CODE

    int8_t   i;
    uint8_t num_lanes;
    uint32_t dacmem[36]  = {0}; /* 4 lines of memory for 16 err counters per lane, each mem read = 9 16-bit words, hence 9*4 = 36 */
    uint32_t dacmem2[18] = {0}; /* 1 line = 9 16-bit words, there are 4 words to read for 16 error counters per lane */
    uint32_t temp32;

    if(!err_analyzer) {
        return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    /* read the number of lanes from register */
    ESTM(num_lanes = rdc_revid_multiplicity());
    if (lane > num_lanes) {
        return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    /* Read DAC pattern memory for error counter 0-15 */
    EFUN(blackhawk7_v1l8p1_read_dac_pattern_memory(sa__,dacmem,(uint8_t)(4*lane),4));

    /* Check for saturation in 35-bit counters
     * example: if the most significant 3 bits of the counters are non-zero, the counters should be maxed out
     * Error counts: err_all=34:32, err0=69:67, err1=104:102, err2=139:137, ...etc
     * dacmem[2]=47:32,dacmem[3]=63:48,dacmem[4]=79:64,dacmem[5]=95:80,
     * dacmem[6]=111:96,dacmem[7]=127:112,dacmem[8]=143::128
     * Calculation for 32-bit counters
     * Error counts: err_all=31:0, err0=66:35, err1=101:70, err2=136:105, ...etc
     * dacmem[0]=15:0, dacmem[1]=31:16,dacmem[2]=47:32,dacmem[3]=63:48,dacmem[4]
     * =79:64,dacmem[5]=95:80,dacmem[6]=111:96,dacmem[7]=127:112,dacmem[8]=143::128 */

    if ((dacmem[2]&0x7)>0)    {
        err_analyzer->prbs_frames_all = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_frames_all = (dacmem[0]&0xffff)       | ((dacmem[1]<<16)&0xffff0000);
    }
    if ((dacmem[4]&0x38)>0)    {
        err_analyzer->prbs_errcnt[0]  = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[0] = ((dacmem[2]>>3)&0x1fff)   | ((dacmem[3]<<13)&0x1fffe000)    | ((dacmem[4]<<29)&0xe0000000);
    }
    if ((dacmem[6]&0x1c0)>0)    {
        err_analyzer->prbs_errcnt[1]  = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[1] = ((dacmem[4]>>6)&0x3ff)    | ((dacmem[5]<<10)&0x3fffc00)     | ((dacmem[6]<<26)&0xfc000000);
    }
    if ((dacmem[8]&0x0e00)>0)    {
        err_analyzer->prbs_errcnt[2]  = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[2] = ((dacmem[6]>>9)&0x7f)     | ((dacmem[7]<<7)&0x7fff80)       | ((dacmem[8]<<23)&0xff800000);
    }
    if ((dacmem[11]&0x7)>0)    {
        err_analyzer->prbs_errcnt[3]  = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[3] = (dacmem[9]&0xffff)        | ((dacmem[10]<<16)&0xffff0000);
    }
    if ((dacmem[13]&0x38)>0)    {
        err_analyzer->prbs_errcnt[4]  = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[4] = ((dacmem[11]>>3)&0x1fff)  | ((dacmem[12]<<13)&0x1fffe000)   | ((dacmem[13]<<29)&0xe0000000);
    }
    if ((dacmem[15]&0x1c0)>0)    {
        err_analyzer->prbs_errcnt[5]  = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[5] = ((dacmem[13]>>6)&0x3ff)   | ((dacmem[14]<<10)&0x3fffc00)    | ((dacmem[15]<<26)&0xfc000000);
    }
    if ((dacmem[17]&0x0e00)>0)    {
        err_analyzer->prbs_errcnt[6]  = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[6] = ((dacmem[15]>>9)&0x7f)    | ((dacmem[16]<<7)&0x7fff80)      | ((dacmem[17]<<23)&0xff800000);
    }
    if ((dacmem[20]&0x7)>0)    {
        err_analyzer->prbs_errcnt[7]  = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[7] = (dacmem[18]&0xffff)       | ((dacmem[19]<<16)&0xffff0000);
    }
    if ((dacmem[22]&0x38)>0)    {
        err_analyzer->prbs_errcnt[8]  = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[8] = ((dacmem[20]>>3)&0x1fff)  | ((dacmem[21]<<13)&0x1fffe000)   | ((dacmem[22]<<29)&0xe0000000);
    }
    if ((dacmem[24]&0x1c0)>0)    {
        err_analyzer->prbs_errcnt[9]  = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[9] = ((dacmem[22]>>6)&0x3ff)   | ((dacmem[23]<<10)&0x3fffc00)    | ((dacmem[24]<<26)&0xfc000000);
    }
    if ((dacmem[26]&0x0e00)>0)    {
        err_analyzer->prbs_errcnt[10] = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[10] = ((dacmem[24]>>9)&0x7f)   | ((dacmem[25]<<7)&0x7fff80)      | ((dacmem[26]<<23)&0xff800000);
    }
    if ((dacmem[29]&0x7)>0)    {
        err_analyzer->prbs_errcnt[11] = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[11] = (dacmem[27]&0xffff)      | ((dacmem[28]<<16)&0xffff0000);
    }
    if ((dacmem[31]&0x38)>0)    {
        err_analyzer->prbs_errcnt[12] = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[12] = ((dacmem[29]>>3)&0x1fff) | ((dacmem[30]<<13)&0x1fffe000)   | ((dacmem[31]<<29)&0xe0000000);
    }
    if ((dacmem[33]&0x1c0)>0)    {
        err_analyzer->prbs_errcnt[13] = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[13] = ((dacmem[31]>>6)&0x3ff)  | ((dacmem[32]<<10)&0x3fffc00)    | ((dacmem[33]<<26)&0xfc000000);
    }
    if ((dacmem[35]&0x0e00)>0)    {
        err_analyzer->prbs_errcnt[14] = 0xFFFFFFFF;
    } else{
        err_analyzer->prbs_errcnt[14] = ((dacmem[33]>>9)&0x7f)   | ((dacmem[34]<<7)&0x7fff80)      | ((dacmem[35]<<23)&0xff800000);
    }


    /* Read DAC pattern memory for error counter >15 */
    blackhawk7_v1l8p1_read_dac_pattern_memory(sa__,dacmem2,(uint8_t)(4*(num_lanes+1)),2);

    /* Allocate correct error counts for lanes and find saturation for >15 errors */
    if (lane==0) {
         if ((dacmem2[2]&0x7)>0) {
             err_analyzer->prbs_errcnt[15] = 0xFFFFFFFF;
         }
         else {
             err_analyzer->prbs_errcnt[15] = (dacmem2[0]&0xffff)       | ((dacmem2[1]<<16)&0xffff0000);
         }
    }
    else if (lane==1) {
         if ((dacmem2[4]&0x38)>0) {
             err_analyzer->prbs_errcnt[15]  = 0xFFFFFFFF;
         }
         else {
             err_analyzer->prbs_errcnt[15] = ((dacmem2[2]>>3)&0x1fff)   | ((dacmem2[3]<<13)&0x1fffe000)    | ((dacmem2[4]<<29)&0xe0000000);
         }
    }
    else if (lane==2) {
         if ((dacmem2[6]&0x1c0)>0) {
             err_analyzer->prbs_errcnt[15]  = 0xFFFFFFFF;
         }
         else {
             err_analyzer->prbs_errcnt[15] = ((dacmem2[4]>>6)&0x3ff)    | ((dacmem2[5]<<10)&0x3fffc00)     | ((dacmem2[6]<<26)&0xfc000000);
         }
    }
    else if (lane==3) {
         if ((dacmem2[8]&0x0e00)>0) {
             err_analyzer->prbs_errcnt[15]  = 0xFFFFFFFF;
         }
         else {
             err_analyzer->prbs_errcnt[15] = ((dacmem2[6]>>9)&0x7f)     | ((dacmem2[7]<<7)&0x7fff80)       | ((dacmem2[8]<<23)&0xff800000);
         }
    }
    else if (lane==4) {
         if ((dacmem2[11]&0x7)>0) {
             err_analyzer->prbs_errcnt[15]  = 0xFFFFFFFF;
         }
         else {
             err_analyzer->prbs_errcnt[15] = (dacmem2[9]&0xffff)        | ((dacmem2[10]<<16)&0xffff0000);
         }
    }
    else if (lane==5) {
         if ((dacmem2[13]&0x38)>0) {
             err_analyzer->prbs_errcnt[15]  = 0xFFFFFFFF;
         }
         else {
             err_analyzer->prbs_errcnt[15] = ((dacmem2[11]>>3)&0x1fff)  | ((dacmem2[12]<<13)&0x1fffe000)   | ((dacmem2[13]<<29)&0xe0000000);
         }
    }
    else if (lane==6) {
         if ((dacmem2[15]&0x1c0)>0) {
             err_analyzer->prbs_errcnt[15]  = 0xFFFFFFFF;
         }
         else {
             err_analyzer->prbs_errcnt[15] = ((dacmem2[13]>>6)&0x3ff)   | ((dacmem2[14]<<10)&0x3fffc00)    | ((dacmem2[15]<<26)&0xfc000000);
         }
    }
    else if (lane==7) {
         if ((dacmem2[17]&0x0e00)>0) {
             err_analyzer->prbs_errcnt[15]  = 0xFFFFFFFF;
         }
         else {
             err_analyzer->prbs_errcnt[15] = ((dacmem2[15]>>9)&0x7f)    | ((dacmem2[16]<<7)&0x7fff80)      | ((dacmem2[17]<<23)&0xff800000);
         }
    }
    /* TBD for aggregate lane */
    else {
         err_analyzer->prbs_errcnt[15] = 0;
    }

    if (PRBS_VERBOSE > 2) {
        EFUN_PRINTF(("\n\n\t=====> DEBUG INFO (start) [Core = %d, Lane = %d]\n", blackhawk7_v1l8p1_acc_get_core(sa__), lane));
        EFUN_PRINTF(("\n\t << Frame Errors accumulated in the DAC Pattern Memory >>\n"));
        EFUN_PRINTF(("\t FEC frames with    0 Errors = %u\n",err_analyzer->prbs_frames_all));
        for (i=0; i<BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS; i++) {
            if (i < (BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS-1)) {
                EFUN_PRINTF(("\t FEC frames with   %2d Errors = %u\n",i+1,err_analyzer->prbs_errcnt[i]));
            }
            else {
                EFUN_PRINTF(("\t FEC frames with > %2d Errors = %u\n",(BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS-1),err_analyzer->prbs_errcnt[i]));
            }
        }
    }

    /* Calculating cumulative error count for respective values of "t" */
    for (i=(BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS-1); i >= 0; i--) {
        if (i!=(BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS-1)) {
            if (err_analyzer->prbs_errcnt[i+1] == 0xFFFFFFFF) {
                err_analyzer->prbs_errcnt[i] = 0xFFFFFFFF;
                if (PRBS_VERBOSE > 2) {
                    EFUN_PRINTF(("\t err_analyzer->prbs_errcnt[%d] == MAX\n", i));
                }
            }
            else {
                /* Check for saturation while accumulating from histogram bins */
                temp32 = 0xFFFFFFFF - err_analyzer->prbs_errcnt[i];
                if ((err_analyzer->prbs_errcnt[i] == 0xFFFFFFFF) || (temp32 <= err_analyzer->prbs_errcnt[i+1])) {
                      err_analyzer->prbs_errcnt[i] = 0xFFFFFFFF;
                      if (PRBS_VERBOSE > 2) {
                          EFUN_PRINTF(("\t err_analyzer->prbs_errcnt[%d] == MAX\n", i));
                      }
                }
                else
                      err_analyzer->prbs_errcnt[i] = (err_analyzer->prbs_errcnt[i] + err_analyzer->prbs_errcnt[i+1]);
            }
        }
    }

    /* Calculating the number of all frames */
    if (err_analyzer->prbs_errcnt[0] == 0xFFFFFFFF) {
        err_analyzer->prbs_frames_all = 0xFFFFFFFF;
        if (PRBS_VERBOSE > 2) {
            EFUN_PRINTF(("\t err_analyzer->prbs_frames_all == MAX\n"));
        }
    }
    else {
        /* Check for saturation while accumulating from histogram bins */
        temp32 = 0xFFFFFFFF - err_analyzer->prbs_frames_all;
        if ((err_analyzer->prbs_frames_all == 0xFFFFFFFF) || (temp32 <= err_analyzer->prbs_errcnt[0])) {
              err_analyzer->prbs_frames_all = 0xFFFFFFFF;
              if (PRBS_VERBOSE > 2) {
                  EFUN_PRINTF(("\t err_analyzer->prbs_frames_all == MAX\n"));
              }
        }
        else
              err_analyzer->prbs_frames_all = (err_analyzer->prbs_frames_all + err_analyzer->prbs_errcnt[0]);
    }

    if (PRBS_VERBOSE > 2) {
        EFUN_PRINTF(("\n\t=====> DEBUG INFO (end)\n\n"));
    }


    return (ERR_CODE_NONE);
}


/* Get PRBS Error Analyzer Error Count */
err_code_t blackhawk7_v1l8p1_get_prbs_error_analyzer_err_count(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st *err_analyzer) {
    INIT_SRDS_ERR_CODE

    int8_t   i;
    uint8_t  prbs_lock, lane;

    if(!err_analyzer) {
        return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    /* Need these runtime info for post processing */
    ESTM(err_analyzer->pp_field2 = blackhawk7_v1l8p1_acc_get_lane(sa__));
    EFUN(blackhawk7_v1l8p1_INTERNAL_get_num_bits_per_ms(sa__, 1, &err_analyzer->pp_field1));

    ESTM(prbs_lock = rd_prbs_chk_lock());
    if(!prbs_lock) {
        EFUN_PRINTF(("\nERROR : PRBS Checker is not locked for Lane %d\n",blackhawk7_v1l8p1_acc_get_lane(sa__)));
        for (i=0; i<BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS; i++) {
            err_analyzer->prbs_errcnt[i] = 0xFFFFFFFF;
        }
        err_analyzer->prbs_frames_all = 0xFFFFFFFF;
        return (ERR_CODE_NONE);
    }

    ESTM(lane = blackhawk7_v1l8p1_acc_get_lane(sa__));

    /* Stop Error Analyzer */
    EFUN(wr_tlb_err_start_error_analyzer(0));

    EFUN(blackhawk7_v1l8p1_get_prbs_error_analyzer_lane_err_count(sa__, err_analyzer, lane));

    return (ERR_CODE_NONE);
}

/* Get PRBS Error Analyzer Aggregate Error Count */
err_code_t blackhawk7_v1l8p1_get_prbs_error_analyzer_aggregate_err_count(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st *err_analyzer) {
    INIT_SRDS_ERR_CODE

    uint8_t num_lanes, lane;
    uint8_t  prbs_lock;
    int8_t i;

    if(!err_analyzer) {
        return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    /* read the number of lanes from register */
    ESTM(num_lanes = rdc_revid_multiplicity());

    for(lane = 0;lane<num_lanes;lane++) {
        if ((err_analyzer->lane_mask_aggregate >> lane) & 0x1) {
            EFUN(blackhawk7_v1l8p1_acc_set_lane(sa__,lane));
            /* Stop error analyzer on the lane */
            EFUN(wr_tlb_err_start_error_analyzer(0));
            ESTM(prbs_lock = rd_prbs_chk_lock());
            if(!prbs_lock) {
                EFUN_PRINTF(("\nERROR : PRBS Checker is not locked for Lane %d\n",blackhawk7_v1l8p1_acc_get_lane(sa__)));
                for (i=0; i<BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS; i++) {
                    err_analyzer->prbs_errcnt[i] = 0xFFFFFFFF;
                }
                err_analyzer->prbs_frames_all = 0xFFFFFFFF;
                return (ERR_CODE_NONE);
            }
        }
    }

    ESTM(lane = blackhawk7_v1l8p1_acc_get_lane(sa__));

    /* Stop Error Analyzer aggregation */
    EFUN(wrc_tlb_err_aggr_start_error_aggregation(0x0));

    EFUN(blackhawk7_v1l8p1_get_prbs_error_analyzer_lane_err_count(sa__, err_analyzer, num_lanes)); /* For aggregate counters, just pass the num_lanes to point to the correct memory location in the DAC */

    return (ERR_CODE_NONE);
}

static err_code_t _blackhawk7_v1l8p1_display_fec_frame_error(srds_access_t *sa__, uint8_t errs_per_frame, uint32_t num_frame_errs) {
    if (num_frame_errs == 0xFFFFFFFF) {
        EFUN_PRINTF(("    FEC Frames with > %2d Errors (t=%2d) =        MAX \n",errs_per_frame,errs_per_frame));
    }
    else {
        EFUN_PRINTF(("    FEC Frames with > %2d Errors (t=%2d) = %10u \n",errs_per_frame,errs_per_frame,num_frame_errs));
    }
    return (ERR_CODE_NONE);
}

/* PRBS Error Analyzer Display */
err_code_t blackhawk7_v1l8p1_display_prbs_error_analyzer_err_count(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st *err_analyzer) {
    INIT_SRDS_ERR_CODE
    uint8_t i;

    if (err_analyzer->diag_mode)
    {
        for (i=0; i<(BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS-1); i++) {
            EFUN_PRINTF(("%08x,", err_analyzer->prbs_errcnt[i]));
        }
        EFUN_PRINTF(("%08x\n", err_analyzer->prbs_errcnt[BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS-1]));
        EFUN_PRINTF(("%08x\n", err_analyzer->prbs_frames_all));
    }
    else
    {
        /* Display PRBS Error Analyzer Err_Counts */
        EFUN_PRINTF(("\n -------------------------------------------------------------\n"));
        EFUN_PRINTF(("  PRBS Error Analyzer Error_Counts for Lane %d:\n", err_analyzer->pp_field2));
        EFUN_PRINTF((" -------------------------------------------------------------\n"));

        for (i=0; i<BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS; i++) {
            EFUN(_blackhawk7_v1l8p1_display_fec_frame_error(sa__, i, err_analyzer->prbs_errcnt[i]));
        }
        EFUN_PRINTF((" -------------------------------------------------------------\n"));
    }

    return (ERR_CODE_NONE);
}

/* PRBS Error Analyzer Aggregate error count Display */
err_code_t blackhawk7_v1l8p1_display_prbs_error_analyzer_aggregate_err_count(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st *err_analyzer) {
    INIT_SRDS_ERR_CODE
    uint8_t i;

    /* Display PRBS Error Analyzer Aggregate Err_Counts */
    EFUN_PRINTF(("\n -------------------------------------------------------------\n"));
    EFUN_PRINTF(("  PRBS Error Analyzer Aggregate Error_Counts\n"));
    EFUN_PRINTF((" -------------------------------------------------------------\n"));

    for (i=0; i<BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS; i++) {
        EFUN(_blackhawk7_v1l8p1_display_fec_frame_error(sa__, i, err_analyzer->prbs_errcnt[i]));
    }
    EFUN_PRINTF((" -------------------------------------------------------------\n"));

    return (ERR_CODE_NONE);
}

/* Display PRBS Error Analyzer Config and Errors for diagnostic dump*/
err_code_t blackhawk7_v1l8p1_display_prbs_error_analyzer_struct(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st *err_analyzer, uint32_t timeout_s) {
    INIT_SRDS_ERR_CODE
    if (err_analyzer->diag_mode)
    {
        /* Start of data dump */
        EFUN_PRINTF(("SERDES PRBS ERR ANALYZER DATA DUMP START\n"));
    }
    /* Display PRBS Error Analyzer Config */
    EFUN(blackhawk7_v1l8p1_display_prbs_error_analyzer_config(sa__, err_analyzer, timeout_s));
    /* Display PRBS Error Analyzer Err_Counts */
    EFUN(blackhawk7_v1l8p1_display_prbs_error_analyzer_err_count(sa__, err_analyzer));
    if (err_analyzer->diag_mode)
    {
        /* End of data dump */
        EFUN_PRINTF(("SERDES PRBS ERR ANALYZER DATA DUMP END\n"));
    }

    return (ERR_CODE_NONE);
}

/* Configure PRBS Error Analyzer for aggregate mode */
err_code_t blackhawk7_v1l8p1_prbs_error_analyzer_aggregate_config(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st *err_analyzer) {
    INIT_SRDS_ERR_CODE
    uint8_t num_lanes, lane;
    /* uint8_t aggr_mode; */

    if(!err_analyzer) {
        return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
    }

    /* read the number of lanes from register */
    ESTM(num_lanes = rdc_revid_multiplicity());

    for(lane = 0;lane<num_lanes;lane++) {
        if ((err_analyzer->lane_mask_aggregate >> lane) & 0x1) {
            EFUN(blackhawk7_v1l8p1_acc_set_lane(sa__,lane));
            EFUN(blackhawk7_v1l8p1_prbs_error_analyzer_config(sa__, err_analyzer));  /* Configure PRBS Error Analyzer */
            EFUN(wr_tlb_err_aggr_lanes_active(0x1)); /* Enable selected lane for aggregation */
        }
    }

    EFUN(wrc_tlb_err_aggr_en_force(0x1));

    if (err_analyzer->num_of_aggregate_lanes == 8)
       EFUN(wrc_tlb_err_aggr_mode(0x3));
    if (err_analyzer->num_of_aggregate_lanes == 4)
       EFUN(wrc_tlb_err_aggr_mode(0x2));
    if (err_analyzer->num_of_aggregate_lanes == 2)
       EFUN(wrc_tlb_err_aggr_mode(0x1));
    else
       EFUN(wrc_tlb_err_aggr_mode(0x0));

    return (ERR_CODE_NONE);
}

/* Display PRBS Error Analyzer Projection */
err_code_t blackhawk7_v1l8p1_display_prbs_error_analyzer_proj(srds_access_t *sa__, uint16_t prbs_err_fec_size, uint32_t timeout_s) {
    INIT_SRDS_ERR_CODE
    uint8_t lane_mask_aggregate;

    if (timeout_s == 0) {
        EFUN_PRINTF(("\nERROR: timeout_s value cannot be 0 for Lane %d >>\n", blackhawk7_v1l8p1_acc_get_lane(sa__)));
        return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }

    ESTM(lane_mask_aggregate = (uint8_t)(0x1 << (blackhawk7_v1l8p1_acc_get_lane(sa__))));

    /* Call the aggregate function to display error analyzer projections */
    EFUN(blackhawk7_v1l8p1_display_aggregate_prbs_error_analyzer_proj(sa__, prbs_err_fec_size, SRDS_IEEE_50GE, lane_mask_aggregate, timeout_s, 0x0));
    return (ERR_CODE_NONE);
}

/* Display Aggregate PRBS Error Analyzer Projection */
err_code_t blackhawk7_v1l8p1_display_aggregate_prbs_error_analyzer_proj(srds_access_t *sa__, uint16_t prbs_err_fec_size, enum srds_prbs_error_analyzer_aggregate_mode_enum prbs_err_aggregate_mode, uint8_t lane_mask_aggregate, uint32_t timeout_s, uint8_t diag_mode) {
    INIT_SRDS_ERR_CODE
    uint8_t  hrs, mins, secs;
    uint8_t num_lanes, lane;
    uint8_t  prbs_lock;
    int8_t i;
    blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st err_analyzer;

    if (timeout_s == 0) {
        EFUN_PRINTF(("\nERROR: timeout_s value cannot be 0 for Lane %d >>\n", blackhawk7_v1l8p1_acc_get_lane(sa__)));
        return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }

    /* read the number of lanes from register */
    ESTM(num_lanes = rdc_revid_multiplicity());

    /* Start out by checking for PRBS lock on all the lanes selected for aggregation */
    for(lane = 0;lane<num_lanes;lane++) {
        if ((lane_mask_aggregate >> lane) & 0x1) {
            EFUN(blackhawk7_v1l8p1_acc_set_lane(sa__,lane));
            ESTM(prbs_lock = rd_prbs_chk_lock());
            if(!prbs_lock) {
                EFUN_PRINTF(("\nERROR : PRBS Checker is not locked for RX lane %d. Cannot run aggregation.\n",lane));
                return (ERR_CODE_NONE);
            }
        }
    }

    ENULL_MEMSET(&err_analyzer, 0, sizeof(blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st));
    err_analyzer.prbs_err_fec_size = prbs_err_fec_size;
    err_analyzer.prbs_err_aggregate_mode = prbs_err_aggregate_mode;
    err_analyzer.lane_mask_aggregate = lane_mask_aggregate;
    err_analyzer.diag_mode         = diag_mode;

    EFUN(blackhawk7_v1l8p1_prbs_error_analyzer_reset(sa__));  /* Reset PRBS Error Analyzer and clear DAC */
    EFUN(blackhawk7_v1l8p1_prbs_error_analyzer_aggregate_config(sa__, &err_analyzer));  /* Configure PRBS Error Analyzer Aggregation Mode*/

    EFUN(wrc_tlb_err_aggr_start_error_aggregation(0x1));   /* Start PRBS Error Analyzer aggregate counters before the inidviual lane counters */

    for(lane = 0;lane<num_lanes;lane++) {
        if ((err_analyzer.lane_mask_aggregate >> lane) & 0x1) {
            EFUN(blackhawk7_v1l8p1_acc_set_lane(sa__,lane));
            EFUN(blackhawk7_v1l8p1_prbs_error_analyzer_start(sa__));  /* Start PRBS Error Analyzer */
        }
    }

    EFUN(blackhawk7_v1l8p1_INTERNAL_seconds_to_displayformat(timeout_s, &hrs, &mins, &secs));
    EFUN_PRINTF((" \n Waiting for PRBS Error Analyzer measurement: time approx %d seconds (%d hr:%d mins: %ds) ",timeout_s,hrs,mins,secs));

    /* Wait for required duration and accumulate errors */
    EFUN(USR_DELAY_MS(((uint32_t)timeout_s) * 1000));
    EFUN_PRINTF(("\n"));

    /* Display PRBS Error Analyzer Err_Counts and projections */
    for(lane = 0;lane<num_lanes;lane++) {
        if ((err_analyzer.lane_mask_aggregate >> lane) & 0x1) {
            EFUN(blackhawk7_v1l8p1_acc_set_lane(sa__,lane));
            /* Clear out prbs err counts from previous lane */
            for (i = 0; i < BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS; i++) {
               err_analyzer.prbs_errcnt[i] = 0;
            }
            err_analyzer.prbs_frames_all = 0;
            EFUN(blackhawk7_v1l8p1_get_prbs_error_analyzer_err_count(sa__, &err_analyzer));
            /* Display PRBS Error Analyzer Config */
            EFUN(blackhawk7_v1l8p1_display_prbs_error_analyzer_struct(sa__, &err_analyzer, timeout_s));
            EFUN(blackhawk7_v1l8p1_prbs_error_analyzer_compute_proj(sa__, err_analyzer, timeout_s));
        }
    }
    if (err_analyzer.num_of_aggregate_lanes > 1)
    {
       /* Clear out prbs err counts from previous lane */
       for (i = 0; i < BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS; i++) {
          err_analyzer.prbs_errcnt[i] = 0;
       }
       err_analyzer.prbs_frames_all = 0;
       EFUN(blackhawk7_v1l8p1_get_prbs_error_analyzer_aggregate_err_count(sa__, &err_analyzer));
       EFUN(blackhawk7_v1l8p1_display_prbs_error_analyzer_aggregate_err_count(sa__, &err_analyzer));
       EFUN(blackhawk7_v1l8p1_prbs_error_analyzer_compute_proj(sa__, err_analyzer, timeout_s));
    }

    return (ERR_CODE_NONE);
}


/*----------------------------------------------------------------------------------------------------------------*/
/* Following calculations are used for projecting the Bit Error Rate (BER) from the PRBS Error Analyzer result    */
/*  - test_time_in_bits     = test_time_ms * num_bits_per_ms                                                      */
/*  - num_frames_in_test    = test_time_in_bits / (num_bits_per_frame * frame_overlap_ratio)                      */
/*  - frame_err_rate  (FER) = num_frame_errors / num_frames_in_test                                               */
/*  - bit_err_rate    (BER) = FER / num_bits_per_frame                                                            */
/*                  => BER  = (num_frame_errors * frame_overlap_ratio) / (test_time_ms * num_bits_per_ms)         */
/*                            where, frame_overlap_ratio = 1 / frame_overlap_factor                               */
/*                                                                                                                */
/* Following guidelines are used while computing projected values -                                               */
/*   i) Only "frame_errors" that are in between "10 and 65535" are considered as valid data points for projection */
/*  ii) Need atleast 2 valid data points to generated projected data                                              */
/*----------------------------------------------------------------------------------------------------------------*/
err_code_t blackhawk7_v1l8p1_prbs_error_analyzer_report_proj(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st err_analyzer, uint32_t timeout_s, blackhawk7_v1l8p1_prbs_err_analyzer_report_st *err_analyzer_report) {
#ifdef SERDES_API_FLOATING_POINT
    INIT_SRDS_ERR_CODE
    uint8_t    delta_n;
    uint8_t     i;
    uint8_t    overlap_factor;
    uint8_t    start_idx, stop_idx;
    uint32_t   num_bits_per_ms;
    USR_DOUBLE meas_fec_ber[BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS];
    USR_DOUBLE proj_ber[16];
    USR_DOUBLE x, y;
    USR_DOUBLE Exy = 0.0;
    USR_DOUBLE Eyy = 0.0;
    USR_DOUBLE Exx = 0.0;
    USR_DOUBLE Ey  = 0.0;
    USR_DOUBLE Ex  = 0.0;
    USR_DOUBLE alpha = 0.0, alpha2 = 0.0, sq_r = 0.0;
    USR_DOUBLE beta = 0.0;

    if (timeout_s == 0) {
        EFUN_PRINTF(("\nERROR: timeout_s value cannot be 0 for Lane %d >>\n", blackhawk7_v1l8p1_acc_get_lane(sa__)));
        return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }

    ENULL_MEMSET(err_analyzer_report, 0, sizeof(blackhawk7_v1l8p1_prbs_err_analyzer_report_st));
    EFUN(blackhawk7_v1l8p1_INTERNAL_get_num_bits_per_ms(sa__, 1, &num_bits_per_ms));

    if (err_analyzer.fec_size_frac == BLACKHAWK7_V1L8P1_PRBS_FEC_SIZE_FRAC_1_8)
        overlap_factor = 8;
    else if (err_analyzer.fec_size_frac == BLACKHAWK7_V1L8P1_PRBS_FEC_SIZE_FRAC_1_4)
        overlap_factor = 4;
    else if (err_analyzer.fec_size_frac == BLACKHAWK7_V1L8P1_PRBS_FEC_SIZE_FRAC_1_2)
        overlap_factor = 2;
    else
        overlap_factor = 1;

    if (PRBS_VERBOSE > 2) {
        EFUN_PRINTF(("\n\t=====> DEBUG INFO (start)\n"));
        EFUN_PRINTF(("\n\t << Measured Equivalent BER for specific error correcting FEC >>\n"));
    }

    /* Loop to calculate the measured BER based on above mentioned calculations */
    for(i=0; i < BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS; i++) {
        if (num_bits_per_ms !=0) {
            meas_fec_ber[i] = ((((USR_DOUBLE)(err_analyzer.prbs_errcnt[i]) / (USR_DOUBLE) overlap_factor) / (USR_DOUBLE)(timeout_s * 1000)) / (USR_DOUBLE) num_bits_per_ms);
        }
        else {
            meas_fec_ber[i] = (USR_DOUBLE)0xFFFFFFFF;
        }
        if (PRBS_VERBOSE > 2) {
            EFUN_PRINTF(("\t Measured Equivalent BER at 't=%2d' (((frameErr/overlap_factor)/time_ms)/rate) = (((%u/%u)/%d)/%d): %0.3e \n",i, err_analyzer.prbs_errcnt[i],
                                                                                overlap_factor, timeout_s*1000, num_bits_per_ms, meas_fec_ber[i]));
        }
    }

    /* Calculating number of MEASURED points available for extrapolation */
    /* start_idx - first non-max FEC frame error value; stop_idx - index where FEC frame errors < 10 errors */
    start_idx = 0;
    stop_idx  = 0;
    if (err_analyzer.prbs_errcnt[BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS-1] != 0xFFFFFFFF) {
        stop_idx  = BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS-1;
    }
    for (i=BLACKHAWK7_V1L8P1_PRBS_NUM_OF_ERROR_ANALYZER_COUNTERS; i > 0; i--) {
        if (err_analyzer.prbs_errcnt[i - 1] != 0xFFFFFFFF) {
            start_idx = (uint8_t)(i - 1);
        }
        if (err_analyzer.prbs_errcnt[i- 1] < ((uint32_t)(10*overlap_factor))) { /* "10*overlap_factor" means "10 error events" */
            stop_idx = (uint8_t)(i - 1);
        }
    }
    delta_n = (uint8_t)(stop_idx - start_idx);                                                  /* Number of MEASURED points available for extrapolation */

    /* Compute covariance and mean for extrapolation */
    if (delta_n >= 2) {                                                              /* There are at least 2 points to trace a line for extrapolation */
        for(i = start_idx; i < stop_idx; i++) {
            x    = (USR_DOUBLE) (i);
            y    = pow((-log10(meas_fec_ber[i])),BLACKHAWK7_V1L8P1_PRBS_ERR_ANALYZER_LINEARITY_ADJUST);
            Exy += ((x * y)/(USR_DOUBLE)delta_n);
            Eyy += ((y * y)/(USR_DOUBLE)delta_n);
            Exx += ((x * x)/(USR_DOUBLE)delta_n);
            Ey  += ((  y  )/(USR_DOUBLE)delta_n);
            Ex  += ((  x  )/(USR_DOUBLE)delta_n);
        }

        /* Compute fit slope and offset: ber = alpha*frame_err_thresh + beta */
        alpha = (Exy - Ey*Ex) / (Exx - Ex*Ex);
        beta  = Ey - Ex*alpha;
        /* Compute alpha2: slope of regression: frame_err_thresh = alpha2*ber + beta2 */
        alpha2 = (Exy - Ey*Ex) / (Eyy - Ey*Ey);
        /* Compute correlation index sq_r */
        sq_r = alpha*alpha2;
    }

    if (PRBS_VERBOSE > 2) {
        EFUN_PRINTF(("\n\t << Computing linear fit from Measured Equivalent BER points >>\n"));
        EFUN_PRINTF(("\t Number of measured points used for extrapolation = %d\n",delta_n));
        EFUN_PRINTF(("\t Exy=%lf, Eyy=%lf, Exx=%lf, Ey=%lf, Ex=%lf\n",Exy,Eyy,Exx,Ey,Ex));
        EFUN_PRINTF(("\t alpha=%lf, beta=%lf\n",alpha,beta));
        EFUN_PRINTF(("\t sq_r=%lf\n",sq_r));
        EFUN_PRINTF(("\n\t=====> DEBUG INFO (end)\n\n"));
    }


    /* Calulate Projected BER => Equivalent projected post-FEC BER for t=15 */
    if (delta_n >= 2) {
        proj_ber[15] = ((alpha * 15) + beta);
        proj_ber[15] = pow(proj_ber[15],1.0/BLACKHAWK7_V1L8P1_PRBS_ERR_ANALYZER_LINEARITY_ADJUST);
        proj_ber[15] = pow(10,-proj_ber[15]);
    }
    else {
        proj_ber[15] = 0.0;
    }



    /* Populate output structure */
    err_analyzer_report->proj_ber  = proj_ber[15];
    err_analyzer_report->ber_proj_invalid  = 0;       /* To be populated: 0 -> valid, +1 -> BER greater than; -1 -> BER less than */
    err_analyzer_report->delta_n   = delta_n;
    return (ERR_CODE_NONE);
#else
    UNUSED(err_analyzer);
    UNUSED(timeout_s);
    UNUSED(*err_analyzer_report);
    EFUN_PRINTF(("This function needs SERDES_API_FLOATING_POINT define to operate \n"));
    return(blackhawk7_v1l8p1_error(sa__, ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
#endif /* SERDES_API_FLOATING_POINT */
}


err_code_t blackhawk7_v1l8p1_prbs_error_analyzer_compute_proj(srds_access_t *sa__, blackhawk7_v1l8p1_prbs_err_analyzer_config_status_st err_analyzer, uint32_t timeout_s) {

#ifdef SERDES_API_FLOATING_POINT
    INIT_SRDS_ERR_CODE
    blackhawk7_v1l8p1_prbs_err_analyzer_report_st err_analyzer_rep;

    if (timeout_s == 0) {
        EFUN_PRINTF(("\nERROR: timeout_s value cannot be 0 for Lane %d >>\n", blackhawk7_v1l8p1_acc_get_lane(sa__)));
        return (ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }

    EFUN(blackhawk7_v1l8p1_prbs_error_analyzer_report_proj(sa__, err_analyzer, timeout_s,  &err_analyzer_rep));


    /* Displaying Projected BER => Equivalent projected post-FEC BER for t=15 */
    if (err_analyzer_rep.delta_n >= 2) {
        EFUN_PRINTF(("\n  PRBS Error Analyzer Projected BER (Equivalent projected post-FEC BER for t=15) for Lane %d = %0.3e\n\n",blackhawk7_v1l8p1_acc_get_lane(sa__), err_analyzer_rep.proj_ber));
    }
    else {
        EFUN_PRINTF(("\n << WARNING: Not enough valid measured points available for BER Projection for Lane %d ! >>\n\n",blackhawk7_v1l8p1_acc_get_lane(sa__)));
    }

#else
    UNUSED(err_analyzer);
    UNUSED(timeout_s);
#endif /* SERDES_API_FLOATING_POINT */


    return (ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p1_get_rx_tuning_status(srds_access_t *sa__, uint8_t *status) {
    INIT_SRDS_ERR_CODE
    ESTM(*status = ((rdv_opt_err_pam4_prbsfb_byte_byte() & 0x40) == 0x40));
    return(ERR_CODE_NONE);
}


err_code_t blackhawk7_v1l8p1_config_BER_HW_mode(srds_access_t *sa__, uint16_t time_ms, struct ber_data_st *ber_data, struct prbs_chk_hw_timer_ctrl_st *prbs_chk_hw_timer_ctrl) {
    INIT_SRDS_ERR_CODE
    uint8_t lcklost = 0;
    uint32_t err_cnt= 0;
    uint8_t clk_gate = 0;
    uint8_t prbs_chk_lock_state = 0;

    if(prbs_chk_hw_timer_ctrl == NULL || ber_data == NULL) {
        return(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }

    ESTM(ber_data->prbs_chk_en = rd_prbs_chk_en());
    ESTM(clk_gate = rd_ln_rx_s_clkgate_frc_on());
    if(ber_data->prbs_chk_en == 0) {
        return(ERR_CODE_PRBS_CHK_DISABLED);
    }
    else if(clk_gate == 1){
        return (ERR_CODE_RX_CLKGATE_FRC_ON);
    }
    ESTM(prbs_chk_lock_state = rd_prbs_chk_lock_state()); /* Do a preemptive read to flush out lock state history */
    UNUSED(prbs_chk_lock_state);
    ENULL_MEMSET(prbs_chk_hw_timer_ctrl, 0, sizeof(struct prbs_chk_hw_timer_ctrl_st));

    /* Check if CDR lost lock */
    EFUN(blackhawk7_v1l8p1_INTERNAL_prbs_chk_cdr_lock_lost(sa__, &ber_data->cdrlcklost));

    /* save prbs hardware timer config registers and prbs mode */
    EFUN(blackhawk7_v1l8p1_get_prbs_chk_hw_timer_ctrl(sa__, prbs_chk_hw_timer_ctrl));

    /* Configure hardware timer to count prbs errors */
    EFUN(blackhawk7_v1l8p1_config_prbs_chk_hw_timer(sa__, time_ms, &prbs_chk_hw_timer_ctrl->time_ms_adjusted_bak));

    EFUN(blackhawk7_v1l8p1_prbs_err_count_state(sa__, &err_cnt, &lcklost)); /* Clear prbs error counters and start timer */
    /* Extra delay to account for the synchronization delay in prbs checker */
    EFUN(blackhawk7_v1l8p1_INTERNAL_hw_timer_sync_delay(sa__));

    return(ERR_CODE_NONE);
}

err_code_t blackhawk7_v1l8p1_get_BER_data_HW_mode(srds_access_t *sa__, struct ber_data_st *ber_data, struct prbs_chk_hw_timer_ctrl_st *prbs_chk_hw_timer_ctrl) {
    INIT_SRDS_ERR_CODE
    uint8_t lcklost = 0;
    uint32_t err_cnt= 0;
    const uint16_t ITER_MAX=10;
    uint16_t iter=0 ;
    uint32_t num_bits_per_ms = 0;
    uint8_t prbs_chk_timeout = 0;
    uint8_t prbs_chk_lock_state = 0;
    if((ber_data == NULL) || (prbs_chk_hw_timer_ctrl == NULL)) {
        return(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
    }
    else if(ber_data->prbs_chk_en == 0) {
        return(ERR_CODE_PRBS_CHK_DISABLED);
    }

    /* Confirm that prbs lock was achieved */
    ESTM(prbs_chk_lock_state = rd_prbs_chk_lock_state());
    if (prbs_chk_lock_state == PRBS_CHECKER_NOT_ENABLED) {
        /* If prbs lock was not achieved, set lcklost = 1 and skip the wait for error accumulation */
        ber_data->lcklost = 1;
    } else {
        /* Poll prbs_chk_lock to make sure timers expired */
        do {
            ESTM(prbs_chk_timeout = rd_prbs_chk_timer_done_lh());
            if (prbs_chk_timeout) {
                break;
            }
            EFUN(USR_DELAY_MS(1));

            iter++;
        } while(iter<(ITER_MAX+(prbs_chk_hw_timer_ctrl->time_ms_adjusted_bak)));

        if(!prbs_chk_timeout) {
            /* timer hasn't expired or it expired and lock didn't go away */
            ber_data->lcklost = 1;
            return ERR_CODE_PRBS_CHK_HW_TIMERS_NOT_EXPIRED;
        }

        ESTM(ber_data->prbs_lck_state = rd_prbs_chk_lock_state());
        switch (ber_data->prbs_lck_state) {
            case PRBS_CHECKER_NOT_ENABLED:
            case PRBS_CHECKER_WAITING_FOR_LOCK:
            case PRBS_CHECKER_LOCK_LOST_AFTER_LOCK:
                ber_data->lcklost = 1;
                break;
            case PRBS_CHECKER_LOCKED:
                /* Read error counters */
                EFUN(blackhawk7_v1l8p1_prbs_err_count_state(sa__, &err_cnt,&lcklost));
                ber_data->lcklost = 0;
                /* Update the BER data structure */
                EFUN(blackhawk7_v1l8p1_INTERNAL_get_num_bits_per_ms(sa__, 1, &num_bits_per_ms));
                COMPILER_64_SET(ber_data->num_bits, 0, num_bits_per_ms);
                COMPILER_64_UMUL_32(ber_data->num_bits, prbs_chk_hw_timer_ctrl->time_ms_adjusted_bak);
                COMPILER_64_SET(ber_data->num_errs, 0, err_cnt);
                break;
            default:
                ber_data->lcklost = 1;
                EFUN_PRINTF(("WARNING: PRBS Check Lock - Invalid lock state! %d\n", prbs_chk_lock_state));
                break;
        }
    }
    /* restore prbs hardware timer config registers */
    EFUN(blackhawk7_v1l8p1_set_prbs_chk_hw_timer_ctrl(sa__, prbs_chk_hw_timer_ctrl));



    return(ERR_CODE_NONE);
}
#endif /* SMALL_FOOTPRINT */



