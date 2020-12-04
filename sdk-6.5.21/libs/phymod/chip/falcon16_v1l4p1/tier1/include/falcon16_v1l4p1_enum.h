/***********************************************************************************
 *                                                                                 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/******************************************************************************
 ******************************************************************************
 *  Revision      :   *
 *                                                                            *
 *  Description   :  Enum types used by Serdes API functions                  *
 */

/** @file falcon16_v1l4p1_enum.h
 * Enum types used by Serdes API functions
 */

#ifndef FALCON16_V1L4P1_API_ENUM_H
#define FALCON16_V1L4P1_API_ENUM_H

#include "falcon16_v1l4p1_ipconfig.h"
#ifdef INCLUDE_STD_HEADERS
#include <stdint.h>
#endif


enum falcon16_v1l4p1_pll_refclk_enum {
    FALCON16_V1L4P1_PLL_REFCLK_UNKNOWN = 0, /* Refclk value to be determined by API.  */
    FALCON16_V1L4P1_PLL_REFCLK_100MHZ          = (int)0x00100064, /* 100 MHz          */
    FALCON16_V1L4P1_PLL_REFCLK_106P25MHZ       = (int)0x004001A9, /* 106.25 MHz       */
    FALCON16_V1L4P1_PLL_REFCLK_122P88MHZ       = (int)0x01900C00, /* 122.88 MHz       */
    FALCON16_V1L4P1_PLL_REFCLK_125MHZ          = (int)0x0010007D, /* 125 MHz          */
    FALCON16_V1L4P1_PLL_REFCLK_155P52MHZ       = (int)0x01900F30, /* 155.52 MHz       */
    FALCON16_V1L4P1_PLL_REFCLK_156P25MHZ       = (int)0x00400271, /* 156.25 MHz       */
    FALCON16_V1L4P1_PLL_REFCLK_159P375MHZ      = (int)0x008004FB, /* 159.375 MHz      */
    FALCON16_V1L4P1_PLL_REFCLK_161MHZ          = (int)0x001000A1, /* 161    MHz       */
    FALCON16_V1L4P1_PLL_REFCLK_161P1328125MHZ  = (int)0x08005091, /* 161.1328125 MHz  */
    FALCON16_V1L4P1_PLL_REFCLK_166P67MHZ       = (int)0x0640411B, /* 166.67 MHz       */
    FALCON16_V1L4P1_PLL_REFCLK_174P703125MHZ   = (int)0x04002BAD, /* 174.703125 MHz   */
    FALCON16_V1L4P1_PLL_REFCLK_176P45MHZ       = (int)0x01400DC9, /* 176.45 MHz       */
    FALCON16_V1L4P1_PLL_REFCLK_212P5MHZ        = (int)0x002001A9, /* 212.5  MHz       */
    FALCON16_V1L4P1_PLL_REFCLK_322MHZ          = (int)0x00100142, /* 322    MHz       */
    FALCON16_V1L4P1_PLL_REFCLK_352P9MHZ        = (int)0x00A00DC9  /* 352.9  MHz       */
    };


enum falcon16_v1l4p1_pll_div_enum {
    FALCON16_V1L4P1_PLL_DIV_UNKNOWN = 0, /* Divide value to be determined by API. */
    FALCON16_V1L4P1_PLL_DIV_64         = (int)0x00000040, /* Divide by 64         */
    FALCON16_V1L4P1_PLL_DIV_66         = (int)0x00000042, /* Divide by 66         */
    FALCON16_V1L4P1_PLL_DIV_80         = (int)0x00000050, /* Divide by 80         */
    FALCON16_V1L4P1_PLL_DIV_96         = (int)0x00000060, /* Divide by 96         */
    FALCON16_V1L4P1_PLL_DIV_100        = (int)0x00000064, /* Divide by 100        */
    FALCON16_V1L4P1_PLL_DIV_120        = (int)0x00000078, /* Divide by 120        */
    FALCON16_V1L4P1_PLL_DIV_127P401984 = (int)0x66E8807F, /* Divide by 127.401984 */
    FALCON16_V1L4P1_PLL_DIV_128        = (int)0x00000080, /* Divide by 128        */
    FALCON16_V1L4P1_PLL_DIV_132        = (int)0x00000084, /* Divide by 132        */
    FALCON16_V1L4P1_PLL_DIV_140        = (int)0x0000008C, /* Divide by 140        */
    FALCON16_V1L4P1_PLL_DIV_144        = (int)0x00000090, /* Divide by 144        */
    FALCON16_V1L4P1_PLL_DIV_147P2      = (int)0x33330093, /* Divide by 147.2      */
    FALCON16_V1L4P1_PLL_DIV_158P4      = (int)0x6666809E, /* Divide by 158.4      */
    FALCON16_V1L4P1_PLL_DIV_160        = (int)0x000000A0, /* Divide by 160        */
    FALCON16_V1L4P1_PLL_DIV_165        = (int)0x000000A5, /* Divide by 165        */
    FALCON16_V1L4P1_PLL_DIV_168        = (int)0x000000A8, /* Divide by 168        */
    FALCON16_V1L4P1_PLL_DIV_170        = (int)0x000000AA, /* Divide by 170        */
    FALCON16_V1L4P1_PLL_DIV_175        = (int)0x000000AF, /* Divide by 175        */
    FALCON16_V1L4P1_PLL_DIV_180        = (int)0x000000B4, /* Divide by 180        */
    FALCON16_V1L4P1_PLL_DIV_184        = (int)0x000000B8, /* Divide by 184        */
    FALCON16_V1L4P1_PLL_DIV_192        = (int)0x000000C0, /* Divide by 192        */
    FALCON16_V1L4P1_PLL_DIV_198        = (int)0x000000C6, /* Divide by 198        */
    FALCON16_V1L4P1_PLL_DIV_200        = (int)0x000000C8, /* Divide by 200        */
    FALCON16_V1L4P1_PLL_DIV_224        = (int)0x000000E0, /* Divide by 224        */
    FALCON16_V1L4P1_PLL_DIV_240        = (int)0x000000F0, /* Divide by 240        */
    FALCON16_V1L4P1_PLL_DIV_264        = (int)0x00000108, /* Divide by 264        */
    FALCON16_V1L4P1_PLL_DIV_280        = (int)0x00000118, /* Divide by 280        */
    FALCON16_V1L4P1_PLL_DIV_330        = (int)0x0000014A, /* Divide by 330        */
    FALCON16_V1L4P1_PLL_DIV_350        = (int)0x0000015E  /* Divide by 350        */
};

/** TX AFE Settings Enum */
enum falcon16_v1l4p1_tx_afe_settings_enum {
    TX_AFE_PRE,
    TX_AFE_MAIN,
    TX_AFE_POST1,
    TX_AFE_POST2,
    TX_AFE_POST3,
    TX_AFE_RPARA
};


#define FALCON16_V1L4P1_PLL_OPTION_REFCLK_MASK 0xF

/** PLL Configuration Options Enum */
enum falcon16_v1l4p1_pll_option_enum {
    FALCON16_V1L4P1_PLL_OPTION_NONE              =  0,
    FALCON16_V1L4P1_PLL_OPTION_REFCLK_DOUBLER_EN =  1,
    FALCON16_V1L4P1_PLL_OPTION_REFCLK_DIV2_EN    =  2,
    FALCON16_V1L4P1_PLL_OPTION_REFCLK_DIV4_EN    =  3,
    FALCON16_V1L4P1_PLL_OPTION_DISABLE_VERIFY    = 16
};

#endif
