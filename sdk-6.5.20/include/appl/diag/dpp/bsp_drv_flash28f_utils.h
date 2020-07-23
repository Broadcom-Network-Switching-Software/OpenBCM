/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __BSP_DRV_FLASH28F_UTILS_INCLUDED
#define __BSP_DRV_FLASH28F_UTILS_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_drv_flash28f_dev.h"

STATUS
  bspDrvFlash28fGet_protected_info(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          char          *string,    /* where to copy flash memory      */
          int           strLen,     /* maximum number of bytes to copy */
          int           offset      /* byte offset into flash memory   */
  );
STATUS
  bspDrvFlash28fKick_protected_info(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          int do_poll_indication,     /* Flag. Non-Zero: print progress indication.*/
          int sector                  /* Number of sector to kick (starting from */
                                      /* zero. If negative then erase the whole  */
                                      /* memory space. If number of sector is    */
                                      /* larger than the maximal for that flash  */
                                      /* type, last sector is erased.            */
  );
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
  );
STATUS
  bspDrvFlash28fWrite_protected_info(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          FLASH_28F_DEF *pFB,        /* string to be copied; use <value> if NULL */
          int       size,        /* size to program in bytes */
          int       offset,      /* byte offset into flash memory */
          FLASH_28F_DEF value,       /* value to program */
          int (*poll_indication_func)(unsigned long current_work_load)    /* Function to call when polling the Flash.*/
                                      /* If NULL then call no function.          */
  );

#ifdef __cplusplus
}
#endif

#endif /* __BSP_DRV_FLASH28F_UTILS_INCLUDED */
