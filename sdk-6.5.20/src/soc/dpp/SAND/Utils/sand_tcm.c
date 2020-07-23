/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_tcm.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_delta_list.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_trace.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>


#define DEBUG_TCM 0

#if SOC_SAND_DEBUG

#if DEBUG_TCM

  #include <stdio.h>
  typedef struct
  {
    uint32   time_signature ;
    uint32   item ;
    uint32   when ;
    uint32   func ;
    uint32   buffer ;
    uint32   size ;
    char            *str ;
  } SOC_TCM_DEBUG_STRUCT ;
  
  #define SOC_TCM_DEBUG_TABLE_SIZE  50
  SOC_TCM_DEBUG_STRUCT  Tcm_debug_table[SOC_TCM_DEBUG_TABLE_SIZE] ;
  uint32      Tcm_debug_table_index = 0 ;
  
  void soc_sand_tcm_debug_table_print(void)
  {
    uint32 jjj ;
    uint32 time_to_print ;
    
    LOG_CLI((BSL_META("                        TCM debug table          \r\n"
"--------------------------------------------------------------------------------\r\n")));
    for (jjj=0 ; jjj<Tcm_debug_table_index ; ++jjj)
    {
      if(jjj>0)
      {
        time_to_print =
          Tcm_debug_table[jjj].time_signature -
            Tcm_debug_table[jjj-1].time_signature;
      }
      else
      {
        time_to_print = Tcm_debug_table[jjj].time_signature;
      }
      LOG_CLI((BSL_META("%4d: %6s: time(%3u): %5lu, item: 0x%lu8, occur: %ld, func: 0x%lu8\r\n"),
jjj+1, Tcm_debug_table[jjj].str,
               (Tcm_debug_table[jjj].time_signature - Tcm_debug_table[jjj-1].time_signature),
               time_to_print, Tcm_debug_table[jjj].item,
               Tcm_debug_table[jjj].when, Tcm_debug_table[jjj].func ));
    }
    soc_sand_tcm_debug_table_clear() ;
  }

  void soc_sand_tcm_debug_table_clear(void)
  {
    Tcm_debug_table_index = 0 ;
  }

#else 


STATIC
  void
    soc_sand_tcm_debug_table(void)
{
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("define DEBUG_TCM to 1 in order to get tcm debuging\r\n")));
}

void
  soc_sand_tcm_debug_table_print(void)
{
  soc_sand_tcm_debug_table() ;
}
void soc_sand_tcm_debug_table_clear(void)
{
  soc_sand_tcm_debug_table() ;
}


#endif 

#endif 


static
  SOC_SAND_DELTA_LIST
    *Soc_sand_callback_delta_list   = NULL ;


static
  uint32
    Soc_sand_tcm_task_priority              = SOC_SAND_TCM_DEFAULT_TASK_PRIORITY ;

static
  uint32
    Soc_sand_tcm_task_stack_size            = SOC_SAND_TCM_DEFAULT_TASK_BYTE_SIZE ;

uint32 soc_sand_tcm_get_task_priority( void )
{
  return Soc_sand_tcm_task_priority ;
}

SOC_SAND_RET soc_sand_tcm_set_task_priority( uint32 soc_tcmtask_priority )
{
  Soc_sand_tcm_task_priority = soc_tcmtask_priority ;
  return SOC_SAND_OK ;
}

uint32 soc_sand_tcm_get_task_stack_size( void )
{
  return Soc_sand_tcm_task_stack_size ;
}

SOC_SAND_RET soc_sand_tcm_set_task_stack_size( uint32 soc_tcmtask_stack_size )
{
  Soc_sand_tcm_task_stack_size = soc_tcmtask_stack_size ;
  return SOC_SAND_OK ;
}

static
  void
    *Soc_sand_tcm_msg_queue = (void *)0 ;

