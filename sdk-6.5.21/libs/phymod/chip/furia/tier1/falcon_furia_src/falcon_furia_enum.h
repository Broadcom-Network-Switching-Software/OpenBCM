/******************************************************************************
 ******************************************************************************
 *  Revision      :  $Id: falcon_furia_enum.h 1017 2015-04-24 22:07:57Z kirand $ *
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

/** @file falcon_furia_enum.h
 * Enum types used by Serdes API functions
 */

#ifndef FALCON_FURIA_API_ENUM_H
#define FALCON_FURIA_API_ENUM_H

#include "../common/srds_api_enum.h"







/** Falcon Furia PLL Config Enum */
enum falcon_furia_pll_enum {
  FALCON_FURIA_pll_div_80x,
  FALCON_FURIA_pll_div_96x,
  FALCON_FURIA_pll_div_120x,
  FALCON_FURIA_pll_div_128x,
  FALCON_FURIA_pll_div_132x,
  FALCON_FURIA_pll_div_140x,
  FALCON_FURIA_pll_div_144x,
  FALCON_FURIA_pll_div_160x,
  FALCON_FURIA_pll_div_160x_vco2,
  FALCON_FURIA_pll_div_160x_refc174,
  FALCON_FURIA_pll_div_165x,
  FALCON_FURIA_pll_div_168x,
  FALCON_FURIA_pll_div_175x,
  FALCON_FURIA_pll_div_180x,
  FALCON_FURIA_pll_div_184x,
  FALCON_FURIA_pll_div_198x,
  FALCON_FURIA_pll_div_200x,
  FALCON_FURIA_pll_div_224x,
  FALCON_FURIA_pll_div_264x,
  FALCON_FURIA_pll_div_120x_refc125,
  FALCON_FURIA_pll_div_132x_refc212,
  FALCON_FURIA_pll_div_165x_refc125,
  FALCON_FURIA_pll_div_180x_refc125
};





#endif
