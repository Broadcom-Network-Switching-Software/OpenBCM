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
#include <appl/diag/dpp/tasks_info.h>
/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defi.h>

#include <appl/diag/dpp/utils_defx.h>


/*
 * Object: Task IDs.
 * {
 */
/*
 * Task IDs
 */
static
   int
   Tid_task[NUM_TASKS] ;
/*
 * Initialize all task IDs to -1 and use to indicate whether
 * a true ID has been obtained.
 */
void
   init_task_id(
        void
         )
{
  unsigned short
      us ;
  for (us = 0 ; us < NUM_TASKS ; us++)
  {
    Tid_task[us] = -1 ;
  }
  return ;
}
void
   set_task_id(
         int task_private_index,
         int tid
        )
{
  if (task_private_index<NUM_TASKS)
  {
    Tid_task[task_private_index] = tid ;
  }
  return ;
}
/*
 * Given private tasd id, get OS task id.
 * If none is found, return -1.
 */
int
   get_task_id(
         int task_private_index
        )
{
  int
      ret ;
  if (task_private_index >= NUM_TASKS)
  {
    ret = -1 ;
  }
  else
  {
    ret = Tid_task[task_private_index] ;
  }
  return(ret) ;
}
/*
 * Given private task id, check whether it is alive.
 */
int
   is_task_alive(
         int task_private_index
        )
{
  int
      ret ;
  if (task_private_index >= NUM_TASKS)
  {
    ret = FALSE ;
  }
  else
  {
    if (Tid_task[task_private_index] == -1)
    {
      ret = FALSE ;
    }
    else
    {
      ret = TRUE ;
    }
  }
  return (ret) ;
}
/*
 * Given private tasd id, clear its entry (load -1)
 * to indicate there is no such task any more.
 * This accompabies task deletion.
 */
void
   clear_task_id(
         int task_private_index
        )
{
  /*
   * If internal task id is out of range,
   * ignore this command.
   */
  if (task_private_index < NUM_TASKS)
  {
    Tid_task[task_private_index] = -1 ;
  }
  return ;
}
/*
 * Given OS task id, get private tasdk id.
 * If none is found, return -1.
 */
int
   get_private_task_id(
             int task_id
            )
{
  int
      ret ;
  int
      ii ;
  ret = -1 ;
  for (ii = 0 ; ii < NUM_TASKS ; ii++)
  {
    if (Tid_task[ii]  == task_id)
    {
      ret = ii ;
      break ;
    }
  }
  return(ret) ;
}
/*
 * End object
 * }
 */