SOC_SAND_RET
  soc_sand_tcm_send_message_to_q_from_task(
    uint32 msg
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_ERR ;
  uint32
    err = 0 ;
  if (!Soc_sand_tcm_msg_queue)
  {
    err = 1 ;
    goto exit ;
  }
  
  ex =
    soc_sand_os_msg_q_send(
      Soc_sand_tcm_msg_queue,(char *)&msg,sizeof(msg),
      SOC_TCM_MSG_Q_SEND_TIMEOUT,0) ;
  if (ex != SOC_SAND_OK)
  {
    err = 2 ;
    goto exit ;
  }
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_TCM_SEND_MESSAGE_TO_Q_FROM_TASK,
        "General error in soc_sand_tcm_send_message_to_q_from_task()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_tcm_send_message_to_q_from_int(
    uint32 msg
  )
{
  SOC_SAND_RET
    ex ;
  uint32
      err ;
#define DBG_MSG_Q_GUESS 0
#if DBG_MSG_Q_GUESS
  static uint32 Soc_sand_msg_q_debug_guess[2] = {0,0};
#endif
  ex = SOC_SAND_ERR ;
  err = 0 ;
  if (!Soc_sand_tcm_msg_queue)
  {
    err = 1 ;
    goto exit ;
  }
  
  ex =
    soc_sand_os_msg_q_send(
      Soc_sand_tcm_msg_queue,(char *)&msg,sizeof(msg),
      SOC_SAND_NO_TIMEOUT,0) ;
  if (ex != SOC_SAND_OK)
  {
    err = 2 ;
    goto exit ;
  }
exit:
#if DBG_MSG_Q_GUESS
  if(err) Msg_q_debug_guess[err-1]++;
#endif
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_TCM_SEND_MESSAGE_TO_Q_FROM_INT,
        "General error in soc_sand_tcm_send_message_to_q_from_int()",err,0,0,0,0,0) ;
  return ex ;
}

static sal_thread_t
    Soc_sand_tcm_started  = 0 ;


sal_thread_t
  soc_sand_tcm_get_is_started(
    void
  )
{
  return Soc_sand_tcm_started;
}

static uint32 Soc_sand_tcm_enable_polling_flag = FALSE;


SOC_SAND_RET
  soc_sand_tcm_set_enable_flag(
    uint32 enable_flag
  )
{
  Soc_sand_tcm_enable_polling_flag = enable_flag;
  return SOC_SAND_OK;
}

uint32
  soc_sand_tcm_get_enable_flag(
    void
  )
{
  return Soc_sand_tcm_enable_polling_flag;
}


static uint32
  Soc_sand_tcm_request_to_die_flag = FALSE;


SOC_SAND_RET
  soc_sand_tcm_set_request_to_die_flag(
    uint32 request_to_die_flag
  )
{
  Soc_sand_tcm_request_to_die_flag = request_to_die_flag;
  return SOC_SAND_OK;
}

uint32
  soc_sand_tcm_get_request_to_die_flag(
    void
  )
{
  return Soc_sand_tcm_request_to_die_flag;
}




SOC_SAND_RET
  soc_sand_tcm_callback_delta_list_take_mutex(
    void
  )
{
  return SOC_SAND_OK;
}

SOC_SAND_RET
  soc_sand_tcm_callback_delta_list_give_mutex(
    void
  )
{
  return SOC_SAND_OK;
}


