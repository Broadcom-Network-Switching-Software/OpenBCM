/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_SAND_64CNT_H_INCLUDED__

#define __SOC_SAND_64CNT_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>


typedef struct
{
  
  SOC_SAND_U64      u64;

  
  uint32   overflowed ;

} SOC_SAND_64CNT;



void
  soc_sand_64cnt_clear(
    SOC_SAND_INOUT SOC_SAND_64CNT* counter
  );


void
  soc_sand_64cnt_clear_ov(
    SOC_SAND_INOUT SOC_SAND_64CNT* counter
  );


void
  soc_sand_64cnt_add_long(
    SOC_SAND_INOUT SOC_SAND_64CNT*   counter,
    SOC_SAND_IN    uint32 value_to_add
  );


void
  soc_sand_64cnt_add_64cnt(
    SOC_SAND_INOUT SOC_SAND_64CNT* counter,
    SOC_SAND_IN    SOC_SAND_64CNT* value_to_add
  );

#if SOC_SAND_DEBUG




void
  soc_sand_64cnt_print(
    SOC_SAND_IN SOC_SAND_64CNT*  counter,
    SOC_SAND_IN uint32 short_format
  );


uint32
  soc_sand_64cnt_test(uint32 silent);


#endif


#ifdef  __cplusplus
}
#endif



#endif
