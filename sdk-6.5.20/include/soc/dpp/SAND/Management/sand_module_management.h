/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifndef __SOC_SAND_MODULE_MANAGEMENT_H_INCLUDED__

#define __SOC_SAND_MODULE_MANAGEMENT_H_INCLUDED__
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>

extern int Soc_sand_start_module_shut_down_mutex;


uint32
  soc_sand_module_open(
    uint32      max_num_devices,
    uint32      system_tick_in_ms,
    uint32      soc_tcmtask_priority,
    uint32      min_time_between_tcm_activation,
    uint32      soc_tcmmockup_interrupts,
    SOC_SAND_ERROR_HANDLER_PTR error_handler,
    char              *error_description,
    uint32      *is_already_opened
  );

uint32
  soc_sand_module_close(
    void
  );

#ifdef  __cplusplus
}
#endif

#endif