STATIC
  int
    soc_sand_tcm_callback_engine_interrupt_serving(
      int unit
      )
{
  uint32             iii, ex = SOC_SAND_OK ;
  uint32             *tmp_bitstream, tmp_mask_bitstream[MAX_INTERRUPT_CAUSE / SOC_SAND_NOF_BITS_IN_UINT32];
  struct soc_sand_tcm_callback_str  *callback_array ;
  SOC_SAND_IS_DEVICE_INTERRUPTS_MASKED are_ints_masked;
  SOC_SAND_GET_DEVICE_INTERRUPTS_MASK  ints_mask_get;
  SOC_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR  is_bit_ac;
  SOC_SAND_UNMASK_FUNC_PTR             unmask;
  
  if ( !soc_sand_is_chip_descriptor_valid(unit) )
  {
    ex = 101 ;
    goto exit ;
  }
  
  if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_take_mutex())
  {
    ex = 102;
    goto exit ;
  }
  if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(unit) )
  {
    ex = 103;
    goto exit_semaphore_delta_list;
  }
  
  are_ints_masked = soc_sand_get_chip_descriptor_is_device_interrupts_masked_func(unit);
  if (are_ints_masked != NULL && !are_ints_masked(unit))
  {
    ex = 104;
    goto exit_semaphore_chip_desc_and_delta_list;
  }
  if (!soc_sand_device_is_between_isr_to_tcm(unit))
  {
    ex = 105;
    goto exit_semaphore_chip_desc_and_delta_list;
  }
  
  tmp_bitstream = soc_sand_get_chip_descriptor_interrupt_bitstream(unit);
  ints_mask_get = soc_sand_chip_descriptor_get_interrupts_mask_func(unit);
  if (ints_mask_get != NULL)
  {
    ints_mask_get(unit, tmp_mask_bitstream);
  }

  
  callback_array = soc_sand_get_chip_descriptor_interrupt_callback_array(unit) ;
  if( !callback_array || !tmp_bitstream)
  {
    ex = 106 ;
    goto exit_semaphore_chip_desc_and_delta_list;
  }
  
  for (iii=0 ; iii<MAX_INTERRUPT_CAUSE ; ++iii)
  {
    
    if ( soc_sand_bitstream_test_bit(tmp_mask_bitstream, iii) &&
         soc_sand_bitstream_test_bit(tmp_bitstream, iii)
       )
    {
      
      if ( callback_array[iii].func )
      {
        callback_array[iii].func(
          callback_array[iii].buffer,
          callback_array[iii].size
          ) ;
      }
      
      soc_sand_bitstream_reset_bit(tmp_bitstream, iii) ;
    }
    
    is_bit_ac = soc_sand_get_chip_descriptor_is_bit_auto_clear_func(unit);
    if (is_bit_ac != NULL && !is_bit_ac(iii))
    {
      soc_sand_bitstream_reset_bit(tmp_bitstream, iii) ;
    }

    
   }
  
  unmask = soc_sand_get_chip_descriptor_unmask_func(unit);
  if (unmask != NULL && SOC_SAND_OK != unmask(unit, TRUE))
  {
    ex = 107 ;
    goto exit_semaphore_chip_desc_and_delta_list ;
  }
exit_semaphore_chip_desc_and_delta_list:
  
  if ( SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit) )
  {
    ex = 108 ;
    goto exit ;
  }
exit_semaphore_delta_list:
  if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_give_mutex() )
  {
    ex = 109 ;
    goto exit ;
  }
  
exit:
  return ex ;
}


