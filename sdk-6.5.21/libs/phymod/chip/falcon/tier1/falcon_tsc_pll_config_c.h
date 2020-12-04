/************************************************************************************
************************************************************************************
*  File Name     :  falcon_tsc_pll_config.c                                        *
*  Created On    :  23/12/2013                                                     *
*  Created By    :  Kiran Divakar                                                  *
*  Description   :  Falcon TSC PLL Configuration API                               *
*  Revision      :  $Id: falcon_tsc_pll_config.c 1586 2018-11-15 21:58:26Z jg943801 $ *
*                                                                                  *
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                            *
*  No portions of this material may be reproduced in any form without              *
*  the written permission of:                                                      *
*      Broadcom Corporation                                                        *
*      5300 California Avenue                                                      *
*      Irvine, CA  92617                                                           *
*                                                                                  *
*  All information contained in this document is Broadcom Corporation              *
*  company private proprietary, and trade secret.                                  *
 */


/** @file falcon_tsc_pll_config.c
 * Falcon TSC PLL Configuration
 */

#include "falcon_tsc_enum.h"


err_code_t falcon_tsc_configure_pll (const phymod_access_t *pa, enum falcon_tsc_pll_enum pll_cfg){
#ifndef ATE_LOG
	uint8_t reset_state;

	/* Restore defaults, needed for non-register reset cases */
	/* EFUN(wrc_pll_mode (0x7)); */
    /* EFUN(wrc_ams_pll_force_kvh_bw                             (   0));  */
    /* EFUN(wrc_ams_pll_kvh_force                                (   0));     */
    /* EFUN(wrc_ams_pll_vco_indicator                            (   0)); */

	/* Use core_s_rstb to re-initialize all registers to default before calling this function. */
	ESTM(reset_state = rdc_core_dp_reset_state());
	if (reset_state < 7) {
		EFUN_PRINTF(("ERROR: falcon_tsc_configure_pll (pa, ..) called without core_dp_s_rstb=0\n"));
		return _error(ERR_CODE_CORE_DP_NOT_RESET);
	}
#endif

	switch (pll_cfg) {
	/******************/
	/*  Integer Mode  */
	/******************/
	case FALCON_TSC_pll_div_128x:
		/* pll_mode<3:0> = 0011, VCO = 20.625  G, Refclk = 106.133MHz */
		EFUN(wrc_pll_mode(0x3));
		break;
	case FALCON_TSC_pll_div_132x:
		/* pll_mode<3:0> = 0100, VCO = 20.625  G, Refclk = 156.25 MHz */
		/* pll_mode<3:0> = 0100, VCO = 28.055  G, Refclk = 212.5  MHz */
		EFUN(wrc_pll_mode(0x4));
		break;
	case FALCON_TSC_pll_div_140x:
		/* pll_mode<3:0> = 0101, VCO = 21.875  G, Refclk = 156.25 MHz */
		EFUN(wrc_pll_mode(0x5));
		break;
	case FALCON_TSC_pll_div_160x:
		/* pll_mode<3:0> = 0110, VCO = 25.0    G, Refclk = 156.25 MHz */
		/* pll_mode<3:0> = 0110, VCO = 25.78125G, Refclk = 161.3  MHz */
		EFUN(wrc_pll_mode(0x6));
		break;
	case FALCON_TSC_pll_div_165x:
		/* pll_mode<3:0> = 0111, VCO = 20.625  G, Refclk = 125    MHz */
		/* pll_mode<3:0> = 0111, VCO = 25.78125G, Refclk = 156.25 MHz */
		EFUN(wrc_pll_mode(0x7));
		break;
	case FALCON_TSC_pll_div_168x:
		/* pll_mode<3:0> = 1000, VCO = 26.25   G, Refclk = 156.25 MHz */
		EFUN(wrc_pll_mode(0x8));
		break;
	case FALCON_TSC_pll_div_175x:
		/* pll_mode<3:0> = 1010, VCO = 27.34375G, Refclk = 156.25 MHz */
		EFUN(wrc_pll_mode(0xA));
		break;
	case FALCON_TSC_pll_div_180x:
		/* pll_mode<3:0> = 1011, VCO = 22.5    G, Refclk = 125.0  MHz */
		/* pll_mode<3:0> = 1011, VCO = 28.125  G, Refclk = 156.25 MHz */
		EFUN(wrc_pll_mode(0xB));
		break;
	case FALCON_TSC_pll_div_184x:
		/* pll_mode<3:0> = 1100, VCO = 23.0    G, Refclk = 125.0  MHz */
		EFUN(wrc_pll_mode(0xC));
		break;
	case FALCON_TSC_pll_div_200x:
		/* pll_mode<3:0> = 1101, VCO = 25.0    G, Refclk = 125.0  MHz */
		EFUN(wrc_pll_mode(0xD));
		break;
	case FALCON_TSC_pll_div_224x:
		/* pll_mode<3:0> = 1110, VCO = 28.0    G, Refclk = 125.0  MHz */
		EFUN(wrc_pll_mode(0xE));
		break;
	case FALCON_TSC_pll_div_264x:
		/* pll_mode<3:0> = 1111, VCO = 28.05   G, Refclk = 106.25  MHz */
		EFUN(wrc_pll_mode(0xF));
		break;
    case FALCON_TSC_pll_div_180x_refc125 :
     /* pll_ mode<4:0> = 01011 , VCO = 22.5    G, Refclk = 125.0      MHz */
        EFUN(wrc_pll_mode             (0x0B));
        EFUN(wrc_ams_pll_force_kvh_bw (   1));
        EFUN(wrc_ams_pll_kvh_force    (   3));
        EFUN(wrc_ams_pll_vco_indicator(   0));
        break;
	/*******************************/
	/*  Invalid 'pll_cfg' Selector */
	/*******************************/
	default:
		return _error(ERR_CODE_INVALID_PLL_CFG);
		break;
	} /* switch (pll_cfg) */

	return ERR_CODE_NONE;
}   /* falcon_tsc_configure_pll */


