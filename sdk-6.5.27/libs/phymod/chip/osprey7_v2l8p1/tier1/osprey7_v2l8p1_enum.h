/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2021 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/******************************************************************************
 ******************************************************************************
 *  Revision      :   *
 *                                                                            *
 *  Description   :  Enum types used by Serdes API functions                  *
 *                                                                            *
 ******************************************************************************
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OSPREY7_V2L8P1_API_ENUM_H
#define OSPREY7_V2L8P1_API_ENUM_H

#include "osprey7_v2l8p1_ipconfig.h"
#ifdef NON_SDK
#include <stdint.h>
#endif

/*! @file
 *  @brief Enum types used by Serdes API functions
 */

/*! @addtogroup APITag
 * @{
 */

/*! @defgroup SerdesAPIEnumTag Serdes API Enums
 * Enums used throughout Serdes APIs.
 */

/*! @addtogroup SerdesAPIEnumTag
 * @{
 */


/*--------------------------*/
/** Serdes RX PAM Mode Enum */
/*--------------------------*/
enum osprey7_v2l8p1_rx_pam_mode_enum {
    NRZ,
    PAM4_NR,
    PAM4_ER
};

enum osprey7_v2l8p1_cdr_enum {
    OS,
    BR
};

/*! Reference clock frequency enums
 *
 */
enum osprey7_v2l8p1_pll_refclk_enum {
    OSPREY7_V2L8P1_PLL_REFCLK_UNKNOWN = 0, /*!< Refclk value to be determined by API. */
    OSPREY7_V2L8P1_PLL_REFCLK_156P637MHZ     = (int)0x3E8263DD, /*!< 156.637 MHz     */
    OSPREY7_V2L8P1_PLL_REFCLK_157P844MHZ     = (int)0x0FA09A25, /*!< 157.844 MHz     */
    OSPREY7_V2L8P1_PLL_REFCLK_158P51MHZ      = (int)0x06403DEB, /*!< 158.51 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_168P04MHZ      = (int)0x01901069, /*!< 168.04 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_172P64MHZ      = (int)0x019010DC, /*!< 172.64 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_173P37MHZ      = (int)0x064043B9, /*!< 173.37 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_100MHZ         = (int)0x00100064, /*!< 100 MHz         */
    OSPREY7_V2L8P1_PLL_REFCLK_106P25MHZ      = (int)0x004001A9, /*!< 106.25 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_122P88MHZ      = (int)0x01900C00, /*!< 122.88 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_125MHZ         = (int)0x0010007D, /*!< 125 MHz         */
    OSPREY7_V2L8P1_PLL_REFCLK_155P52MHZ      = (int)0x01900F30, /*!< 155.52 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_156P25MHZ      = (int)0x00400271, /*!< 156.25 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_159P375MHZ     = (int)0x008004FB, /*!< 159.375 MHz     */
    OSPREY7_V2L8P1_PLL_REFCLK_161P1328125MHZ = (int)0x08005091, /*!< 161.1328125 MHz */
    OSPREY7_V2L8P1_PLL_REFCLK_166P67MHZ      = (int)0x0640411B, /*!< 166.67 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_166P015625MHZ  = (int)0x64040D99, /*!< 166.015625 MHz */
    OSPREY7_V2L8P1_PLL_REFCLK_167P38MHZ      = (int)0x06404162, /*!< 167.38 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_174P703125MHZ  = (int)0x04002BAD, /*!< 174.703125 MHz  */
    OSPREY7_V2L8P1_PLL_REFCLK_212P5MHZ       = (int)0x002001A9, /*!< 212.5 MHz       */
    OSPREY7_V2L8P1_PLL_REFCLK_311P04MHZ      = (int)0x01901E60, /*!< 311.04 MHz      */
    OSPREY7_V2L8P1_PLL_REFCLK_625MHZ         = (int)0x00100271, /*!< 625 MHz         */
    OSPREY7_V2L8P1_PLL_REFCLK_312P5MHZ       = (int)0x00200271  /*!< 312.5 MHz       */
    };

/** @brief Enumeration DIV values used by PLL configuration APIs.
 */
