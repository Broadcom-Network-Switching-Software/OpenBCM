/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_SAND_USER_CALLBACK_H_INCLUDED__

#define __SOC_SAND_USER_CALLBACK_H_INCLUDED__
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>


typedef int (*SOC_SAND_USER_CALLBACK_FUNC)(uint32, uint32 *,uint32 **,
  uint32,
  uint32, uint32,
  uint32, uint32, uint32) ;


typedef struct
{

  
  SOC_SAND_USER_CALLBACK_FUNC callback_func;

  
  uint32*    result_ptr;

  
  uint32      param1;
  uint32      param2;
  uint32      param3;
  uint32      param4;
  uint32      param5;
  uint32      param6;

  
  uint32      interrupt_not_poll;

  
  uint32      callback_rate;

  
  uint32   user_callback_id;

} SOC_SAND_USER_CALLBACK;

#ifdef  __cplusplus
}
#endif


#endif
