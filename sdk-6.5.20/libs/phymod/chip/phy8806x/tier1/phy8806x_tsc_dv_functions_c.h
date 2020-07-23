/**************************************************************************************
**************************************************************************************
*  File Name     :  phy8806x_tsc_dv_functions.c                                        *
*  Created On    :  22/05/2014                                                       *
*  Created By    :  Kiran Divakar                                                    *
*  Description   :  APIs for Serdes IPs ported over from DV                          *
*  Revision      :  $Id: phy8806x_tsc_dv_functions.c 1337 2016-02-04 00:15:32Z kirand $ *
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

/** @file phy8806x_tsc_dv_functions.c
 * Implementation of API functions ported over from DV
 */

/******************************/
/*  TX Pattern Generator APIs */
/******************************/

/* Cofigure shared TX Pattern (Return Val = 0:PASS, 1-6:FAIL (reports 6 possible error scenarios if failed)) */
err_code_t phy8806x_tsc_config_shared_tx_pattern (const phymod_access_t *pa, uint8_t patt_length, const char pattern[]){

	char patt_final[245] = "";
	char patt_mod[245]   = "", bin[5] = "";
	uint8_t str_len, i, k, j = 0;
	uint8_t offset_len, actual_patt_len = 0, hex = 0;
	uint8_t zero_pad_len    = 0;    /* suppress warnings, changed by _calc_patt_gen_mode_sel (pa, ) */
	uint16_t patt_gen_wr_val = 0;
	uint8_t mode_sel        = 0;    /* suppress warnings, changed by _calc_patt_gen_mode_sel (pa, ) */

	EFUN(_calc_patt_gen_mode_sel (pa, &mode_sel, &zero_pad_len, patt_length));

	/* Generating the appropriate write value to patt_gen_seq registers */
	str_len = (int8_t)USR_STRLEN(pattern);

	if ((str_len > 2) && ((USR_STRNCMP(pattern, "0x", 2)) == 0)) {
		/* Hexadecimal Pattern */
		for (i = 2; i < str_len; i++) {
			EFUN(_compute_bin (pa, pattern[i], bin));
			ENULL_STRNCAT(patt_mod, bin);
			if (pattern[i] != '_') {
				actual_patt_len = actual_patt_len + 4;
				if (actual_patt_len > 240) {
					EFUN_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
					return _error(ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN);
				}
			}
		}

		offset_len = (actual_patt_len - patt_length);
		if ((offset_len > 3)  || (actual_patt_len < patt_length)) {
			EFUN_PRINTF(("ERROR: Pattern length provided does not match the hexadecimal pattern provided\n"));
			return _error(ERR_CODE_CFG_PATT_LEN_MISMATCH);
		}else if (offset_len) {
			for (i = 0; i < offset_len; i++) {
				if (patt_mod[i] != '0') {
					EFUN_PRINTF(("ERROR: Pattern length provided does not match the hexadecimal pattern provided\n"));
					return _error(ERR_CODE_CFG_PATT_LEN_MISMATCH);
				}
			}
			for (i = offset_len; i <= actual_patt_len; i++)
				patt_mod[i - offset_len] = patt_mod[i];
		}
	}else  {
		/* Binary Pattern */
		for (i = 0; i < str_len; i++) {
			if ((pattern[i] == '0') || (pattern[i] == '1')) {
				bin[0] = pattern[i];
				bin[1] = '\0';
				ENULL_STRNCAT(patt_mod, bin);
				actual_patt_len++;
				if (actual_patt_len > 240) {
					EFUN_PRINTF(("ERROR: Pattern bigger than max pattern length\n"));
					return _error(ERR_CODE_CFG_PATT_PATTERN_BIGGER_THAN_MAXLEN);
				}
			}else if (pattern[i] != '_') {
				EFUN_PRINTF(("ERROR: Invalid input Pattern\n"));
				return _error(ERR_CODE_CFG_PATT_INVALID_PATTERN);
			}
		}

		if (actual_patt_len != patt_length) {
			EFUN_PRINTF(("ERROR: Pattern length provided does not match the binary pattern provided\n"));
			return _error(ERR_CODE_CFG_PATT_LEN_MISMATCH);
		}
	}

	/* Zero padding upper bits and concatinating patt_mod to form patt_final */
	for (i = 0; i < zero_pad_len; i++) {
		ENULL_STRNCAT(patt_final, "0");
		j++;
	}
	for (i = zero_pad_len; i + patt_length < 241; i = i + patt_length) {
		ENULL_STRNCAT(patt_final, patt_mod);
		j++;
	}

	/* EFUN_PRINTF(("\nFinal Pattern = %s\n\n",patt_final));    */

	for (i = 0; i < 15; i++) {

		for (j = 0; j < 4; j++) {
			k = i * 16 + j * 4;
			bin[0] = patt_final[k];
			bin[1] = patt_final[k + 1];
			bin[2] = patt_final[k + 2];
			bin[3] = patt_final[k + 3];
			bin[4] = '\0';
			EFUN(_compute_hex (pa, bin, &hex));
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
			return _error(ERR_CODE_CFG_PATT_INVALID_SEQ_WRITE);
			break;
		}
	}

	/* Pattern Generator Mode Select */
	/* EFUN(wr_patt_gen_mode_sel(mode_sel)); */
	/* EFUN_PRINTF(("Pattern gen Mode = %d\n",mode));    */

	/* Enable Fixed pattern Generation */
	/* EFUN(wr_patt_gen_en(0x1)); */
	return ERR_CODE_NONE;
}


/* Enable/Disable Shared TX pattern generator */
err_code_t phy8806x_tsc_tx_shared_patt_gen_en (const phymod_access_t *pa, uint8_t enable, uint8_t patt_length){
	uint8_t zero_pad_len = 0;       /* suppress warnings, changed by _calc_patt_gen_mode_sel (pa, ) */
	uint8_t mode_sel     = 0;       /* suppress warnings, changed by _calc_patt_gen_mode_sel (pa, ) */

	EFUN(_calc_patt_gen_mode_sel (pa, &mode_sel, &zero_pad_len, patt_length));

	if (enable) {
		if ((mode_sel < 1) || (mode_sel > 6))
			return _error(ERR_CODE_PATT_GEN_INVALID_MODE_SEL);
		mode_sel = (12 - mode_sel);
		EFUN(wr_patt_gen_start_pos(mode_sel));  /* Start position for pattern */
		EFUN(wr_patt_gen_stop_pos(0x0));        /* Stop position for pattern */
		EFUN(wr_patt_gen_en(0x1));              /* Enable Fixed pattern Generation  */
	}else
		EFUN(wr_patt_gen_en(0x0));              /* Disable Fixed pattern Generation  */
	return ERR_CODE_NONE;
}

/**************************************/
/*  PMD_RX_LOCK and CL72/CL93 Status  */
/**************************************/

err_code_t phy8806x_tsc_pmd_lock_status (const phymod_access_t *pa, uint8_t *pmd_rx_lock){
	if (!pmd_rx_lock)
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
	ESTM(*pmd_rx_lock = rd_pmd_rx_lock());
	return ERR_CODE_NONE;
}


err_code_t phy8806x_tsc_display_cl93n72_status (const phymod_access_t *pa){
	EFUN_PRINTF(("\n\n***************************\n"                                                                                                                                    ));
	ESTM_PRINTF((   "** LANE %d CL93n72 Status **\n", phy8806x_tsc_get_lane (pa )                      ));
	EFUN_PRINTF((    "***************************\n"                                                                                                                                    ));
	ESTM_PRINTF((    "cl93n72_signal_detect         = %d   (1 = CL93n72 training FSM in SEND_DATA state;  0 = CL93n72 in training state)"  "\n", rd_cl93n72_training_fsm_signal_detect()));
	ESTM_PRINTF((    "cl93n72_ieee_training_failure = %d   (1 = Training failure detected;                0 = Training failure not detected)\n", rd_cl93n72_ieee_training_failure()     ));
	ESTM_PRINTF((    "cl93n72_ieee_training_status  = %d   (1 = Start-up protocol in progress;            0 = Start-up protocol complete)" "\n", rd_cl93n72_ieee_training_status()      ));
	ESTM_PRINTF((    "cl93n72_ieee_receiver_status  = %d   (1 = Receiver trained and ready to receive;    0 = Receiver training)"        "\n\n", rd_cl93n72_ieee_receiver_status()      ));
	return ERR_CODE_NONE;
}


