/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */


/* plan to remove this file, not needed */


/*
 * Tasks information.
 */
#include <appl/diag/dpp/tasks_info.h>

#if !DUNE_BCM 
/*
 * INCLUDE FILES:
* {
 */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#if defined(SIM_ON_WINDOWS)
/* { */
#include "time.h"
#include "conio.h"
#include "io.h"
#include "winsock2.h"
/* } */
#elif (defined(LINUX) || defined(UNIX))
/*{*/
#include <sys/time.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define ERROR (-1)
#define OK 0

/*}*/
#else
/* { */
#include "ctype.h"
#include "taskLib.h"
#include "semLib.h"
#include "errnoLib.h"
#include "sysLib.h"
#include "usrLib.h"
#include "tickLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "logLib.h"
#include "pipeDrv.h"
#include "timers.h"
#include "sigLib.h"
#include "intLib.h"
#include "rebootLib.h"
#include "inetLib.h"
/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include "private\timerLibP.h"
#include "logLib.h"
#include "taskHookLib.h"
#include "shellLib.h"
#include "dbgLib.h"
#include "drv/mem/eeprom.h"
#include "usrApp.h"
/* } */
#endif
#endif /* !DUNE_BCM */

#if !DUNE_BCM
#else
#if (defined(LINUX) || defined(UNIX))
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#endif

/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defi.h>
#include <appl/diag/dpp/utils_defx.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
/*
 * }
 */
#if !DUNE_BCM
#else
/*
 * OBJECT:
 *   In ISR
 *     Utilities related to status of processor:
 *     Is it within an ISR or not.
 *     If 'In_isr' is 'true' then system is within
 *     ISR and some operating system calls may not
 *     be called and, if they do, will do nothing.
 * {
 */
static
  unsigned
    int
      In_isr = 0 ;
void
  set_in_isr(
    unsigned int in_isr
  )
{
  In_isr = in_isr ;
  return ;
}
int
  is_in_isr(
    void
  )
{
  return (In_isr) ;
}
#endif
  /*****************************************************
*NAME
*  d_taskSafe
*TYPE: PROC
*DATE: 28/FEB/2002
*FUNCTION:
*  Protects the calling task from deletion.
*  This is a private wrap allowing for
*  control over operating system (single location
*  of invocation).
*CALLING SEQUENCE:
*  d_taskSafe()
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  Private controls are only valid within task context
*  (since taskLock and taskUnlock are used).
*
*  If operation fails, this routine may not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*SEE ALSO:
 */
STATUS
   d_taskSafe(
        void
       )
{
  STATUS
      status ;
#ifndef SIM_ON_WINDOWS
/* { */
  status = soc_sand_os_task_lock() ;
/* } */
#else
/* { */
  status = FALSE ;
/* } */
#endif
  return (status) ;
}
/*****************************************************
*NAME
*  d_taskUnsafe
*TYPE: PROC
*DATE: 28/FEB/2002
*FUNCTION:
*  Release Protection of calling task from deletion.
*  This is a private wrap allowing for
*  control over operating system (single location
*  of invocation).
*CALLING SEQUENCE:
*  d_taskUnsafe()
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  Private controls are only valid within task context
*  (since taskLock and taskUnlock are used).
*
*  If operation fails, this routine may not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*SEE ALSO:
 */
STATUS
   d_taskUnsafe(
        void
         )
{
  STATUS
      status ;
#ifndef SIM_ON_WINDOWS
/* { */
  status = soc_sand_os_task_unlock() ;
/* } */
#else
/* { */
  status = FALSE ;
/* } */
#endif
  return (status) ;
}

/*****************************************************
*NAME
*  d_taskDelay
*TYPE: PROC
*DATE: 28/FEB/2002
*FUNCTION:
*  Delays the calling task as specified.
*  This is a private wrap allowing for
*  control over operating system (single location
*  of invocation).
*CALLING SEQUENCE:
*  d_taskDelay()
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation. taskDelay().
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  If operation fails, this routine may not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*SEE ALSO:
 */
STATUS
   d_taskDelay(
         int ticks
        )
{
  STATUS
      status ;
  uint32 premature;

#ifndef SIM_ON_WINDOWS
/* { */
  status = soc_sand_os_nano_sleep(ticks / soc_sand_os_get_ticks_per_sec() * 1000000000, &premature);
/* } */
#else
/* { */
  {
    uint32
        start_tick ;
    start_tick = clock() ;
    while (((unsigned long)clock() - start_tick) < ((unsigned long)ticks * 60))
    {
    }
    status = FALSE ;
  }
/* } */
#endif
  return (status) ;
}
/*****************************************************
*NAME
*  d_taskSpawn
*TYPE: PROC
*DATE: 17/JAN/2002
*FUNCTION:
*  Create and start a task. This is a private wrap allowing for
*  control over operating system (single location
*  of invocation).
*CALLING SEQUENCE:
*  d_taskSpawn(private_task_id)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as taskSpawn. See documentation
*    In addition:
*      int private_task_id -
*        Private identifier of this task within this
*        system.
*  SOC_SAND_INDIRECT:
*    Same as taskSpawn. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as taskSpawn. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as taskSpawn. See documentation.
*REMARKS:
*  Private controls are only valid within task context
*  (since taskLock and taskUnlock are used). Do not
*  use within ISRs unless interrupt disabling is
*  used instead of locking task switching.
*
*  If operation fails, this routine will not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*
*  This procedure is not callable from interrupt
*  service routines
*SEE ALSO:
 */
