/* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $Copyright
 * $
*/
#ifndef SOC_SAND_CALLBACK_HANDLES_H
#define SOC_SAND_CALLBACK_HANDLES_H
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_delta_list.h>


typedef struct
{
  
  uint32 valid_word;
  
  uint32  int_or_poll;
  
  int  unit;
  
  uint32  proc_id;
  
  uint32 soc_sand_polling_handle;
  
  uint32 soc_sand_interrupt_handle;
} SOC_SAND_CALLBACK_HANDLE;

#define SOC_SAND_CALLBACK_HANDLE_VALID_WORD 0xBCBCBCBC


extern SOC_SAND_DELTA_LIST
    *Soc_sand_handles_list  ;


SOC_SAND_RET
  soc_sand_handles_init_handles(
    void
  );

SOC_SAND_RET
  soc_sand_handles_shut_down_handles(
    void
  );

SOC_SAND_RET
  soc_sand_handles_register_handle(
    uint32  int_or_poll,
    int  unit,
    uint32  proc_id,
    uint32 soc_sand_polling_handle,
    uint32 soc_sand_interrupt_handle,
    uint32 *public_handle
  );

SOC_SAND_RET
  soc_sand_handles_unregister_handle(
    uint32 fe_service_handle
  );

SOC_SAND_RET
  soc_sand_handles_unregister_all_device_handles(
    int unit
  );

SOC_SAND_CALLBACK_HANDLE
  *soc_sand_handles_search_next_handle(
    int        unit,
    uint32        proc_id,
    SOC_SAND_CALLBACK_HANDLE  *current
);

SOC_SAND_RET
  soc_sand_handles_is_handle_exist(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  proc_id,
    SOC_SAND_OUT uint32* callback_exist
  );


SOC_SAND_RET
  soc_sand_handles_delta_list_take_mutex(
    void
  );
SOC_SAND_RET
  soc_sand_handles_delta_list_give_mutex(
    void
  );
sal_thread_t
  soc_sand_handles_delta_list_get_owner(
    void
  );


#ifdef  __cplusplus
}
#endif
#endif
