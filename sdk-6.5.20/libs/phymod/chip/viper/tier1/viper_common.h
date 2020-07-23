/********************************************************************************/
/********************************************************************************/
/*                                                                              */
/*  Revision      :  viper_common.h 382 2014-03-14 04:56:06Z mageshv $          */
/*                                                                              */
/*  Description   :  Defines and Enumerations required by VIPER APIs     */
/*                                                                              */
/*  Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.               */
/*  All Rights Reserved                                                         */
/*  No portions of this material may be reproduced in any form without          */
/*  the written permission of:                                                  */
/*      Broadcom Corporation                                                    */
/*      5300 California Avenue                                                  */
/*      Irvine, CA  92617                                                       */
/*                                                                              */
/*  All information contained in this document is Broadcom Corporation          */
/*  company private proprietary, and trade secret.                              */
/*                                                                              */
/********************************************************************************/
/********************************************************************************/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/

/** @file viper_common.h
 * Defines and Enumerations required by VIPER APIs
 */

#ifndef VIPER_COMMON_H
#define VIPER_COMMON_H

#ifndef _DV_TB_
#define PHYMOD_ST  phymod_access_t
#else
#define PHYMOD_ST  temod_st
#endif


#ifdef __C51__
/** Returns Absolute value of x */
#define abs(x) ( ((x)>0) ? (x) : (-(x)) )
#endif

/** Returns Sign of x */
#define sign(x) ((x>=0) ? 1 : -1)

/* Function return values */
#define SUCCESS              (0)
#define FAIL                 (1)
#define PENDING              (2)

/* Register Address Defines */
/** Address define for PRBS_CHK_ERR_CNT_MSB_STATUS Register */
#define TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS 0xD0DA


/** Address define for MDIO_MASKDATA Register */ 
#define MDIO_MMDSEL_AER_COM_MDIO_MASKDATA  0xFFDB
#define DSC_A_DSC_UC_CTRL 0xd00d


/*-----------------------*/
/*  Serdes Enumerations  */
/*-----------------------*/

/** DSC_STATES Enum */
enum viper_dsc_state_enum {
  DSC_STATE_RESET        = 0,  
  DSC_STATE_RESTART      = 1,
  DSC_STATE_CONFIG       = 2,
  DSC_STATE_WAIT_FOR_SIG = 3,
  DSC_STATE_ACQ_CDR      = 4,
  DSC_STATE_CDR_SETTLE   = 5,
  DSC_STATE_HW_TUNE      = 6,
  DSC_STATE_UC_TUNE      = 7,
  DSC_STATE_MEASURE      = 8,
  DSC_STATE_DONE         = 9
};

/** SERDES_PMD_UC_COMMANDS Enum */
enum viper_pmd_uc_cmd_enum {
  CMD_NULL                =  0,
  CMD_UC_CTRL             =  1,
  CMD_HEYE_OFFSET         =  2,
  CMD_VEYE_OFFSET         =  3,
  CMD_READ_DIE_TEMP       =  4,
  CMD_DIAG_EN             =  5,
  CMD_READ_UC_LANE_BYTE   =  6,
  CMD_WRITE_UC_LANE_BYTE  =  7,
  CMD_READ_UC_CORE_BYTE   =  8,
  CMD_WRITE_UC_CORE_BYTE  =  9,
  CMD_READ_UC_LANE_WORD   = 10,
  CMD_WRITE_UC_LANE_WORD  = 11,
  CMD_READ_UC_CORE_WORD   = 12,
  CMD_WRITE_UC_CORE_WORD  = 13,
  CMD_EVENT_LOG_CTRL      = 14,
  CMD_EVENT_LOG_READ      = 15,
  CMD_CAPTURE_BER_START   = 16,
  CMD_READ_DIAG_DATA_BYTE = 17,
  CMD_READ_DIAG_DATA_WORD = 18,
  CMD_CAPTURE_BER_END     = 19
};