sal_thread_t
   d_taskSpawn(
         char *name,        /* name of new task (stored at pStackBase) */
         int  priority,     /* priority of new task */
         int  options,      /* task option word */
         int  stackSize,    /* size (bytes) of stack needed plus name */
         FUNCPTR_BCM entryPt,   /* entry point of new task */
         int  arg1,         /* 1st of 10 req'd task args to pass to func */
         int  arg2,
         int  arg3,
         int  arg4,
         int  arg5,
         int  arg6,
         int  arg7,
         int  arg8,
         int  arg9,
         int  arg10,
         int  private_task_id
        )
{
  return
    soc_sand_os_task_spawn(
      name, priority, options,stackSize,entryPt,arg1, arg2,
      arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10
    );
}

/*****************************************************
*NAME
*  d_is_task_safe
*TYPE: PROC
*DATE: 11/APR/2002
*FUNCTION:
*  Indicate whether task is protected from deletion.
*  Implementation is specific to OS.
*CALLING SEQUENCE:
*  d_is_task_safe()
*INPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    OS system.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      if non-zero than task is in 'safe' state
*      (protected).
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  d_is_task_safe(
    void
  )
{
  int
    ret ;
/* { */
  ret = FALSE ;
/* } */
  return (ret) ;
}

/*****************************************************
*NAME
*  d_ioGlobalStdGet
*TYPE: PROC
*DATE: 12/APR/2002
*FUNCTION:
*  get the file descriptor for global standard
*  input/output/error.
*  This is a private wrap allowing for control over
*  operating system (single location of invocation).
*CALLING SEQUENCE:
*  d_ioGlobalStdGet
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as taskDelete. See documentation
*  SOC_SAND_INDIRECT:
*    Same as taskDelete. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as taskDelete. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as taskDelete. See documentation.
*REMARKS:
*  If operation fails, this routine will not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*SEE ALSO:
 */
int
  d_ioGlobalStdGet(
    int stdFd /* std input (0), output (1), or error (2) */
  )
{
  int
      ret ;
  ret = stdFd;
  return (ret) ;
}


/*
 * Operating system wrap routines. All procedures
 * start with d_<Original OS Name>
 * {
 */
/*****************************************************
*NAME
*  d_write
*TYPE: PROC
*DATE: 21/JAN/2002
*FUNCTION:
*  Write a buffer to the specified file.
*  This is a private wrap allowing for control over
*  operating system (single location of invocation).
*CALLING SEQUENCE:
*  d_write(std_out_target)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as write. See documentation
*    int std_out_target -
*      If 'true' then output is STD_OUT. In that
*      case, it is blocked when file descriptor
*      for STD_OUT has changed, untill the next
*      restart of user interface task (This happens
*      when telnet is activated or deactivated).
*  SOC_SAND_INDIRECT:
*    Same as write. See documentation.
*    Also, 'is_std_io_blocked' is used to prevent
*    output until proper file descriptor is
*    straightened out.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as write. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as write. See documentation.
*REMARKS:
*  (Currently, this procedure does not lock tasks
*  so all warnings, relating to ISRs, are irrelevant.
*  This may change, however.)
*
*  If operation fails, this routine will not
*  return to the caller (fatal error).
*SEE ALSO:
 */
int
   d_write(
       int    fd,      /* file descriptor on which to write */
       char   *buffer, /* buffer containing bytes to be written */
       int    nbytes,  /* number of bytes to write */
       int    std_out_target
      )
{
  char
      *proc_name ;
  int
      ret ;
  proc_name = "d_write" ;
#ifdef SIM_ON_WINDOWS
/* { */
  ret = nbytes ;
  if (std_out_target)
  {
    unsigned
        int
        ui ;
    char
        *c_ptr ;
    c_ptr = buffer ;
    for (ui = 0 ; ui < (unsigned int)nbytes ; ui++)
    {
      putchar(*c_ptr++) ;
    }
  }
/* } */
#elif (defined(LINUX) || defined(UNIX)) || defined(__DUNE_GTO_BCM_CPU__)
/* { */
  ret = write(fd,buffer,nbytes) ;
  if (ret == (int)ERROR)
  {
    gen_err(TRUE,TRUE,(int)ret,ERROR,"d_write 1 - ",proc_name,
        SVR_FTL,D_WRITE_ERR_01,FALSE,0,-1,FALSE);
  }
/* } */
#else
/* { */
  {
    int
        error_value ;
    if (is_std_io_blocked() && std_out_target)
    {
      ret = 0 ;
    }
    else
    {
      ret = write(fd,buffer,nbytes) ;
      if (ret == (int)ERROR)
      {
        error_value = errnoGet() ;
        if ((error_value == S_iosLib_INVALID_FILE_DESCRIPTOR) && std_out_target)
        {
          set_std_io_blocked(TRUE) ;
        }
        else
        {
          gen_err(TRUE,TRUE,(int)ret,ERROR,"d_write 1 - ",proc_name,
              SVR_FTL,D_WRITE_ERR_01,FALSE,0,-1,FALSE) ;
        }
      }
    }
  }
/* } */
#endif
  return (ret);
}

