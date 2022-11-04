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

/** @file merlin7_pcieg3_common.h
 * Defines and Enumerations shared across MER7/BHK7/OSP7/PER5 APIs but NOT uCode
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MERLIN7_PCIEG3_API_COMMON_H
#define MERLIN7_PCIEG3_API_COMMON_H

#include "merlin7_pcieg3_ipconfig.h"
#include "merlin7_pcieg3_usr_includes.h"

/** Macro to determine sign of a value */
#define sign(x) ((x>=0) ? 1 : -1)

#define SRDS_MAX_BER_STR_LEN        (32)
#define SRDS_MAX_OSR_STR_LEN        (16)


/*
 * IP-Specific Iteration Bounds
 */
# define NUM_PLLS 1


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
} merlin7_pcieg3_osr_mode_st;

#define srds_Y_width (6)
#define srds_X_width (3)
/*! Debug feedback status
 *
 */
typedef struct merlin7_pcieg3_dbgfb_stats_s {
        int32_t  data1[srds_X_width][srds_Y_width];
        int32_t  data2[srds_X_width][srds_Y_width];
        int32_t  data3[srds_X_width][srds_Y_width];
        int32_t  data4[srds_X_width][srds_Y_width];
} merlin7_pcieg3_dbgfb_data_st;

/*! Debug feedback data
 *
 */
typedef struct merlin7_pcieg3_dbgfb_data_s {
    uint32_t time_in_us;
    merlin7_pcieg3_dbgfb_data_st data;
    uint8_t pam4es_en;
    uint8_t temp[3];
} merlin7_pcieg3_dbgfb_stats_st;



#endif
#ifdef __cplusplus
}
#endif
