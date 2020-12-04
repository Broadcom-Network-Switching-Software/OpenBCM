/******************************************************************************
******************************************************************************
*  Revision      :  $Id: eagle_tsc_enum.h 1140 2015-09-18 21:05:29Z kirand $ *
*                                                                            *
*  Description   :  Enum types used by Serdes API functions                  *
*                                                                            *
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                      *
*  No portions of this material may be reproduced in any form without        *
*  the written permission of:                                                *
*      Broadcom Corporation                                                  *
*      5300 California Avenue                                                *
*      Irvine, CA  92617                                                     *
*                                                                            *
*  All information contained in this document is Broadcom Corporation        *
*  company private proprietary, and trade secret.                            *
 */

/** @file eagle_tsc_enum.h
 * Enum types used by Serdes API functions
 */

#ifndef EAGLE_TSC_API_ENUM_H
#define EAGLE_TSC_API_ENUM_H

#include "common/srds_api_enum.h"



/** Eagle PLL Config Enum */
enum eagle_tsc_pll_enum {
	EAGLE_TSC_pll_div_40x,
	EAGLE_TSC_pll_div_42x,
	EAGLE_TSC_pll_div_46x,
	EAGLE_TSC_pll_div_50x,
	EAGLE_TSC_pll_div_52x,
	EAGLE_TSC_pll_div_60x,
	EAGLE_TSC_pll_div_64x_refc161,
	EAGLE_TSC_pll_div_64x_refc156,
	EAGLE_TSC_pll_div_64x,
	EAGLE_TSC_pll_div_66x,
	EAGLE_TSC_pll_div_68x,
	EAGLE_TSC_pll_div_70x,
	EAGLE_TSC_pll_div_72x,
	EAGLE_TSC_pll_div_73p6x,
	EAGLE_TSC_pll_div_80x_refc125,
	EAGLE_TSC_pll_div_80x_refc106,
	EAGLE_TSC_pll_div_80x,
	EAGLE_TSC_pll_div_80x_refc156,
	EAGLE_TSC_pll_div_82p5x,
	EAGLE_TSC_pll_div_87p5x,
	EAGLE_TSC_pll_div_92x,
	EAGLE_TSC_pll_div_100x,
	EAGLE_TSC_pll_div_199p04x,
	EAGLE_TSC_pll_div_36p8x
};














#endif
