/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_SAND_MEM_ACCESS_CALLBACK_H_INCLUDED__

#define __SOC_SAND_MEM_ACCESS_CALLBACK_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>


typedef struct
{
  
  int      unit;
  
  uint32     *result_ptr;
  
  uint32     copy_of_last_result_ptr[SOC_SAND_CALLBACK_BUF_SIZE>>2];
  
  uint32      copy_of_result_is_valid;
  
  uint32     offset;
  
  uint32      size;
  
  uint32      indirect;
  
  uint32     soc_sand_tcm_callback_id;
  
  uint32     user_callback_id;
  
  SOC_SAND_USER_CALLBACK_FUNC  user_callback_proc;
} SOC_SAND_MEM_READ_CALLBACK_STRUCT;



uint32
  soc_sand_mem_read_callback(
    uint32 *buffer,
    uint32 size
  );


#ifdef  __cplusplus
}
#endif


#endif
