/******************************************************************************
******************************************************************************
*  Revision      :  $Id: phy8806x_tsc_enum.h 925 2015-02-24 23:23:58Z kirand $ *
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

/** @file phy8806x_tsc_enum.h
 * Enum types used by Serdes API functions
 */

#ifndef PHY8806X_TSC_API_ENUM_H
#define PHY8806X_TSC_API_ENUM_H

#include "phy8806x_tsc_common_enum.h"












/** Falcon2 MT2 PLL Config Enum */
enum phy8806x_tsc_pll_enum {
	PHY8806X_TSC_pll_div_80x,
	PHY8806X_TSC_pll_div_96x,
	PHY8806X_TSC_pll_div_120x,
	PHY8806X_TSC_pll_div_128x,
	PHY8806X_TSC_pll_div_128x_vco2,
	PHY8806X_TSC_pll_div_132x,
	PHY8806X_TSC_pll_div_140x,
	PHY8806X_TSC_pll_div_144x,
	PHY8806X_TSC_pll_div_160x,
	PHY8806X_TSC_pll_div_160x_vco2,
	PHY8806X_TSC_pll_div_160x_refc174,
	PHY8806X_TSC_pll_div_165x,
	PHY8806X_TSC_pll_div_168x,
	PHY8806X_TSC_pll_div_175x,
	PHY8806X_TSC_pll_div_180x,
	PHY8806X_TSC_pll_div_184x,
	PHY8806X_TSC_pll_div_198x,
	PHY8806X_TSC_pll_div_200x,
	PHY8806X_TSC_pll_div_224x,
	PHY8806X_TSC_pll_div_264x,
	PHY8806X_TSC_pll_div_120x_refc125,
	PHY8806X_TSC_pll_div_132x_refc212,
	PHY8806X_TSC_pll_div_165x_refc125,
	PHY8806X_TSC_pll_div_180x_refc125
};

#endif
