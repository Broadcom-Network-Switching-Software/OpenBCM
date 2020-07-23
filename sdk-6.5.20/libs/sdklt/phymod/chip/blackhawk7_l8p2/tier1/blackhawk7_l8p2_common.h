/***********************************************************************************
 *                                                                                 *
 * Copyright: (c) 2019 Broadcom.                                                   *
 * Broadcom Proprietary and Confidential. All rights reserved.                     *
 */

/********************************************************************************
 ********************************************************************************
 *                                                                              *
 *  Revision      :   *
 *                                                                              *
 *  Description   :  Defines and Enumerations required by Serdes APIs           *
 */

/** @file blackhawk7_l8p2_common.h
 * Defines and Enumerations shared across MER7/BHK7/OSP7 APIs but NOT uCode
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLACKHAWK7_L8P2_API_COMMON_H
#define BLACKHAWK7_L8P2_API_COMMON_H

#include "blackhawk7_l8p2_ipconfig.h"

/** Macro to determine sign of a value */
#define sign(x) ((x>=0) ? 1 : -1)

#define UCODE_MAX_SIZE  (120*1024)    /* 120K CODE RAM */

/*
 * IP-Specific Iteration Bounds
 */
# define NUM_PLLS 2

#endif
#ifdef __cplusplus
}
#endif
