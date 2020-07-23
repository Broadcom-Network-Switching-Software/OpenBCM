/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_rand.h>

#include <sal/appl/io.h>
#include <sal/core/thread.h>

#include <shared/util.h>

#ifndef __KERNEL__
#include <time.h>
#endif

#define SOC_SAND_OS_NOT_IMPLEMENTED 
#define SOC_SAND_OS_LNX_CHECK_0_ERR(_ret, _sand_ret, _str, _label) \
  do { \
    if (_ret != 0) \
    { \
      LOG_INFO(BSL_LS_BCM_COMMON, \
               (BSL_META("*** ERROR in %s:%d, %s: %s\n\r"), __FILE__, __LINE__, FUNCTION_NAME(), _str)); \
      _sand_ret = SOC_SAND_ERR; \
      goto _label; \
    } \
  } while (0)

int S_spl;



void
  soc_sand_os_qsort(
    void         *bot,
    uint32 nmemb,
    uint32 size,
    int (*compar)(void *, void *)
  )
{
  _shr_sort(bot, nmemb, size, compar);
  return ;
}


void
  *soc_sand_os_bsearch(
    void    *key,
    void    *base0,
    uint32  nmemb,
    uint32  size,
    int (*compar)(void *, void *)
    )
{
  void  *ret;
  int   idx;

  idx = _shr_bsearch(base0, nmemb, size, key, compar);

    if (idx > 0) {
        
        ret = ((void *) &((char *)(base0))[(idx) * (size)]);
    } else {
        ret = NULL;
    }

  return (ret) ;
}

SOC_SAND_RET
  soc_sand_os_task_lock(
    void
    )
{
  SOC_SAND_RET
  soc_sand_ret = SOC_SAND_OK;
  uint32 flags;

  soc_sand_ret = soc_sand_os_stop_interrupts(&flags);
  SOC_SAND_OS_LNX_CHECK_0_ERR(soc_sand_ret, soc_sand_ret, "soc_sand_os_stop_interrupts failed", exit);

exit:
  return soc_sand_ret;
}

SOC_SAND_RET
  soc_sand_os_task_unlock(
    void
    )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  uint32 flags = 0;

  soc_sand_ret = soc_sand_os_start_interrupts(flags);
  SOC_SAND_OS_LNX_CHECK_0_ERR(soc_sand_ret, soc_sand_ret, "soc_sand_os_start_interrupts failed", exit);

exit:
  return soc_sand_ret;
}


void
  *soc_sand_os_malloc(
    SOC_SAND_IN uint32 size,
    char *str
  )
{
  return sal_alloc(size, str);
}


SOC_SAND_RET
  soc_sand_os_free(
    SOC_SAND_INOUT  void  *memblock
  )
{
  sal_free(memblock);

  return SOC_SAND_OK;
}

void
  *soc_sand_os_malloc_any_size(
    SOC_SAND_IN uint32 size,
        char *str
  )
{
  
   return sal_alloc(size, str) ; 
}

SOC_SAND_RET
  soc_sand_os_free_any_size(
    SOC_SAND_INOUT  void  *memblock
  )
{
  sal_free(memblock);
  
  return SOC_SAND_OK;
}


sal_thread_t
  soc_sand_os_task_spawn(
    SOC_SAND_IN     char          *name,     
    SOC_SAND_IN     long          priority,  
    SOC_SAND_IN     long          options,   
    SOC_SAND_IN     long          stackSize, 
    SOC_SAND_IN     SOC_SAND_FUNC_PTR      entryPt,  
    SOC_SAND_IN     long          arg1,      
    SOC_SAND_IN     long          arg2,
    SOC_SAND_IN     long          arg3,
    SOC_SAND_IN     long          arg4,
    SOC_SAND_IN     long          arg5,
    SOC_SAND_IN     long          arg6,
    SOC_SAND_IN     long          arg7,
    SOC_SAND_IN     long          arg8,
    SOC_SAND_IN     long          arg9,
    SOC_SAND_IN     long          arg10
  )
{
  return sal_thread_create("soc_sand_os_task_spawn", stackSize, priority, entryPt, NULL);
}



