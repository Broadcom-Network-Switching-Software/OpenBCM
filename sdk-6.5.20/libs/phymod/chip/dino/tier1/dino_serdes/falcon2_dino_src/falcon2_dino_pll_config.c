/**************************************************************************************
 **************************************************************************************
 *  File Name     :  falcon2_dino_pll_config.c                                        *
 *  Created On    :  23/12/2013                                                       *
 *  Created By    :  Kiran Divakar                                                    *
 *  Description   :  Falcon2 Dino PLL Configuration API                               *
 *  Revision      :  $Id: falcon2_dino_pll_config.c 1400 2016-04-14 06:27:04Z cvazquez $ *
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

/** @file
 * Falcon2 Dino PLL Configuration
 */

#include "falcon2_dino_enum.h"


err_code_t falcon2_dino_configure_pll( const phymod_access_t *pa, enum falcon2_dino_pll_enum pll_cfg) {
    uint8_t reset_state;

 /* Restore defaults, needed for non-register reset cases */
 /* FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode                                         (   0x7));   */
 /* FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g                                 (   0x0));   */
 /* FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_force_kvh_bw                             (   0));     */
 /* FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_kvh_force                                (   0));     */
 /* FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco_indicator                            (   0));     */

 /* Use core_s_rstb to re-initialize all registers to default before calling this function. */
    FALCON2_DINO_ESTM(reset_state = falcon2_dino_rdc_core_dp_reset_state());
    if(reset_state < 7) {
        FALCON2_DINO_EFUN_PRINTF(("ERROR: falcon2_dino_configure_pll( pa, ..) called without core_dp_s_rstb=0\n"));
        return (falcon2_dino_error(ERR_CODE_CORE_DP_NOT_RESET));
    }
    switch (pll_cfg) {
    /******************/
    /*  Integer Mode  */
    /******************/
    case FALCON2_DINO_pll_div_80x :
     /* pll_ mode<4:0> = 00010, VCO = 17.0    G, Refclk = 212.5       MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x02));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_force_kvh_bw (   1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_kvh_force    (   2));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_DINO_pll_div_96x :
     /* pll_ mode<4:0> = 10000, VCO = 15.0    G, Refclk = 156.25      MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x10));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x1));
        break;
    case FALCON2_DINO_pll_div_120x :
     /* pll_ mode<4:0> = 10001, VCO = 18.75   G, Refclk = 156.25      MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x11));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x1));
        break;
    case FALCON2_DINO_pll_div_120x_refc125 :
     /* pll_ mode<4:0> = 10001, VCO = 15.0    G, Refclk = 125.0       MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x11));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_force_kvh_bw (   1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_kvh_force    (   3));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_DINO_pll_div_128x :
     /* pll_ mode<4:0> = 00011, VCO = 20.625  G, Refclk = 161.1328125 MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x03));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_128x_vco2 :
     /* pll_ mode<4:0> = 00011, VCO = 19.90656 G, Refclk = 155.52 MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x03));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_force_kvh_bw (   1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_kvh_force    (   0));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_DINO_pll_div_132x :
     /* pll_ mode<4:0> = 00100 , VCO = 20.625  G, Refclk = 156.25     MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x04));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_132x_refc212 :
     /* pll_ mode<4:0> = 00100 , VCO = 28.055  G, Refclk = 212.5      MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x04));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_force_kvh_bw (   1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_kvh_force    (   0));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco_indicator(   1));
        break;
    case FALCON2_DINO_pll_div_140x :
     /* pll_ mode<4:0> = 00101 , VCO = 21.875  G, Refclk = 156.25     MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x05));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_144x :
     /* pll_ mode<4:0> = 10010 , VCO = 22.39488G, Refclk = 155.52     MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x12));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_160x :
     /* pll_ mode<4:0> = 00110 , VCO = 25.0    G, Refclk = 156.25     MHz */
     /* pll_ mode<4:0> = 00110 , VCO = 25.78125G, Refclk = 161.1328125MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x06));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_160x_vco2 :
     /* pll_ mode<4:0> = 00110 , VCO = 17.0    G, Refclk = 106.25     MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x06));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_force_kvh_bw (   1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_kvh_force    (   2));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_DINO_pll_div_160x_refc174 :
     /* pll_mode<3:0> = 00110 , VCO = 27.9525 G, Refclk = 174.7       MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             ( 0x6));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_force_kvh_bw (   1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_kvh_force    (   0));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco_indicator(   1));
        break;
    case FALCON2_DINO_pll_div_165x :
     /* pll_ mode<4:0> = 00111, VCO = 25.78125G, Refclk = 156.25      MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x07));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_165x_refc125 :
     /* pll_ mode<4:0> = 00111, VCO = 20.625  G, Refclk = 125         MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x07));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_force_kvh_bw (   1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_kvh_force    (   3));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_DINO_pll_div_168x :
     /* pll_ mode<4:0> = 01000, VCO = 26.25   G, Refclk = 156.25      MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x08));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_175x :
     /* pll_ mode<4:0> = 01010, VCO = 27.34375G, Refclk = 156.25      MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x0A));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_180x :
     /* pll_ mode<4:0> = 01011 , VCO = 28.125  G, Refclk = 156.25     MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x0B));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_180x_refc125 :
     /* pll_ mode<4:0> = 01011 , VCO = 22.5    G, Refclk = 125.0      MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x0B));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_force_kvh_bw (   1));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_kvh_force    (   3));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_DINO_pll_div_184x :
     /* pll_ mode<4:0> = 01100 , VCO = 23.0    G, Refclk = 125.0      MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x0C));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_198x :
     /* pll_ mode<4:0> = 10011 , VCO = 24.75   G, Refclk = 125.0      MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x13));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_200x :
     /* pll_ mode<4:0> = 01101, VCO = 25.0    G, Refclk = 125.0       MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x0D));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_224x :
     /* pll_ mode<4:0> = 01110 , VCO = 28.0    G, Refclk = 125.0      MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x0E));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_264x :
     /* pll_ mode<4:0> = 01111 , VCO = 28.05   G, Refclk = 106.25     MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x0F));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_64x_refc322 :
        /* pll_ mode<4:0> = 00000 , VCO = 20.625 G, Refclk = 322.0 MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x00));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_DINO_pll_div_80x_refc322 :
        /* pll_ mode<4:0> = 00010 , VCO = 25.78125G, Refclk = 322.0 MHz */
        FALCON2_DINO_EFUN(falcon2_dino_wrc_pll_mode             (0x02));
        FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_vco2_15g     ( 0x0));
        break;

    /*******************************/
    /*  Invalid 'pll_cfg' Selector */
    /*******************************/
    default:
        return falcon2_dino_error(ERR_CODE_INVALID_PLL_CFG);
        break;
    }   /* switch (pll_cfg) */
    FALCON2_DINO_EFUN(falcon2_dino_wrc_ams_pll_pwrdn(0x0));
    return ERR_CODE_NONE;
}   /* falcon2_dino_configure_pll */
