/**************************************************************************************
 **************************************************************************************
 *  File Name     :  falcon2_madura_pll_config.c                                      *
 *  Created On    :  2015-01-20                                                       *
 *  Created By    :  Eric Roesinger                                                   *
 *  Description   :  Falcon2 Madura PLL Configuration API                             *
 *  Revision      :  $Id: falcon2_madura_pll_config.c 1340 2016-02-04 22:50:01Z kirand $                         *
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
 * Falcon2/Madura Repeater PLL Configuration
 */

#include "falcon2_madura_enum.h"


err_code_t falcon2_madura_configure_pll( const phymod_access_t *pa, enum falcon2_madura_pll_enum pll_cfg) {
#ifndef ATE_LOG
    uint8_t reset_state;

 /* Restore defaults, needed for non-register reset cases */
 /* EFUN(wrc_pll_mode                                         (   0x7));            */
 /* EFUN(wrc_ams_pll_vco2_15g                                 (   0x0));    */
 /* EFUN(wrc_ams_pll_force_kvh_bw                             (   0));  */
 /* EFUN(wrc_ams_pll_kvh_force                                (   0));     */
 /* EFUN(wrc_ams_pll_vco_indicator                            (   0)); */

 /* Use core_s_rstb to re-initialize all registers to default before calling this function. */
    ESTM(reset_state = rdc_falcon2_madura_core_dp_reset_state());
    if(reset_state < 7) {
        EFUN_PRINTF(("ERROR: falcon2_madura_configure_pll( pa, ..) called for PLL %u without core_dp_s_rstb=0\n", (unsigned)falcon2_madura_get_pll(pa)));
        return (_error(ERR_CODE_CORE_DP_NOT_RESET));
    }
#endif
    switch (pll_cfg) {
    /******************/
    /*  Integer Mode  */
    /******************/
    case FALCON2_MADURA_pll_div_80x :
     /* pll_ mode<4:0> = 00010, VCO = 17.0    G, Refclk = 212.5       MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x02));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x1));
        EFUN(wrc_falcon2_madura_ams_pll_force_kvh_bw (   1));
        EFUN(wrc_falcon2_madura_ams_pll_kvh_force    (   2));
        EFUN(wrc_falcon2_madura_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_MADURA_pll_div_96x :
     /* pll_ mode<4:0> = 10000, VCO = 15.0    G, Refclk = 156.25      MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x10));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x1));
        break;
    case FALCON2_MADURA_pll_div_120x :
     /* pll_ mode<4:0> = 10001, VCO = 18.75   G, Refclk = 156.25      MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x11));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x1));
        break;
    case FALCON2_MADURA_pll_div_120x_refc125 :
     /* pll_ mode<4:0> = 10001, VCO = 15.0    G, Refclk = 125.0       MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x11));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x1));
        EFUN(wrc_falcon2_madura_ams_pll_force_kvh_bw (   1));
        EFUN(wrc_falcon2_madura_ams_pll_kvh_force    (   3));
        EFUN(wrc_falcon2_madura_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_MADURA_pll_div_128x :
     /* pll_ mode<4:0> = 00011, VCO = 20.625  G, Refclk = 161.1328125 MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x03));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_128x_vco2 :
     /* pll_ mode<4:0> = 00011, VCO = 19.90656 G, Refclk = 155.52 MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x03));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x1));
        EFUN(wrc_falcon2_madura_ams_pll_force_kvh_bw (   1));
        EFUN(wrc_falcon2_madura_ams_pll_kvh_force    (   0));
        EFUN(wrc_falcon2_madura_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_MADURA_pll_div_132x :
     /* pll_ mode<4:0> = 00100 , VCO = 20.625  G, Refclk = 156.25     MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x04));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_132x_refc212 :
     /* pll_ mode<4:0> = 00100 , VCO = 28.055  G, Refclk = 212.5      MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x04));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        EFUN(wrc_falcon2_madura_ams_pll_force_kvh_bw (   1));
        EFUN(wrc_falcon2_madura_ams_pll_kvh_force    (   0));
        EFUN(wrc_falcon2_madura_ams_pll_vco_indicator(   1));
        break;
    case FALCON2_MADURA_pll_div_140x :
     /* pll_ mode<4:0> = 00101 , VCO = 21.875  G, Refclk = 156.25     MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x05));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_144x :
     /* pll_ mode<4:0> = 10010 , VCO = 22.39488G, Refclk = 155.52     MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x12));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_160x :
     /* pll_ mode<4:0> = 00110 , VCO = 25.0    G, Refclk = 156.25     MHz */
     /* pll_ mode<4:0> = 00110 , VCO = 25.78125G, Refclk = 161.1328125MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x06));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_160x_vco2 :
     /* pll_ mode<4:0> = 00110 , VCO = 17.0    G, Refclk = 106.25     MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x06));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x1));
        EFUN(wrc_falcon2_madura_ams_pll_force_kvh_bw (   1));
        EFUN(wrc_falcon2_madura_ams_pll_kvh_force    (   2));
        EFUN(wrc_falcon2_madura_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_MADURA_pll_div_160x_refc174 :
     /* pll_mode<3:0> = 00110 , VCO = 27.9525 G, Refclk = 174.7       MHz */
        EFUN(wrc_falcon2_madura_pll_mode             ( 0x6));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        EFUN(wrc_falcon2_madura_ams_pll_force_kvh_bw (   1));
        EFUN(wrc_falcon2_madura_ams_pll_kvh_force    (   0));
        EFUN(wrc_falcon2_madura_ams_pll_vco_indicator(   1));
        break;
    case FALCON2_MADURA_pll_div_165x :
     /* pll_ mode<4:0> = 00111, VCO = 25.78125G, Refclk = 156.25      MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x07));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_165x_refc125 :
     /* pll_ mode<4:0> = 00111, VCO = 20.625  G, Refclk = 125         MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x07));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        EFUN(wrc_falcon2_madura_ams_pll_force_kvh_bw (   1));
        EFUN(wrc_falcon2_madura_ams_pll_kvh_force    (   3));
        EFUN(wrc_falcon2_madura_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_MADURA_pll_div_168x :
     /* pll_ mode<4:0> = 01000, VCO = 26.25   G, Refclk = 156.25      MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x08));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_175x :
     /* pll_ mode<4:0> = 01010, VCO = 27.34375G, Refclk = 156.25      MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x0A));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_180x :
     /* pll_ mode<4:0> = 01011 , VCO = 28.125  G, Refclk = 156.25     MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x0B));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_180x_refc125 :
     /* pll_ mode<4:0> = 01011 , VCO = 22.5    G, Refclk = 125.0      MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x0B));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        EFUN(wrc_falcon2_madura_ams_pll_force_kvh_bw (   1));
        EFUN(wrc_falcon2_madura_ams_pll_kvh_force    (   3));
        EFUN(wrc_falcon2_madura_ams_pll_vco_indicator(   0));
        break;
    case FALCON2_MADURA_pll_div_184x :
     /* pll_ mode<4:0> = 01100 , VCO = 23.0    G, Refclk = 125.0      MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x0C));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_198x :
     /* pll_ mode<4:0> = 10011 , VCO = 24.75   G, Refclk = 125.0      MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x13));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_200x :
     /* pll_ mode<4:0> = 01101, VCO = 25.0    G, Refclk = 125.0       MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x0D));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_224x :
     /* pll_ mode<4:0> = 01110 , VCO = 28.0    G, Refclk = 125.0      MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x0E));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    case FALCON2_MADURA_pll_div_264x :
     /* pll_ mode<4:0> = 01111 , VCO = 28.05   G, Refclk = 106.25     MHz */
        EFUN(wrc_falcon2_madura_pll_mode             (0x0F));
        EFUN(wrc_falcon2_madura_ams_pll_vco2_15g     ( 0x0));
        break;
    /*******************************/
    /*  Invalid 'pll_cfg' Selector */
    /*******************************/
    default:
        return _error(ERR_CODE_INVALID_PLL_CFG);
        break;
    }   /* switch (pll_cfg) */
    EFUN(wrc_falcon2_madura_ams_pll_pwrdn(0x0));
    return ERR_CODE_NONE;
}   /* falcon2_madura_configure_pll */
