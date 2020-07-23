/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_SAND_TRIGGER_H_INCLUDED__

#define __SOC_SAND_TRIGGER_H_INCLUDED__
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>


#define SOC_SAND_GENERAL_TRIG_BIT   0



SOC_SAND_RET
  soc_sand_trigger_verify_0(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint32  offset,
    SOC_SAND_IN uint32  timeout, 
    SOC_SAND_IN uint32  trigger_bit
  );


SOC_SAND_RET
  soc_sand_trigger_assert_1(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint32  offset,
    SOC_SAND_IN uint32  timeout, 
    SOC_SAND_IN uint32  trigger_bit
  );




SOC_SAND_RET
  soc_sand_trigger_verify_0_by_base(
    SOC_SAND_IN uint32  offset,
    SOC_SAND_IN uint32  timeout, 
    SOC_SAND_IN uint32  trigger_bit,
    SOC_SAND_IN uint32  *device_base_address
  );



SOC_SAND_RET
  soc_sand_trigger_assert_1_by_base(
    SOC_SAND_IN     uint32  offset,
    SOC_SAND_IN     uint32  timeout, 
    SOC_SAND_IN     uint32  trigger_bit,
    SOC_SAND_INOUT  uint32  *device_base_address
  );

#ifdef  __cplusplus
}
#endif


#endif
