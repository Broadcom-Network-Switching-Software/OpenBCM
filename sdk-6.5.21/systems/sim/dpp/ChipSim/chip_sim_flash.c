/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
    #error  "Add your system support for packed attribute."
#endif

#if !(defined(SAND_LOW_LEVEL_SIMULATION) || (defined(LINUX) || defined(UNIX)))
  #error "This module should only be compiled for win_sim environment or dhrp"
#endif

/*************
 * INCLUDES  *
 */
/* { */
#include "chip_sim_flash.h"
#include "chip_sim.h"
#include <string.h>

/* } */

/*************
 * DEFINES   *
 */
/* { */
#define  CSF_MAX_SECTORS (FLASH1_MEM_SIZE_8MEG / FLASH_28F320_SECTOR)
/* } */

/*************
 *  MACROS   *
 */
/* { */

/* } */

/*************
 * TYPE DEFS *
 */
/* { */

/* } */

/*************
 * GLOBALS   *
 */
/* { */
unsigned char Csf_flash[FLASH1_MEM_SIZE_8MEG] = {0};
/* } */

/*************
 * FUNCTIONS *
 */
/* { */
STATUS
  sysFlashGet_protected(
    char   *string,    /* where to copy flash memory      */
    int    strLen,     /* maximum number of bytes to copy */
    int    offset      /* byte offset into flash memory   */
  )
{
  /*
  unsigned int
    curr_pos = 0;
  */

  if ((offset + strLen) > FLASH1_MEM_SIZE_8MEG)
  {
    return ERROR;
  }

  memcpy(
    string,
    &(Csf_flash[offset]),
    strLen
  );

  /*
  for (curr_pos = 0; curr_pos < (unsigned int)strLen; curr_pos++)
  {
    *(string + curr_pos) = Csf_flash[curr_pos];
  }
  */

  return OK;
}

STATUS
  sysFlashGet_protected_info(
    const FLASH_DEF_INF *flash_info,  /*Flash information*/
    char   *string,    /* where to copy flash memory      */
    int    strLen,     /* maximum number of bytes to copy */
    int    offset      /* byte offset into flash memory   */
  )
{
  STATUS
    rtn;

  rtn = sysFlashGet_protected(
          string,
          strLen,
          offset);

  return
    rtn;
}


STATUS
  sysFlashErase_protected(
    unsigned char flashType,    /* type of flash memory on-board           */
    int do_poll_indication,     /* Flag. Non-Zero print progress indication*/
                                /* in percentage.                          */
    int sector                  /* Number of sector to erase (starting from*/
                                /* zero. If negative then erase the whole  */
                                /* memory space. If number of sector is    */
                                /* larger than the maximal for that flash  */
                                /* type, last sector is erased.            */
  )
{
  /*
  unsigned int
    curr_pos = 0;
  */
  unsigned long
    offset = 0;

  if (sector >= CSF_MAX_SECTORS)
  {
    return ERROR;
  }

  offset = sector * FLASH_28F320_SECTOR; /* sector size */

  memset(
    &(Csf_flash[offset]),
    0,
    FLASH_28F320_SECTOR
  );

  /*
  for (curr_pos = 0; curr_pos < (unsigned int)sector; curr_pos++)
  {
    Csf_flash[curr_pos] = 0;
  }
  */

  return OK;
}

#if NEGEV_NATIVE
STATUS
  sysFlashErase_protected_info(
    const FLASH_DEF_INF *flash_info,  /*Flash information*/
    unsigned char flashType,    /* type of flash memory on-board           */
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
    rtn;

  rtn = sysFlashErase_protected(
          flashType,
          do_poll_indication,
          sector);

  return rtn;

}
#endif

STATUS
  sysFlashWrite_protected(
    FLASH_DEF *pFB,        /* string to be copied; use <value> if NULL */
    int       size,        /* size to program in bytes */
    int       offset,      /* byte offset into flash memory */
    UINT8     flashType,   /* type of flash memory on-board */
    FLASH_DEF value,       /* value to program */
    int (*poll_indication)(unsigned long current_work_load)    /* Function to call when polling the Flash.*/
                                /* If NULL then call no function.          */
  )
{

  if ((offset + size) > FLASH1_MEM_SIZE_8MEG)
  {
    return ERROR;
  }

  memcpy(
    &(Csf_flash[offset]),
    pFB,
    size
  );

  return OK;
}

#if NEGEV_NATIVE
STATUS
  sysFlashWrite_protected_info(
    const FLASH_DEF_INF *flash_info,  /*Flash information*/
    FLASH_DEF *pFB,        /* string to be copied; use <value> if NULL */
    int       size,        /* size to program in bytes */
    int       offset,      /* byte offset into flash memory */
    UINT8     flashType,   /* type of flash memory on-board */
    FLASH_DEF value,       /* value to program */
    int (*poll_indication)(unsigned long current_work_load)    /* Function to call when polling the Flash.*/
                                /* If NULL then call no function.          */
  )
{
  STATUS
    rtn = sysFlashWrite_protected(
      pFB,
      size,
      offset,
      flashType,
      value,
      poll_indication);

  return rtn;
}
#endif

unsigned int
  get_flash_device_code()
{
  return FLASH_28F640;
}

#if NEGEV_NATIVE
unsigned int
  get_flash_sector_size()
{
  return FLASH_28F320_SECTOR;
}
#endif
/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif
