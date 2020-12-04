/**************************************************************************************
 **************************************************************************************
 *  File Name     :  falcon_furia_pll_config.c                                        *
 *  Created On    :  23/12/2013                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Falcon Furia PLL Configuration API                               *
 *  Revision      :  $Id: falcon_furia_pll_config.c 857 2015-01-16 22:22:38Z kirand $ *
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


/** @file falcon_furia_pll_config.c
 * Falcon Repeater PLL Configuration
 */

#include "falcon_furia_sesto_enum.h"


err_code_t falcon_furia_sesto_configure_pll( const phymod_access_t *pa, enum falcon_furia_sesto_pll_enum pll_cfg) {
    uint8_t reset_state;

  /* Use core_s_rstb to re-initialize all registers to default before calling this function. */

  /* Restore defaults, needed for non-register reset cases */
    /* wrc_falcon_furia_sesto_pll_mode(0x7);            */
    /* wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);    */
    /* wrc_falcon_furia_sesto_ams_pll_force_kvh_bw(0);  */
    /* wrc_falcon_furia_sesto_ams_pll_kvh_force(0);     */
    /* wrc_falcon_furia_sesto_ams_pll_vco_indicator(0); */
    ESTM(reset_state = rdc_falcon_furia_sesto_core_dp_reset_state());
    if(reset_state < 7) {
        USR_PRINTF(("ERROR: falcon_furia_sesto_configure_pll( pa, ..) called without core_dp_s_rstb=0\n"));
        return (_error(ERR_CODE_CORE_DP_NOT_RESET));
    }

  switch (pll_cfg) {

    /******************/
    /*  Integer Mode  */
    /******************/    

    case FALCON_FURIA_pll_div_80x :
      /* pll_ mode<4:0> = 00010, VCO = 17.0G, Refclk = 212.5MHz  */
      wrc_falcon_furia_sesto_pll_mode(0x02);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x1);
      break;

    case FALCON_FURIA_pll_div_96x :
      /* pll_ mode<4:0> = 10000, VCO = 15.0G, Refclk = 156.25MHz  */
      wrc_falcon_furia_sesto_pll_mode(0x10);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x1);
      break;

    case FALCON_FURIA_pll_div_120x :
      /* pll_ mode<4:0> = 10001, VCO = 15.0G, Refclk = 125.0MHz    */
      /* pll_ mode<4:0> = 10001, VCO = 18.75G, Refclk = 156.25MHz  */
      wrc_falcon_furia_sesto_pll_mode(0x11);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x1);
      break;

    case FALCON_FURIA_pll_div_128x :
      /* pll_ mode<4:0> = 00011, VCO = 20.625G, Refclk = 161.1328125MHz  */
      wrc_falcon_furia_sesto_pll_mode(0x03);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_132x :
      /* pll_ mode<4:0> = 00100 , VCO = 20.625G, Refclk = 156.25MHz */
      /* pll_ mode<4:0> = 00100 , VCO = 28.055G, Refclk = 212.5MHz  */
      wrc_falcon_furia_sesto_pll_mode(0x04);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_140x :
      /* pll_ mode<4:0> = 00101 , VCO = 21.875G, Refclk = 156.25MHz */
      wrc_falcon_furia_sesto_pll_mode(0x05);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_144x :
      /* pll_ mode<4:0> = 10010 , VCO = 22.39488G, Refclk = 155.52MHz */
      wrc_falcon_furia_sesto_pll_mode(0x12);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_160x :
      /* pll_ mode<4:0> = 00110 , VCO = 25.0G, Refclk = 156.25MHz          */
      /* pll_ mode<4:0> = 00110 , VCO = 25.78125G, Refclk = 161.1328125MHz */
      wrc_falcon_furia_sesto_pll_mode(0x06);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_160x_vco2 :
      /* pll_ mode<4:0> = 00110 , VCO = 17.0G, Refclk = 106.25MHz */
      wrc_falcon_furia_sesto_pll_mode(0x06);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x1);
      break;

    case FALCON_FURIA_pll_div_160x_refc174 :
      /* pll_mode<3:0> = 0110 , VCO = 27.9525G, Refclk = 174.7MHz */
      wrc_falcon_furia_sesto_pll_mode(0x6);
      wrc_falcon_furia_sesto_ams_pll_force_kvh_bw(1);
      wrc_falcon_furia_sesto_ams_pll_kvh_force(0);
      wrc_falcon_furia_sesto_ams_pll_vco_indicator(1);
      break;

    case FALCON_FURIA_pll_div_165x : 
      /* pll_ mode<4:0> = 00111, VCO = 20.625G, Refclk = 125MHz      */
      /* pll_ mode<4:0> = 00111, VCO = 25.78125G, Refclk = 156.25MHz */
      wrc_falcon_furia_sesto_pll_mode(0x07);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_168x :
      /* pll_ mode<4:0> = 01000, VCO = 26.25G, Refclk = 156.25MHz */
      wrc_falcon_furia_sesto_pll_mode(0x08);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_175x :
      /* pll_ mode<4:0> = 01010, VCO = 27.34375G, Refclk = 156.25MHz  */
      wrc_falcon_furia_sesto_pll_mode(0x0A);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_180x :
      /* pll_ mode<4:0> = 01011 , VCO = 22.5G, Refclk = 125.0MHz    */
      /* pll_ mode<4:0> = 01011 , VCO = 28.125G, Refclk = 156.25MHz */
      wrc_falcon_furia_sesto_pll_mode(0x0B);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_184x : 
      /* pll_ mode<4:0> = 01100 , VCO = 23.0G, Refclk = 125.0MHz */
      wrc_falcon_furia_sesto_pll_mode(0x0C);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_198x : 
      /* pll_ mode<4:0> = 10011 , VCO = 24.75G, Refclk = 125.0MHz */
      wrc_falcon_furia_sesto_pll_mode(0x13);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_200x :
      /* pll_ mode<4:0> = 01101, VCO = 25.0G, Refclk = 125.0MHz  */
      wrc_falcon_furia_sesto_pll_mode(0x0D);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_224x :
      /* pll_ mode<4:0> = 01110 , VCO = 28.0G, Refclk = 125.0MHz */
      wrc_falcon_furia_sesto_pll_mode(0x0E);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    case FALCON_FURIA_pll_div_264x :
      /* pll_ mode<4:0> = 01111 , VCO = 28.05G, Refclk = 106.25MHz */
      wrc_falcon_furia_sesto_pll_mode(0x0F);
      wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
      break;

    default:                     /* Invalid pll_cfg value  */
      return (_error(ERR_CODE_INVALID_PLL_CFG));
      break; 


  } /* switch (pll_cfg) */
  
  return (ERR_CODE_NONE);

} /* falcon_furia_sesto_configure_pll */

/* Get PLL mode */
err_code_t falcon_furia_sesto_get_pll_mode(const phymod_access_t *pa, uint8_t *fal_pll_mode) {

  ESTM(*fal_pll_mode = rdc_falcon_furia_sesto_pll_mode());

  return (ERR_CODE_NONE);
}
