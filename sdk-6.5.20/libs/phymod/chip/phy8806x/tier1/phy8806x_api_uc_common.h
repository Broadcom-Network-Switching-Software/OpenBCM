/**********************************************************************************
**********************************************************************************
*                                                                                *
*  Revision      :  $Id: falcon_api_uc_common.h 925 2015-02-24 23:23:58Z kirand $  *
*                                                                                *
*  Description   :  Defines and Enumerations required by Falcon ucode            *
*                                                                                *
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                          *
*  No portions of this material may be reproduced in any form without            *
*  the written permission of:                                                    *
*      Broadcom Corporation                                                      *
*      5300 California Avenue                                                    *
*      Irvine, CA  92617                                                         *
*                                                                                *
*  All information contained in this document is Broadcom Corporation            *
*  company private proprietary, and trade secret.                                *
 */

/** @file phy8806x_api_uc_common.h
 * Defines and Enumerations shared by Falcon IP Specific API and Microcode
 */

#ifndef PHY8806X_API_UC_COMMON_H
#define PHY8806X_API_UC_COMMON_H

/* Add phy8806x specific items below this */ 

/** Translate between a VCO frequency in MHz and the vco_rate that is **\
*** found in the Core Config Variable Structure using the formula:    ***
***                                                                   ***
***     vco_rate = (frequency_in_ghz * 16.0) - 224.0                  ***
***                                                                   ***
*** Both functions round to the nearest resulting value.  This        ***
*** provides the highest accuracy possible, and ensures that:         ***
***                                                                   ***
***     vco_rate == MHZ_TO_VCO_RATE(VCO_RATE_TO_MHZ(vco_rate))        ***
***                                                                   ***
*** In the microcode, this should only be called with a numeric       ***
*** literal parameter.                                                ***
\**                                                                   **/
#define MHZ_TO_VCO_RATE(mhz) ((uint8_t)(((((uint16_t)(mhz) * 2) + 62) / 125) - 224))
#define VCO_RATE_TO_MHZ(vco_rate) (((((uint16_t)(vco_rate) + 224) * 125) + 1) >> 1)

/* Please note that when adding entries here you should update the #defines in the phy8806x_tsc_common.h */

/** OSR_MODES Enum */
enum phy8806x_tsc_osr_mode_enum {
	PHY8806X_TSC_OSX1    = 0,
	PHY8806X_TSC_OSX2    = 1,
	PHY8806X_TSC_OSX4    = 2,
	PHY8806X_TSC_OSX8    = 5,
	PHY8806X_TSC_OSX16P5 = 8,
	PHY8806X_TSC_OSX16   = 9,
	PHY8806X_TSC_OSX20P625 = 12,
	PHY8806X_TSC_OSX32     = 13
};

/** CDR mode Enum **/
enum phy8806x_tsc_cdr_mode_enum {
	PHY8806X_TSC_CDR_MODE_OS_ALL_EDGES         = 0,
	PHY8806X_TSC_CDR_MODE_OS_PATTERN           = 1,
	PHY8806X_TSC_CDR_MODE_OS_PATTERN_ENHANCED  = 2,  
	PHY8806X_TSC_CDR_MODE_BR_PATTERN           = 3
};

/** Lane User Control Clause93/72 Force Value **/
enum phy8806x_tsc_cl93n72_frc_val_enum {
	PHY8806X_TSC_CL93N72_FORCE_OS  = 0,
	PHY8806X_TSC_CL93N72_FORCE_BR  = 1  
};


#endif
