/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef SAND_LOW_LEVEL_SIMULATION

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_error_defs.h>

#include <appl/diag/dpp/dffs_driver.h>
#include <appl/diag/dpp/bsp_drv_error_defs.h>

#include <appl/diag/dpp/bsp_drv_flash28f.h>

#if !DUNE_BCM

#include "bsp_drv_flash28f.h"
#include "bsp_drv_flash28f_dev.h"

#include "config.h"
#include "flash29.h"
#include "flash29.h"

#include "usrApp.h"
#include "utils_defx.h"

#endif

/* defines */

/* Establish default values for DELAY parameters */


#ifndef  BSP_DRV_FLASH28_DELAY_SHIFT
#define  BSP_DRV_FLASH28_DELAY_SHIFT 0
#endif /*BSP_DRV_FLASH28_DELAY_SHIFT*/

#ifndef  BSP_DRV_FLASH28_DELAY_ADJ
#define  BSP_DRV_FLASH28_DELAY_ADJ 0
#endif  /* BSP_DRV_FLASH28_DELAY_ADJ */

#ifndef  BSP_DRV_FLASH28_DELAY_INCR
#define  BSP_DRV_FLASH28_DELAY_INCR 1
#endif  /* BSP_DRV_FLASH28_DELAY_INCR */

/* Names of routines, or null values */

#ifdef  BSP_DRV_FLASH28_WRITE
#   define BSP_DRV_FLASH28_WRITE_ENABLE_RTN()  bspDrvFlash28fWriteEnable ()
#   define BSP_DRV_FLASH28_WRITE_DISABLE_RTN() bspDrvFlash28fWriteDisable ()
#else
#   define BSP_DRV_FLASH28_WRITE_ENABLE_RTN()
#   define BSP_DRV_FLASH28_WRITE_DISABLE_RTN()
#endif  /* BSP_DRV_FLASH28_WRITE */

#ifdef  BSP_DRV_FLASH28_BOARD_DELAY
#define BSP_DRV_FLASH28_BOARD_DELAY_RTN()  bspDrvFlash28fBoardDelay ()
#else
#define BSP_DRV_FLASH28_BOARD_DELAY_RTN()
#endif  /* BSP_DRV_FLASH28_BOARD_DELAY */

#ifdef BSP_DRV_FLASH28_TYPE
#define FLASH_MEM_TYPE    BSP_DRV_FLASH28_TYPE
#else
#define FLASH_MEM_TYPE    0
#endif  /* BSP_DRV_FLASH28_TYPE */

/* Operation status bits for Flash 29Fxxx devices */

#define Q7(ix)    ((ix & 0x80) >> 7)  /* DQ7 bit */
#define Q5(ix)    ((ix & 0x20) >> 5)  /* DQ5 bit */

/* globals */


IMPORT  void bspDrvFlash28fWriteEnable (void);
IMPORT  void bspDrvFlash28fWriteDisable (void);
IMPORT  void bspDrvFlash28fBoardDelay (void);

int  flashDelayShift  = BSP_DRV_FLASH28_DELAY_SHIFT;
int  flashDelayAdj  = BSP_DRV_FLASH28_DELAY_ADJ;
int  flashDelayIncr  = BSP_DRV_FLASH28_DELAY_INCR;

/* forward declarations */

#ifdef  __STDC__

void  bspDrvFlash28fDelay (int delayCount);
STATUS  bspDrvFlash28fDataPoll (
            volatile FLASH_28F_DEF *pFA,
            FLASH_28F_DEF value,
            int (*poll_indication)(int reset_count)) ;
UINT8  bspDrvFlash28fTypeGet_28 (void);
UINT8  bspDrvFlash28fTypeGet_29 (void);

#else  /* __STDC__ */

void  bspDrvFlash28fDelay ();
STATUS  bspDrvFlash28fDataPoll ();
STATUS  bspDrvFlash28fErase ();
STATUS  bspDrvFlash28fWrite ();
UINT8  bspDrvFlash28fTypeGet_28 ();
UINT8  bspDrvFlash28fTypeGet_29 ();

#endif  /* __STDC__ */


/******************************************************************************
*
* bspDrvFlash28fGet - get the contents of flash memory
*
* This routine copies the contents of flash memory into a specified
* string.
*
* RETURNS: OK, or ERROR if access is outside the flash memory range.
*
* SEE ALSO: bspDrvFlash28fSet()
*
* INTERNAL
* If multiple tasks are calling bspDrvFlash28fSet() and bspDrvFlash28fGet(),
* they should use a semaphore to ensure mutually exclusive access.
*/

