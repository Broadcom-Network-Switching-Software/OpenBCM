/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * Basic_include_file.
 */

/*
 * Tasks information.
 */
#if !DUNE_BCM
#include "Pub/include/tasks_info.h"
#endif
/*
 * INCLUDE FILES:
 * {
 */
#if !DUNE_BCM

/* { */
  #define WAIT_FOREVER -1
  #include "stdio.h"
  #include "string.h"
  #include "stdlib.h"
  #include "time.h"
  #include <ctype.h>
/* } */
#endif

/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defi.h>

#ifdef SIM_ON_WINDOWS
  #include "conio.h"
#endif

#include <appl/diag/dpp/tasks_info.h>
#include <appl/diag/dpp/utils_char_queue.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

extern
  int send_string_to_screen(char *out_str, int add_cr_lf);

/*
 * checks that the in_count, out_count, read_ptr, write_ptr are adjusted to each other.
 */
STATIC
	uint8
 		check_in_out_counts(
   	  CHAR_QUEUE* in_char_q
  	);


uint8
  char_buff_create(
    int in_size,
    CHAR_QUEUE* in_out_char_q
  )
{
  uint8 valid = TRUE;

  if(in_out_char_q == NULL || in_size <= 0)
  {
    valid = FALSE;
    goto exit;
  }

  /*
   * allocates the semaphore
   */

  in_out_char_q->char_buff_sem = soc_sand_os_mutex_create();

  if(in_out_char_q->char_buff_sem == NULL)
  {
    valid = FALSE;
    goto exit;
  }

  /*
   * allocate the buffer according to size
   */
  in_out_char_q->char_buff = soc_sand_os_malloc(in_size,"in_out_char_q->char_buff");
  /*
   * set the size, so we can call 'clear'
   */
  in_out_char_q->char_buff_size = in_size;

  /*
   * init all members to 0
   */
  char_buff_clear(in_out_char_q);

exit:

  return valid;
}



void
  char_buff_delete(
    CHAR_QUEUE* in_char_q
  )
{
  if(in_char_q == NULL)
  {
    goto exit;
  }

  soc_sand_os_free(in_char_q->char_buff);
  in_char_q->char_buff = NULL;
  soc_sand_os_mutex_delete(in_char_q->char_buff_sem);

exit:

  return;
}



uint8
  char_buff_is_empty(
    CHAR_QUEUE* in_char_q
  )
{
  uint8 res = FALSE;

  /*
   * if q is NULL, the anser is not relevant
   * this should be checked before entering here!
   */
  if(in_char_q == NULL)
  {
    res = TRUE;
    goto exit_without_give_semaphore;
  }

  /*
   * take the semaphore and wait forever
   */
  soc_sand_os_mutex_take(in_char_q->char_buff_sem, WAIT_FOREVER);

  /*
   * Queue empty
   */
  if(in_char_q->char_buff_write_ptr == in_char_q->char_buff_read_ptr)
  {
    res = TRUE;
    goto exit_give_semaphore;
  }


exit_give_semaphore:

  soc_sand_os_mutex_give(in_char_q->char_buff_sem);
  return res;

exit_without_give_semaphore:

  return res;
}



void
  char_buff_clear(
    CHAR_QUEUE* in_char_q
  )
{
  if(in_char_q == NULL)
  {
    goto exit;
  }

  /*
   * take the semaphore and wait forever
   */
  soc_sand_os_mutex_take(in_char_q->char_buff_sem, WAIT_FOREVER);

  /* set all the buffer with 0 */
  soc_sand_os_memset(in_char_q->char_buff, 0x0, in_char_q->char_buff_size);

  /* init all members of char_queue to 0 */
  in_char_q->char_buff_read_ptr  = 0;
  in_char_q->char_buff_write_ptr = 0;
  in_char_q->char_in_cnt         = 0;
  in_char_q->char_out_cnt        = 0;
  in_char_q->char_buff_full      = FALSE;

  soc_sand_os_mutex_give(in_char_q->char_buff_sem);

exit:

  return;
}