/**********************************/
/*  Serdes TX disable/RX Restart  */
/**********************************/

err_code_t phy8806x_tsc_tx_disable (const phymod_access_t *pa, uint8_t enable){

	if (enable)
		EFUN(wr_sdk_tx_disable(0x1));
	else
		EFUN(wr_sdk_tx_disable(0x0));
	return ERR_CODE_NONE;
}


err_code_t phy8806x_tsc_rx_restart (const phymod_access_t *pa, uint8_t enable){

	EFUN(wr_rx_restart_pmd_hold(enable));
	return ERR_CODE_NONE;
}


/******************************************************/
/*  Single function to set/get all RX AFE parameters  */
/******************************************************/

err_code_t phy8806x_tsc_write_rx_afe (const phymod_access_t *pa, enum srds_rx_afe_settings_enum param, int8_t val){
	/* Assumes the micro is not actively tuning */

	switch (param) {
	case RX_AFE_PF:
		return _set_rx_pf_main (pa, val);

	case RX_AFE_PF2:
		return _set_rx_pf2 (pa, val);

	case RX_AFE_VGA:
		return _set_rx_vga (pa, val);

	case RX_AFE_DFE1:
		return _set_rx_dfe1 (pa, val);

	case RX_AFE_DFE2:
		return _set_rx_dfe2 (pa, val);

	case RX_AFE_DFE3:
		return _set_rx_dfe3 (pa, val);

	case RX_AFE_DFE4:
		return _set_rx_dfe4 (pa, val);

	case RX_AFE_DFE5:
		return _set_rx_dfe5 (pa, val);
	case RX_AFE_DFE6:
		return _set_rx_dfe6 (pa, val);

	case RX_AFE_DFE7:
		return _set_rx_dfe7 (pa, val);

	case RX_AFE_DFE8:
		return _set_rx_dfe8 (pa, val);

	case RX_AFE_DFE9:
		return _set_rx_dfe9 (pa, val);

	case RX_AFE_DFE10:
		return _set_rx_dfe10 (pa, val);

	case RX_AFE_DFE11:
		return _set_rx_dfe11 (pa, val);

	case RX_AFE_DFE12:
		return _set_rx_dfe12 (pa, val);

	case RX_AFE_DFE13:
		return _set_rx_dfe13 (pa, val);

	case RX_AFE_DFE14:
		return _set_rx_dfe14 (pa, val);
	default:
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
	}
}


err_code_t phy8806x_tsc_read_rx_afe (const phymod_access_t *pa, enum srds_rx_afe_settings_enum param, int8_t *val){
	/* Assumes the micro is not actively tuning */
	if (!val)
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);

	switch (param) {
	case RX_AFE_PF:
		EFUN(_get_rx_pf_main (pa, val));
		break;
	case RX_AFE_PF2:
		EFUN(_get_rx_pf2 (pa, val));
		break;
	case RX_AFE_VGA:
		EFUN(_get_rx_vga (pa, val));
		break;

	case RX_AFE_DFE1:
		EFUN(_get_rx_dfe1 (pa, val));
		break;
	case RX_AFE_DFE2:
		EFUN(_get_rx_dfe2 (pa, val));
		break;
	case RX_AFE_DFE3:
		EFUN(_get_rx_dfe3 (pa, val));
		break;
	case RX_AFE_DFE4:
		EFUN(_get_rx_dfe4 (pa, val));
		break;
	case RX_AFE_DFE5:
		EFUN(_get_rx_dfe5 (pa, val));
		break;
	case RX_AFE_DFE6:
		EFUN(_get_rx_dfe6 (pa, val));
		break;
	case RX_AFE_DFE7:
		EFUN(_get_rx_dfe7 (pa, val));
		break;
	case RX_AFE_DFE8:
		EFUN(_get_rx_dfe8 (pa, val));
		break;
	case RX_AFE_DFE9:
		EFUN(_get_rx_dfe9 (pa, val));
		break;
	case RX_AFE_DFE10:
		EFUN(_get_rx_dfe10 (pa, val));
		break;
	case RX_AFE_DFE11:
		EFUN(_get_rx_dfe11 (pa, val));
		break;
	case RX_AFE_DFE12:
		EFUN(_get_rx_dfe12 (pa, val));
		break;
	case RX_AFE_DFE13:
		EFUN(_get_rx_dfe13 (pa, val));
		break;
	case RX_AFE_DFE14:
		EFUN(_get_rx_dfe14 (pa, val));
		break;
	default:
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
	}

	return ERR_CODE_NONE;
}


err_code_t phy8806x_tsc_validate_txfir_cfg (const phymod_access_t *pa, int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3){
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

	return _error(err_code);
}


err_code_t phy8806x_tsc_write_tx_afe (const phymod_access_t *pa, enum srds_tx_afe_settings_enum param, int8_t val){

	switch (param) {
	case TX_AFE_PRE:
		return _set_tx_pre (pa, val);
	case TX_AFE_MAIN:
		return _set_tx_main (pa, val);
	case TX_AFE_POST1:
		return _set_tx_post1 (pa, val);
	case TX_AFE_POST2:
		return _set_tx_post2 (pa, val);
	case TX_AFE_POST3:
		return _set_tx_post3 (pa, val);
	case TX_AFE_AMP:
		return _set_tx_amp (pa, val);
	case TX_AFE_DRIVERMODE:
		if (val == DM_NOT_SUPPORTED || val > DM_HALF_AMPLITUDE_HI_IMPED)
			return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
		else
			return wr_ams_tx_driver_mode(val);
	default:
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
	}
}

err_code_t phy8806x_tsc_read_tx_afe (const phymod_access_t *pa, enum srds_tx_afe_settings_enum param, int8_t *val){
	if (!val)
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);

	switch (param) {
	case TX_AFE_PRE:
		EFUN(_get_tx_pre (pa, val));
		break;
	case TX_AFE_MAIN:
		EFUN(_get_tx_main (pa, val));
		break;
	case TX_AFE_POST1:
		EFUN(_get_tx_post1 (pa, val));
		break;
	case TX_AFE_POST2:
		EFUN(_get_tx_post2 (pa, val));
		break;
	case TX_AFE_POST3:
		EFUN(_get_tx_post3 (pa, val));
		break;
	case TX_AFE_AMP:
		EFUN(_get_tx_amp (pa, val));
		break;
	default:
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
	}

	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_apply_txfir_cfg (const phymod_access_t *pa, int8_t pre, int8_t main, int8_t post1, int8_t post2, int8_t post3){

	err_code_t failcode = phy8806x_tsc_validate_txfir_cfg (pa, pre, main, post1, post2, post3);

	if (!failcode) {
		failcode |= _set_tx_pre (pa, pre);
		failcode |= _set_tx_main (pa, main);
		failcode |= _set_tx_post1 (pa, post1);
		failcode |= _set_tx_post2 (pa, post2);
		failcode |= _set_tx_post3 (pa, post3);
	}

	return _error(failcode);
}





/**************************************/
/*  PRBS Generator/Checker Functions  */
/**************************************/