unsigned int
  bspDrvFlash28f_sector_jump_factor(
    const BSP_FLASH28F_DEF_INF *flash_info
  )
{
  return flash_info->f_sector_size;
}

STATUS
  bspDrvFlash28fGet(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          char          *string,      /* where to copy flash memory      */
          int           strLen,       /* maximum number of bytes to copy */
          int           offset        /* byte offset into flash memory   */
  )
{
  unsigned long
    flash_size,
    nof_flash_words,
    flash_word_i;


  if (flash_info->f_type == FLASH_28F640)
  {
    flash_size = FLASH_SIZE_8MEG ;
  }
  else
  {
    flash_size = FLASH_SIZE_4MEG ;
  }

  if ((offset < 0) || (strLen < 0) || ((offset + strLen) > flash_size))
  {
      return (ERROR);
  }

  if (flash_info->f_adrs_jump == 1)
  {
    bcopyBytes((char *) (flash_info->f_adrs + offset), string, strLen) ;
  }
  else
  {
    nof_flash_words = (strLen+1) / FLASH_28F_WIDTH;
    for (flash_word_i = 0; flash_word_i<nof_flash_words; ++flash_word_i)
    {
      bcopyBytes(
        (char *) (flash_info->f_adrs + offset + (flash_word_i*sizeof(long))),
        string + (FLASH_28F_WIDTH*flash_word_i),
        FLASH_28F_WIDTH
      ) ;
    }
  }

  return (OK);
}

/******************************************************************************
*
* bspDrvFlash28fDelay - create a delay for a specified number of microseconds
*
* This routine implements a busy wait for a specified number of microseconds.
* The timing loop can be adjusted on a board-dependent basis by
* defining values for the following macros:
* .iP
* BSP_DRV_FLASH28_DELAY_SHIFT
* .iP
* BSP_DRV_FLASH28_DELAY_ADJ
* .iP
* BSP_DRV_FLASH28_DELAY_INCR
* .LP
* The values BSP_DRV_FLASH28_DELAY_SHIFT and BSP_DRV_FLASH28_DELAY_ADJ
* convert microseconds into a board-dependent tick-count.
* This routine can call a user-defined hook, bspDrvFlash28fBoardDelay(),
* which creates a delay for a number of board-dependent ticks as
* specified by BSP_DRV_FLASH28_DELAY_INCR.  To use bspDrvFlash28fBoardDelay(), define
* BSP_DRV_FLASH28_BOARD_DELAY in config.h.
*
* RETURNS: N/A
*
* SEE ALSO: bspDrvFlash28fErase(), bspDrvFlash28fWrite()
*/

void bspDrvFlash28fDelay
    (
    int delayCount  /* number of uSec to delay */
    )
{
    private_delay((unsigned long)delayCount  /* number of uSec to delay */) ;

}
/******************************************************************************
*
* bspDrvFlash28fDataPoll - wait for a flash device operation to complete
*
* This routine polls a specified address on a 29F\f2xxx\f1 flash device
* until the device operation at that location completes or times out.
*
* While a flash operation is in progress, a read on the device
* returns on Q7 (data bit 7) the complement of the previous value of Q7.  Once
* the flash operation has completed, the Q7 bit returns the true data
* of the last write. Subsequent reads return the correct data in Q0-7.
*
* The Q5 bit implements a timeout functionality.  When a currently
* executing flash operation exceeds specified limits, the Q5 bit is set (to 1).
*
* RETURNS: OK, or ERROR if the timeout (!Q5) occurs before the device operation
* completes.
*
* SEE ALSO: bspDrvFlash28fErase(), bspDrvFlash28fWrite()
*/