/*****************************************************
*NAME
*  d_ioctl
*TYPE: PROC
*DATE: 17/JAN/2002
*FUNCTION:
*  Carry out IO control operation.
*  This is a private wrap allowing for control over
*  operating system (single location of invocation).
*CALLING SEQUENCE:
*  d_ioctl(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as taskDelete. See documentation
*  SOC_SAND_INDIRECT:
*    Same as taskDelete. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as taskDelete. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as taskDelete. See documentation.
*REMARKS:
*  Private controls are only valid within task context
*  (since taskLock and taskUnlock are used). Do not
*  use within ISRs unless interrupt disabling is
*  used instead of locking task switching.
*
*  (Currently, this procedure does not lock tasks
*  so all warnings, relating to ISRs, are irrelevant.
*  This may change, however.)
*
*  If operation fails, this routine will not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*
*  This procedure is not callable from interrupt
*  service routines.
*SEE ALSO:
 */
int
   d_ioctl(
       int fd,       /* file descriptor */
       int function, /* function code */
       int arg       /* arbitrary argument */
      )
{
  int
      ret ;
  char
      *proc_name ;
  proc_name = "d_ioctl" ;
  ret = ioctl(fd,function,arg) ;
  gen_err(TRUE,TRUE,(int)ret,ERROR,"ioctl - ",
      proc_name,SVR_FTL,D_IOCTL_ERR_01,FALSE,0,-1,FALSE) ;
  return (ret) ;
}


/*****************************************************
*NAME
*  d_printf
*TYPE:
*  PROC
*DATE:
*  12-DEC-06
*FUNCTION:
*  Print formatted output to the standard output stream.
*  Wrapper to 'printf'.
*INPUT:
*  SOC_SAND_DIRECT:
*    const char* format -
*       Format control
*    ...                -
*       Optional arguments
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      the number of characters printed, or a negative value if an error occurs.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  d_printf(
    const char* format, ...
  )
{
  va_list args;
  int     i;

  va_start(args, format);
  i = soc_sand_os_printf(format, args);
  va_end(args);

  return i;
}

/*****************************************************
*NAME
*  d_taskLock
*TYPE: PROC
*DATE: 25/APR/2002
*FUNCTION:
*  Disable task rescheduling.
*  This is a private wrap allowing for control over
*  operating system (single location of invocation).
*CALLING SEQUENCE:
*  d_taskLock()
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  If operation fails, this routine may not
*  return to the caller (fatal error).
*SEE ALSO:
 */
STATUS
  d_taskLock(
    void
  )
{
  STATUS
      status ;
  status = OK ;
  if (!is_in_isr())
  {
/* { */
    status = soc_sand_os_task_lock() ;
/* } */
  }
  return (status) ;
}
/*****************************************************
*NAME
*  d_taskUnlock
*TYPE: PROC
*DATE: 25/APR/2002
*FUNCTION:
*  Enable task rescheduling.
*  This is a private wrap allowing for control over
*  operating system (single location of invocation).
*CALLING SEQUENCE:
*  d_taskUnlock()
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  If operation fails, this routine may not
*  return to the caller (fatal error).
*SEE ALSO:
 */
STATUS
  d_taskUnlock(
    void
  )
{
  STATUS
      status ;
  status = OK ;
  if (!is_in_isr())
  {
/* { */
    status = soc_sand_os_task_unlock();
/* } */
  }
  return (status) ;
}

/*****************************************************
*NAME
*  d_clock_gettime
*TYPE: PROC
*DATE: 08/APR/2002
*FUNCTION:
*  get the current time of the clock (POSIX).
*  This is a private wrap allowing for control
*  over operating system (single location of
*  invocation).
*CALLING SEQUENCE:
*  d_clock_gettime(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as clock_gettime. See documentation
*  SOC_SAND_INDIRECT:
*    Same as clock_gettime. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as clock_gettime. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as clock_gettime. See documentation.
*REMARKS:
*  If operation fails, this routine will not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*SEE ALSO:
 */
int
  d_clock_gettime(
    clockid_t       clock_id, /* clock ID (always CLOCK_REALTIME) */
    struct timespec *tp       /* where to store current time */
    )
{
  int
    err,
    ret ;
  char
      *proc_name ;
  proc_name = "d_clock_gettime" ;
  err = clock_gettime(clock_id,tp) ;
  gen_err(TRUE,TRUE,(int)err,ERROR,"clock_gettime - ",
      proc_name,SVR_ERR,D_CLOCK_GETTIME_ERR_01,FALSE,0,-1,FALSE) ;
  ret = err ;
  return (ret) ;
}

/*****************************************************
*NAME
*  d_taskDelayMicroSec
*TYPE: PROC
*DATE: 28/FEB/2002
*FUNCTION:
*  Delays the calling task as specified.
*  Case interrupts are not activate (no task switching),
*  d_taskDelay is not used. Rather, private counting function is used.
*  This is a private wrap allowing for
*  control over operating system (single location
*  of invocation).
*CALLING SEQUENCE:
*  d_taskDelayMicroSec()
*INPUT:
*  SOC_SAND_DIRECT:
*    const long int delay_in_microsec
*      Time in micro-sec.
*      This number will be rounded UP, to the next number
*      in system ticks. Hence, 1 micro sleep request,
*      will result in 16,666 micro sleep.
*  SOC_SAND_INDIRECT:
*    .
*OUTPUT:
*  SOC_SAND_DIRECT:
*    .
*  SOC_SAND_INDIRECT:
*    .
*REMARKS:
*  If operation fails, this routine may not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*SEE ALSO:
 */
/* { */
STATUS
  d_taskDelayMicroSec(
    const long int delay_in_microsec
  )
{
  int
    err = FALSE;
  uint32 premature;

  err = soc_sand_os_nano_sleep(delay_in_microsec * 1000, &premature);

  return err;
}
/* } */

