/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/



#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_trace.h>

#include <soc/dpp/SAND/Management/sand_module_management.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_callback_handles.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>

#define SOC_SAND_MODULE_MANAGEMENT_PRINTFS 0


int Soc_sand_start_module_shut_down_mutex = FALSE ;


STATIC
  SOC_SAND_RET
    soc_sand_module_init_all(
      void
    )
{
  SOC_SAND_RET
    ex ;
  uint32
    err = 0 ;
  ex = soc_sand_trace_init() ;
  if (ex != SOC_SAND_OK)
  {
    err = 1 ;
    goto exit ;
  }
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_MODULE_INIT_ALL,
        "General error in soc_sand_module_init_all()",err,0,0,0,0,0) ;
  return ex ;
}

STATIC
  SOC_SAND_RET
    soc_sand_module_end_all(
      void
    )
{
  SOC_SAND_RET
    ex ;
  uint32
    err = 0 ;
  ex = soc_sand_trace_end() ;
  if (ex != SOC_SAND_OK)
  {
    err = 1 ;
    goto exit ;
  }
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_MODULE_END_ALL,
        "General error in soc_sand_module_end_all()",err,0,0,0,0,0) ;
  return ex ;
}

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
    )
{
  SOC_SAND_RET err;
  uint32 ex;
  SOC_SAND_INTERRUPT_INIT_DEFS;
  err = SOC_SAND_OK ;

  
  SOC_SAND_INTERRUPTS_STOP;
  {
    if (Soc_sand_start_module_shut_down_mutex)
    {
      err = SOC_SAND_DRIVER_BUSY ;
      goto exit_busy ;
    }
    Soc_sand_start_module_shut_down_mutex = TRUE ;
  }
  
  if (soc_sand_general_get_driver_is_started())
  {
    if (is_already_opened)
    {
      *is_already_opened = SOC_SAND_DRIVER_ALREADY_STARTED;
    }
    goto exit ;
  }
  else
  {
    if (is_already_opened)
    {
      *is_already_opened = SOC_SAND_OK ;
    }
  }
  
  if (soc_sand_add_sand_errors())
  {
    err = SOC_SAND_USER_ADD_SAND_ERRORS_ERR_01 ;
    goto exit ;
  }
  if (soc_sand_add_sand_procedures())
  {
    err = SOC_SAND_USER_ADD_SAND_PROCEDURES_ERR_01 ;
    goto exit ;
  }
  
  if (soc_sand_set_user_error_handler(error_handler, error_description) != SOC_SAND_OK)
  {
    err = SOC_SAND_USER_ERROR_HANDLER_ERR_01 ;
    goto exit ;
  }
  
  err =  soc_sand_module_end_all() ;
  if (err != SOC_SAND_OK)
  {
    err = SOC_SAND_MODULE_END_ALL_ERR_01 ;
    goto exit ;
  }
  
  err = soc_sand_module_init_all() ;
  if (err != SOC_SAND_OK)
  {
    err = SOC_SAND_MODULE_INIT_ALL_ERR_01 ;
    goto exit ;
  }
  
  if (soc_sand_general_set_max_num_devices(max_num_devices) != SOC_SAND_OK)
  {
    err = SOC_SAND_MAX_NUM_DEVICES_OUT_OF_RANGE_ERR ;
    goto exit ;
  }
  
  if (soc_sand_general_set_system_tick_in_ms(system_tick_in_ms) != SOC_SAND_OK)
  {
    err = SOC_SAND_SYSTEM_TICK_ERR_01 ;
    goto exit ;
  }
  
  if (soc_sand_general_set_tcm_task_priority(soc_tcmtask_priority) != SOC_SAND_OK)
  {
    err = SOC_SAND_TCM_TASK_PRIORITY_ERR_01 ;
    goto exit ;
  }
  
  if (soc_sand_general_set_min_time_between_tcm_activation(min_time_between_tcm_activation) != SOC_SAND_OK)
  {
    err = SOC_SAND_MIN_TIME_BETWEEN_POLLS_ERR_01 ;
    goto exit ;
  }
  
  if (soc_sand_general_set_tcm_mockup_interrupts(soc_tcmmockup_interrupts) != SOC_SAND_OK)
  {
    err = SOC_SAND_TCM_MOCKUP_INTERRUPTS_ERR_01;
    goto exit;
  }
  
  if (soc_sand_init_chip_descriptors(max_num_devices) != SOC_SAND_OK)
  {
    err = SOC_SAND_INIT_CHIP_DESCRIPTORS_ERR_01 ;
    goto exit ;
  }
  
  if (soc_sand_handles_init_handles() != SOC_SAND_OK)
  {
    err = SOC_SAND_HANDLES_INIT_ERR_01 ;
    goto exit ;
  }
  
  
  if (soc_sand_indirect_clear_info_all() != SOC_SAND_OK)
  {
    err = SOC_SAND_INDIRECT_SET_INFO_ERR_01 ;
    goto exit ;
  }
  
  if (soc_sand_mem_interrupt_mask_address_clear_all() != SOC_SAND_OK)
  {
    err = SOC_SAND_INTERRUPT_CLEAR_ALL_ERR_01 ;
    goto exit ;
  }
  
  soc_sand_general_set_driver_is_started(TRUE) ;
exit:
  Soc_sand_start_module_shut_down_mutex = FALSE ;
exit_busy:
  soc_sand_initialize_error_word(SOC_SAND_MODULE_OPEN,0,&ex) ;
  if ( err != SOC_SAND_OK )
  {
    soc_sand_module_end_all() ;
    soc_sand_set_error_code_into_error_word(err,&ex) ;
#if SOC_SAND_MODULE_MANAGEMENT_PRINTFS
    LOG_INFO(BSL_LS_SOC_MANAGEMENT,
             (BSL_META("soc_sand_module_open(): failure. ex = 0x%X \r\n"), (uint32)ex));
#endif
  }
  else
  {
#if SOC_SAND_MODULE_MANAGEMENT_PRINTFS
    LOG_INFO(BSL_LS_SOC_MANAGEMENT,
             (BSL_META("soc_sand_module_open(): success\r\n")));
#endif
  }
  SOC_SAND_ERROR_REPORT(err,NULL,0,0,SOC_SAND_MODULE_OPEN,
        "error in soc_sand_module_open(): Use error code",0,0,0,0,0,0) ;
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
  return (ex) ;
}

