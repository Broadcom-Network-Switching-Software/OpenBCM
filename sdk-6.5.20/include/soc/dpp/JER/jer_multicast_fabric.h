/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef _SOC_JER_MULTICAST_FABRIC_H
#define _SOC_JER_MULTICAST_FABRIC_H

#include <soc/dpp/ARAD/arad_chip_regs.h>


uint32
  jer_mult_fabric_enhanced_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_SAND_U32_RANGE                  *queue_range
  );

uint32
  jer_mult_fabric_enhanced_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_INOUT SOC_SAND_U32_RANGE                *queue_range
  );

#endif 