/*****************************************************
*NAME
*  d_taskDelete
*TYPE: PROC
*DATE: 17/JAN/2002
*FUNCTION:
*  Delete a task. This is a private wrap allowing for
*  control over operating system (single location
*  of invocation).
*CALLING SEQUENCE:
*  d_taskDelete(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as taskDelete. See documentation
*  SOC_SAND_INDIRECT:
*    Same as taskDelete. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as taskDelete. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as taskDelete. See documentation.
*REMARKS:
*  Private controls are only valid within task context
*  (since taskLock and taskUnlock are used). Do not
*  use within ISRs unless interrupt disabling is
*  used instead of locking task switching.
*
*  If operation fails, this routine will not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*
*  This procedure is not callable from interrupt
*  service routines.
*
*  It is assumed that task was created using
*  d_taskSpawn! Otherwise, a fatal error will occur.
*SEE ALSO:
 */
STATUS
   d_taskDelete(
        sal_thread_t tid /* task ID of task to delete */
         )
{
  STATUS
      ret,
      status ;
  char
      *proc_name ;
  sal_thread_t
      private_task_id ;
  proc_name = "d_taskDelete" ;
  status = soc_sand_os_task_lock() ;
  gen_err(TRUE,TRUE,(int)status,ERROR,"taskLock - ",
      proc_name,SVR_FTL,D_TASKDELETE_ERR_01,FALSE,0,-1,FALSE) ;
  ret = soc_sand_os_task_delete(tid) ;
  gen_err(TRUE,TRUE,(int)ret,ERROR,"taskDelete - ",
      proc_name,SVR_FTL,D_TASKDELETE_ERR_02,FALSE,0,-1,FALSE) ;
  private_task_id = soc_sand_os_get_current_tid() ;
  gen_err(TRUE,TRUE,(private_task_id == (sal_thread_t)(-1)) ? -1 : 0,-1,
          "get_private_task_id - ",
          proc_name,SVR_WRN,D_TASKDELETE_ERR_03,FALSE,0,-1,FALSE) ;
  status = soc_sand_os_task_unlock() ;
  gen_err(TRUE,TRUE,(int)status,ERROR,"taskUnlock - ",
      proc_name,SVR_FTL,D_TASKDELETE_ERR_04,FALSE,0,-1,FALSE) ;
  return (ret) ;
}

#if !(defined(LINUX) || defined(UNIX))
/* { */
/*****************************************************
*NAME
*  d_ioGlobalStdSet
*TYPE: PROC
*DATE: 12/APR/2002
*FUNCTION:
*  Set global IO for the whole system.
*  This is a private wrap allowing for control over
*  operating system (single location of invocation).
*CALLING SEQUENCE:
*  d_ioGlobalStdSet(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as taskDelete. See documentation
*  SOC_SAND_INDIRECT:
*    Same as taskDelete. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as taskDelete. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as taskDelete. See documentation.
*REMARKS:
*  If stdFd is not 0, 1, or 2, this routine has n
*  effect.
*SEE ALSO:
 */
void
  d_ioGlobalStdSet(
    int stdFd, /* std input (0), output (1), or error (2) */
    int newFd  /* new underlying file descriptor */
  )
{
  char
      *proc_name ;
  proc_name = "d_ioGlobalStdSet" ;
  ioGlobalStdSet(stdFd,newFd) ;
  return ;
}

/*****************************************************
*NAME
*  d_read
*TYPE: PROC
*DATE: 24/FEB/2005
*FUNCTION:
*  Read bytes from a file or a device
*CALLING SEQUENCE:
*  d_read(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  d_read(
    int fd,
    char *buffer,
    size_t maxbytes
  )
{
  int
    ret;
  char
      *proc_name ;
  proc_name = "d_read" ;

  ret = read(fd, buffer, maxbytes);
  return ret;
}
/*****************************************************
*NAME
*  d_open
*TYPE: PROC
*DATE: 24/FEB/2005
*FUNCTION:
*  Open a file
*CALLING SEQUENCE:
*  d_open(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  d_open(
      const char *name,
      int flags,
      int mode
  )
{
  int
    ret;
  char
      *proc_name ;
  proc_name = "d_open" ;

  ret = open(name, flags, mode);
  return ret;
}

/*****************************************************
*NAME
*  d_reboot
*TYPE: PROC
*DATE: 19/MAY/2003
*FUNCTION:
*  Dreset network devices and transfer control to boot ROMs.
*  This may be viewed as a private wrap allowing for
*  control over operating system.
*CALLING SEQUENCE:
*  d_reboot(hard_reboot,start_type)
*INPUT:
*  SOC_SAND_DIRECT:
*    D_REBOOT_TYPE reboot_type -
*       D_HARD_REBOOT: Doing hardware reboot (hardwrae watchdog mechanism)
*       D_SOFT_REBOOT: Software reboot -- call to 'reboot()'
*    int start_type -
*       If soft-reboot, how the boot ROMS will reboot (refer to help)
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    void -
*  SOC_SAND_INDIRECT:
*    Restarted device
*REMARKS:
*
*SEE ALSO:
 */
void
  d_reboot(
    D_REBOOT_TYPE reboot_type,
    int           start_type /* If D_SOFT_REBOOT than how the boot ROMS will reboot */
  )
{
  return;
}



/*****************************************************
*NAME
*  d_force_task_unsafe
*TYPE: PROC
*DATE: 11/APR/2002
*FUNCTION:
*  Force indicated task to not be protected from
*  deletion.
*  Implementation is specific to OS.
*CALLING SEQUENCE:
*  d_force_task_unsafe(task_id)
*INPUT:
*  SOC_SAND_DIRECT:
*    int -
*      Id of task to force (OS ID).
*  SOC_SAND_INDIRECT:
*    OS system.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Task made unsafe.
*REMARKS:
*  This is not strictly an operation allowed by
*  OS.
*SEE ALSO:
 */
void
  d_force_task_unsafe(
    int task_id
  )
{
#ifndef SIM_ON_WINDOWS
/* { */
  WIND_TCB
    *wind_tcb ;
  wind_tcb = taskTcb(task_id) ;
/* } */
#endif
  return ;
}

/*****************************************************
*NAME
*  d_is_this_task_owner_of_sem
*TYPE: PROC
*DATE: 03/SEP/2003
*FUNCTION:
*  Indicate whether this task owns specified semaphore.
*  Implementation is specific to OS.
*CALLING SEQUENCE:
*  d_is_this_task_owner_of_sem()
*INPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    OS system.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      if non-zero than task is in 'owner' of
*      specified semaphore.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  d_is_this_task_owner_of_sem(
    SEM_ID semaphore
  )
{
  int
    ret ;
/* { */
  ret = FALSE ;
/* } */

  return (ret) ;
}

/*****************************************************
*NAME
*  d_intLock
*TYPE: PROC
*DATE: 06/MAY/2002
*FUNCTION:
*  Lock out interrupts.
*  This is a private wrap allowing for control over
*  operating system (single location of invocation).
*CALLING SEQUENCE:
*  d_intLock()
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  d_intLock(
    void
  )
{
  int
      ret ;
#ifndef SIM_ON_WINDOWS
/* { */
  ret = intLock() ;
/* } */
#else
/* { */
  ret = 0 ;
/* } */
#endif
  return (ret) ;
}
/*****************************************************
*NAME
*  d_intUnlock
*TYPE: PROC
*DATE: 06/MAY/2002
*FUNCTION:
*  Cancel interrupt locks.
*  This is a private wrap allowing for control over
*  operating system (single location of invocation).
*CALLING SEQUENCE:
*  d_intUnlock()
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  If operation fails, this routine may not
*  return to the caller (fatal error).
*SEE ALSO:
 */
void
  d_intUnlock(
    int lockKey /* lock-out key returned by preceding d_intLock() */
  )
{
#ifndef SIM_ON_WINDOWS
/* { */
  intUnlock(lockKey) ;
/* } */
#else
/* { */
/* } */
#endif
  return ;
}
/*****************************************************
*NAME
*  d_clock_gettime
*TYPE: PROC
*DATE: 08/APR/2002
*FUNCTION:
*  get the current time of the clock (POSIX).
*  This is a private wrap allowing for control
*  over operating system (single location of
*  invocation).
*CALLING SEQUENCE:
*  d_clock_gettime(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as clock_gettime. See documentation
*  SOC_SAND_INDIRECT:
*    Same as clock_gettime. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as clock_gettime. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as clock_gettime. See documentation.
*REMARKS:
*  If operation fails, this routine will not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*SEE ALSO:
 */
#if !DUNE_BCM
int
  d_clock_gettime(
    clockid_t       clock_id, /* clock ID (always CLOCK_REALTIME) */
    struct timespec *tp       /* where to store current time */
    )
{
  int
    err,
    ret ;
  char
      *proc_name ;
  proc_name = "d_clock_gettime" ;
  err = clock_gettime(clock_id,tp) ;
  gen_err(TRUE,TRUE,(int)err,ERROR,"clock_gettime - ",
      proc_name,SVR_ERR,D_CLOCK_GETTIME_ERR_01,FALSE,0,-1,FALSE) ;
  ret = err ;
  return (ret) ;
}
#endif
/*****************************************************
*NAME
*  d_clock
*TYPE: PROC
*DATE: 23/APR/2002
*FUNCTION:
*  Determine the processor time in use (ANSI).
*  This is a private wrap allowing for control
*  over operating system (single location of
*  invocation).
*CALLING SEQUENCE:
*  d_clock(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as clock_gettime. See documentation
*  SOC_SAND_INDIRECT:
*    Same as clock_gettime. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as clock_gettime. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as clock_gettime. See documentation.
*REMARKS:
*  If operation fails, this routine will not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*SEE ALSO:
 */
int
  d_clock(
    void
  )
{
  int
    err,
    ret ;
  char
      *proc_name ;
  proc_name = "d_clock" ;
  err = clock() ;
  gen_err(TRUE,TRUE,(int)err,-1,"clock_gettime - ",
      proc_name,SVR_FTL,D_CLOCK_ERR_01,FALSE,0,-1,FALSE) ;
  ret = err ;
  return (ret) ;
}
/*****************************************************
*NAME
*  d_tickGet
*TYPE: PROC
*DATE: 23/APR/2002
*FUNCTION:
*  Get the value of the kernel's tick counter.
*  This is a private wrap allowing for control
*  over operating system (single location of
*  invocation).
*CALLING SEQUENCE:
*  d_tickGet(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as clock_gettime. See documentation
*  SOC_SAND_INDIRECT:
*    Same as clock_gettime. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as clock_gettime. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as clock_gettime. See documentation.
*REMARKS:
*  This routine returns the current value of the tick
*  counter. This value is set to zero at startup,
*  incremented by tickAnnounce( ), and can be changed
*  using tickSet( ).
*SEE ALSO:
 */
