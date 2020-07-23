/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/******************************************************************************
 ******************************************************************************
 *  Revision      :   *
 *                                                                            *
 *  Description   :  Enum types used by Serdes API functions                  *
 */

/** @file blackhawk7_v1l8p1_enum.h
 * Enum types used by Serdes API functions
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLACKHAWK7_V1L8P1_API_ENUM_H
#define BLACKHAWK7_V1L8P1_API_ENUM_H

#include "blackhawk7_v1l8p1_ipconfig.h"
#ifdef EXCLUDE_STD_HEADERS
#include <stdint.h>
#endif


enum blackhawk7_v1l8p1_pll_refclk_enum {
    BLACKHAWK7_V1L8P1_PLL_REFCLK_UNKNOWN = 0, /* Refclk value to be determined by API. */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_145P947802MHZ  = (int)0x16C0CF85, /* 145.947802 MHz  */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_166P7974882MHZ = (int)0x27D19F0A, /* 166.7974882 MHz */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_167P4107143MHZ = (int)0x0380249F, /* 167.4107143 MHz */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_245P76MHZ      = (int)0x01901800, /* 245.76 MHz      */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_250MHZ         = (int)0x001000FA, /* 250 MHz         */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_425MHZ         = (int)0x001001A9, /* 425 MHz         */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_534MHZ         = (int)0x00100216, /* 534 MHz         */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_705MHZ         = (int)0x001002C1, /* 705 MHz         */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_175MHZ         = (int)0x001000AF, /* 175 MHz         */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_400MHZ         = (int)0x00100190, /* 400 MHz         */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_266P67MHZ      = (int)0x0640682B, /* 266.67 MHz      */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_100MHZ         = (int)0x00100064, /* 100 MHz         */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_106P25MHZ      = (int)0x004001A9, /* 106.25 MHz      */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_122P88MHZ      = (int)0x01900C00, /* 122.88 MHz      */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_125MHZ         = (int)0x0010007D, /* 125 MHz         */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_155P52MHZ      = (int)0x01900F30, /* 155.52 MHz      */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_156P25MHZ      = (int)0x00400271, /* 156.25 MHz      */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_159P375MHZ     = (int)0x008004FB, /* 159.375 MHz     */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_161P1328125MHZ = (int)0x08005091, /* 161.1328125 MHz */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_166P67MHZ      = (int)0x0640411B, /* 166.67 MHz      */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_166P015625MHZ  = (int)0x64040D99, /* 166.015625 MHz */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_167P38MHZ      = (int)0x06404162, /* 167.38 MHz      */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_174P703125MHZ  = (int)0x04002BAD, /* 174.703125 MHz  */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_212P5MHZ       = (int)0x002001A9, /* 212.5 MHz       */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_311P04MHZ      = (int)0x01901E60, /* 311.04 MHz      */
    BLACKHAWK7_V1L8P1_PLL_REFCLK_312P5MHZ       = (int)0x00200271  /* 312.5 MHz       */
    };


