/******************************************************************************
 ******************************************************************************
 *  Revision      :  $Id: eagle2_tsc2pll_enum.h 1543 2017-04-07 20:52:44Z kirand $ *
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

/** @file eagle2_tsc2pll_enum.h
 * Enum types used by Serdes API functions
 */

#ifndef EAGLE2_TSC2PLL_API_ENUM_H
#define EAGLE2_TSC2PLL_API_ENUM_H

#include "common/srds_api_enum.h"



/** Eagle PLL Config Enum */
enum eagle2_tsc2pll_pll_enum {
  EAGLE2_TSC2PLL_pll_div_40x,
  EAGLE2_TSC2PLL_pll_div_42x,
  EAGLE2_TSC2PLL_pll_div_46x,
  EAGLE2_TSC2PLL_pll_div_50x,
  EAGLE2_TSC2PLL_pll_div_52x,
  EAGLE2_TSC2PLL_pll_div_60x,
  EAGLE2_TSC2PLL_pll_div_64x_refc161,
  EAGLE2_TSC2PLL_pll_div_64x_refc156,
  EAGLE2_TSC2PLL_pll_div_64x,
  EAGLE2_TSC2PLL_pll_div_66x,
  EAGLE2_TSC2PLL_pll_div_68x,
  EAGLE2_TSC2PLL_pll_div_70x,
  EAGLE2_TSC2PLL_pll_div_72x,
  EAGLE2_TSC2PLL_pll_div_73p6x,
  EAGLE2_TSC2PLL_pll_div_80x_refc125,
  EAGLE2_TSC2PLL_pll_div_80x_refc106,
  EAGLE2_TSC2PLL_pll_div_80x,
  EAGLE2_TSC2PLL_pll_div_80x_refc156,
  EAGLE2_TSC2PLL_pll_div_82p5x,
  EAGLE2_TSC2PLL_pll_div_87p5x,
  EAGLE2_TSC2PLL_pll_div_92x,
  EAGLE2_TSC2PLL_pll_div_100x,
  EAGLE2_TSC2PLL_pll_div_199p04x,
  EAGLE2_TSC2PLL_pll_div_36p8x,
  EAGLE2_TSC2PLL_pll_div_206p25x,
  EAGLE2_TSC2PLL_pll_div_200p00x,
  EAGLE2_TSC2PLL_pll_div_250p00x
};



















#endif