unsigned
  long
    d_tickGet(
      void
    )
{
  unsigned
    long
      ret ;
  char
      *proc_name ;
  proc_name = "d_tickGet" ;
  ret = tickGet() ;
  return (ret) ;
}

/*
 * When interrupts are not enabled need to call this function.
 */
#if !DUNE_BCM
STATIC int
  utils_d_counter_delay(
    const long int delay_in_microsec
  )
{
  int
    err;
  unsigned long
    timer_before,
    timer_after,
    timer_diff_microsec;
  long int
    remain_time_in_microsec;

  err = FALSE;
  timer_before = read_timer_3_4();
  remain_time_in_microsec = delay_in_microsec;

  while(remain_time_in_microsec > 0)
  {
    timer_after = read_timer_3_4();

    timer_diff_microsec = timer_3_4_to_micro(timer_after - timer_before);
    if (timer_diff_microsec > 0)
    {
      timer_before = read_timer_3_4();
      remain_time_in_microsec -= timer_diff_microsec;
    }
  }

  return err;
}
#endif
/*
 * Check if the interrupts are enabled.
 */
uint8
 utils_d_is_interrupt_enable(
    void
  )
{
  uint8
    answer;
  int
    flags1,
    flags2;

  flags1 = intLock();
  flags2 = intLock();
  intUnlock(flags2);
  intUnlock(flags1);

  if ( (flags1^flags2) !=0 )
  {
    /*
     * difference between 2 flags.
     * Interrupt are enabled
     */
    answer = TRUE;
  }
  else
  {
    /*
     * NO difference between 2 flags.
     * Interrupt are disabled.
     */
    answer = FALSE;
  }

  return answer;
}

/*****************************************************
*NAME
*  d_taskDelaySec
*TYPE: PROC
*DATE: 28/FEB/2002
*FUNCTION:
*  Delays the calling task as specified in SECONDS.
*  Case interrupts are not activate (no task switching),
*  d_taskDelay is not used. Rather, private counting function is used.
*  This is a private wrap allowing for
*  control over operating system (single location
*  of invocation).
*INPUT:
*  SOC_SAND_DIRECT:
*    const long int delay_in_sec
*      Time in sec.
*  SOC_SAND_INDIRECT:
*    .
*OUTPUT:
*  SOC_SAND_DIRECT:
*    .
*  SOC_SAND_INDIRECT:
*    .
*REMARKS:
*  If operation fails, this routine may not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*SEE ALSO:
 */
STATUS
  d_taskDelaySec(
    const long int delay_in_sec
  )
{
  int
    err;
  long int
    sec_i;

  err = FALSE;
  if (delay_in_sec == 0)
  {
    d_taskDelay(0);
    goto exit;
  }

  for (sec_i = 0; sec_i<delay_in_sec; ++sec_i )
  {
    err = d_taskDelayMicroSec(1000000);
    if (err)
    {
      goto exit;
    }
  }

exit:
  return err;
}

/*****************************************************
*NAME
*  d_taskRestart
*TYPE: PROC
*DATE: 17/JAN/2002
*FUNCTION:
*  Restarts a task. This is a private wrap allowing for
*  control over operating system (single location
*  of invocation).
*CALLING SEQUENCE:
*  d_taskRestart(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as taskRestart. See documentation
*  SOC_SAND_INDIRECT:
*    Same as taskRestart. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as taskRestart. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as taskRestart. See documentation.
*REMARKS:
*  Private controls are only valid within task context
*  (since taskLock and taskUnlock are used). Do not
*  use within ISRs unless interrupt disabling is
*  used instead of locking task switching.
*
*  If operation fails, this routine will not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*
*  This procedure is not callable from interrupt
*  service routines.
*
*SEE ALSO:
 */