SOC_SAND_RET
  soc_sand_os_task_delete(
    SOC_SAND_IN sal_thread_t  task_id
  )
{
  SOC_SAND_OS_NOT_IMPLEMENTED;  
  return SOC_SAND_ERR; 
}

sal_thread_t
  soc_sand_os_get_current_tid(
    void
  )
{
  return sal_thread_self();
}



SOC_SAND_RET
  soc_sand_os_task_delay_milisec(
    SOC_SAND_IN uint32 delay_in_milisec
  )
{
  sal_msleep(delay_in_milisec);
  
  return SOC_SAND_OK;
}


int
  soc_sand_os_get_ticks_per_sec(
    void
  )
{
  int
    ticks_per_sec;

    
    ticks_per_sec = 100;

  return ticks_per_sec;
}


SOC_SAND_RET
  soc_sand_os_tick_get(
    SOC_SAND_INOUT     uint32 *ticks
  )
{
  SOC_SAND_OS_NOT_IMPLEMENTED;

  return SOC_SAND_OK ;
}


int
  soc_sand_os_nano_sleep(
    uint32     nano,
    uint32     *premature
  )
{
  sal_usleep(nano/1000);

  if (premature) {
        *premature = 0;
    }
  
  return SOC_SAND_OK;
}

  

SOC_SAND_RET
  soc_sand_os_get_time(
    SOC_SAND_INOUT     uint32 *seconds,
    SOC_SAND_INOUT     uint32 *nano_seconds
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK;
  sal_usecs_t usec;

  usec = sal_time_usecs();

  if (seconds)      *seconds    = usec/SECOND_USEC ;
  if (nano_seconds) *nano_seconds = (usec%SECOND_USEC)*1000;

  return ex ;
}
SOC_SAND_RET
  soc_sand_os_get_time_diff(
    SOC_SAND_IN     uint32 start_seconds,
    SOC_SAND_IN     uint32 start_nano_seconds,
    SOC_SAND_IN     uint32 end_seconds,
    SOC_SAND_IN     uint32 end_nano_seconds,
    SOC_SAND_OUT    uint32 *diff_seconds,
    SOC_SAND_OUT    uint32 *diff_nano_seconds
  )
{
  if(diff_seconds)
  {
    *diff_seconds = end_seconds - start_seconds;
  }
  if(diff_nano_seconds)
  {
    if(end_nano_seconds >= start_nano_seconds)
    {
      *diff_nano_seconds = end_nano_seconds - start_nano_seconds;
    }
    else
    {
      if(diff_seconds)
      {
        *diff_seconds = *diff_seconds - 1;
      }
      *diff_nano_seconds = end_nano_seconds + 1000000000 - start_nano_seconds;
    }
  }

  return SOC_SAND_OK;
}


uint32
  soc_sand_os_get_time_micro(
    void
  )
{
  return sal_time_usecs();
}


sal_mutex_t
  soc_sand_os_mutex_create(
    void
  )
{
  return sal_mutex_create("soc_sand_os_mutex_create");
}


SOC_SAND_RET
  soc_sand_os_mutex_delete(
    SOC_SAND_IN sal_mutex_t  sem_id
  )
{
  sal_mutex_destroy(sem_id);
  
  return SOC_SAND_OK;
}


SOC_SAND_RET
  soc_sand_os_mutex_take(
    SOC_SAND_IN sal_mutex_t  sem_id,
    SOC_SAND_IN long  timeout
  )
{
  int ex;
  
  ex = sal_mutex_take(sem_id, sal_mutex_FOREVER);
  
  return (ex ? SOC_SAND_ERR : SOC_SAND_OK);
}


SOC_SAND_RET
  soc_sand_os_mutex_give(
    SOC_SAND_IN sal_mutex_t  sem_id
  )
{
  int ex;
  
  ex = sal_mutex_give(sem_id);
  
  return (ex ? SOC_SAND_ERR : SOC_SAND_OK);
}