STATIC
  SOC_SAND_RET
    soc_sand_tcm_callback_engine_main(
      void
    )
{
  SOC_SAND_CALLBACK_LIST_ITEM *item ;
  uint32
      msg,
      sleep_period,
      ticks_before_msgq,
      ticks_before,
      ticks_after,
      ii,
      func_res,
      milisec_per_ticks_round_up;
  int
    err,
    recv_reason,
    num_msgs ;
  uint32
    delta_ticks ;
  int delta_ticks_positive;
  uint32 min_time_between_handling;
  min_time_between_handling = 0;
  err = 0 ;
  func_res = SOC_SAND_ERR ;

  soc_sand_tcm_set_request_to_die_flag(FALSE);

  milisec_per_ticks_round_up =
    SOC_SAND_DIV_ROUND_UP(1000,soc_sand_os_get_ticks_per_sec());
  
  if (NULL == Soc_sand_callback_delta_list)
  {
    err = 1 ;
    goto exit ;
  }
  
  if (0 == Soc_sand_callback_delta_list->mutex_id)
  {
    err = 2 ;
    goto exit ;
  }
  
  ii = 0 ;
  while (0 == Soc_sand_tcm_started)
  {
    soc_sand_os_task_delay_milisec(milisec_per_ticks_round_up);
    if (ii++ > soc_sand_os_sys_clk_rate_get()  )
    {
      err = 3 ;
      goto exit ;
    }
  }
  
  while(TRUE)
  {
    if(soc_sand_tcm_get_request_to_die_flag())
    {
      
      soc_sand_tcm_set_enable_flag(FALSE);
      goto exit;
    }

    while( !soc_sand_tcm_get_enable_flag() )
    {
      if(soc_sand_tcm_get_request_to_die_flag())
      {
        
        soc_sand_tcm_set_enable_flag(FALSE);
        goto exit;
      }
     
      soc_sand_os_task_delay_milisec( milisec_per_ticks_round_up * soc_sand_general_get_min_time_between_tcm_activation() );
    }
    
    num_msgs = soc_sand_os_msg_q_num_msgs(Soc_sand_tcm_msg_queue);
    
    if( -1 == num_msgs || SOC_SAND_TCM_MSG_QUEUE_NUM_MSGS == num_msgs )
    {
      err = 4 ;
      goto exit ;
    }
    if (NULL == Soc_sand_callback_delta_list->head)
    {
      
      sleep_period = (uint32)soc_sand_general_get_min_time_between_tcm_activation() ;
    }
    else
    {
      sleep_period = soc_sand_delta_list_get_head_time(Soc_sand_callback_delta_list) ;
    }
    
    soc_sand_os_tick_get(&ticks_before_msgq);
    
    soc_sand_os_task_delay_milisec(milisec_per_ticks_round_up * min_time_between_handling);
    min_time_between_handling = 0;
    
    recv_reason  =
      soc_sand_os_msg_q_recv(
        Soc_sand_tcm_msg_queue,(unsigned char *)&msg,
        sizeof(msg),(long)sleep_period) ;
    
    soc_sand_os_tick_get(&ticks_before) ;
    
    if (0 > recv_reason)
    {
      err = 5 ;
      goto exit ;
    }
    if(0 == recv_reason)
    {
    
      if (SOC_SAND_MAX_DEVICE > msg)
      {
        if (SOC_SAND_OK != (err = soc_sand_tcm_callback_engine_interrupt_serving(msg)) )
        {
          goto exit;
        }
        min_time_between_handling = soc_sand_general_get_min_time_between_tcm_activation();
      }
      else if (soc_sand_is_errors_queue_msg(msg))
      {
        if (SOC_SAND_OK != soc_sand_unload_errors_queue())
        {
          err = 10;
          goto exit;
        }
      }
      
      soc_sand_os_tick_get(&ticks_after) ;
      
      if (SOC_SAND_INFINITE_TIMEOUT != sleep_period)
      {
        
        delta_ticks = ticks_after - ticks_before_msgq ;
        delta_ticks_positive =  ticks_after > ticks_before_msgq ;
      }
      else
      {
        
        delta_ticks =  ticks_after - ticks_before ;
        delta_ticks_positive = ticks_after > ticks_before ;
      }
      
      if (delta_ticks_positive)
      {
        if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_take_mutex() )
        {
          err = 11 ;
          goto exit ;
        }
        else
        {
          if (SOC_SAND_NEW_DELTA_MESSAGE == msg)
          {
            soc_sand_delta_list_decrease_time_from_second_item(
                Soc_sand_callback_delta_list,
                delta_ticks
            );
          }
          else
          {
            soc_sand_delta_list_decrease_time_from_head(
                Soc_sand_callback_delta_list,
                delta_ticks
            ) ;
          }
        }
        if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_give_mutex() )
        {
          err = 12 ;
          goto exit ;
        }
      }
    }
    
    if (0 < recv_reason || 0 == sleep_period)
    {
      if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_take_mutex() )
      {
        err = 6 ;
        goto exit ;
      }
      
      item = (SOC_SAND_CALLBACK_LIST_ITEM *) soc_sand_delta_list_pop_d (Soc_sand_callback_delta_list) ;
      
      if (NULL == item)
      {
        
        if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_give_mutex() )
        {
          err = 7 ;
          goto exit ;
        }
        continue ;
      }
      
      if (CALLBACK_ITEM_VALID_WORD != item->valid_word)
      {
        
        err = 8 ;
        goto exit ;
      }
      
      if (NULL != item->func)
      {
        
#if DEBUG_TCM
  if(SOC_TCM_DEBUG_TABLE_SIZE > Tcm_debug_table_index)
  {
    Tcm_debug_table[Tcm_debug_table_index].time_signature = soc_sand_os_get_time_micro() ;
    Tcm_debug_table[Tcm_debug_table_index].item   = (uint32)item ;
    Tcm_debug_table[Tcm_debug_table_index].func   = (uint32)item->func ;
    Tcm_debug_table[Tcm_debug_table_index].when   = item->when ;
    Tcm_debug_table[Tcm_debug_table_index].str    = "before" ;
    Tcm_debug_table_index++ ;
  }
#endif
        soc_sand_trace_add_entry(PTR_TO_INT(item->func), "A") ;
        func_res = item->func( item->buffer, item->size ) ;
        soc_sand_trace_add_entry(PTR_TO_INT(item->func), "B") ;
#if DEBUG_TCM
  if(SOC_TCM_DEBUG_TABLE_SIZE > Tcm_debug_table_index)
  {
    Tcm_debug_table[Tcm_debug_table_index].time_signature = soc_sand_os_get_time_micro() ;
    Tcm_debug_table[Tcm_debug_table_index].item   = (uint32)item ;
    Tcm_debug_table[Tcm_debug_table_index].func   = (uint32)item->func ;
    Tcm_debug_table[Tcm_debug_table_index].when   = item->when ;
    Tcm_debug_table[Tcm_debug_table_index].str    = "after " ;
    Tcm_debug_table_index++ ;
  }
#endif
        
        soc_sand_os_tick_get(&ticks_after) ;
        
        delta_ticks =  ticks_after - ticks_before ;
        if (ticks_after > ticks_before)
        {
            soc_sand_delta_list_decrease_time_from_head(
              Soc_sand_callback_delta_list,
              delta_ticks
            );
        }
      }
      min_time_between_handling = soc_sand_general_get_min_time_between_tcm_activation();
      
      if( (SOC_SAND_POLLING != item->recurr) ||
          (SOC_SAND_OK != soc_sand_get_error_code_from_error_word(func_res) ) ||
          (item->marked_for_removal)
        )
      {
        
        item->valid_word = 0 ; 
        soc_sand_os_free(item->buffer);
        soc_sand_os_free(item);
        
        if(SOC_SAND_OK != soc_sand_get_error_code_from_error_word(func_res) )
        { 
          uint32 err_id;
          soc_sand_initialize_error_word(SOC_SAND_TCM_CALLBACK_ENGINE_MAIN,0,&err_id);
          soc_sand_set_error_code_into_error_word(SOC_SAND_TCM_MAIN_ERR_02,&err_id);
          soc_sand_set_severity_into_error_word(SOC_SAND_SVR_FTL,&err_id) ;
          soc_sand_invoke_user_error_handler(
            err_id, "Callback method retuned an error, so it is removed from TCM",
            PTR_TO_INT(&item), func_res, 0,0,0,0
          );
        }
        else
        { 
          uint32 err_id;
          soc_sand_initialize_error_word(SOC_SAND_TCM_CALLBACK_ENGINE_MAIN,0,&err_id);
          soc_sand_set_error_code_into_error_word(SOC_SAND_TCM_MAIN_ERR_03,&err_id);
          soc_sand_set_severity_into_error_word(SOC_SAND_SVR_MSG,&err_id);
          soc_sand_invoke_user_error_handler(
            err_id, "Callback method is removed from TCM (due to aging, or specific user request)",
            PTR_TO_INT(&item), func_res, 0,0,0,0
          );
        }
      }
      else
      {
        
        if (SOC_SAND_INFINITE_POLLING > item->times)
        {
          item->times-- ;
        }
        if (0 < item->times)
        {
          soc_sand_delta_list_insert_d(Soc_sand_callback_delta_list, item->when, (void *)item) ;
        }
        else 
        {
          item->valid_word = 0 ; 
          soc_sand_os_free(item->buffer) ;
          soc_sand_os_free(item) ;
        }
      }
      if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_give_mutex() )
      {
        err = 9 ;
        goto exit ;
      }
    }
  }
  
