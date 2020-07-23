/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef _CHIP_SIM_COUNTER_H_
/* { */
#define _CHIP_SIM_COUNTER_H_

#include "chip_sim.h"

/*
 * INCLUDE FILES:
 * {
 */
#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */
/* } */
#else
/* { */
/* } */
#endif


typedef struct
{
  UINT32 chip_offset ;
  UINT32 mask ;
  UINT32 shift ;
  uint8   is_random;
  UINT32 count_per_sec;
  UINT32 min ;
  UINT32 max ;
} CHIP_SIM_COUNTER;


STATUS chip_sim_cntr_malloc(void);
void chip_sim_cntr_free(void);

STATUS chip_sim_counter_run(SOC_SAND_IN UINT32 time_diff);

SOC_SAND_RET chip_sim_cntr_init_chip_specifics(CHIP_SIM_COUNTER * const m_counters);
STATUS chip_sim_cntr_init(void);
STATUS chip_sim_counter_change(SOC_SAND_IN int    chip_ndx,
                               SOC_SAND_IN UINT32 chip_offset,
                               SOC_SAND_IN uint8   is_random,
                               SOC_SAND_IN UINT32 count_per_sec,
                               SOC_SAND_IN UINT32 min,
                               SOC_SAND_IN UINT32 max);

/* } _CHIP_SIM_COUNTER_H_*/
#endif
