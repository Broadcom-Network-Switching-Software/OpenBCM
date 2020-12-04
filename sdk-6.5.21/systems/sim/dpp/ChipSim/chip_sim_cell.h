/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef _CHIP_SIM_CELL_H_
/* { */
#define _CHIP_SIM_CELL_H_

#include "chip_sim.h"

#include <soc/dpp/SAND/SAND_FM/sand_cell.h>

/*
 * Check if has send to cell (data or control).
 * Check if can send the cell (the relevant bit is down).
 * Check if has cell from the list of cell.
 *   case can, copy the cell to the registers and set the trap bit on.
 *    call to the interrupt service to enable the interrupt.
 */
STATUS chip_sim_cell_op(SOC_SAND_IN int chip_ndx);
STATUS chip_sim_cell_tx(SOC_SAND_IN UINT32 time_diff);
STATUS chip_sim_cell_rx(SOC_SAND_IN UINT32 time_diff);
STATUS chip_sim_cell_set_delay_op(SOC_SAND_IN int chip_ndx);
UINT32 chip_sim_cell_tx_get_delay(void);
void   chip_sim_cell_tx_set_delay(SOC_SAND_IN UINT32 delay_mili);
uint8 chip_sim_cell_is_trig_offset(SOC_SAND_IN UINT32 chip_offset);
SOC_SAND_RET chip_sim_cell_init_chip_specifics(SOC_SAND_IN int    i_nof_cell_blocks,
                                           SOC_SAND_IN UINT32 cell_access_trig_address);
SOC_SAND_RET chip_sim_cell_rx_init_chip_specifics(SOC_SAND_IN UINT32 chip_start_data_cell_offset_a,
                                              SOC_SAND_IN UINT32 chip_start_data_cell_offset_b,
                                              SOC_SAND_IN UINT32 data_int_address_a,
                                              SOC_SAND_IN UINT32 data_int_address_b,
                                              SOC_SAND_IN int    data_int_bit_a,
                                              SOC_SAND_IN int    data_int_bit_b,
                                              SOC_SAND_IN UINT32 chip_start_ctrl_cell_offset_a,
                                              SOC_SAND_IN UINT32 chip_start_ctrl_cell_offset_b,
                                              SOC_SAND_IN UINT32 ctrl_int_address_a,
                                              SOC_SAND_IN UINT32 ctrl_int_address_b,
                                              SOC_SAND_IN int    ctrl_int_bit_a,
                                              SOC_SAND_IN int    ctrl_int_bit_b
                                              );
STATUS chip_sim_cell_init(void);
STATUS chip_sim_cell_free();
STATUS chip_sim_cell_malloc();


STATUS chip_sim_cell_add_rx_data(
                                  SOC_SAND_IN int           chip_ndx,
                                  SOC_SAND_IN int           block_ndx,
                                  SOC_SAND_IN unsigned char *data_buff,
                                  SOC_SAND_IN int           data_buff_size,
                                  SOC_SAND_IN uint8          is_random,
                                  SOC_SAND_IN INT32         interval_mili,
                                  SOC_SAND_IN UINT32        cells_count
                                  );



/*
 * Check the necessary bit, if was written.
 *  Case yes: if was configured to loop-back, copy the cell to
 *    list of cells, case no loop-back, reset the bit (now, or remember to reset it later in time).
 * should be called on every write.
 */
STATUS
  chip_sim_read_cell();



/* } _CHIP_SIM_CELL_H_*/
#endif

