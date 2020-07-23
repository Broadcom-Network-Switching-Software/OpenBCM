/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef _ARAD_SIM_EM_BLOCK_H_

#define _ARAD_SIM_EM_BLOCK_H_

#ifndef UINT32
  #define UINT32 unsigned int
#endif

typedef struct
{
  UINT32          read_result_address; 
  UINT32          offset;
  UINT32          table_size;
  UINT32          key_size;
  UINT32          data_nof_bytes;
  UINT32          start_address;
  UINT32          end_address;
  SOC_SAND_MULTI_SET_PTR    multi_set;
  PARSER_HINT_ARR uint8*          base;
} CHIP_SIM_EM_BLOCK;


#endif
