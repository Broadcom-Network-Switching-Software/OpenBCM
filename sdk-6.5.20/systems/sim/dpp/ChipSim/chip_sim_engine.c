/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include "chip_sim.h"
#include "chip_sim_engine.h"
#include "chip_sim_counter.h"
#include "chip_sim_interrupts.h"
#include "chip_sim_indirect.h"
#include "chip_sim_mem.h"
#include "chip_sim_cell.h"




/* { */
/* run over counters */
int
  Cntr_enable = FALSE;

uint8
 chip_sim_engine_get_cntr_enable(
   void
   )
{
  return Cntr_enable;
}

void
 chip_sim_engine_set_cntr_enable(
   SOC_SAND_IN uint8 i_cntr_enable
   )
{
  Cntr_enable = i_cntr_enable;
}
/* } */

/* { */
/* run over interrupts */
int
  Int_enable = FALSE;

uint8
 chip_sim_engine_get_int_enable(
   void
   )
{
  return Int_enable;
}

void
 chip_sim_engine_set_int_enable(
   SOC_SAND_IN uint8 i_int_enable
   )
{
  Int_enable = i_int_enable;
}
/* } */


/*****************************************************
*NAME
*   chip_sim_engine_run
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 18-Sep-02 16:44:08
*FUNCTION:
*  (1) disable interrupts
*  (2) go over all modules.
*  (3) enable interrupts.
*  (4) trigger interrupt - if needed.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) const int chip_ndx
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_engine_run(
    SOC_SAND_IN UINT32 time_diff
    )
{
  STATUS
    status = OK ;
  uint8
    gen_int = FALSE ;

  uint32
    interrupt_flag;

  /* 1 */
  soc_sand_os_stop_interrupts(&interrupt_flag);

  /* 2 */
  chip_sim_indirect_triger(time_diff);
  if (chip_sim_engine_get_cntr_enable())
  {
    chip_sim_counter_run(time_diff);
  }

  chip_sim_cell_tx(time_diff);
  chip_sim_cell_rx(time_diff);

  if (chip_sim_engine_get_int_enable())
  {
    chip_sim_interrupt_run(time_diff, &gen_int);
  }
  /* 3 */
  soc_sand_os_start_interrupts(interrupt_flag);

  /* 4 */
  if (gen_int)
  {
    chip_sim_interrupt_cpu_assert();
  }

  return status ;
}