STATUS bspDrvFlash28fDataPoll
    (
    volatile FLASH_28F_DEF * pFA,  /* programmed address to poll */
    FLASH_28F_DEF value,            /* data programmed to poll address */
    int (*poll_indication)(int reset_count)   /* Function to call when polling the Flash.*/
                                /* If NULL then call no function.          */
    )
{
  STATUS
    retVal ;
  volatile
    UINT8
      *pTest ;
  int
    err,
    ix,      /* byte counter */
    vBit ;   /* programmed value of DQ7 */
  retVal = OK ;
  pTest = (UINT8 *) pFA ;
  err = FALSE ;
  for (ix = (FLASH_28F_WIDTH - 1); (ix >= 0 ) && (retVal == OK); ix--, pTest++)
  {
    vBit = Q7(value >> (ix << 3));
    while (Q7(*pTest) != vBit)
    {
      if (Q5(*pTest) == 1)  /* timeout ? */
      {
        break;
      }
    }
    if (err)
    {
      break ;
    }
    if (Q7(*pTest) != vBit)    /* check Q7 & Q5 race */
    {
      retVal = ERROR;
    }
  }
  if (err)
  {
    retVal = ERROR ;
  }
  return (retVal);
}
/******************************************************************************
*
* bspDrvFlash28fKick - Get Flash memory into a readable state (out of any
*                intermediate/status state.
*
* This routine Changes the satet of flash memory.
*
* Only for 28F320 / 28F640.
*
* RETURNS: OK, or ERROR if operation could not be carreid out.
*/

STATUS
  bspDrvFlash28fKick(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          int do_poll_indication,     /* Flag. Non-Zero: print progress indication.*/
          int sector                  /* Number of sector to kick (starting from */
                                      /* zero. If negative then erase the whole  */
                                      /* memory space. If number of sector is    */
                                      /* larger than the maximal for that flash  */
                                      /* type, last sector is erased.            */
  )
{
  volatile
    FLASH_28F_DEF
      *pFA ;
  STATUS
    retVal ;
  unsigned int
    err,
    num_sectors,
    maximal_sector;

  int (*poll_indication_func)(unsigned long current_work_load);
  /*
   */
  if (do_poll_indication)
  {
    poll_indication_func = load_indication_run;
  }
  else
  {
    poll_indication_func = load_indication_run_silent;
  }

  err = FALSE ;
  retVal = OK ;
  unlock_flash() ;
  switch (flash_info->f_type)
  {
    case (FLASH_28F320):
    case (FLASH_28F640):
    {
      if (flash_info->f_type == FLASH_28F640)
      {
        num_sectors = FLASH_SIZE_8MEG / flash_info->f_sector_size ;
      }
      else
      {
        num_sectors = FLASH_SIZE_4MEG / flash_info->f_sector_size ;
      }
      pFA = FLASH_28F_CAST(flash_info->f_adrs) ;
      if (sector < 0)
      {
        if( do_poll_indication )
        {
          load_indication_start(num_sectors);
        }
        for (sector = 0 ; sector < num_sectors ; sector++)
        {
          BSP_DRV_FLASH28_WRITE_ENABLE_RTN () ;     /* raise Vpp */
          *pFA = FLASH28_CMD_RESET ;
          *pFA = FLASH28_CMD_RESET ;
          *pFA = FLASH28F320_BSP_CMD_LOCK_PREFIX ; /* unlock. Just in case... */
          poll_indication_func(sector);

          do
          {
            *pFA = FLASH28F008_CMD_READ_STATUS ;
          }
          while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
          if (err)
          {
            break ;
          }
          *pFA = FLASH28F320_BSP_CMD_CLEAR_LOCKS ;
          do
          {
            *pFA = FLASH28F008_CMD_READ_STATUS ;
          }
          while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
          if (err)
          {
            break ;
          }
          *pFA = FLASH28_CMD_RESET ;
          *pFA = FLASH28F320_BSP_CMD_RESUME ;      /* Resume. Just in case... */
          do
          {
            *pFA = FLASH28F008_CMD_READ_STATUS ;
          }
          while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
          if (err)
          {
            break ;
          }
          *pFA = FLASH28F008_CMD_CLEAR_STATUS ;
          *pFA = FLASH28_CMD_RESET ;
          BSP_DRV_FLASH28_WRITE_DISABLE_RTN () ;    /* lower Vpp */
          pFA = (FLASH_28F_DEF *)((char *)pFA + (flash_info->f_adrs_jump * flash_info->f_sector_size)) ;
        }
      }
      else
      {
        maximal_sector = num_sectors - 1 ;
        if (sector > maximal_sector)
        {
          sector = maximal_sector ;
        }
        /*
         * 6 places in the code where there are loops.
         */
        if (do_poll_indication)
        {
          load_indication_start(6);
        }

        pFA = (FLASH_28F_DEF *)((char *)pFA + (flash_info->f_adrs_jump * sector * flash_info->f_sector_size)) ;
        BSP_DRV_FLASH28_WRITE_ENABLE_RTN () ;      /* raise Vpp */
        *pFA = FLASH28_CMD_RESET ;
        *pFA = FLASH28_CMD_RESET ;
        *pFA = FLASH28F320_BSP_CMD_LOCK_PREFIX ; /* Unlock. Just in case... */

        err = poll_indication_func(1) ;
        if (err)
        {
          break ;
        }

        do
        {
          *pFA = FLASH28F008_CMD_READ_STATUS ;
        }
        while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
        if (err)
        {
          break ;
        }
        err = poll_indication_func(2) ;
        if (err)
        {
          break ;
        }

        *pFA = FLASH28F320_BSP_CMD_CLEAR_LOCKS ;
        do
        {
          *pFA = FLASH28F008_CMD_READ_STATUS ;
        }
        while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
        if (err)
        {
          break ;
        }
        err = poll_indication_func(3) ;
        if (err)
        {
          break ;
        }
        *pFA = FLASH28_CMD_RESET ;
        *pFA = FLASH28F320_BSP_CMD_RESUME ;      /* Resume. Just in case... */
        do
        {
          *pFA = FLASH28F008_CMD_READ_STATUS ;
        }
        while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
        if (err)
        {
          break ;
        }
        err = poll_indication_func(4) ;
        if (err)
        {
          break ;
        }
        *pFA = FLASH28F008_CMD_CLEAR_STATUS ;
        *pFA = FLASH28_CMD_RESET ;
        BSP_DRV_FLASH28_WRITE_DISABLE_RTN () ;    /* lower Vpp */
        err = poll_indication_func(5) ;
        if (err)
        {
          break ;
        }
        err = poll_indication_func(6) ;
        if (err)
        {
          break ;
        }
      }
      break;
    }
    default:
    {
      retVal = ERROR ;
    }
  }
  lock_flash() ;
  if (err)
  {
    retVal = ERROR ;
  }
  return (retVal);
}
/******************************************************************************
*
* bspDrvFlash28fErase - erase the contents of flash memory
*
* This routine clears the contents of flash memory.
*
* Flash 28F\f2xxx\f1 devices are erased by writing a flash erase command to
* the device and verifying that each flash location is set to a high value
* (0xFF).
*
* Flash 29F\f2xxx\f1 devices are erased by writing the six-byte erase code
* into specific address locations, which sets all byte locations to a high
* value (0xFF).
*
* RETURNS: OK, or ERROR if the contents of flash memory cannot be erased.
*/

