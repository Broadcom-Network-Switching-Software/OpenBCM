/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/



#include <shared/bsl.h>

#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/SAND/Utils/sand_tcm.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>





static const
  unsigned
    long
      Soc_sand_ver = 7110;


void
    soc_get_sand_string_ver(
      char string_buff[SOC_SAND_VER_STRING_SIZE]
    )
{

  if (NULL == string_buff)
  {
    goto exit;
  }

  soc_sand_version_to_string(Soc_sand_ver, string_buff);

exit:
  return;
}


unsigned
  long
    soc_get_sand_ver(
      void
    )
{
  int
    ret ;
  ret = Soc_sand_ver ;
  return (ret) ;
}


void
  soc_sand_version_to_string(
    SOC_SAND_IN    uint32  version,
    SOC_SAND_INOUT char           string_buff[SOC_SAND_VER_STRING_SIZE]
  )
{

  if (NULL == string_buff)
  {
    goto exit;
  }

  sal_sprintf(
    string_buff,
    "%u.%03u", version/1000, version%1000
  );

exit:
  return;
}





uint32  Soc_sand_max_num_devices                 = SOC_SAND_MAX_DEVICE;
uint32  Soc_sand_system_ticks_in_ms              = 0;
uint32  Soc_sand_min_time_between_tcm_activation = SOC_SAND_MIN_TCM_ACTIVATION_PERIOD;
uint32  Soc_sand_driver_is_started               = 0;
uint32  Soc_sand_tcm_mockup_interrupts        = 0;


uint32
  soc_sand_general_get_max_num_devices(
    void
  )
{
  return Soc_sand_max_num_devices ;
}

SOC_SAND_RET
  soc_sand_general_set_max_num_devices(
    uint32  max_num_devices
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_ERR ;
  uint32
    err = 0 ;
  if (SOC_SAND_MAX_DEVICE < max_num_devices)
  {
    err = 1 ;
    goto exit ;
  }
  Soc_sand_max_num_devices = max_num_devices ;
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_GENERAL_SET_MAX_NUM_DEVICES,
        "General error in soc_sand_general_set_max_num_devices()",err,0,0,0,0,0) ;
  return ex ;
}


uint32
  soc_sand_general_get_system_tick_in_ms(
    void
  )
{
  return Soc_sand_system_ticks_in_ms ;
}

SOC_SAND_RET
  soc_sand_general_set_system_tick_in_ms(
    uint32 system_tick_in_ms
  )
{
  Soc_sand_system_ticks_in_ms = system_tick_in_ms ;
  return SOC_SAND_OK ;
}


uint32
  soc_sand_general_get_tcm_task_priority(
    void
  )
{
  return soc_sand_tcm_get_task_priority() ;
}

SOC_SAND_RET
  soc_sand_general_set_tcm_task_priority(
    uint32 soc_tcmtask_priority
  )
{
  soc_sand_tcm_set_task_priority(soc_tcmtask_priority) ;
  return SOC_SAND_OK ;
}


uint32
  soc_sand_general_get_min_time_between_tcm_activation(
    void
  )
{
  return Soc_sand_min_time_between_tcm_activation;
}

SOC_SAND_RET
  soc_sand_general_set_min_time_between_tcm_activation(
    uint32 min_time_between_tcm_activation
  )
{
  if(min_time_between_tcm_activation < SOC_SAND_MIN_TCM_ACTIVATION_PERIOD)
  {
    return SOC_SAND_ERR;
  }
  Soc_sand_min_time_between_tcm_activation = min_time_between_tcm_activation;
  return SOC_SAND_OK ;
}


uint32
  soc_sand_general_get_driver_is_started(
    void
  )
{
  return Soc_sand_driver_is_started;
}

void
  soc_sand_general_set_driver_is_started(
    uint32  driver_is_started
  )
{
  Soc_sand_driver_is_started = driver_is_started ;
}


uint32
  soc_sand_general_get_tcm_mockup_interrupts(
    void
  )
{
  return Soc_sand_tcm_mockup_interrupts;
}

SOC_SAND_RET
  soc_sand_general_set_tcm_mockup_interrupts(
    uint32 soc_tcmmockup_interrupts
  )
{
  Soc_sand_tcm_mockup_interrupts = soc_tcmmockup_interrupts;
  return SOC_SAND_OK;
}



#if SOC_SAND_DEBUG

void
  soc_sand_status_print(
    void
  )
{
  if (soc_sand_general_get_driver_is_started() == FALSE)
  {
    LOG_CLI((BSL_META("SOC_SAND driver is not running\n\r")));
    goto exit;
  }
  LOG_CLI((BSL_META("SOC_SAND driver is running\n\r")));
  
  soc_sand_chip_descriptors_print();
  
  soc_sand_tcm_general_status_print();
  
  LOG_CLI((BSL_META("\n\r")));
  soc_sand_physical_access_print();

exit:
  return;
}
#endif