SOC_SAND_RET
  soc_sand_os_stop_interrupts(
    uint32 *flags
  )
{
     
    {
        S_spl = sal_splhi();
    }

    return SOC_SAND_OK;
}


SOC_SAND_RET
  soc_sand_os_start_interrupts(
    uint32 flags
  )
{
     
    {
        sal_spl(S_spl--);
    }
    return SOC_SAND_OK;
}


void
  *soc_sand_os_msg_q_create(
    int max_msgs,
    int max_msglength
    )
{

  SOC_SAND_OS_NOT_IMPLEMENTED;
  return (void*)0xf; 
  
}

SOC_SAND_RET
  soc_sand_os_msg_q_delete(
    void *msg_q_id
    )
{

  SOC_SAND_OS_NOT_IMPLEMENTED;
  return SOC_SAND_OK ;
}

SOC_SAND_RET
  soc_sand_os_msg_q_send(
    void         *msg_q_id,
    char         *data,
    uint32 data_size,
    int          timeout,
    int          priority
  )
{
  SOC_SAND_OS_NOT_IMPLEMENTED;
  return SOC_SAND_OK ;
}

int
  soc_sand_os_msg_q_recv(
    void          *msg_q_id,
    unsigned char *data,
    uint32  max_nbytes,
    long          timeout_in_ticks
    )
{
  SOC_SAND_OS_NOT_IMPLEMENTED;
  return SOC_SAND_OK ;
}

int
  soc_sand_os_msg_q_num_msgs(
    void          *msg_q_id
    )
{
  SOC_SAND_OS_NOT_IMPLEMENTED;
  return SOC_SAND_OK ;
}


SOC_SAND_RET
  soc_sand_os_memcpy(
    void          *destination, 
    const void    *source,      
    uint32 size          
    )
{
  if(destination == NULL || source == NULL) {
    return SOC_SAND_ERR;
  }
  
  sal_memcpy(destination, source, (size_t)size) ;
 
  return SOC_SAND_OK ;
}

int
  soc_sand_os_memcmp(
    const     void *s1, 
    const     void *s2, 
    uint32 n   
  )
{
  assert(s1 != NULL && s2 != NULL);
  
  return sal_memcmp(s1, s2, (size_t)n) ;
}


SOC_SAND_RET
  soc_sand_os_memset(
    void *m,           
    int   c,           
    uint32 n   
  )
{
  if(m == NULL) {
    return SOC_SAND_ERR;
  }
  
  sal_memset(m, c, (size_t)n) ;
 
  return SOC_SAND_OK ;
}


uint32
  soc_sand_os_strlen(
    const char* s 
  )
{
  uint32 string_len ;
  string_len = sal_strlen(s ) ;
  return string_len ;
}



SOC_SAND_RET
  soc_sand_os_strncpy(
    char          *s1, 
    const char    *s2, 
    uint32 n   
  )
{
  sal_strncpy(s1, s2, (size_t)n ) ;
  
  return SOC_SAND_OK ;
}

uint32
  soc_sand_os_sys_clk_rate_get(
    void
  )
{

  return 1000000;
}




static
  SOC_SAND_RAND
   Soc_sand_os_rand = {{0}, 0, 0, 0, 0};


void
  soc_sand_os_srand(
    uint32 x
  )
{
  soc_sand_rand_seed_set(&Soc_sand_os_rand, x);
  return;
}

uint32
  soc_sand_os_rand(
    void
  )
{
  uint32
    x;
  x = soc_sand_rand_u_long(&Soc_sand_os_rand);
  return x;
}



int
  soc_sand_os_printf(
    SOC_SAND_IN char* format, ...
  )
{
  va_list
    args ;
  int
    i;

  va_start(args, format) ;
  i = sal_vprintf(format, args);
  va_end(args) ;
  
  return i;
}



int
sand_sal_rand_in_kernel_mode(void)
{
  return 0;
}

