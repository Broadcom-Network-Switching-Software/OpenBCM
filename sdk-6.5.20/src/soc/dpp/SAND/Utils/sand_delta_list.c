/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_delta_list.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#define PRINT_LIST 0


void
  soc_sand_delta_list_print(
    SOC_SAND_DELTA_LIST  *plist
  )
{
  SOC_SAND_DELTA_LIST_NODE *iter ;
  int nof_item ;

  nof_item = 0 ;
  
  LOG_CLI((BSL_META("  soc_sand_delta_list_print():\r\n")));
  if (NULL == plist)
  {
    LOG_CLI((BSL_META("plist is NULL. \r\n")));
    goto exit ;
  }

  if (NULL == plist->head)
  {
    LOG_CLI((BSL_META("Empty list: \r\n")));
  }
  
  LOG_CLI((BSL_META("List = 0x%p, head = 0x%p, head time = 0x%X and mutex = 0x%lX\r\n"),
(void *)plist, (void *)(plist->head), (uint32)plist->head_time, (unsigned long)plist->mutex_id));
  
  iter = plist->head ;
  while(NULL != iter)
  {
    LOG_CLI((BSL_META("iter = 0x%p, next = 0x%p data = 0x%X and delta time = 0x%X\r\n"),
(void *)iter, (void *)(iter->next), PTR_TO_INT(iter->data), PTR_TO_INT(iter->num)));
    
    iter = iter->next ;
    nof_item ++ ;
  }
  LOG_CLI((BSL_META("number of items in the list: %d\r\n"), nof_item));
  goto exit ;
  
exit:
  return ;
}


sal_thread_t
  soc_sand_delta_list_get_owner(
    SOC_SAND_DELTA_LIST  *plist
  )
{
  if(!plist)
    return 0;
  else
    return plist->mutex_owner;
}


int
  soc_sand_delta_list_is_empty(
    SOC_SAND_IN   SOC_SAND_DELTA_LIST      *plist
  )
{
  if (!plist || ! plist->head )
  {
    return TRUE ;
  }
  return FALSE ;
}

SOC_SAND_RET
  soc_sand_delta_list_take_mutex(
    SOC_SAND_INOUT   SOC_SAND_DELTA_LIST      *plist
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    err = 0 ;
  
  if ( !plist || !plist->mutex_id )
  {
    ex = SOC_SAND_ERR ;
    err = 1 ;
    goto exit ;
  }

 
  if (soc_sand_os_get_current_tid() == plist->mutex_owner)
  {
    ex = SOC_SAND_OK ;
    plist->mutex_counter ++ ;
    goto exit ;
  }
  
  if (SOC_SAND_OK != (ex = soc_sand_os_mutex_take(plist->mutex_id, (long)SOC_SAND_INFINITE_TIMEOUT)))
  {
    err = 2 ;
    goto exit ;
  }
  
  plist->mutex_owner   = soc_sand_os_get_current_tid() ;
  plist->mutex_counter = 1 ;
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_DELTA_LIST_TAKE_MUTEX,
        "General error in soc_sand_delta_list_take_mutex()",err,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_delta_list_give_mutex(
    SOC_SAND_INOUT   SOC_SAND_DELTA_LIST      *plist
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    err = 0 ;

  
  if ( !plist || !plist->mutex_id )
  {
    ex = SOC_SAND_ERR ;
    err = 1 ;
    goto exit ;
  }
  
  if(soc_sand_os_get_current_tid() != plist->mutex_owner)
  {
    ex = SOC_SAND_ERR ;
    err = 2 ;
    goto exit ;
  }
  
  plist->mutex_counter -- ;
  if(0 == plist->mutex_counter)
  {
    plist->mutex_owner = 0 ;
    if (SOC_SAND_OK != (ex = soc_sand_os_mutex_give(plist->mutex_id)))
    {
      ex = SOC_SAND_ERR ;
      err = 3 ;
      goto exit ;
    }
  }
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_DELTA_LIST_GIVE_MUTEX,
        "General error in soc_sand_delta_list_give_mutex()",err,0,0,0,0,0) ;
  return ex ;
}