/* Configure PRBS Generator */
err_code_t phy8806x_tsc_config_tx_prbs (const phymod_access_t *pa, enum srds_prbs_polynomial_enum prbs_poly_mode, uint8_t prbs_inv){

	EFUN(wr_prbs_gen_mode_sel((uint8_t)prbs_poly_mode));    /* PRBS Generator mode sel */
	EFUN(wr_prbs_gen_inv(prbs_inv));                        /* PRBS Invert Enable/Disable */
	/* To enable PRBS Generator */
	/* EFUN(wr_prbs_gen_en(0x1)); */
	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_get_tx_prbs_config (const phymod_access_t *pa, enum srds_prbs_polynomial_enum *prbs_poly_mode, uint8_t *prbs_inv){
	uint8_t val;

	ESTM(val = rd_prbs_gen_mode_sel());             /* PRBS Generator mode sel */
	*prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;
	ESTM(val = rd_prbs_gen_inv());                  /* PRBS Invert Enable/Disable */
	*prbs_inv = val;

	return ERR_CODE_NONE;
}

/* PRBS Generator Enable/Disable */
err_code_t phy8806x_tsc_tx_prbs_en (const phymod_access_t *pa, uint8_t enable){

	if (enable)
		EFUN(wr_prbs_gen_en(0x1));              /* Enable PRBS Generator */
	else
		EFUN(wr_prbs_gen_en(0x0));              /* Disable PRBS Generator */
	return ERR_CODE_NONE;
}

/* Get PRBS Generator Enable/Disable */
err_code_t phy8806x_tsc_get_tx_prbs_en (const phymod_access_t *pa, uint8_t *enable){

	ESTM(*enable = rd_prbs_gen_en());

	return ERR_CODE_NONE;
}

/* PRBS 1-bit error injection */
err_code_t phy8806x_tsc_tx_prbs_err_inject (const phymod_access_t *pa, uint8_t enable){
	/* PRBS Error Insert.
	   0 to 1 transition on this signal will insert single bit error in the MSB bit of the data bus.
	   Reset value is 0x0.
	 */
	if (enable)
		EFUN(wr_prbs_gen_err_ins(0x1));
	EFUN(wr_prbs_gen_err_ins(0));
	return ERR_CODE_NONE;
}

/* Configure PRBS Checker */
err_code_t phy8806x_tsc_config_rx_prbs (const phymod_access_t *pa, enum srds_prbs_polynomial_enum prbs_poly_mode, enum srds_prbs_checker_mode_enum prbs_checker_mode, uint8_t prbs_inv){

	EFUN(wr_prbs_chk_mode_sel((uint8_t)prbs_poly_mode));    /* PRBS Checker Polynomial mode sel  */
	EFUN(wr_prbs_chk_mode((uint8_t)prbs_checker_mode));     /* PRBS Checker mode sel (PRBS LOCK state machine select) */
	EFUN(wr_prbs_chk_inv(prbs_inv));                        /* PRBS Invert Enable/Disable */
	/* To enable PRBS Checker */
	/* EFUN(wr_prbs_chk_en(0x1)); */
	return ERR_CODE_NONE;
}

/* get PRBS Checker */
err_code_t phy8806x_tsc_get_rx_prbs_config (const phymod_access_t *pa, enum srds_prbs_polynomial_enum *prbs_poly_mode, enum srds_prbs_checker_mode_enum *prbs_checker_mode, uint8_t *prbs_inv){
	uint8_t val;

	ESTM(val = rd_prbs_chk_mode_sel());             /* PRBS Checker Polynomial mode sel  */
	*prbs_poly_mode = (enum srds_prbs_polynomial_enum)val;
	ESTM(val = rd_prbs_chk_mode());                 /* PRBS Checker mode sel (PRBS LOCK state machine select) */
	*prbs_checker_mode = (enum srds_prbs_checker_mode_enum)val;
	ESTM(val = rd_prbs_chk_inv());                  /* PRBS Invert Enable/Disable */
	*prbs_inv = val;
	return ERR_CODE_NONE;
}

/* PRBS Checker Enable/Disable */
err_code_t phy8806x_tsc_rx_prbs_en (const phymod_access_t *pa, uint8_t enable){

	if (enable) {
		EFUN(wr_prbs_chk_burst_err_cnt_en(0x1));        /* must be enabled before prbs_chk_en */
		EFUN(wr_prbs_chk_en(0x1));                      /* Enable PRBS Checker */
	}else  {
		EFUN(wr_prbs_chk_en(0x0));                      /* Disable PRBS Checker */
		EFUN(wr_prbs_chk_burst_err_cnt_en(0x0));
	}
	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_get_rx_prbs_en (const phymod_access_t *pa, uint8_t *enable){

	ESTM(*enable = rd_prbs_chk_en());

	return ERR_CODE_NONE;
}


/* PRBS Checker Lock State */
err_code_t phy8806x_tsc_prbs_chk_lock_state (const phymod_access_t *pa, uint8_t *chk_lock){
	if (!chk_lock)
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);

	ESTM(*chk_lock = rd_prbs_chk_lock());              /* PRBS Checker Lock Indication 1 = Locked, 0 = Out of Lock */
	return ERR_CODE_NONE;
}

/* PRBS Error Count and Lock Lost (bit 31 in lock lost) */
err_code_t phy8806x_tsc_prbs_err_count_ll (const phymod_access_t *pa, uint32_t *prbs_err_cnt){
	uint16_t rddata;

	if (!prbs_err_cnt)
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
	EFUN(phy8806x_tsc_pmd_rdt_reg (pa, TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS, &rddata));
	*prbs_err_cnt = ((uint32_t)rddata) << 16;
	ESTM(*prbs_err_cnt = (*prbs_err_cnt | rd_prbs_chk_err_cnt_lsb()));
	return ERR_CODE_NONE;
}

/* PRBS Error Count State  */
err_code_t phy8806x_tsc_prbs_err_count_state (const phymod_access_t *pa, uint32_t *prbs_err_cnt, uint8_t *lock_lost){
	if (!prbs_err_cnt || !lock_lost)
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
	EFUN(phy8806x_tsc_prbs_err_count_ll (pa, prbs_err_cnt));
	*lock_lost    = (*prbs_err_cnt >> 31);
	*prbs_err_cnt = (*prbs_err_cnt & 0x7FFFFFFF);
	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_display_detailed_prbs_state_hdr (const phymod_access_t *pa){
	EFUN_PRINTF(("\nPRBS Detailed Display\n"));

	EFUN_PRINTF((" LN TX-Mode RX-Mode Lck LL PRBS-Error-Cnt Burst-Error"));
	EFUN_PRINTF((" Burst-Length Max-Burst-Length"));
	EFUN_PRINTF(("\n"));
	return ERR_CODE_NONE;

}

err_code_t phy8806x_tsc_display_detailed_prbs_state (const phymod_access_t *pa){
	uint32_t err_cnt = 0;
	uint8_t lock_lost = 0, enabled = 0;

	ESTM_PRINTF(("  %d ", phy8806x_tsc_get_lane (pa )));
	ESTM(enabled = rd_prbs_gen_en());
	if (enabled)
		ESTM_PRINTF((" PRBS%-2d", srds_return_prbs_order(rd_prbs_gen_mode_sel())));
	else
		EFUN_PRINTF(("  OFF  "));
	ESTM(enabled = rd_prbs_chk_en());
	if (enabled)
		ESTM_PRINTF(("  PRBS%-2d", srds_return_prbs_order(rd_prbs_chk_mode_sel())));
	else
		EFUN_PRINTF(("   OFF  "));
	ESTM_PRINTF(("  %d ", rd_prbs_chk_lock()));
	EFUN(phy8806x_tsc_prbs_err_count_state (pa, &err_cnt, &lock_lost));
	EFUN_PRINTF(("  %d  %010d ", lock_lost, err_cnt));
	ESTM_PRINTF(("      %4d ", rd_prbs_chk_burst_err_cnt()));
	ESTM_PRINTF(("         %2d ", rd_prbs_burst_err_length_status()));
	ESTM_PRINTF(("           %2d ", rd_max_prbs_burst_err_length_status()));
	EFUN_PRINTF(("\n"));

	return ERR_CODE_NONE;
}
/**********************************************/
/*  Loopback and Ultra-Low Latency Functions  */
/**********************************************/

/* Enable/Diasble Digital Loopback */
err_code_t phy8806x_tsc_dig_lpbk (const phymod_access_t *pa, uint8_t enable){
	EFUN(wr_rx_pi_manual_mode(!enable));
	EFUN(wr_dig_lpbk_en(enable));                   /* 0 = diabled, 1 = enabled */
	return ERR_CODE_NONE;
}

/* Enable/Diasble Digital Loopback for Repeater */
err_code_t phy8806x_tsc_dig_lpbk_rptr (const phymod_access_t *pa, uint8_t enable, enum srds_rptr_mode_enum mode){
	switch (mode) {
	case DATA_IN_SIDE:
		/* DATA_IN_SIDE  - The side where data is fed into the RX serial input */
		if (enable) {
			EFUN(phy8806x_tsc_loop_timing (pa, 0));
			EFUN(phy8806x_tsc_loop_timing (pa, 1)); /* Program DATA_IN_SIDE side TX_PI for loop-timing mode to lock SYS TX to SYS TX clock freq. */
		} else
			EFUN(phy8806x_tsc_loop_timing (pa, 0));
		break;

	case DIG_LPBK_SIDE:
		/* DIG_LPBK_SIDE - The side where data is looped back using digital loopback */
	{ uint8_t osr_mode;
	  ESTM(osr_mode = rd_osr_mode());
	  if ((osr_mode == PHY8806X_TSC_OSX16P5) || (osr_mode == PHY8806X_TSC_OSX20P625))
		  EFUN(wr_rx_pi_manual_mode(0x0)); }
		EFUN(phy8806x_tsc_tx_rptr_mode_timing (pa, 1));       /* Program DIG_LPBK_SIDE side TX_PI for repeater mode to lock LINE TX to SYS RX clock freq. */
		EFUN(wr_dig_lpbk_en(enable));                   /* 0 = diabled, 1 = enabled */
		EFUN(wr_ams_tx_sel_ll(00));                     /* Switch the tx data mux to ULL traffic and check for data integrity */
		break;

	default: return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
		break;
	}

	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_ull_from_dig_lpbk (const phymod_access_t *pa, enum srds_rptr_mode_enum mode){
	switch (mode) {
	case DATA_IN_SIDE:
		EFUN(phy8806x_tsc_dig_lpbk_rptr (pa, 0, mode));
		EFUN(phy8806x_tsc_tx_ull_config (pa, 1));
		break;
	case DIG_LPBK_SIDE:
		EFUN(phy8806x_tsc_dig_lpbk_rptr (pa, 0, mode));
		EFUN(phy8806x_tsc_rx_ull_config (pa, 1));
		break;
	default: return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
		break;

	}
	return ERR_CODE_NONE;

}
err_code_t phy8806x_tsc_nl_from_dig_lpbk (const phymod_access_t *pa, enum srds_rptr_mode_enum mode){
	EFUN(phy8806x_tsc_dig_lpbk_rptr (pa, 0, mode));
	return ERR_CODE_NONE;

}




/* Locks TX_PI to Loop timing */
err_code_t phy8806x_tsc_loop_timing (const phymod_access_t *pa, uint8_t enable){
	uint8_t osr_mode;

	if (enable) {
		EFUN(wr_tx_pi_loop_timing_src_sel(0x1)); /* RX phase_sum_val_logic enable */
		ESTM(osr_mode = rd_osr_mode());
		if ((osr_mode == PHY8806X_TSC_OSX16P5) || (osr_mode == PHY8806X_TSC_OSX20P625))
			EFUN(wr_rx_pi_manual_mode(0x1));
		EFUN(wr_tx_pi_repeater_mode_en(0x0));
		EFUN(wr_tx_pi_en(0x1));                 /* TX_PI enable: 0 = diabled, 1 = enabled */
		EFUN(wr_tx_pi_jitter_filter_en(0x1));   /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */
		EFUN(phy8806x_tsc_delay_us(25));         /* Wait for tclk to lock to CDR */
	} else {
		EFUN(wr_tx_pi_jitter_filter_en(0x0));   /* Jitter filter enable to lock freq: 0 = diabled, 1 = enabled */
		EFUN(wr_tx_pi_en(0x0));                 /* TX_PI enable: 0 = diabled, 1 = enabled */
		EFUN(wr_tx_pi_repeater_mode_en(0x1));
		/* EFUN(wr_tx_pi_loop_timing_src_sel(0x0)); Through timing is default mode in Repeater cores. So never disabled */
	}
	return ERR_CODE_NONE;
}


/* Setup Remote Loopback mode  */
err_code_t phy8806x_tsc_rmt_lpbk (const phymod_access_t *pa, uint8_t enable){
	if (enable) {
		EFUN(phy8806x_tsc_loop_timing (pa, enable));
		EFUN(wr_tx_pi_ext_ctrl_en(0x1));        /* PD path enable: 0 = diabled, 1 = enabled */
		EFUN(wr_rmt_lpbk_en(0x1));              /* Remote Loopback Enable: 0 = diabled, 1 = enable  */
		EFUN(phy8806x_tsc_delay_us(50));         /* Wait for rclk and tclk phase lock before expecing good data from rmt loopback */
	} else {                                        /* Might require longer wait time for smaller values of tx_pi_ext_phase_bwsel_integ */
		EFUN(wr_rmt_lpbk_en(0x0));              /* Remote Loopback Enable: 0 = diabled, 1 = enable  */
		EFUN(wr_tx_pi_ext_ctrl_en(0x0));        /* PD path enable: 0 = diabled, 1 = enabled */
		EFUN(phy8806x_tsc_loop_timing (pa, enable));
	}
	return ERR_CODE_NONE;
}


/* Repeater Only APIs (Not applicable to PMD) */

/* Switch to remote loopback from ULL mode in repeater */
err_code_t phy8806x_tsc_rmt_lpbk_from_ull (const phymod_access_t *pa){
	EFUN(phy8806x_tsc_rmt_lpbk (pa, 0));  /* Reset TX_PI */
	EFUN(phy8806x_tsc_rmt_lpbk (pa, 1));  /* Enable RMT LPBK */
	return ERR_CODE_NONE;
}


/* Switch to remote loopback from NL mode in repeater */
err_code_t phy8806x_tsc_rmt_lpbk_from_nl (const phymod_access_t *pa){
	EFUN(phy8806x_tsc_rmt_lpbk (pa, 0));  /* Reset TX_PI */
	EFUN(phy8806x_tsc_rmt_lpbk (pa, 1));  /* Enable RMT LPBK */
	return ERR_CODE_NONE;
}


/* Switch back from remote loopback to ULL mode in repeater */
err_code_t phy8806x_tsc_ull_from_rmt_lpbk (const phymod_access_t *pa, enum srds_rptr_mode_enum mode){
	switch (mode) {
	case DATA_IN_SIDE:
		EFUN(phy8806x_tsc_rx_rptr_mode_timing (pa, 1)); /* Enable jitter filter enable */
		EFUN(_ull_rx_mode_setup (pa, 1));
		break;
	case RMT_LPBK_SIDE:
		EFUN(phy8806x_tsc_rmt_lpbk (pa, 0));                  /* Reset TX_PI */
		EFUN(phy8806x_tsc_tx_rptr_mode_timing (pa, 1));       /* Enable jitter filter enable */
		EFUN(_ull_tx_mode_setup (pa, 1));
		break;
	default: return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
		break;

	}
	return ERR_CODE_NONE;
}


/* Switch back from remote loopback to NL mode in repeater */
err_code_t phy8806x_tsc_nl_from_rmt_lpbk (const phymod_access_t *pa, enum srds_rptr_mode_enum mode){
	switch (mode) {
	case DATA_IN_SIDE:
		EFUN(phy8806x_tsc_rx_rptr_mode_timing (pa, 1)); /* Enable RX PI transfer */
		break;
	case RMT_LPBK_SIDE:
		EFUN(phy8806x_tsc_rmt_lpbk (pa, 0));                  /* Reset TX_PI */
		EFUN(phy8806x_tsc_tx_rptr_mode_timing (pa, 1));       /* Enable jitter filter enable */
		EFUN(wr_ams_tx_sel_ll(0x0));                    /* Switch the tx data mux to PCS data */
		break;
	default: return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
		break;

	}
	return ERR_CODE_NONE;
}




/**********************************/
/*  TX_PI Jitter Generation APIs  */
/**********************************/

/* TX_PI Frequency Override (Fixed Frequency Mode) */
err_code_t phy8806x_tsc_tx_pi_freq_override (const phymod_access_t *pa, uint8_t enable, int16_t freq_override_val){
	if (enable) {
		EFUN(wr_tx_pi_en(0x1));                                 /* TX_PI enable :            0 = disabled, 1 = enabled */
		EFUN(wr_tx_pi_freq_override_en(0x1));                   /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
		EFUN(wr_tx_pi_freq_override_val(freq_override_val));    /* Fixed Freq Override Value (+/-8192) */
	}else  {
		EFUN(wr_tx_pi_freq_override_val(0));                    /* Fixed Freq Override Value to 0 */
		EFUN(wr_tx_pi_freq_override_en(0x0));                   /* Fixed freq mode enable:   0 = disabled, 1 = enabled */
		EFUN(wr_tx_pi_en(0x0));                                 /* TX_PI enable :            0 = disabled, 1 = enabled */
	}
	return ERR_CODE_NONE;
}


/* TX_PI Sinusoidal or Spread-Spectrum (SSC) Jitter Generation  */
err_code_t phy8806x_tsc_tx_pi_jitt_gen (const phymod_access_t *pa, uint8_t enable, int16_t freq_override_val, enum srds_tx_pi_freq_jit_gen_enum jit_type, uint8_t tx_pi_jit_freq_idx, uint8_t tx_pi_jit_amp){
	/* Added a limiting for the jitter amplitude index, per freq_idx */
	uint8_t max_amp_idx_r20_os1[] = { 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 42, 48, 56, 33, 39, 47, 58, 37, 48, 33, 47, 37, 33, 37, 37 };

	/* Irrespective of the osr_mode, txpi runs @ os1. Thus the max amp idx values remain the same. */
	if (jit_type == TX_PI_SJ)
		if (tx_pi_jit_amp > max_amp_idx_r20_os1[tx_pi_jit_freq_idx])
			tx_pi_jit_amp = max_amp_idx_r20_os1[tx_pi_jit_freq_idx];

	EFUN(phy8806x_tsc_tx_pi_freq_override (pa, enable, freq_override_val));

	if (enable) {
		EFUN(wr_tx_pi_jit_freq_idx(tx_pi_jit_freq_idx));
		EFUN(wr_tx_pi_jit_amp(tx_pi_jit_amp));

		if (jit_type == TX_PI_SSC_HIGH_FREQ) {
			EFUN(wr_tx_pi_jit_ssc_freq_mode(0x1));  /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
			EFUN(wr_tx_pi_ssc_gen_en(0x1));         /* SSC jitter enable:         0 = disabled,    1 = enabled */
		}else if (jit_type == TX_PI_SSC_LOW_FREQ) {
			EFUN(wr_tx_pi_jit_ssc_freq_mode(0x0));  /* SSC_FREQ_MODE:             0 = 6G SSC mode, 1 = 10G SSC mode */
			EFUN(wr_tx_pi_ssc_gen_en(0x1));         /* SSC jitter enable:         0 = disabled,    1 = enabled */
		}else if (jit_type == TX_PI_SJ)
			EFUN(wr_tx_pi_sj_gen_en(0x1));          /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
	}else  {
		EFUN(wr_tx_pi_ssc_gen_en(0x0));                 /* SSC jitter enable:         0 = disabled,    1 = enabled */
		EFUN(wr_tx_pi_sj_gen_en(0x0));                  /* Sinusoidal jitter enable:  0 = disabled,    1 = enabled */
	}
	return ERR_CODE_NONE;
}


/***************************/
/*  Configure Serdes IDDQ  */
/***************************/

err_code_t phy8806x_tsc_core_config_for_iddq (const phymod_access_t *pa){

	return ERR_CODE_NONE;
}


err_code_t phy8806x_tsc_lane_config_for_iddq (const phymod_access_t *pa){

	/* Use frc/frc_val to force all RX and TX clk_vld signals to 0 */
	EFUN(wr_pmd_rx_clk_vld_frc_val(0x0));
	EFUN(wr_pmd_rx_clk_vld_frc(0x1));
	EFUN(wr_pmd_tx_clk_vld_frc_val(0x0));
	EFUN(wr_pmd_tx_clk_vld_frc(0x1));


	/* Use frc/frc_val to force all pmd_rx_lock signals to 0 */
	EFUN(wr_rx_dsc_lock_frc_val(0x0));
	EFUN(wr_rx_dsc_lock_frc(0x1));

	/* Switch all the lane clocks to comclk by writing to RX/TX comclk_sel registers */
	EFUN(wr_ln_rx_s_comclk_sel(0x1));
	EFUN(wr_ln_tx_s_comclk_sel(0x1));

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
	return ERR_CODE_NONE;
}


/*********************************/
/*  uc_active and uc_reset APIs  */
/*********************************/

/* Set the uC active mode */
err_code_t phy8806x_tsc_uc_active_enable (const phymod_access_t *pa, uint8_t enable){
	EFUN(wrc_uc_active(enable));
	return ERR_CODE_NONE;
}



/* Enable or Disable the uC reset */
err_code_t phy8806x_tsc_uc_reset (const phymod_access_t *pa, uint8_t enable){
	if (enable) {
		/* Assert micro reset and reset all micro registers (all non-status registers written to default value) */
		EFUN(wrc_micro_core_clk_en(0x0));               /* Disable clock to M0 core */
		EFUN(wrc_micro_master_clk_en(0x0));             /* Disable clock to microcontroller subsystem */
		EFUN(wrc_micro_protect_fwcode(0x1));            /* Gate-off RAM chip select and other signals to avoid ucode corruption */
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD200, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD201, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD202, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD204, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD205, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD206, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD207, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD208, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD209, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD20A, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD20B, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD20C, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD20D, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD20E, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD211, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD212, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD213, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD214, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD215, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD216, 0x0007));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD217, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD218, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD219, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD21A, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD21B, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD220, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD221, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD224, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD226, 0x0000));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD225, 0x8803));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD228, 0x0101));
		EFUN(phy8806x_tsc_pmd_wr_reg (pa, 0xD229, 0x0000));
		EFUN(wrc_micro_protect_fwcode(0x0));      /* Remove Gate-off RAM chip select and other signals */
	}else  {
		/* De-assert micro reset - Start executing code */
		EFUN(wrc_micro_master_clk_en(0x1));             /* Enable clock to microcontroller subsystem */
		EFUN(wrc_micro_master_rstb(0x1));               /* De-assert reset to microcontroller sybsystem */
		EFUN(wrc_micro_ra_init(0x2));                   /* Write command for data RAM initialization */
		EFUN(phy8806x_tsc_poll_micro_ra_initdone (pa, 250));  /* Poll   status of  data RAM initialization */
		EFUN(wrc_micro_ra_init(0x0));                   /* Clear command for data RAM initialization */
		EFUN(wrc_micro_core_clk_en(0x1));               /* Enable clock to M0 core */
		EFUN(wrc_micro_core_rstb(0x1));
	}
	return ERR_CODE_NONE;
}

