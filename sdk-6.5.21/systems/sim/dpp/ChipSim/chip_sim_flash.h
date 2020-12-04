/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __CHIP_SIM_FLASH_H_INCLUDED__
/* { */
#define __CHIP_SIM_FLASH_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

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

/*************
 * INCLUDES  *
 */
/* { */

/* } */

/*************
 * DEFINES   *
 */


/*
DESCRIPTION
This file contains header information for 28F0X0 FLASH memory devices.
*/




/* defines */

#define  FLASH28_CMD_READ_MEM       (FLASH_DEF) 0x00000000
#define  FLASH28_CMD_READ_ID        (FLASH_DEF) 0x90909090
#define  FLASH28_CMD_ERASE_SETUP    (FLASH_DEF) 0x20202020
#define  FLASH28_CMD_ERASE          (FLASH_DEF) 0x20202020
#define  FLASH28_CMD_ERASE_VERIFY   (FLASH_DEF) 0xa0a0a0a0
#define  FLASH28_CMD_PROG_SETUP     (FLASH_DEF) 0x40404040
#define  FLASH28_CMD_PROG_VERIFY    (FLASH_DEF) 0xc0c0c0c0
#define  FLASH28_CMD_RESET           (FLASH_DEF) 0xffffffff
#define FLASH28F008_CMD_READ_STATUS  (FLASH_DEF) 0x70707070
#define FLASH28F008_CMD_CLEAR_STATUS (FLASH_DEF) 0x50505050
#define FLASH28F008_CMD_ERASE        (FLASH_DEF) 0xD0D0D0D0

#define FLASH28F320_CMD_LOCK_PREFIX  (FLASH_DEF) 0x60606060
#define FLASH28F320_CMD_SET_LOCK     (FLASH_DEF) 0x01010101
#define FLASH28F320_CMD_CLEAR_LOCKS  (FLASH_DEF) 0xD0D0D0D0
#define FLASH28F320_CMD_RESUME       (FLASH_DEF) 0xD0D0D0D0

#ifdef FLASH_WIDTH_SPECIAL_2
#define FLASH28F008_STAT_WSMS    (FLASH_DEF) 0x00800080
#else
#define FLASH28F008_STAT_WSMS    (FLASH_DEF) 0x80808080
#endif /* FLASH_WIDTH_SPECIAL_2 */
#define FLASH28F008_STAT_BWS     (FLASH_DEF) 0x10101010
#define FLASH28F008_STAT_EWS     (FLASH_DEF) 0x20202020

#define FLASH_28F256      0xa1    /* device code 28F256 */
#define FLASH_28F512      0x25    /* device code 28F512 */
#define FLASH_28F010      0xa7    /* device code 28F010 */
#define FLASH_28F020      0x2a    /* device code 28F020 */
#define FLASH_28F008      0xa2    /* device code 28F008 */
#define FLASH_28F016      0xaa    /* device code 28F016 */
#define FLASH_28F160      0xd0    /* device code 28F160 */
#define FLASH_28F320      0x16    /* device code 28F320 */
#define FLASH_28F640      0x17    /* device code 28F640 */
#define FLASH_28F128      0x18    /* device code 28F128 */

#define  FLASH_28F320_SECTOR    0x20000

#define FLASH_WIDTH 2

#if     (FLASH_WIDTH == 1)
#define FLASH_DEF       unsigned char
#define FLASH_CAST      (unsigned char *)
#endif  /* FLASH_WIDTH */

#if     (FLASH_WIDTH == 2)
#define FLASH_DEF       unsigned short
#define FLASH_CAST      (unsigned short *)
#endif  /* FLASH_WIDTH */

#if     (FLASH_WIDTH == 4)
#define FLASH_DEF       unsigned long
#define FLASH_CAST      (unsigned long *)
#endif  /* FLASH_WIDTH */

#define  FLASH_UNKNOWN  0

#define FLASH1_MEM_BASE_ADRS_4MEG 0xFFC00000
#define FLASH1_MEM_SIZE_4MEG      0x00400000 /* 4MB */
#define FLASH1_MEM_BASE_ADRS_8MEG 0xFF800000
#define FLASH1_MEM_SIZE_8MEG      0x00800000 /* 8MB */
#define BOOT_FLASH_BASE_ADRS      0xFFF00000
#define BOOT_FLASH_SIZE           0x00100000 /* 1MB */

#define B_DEFAULT_MEM_DRV_LEN_4MEG       0x100000
/*1.5 Meg, support both kind of Flash (4 and 8)*/
#define B_DEFAULT_MEM_DRV_LEN_8MEG       0x180000
/*2.5 Meg, support both only one kind of Flash of 8 Mega*/
#define B_DEFAULT_MEM_DRV_LEN_8MEG_VER_B 0x280000
/*4 Meg, not suitable for Flash less then 8 Mega*/
#define B_DEFAULT_MEM_DRV_LEN_8MEG_VER_C 0x400000

/*
 * Default value for maximal number of bytes in
 * local flash memory to contain all application
 * programs.
 * The maximal number of application programs
 * that can be stored in flash memory is
 * floor(app_flash_size/mem_drv_len).
 * Note that the LS 17 bits must be zero
 * and, in any case, are ignored. Address must,
 * then, be a multiple of 128K.
 */
#define B_DEFAULT_APP_FLASH_SIZE_4MEG         (2 * B_DEFAULT_MEM_DRV_LEN_4MEG)
#define B_DEFAULT_APP_FLASH_SIZE_8MEG         (2 * B_DEFAULT_MEM_DRV_LEN_8MEG_VER_B)
#define B_DEFAULT_APP_FLASH_SIZE_8MEG_VER_C   (B_DEFAULT_MEM_DRV_LEN_8MEG_VER_C)


#if NEGEV_NATIVE
these are also defined in appl/diag/dpp/utils_defx.h

typedef struct
{
  /*
   * This hold s the structure version.
   * Version 1 - FLASH_DEF_INF_V1:
   *   Holds: (1) unsigned long f_adrs
   *          (2) unsigned char f_adrs_jump
   *          (3) unsigned char f_type
   *          (4) unsigned long f_sector_size
   * Version 2 - FLASH_DEF_INF_V2:
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
  unsigned char f_type;        /*FLASH type. Supported: FLASH_28F640, FLASH_28F320*/
  unsigned long f_sector_size; /*Sector Byte size*/
  /* ****            **** */
  /* Version 1 ends    }  */
  /* ****            **** */
} FLASH_DEF_INF;

#endif /* NEGEV_NATIVE */


typedef enum
{
  FLASH_DEF_INF_UN_DEF = 0,
  FLASH_DEF_INF_V1 = 1,

  /*Last One*/
  FLASH_DEF_INF_NOF_VERSIONS
} FLASH_DEF_INF_VERSION;

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
  unsigned char f_type;        /*FLASH type. Supported: FLASH_28F640, FLASH_28F320*/
  unsigned long f_sector_size; /*Sector Byte size*/
  unsigned int  flash_width;
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
#if NEGEV_NATIVE
typedef unsigned char UINT8;
#endif
/* } */

/*************
 * GLOBALS   *
 */
/* { */

/* } */

/*************
 * FUNCTIONS *
 */
/* { */

/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __CHIP_SIM_FLASH_H_INCLUDED__*/
#endif