uint32
  soc_sand_delta_list_get_head_time(
    SOC_SAND_IN     SOC_SAND_DELTA_LIST      *plist
  )
{
  if (!plist || !plist->head)
  {
      return 0 ;
  }
  return plist->head_time ;
}

SOC_SAND_DELTA_LIST_STATISTICS
  *soc_sand_delta_list_get_statisics(
    SOC_SAND_INOUT  SOC_SAND_DELTA_LIST      *plist
  )
{
  if(plist)
 {
   return &(plist->stats) ;
 }
 else
 {
   return NULL ;
 }
}



SOC_SAND_DELTA_LIST
  *soc_sand_delta_list_create(
  )
{
  SOC_SAND_DELTA_LIST      *plist ;
  
  plist = NULL ;
  
  plist = (SOC_SAND_DELTA_LIST *)soc_sand_os_malloc(sizeof(SOC_SAND_DELTA_LIST),"plist delta_list_create") ;
  if(!plist)
  {
    goto exit ;
  }
  
  plist->head       = NULL ;
  plist->head_time  = 0 ;
  plist->mutex_id   = 0 ;
  plist->mutex_id = soc_sand_os_mutex_create() ;
  plist->mutex_counter        = 0 ;
  plist->mutex_owner          = 0 ;
  plist->stats.current_size   = 0 ;
  plist->stats.no_of_inserts  = 0 ;
  plist->stats.no_of_pops     = 0 ;
  plist->stats.no_of_removes  = 0 ;
  if(!plist->mutex_id)
  {
     soc_sand_os_free(plist) ;
     plist = NULL ;
     goto exit ;
  }
  
exit:
  return plist ;
}

SOC_SAND_RET
  soc_sand_delta_list_destroy(
    SOC_SAND_INOUT  SOC_SAND_DELTA_LIST      *plist
  )
{
  SOC_SAND_RET
    ex ;
  ex = SOC_SAND_ERR ;
  
  if (!plist || plist->head)
  {
    goto exit ;
  }
  
  soc_sand_os_mutex_delete(plist->mutex_id) ;
  plist->head_time = 0 ;
  plist->mutex_id  = 0 ;
  soc_sand_os_free(plist) ;
  ex = SOC_SAND_OK ;
exit:
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_DELTA_LIST_DESTROY,
        "General error in soc_sand_delta_list_destroy()",0,0,0,0,0,0) ;
  return ex ;
}


SOC_SAND_RET
  soc_sand_delta_list_insert_d(
    SOC_SAND_INOUT  SOC_SAND_DELTA_LIST      *plist,
    SOC_SAND_IN     uint32    time,
    SOC_SAND_INOUT  void            *data
  )
{
  SOC_SAND_DELTA_LIST_NODE
    *p_new,
    *iter ;
  uint32
      sum ;
  SOC_SAND_RET
    ex = SOC_SAND_ERR ;
  uint32
    err = 0 ;
  if (NULL == plist)
  {
    err = 1 ;
    goto exit ;
  }
  
  p_new = soc_sand_os_malloc(sizeof(SOC_SAND_DELTA_LIST_NODE), "delta_list_insert_d") ;
  if (p_new == NULL)
  {
    err = 2 ;
    goto exit ;
  }
  p_new->data  = (void *)data ;
  
  sum = 0 ;
  if (time < plist->head_time)
  {
    
    p_new->num       = plist->head_time - time ;
    plist->head_time = time ;
    p_new->next      = plist->head ;
    plist->head      = p_new ;
  }
  else
  {
    
    sum += plist->head_time ;
    for (iter=plist->head ; iter!=NULL ; iter=iter->next)
    {
      if ( (sum += iter->num) > time) break ;
    }
    if(iter != NULL)
    {
     
      p_new->num  =  sum - time ;
      p_new->next =  iter->next ;
      iter->num  -=  p_new->num ;
      iter->next  =  p_new ;
    }
    else
    {
     
      if (!plist->head)
      {
        
        p_new->num       = 0 ;
        p_new->next      = NULL ;
        plist->head      = p_new ;
        plist->head_time = time ;
      }
      else
      {
        
        for (sum=plist->head_time, iter=plist->head ; iter->next!=NULL ; iter=iter->next)
        {
          sum += iter->num ;
        }
        p_new->num  = 0 ;
        p_new->next = NULL ;
        iter->num   = time - sum ;
        iter->next  = p_new ;
      }
    }
  }
  plist->stats.current_size  ++ ;
  plist->stats.no_of_inserts ++ ;
  ex = SOC_SAND_OK ;
exit:
#if PRINT_LIST
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("\r\nafter insert data = 0x%X and time = 0x%X\r\n"), (uint32)data, (uint32)time));
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("===============================================================================\r\n")));
  soc_sand_delta_list_print(plist) ;
