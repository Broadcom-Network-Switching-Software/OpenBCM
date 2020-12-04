/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_SAND_WORKLOAD_STATUS_H_INCLUDED__

#define __SOC_SAND_WORKLOAD_STATUS_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>

#if SOC_SAND_DEBUG


void
  soc_sand_workload_status_start(
    int unit,
    uint32 total_work_load
  );

void
  soc_sand_workload_status_run(
    int unit,
    uint32 current_workload
  );

void
  soc_sand_workload_status_get(
    int  unit,
    uint32 *percent
  );

void
  soc_sand_workload_status_run_no_print(
    int  unit,
    uint32 current_workload
  );

#ifdef  __cplusplus
}
#endif


#endif 


#endif