/** SERDES_UC_CTRL_COMMANDS Enum */
enum viper_pmd_uc_ctrl_cmd_enum {
  CMD_UC_CTRL_STOP_GRACEFULLY   = 0,
  CMD_UC_CTRL_STOP_IMMEDIATE    = 1,
  CMD_UC_CTRL_RESUME            = 2,
  CMD_UC_CTRL_STOP_HOLD         = 3,
  CMD_UC_CTRL_RELEASE_HOLD_ONCE = 4
};

/** SERDES_EVENT_LOG_READ Enum */
enum viper_pmd_event_rd_cmd_enum {
  CMD_EVENT_LOG_READ_START      = 0,
  CMD_EVENT_LOG_READ_NEXT       = 1,
  CMD_EVENT_LOG_READ_DONE       = 2
};

/** SERDES_UC_DIAG_COMMANDS Enum */
enum viper_pmd_uc_diag_cmd_enum {
  CMD_UC_DIAG_DISABLE         = 3,
  CMD_UC_DIAG_PASSIVE         = 1,
  CMD_UC_DIAG_DENSITY         = 2,
  CMD_UC_DIAG_START_VSCAN_EYE = 4,
  CMD_UC_DIAG_START_HSCAN_EYE = 5,
  CMD_UC_DIAG_GET_EYE_SAMPLE  = 6
};

/** DIAG_BER mode settings **/
enum viper_diag_ber_mode_enum {
	DIAG_BER_POS  = 0,
	DIAG_BER_NEG  = 1,
	DIAG_BER_VERT = 0,
	DIAG_BER_HORZ = 1<<1,
	DIAG_BER_PASS = 0,
	DIAG_BER_INTR = 1<<2,
	DIAG_BER_P1_NARROW = 1<<3,
	DIAG_BER_P1_WIDE = 0,
    DIAG_BER_FAST = 1<<6

};

enum viper_eye_scan_dir_enum {
      EYE_SCAN_HORIZ = 0,
      EYE_SCAN_VERTICAL = 1
};

typedef uint8_t float8_t;

/** OSR_MODES Enum */
enum viper_osr_mode_enum {
  OSX1    = 0,
  OSX2    = 1,
  OSX3    = 2,
  OSX3P3  = 3,
  OSX4    = 4,
  OSX5    = 5,
  OSX7P5  = 6,
  OSX8    = 7,
  OSX8P25 = 8,
  OSX10   = 9
};

/** Media type Enum */
enum viper_media_type_enum {
  MEDIA_TYPE_PCB_TRACE_BACKPLANE = 0,
  MEDIA_TYPE_COPPER_CABLE        = 1,
  MEDIA_TYPE_OPTICS              = 2
};

/** CDR mode Enum **/
enum cdr_mode_enum {
  CDR_MODE_OS_ALL_EDGES = 0,
  CDR_MODE_OS_PATTERN   = 1,
  CDR_MODE_BR_PATTERN   = 2
};

/** VCO_RATE Enum */
enum viper_vco_rate_enum {
  VCO_5P5G = 0,
  VCO_5P75G,
  VCO_6G,
  VCO_6P25G,
  VCO_6P5G,
  VCO_6P75G,
  VCO_7G,
  VCO_7P25G,
  VCO_7P5G,
  VCO_7P75G,
  VCO_8G,
  VCO_8P25G,
  VCO_8P5G,
  VCO_8P75G,
  VCO_9G,
  VCO_9P25G,
  VCO_9P5G,
  VCO_9P75G,
  VCO_10G,
  VCO_10P25G,
  VCO_10P5G,
  VCO_10P75G,
  VCO_11G,
  VCO_11P25G,
  VCO_11P5G,
  VCO_11P75G,
  VCO_12G,
  VCO_12P25G,
  VCO_12P5G,
  VCO_12P75G,
  VCO_13G,
  VCO_13P25G
};


/** Core User Control Event Log Level **/
#define USR_CTRL_CORE_EVENT_LOG_UPTO_FULL (0xFF)

#endif  
