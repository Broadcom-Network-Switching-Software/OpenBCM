/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef _CHIP_SIM_MEM_H_
/* { */
#define _CHIP_SIM_MEM_H_

#include <appl/diag/dpp/dune_chips.h>

#include "chip_sim_counter.h"

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

void chip_sim_mem_init(void);
STATUS chip_sim_mem_malloc(
  SOC_SAND_IN SOC_SAND_DEVICE_TYPE chip_type,
  SOC_SAND_IN unsigned int     chip_ver,
  SOC_SAND_IN DESC_FE_REG*     desc,
  SOC_SAND_IN UINT32           start_address[CHIP_SIM_NOF_CHIPS]);
STATUS chip_sim_mem_free(void);
STATUS chip_sim_mem_address_to_offset(SOC_SAND_IN  UINT32  address,
                                      SOC_SAND_OUT UINT32* chip_offset,
                                      SOC_SAND_OUT int*    chip_ndx);
STATUS chip_sim_mem_check_is_write_access(SOC_SAND_IN UINT32 chip_offset);
STATUS
  chip_sim_mem_reset(SOC_SAND_IN int chip_ndx);
void chip_sim_mem_init(void);


STATUS chip_sim_indirect_set_delay_time(SOC_SAND_IN UINT32 delay);

STATUS chip_sim_mem_write(SOC_SAND_IN int    chip_ndx,
                          SOC_SAND_IN UINT32 chip_offset,
                          SOC_SAND_IN UINT32 value);
STATUS chip_sim_mem_read( SOC_SAND_IN  int    chip_ndx,
                          SOC_SAND_IN  UINT32 chip_offset,
                          SOC_SAND_OUT UINT32 *value);
STATUS chip_sim_mem_write_field(SOC_SAND_IN int    chip_ndx,
                                SOC_SAND_IN UINT32 chip_offset,
                                SOC_SAND_IN UINT32 value,
                                SOC_SAND_IN UINT32 shift,
                                SOC_SAND_IN UINT32 mask);
STATUS chip_sim_mem_read_field(SOC_SAND_IN  int    chip_ndx,
                               SOC_SAND_IN  UINT32 chip_offset,
                               SOC_SAND_OUT UINT32 *value,
                               SOC_SAND_IN  UINT32 shift,
                               SOC_SAND_IN  UINT32 mask);
STATUS chip_sim_mem_check_is_indirect_access(SOC_SAND_IN UINT32 chip_offset);
STATUS chip_sim_mem_check_is_write_access(SOC_SAND_IN UINT32 chip_offset);
STATUS chip_sim_mem_check_is_read_access(SOC_SAND_IN UINT32 chip_offset);
STATUS chip_sim_mem_check_is_clear_access(SOC_SAND_IN UINT32 chip_offset);


/* } _CHIP_SIM_MEM_H_*/
#endif

