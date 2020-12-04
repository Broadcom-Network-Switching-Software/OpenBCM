/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_SAND_CALLBACKS_H
#define SOC_SAND_CALLBACKS_H
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_delta_list.h>


#define SOC_SAND_TCM_DEFAULT_TASK_BYTE_SIZE   50*1024

#define SOC_SAND_TCM_DEFAULT_TASK_PRIORITY    70

#define SOC_SAND_TCM_MSG_QUEUE_NUM_MSGS   (2 * (SOC_SAND_MAX_DEVICE + SOC_SAND_MAX_DEVICE + 1))

#define SOC_TCM_MSG_Q_SEND_TIMEOUT         20

#define SOC_TCM_MSG_QUEUE_SIZE_MSG  sizeof(uint32)

#define SOC_SAND_NEW_DELTA_MESSAGE             (SOC_SAND_MAX_DEVICE+1)
#define SOC_SAND_NEW_ERROR_MESSAGE             (SOC_SAND_MAX_DEVICE+2)
typedef
  uint32 (* SOC_SAND_TCM_CALLBACK)(
               uint32 *buffer,
               uint32 size
  ) ;

typedef struct soc_sand_tcm_callback_str
{
  SOC_SAND_TCM_CALLBACK   func ;
  uint32       *buffer ;
  uint32       size ;
} soc_sand_tcm_callback_str_t;

typedef enum
{
  SOC_SAND_ONCE,
  SOC_SAND_POLLING
} SOC_SAND_RECURRENCE ;

#define SOC_SAND_INFINITE_POLLING     0xffffffff

#define CALLBACK_ITEM_VALID_WORD  0xA5A5A5A5

typedef struct
{
  uint32     valid_word ;
  uint32     when ;
  SOC_SAND_TCM_CALLBACK func ;
  uint32     *buffer ;
  uint32     size ;
  SOC_SAND_RECURRENCE   recurr ;
  uint32     times ;
  uint32      marked_for_removal ;
} SOC_SAND_CALLBACK_LIST_ITEM ;

uint32
  soc_sand_tcm_get_task_priority(
    void
    ) ;


sal_thread_t
  soc_sand_tcm_get_is_started(
  void
  );

SOC_SAND_RET
  soc_sand_tcm_set_task_priority(
    uint32 soc_tcmtask_priority
  );

uint32
  soc_sand_tcm_get_task_stack_size(
    void
  ) ;

SOC_SAND_RET
  soc_sand_tcm_set_task_stack_size(
    uint32 soc_tcmtask_stack_size
  ) ;

SOC_SAND_RET
  soc_sand_tcm_send_message_to_q_from_task(
    uint32 msg
  ) ;

SOC_SAND_RET
  soc_sand_tcm_send_message_to_q_from_int(
    uint32 msg
  );

SOC_SAND_RET
  soc_sand_tcm_callback_engine_start(
    void
  ) ;

SOC_SAND_RET
  soc_sand_tcm_callback_engine_stop(
    void
  ) ;

SOC_SAND_RET
  soc_sand_tcm_set_enable_flag(
    uint32 enable_flag
  );
uint32
  soc_sand_tcm_get_enable_flag(
    void
  );


SOC_SAND_RET
  soc_sand_tcm_set_request_to_die_flag(
    uint32 request_to_die_flag
  );
uint32
  soc_sand_tcm_get_request_to_die_flag(
    void
  );

SOC_SAND_RET
  soc_sand_tcm_register_polling_callback(
    SOC_SAND_IN     SOC_SAND_TCM_CALLBACK      func,  
    SOC_SAND_INOUT  uint32      *buffer,
    SOC_SAND_IN     uint32      size,  
    SOC_SAND_IN     SOC_SAND_RECURRENCE      poll,  
    SOC_SAND_IN     uint32      poll_interval, 
    SOC_SAND_IN     uint32      poll_times,  
    SOC_SAND_INOUT  uint32           *handle
  ) ;

SOC_SAND_RET
  soc_sand_tcm_unregister_polling_callback(
    SOC_SAND_IN     uint32            handle
  ) ;


SOC_SAND_RET
  soc_sand_tcm_callback_delta_list_take_mutex(
    void
  );
SOC_SAND_RET
  soc_sand_tcm_callback_delta_list_give_mutex(
    void
  );

#if SOC_SAND_DEBUG
void soc_sand_tcm_debug_table_print(void) ;
void soc_sand_tcm_debug_table_clear(void) ;

void
  soc_sand_tcm_print_delta_list(
    void
  ) ;

void
  soc_sand_tcm_general_status_print(
    void
  );
#endif 

#ifdef  __cplusplus
}
#endif

#endif