/******************/
/*  Lane Mapping  */
/******************/

err_code_t phy8806x_tsc_map_lanes (const phymod_access_t *pa, uint8_t num_lanes, uint8_t const *tx_lane_map, uint8_t const *rx_lane_map){
	uint8_t rd_val = 0;

	/* Verify that the core data path is held in reset. */
	ESTM(rd_val = rdc_uc_active());
	if (rd_val != 0)
		return ERR_CODE_UC_ACTIVE;

	/* Verify that the num_lanes parameter is correct. */
	ESTM(rd_val = rdc_revid_multiplicity());
	if (rd_val != num_lanes)
		return ERR_CODE_BAD_LANE_COUNT;

	/* Verify that tx_lane_map and rx_lane_map are valid. */
	{
		uint8_t index1, index2;
		uint8_t lp_message_printed = 0;

		/* Avoid unused variable messages if not used for core. */
		(void)lp_message_printed;
		(void)index2;

		for (index1 = 0; index1 < num_lanes; ++index1) {

#if defined(wrc_tx_lane_addr_0) || defined(wrc_tx_lane_map_0)

			/* Not able to configure lane mapping.  So verify that it's the default mapping. */
			if ((tx_lane_map[index1] != index1)
			    || (rx_lane_map[index1] != index1))
				return ERR_CODE_BAD_LANE;

#else                   /* defined(wrc_tx_lane_addr_0) || defined(wrc_tx_lane_map_0)*/

			/* Verify that a lane map is not to an invalid lane. */
			if ((tx_lane_map[index1] >= num_lanes)
			    || (rx_lane_map[index1] >= num_lanes))
				return ERR_CODE_BAD_LANE;

			/* For this core, TX and RX lane mapping can be independent.
			 * But warn if they are different.
			 */
			if ((tx_lane_map[index1] != rx_lane_map[index1])
			    && (!lp_message_printed)) {
				EFUN_PRINTF(("Warning:  In core %d, TX lane %d is mapped to %d, while RX lane %d is mapped to %d.\n          Digital and remote loopback will not operate as expected.\n          Further warnings are suppressed.\n",
					     EFUN(phy8806x_tsc_get_core()), index1, tx_lane_map[index1], index1, rx_lane_map[index1]));
				lp_message_printed = 1;
			}

			/* Verify that a lane map is not used twice. */
			for (index2 = index1 + 1; index2 < num_lanes; ++index2) {
				if ((tx_lane_map[index1] == tx_lane_map[index2])
				    || (rx_lane_map[index1] == rx_lane_map[index2]))
					return ERR_CODE_BAD_LANE;
			}

#endif                  /* defined(wrc_tx_lane_addr_0) || defined(wrc_tx_lane_map_0)*/

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

	return ERR_CODE_NONE;
}

/****************************************************/
/*  Serdes Powerdown, ClockGate and Deep_Powerdown  */
/****************************************************/

err_code_t phy8806x_tsc_core_pwrdn (const phymod_access_t *pa, enum srds_core_pwrdn_mode_enum mode){
	switch (mode) {
	case PWR_ON:
		EFUN(_phy8806x_tsc_core_clkgate (pa, 0));
		EFUN(wrc_afe_s_pll_pwrdn(0x0));
		EFUN(wrc_core_dp_s_rstb(0x1));
		break;
	case PWRDN:
		EFUN(wrc_core_dp_s_rstb(0x0));
		EFUN(phy8806x_tsc_delay_ns(   500)); /* wait >50 comclk cycles  */
		EFUN(wrc_afe_s_pll_pwrdn(0x1));
		break;
	case PWRDN_DEEP:
	{   uint8_t lane_orig = phy8806x_tsc_get_lane (pa );
	    uint8_t lane;
	    for (lane = 0; lane < SERDES_NUM_LANES; ++lane) {
		    EFUN(phy8806x_tsc_set_lane(  lane));
		    EFUN(wr_ln_rx_s_pwrdn(  0x1));
		    EFUN(wr_ln_tx_s_pwrdn(  0x1));
		    EFUN(_phy8806x_tsc_lane_clkgate (pa, 1));
		    EFUN(wr_ln_dp_s_rstb(   0x0));
	    }
	    EFUN(phy8806x_tsc_set_lane(lane_orig)); }
		EFUN(wrc_core_dp_s_rstb(0x0));
		EFUN(phy8806x_tsc_delay_ns(   500)); /* wait >50 comclk cycles  */
		{   uint8_t pll_orig = phy8806x_tsc_get_pll(pa);
		    uint8_t pll;
		    for (pll = 0; pll < SERDES_NUM_PLLS; ++pll) {
			    EFUN(phy8806x_tsc_set_pll(pa, pll));
			    EFUN(wrc_afe_s_pll_pwrdn(0x1));
		    }
		    EFUN(phy8806x_tsc_set_pll(pa, pll_orig)); }
		EFUN(_phy8806x_tsc_core_clkgate (pa, 1));
		break;
	default:
		EFUN(_error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT));
		break;
	}
	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_lane_pwrdn (const phymod_access_t *pa, enum srds_core_pwrdn_mode_enum mode){
	switch (mode) {
	case PWR_ON:
		EFUN(wr_ln_tx_s_pwrdn(0x0));
		EFUN(wr_ln_rx_s_pwrdn(0x0));
		EFUN(_phy8806x_tsc_lane_clkgate (pa, 0));
		break;
	case PWRDN:
		/* do the RX first, since that is what is most users care about */
		EFUN(wr_ln_rx_s_pwrdn(0x1));
		EFUN(wr_ln_tx_s_pwrdn(0x1));
		break;
	case PWRDN_DEEP:
		/* do the RX first, since that is what is most users care about */
		EFUN(wr_ln_rx_s_pwrdn(0x1));
		EFUN(wr_ln_tx_s_pwrdn(0x1));
		EFUN(_phy8806x_tsc_lane_clkgate (pa, 1));
		EFUN(wr_ln_dp_s_rstb(0x0));
		break;
	case PWRDN_TX:
		EFUN(wr_ln_tx_s_pwrdn(0x1));
		break;
	case PWRDN_RX:
		EFUN(wr_ln_rx_s_pwrdn(0x1));
		break;
	default:
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
		break;
	}
	return ERR_CODE_NONE;
}


/*******************************/
/*  Isolate Serdes Input Pins  */
/*******************************/

err_code_t phy8806x_tsc_isolate_ctrl_pins (const phymod_access_t *pa, uint8_t enable){

	EFUN(phy8806x_tsc_isolate_lane_ctrl_pins (pa, enable));
	EFUN(phy8806x_tsc_isolate_core_ctrl_pins (pa, enable));

	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_isolate_lane_ctrl_pins (const phymod_access_t *pa, uint8_t enable){

	if (enable) {
		EFUN(wr_pmd_ln_tx_h_pwrdn_pkill(0x1));
		EFUN(wr_pmd_ln_rx_h_pwrdn_pkill(0x1));
		EFUN(wr_pmd_ln_dp_h_rstb_pkill(0x1));
		EFUN(wr_pmd_ln_h_rstb_pkill(0x1));
		EFUN(wr_pmd_tx_disable_pkill(0x1));
		EFUN(wr_pmd_ln_rx_h_rstb_pkill(0x1));
		EFUN(wr_pmd_ln_rx_dp_h_rstb_pkill(0x1));
		EFUN(wr_pmd_ln_tx_dp_h_rstb_pkill(0x1));
		EFUN(wr_pmd_ln_tx_h_rstb_pkill(0x1));
	}else  {
		EFUN(wr_pmd_ln_tx_h_pwrdn_pkill(0x0));
		EFUN(wr_pmd_ln_rx_h_pwrdn_pkill(0x0));
		EFUN(wr_pmd_ln_dp_h_rstb_pkill(0x0));
		EFUN(wr_pmd_ln_h_rstb_pkill(0x0));
		EFUN(wr_pmd_tx_disable_pkill(0x0));
		EFUN(wr_pmd_ln_rx_h_rstb_pkill(0x0));
		EFUN(wr_pmd_ln_rx_dp_h_rstb_pkill(0x0));
		EFUN(wr_pmd_ln_tx_dp_h_rstb_pkill(0x0));
		EFUN(wr_pmd_ln_tx_h_rstb_pkill(0x0));
	}
	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_isolate_core_ctrl_pins (const phymod_access_t *pa, uint8_t enable){

	if (enable) {
		EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x1));
		EFUN(wrc_pmd_mdio_trans_pkill(0x1));
	}else  {
		EFUN(wrc_pmd_core_dp_h_rstb_pkill(0x0));
		EFUN(wrc_pmd_mdio_trans_pkill(0x0));
	}
	return ERR_CODE_NONE;
}




/***********************************************/
/*  Microcode Load into Program RAM Functions  */
/***********************************************/


/* uCode Load through Register (MDIO) Interface [Return Val = Error_Code (0 = PASS)] */
err_code_t phy8806x_tsc_ucode_mdio_load (const phymod_access_t *pa, uint8_t *ucode_image, uint16_t ucode_len){
	uint16_t ucode_len_padded, count = 0;
	uint16_t wrdata_lsw;
	uint8_t wrdata_lsb;

	if (!ucode_image)
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);

	if (ucode_len > UCODE_MAX_SIZE)                   /* uCode size should be less than UCODE_MAX_SIZE  */
		return _error(ERR_CODE_INVALID_UCODE_LEN);

	EFUN(wrc_micro_protect_fwcode(0x0));                    /* Remove Gate-off RAM chip select and other signals */
	EFUN(wrc_micro_master_clk_en(0x1));                     /* Enable clock to microcontroller subsystem */
	EFUN(wrc_micro_master_rstb(0x1));                       /* De-assert reset to microcontroller sybsystem */
	EFUN(wrc_micro_master_rstb(0x0));                       /* Assert reset to microcontroller sybsystem - Toggling reset*/
	EFUN(wrc_micro_master_rstb(0x1));                       /* De-assert reset to microcontroller sybsystem */

	EFUN(wrc_micro_ra_init(0x1));                           /* Set initialization command to initialize code RAM */
	EFUN(phy8806x_tsc_poll_micro_ra_initdone (pa, 250));          /* Poll for micro_ra_initdone = 1 to indicate initialization done */

	EFUN(wrc_micro_ra_init(0x0));                           /* Clear initialization command */

	ucode_len_padded = ((ucode_len + 3) & 0xFFFC);          /* Aligning ucode size to 4-byte boundary */

	/* Code to Load microcode */
	EFUN(wrc_micro_autoinc_wraddr_en(0x1));                                 /* To auto increment RAM write address */
	EFUN(wrc_micro_ra_wrdatasize(0x1));                                     /* Select 16bit transfers */
	EFUN(wrc_micro_ra_wraddr_msw(0x0));                                     /* Upper 16bits of start address of Program RAM where the ucode is to be loaded */
	EFUN(wrc_micro_ra_wraddr_lsw(0x0));                                     /* Lower 16bits of start address of Program RAM where the ucode is to be loaded */

	do {                                                                    /* ucode_image loaded 16bits at a time */
		wrdata_lsb = (count < ucode_len) ? ucode_image[count] : 0x0;    /* wrdata_lsb read from ucode_image; zero padded to 4byte boundary */
		count++;
		wrdata_lsw = (count < ucode_len) ? ucode_image[count] : 0x0;    /* wrdata_msb read from ucode_image; zero padded to 4byte boundary */
		count++;
		wrdata_lsw = ((wrdata_lsw << 8) | wrdata_lsb);                  /* 16bit wrdata_lsw formed from 8bit msb and lsb values read from ucode_image */
		EFUN(wrc_micro_ra_wrdata_lsw(wrdata_lsw));                      /* Program RAM lower 16bits write data */
	}   while (count < ucode_len_padded);                                   /* Loop repeated till entire image loaded (upto the 4byte boundary) */

	EFUN(wrc_micro_ra_wrdatasize(0x2));                                     /* Select 32bit transfers as default */
	EFUN(wrc_micro_core_clk_en(0x1));                                       /* Enable clock to M0 core */
	/* EFUN(wrc_micro_core_rstb(0x1)); */                    /* De-assert reset to micro to start executing microcode */
	return ERR_CODE_NONE;                                                   /* NO Errors while loading microcode (uCode Load PASS) */
}


/* Read-back uCode from Program RAM and verify against ucode_image [Return Val = Error_Code (0 = PASS)]  */
err_code_t phy8806x_tsc_ucode_load_verify (const phymod_access_t *pa, uint8_t *ucode_image, uint16_t ucode_len){

	uint16_t ucode_len_padded, count = 0;
	uint16_t rddata_lsw;
	uint16_t data_lsw;
	uint8_t rddata_lsb;

	if (!ucode_image)
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);

	ucode_len_padded = ((ucode_len + 3) & 0xFFFC);          /* Aligning ucode size to 4-byte boundary */

	if (ucode_len_padded > UCODE_MAX_SIZE)                  /* uCode size should be less than UCODE_MAX_SIZE */
		return _error(ERR_CODE_INVALID_UCODE_LEN);

	EFUN(wrc_micro_autoinc_rdaddr_en(0x1));                                 /* To auto increment RAM read address */
	EFUN(wrc_micro_ra_rddatasize(0x1));                                     /* Select 16bit transfers */
	EFUN(wrc_micro_ra_rdaddr_msw(0x0));                                     /* Upper 16bits of start address of Program RAM from where to read ucode */
	EFUN(wrc_micro_ra_rdaddr_lsw(0x0));                                     /* Lower 16bits of start address of Program RAM from where to read ucode */

	do {                                                                    /* ucode_image read 16bits at a time */
		rddata_lsb = (count < ucode_len) ? ucode_image[count] : 0x0;    /* rddata_lsb read from ucode_image; zero padded to 4byte boundary */
		count++;
		rddata_lsw = (count < ucode_len) ? ucode_image[count] : 0x0;    /* rddata_msb read from ucode_image; zero padded to 4byte boundary */
		count++;
		rddata_lsw = ((rddata_lsw << 8) | rddata_lsb);                  /* 16bit rddata_lsw formed from 8bit msb and lsb values read from ucode_image */
		                                                                /* Compare Program RAM ucode to ucode_image (Read to micro_ra_rddata_lsw reg auto-increments the ram_address) */
		ESTM(data_lsw = rdc_micro_ra_rddata_lsw());
		if (data_lsw != rddata_lsw) {
			EFUN_PRINTF(("Ucode_Load_Verify_FAIL: Addr = 0x%x: Read_data = 0x%x :  Expected_data = 0x%x \n", (count - 2), data_lsw, rddata_lsw));
			return _error(ERR_CODE_UCODE_VERIFY_FAIL);   /* Verify uCode FAIL */
		}

	} while (count < ucode_len_padded);                     /* Loop repeated till entire image loaded (upto the 4byte boundary) */

	EFUN(wrc_micro_ra_rddatasize(0x2));                     /* Select 32bit transfers ad default */
	return ERR_CODE_NONE;                                   /* Verify uCode PASS */
}