enum osprey7_v2l8p1_pll_div_enum {
    OSPREY7_V2L8P1_PLL_DIV_UNKNOWN = 0, /*!< Divide value to be determined by API. */
    OSPREY7_V2L8P1_PLL_DIV_179P2      = (int)0x333340B3, /*!< Divide by 179.2      */
    OSPREY7_V2L8P1_PLL_DIV_256        = (int)0x00000100, /*!< Divide by 256        */
    OSPREY7_V2L8P1_PLL_DIV_288        = (int)0x00000120, /*!< Divide by 288        */
    OSPREY7_V2L8P1_PLL_DIV_294P4      = (int)0x66668126, /*!< Divide by 294.4      */
    OSPREY7_V2L8P1_PLL_DIV_316P8      = (int)0xCCCCC13C, /*!< Divide by 316.8      */
    OSPREY7_V2L8P1_PLL_DIV_320        = (int)0x00000140, /*!< Divide by 320        */
    OSPREY7_V2L8P1_PLL_DIV_336        = (int)0x00000150, /*!< Divide by 336        */
    OSPREY7_V2L8P1_PLL_DIV_340        = (int)0x00000154, /*!< Divide by 340        */
    OSPREY7_V2L8P1_PLL_DIV_358        = (int)0x00000166, /*!< Divide by 358        */
    OSPREY7_V2L8P1_PLL_DIV_358P4      = (int)0x66668166, /*!< Divide by 358.4      */
    OSPREY7_V2L8P1_PLL_DIV_360        = (int)0x00000168, /*!< Divide by 360        */
    OSPREY7_V2L8P1_PLL_DIV_368        = (int)0x00000170, /*!< Divide by 368        */
    OSPREY7_V2L8P1_PLL_DIV_396        = (int)0x0000018C, /*!< Divide by 396        */
    OSPREY7_V2L8P1_PLL_DIV_400        = (int)0x00000190, /*!< Divide by 400        */
    OSPREY7_V2L8P1_PLL_DIV_412P5      = (int)0x8000019C, /*!< Divide by 412.5      */
    OSPREY7_V2L8P1_PLL_DIV_448        = (int)0x000001C0, /*!< Divide by 448        */
    OSPREY7_V2L8P1_PLL_DIV_480        = (int)0x000001E0, /*!< Divide by 480        */
    OSPREY7_V2L8P1_PLL_DIV_528        = (int)0x00000210, /*!< Divide by 528        */
    OSPREY7_V2L8P1_PLL_DIV_560        = (int)0x00000230, /*!< Divide by 560        */
    OSPREY7_V2L8P1_PLL_DIV_132        = (int)0x00000084, /*!< Divide by 132        */
    OSPREY7_V2L8P1_PLL_DIV_147P2      = (int)0x33330093, /*!< Divide by 147.2      */
    OSPREY7_V2L8P1_PLL_DIV_158P4      = (int)0x6666809E, /*!< Divide by 158.4      */
    OSPREY7_V2L8P1_PLL_DIV_160        = (int)0x000000A0, /*!< Divide by 160        */
    OSPREY7_V2L8P1_PLL_DIV_165        = (int)0x000000A5, /*!< Divide by 165        */
    OSPREY7_V2L8P1_PLL_DIV_168        = (int)0x000000A8, /*!< Divide by 168        */
    OSPREY7_V2L8P1_PLL_DIV_170        = (int)0x000000AA, /*!< Divide by 170        */
    OSPREY7_V2L8P1_PLL_DIV_175        = (int)0x000000AF, /*!< Divide by 175        */
    OSPREY7_V2L8P1_PLL_DIV_180        = (int)0x000000B4, /*!< Divide by 180        */
    OSPREY7_V2L8P1_PLL_DIV_264        = (int)0x00000108, /*!< Divide by 264        */
    OSPREY7_V2L8P1_PLL_DIV_280        = (int)0x00000118, /*!< Divide by 280        */
    OSPREY7_V2L8P1_PLL_DIV_330        = (int)0x0000014A, /*!< Divide by 330        */
    OSPREY7_V2L8P1_PLL_DIV_350        = (int)0x0000015E  /*!< Divide by 350        */
};

/** TX AFE Settings Enum */
enum osprey7_v2l8p1_tx_afe_settings_enum {
    OSPREY7_V2L8P1_TX_AFE_TAP0,
    OSPREY7_V2L8P1_TX_AFE_TAP1,
    OSPREY7_V2L8P1_TX_AFE_TAP2,
    OSPREY7_V2L8P1_TX_AFE_TAP3,
    OSPREY7_V2L8P1_TX_AFE_TAP4,
    OSPREY7_V2L8P1_TX_AFE_TAP5
};

/** TXFIR Tap Enable Enum */
enum osprey7_v2l8p1_txfir_tap_enable_enum {
    OSPREY7_V2L8P1_NRZ_LP_3TAP  = 0,
    OSPREY7_V2L8P1_NRZ_6TAP     = 1,
    OSPREY7_V2L8P1_PAM4_LP_3TAP = 2,
    OSPREY7_V2L8P1_PAM4_6TAP    = 3
};

/*! @def OSPREY7_V2L8P1_PLL_OPTION_REFCLK_MASK
 * Mask used for REFCLK options within enum osprey7_v2l8p1_pll_option_enum.
 * All REFCLK options should fall within this mask's range.
 */
#define OSPREY7_V2L8P1_PLL_OPTION_REFCLK_MASK (0xF)

/** PLL Configuration Options Enum used for configure_pll APIs
 *  If using REFCLK options, only one REFCLK option may be used at a time.
 */
enum osprey7_v2l8p1_pll_option_enum {
    OSPREY7_V2L8P1_PLL_OPTION_NONE              =  0,
    OSPREY7_V2L8P1_PLL_OPTION_REFCLK_DOUBLER_EN =  1,
    OSPREY7_V2L8P1_PLL_OPTION_REFCLK_DIV2_EN    =  2,
    OSPREY7_V2L8P1_PLL_OPTION_REFCLK_DIV4_EN    =  3,
    OSPREY7_V2L8P1_PLL_OPTION_POWERDOWN         =  1<<4,
    OSPREY7_V2L8P1_PLL_OPTION_DISABLE_VERIFY    =  1<<5
};


/** Force / Get Force CDR Mode Enum */
enum osprey7_v2l8p1_force_cdr_mode_enum {
    OSPREY7_V2L8P1_OSCDR_FORCE_ENABLE,     /*!< Force OS-CDR mode */
    OSPREY7_V2L8P1_OSCDR_FORCE_DISABLE,    /*!< Disable Force OS-CDR mode */
    OSPREY7_V2L8P1_BRCDR_FORCE_ENABLE,     /*!< Force BR-CDR mode */
    OSPREY7_V2L8P1_BRCDR_FORCE_DISABLE,    /*!< Disable Force BR-CDR mode */
    OSPREY7_V2L8P1_CDR_FORCE_DISABLE       /*!< Disable Force CDR modes */
};

/*! @} SerdesAPIEnumTag */
/*! @} APITag */
#endif /*#define OSPREY7_V2L8P1_API_ENUM_H*/
#ifdef __cplusplus
}
#endif