enum blackhawk7_v1l8p1_pll_div_enum {
    BLACKHAWK7_V1L8P1_PLL_DIV_UNKNOWN = 0, /* Divide value to be determined by API. */
    BLACKHAWK7_V1L8P1_PLL_DIV_66         = (int)0x00000042, /* Divide by 66         */
    BLACKHAWK7_V1L8P1_PLL_DIV_67         = (int)0x00000043, /* Divide by 67         */
    BLACKHAWK7_V1L8P1_PLL_DIV_70         = (int)0x00000046, /* Divide by 70         */
    BLACKHAWK7_V1L8P1_PLL_DIV_72         = (int)0x00000048, /* Divide by 72         */
    BLACKHAWK7_V1L8P1_PLL_DIV_74P25      = (int)0x4000004A, /* Divide by 74.25      */
    BLACKHAWK7_V1L8P1_PLL_DIV_76         = (int)0x0000004C, /* Divide by 76         */
    BLACKHAWK7_V1L8P1_PLL_DIV_73P6       = (int)0x99998049, /* Divide by 73.6       */
    BLACKHAWK7_V1L8P1_PLL_DIV_79P2       = (int)0x3333004F, /* Divide by 79.2       */
    BLACKHAWK7_V1L8P1_PLL_DIV_80         = (int)0x00000050, /* Divide by 80         */
    BLACKHAWK7_V1L8P1_PLL_DIV_82P5       = (int)0x80000052, /* Divide by 82.5       */
    BLACKHAWK7_V1L8P1_PLL_DIV_84         = (int)0x00000054, /* Divide by 84         */
    BLACKHAWK7_V1L8P1_PLL_DIV_85         = (int)0x00000055, /* Divide by 85         */
    BLACKHAWK7_V1L8P1_PLL_DIV_87P5       = (int)0x80000057, /* Divide by 87.5       */
    BLACKHAWK7_V1L8P1_PLL_DIV_89P6       = (int)0x99998059, /* Divide by 89.6       */
    BLACKHAWK7_V1L8P1_PLL_DIV_90         = (int)0x0000005A, /* Divide by 90         */
    BLACKHAWK7_V1L8P1_PLL_DIV_90P75      = (int)0xC000005A, /* Divide by 90.75      */
    BLACKHAWK7_V1L8P1_PLL_DIV_96         = (int)0x00000060, /* Divide by 96         */
    BLACKHAWK7_V1L8P1_PLL_DIV_100        = (int)0x00000064, /* Divide by 100        */
    BLACKHAWK7_V1L8P1_PLL_DIV_120        = (int)0x00000078, /* Divide by 120        */
    BLACKHAWK7_V1L8P1_PLL_DIV_127P401984 = (int)0x66E8807F, /* Divide by 127.401984 */
    BLACKHAWK7_V1L8P1_PLL_DIV_128        = (int)0x00000080, /* Divide by 128        */
    BLACKHAWK7_V1L8P1_PLL_DIV_136        = (int)0x00000088, /* Divide by 136        */
    BLACKHAWK7_V1L8P1_PLL_DIV_140        = (int)0x0000008C, /* Divide by 140        */
    BLACKHAWK7_V1L8P1_PLL_DIV_144        = (int)0x00000090, /* Divide by 144        */
    BLACKHAWK7_V1L8P1_PLL_DIV_152        = (int)0x00000098, /* Divide by 152        */
    BLACKHAWK7_V1L8P1_PLL_DIV_184        = (int)0x000000B8, /* Divide by 184        */
    BLACKHAWK7_V1L8P1_PLL_DIV_192        = (int)0x000000C0, /* Divide by 192        */
    BLACKHAWK7_V1L8P1_PLL_DIV_198        = (int)0x000000C6, /* Divide by 198        */
    BLACKHAWK7_V1L8P1_PLL_DIV_200        = (int)0x000000C8, /* Divide by 200        */
    BLACKHAWK7_V1L8P1_PLL_DIV_224        = (int)0x000000E0, /* Divide by 224        */
    BLACKHAWK7_V1L8P1_PLL_DIV_240        = (int)0x000000F0, /* Divide by 240        */
    BLACKHAWK7_V1L8P1_PLL_DIV_54         = (int)0x00000036, /* Divide by 54         */
    BLACKHAWK7_V1L8P1_PLL_DIV_108        = (int)0x0000006C, /* Divide by 108        */
    BLACKHAWK7_V1L8P1_PLL_DIV_132        = (int)0x00000084, /* Divide by 132        */
    BLACKHAWK7_V1L8P1_PLL_DIV_147P2      = (int)0x33330093, /* Divide by 147.2      */
    BLACKHAWK7_V1L8P1_PLL_DIV_158P4      = (int)0x6666809E, /* Divide by 158.4      */
    BLACKHAWK7_V1L8P1_PLL_DIV_160        = (int)0x000000A0, /* Divide by 160        */
    BLACKHAWK7_V1L8P1_PLL_DIV_165        = (int)0x000000A5, /* Divide by 165        */
    BLACKHAWK7_V1L8P1_PLL_DIV_168        = (int)0x000000A8, /* Divide by 168        */
    BLACKHAWK7_V1L8P1_PLL_DIV_170        = (int)0x000000AA, /* Divide by 170        */
    BLACKHAWK7_V1L8P1_PLL_DIV_175        = (int)0x000000AF, /* Divide by 175        */
    BLACKHAWK7_V1L8P1_PLL_DIV_180        = (int)0x000000B4, /* Divide by 180        */
    BLACKHAWK7_V1L8P1_PLL_DIV_264        = (int)0x00000108, /* Divide by 264        */
    BLACKHAWK7_V1L8P1_PLL_DIV_280        = (int)0x00000118, /* Divide by 280        */
    BLACKHAWK7_V1L8P1_PLL_DIV_330        = (int)0x0000014A, /* Divide by 330        */
    BLACKHAWK7_V1L8P1_PLL_DIV_350        = (int)0x0000015E  /* Divide by 350        */
};