uint8
  put_char(
    CHAR_QUEUE* in_char_q,
    const char in_char
  )
{
  uint8 valid = TRUE;
  char proc_name[] = "put_char";

  if(in_char_q == NULL)
  {
    valid = FALSE;
    
    gen_err(FALSE,FALSE, 0, 0,
                          "Internal error in char_queue (in_char_q is NULL)", proc_name,
                          SVR_ERR, CHAR_Q_INTERNAL_ERR,TRUE, 0, 1000, FALSE);
    
    goto exit_without_give_semaphore;
  }

  /*
   * take the semaphore and wait forever
   */
  soc_sand_os_mutex_take(in_char_q->char_buff_sem, WAIT_FOREVER);

  if(((in_char_q->char_buff_write_ptr + 1) % in_char_q->char_buff_size) ==
        in_char_q->char_buff_read_ptr)
  {
    /*
     * if Q is full
     * Exit and do nothing
     */
    if(in_char_q->char_buff_full == TRUE)
    {
      valid = FALSE;

        gen_err(FALSE,FALSE, 0, 0,
                 "Internal error in char_queue (queue is full)", proc_name,
                 SVR_ERR, CHAR_Q_INTERNAL_ERR,TRUE, 0, 1000, FALSE);
        
      goto exit_give_semaphore;
    }
    /*
     * if last place is still empty
     */
    else
    {
      in_char_q->char_buff_full = TRUE;
      /*
       * Add the char and DONNOT advance the write pointer
       */
      in_char_q->char_in_cnt++;
      in_char_q->char_buff[in_char_q->char_buff_write_ptr] = in_char;


      if(check_in_out_counts(in_char_q) == FALSE)
      {
        gen_err(FALSE,FALSE, 0, 0,
                "Internal error in char_queue ('check_in_out_counts' failed)", proc_name,
                SVR_ERR, CHAR_Q_INTERNAL_ERR,TRUE, 0, 1000, FALSE);

        goto exit_give_semaphore;
      }
    }
  }
  /*
   * Q is not full at all...
   */
  else
  {
    /*
     * Add the char and advance the write pointer
     */
    in_char_q->char_in_cnt++;
    in_char_q->char_buff[in_char_q->char_buff_write_ptr] = in_char;
    in_char_q->char_buff_write_ptr++;
    in_char_q->char_buff_write_ptr %= in_char_q->char_buff_size;


    if(check_in_out_counts(in_char_q) == FALSE)
    {
      gen_err(FALSE,FALSE, 0, 0,
              "Internal error in char_queue ('check_in_out_counts' failed)", proc_name,
              SVR_ERR, CHAR_Q_INTERNAL_ERR,TRUE, 0, 1000, FALSE);

      goto exit_give_semaphore;
    }
  }

exit_give_semaphore:

  soc_sand_os_mutex_give(in_char_q->char_buff_sem);
  return valid;

exit_without_give_semaphore:

  return valid;
}



uint8
  get_char(
    CHAR_QUEUE* in_char_q,
    char* out_char
  )
{
  uint8 valid = TRUE;
  char proc_name[] = "get_char";

  if(in_char_q == NULL || out_char == NULL)
  {
    valid = FALSE;
    goto exit_without_give_semaphore;
  }

  /*
   * Queue empty
   */
  if(char_buff_is_empty(in_char_q) == TRUE)
  {
    valid = FALSE;
    goto exit_without_give_semaphore;
  }

  /*
   * take the semaphore and wait forever
   */
  soc_sand_os_mutex_take(in_char_q->char_buff_sem, WAIT_FOREVER);

  /*
   * Get the char
   */
  *out_char = in_char_q->char_buff[in_char_q->char_buff_read_ptr];

  in_char_q->char_out_cnt++;
  in_char_q->char_buff_read_ptr++;
  in_char_q->char_buff_read_ptr %= in_char_q->char_buff_size;

  /*
   * if Q was full until now - make it UNFULL
   *  and set the write pointer +1, to the next available place
   *  because now it pointed to a taken place
   */
  if(in_char_q->char_buff_full == TRUE)
  {
    in_char_q->char_buff_full = FALSE;
    in_char_q->char_buff_write_ptr++;
    in_char_q->char_buff_write_ptr %= in_char_q->char_buff_size;
  }


  if(check_in_out_counts(in_char_q) == FALSE)
  {
    gen_err(FALSE,FALSE, 0, 0,
            "Internal error in char_queue ('check_in_out_counts' failed)", proc_name,
            SVR_ERR, CHAR_Q_INTERNAL_ERR,TRUE, 0, 1000, FALSE);

    goto exit_give_semaphore;
  }

exit_give_semaphore:

  soc_sand_os_mutex_give(in_char_q->char_buff_sem);
  return valid;

exit_without_give_semaphore:

  return valid;
}


