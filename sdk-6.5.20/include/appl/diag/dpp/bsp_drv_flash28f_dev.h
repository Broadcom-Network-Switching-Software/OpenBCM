/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __BSP_DRV_FLASH28F_DEV_INCLUDED
#define __BSP_DRV_FLASH28F_DEV_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif


/* defines */

#define FLASH_28F_WIDTH 4

#ifndef FLASH_28F128
#define FLASH_28F128      0x18    /* device code 28F128 */
#endif

#ifndef FLASH_28F640
#define FLASH_28F640      0x17    /* device code 28F640 */
#endif

#ifndef FLASH_28F320
#define FLASH_28F320      0x16    /* device code 28F320 */
#endif

#if     (FLASH_28F_WIDTH == 1)
#define FLASH_28F_DEF       unsigned char
#define FLASH_28F_CAST      (unsigned char *)
#endif  /* FLASH_28F_WIDTH */

#if     (FLASH_28F_WIDTH == 2)
#define FLASH_28F_DEF       unsigned short
#define FLASH_28F_CAST      (unsigned short *)
#endif  /* FLASH_28F_WIDTH */

#if     (FLASH_28F_WIDTH == 4)
#define FLASH_28F_DEF       unsigned long
#define FLASH_28F_CAST      (unsigned long *)
#endif  /* FLASH_28F_WIDTH */


#ifndef SAND_LOW_LEVEL_SIMULATION

/* function declarations */

typedef struct
{
  /*
   * This hold s the structure version.
   * Version 1 - BSP_FLASH28F_DEF_INF_V1:
   *   Holds: (1) unsigned long f_adrs
   *          (2) unsigned char f_adrs_jump
   *          (3) unsigned char f_type
   *          (4) unsigned long f_sector_size
   * Version 2 - BSP_FLASH28F_DEF_INF_V2:
   *   TBD in the future.
   * NOTE:
   * (a) In order to support older application version in newer boot, every
   *     structure version can not delete parameters, only to add.
   * (b) Newer boot version may use only the structure section indicated by the version number.
   */
  unsigned char struct_version;

  /* ****            **** */
  /* Structure parameters */
  /* Version 1 starts  {  */
  /* ****            **** */
  unsigned long f_adrs;        /*Base address of FLASH*/
  unsigned char f_adrs_jump;   /*E.g. FLASH is 16 bits.*/
                               /* + If we access it by N, N+2, N+4, than put here ONE.*/
                               /* + If we access it by N, N+4, N+8, than put here TWO.*/
  unsigned int  flash_width;   /* word size */
  unsigned char f_type;        /*FLASH type. Supported: FLASH_28F640, FLASH_28F320*/
  unsigned long f_sector_size; /*Sector Byte size*/
  /* ****            **** */
  /* Version 1 ends    }  */
  /* ****            **** */
} BSP_FLASH28F_DEF_INF;

typedef enum
{
  BSP_FLASH28F_DEF_INF_UN_DEF = 0,
  BSP_FLASH28F_DEF_INF_V1 = 1,

  /*Last One*/
  BSP_FLASH28F_DEF_INF_NOF_VERSIONS
} BSP_FLASH28F_DEF_INF_VERSION;

IMPORT  UINT8 bspDrvFlash28fTypeGet_28(void) ;
IMPORT  UINT8 bspDrvFlash28fTypeGet_29(void) ;

IMPORT  STATUS
  bspDrvFlash28fGet(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          char          *string,      /* where to copy flash memory      */
          int           strLen,       /* maximum number of bytes to copy */
          int           offset        /* byte offset into flash memory   */
  ) ;

IMPORT  STATUS  bspDrvFlash28fSet (
    char *string, int strLen, int offset,
    int (*poll_indication)()    /* Function to call when polling the Flash.*/
                                /* If NULL then call no function.          */
) ;
IMPORT  unsigned long sysGetFlashBase(void) ;
STATUS
  bspDrvFlash28fKick(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          int do_poll_indication,     /* Flag. Non-Zero: print progress indication.*/
          int sector                  /* Number of sector to kick (starting from */
                                      /* zero. If negative then erase the whole  */
                                      /* memory space. If number of sector is    */
                                      /* larger than the maximal for that flash  */
                                      /* type, last sector is erased.            */
  ) ;
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
  ) ;

IMPORT  STATUS
  bspDrvFlash28fWrite(
    const BSP_FLASH28F_DEF_INF *flash_info,  /*Flash information*/
          FLASH_28F_DEF *pFB,       /* string to be copied; use <value> if NULL */
          int       size,       /* size to program in bytes */
          int       offset,     /* byte offset into flash memory */
          FLASH_28F_DEF value,       /* value to program */
          int (*poll_indication)(unsigned long current_work_load) /* Function to call when polling the Flash.*/
                                      /* If NULL then call no function.          */
    ) ;
#endif /*SAND_LOW_LEVEL_SIMULATION*/

#ifdef __cplusplus
}
#endif

#endif /* __BSP_DRV_FLASH28F_DEV_INCLUDED */