uint32
  soc_sand_module_close(
    void
  )
{
  SOC_SAND_RET err;
  uint32 ex;
  SOC_SAND_INTERRUPT_INIT_DEFS;

  err = SOC_SAND_OK ;

  
  SOC_SAND_INTERRUPTS_STOP;
  {
    if (Soc_sand_start_module_shut_down_mutex)
    {
      err = SOC_SAND_DRIVER_BUSY ;
      goto exit ;
    }
    Soc_sand_start_module_shut_down_mutex = TRUE ;
  }

  
  if (soc_sand_is_any_chip_descriptor_taken()) {
      goto exit;
  }

  
  if (!soc_sand_general_get_driver_is_started())
  {
    err = SOC_SAND_DRIVER_NOT_STARTED ;
    goto exit ;
  }
  
  soc_sand_general_set_driver_is_started(FALSE) ;

  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
  
  
  soc_sand_handles_shut_down_handles() ;
  
  soc_sand_delete_chip_descriptors() ;
  
  soc_sand_mem_interrupt_mask_address_clear_all() ;
  
  soc_sand_indirect_clear_info_all() ;
  
  soc_sand_set_user_error_handler((SOC_SAND_ERROR_HANDLER_PTR)0,(char *)0) ;
  
  soc_sand_close_all_error_pool() ;
  
  soc_sand_close_all_proc_id_pool() ;
  soc_sand_module_end_all() ;
exit:
  Soc_sand_start_module_shut_down_mutex = FALSE ;
  soc_sand_initialize_error_word(SOC_SAND_MODULE_CLOSE,0,&ex) ;
  if (err != SOC_SAND_OK)
  {
    soc_sand_set_error_code_into_error_word(err,&ex) ;
  }
  SOC_SAND_ERROR_REPORT(err,NULL,0,0,SOC_SAND_MODULE_CLOSE,
        "error in soc_sand_module_close(): Use error code",0,0,0,0,0,0) ;
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
  return (ex) ;
}

