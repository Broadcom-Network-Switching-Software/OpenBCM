/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef _CHIP_SIM_COMMAND_H_
/* { */
#define _CHIP_SIM_COMMAND_H_

#include "chip_sim.h"


STATUS chip_sim_command_indirect_delay(SOC_SAND_IN UINT32 delay,
                                       SOC_SAND_OUT char  msg[]) ;
STATUS chip_sim_command_wake_up_task(SOC_SAND_OUT char  msg[]) ;
STATUS chip_sim_command_set_task_sleep_time(SOC_SAND_IN  UINT32 mili_sec,
                                            SOC_SAND_OUT char   msg[]) ;
STATUS chip_sim_command_interrupt_assert(SOC_SAND_OUT char  msg[]) ;
STATUS chip_sim_command_mask_all_int(SOC_SAND_IN  uint8  active,
                                     SOC_SAND_OUT char  msg[]
                                     ) ;
STATUS chip_sim_command_int(SOC_SAND_IN int    chip_ndx,
                            SOC_SAND_IN UINT32 int_chip_offset,
                            SOC_SAND_IN UINT32 int_bit,
                            SOC_SAND_IN uint8   is_random,
                            SOC_SAND_IN INT32  interval_milisec,
                            SOC_SAND_IN UINT32 count,
                            SOC_SAND_OUT char  msg[]
                            ) ;
STATUS
  chip_sim_command_cell_tx_delay(
    SOC_SAND_IN UINT32 delay,
    SOC_SAND_OUT char  msg[]
    );

STATUS
  chip_sim_command_data_cell_rx(
    SOC_SAND_IN int           chip_ndx,
    SOC_SAND_IN int           block_ndx,
    SOC_SAND_IN unsigned char *data_buff,
    SOC_SAND_IN int           data_buff_size,
    SOC_SAND_IN uint8          is_random,
    SOC_SAND_IN INT32         interval_mili,
    SOC_SAND_IN UINT32        cells_count,
    SOC_SAND_OUT char         msg[]
    );

STATUS
  chip_sim_command_counter(
    SOC_SAND_IN  uint8  is_random,
    SOC_SAND_OUT char  msg[]
    );

STATUS
  chip_sim_command_en_counter(
    SOC_SAND_IN  uint8  active,
    SOC_SAND_OUT char  msg[]
    );

STATUS
  chip_sim_command_en_int(
    SOC_SAND_IN  uint8  active,
    SOC_SAND_OUT char  msg[]
    );

STATUS
  chip_sim_command_get_time_monitor(
    SOC_SAND_OUT char   msg[]
    );

STATUS
  chip_sim_command_interrupt_assert(
    SOC_SAND_OUT char  msg[]
    );
/* } _CHIP_SIM_COMMAND_H_*/
#endif

