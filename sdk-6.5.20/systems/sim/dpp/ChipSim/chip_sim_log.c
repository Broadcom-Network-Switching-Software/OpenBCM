/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include "chip_sim.h"
#include "chip_sim_log.h"

#include <string.h>
#include <stdlib.h>

char *Sim_log = NULL;
int  Sim_log_size=0;
int  Sim_log_ndx=0;

uint8 Spy_low_mem = FALSE;

sal_mutex_t Log_sem_id = 0;

/*****************************************************
*NAME
*  chip_sim_log_malloc
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 23-Sep-02 10:31:06
*FUNCTION:
*  Allocate local-module resources.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int log_char_size
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_log_malloc(
    SOC_SAND_IN int log_char_size
  )
{
  STATUS
    status = OK;
  chip_sim_log_free();

  Sim_log = (char*)CHIP_SIM_MALLOC(log_char_size*sizeof(char),"Sim_log");
  Sim_log_size = log_char_size ;
  Sim_log_ndx =0;

  if (NULL == Sim_log)
  {
    chip_sim_log_free();
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }
  else
  {
    Sim_log[0] = '\0';
  }

  Log_sem_id = soc_sand_os_mutex_create();
  if (0 == Log_sem_id)
  {
    chip_sim_log_free();
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }


FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_log_free
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 23-Sep-02 10:31:19
*FUNCTION:
*  Free local-module resources.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*   STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_log_free(void)
{
  STATUS
    status = OK;
  if (NULL != Sim_log)
  {
    CHIP_SIM_FREE(Sim_log);
  }

  if (0 != Log_sem_id)
  {
    status = soc_sand_os_mutex_delete(Log_sem_id);
  }

  Sim_log = NULL;
  Sim_log_size = 0;
  Sim_log_ndx = 0;
  return status;
}


/*****************************************************
*NAME
*  chip_sim_log_run
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 23-Sep-02 10:31:23
*FUNCTION:
*  Print error to screen and ChipSim log.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN char str[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*   STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_log_run(SOC_SAND_IN char str[])
{
  STATUS
    status = OK;
  unsigned long
    str_size =0;
  uint8
    int_context ;

#ifdef __VXWORKS__
  int_context = intContext();
#else
  static int Print_messages_in_windows = FALSE;

  /*
   * Chips do not have interrupts on MS-windows/linux
   */
  int_context = FALSE;
#endif

  str_size = strlen(str);

  if (!int_context)
  { /* if NOT in interrupt - take the semaphore*/
    if (soc_sand_os_mutex_take(Log_sem_id, (long)SOC_SAND_INFINITE_TIMEOUT) )
    {
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
  }

  if ( (str_size+Sim_log_ndx) < (unsigned long)(Sim_log_size-1))
  {
    /*
     * Print it to the screen.
     */
#if defined(WIN32) || (defined(LINUX) || defined(UNIX))
    if(Print_messages_in_windows)
#endif
      soc_sand_os_printf(
        "[ChipSim-Log] %s", str
      );

    /*
     * Copy ot to local buffer.
     */

    /* copy the string - if have enogh space in the Sim_log buffer*/
    memcpy(Sim_log+Sim_log_ndx, str, str_size);
    Sim_log_ndx += str_size;
    Sim_log[Sim_log_ndx] = '\0' ;
  }
  else
  {/* DO NOT have enogh space in the Sim_log buffer*/
    status = ERROR;
    soc_sand_os_printf(
      "[ChipSim-Log] DO NOT have enogh space in the Sim_log buffer\n"
    );
  }


  if (!int_context)
  {
    if (soc_sand_os_mutex_give(Log_sem_id) )
    { /* if NOT in interrupt - return the semaphore*/
      status = ERROR;
      GOTO_FUNC_EXIT_POINT;
    }
  }

FUNC_EXIT_POINT:
  return status ;
}

/*****************************************************
*NAME
*  chip_sim_log_get_and_lock
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:00:57
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  SHOULD CALL ********** chip_sim_log_unlock_clear() *******
*   After calling this function.
*
*SEE ALSO:
 */
char*
  chip_sim_log_get_and_lock(
    void
  )
{
  char
    *str_p = NULL;

  if (soc_sand_os_mutex_take(Log_sem_id, (long)SOC_SAND_INFINITE_TIMEOUT) )
  {
    GOTO_FUNC_EXIT_POINT;
  }

  str_p = Sim_log ;

FUNC_EXIT_POINT:
  return str_p;
}

/*****************************************************
*NAME
*  chip_sim_log_unlock_clear
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:02
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  chip_sim_log_unlock_clear(
    void
  )
{
  if (soc_sand_os_mutex_give(Log_sem_id) )
  {
    GOTO_FUNC_EXIT_POINT;
  }

  Sim_log_ndx = 0;
  if (NULL != Sim_log)
  {
    Sim_log[0] = '\0';
  }

FUNC_EXIT_POINT:
  return;
}

/*****************************************************
*NAME
*  chip_sim_log_get_spy_low_mem
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:07
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
uint8
  chip_sim_log_get_spy_low_mem(
    void
  )
{
  return Spy_low_mem;
}

/*****************************************************
*NAME
*  chip_sim_log_set_spy_low_mem
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:01:12
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) uint8 b

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  chip_sim_log_set_spy_low_mem(
    uint8 b
  )
{
  Spy_low_mem = b;
}