STATUS
   d_taskRestart(
     sal_thread_t tid /* task ID of task to restart */
     )
{
  STATUS
      ret = OK,
      status ;
  char
      *proc_name ;
  int os_tid;
  proc_name = "d_taskRestart" ;

#ifndef SIM_ON_WINDOWS
  os_tid = PTR_TO_INT(tid);
  if ((!os_tid) || (os_tid == d_taskIdSelf())) {
    ret = ERROR;
    gen_err(TRUE,TRUE,(int)ret,ERROR,"Task tried to restart itself",
        proc_name,SVR_ERR,D_TASKRESTART_ERR_01,FALSE,0,-1,FALSE) ;
    goto exit;
  }
  status = d_taskLock() ;
  gen_err(TRUE,TRUE,(int)status,ERROR,"taskLock - ",
      proc_name,SVR_FTL,D_TASKRESTART_ERR_02,FALSE,0,-1,FALSE) ;
  ret = taskRestart(os_tid) ;
  gen_err(TRUE,TRUE,(int)ret,ERROR,"taskRestart - ",
      proc_name,SVR_FTL,D_TASKRESTART_ERR_03,FALSE,0,-1,FALSE) ;
  status = d_taskUnlock() ;
  gen_err(TRUE,TRUE,(int)status,ERROR,"taskUnlock - ",
      proc_name,SVR_FTL,D_TASKRESTART_ERR_04,FALSE,0,-1,FALSE) ;
#endif

exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  d_taskResume
*TYPE: PROC
*DATE: 17/JAN/2002
*FUNCTION:
*  Eesumes a task. This is a private wrap allowing for
*  control over operating system (single location
*  of invocation).
*CALLING SEQUENCE:
*  d_taskResume(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as taskResume. See documentation
*  SOC_SAND_INDIRECT:
*    Same as taskResume. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as taskResume. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as taskResume. See documentation.
*REMARKS:
*  Private controls are only valid within task context
*  (since taskLock and taskUnlock are used). Do not
*  use within ISRs unless interrupt disabling is
*  used instead of locking task switching.
*
*  If operation fails, this routine will not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*
*  This procedure is not callable from interrupt
*  service routines.
*
*SEE ALSO:
 */
STATUS
   d_taskResume(
     sal_thread_t tid /* task ID of task to resume */
     )
{
  STATUS
      ret = OK,
      status ;
  char
      *proc_name ;
  proc_name = "d_taskResume" ;

#ifndef SIM_ON_WINDOWS
  status = d_taskLock() ;
  gen_err(TRUE,TRUE,(int)status,ERROR,"taskLock - ",
      proc_name,SVR_FTL,D_TASKRESUME_ERR_01,FALSE,0,-1,FALSE) ;
  ret = taskResume(PTR_TO_INT(tid)) ;
  gen_err(TRUE,TRUE,(int)ret,ERROR,"taskResume - ",
      proc_name,SVR_FTL,D_TASKRESUME_ERR_02,FALSE,0,-1,FALSE) ;
  status = d_taskUnlock() ;
  gen_err(TRUE,TRUE,(int)status,ERROR,"taskUnlock - ",
      proc_name,SVR_FTL,D_TASKRESUME_ERR_04,FALSE,0,-1,FALSE) ;
#endif

  return (ret) ;
}
/*****************************************************
*NAME
*  d_taskSuspend
*TYPE: PROC
*DATE: 17/JAN/2002
*FUNCTION:
*  Suspends a task. This is a private wrap allowing for
*  control over operating system (single location
*  of invocation).
*CALLING SEQUENCE:
*  d_taskSuspend(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    Same as taskSuspend. See documentation
*  SOC_SAND_INDIRECT:
*    Same as taskSuspend. See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Same as taskSuspend. See documentation.
*  SOC_SAND_INDIRECT:
*    Same as taskSuspend. See documentation.
*REMARKS:
*  Private controls are only valid within task context
*  (since taskLock and taskUnlock are used). Do not
*  use within ISRs unless interrupt disabling is
*  used instead of locking task switching.
*
*  If operation fails, this routine will not
*  return to the caller (fatal error). Note that
*  all error handling routines are carried out
*  under taskLock!
*
*  This procedure is not callable from interrupt
*  service routines.
*
*SEE ALSO:
 */
STATUS
   d_taskSuspend(
     sal_thread_t tid /* task ID of task to suspend */
     )
{
  STATUS
      ret = OK,
      status ;
  char
      *proc_name ;
  int os_tid;
  proc_name = "d_taskSuspend" ;

#ifndef SIM_ON_WINDOWS
  os_tid = PTR_TO_INT(tid);
  if ((!os_tid) || (os_tid == d_taskIdSelf())) {
    ret = ERROR;
    gen_err(TRUE,TRUE,(int)ret,ERROR,"Task tried to suspend itself",
        proc_name,SVR_ERR,D_TASKSUSPEND_ERR_01,FALSE,0,-1,FALSE) ;
    goto exit;
  }
  status = d_taskLock() ;
  gen_err(TRUE,TRUE,(int)status,ERROR,"taskLock - ",
      proc_name,SVR_FTL,D_TASKSUSPEND_ERR_02,FALSE,0,-1,FALSE) ;
  ret = taskSuspend(os_tid) ;
  gen_err(TRUE,TRUE,(int)ret,ERROR,"taskSuspend - ",
      proc_name,SVR_FTL,D_TASKSUSPEND_ERR_03,FALSE,0,-1,FALSE) ;
  status = d_taskUnlock() ;
  gen_err(TRUE,TRUE,(int)status,ERROR,"taskUnlock - ",
      proc_name,SVR_FTL,D_TASKSUSPEND_ERR_04,FALSE,0,-1,FALSE) ;
#endif

exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  d_taskIdVerify
*TYPE: PROC
*DATE: 24/FEB/2005
*FUNCTION:
*  Verifies the existence of a task
*CALLING SEQUENCE:
*  d_taskIdVerify(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*
*SEE ALSO:
 */
STATUS 	
  d_taskIdVerify (
    sal_thread_t tid
    )
{
  STATUS
      ret = OK;
  char
      *proc_name ;
  proc_name = "d_taskIdVerify" ;

#ifndef SIM_ON_WINDOWS
  ret = taskIdVerify(PTR_TO_INT(tid)) ;
#endif
  return (ret) ;
}

/* OS sockLib wrappers */

/*****************************************************
*NAME
*  d_socket
*TYPE: PROC
*DATE: 24/FEB/2005
*FUNCTION:
*  Create a new socket
*CALLING SEQUENCE:
*  d_socket(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  d_socket(
    int domain,
    int type,
    int protocol
  )
{
  int
    ret;
  char
      *proc_name ;
  proc_name = "d_socket" ;

  ret = socket(domain, type, protocol);
  return ret;
}

/*****************************************************
*NAME
*  d_bind
*TYPE: PROC
*DATE: 24/FEB/2005
*FUNCTION:
*  Binds a name or an address to a socket
*CALLING SEQUENCE:
*  d_bind(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  d_bind(
    int s,
    struct sockaddr *name,
    int namelen
  )
{
  int
    ret;
  char
      *proc_name ;
  proc_name = "d_bind" ;

  ret = bind(s, name, namelen);
  return ret;
}

/*****************************************************
*NAME
*  d_sendto
*TYPE: PROC
*DATE: 24/FEB/2005
*FUNCTION:
*  Sends a message to a socket.
*CALLING SEQUENCE:
*  d_sendto(OS parameters)
*INPUT:
*  SOC_SAND_DIRECT:
*    See documentation
*  SOC_SAND_INDIRECT:
*    See documentation.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    See documentation.
*  SOC_SAND_INDIRECT:
*    See documentation.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  d_sendto(
    int s,
    caddr_t buf,
    int bufLen,
    int flags,
    struct sockaddr *to,
    int tolen
  )
{
  int
    ret;
  char
      *proc_name ;
  proc_name = "d_sendto" ;

  ret = sendto(s, buf, bufLen, flags, to, tolen);
  return ret;
}

/* syslog wrapper */

/*****************************************************
*NAME
*  d_syslog
*TYPE: PROC
*DATE: 24/FEB/2005
*FUNCTION:
*  Send a messages to the (remote) system logger
*CALLING SEQUENCE:
*  d_syslog(pri, message)
*INPUT:
*  SOC_SAND_DIRECT:
*    pri -
*      int. Severity of this event, in case of error.
*      Can be:
*        SVR_MSG (0) - Just an info message
*        SVR_WRN (1) - Warnining.
*        SVR_ERR (2) - Error.
*        SVR_FTL (3) - Fatal Error (unrecoverable).
*    message -
*      char *. Private message to print.
*  SOC_SAND_INDIRECT:
*    String in parameter 'message'.
*    downloading_host_addr NVRAM param which specifies
*     the host that runs a syslog server.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error has occurred and
*      remote logging could not be carried out.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  NOTICE - d_syslog() is currently NOT reentrant,
*  and therefore can only be called from within
*  a critical section !!!
*  Ths syslog server address is currently read
*  each time a call to syslog() is made.
*SEE ALSO:
 */
int
  d_syslog(
    int pri,
    char *message
  )
{
  int
    ret;
  char
      *proc_name ;
  proc_name = "d_syslog" ;

#if !DUNE_BCM
  ret = syslog(pri, message);
#else
  assert(1);
  ret=0;
#endif
  return ret;
}

unsigned long
  d_inet_addr(
    const char* cp
  )
{
  unsigned long
    in_addr = 0;

  in_addr = inet_addr((char *)cp);

  if(in_addr == INADDR_NONE)
  {
    return 0xFFFFFFFF;
  }

#ifdef SIM_ON_WINDOWS
  in_addr = htonl(in_addr);
#endif

  return in_addr;
}

 /*} LINUX */
#endif

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif  /* MIN */

/*****************************************************
*NAME
*  d_strnicmp
*TYPE: PROC
*DATE: 10/May/2007
*FUNCTION:
*  Compares two strings and returns a numeric result indicating
*  whether the first string is lexically less than, greater than,
*  or equal to the second string.
*  The comparison will carry on for not more than length characters of the shorter string.
*  Comparing is case-insensitive.
*INPUT:
*  SOC_SAND_DIRECT:
*    str1 -
*      char *. 1st string
*    str2 -
*      char *. 2nd string
*    n -
*      int. The maximum length of the comparison.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error has occurred and
*      remote logging could not be carried out.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
 */
int
  d_strnicmp(
    const char *str1,
    const char *str2,
    size_t     n
   )
{
  int
    ret = 0;
  int
    ndx;
  int
    delta = 'a' - 'A';
  int
    n_orig = n;
  char
    c1, c2;

  n = MIN(n, MIN(strlen(str1), strlen(str2)));

  for (ndx=0; ndx<n; ndx++)
  {
    c1 = str1[ndx];
    c2 = str2[ndx];

    /* to lower c1*/
    if (c1>='a' && c1<='z')
    {
      c1 = c1 - delta;
    }

    /* to lower c2*/
    if (c2>='a' && c2<='z')
    {
      c2 = c2 - delta;
    }

    /* if different */
    if (c1!=c2)
    {
      ret = (c1 > c2) ? ( 1 ) : ( -1 );
      break;
    }
  }

  /*
   * if size was trunked and prefix is the same
   * then larger string is bigger
   */
  if (ret==0 && n_orig!=n)
  {
    ret = (strlen(str1)>strlen(str2)) ? (1) : (-1);
  }

  return (ret) ;
}

/*****************************************************
*NAME
*  d_stricmp
*TYPE: PROC
*DATE: 10/May/2007
*FUNCTION:
*  Compares two strings and returns a numeric result indicating
*  whether the first string is lexically less than, greater than,
*  or equal to the second string.
*  The comparison will carry on for not more than length characters of the shorter string.
*  Comparing is case-insensitive.
*INPUT:
*  SOC_SAND_DIRECT:
*    str1 -
*      char *. 1st string
*    str2 -
*      char *. 2nd string
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error has occurred and
*      remote logging could not be carried out.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
 */
int
  d_stricmp(
    const char *str1,
    const char *str2
   )
{
  int
    ret = 0;
  int
    n = strlen(str1);

  ret = d_strnicmp(str1, str2, n);

  return (ret) ;
}

/*
 * }
 */



