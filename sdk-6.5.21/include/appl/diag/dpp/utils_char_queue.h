/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __UTILS_CHAR_QUEUE_H_INCLUDED__
/* { */
#define __UTILS_CHAR_QUEUE_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif


/*
 * Basic_include_file.
 */
#include <soc/dpp/SAND/Utils/sand_framework.h>

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
#include <appl/diag/dpp/utils_defx.h>
/*
 * INCLUDE FILES:
 * {
 */

#ifdef  __cplusplus
}
#endif

/* CHAR_QUEUE
 *
 * ------*+++++++++++++++++X-----
 *    read_ptr        write_ptr
 */

typedef struct CHAR_QUEUE
{
  /*
   * this is the char queue/buffer -
   * when calling 'create' - will be allocated dynamically
   */
  unsigned char*
    char_buff;

  /*
   * size of buffer
   */
  unsigned long
    char_buff_size;

  /*
   * read/write pointer
   *  the write pointer points to the next available place to write.
   *    (only if the buffer is full, the write pointer points to the last char
   *     that has been written)
   *  the read pointer points to the next char that hasn't been read already.
   */
  unsigned long
    char_buff_read_ptr;
  unsigned long
    char_buff_write_ptr;

  /*
   * true - if it's full,
   *  means if the write pointer is 1 before the read pointer,
   *  and the write pointer points to a char that is already written
   */
  uint8
    char_buff_full;


  /*
   * counts the inputs/outputs of the queue
   */
  unsigned long
    char_in_cnt;
  unsigned long
    char_out_cnt;

  /*
   * semaphore - to control read/write
   */
  sal_mutex_t
    char_buff_sem;

} CHAR_QUEUE;



/*
 * functions implementing the char queue/buffer
 */

/*****************************************************
*NAME
*  char_buff_create
*DATE: 05/MAY/2004
*FUNCTION:
*  Allocates memory for the char buffer according to the requested size in bytes
*  Allocates its semaphore
*  and init all its members to 0 (by calling 'char_buff_clear')
*CALLING SEQUENCE:
*  char_buff_create(
*    int in_size,
*    CHAR_QUEUE* out_char_q
*   )
*INPUT:
*  SOC_SAND_DIRECT:
*    1. int in_size - the requested size in bytes/chars
*    2. CHAR_QUEUE* in_out_char_q - the char queue that we want to create.
*                           this MUST NOT be NULL. we allocate the char buffer inside it
*
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint8 - FALSE if (in_out_char_q == NULL) or (size <= 0)
*           TRUE if Success
*REMARKS:
*  None.
*SEE ALSO:
 */
uint8
  char_buff_create(
    int in_size,
    CHAR_QUEUE* in_out_char_q
  );


/*****************************************************
*NAME
*  char_buff_delete
*DATE: 05/MAY/2004
*FUNCTION:
*  free the memory of the char buffer and puts NULL in it.
*  free the semaphore
*CALLING SEQUENCE:
*  char_buff_delete(
*    CHAR_QUEUE* in_char_q
*   )
*INPUT:
*  SOC_SAND_DIRECT:
*    1. CHAR_QUEUE* in_char_q - the char buffer to be freed
*
*OUTPUT:
*  SOC_SAND_DIRECT:
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  char_buff_delete(
    CHAR_QUEUE* in_char_q
  );


/*****************************************************
*NAME
*  char_buff_is_empty
*DATE: 05/MAY/2004
*FUNCTION:
*  checks if the buffer is empty (read pointer = write pointer)
*  !!! This function uses the char buffer semaphore !!!
*CALLING SEQUENCE:
*  char_buff_is_empty(
*     CHAR_QUEUE* in_char_q
*   )
*INPUT:
*  SOC_SAND_DIRECT:
*    CHAR_QUEUE* in_char_q -
*      the pointer to the char queue
*
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint8 - TRUE if queue is empty (or if buffer is NULL)
*           FALSE if queue not empty
*REMARKS:
*  None.
*SEE ALSO:
 */
