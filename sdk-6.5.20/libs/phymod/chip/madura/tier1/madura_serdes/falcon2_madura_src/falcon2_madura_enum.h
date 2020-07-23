/******************************************************************************
 ******************************************************************************
 *  Revision      :  $Id: falcon2_madura_enum.h 1340 2016-02-04 22:50:01Z kirand $ *
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

/** @file falcon2_madura_enum.h
 * Enum types used by Serdes API functions
 */

#ifndef FALCON2_MADURA_API_ENUM_H
#define FALCON2_MADURA_API_ENUM_H

#include "../common2/srds_api_enum.h"









/** Falcon2 Madura PLL Config Enum */
enum falcon2_madura_pll_enum {
  FALCON2_MADURA_pll_div_80x,
  FALCON2_MADURA_pll_div_96x,
  FALCON2_MADURA_pll_div_120x,
  FALCON2_MADURA_pll_div_128x,
  FALCON2_MADURA_pll_div_128x_vco2,
  FALCON2_MADURA_pll_div_132x,
  FALCON2_MADURA_pll_div_140x,
  FALCON2_MADURA_pll_div_144x,
  FALCON2_MADURA_pll_div_160x,
  FALCON2_MADURA_pll_div_160x_vco2,
  FALCON2_MADURA_pll_div_160x_refc174,
  FALCON2_MADURA_pll_div_165x,
  FALCON2_MADURA_pll_div_168x,
  FALCON2_MADURA_pll_div_175x,
  FALCON2_MADURA_pll_div_180x,
  FALCON2_MADURA_pll_div_184x,
  FALCON2_MADURA_pll_div_198x,
  FALCON2_MADURA_pll_div_200x,
  FALCON2_MADURA_pll_div_224x,
  FALCON2_MADURA_pll_div_264x,
  FALCON2_MADURA_pll_div_120x_refc125,
  FALCON2_MADURA_pll_div_132x_refc212,
  FALCON2_MADURA_pll_div_165x_refc125,
  FALCON2_MADURA_pll_div_180x_refc125
};











#endif