exit:
  if (err)
  {
    
    uint32
        err_id ;
    soc_sand_initialize_error_word(SOC_SAND_TCM_CALLBACK_ENGINE_MAIN,0,&err_id) ;
    soc_sand_set_error_code_into_error_word(SOC_SAND_TCM_MAIN_ERR_01,&err_id) ;
    soc_sand_set_severity_into_error_word(SOC_SAND_SVR_FTL,&err_id) ;
    soc_sand_invoke_user_error_handler(
          err_id,
          "Error reported in TCM task (see first parameter). No recovery path. Quit.",
          (uint32)err,
          0,0,0,0,0
          ) ;
  }
  Soc_sand_tcm_started = FALSE;
  return SOC_SAND_ERR ;
}


SOC_SAND_RET
  soc_sand_tcm_callback_engine_start(
    void
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_ERR ;
  unsigned
    err = 0 ;

  
  Soc_sand_callback_delta_list = soc_sand_delta_list_create() ;
  if (!Soc_sand_callback_delta_list)
  {
    err = 1 ;
    goto exit ;
  }
  
  Soc_sand_tcm_msg_queue =
    soc_sand_os_msg_q_create(SOC_SAND_TCM_MSG_QUEUE_NUM_MSGS,SOC_TCM_MSG_QUEUE_SIZE_MSG) ;
  if (!Soc_sand_tcm_msg_queue)
  {
    soc_sand_delta_list_destroy(Soc_sand_callback_delta_list) ;
    err = 2 ;
    goto exit ;
  }
  
  Soc_sand_tcm_started = soc_sand_os_task_spawn(
              "dTcmTask",
              (int)soc_sand_tcm_get_task_priority(),
              0,
              (int)soc_sand_tcm_get_task_stack_size(),
              (SOC_SAND_FUNC_PTR) soc_sand_tcm_callback_engine_main,
              0,0,0,0,0,0,0,0,0,0
            ) ;
  
  if (!Soc_sand_tcm_started)
  {
    soc_sand_os_msg_q_delete(Soc_sand_tcm_msg_queue) ;
    soc_sand_delta_list_destroy(Soc_sand_callback_delta_list) ;
    err = 3 ;
    goto exit ;
  }
  soc_sand_tcm_set_enable_flag(TRUE);
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_TCM_CALLBACK_ENGINE_START,
        "General error in soc_sand_tcm_callback_engine_start()",err,0,0,0,0,0) ;
  return ex ;
}


