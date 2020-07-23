/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/
#ifndef SOC_SAND_GENERAL_PARAMS_H
#define SOC_SAND_GENERAL_PARAMS_H
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>



#define SOC_SAND_VER_STRING_SIZE  (20)


extern uint32  Soc_sand_max_num_devices;
extern uint32  Soc_sand_system_ticks_in_ms;
extern uint32  Soc_sand_min_time_between_tcm_activation;
extern uint32  Soc_sand_driver_is_started;
extern uint32  Soc_sand_tcm_mockup_interrupts;

void
  soc_get_sand_string_ver(
    char string_buff[SOC_SAND_VER_STRING_SIZE]
  );

unsigned
  long
    soc_get_sand_ver(
      void
    ) ;

void
  soc_sand_version_to_string(
    SOC_SAND_IN    uint32  version,
    SOC_SAND_INOUT char           string_buff[SOC_SAND_VER_STRING_SIZE]
  );



uint32
  soc_sand_general_get_max_num_devices(
    void
  );

SOC_SAND_RET
  soc_sand_general_set_max_num_devices(
    uint32  max_num_devices
  );


uint32
  soc_sand_general_get_system_tick_in_ms(
    void
  );

SOC_SAND_RET
  soc_sand_general_set_system_tick_in_ms(
    uint32 system_tick_in_ms
  );


uint32
  soc_sand_general_get_tcm_task_priority(
    void
  );

SOC_SAND_RET
  soc_sand_general_set_tcm_task_priority(
    uint32 soc_tcmtask_priority
  );


uint32
  soc_sand_general_get_min_time_between_tcm_activation(
    void
  );

SOC_SAND_RET
  soc_sand_general_set_min_time_between_tcm_activation(
    uint32 min_time_between_tcm_activation
  );


uint32
  soc_sand_general_get_driver_is_started(
    void
  );

void
  soc_sand_general_set_driver_is_started(
    uint32  driver_is_started
  );


uint32
  soc_sand_general_get_tcm_mockup_interrupts(
    void
  );

SOC_SAND_RET
  soc_sand_general_set_tcm_mockup_interrupts(
    uint32 soc_tcmmockup_interrupts
  );




#if SOC_SAND_DEBUG
void
  soc_sand_status_print(
    void
  );
#endif
#ifdef  __cplusplus
}
#endif

#endif