/*************************************************/
/*  RAM access through Micro Register Interface  */
/*************************************************/


/* Micro RAM Word Write */
err_code_t phy8806x_tsc_wrw_uc_ram (const phymod_access_t *pa, uint16_t addr, uint16_t wr_val){

	EFUN(wrc_micro_autoinc_wraddr_en(0));
	EFUN(wrc_micro_ra_wrdatasize(0x1));             /* Select 16bit write datasize */
	EFUN(wrc_micro_ra_wraddr_msw(0x2000));          /* Upper 16bits of RAM address to be written to */
	EFUN(wrc_micro_ra_wraddr_lsw(addr));            /* Lower 16bits of RAM address to be written to */
	EFUN(wrc_micro_ra_wrdata_lsw(wr_val));          /* uC RAM lower 16bits write data */
	return ERR_CODE_NONE;
}

/* Micro RAM Byte Write */
err_code_t phy8806x_tsc_wrb_uc_ram (const phymod_access_t *pa, uint16_t addr, uint8_t wr_val){
	EFUN(wrc_micro_autoinc_wraddr_en(0));
	EFUN(wrc_micro_ra_wrdatasize(0x0));             /* Select 8bit write datasize */
	EFUN(wrc_micro_ra_wraddr_msw(0x2000));          /* Upper 16bits of RAM address to be written to */
	EFUN(wrc_micro_ra_wraddr_lsw(addr));            /* Lower 16bits of RAM address to be written to */
	EFUN(wrc_micro_ra_wrdata_lsw(wr_val));          /* uC RAM lower 16bits write data */
	return ERR_CODE_NONE;
}