#endif
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_DELTA_LIST_INSERT_D,
        "General error in soc_sand_delta_list_insert_d()",err,0,0,0,0,0) ;
  return ex ;
}


void
  *soc_sand_delta_list_pop_d(
    SOC_SAND_INOUT  SOC_SAND_DELTA_LIST      *plist
  )
{
  void *data ;
  SOC_SAND_DELTA_LIST_NODE *p_tmp ;
  
  data = NULL ;
  if (NULL == plist)
  {
      goto exit ;
  }
  
  if (NULL == plist->head)
  {
    
    data = NULL ;
    goto exit ;
  }
  
  p_tmp = plist->head ;
  data  = plist->head->data ;
  plist->head_time = p_tmp->num ;
  plist->head = plist->head->next ;
  soc_sand_os_free (p_tmp) ;
  plist->stats.current_size -- ;
  plist->stats.no_of_pops   ++ ;

  
exit:
#if PRINT_LIST
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("\r\nafter pop data = 0x%X\r\n"), (uint32)data));
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("===============================================================================\r\n")));
  soc_sand_delta_list_print(plist) ;
#endif
  return data ;
}

SOC_SAND_RET
  soc_sand_delta_list_decrease_time_from_head(
    SOC_SAND_INOUT SOC_SAND_DELTA_LIST       *plist,
    SOC_SAND_IN    uint32    time_to_substract

  )
{
  SOC_SAND_DELTA_LIST_NODE *p_curr;
  SOC_SAND_RET        ex;
  uint32   tmp_time_to_substract;
  
  ex = SOC_SAND_ERR ;
  if(!plist || !plist->mutex_id)
  {
    goto exit ;
  }
  
  ex = SOC_SAND_OK;
  if (plist->head_time > time_to_substract)
  {
    plist->head_time -= time_to_substract;
    goto exit;
  }
  
  tmp_time_to_substract = time_to_substract - plist->head_time;
  plist->head_time = 0;
  p_curr = plist->head;
  while(p_curr != NULL && tmp_time_to_substract > 0)
  {
    if(p_curr->num > tmp_time_to_substract)
    {
      p_curr->num -= tmp_time_to_substract;
      goto exit;
    }
    
    tmp_time_to_substract -= p_curr->num;
    p_curr->num = 0;
    p_curr = p_curr->next;
  }
  
exit:
#if PRINT_LIST
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("\r\nafter decrease_time_from_head time to decrease = 0x%X and curent head time is 0x%X \r\n"),
(uint32)time_to_substract, (uint32)plist->head_time));
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("===============================================================================\r\n")));
  soc_sand_delta_list_print(plist) ;
#endif
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_DELTA_LIST_DECREASE_TIME_FROM_HEAD,
        "General error in soc_sand_delta_list_decrease_time_from_head()",0,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_delta_list_decrease_time_from_second_item(
    SOC_SAND_INOUT SOC_SAND_DELTA_LIST       *plist,
    SOC_SAND_INOUT  uint32   time_to_substract
  )
{
  SOC_SAND_DELTA_LIST_NODE *p_curr;
  SOC_SAND_RET        ex;
  
  ex = SOC_SAND_OK ;
  if(!plist || !plist->mutex_id)
  {
    ex = SOC_SAND_ERR ;
    goto exit ;
  }
  
  p_curr = plist->head;
  while(p_curr != NULL && time_to_substract > 0)
  {
    if(p_curr->num > time_to_substract)
    {
      p_curr->num -= time_to_substract;
      goto exit;
    }
    
    time_to_substract -= p_curr->num;
    p_curr->num = 0;
    p_curr = p_curr->next;
  }
  
exit:
#if PRINT_LIST
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("\r\nafter soc_sand_delta_list_decrease_time_from_second_item time to decrease = 0x%X and curent head time is 0x%X \r\n"),
(uint32)time_to_substract, (uint32)plist->head_time));
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("===============================================================================\r\n")));
  soc_sand_delta_list_print(plist) ;