STATUS
  bspDrvFlash28fErase(
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
  volatile
    FLASH_28F_DEF
      *pFA ;
  STATUS
    retVal ;
  unsigned
    int
      num_sectors,
      maximal_sector ;
  int
    err ;
  unsigned long
    flash_size;

  int (*poll_indication_func)(unsigned long current_work_load);
  /*
   */
  if (do_poll_indication)
  {
    poll_indication_func = load_indication_run;
  }
  else
  {
    poll_indication_func = load_indication_run_silent;
  }

  retVal = OK ;
  unlock_flash() ;
  flash_size = FLASH_SIZE_4MEG ;

  pFA = FLASH_28F_CAST(flash_info->f_adrs) ;
  err = FALSE ;
  switch (flash_info->f_type)
  {
    case (FLASH_28F320):
    case (FLASH_28F640):
    {
      if (flash_info->f_type == FLASH_28F640)
      {
        flash_size = FLASH_SIZE_8MEG ;
        pFA = FLASH_28F_CAST(flash_info->f_adrs) ;
        num_sectors = flash_size / flash_info->f_sector_size ;
      }
      else
      {
        num_sectors = flash_size / flash_info->f_sector_size ;
      }
      maximal_sector = num_sectors - 1 ;
      if (sector < 0)
      {
        if (do_poll_indication)
        {
          load_indication_start(num_sectors);
        }
        for (sector = 0 ; sector < num_sectors ; sector++)
        {
          BSP_DRV_FLASH28_WRITE_ENABLE_RTN () ;     /* raise Vpp */
          *pFA = FLASH28_CMD_RESET ;
          *pFA = FLASH28_CMD_RESET ;
          *pFA = FLASH28F320_BSP_CMD_LOCK_PREFIX ; /* unlock. Just in case... */
          err = poll_indication_func(sector) ;
          if (err)
          {
            break ;
          }
          do
          {
            *pFA = FLASH28F008_CMD_READ_STATUS ;
          }
          while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
          if (err)
          {
            break ;
          }
          *pFA = FLASH28F320_BSP_CMD_CLEAR_LOCKS ;
          do
          {
            *pFA = FLASH28F008_CMD_READ_STATUS ;
          }
          while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
          if (err)
          {
            break ;
          }
          *pFA = FLASH28_CMD_RESET ;
          *pFA = FLASH28F320_BSP_CMD_RESUME ;      /* Resume. Just in case... */
          do
          {
            *pFA = FLASH28F008_CMD_READ_STATUS ;
          }
          while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
          if (err)
          {
            break ;
          }
          *pFA = FLASH28F008_CMD_CLEAR_STATUS ;
          *pFA = FLASH28_CMD_RESET ;
          *pFA = FLASH28_CMD_ERASE_SETUP ;    /* setup */
          *pFA = FLASH28F008_CMD_ERASE ;      /* erase */
          /*
           * Check Write State Machine Status
           */
          do
          {
            *pFA = FLASH28F008_CMD_READ_STATUS ;
          }
          while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
          if (err)
          {
            break ;
          }
          /*
           * Check Erase Error Status
           */
          if ((*pFA & FLASH28F008_STAT_EWS) != 0)
          {
            *pFA = FLASH28F008_CMD_CLEAR_STATUS ;
            retVal = ERROR ;
          }
          pFA = FLASH_28F_CAST (flash_info->f_adrs) ;
          *pFA = FLASH28_CMD_RESET ;
          BSP_DRV_FLASH28_WRITE_DISABLE_RTN () ;    /* lower Vpp */
          pFA = (FLASH_28F_DEF *)((char *)pFA + bspDrvFlash28f_sector_jump_factor(flash_info));
        }
      }
      else
      {
        if (sector > maximal_sector)
        {
          sector = maximal_sector ;
        }
        /*
         * 6 places in the code where there are loops.
         */
        if (do_poll_indication)
        {
          load_indication_start(6);
        }

        pFA = (FLASH_28F_DEF *)((char *)pFA + (bspDrvFlash28f_sector_jump_factor(flash_info) * sector));

        BSP_DRV_FLASH28_WRITE_ENABLE_RTN () ;      /* raise Vpp */
        *pFA = FLASH28_CMD_RESET ;
        *pFA = FLASH28_CMD_RESET ;
        *pFA = FLASH28F320_BSP_CMD_LOCK_PREFIX ; /* Unlock. Just in case... */
        do
        {
          *pFA = FLASH28F008_CMD_READ_STATUS;
        }
        while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
        if (err)
        {
          break ;
        }

        err = poll_indication_func(1) ;
        if (err)
        {
          break ;
        }

        *pFA = FLASH28F320_BSP_CMD_CLEAR_LOCKS ;
        do
        {
          *pFA = FLASH28F008_CMD_READ_STATUS ;
        }
        while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
        if (err)
        {
          break ;
        }

        err = poll_indication_func(2) ;
        if (err)
        {
          break ;
        }

        *pFA = FLASH28_CMD_RESET ;
        *pFA = FLASH28F320_BSP_CMD_RESUME ;      /* Resume. Just in case... */
        do
        {
          *pFA = FLASH28F008_CMD_READ_STATUS ;
        }
        while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
        err = poll_indication_func(3) ;
        if (err)
        {
          break ;
        }
        *pFA = FLASH28F008_CMD_CLEAR_STATUS ;
        *pFA = FLASH28_CMD_RESET ;
        *pFA = FLASH28_CMD_ERASE_SETUP ;     /* setup */
        *pFA = FLASH28F008_CMD_ERASE ;       /* erase */
        /*
         * Check Write State Machine Status
         */
        do
        {
          *pFA = FLASH28F008_CMD_READ_STATUS ;
        }
        while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
        err = poll_indication_func(4) ;
        if (err)
        {
          break ;
        }
        /*
         * Check Erase Error Status
         */
        if ((*pFA & FLASH28F008_STAT_EWS) != 0)
        {
          *pFA = FLASH28F008_CMD_CLEAR_STATUS ;
          retVal = ERROR ;
        }
        err = poll_indication_func(5) ;
        if (err)
        {
          break ;
        }

        pFA = FLASH_28F_CAST (flash_info->f_adrs) ;
        *pFA = FLASH28_CMD_RESET ;
        BSP_DRV_FLASH28_WRITE_DISABLE_RTN () ;    /* lower Vpp */

        err = poll_indication_func(6) ;
        if (err)
        {
          break ;
        }
      }
      if (err)
      {
        retVal = ERROR ;
      }
      break;
    }

    default:
    {
      retVal = ERROR;
    }
  }
  lock_flash() ;
  if (err)
  {
    retVal = ERROR ;
  }
  return (retVal);
}

