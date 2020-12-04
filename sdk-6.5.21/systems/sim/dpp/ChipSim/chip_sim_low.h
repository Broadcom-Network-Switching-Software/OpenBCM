/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef _CHIP_SIM_LOW_H_
/* { */
#define _CHIP_SIM_LOW_H_

/*
#include "chip_sim.h"
*/
#ifndef SOC_SAND_IN
  #define SOC_SAND_IN const
#endif
#ifndef SOC_SAND_OUT
  #define SOC_SAND_OUT
#endif
#ifndef SOC_SAND_INOUT
  #define SOC_SAND_INOUT
#endif
#ifndef UINT32
#define UINT32 uint32
#endif
#ifndef STATUS
  #define STATUS int
#endif

/*
 * Taking this file out since it is in dpp-specific directory and stands
 * on the way of separating DPP and DNX.
 *
 * #include <soc/dpp/SAND/Utils/sand_framework.h>
 */
/*
 * if valid address .. write the value.
 *  check any trigger was enabled (send cell).
 */
STATUS chip_sim_write(SOC_SAND_IN UINT32 address, SOC_SAND_IN UINT32 value);

/*
 * if valid address .. read this address
 */
STATUS chip_sim_read(SOC_SAND_IN UINT32  address,  SOC_SAND_OUT UINT32 *value);

/*
 * Return type should be SOC_SAND_RET but this would require 'sand_framework.h' and
 * would stand on the way for separating DPP and DNX.
 */
unsigned short 
  soc_sand_sim_write(
    SOC_SAND_IN     uint32 *array,
    SOC_SAND_INOUT  uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
  );

unsigned short 
  soc_sand_sim_read(
    SOC_SAND_INOUT  uint32 *array,
    SOC_SAND_IN     uint32 *base_address,
    SOC_SAND_IN     uint32 offset,
    SOC_SAND_IN     uint32 size
  );

/* } _CHIP_SIM_LOW_H_*/
#endif