uint8
  check_in_out_counts(
    CHAR_QUEUE* in_char_q
  )
{
  if(in_char_q == NULL)
  {
    return FALSE;
  }

  if((in_char_q->char_in_cnt - in_char_q->char_out_cnt) >= in_char_q->char_buff_size)
  {
    if(in_char_q->char_buff_full == TRUE &&
         ((in_char_q->char_buff_write_ptr + 1) % in_char_q->char_buff_size == in_char_q->char_buff_read_ptr))
    {
      return TRUE;
    }
    else
    {
       return FALSE;
    }
  }
  else if((in_char_q->char_in_cnt - in_char_q->char_out_cnt) !=
                  ((in_char_q->char_buff_write_ptr + in_char_q->char_buff_size - in_char_q->char_buff_read_ptr)
                          % in_char_q->char_buff_size))
  {
    return FALSE;
  }

  return TRUE;

}

void
  char_buff_print(
    CHAR_QUEUE* in_char_q
  )
{
  unsigned long
    index;

  char
    str_to_print[200];

  if(in_char_q == NULL)
  {
    goto exit;
  }

  sal_sprintf(str_to_print,"\r\nChar Queue:\r\n"
          "Buffer size: %lu\r\n"
          "Read pointer: %lu\r\n"
          "Write pointer: %lu\r\n"
          "Is full: %u\r\n"
          "In count: %lu\r\n"
          "Out count: %lu\r\n"
          "The buffer:\r\n",
          in_char_q->char_buff_size,
          in_char_q->char_buff_read_ptr,
          in_char_q->char_buff_write_ptr,
          in_char_q->char_buff_full,
          in_char_q->char_in_cnt,
          in_char_q->char_out_cnt
  );

  send_string_to_screen(str_to_print, FALSE);

  for(index = in_char_q->char_buff_read_ptr;
      index != in_char_q->char_buff_write_ptr;
      index = (index + 1) % in_char_q->char_buff_size)
  {
#if !DUNE_BCM
    if (isprint(in_char_q->char_buff[index]))
#else
    if (1)
#endif
    {
      d_printf("%c ", in_char_q->char_buff[index]);
    }
    else
    {
      d_printf("%d ", in_char_q->char_buff[index]);
    }
  }
  if(in_char_q->char_buff_full == TRUE)
  {
    d_printf("%c ", in_char_q->char_buff[in_char_q->char_buff_write_ptr]);
  }
  d_printf("\n");

exit:

  return;
}





void
  char_buff_test(
  )
{
  int task_nof_writes[10] = {0};
  int task_nof_reads[10] = {0};
  int total_writes = 0;
  int total_reads = 0;

  int i;
  CHAR_QUEUE*
      test_char_buff;

  /*
   * allocate char buff structure
   */
  test_char_buff = soc_sand_os_malloc(sizeof(CHAR_QUEUE),"test_char_buff");

  /*
   * create a char buff of a certain size
   */
  char_buff_create(10, test_char_buff);


  /*
   * create tasks that each one of them will read & write to the buffer
   * and will count its read / write actions
   */
  for(i=1; i<10; ++i)
  {
  }


/*
  do
  {
    nof_active_tasks = 0;
    for(i=0; i<2; ++i)
    {
      res = taskIdVerify(task_id[i]);
      if(taskIdVerify(task_id[i]) == OK)
      {
        ++nof_active_tasks;
      }
    }
    d_printf("\n\nnof_active_tasks: %d\n\n", nof_active_tasks);
    d_taskDelayMicroSec(20000000);
  }
  while(nof_active_tasks > 0);
*/

  d_taskDelayMicroSec(4000000);

  /*
   * After all tasks ended - check:
   * 1. the sum of all writes = in count of the buffer
   * 2. the sum of all reads = out count of the buffer
   * 3. writes - reads = nof chars in the buffer
   */
  for(i=1; i<10; ++i)
  {
    total_writes += task_nof_writes[i];
    total_reads  += task_nof_reads[i];
  }
  if(total_writes != test_char_buff->char_in_cnt ||
     total_reads != test_char_buff->char_out_cnt ||
     check_in_out_counts(test_char_buff) == FALSE)
  {
    d_printf("\n"
           "ERROR - Char buff test tasks!!!\n");
  }
  else
  {
    d_printf("\n"
           "ALL Char buff test tasks ended SUCCESSFULLY...\n");
  }

  d_printf("\n"
         "Total writes: %d\n"
         "Total reads: %d\n",
         total_writes,
         total_reads);

  char_buff_print(test_char_buff);

  soc_sand_os_free(test_char_buff);
}

int get_count(CHAR_QUEUE* in_char_q)
{		
  if (in_char_q->char_buff_write_ptr >= in_char_q->char_buff_read_ptr)
  {
    return (in_char_q->char_buff_write_ptr - in_char_q->char_buff_read_ptr); 
  }
  else
  {
    return (in_char_q->char_buff_size - in_char_q->char_buff_read_ptr + in_char_q->char_buff_write_ptr);
  }
}