/******************************************************************************
*
* bspDrvFlash28fWrite - write data to flash memory
*
* This routine copies specified data of a specified length, <size>, into a
* specified offset, <offset>, in the flash memory.  Data is passed as a string,
* <pFB>, if not NULL.  If NULL, data is taken as a repeated sequence of
* <value>.
* The parameter <flashType> should be set to the flash device code.
* The parameter <offset> must be appropriately aligned for the width of
* the Flash devices in use.
*
* Flash 28F\f2xxx\f1 devices are programmed by a sequence of operations:
* .iP
* set up device to write
* .iP
* perform write
* .iP
* verify the write
* .LP
*
* Flash 29F\f2xxx\f1 devices are programmed by a sequence of operations:
* .iP
* set up device to write
* .iP
* perform write
* .iP
* wait for the write to complete
* .LP
*
* RETURNS: OK, or ERROR if the write operation fails.
*
* SEE ALSO: bspDrvFlash28fSet()
*/
STATUS
  bspDrvFlash28fWrite(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          FLASH_28F_DEF *pFB,       /* string to be copied; use <value> if NULL */
          int       size,       /* size to program in bytes */
          int       offset,     /* byte offset into flash memory */
          FLASH_28F_DEF value,       /* value to program */
          int (*poll_indication_func)(unsigned long current_work_load)    /* Function to call when polling the Flash.*/
                                      /* If NULL then call no function.          */
  )
{
  volatile FLASH_28F_DEF * pFA;    /* flash address */
  STATUS retVal ;
  int
    err,
    twc,  /* time for write completion */
    sectorSize ;
  unsigned
    long
      bytes_writen_till_now;

  bytes_writen_till_now = 0;
  sectorSize = 128 ;
  retVal = OK ;
  twc = 2 ;
  err = FALSE ;
  unlock_flash() ;
  switch (flash_info->f_type)
  {
    case (FLASH_28F320):
    case (FLASH_28F640):
    {
      int
        retries ;

      BSP_DRV_FLASH28_WRITE_ENABLE_RTN ();    /* raise Vpp */
      pFA = FLASH_28F_CAST (flash_info->f_adrs + offset) ;
      *pFA = FLASH28_CMD_RESET ;
      *pFA = FLASH28_CMD_RESET ;
      *pFA = FLASH28F320_BSP_CMD_LOCK_PREFIX ; /* unlock. Just in case... */
      do
      {
        *pFA = FLASH28F008_CMD_READ_STATUS ;
      }
      while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
      if (err)
      {
        break ;
      }
      *pFA = FLASH28F320_BSP_CMD_CLEAR_LOCKS ;
      do
      {
        *pFA = FLASH28F008_CMD_READ_STATUS ;
      }
      while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
      if (err)
      {
        break ;
      }
      *pFA = FLASH28_CMD_RESET ;
      *pFA = FLASH28F320_BSP_CMD_RESUME ;      /* Resume. Just in case... */
      do
      {
        *pFA = FLASH28F008_CMD_READ_STATUS ;
      }
      while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
      if (err)
      {
        break ;
      }
      *pFA = FLASH28F008_CMD_CLEAR_STATUS ;
      *pFA = FLASH28_CMD_RESET ;
      retries = 2 ;
      for ( ; (pFA < FLASH_28F_CAST(flash_info->f_adrs + flash_info->f_adrs_jump*size + offset)) && (retVal == OK);
                  pFA+=flash_info->f_adrs_jump)
      {
        if (pFB != NULL)
        {
          value = *pFB;
        }
        if (poll_indication_func)
        {
          bytes_writen_till_now += FLASH_28F_WIDTH;
          poll_indication_func(bytes_writen_till_now);
        }

        *pFA = FLASH28_CMD_PROG_SETUP ; /* write setup */
        *pFA = value ;                  /* data to write */
        /* Check Write State Machine Status */
        do
        {
          *pFA = FLASH28F008_CMD_READ_STATUS;
        }
        while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
        if (err)
        {
          break ;
        }
        if (retries < 2)
        {
          /*
           * If this is a second cycle, validate.
           */
          *pFA = FLASH28_CMD_RESET ;
          *pFA = FLASH28_CMD_RESET ;
          if (*pFA != value)
          {
            retVal = ERROR ;
          }
          do
          {
            *pFA = FLASH28F008_CMD_READ_STATUS;
          }
          while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
          if (err)
          {
            break ;
          }
        }
        /* Check Byte Write Error Status */
        if ((*pFA & FLASH28F008_STAT_BWS) != 0)
        {
          *pFA = FLASH28F008_CMD_CLEAR_STATUS ;
          retries-- ;
          if (retries <= 0)
          {
            retVal = ERROR ;
          }
          else
          {
            *pFA = FLASH28_CMD_RESET ;
            *pFA = FLASH28F320_BSP_CMD_LOCK_PREFIX ; /* unlock. Just in case... */
            do
            {
              *pFA = FLASH28F008_CMD_READ_STATUS ;
            }
            while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
            if (err)
            {
              break ;
            }
            *pFA = FLASH28F320_BSP_CMD_CLEAR_LOCKS ;
            do
            {
              *pFA = FLASH28F008_CMD_READ_STATUS ;
            }
            while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
            if (err)
            {
              break ;
            }
            *pFA = FLASH28_CMD_RESET ;
            *pFA = FLASH28F320_BSP_CMD_RESUME ;      /* Resume. Just in case... */
            do
            {
              *pFA = FLASH28F008_CMD_READ_STATUS ;
            }
            while ((*pFA & FLASH28F008_BSP_STAT_WSMS) != FLASH28F008_BSP_STAT_WSMS);
            if (err)
            {
              break ;
            }
            *pFA = FLASH28F008_CMD_CLEAR_STATUS ;
            *pFA = FLASH28_CMD_RESET ;
            pFA-=flash_info->f_adrs_jump ;
          }
        }
        else
        { /* got here on success */
          retries = 2 ;
          pFB++;
        }
      }
      if (err)
      {
        break ;
      }
#if !DUNE_BCM
      d_taskDelay(100);
#else
#endif
      pFA = FLASH_28F_CAST (flash_info->f_adrs  + offset);
      *pFA = FLASH28_CMD_RESET;
      BSP_DRV_FLASH28_WRITE_DISABLE_RTN ();    /* lower Vpp */
      break;
    }

    default:
    {
      retVal = ERROR;
    }
  }
  lock_flash() ;
  if (err)
  {
    retVal = ERROR ;
  }
  return (retVal);
}

