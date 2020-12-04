/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/
/* 
 *
 *  Broadcom Confidential/Proprietary.
 *  Copyright (c) 2015 Broadcom Corporation. All rights reserved.
 */

#ifndef MADURA_SDK_INTERFACE_H_
#define MADURA_SDK_INTERFACE_H_

/** @file madura_sdk_interface.h
 *  Declaration of data structure and constants shared by Madura firmware and Madura SDK.
 */

/** @enum E_MADURA_SYS_LINE_SEL
 *  Constants to select Madura system side or line side in some functions.
 */
/** @enum E_MADURA_MEDIA_TYPE
 *  Constants to select Serdes media type.
 */

/** @enum E_MADURA_DFE_OPTION
 *  Constants to select DFE type.
 */

/** @enum E_MADURA_TX_TIMING_MODE
 *  Constants to select Serdes Tx timing source.
 */

/** @typedef union madura_user_port_config_un madura_port_config_t
 *  Data structure to specify Madura user configuration at port level.
 */

enum E_MADURA_SYS_LINE_SEL
{
    MADURA_SYS  = 0,  /**< select Madura system side */
    MADURA_LINE = 1   /**< select Madura line side */
};

enum E_MADURA_MEDIA_TYPE
{
    SERDES_MEDIA_TYPE_BACK_PLANE             = 0,  /**< KR link: back plane media  */
    SERDES_MEDIA_TYPE_COPPER_CABLE           = 1,  /**< CR link: direct-attached copper cable  */
    SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS   = 2,  /**< SR/LR link: optical fiber with reliable LOS detection  */
    SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS = 3   /**< SR/LR link: optical fiber with unreliable LOS detection  */
};

enum E_MADURA_DFE_OPTION
{
    SERDES_DFE_OPTION_NO_DFE      = 0,  /**< no DFE is used  */
    SERDES_DFE_OPTION_DFE         = 1,  /**< DFE is enabled  */
    SERDES_DFE_OPTION_BRDEF       = 2,  /**< Force BRDFE mode, DFE will be on, too.  */
    SERDES_DFE_OPTION_DFE_LP_MODE = 3   /**< DFE is enabled in low-power mode  */
};

enum E_MADURA_TX_TIMING_MODE
{
    MADURA_TX_REPEATER      = 0, /**< Tx repeater mode, chip default  */
    MADURA_TX_REPEATER1     = 1, /**< Alternative value for Tx repeater mode  */
    MADURA_TX_RETIMER       = 2, /**< Tx retimer mode  */
    MADURA_TX_REPEATER_ULL  = 3  /**< Tx repeater mode with ULL data path  */
};

#ifdef  MADURA_UNTESTED_CODE
typedef union madura_port_user_config_un
{
    union {
        uint16_t word;
        struct {
            uint8_t sys_media_type       : 2; /**< [1:0] use the symbols in E_MADURA_MEDIA_TYPE  */
            uint8_t sys_DFE_option       : 2; /**< [3:2] use the symbols in E_MADURA_DFE_OPTION_TYPE  */
            uint8_t sys_Tx_training_en   : 1; /**< [4]   0/1: disable/enable forced Tx training at system side  */
            uint8_t sys_Tx_timing_mode   : 2; /**< [6:5] use the symbols in E_MADURA_TX_TIMING_MODE for LRx->STx direction  */
            uint8_t loopback_change      : 1; /**< [7]   0/1: loopback config at either side no change/changed.  */

            uint8_t line_media_type      : 2; /**< [9:8] use the symbols in E_MADURA_MEDIA_TYPE  */
            uint8_t line_DFE_option      : 2; /**< [11:10] use the symbols in E_MADURA_DFE_OPTION_TYPE  */
            uint8_t line_Tx_training_en  : 1; /**< [12]  0/1: disable/enable forced Tx training at line side  */
            uint8_t line_Tx_timing_mode  : 2; /**< [14:13] use the symbols in E_MADURA_TX_TIMING_MODE for SRx->LTx direction  */
            uint8_t AN_enable            : 1; /**< [15]  0/1: disable/enable auto-negotiation at line side  */
        } fields;
    } both_sides;

    union {
        uint8_t byte;
        struct {
            uint8_t media_type           : 2; /**< [1:0] use the symbols in E_MADURA_MEDIA_TYPE  */
            uint8_t DFE_option           : 2; /**< [3:2] use the symbols in E_MADURA_DFE_OPTION_TYPE  */
            uint8_t Tx_training_en       : 1; /**< [4]   0/1: disable/enable forced Tx training at system side  */
            uint8_t Tx_timing_mode       : 2; /**< [6:5] use the symbols in E_MADURA_TX_TIMING_MODE  */
            uint8_t chip_feature_en      : 1; /**< [7]   0/1: disable/enable chip level features such as loopback and auto-negotiation  */
        } fields;
    } at_side[2];
} madura_port_user_config_t;
#endif /* MADURA_UNTESTED_CODE */

#endif /* MADURA_SDK_INTERFACE_H_ */

