/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_delta_list.h>
#include <soc/dpp/SAND/Utils/sand_tcm.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_callback_handles.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>


  SOC_SAND_DELTA_LIST
    *Soc_sand_handles_list  = NULL ;

SOC_SAND_DELTA_LIST
  *soc_sand_handles_get_handles_list(
    void
  )
{
  return Soc_sand_handles_list ;
}

SOC_SAND_RET
  soc_sand_handles_init_handles(
    void
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK ;
  
  Soc_sand_handles_list = soc_sand_delta_list_create() ;
  if (!Soc_sand_handles_list)
  {
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_HANDLES_INIT_HANDLES,
        "General error in soc_sand_handles_init_handles()",0,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_handles_shut_down_handles(
    void
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK ;
  
  if (SOC_SAND_OK != soc_sand_delta_list_destroy(Soc_sand_handles_list) )
  {
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_HANDLES_SHUT_DOWN_HANDLES,
        "General error in soc_sand_handles_shut_down_handles()",0,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_handles_register_handle(
    uint32  int_or_poll,
    int  unit,
    uint32  proc_id,
    uint32 soc_sand_polling_handle,
    uint32 soc_sand_interrupt_handle,
    uint32 *public_handle
  )
{
  SOC_SAND_CALLBACK_HANDLE
    *item ;
  uint32
      err ;
  SOC_SAND_RET
    ex ;
  
  item = NULL ;
  ex   = SOC_SAND_ERR ;
  err = 0 ;
  
  if (!Soc_sand_handles_list)
  {
    err = 1 ;
    goto exit ;
  }
  
  item = soc_sand_os_malloc( sizeof(SOC_SAND_CALLBACK_HANDLE) , "item handles_register_handle") ;
  if (!item)
  {
    err = 2 ;
    goto exit ;
  }
  item->int_or_poll           = int_or_poll ;
  item->unit             = unit ;
  item->proc_id               = proc_id ;
  item->soc_sand_polling_handle   = soc_sand_polling_handle ;
  item->soc_sand_interrupt_handle = soc_sand_interrupt_handle ;
  item->valid_word            = SOC_SAND_CALLBACK_HANDLE_VALID_WORD ;
  
  if (SOC_SAND_OK != soc_sand_handles_delta_list_take_mutex())
  {
    err = 3 ;
    soc_sand_os_free(item) ;
    goto exit ;
  }
  else 
  {
    if (SOC_SAND_OK != soc_sand_delta_list_insert_d(Soc_sand_handles_list, 0, (void *)item))
    {
      err = 4 ;
      soc_sand_os_free(item) ;
      goto exit ;
    }
  }
  if (SOC_SAND_OK != soc_sand_handles_delta_list_give_mutex())
  {
    err = 5 ;
    goto exit ;
  }
  
  *public_handle = PTR_TO_INT(item);
  ex = SOC_SAND_OK ;
  
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_HANDLES_REGISTER_HANDLE,
        "General error in soc_sand_handles_register_handle()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_handles_unregister_handle(
    uint32 soc_sand_service_handle
  )
{
  SOC_SAND_CALLBACK_HANDLE
    *item ;
  SOC_SAND_RET
    ex ;
  uint32
      err ;
  ex = SOC_SAND_OK ;
  err = 0 ;
  
  if (soc_sand_delta_list_is_empty(Soc_sand_handles_list))
  {
    err = 1 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  item  = (SOC_SAND_CALLBACK_HANDLE *)INT_TO_PTR(soc_sand_service_handle);
  
  if(!item || SOC_SAND_CALLBACK_HANDLE_VALID_WORD != item->valid_word)
  {
    err = 2 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  
  switch (item->int_or_poll)
  {
    case 1:
    {
      if (SOC_SAND_OK != soc_sand_tcm_unregister_polling_callback(item->soc_sand_polling_handle))
      {
        err = 3 ;
        ex = SOC_SAND_ERR ;
        goto exit ;
      }
      break ;
    }
    case 2:
    {
      if (SOC_SAND_OK != soc_sand_unregister_event_callback(item->soc_sand_interrupt_handle))
      {
        err = 4 ;
        ex = SOC_SAND_ERR ;
        goto exit ;
      }
      break ;
    }
    case 3:
    {
      if (SOC_SAND_OK != soc_sand_tcm_unregister_polling_callback(item->soc_sand_polling_handle))
      {
        err = 5 ;
        ex = SOC_SAND_ERR ;
      }
      if (SOC_SAND_OK != soc_sand_unregister_event_callback(item->soc_sand_interrupt_handle))
      {
        err = 6 ;
        ex = SOC_SAND_ERR ;
      }
      if (SOC_SAND_ERR == ex)
      {
        goto exit ;
      }
      break ;
    }
    default:
    {
      err = 7 ;
      ex = SOC_SAND_ERR ;
      goto exit ;
    }
  }
  
  if (SOC_SAND_OK != soc_sand_handles_delta_list_take_mutex())
  {
    err = 8 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  
  item->valid_word = 0 ;
  if (SOC_SAND_OK != soc_sand_delta_list_remove(Soc_sand_handles_list, (void *)item))
  {
    err = 9 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  
  if (SOC_SAND_OK != soc_sand_handles_delta_list_give_mutex())
  {
    err = 10 ;
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_HANDLES_UNREGISTER_HANDLE,
        "General error in soc_sand_handles_unregister_handle()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_handles_unregister_all_device_handles(
    int unit
  )
{
  SOC_SAND_CALLBACK_HANDLE
    *curr_item ;
  SOC_SAND_DELTA_LIST_NODE
    *curr_node,
    *del_node,
    *prev_node ;
  SOC_SAND_RET
    soc_sand_ret,
    ex ;
  uint32
      err ;
  
  curr_item    = NULL ;
  curr_node    = NULL ;
  del_node     = NULL ;
  prev_node    = NULL ;
  ex = SOC_SAND_ERR ;
  err = 0 ;
  if (soc_sand_delta_list_is_empty(Soc_sand_handles_list))
  {
    ex = SOC_SAND_OK ;  
    goto exit ;
  }
  
  if (SOC_SAND_OK != soc_sand_handles_delta_list_take_mutex())
  {
    err = 1 ;
    goto exit ;
  }
  
  curr_node = Soc_sand_handles_list->head ;
  while(NULL != curr_node)
  {
    
    curr_item = (SOC_SAND_CALLBACK_HANDLE *)curr_node->data ;
    
    if (unit == curr_item->unit)
    {
      
      del_node  = curr_node ;
      curr_node = curr_node->next ;
      soc_sand_os_free(del_node) ;  
      if (prev_node)
      {
        
        prev_node->next = curr_node ;
      }
      else
      {
        
        Soc_sand_handles_list->head = curr_node ;
      }
      
      switch (curr_item->int_or_poll)
      {
        case 1:
        {
          soc_sand_ret = soc_sand_tcm_unregister_polling_callback(curr_item->soc_sand_polling_handle) ;
          if (SOC_SAND_OK != soc_sand_ret)
          {
            err = 2 ;
            goto exit_semaphore ;
          }
          break ;
        }
        case 2:
        {
          soc_sand_ret = soc_sand_unregister_event_callback(curr_item->soc_sand_interrupt_handle) ;
          if (SOC_SAND_OK != soc_sand_ret)
          {
            err = 3 ;
            goto exit_semaphore ;
          }
          break ;
        }
        case 3:
        {
          soc_sand_ret = soc_sand_tcm_unregister_polling_callback(curr_item->soc_sand_polling_handle) ;
          if (SOC_SAND_OK != soc_sand_ret)
          {
            err = 4 ;
            goto exit_semaphore ;
          }
          soc_sand_ret = soc_sand_unregister_event_callback(curr_item->soc_sand_interrupt_handle) ;
          if (SOC_SAND_OK != soc_sand_ret)
          {
            err = 5 ;
            goto exit_semaphore ;
          }
          break ;
        }
        default:
        {
          break ;
        }
      }
      curr_item->valid_word = 0 ; 
      soc_sand_os_free(curr_item) ;
      Soc_sand_handles_list->stats.current_size  -- ;
      Soc_sand_handles_list->stats.no_of_removes ++ ;
    }
    else
    {
      
      prev_node = curr_node ;
      curr_node = curr_node->next ;
    }
  }
exit_semaphore:
  if (SOC_SAND_OK != soc_sand_handles_delta_list_give_mutex())
  {
    err = 6 ;
    goto exit ;
  }
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_HANDLES_UNREGISTER_ALL_DEVICE_HANDLES,
        "General error in soc_sand_handles_unregister_all_device_handles()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_CALLBACK_HANDLE
  *soc_sand_handles_search_next_handle(
    int          unit,
    uint32          proc_id,
    SOC_SAND_CALLBACK_HANDLE  *current
)
{
  SOC_SAND_CALLBACK_HANDLE
    *ex ;
  SOC_SAND_CALLBACK_HANDLE
    *curr_item ;
  SOC_SAND_DELTA_LIST_NODE
    *curr_node ;
  uint32
    current_found ;

  
  ex            = NULL ;
  curr_item     = NULL ;
  curr_node     = NULL ;
  current_found = FALSE ;
  if (!current)
  {
    current_found = TRUE ;
  }
  
  if (soc_sand_delta_list_is_empty(Soc_sand_handles_list))
  {
    
    goto exit ;
  }
  
  if (SOC_SAND_OK != soc_sand_handles_delta_list_take_mutex())
  {
    goto exit ;
  }
  
  curr_node = Soc_sand_handles_list->head ;
  while(NULL != curr_node)
  {
    
    curr_item = (SOC_SAND_CALLBACK_HANDLE *)curr_node->data ;
    
    if (unit == curr_item->unit &&
        proc_id   == curr_item->proc_id)
    {
      if (current_found)
      {
        ex = curr_item ;
        goto exit_semaphore ;
      }
      else
      {
        if (curr_item == current)
        {
          current_found = TRUE ;
        }
      }
    }
    curr_node = curr_node->next ;
  }

exit_semaphore:
  if (SOC_SAND_OK != soc_sand_handles_delta_list_give_mutex())
  {
    ex = NULL ;
    goto exit ;
  }
  
exit:
  return ex ;
}


SOC_SAND_RET
  soc_sand_handles_is_handle_exist(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  proc_id,
    SOC_SAND_OUT uint32* callback_exist
  )
{
  SOC_SAND_RET
    soc_sand_ret;
  SOC_SAND_CALLBACK_HANDLE
    *soc_sand_handle;

  soc_sand_ret = SOC_SAND_OK;

  if(callback_exist == NULL)
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR;
    goto exit;
  }

  soc_sand_handle = NULL;
  *callback_exist = FALSE;

  
  if (SOC_SAND_OK != soc_sand_handles_delta_list_take_mutex() )
  {
    soc_sand_ret = SOC_SAND_SEM_TAKE_FAIL;
    goto exit;
  }
  
  soc_sand_handle = soc_sand_handles_search_next_handle(unit, proc_id, soc_sand_handle);
  if(NULL != soc_sand_handle)
  {
      if (SOC_SAND_OK != soc_sand_handles_delta_list_give_mutex())
      {
          soc_sand_ret = SOC_SAND_SEM_GIVE_FAIL;
      }
      *callback_exist = TRUE;
      goto exit;
  }
  if (SOC_SAND_OK != soc_sand_handles_delta_list_give_mutex())
  {
    soc_sand_ret = SOC_SAND_SEM_GIVE_FAIL;
    goto exit;
  }

exit:
  return soc_sand_ret;
}


SOC_SAND_RET
  soc_sand_handles_delta_list_take_mutex(
    void
  )
{
  return
    soc_sand_delta_list_take_mutex(
      Soc_sand_handles_list
    );
}

SOC_SAND_RET
  soc_sand_handles_delta_list_give_mutex(
    void
  )
{
  return
    soc_sand_delta_list_give_mutex(
      Soc_sand_handles_list
    );
}

sal_thread_t
  soc_sand_handles_delta_list_get_owner(
    void
  )
{
  return
    soc_sand_delta_list_get_owner(
      Soc_sand_handles_list
    );
}