SOC_SAND_RET
  soc_sand_tcm_callback_engine_stop(
    void
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  SOC_SAND_CALLBACK_LIST_ITEM
    *item;
  uint32
    milisec_per_ticks_round_up = 0;
  uint32
    ex,
    no_err;

  soc_sand_initialize_error_word(0, 0, &ex);
  no_err = ex;

  

  milisec_per_ticks_round_up =
    SOC_SAND_DIV_ROUND_UP(1000,soc_sand_os_get_ticks_per_sec());

  if (!Soc_sand_tcm_started)
  {
    goto exit ;
  }
  
  ret = soc_sand_tcm_callback_delta_list_take_mutex(
        );
  soc_sand_set_error_code_into_error_word(
    soc_sand_get_error_code_from_error_word(ret),
    &ex
  );
  if(ex != no_err)
  {
    goto exit;
  }

  while( NULL != (item = (SOC_SAND_CALLBACK_LIST_ITEM *) soc_sand_delta_list_pop_d (Soc_sand_callback_delta_list)) )
  {
    item->valid_word = 0 ;
    if(item->buffer)
    {
      soc_sand_os_free(item->buffer) ;
    }
    soc_sand_os_free(item) ;
  }
  
  ret = soc_sand_tcm_set_request_to_die_flag(
          TRUE
        );
  soc_sand_set_error_code_into_error_word(
    soc_sand_get_error_code_from_error_word(ret),
    &ex
  );
  if(ex != no_err)
  {
    goto exit_semaphore;
  }

  soc_sand_os_task_delay_milisec(
    2*milisec_per_ticks_round_up * soc_sand_general_get_min_time_between_tcm_activation()
  );

  if (Soc_sand_tcm_started)
  {
   
    ret = soc_sand_os_task_delete(
            Soc_sand_tcm_started
          ) ;
    soc_sand_set_error_code_into_error_word(
      soc_sand_get_error_code_from_error_word(ret),
      &ex
    );
    if(ex != no_err)
    {
      goto exit_semaphore;
    }
  }

  if (Soc_sand_tcm_msg_queue)
  {
    soc_sand_os_msg_q_delete(Soc_sand_tcm_msg_queue) ;
  }

  
  ret = soc_sand_delta_list_destroy(
          Soc_sand_callback_delta_list
        ) ;
  soc_sand_set_error_code_into_error_word(
    soc_sand_get_error_code_from_error_word(ret),
    &ex
  );
  if(ex != no_err)
  {
    goto exit_semaphore;
  }

  goto exit;

exit_semaphore:
  soc_sand_tcm_callback_delta_list_give_mutex();
exit:
  return ret;
}