/** TX AFE Settings Enum */
enum blackhawk7_v1l8p1_tx_afe_settings_enum {
    BLACKHAWK7_V1L8P1_TX_AFE_TAP0,
    BLACKHAWK7_V1L8P1_TX_AFE_TAP1,
    BLACKHAWK7_V1L8P1_TX_AFE_TAP2,
    BLACKHAWK7_V1L8P1_TX_AFE_TAP3,
    BLACKHAWK7_V1L8P1_TX_AFE_TAP4,
    BLACKHAWK7_V1L8P1_TX_AFE_TAP5
};

/** TXFIR Tap Enable Enum */
enum blackhawk7_v1l8p1_txfir_tap_enable_enum {
    BLACKHAWK7_V1L8P1_NRZ_LP_3TAP  = 0,
    BLACKHAWK7_V1L8P1_NRZ_6TAP     = 1,
    BLACKHAWK7_V1L8P1_PAM4_LP_3TAP = 2,
    BLACKHAWK7_V1L8P1_PAM4_6TAP    = 3
};

#define BLACKHAWK7_V1L8P1_PLL_OPTION_REFCLK_MASK 0xF

/** PLL Configuration Options Enum */
enum blackhawk7_v1l8p1_pll_option_enum {
    BLACKHAWK7_V1L8P1_PLL_OPTION_NONE              =  0,
    BLACKHAWK7_V1L8P1_PLL_OPTION_REFCLK_DOUBLER_EN =  1,
    BLACKHAWK7_V1L8P1_PLL_OPTION_REFCLK_DIV2_EN    =  2,
    BLACKHAWK7_V1L8P1_PLL_OPTION_REFCLK_DIV4_EN    =  3,
    BLACKHAWK7_V1L8P1_PLL_OPTION_POWERDOWN         =  4,
    BLACKHAWK7_V1L8P1_PLL_OPTION_DISABLE_VERIFY    = 16
};

/** DCO DAC Step Size Enum */
enum blackhawk7_v1l8p1_dco_dac_step_size_enum {
    BLACKHAWK7_V1L8P1_DCO_DAC_STEP_SIZE_0P8_MV           = 2,
    BLACKHAWK7_V1L8P1_DCO_DAC_STEP_SIZE_1P2_MV           = 3
};
/** DCO DAC Step Size Enum */
enum blackhawk7_v1l8p1_dco_dac_step_size_boost_enum {
    BLACKHAWK7_V1L8P1_DCO_DAC_STEP_SIZE_BOOST_n12P5_PCT  = 1,
    BLACKHAWK7_V1L8P1_DCO_DAC_STEP_SIZE_BOOST_0_PCT      = 3,
    BLACKHAWK7_V1L8P1_DCO_DAC_STEP_SIZE_BOOST_12P5_PCT   = 5,
    BLACKHAWK7_V1L8P1_DCO_DAC_STEP_SIZE_BOOST_25_PCT     = 7
};

#endif /*#define BLACKHAWK7_V1L8P1_API_ENUM_H*/
#ifdef __cplusplus
}
#endif
