/********************************************************************************
********************************************************************************
*                                                                              *
*  Revision      :  $Id: phy8806x_tsc_common.h 1266 2015-11-20 13:18:43Z kirand $ *
*                                                                              *
*  Description   :  Defines and Enumerations required by Serdes APIs           *
*                                                                              *
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                        *
*  No portions of this material may be reproduced in any form without          *
*  the written permission of:                                                  *
*      Broadcom Corporation                                                    *
*      5300 California Avenue                                                  *
*      Irvine, CA  92617                                                       *
*                                                                              *
*  All information contained in this document is Broadcom Corporation          *
*  company private proprietary, and trade secret.                              *
 */

/** @file phy8806x_tsc_common.h
 * Defines and Enumerations shared across Eagle/Merlin/Falcon APIs BUT NOT MICROCODE
 */

#ifndef PHY8806X_TSC_API_COMMON_H
#define PHY8806X_TSC_API_COMMON_H
#include "phy8806x_tsc_common_uc_common.h"

/** Macro to determine sign of a value */
#define sign(x) ((x >= 0) ? 1 : -1)

#define UCODE_MAX_SIZE  40960

/*
 * Register Address Defines used by the API that are different between IPs
 */
#define DSC_A_DSC_UC_CTRL      0xD03d
#define DSC_E_RX_PI_CNT_BIN_D  0xD075
#define DSC_E_RX_PI_CNT_BIN_P  0xD076
#define DSC_E_RX_PI_CNT_BIN_L  0xD077
#define DSC_E_RX_PI_CNT_BIN_PD 0xD070
#define DSC_E_RX_PI_CNT_BIN_LD 0xD071
#define TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS 0xD16A
#define TLB_RX_PRBS_CHK_ERR_CNT_LSB_STATUS 0xD16B

/* PLL Lock and change Status Register define */
#define PLL_STATUS_ADDR 0xD148

/* PMD Lock and change Status Register define */
#define PMD_LOCK_STATUS_ADDR 0xD16C

/* Sigdet and change Status Register define */
#define SIGDET_STATUS_ADDR 0xD0E8

#define DIG_COM_MASKDATA_REG  0xD103

/* PCIe Gen3 Register Address defines*/

/*
 * Register Address Defines used by the API that are COMMON across IPs
 */


/*
 * IP-Specific Iteration Bounds
 */
#define  SERDES_NUM_PLLS  2
#define  SERDES_NUM_LANES 4

#endif
