/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __CHIP_SIM_INDIRECT_H__
/* { */
#define __CHIP_SIM_INDIRECT_H__


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

/*  } */


typedef struct
{
  /* In RTP block
   * description of the address space (from 0x30002000 to 0x3000381f in FE)
   * these are long orientated.
   */
  UINT32 read_result_address;
  UINT32 access_trig_offset;
  UINT32 access_address_offset;
  UINT32 write_val_offset;

  UINT32 start_address;
  UINT32 end_address;

  int nof_longs_to_move;

  /*
   * The actual address
   */
  UINT32* base[CHIP_SIM_NOF_CHIPS] ;
} CHIP_SIM_INDIRECT_BLOCK;

STATUS chip_sim_indirect_op(SOC_SAND_IN int chip_ndx);
void chip_sim_indirect_init(void);
void chip_sim_indirect_init_chip_specifics( CHIP_SIM_INDIRECT_BLOCK* indirect_blocks
                                           );
STATUS chip_sim_indirect_malloc(void);
void chip_sim_indirect_free(void);
STATUS chip_sim_indirect_triger(SOC_SAND_IN UINT32 time_diff);
uint8 chip_sim_indirect_is_access_trig(SOC_SAND_IN UINT32 chip_offset);
UINT32 chip_sim_indirect_get_delay(void);
STATUS chip_sim_indirect_set_delay_op(SOC_SAND_IN int chip_ndx);




/* } __CHIP_SIM_INDIRECT_H__*/
#endif
