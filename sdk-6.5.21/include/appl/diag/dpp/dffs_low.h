/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __DFFS_LOW_H_INCLUDED__
/* { */
#define __DFFS_LOW_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

/*************
 * INCLUDES  *
 */
/* { */

#ifdef SAND_LOW_LEVEL_SIMULATION
  #include <sim/dpp/ChipSim/chip_sim_flash.h>
#else
  #include "bsp_drv_flash28f_dev.h"
#endif /* SAND_LOW_LEVEL_SIMULATION */

/* } */

/*************
 * DEFINES   *
 */
/* { */
#define DFFS_LOW_CRC16_SIZE sizeof(unsigned short)
#define DFFS_LOW_CRC32_SIZE sizeof(unsigned long)

/* } */

/*************
 *  MACROS   *
 */
/* { */

/* non-zero if the specified bit in the bitmap is on */
#define DFFS_LOW_BIT_IS_ON(_dffsl_bitmap, _dffsl_bit_idx) \
  (_dffsl_bitmap & SOC_SAND_BIT(_dffsl_bit_idx))

/* } */

/*************
 * TYPE DEFS *
 */
/* { */
typedef struct
{
  unsigned char        dev_code;
  unsigned char        descriptor_sector;
  unsigned char        first_data_sector;
  unsigned char        last_data_sector;
  unsigned long        sector_size;
  unsigned int         use_bsp_driver;     /* if 1 use BSP driver (see bsp_drv_flash28f_dev.h).
                                              Else, use boot driver (see flash_dev.h) */
  FLASH_DEF_INF        flash_def_inf;      /* if use_bsp_driver==0, this descriptor is used */
  BSP_FLASH28F_DEF_INF bsp_flash_def_inf;  /* if use_bsp_driver==1, this descriptor is used */
}DFFS_LOW_FLASH_INFO_TYPE;

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

/*****************************************************
*NAME
* dffs_low_get_flash_info
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  get information on the flash in use
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info -
*     flash information
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    flash_info.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_low_get_flash_info(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info
  );

/*****************************************************
*NAME
*  dffs_low_is_flash_invalid
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Checks if the flash is invalid for the DFFS
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication - error means the flash is invalid for DFFS
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_low_is_flash_invalid(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info
  );

/*****************************************************
*NAME
*  dffs_low_read_descriptor
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Read a dffs descriptor to in-memory buffer.
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information.
*    char* in_mem_buff - in-memory buffer.
*    unsigned char silent - if TRUE, printing is suppressed.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_low_read_descriptor(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    char* in_mem_buff,
    unsigned char silent
  );

/*****************************************************
*NAME
*  dffs_low_write_descriptor
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Write a dffs descriptor to flash from in-memory buffer.
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information.
*    char* in_mem_buff - in-memory buffer.
*    unsigned char silent - if TRUE, printing is suppressed.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_low_write_descriptor(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    char* in_mem_buff,
    unsigned char silent
  );

/*****************************************************
*NAME
*  dffs_low_read_file
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Read a file from flash to in-memory buffer, based on the supplied bitmap.
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information.
*    char* file_buff - the in-memory buffer.
*    unsigned long sectors_bitmap - if bit i is on, the appropriate sector has
*                                   to be read.
*    unsigned long file_size - the size of the file to read.
*    unsigned int* crc_err_encountered - if TRUE, crc error accured
*                   while reading file from flash
*    unsigned char silent - if TRUE, printing is suppressed.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    file_buff.
*REMARKS:
*    If the bitmap doesn't have enough 'on' bits to cover the requested size,
*     an error is initiated.
*SEE ALSO:
 */
unsigned int
  dffs_low_read_file(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    char* file_buff,
    unsigned long sectors_bitmap,
    unsigned long file_size,
    unsigned int* crc_err_encountered,
    unsigned char silent
  );
/*****************************************************
*NAME
*  dffs_low_write_file
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Write a file from in-memory buffer to flash, based on the supplied bitmap.
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information.
*    char* file_buff - the in-memory buffer.
*    unsigned long sectors_bitmap - if bit i is on, the appropriate sector has
*                                   to be written.
*    unsigned long file_size - the size of the file to write.
*    unsigned char silent - if TRUE, printing is suppressed.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    file_buff.
*REMARKS:
*    If the bitmap doesn't have enough 'on' bits to cover the requested size,
*     an error is initiated.
*SEE ALSO:
 */
unsigned int
  dffs_low_write_file(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    char* file_buff,
    unsigned long sectors_bitmap,
    unsigned long file_size,
    unsigned char silent
  );

/*****************************************************
*NAME
*  dffs_low_erase_file
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Erase a file on flash, based on the supplied bitmap.
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information.
*    unsigned long sectors_bitmap - if bit i is on, the appropriate sector has
*                                   to be erased.
*    unsigned long file_size - the size of the file to erase.
*    unsigned char silent - if TRUE, printing is suppressed.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    file_buff.
*REMARKS:
*    If the bitmap doesn't have enough 'on' bits to cover the requested size,
*     an error is initiated.
*SEE ALSO:
 */
unsigned int
  dffs_low_erase_file(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    unsigned long sectors_bitmap,
    unsigned long file_size,
    unsigned char silent
  );

/*****************************************************
*NAME
*  dffs_low_clear_dffs
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Erase the bits on flash dedicated to DFFS descriptor.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    unsigned char silent - if TRUE, printing is suppressed.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_low_clear_dffs(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    unsigned char silent
  );

/*****************************************************
*NAME
*  dffs_low_write_sector
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Writes the specified sector from in-memory buffer flash.
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information.
*    char* buff - input buffer containing the information.
*    unsigned int len - nof bytes to write.
*    unsigned int sector_idx - the index of the sector to write
*                               (0 means flash base address)
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_low_write_sector(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    char* buff,
    unsigned int len,
    unsigned int sector_idx
  );

/*****************************************************
*NAME
*  dffs_low_read_sector
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Reads the specified sector from flash to in-memory buffer.
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information.
*    char* buff - where to write the information.
*    unsigned int len - nof bytes to read from the sector to buff.
*    unsigned int sector_idx - the index of the sector to read
*                               (0 means flash base address)
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    buff.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_low_read_sector(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    char* buff,
    unsigned int len,
    unsigned int sector_idx
  );
/* } */

#ifdef  __cplusplus
}
#endif

/* } __DFFS_LOW_H_INCLUDED__*/
#endif