SOC_SAND_RET
  soc_sand_tcm_register_polling_callback(
    SOC_SAND_IN     SOC_SAND_TCM_CALLBACK func,
    SOC_SAND_INOUT  uint32           *buffer,
    SOC_SAND_IN     uint32           size,
    SOC_SAND_IN     SOC_SAND_RECURRENCE         poll,
    SOC_SAND_IN     uint32           poll_interval,
    SOC_SAND_IN     uint32           poll_times,
    SOC_SAND_INOUT  uint32           *handle
  )
{
  SOC_SAND_CALLBACK_LIST_ITEM
    *item ;
  int
    insert_at_head_flag ;
  SOC_SAND_RET
    ex = SOC_SAND_ERR ;
  uint32
    err = 0 ;
  if (!Soc_sand_callback_delta_list || !handle)
  {
    err = 1 ;
    goto exit ;
  }
  item = soc_sand_os_malloc(sizeof(SOC_SAND_CALLBACK_LIST_ITEM), "item soc_tcmregister_polling_callback") ;
  if (NULL == item)
  {
    err = 2 ;
    goto exit ;
  }
  item->recurr   = poll ;
  item->times    = poll_times ;
  item->when     = poll_interval ;
  item->func     = func ;
  item->buffer   = (uint32 *)buffer ;
  item->size     = size ;
  
  item->marked_for_removal  = FALSE ;
  
  item->valid_word = CALLBACK_ITEM_VALID_WORD ;
  
  if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_take_mutex() )
  {
    err = 3 ;
    goto exit ;
  }
  
  insert_at_head_flag = FALSE ;
  if ( (soc_sand_delta_list_is_empty(Soc_sand_callback_delta_list)) ||
       (soc_sand_delta_list_get_head_time(Soc_sand_callback_delta_list) > poll_interval)
     )
  {
    insert_at_head_flag = TRUE;
  }
  
  if (SOC_SAND_OK != (ex = soc_sand_delta_list_insert_d(Soc_sand_callback_delta_list, item->when, (void *)item)))
  {
    err = 4 ;
    goto exit_sempahore ;
  }
  
  if (insert_at_head_flag)
  {
    insert_at_head_flag = FALSE;
    ex = soc_sand_tcm_send_message_to_q_from_task((uint32)(SOC_SAND_NEW_DELTA_MESSAGE)) ;
    if (SOC_SAND_OK != ex)
    {
      err = 5 ;
      goto exit_sempahore ;
    }
  }
  
  *handle = PTR_TO_INT(item);
  ex = SOC_SAND_OK ;

