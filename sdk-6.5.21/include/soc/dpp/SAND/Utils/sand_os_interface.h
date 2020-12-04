/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_SAND_OS_INTERFACE_H_INCLUDED
#define SOC_SAND_OS_INTERFACE_H_INCLUDED


#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/drv.h>

#define SOC_SAND_INFINITE_TIMEOUT (-1)
#define SOC_SAND_NO_TIMEOUT       0


#define sal_dpp_alloc(size,str) sal_alloc(size,str)


#ifndef __KERNEL__
#include <sal/appl/sal.h>
#else
extern int  sand_sal_rand_in_kernel_mode(void);
#define sal_rand sand_sal_rand_in_kernel_mode
#endif

#ifdef SOC_SAND_PRINTF_ATTRIBUTE_ENABLE_CHECK
  #ifdef __GNUC__
    #define SOC_SAND_PRINTF_ATTRIBUTE(string_index, first_to_check)  \
      __attribute__ ((format (__printf__, string_index, first_to_check)))
  #else
    #error  "Add your system support for 'SOC_SAND_PRINTF_ATTRIBUTE(string_index, first_to_check)'."
  #endif

#elif defined(COMPILER_ATTRIBUTE)
  #define SOC_SAND_PRINTF_ATTRIBUTE(string_index, first_to_check) \
    COMPILER_ATTRIBUTE ((format (__printf__, string_index, first_to_check)))
#else
  #define SOC_SAND_PRINTF_ATTRIBUTE(string_index, first_to_check)
#endif


typedef void (*SOC_SAND_FUNC_PTR)(void*);




void
  soc_sand_os_qsort(
    void         *bot,
    uint32 nmemb,
    uint32 size,
    int (*compar)(void *, void *)
    ) ;

void
  *soc_sand_os_bsearch(
    void    *key,
    void    *base0,
    uint32  nmemb,
    uint32  size,
    int (*compar)(void *, void *)
    ) ;






SOC_SAND_RET
  soc_sand_os_task_lock(
    void
    ) ;
SOC_SAND_RET
  soc_sand_os_task_unlock(
    void
    ) ;



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
  );

SOC_SAND_RET
  soc_sand_os_task_delete(
    SOC_SAND_IN     sal_thread_t          task_id
  );

sal_thread_t
  soc_sand_os_get_current_tid(
    void
  );






void
  *soc_sand_os_malloc(
    SOC_SAND_IN     uint32 size,
    char            *str
  );

SOC_SAND_RET
  soc_sand_os_free(
    SOC_SAND_INOUT  void          *memblock
  );


void
  *soc_sand_os_malloc_any_size(
    SOC_SAND_IN     uint32 size,
    char            *str
  );

SOC_SAND_RET
  soc_sand_os_free_any_size(
    SOC_SAND_INOUT  void          *memblock
  );

uint32
  soc_sand_os_get_total_allocated_memory(
    void
  );



SOC_SAND_RET
  soc_sand_os_memcpy(
    void          *destination, 
    const void    *source,      
    uint32 size          
    );

int
  soc_sand_os_memcmp(
    const     void *s1, 
    const     void *s2, 
    uint32 n   
  );

SOC_SAND_RET
  soc_sand_os_memset(
    void *m,           
    int   c,           
    uint32 n   
  );






SOC_SAND_RET
  soc_sand_os_task_delay_milisec(
    SOC_SAND_IN     uint32 delay_in_milisec
  );

int
  soc_sand_os_get_ticks_per_sec(
    void
  );

int
  soc_sand_os_nano_sleep(
    uint32     nano,
    uint32     *premature
  );

SOC_SAND_RET
  soc_sand_os_tick_get(
    SOC_SAND_INOUT     uint32 *ticks
  );

SOC_SAND_RET
  soc_sand_os_get_time(
    SOC_SAND_INOUT     uint32 *seconds,
    SOC_SAND_INOUT     uint32 *nano_seconds
  );


SOC_SAND_RET
  soc_sand_os_get_time_diff(
    SOC_SAND_IN     uint32 start_seconds,
    SOC_SAND_IN     uint32 start_nano_seconds,
    SOC_SAND_IN     uint32 end_seconds,
    SOC_SAND_IN     uint32 end_nano_seconds,
    SOC_SAND_OUT    uint32 *diff_seconds,
    SOC_SAND_OUT    uint32 *diff_nano_seconds
  );
uint32
  soc_sand_os_get_time_micro(
    void
  );


uint32
  soc_sand_os_sys_clk_rate_get(
    void
  );








sal_mutex_t
  soc_sand_os_mutex_create(
    void
  );

SOC_SAND_RET
  soc_sand_os_mutex_delete(
    SOC_SAND_IN     sal_mutex_t          sem_id
  );

SOC_SAND_RET
  soc_sand_os_mutex_take(
    SOC_SAND_IN     sal_mutex_t          sem_id,
    SOC_SAND_IN     long          timeout
  );

SOC_SAND_RET
  soc_sand_os_mutex_give(
    SOC_SAND_IN     sal_mutex_t          sem_id
  );






SOC_SAND_RET
  soc_sand_os_stop_interrupts(
    uint32 *flags
  );

SOC_SAND_RET
  soc_sand_os_start_interrupts(
    uint32 flags
  );






void
  *soc_sand_os_msg_q_create(
    int max_msgs,
    int max_msglength
    ) ;

SOC_SAND_RET
  soc_sand_os_msg_q_delete(
    void *msg_q_id
    );

SOC_SAND_RET
  soc_sand_os_msg_q_send(
    void         *msg_q_id,
    char         *data,
    uint32 data_size,
    int          timeout,
    int          priority
  ) ;

int
  soc_sand_os_msg_q_recv(
    void          *msg_q_id,
    unsigned char *data,
    uint32  max_nbytes,
    long          timeout_in_ticks
    ) ;
int
  soc_sand_os_msg_q_num_msgs(
    void          *msg_q_id
    ) ;







uint32
  soc_sand_os_strlen(
    const char* s 
  );


int
  soc_sand_os_strncmp(
    const char *string1,
    const char *string2,
    uint32 count
  );


SOC_SAND_RET
  soc_sand_os_strncpy(
    char          *s1, 
    const char    *s2, 
    uint32 n   
  );



void
  soc_sand_os_srand(
    uint32
  );

uint32
  soc_sand_os_rand(
    void
  );



int
  soc_sand_os_printf(
    SOC_SAND_IN char* format, ...
  ) SOC_SAND_PRINTF_ATTRIBUTE(1, 2);





#ifdef SOC_SAND_DEBUG



SOC_SAND_RET
  soc_sand_os_resource_print(
    void
  );


#endif


#ifdef  __cplusplus
}
#endif



#endif

