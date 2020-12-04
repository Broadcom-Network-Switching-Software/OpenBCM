/*****************************************************************************************
*****************************************************************************************
*                                                                                       *
*  Revision      :  $Id: eagle_merlin_api_uc_common.h 1138 2015-09-17 20:58:34Z kirand $ *
*                                                                                       *
*  Description   :  Defines and Enumerations required by Eagle/Merlin APIs              *
*                                                                                       *
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                                 *
*  No portions of this material may be reproduced in any form without                   *
*  the written permission of:                                                           *
*      Broadcom Corporation                                                             *
*      5300 California Avenue                                                           *
*      Irvine, CA  92617                                                                *
*                                                                                       *
*  All information contained in this document is Broadcom Corporation                   *
*  company private proprietary, and trade secret.                                       *
 */

/** @file eagle_merlin_api_uc_common.h
 * Defines and Enumerations shared by Eagle & Merlin IP Specific API and Microcode
 */

#ifndef EAGLE_MERLIN_API_UC_COMMON_H
#define EAGLE_MERLIN_API_UC_COMMON_H

/* Add Eagle/Merlin specific items below this */

/** Translate between a VCO frequency in MHz and the vco_rate that is **\
*** found in the Core Config Variable Structure using the formula:    ***
***                                                                   ***
***     vco_rate = (frequency_in_ghz * 4.0) - 22.0                    ***
***                                                                   ***
*** Both functions round to the nearest resulting value.  This        ***
*** provides the highest accuracy possible, and ensures that:         ***
***                                                                   ***
***     vco_rate == MHZ_TO_VCO_RATE(VCO_RATE_TO_MHZ(vco_rate))        ***
***                                                                   ***
*** In the microcode, this should only be called with a numeric       ***
*** literal parameter.                                                ***
\**                                                                   **/
#define MHZ_TO_VCO_RATE(mhz) ((uint8_t)((((uint16_t)(mhz)) / 250) - 22))
#define VCO_RATE_TO_MHZ(vco_rate) ((((uint16_t)(vco_rate) + 22) * 1000) >> 2)

/* Please note that when adding entries here you should update the #defines in the eagle_tsc_common.h */

/** OSR_MODES Enum */
enum eagle_tsc_osr_mode_enum {
	EAGLE_TSC_OSX1    = 0,
	EAGLE_TSC_OSX2    = 1,
	EAGLE_TSC_OSX3    = 2,
	EAGLE_TSC_OSX3P3  = 3,
	EAGLE_TSC_OSX4    = 4,
	EAGLE_TSC_OSX5    = 5,
	EAGLE_TSC_OSX7P5  = 6,
	EAGLE_TSC_OSX8    = 7,
	EAGLE_TSC_OSX8P25 = 8,
	EAGLE_TSC_OSX10   = 9
};


/** VCO_RATE Enum */
enum eagle_tsc_vco_rate_enum {
	EAGLE_TSC_VCO_5P5G = 0,
	EAGLE_TSC_VCO_5P75G,
	EAGLE_TSC_VCO_6G,
	EAGLE_TSC_VCO_6P25G,
	EAGLE_TSC_VCO_6P5G,
	EAGLE_TSC_VCO_6P75G,
	EAGLE_TSC_VCO_7G,
	EAGLE_TSC_VCO_7P25G,
	EAGLE_TSC_VCO_7P5G,
	EAGLE_TSC_VCO_7P75G,
	EAGLE_TSC_VCO_8G,
	EAGLE_TSC_VCO_8P25G,
	EAGLE_TSC_VCO_8P5G,
	EAGLE_TSC_VCO_8P75G,
	EAGLE_TSC_VCO_9G,
	EAGLE_TSC_VCO_9P25G,
	EAGLE_TSC_VCO_9P5G,
	EAGLE_TSC_VCO_9P75G,
	EAGLE_TSC_VCO_10G,
	EAGLE_TSC_VCO_10P25G,
	EAGLE_TSC_VCO_10P5G,
	EAGLE_TSC_VCO_10P75G,
	EAGLE_TSC_VCO_11G,
	EAGLE_TSC_VCO_11P25G,
	EAGLE_TSC_VCO_11P5G,
	EAGLE_TSC_VCO_11P75G,
	EAGLE_TSC_VCO_12G,
	EAGLE_TSC_VCO_12P25G,
	EAGLE_TSC_VCO_12P5G,
	EAGLE_TSC_VCO_12P75G,
	EAGLE_TSC_VCO_13G,
	EAGLE_TSC_VCO_13P25G
};

#endif