uint8
  char_buff_is_empty(
    CHAR_QUEUE* in_char_q
  );


/*****************************************************
*NAME
*  char_buff_clear
*DATE: 05/MAY/2004
*FUNCTION:
*  Clear a char queue/buffer.
*  init to 0 all its memebers besides the size...
*  init also the queue itself to all 0
*  !!! This function uses the char buffer semaphore !!!
*CALLING SEQUENCE:
*  char_buff_clear(
*     CHAR_QUEUE* in_char_q
*   )
*INPUT:
*  SOC_SAND_DIRECT:
*    CHAR_QUEUE* in_char_q -
*      the pointer to the char queue
*
*OUTPUT:
*  SOC_SAND_DIRECT:
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  char_buff_clear(
    CHAR_QUEUE* in_char_q
  );


/*****************************************************
*NAME
*  put_char
*DATE: 05/MAY/2004
*FUNCTION:
*  Insert a char to the queue in the write pointer position
*  If the queue is full - returns error - q full
*  !!! This function uses the char buffer semaphore !!!
*CALLING SEQUENCE:
*  put_char(
*     CHAR_QUEUE* in_char_q,
*    const char in_char
*   )
*INPUT:
*  SOC_SAND_DIRECT:
*    CHAR_QUEUE* in_char_q -
*      the pointer to the char queue.
*    const char in_char - the char to insert.
*
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint8 valid - TRUE if success / FLASE if queue is NULL or Full
*REMARKS:
*  None.
*SEE ALSO:
 */
uint8
  put_char(
    CHAR_QUEUE* in_char_q,
    const char in_char
  );



/*****************************************************
*NAME
*  get_char
*DATE: 05/MAY/2004
*FUNCTION:
*  read a char from the buffer (from the read pointer)
*  !!! This function uses the char buffer semaphore !!!
*CALLING SEQUENCE:
*  get_char(
*     CHAR_QUEUE* in_char_q,
*    char* out_char
*   )
*INPUT:
*  SOC_SAND_DIRECT:
*    CHAR_QUEUE* in_char_q -
*      the pointer to the char queue.
*
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint8 valid - TRUE if success
*                 FLASE if queue is NULL or out_char is NULL or queue is Empty
*  SOC_SAND_INDIRECT:
*    char* out_char - the char that we get. MUST be allocated before. not NULL.
*REMARKS:
*  None.
*SEE ALSO:
 */
uint8
  get_char(
    CHAR_QUEUE* in_char_q,
    char* out_char
  );



/*****************************************************
*NAME
*  char_buff_print
*DATE: 05/MAY/2004
*FUNCTION:
*  prints the char buffer to the stdout
*  prints all the char buffer members
*CALLING SEQUENCE:
*  char_buff_print(
*     CHAR_QUEUE* in_char_q,
*   )
*INPUT:
*  SOC_SAND_DIRECT:
*    CHAR_QUEUE* in_char_q -
*      the pointer to the char queue.
*
*OUTPUT:
*  SOC_SAND_DIRECT:
*  SOC_SAND_INDIRECT:
*    print the buffer to the stdout.
*REMARKS:
*  This function does not protect the queue by taking the semaphore
*  So in case of tasks entering together to the queue, it might print non expected values.
*SEE ALSO:
 */
void
  char_buff_print(
    CHAR_QUEUE* in_char_q
  );



/*****************************************************
*NAME
*  char_buff_test
*DATE: 05/MAY/2004
*FUNCTION:
*  for DEBUG.
*  Creates a buffer. Creates a few tasks that each one writes & reads its own index (1-10)
*   to the buffer. At the end - each one prints how many reads & writes
*   it succeeded and prints the buffer.
*  deletes the buffer
*CALLING SEQUENCE:
*  char_buff_test(
*   )
*INPUT:
*  SOC_SAND_DIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*  SOC_SAND_INDIRECT:
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  char_buff_test(
  );



/* } __UTILS_CHAR_QUEUE_H_INCLUDED__*/
#endif


