/*******************************************************************************
 *******************************************************************************
 *  File Name     :  eagle_pll_config.c                                        *
 *  Created On    :  14/07/2013                                                *
 *  Created By    :  Kiran Divakar                                             *
 *  Description   :  Eagle PLL Configuration API                               *
 *  Revision      :  $Id: eagle_pll_config.c 1543 2017-04-07 20:52:44Z kirand $ *
 *                                                                             *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                       *
 *  No portions of this material may be reproduced in any form without         *
 *  the written permission of:                                                 *
 *      Broadcom Corporation                                                   *
 *      5300 California Avenue                                                 *
 *      Irvine, CA  92617                                                      *
 *                                                                             *
 *  All information contained in this document is Broadcom Corporation         *
 *  company private proprietary, and trade secret.                             *
 */

/** @file
 * Eagle PLL Configuration
 */

#include "eagle_phy_1_enum.h"

err_code_t eagle_phy_1_configure_pll(enum eagle_phy_1_pll_enum pll_cfg) {

    /* KVH VCO Range          */
    /* 00  11.5    -  13.125  */
    /* 01  10.3125 - <11.5    */
    /* 10   9.375  - <10.3125 */
    /* 11   8.125  - < 9.375  */

    /* CKBWVCO Range          */
    /* 00  12.5               */
    /* 10  10.625  - 11.5G    */
    /* 01   9.375  - 10.3125G */
    /* 11  <9.375G            */

    /*  Use this to restore defaults if reprogramming the PLL under dp-reset (typically Auto-Neg FW) */
    /*  EFUN(wrc_pll_mode              (   0xA)); */
    /*  EFUN(wrc_ams_pll_fracn_ndiv_int(   0x0)); */
    /*  EFUN(wrc_ams_pll_fracn_div_h   (   0x0)); */
    /*  EFUN(wrc_ams_pll_fracn_div_l   (   0x0)); */
    /*  EFUN(wrc_ams_pll_fracn_bypass  (   0x0)); */
    /*  EFUN(wrc_ams_pll_fracn_divrange(   0x0)); */
    /*  EFUN(wrc_ams_pll_fracn_sel     (   0x0)); */
    /*  EFUN(wrc_ams_pll_ditheren      (   0x0)); */
    /*  EFUN(wrc_ams_pll_force_kvh_bw  (   0x0)); */
    /*  EFUN(wrc_ams_pll_kvh_force     (   0x0)); */
    /*  EFUN(wrc_ams_pll_2rx_clkbw     (   0x0)); */
    /*  EFUN(wrc_ams_pll_vco_div2      (   0x0)); */
    /*  EFUN(wrc_ams_pll_vco_div4      (   0x0)); */
    /*  EFUN(wrc_ams_pll_refclk_doubler(   0x0)); */
    /*  EFUN(wrc_ams_pll_fp3_ctrl      (   0x0)); */
    /*  EFUN(wrc_ams_pll_fp3_rh        (   0x0)); */

    /* Use core_s_rstb to re-initialize all registers to default before calling this function. */
#ifndef ATE_LOG
    uint8_t reset_state;

    ESTM(reset_state = rdc_core_dp_reset_state());
    if(reset_state < 7) {
        EFUN_PRINTF(("ERROR: eagle_phy_1_configure_pll(..) called without core_dp_s_rstb=0\n"));
        return (_error(ERR_CODE_CORE_DP_NOT_RESET));
    }
#endif
    switch (pll_cfg) {
    /******************/
    /*  Integer Mode  */
    /******************/
    case EAGLE_PHY_1_pll_div_80x_refc125: /* pll_mode<3:0> 1101 VCO 10G      REF 125   MHz */
        EFUN(wrc_pll_mode              ( 0xD));
        EFUN(wrc_ams_pll_force_kvh_bw  ( 0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     ( 0x1));
        EFUN(wrc_ams_pll_kvh_force     ( 0x2));
        EFUN(wrc_ams_pll_vco_div2      ( 0x0));
        break;
    case EAGLE_PHY_1_pll_div_80x_refc106: /* pll_mode<3:0> 1101 VCO  8.5G    REF 106.25MHz */
        EFUN(wrc_pll_mode              ( 0xD));
        EFUN(wrc_ams_pll_force_kvh_bw  ( 0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     ( 0x3));
        EFUN(wrc_ams_pll_kvh_force     ( 0x3));
        EFUN(wrc_ams_pll_vco_div2 ( 0x0));
        break;
    case EAGLE_PHY_1_pll_div_80x:
    case EAGLE_PHY_1_pll_div_80x_refc156: /* pll_mode<3:0> 1101 VCO 12.5G    REF 156.25MHz */
        EFUN(wrc_pll_mode             ( 0xD));
        EFUN(wrc_ams_pll_force_kvh_bw ( 0x1));
        EFUN(wrc_ams_pll_2rx_clkbw    ( 0x0));
        EFUN(wrc_ams_pll_kvh_force    ( 0x2));
        EFUN(wrc_ams_pll_vco_div2     ( 0x0));
        break;
    case EAGLE_PHY_1_pll_div_46x:         /* pll_mode<3:0> 0000 VCO  5.75G   REF 125   MHz */
        EFUN(wrc_pll_mode              ( 0x0));
        break;
    case EAGLE_PHY_1_pll_div_50x:         /* pll_mode<3:0> 0101 VCO  6.25G   REF 125   MHz */
        EFUN(wrc_pll_mode              ( 0x5));
        break;
    case EAGLE_PHY_1_pll_div_68x:         /* pll_mode<3:0> 1011 VCO  8.5G    REF 125   MHz */
        EFUN(wrc_pll_mode              ( 0xB));
        break;
    case EAGLE_PHY_1_pll_div_92x:         /* pll_mode<3:0> 1110 VCO 11.5G    REF 125   MHz */
        EFUN(wrc_pll_mode              ( 0xE));
        break;
    case EAGLE_PHY_1_pll_div_100x:        /* pll_mode<3:0> 1111 VCO 12.5G    REF 125   MHz */
        EFUN(wrc_pll_mode             ( 0xF));
        EFUN(wrc_ams_pll_force_kvh_bw ( 0x1));
        EFUN(wrc_ams_pll_2rx_clkbw    ( 0x0));
        EFUN(wrc_ams_pll_kvh_force    ( 0x2));
        EFUN(wrc_ams_pll_vco_div2     ( 0x0));
        break;
    case EAGLE_PHY_1_pll_div_40x:         /* pll_mode<3:0> 0010 VCO  6.25G   REF 156.25MHz  */
        EFUN(wrc_pll_mode              ( 0x2));
        EFUN(wrc_ams_pll_force_kvh_bw  ( 0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     ( 0x3));
        EFUN(wrc_ams_pll_kvh_force     ( 0x2));
        EFUN(wrc_ams_pll_vco_div2      ( 0x1));
        break;
    case EAGLE_PHY_1_pll_div_42x:         /* pll_mode<3:0> 0011 VCO  6.5625G REF 156.25MHz */
        EFUN(wrc_pll_mode              ( 0x3));
        EFUN(wrc_ams_pll_force_kvh_bw  ( 0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     ( 0x3));
        EFUN(wrc_ams_pll_kvh_force     ( 0x2));
        EFUN(wrc_ams_pll_vco_div2      ( 0x1));
        break;
    case EAGLE_PHY_1_pll_div_52x:         /* pll_mode<3:0> 0110 VCO  8.125G  REF 156.25MHz */
        EFUN(wrc_pll_mode              ( 0x6));
        break;
 /* 5.0G   mode NOT verified by digital team or DVT */
 /* case EAGLE_PHY_1_pll_div_32x:       *//* pll_mode<3:0> 0111 VCO  5.0G    REF 156.25MHz  */
 /*     EFUN(wrc_pll_mode              (   0x7));   */
 /*     break;                                      */
 /* 3.125G mode NOT verified by digital team or DVT */
 /* case EAGLE_PHY_1_pll_div_20x:       *//* pll_mode<3:0> 0100 VCO  3.125G  REF 156.25MHz  */
 /*     EFUN(wrc_pll_mode              (   0x4));   */
 /*     break;                                      */
    case EAGLE_PHY_1_pll_div_60x:
        /* pll_mode<3:0> 1000 VCO  9.375 G REF 156.25MHz */
        EFUN(wrc_pll_mode              (   0x8));
        break;
    case EAGLE_PHY_1_pll_div_64x_refc161:
        /* pll_mode<3:0> 1001 VCO 10.3125G REF 161.13MHz */
        EFUN(wrc_pll_mode              (   0x9));
        EFUN(wrc_ams_pll_force_kvh_bw  (   0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     (   0x1));
        EFUN(wrc_ams_pll_kvh_force     (   0x2));
        EFUN(wrc_ams_pll_vco_div2 ( 0x0));
        break;
    case EAGLE_PHY_1_pll_div_64x:
    case EAGLE_PHY_1_pll_div_64x_refc156:
        /* pll_mode<3:0> 1001 VCO 10G      REF 156.25MHzMHz */
        EFUN(wrc_pll_mode              (   0x9));
        EFUN(wrc_ams_pll_force_kvh_bw  (   0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     (   0x1));
        EFUN(wrc_ams_pll_kvh_force     (   0x1));
        EFUN(wrc_ams_pll_vco_div2 ( 0x0));
        break;
    case EAGLE_PHY_1_pll_div_66x:
        /* pll_mode<3:0> 1010 VCO 10.3125G REF 156.25MHz */
        EFUN(wrc_pll_mode              (   0xA));
        break;
    case EAGLE_PHY_1_pll_div_70x:
        /* pll_mode<3:0> 1100 VCO 10.9375G REF 156.25MHz  */
        EFUN(wrc_pll_mode              (   0xC));
        break;
    case EAGLE_PHY_1_pll_div_72x:
        /* pll_mode<3:0> 0001 VCO 11.25  G REF 156.25MHz */
        EFUN(wrc_pll_mode              (   0x1));
        break;
      /*****************/
      /*  Frac-N Mode  */
      /*****************/
    case EAGLE_PHY_1_pll_div_82p5x:  /* DIV  82.5  VCO 10.3125G REF 125   MHz */
        EFUN(wrc_ams_pll_fracn_ndiv_int(  0x52));
        EFUN(wrc_ams_pll_fracn_div_l   (0x0000));
        EFUN(wrc_ams_pll_fracn_div_h   (   0x2));
        EFUN(wrc_ams_pll_fracn_bypass  (   0x0));
        EFUN(wrc_ams_pll_fracn_divrange(   0x0));
        EFUN(wrc_ams_pll_fracn_sel     (   0x1));
        EFUN(wrc_ams_pll_ditheren      (   0x1));
        EFUN(wrc_ams_pll_force_kvh_bw  (   0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     (   0x1));
        EFUN(wrc_ams_pll_kvh_force     (   0x1));
        EFUN(wrc_ams_pll_vco_div2      (   0x0));
        EFUN(wrc_ams_pll_vco_div4      (   0x0));
        EFUN(wrc_ams_pll_refclk_doubler(   0x0));
        EFUN(wrc_ams_pll_fp3_ctrl      (   0x3));
        EFUN(wrc_ams_pll_fp3_rh        (   0x1));
        break;
    case EAGLE_PHY_1_pll_div_87p5x:  /* DIV  87.5  VCO 10.9375G REF 125   MHz */
        EFUN(wrc_ams_pll_fracn_ndiv_int(  0x57));
        EFUN(wrc_ams_pll_fracn_div_l   (0x0000));
        EFUN(wrc_ams_pll_fracn_div_h   (   0x2));
        EFUN(wrc_ams_pll_fracn_bypass  (   0x0));
        EFUN(wrc_ams_pll_fracn_divrange(   0x0));
        EFUN(wrc_ams_pll_fracn_sel     (   0x1));
        EFUN(wrc_ams_pll_ditheren      (   0x1));
        EFUN(wrc_ams_pll_force_kvh_bw  (   0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     (   0x2));
        EFUN(wrc_ams_pll_kvh_force     (   0x1));
        EFUN(wrc_ams_pll_vco_div2      (   0x0));
        EFUN(wrc_ams_pll_vco_div4      (   0x0));
        EFUN(wrc_ams_pll_refclk_doubler(   0x0));
        EFUN(wrc_ams_pll_fp3_ctrl      (   0x3));
        EFUN(wrc_ams_pll_fp3_rh        (   0x1));
        break;
    case EAGLE_PHY_1_pll_div_73p6x:  /* DIV  73.6  VCO 11.5G    REF 156.25MHz */
        EFUN(wrc_ams_pll_fracn_ndiv_int(  0x49));
        EFUN(wrc_ams_pll_fracn_div_l   (0x6666));
        EFUN(wrc_ams_pll_fracn_div_h   (   0x2));
        EFUN(wrc_ams_pll_fracn_bypass  (   0x0));
        EFUN(wrc_ams_pll_fracn_divrange(   0x0));
        EFUN(wrc_ams_pll_fracn_sel     (   0x1));
        EFUN(wrc_ams_pll_ditheren      (   0x1));
        EFUN(wrc_ams_pll_force_kvh_bw  (   0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     (   0x2));
        EFUN(wrc_ams_pll_kvh_force     (   0x0));
        EFUN(wrc_ams_pll_vco_div2      (   0x0));
        EFUN(wrc_ams_pll_vco_div4      (   0x0));
        EFUN(wrc_ams_pll_refclk_doubler(   0x0));
        EFUN(wrc_ams_pll_fp3_ctrl      (   0x3));
        EFUN(wrc_ams_pll_fp3_rh        (   0x1));
        break;
    case EAGLE_PHY_1_pll_div_36p8x:  /* DIV  73.6  VCO 11.5G/2  REF 156.25MHz */
        EFUN(wrc_ams_pll_fracn_ndiv_int(  0x49));
        EFUN(wrc_ams_pll_fracn_div_l   (0x6666));
        EFUN(wrc_ams_pll_fracn_div_h   (   0x2));
        EFUN(wrc_ams_pll_fracn_bypass  (   0x0));
        EFUN(wrc_ams_pll_fracn_divrange(   0x0));
        EFUN(wrc_ams_pll_fracn_sel     (   0x1));
        EFUN(wrc_ams_pll_ditheren      (   0x1));
        EFUN(wrc_ams_pll_force_kvh_bw  (   0x1));
        EFUN(wrc_ams_pll_kvh_force     (   0x0));
        EFUN(wrc_ams_pll_2rx_clkbw     (   0x3));
        EFUN(wrc_ams_pll_vco_div2      (   0x1));
        EFUN(wrc_ams_pll_vco_div4      (   0x0));
        EFUN(wrc_ams_pll_refclk_doubler(   0x0));
        EFUN(wrc_ams_pll_fp3_ctrl      (   0x3));
        EFUN(wrc_ams_pll_fp3_rh        (   0x1));
        break;
    case EAGLE_PHY_1_pll_div_199p04x:/* DIV 199.04 VCO  9.952G  REF  25   MHz */
        EFUN(wrc_ams_pll_fracn_ndiv_int(  0xc7));
        EFUN(wrc_ams_pll_fracn_div_l   (0x28f5));
        EFUN(wrc_ams_pll_fracn_div_h   (   0x0));
        EFUN(wrc_ams_pll_fracn_bypass  (   0x0));
        EFUN(wrc_ams_pll_fracn_divrange(   0x0));
        EFUN(wrc_ams_pll_fracn_sel     (   0x1));
        EFUN(wrc_ams_pll_ditheren      (   0x1));
        EFUN(wrc_ams_pll_force_kvh_bw  (   0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     (   0x1));
        EFUN(wrc_ams_pll_kvh_force     (   0x2));
        EFUN(wrc_ams_pll_vco_div2      (   0x0));
        EFUN(wrc_ams_pll_vco_div4      (   0x0));
        EFUN(wrc_ams_pll_refclk_doubler(   0x1));
        EFUN(wrc_ams_pll_fp3_ctrl      (   0x3));
        EFUN(wrc_ams_pll_fp3_rh        (   0x1));
        break;
    case EAGLE_PHY_1_pll_div_206p25x: /* DIV 206.25  VCO 10.3125G  REF 50 MHz */
        EFUN(wrc_ams_pll_fracn_ndiv_int(  0xCE));
        EFUN(wrc_ams_pll_fracn_div_l   (0x0000));
        EFUN(wrc_ams_pll_fracn_div_h   (   0x1));
        EFUN(wrc_ams_pll_fracn_bypass  (   0x0));
        EFUN(wrc_ams_pll_fracn_divrange(   0x0));
        EFUN(wrc_ams_pll_fracn_sel     (   0x1));
        EFUN(wrc_ams_pll_ditheren      (   0x1));
        EFUN(wrc_ams_pll_force_kvh_bw  (   0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     (   0x1));
        EFUN(wrc_ams_pll_kvh_force     (   0x2));
        EFUN(wrc_ams_pll_vco_div2      (   0x0));
        EFUN(wrc_ams_pll_vco_div4      (   0x0));
        EFUN(wrc_ams_pll_refclk_doubler(   0x0));
        EFUN(wrc_ams_pll_fp3_ctrl      (   0x3));
        EFUN(wrc_ams_pll_fp3_rh        (   0x1));
        EFUN(wrc_ams_pll_en_hrz        (   0x1));
        break;
    case EAGLE_PHY_1_pll_div_200p00x: /* DIV 200.00  VCO 10G  REF 50 MHz */
        EFUN(wrc_ams_pll_fracn_ndiv_int(  0xC8));
        EFUN(wrc_ams_pll_fracn_div_l   (0x0000));
        EFUN(wrc_ams_pll_fracn_div_h   (   0x0));
        EFUN(wrc_ams_pll_fracn_bypass  (   0x0));
        EFUN(wrc_ams_pll_fracn_divrange(   0x0));
        EFUN(wrc_ams_pll_fracn_sel     (   0x1));
        EFUN(wrc_ams_pll_ditheren      (   0x0));
        EFUN(wrc_ams_pll_force_kvh_bw  (   0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     (   0x1));
        EFUN(wrc_ams_pll_kvh_force     (   0x2));
        EFUN(wrc_ams_pll_vco_div2      (   0x0));
        EFUN(wrc_ams_pll_vco_div4      (   0x0));
        EFUN(wrc_ams_pll_refclk_doubler(   0x0));
        EFUN(wrc_ams_pll_fp3_ctrl      (   0x0));
        EFUN(wrc_ams_pll_fp3_rh        (   0x0));
        EFUN(wrc_ams_pll_en_hrz        (   0x1));
        break;
    case EAGLE_PHY_1_pll_div_250p00x: /* DIV 250.00  VCO 12.5G  REF 50 MHz */
        EFUN(wrc_ams_pll_fracn_ndiv_int(  0xFA));
        EFUN(wrc_ams_pll_fracn_div_l   (0x0000));
        EFUN(wrc_ams_pll_fracn_div_h   (   0x0));
        EFUN(wrc_ams_pll_fracn_bypass  (   0x0));
        EFUN(wrc_ams_pll_fracn_divrange(   0x0));
        EFUN(wrc_ams_pll_fracn_sel     (   0x1));
        EFUN(wrc_ams_pll_ditheren      (   0x0));
        EFUN(wrc_ams_pll_force_kvh_bw  (   0x1));
        EFUN(wrc_ams_pll_2rx_clkbw     (   0x0));
        EFUN(wrc_ams_pll_kvh_force     (   0x2));
        EFUN(wrc_ams_pll_vco_div2      (   0x0));
        EFUN(wrc_ams_pll_vco_div4      (   0x0));
        EFUN(wrc_ams_pll_refclk_doubler(   0x0));
        EFUN(wrc_ams_pll_fp3_ctrl      (   0x0));
        EFUN(wrc_ams_pll_fp3_rh        (   0x0));
        EFUN(wrc_ams_pll_en_hrz        (   0x1));
        break;
    /*******************************/
    /*  Invalid 'pll_cfg' Selector */
    /*******************************/
    default:                     /* Invalid pll_cfg value  */
        return _error(ERR_CODE_INVALID_PLL_CFG);
        break;
    }  /* switch (pll_cfg) */
    return ERR_CODE_NONE;
}   /* eagle_phy_1_configure_pll */
