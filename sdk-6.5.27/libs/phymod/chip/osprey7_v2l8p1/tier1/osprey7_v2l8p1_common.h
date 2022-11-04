/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2021 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 *                                                                                 *
 ***********************************************************************************/

/********************************************************************************
 ********************************************************************************
 *                                                                              *
 *  Revision      :   *
 *                                                                              *
 *  Description   :  Defines and Enumerations required by Serdes APIs           *
 *                                                                              *
 ********************************************************************************
 ********************************************************************************/

/** @file osprey7_v2l8p1_common.h
 * Defines and Enumerations shared across MER7/BHK7/OSP7/PER5 APIs but NOT uCode
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef OSPREY7_V2L8P1_API_COMMON_H
#define OSPREY7_V2L8P1_API_COMMON_H

#include "osprey7_v2l8p1_ipconfig.h"
#include "osprey7_v2l8p1_usr_includes.h"

/** Macro to determine sign of a value */
#define sign(x) ((x>=0) ? 1 : -1)

#define SRDS_MAX_BER_STR_LEN        (32)
#define SRDS_MAX_OSR_STR_LEN        (16)

#define SRDS_MAX_UC_STS_STR_LEN     (512)
#define SRDS_MAX_UC_STS_EXT_STR_LEN (512)

/*
 * IP-Specific Iteration Bounds
 */
# define NUM_PLLS 1

#define TXFIR_ST_NUM_TAPS (6)

/*! TX FIR tap values
 *
 */
typedef struct osprey7_v2l8p1_txfir {
  int16_t tap[TXFIR_ST_NUM_TAPS];
} osprey7_v2l8p1_txfir_st;

/*------------------------------*/
/** Serdes OSR Mode Structure   */
/*------------------------------*/
typedef struct {
  /** TX OSR Mode */
  uint8_t tx;
  /** RX OSR Mode */
  uint8_t rx;
  /** OSR Mode for TX and RX (used when both TX and RX should have same OSR Mode) */
  uint8_t tx_rx;
} osprey7_v2l8p1_osr_mode_st;

#define srds_Y_width (6)
#define srds_X_width (5)
/*! Debug feedback status
 *
 */
typedef struct osprey7_v2l8p1_dbgfb_stats_s {
        int32_t  data1[srds_X_width][srds_Y_width];
        int32_t  data2[srds_X_width][srds_Y_width];
        int32_t  data3[srds_X_width][srds_Y_width];
        int32_t  data4[srds_X_width][srds_Y_width];
} osprey7_v2l8p1_dbgfb_data_st;

/*! Debug feedback data
 *
 */
typedef struct osprey7_v2l8p1_dbgfb_data_s {
    uint32_t time_in_us;
    osprey7_v2l8p1_dbgfb_data_st data;
    uint8_t pam4es_en;
    uint8_t temp[3];
} osprey7_v2l8p1_dbgfb_stats_st;



#endif
#ifdef __cplusplus
}
#endif
