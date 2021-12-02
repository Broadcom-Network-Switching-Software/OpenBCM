/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2020 Broadcom.                                                   *
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

/** @file blackhawk7_l2p2_common.h
 * Defines and Enumerations shared across MER7/BHK7/OSP7/PER5 APIs but NOT uCode
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLACKHAWK7_L2P2_API_COMMON_H
#define BLACKHAWK7_L2P2_API_COMMON_H

#include "blackhawk7_l2p2_ipconfig.h"
#include "blackhawk7_l2p2_usr_includes.h"

/** Macro to determine sign of a value */
#define sign(x) ((x>=0) ? 1 : -1)

#define UCODE_MAX_SIZE  (120*1024)    /* 120K CODE RAM */

/*
 * IP-Specific Iteration Bounds
 */
# define NUM_PLLS 2

#define TXFIR_ST_NUM_TAPS (6)

/*! TX FIR tap values
 *
 */
typedef struct blackhawk7_l2p2_txfir {
  int16_t tap[TXFIR_ST_NUM_TAPS];
} blackhawk7_l2p2_txfir_st;


#define srds_Y_width (6)
#define srds_X_width (3)
/*! Debug feedback status
 *
 */
typedef struct blackhawk7_l2p2_dbgfb_stats_s {
        int32_t  data1[srds_X_width][srds_Y_width];
        int32_t  data2[srds_X_width][srds_Y_width];
        int32_t  data3[srds_X_width][srds_Y_width];
        int32_t  data4[srds_X_width][srds_Y_width];
} blackhawk7_l2p2_dbgfb_data_st;

/*! Debug feedback data
 *
 */
typedef struct blackhawk7_l2p2_dbgfb_data_s {
    uint32_t time_in_us;
    blackhawk7_l2p2_dbgfb_data_st data;
    uint8_t pam4es_en;
    uint8_t temp[3];
} blackhawk7_l2p2_dbgfb_stats_st;



#endif
#ifdef __cplusplus
}
#endif