exit_sempahore:
  if (SOC_SAND_OK != (ex = soc_sand_tcm_callback_delta_list_give_mutex()))
  {
    err = 5 ;
    goto exit ;
  }
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_TCM_REGISTER_POLLING_CALLBACK,
        "General error in soc_sand_tcm_register_polling_callback()",err,0,0,0,0,0) ;
  return ex ;
}


SOC_SAND_RET
  soc_sand_tcm_unregister_polling_callback(
    SOC_SAND_IN     uint32            handle
  )
{
  SOC_SAND_CALLBACK_LIST_ITEM
    *item ;
  uint32
      *local_buf_ptr ;
  SOC_SAND_RET
    ex ;
  uint32
      err ;
  err = 0 ;
  item = (SOC_SAND_CALLBACK_LIST_ITEM *)INT_TO_PTR(handle);
  ex = SOC_SAND_ERR ;
  
  if (!Soc_sand_callback_delta_list || !item)
  {
    err = 1 ;
    goto exit ;
  }
 
  if (CALLBACK_ITEM_VALID_WORD != item->valid_word)
  {
    
    err = 2 ;
    goto exit ;
  }
  
  if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_take_mutex() )
  {
    
    err = 3 ;
    goto exit ;
  }
  
  local_buf_ptr    = item->buffer ;
  
  item->valid_word = 0 ;
  if ( SOC_SAND_OK != (ex = soc_sand_delta_list_remove(Soc_sand_callback_delta_list, (void *)item)))
  {
    
    err = 5 ;
    goto exit_sempahore ;
  }
  if (SOC_SAND_OK != (ex = soc_sand_os_free(item)))
  {
    err = 6 ;
    goto exit_sempahore ;
  }
  
  if (local_buf_ptr)
  {
    if(SOC_SAND_OK != (ex = soc_sand_os_free(local_buf_ptr)))
    {
      err = 7 ;
      goto exit_sempahore ;
    }
  }
  ex = SOC_SAND_OK ;

exit_sempahore:
  if (SOC_SAND_OK != (ex = soc_sand_tcm_callback_delta_list_give_mutex()))
  {
    err = 8 ;
    goto exit ;
  }

exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_TCM_UNREGISTER_POLLING_CALLBACK,
        "General error in SOC_SAND_TCM_UNREGISTER_POLLING_CALLBACK()",err,0,0,0,0,0);
  if(SOC_SAND_OK == ex)
  {
    uint32 err_id;
    soc_sand_initialize_error_word(SOC_SAND_TCM_UNREGISTER_POLLING_CALLBACK,0,&err_id);
    soc_sand_set_error_code_into_error_word(SOC_SAND_TCM_MAIN_ERR_03,&err_id);
    soc_sand_set_severity_into_error_word(SOC_SAND_SVR_MSG,&err_id);
    soc_sand_invoke_user_error_handler(
      err_id, "Callback method was removed from TCM (due to specific user request)",
      handle, 0,0,0,0,0
    );
  }
  return ex ;
}

#if SOC_SAND_DEBUG


void
  soc_sand_tcm_print_delta_list(
    void
  )
{
  LOG_CLI((BSL_META("Soc_sand_callback_delta_list:\r\n")));
  soc_sand_delta_list_print(Soc_sand_callback_delta_list) ;
}


void
  soc_sand_tcm_general_status_print(
    void
  )
{
  if (Soc_sand_callback_delta_list == NULL)
  {
    LOG_CLI((BSL_META("soc_sand_tcm_general_status_print(): do not have internal params (probably the driver was not started)\n\r")));
    goto exit;
  }
  LOG_CLI((BSL_META("TCM is %sstarted and polling is %senabled \r\n"),
((Soc_sand_tcm_started) ? "" : "not "),
           ((soc_sand_tcm_get_enable_flag()) ? "" : "not ")
           ));
  LOG_CLI((BSL_META("TCM info --  Delta List:\n\r")));
  soc_sand_delta_list_print_DELTA_LIST(Soc_sand_callback_delta_list);
  LOG_CLI((BSL_META("\n\r")));

exit:
  return;
}

#endif 
