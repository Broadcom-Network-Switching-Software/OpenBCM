/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#ifndef __SOC_SAND_TRACE_H_INCLUDED__

#define __SOC_SAND_TRACE_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>


SOC_SAND_RET
  soc_sand_trace_init(
    void
  );


SOC_SAND_RET
  soc_sand_trace_end(
    void
  );


SOC_SAND_RET
  soc_sand_trace_clear(
    void
  );


SOC_SAND_RET
  soc_sand_trace_add_entry(
    SOC_SAND_IN uint32  identifier,
    SOC_SAND_IN char* const   str    
  );



SOC_SAND_RET
  soc_sand_trace_print(
    void
  );

#ifdef  __cplusplus
}
#endif


#endif