/* Micro RAM Word Read */
uint16_t phy8806x_tsc_rdw_uc_ram (const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr){
	uint16_t rddata;

	if (!err_code_p)
		return 0;
	*err_code_p = ERR_CODE_NONE;
	EPFUN(wrc_micro_autoinc_rdaddr_en(0));
	EPFUN(wrc_micro_ra_rddatasize(0x1));            /* Select 16bit read datasize */
	EPFUN(wrc_micro_ra_rdaddr_msw(0x2000));         /* Upper 16bits of RAM address to be read */
	EPFUN(wrc_micro_ra_rdaddr_lsw(addr));           /* Lower 16bits of RAM address to be read */
	EPSTM(rddata = rdc_micro_ra_rddata_lsw());      /* 16bit read data */
	return rddata;
}

/* Micro RAM Byte Read */
uint8_t phy8806x_tsc_rdb_uc_ram (const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr){
	uint8_t rddata;

	if (!err_code_p)
		return 0;
	*err_code_p = ERR_CODE_NONE;
	EPFUN(wrc_micro_autoinc_rdaddr_en(0));
	EPFUN(wrc_micro_ra_rddatasize(0x0));                    /* Select 8bit read datasize */
	EPFUN(wrc_micro_ra_rdaddr_msw(0x2000));                 /* Upper 16bits of RAM address to be read */
	EPFUN(wrc_micro_ra_rdaddr_lsw(addr));                   /* Lower 16bits of RAM address to be read */
	EPSTM(rddata = (uint8_t)rdc_micro_ra_rddata_lsw());     /* 16bit read data */
	return rddata;
}



