/********************************************************************************
 ********************************************************************************
 *                                                                              *
 *  Revision      :  $Id: merlin_quadra28_common.h 924 2015-02-24 18:08:11Z eroes $ *
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

/** @file merlin_quadra28_common.h
 * Defines and Enumerations shared across Eagle/Merlin/Falcon APIs BUT NOT MICROCODE
 */

#ifndef MERLIN_QUADRA28_API_COMMON_H
#define MERLIN_QUADRA28_API_COMMON_H
#include "common/srds_api_uc_common.h"

/** Macro to determine sign of a value */
#define sign(x) ((x>=0) ? 1 : -1)

#define UCODE_MAX_SIZE  32768

/*
 * Register Address Defines used by the API that are different between IPs
 */
#define DSC_A_DSC_UC_CTRL 0xd00d
#define TLB_RX_PRBS_CHK_ERR_CNT_MSB_STATUS 0xD0DA

/*
 * Register Address Defines used by the API that are COMMON across IPs
 */
#define MDIO_MMDSEL_AER_COM_MDIO_MASKDATA  0xFFDB

/*
 * IP-Specific Iteration Bounds
 */

#endif
