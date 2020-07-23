/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * General include file for reference design.
 */

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_error_defs.h>

#include <appl/diag/dpp/dffs_driver.h>
#include <appl/diag/dpp/bsp_drv_error_defs.h>


#ifndef SAND_LOW_LEVEL_SIMULATION

#if !DUNE_BCM
#include "ref_sys.h"
/*
 * INCLUDE FILES:
 * {
 */
#include "usrLib.h"
#include "config.h"
#include "bsp_drv_flash28f_dev.h"
#include "bsp_drv_flash28f.h"

#include "usrApp.h"

/*#include "flash28.h"*/
#include "flash29.h"

#endif
/*
 * }
 */

#if !DUNE_BCM

/*
 * Utilities for semaphore protected Flash memory access.
 * {
 */
/*
 * Semaphore handling for flash read/write protection.
 * Semaphore is taken every time Flash is accessed and
 * release when access has ended.
 * Use binary semaphore rather than 'mutual exclusion' semaphore since
 * we DO NOT want the 'recursive' option.
 *
 * For all utilities: Returning a non-zero value indicates error.
 *
 * Interface to direct Flash access routines is preserved. See
 * details in flashmem.c
 */
static
  SEM_ID
    Flash_sem = (SEM_ID)0 ;

static
  unsigned int
    Flash_sem_is_initilized = 0;
int
  init_flash_sem(
    void
  )
{
  int
    ret ;
  /*
   * Initialize semaphore for Flash memory read/write protection.
   * Use binary semaphore rather than 'mutual exclusion' semaphore since
   * we DO NOT want the 'recursive' option.
   */
  ret = 0 ;
  Flash_sem = semBCreate(SEM_Q_PRIORITY,SEM_FULL) ;
  if (Flash_sem == NULL)
  {
    ret = 1 ;
  }
  else
  {
    Flash_sem_is_initilized = 1;
  }
  return (ret) ;
}
int
  take_flash_sem(
    void
  )
{
  int
    ret ;
  STATUS
    status ;
  ret = 0 ;

  if (!Flash_sem_is_initilized)
  {
    init_flash_sem();
  }
  /*
   * First make sure calling task will not be deleted
   * while this operation is on (Flash being read/written).
   */
  taskSafe() ;
  if (Flash_sem == (SEM_ID)0)
  {
    ret = 2 ;
  }
  else
  {
    status = semTake(Flash_sem,WAIT_FOREVER) ;
    if (status == ERROR)
    {
      ret = 1 ;
    }
  }
  return (ret) ;
}
int
  give_back_flash_sem(
    void
  )
{
  int
    ret ;
  STATUS
    status ;
  ret = 0 ;

  if (!Flash_sem_is_initilized)
  {
    init_flash_sem();
  }

  if (Flash_sem == (SEM_ID)0)
  {
    ret = 2 ;
  }
  else
  {
    status = semGive(Flash_sem) ;
    if (status == ERROR)
    {
      ret = 1 ;
    }
  }
  taskUnsafe() ;
  return (ret) ;
}

#endif /* !DUNE_BCM */


STATUS
  bspDrvFlash28fGet_protected_info(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          char          *string,    /* where to copy flash memory      */
          int           strLen,     /* maximum number of bytes to copy */
          int           offset      /* byte offset into flash memory   */
  )
{
  STATUS
    ret ;
  int
    err ;
  ret = OK ;
  err = take_flash_sem() ;
  if (err)
  {
    ret = ERROR ;
    goto exit ;
  }

  ret = bspDrvFlash28fGet(flash_info,string,strLen,offset) ;
  err = give_back_flash_sem() ;
  if (err || ret)
  {
    ret = ERROR ;
  }
exit:
  return (ret) ;
}


STATUS
  bspDrvFlash28fKick_protected_info(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          int do_poll_indication,     /* Flag. Non-Zero: print progress indication.*/
          int sector                  /* Number of sector to kick (starting from */
                                      /* zero. If negative then erase the whole  */
                                      /* memory space. If number of sector is    */
                                      /* larger than the maximal for that flash  */
                                      /* type, last sector is erased.            */
  )
{
  STATUS
    ret ;
  int
    err ;
  ret = OK ;

  err = take_flash_sem() ;
  if (err)
  {
    ret = ERROR ;
    goto exit ;
  }
  ret = bspDrvFlash28fKick(flash_info,do_poll_indication,sector) ;
  err = give_back_flash_sem() ;
  if (err || ret)
  {
    ret = ERROR ;
  }
exit:
  return (ret) ;
}

STATUS
  bspDrvFlash28fErase_protected_info(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          int do_poll_indication,     /* Flag. Non-Zero print progress indication*/
                                      /* in percentage.                          */
          int sector                  /* Number of sector to erase (starting from*/
                                      /* zero. If negative then erase the whole  */
                                      /* memory space. If number of sector is    */
                                      /* larger than the maximal for that flash  */
                                      /* type, last sector is erased.            */
  )
{
  STATUS
    ret ;
  int
    err ;
  ret = OK ;

  err = take_flash_sem() ;
  if (err)
  {
    ret = ERROR ;
    goto exit ;
  }
  ret = bspDrvFlash28fErase(flash_info, do_poll_indication, sector) ;

  err = give_back_flash_sem() ;
  if (err || ret)
  {
    ret = ERROR ;
  }
exit:
  return (ret) ;
}


STATUS
  bspDrvFlash28fWrite_protected_info(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          FLASH_28F_DEF *pFB,        /* string to be copied; use <value> if NULL */
          int       size,        /* size to program in bytes */
          int       offset,      /* byte offset into flash memory */
          FLASH_28F_DEF value,       /* value to program */
          int (*poll_indication_func)(unsigned long current_work_load)    /* Function to call when polling the Flash.*/
                                      /* If NULL then call no function.          */
  )
{
  STATUS
    ret ;
  int
    err ;
  ret = OK ;
  err = take_flash_sem() ;
  if (err)
  {
    ret = ERROR ;
    goto exit ;
  }

  ret = bspDrvFlash28fWrite(flash_info, pFB, size, offset, value, poll_indication_func) ;
  err = give_back_flash_sem() ;
  if (err || ret)
  {
    ret = ERROR ;
  }
exit:
  return (ret) ;
}


/*
 * }
 */

#endif /*#ifndef SAND_LOW_LEVEL_SIMULATION*/