/* Micro RAM Word Signed Write */
err_code_t phy8806x_tsc_wrws_uc_ram (const phymod_access_t *pa, uint16_t addr, int16_t wr_val){
	return phy8806x_tsc_wrw_uc_ram (pa, addr, wr_val);
}

/* Micro RAM Byte Signed Write */
err_code_t phy8806x_tsc_wrbs_uc_ram (const phymod_access_t *pa, uint16_t addr, int8_t wr_val){
	return phy8806x_tsc_wrb_uc_ram (pa, addr, wr_val);
}

/* Micro RAM Word Signed Read */
int16_t phy8806x_tsc_rdws_uc_ram (const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr){
	return (int16_t)phy8806x_tsc_rdw_uc_ram (pa, err_code_p, addr);
}

/* Micro RAM Byte Signed Read */
int8_t phy8806x_tsc_rdbs_uc_ram (const phymod_access_t *pa, err_code_t *err_code_p, uint16_t addr){
	return (int8_t)phy8806x_tsc_rdb_uc_ram (pa, err_code_p, addr);
}


/* Micro RAM Byte Read */
err_code_t phy8806x_tsc_rdblk_uc_ram (const phymod_access_t *pa, uint8_t *mem, uint16_t addr, uint16_t cnt){
	if (!mem)
		return _error(ERR_CODE_BAD_PTR_OR_INVALID_INPUT);
	while (cnt--) {
		err_code_t error_code;
		uint8_t tmp = phy8806x_tsc_rdb_uc_ram (pa, &error_code, addr++); /* Value read from the required Data RAM Address */
		if (error_code)
			return _error(error_code);
		*mem++ = tmp;
	}
	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_pll_drv_hv_disable (const phymod_access_t *pa, uint8_t disable){
	return wrc_ams_pll_drv_hv_disable(disable & 0x1);
}

#ifdef TDT_CHARACTERIZATION
err_code_t phy8806x_tsc_tdt_start (const phymod_access_t *pa, enum srds_tdt_pattern_enum tdt_pattern, uint8_t tdt_mode){
	uint8_t lock;
	uint8_t micro_stopped;


	ESTM(lock = rd_pmd_rx_lock());
	if (lock == 0) {
		EFUN_PRINTF(("Error: No PMD_RX_LOCK on lane requesting TDT\n"));
		return ERR_CODE_DIAG_SCAN_NOT_COMPLETE;
	}

	ESTM(micro_stopped = rdv_usr_sts_micro_stopped());
	if (micro_stopped < 1) {
		EFUN_PRINTF(("Error: Micro not stopped on lane requesting TDT\n"));
		return ERR_CODE_DIAG_SCAN_NOT_COMPLETE;
	}

	EFUN(phy8806x_tsc_pmd_uc_cmd (pa, CMD_TDT_EN, ((TDT_START << 4) | ((tdt_pattern & 0x7) << 1) | (tdt_mode & 1)), 2000));

	switch (tdt_pattern) {
	case TDT_PAT_40:
		EFUN(phy8806x_tsc_config_shared_tx_pattern (pa, 240, "0xFFFFF00000FFFFF00000FFFFF00000FFFFF00000FFFFF00000FFFFF00000"));
		EFUN(phy8806x_tsc_tx_shared_patt_gen_en (pa, 1, 240));
		break;
	case TDT_PAT_80:
		EFUN(phy8806x_tsc_config_shared_tx_pattern (pa, 240, "0xFFFFFFFFFF0000000000FFFFFFFFFF0000000000FFFFFFFFFF0000000000"));
		EFUN(phy8806x_tsc_tx_shared_patt_gen_en (pa, 1, 240));
		break;
	case TDT_PAT_120:
		EFUN(phy8806x_tsc_config_shared_tx_pattern (pa, 240, "0xFFFFFFFFFFFFFFF000000000000000FFFFFFFFFFFFFFF000000000000000"));
		EFUN(phy8806x_tsc_tx_shared_patt_gen_en (pa, 1, 240));
		break;
	case TDT_PAT_240:
		EFUN(phy8806x_tsc_config_shared_tx_pattern (pa, 240, "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000000000000"));
		EFUN(phy8806x_tsc_tx_shared_patt_gen_en (pa, 1, 240));
		break;
	case TDT_PAT_PRBS7:
		EFUN(phy8806x_tsc_config_tx_prbs (pa, PRBS_7, 0));
		break;
	case TDT_PAT_PRBS9:
		EFUN(phy8806x_tsc_config_tx_prbs (pa, PRBS_9, 0));
		break;
	default:
		EFUN_PRINTF(("Error: Unknown TX TDT pattern\n"));
		return ERR_CODE_DIAG_SCAN_NOT_COMPLETE;
	}

	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_tdt_read_data (const phymod_access_t *pa, uint8_t tdt_stepsize_shift, struct srds_tdt_char_st *tdt_sample){
	ESTM((*tdt_sample).pi_d       = rd_rx_pi_cnt_bin_d());
	ESTM((*tdt_sample).pi_l       = rd_rx_pi_cnt_bin_l());
	ESTM((*tdt_sample).lms_thresh = rd_rx_lms_thresh_sel());
	EFUN(phy8806x_tsc_pmd_uc_cmd (pa, CMD_TDT_EN, ((TDT_STEP << 4) | tdt_stepsize_shift), 2000));
	EFUN(phy8806x_tsc_pmd_uc_cmd (pa, CMD_TDT_EN, (TDT_READ << 4), 4000));
	ESTM((*tdt_sample).lvl_raw    = (int16_t)rd_uc_dsc_data());
	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_tdt_display_hdr (const phymod_access_t *pa){
	uint8_t verbose = 0;

	ESTM_PRINTF(("**** BEGIN TDT DATA Core = %d, Lane = %d ****\n", phy8806x_tsc_get_core(), phy8806x_tsc_get_lane (pa )));
	if (verbose)
		EFUN_PRINTF(("[::] step_cnt, pi_d, pi_l, lms_thresh, lvl_raw, lvl_mv\n"));
	else
		EFUN_PRINTF(("[::] step_cnt, lvl_mv\n"));
	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_tdt_display (const phymod_access_t *pa, uint32_t step_cnt, struct srds_tdt_char_st tdt_sample){
	uint8_t verbose = 0;

	if (verbose)
		ESTM_PRINTF(("[::] %d, %d, %d, %d, %d, %d.%05d\n", step_cnt, tdt_sample.pi_d, tdt_sample.pi_l,
			     tdt_sample.lms_thresh, tdt_sample.lvl_raw, tdt_sample.lvl_raw / 32, (_abs(tdt_sample.lvl_raw) % 32) * (100000 / 32)));
	else
		ESTM_PRINTF(("[::] %d, %d.%05d\n", step_cnt, tdt_sample.lvl_raw / 32, (_abs(tdt_sample.lvl_raw) % 32) * (100000 / 32)));
	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_tdt_done (const phymod_access_t *pa){
	EFUN(phy8806x_tsc_pmd_uc_cmd (pa, CMD_TDT_EN, (TDT_DISABLE << 4), 2000));
	EFUN_PRINTF(("**** END TDT DATA ****\n"));
	return ERR_CODE_NONE;
}

err_code_t phy8806x_tsc_tdt_char (const phymod_access_t *pa, enum srds_tdt_pattern_enum tdt_pattern, uint8_t tdt_mode, uint8_t tdt_stepsize_shift){
	uint32_t step_cnt;
	uint8_t prbs_poly;
	struct srds_tdt_char_st tdt_sample = { 0 };

	/* Stop micro */
	EFUN(phy8806x_tsc_stop_rx_adaptation (pa, 1));

	/* Save TX PRBS */
	ESTM(prbs_poly = rd_prbs_gen_mode_sel());

	EFUN(phy8806x_tsc_tdt_start (pa, tdt_pattern, tdt_mode));                             /* TDT start, pattern configuration */
	EFUN(phy8806x_tsc_tdt_display_hdr (pa ));                                            /* Display TDT header */
	for (step_cnt = 0; step_cnt < get_tdt_total_sweep_cnt(tdt_pattern); step_cnt += (1 << tdt_stepsize_shift)) {
		EFUN(phy8806x_tsc_tdt_read_data (pa, tdt_stepsize_shift, &tdt_sample));       /* Step through phases and read data */
		EFUN(phy8806x_tsc_tdt_display (pa, step_cnt, tdt_sample));                    /* Display data in sample structure */
	}
	EFUN(phy8806x_tsc_tdt_done (pa ));                                                   /* TDT disable */

	/* Restore TX pattern/PRBS setup */
	EFUN(phy8806x_tsc_tx_shared_patt_gen_en (pa, 0, 240));
	EFUN(phy8806x_tsc_config_tx_prbs (pa, (enum srds_prbs_polynomial_enum)prbs_poly, 0));

	/* Resume micro */
	EFUN(phy8806x_tsc_stop_rx_adaptation (pa, 0));

	return ERR_CODE_NONE;
}
#endif
