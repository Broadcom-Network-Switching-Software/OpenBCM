/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * INCLUDE FILES:
 */
#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
/*
 * Definitions specific to reference system.
 */
#include <usrApp.h>
#include <memDrv.h>
#include <appl/diag/dpp/utils_nvram_configuration.h>

/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defx.h>
/*
 * User interface external include file.
 */
#include <appl/diag/dpp/ui_defx.h>
/*
 * User interface internal include file.
 */
#include <appl/dpp/UserInterface/ui_defi.h>
/*
 * Dune chips include file.
 */
#include <appl/diag/dpp/dune_chips.h>
/* } */
#else
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <taskLib.h>
#include <errnoLib.h>
#include <usrLib.h>
#include <tickLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <logLib.h>
#include <pipeDrv.h>
#include <timers.h>
#include <sigLib.h>
#include <cacheLib.h>
#include <drv/mem/eeprom.h>
#include <usrApp.h>
/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include <private\timerLibP.h>
#include <shellLib.h>
#include <dbgLib.h>
#include <flash28.h>
#include <arch/ppc/vxPpcLib.h>
#include <routeLib.h>
/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defx.h>
/*
 * User interface external include file.
 */
#include <appl/diag/dpp/ui_defx.h>
/*
 * User interface internal include file.
 */
#include <appl/dpp/UserInterface/ui_defi.h>
/*
 * Dune chips include file.
 */
#include <appl/diag/dpp/dune_chips.h>

#include <appl/dpp/UserInterface/ui_pure_defi.h>

/* } */
#endif

#include <appl/diag/dpp/utils_string.h>
#include <appl/diag/dpp/utils_host_board.h>
#include <appl/diag/dpp/utils_line_GFA.h>
#if LINK_FAP21V_LIBRARIES
  #include <appl/diag/dpp/utils_line_GFA_FAP21V.h>
#endif

/*
 * {
 * Forward declaration
 */
int
  get_nv_vals(
    void         **val_ptrs,
    unsigned int size,
    unsigned int offset
  );
/*
 * }
 */
/*
 * Local prototypes
 * {
 */
int
  handle_nv_val(
    void         **val_ptrs,
    unsigned int size,
    unsigned int offset,
    unsigned int action,
    unsigned int *changed
  ) ;
int
  print_nvram_info(
    unsigned int block
  ) ;
void
  print_nv_header(
    void
  ) ;
int
  handle_one_update_item(
    CURRENT_LINE *current_line,
    unsigned int *num_handled_ptr,
    int          param_id,
    char         *param_id_text,
    unsigned long param_type,
    unsigned int  update
  ) ;
int
  update_nvram_param(
    unsigned int update,
    unsigned int param_id,
    unsigned int param_val
  ) ;
/*
 * }
 */
/*
 * Semaphore handling for parallel NVRAM read/write protection.
 * {
 *
 * Semaphore is taken every time parallel NVRAM is accessed and
 * released when access has ended.
 * Use mutual exclusion semaphore rather since we want the
 * 'recursive' option.
 *
 * For all utilities: Returning a non-zero value indicates error.
 *
 * To carry out direct parallel NVRAM access, use 'ignore_nv_sem()'
 */
static
  sal_mutex_t
    Par_nv_sem = NULL ;
unsigned
  int
    Use_par_nv_sem = FALSE ;
unsigned
  int
    get_use_par_nv_sem(
      void
    )
{
  return (Use_par_nv_sem) ;
}
void
  set_use_par_nv_sem(
    unsigned int val
  )
{
  Use_par_nv_sem = val ;
  return ;
}
int
  init_par_nv_sem(
    void
  )
{
  int
    ret ;
  /*
   * Initialize semaphore for Parallel NV memory read/write protection.
   * Use mutual exclusion semaphore since we want the 'recursive' option.
   */
  ret = 0 ;
  Par_nv_sem = (sal_mutex_t)semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE) ;
  if (Par_nv_sem == NULL)
  {
    ret = 1 ;
  }
  else
  {
    Use_par_nv_sem = TRUE ;
  }
  return (ret) ;
}
int
  take_par_nv_sem(
    void
  )
{
  int
    ret ;
  ret = 0 ;
  if (Par_nv_sem == NULL)
  {
    ret = 2 ;
  }
  else
  {
    if (Use_par_nv_sem)
    {
      if (soc_sand_os_mutex_take(Par_nv_sem, (long)SOC_SAND_INFINITE_TIMEOUT) != SOC_SAND_OK)
      {
        ret = 1 ;
      }
    }
  }
  return (ret) ;
}
int
  give_back_par_nv_sem(
    void
  )
{
  int
    ret ;
  ret = 0 ;
  if (Par_nv_sem == NULL)
  {
    ret = 2 ;
  }
  else
  {
    if (Use_par_nv_sem)
    {
      if (soc_sand_os_mutex_give(Par_nv_sem) != SOC_SAND_OK)
      {
        ret = 1 ;
      }
    }
  }
  return (ret) ;
}
/*
 * }
 */
/*
 * Parallel NVRAM utilities related to polling.
 * See eepromWriteBytes()
 * {
 */
/*
 * When 'Tot_par_poll_time' srosses
 * TOT_PAR_POLL_THRESHOLD, monitoring
 * stops.
 */
#define TOT_PAR_POLL_THRESHOLD (unsigned long)(-(2 * (unsigned long)((unsigned short)(-1))))
/*
 * Number of times polling on parallel NVRAM has
 * started.
 */
static
  unsigned
    long
      Num_start_par_poll = 0 ;
/*
 * Number of times polling on parallel NVRAM has
 * ended in success. If not equal to 'Num_start_par_poll'
 * then there have been failures along the way.
 */
static
  unsigned
    long
      Num_end_par_poll = 0 ;
/*
 * Value of timer_2 at starting the last poll.
 */
static
  unsigned
    short
      Start_par_poll_timer2 = 0 ;
/*
 * Value of timer_2 at ending the last poll.
 */
static
  unsigned
    short
      End_par_poll_timer2 = 0 ;
/*
 * Maximum poll time found. In units of timer_t
 * ticks.
 */
static
  unsigned
    short
      Max_par_poll_time = 0 ;
/*
 * Minimum poll time found. In units of timer_t
 * ticks.
 */
static
  unsigned
    short
      Min_par_poll_time = (unsigned short)(-1) ;
/*
 * Total poll time so far. In units of timer_t
 * ticks. To get the average, divide this number
 * by 'Num_end_par_poll'.
 */
static
  unsigned
    long
      Tot_par_poll_time = 0 ;
/*
 * Total number of bytes written into parallel
 * NVRAM so far. To get the average time per
 * byte, divide 'Tot_par_poll_time' by this
 * number.
 */
static
  unsigned
    long
      Tot_par_bytes = 0 ;
/*
 * Flag indicating whether monitoring of polling
 * times can continue. When 'Tot_par_poll_time'
 * srosses TOT_PAR_POLL_THRESHOLD, monitoring
 * stops.
 */
static
  unsigned
    int
      Tot_par_poll_overflow = FALSE ;
/*
 * Counter of the number of polling cycles carried
 * out between starting and ending a full cycle.
 * This counter is incremented once every time
 * 'parallel_nv_poll' is called.
 */
static
  unsigned
    long
      Tot_num_par_polls = 0 ;
/*****************************************************
*NAME
*  par_nv_poll_init
*TYPE: PROC
*DATE: 03/JULY/2002
*FUNCTION:
*  Initialize the monitoring system for parallel NVRAM
*  polling.
*  It is meant for collecting statistics on parallel
*  NV ram polling time.
*CALLING SEQUENCE:
*  par_nv_poll_init()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    All relevant static variables within this object.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    Updated static variables of this object.
*REMARKS:
*  NONE.
*SEE ALSO:
*  eepromWriteBytes() in eeprom.c
 */
void
  par_nv_poll_init(
    void
  )
{
  Num_start_par_poll = 0 ;
  Num_end_par_poll = 0 ;
  Start_par_poll_timer2 = 0 ;
  End_par_poll_timer2 = 0 ;
  Max_par_poll_time = 0 ;
  Min_par_poll_time = (unsigned short)(-1) ;
  Tot_par_poll_time = 0 ;
  Tot_par_poll_overflow = FALSE ;
  Tot_num_par_polls = 0 ;
  Tot_par_bytes = 0 ;
  set_sw_delay_proc(parallel_nv_poll) ;
  set_start_parallel_poll_proc(parallel_nv_start) ;
  set_end_parallel_poll_proc(parallel_nv_end) ;
  return ;
}
/*****************************************************
*NAME
*  get_par_nv_poll_min_time
*TYPE: PROC
*DATE: 03/JULY/2002
*FUNCTION:
*  Get minimal polling time from the monitoring
*  system for parallel NVRAM polling.
*CALLING SEQUENCE:
*  get_par_nv_poll_min_time()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Relevant static variables within this object.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned short -
*      Minimal polling time, in units of
*      timer2 ticks.
*  SOC_SAND_INDIRECT:
*    None
*REMARKS:
*  NONE.
*SEE ALSO:
*  eepromWriteBytes() in eeprom.c
 */
unsigned
  short
    get_par_nv_poll_min_time(
      void
  )
{
  unsigned
    short
      ret ;
  ret = Min_par_poll_time ;
  return (ret) ;
}
/*****************************************************
*NAME
*  get_par_nv_poll_max_time
*TYPE: PROC
*DATE: 03/JULY/2002
*FUNCTION:
*  Get maximal polling time from the monitoring
*  system for parallel NVRAM polling.
*CALLING SEQUENCE:
*  get_par_nv_poll_max_time()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Relevant static variables within this object.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned short -
*      Maximal polling time, in units of
*      timer2 ticks.
*  SOC_SAND_INDIRECT:
*    None
*REMARKS:
*  NONE.
*SEE ALSO:
*  eepromWriteBytes() in eeprom.c
 */
unsigned
  short
    get_par_nv_poll_max_time(
      void
  )
{
  unsigned
    short
      ret ;
  ret = Max_par_poll_time ;
  return (ret) ;
}
/*****************************************************
*NAME
*  get_par_nv_poll_cycles
*TYPE: PROC
*DATE: 03/JULY/2002
*FUNCTION:
*  Get number of full polling cycles (one count
*  per writing and getting to read the correct
*  value written) for parallel NVRAM polling.
*CALLING SEQUENCE:
*  get_par_nv_poll_cycles()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Relevant static variables within this object.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned long -
*      number of full polling cycles (one count
*      per writing and getting to read the correct
*      value written).
*  SOC_SAND_INDIRECT:
*    None
*REMARKS:
*  NONE.
*SEE ALSO:
*  eepromWriteBytes() in eeprom.c
 */
unsigned
  long
    get_par_nv_poll_cycles(
      void
  )
{
  unsigned
    long
      ret ;
  ret = Num_end_par_poll ;
  return (ret) ;
}
/*****************************************************
*NAME
*  get_par_nv_poll_starts
*TYPE: PROC
*DATE: 03/JULY/2002
*FUNCTION:
*  Get number of starts of full polling cycles
*  (one count per writing and getting to read
*  the correct value written) for parallel
*  NVRAM polling. If number of starts is not
*  equal to the number of endings (cycles) then
*  some error has been encountered.
*CALLING SEQUENCE:
*  get_par_nv_poll_starts()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Relevant static variables within this object.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned long -
*      Number of starts of full polling cycles
*      (one count per writing and getting to read
*      the correct value written).
*  SOC_SAND_INDIRECT:
*    None
*REMARKS:
*  NONE.
*SEE ALSO:
*  eepromWriteBytes() in eeprom.c
 */
unsigned
  long
    get_par_nv_poll_starts(
      void
  )
{
  unsigned
    long
      ret ;
  ret = Num_start_par_poll ;
  return (ret) ;
}
/*****************************************************
*NAME
*  get_par_nv_poll_avg_time
*TYPE: PROC
*DATE: 03/JULY/2002
*FUNCTION:
*  Get average polling time (from starting a poll
*  cycle untill NVRAM is ready) from the monitoring
*  system for parallel NVRAM polling.
*CALLING SEQUENCE:
*  get_par_nv_poll_avg_time()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Relevant static variables within this object.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned long -
*      Calculated average (from total polling times,
*      and total count, so far). In units of timer2
*      ticks.
*  SOC_SAND_INDIRECT:
*    None
*REMARKS:
*  NONE.
*SEE ALSO:
*  eepromWriteBytes() in eeprom.c
 */
unsigned
  long
    get_par_nv_poll_avg_time(
      void
  )
{
  unsigned
    long
      ret ;
  ret = (Tot_par_poll_time / Num_end_par_poll) ;
  return (ret) ;
}
/*****************************************************
*NAME
*  get_par_nv_poll_avg_byte
*TYPE: PROC
*DATE: 04/JULY/2002
*FUNCTION:
*  Get average polling time per one byte from the
*  monitoring system for parallel NVRAM polling.
*CALLING SEQUENCE:
*  get_par_nv_poll_avg_byte()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Relevant static variables within this object.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned long -
*      Calculated average (from total polling times,
*      and total bytes written, so far). In micro
*      seconds.
*  SOC_SAND_INDIRECT:
*    None
*REMARKS:
*  NONE.
*SEE ALSO:
*  eepromWriteBytes() in eeprom.c
 */
unsigned
  long
    get_par_nv_poll_avg_byte(
      void
  )
{
  unsigned
    long
      ret ;
  ret = (timer_2_to_micro(Tot_par_poll_time) / Tot_par_bytes) ;
  return (ret) ;
}
/*****************************************************
*NAME
*  get_par_nv_poll_avg_num
*TYPE: PROC
*DATE: 03/JULY/2002
*FUNCTION:
*  Get average number of poll tests from the monitoring
*  system for parallel NVRAM polling.
*CALLING SEQUENCE:
*  get_par_nv_poll_avg_num()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Relevant static variables within this object.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned long -
*      Calculated average (from total number of polls,
*      and total count, so far). Pure number.
*  SOC_SAND_INDIRECT:
*    None
*REMARKS:
*  NONE.
*SEE ALSO:
*  eepromWriteBytes() in eeprom.c
 */
unsigned
  long
    get_par_nv_poll_avg_num(
      void
  )
{
  unsigned
    long
      ret ;
  ret = (Tot_num_par_polls / Num_end_par_poll) ;
  return (ret) ;
}
/*****************************************************
*NAME
*  parallel_nv_end
*TYPE: PROC
*DATE: 26/JUNE/2002
*FUNCTION:
*  This function is set, at application startup
*  (starterProc) to be called when NV mem ends
*  a polling cycle for completion of some long
*  action.
*  It is meant for collecting statistics on
*  parallel NV ram polling time.
*CALLING SEQUENCE:
*  parallel_nv_end(count)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long  count -
*      This is the number of bytes written into
*      NV ram for which polling has been carried out
*      (for eeproms which have 'page' mode, this
*      could be more than 1).
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
*  eepromWriteBytes() in eeprom.c
 */
int
  parallel_nv_end(
    unsigned long count
  )
{
  int
    ret ;
  unsigned
    short
      poll_time ;
  End_par_poll_timer2 = read_timer_2() ;
  if (timer_2_to_micro(Tot_par_poll_time) >= TOT_PAR_POLL_THRESHOLD)
  {
    ret = 1 ;
    Tot_par_poll_overflow = TRUE ;
  }
  else
  {
    ret = 0 ;
    Num_end_par_poll++ ;
    poll_time = (End_par_poll_timer2 - Start_par_poll_timer2) ;
    Tot_par_poll_time += (unsigned long)poll_time ;
    Tot_par_bytes += count ;
    if (poll_time > Max_par_poll_time)
    {
      Max_par_poll_time = poll_time ;
    }
    else if (poll_time < Min_par_poll_time)
    {
      Min_par_poll_time = poll_time ;
    }
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  parallel_nv_start
*TYPE: PROC
*DATE: 26/JUNE/2002
*FUNCTION:
*  This function is set, at application startup
*  (starterProc) to be called when NV mem starts
*  a polling cycle for completion of some long
*  action.
*  It is meant for collecting statistics on
*  parallel NV ram polling time.
*CALLING SEQUENCE:
*  parallel_nv_start(address)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long  address -
*      This is the physical address on NV ram
*      which is to be polled.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
*  eepromWriteBytes() in eeprom.c
 */
int
  parallel_nv_start(
    unsigned long address
  )
{
  int
    ret ;
  if (timer_2_to_micro(Tot_par_poll_time) >= TOT_PAR_POLL_THRESHOLD)
  {
    ret = 1 ;
    Tot_par_poll_overflow = TRUE ;
  }
  else
  {
    ret = 0 ;
    Num_start_par_poll++ ;
    Start_par_poll_timer2 = read_timer_2() ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  parallel_nv_poll
*TYPE: PROC
*DATE: 26/JUNE/2002
*FUNCTION:
*  This function is set, at application startup,
*  (starterProc) to be called when NV mem is polled
*  for completion of some long action.
*  In the application level, we release CPU to
*  allow other tasks to execute while NV me is
*  being polled.
*CALLING SEQUENCE:
*  parallel_nv_poll(delay)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long  delay -
*      This is a measure of the delay required by
*      the caller. This procedure will simply
*      execute a software loop.
*  SOC_SAND_INDIRECT:
*    None
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
*  sw_delay() in eeprom.c
 */
int
  parallel_nv_poll(
    unsigned long delay
  )
{
  static
    volatile
      unsigned
        int
          uj ;
  int
    ret ;
  ret = 0 ;
  for (uj = 0 ; uj < (delay * delay) ; uj++)
  {
    if ((uj % 4) == 0)
    {
      d_taskDelay(0) ;
    }
  }
  if (Tot_par_poll_time < TOT_PAR_POLL_THRESHOLD)
  {
    Tot_num_par_polls++ ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  display_poll_statistics
*TYPE: PROC
*DATE: 03/JULY/2002
*FUNCTION:
*  Display statistics collected so far from monitoring
*  system of parallel NVRAM plooing.
*CALLING SEQUENCE:
*  display_poll_statistics()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Relevant static variables within this object.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    Displayed statistics.
*REMARKS:
*  NONE.
*SEE ALSO:
*  sw_delay() in eeprom.c
 */
void
  display_poll_statistics(
    void
  )
{
  char
    err_msg[9 * 80] ;
  unsigned
    int
      err_msg_index ;
  unsigned
    short
      us ;
  unsigned
    long
      num_poll_starts,
      avg_time_micro,
      byte_time_micro,
      min_time_micro,
      max_time_micro ;
  err_msg[0] = 0 ;
  err_msg_index = 0 ;
  us = (unsigned short)get_par_nv_poll_avg_time() ;
  avg_time_micro = timer_2_to_micro(us) ;
  min_time_micro = timer_2_to_micro(get_par_nv_poll_min_time()) ;
  max_time_micro = timer_2_to_micro(get_par_nv_poll_max_time()) ;
  byte_time_micro = get_par_nv_poll_avg_byte() ;
  sal_sprintf(&err_msg[err_msg_index],
    "---------- Parallel NVRAM polling statistics ----------\r\n"
    "From changing memory until verified -\r\n"
    "  Average page time       : %5lu microsec.\r\n"
    "  Minimal page time       : %5lu microsec.\r\n"
    "  Maximal page time       : %5lu microsec.\r\n"
    "  Average time per byte   : %5lu microsec.\r\n"
    "  Num. cycles (pages or\r\n"
    "    part pages)           : %5lu\r\n"
    "For one cycle of changing\r\n"
    "  memory until verified -\r\n"
    "  Average polls           : %5lu per cycle\r\n",
    avg_time_micro,
    min_time_micro,
    max_time_micro,
    byte_time_micro,
    get_par_nv_poll_cycles(),
    get_par_nv_poll_avg_num()
    ) ;
  err_msg_index += strlen(&err_msg[err_msg_index]) ; ;
  send_string_to_screen(err_msg,TRUE) ;
  err_msg_index = 0 ; ;
  num_poll_starts = get_par_nv_poll_starts() ;
  if (num_poll_starts != get_par_nv_poll_cycles())
  {
    sal_sprintf(&err_msg[err_msg_index],
      "NOTE:\r\n"
      "Number of started cycles (%lu) is not equal to number of\r\n"
      "completed cycles (%lu). Some error has been detected!\r\n",
      num_poll_starts,
      get_par_nv_poll_cycles()
      ) ;
    err_msg_index += strlen(&err_msg[err_msg_index]) ;
  }
  if (Tot_par_poll_overflow)
  {
    sal_sprintf(&err_msg[err_msg_index],
      "NOTE:\r\n"
      "Statistics collection has been stopped since total time (%lu)\r\n"
      "has crossed the overflow point (%lu)!\r\n",
      Tot_par_poll_time,
      TOT_PAR_POLL_THRESHOLD
      ) ;
    err_msg_index += strlen(&err_msg[err_msg_index]) ;
  }
  if (err_msg_index)
  {
    send_string_to_screen(err_msg,TRUE) ;
  }
  return ;
}
/*
 * }
 */
#define NV_RAM_READ_B(x)      ((volatile)eepromReadByte(x))
#define NV_RAM_WRITE_B(x,y,z) eepromWriteBytes(x,(unsigned char *)y,(unsigned int)z)
static
  EE_AREA
    Ee_image_runtime ;
static
  SERIAL_EE_AREA
    Serial_ee_image_runtime ;
/*
 * Utilities related to event log block in NV ram
 * (block_05)
 * {
 */
/*****************************************************
*NAME
*  index_num_from_nv
*TYPE: PROC
*DATE: 26/JUNE/2002
*FUNCTION:
*  GET VALUE OF NEXT_IN_INDEX AND NUM_EVENTS_LOGGED,
*  WHICH INDICATE HOW MANY EVENTS ARE LOGGED AND WHERE
*  IN NVRAM LOG 'FILE', FROM NV RAM.
*CALLING SEQUENCE:
*  index_num_from_nv(next_in_index_ptr,
                      num_events_logged_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  *next_in_index_ptr -
*      This procedure loads the pointed address with
*      value of NEXT_IN_INDEX value, as stored in
*      NV ram.
*    unsigned char  *num_events_logged_ptr -
*      This procedure loads the pointed address with
*      value of NUM_EVENTS_LOGGED value, as stored
*      in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  index_num_from_nv(
    unsigned char  *next_in_index_ptr,
    unsigned char  *num_events_logged_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_05.
                        un_ee_block_05.block_05.next_in_index)) ;
  status =
    nvRamGet(next_in_index_ptr,sizeof(*next_in_index_ptr),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
  offset = (unsigned int)((char *)&(ee_area->ee_block_05.
                        un_ee_block_05.block_05.num_events_logged)) ;
  status =
    nvRamGet(num_events_logged_ptr,sizeof(*num_events_logged_ptr),(int)offset) ;
  if (status == ERROR)
  {
    ret = 2 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  index_num_to_nv
*TYPE: PROC
*DATE: 26/JUNE/2002
*FUNCTION:
*  SET VALUES OF NEXT_IN_INDEX AND NUM_EVENTS_LOGGED,
*  WHICH INDICATE HOW MANY EVENTS ARE LOGGED AND WHERE
*  IN NVRAM LOG 'FILE', INTO NV RAM.
*CALLING SEQUENCE:
*  index_num_to_nv(next_in_index,num_events_logged)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  next_in_index -
*      This procedure loads input value into
*      location of NEXT_IN_INDEX value in NV ram.
*    unsigned char  num_events_logged -
*      This procedure loads input value into
*      location of NUM_EVENTS_LOGGED value
*      in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be updated.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  index_num_to_nv(
    unsigned char  next_in_index,
    unsigned char  num_events_logged
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_05.
                        un_ee_block_05.block_05.next_in_index)) ;
  status =
    nvRamSet((char *)&next_in_index,sizeof(next_in_index),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
  offset = (unsigned int)((char *)&(ee_area->ee_block_05.
                        un_ee_block_05.block_05.num_events_logged)) ;
  status =
    nvRamSet((char *)&num_events_logged,sizeof(num_events_logged),(int)offset) ;
  if (status == ERROR)
  {
    ret = 2 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  event_item_from_nv
*TYPE: PROC
*DATE: 26/JUNE/2002
*FUNCTION:
*  GET A FULL EVENT_LOG_ITEM STRUCTURE (ONE
*  ELEMENT IN THE 'EVENT_LOG_ITEM' ARRAY), WHICH
*  CONTAINS ALL INFORMATION RELATED TO
*  ONE EVENT AS STORED IN NV RAM.
*CALLING SEQUENCE:
*  event_item_from_nv(
*    event_item_ptr,event_item_index)
*INPUT:
*  SOC_SAND_DIRECT:
*    EVENT_LOG_ITEM  *event_item_ptr -
*      This procedure loads the pointed address
*      (EVENT_LOG_ITEM structure) with values of a
*      full event_log_item element (whose index
*      is event_item_index, as stored in NV ram.
*    unsigned char  event_item_index -
*      Index of required element (see event_item_ptr)
*      from NV ram. It is implicitly assumed that
*      there are, at the most, 255 elements.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  event_item_from_nv(
    EVENT_LOG_ITEM  *event_item_ptr,
    unsigned char   event_item_index
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  unsigned
    char
      num_event_log_items ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  num_event_log_items =
    sizeof(ee_area->ee_block_05.un_ee_block_05.block_05.
      event_log_item) /
        sizeof(ee_area->ee_block_05.un_ee_block_05.block_05.
              event_log_item[0]) ;
  if (event_item_index >= num_event_log_items)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
  offset = (unsigned int)((char *)&(ee_area->ee_block_05.
                        un_ee_block_05.block_05.
                        event_log_item[event_item_index])) ;
  status =
    nvRamGet((char *)event_item_ptr,sizeof(*event_item_ptr),(int)offset) ;
  if (status == ERROR)
  {
    ret = 2 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  event_item_to_nv
*TYPE: PROC
*DATE: 26/JUNE/2002
*FUNCTION:
*  SET A FULL EVENT_LOG_ITEM STRUCTURE (ONE
*  ELEMENT IN THE 'EVENT_LOG_ITEM' ARRAY), WHICH
*  CONTAINS ALL INFORMATION RELATED TO
*  ONE EVENT, IN NV RAM.
*CALLING SEQUENCE:
*  event_item_to_nv(
*    event_item_ptr,event_item_index)
*INPUT:
*  SOC_SAND_DIRECT:
*    EVENT_LOG_ITEM  *event_item_ptr -
*      This procedure reads bytes from pointed address
*      (EVENT_LOG_ITEM structure) and loads
*      them into NV ram.
*    unsigned char  event_item_index -
*      Index of targeted element (see event_item_ptr)
*      in NV ram. It is implicitly assumed that
*      there are, at the most, 255 elements.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  event_item_to_nv(
    EVENT_LOG_ITEM  *event_item_ptr,
    unsigned char   event_item_index
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  unsigned
    int
      sizeof_element ;
  unsigned
    char
      num_event_log_items ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  sizeof_element =
    sizeof(ee_area->ee_block_05.un_ee_block_05.
              block_05.event_log_item[0]) ;
  num_event_log_items =
    sizeof(ee_area->ee_block_05.un_ee_block_05.block_05.
      event_log_item) / sizeof_element ;
  if (event_item_index >= num_event_log_items)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
  offset = (unsigned int)((char *)&(ee_area->ee_block_05.
                        un_ee_block_05.block_05.
                        event_log_item[event_item_index])) ;
  status =
    nvRamSet((char *)event_item_ptr,sizeof_element,(int)offset) ;
  if (status == ERROR)
  {
    ret = 2 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  next_event_item_to_nv
*TYPE: PROC
*DATE: 26/JUNE/2002
*FUNCTION:
*  LOAD A FULL EVENT_LOG_ITEM STRUCTURE, ONE
*  ELEMENT IN THE 'EVENT_LOG_ITEM' ARRAY, AT THE
*  NEXT AVAILABLE LOCATION IN THAT ARRAY, IN NV RAM.
*CALLING SEQUENCE:
*  next_event_item_to_nv(event_item_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    EVENT_LOG_ITEM  *event_item_ptr -
*      This procedure reads bytes from pointed address
*      (EVENT_LOG_ITEM structure) and loads
*      them into NV ram at the next available
*      entry.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read or written.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  next_event_item_to_nv(
    EVENT_LOG_ITEM  *event_item_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  unsigned
    int
      sizeof_element ;
  unsigned
    char
      num_event_log_items ;
  BLOCK_05
    block_05 ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  sizeof_element =
    sizeof(ee_area->ee_block_05.un_ee_block_05.
              block_05.event_log_item[0]) ;
  num_event_log_items =
    (unsigned char)
      (sizeof(ee_area->ee_block_05.un_ee_block_05.block_05.
        event_log_item) / sizeof_element) ;
  /*
   * Get the full array.
   */
  offset = (unsigned int)((char *)&(ee_area->ee_block_05.
                        un_ee_block_05.block_05)) ;
  status =
    nvRamGet((char *)&block_05,sizeof(block_05),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
  memcpy(
    &block_05.event_log_item[block_05.next_in_index],
    event_item_ptr,sizeof_element) ;
  block_05.next_in_index++ ;
  if (block_05.next_in_index >= num_event_log_items)
  {
    block_05.next_in_index = 0 ;
  }
  if (block_05.num_events_logged < num_event_log_items)
  {
    /*
     * Enter if event log array in NVRAM is NOT full.
     * Just add new entry at next available location
     * and update number loaded.
     */
    block_05.num_events_logged++ ;
  }
  /*
   * 'offset' is still loaded by offset of event log
   * array in NVRAM.
   */
  status =
    nvRamSet((char *)&block_05,sizeof(block_05),(int)offset) ;
  if (status == ERROR)
  {
    ret = 2 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  next_event_item_from_nv
*TYPE: PROC
*DATE: 30/JUNE/2002
*FUNCTION:
*  GET A FULL EVENT_LOG_ITEM STRUCTURE, ONE
*  ELEMENT IN THE 'EVENT_LOG_ITEM' ARRAY, FROM THE
*  NEXT INDICATED LOCATION IN THE NVRAM EVENT LOG
*  ARRAY. (THIS IS LIKE SNMP'S GET_NEXT).
*CALLING SEQUENCE:
*  next_event_item_from_nv(event_item_ptr,index_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    EVENT_LOG_ITEM  *event_item_ptr -
*      This procedure reads bytes from specified
*      index (EVENT_LOG_ITEM structure) in the
*      nv ram log array into the indicated
*      memory location.
*    int             *index_ptr -
*      As input:
*      This pointer points to the index of the
*      required item (in the nv ram log array). If
*      pointed value is negative then caller requires
*      the first available entry.
*      As output:
*      This pointer points to the index of the
*      item (in the nv ram log array) following the
*      item specified as input. If input is negative
*      then output is the index of the first entry
*      If output is negative then there are no
*      more entries beyond specified input index
*      (i.e. either there are no valid entries at
*      all or input index is exactly that of the
*      last entry).
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  next_event_item_from_nv(
    EVENT_LOG_ITEM  *event_item_ptr,
    int             *index_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  STATUS
    status ;
  unsigned
    int
      index,
      offset,
      sizeof_element,
      first_loaded_index,
      last_loaded_index ;
  unsigned
    char
      num_event_log_items ;
  BLOCK_05
    block_05 ;
  ret = 0 ;
  index = (unsigned int)(*index_ptr) ;
  ee_area = (EE_AREA *)0 ;
  sizeof_element =
    sizeof(ee_area->ee_block_05.un_ee_block_05.
              block_05.event_log_item[0]) ;
  num_event_log_items =
    (unsigned char)
      (sizeof(ee_area->ee_block_05.un_ee_block_05.block_05.
        event_log_item) / sizeof_element) ;
  /*
   * Get the full array.
   */
  offset = (unsigned int)((char *)&(ee_area->ee_block_05.
                        un_ee_block_05.block_05)) ;
  status =
    nvRamGet((char *)&block_05,sizeof(block_05),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    /*
     * Just making sure...
     */
    *index_ptr = -3 ;
    goto tifn_exit ;
  }
  if (block_05.num_events_logged == 0)
  {
    /*
     * There are no entries to read. Just quit.
     */
    *index_ptr = -1 ;
    goto tifn_exit ;
  }
  /*
   * Reach this point if there are some entries stored
   * in NVRAM array.
   */
  /*
   * Mark the first and last index. If last index is
   * smaller than first, increase modulu number of
   * elements in array.
   */
  if (block_05.num_events_logged > block_05.next_in_index)
  {
    /*
     * Enter if events logged in have wrapped around
     * the end of the array.
     */
    first_loaded_index =
      (unsigned int)(num_event_log_items -
            (block_05.num_events_logged - block_05.next_in_index))  ;
    if (block_05.next_in_index)
    {
      last_loaded_index = (unsigned int)(block_05.next_in_index - 1) ;
    }
    else
    {
      last_loaded_index = (unsigned int)(num_event_log_items - 1) ;
    }
  }
  else
  {
    /*
     * Enter if events logged in have NOT wrapped around
     * the end of the array.
     */
    first_loaded_index =
      (unsigned int)(block_05.next_in_index - block_05.num_events_logged)  ;
    last_loaded_index = (unsigned int)(block_05.next_in_index - 1) ;
  }
  if ((int)index < 0)
  {
    index = first_loaded_index ;
  }
  else
  {
    if (index == last_loaded_index)
    {
      /*
       * Caller required index next to last.
       * There are no entries to read. Just quit.
       */
      *index_ptr = -2 ;
      goto tifn_exit ;
    }
    /*
     * Now calculate the next available index.
     */
    index++ ;
    if (index >= num_event_log_items)
    {
      index = 0 ;
    }
    if (last_loaded_index < first_loaded_index)
    {
      if ((index > last_loaded_index) && (index < first_loaded_index))
      {
        index = first_loaded_index ;
      }
    }
    else
    {
      if ((index > last_loaded_index) || (index < first_loaded_index))
      {
        index = first_loaded_index ;
      }
    }
  }
  *event_item_ptr = block_05.event_log_item[index] ;
  *index_ptr = (int)index ;
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  block_05_from_nv
*TYPE: PROC
*DATE: 27/JUNE/2002
*FUNCTION:
*  GET THE FULL BLOCK_05 STRUCTURE, WHICH
*  CONTAINS EVENT LOG FILE.
*CALLING SEQUENCE:
*  block_05_from_nv(block_05_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    void  *block_05_ptr -
*      This procedure loads the pointed address
*      (BLOCK_05 structure) with value of the
*      full block_05, which contains event log
*      'file', as stored in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  block_05_from_nv(
    BLOCK_05  *block_05_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_05.
                        un_ee_block_05.block_05)) ;
  status =
    nvRamGet((char *)block_05_ptr,sizeof(*block_05_ptr),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  block_05_to_nv
*TYPE: PROC
*DATE: 27/JUNE/2002
*FUNCTION:
*  LOAD THE FULL BLOCK_05 STRUCTURE, WHICH
*  CONTAINS EVENT LOG 'FILE'.
*CALLING SEQUENCE:
*  block_05_to_nv(block_05_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    void  *block_05_ptr -
*      This procedure loads data from pointed
*      address (BLOCK_05 structure) into the full
*      block_05, which contains event log file,
*      in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  block_05_to_nv(
    BLOCK_05  *block_05_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_05.
                        un_ee_block_05.block_05)) ;
  status =
    nvRamSet((char *)block_05_ptr,sizeof(BLOCK_05),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*
 * }
 */
/*
 * Utilities related to reset exception block in NV ram
 * (block_04)
 * {
 */
/*****************************************************
*NAME
*  new_n_valid_from_nv
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  GET VALUE OF NEW_N_VALID FLAG, WHICH INDICATES WHETHER
*  RESET EXCEPTION NVRAM BLOCK CONTAINS NEW AND MEANINGFUL
*  INFORMATION, FROM NV RAM.
*CALLING SEQUENCE:
*  new_n_valid_from_nv(new_n_valid_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  *new_n_valid_ptr -
*      This procedure loads the pointed address with
*      value of NEW_N_VALID flag, as stored in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  new_n_valid_from_nv(
    unsigned char  *new_n_valid_ptr
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_04.
                        un_ee_block_04.block_04.new_n_valid)) ;
  status =
    nvRamGet(new_n_valid_ptr,sizeof(*new_n_valid_ptr),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  new_n_valid_to_nv
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  SET VALUE OF NEW_N_VALID FLAG, WHICH INDICATES WHETHER
*  RESET EXCEPTION NVRAM BLOCK CONTAINS NEW AND MEANINGFUL
*  INFORMATION, INTO NV RAM.
*CALLING SEQUENCE:
*  new_n_valid_to_nv(new_n_valid)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  new_n_valid -
*      This procedure loads input value into
*      location of NEW_N_VALID flag in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be updated.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  new_n_valid_to_nv(
    unsigned char  new_n_valid
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_04.
                        un_ee_block_04.block_04.new_n_valid)) ;
  status =
    nvRamSet((char *)&new_n_valid,sizeof(new_n_valid),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  handler_processing_time_from_nv
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  GET VALUE OF HANDLER_PROCESSING_TIME FLAG,
*  WHICH INDICATES HOW MUCH TIME THE HANDLING OF
*  RESET EXCEPTION HAS TAKEN, FROM NV RAM.
*CALLING SEQUENCE:
*  handler_processing_time_from_nv(handler_processing_time_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  *handler_processing_time_ptr -
*      This procedure loads the pointed address with
*      value of HANDLER_PROCESSING_TIME, as
*      stored in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  handler_processing_time_from_nv(
    unsigned long  *handler_processing_time_ptr
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset =
    (unsigned int)((char *)&(ee_area->ee_block_04.
                   un_ee_block_04.block_04.handler_processing_time)) ;
  status =
    nvRamGet((char *)handler_processing_time_ptr,
                sizeof(*handler_processing_time_ptr),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  handler_processing_time_to_nv
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  SET VALUE OF HANDLER_PROCESSING_TIME FLAG, WHICH
*  INDICATES HOW MUCH TIME RESET EXCEPTION HANDLING
*  HAS TAKEN, INTO NV RAM.
*CALLING SEQUENCE:
*  handler_processing_time_to_nv(handler_processing_time)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  handler_processing_time -
*      This procedure loads input value into
*      location of HANDLER_PROCESSING_TIME
*      in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be updated.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  handler_processing_time_to_nv(
    unsigned long  handler_processing_time
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_04.
                        un_ee_block_04.block_04.handler_processing_time)) ;
  status =
    nvRamSet((char *)&handler_processing_time,
                  sizeof(handler_processing_time),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  error_identifier_from_nv
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  GET VALUE OF ERROR_IDENTIFIER VARIABLE, WHICH
*  IDENTIFIES THE TYPE OF ERROR EVENT RELATED TO
*  RESET EXCEPTION AS STORED IN NV RAM.
*CALLING SEQUENCE:
*  error_identifier_from_nv(error_identifier_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long  *error_identifier_ptr -
*      This procedure loads the pointed address with
*      value of ERROR_IDENTIFIER flag, as stored in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  error_identifier_from_nv(
    unsigned long  *error_identifier_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  char
    block_data[sizeof(char) + 1] ;
  unsigned
    long
      error_identifier ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_04.
                        un_ee_block_04.block_04.error_identifier)) ;
  status =
    nvRamGet(block_data,sizeof(error_identifier),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
  error_identifier = *((unsigned long *)block_data) ;
  *error_identifier_ptr = error_identifier ;
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  error_identifier_to_nv
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  SET VALUE OF ERROR_IDENTIFIER VARIABLE, WHICH
*  IDENTIFIES THE TYPE OF ERROR EVENT RELATED TO
*  RESET EXCEPTION INTO NV RAM.
*CALLING SEQUENCE:
*  error_identifier_to_nv(error_identifier)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  error_identifier -
*      This procedure loads input value into
*      location of ERROR_IDENTIFIER variable in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be updated.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  error_identifier_to_nv(
    unsigned long  error_identifier
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_04.
                        un_ee_block_04.block_04.error_identifier)) ;
  status =
    nvRamSet((char *)&error_identifier,sizeof(error_identifier),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  exception_counter_from_nv
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  GET VALUE OF EXCEPTION_COUNTER FROM NV RAM, WHICH
*  INDICATES HOW MANY RESET EXCEPTIONS WERE DETECTED
*  BEFORE INFORMATION HAS BEEN RETRIEVED (AT NEXT
*  STARTUP).
*CALLING SEQUENCE:
*  exception_counter_from_nv(exception_counter_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  *exception_counter_ptr -
*      This procedure loads the pointed address with
*      value of EXCEPTION_COUNTER, as stored in
*      NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  exception_counter_from_nv(
    unsigned char  *exception_counter_ptr
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    char
      exception_counter ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_04.
                        un_ee_block_04.block_04.exception_counter)) ;
  status =
    nvRamGet(&exception_counter,sizeof(exception_counter),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
  *exception_counter_ptr = exception_counter ;
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  block_04_from_nv
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  GET THE FULL BLOCK_04 STRUCTURE, WHICH
*  CONTAINS ALL INFORMATION RELATED TO
*  RESET EXCEPTION AS STORED IN NV RAM.
*CALLING SEQUENCE:
*  block_04_from_nv(block_04_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    void  *block_04_ptr -
*      This procedure loads the pointed address
*      (BLOCK_04 structure) with value of the
*      full block_04, which contains all
*      information related to reset exception,
*      as stored in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  block_04_from_nv(
    void  *block_04_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_04.
                        un_ee_block_04.block_04)) ;
  status =
    nvRamGet(block_04_ptr,sizeof(BLOCK_04),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  block_04_to_nv
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  LOAD THE FULL BLOCK_04 STRUCTURE, WHICH
*  CONTAINS ALL INFORMATION RELATED TO
*  RESET EXCEPTION INTO NV RAM.
*CALLING SEQUENCE:
*  block_04_to_nv(block_04_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    void  *block_04_ptr -
*      This procedure loads data from pointed
*      address (BLOCK_04 structure) into the full
*      block_04, which contains all information
*      related to reset exception, in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  block_04_to_nv(
    void  *block_04_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_04.
                        un_ee_block_04.block_04)) ;
  status =
    nvRamSet(block_04_ptr,sizeof(BLOCK_04),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*
 * }
 */
/*
 * Utilities related to software emulation exception block in NV ram
 * (block_07)
 * {
 */
/*****************************************************
*NAME
*  software_exception_new_n_valid_from_nv
*TYPE: PROC
*DATE: 27/APR/2003
*FUNCTION:
*  GET VALUE OF NEW_N_VALID FLAG, WHICH INDICATES WHETHER
*  RESET EXCEPTION NVRAM BLOCK CONTAINS NEW AND MEANINGFUL
*  INFORMATION, FROM NV RAM.
*CALLING SEQUENCE:
*  software_exception_new_n_valid_from_nv(new_n_valid_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  *new_n_valid_ptr -
*      This procedure loads the pointed address with
*      value of NEW_N_VALID flag, as stored in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  software_exception_new_n_valid_from_nv(
    unsigned char  *new_n_valid_ptr
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_07.
                        un_ee_block_07.block_07.new_n_valid)) ;
  status =
    nvRamGet(new_n_valid_ptr,sizeof(*new_n_valid_ptr),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  software_exception_new_n_valid_to_nv
*TYPE: PROC
*DATE: 27/APR/2003
*FUNCTION:
*  SET VALUE OF NEW_N_VALID FLAG, WHICH INDICATES WHETHER
*  RESET EXCEPTION NVRAM BLOCK CONTAINS NEW AND MEANINGFUL
*  INFORMATION, INTO NV RAM.
*CALLING SEQUENCE:
*  software_exception_new_n_valid_to_nv(new_n_valid)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  new_n_valid -
*      This procedure loads input value into
*      location of NEW_N_VALID flag in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be updated.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  software_exception_new_n_valid_to_nv(
    unsigned char  new_n_valid
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_07.
                        un_ee_block_07.block_07.new_n_valid)) ;
  status =
    nvRamSet((char *)&new_n_valid,sizeof(new_n_valid),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  software_exception_handler_processing_time_from_nv
*TYPE: PROC
*DATE: 27/APR/2003
*FUNCTION:
*  GET VALUE OF HANDLER_PROCESSING_TIME FLAG,
*  WHICH INDICATES HOW MUCH TIME THE HANDLING OF
*  RESET EXCEPTION HAS TAKEN, FROM NV RAM.
*CALLING SEQUENCE:
*  software_exception_handler_processing_time_from_nv(handler_processing_time_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  *handler_processing_time_ptr -
*      This procedure loads the pointed address with
*      value of HANDLER_PROCESSING_TIME, as
*      stored in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  software_exception_handler_processing_time_from_nv(
    unsigned long  *handler_processing_time_ptr
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset =
    (unsigned int)((char *)&(ee_area->ee_block_07.
                   un_ee_block_07.block_07.handler_processing_time)) ;
  status =
    nvRamGet((char *)handler_processing_time_ptr,
                sizeof(*handler_processing_time_ptr),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  software_exception_handler_processing_time_to_nv
*TYPE: PROC
*DATE: 27/APR/2003
*FUNCTION:
*  SET VALUE OF HANDLER_PROCESSING_TIME FLAG, WHICH
*  INDICATES HOW MUCH TIME RESET EXCEPTION HANDLING
*  HAS TAKEN, INTO NV RAM.
*CALLING SEQUENCE:
*  software_exception_handler_processing_time_to_nv(handler_processing_time)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  handler_processing_time -
*      This procedure loads input value into
*      location of HANDLER_PROCESSING_TIME
*      in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be updated.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  software_exception_handler_processing_time_to_nv(
    unsigned long  handler_processing_time
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_07.
                        un_ee_block_07.block_07.handler_processing_time)) ;
  status =
    nvRamSet((char *)&handler_processing_time,
                  sizeof(handler_processing_time),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  software_exception_error_identifier_from_nv
*TYPE: PROC
*DATE: 27/APR/2003
*FUNCTION:
*  GET VALUE OF ERROR_IDENTIFIER VARIABLE, WHICH
*  IDENTIFIES THE TYPE OF ERROR EVENT RELATED TO
*  RESET EXCEPTION AS STORED IN NV RAM.
*CALLING SEQUENCE:
*  software_exception_error_identifier_from_nv(error_identifier_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long  *error_identifier_ptr -
*      This procedure loads the pointed address with
*      value of ERROR_IDENTIFIER flag, as stored in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  software_exception_error_identifier_from_nv(
    unsigned long  *error_identifier_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  char
    block_data[sizeof(char) + 1] ;
  unsigned
    long
      error_identifier ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_07.
                        un_ee_block_07.block_07.error_identifier)) ;
  status =
    nvRamGet(block_data,sizeof(error_identifier),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
  error_identifier = *((unsigned long *)block_data) ;
  *error_identifier_ptr = error_identifier ;
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  software_exception_error_identifier_to_nv
*TYPE: PROC
*DATE: 27/APR/2003
*FUNCTION:
*  SET VALUE OF ERROR_IDENTIFIER VARIABLE, WHICH
*  IDENTIFIES THE TYPE OF ERROR EVENT RELATED TO
*  RESET EXCEPTION INTO NV RAM.
*CALLING SEQUENCE:
*  software_exception_error_identifier_to_nv(error_identifier)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  error_identifier -
*      This procedure loads input value into
*      location of ERROR_IDENTIFIER variable in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be updated.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  software_exception_error_identifier_to_nv(
    unsigned long  error_identifier
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_07.
                        un_ee_block_07.block_07.error_identifier)) ;
  status =
    nvRamSet((char *)&error_identifier,sizeof(error_identifier),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  software_exception_exception_counter_from_nv
*TYPE: PROC
*DATE: 27/APR/2003
*FUNCTION:
*  GET VALUE OF EXCEPTION_COUNTER FROM NV RAM, WHICH
*  INDICATES HOW MANY RESET EXCEPTIONS WERE DETECTED
*  BEFORE INFORMATION HAS BEEN RETRIEVED (AT NEXT
*  STARTUP).
*CALLING SEQUENCE:
*  software_exception_exception_counter_from_nv(exception_counter_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char  *exception_counter_ptr -
*      This procedure loads the pointed address with
*      value of EXCEPTION_COUNTER, as stored in
*      NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  software_exception_exception_counter_from_nv(
    unsigned char  *exception_counter_ptr
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    char
      exception_counter ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_07.
                        un_ee_block_07.block_07.exception_counter)) ;
  status =
    nvRamGet(&exception_counter,sizeof(exception_counter),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
  *exception_counter_ptr = exception_counter ;
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  block_07_from_nv
*TYPE: PROC
*DATE: 27/APR/2003
*FUNCTION:
*  GET THE FULL BLOCK_07 STRUCTURE, WHICH
*  CONTAINS ALL INFORMATION RELATED TO
*  RESET EXCEPTION AS STORED IN NV RAM.
*CALLING SEQUENCE:
*  block_07_from_nv(block_07_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    void  *block_07_ptr -
*      This procedure loads the pointed address
*      (BLOCK_07 structure) with value of the
*      full block_07, which contains all
*      information related to reset exception,
*      as stored in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  block_07_from_nv(
    void  *block_07_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_07.
                        un_ee_block_07.block_07)) ;
  status =
    nvRamGet(block_07_ptr,sizeof(BLOCK_07),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  block_07_to_nv
*TYPE: PROC
*DATE: 27/APR/2003
*FUNCTION:
*  LOAD THE FULL BLOCK_07 STRUCTURE, WHICH
*  CONTAINS ALL INFORMATION RELATED TO
*  RESET EXCEPTION INTO NV RAM.
*CALLING SEQUENCE:
*  block_07_to_nv(block_07_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    void  *block_07_ptr -
*      This procedure loads data from pointed
*      address (BLOCK_07 structure) into the full
*      block_07, which contains all information
*      related to reset exception, in NV ram.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  block_07_to_nv(
    void  *block_07_ptr
  )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  STATUS
    status ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_07.
                        un_ee_block_07.block_07)) ;
  status =
    nvRamSet(block_07_ptr,sizeof(BLOCK_07),(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*
 * }
 */
/*
 * Utilities related to level of handling events (display
 * and storage in NVRAM log file (block_03))
 * {
 */
/*****************************************************
*NAME
*  get_err_display_level
*TYPE: PROC
*DATE: 09/JULY/2002
*FUNCTION:
*  GET THE RUN-TIME VALUE OF SEVERITY LEVEL ABOVE
*  WHICH EVENTS ARE TO BE DISPLAYED.
*CALLING SEQUENCE:
*  get_err_display_level(level_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *level_ptr -
*      This procedure loads the pointed address
*      with run-time value of severity level above
*      which events are to be displayed.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_err_display_level(
    unsigned char *level_ptr
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.event_display_level)) ;
  err =
    get_run_vals(level_ptr,sizeof(*level_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *level_ptr = SVR_WRN ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  get_nv_store_level
*TYPE: PROC
*DATE: 09/JULY/2002
*FUNCTION:
*  GET THE RUN-TIME VALUE OF SEVERITY LEVEL ABOVE
*  WHICH EVENTS ARE TO BE STORED IN NVRAM LOG
*  FILE.
*CALLING SEQUENCE:
*  get_nv_store_level(level_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *level_ptr -
*      This procedure loads the pointed address
*      with run-time value of severity level above
*      which events are to be stored in NVRAM
*      log file.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_nv_store_level(
    unsigned char *level_ptr
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.event_nvram_level)) ;
  err =
    get_run_vals(level_ptr,sizeof(*level_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *level_ptr = SVR_ERR ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  get_err_syslog_level
*TYPE: PROC
*DATE: 23/FEB/2005
*FUNCTION:
*  GET THE RUN-TIME VALUE OF SEVERITY LEVEL ABOVE
*  WHICH EVENTS ARE TO BE SENT TO A SYSLOG SERVER.
*CALLING SEQUENCE:
*  get_err_syslog_level(level_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *level_ptr -
*      This procedure loads the pointed address
*      with run-time value of severity level above
*      which events are to be sent to a syslog server
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_err_syslog_level(
    unsigned char *level_ptr
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.event_syslog_level)) ;
  err =
    get_run_vals(level_ptr,sizeof(*level_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *level_ptr = SVR_LAST ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*
 * }
 */
/*
 * Utilities related to level of handling DPSS (display,
 * debug, activate (block_03))
 * {
 */
/*****************************************************
*NAME
*  get_dpss_display_level
*TYPE: PROC
*DATE: 19/MARCH/2002
*FUNCTION:
*  GET THE RUN-TIME VALUE OF DISPLAY LEVEL FOR
*  MARVELL'S DPSS PACKAGE.
*CALLING SEQUENCE:
*  get_dpss_display_level(level_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *level_ptr -
*      This procedure loads the pointed address
*      with run-time value of dpss display level.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_dpss_display_level(
    unsigned char *level_ptr
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.dpss_display_level)) ;
  err =
    get_run_vals(level_ptr,sizeof(*level_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *level_ptr = 0 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  get_dpss_debug_level
*TYPE: PROC
*DATE: 19/MARCH/2002
*FUNCTION:
*  GET THE RUN-TIME VALUE OF DEBUG LEVEL FOR
*  MARVELL'S DPSS PACKAGE.
*CALLING SEQUENCE:
*  get_dpss_debug_level(level_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *level_ptr -
*      This procedure loads the pointed address
*      with run-time value of dpss debug level.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_dpss_debug_level(
    unsigned char *level_ptr
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.dpss_debug_level)) ;
  err =
    get_run_vals(level_ptr,sizeof(*level_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *level_ptr = 0 ;
    goto exit ;
  }
exit:
  return (ret) ;
}

/*
 * }
 */
/*****************************************************
*NAME
*  get_run_cli_script_at_stratup
*TYPE: PROC
*DATE: 22/JUL/2003
*FUNCTION:
*  GET THE RUN-TIME VALUE OF FLAG INDICATING
*  WHETHER TO RUN CLI STARTUP SCRIPT.
*CALLING SEQUENCE:
*  get_run_cli_script_at_stratup(val_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *val_ptr -
*      This procedure loads the pointed address
*      with run-time value of flag indicating
*      whether to run cli script at startup or not.
*      A non zero value indicates 'run'.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_run_cli_script_at_stratup(
    unsigned char *val_ptr
  )
{
  int err, ret;
  EE_AREA *ee_area;
  unsigned int offset;
  /**/
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.run_ui_startup_script)) ;
  err =
    get_run_vals(val_ptr,sizeof(*val_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *val_ptr = 0 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*
 * }
 */
/*****************************************************
*NAME
*  get_load_dpss_config_file_from_flash
*TYPE: PROC
*DATE: 22/JUL/2003
*FUNCTION:
*  GET THE RUN-TIME VALUE OF FLAG INDICATING
*  WHETHER TO LOAD DPSS CONFIG FILE FROM FLASH.
*CALLING SEQUENCE:
*  get_load_dpss_config_file_from_flash(val_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *val_ptr -
*      This procedure loads the pointed address
*      with run-time value of flag indicating
*      whether to load dpss config file from flash
*      or from tftp. int
  get_load_dpss_config_file_from_flash(
    unsigned char *val_ptr
  )

*      A non zero value indicates 'flash'.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_load_dpss_config_file_from_flash(
    unsigned char *val_ptr
  )
{
  int err, ret;
  EE_AREA *ee_area;
  unsigned int offset;
  /**/
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.load_dpss_from_file_on_flash)) ;
  err =
    get_run_vals(val_ptr,sizeof(*val_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *val_ptr = 0 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*
 * }
 */
/*****************************************************
*NAME
*  get_start_application_in_demo_mode
*TYPE: PROC
*DATE: 06/AUG/2003
*FUNCTION:
*  GET THE RUN-TIME VALUE OF FLAG INDICATING
*  WHETHER TO START_IN_DEMO_MODE.
*CALLING SEQUENCE:
*  get_start_application_in_demo_mode(val_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *val_ptr -
*      This procedure loads the pointed address
*      with run-time value of flag indicating
*      whether to run cli script at startup or not.
*      A non zero value indicates 'run'.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_start_application_in_demo_mode(
    unsigned char *val_ptr
  )
{
  int err, ret;
  EE_AREA *ee_area;
  unsigned int offset;
  /**/
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.start_application_in_demo_mode)) ;
  err =
    get_run_vals(val_ptr,sizeof(*val_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *val_ptr = 0 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*
 * }
 */
/*
 * Utilities related to telnet activity items and actions
 * (block_03)
 * {
 */
/*****************************************************
*NAME
*  get_fap10m_run_mode
*TYPE: PROC
*DATE: 06/AUG/2003
*FUNCTION:
*  GET THE RUN-TIME VALUE OF FLAG INDICATING
*  THE FAP10M_RUN_MODE.
*CALLING SEQUENCE:
*  get_fap10m_run_mode(val_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *val_ptr -
*      This procedure loads the pointed address
*      with run-time value of flag indicating
*      the fap10m_run_mode.
*      ZERO - ECI
*      ONE  - PCI
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_fap10m_run_mode(
    unsigned char *val_ptr
  )
{
  int err, ret;
  EE_AREA *ee_area;
  unsigned int offset;
  /**/
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.fap10m_run_mode)) ;
  err =
    get_run_vals(val_ptr,sizeof(*val_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *val_ptr = 0 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  get_telnet_activity_timeout_std
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  GET THE RUN-TIME VALUE OF THE TELNET STANDARD
*  INACTIVITY PERIOD (IN UNITS OF SECONDS).
*CALLING SEQUENCE:
*  get_telnet_activity_timeout_std(period_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *period_ptr -
*      This procedure loads the pointed address
*      with run-time value of the telnet standard
*      inactivity period, in units of seconds.
*      A value of -1 (255) indicates infinite timeout.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_telnet_activity_timeout_std(
    unsigned char *period_ptr
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.telnet_activity_timeout_std)) ;
  err =
    get_run_vals(period_ptr,sizeof(*period_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *period_ptr = 10 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  get_telnet_activity_timeout_cont
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  GET THE RUN-TIME VALUE OF THE TELNET INACTIVITY
*  PERIOD FOR CONTINUOUS DISPLAY (IN UNITS OF SECONDS).
*CALLING SEQUENCE:
*  get_telnet_activity_timeout_cont(period_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *period_ptr -
*      this procedure loads the pointed address
*      with run-time value of the telnet continuous
*      display inactivity period, in units of seconds.
*      A value of -1 (255) indicates infinite timeout.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_telnet_activity_timeout_cont(
    unsigned char *period_ptr
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.telnet_activity_timeout_cont)) ;
  err =
    get_run_vals(period_ptr,sizeof(*period_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *period_ptr = 10 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  set_telnet_activity_timeout_std
*TYPE: PROC
*DATE: 18/MARCH/2003
*FUNCTION:
*  SET THE RUN-TIME VALUE OF THE TELNET STANDARD
*  INACTIVITY PERIOD (IN UNITS OF SECONDS).
*CALLING SEQUENCE:
*  set_telnet_activity_timeout_std(period)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char period -
*      This procedure sets the run-time value of
*      the telnet standard inactivity period, in
*      units of seconds. A value of -1 (255) indicates
*      infinite timeout.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be written.
*  SOC_SAND_INDIRECT:
*    Updated runtime 'NV ram' structure.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  set_telnet_activity_timeout_std(
    unsigned char period
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset =
    (unsigned int)((char *)&(ee_area->ee_block_03.
              un_ee_block_03.block_03.telnet_activity_timeout_std)) ;
  err =
    set_run_vals(&period,sizeof(period),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  set_telnet_activity_timeout_cont
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  SET THE RUN-TIME VALUE OF THE TELNET INACTIVITY
*  PERIOD FOR CONTINUOUS DISPLAY (IN UNITS OF SECONDS).
*CALLING SEQUENCE:
*  set_telnet_activity_timeout_cont(period)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char period -
*      This procedure sets the run-time value of
*      in the telnet continuous display inactivity
*      period, units of seconds. A value of -1 (255)
*      indicates infinite timeout.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be written.
*  SOC_SAND_INDIRECT:
*    Updated runtime 'NV ram' structure.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  set_telnet_activity_timeout_cont(
    unsigned char period
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset =
   (unsigned int)((char *)&(ee_area->ee_block_03.
              un_ee_block_03.block_03.telnet_activity_timeout_cont)) ;
  err =
    set_run_vals(&period,sizeof(period),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*
 * }
 */
/*
 * Utilities related to watchdog items and actions
 * (block_03)
 * {
 */
/*****************************************************
*NAME
*  get_watchdog_period
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  GET THE RUN-TIME VALUE OF THE WATCHDOG TIMER PERIOD
*  (IN UNITS OF 100 MS).
*CALLING SEQUENCE:
*  get_watchdog_period(watchdog_period_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *watchdog_period_ptr -
*      This procedure loads the pointed address
*      with run-time value of the watchdog timer
*      period, in units of 100 ms.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  get_watchdog_period(
    unsigned char *watchdog_period_ptr
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.watchdog_period)) ;
  err =
    get_run_vals(watchdog_period_ptr,sizeof(*watchdog_period_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *watchdog_period_ptr = 0 ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  is_watchdog_active
*TYPE: PROC
*DATE: 29/MAY/2002
*FUNCTION:
*  GET THE RUN-TIME VALUE OF
*  THE ACTIVATE_WATCHDOG FLAG.
*CALLING SEQUENCE:
*  is_watchdog_active()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If positive then watchdog is active.
*      If zero then watchdog is NOT active.
*      If negative then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  is_watchdog_active(
    void
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  unsigned
    char
      activate_watchdog ;
  ret = -1 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.activate_watchdog)) ;
  err =
    get_run_vals(&activate_watchdog,sizeof(activate_watchdog),(int)offset) ;
  if (err)
  {
    goto tifn_exit ;
  }
  if (activate_watchdog)
  {
    ret = 1 ;
  }
  else
  {
    ret = 0 ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  nv_is_watchdog_active
*TYPE: PROC
*DATE: 18/AUG/2002
*FUNCTION:
*  GET THE NVRAM VALUE OF THE THE ACTIVATE_WATCHDOG
*  FLAG.
*CALLING SEQUENCE:
*  nv_is_watchdog_active()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If positive then watchdog is active.
*      If zero then watchdog is NOT active.
*      If negative then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  nv_is_watchdog_active(
    void
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  unsigned
    char
      *activate_watchdog_ptrs[3] ;
  unsigned
    char
      activate_watchdog[3] ;
  activate_watchdog_ptrs[0] = &activate_watchdog[0] ;
  activate_watchdog_ptrs[1] = &activate_watchdog[1] ;
  activate_watchdog_ptrs[2] = &activate_watchdog[2] ;
  ret = -1 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.activate_watchdog)) ;
  err =
    get_nv_vals((void**)&activate_watchdog_ptrs,sizeof(activate_watchdog),(int)offset) ;
  if (err)
  {
    goto tifn_exit ;
  }
  if (activate_watchdog[0])
  {
    ret = 1 ;
  }
  else
  {
    ret = 0 ;
  }
tifn_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  nv_is_hook_software_exception_active
*TYPE: PROC
*DATE: 18/AUG/2002
*FUNCTION:
*  get the nvram value of the the soft_emu_exception_active
*  flag.
*CALLING SEQUENCE:
*  nv_is_hook_software_exception_active()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If positive then soft_emu_exception should be attached.
*      If zero then soft_emu_exception should not be attached.
*         and the OS defualt should work.
*      If negative then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  nv_is_hook_software_exception_active(
    void
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  unsigned
    char
      *hook_software_exception_ptrs[3] ;
  unsigned
    char
      hook_software_exception[3] ;
  hook_software_exception_ptrs[0] = &hook_software_exception[0] ;
  hook_software_exception_ptrs[1] = &hook_software_exception[1] ;
  hook_software_exception_ptrs[2] = &hook_software_exception[2] ;
  ret = -1 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.hook_software_exception)) ;
  err =
    get_nv_vals((void**)&hook_software_exception_ptrs,
                 sizeof(hook_software_exception),(int)offset) ;
  if (err)
  {
    goto exit ;
  }
  if (hook_software_exception[0])
  {
    ret = 1 ;
  }
  else
  {
    ret = 0 ;
  }
exit:
  return (ret) ;
}

/*****************************************************
*NAME
*  nv_is_bckg_temperatures_en_active
*TYPE: PROC
*DATE: 4/JULY/2003
*FUNCTION:
*  get the nvram value of the the soft_emu_exception_active
*  flag.
*CALLING SEQUENCE:
*  nv_is_bckg_temperatures_en_active()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If positive then collect in backgroud the tempruture.
*      If negative then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int
  nv_is_bckg_temperatures_en_active(
    void
  )
{
  int
    err,
    ret ;
  EE_AREA
    *ee_area ;
  unsigned
    int
      offset ;
  unsigned
    char
      *bckg_temperatures_en_ptrs[3] ;
  unsigned
    char
      bckg_temperatures_en[3] ;
  bckg_temperatures_en_ptrs[0] = &bckg_temperatures_en[0] ;
  bckg_temperatures_en_ptrs[1] = &bckg_temperatures_en[1] ;
  bckg_temperatures_en_ptrs[2] = &bckg_temperatures_en[2] ;
  ret = -1 ;
  ee_area = (EE_AREA *)0 ;
  offset = (unsigned int)((char *)&(ee_area->ee_block_03.
                              un_ee_block_03.block_03.bckg_temperatures_en)) ;
  err =
    get_nv_vals((void**)&bckg_temperatures_en_ptrs,
                 sizeof(bckg_temperatures_en),(int)offset) ;
  if (err)
  {
    goto exit ;
  }
  if (bckg_temperatures_en[2])
  {
    ret = 1 ;
  }
  else
  {
    ret = 0 ;
  }
exit:
  return (ret) ;
}

/*
 * }
 */
/*
 * Utilities related to snmp block
 * (block_06)
 * {
 */
/*****************************************************
*NAME
*  get_download_mibs
*  get_load_netsnmp_flag
*  get_net_snmp_cmd_line
*TYPE: PROC
*DATE: 29/JULY/2002
*FUNCTION:
*  GET THE RUN-TIME VALUES OF FLAGS INDICATING WHETHER
*  MIBS SHOULD OR SHOULD NOT BE DOWNLOADED,
*  NET-SNMP PACKAGE SHOULD BE LOADED and THE COMMAND
*  LINE THAT SHOULD BE PASSED TO IT IF SO.
*CALLING SEQUENCE:
*  get_download_mibs(flag_ptr)
*  get_load_netsnmp_flag(flag_ptr)
*  get_net_snmp_cmd_line(line)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned char *flag_ptr -
*      This procedure loads the pointed address
*      with run-time value of flag indicating
*      whether it is required to
*      download mibs / load netsnmp / command line
*      at startup or not. If the value of the
*      flag is non-zero then mibs / net-snmp are
*      required.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Value could not be read.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
int get_download_mibs(unsigned char *flag_ptr)
{
  int           err, ret;
  EE_AREA       *ee_area ;
  unsigned int  offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset  = (unsigned int)((char *)&(ee_area->ee_block_06.
                              un_ee_block_06.block_06.download_mibs)) ;
  err     = get_run_vals(flag_ptr,sizeof(*flag_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *flag_ptr = FALSE ;
    goto tifn_exit ;
  }
tifn_exit:
  return (ret) ;
}
/*
 */
int get_load_netsnmp_flag(unsigned char *flag_ptr)
{
  int           err, ret;
  EE_AREA       *ee_area ;
  unsigned int  offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset  = (unsigned int)((char *)&(ee_area->ee_block_06.
                              un_ee_block_06.block_06.load_netsnmp)) ;
  err     = get_run_vals(flag_ptr,sizeof(*flag_ptr),(int)offset) ;
  if (err)
  {
    ret = 1 ;
    *flag_ptr = FALSE ;
    goto ld_nsnmp_exit ;
  }
ld_nsnmp_exit:
  return (ret) ;
}
/*
 */
int get_net_snmp_cmd_line(char *line)
{
  int           err, ret;
  EE_AREA       *ee_area ;
  unsigned int  offset ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  offset  = (unsigned int)((char *)&(ee_area->ee_block_06.
                              un_ee_block_06.block_06.snmp_cmd_line)) ;
  
  err     = get_run_vals(line,SNMP_CMD_LINE_SIZE,(int)offset) ;
  if (err)
  {
    ret = 1 ;
    line = NULL ;
    goto cmd_line_exit ;
  }
cmd_line_exit:
  return (ret) ;
}
/*
 * }
 */
/*****************************************************
*NAME
*   GET_BLOCK_INFO
*TYPE: PROC
*DATE: 13/MAR/2002
*FUNCTION:
*  GET INFORMATION ON THE SPECIFIED BLOCK IN NV RAM.
*CALLING SEQUENCE:
*  get_block_info(block_id,
*            block_base,block_size,block_crc_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int  block_id -
*      Number of block in nv ram (starting from '1').
*      A non-existing block number will return
*      an error.
*      If block is larger than/equal to
*      'SERIAL_EE_FIRST_BLOCK_ID' then this is a block
*      of the serial eeprom.
*    char **block_base -
*      Pointer to char pointer . On output, this
*      procedure loads it with the pointer to
*      the base of the indicated block.
*    unsigned long *block_size -
*      Pointer to unsigned long . On output, this
*      procedure loads it with the size of the data
*      section of the indicated block.
*    short **block_crc_ptr -
*      Pointer to char pointer . On output, this
*      procedure loads it with the pointer to
*      the base of the indicated block.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV ram is not updated.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  get_block_info(
    unsigned int    block_id,
    char            **block_base,
    unsigned long   *block_size,
    unsigned short  **block_crc_ptr
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  SERIAL_EE_AREA
    *serial_ee_area ;
  char
    *block_data_ptr ;
  unsigned
    int
      block_len ;
  ret = FALSE ;
  ee_area = (EE_AREA *)NV_RAM_ADRS ;
  serial_ee_area = get_high_image_ptr() ;
  switch (block_id)
  {
    case 1:
    {
      *block_base = block_data_ptr = (char *)&(ee_area->ee_block_01.un_ee_block_01) ;
      *block_size = block_len = SIZEOF_EE_BLOCK_01 ;
      break ;
    }
    case 2:
    {
      *block_base = block_data_ptr = (char *)&(ee_area->ee_block_02.un_ee_block_02) ;
      *block_size = block_len = SIZEOF_EE_BLOCK_02 ;
      break ;
    }
    case 3:
    {
      *block_base = block_data_ptr = (char *)&(ee_area->ee_block_03.un_ee_block_03) ;
      *block_size = block_len = SIZEOF_EE_BLOCK_03 ;
      break ;
    }
    case 4:
    {
      *block_base = block_data_ptr = (char *)&(ee_area->ee_block_04.un_ee_block_04) ;
      *block_size = block_len = SIZEOF_EE_BLOCK_04 ;
      break ;
    }
    case 5:
    {
      *block_base = block_data_ptr = (char *)&(ee_area->ee_block_05.un_ee_block_05) ;
      *block_size = block_len = SIZEOF_EE_BLOCK_05 ;
      break ;
    }
    case 6:
    {
      *block_base = block_data_ptr = (char *)&(ee_area->ee_block_06.un_ee_block_06) ;
      *block_size = block_len = SIZEOF_EE_BLOCK_06 ;
      break ;
    }
    case 7:
    {
      *block_base = block_data_ptr = (char *)&(ee_area->ee_block_07.un_ee_block_07) ;
      *block_size = block_len = SIZEOF_EE_BLOCK_07 ;
      break ;
    }
    case SERIAL_EE_FIRST_BLOCK_ID:
    {
      *block_base =
        block_data_ptr = (char *)&(serial_ee_area->ser_ee_block_01.un_serial_ee_block_01) ;
      *block_size = block_len = SIZEOF_SERIAL_EE_BLOCK_01 ;
      break ;
    }
    case (SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      *block_base =
        block_data_ptr = (char *)&(serial_ee_area->ser_ee_block_02.un_serial_ee_block_02) ;
      *block_size = block_len = SIZEOF_SERIAL_EE_BLOCK_02 ;
      break ;
    }
    case HOST_SERIAL_EE_FIRST_BLOCK_ID:
    {
      *block_base =
        block_data_ptr =
          (char *)&(serial_ee_area->host_ser_ee_block_01.un_host_serial_ee_block_01) ;
      *block_size = block_len = HOST_SIZEOF_SERIAL_EE_BLOCK_01 ;
      break ;
    }
    case (HOST_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      *block_base =
        block_data_ptr =
          (char *)&(serial_ee_area->host_ser_ee_block_02.un_host_serial_ee_block_02) ;
      *block_size = block_len = HOST_SIZEOF_SERIAL_EE_BLOCK_02 ;
      break ;
    }
    case FRONT_SERIAL_EE_FIRST_BLOCK_ID:
    {
      *block_base =
        block_data_ptr =
          (char *)&(serial_ee_area->front_ser_ee_block_01.un_front_serial_ee_block_01) ;
      *block_size = block_len = FRONT_SIZEOF_SERIAL_EE_BLOCK_01 ;
      break ;
    }
    case (FRONT_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      *block_base =
        block_data_ptr =
          (char *)&(serial_ee_area->front_ser_ee_block_02.un_front_serial_ee_block_02) ;
      *block_size = block_len = FRONT_SIZEOF_SERIAL_EE_BLOCK_02 ;
      break ;
    }
    case HOST_DB_SERIAL_EE_FIRST_BLOCK_ID:
    {
      *block_base =
        block_data_ptr =
          (char *)&(serial_ee_area->host_db_ser_ee_block_01.un_host_db_serial_ee_block_01) ;
      *block_size = block_len = HOST_DB_SIZEOF_SERIAL_EE_BLOCK_01 ;
      break ;
    }
    case (HOST_DB_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      *block_base =
        block_data_ptr =
          (char *)&(serial_ee_area->host_db_ser_ee_block_02.un_host_db_serial_ee_block_02) ;
      *block_size = block_len = HOST_DB_SIZEOF_SERIAL_EE_BLOCK_02 ;
      break ;
    }
    default:
    {
      ret = TRUE ;
      goto gebl_exit ;
      break ;
    }
  }
  *block_crc_ptr = (unsigned short *)(block_data_ptr + block_len) ;
gebl_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   ee_offset_to_block_id
*TYPE: PROC
*DATE: 13/MAR/2002
*FUNCTION:
*  Convert offset (in nv ram) and size of field to the
*  id of the corresponding block.
*CALLING SEQUENCE:
*  ee_offset_to_block_id(offset,size,block_id)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int  offset -
*      Offset of field (from the beginning of NV ram).
*      If offset is larger than/equal to
*      'SERIAL_EE_FIRST_OFFSET' then this is offest
*      in the serial eeprom (starting at
*      'SERIAL_EE_FIRST_OFFSET', which is equivalent
*      to '0').
*    unsigned int  size -
*      Size of field (number of bytes).
*    unsigned int  *block_id -
*      Pointer to unsigned int. This procedure
*      will load pointed location by ID of the matching
*      block (provided return value is zero).
*      Block ID start at '1' (not '0').
*      If block is larger than/equal to
*      'SERIAL_EE_FIRST_BLOCK_ID' then this is a block
*      of the serial eeprom.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Probaly this field cannot be fit into
*      current NV ram structure.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  ee_offset_to_block_id(
    unsigned int  offset,
    unsigned int  size,
    unsigned int  *block_id
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  SERIAL_EE_AREA
    *serial_ee_area ;
  char
    *block_data_ptr,
    *block_data_end,
    *field_base ;
  unsigned
    long
      block_size ;
  unsigned
    short
      *crc_location;
  unsigned
    int
      found,
      cur_block_id ;
  ret = 0 ;
  if (offset < SERIAL_EE_FIRST_OFFSET)
  {
    ee_area = (EE_AREA *)NV_RAM_ADRS ;
    field_base = (char *)ee_area + offset ;
  }
  else
  {
    serial_ee_area = get_high_image_ptr() ;
    field_base = (char *)serial_ee_area + offset - SERIAL_EE_FIRST_OFFSET ;
  }
  found = FALSE ;
  for (cur_block_id = 1 ;
          cur_block_id <= NUM_EE_BLOCKS ; cur_block_id++)
  {
    ret = get_block_info(cur_block_id,
            &block_data_ptr,&block_size,&crc_location) ;
    if (ret)
    {
      ret = 1 ;
      goto eotb_exit ;
    }
    block_data_end = block_data_ptr + block_size - 1 ;
    if ((field_base >= block_data_ptr) && (field_base <= block_data_end))
    {
      /*
       * block has been found. Now make sure that field is contained
       * within block.
       */
      if ((field_base + size - 1) > block_data_end)
      {
        /*
         * Field is out of bounds. Return with error.
         */
        ret = 2 ;
        goto eotb_exit ;
      }
      else
      {
        found = TRUE ;
        *block_id = cur_block_id ;
        break ;
      }
    }
  }
  if (!found)
  {
    for (cur_block_id = SERIAL_EE_FIRST_BLOCK_ID ;
            cur_block_id < (SERIAL_EE_FIRST_BLOCK_ID + NUM_SERIAL_EE_BLOCKS) ; cur_block_id++)
    {
      ret = b_get_block_info(cur_block_id,
              &block_data_ptr,&block_size,&crc_location) ;
      if (ret)
      {
        ret = 3 ;
        goto eotb_exit ;
      }
      block_data_end = block_data_ptr + block_size - 1 ;
      if ((field_base >= block_data_ptr) && (field_base <= block_data_end))
      {
        /*
         * block has been found. Now make sure that field is contained
         * within block.
         */
        if ((field_base + size - 1) > block_data_end)
        {
          /*
           * Field is out of bounds. Return with error.
           */
          ret = 4 ;
          goto eotb_exit ;
        }
        else
        {
          found = TRUE ;
          *block_id = cur_block_id ;
          break ;
        }
      }
    }
  }
  if (!found)
  {
    for (cur_block_id = HOST_SERIAL_EE_FIRST_BLOCK_ID ;
            cur_block_id <
              (HOST_SERIAL_EE_FIRST_BLOCK_ID + HOST_NUM_SERIAL_EE_BLOCKS) ; cur_block_id++)
    {
      ret = b_get_block_info(cur_block_id,
              &block_data_ptr,&block_size,&crc_location) ;
      if (ret)
      {
        ret = 5 ;
        goto eotb_exit ;
      }
      block_data_end = block_data_ptr + block_size - 1 ;
      if ((field_base >= block_data_ptr) && (field_base <= block_data_end))
      {
        /*
         * block has been found. Now make sure that field is contained
         * within block.
         */
        if ((field_base + size - 1) > block_data_end)
        {
          /*
           * Field is out of bounds. Return with error.
           */
          ret = 6 ;
          goto eotb_exit ;
        }
        else
        {
          found = TRUE ;
          *block_id = cur_block_id ;
          break ;
        }
      }
    }
  }
  if (!found)
  {
    for (cur_block_id = FRONT_SERIAL_EE_FIRST_BLOCK_ID ;
            cur_block_id <
              (FRONT_SERIAL_EE_FIRST_BLOCK_ID + FRONT_NUM_SERIAL_EE_BLOCKS) ; cur_block_id++)
    {
      ret = b_get_block_info(cur_block_id,
              &block_data_ptr,&block_size,&crc_location) ;
      if (ret)
      {
        ret = 5 ;
        goto eotb_exit ;
      }
      block_data_end = block_data_ptr + block_size - 1 ;
      if ((field_base >= block_data_ptr) && (field_base <= block_data_end))
      {
        /*
         * block has been found. Now make sure that field is contained
         * within block.
         */
        if ((field_base + size - 1) > block_data_end)
        {
          /*
           * Field is out of bounds. Return with error.
           */
          ret = 6 ;
          goto eotb_exit ;
        }
        else
        {
          found = TRUE ;
          *block_id = cur_block_id ;
          break ;
        }
      }
    }
  }
  if (!found)
  {
    for (cur_block_id = HOST_DB_SERIAL_EE_FIRST_BLOCK_ID ;
            cur_block_id <
              (HOST_DB_SERIAL_EE_FIRST_BLOCK_ID + HOST_DB_NUM_SERIAL_EE_BLOCKS) ; cur_block_id++)
    {
      ret = b_get_block_info(cur_block_id,
              &block_data_ptr,&block_size,&crc_location) ;
      if (ret)
      {
        ret = 7 ;
        goto eotb_exit ;
      }
      block_data_end = block_data_ptr + block_size - 1 ;
      if ((field_base >= block_data_ptr) && (field_base <= block_data_end))
      {
        /*
         * block has been found. Now make sure that field is contained
         * within block.
         */
        if ((field_base + size - 1) > block_data_end)
        {
          /*
           * Field is out of bounds. Return with error.
           */
          ret = 8 ;
          goto eotb_exit ;
        }
        else
        {
          found = TRUE ;
          *block_id = cur_block_id ;
          break ;
        }
      }
    }
  }
  if (!found)
  {
    ret = 9 ;
    goto eotb_exit ;
  }
eotb_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   check_block_crc
*TYPE: PROC
*DATE: 12/MAR/2002
*FUNCTION:
*  CALCULATE AND CHECK CRC16 FOR THE SPECIFIED
*  BLOCK IN NV RAM.
*CALLING SEQUENCE:
*  check_block_crc(block_id)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int  block_id -
*      Number of block in nv ram (starting from '1').
*      A non-existing block number will return
*      an error.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If positive then there is no match between
*      calculated CRC and value in NV ram.
*      If zero then there is a match between
*      calculated CRC and value in NV ram.
*      If negative then some error was encountered.
*      NV check has not been completed.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  This procedure must be carried out uninterrupted
*  by other potential users. Make sure to not invoke
*  any operating system services that might enable
*  interrupts (through enabling task switching).
*SEE ALSO:
 */
int
  check_block_crc(
    unsigned int  block_id
    )
{
  int
    ret ;
  char
    *block_data_ptr ;
  unsigned
    long
      block_size ;
  unsigned
    short
      *crc_location,
      crc_starter,
      crc_16 ;
  ret = get_block_info(block_id,
            &block_data_ptr,&block_size,&crc_location) ;
  if (ret)
  {
    ret = -1 ;
    goto chbl_exit ;
  }
  crc_starter = 0xFFFF ;
  if (get_use_par_nv_sem())
  {
    take_par_nv_sem() ;
  }
  crc_16 =
    crc16_with_ini(block_data_ptr,block_size,crc_starter) ;
  if (*crc_location == crc_16)
  {
    ret = 0 ;
  }
  else
  {
    ret = 1 ;
  }
  if (get_use_par_nv_sem())
  {
    give_back_par_nv_sem() ;
  }
chbl_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   update_block_crc
*TYPE: PROC
*DATE: 13/MAR/2002
*FUNCTION:
*  CALCULATE AND CHECK CRC16 FOR THE SPECIFIED
*  BLOCK IN NV RAM.
*CALLING SEQUENCE:
*  update_block_crc(block_id)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int  block_id -
*      Number of block in nv ram (starting from '1').
*      A non-existing block number will return
*      an error.
*      If block is larger than/equal to
*      'SERIAL_EE_FIRST_BLOCK_ID' then this is a block
*      of the serial eeprom.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV update has not been completed.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  This procedure must be carried out uninterrupted
*  by other potential users.
*SEE ALSO:
 */
int
  update_block_crc(
    unsigned int  block_id
    )
{
  int
    ret ;
  EE_AREA
    *ee_area ;
  char
    *crc_bytes,
    *block_data_ptr ;
  unsigned
    long
      block_size ;
  unsigned
    int
      offset;
  unsigned
    short
      *crc_location,
      crc_starter,
      crc_16 ;
  STATUS
    status ;
  unsigned
    int
      sem_taken ;
  sem_taken = FALSE ;
  ee_area = (EE_AREA *)NV_RAM_ADRS ;
  ret = get_block_info(block_id,
            &block_data_ptr,&block_size,&crc_location) ;
  if (ret)
  {
    ret = -1 ;
    goto upbl_exit ;
  }
  crc_starter = 0xFFFF ;
  if (get_use_par_nv_sem())
  {
    take_par_nv_sem() ;
    sem_taken = TRUE ;
  }
  crc_16 =
    crc16_with_ini(block_data_ptr,block_size,crc_starter) ;
  if (block_id < SERIAL_EE_FIRST_BLOCK_ID)
  {
    /*
     * Handle parallel eeprom.
     */
    ee_area = (EE_AREA *)NV_RAM_ADRS ;
    offset = (char *)crc_location - (char *)ee_area ;
    crc_bytes = (char *)&crc_16 ;
    status =
      eepromWriteBytes(offset,
            (unsigned char *)crc_bytes,(unsigned int)sizeof(crc_16)) ;
    if (status == ERROR)
    {
      ret = TRUE ;
      goto upbl_exit ;
    }
  }
  else
  {
    /*
     * Handle serial eeprom.
     */
    *crc_location = crc_16 ;
  }
upbl_exit:
  if (sem_taken)
  {
    give_back_par_nv_sem() ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*   get_defaults
*TYPE: PROC
*DATE: 12/MAR/2002
*FUNCTION:
*  Get the value of the specified byte at the specified
*  offset from the defaults image of NV ram.
*CALLING SEQUENCE:
*  get_defaults(char *string,int strLen,int offset)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *string -
*      Where to copy pointed bytes.
*    int strLen -
*      Number of bytes to copy from defaults image.
*    int offset -
*      Offset (within NV ram) of block of bytes
*      (of size strLen) to copy from defaults image.
*      If offset is larger than/equal to
*      'SERIAL_EE_FIRST_OFFSET' then this is offest
*      in the serial eeprom (starting at
*      'SERIAL_EE_FIRST_OFFSET', which is equivalent
*      to '0').
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Default reading has not been completed.
*  SOC_SAND_INDIRECT:
*    See 'string'.
*REMARKS:
*  This procedure is supposed to be called only after
*  load_flash_device_code_to_ram() has been called.
*SEE ALSO:
 */
int
  get_defaults(
    char *string,
    int  strLen,
    int  offset
    )
{
  int
    ret,
    err,
    fill_char ;
  EE_AREA
    ee_area ;
  SERIAL_EE_AREA
    serial_ee_area ;
  char
    *block_data_ptr ;
  unsigned
    long
      block_size ;
  unsigned
    int
      block_id,
      ui ;
  unsigned
    short
      crc_starter,
      crc_16 ;
  err =
    ee_offset_to_block_id(
              (unsigned int)offset,(unsigned int)strLen,&block_id) ;
  if (err)
  {
    ret = 1 ;
    goto gete_exit ;
  }
  ret = FALSE ;
  fill_char = 0x00 ;
  crc_starter = 0xFFFF ;
  switch (block_id)
  {
    case 1:
    {
      EE_BLOCK_01
        *ee_block_01 ;
      UN_EE_BLOCK_01
        *un_ee_block_01 ;
      ee_block_01 = &(ee_area.ee_block_01) ;
      un_ee_block_01 = &(ee_block_01->un_ee_block_01) ;
      block_size = SIZEOF_EE_BLOCK_01 ;
      memset(&(un_ee_block_01->fill[0]),fill_char,block_size) ;
      un_ee_block_01->block_01.startup_counter = B_DEFAULT_STARTUP_COUNTER ;
      crc_16 =
        crc16_with_ini(ee_block_01,block_size,crc_starter) ;
      ee_block_01->crc16 = crc_16 ;
      block_data_ptr = ((char *)&ee_area) + offset ;
      break ;
    }
    case 2:
    {
      EE_BLOCK_02
        *ee_block_02 ;
      UN_EE_BLOCK_02
        *un_ee_block_02 ;
      ee_block_02 = &(ee_area.ee_block_02) ;
      un_ee_block_02 = &(ee_block_02->un_ee_block_02) ;
      block_size = SIZEOF_EE_BLOCK_02 ;
      memset(&(un_ee_block_02->fill[0]),fill_char,block_size) ;
      un_ee_block_02->block_02.ip_addr = B_DEFAULT_IP_ADDR ;
      un_ee_block_02->block_02.ip_mask = B_DEFAULT_IP_MASK ;
      un_ee_block_02->block_02.default_gateway_addr = B_DEFAULT_GATEWAY_ADDR ;
      un_ee_block_02->block_02.downloading_host_addr = B_DEFAULT_DOWNLOADING_HOST_ADDR ;
      /*if (get_flash_device_code() == FLASH_28F128)
      {
        un_ee_block_02->block_02.mem_drv_len = B_DEFAULT_MEM_DRV_LEN_16MEG ;
        un_ee_block_02->block_02.app_flash_size = B_DEFAULT_APP_FLASH_SIZE_16MEG ;
        un_ee_block_02->block_02.app_flash_base = (char *)B_DEFAULT_APP_FLASH_BASE_16MEG ;
      }
      else */
      if (get_flash_device_code() == FLASH_28F640)
      {
        un_ee_block_02->block_02.mem_drv_len = B_DEFAULT_MEM_DRV_LEN_8MEG_VER_B ;
        un_ee_block_02->block_02.app_flash_size = B_DEFAULT_APP_FLASH_SIZE_8MEG ;
        un_ee_block_02->block_02.app_flash_base = (char *)B_DEFAULT_APP_FLASH_BASE_8MEG ;
      }
      else
      {
        un_ee_block_02->block_02.mem_drv_len = B_DEFAULT_MEM_DRV_LEN_4MEG ;
        un_ee_block_02->block_02.app_flash_size = B_DEFAULT_APP_FLASH_SIZE_4MEG ;
        un_ee_block_02->block_02.app_flash_base = (char *)B_DEFAULT_APP_FLASH_BASE_4MEG ;
      }
      un_ee_block_02->block_02.app_source = B_DEFAULT_APP_SOURCE ;
      un_ee_block_02->block_02.app_load_method = B_DEFAULT_APP_LOAD_METHOD ;
      strcpy(un_ee_block_02->block_02.mem_device_name,B_DEFAULT_MEM_DEVICE_NAME) ;
      strcpy(un_ee_block_02->block_02.dld_file_name,B_DEFAULT_DLD_FILE_NAME) ;
      strcpy(un_ee_block_02->block_02.target_name,B_DEFAULT_TARGET_NAME) ;
      un_ee_block_02->block_02.auto_flash_load = B_DEFAULT_AUTO_FLASH_LOAD ;
      un_ee_block_02->block_02.kernel_startup = B_DEFAULT_KERNEL_STARTUP ;
      un_ee_block_02->block_02.activate_inst_cache = B_DEFAULT_ACTIVATE_INST_CACHE ;
      un_ee_block_02->block_02.activate_data_cache = B_DEFAULT_ACTIVATE_DATA_CACHE ;
      un_ee_block_02->block_02.console_baud_rate = B_DEFAULT_CONSOLE_BAUD_RATE ;
      un_ee_block_02->block_02.ip_source_mode = B_DEFAULT_IP_SOURCE_MODE ;
      un_ee_block_02->block_02.test_at_startup = B_DEFAULT_TEST_AT_STARTUP ;
      un_ee_block_02->block_02.base_register_04   = B_DEFAULT_BASE_REGISTER_04 ;
      un_ee_block_02->block_02.base_register_05   = B_DEFAULT_BASE_REGISTER_05 ;
      un_ee_block_02->block_02.option_register_04 = B_DEFAULT_OPTION_REGISTER_04 ;
      un_ee_block_02->block_02.option_register_05 = B_DEFAULT_OPTION_REGISTER_05 ;
      un_ee_block_02->block_02.tcp_timeout = B_DEFAULT_TCP_TIMEOUT ;
      un_ee_block_02->block_02.tcp_retries = B_DEFAULT_TCP_RETRIES ;
      un_ee_block_02->block_02.debug_register_01 = B_DEFAULT_CLK_MIRROR_DATA ;

      crc_16 =
        crc16_with_ini(ee_block_02,block_size,crc_starter) ;
      ee_block_02->crc16 = crc_16 ;
      block_data_ptr = ((char *)&ee_area) + offset ;
      break ;
    }
    case 3:
    {
      EE_BLOCK_03
        *ee_block_03 ;
      UN_EE_BLOCK_03
        *un_ee_block_03 ;
      ee_block_03 = &(ee_area.ee_block_03) ;
      un_ee_block_03 = &(ee_block_03->un_ee_block_03) ;
      block_size = SIZEOF_EE_BLOCK_03 ;
      memset(&(un_ee_block_03->fill[0]),fill_char,block_size) ;
      un_ee_block_03->block_03.page_mode              = DEFAULT_PAGE_MODE ;
      un_ee_block_03->block_03.page_lines             = DEFAULT_PAGE_LINES ;
      for (ui = 0 ; ui < NUM_FE_ON_BOARD ; ui++)
      {
        un_ee_block_03->block_03.conf_from_md[ui]     = DEFAULT_CONF_FROM_MD ;
        un_ee_block_03->block_03.fe_configuration[ui] = DEFAULT_FE_CONFIGURATION ;
      }
      un_ee_block_03->block_03.line_mode          = DEFAULT_LINE_MODE ;
      un_ee_block_03->block_03.activate_watchdog  = DEFAULT_ACTIVATE_WATCHDOG ;
      un_ee_block_03->block_03.watchdog_period    = DEFAULT_WATCHDOG_PERIOD ;
      un_ee_block_03->block_03.
            telnet_activity_timeout_std           = DEFEULT_TELNET_ACTIVITY_TIMEOUT_STD ;
      un_ee_block_03->block_03.
            telnet_activity_timeout_cont          = DEFAULT_TELNET_ACTIVITY_TIMEOUT_CONT ;
      un_ee_block_03->block_03.
            event_display_level                   = DEFAULT_EVENT_DISPLAY_LEVEL ;
      un_ee_block_03->block_03.event_nvram_level  = DEFAULT_EVENT_NVRAM_LEVEL ;
      un_ee_block_03->block_03.event_syslog_level  = DEFAULT_EVENT_SYSLOG_LEVEL ;
      un_ee_block_03->block_03.usr_app_flavor      = SOC_D_USR_APP_FLAVOR_NONE;
      un_ee_block_03->block_03.dpss_display_level      = DEFAULT_DPSS_DISPLAY_LEVEL;
      un_ee_block_03->block_03.dpss_debug_level        = DEFAULT_DPSS_DEBUG_LEVEL;
      un_ee_block_03->block_03.run_ui_startup_script   = DEFAULT_RUN_UI_STARTUP_SCRIPT;
      un_ee_block_03->block_03.load_dpss_from_file_on_flash   = DEFAULT_DPSS_LOAD_FROM_FILE;
      un_ee_block_03->block_03.
        start_application_in_demo_mode = DEFAULT_START_APPLICATION_IN_DEMO_MODE;
      un_ee_block_03->block_03.hook_software_exception = DEFAULT_HOOK_SOFTWARE_EXCEPTION ;
      un_ee_block_03->block_03.bckg_temperatures_en    = DEFAULT_BCKG_TEMPERATURES_EN ;
      crc_16 =
        crc16_with_ini(ee_block_03,block_size,crc_starter) ;
      ee_block_03->crc16 = crc_16 ;
      block_data_ptr = ((char *)&ee_area) + offset ;
      break ;
    }
    case 4:
    {
      EE_BLOCK_04
        *ee_block_04 ;
      UN_EE_BLOCK_04
        *un_ee_block_04 ;
      ee_block_04 = &(ee_area.ee_block_04) ;
      un_ee_block_04 = &(ee_block_04->un_ee_block_04) ;
      block_size = SIZEOF_EE_BLOCK_04 ;
      memset(&(un_ee_block_04->fill[0]),fill_char,block_size) ;
      un_ee_block_04->block_04.new_n_valid = DEFAULT_NEW_N_VALID ;
      un_ee_block_04->block_04.error_identifier = DEFAULT_ERROR_IDENTIFIER ;
      crc_16 =
        crc16_with_ini(ee_block_04,block_size,crc_starter) ;
      ee_block_04->crc16 = crc_16 ;
      block_data_ptr = ((char *)&ee_area) + offset ;
      break ;
    }
    case 5:
    {
      EE_BLOCK_05
        *ee_block_05 ;
      UN_EE_BLOCK_05
        *un_ee_block_05 ;
      ee_block_05 = &(ee_area.ee_block_05) ;
      un_ee_block_05 = &(ee_block_05->un_ee_block_05) ;
      block_size = SIZEOF_EE_BLOCK_05 ;
      memset(&(un_ee_block_05->fill[0]),fill_char,block_size) ;
      un_ee_block_05->block_05.num_events_logged = 0 ;
      un_ee_block_05->block_05.next_in_index = 0 ;
      crc_16 =
        crc16_with_ini(ee_block_05,block_size,crc_starter) ;
      ee_block_05->crc16 = crc_16 ;
      block_data_ptr = ((char *)&ee_area) + offset ;
      break ;
    }
    case 6:
    {
      EE_BLOCK_06
        *ee_block_06 ;
      UN_EE_BLOCK_06
        *un_ee_block_06 ;
      ee_block_06 = &(ee_area.ee_block_06) ;
      un_ee_block_06 = &(ee_block_06->un_ee_block_06) ;
      block_size = SIZEOF_EE_BLOCK_06 ;
      memset(&(un_ee_block_06->fill[0]),fill_char,block_size) ;
      un_ee_block_06->block_06.load_netsnmp   = DEFAULT_LOAD_NETSNMP;
      un_ee_block_06->block_06.download_mibs  = DEFAULT_DOWNLOAD_MIBS;
      strcpy(un_ee_block_06->block_06.snmp_cmd_line,DEFAULT_SNMP_CMD_LINE);
      crc_16 =
        crc16_with_ini(ee_block_06,block_size,crc_starter) ;
      ee_block_06->crc16 = crc_16 ;
      block_data_ptr = ((char *)&ee_area) + offset ;
      break ;
    }
    case 7:
    {
      EE_BLOCK_07
        *ee_block_07 ;
      UN_EE_BLOCK_07
        *un_ee_block_07 ;
      ee_block_07 = &(ee_area.ee_block_07) ;
      un_ee_block_07 = &(ee_block_07->un_ee_block_07) ;
      block_size = SIZEOF_EE_BLOCK_07 ;
      memset(&(un_ee_block_07->fill[0]),fill_char,block_size) ;
      un_ee_block_07->block_07.new_n_valid = DEFAULT_NEW_N_VALID ;
      un_ee_block_07->block_07.error_identifier = DEFAULT_ERROR_IDENTIFIER ;
      crc_16 =
        crc16_with_ini(ee_block_07,block_size,crc_starter) ;
      ee_block_07->crc16 = crc_16 ;
      block_data_ptr = ((char *)&ee_area) + offset ;
      break ;
    }
    case SERIAL_EE_FIRST_BLOCK_ID:
    {
      SER_EE_BLOCK_01
        *ser_ee_block_01 ;
      UN_SERIAL_EE_BLOCK_01
        *un_serial_ee_block_01 ;
      ser_ee_block_01 = &(serial_ee_area.ser_ee_block_01) ;
      un_serial_ee_block_01 = &(ser_ee_block_01->un_serial_ee_block_01) ;
      block_size = SIZEOF_SERIAL_EE_BLOCK_01 ;
      memset(&(un_serial_ee_block_01->fill[0]),fill_char,block_size) ;
      un_serial_ee_block_01->serial_ee_block_01.eth_addr[0] = DN_ENET0 ;
      un_serial_ee_block_01->serial_ee_block_01.eth_addr[1] = DN_ENET1 ;
      un_serial_ee_block_01->serial_ee_block_01.eth_addr[2] = DN_ENET2 ;
      un_serial_ee_block_01->serial_ee_block_01.eth_addr[3] = CUST_ENET3 ;
      un_serial_ee_block_01->serial_ee_block_01.eth_addr[4] = CUST_ENET4 ;
      un_serial_ee_block_01->serial_ee_block_01.eth_addr[5] = CUST_ENET5 ;
      crc_16 =
        crc16_with_ini(ser_ee_block_01,block_size,crc_starter) ;
      ser_ee_block_01->crc16 = crc_16 ;
      block_data_ptr = ((char *)&serial_ee_area) + offset - SERIAL_EE_FIRST_OFFSET ;
      break ;
    }
    case (SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      SER_EE_BLOCK_02
        *ser_ee_block_02 ;
      UN_SERIAL_EE_BLOCK_02
        *un_serial_ee_block_02 ;
      ser_ee_block_02 = &(serial_ee_area.ser_ee_block_02) ;
      un_serial_ee_block_02 = &(ser_ee_block_02->un_serial_ee_block_02) ;
      block_size = SIZEOF_SERIAL_EE_BLOCK_02 ;
      memset(&(un_serial_ee_block_02->fill[0]),fill_char,block_size) ;
      memset(un_serial_ee_block_02->
            serial_ee_block_02.board_description,
            0,B_MAX_BOARD_DESCRIPTION_LEN) ;
      memset(un_serial_ee_block_02->
            serial_ee_block_02.board_name,
            0,B_MAX_BOARD_NAME_LEN) ;
      /*
       * Load different defaults for new mezzanine board (PCI).
       */
      if (get_epld_board_id() == (unsigned char)NEW_MEZZANINE_WITH_PCI)
      {
        un_serial_ee_block_02->
            serial_ee_block_02.board_serial_number = B_DEFAULT_BOARD_SERIAL_NUMBER_NEW ;
        un_serial_ee_block_02->
            serial_ee_block_02.board_version = B_DEFAULT_BOARD_VERSION_NEW ;
        strcpy(un_serial_ee_block_02->
            serial_ee_block_02.board_description,
            B_DEFAULT_BOARD_DESCRIPTION_NEW) ;
        strcpy(un_serial_ee_block_02->
            serial_ee_block_02.board_name,
            B_DEFAULT_BOARD_NAME_NEW) ;
      }
      else
      {
        un_serial_ee_block_02->
            serial_ee_block_02.board_serial_number = B_DEFAULT_BOARD_SERIAL_NUMBER ;
        un_serial_ee_block_02->
            serial_ee_block_02.board_version = B_DEFAULT_BOARD_VERSION ;
        strcpy(un_serial_ee_block_02->
            serial_ee_block_02.board_description,
            B_DEFAULT_BOARD_DESCRIPTION) ;
        strcpy(un_serial_ee_block_02->
            serial_ee_block_02.board_name,
            B_DEFAULT_BOARD_NAME) ;
      }
      crc_16 =
        crc16_with_ini(ser_ee_block_02,block_size,crc_starter) ;
      ser_ee_block_02->crc16 = crc_16 ;
      block_data_ptr = ((char *)&serial_ee_area) + offset - SERIAL_EE_FIRST_OFFSET ;
      break ;
    }
    case HOST_SERIAL_EE_FIRST_BLOCK_ID:
    {
      HOST_SER_EE_BLOCK_01        *ser_ee_host_block_01;
      UN_HOST_SERIAL_EE_BLOCK_01  *un_host_serial_ee_block_01;
      ser_ee_host_block_01        = &(serial_ee_area.host_ser_ee_block_01);
      un_host_serial_ee_block_01  = &(ser_ee_host_block_01->un_host_serial_ee_block_01);
      block_size                  = HOST_SIZEOF_SERIAL_EE_BLOCK_01;
      memset(&(un_host_serial_ee_block_01->fill[0]),fill_char,block_size) ;
      un_host_serial_ee_block_01->
        host_serial_ee_block_01.host_board_type = B_DEFAULT_HOST_BOARD_TYPE;
      crc_16 = crc16_with_ini(ser_ee_host_block_01,block_size,crc_starter) ;
      ser_ee_host_block_01->crc16 = crc_16 ;
      block_data_ptr = ((char *)&serial_ee_area) + offset - SERIAL_EE_FIRST_OFFSET;
      break ;
    }
    case (HOST_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      HOST_SER_EE_BLOCK_02        *ser_ee_host_block_02;
      UN_HOST_SERIAL_EE_BLOCK_02  *un_host_serial_ee_block_02;
      ser_ee_host_block_02        = &(serial_ee_area.host_ser_ee_block_02);
      un_host_serial_ee_block_02  = &(ser_ee_host_block_02->un_host_serial_ee_block_02);
      block_size                  = HOST_SIZEOF_SERIAL_EE_BLOCK_02;
      memset(&(un_host_serial_ee_block_02->fill[0]),fill_char,block_size) ;
     strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_catalog_number,
        B_DEFAULT_HOST_CATALOG_NUMBER);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_description,
        B_DEFAULT_HOST_BOARD_DESCRIPTION);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_version,
        B_DEFAULT_HOST_BOARD_VERSION);
      un_host_serial_ee_block_02->
        host_serial_ee_block_02.host_board_serial_number = B_DEFAULT_HOST_BOARD_SN;
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_param_1,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_param_2,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_param_3,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_param_4,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_param_5,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_param_6,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_param_7,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_param_8,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_param_9,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_serial_ee_block_02->host_serial_ee_block_02.host_board_param_10,
        B_DEFAULT_ENPTY_PARAM);
      crc_16 = crc16_with_ini(ser_ee_host_block_02,block_size,crc_starter) ;
      ser_ee_host_block_02->crc16 = crc_16 ;
      block_data_ptr = ((char *)&serial_ee_area) + offset - SERIAL_EE_FIRST_OFFSET;
      break ;
    }
    case FRONT_SERIAL_EE_FIRST_BLOCK_ID:
    {
      FRONT_SER_EE_BLOCK_01        *ser_ee_front_block_01;
      UN_FRONT_SERIAL_EE_BLOCK_01  *un_front_serial_ee_block_01;
      ser_ee_front_block_01        = &(serial_ee_area.front_ser_ee_block_01);
      un_front_serial_ee_block_01  = &(ser_ee_front_block_01->un_front_serial_ee_block_01);
      block_size                  = FRONT_SIZEOF_SERIAL_EE_BLOCK_01;
      memset(&(un_front_serial_ee_block_01->fill[0]),fill_char,block_size) ;
      un_front_serial_ee_block_01->
        front_serial_ee_block_01.front_board_type = B_DEFAULT_FRONT_BOARD_TYPE;
      crc_16 = crc16_with_ini(ser_ee_front_block_01,block_size,crc_starter) ;
      ser_ee_front_block_01->crc16 = crc_16 ;
      block_data_ptr = ((char *)&serial_ee_area) + offset - SERIAL_EE_FIRST_OFFSET;
      break ;
    }
    case (FRONT_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      FRONT_SER_EE_BLOCK_02        *ser_ee_front_block_02;
      UN_FRONT_SERIAL_EE_BLOCK_02  *un_front_serial_ee_block_02;
      ser_ee_front_block_02        = &(serial_ee_area.front_ser_ee_block_02);
      un_front_serial_ee_block_02  = &(ser_ee_front_block_02->un_front_serial_ee_block_02);
      block_size                  = FRONT_SIZEOF_SERIAL_EE_BLOCK_02;
      memset(&(un_front_serial_ee_block_02->fill[0]),fill_char,block_size) ;
     strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_catalog_number,
        B_DEFAULT_FRONT_CATALOG_NUMBER);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_description,
        B_DEFAULT_FRONT_BOARD_DESCRIPTION);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_version,
        B_DEFAULT_FRONT_BOARD_VERSION);
      un_front_serial_ee_block_02->
        front_serial_ee_block_02.front_board_serial_number = B_DEFAULT_FRONT_BOARD_SN;
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_param_1,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_param_2,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_param_3,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_param_4,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_param_5,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_param_6,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_param_7,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_param_8,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_param_9,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_front_serial_ee_block_02->front_serial_ee_block_02.front_board_param_10,
        B_DEFAULT_ENPTY_PARAM);
      crc_16 = crc16_with_ini(ser_ee_front_block_02,block_size,crc_starter) ;
      ser_ee_front_block_02->crc16 = crc_16 ;
      block_data_ptr = ((char *)&serial_ee_area) + offset - SERIAL_EE_FIRST_OFFSET;
      break ;
    }
    case HOST_DB_SERIAL_EE_FIRST_BLOCK_ID:
    {
      HOST_DB_SER_EE_BLOCK_01        *ser_ee_host_db_block_01;
      UN_HOST_DB_SERIAL_EE_BLOCK_01  *un_host_db_serial_ee_block_01;
      ser_ee_host_db_block_01        = &(serial_ee_area.host_db_ser_ee_block_01);
      un_host_db_serial_ee_block_01  = &(ser_ee_host_db_block_01->un_host_db_serial_ee_block_01);
      block_size                  = HOST_DB_SIZEOF_SERIAL_EE_BLOCK_01;
      memset(&(un_host_db_serial_ee_block_01->fill[0]),fill_char,block_size) ;
      un_host_db_serial_ee_block_01->
        host_db_serial_ee_block_01.host_db_board_type = B_DEFAULT_HOST_DB_BOARD_TYPE;
      crc_16 = crc16_with_ini(ser_ee_host_db_block_01,block_size,crc_starter) ;
      ser_ee_host_db_block_01->crc16 = crc_16 ;
      block_data_ptr = ((char *)&serial_ee_area) + offset - SERIAL_EE_FIRST_OFFSET;
      break ;
    }
    case (HOST_DB_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      HOST_DB_SER_EE_BLOCK_02        *ser_ee_host_db_block_02;
      UN_HOST_DB_SERIAL_EE_BLOCK_02  *un_host_db_serial_ee_block_02;
      ser_ee_host_db_block_02        = &(serial_ee_area.host_db_ser_ee_block_02);
      un_host_db_serial_ee_block_02  = &(ser_ee_host_db_block_02->un_host_db_serial_ee_block_02);
      block_size                  = HOST_DB_SIZEOF_SERIAL_EE_BLOCK_02;
      memset(&(un_host_db_serial_ee_block_02->fill[0]),fill_char,block_size) ;
     strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_catalog_number,
        B_DEFAULT_HOST_DB_CATALOG_NUMBER);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_description,
        B_DEFAULT_HOST_DB_BOARD_DESCRIPTION);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_version,
        B_DEFAULT_HOST_DB_BOARD_VERSION);
      un_host_db_serial_ee_block_02->
        host_db_serial_ee_block_02.host_db_board_serial_number = B_DEFAULT_HOST_DB_BOARD_SN;
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_param_1,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_param_2,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_param_3,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_param_4,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_param_5,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_param_6,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_param_7,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_param_8,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_param_9,
        B_DEFAULT_ENPTY_PARAM);
      strcpy(
        un_host_db_serial_ee_block_02->host_db_serial_ee_block_02.host_db_board_param_10,
        B_DEFAULT_ENPTY_PARAM);
      crc_16 = crc16_with_ini(ser_ee_host_db_block_02,block_size,crc_starter) ;
      ser_ee_host_db_block_02->crc16 = crc_16 ;
      block_data_ptr = ((char *)&serial_ee_area) + offset - SERIAL_EE_FIRST_OFFSET;
      break ;
    }
    default:
    {
      ret = 2 ;
      goto gete_exit ;
      break ;
    }
  }
  memcpy(string,block_data_ptr,strLen) ;
gete_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   get_run_vals
*TYPE: PROC
*DATE: 25/MAR/2002
*FUNCTION:
*  Get the value of the specified byte at the specified
*  offset from the runtime image of NV ram.
*CALLING SEQUENCE:
*  get_run_vals(char *string,int strLen,int offset)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *string -
*      Where to copy pointed bytes.
*    int strLen -
*      Number of bytes to copy from runtime image.
*    int offset -
*      Offset (within NV ram) of block of bytes
*      (of size strLen) to copy from runtime image.
*      If offset is larger than/equal to
*      'SERIAL_EE_FIRST_OFFSET' then this is offest
*      in the serial eeprom (starting at
*      'SERIAL_EE_FIRST_OFFSET', which is equivalent
*      to '0').
*  SOC_SAND_INDIRECT:
*    Internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Runtime values reading has not been completed.
*  SOC_SAND_INDIRECT:
*    See 'string'.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  get_run_vals(
    char *string,
    int  strLen,
    int  offset
    )
{
  int
    ret,
    err;
  EE_AREA
    *ee_area ;
  SERIAL_EE_AREA
    *serial_ee_area ;
  char
    *block_data_ptr ;
  unsigned
    int
      block_id ;
  err =
    ee_offset_to_block_id(
              (unsigned int)offset,(unsigned int)strLen,&block_id) ;
  if (err)
  {
    ret = 1 ;
    goto geru_exit ;
  }
  ret = FALSE ;
  if (offset < SERIAL_EE_FIRST_OFFSET)
  {
    ee_area = &Ee_image_runtime ;
    block_data_ptr = ((char *)ee_area) + offset ;
  }
  else
  {
    serial_ee_area = &Serial_ee_image_runtime ;
    block_data_ptr = ((char *)serial_ee_area) + offset - SERIAL_EE_FIRST_OFFSET ;
  }
  memcpy(string,block_data_ptr,strLen) ;
geru_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   get_run_downloading_host_addr
*TYPE: PROC
*DATE: 05/MAR/2003
*FUNCTION:
*  Get the run time value of downloading host IP
*  address.
*CALLING SEQUENCE:
*  get_run_downloading_host_addr(
*    ip_addr_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long *ip_addr_ptr -
*      This procedure loads pointed address by value
*      of IP address of currently assigned
*      downloading host.
*  SOC_SAND_INDIRECT:
*    Internal structure of all runtime values.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Runtime value reading has not been completed.
*  SOC_SAND_INDIRECT:
*    See 'ip_addr_ptr'.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  get_run_downloading_host_addr(
    unsigned long *ip_addr_ptr
  )
{
  unsigned
    int
      addr_size,
      addr_offset ;
  int
    err,
    ret ;
  ret = 0 ;
  addr_offset =
    (unsigned int)((char *)&(((EE_AREA *)0)->
            ee_block_02.un_ee_block_02.block_02.downloading_host_addr)) ;
  addr_size =
    sizeof(((EE_AREA *)0)->
            ee_block_02.un_ee_block_02.block_02.downloading_host_addr) ;
  err = get_run_vals((char *)(ip_addr_ptr),(int)addr_size,(int)addr_offset) ;
  if (err)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   get_run_ip_addr
*TYPE: PROC
*DATE: 12/MAR/2003
*FUNCTION:
*  Get the run time value of this station's IP
*  address.
*CALLING SEQUENCE:
*  get_run_ip_addr(
*    ip_addr_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long *ip_addr_ptr -
*      This procedure loads pointed address by value
*      of IP address currently assigned to this host.
*  SOC_SAND_INDIRECT:
*    Internal structure of all runtime values.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Runtime value reading has not been completed.
*  SOC_SAND_INDIRECT:
*    See 'ip_addr_ptr'.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  get_run_ip_addr(
    unsigned long *ip_addr_ptr
    )
{
  unsigned
    int
      addr_size,
      addr_offset ;
  int
    err,
    ret ;
  ret = 0 ;
  addr_offset =
    (unsigned int)((char *)&(((EE_AREA *)0)->
            ee_block_02.un_ee_block_02.block_02.ip_addr)) ;
  addr_size =
    sizeof(((EE_AREA *)0)->
            ee_block_02.un_ee_block_02.block_02.ip_addr) ;
  err = get_run_vals((char *)(ip_addr_ptr),(int)addr_size,(int)addr_offset) ;
  if (err)
  {
    ret = 1 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   set_run_vals
*TYPE: PROC
*DATE: 26/MAR/2002
*FUNCTION:
*  Set the value of the specified byte at the specified
*  offset from the runtime image of NV ram.
*CALLING SEQUENCE:
*  set_run_vals(char *string,int strLen,int offset)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *string -
*      Where to copy pointed bytes from.
*    int strLen -
*      Number of bytes to set at runtime image.
*    int offset -
*      Offset (within NV ram) of block of bytes
*      (of size strLen) to copy into runtime image.
*      If offset is larger than/equal to
*      'SERIAL_EE_FIRST_OFFSET' then this is offest
*      in the serial eeprom (starting at
*      'SERIAL_EE_FIRST_OFFSET', which is equivalent
*      to '0').
*  SOC_SAND_INDIRECT:
*    Internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Runtime values update has not been completed.
*  SOC_SAND_INDIRECT:
*    See 'string'.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  set_run_vals(
    char *string,
    int  strLen,
    int  offset
    )
{
  int
    ret,
    err;
  EE_AREA
    *ee_area ;
  SERIAL_EE_AREA
    *serial_ee_area ;
  char
    *block_data_ptr ;
  unsigned
    int
      block_id ;
  err =
    ee_offset_to_block_id(
              (unsigned int)offset,(unsigned int)strLen,&block_id) ;
  if (err)
  {
    ret = 1 ;
    goto geru_exit ;
  }
  ret = FALSE ;
  if (offset < SERIAL_EE_FIRST_OFFSET)
  {
    ee_area = &Ee_image_runtime ;
    block_data_ptr = ((char *)ee_area) + offset ;
  }
  else
  {
    serial_ee_area = &Serial_ee_image_runtime ;
    block_data_ptr = ((char *)serial_ee_area) + offset - SERIAL_EE_FIRST_OFFSET ;
  }
  memcpy(block_data_ptr,string,strLen) ;
geru_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   load_defaults
*TYPE: PROC
*DATE: 12/MAR/2002
*FUNCTION:
*  LOAD THE SPECIFIED BLOCK, IN NV RAM, BY DEFAULT
*  VALUES.
*CALLING SEQUENCE:
*  load_defaults(block_id)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int  block_id -
*      Number of block in nv ram (starting from '1').
*      A value of '0' indicates 'all blocks except
*      for block 1 and except for the serial eeprom's
*      first block.
*      A non-existing block number will return
*      an error.
*      If block is larger than/equal to
*      'SERIAL_EE_FIRST_BLOCK_ID' then this is a block
*      of the serial eeprom.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV update has not been completed.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  This procedure must be carried out uninterrupted
*  by other potential users. Make sure to not invoke
*  any operating system services that might enable
*  interrupts (through enabling task switching).
*SEE ALSO:
 */
int
  load_defaults(
    unsigned int  block_id
    )
{
  int
    err,
    ret ;
  int
    offset ;
  EE_AREA
    *ee_area ;
  SERIAL_EE_AREA
    *serial_ee_area ;
  char
    *block_data_ptr ;
  unsigned
    long
      block_size ;
  unsigned
    int
      parallel_eeprom,
      burn,
      ui ;
  unsigned
    short
      crc_starter,
      crc_16 ;
  STATUS
    status ;
  unsigned
    int
      sem_taken ;
  EE_BLOCK_01
    ee_block_01 ;
  EE_BLOCK_02
    ee_block_02 ;
  EE_BLOCK_03
    ee_block_03 ;
  EE_BLOCK_04
    ee_block_04 ;
  EE_BLOCK_05
    ee_block_05 ;
  EE_BLOCK_06
    ee_block_06 ;
  EE_BLOCK_07
    ee_block_07 ;

  sem_taken = FALSE ;
  ret = FALSE ;
  ee_area = (EE_AREA *)NV_RAM_ADRS ;
  serial_ee_area = get_high_image_ptr() ;
  crc_starter = 0xFFFF ;
  parallel_eeprom = FALSE ;
  burn = TRUE ;
  offset = 0;
  block_data_ptr = NULL;
  block_size = 0;

  if (get_use_par_nv_sem())
  {
    take_par_nv_sem() ;
    sem_taken = TRUE ;
  }
  switch (block_id)
  {
    case 0:
    {
      err = load_defaults(2) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(3) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(4) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(5) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(6) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(SERIAL_EE_FIRST_BLOCK_ID + 1) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(HOST_SERIAL_EE_FIRST_BLOCK_ID) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(HOST_SERIAL_EE_FIRST_BLOCK_ID + 1) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(FRONT_SERIAL_EE_FIRST_BLOCK_ID) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(FRONT_SERIAL_EE_FIRST_BLOCK_ID + 1) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(HOST_DB_SERIAL_EE_FIRST_BLOCK_ID) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      err = load_defaults(HOST_DB_SERIAL_EE_FIRST_BLOCK_ID + 1) ;
      if (err)
      {
        ret = TRUE ;
        goto exit ;
      }
      burn = FALSE ;
      break ;
    }
    case 1:
    {
      UN_EE_BLOCK_01
        *un_ee_block_01 ;
      un_ee_block_01 = &ee_block_01.un_ee_block_01 ;
      block_size = SIZEOF_EE_BLOCK_01 ;
      offset = (int)((char *)&(((EE_AREA *)0)->ee_block_01)) ;
      get_defaults((char *)&ee_block_01,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(&ee_block_01,block_size,crc_starter) ;
      ee_block_01.crc16 = crc_16 ;
      block_size = sizeof(ee_block_01) ;
      block_data_ptr = (char *)&ee_block_01 ;
      parallel_eeprom = TRUE ;
      break ;
    }
    case 2:
    {
      UN_EE_BLOCK_02
        *un_ee_block_02 ;
      un_ee_block_02 = &ee_block_02.un_ee_block_02 ;
      block_size = SIZEOF_EE_BLOCK_02 ;
      offset = (int)((char *)&(((EE_AREA *)0)->ee_block_02)) ;
      get_defaults((char *)&ee_block_02,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(&ee_block_02,block_size,crc_starter) ;
      ee_block_02.crc16 = crc_16 ;
      block_size = sizeof(ee_block_02) ;
      block_data_ptr = (char *)&ee_block_02 ;
      parallel_eeprom = TRUE ;
      break ;
    }
    case 3:
    {
      UN_EE_BLOCK_03
        *un_ee_block_03 ;
      un_ee_block_03 = &ee_block_03.un_ee_block_03 ;
      block_size = SIZEOF_EE_BLOCK_03 ;
      offset = (int)((char *)&(((EE_AREA *)0)->ee_block_03)) ;
      get_defaults((char *)&ee_block_03,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(&ee_block_03,block_size,crc_starter) ;
      ee_block_03.crc16 = crc_16 ;
      block_size = sizeof(ee_block_03) ;
      block_data_ptr = (char *)&ee_block_03 ;
      parallel_eeprom = TRUE ;
      break ;
    }
    case 4:
    {
      UN_EE_BLOCK_04
        *un_ee_block_04 ;
      un_ee_block_04 = &ee_block_04.un_ee_block_04 ;
      block_size = SIZEOF_EE_BLOCK_04 ;
      offset = (int)((char *)&(((EE_AREA *)0)->ee_block_04)) ;
      get_defaults((char *)&ee_block_04,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(&ee_block_04,block_size,crc_starter) ;
      ee_block_04.crc16 = crc_16 ;
      block_size = sizeof(ee_block_04) ;
      block_data_ptr = (char *)&ee_block_04 ;
      parallel_eeprom = TRUE ;
      break ;
    }
    case 5:
    {
      UN_EE_BLOCK_05
        *un_ee_block_05 ;
      /*
       * This is a large block which is, essentially, a file
       * with a header. Get its current contents, just
       * clear the header and, then, calculate CRC.
       */
      un_ee_block_05 = &ee_block_05.un_ee_block_05 ;
      block_size = SIZEOF_EE_BLOCK_05 ;
      offset = (int)((char *)&(((EE_AREA *)0)->ee_block_05)) ;
      block_data_ptr = (char *)(&ee_block_05) ;
      for (ui = 0 ; ui < block_size ; ui++)
      {
        *block_data_ptr++ = NV_RAM_READ_B(offset) ;
        offset++ ;
      }
      /*
       * Get default values for control header only.
       */
      block_size =
        sizeof(un_ee_block_05->block_05) - sizeof(un_ee_block_05->block_05.event_log_item) ;
      offset = (int)((char *)&(((EE_AREA *)0)->ee_block_05.un_ee_block_05.block_05)) ;
      get_defaults(
          (char *)&ee_block_05.un_ee_block_05.block_05,
                                    (int)block_size,(int)offset) ;
      /*
       * Now calculate CRC for the whole block, which is mostly similar
       * to whatever has already been there, except for the control
       * header.
       */
      block_size = SIZEOF_EE_BLOCK_05 ;
      crc_16 =
        crc16_with_ini(&ee_block_05,block_size,crc_starter) ;
      ee_block_05.crc16 = crc_16 ;
      /*
       * Now take size including CRC.
       */
      block_size = sizeof(ee_block_05) ;
      offset = (int)((char *)&(((EE_AREA *)0)->ee_block_05)) ;
      block_data_ptr = (char *)&ee_block_05 ;
      parallel_eeprom = TRUE ;
      break ;
    }
    case 6:
    {
      UN_EE_BLOCK_06
        *un_ee_block_06 ;
      un_ee_block_06 = &ee_block_06.un_ee_block_06 ;
      block_size = SIZEOF_EE_BLOCK_06 ;
      offset = (int)((char *)&(((EE_AREA *)0)->ee_block_06)) ;
      get_defaults((char *)&ee_block_06,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(&ee_block_06,block_size,crc_starter) ;
      ee_block_06.crc16 = crc_16 ;
      block_size = sizeof(ee_block_06) ;
      block_data_ptr = (char *)&ee_block_06 ;
      parallel_eeprom = TRUE ;
      break ;
    }
    case 7:
    {
      UN_EE_BLOCK_07
        *un_ee_block_07 ;
      un_ee_block_07 = &ee_block_07.un_ee_block_07 ;
      block_size = SIZEOF_EE_BLOCK_07 ;
      offset = (int)((char *)&(((EE_AREA *)0)->ee_block_07)) ;
      get_defaults((char *)&ee_block_07,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(&ee_block_07,block_size,crc_starter) ;
      ee_block_07.crc16 = crc_16 ;
      block_size = sizeof(ee_block_07) ;
      block_data_ptr = (char *)&ee_block_07 ;
      parallel_eeprom = TRUE ;
      break ;
    }
    case SERIAL_EE_FIRST_BLOCK_ID:
    {
      SER_EE_BLOCK_01
        *ser_ee_block_01 ;
      block_size = SIZEOF_SERIAL_EE_BLOCK_01 ;
      ser_ee_block_01 = &(serial_ee_area->ser_ee_block_01) ;
      offset =
        (int)((char *)&(((SERIAL_EE_AREA *)0)->ser_ee_block_01)) + SERIAL_EE_FIRST_OFFSET ;
      b_get_defaults((char *)ser_ee_block_01,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(ser_ee_block_01,block_size,crc_starter) ;
      ser_ee_block_01->crc16 = crc_16 ;
      break ;
    }
    case (SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      SER_EE_BLOCK_02
        *ser_ee_block_02 ;
      block_size = SIZEOF_SERIAL_EE_BLOCK_02 ;
      ser_ee_block_02 = &(serial_ee_area->ser_ee_block_02) ;
      offset =
        (int)((char *)&(((SERIAL_EE_AREA *)0)->ser_ee_block_02)) + SERIAL_EE_FIRST_OFFSET ;
      b_get_defaults((char *)ser_ee_block_02,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(ser_ee_block_02,block_size,crc_starter) ;
      ser_ee_block_02->crc16 = crc_16 ;
      break ;
    }
    case HOST_SERIAL_EE_FIRST_BLOCK_ID:
    {
      HOST_SER_EE_BLOCK_01
        *host_ser_ee_block_01 ;
      block_size = HOST_SIZEOF_SERIAL_EE_BLOCK_01 ;
      host_ser_ee_block_01 = &(serial_ee_area->host_ser_ee_block_01) ;
      offset =
        (int)((char *)&(((SERIAL_EE_AREA *)0)->host_ser_ee_block_01)) + SERIAL_EE_FIRST_OFFSET ;
      b_get_defaults((char *)host_ser_ee_block_01,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(host_ser_ee_block_01,block_size,crc_starter) ;
      host_ser_ee_block_01->crc16 = crc_16 ;
      break ;
    }
    case (HOST_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      HOST_SER_EE_BLOCK_02
        *host_ser_ee_block_02 ;
      block_size = HOST_SIZEOF_SERIAL_EE_BLOCK_02 ;
      host_ser_ee_block_02 = &(serial_ee_area->host_ser_ee_block_02) ;
      offset =
        (int)((char *)&(((SERIAL_EE_AREA *)0)->host_ser_ee_block_02)) + SERIAL_EE_FIRST_OFFSET ;
      b_get_defaults((char *)host_ser_ee_block_02,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(host_ser_ee_block_02,block_size,crc_starter) ;
      host_ser_ee_block_02->crc16 = crc_16 ;
      break ;
    }
    case FRONT_SERIAL_EE_FIRST_BLOCK_ID:
    {
      FRONT_SER_EE_BLOCK_01
        *front_ser_ee_block_01 ;
      block_size = FRONT_SIZEOF_SERIAL_EE_BLOCK_01 ;
      front_ser_ee_block_01 = &(serial_ee_area->front_ser_ee_block_01) ;
      offset =
        (int)((char *)&(((SERIAL_EE_AREA *)0)->front_ser_ee_block_01)) + SERIAL_EE_FIRST_OFFSET ;
      b_get_defaults((char *)front_ser_ee_block_01,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(front_ser_ee_block_01,block_size,crc_starter) ;
      front_ser_ee_block_01->crc16 = crc_16 ;
      break ;
    }
    case (FRONT_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      FRONT_SER_EE_BLOCK_02
        *front_ser_ee_block_02 ;
      block_size = FRONT_SIZEOF_SERIAL_EE_BLOCK_02 ;
      front_ser_ee_block_02 = &(serial_ee_area->front_ser_ee_block_02) ;
      offset =
        (int)((char *)&(((SERIAL_EE_AREA *)0)->front_ser_ee_block_02)) + SERIAL_EE_FIRST_OFFSET ;
      b_get_defaults((char *)front_ser_ee_block_02,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(front_ser_ee_block_02,block_size,crc_starter) ;
      front_ser_ee_block_02->crc16 = crc_16 ;
      break ;
    }
    case HOST_DB_SERIAL_EE_FIRST_BLOCK_ID:
    {
      HOST_DB_SER_EE_BLOCK_01
        *host_db_ser_ee_block_01 ;
      block_size = HOST_DB_SIZEOF_SERIAL_EE_BLOCK_01 ;
      host_db_ser_ee_block_01 = &(serial_ee_area->host_db_ser_ee_block_01) ;
      offset =
        (int)((char *)&(((SERIAL_EE_AREA *)0)->host_db_ser_ee_block_01)) + SERIAL_EE_FIRST_OFFSET ;
      b_get_defaults((char *)host_db_ser_ee_block_01,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(host_db_ser_ee_block_01,block_size,crc_starter) ;
      host_db_ser_ee_block_01->crc16 = crc_16 ;
      break ;
    }
    case (HOST_DB_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      HOST_DB_SER_EE_BLOCK_02
        *host_db_ser_ee_block_02 ;
      block_size = HOST_DB_SIZEOF_SERIAL_EE_BLOCK_02 ;
      host_db_ser_ee_block_02 = &(serial_ee_area->host_db_ser_ee_block_02) ;
      offset =
        (int)((char *)&(((SERIAL_EE_AREA *)0)->host_db_ser_ee_block_02)) + SERIAL_EE_FIRST_OFFSET ;
      b_get_defaults((char *)host_db_ser_ee_block_02,(int)block_size,(int)offset) ;
      crc_16 =
        crc16_with_ini(host_db_ser_ee_block_02,block_size,crc_starter) ;
      host_db_ser_ee_block_02->crc16 = crc_16 ;
      break ;
    }
    default:
    {
      ret = TRUE ;
      goto exit ;
      break ;
    }
  }
  if (burn)
  {
    if (parallel_eeprom)
    {
      status = NV_RAM_WRITE_B(offset,block_data_ptr,block_size) ;
      if (status == ERROR)
      {
        ret = TRUE ;
        goto exit ;
      }
    }
  }
exit:
  if (sem_taken)
  {
    give_back_par_nv_sem() ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*   load_defaults_conditional
*TYPE: PROC
*DATE: 23/APR/2002
*FUNCTION:
*  LOAD THE SPECIFIED BLOCK, IN NV RAM, BY DEFAULT
*  VALUES IF ITS CRC SIGNATURE IS WRONG.
*CALLING SEQUENCE:
*  load_defaults_conditional(block_id)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int  block_id -
*      Number of block in nv ram (starting from '1').
*      A value of '0' indicates 'all blocks except
*      for block 1 and except for the serial eeprom's
*      first block.
*      A non-existing block number will return
*      an error.
*      If block is larger than/equal to
*      'SERIAL_EE_FIRST_BLOCK_ID' then this is a block
*      of the serial eeprom.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      NV update has not been completed.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  This procedure must be carried out uninterrupted
*  by other potential users. Make sure to not invoke
*  any operating system services that might enable
*  interrupts (through enabling task switching).
*SEE ALSO:
 */
int
  load_defaults_conditional(
    unsigned int  block_id
    )
{
  int
    err,
    ret ;
  int
    offset ;
  EE_AREA
    *ee_area ;
  SERIAL_EE_AREA
    *serial_ee_area ;
  char
    *block_data_ptr ;
  unsigned
    long
      block_size ;
  unsigned
    int
      parallel_eeprom,
      burn;
  unsigned
    short
      crc_starter,
      crc_16 ;
  STATUS
    status ;
  unsigned
    int
      sem_taken ;
  ret = FALSE ;
  block_data_ptr = NULL;
  block_size = 0;
  ee_area = (EE_AREA *)NV_RAM_ADRS ;
  serial_ee_area = get_high_image_ptr() ;
  crc_starter = 0xFFFF ;
  parallel_eeprom = FALSE ;
  burn = TRUE ;
  sem_taken = FALSE ;
  offset = 0;

  if (get_use_par_nv_sem())
  {
    take_par_nv_sem() ;
    sem_taken = TRUE ;
  }
  switch (block_id)
  {
    case 0:
    {
      err = check_block_crc(2) ;
      if (err > 0)
      {
        err = load_defaults(2) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(3) ;
      if (err > 0)
      {
        err = load_defaults(3) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(4) ;
      if (err > 0)
      {
        err = load_defaults(4) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(5) ;
      if (err > 0)
      {
        err = load_defaults(5) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(6) ;
      if (err > 0)
      {
        err = load_defaults(6) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(SERIAL_EE_FIRST_BLOCK_ID + 1) ;
      if (err > 0)
      {
        err = load_defaults(SERIAL_EE_FIRST_BLOCK_ID + 1) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(HOST_SERIAL_EE_FIRST_BLOCK_ID) ;
      if (err > 0)
      {
        err = load_defaults(HOST_SERIAL_EE_FIRST_BLOCK_ID) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(HOST_SERIAL_EE_FIRST_BLOCK_ID + 1) ;
      if (err > 0)
      {
        err = load_defaults(HOST_SERIAL_EE_FIRST_BLOCK_ID + 1) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(FRONT_SERIAL_EE_FIRST_BLOCK_ID) ;
      if (err > 0)
      {
        err = load_defaults(FRONT_SERIAL_EE_FIRST_BLOCK_ID) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(FRONT_SERIAL_EE_FIRST_BLOCK_ID + 1) ;
      if (err > 0)
      {
        err = load_defaults(FRONT_SERIAL_EE_FIRST_BLOCK_ID + 1) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(HOST_DB_SERIAL_EE_FIRST_BLOCK_ID) ;
      if (err > 0)
      {
        err = load_defaults(HOST_DB_SERIAL_EE_FIRST_BLOCK_ID) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      err = check_block_crc(HOST_DB_SERIAL_EE_FIRST_BLOCK_ID + 1) ;
      if (err > 0)
      {
        err = load_defaults(HOST_DB_SERIAL_EE_FIRST_BLOCK_ID + 1) ;
        if (err)
        {
          ret = TRUE ;
          goto lode_exit ;
        }
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }

      burn = FALSE ;
      break ;
    }
    case 1:
    {
      EE_BLOCK_01
        ee_block_01 ;
      UN_EE_BLOCK_01
        *un_ee_block_01 ;
      err = check_block_crc(1) ;
      if (err > 0)
      {
        un_ee_block_01 = &ee_block_01.un_ee_block_01 ;
        block_size = SIZEOF_EE_BLOCK_01 ;
        offset = (int)((char *)&(((EE_AREA *)0)->ee_block_01)) ;
        get_defaults((char *)&ee_block_01,(int)block_size,(int)offset) ;
        block_data_ptr = (char *)&(ee_area->ee_block_01) ;
        crc_16 =
          crc16_with_ini(&ee_block_01,block_size,crc_starter) ;
        ee_block_01.crc16 = crc_16 ;
        block_size = sizeof(ee_block_01) ;
        offset = block_data_ptr - (char *)ee_area ;
        block_data_ptr = (char *)&ee_block_01 ;
        parallel_eeprom = TRUE ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case 2:
    {
      EE_BLOCK_02
        ee_block_02 ;
      UN_EE_BLOCK_02
        *un_ee_block_02 ;
      err = check_block_crc(2) ;
      if (err > 0)
      {
        un_ee_block_02 = &ee_block_02.un_ee_block_02 ;
        block_size = SIZEOF_EE_BLOCK_02 ;
        offset = (int)((char *)&(((EE_AREA *)0)->ee_block_02)) ;
        get_defaults((char *)&ee_block_02,(int)block_size,(int)offset) ;
        block_data_ptr = (char *)&(ee_area->ee_block_02) ;
        crc_16 =
          crc16_with_ini(&ee_block_02,block_size,crc_starter) ;
        ee_block_02.crc16 = crc_16 ;
        block_size = sizeof(ee_block_02) ;
        offset = block_data_ptr - (char *)ee_area ;
        block_data_ptr = (char *)&ee_block_02 ;
        parallel_eeprom = TRUE ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case 3:
    {
      EE_BLOCK_03
        ee_block_03 ;
      UN_EE_BLOCK_03
        *un_ee_block_03 ;
      err = check_block_crc(3) ;
      if (err > 0)
      {
        un_ee_block_03 = &ee_block_03.un_ee_block_03 ;
        block_size = SIZEOF_EE_BLOCK_03 ;
        offset = (int)((char *)&(((EE_AREA *)0)->ee_block_03)) ;
        get_defaults((char *)&ee_block_03,(int)block_size,(int)offset) ;
        block_data_ptr = (char *)&(ee_area->ee_block_03) ;
        crc_16 =
          crc16_with_ini(&ee_block_03,block_size,crc_starter) ;
        ee_block_03.crc16 = crc_16 ;
        block_size = sizeof(ee_block_03) ;
        offset = block_data_ptr - (char *)ee_area ;
        block_data_ptr = (char *)&ee_block_03 ;
        parallel_eeprom = TRUE ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case 4:
    {
      EE_BLOCK_04
        ee_block_04 ;
      UN_EE_BLOCK_04
        *un_ee_block_04 ;
      err = check_block_crc(4) ;
      if (err > 0)
      {
        un_ee_block_04 = &ee_block_04.un_ee_block_04 ;
        block_size = SIZEOF_EE_BLOCK_04 ;
        offset = (int)((char *)&(((EE_AREA *)0)->ee_block_04)) ;
        get_defaults((char *)&ee_block_04,(int)block_size,(int)offset) ;
        block_data_ptr = (char *)&(ee_area->ee_block_04) ;
        crc_16 =
          crc16_with_ini(&ee_block_04,block_size,crc_starter) ;
        ee_block_04.crc16 = crc_16 ;
        block_size = sizeof(ee_block_04) ;
        offset = block_data_ptr - (char *)ee_area ;
        block_data_ptr = (char *)&ee_block_04 ;
        parallel_eeprom = TRUE ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case 5:
    {
      EE_BLOCK_05
        ee_block_05 ;
      UN_EE_BLOCK_05
        *un_ee_block_05 ;
      err = check_block_crc(5) ;
      if (err > 0)
      {
        un_ee_block_05 = &ee_block_05.un_ee_block_05 ;
        block_size = SIZEOF_EE_BLOCK_05 ;
        offset = (int)((char *)&(((EE_AREA *)0)->ee_block_05)) ;
        get_defaults((char *)&ee_block_05,(int)block_size,(int)offset) ;
        block_data_ptr = (char *)&(ee_area->ee_block_05) ;
        crc_16 =
          crc16_with_ini(&ee_block_05,block_size,crc_starter) ;
        ee_block_05.crc16 = crc_16 ;
        block_size = sizeof(ee_block_05) ;
        offset = block_data_ptr - (char *)ee_area ;
        block_data_ptr = (char *)&ee_block_05 ;
        parallel_eeprom = TRUE ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case 6:
    {
      EE_BLOCK_06
        ee_block_06 ;
      UN_EE_BLOCK_06
        *un_ee_block_06 ;
      err = check_block_crc(6) ;
      if (err > 0)
      {
        un_ee_block_06 = &ee_block_06.un_ee_block_06 ;
        block_size = SIZEOF_EE_BLOCK_06 ;
        offset = (int)((char *)&(((EE_AREA *)0)->ee_block_06)) ;
        get_defaults((char *)&ee_block_06,(int)block_size,(int)offset) ;
        block_data_ptr = (char *)&(ee_area->ee_block_06) ;
        crc_16 =
          crc16_with_ini(&ee_block_06,block_size,crc_starter) ;
        ee_block_06.crc16 = crc_16 ;
        block_size = sizeof(ee_block_06) ;
        offset = block_data_ptr - (char *)ee_area ;
        block_data_ptr = (char *)&ee_block_06 ;
        parallel_eeprom = TRUE ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case 7:
    {
      EE_BLOCK_07
        ee_block_07 ;
      UN_EE_BLOCK_07
        *un_ee_block_07 ;
      err = check_block_crc(7) ;
      if (err > 0)
      {
        un_ee_block_07 = &ee_block_07.un_ee_block_07 ;
        block_size = SIZEOF_EE_BLOCK_07 ;
        offset = (int)((char *)&(((EE_AREA *)0)->ee_block_07)) ;
        get_defaults((char *)&ee_block_07,(int)block_size,(int)offset) ;
        block_data_ptr = (char *)&(ee_area->ee_block_07) ;
        crc_16 =
          crc16_with_ini(&ee_block_07,block_size,crc_starter) ;
        ee_block_07.crc16 = crc_16 ;
        block_size = sizeof(ee_block_07) ;
        offset = block_data_ptr - (char *)ee_area ;
        block_data_ptr = (char *)&ee_block_07 ;
        parallel_eeprom = TRUE ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case SERIAL_EE_FIRST_BLOCK_ID:
    {
      SER_EE_BLOCK_01
        *ser_ee_block_01 ;
      err = check_block_crc(SERIAL_EE_FIRST_BLOCK_ID) ;
      if (err > 0)
      {
        block_size = SIZEOF_SERIAL_EE_BLOCK_01 ;
        ser_ee_block_01 = &(serial_ee_area->ser_ee_block_01) ;
        offset =
          (int)((char *)&(((SERIAL_EE_AREA *)0)->ser_ee_block_01)) + SERIAL_EE_FIRST_OFFSET ;
        b_get_defaults((char *)ser_ee_block_01,(int)block_size,(int)offset) ;
        crc_16 =
          crc16_with_ini(ser_ee_block_01,block_size,crc_starter) ;
        ser_ee_block_01->crc16 = crc_16 ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case (SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      SER_EE_BLOCK_02
        *ser_ee_block_02 ;
      err = check_block_crc(SERIAL_EE_FIRST_BLOCK_ID+1) ;
      if (err > 0)
      {
        block_size = SIZEOF_SERIAL_EE_BLOCK_02 ;
        ser_ee_block_02 = &(serial_ee_area->ser_ee_block_02) ;
        offset =
          (int)((char *)&(((SERIAL_EE_AREA *)0)->ser_ee_block_02)) + SERIAL_EE_FIRST_OFFSET ;
        b_get_defaults((char *)ser_ee_block_02,(int)block_size,(int)offset) ;
        crc_16 =
          crc16_with_ini(ser_ee_block_02,block_size,crc_starter) ;
        ser_ee_block_02->crc16 = crc_16 ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case HOST_SERIAL_EE_FIRST_BLOCK_ID:
    {
      HOST_SER_EE_BLOCK_01
        *host_ser_ee_block_01 ;
      err = check_block_crc(HOST_SERIAL_EE_FIRST_BLOCK_ID) ;
      if (err > 0)
      {
        block_size = HOST_SIZEOF_SERIAL_EE_BLOCK_01 ;
        host_ser_ee_block_01 = &(serial_ee_area->host_ser_ee_block_01) ;
        offset =
          (int)((char *)&(((SERIAL_EE_AREA *)0)->host_ser_ee_block_01)) + SERIAL_EE_FIRST_OFFSET ;
        b_get_defaults((char *)host_ser_ee_block_01,(int)block_size,(int)offset) ;
        crc_16 =
          crc16_with_ini(host_ser_ee_block_01,block_size,crc_starter) ;
        host_ser_ee_block_01->crc16 = crc_16 ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case (HOST_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      HOST_SER_EE_BLOCK_02
        *host_ser_ee_block_02 ;
      err = check_block_crc(HOST_SERIAL_EE_FIRST_BLOCK_ID+1) ;
      if (err > 0)
      {
        block_size = HOST_SIZEOF_SERIAL_EE_BLOCK_02 ;
        host_ser_ee_block_02 = &(serial_ee_area->host_ser_ee_block_02) ;
        offset =
          (int)((char *)&(((SERIAL_EE_AREA *)0)->host_ser_ee_block_02)) + SERIAL_EE_FIRST_OFFSET ;
        b_get_defaults((char *)host_ser_ee_block_02,(int)block_size,(int)offset) ;
        crc_16 =
          crc16_with_ini(host_ser_ee_block_02,block_size,crc_starter) ;
        host_ser_ee_block_02->crc16 = crc_16 ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case FRONT_SERIAL_EE_FIRST_BLOCK_ID:
    {
      FRONT_SER_EE_BLOCK_01
        *front_ser_ee_block_01 ;
      err = check_block_crc(FRONT_SERIAL_EE_FIRST_BLOCK_ID) ;
      if (err > 0)
      {
        block_size = FRONT_SIZEOF_SERIAL_EE_BLOCK_01 ;
        front_ser_ee_block_01 = &(serial_ee_area->front_ser_ee_block_01) ;
        offset =
          (int)((char *)&(((SERIAL_EE_AREA *)0)->
                      front_ser_ee_block_01)) + SERIAL_EE_FIRST_OFFSET ;
        b_get_defaults((char *)front_ser_ee_block_01,(int)block_size,(int)offset) ;
        crc_16 =
          crc16_with_ini(front_ser_ee_block_01,block_size,crc_starter) ;
        front_ser_ee_block_01->crc16 = crc_16 ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case (FRONT_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      FRONT_SER_EE_BLOCK_02
        *front_ser_ee_block_02 ;
      err = check_block_crc(FRONT_SERIAL_EE_FIRST_BLOCK_ID+1) ;
      if (err > 0)
      {
        block_size = FRONT_SIZEOF_SERIAL_EE_BLOCK_02 ;
        front_ser_ee_block_02 = &(serial_ee_area->front_ser_ee_block_02) ;
        offset =
          (int)((char *)&(((SERIAL_EE_AREA *)0)->
                      front_ser_ee_block_02)) + SERIAL_EE_FIRST_OFFSET ;
        b_get_defaults((char *)front_ser_ee_block_02,(int)block_size,(int)offset) ;
        crc_16 =
          crc16_with_ini(front_ser_ee_block_02,block_size,crc_starter) ;
        front_ser_ee_block_02->crc16 = crc_16 ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case HOST_DB_SERIAL_EE_FIRST_BLOCK_ID:
    {
      HOST_DB_SER_EE_BLOCK_01
        *host_db_ser_ee_block_01 ;
      err = check_block_crc(HOST_DB_SERIAL_EE_FIRST_BLOCK_ID) ;
      if (err > 0)
      {
        block_size = HOST_DB_SIZEOF_SERIAL_EE_BLOCK_01 ;
        host_db_ser_ee_block_01 = &(serial_ee_area->host_db_ser_ee_block_01) ;
        offset =
          (int)((char *)&(((SERIAL_EE_AREA *)0)->
                      host_db_ser_ee_block_01)) + SERIAL_EE_FIRST_OFFSET ;
        b_get_defaults((char *)host_db_ser_ee_block_01,(int)block_size,(int)offset) ;
        crc_16 =
          crc16_with_ini(host_db_ser_ee_block_01,block_size,crc_starter) ;
        host_db_ser_ee_block_01->crc16 = crc_16 ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    case (HOST_DB_SERIAL_EE_FIRST_BLOCK_ID + 1):
    {
      HOST_DB_SER_EE_BLOCK_02
        *host_db_ser_ee_block_02 ;
      err = check_block_crc(HOST_DB_SERIAL_EE_FIRST_BLOCK_ID+1) ;
      if (err > 0)
      {
        block_size = HOST_DB_SIZEOF_SERIAL_EE_BLOCK_02 ;
        host_db_ser_ee_block_02 = &(serial_ee_area->host_db_ser_ee_block_02) ;
        offset =
          (int)((char *)&(((SERIAL_EE_AREA *)0)->
                      host_db_ser_ee_block_02)) + SERIAL_EE_FIRST_OFFSET ;
        b_get_defaults((char *)host_db_ser_ee_block_02,(int)block_size,(int)offset) ;
        crc_16 =
          crc16_with_ini(host_db_ser_ee_block_02,block_size,crc_starter) ;
        host_db_ser_ee_block_02->crc16 = crc_16 ;
      }
      else if (err < 0)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
      break ;
    }
    default:
    {
      ret = TRUE ;
      goto lode_exit ;
      break ;
    }
  }
  if (burn)
  {
    if (parallel_eeprom)
    {
      status = eepromWriteBytes(offset,
                  (unsigned char *)block_data_ptr,(unsigned int)block_size) ;
      if (status == ERROR)
      {
        ret = TRUE ;
        goto lode_exit ;
      }
    }
  }
lode_exit:
  if (sem_taken)
  {
    give_back_par_nv_sem() ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*   load_run_vals
*TYPE: PROC
*DATE: 25/MAR/2002
*FUNCTION:
*  LOAD ALL BLOCKS, IN IMAGE OF NV RAM THAT
*  CONTAINS RUN TIME VALUES, FROM NV RAM ITSELF.
*  TO BE USED, MAINLY, AT STARTUP.
*CALLING SEQUENCE:
*  load_run_vals()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    internal structure of nv ram.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error was encountered.
*      Runtime image update has not been completed.
*  SOC_SAND_INDIRECT:
*    Updated run time image of NV ram.
*REMARKS:
*  This procedure must be carried out uninterrupted
*  by other potential users. Make sure to not invoke
*  any operating system services that might enable
*  interrupts (through enabling task switching).
*SEE ALSO:
 */
int
  load_run_vals(
    void
    )
{
  int
    ret ;
  int
    offset ;
  char
    *block_data_ptr ;
  int
    block_size ;
  unsigned
    int
      sem_taken ;
  ret = FALSE ;
  sem_taken = FALSE ;
  if (get_use_par_nv_sem())
  {
    take_par_nv_sem() ;
    sem_taken = TRUE ;
  }
  {
    block_data_ptr = (char *)&Ee_image_runtime ;
    block_size = sizeof(Ee_image_runtime) ;
    offset = 0 ;
    for (offset = 0 ; offset < block_size ; offset++)
    {
      *block_data_ptr++ = NV_RAM_READ_B(offset) ;
    }
  }
  {
    unsigned
      int
        err_flg ;
    char
      err_msg[160] ;
    block_data_ptr = (char *)&Serial_ee_image_runtime ;
    block_size = sizeof(Serial_ee_image_runtime) ;
    offset = 0 ;
    ser_ee_high_image_read(
      (unsigned int)offset,
      (unsigned int)block_size,
      (unsigned char *)block_data_ptr,
      &err_flg,err_msg) ;
  }
  if (sem_taken)
  {
    give_back_par_nv_sem() ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  get_nv_vals
*TYPE: PROC
*DATE: 24/MAR/2002
*FUNCTION:
*  Get values related to NV ram as follows: Value
*  contained in NV ram, Default value, runtime value.
*CALLING SEQUENCE:
*  get_nv_vals(val_ptrs,size,offset)
*INPUT:
*  SOC_SAND_DIRECT:
*    void         **val_ptrs -
*      Pointer to an array of 3 pointers which,
*      themaselves, point to the location to load
*      information as specified above (in this
*      order): NV ram, Default value, runtime value.
*    unsigned int size -
*      Number of bytes to load from each of the
*      sources.
*    unsigned int offset -
*      Offest of required info, from the beginning
*      of each of the sources/images.
*  SOC_SAND_INDIRECT:
*    EEPROM definitions.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  get_nv_vals(
    void         **val_ptrs,
    unsigned int size,
    unsigned int offset
  )
{
  int
    err,
    ret ;
  void
    *dest ;
  STATUS
    status ;
  ret = 0 ;
  dest = val_ptrs[0] ;
  status = nvRamGet((char *)dest,(int)size,(int)offset) ;
  if (status == ERROR)
  {
    ret = 1 ;
    goto genv_exit ;
  }
  dest = val_ptrs[1] ;
  err = get_defaults((char *)dest,(int)size,(int)offset) ;
  if (err)
  {
    ret = 2 ;
    goto genv_exit ;
  }
  dest = val_ptrs[2] ;
  err = get_run_vals((char *)dest,(int)size,(int)offset) ;
  if (err)
  {
    ret = 3 ;
    goto genv_exit ;
  }
genv_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  handle_nv_val
*TYPE: PROC
*DATE: 23/APR/2002
*FUNCTION:
*  Depaending on action, get/set char/short/long
*  values in NV ram and then load:
*    Value contained in NV ram,
*    Default value,
*    Runtime value.
*CALLING SEQUENCE:
*  handle_nv_val(val_ptrs,size,
*                     offset,action,changed)
*INPUT:
*  SOC_SAND_DIRECT:
*    void         **val_ptrs -
*      Pointer to an array of 3 pointers which,
*      themselves, point to the location to load/get
*      information as specified above (in this
*      order): NV ram, Default value, runtime value.
*    unsigned int size -
*      Number of bytes to load from each of the
*      sources.
*    unsigned int offset -
*      Offest of required info, from the beginning
*      of each of the sources/images.
*    unsigned int action -
*      Bit map. Required action:
*        PRINT_ITEMS -
*          Read parameters, load as specified in
*          'val_ptrs' and print according to table
*          conventions. Currently not in use by
*          this procedure.
*        CHANGE_NV -
*          The value at *val_ptrs[0] (as input)
*          is to be loaded into NV ram.
*        CHANGE_RUN_TIME -
*          The value at *val_ptrs[2] (as input)
*          is to be loaded into rantime image.
*    unsigned int *changed -
*      Flag. This procedure loads pointed address
*      by a non-zero value if three things are found
*      true:
*      1. 'action' is also CHANGE_RUN_TIME.
*      2. New value for parameter is different
*         from initial value.
*      3. Size is smaller than (MAX_SIZE_OF_TEXT_VAR+1).
*      Initially, a value of '0' is loaded into
*      pointed address.
*  SOC_SAND_INDIRECT:
*    EEPROM definitions.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    changed, val_ptrs.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  handle_nv_val(
    void         **val_ptrs,
    unsigned int size,
    unsigned int offset,
    unsigned int action,
    unsigned int *changed
  )
{
  int
    err,
    ret ;
  void
    *dest ;
  STATUS
    status ;
  unsigned
    int
      handled ;
  unsigned
    char
      uc[MAX_SIZE_OF_TEXT_VAR] ;
  ret = 0 ;
  *changed = 0 ;
  if (action & CHANGE_NV)
  {
    dest = val_ptrs[0] ;
    status = nvRamSet((char *)dest,(int)size,(int)offset) ;
    if (status == ERROR)
    {
      ret = 1 ;
      goto genv_exit ;
    }
  }
  if (action & CHANGE_RUN_TIME)
  {
    handled = FALSE ;
    if (size <= MAX_SIZE_OF_TEXT_VAR)
    {
      err = get_run_vals((char *)&uc[0],(int)size,(int)offset) ;
      if (err)
      {
        ret = 4 ;
       goto genv_exit ;
      }
      handled = TRUE ;
    }
    dest = val_ptrs[2] ;
    err = set_run_vals((char *)dest,(int)size,(int)offset) ;
    if (err)
    {
      ret = 3 ;
     goto genv_exit ;
    }
    if (handled)
    {
      *changed = memcmp((void *)uc,dest,size) ;
    }
  }
  err = get_nv_vals((void **)val_ptrs,size,(int)offset) ;
  if (err)
  {
    ret = 2 ;
    goto genv_exit ;
  }
genv_exit:
  return (ret) ;
}

void nice_printed_snmp_cmd_line(char *out, char *in1, char *in2, char *in3 , int out_len)
{
  int size1, size2, size3, no_of_lines, i;
  char line[80];
  char nv[8][17];
  char df[8][17];
  char rt[8][17];
  /*
   */
  size1 = strlen(in1)+1;
  size2 = strlen(in2)+1;
  size3 = strlen(in3)+1;
  no_of_lines = (size1 > size2) ? size1 : size2 ;
  no_of_lines = (no_of_lines > size3) ? no_of_lines : size3 ;
  no_of_lines += 15; /*round up*/
  no_of_lines /= 16; /*num_of_lines*/
  if(no_of_lines<2)
  {
    no_of_lines = 2; /*print at least 2 rows*/
  }
  /*
   */
  for(i=0; i<no_of_lines; ++i)
  {
    strncpy(nv[i], in1+(16*i), 16);
    strncpy(df[i], in2+(16*i), 16);
    strncpy(rt[i], in3+(16*i), 16);
    nv[i][16] = '\0';
    df[i][16] = '\0';
    rt[i][16] = '\0';
  }
  /*
   */
  sal_sprintf(out,  " SNMP cmd line   |%-16s|%-16s|%-16s|\r\n", nv[0], df[0], rt[0]);
  sal_sprintf(line, "   arguments     |%-16s|%-16s|%-16s|\r\n", nv[1], df[1], rt[1]);

  UTILS_STRCAT_SAFE(out,line, out_len) ;

  for(i=2; i<no_of_lines; ++i)
  {
    sal_sprintf(line, "                 |%-16s|%-16s|%-16s|\r\n", nv[i], df[i], rt[i]);
    UTILS_STRCAT_SAFE(out,line, out_len) ;
  }
}
/*****************************************************
*NAME
*  handle_nv_item
*TYPE: PROC
*DATE: 26/MAR/2002
*FUNCTION:
*  Print specified parameter for display of nvram,
*  default and runtime values. Also, get and/or change.
*CALLING SEQUENCE:
*  handle_nv_item(val_ptrs,param_id,action)
*INPUT:
*  SOC_SAND_DIRECT:
*    void         **val_ptrs -
*      Pointer to an array of 3 pointers which,
*      themselves, point to the location to load
*      information as specified by param_id (in this
*      order): NV ram, Default value, runtime value.
*      Type of pointed locations must correspond
*      to type of NVRAM variable.
*      Pointed locations may also serve as input
*      parameters. See 'action'.
*    unsigned int param_id -
*      Identifier of parameter to handle:
*        PARAM_NVRAM_DATA_CACHE_ID -
*          Data cache enable/disable.
*    unsigned int action -
*      Bit map. Required action:
*        PRINT_ITEMS -
*          Read parameters, load as specified in
*          'val_ptrs' and print according to table
*          conventions.
*        CHANGE_NV -
*          The value at *val_ptrs[0] (as input)
*          is to be loaded into NV ram.
*        CHANGE_RUN_TIME -
*          The value at *val_ptrs[2] (as input)
*          is to be loaded into runtime image.
*  SOC_SAND_INDIRECT:
*    Table conventions. Eeprom structure.
*    val_ptrs.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Printed parameter values. Updated parameter
*    values.
*    val_ptrs.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  handle_nv_item(
    void         **val_ptrs,
    unsigned int param_id,
    unsigned int action
  )
{
  int
    ret,
    err = 0;
  char
    *err_msg = NULL,
    cat_str[80] ;
  unsigned
    int
      changed,
      size,
      counter,
      offset ;
  unsigned
    char
      *uc_ptrs[3] ;
  unsigned
    long
      *ul_ptrs[3] ;
  unsigned
    short
      *us_ptrs[3] ;
  unsigned long reg_base;
  STATUS
    status ;
  ret = 0 ;
  size = 0;
  offset = 0;

  err_msg = sal_alloc(100*25);
  if(err_msg == NULL)
  {
  	send_string_to_screen("Memory allocation failed\n\r", TRUE);
  	goto exit;
  }
  
  for (counter = 0 ; counter < (sizeof(uc_ptrs)/sizeof(uc_ptrs[0])) ; counter++)
  {
    uc_ptrs[counter] = ((unsigned char **)val_ptrs)[counter] ;
  }
  for (counter = 0 ; counter < (sizeof(ul_ptrs)/sizeof(ul_ptrs[0])) ; counter++)
  {
    ul_ptrs[counter] = ((unsigned long **)val_ptrs)[counter] ;
  }
  for (counter = 0 ; counter < (sizeof(us_ptrs)/sizeof(us_ptrs[0])) ; counter++)
  {
    us_ptrs[counter] = ((unsigned short **)val_ptrs)[counter] ;
  }
  switch (param_id)
  {
    case PARAM_SNMP_DOWNLOAD_MIBS_ID:
    {
      unsigned char d_mibs ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_06.un_ee_block_06.block_06.download_mibs)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_06.un_ee_block_06.block_06.download_mibs) ;
      if (action & CHANGE_RUN_TIME)
      {
        err = get_run_vals((char *)(&d_mibs),(int)size,(int)offset) ;
        if (err)
        {
          ret = 15 ;
          goto hanv_exit ;
        }
      }
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 17 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is nothing to do, concerning mib downloading, in runtime.
         * It specifically relates to steps taken at startup only!
         * If this logic is changed, this is the place to make the change.
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Download MIBs at|                |                |                |\r\n"
          " startup (0=NO)  |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_SNMP_INCLUDE_NETSNMP_ID:
    {
      unsigned char snmp_flag ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_06.un_ee_block_06.block_06.load_netsnmp)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_06.un_ee_block_06.block_06.load_netsnmp) ;
      if (action & CHANGE_RUN_TIME)
      {
        err = get_run_vals((char *)(&snmp_flag),(int)size,(int)offset) ;
        if (err)
        {
          ret = 16 ;
          goto hanv_exit ;
        }
      }
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 18 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is nothing to do, concerning including the package, in runtime.
         * It specifically relates to steps taken at startup only!
         * If this logic is changed, this is the place to make the change.
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Load NetSNMP at |                |                |                |\r\n"
          " startup (0=NO)  |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_SNMP_SET_CMD_LINE_ID:
    {
      char          snmp_cmd_line[SNMP_CMD_LINE_SIZE + 1] ;
      unsigned char *c_ptr ;
      unsigned int  c_size ;
      changed = 0;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_06.un_ee_block_06.block_06.snmp_cmd_line)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_06.un_ee_block_06.block_06.snmp_cmd_line) ;
      if (action & CHANGE_NV)
      {
        memset(snmp_cmd_line,0,sizeof(snmp_cmd_line)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(snmp_cmd_line)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(snmp_cmd_line,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(snmp_cmd_line) ;
          memcpy(snmp_cmd_line,uc_ptrs[0],c_size) ;
          snmp_cmd_line[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)snmp_cmd_line,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 22 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(snmp_cmd_line,0,sizeof(snmp_cmd_line)) ;
        c_ptr = memchr(uc_ptrs[2],0,sizeof(snmp_cmd_line)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(snmp_cmd_line,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(snmp_cmd_line) ;
          memcpy(snmp_cmd_line,uc_ptrs[2],c_size) ;
          snmp_cmd_line[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)snmp_cmd_line,(int)size,(int)offset) ;
        if (err)
        {
          ret = 32 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 11 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        nice_printed_snmp_cmd_line(err_msg, uc_ptrs[0], uc_ptrs[1], uc_ptrs[2], 80*10);
        send_string_to_screen(err_msg,FALSE) ;
      }
      break ;
    }
    case PARAM_NVRAM_BASE_REGISTER_04_ID:
    case PARAM_NVRAM_BASE_REGISTER_05_ID:
    case PARAM_NVRAM_OPTION_REGISTER_04_ID:
    case PARAM_NVRAM_OPTION_REGISTER_05_ID:
    {
      if (PARAM_NVRAM_BASE_REGISTER_04_ID == param_id)
      {
        offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                      ee_block_02.un_ee_block_02.block_02.base_register_04)) ;
        size = sizeof(((EE_AREA *)0)->
                      ee_block_02.un_ee_block_02.block_02.base_register_04) ;
      }
      else if (PARAM_NVRAM_BASE_REGISTER_05_ID == param_id)
      {
        offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                      ee_block_02.un_ee_block_02.block_02.base_register_05)) ;
        size = sizeof(((EE_AREA *)0)->
                      ee_block_02.un_ee_block_02.block_02.base_register_05) ;
      }
      else if (PARAM_NVRAM_OPTION_REGISTER_04_ID == param_id)
      {
        offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                      ee_block_02.un_ee_block_02.block_02.option_register_04)) ;
        size = sizeof(((EE_AREA *)0)->
                      ee_block_02.un_ee_block_02.block_02.option_register_04) ;
      }
      else if (PARAM_NVRAM_OPTION_REGISTER_05_ID == param_id)
      {
        offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                      ee_block_02.un_ee_block_02.block_02.option_register_05)) ;
        size = sizeof(((EE_AREA *)0)->
                      ee_block_02.un_ee_block_02.block_02.option_register_05) ;
      }
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        reg_base = vxImmrGet();
        if (PARAM_NVRAM_BASE_REGISTER_04_ID == param_id)
        {
          *BR4(reg_base) = *ul_ptrs[2];
        }
        else if (PARAM_NVRAM_BASE_REGISTER_05_ID == param_id)
        {
          *BR5(reg_base) = *ul_ptrs[2];
        }
        else if (PARAM_NVRAM_OPTION_REGISTER_04_ID == param_id)
        {
          *OR4(reg_base) = *ul_ptrs[2];
        }
        else if (PARAM_NVRAM_OPTION_REGISTER_05_ID == param_id)
        {
          *OR5(reg_base) = *ul_ptrs[2];
        }
      }
      if (action & PRINT_ITEMS)
      {
        if (PARAM_NVRAM_BASE_REGISTER_04_ID == param_id)
        {
          sal_sprintf(err_msg," Base register 04|");
        }
        else if (PARAM_NVRAM_BASE_REGISTER_05_ID == param_id)
        {
          sal_sprintf(err_msg," Base register 05|");
        }
        else if (PARAM_NVRAM_OPTION_REGISTER_04_ID == param_id)
        {
          sal_sprintf(err_msg," Option reg 04   |");
        }
        else if (PARAM_NVRAM_OPTION_REGISTER_05_ID == param_id)
        {
          sal_sprintf(err_msg," Option reg 05   |");
        }
        sal_sprintf(cat_str,"   0x%08lX   |   0x%08lX   |   0x%08lX   |",*ul_ptrs[0],*ul_ptrs[1],*ul_ptrs[2]);

        UTILS_STRCAT_SAFE(err_msg,cat_str, 80*10) ;
        send_string_to_screen(err_msg,TRUE);
      }
      break ;
    }
    case PARAM_NVRAM_LINE_MODE_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.line_mode)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.line_mode) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        unsigned
          char
            line_mode ;
        line_mode = *uc_ptrs[2] ;
        if (line_mode)
        {
          set_line_mode() ;
        }
        else
        {
          clear_line_mode() ;
        }
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Line mode (1=ON)|       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_ACTIVATE_WATCHDOG_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.activate_watchdog)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.activate_watchdog) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        unsigned
          char
            watchdog_period,
            activate_watchdog ;
        activate_watchdog = *uc_ptrs[2] ;
        if (activate_watchdog)
        {
          err = get_watchdog_period(&watchdog_period) ;
          err |= set_watchdog_inject_time() ;
          err |=
            attach_irq((void *)watchdog_handler,0,(int)0,err_msg) ;
          if (err)
          {
            ret = 30 + err ;
            goto hanv_exit ;
          }
          enable_internal_watchdog(watchdog_period,FALSE) ;
        }
        else
        {
          disable_internal_watchdog() ;
          err = detach_irq(0,err_msg) ;
          if (err)
          {
            ret = 40 + err ;
            goto hanv_exit ;
          }
        }
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Watchdog (1=ON) |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOOK_SOFTWARE_EXCEPTION_EXC_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.hook_software_exception)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.hook_software_exception) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        send_string_to_screen(" Hook software   |                |                |                |"
                              ,TRUE) ;
        sal_sprintf(
          err_msg,
          " exception (1=ON)|       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_BCKG_TEMPERATURES_EN_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.bckg_temperatures_en)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.bckg_temperatures_en) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        send_string_to_screen(" Bckg temperatur |                |                |                |"
                              ,TRUE) ;
        sal_sprintf(
          err_msg,
          " en collect(1=ON)|       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_ACTIVATE_DPSS_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.usr_app_flavor)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.usr_app_flavor) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is no need for special action here since control
         * of DPSS activation depends only on nvram value of this
         * parameter (at startup).
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Activate Marvell|                |                |                |\r\n"
          " DPSS (1=ON)     |                |                |                |\r\n"
          "(On SOC_SAND_FAP20V system|                |                |                |\r\n"
          "activate app)    |       %3u      |       %3u      |       %3u      |\r\n"
          "                 | Deprecated feature. Use 'usr_app_flavor'.  |\r\n"
          "                 |                                                  |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_USR_APP_FLAVOR_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.usr_app_flavor)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.usr_app_flavor) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is no need for special action here since control
         * of DPSS activation depends only on nvram value of this
         * parameter (at startup).
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " User            |                |                |                |\r\n"
          " Application     |       %02u       |       %02u       |       %02u       |\r\n"
          " Flavor          | %-15s| %-15s| %-15s|\r\n"
          "                 |                                                  |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2],
          d_usr_app_flavor_to_str((SOC_D_USR_APP_FLAVOR)*uc_ptrs[0], TRUE),
          d_usr_app_flavor_to_str((SOC_D_USR_APP_FLAVOR)*uc_ptrs[1], TRUE),
          d_usr_app_flavor_to_str((SOC_D_USR_APP_FLAVOR)*uc_ptrs[2], TRUE)) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_RUN_UI_STARTUP_SCRIPT_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.run_ui_startup_script)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.run_ui_startup_script) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is no need for special action here since control
         * of ui startup script activation depends only on nvram value of this
         * parameter (at startup).
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Run ui script at|                |                |                |\r\n"
          " startup  (1=ON) |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_DPSS_LOAD_FROM_FLASH_NOT_TFTP_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.load_dpss_from_file_on_flash)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.load_dpss_from_file_on_flash) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is no need for special action here since control
         * of DPSS config file load method depends only on nvram value of this
         * parameter (at startup).
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " DPSS load file  |                |                |                |\r\n"
          " from flash(1=ON)|       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_PAGE_MODE_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.page_mode)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.page_mode) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        unsigned
          char
            page_mode ;
        page_mode = *uc_ptrs[2] ;
        if (page_mode)
        {
          set_page_mode() ;
        }
        else
        {
          clear_page_mode() ;
        }
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Page mode (1=ON)|       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_PAGE_LINES_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.page_lines)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.page_lines) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        unsigned
          char
            lines_per_page ;
        lines_per_page = *uc_ptrs[2] ;
        set_lines_per_page((unsigned int)lines_per_page) ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Lines per page  |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_WATCHDOG_PERIOD_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.watchdog_period)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.watchdog_period) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        unsigned
          char
            watchdog_period ;
        watchdog_period = *uc_ptrs[2] ;
        err = set_watchdog_inject_time() ;
        if (err)
        {
          /*
           * This is an error. 'set_watchdog_inject_time' failed (could not read
           * clock).
           */
          ret = 30 + err ;
          goto hanv_exit ;
        }
        set_internal_watchdog_period(watchdog_period) ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Watchdog period |                |                |                |\r\n"
          " Units: 100 ms   |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_DPSS_DISPLAY_LEVEL_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.dpss_display_level)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.dpss_display_level) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is no need for special action here since displaying
         * of DPSS events depends directly on the runtime value of this
         * parameter (via get_dpss_display_level).
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " DPSS display    |                |                |                |\r\n"
          " level           |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_DPSS_DEBUG_LEVEL_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.dpss_debug_level)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.dpss_debug_level) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is no need for special action here since debuging
         * of DPSS events depends directly on the runtime value of this
         * parameter (via get_dpss_debug_level).
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " DPSS debug level|       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_STARTUP_DEMO_MODE_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.start_application_in_demo_mode)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.start_application_in_demo_mode) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is no need for special action here since debuging
         * of DPSS events depends directly on the runtime value of this
         * parameter (via get_dpss_debug_level).
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " (DPSS) start app|       %02u       |       %02u       |       %02u       |\r\n"
          "   in demo mode  | Legend: {00 = no_demo}, {10 = negev_demo}        |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_FAP10M_RUN_MODE_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                      ee_block_03.un_ee_block_03.block_03.fap10m_run_mode)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.fap10m_run_mode) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is no need for special action here since debuging
         * of DPSS events depends directly on the runtime value of this
         * parameter (via get_dpss_debug_level).
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " SOC_SAND_FAP10M run mode |       %2u       |       %2u       |       %2u       |\r\n"
          "                 | Legend: {0 = eci}, {1 = pci}, {2 = i2c}          |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_EVENT_DISPLAY_LVL_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.event_display_level)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.event_display_level) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is no need for special action here since displaying
         * of events depends directly on the runtime value of this
         * parameter (via get_err_display_level).
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Event display   |                |                |                |\r\n"
          " level           |       %s      |       %s      |       %s      |",
          severity_to_text((int)((unsigned int)(*uc_ptrs[0]))),
          severity_to_text((int)((unsigned int)(*uc_ptrs[1]))),
          severity_to_text((int)((unsigned int)(*uc_ptrs[2])))) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_EVENT_NVRAM_LVL_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.event_nvram_level)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.event_nvram_level) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * There is no need for special action here since displaying
         * of events depends directly on the runtime value of this
         * parameter (via get_nv_store_level).
         */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Event level to  |                |                |                |\r\n"
          " store in NVRAM  |                |                |                |\r\n"
          " log file        |       %s      |       %s      |       %s      |",
          severity_to_text((int)((unsigned int)(*uc_ptrs[0]))),
          severity_to_text((int)((unsigned int)(*uc_ptrs[1]))),
          severity_to_text((int)((unsigned int)(*uc_ptrs[2])))) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_EVENT_SYSLOG_LVL_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                                        ee_block_03.un_ee_block_03.block_03.event_syslog_level)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.event_syslog_level) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
      /*
       * There is no need for special action here since displaying
       * of events depends directly on the runtime value of this
       * parameter (via get_nv_store_level).
       */
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
            err_msg,
            " Event level to  |                |                |                |\r\n"
            " send to syslog  |       %s      |       %s      |       %s      |",
            severity_to_text((int)((unsigned int)(*uc_ptrs[0]))),
            severity_to_text((int)((unsigned int)(*uc_ptrs[1]))),
            severity_to_text((int)((unsigned int)(*uc_ptrs[2])))) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_TELNET_TIMEOUT_STD_ID:
    {
      char
        strings[3][5] ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.telnet_activity_timeout_std)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.telnet_activity_timeout_std) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        unsigned
          char
            telnet_activity_timeout_std ;
        unsigned
          long
            val ;
        telnet_activity_timeout_std = *uc_ptrs[2] ;
        if (telnet_activity_timeout_std == TELNET_INACTIVITY_NONE)
        {
          val = -1 ;
        }
        else
        {
          val = (unsigned long)telnet_activity_timeout_std ;
        }
        set_telnet_timeout_val(val) ;
      }
      if (action & PRINT_ITEMS)
      {
        unsigned
          short
            us ;
        for (counter = 0 ; counter < 3 ; counter++)
        {
          us = (unsigned short)(*uc_ptrs[counter]) ;
          if (us == TELNET_INACTIVITY_NONE)
          {
            sal_sprintf(&strings[counter][0],"NONE") ;
          }
          else
          {
            sal_sprintf(&strings[counter][0],"%04u",us) ;
          }
        }
        sal_sprintf(
          err_msg,
          " Standard Telnet |                |                |                |\r\n"
          " inactivity      |                |                |                |\r\n"
          " timeout         |                |                |                |\r\n"
          " Units: Seconds  |      %s      |      %s      |      %s      |",
          strings[0],strings[1],strings[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_TELNET_TIMEOUT_CONT_ID:
    {
      char
        strings[3][5] ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.telnet_activity_timeout_cont)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.telnet_activity_timeout_cont) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * Note,
         * Continuous display can not be active at this point so
         * there is nothing to change online. Timeout for continuous display
         * is changed when continuous display is activated.
         */
      }
      if (action & PRINT_ITEMS)
      {
        unsigned
          short
            us ;
        for (counter = 0 ; counter < 3 ; counter++)
        {
          us = (unsigned short)(*uc_ptrs[counter]) ;
          if (us == TELNET_INACTIVITY_NONE)
          {
            sal_sprintf(&strings[counter][0],"NONE") ;
          }
          else
          {
            sal_sprintf(&strings[counter][0],"%04u",us) ;
          }
        }
        sal_sprintf(
          err_msg,
          " Tlnet Inactivity|                |                |                |\r\n"
          " timeout for     |                |                |                |\r\n"
          " periodic display|                |                |                |\r\n"
          " Units: Seconds  |      %s      |      %s      |      %s      |",
          strings[0],strings[1],strings[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_CONF_FROM_MD_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.conf_from_md[get_pointed_fe()])) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.conf_from_md[get_pointed_fe()]) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " FE: Source of   |                |                |                |\r\n"
          " \'configuration\' |                |                |                |\r\n"
          " (1=MD,0=NVRAM)  |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_FE_CONFIGURATION_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.fe_configuration[get_pointed_fe()])) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_03.un_ee_block_03.block_03.fe_configuration[get_pointed_fe()]) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " FE: Value of    |                |                |                |\r\n"
          " \'configuration\' |                |                |                |\r\n"
          " register (hex)  |     0x%04X     |     0x%04X     |     0x%04X     |",
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_BOARD_DESCRIPTION_ID:
    {
      char
        board_description[B_MAX_BOARD_DESCRIPTION_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            ser_ee_block_02.un_serial_ee_block_02.
            serial_ee_block_02.board_description)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            ser_ee_block_02.un_serial_ee_block_02.
            serial_ee_block_02.board_description) ;
      if (action & CHANGE_NV)
      {
        memset(board_description,0,sizeof(board_description)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(board_description)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(board_description,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(board_description) ;
          memcpy(board_description,uc_ptrs[0],c_size) ;
          board_description[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)board_description,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(board_description,0,sizeof(board_description)) ;
        c_ptr = memchr(uc_ptrs[2],0,sizeof(board_description)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(board_description,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(board_description) ;
          memcpy(board_description,uc_ptrs[2],c_size) ;
          board_description[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)board_description,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          "Board description|--------------- |--------------- |--------------- |\r\n"
          "  NVRAM          | %-40s         |\r\n"
          "    Default      |   %-40s       |\r\n"
          "      Run time   |     %-40s     |",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_BOARD_DESC_ID:
    {
      char
        host_board_description[B_HOST_MAX_BOARD_DESCRIPTION_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            host_ser_ee_block_02.un_host_serial_ee_block_02.
            host_serial_ee_block_02.host_board_description)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            host_ser_ee_block_02.un_host_serial_ee_block_02.
            host_serial_ee_block_02.host_board_description) ;
      if (action & CHANGE_NV)
      {
        memset(host_board_description,0,sizeof(host_board_description)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(host_board_description)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(host_board_description,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(host_board_description) ;
          memcpy(host_board_description,uc_ptrs[0],c_size) ;
          host_board_description[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)host_board_description,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(host_board_description,0,sizeof(host_board_description)) ;
        c_ptr = memchr(uc_ptrs[2],0,sizeof(host_board_description)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(host_board_description,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(host_board_description) ;
          memcpy(host_board_description,uc_ptrs[2],c_size) ;
          host_board_description[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)host_board_description,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          "  Host board     |------------------------------------------------- |\r\n"
          "   description   |                                                  |\r\n"
          "  NVRAM          |%-50s|\r\n"
          "    Default      |%-50s|\r\n"
          "      Run time   |%-50s|",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_BOARD_CAT_NUM_ID:
    {
      char
        catalog_number[B_HOST_MAX_CATALOG_NUMBER_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            host_ser_ee_block_02.un_host_serial_ee_block_02.
            host_serial_ee_block_02.host_board_catalog_number)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            host_ser_ee_block_02.un_host_serial_ee_block_02.
            host_serial_ee_block_02.host_board_catalog_number) ;
      if (action & CHANGE_NV)
      {
        memset(catalog_number,0,sizeof(catalog_number)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(catalog_number)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(catalog_number,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(catalog_number) ;
          memcpy(catalog_number,uc_ptrs[0],c_size) ;
          catalog_number[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)catalog_number,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(catalog_number,0,sizeof(catalog_number));
        c_ptr = memchr(uc_ptrs[2],0,sizeof(catalog_number));
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(catalog_number,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(catalog_number) ;
          memcpy(catalog_number,uc_ptrs[2],c_size) ;
          catalog_number[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)catalog_number,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Catalog number  |    %-9s   |    %-9s   |    %-9s   |",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_BOARD_VER_ID:
    {
      char
        version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            host_ser_ee_block_02.un_host_serial_ee_block_02.
            host_serial_ee_block_02.host_board_version)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            host_ser_ee_block_02.un_host_serial_ee_block_02.
            host_serial_ee_block_02.host_board_version) ;
      if (action & CHANGE_NV)
      {
        memset(version,0,sizeof(version)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(version)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(version,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(version) ;
          memcpy(version,uc_ptrs[0],c_size) ;
          version[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)version,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(version,0,sizeof(version));
        c_ptr = memchr(uc_ptrs[2],0,sizeof(version));
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(version,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(version) ;
          memcpy(version,uc_ptrs[2],c_size) ;
          version[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)version,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Host board ver  |     %-7s    |     %-7s    |     %-7s    |",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_FRONT_BOARD_DESC_ID:
    {
      char
        front_board_description[B_FRONT_MAX_BOARD_DESCRIPTION_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            front_ser_ee_block_02.un_front_serial_ee_block_02.
            front_serial_ee_block_02.front_board_description)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            front_ser_ee_block_02.un_front_serial_ee_block_02.
            front_serial_ee_block_02.front_board_description) ;
      if (action & CHANGE_NV)
      {
        memset(front_board_description,0,sizeof(front_board_description)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(front_board_description)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(front_board_description,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(front_board_description) ;
          memcpy(front_board_description,uc_ptrs[0],c_size) ;
          front_board_description[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)front_board_description,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(front_board_description,0,sizeof(front_board_description)) ;
        c_ptr = memchr(uc_ptrs[2],0,sizeof(front_board_description)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(front_board_description,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(front_board_description) ;
          memcpy(front_board_description,uc_ptrs[2],c_size) ;
          front_board_description[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)front_board_description,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          "  Front board    |------------------------------------------------- |\r\n"
          "   description   |                                                  |\r\n"
          "  NVRAM          |%-50s|\r\n"
          "    Default      |%-50s|\r\n"
          "      Run time   |%-50s|",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_FRONT_BOARD_CAT_NUM_ID:
    {
      char
        catalog_number[B_FRONT_MAX_CATALOG_NUMBER_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            front_ser_ee_block_02.un_front_serial_ee_block_02.
            front_serial_ee_block_02.front_board_catalog_number)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            front_ser_ee_block_02.un_front_serial_ee_block_02.
            front_serial_ee_block_02.front_board_catalog_number) ;
      if (action & CHANGE_NV)
      {
        memset(catalog_number,0,sizeof(catalog_number)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(catalog_number)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(catalog_number,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(catalog_number) ;
          memcpy(catalog_number,uc_ptrs[0],c_size) ;
          catalog_number[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)catalog_number,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(catalog_number,0,sizeof(catalog_number));
        c_ptr = memchr(uc_ptrs[2],0,sizeof(catalog_number));
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(catalog_number,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(catalog_number) ;
          memcpy(catalog_number,uc_ptrs[2],c_size) ;
          catalog_number[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)catalog_number,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Catalog number  |    %-9s   |    %-9s   |    %-9s   |",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_FRONT_BOARD_VER_ID:
    {
      char
        version[B_FRONT_MAX_DEFAULT_BOARD_VERSION_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            front_ser_ee_block_02.un_front_serial_ee_block_02.
            front_serial_ee_block_02.front_board_version)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            front_ser_ee_block_02.un_front_serial_ee_block_02.
            front_serial_ee_block_02.front_board_version) ;
      if (action & CHANGE_NV)
      {
        memset(version,0,sizeof(version)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(version)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(version,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(version) ;
          memcpy(version,uc_ptrs[0],c_size) ;
          version[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)version,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(version,0,sizeof(version));
        c_ptr = memchr(uc_ptrs[2],0,sizeof(version));
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(version,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(version) ;
          memcpy(version,uc_ptrs[2],c_size) ;
          version[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)version,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Front board ver |     %-7s    |     %-7s    |     %-7s    |",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_DB_BOARD_DESC_ID:
    {
      char
        host_db_board_description[B_HOST_DB_MAX_BOARD_DESCRIPTION_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
            host_db_serial_ee_block_02.host_db_board_description)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
            host_db_serial_ee_block_02.host_db_board_description) ;
      if (action & CHANGE_NV)
      {
        memset(host_db_board_description,0,sizeof(host_db_board_description)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(host_db_board_description)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(host_db_board_description,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(host_db_board_description) ;
          memcpy(host_db_board_description,uc_ptrs[0],c_size) ;
          host_db_board_description[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)host_db_board_description,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(host_db_board_description,0,sizeof(host_db_board_description)) ;
        c_ptr = memchr(uc_ptrs[2],0,sizeof(host_db_board_description)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(host_db_board_description,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(host_db_board_description) ;
          memcpy(host_db_board_description,uc_ptrs[2],c_size) ;
          host_db_board_description[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)host_db_board_description,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          "  Host_db board  |------------------------------------------------- |\r\n"
          "   description   |                                                  |\r\n"
          "  NVRAM          |%-50s|\r\n"
          "    Default      |%-50s|\r\n"
          "      Run time   |%-50s|",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_DB_BOARD_CAT_NUM_ID:
    {
      char
        catalog_number[B_HOST_DB_MAX_CATALOG_NUMBER_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
            host_db_serial_ee_block_02.host_db_board_catalog_number)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
            host_db_serial_ee_block_02. host_db_board_catalog_number) ;
      if (action & CHANGE_NV)
      {
        memset(catalog_number,0,sizeof(catalog_number)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(catalog_number)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(catalog_number,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(catalog_number) ;
          memcpy(catalog_number,uc_ptrs[0],c_size) ;
          catalog_number[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)catalog_number,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(catalog_number,0,sizeof(catalog_number));
        c_ptr = memchr(uc_ptrs[2],0,sizeof(catalog_number));
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(catalog_number,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(catalog_number) ;
          memcpy(catalog_number,uc_ptrs[2],c_size) ;
          catalog_number[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)catalog_number,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Catalog number  |    %-9s   |    %-9s   |    %-9s   |",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_DB_BOARD_VER_ID:
    {
      char
        version[B_HOST_DB_MAX_DEFAULT_BOARD_VERSION_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
            host_db_serial_ee_block_02.host_db_board_version)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
            host_db_serial_ee_block_02.host_db_board_version) ;
      if (action & CHANGE_NV)
      {
        memset(version,0,sizeof(version)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(version)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(version,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(version) ;
          memcpy(version,uc_ptrs[0],c_size) ;
          version[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)version,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(version,0,sizeof(version));
        c_ptr = memchr(uc_ptrs[2],0,sizeof(version));
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(version,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(version) ;
          memcpy(version,uc_ptrs[2],c_size) ;
          version[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)version,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Host_db brd ver |     %-7s    |     %-7s    |     %-7s    |",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_BOARD_TYPE_ID:
    {
      char
        board_name[B_MAX_BOARD_NAME_LEN] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
            ser_ee_block_02.un_serial_ee_block_02.
            serial_ee_block_02.board_name)) + SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(((SERIAL_EE_AREA *)0)->
            ser_ee_block_02.un_serial_ee_block_02.
            serial_ee_block_02.board_name) ;
      if (action & CHANGE_NV)
      {
        memset(board_name,0,sizeof(board_name)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(board_name)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(board_name,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(board_name) ;
          memcpy(board_name,uc_ptrs[0],c_size) ;
          board_name[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)board_name,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(board_name,0,sizeof(board_name)) ;
        c_ptr = memchr(uc_ptrs[2],0,sizeof(board_name)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(board_name,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(board_name) ;
          memcpy(board_name,uc_ptrs[2],c_size) ;
          board_name[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)board_name,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Board type      |   %-10s   |   %-10s   |   %-10s   |",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_DLD_FILE_NAME_ID:
    {
      char
        dld_file_name[DLD_FILE_NAME_LEN + 1] ;
      unsigned
        char
          *c_ptr ;
      unsigned
        int
          c_size ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.dld_file_name)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.dld_file_name) ;
      if (action & CHANGE_NV)
      {
        memset(dld_file_name,0,sizeof(dld_file_name)) ;
        c_ptr = memchr(uc_ptrs[0],0,sizeof(dld_file_name)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[0]) ;
          c_size++ ;
          memcpy(dld_file_name,uc_ptrs[0],c_size) ;
        }
        else
        {
          c_size = sizeof(dld_file_name) ;
          memcpy(dld_file_name,uc_ptrs[0],c_size) ;
          dld_file_name[c_size - 1] = 0 ;
        }
        status = nvRamSet((char *)dld_file_name,(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        memset(dld_file_name,0,sizeof(dld_file_name)) ;
        c_ptr = memchr(uc_ptrs[2],0,sizeof(dld_file_name)) ;
        if (c_ptr)
        {
          c_size = (unsigned int)(c_ptr - uc_ptrs[2]) ;
          c_size++ ;
          memcpy(dld_file_name,uc_ptrs[2],c_size) ;
        }
        else
        {
          c_size = sizeof(dld_file_name) ;
          memcpy(dld_file_name,uc_ptrs[2],c_size) ;
          dld_file_name[c_size - 1] = 0 ;
        }
        err = set_run_vals((char *)dld_file_name,(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Download file   |                |                |                |\r\n"
          "   name          |%-16s|%-16s|%-16s|",
          uc_ptrs[0],
          uc_ptrs[1],
          uc_ptrs[2]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_DLD_HOST_ADDR_ID:
    {
      char
        ascii_ip[3][16] ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.downloading_host_addr)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.downloading_host_addr) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Downloading     |                |                |                |\r\n"
          "   host          |%s |%s |%s |",
              sprint_ip(ascii_ip[0],*ul_ptrs[0], FALSE),
              sprint_ip(ascii_ip[1],*ul_ptrs[1], FALSE),
              sprint_ip(ascii_ip[2],*ul_ptrs[2], FALSE)
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_GATEWAY_ADDR_ID:
    {
      unsigned
        long
          old_gw_addr,
          new_gw_addr ;
      char
        ascii_ip[3][16] ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.default_gateway_addr)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.default_gateway_addr) ;
      if (action & CHANGE_RUN_TIME)
      {
        err = get_run_vals((char *)(&old_gw_addr),(int)size,(int)offset) ;
        if (err)
        {
          ret = 40 ;
          goto hanv_exit ;
        }
      }
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      new_gw_addr = *ul_ptrs[2] ;
      /*
       * Note that 'action' is not tested here, again, since
       * a non-zero 'changed' indicates both a change and
       * (action & CHANGE_RUN_TIME) which is non-zero.
       */
      if (changed)
      {
        char
          interfaceAddress[3*4+3+2] ;
        sprint_ip(interfaceAddress,old_gw_addr, TRUE) ;
        status = routeDelete("0",&interfaceAddress[0]) ;
        if (status == ERROR)
        {
          ret = 70 ;
          goto hanv_exit ;
        }
        sprint_ip(interfaceAddress,new_gw_addr, TRUE) ;
        status = routeAdd("0",&interfaceAddress[0]) ;
        if (status == ERROR)
        {
          sal_sprintf(
            err_msg,
              "\r\n\n"
              "*** Error trying to set new default gateway address (%s)\r\n"
              "    at runtime. Go back to old value (%s).\r\n",
                    sprint_ip(ascii_ip[0],new_gw_addr, FALSE),
                    sprint_ip(ascii_ip[1],old_gw_addr, FALSE)
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          err = set_run_vals((char *)(&old_gw_addr),(int)size,(int)offset) ;
          if (err)
          {
            ret = 80 ;
            goto hanv_exit ;
          }
          *ul_ptrs[2] = old_gw_addr ;
          sprint_ip(interfaceAddress,old_gw_addr, TRUE) ;
          status = routeAdd("0",&interfaceAddress[0]) ;
          if (status == ERROR)
          {
            ret = 90 ;
            goto hanv_exit ;
          }
        }
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Default         |                |                |                |\r\n"
          "   gateway       |%s |%s |%s |",
                sprint_ip(ascii_ip[0],*ul_ptrs[0], FALSE),
                sprint_ip(ascii_ip[1],*ul_ptrs[1], FALSE),
                sprint_ip(ascii_ip[2],*ul_ptrs[2], FALSE)
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_LOC_IP_MASK_ID:
    {
      unsigned
        long
          old_ip_mask,
          new_ip_mask ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.ip_mask)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.ip_mask) ;
      if (action & CHANGE_RUN_TIME)
      {
        err = get_run_vals((char *)(&old_ip_mask),(int)size,(int)offset) ;
        if (err)
        {
          ret = 40 ;
          goto hanv_exit ;
        }
      }
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      new_ip_mask = *ul_ptrs[2] ;
      /*
       * Note that 'action' is not tested here, again, since
       * a non-zero 'changed' indicates both a change and
       * (action & CHANGE_RUN_TIME) which is non-zero.
       */
      if (changed)
      {
        char
          interfaceAddress[3*4+3+2],
          destination[3*4+3+2] ;
        unsigned
          int
          addr_size,
          addr_offset ;
        unsigned
          long
            ip_addr ;
        addr_offset =
          (unsigned int)((char *)&(((EE_AREA *)0)->
                  ee_block_02.un_ee_block_02.block_02.ip_addr)) ;
        addr_size =
          sizeof(((EE_AREA *)0)->
                  ee_block_02.un_ee_block_02.block_02.ip_addr) ;
        err = get_run_vals((char *)(&ip_addr),(int)addr_size,(int)addr_offset) ;
        if (err)
        {
          ret = 50 ;
          goto hanv_exit ;
        }
        sprint_ip(interfaceAddress,ip_addr, TRUE) ;
        sprint_ip(destination,(ip_addr & old_ip_mask), TRUE) ;
        status = routeDelete(destination,interfaceAddress) ;
        if (status == ERROR)
        {
          ret = 80 ;
          goto hanv_exit ;
        }
        status = ifMaskSet(BOOT_DEV_NAME"0",new_ip_mask) ;
        if (status == ERROR)
        {
          ret = 60 ;
          goto hanv_exit ;
        }
        status = ifAddrSet(BOOT_DEV_NAME"0",&interfaceAddress[0]) ;
        if (status == ERROR)
        {
          ret = 70 ;
          goto hanv_exit ;
        }
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(err_msg,
          " IP mask         |   0x%08lX   |  0x%08lX    |  0x%08lX    |",
          *ul_ptrs[0],*ul_ptrs[1],*ul_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_LOC_IP_ADDR_ID:
    {
      unsigned
        long
          old_ip_addr,
          new_ip_addr ;
      char
        ascii_ip[3][16] ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.ip_addr)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.ip_addr) ;
      if (action & CHANGE_RUN_TIME)
      {
        err = get_run_vals((char *)(&old_ip_addr),(int)size,(int)offset) ;
        if (err)
        {
          ret = 40 ;
          goto hanv_exit ;
        }
      }
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      new_ip_addr = *ul_ptrs[2] ;
      /*
       * Note that 'action' is not tested here, again, since
       * a non-zero 'changed' indicates both a change and
       * (action & CHANGE_RUN_TIME) which is non-zero.
       */
      if (changed)
      {
        char
          interfaceAddress[3*4+3+2],
          destination[3*4+3+2] ;
        unsigned
          int
          mask_size,
          mask_offset ;
        int
          interfaceMask ;
        mask_offset =
          (unsigned int)((char *)&(((EE_AREA *)0)->
                  ee_block_02.un_ee_block_02.block_02.ip_mask)) ;
        mask_size =
          sizeof(((EE_AREA *)0)->
                  ee_block_02.un_ee_block_02.block_02.ip_mask) ;
        err = get_run_vals((char *)(&interfaceMask),(int)mask_size,(int)mask_offset) ;
        if (err)
        {
          ret = 50 ;
          goto hanv_exit ;
        }
        sprint_ip(destination,(old_ip_addr & interfaceMask), TRUE) ;
        sprint_ip(interfaceAddress,old_ip_addr, TRUE) ;
        status = routeDelete(destination,interfaceAddress) ;
        if (status == ERROR)
        {
          ret = 80 ;
          goto hanv_exit ;
        }
        status = ifMaskSet(BOOT_DEV_NAME"0",interfaceMask) ;
        if (status == ERROR)
        {
          ret = 60 ;
          goto hanv_exit ;
        }
        sprint_ip(interfaceAddress,new_ip_addr, TRUE) ;
        status = ifAddrSet(BOOT_DEV_NAME"0",&interfaceAddress[0]) ;
        if (status == ERROR)
        {
          ret = 70 ;
          goto hanv_exit ;
        }
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(err_msg," IP address      |%s |%s |%s |",
                sprint_ip(ascii_ip[0],*ul_ptrs[0], FALSE),
                sprint_ip(ascii_ip[1],*ul_ptrs[1], FALSE),
                sprint_ip(ascii_ip[2],*ul_ptrs[2], FALSE)) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_BOARD_TYPE_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                host_ser_ee_block_01.un_host_serial_ee_block_01.
                host_serial_ee_block_01.host_board_type)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                host_ser_ee_block_01.un_host_serial_ee_block_01.
                host_serial_ee_block_01.host_board_type) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 120 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          "  Host board\'s                |                |                |                |\r\n"
          "   type                        |                |                |                |\r\n"
          "%04u (unknown)                 |                |                |                |\r\n"
          "%04u (standalone)              |                |                |                |\r\n"
          "%04u (fabric)                  |                |                |                |\r\n"
          "%04u (SOC_SAND_FE600)                   |                |                |                |\r\n"
          "%04u (line_card)               |                |                |                |\r\n"
          "%04u (fap10m_b)                |                |                |                |\r\n"
          "%04u (line GFA SOC_SAND_FAP20V)         |                |                |                |\r\n"
          "%04u (GFA_MB SOC_SAND_FAP20V)           |                |                |                |\r\n"
          "%04u (line GFA SOC_SAND_FAP21V)         |                |                |                |\r\n"
          "%04u (GFA_MB SOC_SAND_FAP21V)           |                |                |                |\r\n"
          "%04u (line GFA SOC_PETRA DDR3)     |                |                |                |\r\n"
          "%04u (line GFA SOC_PETRA DDR3 Str) |                |                |                |\r\n"
          "%04u (line GFA SOC_PETRA DDR2)     |                |                |                |\r\n"
          "%04u (line GFA B)              |                |                |                |\r\n"
          "%04u (line GFA B Str)          |                |                |                |\r\n"
          "%04u (line GFA B Soc_petra A)      |                |                |                |\r\n"
          "%04u (line GFA B Soc_petra A Str)  |                |                |                |\r\n"
          "%04u (line GFA B Interlaken)   |                |                |                |\r\n"
          "%04u (line GFA B Interlaken_2) |                |                |                |\r\n"
          "%04u (fabric card FE1600)      |                |                |                |\r\n"
          "%04u (fabric card FE1600 BCM88754)      |                |                |                |\r\n"
          "%04u (load board SOC_SAND_FE600)        |                |                |                |\r\n"
          "%04u (load board petra)        |                |                |                |\r\n"
          "%04u (line TEVB)               |      %04u      |      %04u      |      %04u      |",
          SOC_UNKNOWN_CARD,
          STANDALONE_MEZZANINE,
          FABRIC_CARD_01,
          FABRIC_CARD_FE600_01,
          LINE_CARD_01,
          LINE_CARD_FAP10M_B,
          LINE_CARD_GFA_FAP20V,
          LINE_CARD_GFA_MB_FAP20V,
          LINE_CARD_GFA_FAP21V,
          LINE_CARD_GFA_MB_FAP21V,
          LINE_CARD_GFA_PETRA_DDR3,
          LINE_CARD_GFA_PETRA_DDR3_STREAMING,
          LINE_CARD_GFA_PETRA_DDR2,
          LINE_CARD_GFA_PETRA_B_DDR3,
          LINE_CARD_GFA_PETRA_B_DDR3_STREAMING,
          LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3,
          LINE_CARD_GFA_PETRA_B_WITH_PETRA_A_DDR3_STREAMING,
          LINE_CARD_GFA_PETRA_B_INTERLAKEN,
		  LINE_CARD_GFA_PETRA_B_INTERLAKEN_2,
		  FABRIC_CARD_FE1600,
          FABRIC_CARD_FE1600_BCM88754,
          LOAD_BOARD_FE600,
          LOAD_BOARD_PB,
          LINE_CARD_TEVB,
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_BOARD_SN_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                host_ser_ee_block_02.un_host_serial_ee_block_02.
                host_serial_ee_block_02.host_board_serial_number)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                host_ser_ee_block_02.un_host_serial_ee_block_02.
                host_serial_ee_block_02.host_board_serial_number) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 120 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          "  Host board\'s   |                |                |                |\r\n"
          "   serial number |      %04u      |      %04u      |      %04u      |",
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_FRONT_BOARD_TYPE_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                front_ser_ee_block_01.un_front_serial_ee_block_01.
                front_serial_ee_block_01.front_board_type)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                front_ser_ee_block_01.un_front_serial_ee_block_01.
                front_serial_ee_block_01.front_board_type) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 120 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          "  Front board\'s   |                |                |                |\r\n"
          "   type            |                |                |                |\r\n"
          "%02u (unknown/none)|                |                |                |\r\n"
          "%04u (TGS)         |                |                |                |\r\n"
          "%04u (FTG)         |                |                |                |\r\n"
          "%04u (PTG)         |                |                |                |\r\n"
          "%04u (TEVB)        |      %04u      |      %04u      |      %04u      |",
          SOC_UNKNOWN_CARD,
          FRONT_END_TGS,
          FRONT_END_FTG,
          SOC_FRONT_END_PTG,
          FRONT_END_TEVB,
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_FRONT_BOARD_SN_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                front_ser_ee_block_02.un_front_serial_ee_block_02.
                front_serial_ee_block_02.front_board_serial_number)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                front_ser_ee_block_02.un_front_serial_ee_block_02.
                front_serial_ee_block_02.front_board_serial_number) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 120 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          "  Front board\'s  |                |                |                |\r\n"
          "   serial number |      %04u      |      %04u      |      %04u      |",
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_DB_BOARD_TYPE_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                host_db_ser_ee_block_01.un_host_db_serial_ee_block_01.
                host_db_serial_ee_block_01.host_db_board_type)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                host_db_ser_ee_block_01.un_host_db_serial_ee_block_01.
                host_db_serial_ee_block_01.host_db_board_type) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 120 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Host_db board     |                |                |                |\r\n"
          "   type            |                |                |                |\r\n"
          "%02u (unknown/none)|                |                |                |\r\n"
          "%u (MEZ GDDR1)     |                |                |                |\r\n"
          "%u (MEZ DDR2)      |                |                |                |\r\n"
          "%u (MEZ SOC_SAND_FAP21V)    |      %04u      |      %04u      |      %04u      |\r\n",
          SOC_UNKNOWN_CARD,
          FAP_MEZ_GDDR1,
          FAP_MEZ_DDR2,
          FAP_MEZ_FAP21V,
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_DB_BOARD_SN_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
                host_db_serial_ee_block_02.host_db_board_serial_number)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.
                host_db_serial_ee_block_02.host_db_board_serial_number) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 120 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Host_db board   |                |                |                |\r\n"
          "   serial number |      %04u      |      %04u      |      %04u      |",
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_BOARD_FAP_PP_IF_CLK_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                host_ser_ee_block_02.un_host_serial_ee_block_02.
                host_serial_ee_block_02.fap_pp_interface_clk)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                host_ser_ee_block_02.un_host_serial_ee_block_02.
                host_serial_ee_block_02.fap_pp_interface_clk) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 120 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " FAP-PP interface|                |                |                |\r\n"
          " clock rate (MHZ)|      %4u      |      %4u      |      %4u      |",
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_EEPROM_FRONT_CARD_TYPE_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                host_ser_ee_block_02.un_host_serial_ee_block_02.
                host_serial_ee_block_02.eeprom_front_card_type)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                host_ser_ee_block_02.un_host_serial_ee_block_02.
                host_serial_ee_block_02.eeprom_front_card_type) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 120 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          "Front Card Type  |                |                |                |\r\n"
          "%04u (NONE)      |                |                |                |\r\n"
          "%04u (X10)       |                |                |                |\r\n"
          "%04u (IXP2800)   |      %04u      |      %04u      |      %04u      |",
          HOST_EEPROM_FRONT_CARD_TYPE_NONE,
          HOST_EEPROM_FRONT_CARD_TYPE_X10,
          HOST_EEPROM_FRONT_CARD_TYPE_IXP2800,
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_HOST_BOARD_FAP_USE_EEPROM_VALS_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                host_ser_ee_block_02.un_host_serial_ee_block_02.
                host_serial_ee_block_02.fap_use_eeprom_vals)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                host_ser_ee_block_02.un_host_serial_ee_block_02.
                host_serial_ee_block_02.fap_use_eeprom_vals) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 121 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " FAP use EERPOM  |                |                |                |\r\n"
          " vals            |      %4u      |      %4u      |      %4u      |",
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_BOARD_SERIAL_NUM_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                ser_ee_block_02.un_serial_ee_block_02.
                serial_ee_block_02.board_serial_number)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                ser_ee_block_02.un_serial_ee_block_02.
                serial_ee_block_02.board_serial_number) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Board's serial  |                |                |                |\r\n"
          "   number        |      %04u      |      %04u      |      %04u      |",
          *us_ptrs[0],*us_ptrs[1],*us_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_BOARD_HW_VERSION_ID:
    {
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
                ser_ee_block_02.un_serial_ee_block_02.
                serial_ee_block_02.board_version)) +
                SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
                ser_ee_block_02.un_serial_ee_block_02.
                serial_ee_block_02.board_version) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        char
          acsii_board_version[3][5] ;
        sal_sprintf(
          err_msg,
          " Board\'s hardware|                |                |                |\r\n"
          "   version       |     %c%s      |     %c%s      |     %c%s      |",
          'v',version_to_ascii(acsii_board_version[0],*us_ptrs[0],0),
          'v',version_to_ascii(acsii_board_version[1],*us_ptrs[1],0),
          'v',version_to_ascii(acsii_board_version[2],*us_ptrs[2],0)
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_STARTUP_COUNTER_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_01.un_ee_block_01.block_01.startup_counter)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_01.un_ee_block_01.block_01.startup_counter) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Startup counter |   %10lu   |   %10lu   |   %10lu   |",
          *ul_ptrs[0],*ul_ptrs[1],*ul_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_CONSOLE_BAUD_ID:
    {
      unsigned
        long
          console_baud_rate ;
      int
        console_fd,
        baud_rate ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.console_baud_rate)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.console_baud_rate) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        err = get_run_vals((char *)(&console_baud_rate),(int)size,(int)offset) ;
        if (err)
        {
          ret = 40 ;
          goto hanv_exit ;
        }
        baud_rate = console_baud_rate ;
        console_fd = get_console_fd() ;
        (void) ioctl(console_fd,FIOBAUDRATE,baud_rate) ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Console baud    |                |                |                |\r\n"
          "   rate          |     %06lu     |     %06lu     |     %06lu     |",
          *ul_ptrs[0],*ul_ptrs[1],*ul_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_CLK_MIRROR_DATA_ID:
    {
      unsigned
        long
          debug_register_01 ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.debug_register_01)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.debug_register_01) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        err = get_run_vals((char *)(&debug_register_01),(int)size,(int)offset) ;
        if (err)
        {
          ret = 40 ;
          goto hanv_exit ;
        }
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Clk mirror      |                |                |                |\r\n"
          "   data          |   0x%08lx  |   0x%08lx  |   0x%08lx   |",
          *ul_ptrs[0],*ul_ptrs[1],*ul_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }

    case PARAM_NVRAM_ETH_ADDRESS_ID:
    {
      char
        eth_addr[3][MAC_ADRS_LEN] ;
      unsigned
        int
          mac_adrs_len ;
      unsigned
        long
          ls_32_eth_address[3] ;
      for (counter = 0 ; counter < (sizeof(ul_ptrs)/sizeof(ul_ptrs[0])) ; counter++)
      {
        ls_32_eth_address[counter] = *ul_ptrs[counter] & (unsigned long)DUNE_MASK ;
      }
      mac_adrs_len =
        sizeof(((SERIAL_EE_AREA *)0)->ser_ee_block_01.
                        un_serial_ee_block_01.serial_ee_block_01.eth_addr) ;
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
          ser_ee_block_01.un_serial_ee_block_01.
            serial_ee_block_01.eth_addr[mac_adrs_len - sizeof(unsigned long)])) +
              SERIAL_EE_FIRST_OFFSET ;
      size = sizeof(unsigned long) ;
      if (action & CHANGE_NV)
      {
        status = nvRamGet((char *)(&ls_32_eth_address[0]),(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 50 ;
          goto hanv_exit ;
        }
        ls_32_eth_address[0] &= ~((unsigned long)DUNE_MASK) ;
        ls_32_eth_address[0] |= (*ul_ptrs[0] & (unsigned long)DUNE_MASK) ;
        status = nvRamSet((char *)(&ls_32_eth_address[0]),(int)size,(int)offset) ;
        if (status == ERROR)
        {
          ret = 20 ;
          goto hanv_exit ;
        }
      }
      if (action & CHANGE_RUN_TIME)
      {
        err = get_run_vals((char *)(&ls_32_eth_address[2]),(int)size,(int)offset) ;
        if (err)
        {
          ret = 40 ;
          goto hanv_exit ;
        }
        ls_32_eth_address[2] &= ~((unsigned long)DUNE_MASK) ;
        ls_32_eth_address[2] |= (*ul_ptrs[2] & (unsigned long)DUNE_MASK) ;
        err = set_run_vals((char *)(&ls_32_eth_address[2]),(int)size,(int)offset) ;
        if (err)
        {
          ret = 30 ;
          goto hanv_exit ;
        }
      }
      for (counter = 0 ; counter < (sizeof(ul_ptrs)/sizeof(ul_ptrs[0])) ; counter++)
      {
        *ul_ptrs[counter] = ls_32_eth_address[counter] & (unsigned long)DUNE_MASK ;
      }
      for (counter = 0 ; counter < (sizeof(eth_addr)/sizeof(eth_addr[0])) ; counter++)
      {
        uc_ptrs[counter] = &eth_addr[counter][0] ;
      }
      offset =
        (unsigned int)((char *)&(((SERIAL_EE_AREA *)0)->
          ser_ee_block_01.un_serial_ee_block_01.
            serial_ee_block_01.eth_addr)) +
              SERIAL_EE_FIRST_OFFSET ;
      size =
        sizeof(((SERIAL_EE_AREA *)0)->
          ser_ee_block_01.un_serial_ee_block_01.
            serial_ee_block_01.eth_addr) ;
      err = get_nv_vals((void **)uc_ptrs,size,(int)offset) ;
      if (err)
      {
        ret = 10 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Ethernet address|0x%02X%02X%02X %02X%02X%02X |"
                            "0x%02X%02X%02X %02X%02X%02X |0x%02X%02X%02X %02X%02X%02X |\r\n",
          eth_addr[0][0],eth_addr[0][1],eth_addr[0][2],eth_addr[0][3],eth_addr[0][4],eth_addr[0][5],
          eth_addr[1][0],eth_addr[1][1],eth_addr[1][2],eth_addr[1][3],eth_addr[1][4],eth_addr[1][5],
          eth_addr[2][0],eth_addr[2][1],eth_addr[2][2],eth_addr[2][3],eth_addr[2][4],eth_addr[2][5]
          ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_DATA_CACHE_ID:
    {
      unsigned
        char
          activate_data_cache ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.activate_data_cache)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.activate_data_cache) ;
      if (action & CHANGE_RUN_TIME)
      {
        err = get_run_vals((char *)(&activate_data_cache),(int)size,(int)offset) ;
        if (err)
        {
          ret = 40 ;
          goto hanv_exit ;
        }
      }
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        if (activate_data_cache)
        {
          /*
           * Disable data cache
           */
          cacheDisable (DATA_CACHE) ;
        }
        else
        {
          /*
           * Enable data cache
           */
          cacheEnable (DATA_CACHE) ;
        }
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Activate data   |                |                |                |\r\n"
          "   cache (0=OFF) |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_INST_CACHE_ID:
    {
      unsigned
        char
          activate_inst_cache ;
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.activate_inst_cache)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.activate_inst_cache) ;
      if (action & CHANGE_RUN_TIME)
      {
        err = get_run_vals((char *)(&activate_inst_cache),(int)size,(int)offset) ;
        if (err)
        {
          ret = 40 ;
          goto hanv_exit ;
        }
      }
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        if (activate_inst_cache)
        {
          /*
           * Disable instruction cache
           */
          cacheDisable (INSTRUCTION_CACHE) ;
        }
        else
        {
          /*
           * Enable instruction cache
           */
          cacheEnable (INSTRUCTION_CACHE) ;
        }
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Activate inst.  |                |                |                |\r\n"
          "   cache (0=OFF) |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_TCP_TIMEOUT_ID:
    {
      unsigned
        char
          tcp_timeout ;
      tcp_timeout = 0;

      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.tcp_timeout)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.tcp_timeout) ;
      if (action & CHANGE_RUN_TIME)
      {
        tcp_timeout = *uc_ptrs[2] ;
      }
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * Update tcp timeout runtime value (and related internal TCP
         * module variables).
         */
        set_run_tcp_timeout(tcp_timeout) ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " TCP timeout     |                |                |                |\r\n"
          "   (in seconds)  |      %03u       |      %03u       |      %03u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_TCP_RETRIES_ID:
    {
      unsigned
        char
          tcp_retries ;
      tcp_retries = 0;

      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.tcp_retries)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.tcp_retries) ;
      if (action & CHANGE_RUN_TIME)
      {
        tcp_retries = *uc_ptrs[2] ;
      }
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (changed)
      {
        /*
         * Update tcp retries runtime value (and related internal TCP
         * module variables).
         */
        set_run_tcp_retries(tcp_retries) ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " TCP retries     |                |                |                |\r\n"
          "   (in seconds)  |      %03u       |      %03u       |      %03u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_AUTO_FLASH_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.auto_flash_load)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.auto_flash_load) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Automatic Flash |                |                |                |\r\n"
          "  loading (0=OFF)|       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_IP_SOURCE_MODE_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                          ee_block_02.un_ee_block_02.block_02.ip_source_mode)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.ip_source_mode) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Ip source mode: |                |                |                |\r\n"
          "0=IP_FROM_NV     |                |                |                |\r\n"
          "1=IP_FROM_SLOT_ID|                |                |                |\r\n"
          "2=IP_FROM_CRATE  |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_KERNEL_STARTUP_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.kernel_startup)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.kernel_startup) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Run shell, not  |                |                |                |\r\n"
          "  application    |                |                |                |\r\n"
          "0=appl.,1=shell, |                |                |                |\r\n"
          "2 or 3=shell once|       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_TEST_AT_STARTUP_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.test_at_startup)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.test_at_startup) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Bit map: Tests  |                |                |                |\r\n"
          " at startup (BIT0|                |                |                |\r\n"
          " =IRQ6, B1=NMI)  |      0x%02X      |      0x%02X      |      0x%02X      |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_APP_SOURCE_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.app_source)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.app_source) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " Src of applic.  |                |                |                |\r\n"
          "   file (0=Host, |                |                |                |\r\n"
          "   other=Flash)  |       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_LOAD_METHOD_ID:
    {
      offset = (unsigned int)((char *)&(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.app_load_method)) ;
      size = sizeof(((EE_AREA *)0)->
                    ee_block_02.un_ee_block_02.block_02.app_load_method) ;
      err = handle_nv_val(val_ptrs,size,offset,action,&changed) ;
      if (err)
      {
        ret = 20 + err ;
        goto hanv_exit ;
      }
      if (action & PRINT_ITEMS)
      {
        sal_sprintf(
          err_msg,
          " App. load method|                |                |                |\r\n"
          "  0=Newest,      |                |                |                |\r\n"
          "  1=File..newest |                |                |                |\r\n"
          "  2=Flash..newest|       %02u       |       %02u       |       %02u       |",
          *uc_ptrs[0],*uc_ptrs[1],*uc_ptrs[2]) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      break ;
    }
    default:
    {
      sal_sprintf(err_msg,
        "\r\n\n"
        "*** Software error in \'print_nv_item\': Unknown param_id (=%d).\r\n",
        param_id) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = 1 ;
      break ;
    }
  }
hanv_exit:
exit:
  if(err_msg)
  {
  	sal_free(err_msg);
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  print_nv_header
*TYPE: PROC
*DATE: 26/MAR/2002
*FUNCTION:
*  Print header for display of nvram, default and
*  runtime values.
*CALLING SEQUENCE:
*  print_nv_header()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Table conventions.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Printed header.
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  print_nv_header(
    void
  )
{
  send_string_to_screen(
    "  Description    |     NVRAM      |    Default     |    Runtime     |\r\n"
    "                 |     value      |     value      |     value      |\r\n"
    "=====================================================================",
    TRUE
    ) ;
  return ;
}
/*****************************************************
*NAME
*  update_nvram_param
*TYPE: PROC
*DATE: 26/MAR/2002
*FUNCTION:
*  Update NVRAM or runtime parameter and print indication
*  for this update.
*CALLING SEQUENCE:
*  update_nvram_param(update,param_id,param_val)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int update -
*      Indication on what to update:
*        RUN_TIME_EQUIVALENT -
*          Update run time image only.
*        NEXT_STARTUP_EQUIVALENT -
*          Update NVRAM so that next startup it will go
*          into effect
*        BOTH_EQUIVALENT -
*          Update both runtime image and NVRAM (so
*          that change will go into effect immediately
*          and will also be remembered after next
*          reset).
*    unsigned int param_id -
*      Identifier of parameter to handle:
*        PARAM_NVRAM_DATA_CACHE_ID -
*          Data cache enable/disable.
*    unsigned int param_val -
*      Value to store in selected parameter (or an
*      indication on what to store, depending on each
*      specific parameter). For 'text' variables,
*      this is the address of the string containing
*      the text (so casting of (char *) is requied).
*  SOC_SAND_INDIRECT:
*    EEPROM definitions.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  It is possible that BOTH_EQUIVALENT is required but
*  only NVRAM value can be changed. In this case, action
*  will be carried out for NVRAM only.
*SEE ALSO:
 */
int
  update_nvram_param(
    unsigned int update,
    unsigned int param_id,
    unsigned int param_val
  )
{
  int
    err,
    ret ;
  unsigned
    int
      changed,
      action,
      counter;
  unsigned
    short
      usb[3],
      usa[3] ;
  unsigned
    short
      *usb_ptrs[3],
      *usa_ptrs[3] ;
  unsigned
    long
      ulb[3],
      ula[3] ;
  unsigned
    long
      *ulb_ptrs[3],
      *ula_ptrs[3] ;
  unsigned
    char
      ucb[3],
      uca[3] ;
  unsigned
    char
      *ucb_ptrs[3],
      *uca_ptrs[3] ;
  char
    err_msg[80*4],
    after[80*3],
    conclude[80],
    before[80*2] ;
  ret = 0 ;
  for (counter = 0 ; counter < (sizeof(uca)/sizeof(uca[0])) ; counter++)
  {
    uca_ptrs[counter] = &uca[counter] ;
    ucb_ptrs[counter] = &ucb[counter] ;
  }
  for (counter = 0 ; counter < (sizeof(usa)/sizeof(usa[0])) ; counter++)
  {
    usa_ptrs[counter] = &usa[counter] ;
    usb_ptrs[counter] = &usb[counter] ;
  }
  for (counter = 0 ; counter < (sizeof(ula)/sizeof(ula[0])) ; counter++)
  {
    ula_ptrs[counter] = &ula[counter] ;
    ulb_ptrs[counter] = &ulb[counter] ;
  }
  sal_sprintf(
    before,
    "= Values BEFORE =|                |                |                |\r\n"
    "=================|================|================|================|"
    ) ;
  sal_sprintf(
    after,
    "=================|================|================|================|\r\n"
    "= Values AFTER ==|                |                |                |\r\n"
    "=================|================|================|================|"
    ) ;
  sal_sprintf(
    conclude,
    "=================|================|================|================|"
    ) ;
  send_array_to_screen("\r\n\n",3) ;
  switch (param_id)
  {
    case PARAM_SNMP_SET_CMD_LINE_ID:
      {
        unsigned
          char
            text_a[3][SNMP_CMD_LINE_SIZE + 1],
            text_b[3][SNMP_CMD_LINE_SIZE + 1] ;
        for (counter = 0 ; counter < (sizeof(text_b)/sizeof(text_b[0])) ; counter++)
        {
          uca_ptrs[counter] = &text_a[counter][0] ;
          ucb_ptrs[counter] = &text_b[counter][0] ;
        }
        print_nv_header() ;
        send_string_to_screen(before,TRUE) ;
        action = PRINT_ITEMS ;
        err = handle_nv_item((void **)ucb_ptrs,param_id,action) ;
        if (err)
        {
          ret = 15 + err ;
          goto unvp_exit ;
        }
        if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_RUN_TIME ;
          strncpy(uca_ptrs[2],(char *)param_val,sizeof(text_a[2])) ;
          text_a[2][sizeof(text_a[2]) - 1] = 0 ;
        }
        if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_NV ;
          strncpy(uca_ptrs[0],(char *)param_val,sizeof(text_a[0])) ;
          text_a[0][sizeof(text_a[0]) - 1] = 0 ;
        }
        send_string_to_screen(after,TRUE) ;
        err = handle_nv_item((void **)uca_ptrs,param_id,action) ;
        if (err)
        {
          ret = 16 + err ;
          goto unvp_exit ;
        }
        send_string_to_screen(conclude,TRUE) ;
        changed = FALSE ;
        for (counter = 0 ; counter < (sizeof(text_b)/sizeof(text_b[0])) ; counter++)
        {
          if (strcmp(text_a[counter],text_b[counter]) != 0)
          {
            changed = TRUE ;
            break ;
          }
        }
        if (action & CHANGE_RUN_TIME)
        {
         /*
          * if something changed at real time, this would be the place to catch
          * it, but since net-snmp is not build for restarting, witout taking down
          * the whole system (lots of globals, lots of calls to exit, etc), we don't
          * support this feature.
          */
        }
        if (!changed)
        {
          send_string_to_screen(
             "\r\n    NO value has been changed!!",TRUE) ;
        }
        break ;
      }
    case PARAM_NVRAM_BOARD_DESCRIPTION_ID:
    case PARAM_NVRAM_BOARD_TYPE_ID:
    case PARAM_NVRAM_DLD_FILE_NAME_ID:
    case PARAM_NVRAM_HOST_BOARD_DESC_ID:
    case PARAM_NVRAM_HOST_BOARD_CAT_NUM_ID:
    case PARAM_NVRAM_HOST_BOARD_VER_ID:
    case PARAM_NVRAM_FRONT_BOARD_DESC_ID:
    case PARAM_NVRAM_FRONT_BOARD_CAT_NUM_ID:
    case PARAM_NVRAM_FRONT_BOARD_VER_ID:
    case PARAM_NVRAM_HOST_DB_BOARD_DESC_ID:
    case PARAM_NVRAM_HOST_DB_BOARD_CAT_NUM_ID:
    case PARAM_NVRAM_HOST_DB_BOARD_VER_ID:
    {
      unsigned
        char
          text_a[3][MAX_SIZE_OF_TEXT_VAR + 1],
          text_b[3][MAX_SIZE_OF_TEXT_VAR + 1] ;
      for (counter = 0 ; counter < (sizeof(text_b)/sizeof(text_b[0])) ; counter++)
      {
        uca_ptrs[counter] = &text_a[counter][0] ;
        ucb_ptrs[counter] = &text_b[counter][0] ;
      }
      print_nv_header() ;
      send_string_to_screen(before,TRUE) ;
      action = PRINT_ITEMS ;
      err = handle_nv_item((void **)ucb_ptrs,param_id,action) ;
      if (err)
      {
        ret = 10 + err ;
        goto unvp_exit ;
      }
      if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
      {
        action |= CHANGE_RUN_TIME ;
        strncpy(uca_ptrs[2],(char *)param_val,sizeof(text_a[2])) ;
        text_a[2][sizeof(text_a[2]) - 1] = 0 ;
      }
      if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
      {
        action |= CHANGE_NV ;
        strncpy(uca_ptrs[0],(char *)param_val,sizeof(text_a[0])) ;
        text_a[0][sizeof(text_a[0]) - 1] = 0 ;
      }
      send_string_to_screen(after,TRUE) ;
      err = handle_nv_item((void **)uca_ptrs,param_id,action) ;
      if (err)
      {
        ret = 11 + err ;
        goto unvp_exit ;
      }
      send_string_to_screen(conclude,TRUE) ;
      changed = FALSE ;
      for (counter = 0 ; counter < (sizeof(text_b)/sizeof(text_b[0])) ; counter++)
      {
        if (strcmp(text_a[counter],text_b[counter]) != 0)
        {
          changed = TRUE ;
          break ;
        }
      }
      if (!changed)
      {
        send_string_to_screen(
           "\r\n    NO value has been changed!!",TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_LOC_IP_MASK_ID:
    case PARAM_NVRAM_DLD_HOST_ADDR_ID:
    case PARAM_NVRAM_GATEWAY_ADDR_ID:
    case PARAM_NVRAM_LOC_IP_ADDR_ID:
    case PARAM_NVRAM_CONSOLE_BAUD_ID:
    case PARAM_NVRAM_CLK_MIRROR_DATA_ID:
    case PARAM_NVRAM_BASE_REGISTER_04_ID:
    case PARAM_NVRAM_BASE_REGISTER_05_ID:
    case PARAM_NVRAM_OPTION_REGISTER_04_ID:
    case PARAM_NVRAM_OPTION_REGISTER_05_ID:

    {
      print_nv_header() ;
      send_string_to_screen(before,TRUE) ;
      action = PRINT_ITEMS ;
      err = handle_nv_item((void **)ulb_ptrs,param_id,action) ;
      if (err)
      {
        ret = 10 + err ;
        goto unvp_exit ;
      }
      if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
      {
        action |= CHANGE_RUN_TIME ;
        ula[2] = (unsigned long)param_val ;
      }
      if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
      {
        action |= CHANGE_NV ;
        ula[0] = (unsigned long)param_val ;
      }
      send_string_to_screen(after,TRUE) ;
      err = handle_nv_item((void **)ula_ptrs,param_id,action) ;
      if (err)
      {
        ret = 11 + err ;
        goto unvp_exit ;
      }
      send_string_to_screen(conclude,TRUE) ;
      changed = FALSE ;
      for (counter = 0 ; counter < (sizeof(ula)/sizeof(ula[0])) ; counter++)
      {
        if (ula[counter] != ulb[counter])
        {
          changed = TRUE ;
          break ;
        }
      }
      if (!changed)
      {
        send_string_to_screen(
           "\r\n    NO value has been changed!!",TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_BOARD_HW_VERSION_ID:
    case PARAM_NVRAM_BOARD_SERIAL_NUM_ID:
    case PARAM_NVRAM_HOST_BOARD_SN_ID:
    case PARAM_NVRAM_FRONT_BOARD_SN_ID:
    case PARAM_NVRAM_HOST_DB_BOARD_SN_ID:
    case PARAM_NVRAM_HOST_BOARD_FAP_PP_IF_CLK_ID:
    case PARAM_NVRAM_EEPROM_FRONT_CARD_TYPE_ID:
    case PARAM_NVRAM_EEPROM_HOST_DB_CARD_TYPE_ID:
    case PARAM_NVRAM_HOST_BOARD_FAP_USE_EEPROM_VALS_ID:
    case PARAM_NVRAM_HOST_BOARD_TYPE_ID:
    case PARAM_NVRAM_FRONT_BOARD_TYPE_ID:
    case PARAM_NVRAM_HOST_DB_BOARD_TYPE_ID:
    case PARAM_NVRAM_FE_CONFIGURATION_ID:
    {
      print_nv_header() ;
      send_string_to_screen(before,TRUE) ;
      action = PRINT_ITEMS ;
      err = handle_nv_item((void **)usb_ptrs,param_id,action) ;
      if (err)
      {
        ret = 10 + err ;
        goto unvp_exit ;
      }
      if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
      {
        action |= CHANGE_RUN_TIME ;
        usa[2] = (unsigned short)param_val ;
      }
      if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
      {
        action |= CHANGE_NV ;
        usa[0] = (unsigned short)param_val ;
      }
      send_string_to_screen(after,TRUE) ;
      err = handle_nv_item((void **)usa_ptrs,param_id,action) ;
      if (err)
      {
        ret = 11 + err ;
        goto unvp_exit ;
      }
      send_string_to_screen(conclude,TRUE) ;
      changed = FALSE ;
      for (counter = 0 ; counter < (sizeof(usa)/sizeof(usa[0])) ; counter++)
      {
        if (usa[counter] != usb[counter])
        {
          changed = TRUE ;
          break ;
        }
      }
      if (!changed)
      {
        send_string_to_screen(
           "\r\n    NO value has been changed!!",TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_TCP_TIMEOUT_ID:
    case PARAM_NVRAM_TCP_RETRIES_ID:
    case PARAM_NVRAM_EVENT_DISPLAY_LVL_ID:
    case PARAM_NVRAM_EVENT_NVRAM_LVL_ID:
    case PARAM_NVRAM_EVENT_SYSLOG_LVL_ID:
    case PARAM_NVRAM_TELNET_TIMEOUT_CONT_ID:
    case PARAM_NVRAM_TELNET_TIMEOUT_STD_ID:
    case PARAM_NVRAM_WATCHDOG_PERIOD_ID:
    case PARAM_NVRAM_PAGE_LINES_ID:
    case PARAM_NVRAM_TEST_AT_STARTUP_ID:
    case PARAM_NVRAM_DPSS_DISPLAY_LEVEL_ID:
    case PARAM_NVRAM_DPSS_DEBUG_LEVEL_ID:
    case PARAM_NVRAM_STARTUP_DEMO_MODE_ID:
    case PARAM_NVRAM_FAP10M_RUN_MODE_ID:
    case PARAM_NVRAM_USR_APP_FLAVOR_ID:
    {
      print_nv_header() ;
      send_string_to_screen(before,TRUE) ;
      action = PRINT_ITEMS ;
      err = handle_nv_item((void **)ucb_ptrs,param_id,action) ;
      if (err)
      {
        ret = 10 + err ;
        goto unvp_exit ;
      }
      if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
      {
        action |= CHANGE_RUN_TIME ;
        uca[2] = (unsigned char)param_val ;
      }
      if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
      {
        action |= CHANGE_NV ;
        uca[0] = (unsigned char)param_val ;
      }
      send_string_to_screen(after,TRUE) ;
      err = handle_nv_item((void **)uca_ptrs,param_id,action) ;
      if (err)
      {
        ret = 11 + err ;
        goto unvp_exit ;
      }
      send_string_to_screen(conclude,TRUE) ;
      changed = FALSE ;
      for (counter = 0 ; counter < (sizeof(uca)/sizeof(uca[0])) ; counter++)
      {
        if (uca[counter] != ucb[counter])
        {
          changed = TRUE ;
          break ;
        }
      }
      if (!changed)
      {
        send_string_to_screen(
           "\r\n    NO value has been changed!!",TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_RUN_UI_STARTUP_SCRIPT_ID:
    case PARAM_NVRAM_DPSS_LOAD_FROM_FLASH_NOT_TFTP_ID:
    case PARAM_NVRAM_ACTIVATE_DPSS_ID:
    case PARAM_NVRAM_AUTO_FLASH_ID:
    case PARAM_NVRAM_INST_CACHE_ID:
    case PARAM_NVRAM_DATA_CACHE_ID:
    case PARAM_NVRAM_PAGE_MODE_ID:
    case PARAM_NVRAM_LINE_MODE_ID:
    case PARAM_NVRAM_ACTIVATE_WATCHDOG_ID:
    case PARAM_NVRAM_CONF_FROM_MD_ID:
    case PARAM_NVRAM_HOOK_SOFTWARE_EXCEPTION_EXC_ID:
    case PARAM_NVRAM_BCKG_TEMPERATURES_EN_ID:
    case PARAM_SNMP_DOWNLOAD_MIBS_ID:
    case PARAM_SNMP_INCLUDE_NETSNMP_ID:
    {
      if (param_id == PARAM_NVRAM_ACTIVATE_DPSS_ID)
      {
        send_string_to_screen("Deprecated feature. Use 'usr_app_flavor'\n\r",TRUE);
      }
      print_nv_header() ;
      send_string_to_screen(before,TRUE) ;
      action = PRINT_ITEMS ;
      err = handle_nv_item((void **)ucb_ptrs,param_id,action) ;
      if (err)
      {
        ret = 10 + err ;
        goto unvp_exit ;
      }
      if (param_val == OFF_EQUIVALENT)
      {
        if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_RUN_TIME ;
          uca[2] = 0 ;
        }
        if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_NV ;
          uca[0] = 0 ;
        }
      }
      else
      {
        if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_RUN_TIME ;
          uca[2] = 1 ;
        }
        if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_NV ;
          uca[0] = 1 ;
        }
      }
      send_string_to_screen(after,TRUE) ;
      err = handle_nv_item((void **)uca_ptrs,param_id,action) ;
      if (err)
      {
        ret = 11 + err ;
        goto unvp_exit ;
      }
      send_string_to_screen(conclude,TRUE) ;
      changed = FALSE ;
      for (counter = 0 ; counter < (sizeof(uca)/sizeof(uca[0])) ; counter++)
      {
        if (uca[counter] != ucb[counter])
        {
          changed = TRUE ;
          break ;
        }
      }
      if (!changed)
      {
        send_string_to_screen(
           "\r\n    NO value has been changed!!",TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_IP_SOURCE_MODE_ID:
    {
      print_nv_header() ;
      send_string_to_screen(before,TRUE) ;
      action = PRINT_ITEMS ;
      err = handle_nv_item((void **)ucb_ptrs,param_id,action) ;
      if (err)
      {
        ret = 10 + err ;
        goto unvp_exit ;
      }

      if ((param_val == IP_FROM_NV) ||
          (param_val == IP_FROM_SLOT_ID) ||
          (param_val == IP_FROM_CRATE))
      {
        if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_RUN_TIME ;
          uca[2] = param_val ;
        }
        if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_NV ;
          uca[0] = param_val ;
        }
      }
      else
      {
        /*
         * Unknown input parameter (param_val). Probably SW error.
         */
        sal_sprintf(err_msg,
           "\r\n\n"
           "*** Unknown \'param_val\' (=%d) in \'update_nvram_param\'. Probably SW error.",
           param_val) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = 20 ;
        goto unvp_exit ;
      }
      send_string_to_screen(after,TRUE) ;
      err = handle_nv_item((void **)uca_ptrs,param_id,action) ;
      if (err)
      {
        ret = 11 + err ;
        goto unvp_exit ;
      }
      send_string_to_screen(conclude,TRUE) ;
      changed = FALSE ;
      for (counter = 0 ; counter < (sizeof(uca)/sizeof(uca[0])) ; counter++)
      {
        if (uca[counter] != ucb[counter])
        {
          changed = TRUE ;
          break ;
        }
      }
      if (!changed)
      {
        send_string_to_screen(
           "\r\n    NO value has been changed!!",TRUE) ;
      }
      break ;

    }
    case PARAM_NVRAM_KERNEL_STARTUP_ID:
    {
      print_nv_header() ;
      send_string_to_screen(before,TRUE) ;
      action = PRINT_ITEMS ;
      err = handle_nv_item((void **)ucb_ptrs,param_id,action) ;
      if (err)
      {
        ret = 10 + err ;
        goto unvp_exit ;
      }
      if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
      {
        /*
         * This parameter may only be changed in NVRAM. No point in changing
         * runtime value.
         */
        sal_sprintf(err_msg,
           "\r\n\n"
           " This parameter (shell startup method) may only be changed in NVRAM\r\n"
           " No point in changing runtime value.\r\n") ;
        send_string_to_screen(err_msg,TRUE) ;
        goto unvp_exit ;
      }
      if (param_val & (~(SHELL_ONCE | SHELL_THEN_APPLICATION)))
      {
        /*
         * Unknown input parameter (param_val). Probably SW error.
         */
        sal_sprintf(err_msg,
           "\r\n\n"
           "*** Unknown \'param_val\' (=%d) in \'update_nvram_param\'. Probably SW error.",
           param_val) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = 20 ;
        goto unvp_exit ;
      }
      else
      {
        /*
         * Next 'if' redundant, but safe.
         */
        if (update == NEXT_STARTUP_EQUIVALENT)
        {
          action |= CHANGE_NV ;
          uca[0] = (unsigned char)param_val ;
        }
      }
      send_string_to_screen(after,TRUE) ;
      err = handle_nv_item((void **)uca_ptrs,param_id,action) ;
      if (err)
      {
        ret = 11 + err ;
        goto unvp_exit ;
      }
      send_string_to_screen(conclude,TRUE) ;
      changed = FALSE ;
      for (counter = 0 ; counter < (sizeof(uca)/sizeof(uca[0])) ; counter++)
      {
        if (uca[counter] != ucb[counter])
        {
          changed = TRUE ;
          break ;
        }
      }
      if (!changed)
      {
        send_string_to_screen(
           "\r\n    NO value has been changed!!",TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_LOAD_METHOD_ID:
    {
      print_nv_header() ;
      send_string_to_screen(before,TRUE) ;
      action = PRINT_ITEMS ;
      err = handle_nv_item((void **)ucb_ptrs,param_id,action) ;
      if (err)
      {
        ret = 10 + err ;
        goto unvp_exit ;
      }
      if (param_val == METHOD_NEWEST_VERSION)
      {
        if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_RUN_TIME ;
          uca[2] = METHOD_NEWEST_VERSION ;
        }
        if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_NV ;
          uca[0] = METHOD_NEWEST_VERSION ;
        }
      }
      else if (param_val == METHOD_FILE_THEN_NEWEST)
      {
        if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_RUN_TIME ;
          uca[2] = METHOD_FILE_THEN_NEWEST ;
        }
        if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_NV ;
          uca[0] = METHOD_FILE_THEN_NEWEST ;
        }
      }
      else if (param_val == METHOD_FLASH_THEN_NEWEST)
      {
        if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_RUN_TIME ;
          uca[2] = METHOD_FLASH_THEN_NEWEST ;
        }
        if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_NV ;
          uca[0] = METHOD_FLASH_THEN_NEWEST ;
        }
      }
      else
      {
        /*
         * Unknown input parameter (param_val). Probably SW error.
         */
        sal_sprintf(err_msg,
           "\r\n\n"
           "*** Unknown \'param_val\' (=%d) in \'update_nvram_param\'. Probably SW error.",
           param_val) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = 20 ;
        goto unvp_exit ;
      }
      send_string_to_screen(after,TRUE) ;
      err = handle_nv_item((void **)uca_ptrs,param_id,action) ;
      if (err)
      {
        ret = 11 + err ;
        goto unvp_exit ;
      }
      send_string_to_screen(conclude,TRUE) ;
      changed = FALSE ;
      for (counter = 0 ; counter < (sizeof(uca)/sizeof(uca[0])) ; counter++)
      {
        if (uca[counter] != ucb[counter])
        {
          changed = TRUE ;
          break ;
        }
      }
      if (!changed)
      {
        send_string_to_screen(
           "\r\n    NO value has been changed!!",TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_APP_SOURCE_ID:
    {
      print_nv_header() ;
      send_string_to_screen(before,TRUE) ;
      action = PRINT_ITEMS ;
      err = handle_nv_item((void **)ucb_ptrs,param_id,action) ;
      if (err)
      {
        ret = 10 + err ;
        goto unvp_exit ;
      }
      if (param_val == HOST_SOURCE_EQUIVALENT)
      {
        if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_RUN_TIME ;
          uca[2] = HOST_SOURCE_EQUIVALENT ;
        }
        if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_NV ;
          uca[0] = HOST_SOURCE_EQUIVALENT ;
        }
      }
      else if ((param_val <= get_num_flash_files()) && (param_val >= 1))
      {
        if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_RUN_TIME ;
          uca[2] = (unsigned char)param_val ;
        }
        if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_NV ;
          uca[0] = (unsigned char)param_val ;
        }
      }
      else
      {
        /*
         * Unknown input parameter (param_val). Probably SW error.
         */
        sal_sprintf(err_msg,
           "\r\n\n"
           "*** Unknown \'param_val\' (=%d) in \'update_nvram_param\'. Probably SW error.",
            param_val) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = 20 ;
        goto unvp_exit ;
      }
      send_string_to_screen(after,TRUE) ;
      err = handle_nv_item((void **)uca_ptrs,param_id,action) ;
      if (err)
      {
        ret = 11 + err ;
        goto unvp_exit ;
      }
      send_string_to_screen(conclude,TRUE) ;
      changed = FALSE ;
      for (counter = 0 ; counter < (sizeof(uca)/sizeof(uca[0])) ; counter++)
      {
        if (uca[counter] != ucb[counter])
        {
          changed = TRUE ;
          break ;
        }
      }
      if (!changed)
      {
        send_string_to_screen(
           "\r\n    NO value has been changed!!",TRUE) ;
      }
      break ;
    }
    case PARAM_NVRAM_ETH_ADDRESS_ID:
    {
      unsigned
        long
          bot_eth_addr ;
      print_nv_header() ;
      send_string_to_screen(before,TRUE) ;
      action = PRINT_ITEMS ;
      err = handle_nv_item((void **)ulb_ptrs,param_id,action) ;
      if (err)
      {
        ret = 10 + err ;
        goto unvp_exit ;
      }
      bot_eth_addr = BOTTOM_ETH_ADDRESS ;
      if ((param_val <= (unsigned int)DUNE_MASK) && ((int)param_val >= 0))
      {
        if ((update == RUN_TIME_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_RUN_TIME ;
          ula[2] = (unsigned long)param_val | bot_eth_addr ;
        }
        if ((update == NEXT_STARTUP_EQUIVALENT) || (update == BOTH_EQUIVALENT))
        {
          action |= CHANGE_NV ;
          ula[0] = (unsigned long)param_val | bot_eth_addr ;
        }
      }
      else
      {
        /*
         * Unknown input parameter (param_val). Probably SW error.
         */
        sal_sprintf(err_msg,
           "\r\n\n"
           "*** Unacceptable \'param_val\' (=0x%08lX) in \'update_nvram_param\'. Probably SW error.",
           (unsigned long)param_val) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = 20 ;
        goto unvp_exit ;
      }
      send_string_to_screen(after,TRUE) ;
      err = handle_nv_item((void **)ula_ptrs,param_id,action) ;
      if (err)
      {
        ret = 11 + err ;
        goto unvp_exit ;
      }
      send_string_to_screen(conclude,TRUE) ;
      changed = FALSE ;
      for (counter = 0 ; counter < (sizeof(ula)/sizeof(ula[0])) ; counter++)
      {
        if (ula[counter] != ulb[counter])
        {
          changed = TRUE ;
          break ;
        }
      }
      if (!changed)
      {
        send_string_to_screen(
           "\r\n    NO value has been changed!!",TRUE) ;
      }
      break ;
    }
    default:
    {
      sal_sprintf(err_msg,
        "\r\n\n"
        "*** Software error in \'update_nvram_param\': Unknown param_id (=%d).\r\n",
        param_id) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = 1 ;
      break ;
    }
  }
unvp_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  print_nvram_info
*TYPE: PROC
*DATE: 24/MAR/2002
*FUNCTION:
*  Print information on part or all nvram (EEPROM)
*  blocks available via 'nvram' command.
*CALLING SEQUENCE:
*  print_nvram_info(block)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int block -
*      Identifier of the block to get info about
*      (A value of '0' stands for 'all blocks'.
*       Otherwise, this is the identifier of
*       the specific block to display).
*  SOC_SAND_INDIRECT:
*    EEPROM definitions.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  print_nvram_info(
    unsigned int block
  )
{
  int
    err,
    ret ;
  char
    *proc_name,
    nvram_info[80*10] ;
  unsigned
    int
      offset;
  unsigned
    short
      counter ;
  EE_AREA
    *ee_area ;
  SERIAL_EE_AREA
    *serial_ee_area ;
  unsigned
    char
      uc[3] ;
  unsigned
    char
      *uc_ptrs[3] ;
  unsigned
    long
      ul[3] ;
  unsigned
    short
      us[3] ;
  unsigned
    long
      *ul_ptrs[3] ;
  unsigned
    short
      *us_ptrs[3] ;
  ret = 0 ;
  ee_area = (EE_AREA *)0 ;
  serial_ee_area = (SERIAL_EE_AREA *)0 ;
  proc_name = "print_nvram_info" ;
  send_array_to_screen("\r\n\n",3) ;
  for (counter = 0 ; counter < (sizeof(ul)/sizeof(ul[0])) ; counter++)
  {
    ul_ptrs[counter] = &ul[counter] ;
  }
  for (counter = 0 ; counter < (sizeof(us)/sizeof(us[0])) ; counter++)
  {
    us_ptrs[counter] = &us[counter] ;
  }
  for (counter = 0 ; counter < (sizeof(uc)/sizeof(uc[0])) ; counter++)
  {
    uc_ptrs[counter] = &uc[counter] ;
  }
  switch (block)
  {
    case 0:
    {
      unsigned short card_type;
      sal_sprintf(
          &nvram_info[0],
          "CPU Parallel EEPROM\r\n"
          "===================\r\n"
          "  EEPROM base address : 0x%08lX.\r\n"
          "  Size (in bytes)     : %lu.\r\n"
          "  No. of blocks       : %u.\r\n"
          "  First %u blocks are used at boot time.\r\n\n"
          "CPU Serial EEPROM\r\n"
          "=================\r\n"
          "  I2C device address  : 0x%02X.\r\n"
          "  Size (in bytes)     : %lu.\r\n"
          "  No. of blocks       : %u.\r\n\n",
          (unsigned long)NV_RAM_ADRS,
          (unsigned long)NV_RAM_SIZE,
          (unsigned short)NUM_EE_BLOCKS,
          (unsigned short)NUM_B_EE_BLOCKS,
          (unsigned char)SER_EPROM_I2C_ADDRESS_WRITE,
          (unsigned long)SIZEOF_SERIAL_EE,
          (unsigned short)NUM_SERIAL_EE_BLOCKS
          ) ;
      send_string_to_screen(nvram_info,FALSE) ;
      host_board_type_from_nv(&card_type);
      if ( get_slot_id() )
      {
        if ( !get_host_ser_ee_err_boot_flag() )
        {
          /* we have a host board, so we can present its serial eeprom here as well */
          sal_sprintf(
              &nvram_info[0],
              "Host Serial EEPROM\r\n"
              "==================\r\n"
              "  I2C device address  : 0x%02X.\r\n"
              "  Size (in bytes)     : %lu.\r\n"
              "  No. of blocks       : %u.\r\n\n",
              (unsigned char)HOST_SER_EPROM_I2C_ADDRESS_WRITE,
              (unsigned long)SIZEOF_SERIAL_EE,
              (unsigned short)HOST_NUM_SERIAL_EE_BLOCKS
              ) ;
          send_string_to_screen(nvram_info,FALSE) ;
        }
        else
        {
          send_string_to_screen("Host Serial EEPROM is not reponding.\r\n", TRUE);
        }
      }
      counter = BLOCK_1_EQUIVALENT - 1 ;
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Parallel EEPROM.\r\n",
          counter,
          OFFSETOF(EE_AREA,ee_block_01),
          sizeof(ee_area->ee_block_01),
          sizeof(ee_area->ee_block_01.crc16),
          sizeof(ee_area->ee_block_01.un_ee_block_01.block_01) +
                                 sizeof(ee_area->ee_block_01.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Parallel EEPROM.\r\n",
          counter,
          OFFSETOF(EE_AREA,ee_block_02),
          sizeof(ee_area->ee_block_02),
          sizeof(ee_area->ee_block_02.crc16),
          sizeof(ee_area->ee_block_02.un_ee_block_02.block_02) +
                                 sizeof(ee_area->ee_block_02.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      counter = BLOCK_100_EQUIVALENT - 1 ;
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Serial EEPROM.\r\n",
          counter,
          OFFSETOF(SERIAL_EE_AREA,ser_ee_block_01),
          sizeof(serial_ee_area->ser_ee_block_01),
          sizeof(serial_ee_area->ser_ee_block_01.crc16),
          sizeof(serial_ee_area->ser_ee_block_01.un_serial_ee_block_01.serial_ee_block_01) +
                             sizeof(serial_ee_area->ser_ee_block_01.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Serial EEPROM.\r\n",
          counter,
          OFFSETOF(SERIAL_EE_AREA,ser_ee_block_02),
          sizeof(serial_ee_area->ser_ee_block_02),
          sizeof(serial_ee_area->ser_ee_block_02.crc16),
          sizeof(serial_ee_area->ser_ee_block_02.un_serial_ee_block_02.serial_ee_block_02) +
                             sizeof(serial_ee_area->ser_ee_block_02.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      counter = BLOCK_3_EQUIVALENT - 1 ;
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Parallel EEPROM.\r\n",
          counter,
          OFFSETOF(EE_AREA,ee_block_03),
          sizeof(ee_area->ee_block_03),
          sizeof(ee_area->ee_block_03.crc16),
          sizeof(ee_area->ee_block_03.un_ee_block_03.block_03) +
                                 sizeof(ee_area->ee_block_03.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Parallel EEPROM.\r\n",
          counter,
          OFFSETOF(EE_AREA,ee_block_04),
          sizeof(ee_area->ee_block_04),
          sizeof(ee_area->ee_block_04.crc16),
          sizeof(ee_area->ee_block_04.un_ee_block_04.block_04) +
                                 sizeof(ee_area->ee_block_04.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Parallel EEPROM.\r\n",
          counter,
          OFFSETOF(EE_AREA,ee_block_05),
          sizeof(ee_area->ee_block_05),
          sizeof(ee_area->ee_block_05.crc16),
          sizeof(ee_area->ee_block_05.un_ee_block_05.block_05) +
                                 sizeof(ee_area->ee_block_05.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Parallel EEPROM.\r\n",
          counter,
          OFFSETOF(EE_AREA,ee_block_06),
          sizeof(ee_area->ee_block_06),
          sizeof(ee_area->ee_block_06.crc16),
          sizeof(ee_area->ee_block_06.un_ee_block_06.block_06) +
                                 sizeof(ee_area->ee_block_06.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      counter = BLOCK_7_EQUIVALENT - 1 ;
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Parallel EEPROM.\r\n",
          counter,
          OFFSETOF(EE_AREA,ee_block_07),
          sizeof(ee_area->ee_block_07),
          sizeof(ee_area->ee_block_07.crc16),
          sizeof(ee_area->ee_block_07.un_ee_block_07.block_07) +
                                 sizeof(ee_area->ee_block_07.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      if (get_slot_id())
      {
        if ( !get_host_ser_ee_err_boot_flag() )
        {
          /* we have a host board, so we can present its serial eeprom here as well */
          counter = BLOCK_200_EQUIVALENT - 1 ;
          {
            counter++ ;
            sal_sprintf(
              &nvram_info[0],
              "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
              "         Currently occupying %u bytes. Serial EEPROM.\r\n",
              counter,
              OFFSETOF(SERIAL_EE_AREA,host_ser_ee_block_01),
              sizeof(serial_ee_area->host_ser_ee_block_01),
              sizeof(serial_ee_area->host_ser_ee_block_01.crc16),
              sizeof(serial_ee_area->host_ser_ee_block_01.
                        un_host_serial_ee_block_01.host_serial_ee_block_01) +
                                 sizeof(serial_ee_area->host_ser_ee_block_01.crc16)
              ) ;
            send_string_to_screen(nvram_info,TRUE) ;
          }
          {
            counter++ ;
            sal_sprintf(
              &nvram_info[0],
              "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
              "         Currently occupying %u bytes. Serial EEPROM.\r\n",
              counter,
              OFFSETOF(SERIAL_EE_AREA,host_ser_ee_block_02),
              sizeof(serial_ee_area->host_ser_ee_block_02),
              sizeof(serial_ee_area->host_ser_ee_block_02.crc16),
              sizeof(serial_ee_area->host_ser_ee_block_02.
                      un_host_serial_ee_block_02.host_serial_ee_block_02) +
                                 sizeof(serial_ee_area->host_ser_ee_block_02.crc16)
              ) ;
            send_string_to_screen(nvram_info,TRUE) ;
          }
        }
        else
        {
            sal_sprintf(
              &nvram_info[0],
              "Cannot get blocks %u) & %u) - host serial eeprom not responding.\r\n"
              , BLOCK_200_EQUIVALENT, BLOCK_201_EQUIVALENT
            );
            send_string_to_screen(nvram_info,TRUE) ;
        }
      }
      if (front_does_card_exist)
      {
        if (front_does_card_exist())
        {
          /*
           * There is a front board, so we can present its
           * serial eeprom here as well
           */
          counter = BLOCK_300_EQUIVALENT - 1 ;
          {
            counter++ ;
            sal_sprintf(
              &nvram_info[0],
              "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
              "         Currently occupying %u bytes. Serial EEPROM.\r\n",
              counter,
              OFFSETOF(SERIAL_EE_AREA,front_ser_ee_block_01),
              sizeof(serial_ee_area->front_ser_ee_block_01),
              sizeof(serial_ee_area->front_ser_ee_block_01.crc16),
              sizeof(serial_ee_area->front_ser_ee_block_01.
                        un_front_serial_ee_block_01.front_serial_ee_block_01) +
                                 sizeof(serial_ee_area->front_ser_ee_block_01.crc16)
              ) ;
            send_string_to_screen(nvram_info,TRUE) ;
          }
          {
            counter++ ;
            sal_sprintf(
              &nvram_info[0],
              "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
              "         Currently occupying %u bytes. Serial EEPROM.\r\n",
              counter,
              OFFSETOF(SERIAL_EE_AREA,front_ser_ee_block_02),
              sizeof(serial_ee_area->front_ser_ee_block_02),
              sizeof(serial_ee_area->front_ser_ee_block_02.crc16),
              sizeof(serial_ee_area->front_ser_ee_block_02.
                      un_front_serial_ee_block_02.front_serial_ee_block_02) +
                                 sizeof(serial_ee_area->front_ser_ee_block_02.crc16)
              ) ;
            send_string_to_screen(nvram_info,TRUE) ;
          }
        }
        else
        {
            sal_sprintf(
              &nvram_info[0],
              "Cannot get blocks %u) & %u) -\r\n"
              " FRONT serial eeprom not responding.\r\n",
              BLOCK_300_EQUIVALENT, BLOCK_301_EQUIVALENT
            );
            send_string_to_screen(nvram_info,TRUE) ;
        }
      }
      else
      {
        sal_sprintf(
          &nvram_info[0],
          "Cannot get blocks %u) & %u) -\r\n"
          " FRONT serial eeprom not defined on this boot.\r\n",
          BLOCK_300_EQUIVALENT, BLOCK_301_EQUIVALENT
        );
        send_string_to_screen(nvram_info,TRUE) ;
      }

      /* GFA DB CARD { */

#if LINK_FAP21V_LIBRARIES
    if (gfa_fap21v_is_db_exist_on_board())
    {
      /*
      * There is a host_db board, so we can present its
      * serial eeprom here as well
      */
      counter = BLOCK_400_EQUIVALENT - 1 ;
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Serial EEPROM.\r\n",
          counter,
          OFFSETOF(SERIAL_EE_AREA,host_db_ser_ee_block_01),
          sizeof(serial_ee_area->host_db_ser_ee_block_01),
          sizeof(serial_ee_area->host_db_ser_ee_block_01.crc16),
          sizeof(serial_ee_area->host_db_ser_ee_block_01.
          un_host_db_serial_ee_block_01.host_db_serial_ee_block_01) +
          sizeof(serial_ee_area->host_db_ser_ee_block_01.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Serial EEPROM.\r\n",
          counter,
          OFFSETOF(SERIAL_EE_AREA,host_db_ser_ee_block_02),
          sizeof(serial_ee_area->host_db_ser_ee_block_02),
          sizeof(serial_ee_area->host_db_ser_ee_block_02.crc16),
          sizeof(serial_ee_area->host_db_ser_ee_block_02.
          un_host_db_serial_ee_block_02.host_db_serial_ee_block_02) +
          sizeof(serial_ee_area->host_db_ser_ee_block_02.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
    }
#endif /*LINK_FAP21V_LIBRARIES*/
#if LINK_FAP20V_LIBRARIES
    else if (gfa_is_db_exist_on_board())
    {
      /*
      * There is a host_db board, so we can present its
      * serial eeprom here as well
      */
      counter = BLOCK_400_EQUIVALENT - 1 ;
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Serial EEPROM.\r\n",
          counter,
          OFFSETOF(SERIAL_EE_AREA,host_db_ser_ee_block_01),
          sizeof(serial_ee_area->host_db_ser_ee_block_01),
          sizeof(serial_ee_area->host_db_ser_ee_block_01.crc16),
          sizeof(serial_ee_area->host_db_ser_ee_block_01.
          un_host_db_serial_ee_block_01.host_db_serial_ee_block_01) +
          sizeof(serial_ee_area->host_db_ser_ee_block_01.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
      {
        counter++ ;
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Serial EEPROM.\r\n",
          counter,
          OFFSETOF(SERIAL_EE_AREA,host_db_ser_ee_block_02),
          sizeof(serial_ee_area->host_db_ser_ee_block_02),
          sizeof(serial_ee_area->host_db_ser_ee_block_02.crc16),
          sizeof(serial_ee_area->host_db_ser_ee_block_02.
          un_host_db_serial_ee_block_02.host_db_serial_ee_block_02) +
          sizeof(serial_ee_area->host_db_ser_ee_block_02.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
      }
    }
#endif /*LINK_FAP20V_LIBRARIES*/
#if !(LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES)
    if(0)
    {
    }
#endif /*!(LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES)*/
    else
      {
          sal_sprintf(
            &nvram_info[0],
            "Cannot get blocks %u) & %u) -\r\n"
            " HOST_DB does not exist or serial eeprom not responding.\r\n",
            BLOCK_400_EQUIVALENT, BLOCK_401_EQUIVALENT
          );
          send_string_to_screen(nvram_info,TRUE) ;
      }
      /* GFA DB CARD }*/
      display_poll_statistics() ;
      break ;
    }
    case BLOCK_1_EQUIVALENT:
    {
      sal_sprintf(
        &nvram_info[0],
        "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
        "         Currently occupying %u bytes.\r\n",
        (unsigned short)block,
        OFFSETOF(EE_AREA,ee_block_01),
        sizeof(ee_area->ee_block_01),
        sizeof(ee_area->ee_block_01.crc16),
        sizeof(ee_area->ee_block_01.un_ee_block_01.block_01) +
                               sizeof(ee_area->ee_block_01.crc16)
        ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      print_nv_header() ;
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_STARTUP_COUNTER_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 140 + err ;
        goto pnbi_exit ;
      }
      break ;
    }
    case BLOCK_2_EQUIVALENT:
    {
      char
        dld_file_name[3][DLD_FILE_NAME_LEN + 1] ;
      sal_sprintf(
        &nvram_info[0],
        "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
        "         Currently occupying %u bytes.\r\n",
        (unsigned short)block,
        OFFSETOF(EE_AREA,ee_block_02),
        sizeof(ee_area->ee_block_02),
        sizeof(ee_area->ee_block_02.crc16),
        sizeof(ee_area->ee_block_02.un_ee_block_02.block_02) +
                               sizeof(ee_area->ee_block_02.crc16)
        ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      print_nv_header() ;
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_LOC_IP_ADDR_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 10 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_LOC_IP_MASK_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 20 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_GATEWAY_ADDR_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 30 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_DLD_HOST_ADDR_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 40 + err ;
        goto pnbi_exit ;
      }
      offset = (unsigned int)((char *)&(ee_area->
                    ee_block_02.un_ee_block_02.block_02.mem_drv_len)) ;
      err = get_nv_vals((void **)ul_ptrs,sizeof(*ul_ptrs[0]),(int)offset) ;
      if (err)
      {
        ret = 50 + err ;
        goto pnbi_exit ;
      }
      sal_sprintf(
        nvram_info,
        " File size on    |                |                |                |\r\n"
        "   Flash memory  |   0x%08lX   |  0x%08lX    |  0x%08lX    |",
        ul[0],ul[1],ul[2]) ;
      send_string_to_screen(nvram_info,TRUE) ;
      offset = (unsigned int)((char *)&(ee_area->
                    ee_block_02.un_ee_block_02.block_02.app_flash_size)) ;
      err = get_nv_vals((void **)ul_ptrs,sizeof(*ul_ptrs[0]),(int)offset) ;
      if (err)
      {
        ret = 60 + err ;
        goto pnbi_exit ;
      }
      sal_sprintf(
        nvram_info,
        " Total Flash size|                |                |                |\r\n"
        "   assigned to   |                |                |                |\r\n"
        "   program files |   0x%08lX   |  0x%08lX    |  0x%08lX    |",
        ul[0],ul[1],ul[2]) ;
      send_string_to_screen(nvram_info,TRUE) ;
      offset = (unsigned int)((char *)&(ee_area->
                    ee_block_02.un_ee_block_02.block_02.app_flash_base)) ;
      err = get_nv_vals((void **)ul_ptrs,sizeof(*ul_ptrs[0]),(int)offset) ;
      if (err)
      {
        ret = 60 + err ;
        goto pnbi_exit ;
      }
      sal_sprintf(
        nvram_info,
        " Base of Flash   |                |                |                |\r\n"
        "   assigned to   |                |                |                |\r\n"
        "   program files |   0x%08lX   |  0x%08lX    |  0x%08lX    |",
        ul[0],ul[1],ul[2]) ;
      send_string_to_screen(nvram_info,TRUE) ;
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_APP_SOURCE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 60 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_LOAD_METHOD_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 70 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_AUTO_FLASH_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 80 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_KERNEL_STARTUP_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 90 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_INST_CACHE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 100 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_DATA_CACHE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 110 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_CONSOLE_BAUD_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 120 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_CLK_MIRROR_DATA_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 420 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_TEST_AT_STARTUP_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 130 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_IP_SOURCE_MODE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 140 + err ;
        goto pnbi_exit ;
      }
      for (counter = 0 ; counter < (sizeof(dld_file_name)/sizeof(dld_file_name[0])) ; counter++)
      {
        uc_ptrs[counter] = &dld_file_name[counter][0] ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_DLD_FILE_NAME_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 150 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_BASE_REGISTER_04_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 491 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_OPTION_REGISTER_04_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 493 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_BASE_REGISTER_05_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 492 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_OPTION_REGISTER_05_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 494 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_TCP_TIMEOUT_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 496 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_TCP_RETRIES_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 498 + err ;
        goto pnbi_exit ;
      }
      break ;
    }
    case BLOCK_3_EQUIVALENT:
    {
      sal_sprintf(
        &nvram_info[0],
        "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
        "         Currently occupying %u bytes.\r\n",
        (unsigned short)block,
        OFFSETOF(EE_AREA,ee_block_03),
        sizeof(ee_area->ee_block_03),
        sizeof(ee_area->ee_block_03.crc16),
        sizeof(ee_area->ee_block_03.un_ee_block_03.block_03) +
                               sizeof(ee_area->ee_block_03.crc16)
        ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      print_nv_header() ;
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_ACTIVATE_WATCHDOG_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 410 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_WATCHDOG_PERIOD_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 420 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_LINE_MODE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 430 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_PAGE_MODE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 440 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_PAGE_LINES_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 450 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_CONF_FROM_MD_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 460 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)us_ptrs,PARAM_NVRAM_FE_CONFIGURATION_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 465 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_TELNET_TIMEOUT_STD_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 470 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_TELNET_TIMEOUT_CONT_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 475 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_EVENT_DISPLAY_LVL_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 480 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_EVENT_NVRAM_LVL_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 485 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_EVENT_SYSLOG_LVL_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 482 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_ACTIVATE_DPSS_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 489 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_USR_APP_FLAVOR_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 490 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_DPSS_DISPLAY_LEVEL_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 491 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_DPSS_DEBUG_LEVEL_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 492 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_DPSS_LOAD_FROM_FLASH_NOT_TFTP_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 488 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_RUN_UI_STARTUP_SCRIPT_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 487 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_STARTUP_DEMO_MODE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 486 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_FAP10M_RUN_MODE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 493 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_HOOK_SOFTWARE_EXCEPTION_EXC_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 494 + err ;
        goto pnbi_exit ;
      }
              err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_BCKG_TEMPERATURES_EN_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 495 + err ;
        goto pnbi_exit ;
      }
      break ;
    }
    case BLOCK_4_EQUIVALENT:
    {
      sal_sprintf(
        &nvram_info[0],
        "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
        "         Currently occupying %u bytes.\r\n",
        (unsigned short)block,
        OFFSETOF(EE_AREA,ee_block_04),
        sizeof(ee_area->ee_block_04),
        sizeof(ee_area->ee_block_04.crc16),
        sizeof(ee_area->ee_block_04.un_ee_block_04.block_04) +
                               sizeof(ee_area->ee_block_04.crc16)
        ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      err = display_watchdog_block(FALSE,FALSE,FALSE) ;
      if (err)
      {
        ret = 510 ;
        goto pnbi_exit ;
      }
      break ;
    }
    case BLOCK_5_EQUIVALENT:
    {
      sal_sprintf(
        &nvram_info[0],
        "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
        "         Currently occupying %u bytes.\r\n",
        (unsigned short)block,
        OFFSETOF(EE_AREA,ee_block_05),
        sizeof(ee_area->ee_block_05),
        sizeof(ee_area->ee_block_05.crc16),
        sizeof(ee_area->ee_block_05.un_ee_block_05.block_05) +
                               sizeof(ee_area->ee_block_05.crc16)
        ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      err = display_event_log_block() ;
      if (err)
      {
        ret = 520 ;
        goto pnbi_exit ;
      }
      break ;
    }
    case BLOCK_6_EQUIVALENT:
    {
      char snmp_cmd_line[3][SNMP_CMD_LINE_SIZE + 1] ;

      sal_sprintf(
        &nvram_info[0],
        "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
        "         Currently occupying %u bytes.\r\n",
        (unsigned short)block,
        OFFSETOF(EE_AREA,ee_block_06),
        sizeof(ee_area->ee_block_06),
        sizeof(ee_area->ee_block_06.crc16),
        sizeof(ee_area->ee_block_06.un_ee_block_06.block_06) +
                               sizeof(ee_area->ee_block_06.crc16)
        ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      print_nv_header() ;
      err = handle_nv_item((void **)uc_ptrs,PARAM_SNMP_DOWNLOAD_MIBS_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 504 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_SNMP_INCLUDE_NETSNMP_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 505 + err ;
        goto pnbi_exit ;
      }
      for (counter = 0 ; counter < (sizeof(snmp_cmd_line)/sizeof(snmp_cmd_line[0])) ; counter++)
      {
        uc_ptrs[counter] = &snmp_cmd_line[counter][0] ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_SNMP_SET_CMD_LINE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 503 + err ;
        goto pnbi_exit ;
      }
      break ;
    }
    case BLOCK_7_EQUIVALENT:
    {
      sal_sprintf(
        &nvram_info[0],
        "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
        "         Currently occupying %u bytes.\r\n",
        (unsigned short)block,
        OFFSETOF(EE_AREA,ee_block_07),
        sizeof(ee_area->ee_block_07),
        sizeof(ee_area->ee_block_07.crc16),
        sizeof(ee_area->ee_block_07.un_ee_block_07.block_07) +
                               sizeof(ee_area->ee_block_07.crc16)
        ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      err = display_software_exception_block(FALSE,FALSE,FALSE) ;
      if (err)
      {
        ret = 530 ;
        goto pnbi_exit ;
      }
      break ;
    }
    case BLOCK_100_EQUIVALENT:
    {
      sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes.\r\n",
          (unsigned short)block,
          OFFSETOF(SERIAL_EE_AREA,ser_ee_block_01),
          sizeof(serial_ee_area->ser_ee_block_01),
          sizeof(serial_ee_area->ser_ee_block_01.crc16),
          sizeof(serial_ee_area->ser_ee_block_01.un_serial_ee_block_01.serial_ee_block_01) +
                             sizeof(serial_ee_area->ser_ee_block_01.crc16)
          ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      print_nv_header() ;
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_ETH_ADDRESS_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 200 + err ;
        goto pnbi_exit ;
      }
      break ;
    }
    case BLOCK_101_EQUIVALENT:
    {
      char
        board_name[3][B_MAX_BOARD_NAME_LEN] ;
      char
        board_description[3][B_MAX_BOARD_DESCRIPTION_LEN] ;
      sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes.\r\n",
          (unsigned short)block,
          OFFSETOF(SERIAL_EE_AREA,ser_ee_block_02),
          sizeof(serial_ee_area->ser_ee_block_02),
          sizeof(serial_ee_area->ser_ee_block_02.crc16),
          sizeof(serial_ee_area->ser_ee_block_02.un_serial_ee_block_02.serial_ee_block_02) +
                             sizeof(serial_ee_area->ser_ee_block_02.crc16)
          ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      print_nv_header() ;
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_BOARD_SERIAL_NUM_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 300 + err ;
        goto pnbi_exit ;
      }
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_BOARD_HW_VERSION_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 301 + err ;
        goto pnbi_exit ;
      }
      for (counter = 0 ; counter < (sizeof(board_name)/sizeof(board_name[0])) ; counter++)
      {
        uc_ptrs[counter] = &board_name[counter][0] ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_BOARD_TYPE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 302 + err ;
        goto pnbi_exit ;
      }
      for (counter = 0 ; counter < (sizeof(board_description)/sizeof(board_description[0])) ; counter++)
      {
        uc_ptrs[counter] = &board_description[counter][0] ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_BOARD_DESCRIPTION_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 303 + err ;
        goto pnbi_exit ;
      }
      break ;
    }
    case BLOCK_200_EQUIVALENT:
    {
      unsigned short card_type;
      host_board_type_from_nv(&card_type);
      if ( get_slot_id() )
      {
        if ( !get_host_ser_ee_err_boot_flag() )
        {
          /* we have a host board, so we can present its serial eeprom here as well */
          sal_sprintf(
            &nvram_info[0],
            "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
            "         Currently occupying %u bytes. Serial EEPROM.\r\n",
            (unsigned short)block,
            OFFSETOF(SERIAL_EE_AREA,host_ser_ee_block_01),
            sizeof(serial_ee_area->host_ser_ee_block_01),
            sizeof(serial_ee_area->host_ser_ee_block_01.crc16),
            sizeof(serial_ee_area->
                    host_ser_ee_block_01.un_host_serial_ee_block_01.host_serial_ee_block_01) +
                               sizeof(serial_ee_area->host_ser_ee_block_01.crc16)
            ) ;
          send_string_to_screen(nvram_info,TRUE) ;
          print_nv_header() ;
          err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_HOST_BOARD_TYPE_ID,PRINT_ITEMS) ;
          if (err)
          {
            ret = 400 + err ;
            goto pnbi_exit ;
          }
        }
        else
        {
          sal_sprintf(
            &nvram_info[0],
              "Cannot get block %u) - host serial eeprom in not responding.\r\n"
              , BLOCK_200_EQUIVALENT
          );
          send_string_to_screen(nvram_info,TRUE) ;
        }
      }
      break ;
    }
    case BLOCK_201_EQUIVALENT:
    {
      char
        host_board_catalog_number[3][B_HOST_MAX_CATALOG_NUMBER_LEN];
      char
        host_board_description[3][B_HOST_MAX_BOARD_DESCRIPTION_LEN];
      char
        host_board_version[3][B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN];
      unsigned short card_type;
      host_board_type_from_nv(&card_type);
      if ( get_slot_id() )
      {
        if ( !get_host_ser_ee_err_boot_flag() )
        {
          /* we have a host board, so we can present its serial eeprom here as well */
          sal_sprintf(
            &nvram_info[0],
            "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
            "         Currently occupying %u bytes. Serial EEPROM.\r\n",
            (unsigned short)block,
            OFFSETOF(SERIAL_EE_AREA,host_ser_ee_block_02),
            sizeof(serial_ee_area->host_ser_ee_block_02),
            sizeof(serial_ee_area->host_ser_ee_block_02.crc16),
            sizeof(serial_ee_area->
                host_ser_ee_block_02.un_host_serial_ee_block_02.host_serial_ee_block_02) +
                               sizeof(serial_ee_area->host_ser_ee_block_02.crc16)
            ) ;
          send_string_to_screen(nvram_info,TRUE) ;
          print_nv_header() ;
          err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_HOST_BOARD_SN_ID,PRINT_ITEMS) ;
          if (err)
          {
            ret = 500 + err ;
            goto pnbi_exit ;
          }

          err =
            handle_nv_item(
              (void **)ul_ptrs,PARAM_NVRAM_HOST_BOARD_FAP_PP_IF_CLK_ID,PRINT_ITEMS) ;
          if (err)
          {
            ret = 501 + err ;
            goto pnbi_exit ;
          }
          err =
            handle_nv_item(
              (void **)ul_ptrs,PARAM_NVRAM_HOST_BOARD_FAP_USE_EEPROM_VALS_ID,PRINT_ITEMS) ;
          if (err)
          {
            ret = 501 + err ;
            goto pnbi_exit ;
          }
          err =
            handle_nv_item(
              (void **)ul_ptrs,PARAM_NVRAM_EEPROM_FRONT_CARD_TYPE_ID,PRINT_ITEMS) ;
          if (err)
          {
            ret = 509 + err ;
            goto pnbi_exit ;
          }

          for (counter = 0 ;
                  counter <
                    (sizeof(host_board_catalog_number) /
                            sizeof(host_board_catalog_number[0])) ; counter++)
          {
            uc_ptrs[counter] = &host_board_catalog_number[counter][0] ;
          }
          err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_HOST_BOARD_CAT_NUM_ID,PRINT_ITEMS) ;
          if (err)
          {
            ret = 502 + err ;
            goto pnbi_exit ;
          }
          for (counter = 0 ;
                  counter < (sizeof(host_board_version) /
                          sizeof(host_board_version[0])) ; counter++)
          {
            uc_ptrs[counter] = &host_board_version[counter][0] ;
          }
          err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_HOST_BOARD_VER_ID,PRINT_ITEMS) ;
          if (err)
          {
            ret = 503 + err ;
            goto pnbi_exit ;
          }
          for (counter = 0 ;
                counter < (sizeof(host_board_description) /
                            sizeof(host_board_description[0])) ; counter++)
          {
            uc_ptrs[counter] = &host_board_description[counter][0] ;
          }
          err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_HOST_BOARD_DESC_ID,PRINT_ITEMS) ;
          if (err)
          {
            ret = 505 + err ;
            goto pnbi_exit ;
          }
        }
        else
        {
          sal_sprintf(
            &nvram_info[0],
              "Can not get Block %u) - host serial eeprom is not responding.\r\n"
              , BLOCK_201_EQUIVALENT
          );
          send_string_to_screen(nvram_info,TRUE) ;
        }
      }
      break ;
    }
    case BLOCK_300_EQUIVALENT:
    {
      if (!front_does_card_exist)
      {
        sal_sprintf(
          &nvram_info[0],
            "Cannot get block %u) -\r\n"
            " FRONT serial eeprom not supported on this boot.\r\n",
            BLOCK_300_EQUIVALENT
        );
        send_string_to_screen(nvram_info,TRUE) ;
        break ;
      }
      if (front_does_card_exist() )
      {
        /* There is a host board, so we can present its serial
         * eeprom here as well
         */
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Serial EEPROM.\r\n",
          (unsigned short)block,
          OFFSETOF(SERIAL_EE_AREA,front_ser_ee_block_01),
          sizeof(serial_ee_area->front_ser_ee_block_01),
          sizeof(serial_ee_area->front_ser_ee_block_01.crc16),
          sizeof(serial_ee_area->
                  front_ser_ee_block_01.un_front_serial_ee_block_01.front_serial_ee_block_01) +
                             sizeof(serial_ee_area->front_ser_ee_block_01.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
        print_nv_header() ;
        err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_FRONT_BOARD_TYPE_ID,PRINT_ITEMS) ;
        if (err)
        {
          ret = 400 + err ;
          goto pnbi_exit ;
        }
      }
      else
      {
        sal_sprintf(
          &nvram_info[0],
            "Cannot get block %u) -\r\n"
            " FRONT serial eeprom in not responding.\r\n",
            BLOCK_300_EQUIVALENT
        );
        send_string_to_screen(nvram_info,TRUE) ;
      }
      break ;
    }
    case BLOCK_301_EQUIVALENT:
    {
      char
        front_board_catalog_number[3][B_FRONT_MAX_CATALOG_NUMBER_LEN];
      char
        front_board_description[3][B_FRONT_MAX_BOARD_DESCRIPTION_LEN];
      char
        front_board_version[3][B_FRONT_MAX_DEFAULT_BOARD_VERSION_LEN];
      if (!front_does_card_exist)
      {
        sal_sprintf(
          &nvram_info[0],
            "Cannot get block %u) -\r\n"
            " FRONT serial eeprom not supported on this boot.\r\n",
            BLOCK_301_EQUIVALENT
        );
        send_string_to_screen(nvram_info,TRUE) ;
        break ;
      }
      if (front_does_card_exist())
      {
        /*
         * There is a front board, so we can present its serial
         * eeprom here as well
         */
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Serial EEPROM.\r\n",
          (unsigned short)block,
          OFFSETOF(SERIAL_EE_AREA,front_ser_ee_block_02),
          sizeof(serial_ee_area->front_ser_ee_block_02),
          sizeof(serial_ee_area->front_ser_ee_block_02.crc16),
          sizeof(serial_ee_area->
              front_ser_ee_block_02.un_front_serial_ee_block_02.front_serial_ee_block_02) +
                             sizeof(serial_ee_area->front_ser_ee_block_02.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
        print_nv_header() ;
        err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_FRONT_BOARD_SN_ID,PRINT_ITEMS) ;
        if (err)
        {
          ret = 500 + err ;
          goto pnbi_exit ;
        }
        for (counter = 0 ;
                counter <
                  (sizeof(front_board_catalog_number) /
                          sizeof(front_board_catalog_number[0])) ; counter++)
        {
          uc_ptrs[counter] = &front_board_catalog_number[counter][0] ;
        }
        err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_FRONT_BOARD_CAT_NUM_ID,PRINT_ITEMS) ;
        if (err)
        {
          ret = 502 + err ;
          goto pnbi_exit ;
        }
        for (counter = 0 ;
                counter < (sizeof(front_board_version) /
                        sizeof(front_board_version[0])) ; counter++)
        {
          uc_ptrs[counter] = &front_board_version[counter][0] ;
        }
        err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_FRONT_BOARD_VER_ID,PRINT_ITEMS) ;
        if (err)
        {
          ret = 503 + err ;
          goto pnbi_exit ;
        }
        for (counter = 0 ;
              counter < (sizeof(front_board_description) /
                          sizeof(front_board_description[0])) ; counter++)
        {
          uc_ptrs[counter] = &front_board_description[counter][0] ;
        }
        err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_FRONT_BOARD_DESC_ID,PRINT_ITEMS) ;
        if (err)
        {
          ret = 505 + err ;
          goto pnbi_exit ;
        }
      }
      else
      {
        sal_sprintf(
          &nvram_info[0],
            "Can not get Block %u) -\r\n"
            " FRONT serial eeprom is not responding.\r\n",
            BLOCK_301_EQUIVALENT
        );
        send_string_to_screen(nvram_info,TRUE) ;
      }
      break ;
    }
    /* GFA DB CARD { */
    case BLOCK_400_EQUIVALENT:
    {
#if LINK_FAP21V_LIBRARIES
      if (gfa_fap21v_is_db_exist_on_board())
      {
        /* There is a host board, so we can present its serial
         * eeprom here as well
         */
        sal_sprintf(
          &nvram_info[0],
          "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
          "         Currently occupying %u bytes. Serial EEPROM.\r\n",
          (unsigned short)block,
          OFFSETOF(SERIAL_EE_AREA,host_db_ser_ee_block_01),
          sizeof(serial_ee_area->host_db_ser_ee_block_01),
          sizeof(serial_ee_area->host_db_ser_ee_block_01.crc16),
          sizeof(serial_ee_area->
                  host_db_ser_ee_block_01.un_host_db_serial_ee_block_01.host_db_serial_ee_block_01) +
                             sizeof(serial_ee_area->host_db_ser_ee_block_01.crc16)
          ) ;
        send_string_to_screen(nvram_info,TRUE) ;
        print_nv_header() ;
        err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_HOST_DB_BOARD_TYPE_ID,PRINT_ITEMS) ;
        if (err)
        {
          ret = 400 + err ;
          goto pnbi_exit ;
        }
      }
#endif /*LINK_FAP21V_LIBRARIES*/
#if LINK_FAP20V_LIBRARIES
    else if (gfa_is_db_exist_on_board())
    {
      /* There is a host board, so we can present its serial
      * eeprom here as well
      */
      sal_sprintf(
        &nvram_info[0],
        "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
        "         Currently occupying %u bytes. Serial EEPROM.\r\n",
        (unsigned short)block,
        OFFSETOF(SERIAL_EE_AREA,host_db_ser_ee_block_01),
        sizeof(serial_ee_area->host_db_ser_ee_block_01),
        sizeof(serial_ee_area->host_db_ser_ee_block_01.crc16),
        sizeof(serial_ee_area->
        host_db_ser_ee_block_01.un_host_db_serial_ee_block_01.host_db_serial_ee_block_01) +
        sizeof(serial_ee_area->host_db_ser_ee_block_01.crc16)
        ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      print_nv_header() ;
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_HOST_DB_BOARD_TYPE_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 400 + err ;
        goto pnbi_exit ;
      }
    }

#endif /*LINK_FAP20V_LIBRARIES*/
#if !(LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES)
      if (0) 
      {
      }
#endif /*!(LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES)*/
      else
      {
        sal_sprintf(
          &nvram_info[0],
            "Cannot get block %u) -\r\n"
            " HOST_DB does not exist or serial eeprom not responding.\r\n",
            BLOCK_400_EQUIVALENT
        );
        send_string_to_screen(nvram_info,TRUE) ;
      }
      break ;
    }
    case BLOCK_401_EQUIVALENT:
    {
#if LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES
      char
        host_db_board_catalog_number[3][B_HOST_DB_MAX_CATALOG_NUMBER_LEN];
      char
        host_db_board_description[3][B_HOST_DB_MAX_BOARD_DESCRIPTION_LEN];
      char
        host_db_board_version[3][B_HOST_DB_MAX_DEFAULT_BOARD_VERSION_LEN];
#endif

#if LINK_FAP21V_LIBRARIES
    if (gfa_fap21v_is_db_exist_on_board())
    {
      /*
      * There is a host_db board, so we can present its serial
      * eeprom here as well
      */
      sal_sprintf(
        &nvram_info[0],
        "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
        "         Currently occupying %u bytes. Serial EEPROM.\r\n",
        (unsigned short)block,
        OFFSETOF(SERIAL_EE_AREA,host_db_ser_ee_block_02),
        sizeof(serial_ee_area->host_db_ser_ee_block_02),
        sizeof(serial_ee_area->host_db_ser_ee_block_02.crc16),
        sizeof(serial_ee_area->
        host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.host_db_serial_ee_block_02) +
        sizeof(serial_ee_area->host_db_ser_ee_block_02.crc16)
        ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      print_nv_header() ;
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_HOST_DB_BOARD_SN_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 500 + err ;
        goto pnbi_exit ;
      }
      for (counter = 0 ;
        counter <
        (sizeof(host_db_board_catalog_number) /
        sizeof(host_db_board_catalog_number[0])) ; counter++)
      {
        uc_ptrs[counter] = &host_db_board_catalog_number[counter][0] ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_HOST_DB_BOARD_CAT_NUM_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 502 + err ;
        goto pnbi_exit ;
      }
      for (counter = 0 ;
        counter < (sizeof(host_db_board_version) /
        sizeof(host_db_board_version[0])) ; counter++)
      {
        uc_ptrs[counter] = &host_db_board_version[counter][0] ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_HOST_DB_BOARD_VER_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 503 + err ;
        goto pnbi_exit ;
      }
      for (counter = 0 ;
        counter < (sizeof(host_db_board_description) /
        sizeof(host_db_board_description[0])) ; counter++)
      {
        uc_ptrs[counter] = &host_db_board_description[counter][0] ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_HOST_DB_BOARD_DESC_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 505 + err ;
        goto pnbi_exit ;
      }
    }
#endif /*LINK_FAP21V_LIBRARIES*/
#if LINK_FAP20V_LIBRARIES
    if (gfa_is_db_exist_on_board())
    {
      /*
      * There is a host_db board, so we can present its serial
      * eeprom here as well
      */
      sal_sprintf(
        &nvram_info[0],
        "Block %u) Starts at offest %lu. Size: %u, including %u bytes of CRC.\r\n"
        "         Currently occupying %u bytes. Serial EEPROM.\r\n",
        (unsigned short)block,
        OFFSETOF(SERIAL_EE_AREA,host_db_ser_ee_block_02),
        sizeof(serial_ee_area->host_db_ser_ee_block_02),
        sizeof(serial_ee_area->host_db_ser_ee_block_02.crc16),
        sizeof(serial_ee_area->
        host_db_ser_ee_block_02.un_host_db_serial_ee_block_02.host_db_serial_ee_block_02) +
        sizeof(serial_ee_area->host_db_ser_ee_block_02.crc16)
        ) ;
      send_string_to_screen(nvram_info,TRUE) ;
      print_nv_header() ;
      err = handle_nv_item((void **)ul_ptrs,PARAM_NVRAM_HOST_DB_BOARD_SN_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 500 + err ;
        goto pnbi_exit ;
      }
      for (counter = 0 ;
        counter <
        (sizeof(host_db_board_catalog_number) /
        sizeof(host_db_board_catalog_number[0])) ; counter++)
      {
        uc_ptrs[counter] = &host_db_board_catalog_number[counter][0] ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_HOST_DB_BOARD_CAT_NUM_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 502 + err ;
        goto pnbi_exit ;
      }
      for (counter = 0 ;
        counter < (sizeof(host_db_board_version) /
        sizeof(host_db_board_version[0])) ; counter++)
      {
        uc_ptrs[counter] = &host_db_board_version[counter][0] ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_HOST_DB_BOARD_VER_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 503 + err ;
        goto pnbi_exit ;
      }
      for (counter = 0 ;
        counter < (sizeof(host_db_board_description) /
        sizeof(host_db_board_description[0])) ; counter++)
      {
        uc_ptrs[counter] = &host_db_board_description[counter][0] ;
      }
      err = handle_nv_item((void **)uc_ptrs,PARAM_NVRAM_HOST_DB_BOARD_DESC_ID,PRINT_ITEMS) ;
      if (err)
      {
        ret = 505 + err ;
        goto pnbi_exit ;
      }
    }
#endif /*LINK_FAP20V_LIBRARIES*/
#if LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES
      else
#endif /* LINK_FAP20V_LIBRARIES || LINK_FAP21V_LIBRARIES */
      {
        sal_sprintf(
          &nvram_info[0],
            "Can not get Block %u) -\r\n"
            " HOST_DB does not exist or serial eeprom not responding.\r\n",
            BLOCK_401_EQUIVALENT
        );
        send_string_to_screen(nvram_info,TRUE) ;
      }
      break ;
    }
    /* GFA DB CARD } */
    default:
    {
      ret = 1 ;
      goto pnbi_exit ;
      break ;
    }
  }
pnbi_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  handle_one_update_item
*TYPE: PROC
*DATE: 24/APR/2002
*FUNCTION:
*  Process input nvram/runtime parameter on a line
*  which has the 'update' item (within 'nvram' subject).
*CALLING SEQUENCE:
*  handle_one_update_item(current_line,)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    unsigned int *num_handled_ptr -
*      Pointer to counter of number of items
*      already handled. It indicated parameter
*      has been found by this procedure then
*      counter is incremented.
*    int param_id -
*      Identifier of the parameter to search for.
*    char *param_id_text -
*      Human readable text identifier of the parameter
*      to search for (display purposes only). Must
*      be a string shorter than 80 characters.
*    unsigned long param_type -
*      Kind of parameter to handle. Must be a
*      mask made out of a combination of:
*      VAL_NUMERIC, VAL_SYMBOL, VAL_TEXT, VAL_IP.
*    unsigned int  update -
*      Indication on what is to be updated:
*        RUN_TIME_EQUIVALENT     - update run time value
*        NEXT_STARTUP_EQUIVALENT - update NVRAM value
*        BOTH_EQUIVALENT         - update both
*  SOC_SAND_INDIRECT:
*    current_line.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    num_handled, error text printed (in case of error)
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  handle_one_update_item(
    CURRENT_LINE *current_line,
    unsigned int *num_handled_ptr,
    int          param_id,
    char         *param_id_text,
    unsigned long param_type,
    unsigned int  update
  )
{
  unsigned
    int
      num_handled ;
  int
    err,
    ret ;
  char
    err_msg[2][80*2] ;
  PARAM_VAL
    *param_val ;
  unsigned
    int
      match_index ;
  ret = FALSE ;
  num_handled = *num_handled_ptr ;
  if (!search_param_val_pairs(current_line,&match_index,param_id,1))
  {
    /*
     * Enter if 'param_id' is on the line.
     */
    if (!get_val_of(
            current_line,(int *)&match_index,param_id,1,
            &param_val,param_type,err_msg[1]))
    {
      /*
       * Value of 'param_id' parameter has been retrieved.
       */
      num_handled++ ;
      switch (param_val->val_type)
      {
        case VAL_NUMERIC:
        {
          err =
            update_nvram_param(update,
              param_id,
              (unsigned int)param_val->value.ulong_value) ;
          break ;
        }
        case VAL_SYMBOL:
        {
          err =
            update_nvram_param(update,
              param_id,
              (unsigned int)param_val->numeric_equivalent) ;
          break ;
        }
        case VAL_IP:
        {
          err =
            update_nvram_param(update,
              param_id,
              (unsigned int)param_val->value.ip_value) ;
          break ;
        }
        case VAL_TEXT:
        {
          err =
            update_nvram_param(update,
              param_id,
              (unsigned int)param_val->value.val_text) ;
          break ;
        }
        default:
        {
          err = -1 ;
          break ;
        }
      }
      if (err)
      {
        sal_sprintf(err_msg[0],
            "\r\n\n"
            "*** \'update_nvram_param\' returned with error (=%d)\r\n"
            "*** or \'unknown parameter type\' (=%lu).\r\n"
            "    Probably SW error\r\n",
            err,param_type) ;
        send_string_to_screen(err_msg[0],TRUE) ;
        ret = TRUE ;
        goto houp_exit ;
      }
    }
    else
    {
      /*
       * Value of 'param_id' parameter could not be retrieved.
       */
      sal_sprintf(err_msg[0],
          "\r\n\n"
          "*** Value of \'%s\' could not be interpreted. SW error.\r\n",param_id_text) ;
      send_string_to_screen(err_msg[0],TRUE) ;
      send_string_to_screen(err_msg[1],TRUE) ;
      ret = TRUE ;
      goto houp_exit ;
    }
  }
houp_exit:
  *num_handled_ptr = num_handled ;
  return (ret) ;
}
/*****************************************************
*NAME
*  subject_nvram
*TYPE: PROC
*DATE: 24/MAR/2002
*FUNCTION:
*  Process input line which has a 'nvram' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_nvram(current_line,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int
  subject_nvram(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  int
    ret,
    err ;
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4] ;
  ret = FALSE ;
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'nvram').
     */
    print_nvram_info(0) ;
  }
  else
  {
    /*
     * Enter if there are parameters on the line (not just 'nvram').
     */
    unsigned
      int
        match_index ;
    if (!search_param_val_pairs(current_line,&match_index,PARAM_NVRAM_SHOW_ID,1))
    {
      PARAM_VAL
        *block_param_val ;
      unsigned
        int
          block ;
      unsigned short card_type;
      /*
       * Enter if this is a 'nvram show' request.
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_NVRAM_BLOCK_ID,1,
              &block_param_val,VAL_NUMERIC | VAL_SYMBOL,err_msg))
      {
        /*
         * Value of block parameter could not be retrieved.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto sunv_exit ;
      }
      if (block_param_val->val_type & VAL_NUMERIC)
      {
        block = (unsigned int)block_param_val->value.ulong_value ;
      }
      else
      {
        block = (unsigned int)block_param_val->numeric_equivalent ;
      }
      host_board_type_from_nv(&card_type);
      if (BLOCK_200_EQUIVALENT == block || BLOCK_201_EQUIVALENT == block)
      {
        if (get_slot_id())
        {
          if ( get_host_ser_ee_err_boot_flag() )
          {
            /* found communication error with host serial eeprom at boot time */
            send_string_to_screen(
                "\r\n\n"
                "*** There was no response from host serial eeprom.\r\n",TRUE);
            ret = FALSE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /* this is a standalone board (slot_id == 0) */
          send_string_to_screen(
              "\r\n\n"
              "*** This parameter is only relevant for Fabric card or Line card\r\n",TRUE) ;
          ret = FALSE ;
          goto sunv_exit ;
        }
      }
      if (BLOCK_300_EQUIVALENT == block || BLOCK_301_EQUIVALENT == block)
      {

        if (!front_does_card_exist)
        {
          send_string_to_screen(
              "\r\n\n"
              "*** This boot version does not support Front End board.\r\n",TRUE) ;
          ret = FALSE ;
          goto sunv_exit ;
        }
        if (!front_does_card_exist())
        {
          /*
           * Communication error with front board serial eeprom (at boot time)
           * or, simply, there is no such card
           */
          send_string_to_screen(
              "\r\n\n"
              "*** No response from front serial eeprom (or card does not exist)\r\n",
              TRUE) ;
          ret = FALSE ;
          goto sunv_exit ;
        }
      }
      /* GFA DB CARD { */
      if (BLOCK_400_EQUIVALENT == block || BLOCK_401_EQUIVALENT == block)
      {
#if LINK_FAP21V_LIBRARIES
        if (!gfa_fap21v_is_db_exist_on_board())
#else
        if (1)
#endif /*#if LINK_FAP21V_LIBRARIES*/
        {
          /*
           * Communication error with host_db board serial eeprom (at boot time)
           * or, simply, there is no such card
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Host DB card does not exist on board.\r\n",
              TRUE) ;
          ret = FALSE ;
          goto sunv_exit ;
        }
#if LINK_FAP20V_LIBRARIES

    if (!gfa_is_db_exist_on_board())
    {
      /*
      * Communication error with host_db board serial eeprom (at boot time)
      * or, simply, there is no such card
      */
      send_string_to_screen(
        "\r\n\n"
        "*** Host DB card does not exist on board.\r\n",
        TRUE) ;
      ret = FALSE ;
      goto sunv_exit ;
    }
#endif /*LINK_FAP20V_LIBRARIES*/
      }
      /* GFA DB CARD } */

      err = print_nvram_info(block) ;
      if (err)
      {
        /*
         * Enter if 'print_nvram_info' returned with error.
         */
        sal_sprintf(err_msg,
          "\r\n"
          "*** Error (%d) from \'print_nvram_info\'.\r\n"
          "    Probably software error...\r\n",
          err) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto sunv_exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_NVRAM_L_DEFAULTS_ID,1))
    {
      PARAM_VAL
        *block_param_val ;
      unsigned
        int
          block ;
      /*
       * Enter if this is a 'nvram load_defaults' request.
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_NVRAM_L_DEFAULTS_ID,1,
              &block_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of load_defaults parameter could not be
         * retrieved (id of block to load).
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto sunv_exit ;
      }
      block = (unsigned int)block_param_val->numeric_equivalent ;
      switch (block)
      {
        case DEFAULTS_BLOCK_2_EQUIVALENT:
        {
          block = BLOCK_2_EQUIVALENT ;
          sal_sprintf(err_msg,
            "\r\n"
            "=== Defaults loaded into block no. %d.\r\n",
            block) ;
          block = 2 ;
          break ;
        }
        case DEFAULTS_BLOCK_3_EQUIVALENT:
        {
          block = BLOCK_3_EQUIVALENT ;
          sal_sprintf(err_msg,
            "\r\n"
            "=== Defaults loaded into block no. %d.\r\n",
            block) ;
          block = 3 ;
          break ;
        }
        case DEFAULTS_BLOCK_4_EQUIVALENT:
        {
          block = BLOCK_4_EQUIVALENT ;
          sal_sprintf(err_msg,
            "\r\n"
            "=== Defaults loaded into block no. %d.\r\n",
            block) ;
          block = 4 ;
          break ;
        }
        case DEFAULTS_BLOCK_5_EQUIVALENT:
        {
          block = BLOCK_5_EQUIVALENT ;
          sal_sprintf(err_msg,
            "\r\n"
            "=== Defaults loaded into block no. %d.\r\n",
            block) ;
          block = 5 ;
          break ;
        }
        case DEFAULTS_BLOCK_6_EQUIVALENT:
        {
          block = BLOCK_6_EQUIVALENT ;
          sal_sprintf(err_msg,
            "\r\n"
            "=== Defaults loaded into block no. %d.\r\n",
            block) ;
          block = 6 ;
          break ;
        }
        case DEFAULTS_BLOCK_7_EQUIVALENT:
        {
          block = BLOCK_7_EQUIVALENT ;
          sal_sprintf(err_msg,
            "\r\n"
            "=== Defaults loaded into block no. %d.\r\n",
            block) ;
          block = 7 ;
          break ;
        }
        case DEFAULTS_BLOCK_101_EQUIVALENT:
        {
          block = BLOCK_101_EQUIVALENT ;
          sal_sprintf(err_msg,
            "\r\n"
            "=== Defaults loaded into block no. %d.\r\n",
            block) ;
          block = (SERIAL_EE_FIRST_BLOCK_ID + 1) ;
          break ;
        }
        case DEFAULTS_BLOCK_200_EQUIVALENT:
        {
          block = BLOCK_200_EQUIVALENT ;
          sal_sprintf(err_msg,
            "\r\n"
            "=== Defaults loaded into block no. %d.\r\n",
            block) ;
          block = (HOST_SERIAL_EE_FIRST_BLOCK_ID) ;
          break ;
        }
        case DEFAULTS_BLOCK_201_EQUIVALENT:
        {
          block = BLOCK_201_EQUIVALENT ;
          sal_sprintf(err_msg,
            "\r\n"
            "=== Defaults loaded into block no. %d.\r\n",
            block) ;
          block = (HOST_SERIAL_EE_FIRST_BLOCK_ID + 1) ;
          break ;
        }
        default:
        case DEFAULTS_ALL_EQUIVALENT:
        {
          block = 0 ;
          sal_sprintf(err_msg,
            "\r\n\n"
            "=== Defaults loaded into all blocks except first ones."
            ) ;
          break ;
        }
      }
      err = ask_get(
          "\r\n\n"
          "This will reload the selected block(s) with default values!\r\n"
          "Hit \'space\' to reconfirm or any other key to\r\n"
          "cancel the operation ===> ",
          EXPECT_CONT_ABORT,FALSE,TRUE) ;
      if (err)
      {
        /*
         * Enter if user has chosen to continue with reloading of NVRAM
         * block.
         */
        err = load_defaults(block) ;
        if (err)
        {
          /*
           * Enter if 'load_defaults' returned with error.
           */
          sal_sprintf(err_msg,
            "\r\n"
            "*** Error (%d) from \'load_defaults\'.\r\n"
            "    Probably software error...\r\n",
            err) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sunv_exit ;
        }
        else
        {
          /*
           * Enter if 'load_defaults' returned with OK.
           */
          send_string_to_screen(err_msg,TRUE) ;
        }
      }
    }
    else if (!get_val_of(
              current_line,(int *)&match_index,PARAM_NVRAM_UPDATE_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
    {
      /*
       * Value of 'update' parameter could be retrieved. This should
       * be one of those parameters which can have both their runtime
       * value and their NVRAM value updated.
       */
      unsigned
        int
          num_handled,
          update ;
      update = (unsigned int)param_val->numeric_equivalent ;
      num_handled = 0 ;
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_SNMP_INCLUDE_NETSNMP_ID,
            "PARAM_SNMP_INCLUDE_NETSNMP_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_SNMP_DOWNLOAD_MIBS_ID,
            "PARAM_SNMP_DOWNLOAD_MIBS_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_SNMP_SET_CMD_LINE_ID,
            "PARAM_SNMP_SET_CMD_LINE_ID",VAL_TEXT,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_FE_CONFIGURATION_ID,
            "PARAM_NVRAM_FE_CONFIGURATION_ID",VAL_NUMERIC,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_CONF_FROM_MD_ID,
            "PARAM_NVRAM_CONF_FROM_MD_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_PAGE_LINES_ID,
            "PARAM_NVRAM_PAGE_LINES_ID",VAL_NUMERIC,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_PAGE_MODE_ID,
            "PARAM_NVRAM_PAGE_MODE_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_LINE_MODE_ID,
            "PARAM_NVRAM_LINE_MODE_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_ACTIVATE_WATCHDOG_ID,
            "PARAM_NVRAM_ACTIVATE_WATCHDOG_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_BCKG_TEMPERATURES_EN_ID,
            "PARAM_NVRAM_BCKG_TEMPERATURES_EN_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_HOOK_SOFTWARE_EXCEPTION_EXC_ID,
            "PARAM_NVRAM_HOOK_SOFTWARE_EXCEPTION_EXC_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_WATCHDOG_PERIOD_ID,
            "PARAM_NVRAM_WATCHDOG_PERIOD_ID",VAL_NUMERIC,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_DLD_HOST_ADDR_ID,
            "PARAM_NVRAM_DLD_HOST_ADDR_ID",VAL_IP,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_GATEWAY_ADDR_ID,
            "PARAM_NVRAM_GATEWAY_ADDR_ID",VAL_IP,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_LOC_IP_MASK_ID,
            "PARAM_NVRAM_LOC_IP_MASK_ID",VAL_NUMERIC,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_LOC_IP_ADDR_ID,
            "PARAM_NVRAM_LOC_IP_ADDR_ID",VAL_IP,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_DLD_FILE_NAME_ID,
            "PARAM_NVRAM_DLD_FILE_NAME_ID",VAL_TEXT,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_CONSOLE_BAUD_ID,
            "PARAM_NVRAM_CONSOLE_BAUD_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_CLK_MIRROR_DATA_ID,
            "PARAM_NVRAM_CLK_MIRROR_DATA_ID",VAL_NUMERIC,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_DATA_CACHE_ID,
            "PARAM_NVRAM_DATA_CACHE_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_INST_CACHE_ID,
            "PARAM_NVRAM_INST_CACHE_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_AUTO_FLASH_ID,
            "PARAM_NVRAM_AUTO_FLASH_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_IP_SOURCE_MODE_ID,
            "PARAM_NVRAM_IP_SOURCE_MODE_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_KERNEL_STARTUP_ID,
            "PARAM_NVRAM_KERNEL_STARTUP_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_LOAD_METHOD_ID,
            "PARAM_NVRAM_LOAD_METHOD_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_APP_SOURCE_ID,
            "PARAM_NVRAM_APP_SOURCE_ID",VAL_SYMBOL | VAL_NUMERIC,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_TEST_AT_STARTUP_ID,
            "PARAM_NVRAM_TEST_AT_STARTUP_ID",VAL_NUMERIC,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_TELNET_TIMEOUT_STD_ID,
            "PARAM_NVRAM_TELNET_TIMEOUT_STD_ID",VAL_NUMERIC | VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_TELNET_TIMEOUT_CONT_ID,
            "PARAM_NVRAM_TELNET_TIMEOUT_CONT_ID",VAL_NUMERIC | VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_EVENT_DISPLAY_LVL_ID,
            "PARAM_NVRAM_EVENT_DISPLAY_LVL_ID",VAL_NUMERIC | VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_EVENT_NVRAM_LVL_ID,
            "PARAM_NVRAM_EVENT_NVRAM_LVL_ID",VAL_NUMERIC | VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_EVENT_SYSLOG_LVL_ID,
            "PARAM_NVRAM_EVENT_SYSLOG_LVL_ID",VAL_NUMERIC | VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_RUN_UI_STARTUP_SCRIPT_ID,
            "PARAM_NVRAM_RUN_UI_STARTUP_SCRIPT_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_DPSS_LOAD_FROM_FLASH_NOT_TFTP_ID,
            "PARAM_NVRAM_DPSS_LOAD_FROM_FLASH_NOT_TFTP_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_DPSS_DISPLAY_LEVEL_ID,
            "PARAM_NVRAM_DPSS_DISPLAY_LEVEL_ID",VAL_NUMERIC,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_DPSS_DEBUG_LEVEL_ID,
            "PARAM_NVRAM_DPSS_DEBUG_LEVEL_ID",VAL_NUMERIC,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_STARTUP_DEMO_MODE_ID,
            "PARAM_NVRAM_STARTUP_DEMO_MODE_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_FAP10M_RUN_MODE_ID,
            "PARAM_NVRAM_FAP10M_RUN_MODE_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_USR_APP_FLAVOR_ID,
            "PARAM_NVRAM_USR_APP_FLAVOR_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_ACTIVATE_DPSS_ID,
            "PARAM_NVRAM_ACTIVATE_DPSS_ID",VAL_SYMBOL,update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_BASE_REGISTER_04_ID,
            "PARAM_NVRAM_BASE_REGISTER_04_ID",VAL_NUMERIC, update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_OPTION_REGISTER_04_ID,
            "PARAM_NVRAM_OPTION_REGISTER_04_ID",VAL_NUMERIC, update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_BASE_REGISTER_05_ID,
            "PARAM_NVRAM_BASE_REGISTER_05_ID",VAL_NUMERIC, update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_OPTION_REGISTER_05_ID,
            "PARAM_NVRAM_OPTION_REGISTER_05_ID",VAL_NUMERIC, update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_TCP_TIMEOUT_ID,
            "PARAM_NVRAM_TCP_TIMEOUT_ID",VAL_NUMERIC, update))
      {
        goto sunv_exit ;
      }
      if (handle_one_update_item(
            current_line,&num_handled,PARAM_NVRAM_TCP_RETRIES_ID,
            "PARAM_NVRAM_TCP_RETRIES_ID",VAL_NUMERIC, update))
      {
        goto sunv_exit ;
      }
      if (num_handled == 0)
      {
        /*
         * No NVRAM variable has been entered.
         * There must be at least one!
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Neither \'show\' nor any NVRAM parameter has been found.\r\n"
            "    At least one must be entered (e.g. \'data_cache\').\r\n",
            TRUE) ;
        ret = TRUE ;
        goto sunv_exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_NVRAM_BOARD_PARAMS_ID,1))
    {
      /*
       * Value of 'board_params' parameter was retrieved. This should
       * be one of those parameters which can only have their NVRAM value updated,
       * For example, 'cpu_board_description', 'cpu_board_hw_version',
       * 'host_board_version'.
       * The term 'cpu' here indicates 'mezzanine board with cpu' while 'host'
       * indicates 'host board which carries the mezzanine board'.
       */
      unsigned
        int
          num_handled ;
      num_handled = 0 ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_NVRAM_ETH_ADDRESS_ID,1))
      {
        /*
         * MEZZANINE
         * Enter if 'ethernet_address' is on the line.
         */
        if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_ETH_ADDRESS_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of 'ethernet_address' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_ETH_ADDRESS_ID,
              (unsigned int)param_val->value.ulong_value) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /*
           * Value of 'ethernet_address' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Value of \'ethernet_address\' could not be interpreted. SW error.\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sunv_exit ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_NVRAM_BOARD_TYPE_ID,1))
      {
        /*
         * MEZZANINE
         * Enter if 'board_type' is on the line.
         */
        if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_BOARD_TYPE_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'board_type' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_BOARD_TYPE_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /*
           * Value of 'board_type' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Value of \'board_type\' could not be interpreted. SW error.\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sunv_exit ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_NVRAM_BOARD_DESCRIPTION_ID,1))
      {
        /*
         * MEZZANINE
         * Enter if 'board_description' is on the line.
         */
        if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_BOARD_DESCRIPTION_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'board_description' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_BOARD_DESCRIPTION_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /*
           * Value of 'board_description' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Value of \'board_description\' could not be interpreted. SW error.\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sunv_exit ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_NVRAM_BOARD_SERIAL_NUM_ID,1))
      {
        /*
         * MEZZANINE
         * Enter if 'board_serial_number' is on the line.
         */
        if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_BOARD_SERIAL_NUM_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of 'board_serial_number' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_BOARD_SERIAL_NUM_ID,
              (unsigned int)param_val->value.ulong_value) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /*
           * Value of 'board_serial_number' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Value of \'board_serial_number\' could not be interpreted. SW error.\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sunv_exit ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_NVRAM_BOARD_HW_VERSION_ID,1))
      {
        /*
         * MEZZANINE
         * Enter if 'board_hw_version' is on the line. This is the hardware version
         * of the mezzanine board.
         */
        if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_BOARD_HW_VERSION_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of 'board_hw_version' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_BOARD_HW_VERSION_ID,
              (unsigned int)param_val->value.ulong_value) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /*
           * Value of 'board_hw_version' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Value of \'board_hw_version\' could not be interpreted. SW error.\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sunv_exit ;
        }
      }
      /*
       * Here start handling of HOST board parameters.
       */
      else if (
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_BOARD_TYPE_ID,1))     ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_BOARD_CAT_NUM_ID,1))  ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_BOARD_DESC_ID,1))     ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_BOARD_VER_ID,1))      ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_BOARD_SN_ID,1))       ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_BOARD_FAP_PP_IF_CLK_ID,1)) ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_EEPROM_FRONT_CARD_TYPE_ID,1)) ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_BOARD_FAP_USE_EEPROM_VALS_ID,1))
        )
      {
        /*
         * HOST
         *
         * Enter if this is one of HOST board parameters. To be able to update
         * those parameters, communication with host board must, obviously, be
         * established.
         * Also, card must NOT be stand alone (judging from 'slot id').
         */
        if (get_slot_id())
        {
          /*
           * Enter if this is NOT a stand alone card.
           */
          if (get_host_ser_ee_err_boot_flag())
          {
            /*
             * Found communication error with host serial eeprom at boot time
             */
            send_string_to_screen(
                "\r\n\n"
                "*** There was no response from host serial eeprom.\r\n",TRUE);
            ret = FALSE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /*
           * This is a standalone board (slot_id == 0)
           */
          send_string_to_screen(
              "\r\n\n"
              "*** This parameter is only relevant when HOST board is present\r\n",TRUE) ;
          ret = FALSE ;
          goto sunv_exit ;
        }
        /*
         * At this point, it has been established that this is NOT a stand alone
         * card and there is established communication with serial eeprom on host
         * board.
         */
        if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_BOARD_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of 'host_board_type' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_HOST_BOARD_TYPE_ID,
              (unsigned int)param_val->numeric_equivalent) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_EEPROM_FRONT_CARD_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of 'host_board_front_card_type' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_EEPROM_FRONT_CARD_TYPE_ID,
              (unsigned int)param_val->numeric_equivalent) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'host_board_front_card_type\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_BOARD_CAT_NUM_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'host_board_catalog_number' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_HOST_BOARD_CAT_NUM_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_BOARD_DESC_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'host_board_description' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_HOST_BOARD_DESC_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_BOARD_VER_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'host_board_version' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_HOST_BOARD_VER_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_BOARD_SN_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of 'host_board_serial_number' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_HOST_BOARD_SN_ID,
              (unsigned int)param_val->value.ulong_value) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_BOARD_FAP_PP_IF_CLK_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of 'host_board_fap_pp_if_clk' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_HOST_BOARD_FAP_PP_IF_CLK_ID,
              (unsigned int)param_val->value.ulong_value) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'host_board_fap_pp_if_clk\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_BOARD_FAP_USE_EEPROM_VALS_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of 'fap_use_eeprom_vals' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(NEXT_STARTUP_EQUIVALENT,
              PARAM_NVRAM_HOST_BOARD_FAP_USE_EEPROM_VALS_ID,
              (unsigned int)param_val->value.ulong_value) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'fap_use_eeprom_vals\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /*
           * Value of 'host_board_catalog_number' parameter could not be retrieved OR
           * Value of 'host_board_type' parameter could not be retrieved OR
           * Value of 'host_board_catalog_number' parameter could not be retrieved OR
           * Value of 'host_board_version' parameter could not be retrieved OR
           * Value of 'host_board_serial_number' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Value of \'host_board_catalog_number\' or of \'host_board_type\'\r\n"
              "    or of \'host_board_description\' or of \'host_board_version\'\r\n"
              "    or of \'host_board_serial_number\' could not be interpreted. SW error.\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sunv_exit ;
        }
      }
      /*
       * Here start handling of FRONT board parameters.
       */
      else if (
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_FRONT_BOARD_TYPE_ID,1))     ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_FRONT_BOARD_CAT_NUM_ID,1))  ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_FRONT_BOARD_DESC_ID,1))     ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_FRONT_BOARD_VER_ID,1))      ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_FRONT_BOARD_SN_ID,1))
        )
      {
        /*
         * FRONT end board
         *
         * Enter if this is one of FRONT board parameters. To be able to update
         * those parameters, communication with front board must, obviously, be
         * established.
         * Also, carrying card must have Front end
         */
        if (front_does_card_exist)
        {
          /*
           * Enter if boot supports communication with Front End card.
           */
          if (!front_does_card_exist())
          {
            /*
             * Could not establish communication with front serial eeprom
             */
            send_string_to_screen(
                "\r\n\n"
                "*** Could not establish communication with front end card.\r\n",TRUE);
            ret = FALSE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /*
           * This boot version does not support Front End Eeprom
           */
          send_string_to_screen(
              "\r\n\n"
              "*** This boot version does not support Front End Eeprom\r\n",TRUE) ;
          ret = FALSE ;
          goto sunv_exit ;
        }
        /*
         * At this point, communication was established: Front end card exists
         */
        if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_FRONT_BOARD_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of 'front_board_type' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_FRONT_BOARD_TYPE_ID,
              (unsigned int)param_val->numeric_equivalent) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_EEPROM_FRONT_CARD_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of 'front_board_front_card_type' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_EEPROM_FRONT_CARD_TYPE_ID,
              (unsigned int)param_val->numeric_equivalent) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'front_board_front_card_type\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_FRONT_BOARD_CAT_NUM_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'front_board_catalog_number' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_FRONT_BOARD_CAT_NUM_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_FRONT_BOARD_DESC_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'front_board_description' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_FRONT_BOARD_DESC_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_FRONT_BOARD_VER_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'front_board_version' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_FRONT_BOARD_VER_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_FRONT_BOARD_SN_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of 'front_board_serial_number' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_FRONT_BOARD_SN_ID,
              (unsigned int)param_val->value.ulong_value) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /*
           * Value of 'front_board_catalog_number' parameter could not be retrieved OR
           * Value of 'front_board_type' parameter could not be retrieved OR
           * Value of 'front_board_catalog_number' parameter could not be retrieved OR
           * Value of 'front_board_version' parameter could not be retrieved OR
           * Value of 'front_board_serial_number' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Value of \'front_board_catalog_number\' or of \'front_board_type\'\r\n"
              "    or of \'front_board_description\' or of \'front_board_version\'\r\n"
              "    or of \'front_board_serial_number\' could not be interpreted. SW error.\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sunv_exit ;
        }
      }
      /*
       * Here start handling of HOST_DB board parameters.
       */
      else if (
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_DB_BOARD_TYPE_ID,1))     ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_DB_BOARD_CAT_NUM_ID,1))  ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_DB_BOARD_DESC_ID,1))     ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_DB_BOARD_VER_ID,1))      ||
        (!search_param_val_pairs(
            current_line,&match_index,PARAM_NVRAM_HOST_DB_BOARD_SN_ID,1))
        )
      {
        /*
         * HOST_DB board
         *
         * Enter if this is one of HOST_DB board parameters. To be able to update
         * those parameters, communication with host_db board must, obviously, be
         * established.
         * Also, carrying card must have Host_db
         */
#if LINK_FAP21V_LIBRARIES
        if (!gfa_fap21v_is_db_exist_on_board())
#else
        if (1)
#endif /*#if LINK_FAP21V_LIBRARIES*/
      {
        /*
        * Could not establish communication with host_db serial eeprom
        */
        send_string_to_screen(
          "\r\n\n"
          "*** Host DB card does not exist on board.\r\n",TRUE);
        ret = FALSE ;
        goto sunv_exit ;
      }
#if LINK_FAP20V_LIBRARIES
      if ((!gfa_is_db_exist_on_board()))
      {
        /*
        * Could not establish communication with host_db serial eeprom
        */
        send_string_to_screen(
          "\r\n\n"
          "*** Host DB card does not exist on board.\r\n",TRUE);
        ret = FALSE ;
        goto sunv_exit ;
      }
#endif /*LINK_FAP20V_LIBRARIES*/

        /*
         * At this point, communication was established: Host_db end card exists
         */
        if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_DB_BOARD_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of 'host_db_board_type' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_HOST_DB_BOARD_TYPE_ID,
              (unsigned int)param_val->numeric_equivalent) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_EEPROM_HOST_DB_CARD_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of 'host_db_board_host_db_card_type' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_EEPROM_HOST_DB_CARD_TYPE_ID,
              (unsigned int)param_val->numeric_equivalent) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'host_db_board_host_db_card_type\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_DB_BOARD_CAT_NUM_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'host_db_board_catalog_number' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_HOST_DB_BOARD_CAT_NUM_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_DB_BOARD_DESC_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'host_db_board_description' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_HOST_DB_BOARD_DESC_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_DB_BOARD_VER_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of 'host_db_board_version' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_HOST_DB_BOARD_VER_ID,
              (unsigned int)param_val->value.val_text) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else if (!get_val_of(
                current_line,(int *)&match_index,PARAM_NVRAM_HOST_DB_BOARD_SN_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of 'host_db_board_serial_number' parameter has been retrieved.
           */
          num_handled++ ;
          err =
            update_nvram_param(BOTH_EQUIVALENT,
              PARAM_NVRAM_HOST_DB_BOARD_SN_ID,
              (unsigned int)param_val->value.ulong_value) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update_nvram_param\' returned with error (=%d).\r\n"
                "    Probably SW error\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto sunv_exit ;
          }
        }
        else
        {
          /*
           * Value of 'host_db_board_catalog_number' parameter could not be retrieved OR
           * Value of 'host_db_board_type' parameter could not be retrieved OR
           * Value of 'host_db_board_catalog_number' parameter could not be retrieved OR
           * Value of 'host_db_board_version' parameter could not be retrieved OR
           * Value of 'host_db_board_serial_number' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Value of \'host_db_board_catalog_number\' or of \'host_db_board_type\'\r\n"
              "    or of \'host_db_board_description\' or of \'host_db_board_version\'\r\n"
              "    or of \'host_db_board_serial_number\' could not be interpreted. SW error.\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto sunv_exit ;
        }
      }
      if (num_handled == 0)
      {
        /*
         * No NVRAM variable has been entered.
         * There must be at least one!
         */
        send_string_to_screen(
            "\r\n\n"
            "*** No NVRAM board parameters have been found.\r\n"
            "    At least one must be entered (e.g. \'host_board_serial_number\').\r\n",
            TRUE) ;
        ret = TRUE ;
        goto sunv_exit ;
      }
    }
    else
    {
      /*
       * Enter if an unknown request.
       */
      send_string_to_screen(
        "\r\n"
        "*** Nvram command without \'read\', \'write\', \'load_defaults\'\r\n"
        "*** or \'board_params\'. Syntax error...\r\n",
        TRUE) ;
      ret = TRUE ;
      goto sunv_exit ;
    }
  }
sunv_exit:
  return (ret) ;
}
/*****************************************************
*NAME
*   APP_SOURCE_CHECKER
*TYPE: PROC
*DATE: 29/MAR/2002
*FUNCTION:
*  Check app_source_file input as entered by the user
*  in the context of 'nvram' command line subject.
*CALLING SEQUENCE:
*  app_source_checker(current_line_ptr,source_id,err_msg)
*INPUT:
*  SOC_SAND_DIRECT:
*    void   *current_line_ptr -
*      Pointer to a structure of type 'CURRENT_LINE':
*      current prompt line.
*    unsigned long  source_id -
*      Id of file in Flash (starting from '1')
*      to use (first) as the application source file.
*      (Of course, if there is no legitimate source
*      in that location, other files are given a try).
*    char *err_msg -
*      This output is only meaningful when return value
*      is non-zero. Pointer to string to load with
*      explanation of why validation of input parameter
*      has failed.
*    unsigned long partial -
*      Flag.
*      If 'TRUE' then input field may be
*        partial (i.e. user has not yet filled the
*        whole field.) Check legitimacy so far.
*      If 'FALSE' then input string must be
*        a complete legitimate field. This
*        means that it should conform to all
*        legitimacy checks.
*  SOC_SAND_INDIRECT:
*    Utility related to number of files on Flash:
*    get_num_flash_files().
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then indicated source file id is
*      out of range.
*  SOC_SAND_INDIRECT:
*    Updated NV ram.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  app_source_checker(
    void           *current_line_ptr,
    unsigned long  source_id,
    char           *err_msg,
    unsigned long  partial
    )
{
  int
    ret ;
  unsigned
    int
      num_flash_files ;
  ret = FALSE ;
  err_msg[0] = 0 ;
  num_flash_files = get_num_flash_files() ;
  if ((source_id < 1) || ((source_id > num_flash_files)))
  {
    ret = TRUE ;
    sal_sprintf(err_msg,
      "*** Source file id is out of range (should be between 1 and %d).\r\n",num_flash_files) ;
  }
  return (ret) ;
}