#endif
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_DELTA_LIST_DECREASE_TIME_FROM_SECOND_ITEM,
        "General error in soc_sand_delta_list_decrease_time_from_second_item()",0,0,0,0,0,0) ;
  return ex ;
}

SOC_SAND_RET
  soc_sand_delta_list_remove(
    SOC_SAND_INOUT SOC_SAND_DELTA_LIST       *plist,
    SOC_SAND_IN    void             *data

  )
{
  SOC_SAND_DELTA_LIST_NODE *p_tmp, *p_prev ;
  SOC_SAND_RET
    ex ;
  uint32
    err = 0 ;
  ex = SOC_SAND_ERR ;
  if(!plist || !plist->head)
  {
    err = 1 ;
    goto exit ;
  }
  p_prev = NULL ;
  p_tmp  = plist->head ;
  while (NULL != p_tmp)
  {
    if (p_tmp->data == data)
    {
      if (NULL == p_prev)
      {
        
        plist->head_time += p_tmp->num ;
        plist->head       = p_tmp->next ;
      }
      else
      {
        if (p_tmp->next)
        {
          
          p_prev->num += p_tmp->num ;
          p_prev->next = p_tmp->next ;
        }
        else
        {
         
          p_prev->num  = 0 ;
          p_prev->next = NULL ;
        }
      }
      break ;
    }
    p_prev = p_tmp ;
    p_tmp  = p_tmp->next ;
  }
  if(NULL != p_tmp)
  {
    
    soc_sand_os_free(p_tmp) ;
    plist->stats.current_size  -- ;
    plist->stats.no_of_removes ++ ;
    ex = SOC_SAND_OK ;
    goto exit ;
  }
exit:
#if PRINT_LIST
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("\r\nafter remove data = 0x%X \r\n"), (uint32)data));
  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("===============================================================================\r\n")));
  soc_sand_delta_list_print(plist) ;
#endif
  SOC_SAND_ERROR_REPORT(ex,NULL,0,0,SOC_SAND_DELTA_LIST_REMOVE,
        "General error in soc_sand_delta_list_remove()",err,0,0,0,0,0) ;
  return ex ;
}



#if SOC_SAND_DEBUG



void
  soc_sand_delta_list_print_DELTA_LIST_STATISTICS(
    SOC_SAND_IN SOC_SAND_DELTA_LIST_STATISTICS* delta_list_statistics
  )
{
  if (NULL == delta_list_statistics)
  {
    LOG_CLI((BSL_META("soc_sand_delta_list_print_DELTA_LIST_STATISTICS got NULL\n\r")));
    goto exit;
  }
  
  LOG_CLI((BSL_META(" current_size = %u, no_of_inserts = %u,\n\r"
" no_of_pops = %u, no_of_removes = %u"),
           delta_list_statistics->current_size,
           delta_list_statistics->no_of_inserts,
           delta_list_statistics->no_of_pops,
           delta_list_statistics->no_of_removes
           ));
exit:
  return;
}


void
  soc_sand_delta_list_print_DELTA_LIST(
    SOC_SAND_IN SOC_SAND_DELTA_LIST* delta_list
  )
{

  if (NULL == delta_list)
  {
    LOG_CLI((BSL_META("soc_sand_delta_list_print_DELTA_LIST got NULL\n\r")));
    goto exit;
  }

  
  LOG_CLI((BSL_META(" head= %08X, head_time= %u,\n\r"
                    " mutex_id = %p, mutex_owner = %p, mutex_counter = %u\n\r"),
           PTR_TO_INT(delta_list->head),
           delta_list->head_time,
           (void *)(delta_list->mutex_id),
           (void *)(delta_list->mutex_owner),
           delta_list->mutex_counter
           ));
  soc_sand_delta_list_print_DELTA_LIST_STATISTICS(
    &(delta_list->stats)
  );

exit:
  return;
}


#endif

