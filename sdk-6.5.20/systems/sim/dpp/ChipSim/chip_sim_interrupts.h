/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#ifndef _CHIP_SIM_INTERRUPT_H_
/* { */
#define _CHIP_SIM_INTERRUPT_H_

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
  /*
   * field_id in DESC_SAND_REG
   */
  UINT32   int_chip_offset ;
  UINT32   int_bit;
  UINT32   mask_chip_offset ;
  UINT32   mask_bit ;

  UINT32   father_chip_offset ;
  UINT32   father_bit ;
  UINT32   father_mask_chip_offset ;
  UINT32   father_mask_bit ;


  uint8  is_random;
  INT32 interval_milisec;
  INT32 time_left_milisec;
  UINT32 count;
} CHIP_SIM_INTERRUPT;


STATUS chip_sim_interrupt_init(void);
STATUS chip_sim_interrupt_init_chip_specifics(SOC_SAND_IN CHIP_SIM_INTERRUPT* const m_sim_int,
                                              SOC_SAND_IN UINT32  i_general_int_chip_offset,
                                              SOC_SAND_IN int     i_general_int_bit,
                                              SOC_SAND_IN UINT32  i_general_mask_chip_offset,
                                              SOC_SAND_IN int     i_general_mask_bit
                                              );
STATUS chip_sim_interrupt_malloc(void);
void   chip_sim_interrupt_free(void);
STATUS chip_sim_interrupt_profile(SOC_SAND_IN    int                profile,
                                  SOC_SAND_INOUT CHIP_SIM_INTERRUPT *interrupt);
STATUS chip_sim_interrupt_run(SOC_SAND_IN  UINT32 time_diff,
                              SOC_SAND_OUT uint8   *general_int
                              );
STATUS chip_sim_interrupt_op(SOC_SAND_IN int chip_ndx,CHIP_SIM_INTERRUPT *int_p);
STATUS chip_sim_interrupt_mask_all(SOC_SAND_IN UINT32 mask);



void chip_sim_interrupt_cpu_assert(void);
void chip_sim_interrupt_cpu_deassert(void);
STATUS chip_sim_interrupt_cpu_hock(SOC_SAND_OUT char msg[]);
STATUS chip_sim_interrupt_cpu_unhock(SOC_SAND_OUT char msg[]);
STATUS chip_sim_interrupt_set_interrupt_op(SOC_SAND_IN int    chip_ndx,
                                           SOC_SAND_IN UINT32 int_chip_offset,
                                           SOC_SAND_IN UINT32 int_bit,
                                           SOC_SAND_IN uint8   is_random,
                                           SOC_SAND_IN INT32  interval_milisec,
                                           SOC_SAND_IN UINT32 count,
                                           SOC_SAND_OUT char  msg[]
                                           );
/* } _CHIP_SIM_INTERRUPT_H_*/
#endif