/******************************************************************************
*
* bspDrvFlash28fTypeGet_28 - determine the device type of on-board flash memory
*
* This routine uses the `autoselect' command to determine the device type of
* on-board flash memory for flash 28F devices.
*
* RETURNS: An integer indicating the device type of on-board flash memory.
*/

UINT8 bspDrvFlash28fTypeGet_28 (void)
{
  volatile
    FLASH_28F_DEF *pFA ;  /* flash address */
  UINT8
    retVal ;
  static
    volatile int
      dummy,
      ui ;
  unlock_flash() ;
  pFA = FLASH_28F_CAST(BOOT_FLASH_BASE_ADRS) ;
  BSP_DRV_FLASH28_WRITE_ENABLE_RTN ();      /* enable writes */
  *pFA = FLASH28_CMD_RESET ;
  for (ui = 0 ; ui < 20000 ; ui++)
  {
    dummy = ui ;
  }
  *pFA = FLASH28_CMD_READ_ID ;
  for (ui = 0 ; ui < 20000 ; ui++)
  {
    dummy = ui ;
  }
  retVal = (UINT8) *++pFA;
  *pFA = FLASH28_CMD_RESET ;
  for (ui = 0 ; ui < 20000 ; ui++)
  {
    dummy = ui ;
  }
  BSP_DRV_FLASH28_WRITE_DISABLE_RTN ();      /* disable writes */
  lock_flash() ;
  return (retVal);
}

#endif /*#ifndef SAND_LOW_LEVEL_SIMULATION*/
