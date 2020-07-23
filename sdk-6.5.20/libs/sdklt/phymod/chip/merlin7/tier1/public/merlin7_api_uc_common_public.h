/*************************************************************************************
 *                                                                                   *
 * Copyright: (c) 2018 Broadcom.                                                     *
 * Broadcom Confidential. All rights reserved.                                       *
 */

/***************************************************************************************
 ***************************************************************************************
 *                                                                                     *
 *  Revision      :   *
 *                                                                                     *
 *  Description   :  Defines and Enumerations required by Merlin7 APIs                 *
 *                                                                                     *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                               *
 *  No portions of this material may be reproduced in any form without                 *
 *  the written permission of:                                                         *
 *      Broadcom Corporation                                                           *
 *      5300 California Avenue                                                         *
 *      Irvine, CA  92617                                                              *
 *                                                                                     *
 *  All information contained in this document is Broadcom Corporation                 *
 *  company private proprietary, and trade secret.                                     *
 */

/** @file merlin7_api_uc_common.h
 * Defines and Enumerations shared by Merlin7 IP Specific API and Microcode
 */

#ifndef MERLIN7_API_UC_COMMON_PUBLIC_H
#define MERLIN7_API_UC_COMMON_PUBLIC_H

/* Add Merlin7 specific items below this */ 

/* Please note that when adding entries here you should update the #defines in the merlin7_tsc_common.h */

/** OSR_MODES Enum */
enum merlin7_tsc_osr_mode_enum {
  MERLIN7_TSC_OSX1    = 0,
  MERLIN7_TSC_OSX2    = 1,
  MERLIN7_TSC_OSX3    = 2,
  MERLIN7_TSC_OSX3P3  = 3,
  MERLIN7_TSC_OSX4    = 4,
  MERLIN7_TSC_OSX5    = 5,
  MERLIN7_TSC_OSX7P5  = 6,
  MERLIN7_TSC_OSX8    = 7,
  MERLIN7_TSC_OSX8P25 = 8,
  MERLIN7_TSC_OSX10   = 9
};

#endif