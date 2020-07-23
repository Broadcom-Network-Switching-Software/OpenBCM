/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*************
 * INCLUDES  *
 */
/* { */

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_error_defs.h>

#include <appl/diag/dpp/dffs_driver.h>
#include <appl/diag/dpp/bsp_drv_error_defs.h>

#include <appl/diag/dpp/bsp_drv_flash28f_utils.h>


#if !DUNE_BCM

#include "dffs_low.h"
#include "bsp_drv_error_defs.h"
#include "dffs_descriptor.h"

#include "ref_sys.h"
#include "utils_defx.h"
/*Boot { */
#include "usrApp.h"
/*Boot } */

#ifdef SAND_LOW_LEVEL_SIMULATION
  #include "Dune/chipsim/include/chip_sim_flash.h"
#else
  #include "eeprom.h"
  #include "flash28.h"
#endif

#include "bsp_drv_flash28f_utils.h"

#endif

/* } */

/*************
 * DEFINES   *
 */
/* { */

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
  )
{

  unsigned int
    dev_code = 0,
    fs_version = 0;
  unsigned long
    base_addr = 0,
    top_addr = 0,
    sector_size = 0,
    app_size = 0,
    fs_size = 0,
    fs_addr = 0,
    data_addr = 0;

  BSP_DRV_INIT_ERR_DEFS("dffs_get_flash_info", BSP_DRV_SILENT);

  BSP_DRV_ERR_IF_NULL(flash_info, 5);
  BSP_DRV_BOOT_FUNC_VALIDATE(get_flash_device_code, 10);
  BSP_DRV_BOOT_FUNC_VALIDATE(get_flash_sector_size, 20);
  BSP_DRV_BOOT_FUNC_VALIDATE(boot_get_fs_version,   30);


  dev_code = get_flash_device_code();
  if (dev_code == FLASH_28F640)
  {
    base_addr = FLASH1_MEM_BASE_ADRS_8MEG;
  }
  else
  {
    base_addr = FLASH1_MEM_BASE_ADRS_4MEG;
  }

  sector_size = get_flash_sector_size();
  fs_version = boot_get_fs_version();
  if (fs_version == BT_FS_VERSION_0)
  {
    app_size = B_DEFAULT_APP_FLASH_SIZE_8MEG;
  }
  else if (fs_version == BT_FS_VERSION_1)
  {
    app_size = B_DEFAULT_APP_FLASH_SIZE_8MEG_VER_C;
  }
  else
  {
    BSP_DRV_SET_ERR_AND_EXIT(40);
  }

  fs_addr = base_addr + app_size;

  /* make sure fs_addr is a multiple of sector_size */
  fs_addr = SOC_SAND_DIV_ROUND_UP(fs_addr, sector_size);
  fs_addr *= sector_size;

  /*
   *fs descriptor takes one sector - the next sector if the base
   *sector of the fs data
   */
  data_addr = fs_addr + sector_size;

  top_addr = BOOT_FLASH_BASE_ADRS;
  /* make sure top_addr is a multiple of sector_size */
  top_addr = SOC_SAND_DIV_ROUND_DOWN(top_addr, sector_size);
  top_addr *= sector_size;
  top_addr -= sector_size;


  fs_size = top_addr - data_addr + sector_size;

  if (fs_size <= sector_size)
  {
    BSP_DRV_SET_ERR_AND_EXIT(50);
  }

  flash_info->descriptor_sector = (unsigned char)((fs_addr - base_addr)   / sector_size);
  flash_info->first_data_sector = (unsigned char)((data_addr - base_addr) / sector_size);
  flash_info->last_data_sector  = (unsigned char)((top_addr - base_addr)  / sector_size);
  flash_info->dev_code          = dev_code;
  flash_info->sector_size       = sector_size;

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_low_sector_out_of_range
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  returns TRUE if the sector is in the
*  range of the File System's sectors dedicated to files (data).
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information
*    unsigned int sector_idx - the index of the sector (base address is 0)
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    TRUE / FALSE indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
 */
static unsigned char
  dffs_low_sector_out_of_range(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    unsigned int sector_idx
  )
{
  if (
      sector_idx < (flash_info->first_data_sector) ||
      sector_idx > (flash_info->last_data_sector)
      )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

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
  )
{
  BSP_DRV_INIT_ERR_DEFS("dffs_low_is_flash_invalid", BSP_DRV_SILENT);

  BSP_DRV_ERR_IF_NULL(flash_info, 5);

  if (flash_info->dev_code != FLASH_28F640)
  {
    BSP_DRV_SET_ERR_AND_EXIT(10);
  }

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}


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
  )
{
  unsigned long
    offset = 0;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_read_sector", BSP_DRV_SILENT);
  BSP_DRV_ERR_IF_NULL(flash_info, 5);
  BSP_DRV_BOOT_FUNC_VALIDATE(sysFlashGet_protected_info, 10);

  err =
    dffs_low_sector_out_of_range(
      flash_info,
      sector_idx
    );
  BSP_DRV_EXIT_IF_ERR(err, 15);

  offset = (flash_info->sector_size) * sector_idx;


  if (!flash_info->use_bsp_driver)
  {
    err =
      sysFlashGet_protected_info(
        &(flash_info->flash_def_inf),
        buff,
        len,
        offset
      );
    BSP_DRV_EXIT_IF_ERR(err, 20);
  }
  else
  {
    err =
      bspDrvFlash28fGet_protected_info(
        &(flash_info->bsp_flash_def_inf),
        buff,
        len,
        offset
      );
    BSP_DRV_EXIT_IF_ERR(err, 20);
  }


exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

static unsigned int
  dffs_low_read_crc(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    unsigned short* crc16_res,
    unsigned int sector_idx
  )
{
  unsigned long
    offset = 0;
  unsigned short
    crc16 = 0;
  unsigned long
    crc32 = 0;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_read_crc", BSP_DRV_SILENT);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(crc16_res, 20);
  BSP_DRV_BOOT_FUNC_VALIDATE(sysFlashGet_protected_info, 30);

  err =
    dffs_low_sector_out_of_range(
      flash_info,
      sector_idx
    );
  BSP_DRV_EXIT_IF_ERR(err, 40);

  offset = (flash_info->sector_size) * (sector_idx + 1);
  if (flash_info->use_bsp_driver)
  {
    offset -= DFFS_LOW_CRC32_SIZE;
  }
  else
  {
    offset -= DFFS_LOW_CRC16_SIZE;
  }
  if (flash_info->use_bsp_driver)
  {
    err =
      bspDrvFlash28fGet_protected_info(
        &(flash_info->bsp_flash_def_inf),
        (char*)&crc32,
        DFFS_LOW_CRC32_SIZE,
        offset
      );
    BSP_DRV_EXIT_IF_ERR(err, 50);

    crc16 = (unsigned short)crc32;
  }
  else
  {
    err =
      sysFlashGet_protected_info(
        &(flash_info->flash_def_inf),
        (char*)&crc16,
        DFFS_LOW_CRC16_SIZE,
        offset
      );
    BSP_DRV_EXIT_IF_ERR(err, 50);
  }


  *crc16_res = crc16;

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}


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
  )
{
  unsigned long
    offset = 0;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_write_sector", BSP_DRV_SILENT);
  BSP_DRV_ERR_IF_NULL(flash_info, 5);
  BSP_DRV_BOOT_FUNC_VALIDATE(sysFlashWrite_protected_info, 10);

  err =
    dffs_low_sector_out_of_range(
      flash_info,
      sector_idx
    );
  BSP_DRV_EXIT_IF_ERR(err, 20);

  offset = (flash_info->sector_size) * sector_idx;

  if (flash_info->use_bsp_driver)
  {
    err =
      bspDrvFlash28fErase_protected_info(
        &(flash_info->bsp_flash_def_inf),
        FALSE,
        sector_idx
      );
    BSP_DRV_EXIT_IF_ERR(err, 30);

    err =
      bspDrvFlash28fWrite_protected_info(
        &(flash_info->bsp_flash_def_inf),
        FLASH_28F_CAST(buff),
        len,
        offset,
        (FLASH_28F_DEF)0,
        NULL
     );
    BSP_DRV_EXIT_IF_ERR(err, 40);

  }
  else
  {
    err =
      sysFlashErase_protected_info(
        &(flash_info->flash_def_inf),
        FALSE,
        sector_idx
      );
    BSP_DRV_EXIT_IF_ERR(err, 30);

    err =
      sysFlashWrite_protected_info(
        &(flash_info->flash_def_inf),
        FLASH_CAST(buff),
        len,
        offset,
        (FLASH_DEF)0,
        NULL
     );
    BSP_DRV_EXIT_IF_ERR(err, 40);
  }



exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*  dffs_low_erase_sector
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Erases the specified sector on flash.
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information.
*    char* buff - where to write the information.
*    unsigned int len - nof bytes to erase from the sector to buff.
*    unsigned int sector_idx - the index of the sector to erase
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
static unsigned int
  dffs_low_erase_sector(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    unsigned int sector_idx
  )
{
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_erase_sector", BSP_DRV_SILENT);
  BSP_DRV_ERR_IF_NULL(flash_info, 5);
  BSP_DRV_BOOT_FUNC_VALIDATE(sysFlashErase_protected_info, 10);

  err =
    dffs_low_sector_out_of_range(
      flash_info,
      sector_idx
    );
  BSP_DRV_EXIT_IF_ERR(err, 15);

  if (flash_info->use_bsp_driver)
  {
    err =
      bspDrvFlash28fErase_protected_info(
        &(flash_info->bsp_flash_def_inf),
        FALSE,
        sector_idx
      );
    BSP_DRV_EXIT_IF_ERR(err, 20);
  }
  else
  {
    err =
      sysFlashErase_protected_info(
        &(flash_info->flash_def_inf),
        FALSE,
        sector_idx
      );
    BSP_DRV_EXIT_IF_ERR(err, 20);
  }


exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
* dffs_low_size_to_sectors
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Calculates the amount of sectors needed to cover the specified size.
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long sector_size - the size of a single sector.
*    unsigned long size - the size in bytes
*    unsigned int *nof_sectors - the amount of sectors needed
*    unsigned long *last_sec_size - nof bytes used in the last sector -
*                                   between 0 and sector_size - 1.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    nof_sectors, last_sec_size.
*REMARKS:
*    None.
*SEE ALSO:
 */
STATIC void
  dffs_low_size_to_sectors(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    unsigned long sector_size,
    unsigned long size,
    unsigned int *nof_sectors,
    unsigned long *last_sec_size
  )
{
  unsigned int
    nof_sctrs = 0,
    effictive_size = 0;
  unsigned long
    last_sctr_size = 0;

  if (flash_info->use_bsp_driver)
  {
    effictive_size = sector_size - DFFS_LOW_CRC32_SIZE;
  }
  else
  {
    effictive_size = sector_size - DFFS_LOW_CRC16_SIZE;
  }

  nof_sctrs = SOC_SAND_DIV_ROUND_UP(size, effictive_size);
  last_sctr_size = size % effictive_size;

  if (last_sctr_size == 0)
  {
    if (nof_sctrs != 0)
    {
      last_sctr_size = effictive_size;
    }
  }

  *nof_sectors = nof_sctrs;
  *last_sec_size = last_sctr_size;
}


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
  )
{
  unsigned int
    descriptor_size = 0,
    offset = 0;

  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_read_descriptor", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 5);
  BSP_DRV_ERR_IF_NULL(in_mem_buff, 10);
  BSP_DRV_BOOT_FUNC_VALIDATE(sysFlashGet_protected_info, 20);

  descriptor_size = sizeof(DFFS_DESCRIPTOR_TYPE);
  offset = flash_info->descriptor_sector * flash_info->sector_size;

  if (flash_info->use_bsp_driver)
  {
    err =
      bspDrvFlash28fGet_protected_info(
        &(flash_info->bsp_flash_def_inf),
        in_mem_buff,
        descriptor_size,
        offset
      );
    BSP_DRV_EXIT_IF_ERR(err, 30);
  }
  else
  {
    err =
      sysFlashGet_protected_info(
        &(flash_info->flash_def_inf),
        in_mem_buff,
        descriptor_size,
        offset
      );
    BSP_DRV_EXIT_IF_ERR(err, 30);
  }


exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

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
  )
{
  unsigned int
    descriptor_size = 0,
    offset = 0;

  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_write_descriptor", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 5);
  BSP_DRV_ERR_IF_NULL(in_mem_buff, 10);
  BSP_DRV_BOOT_FUNC_VALIDATE(sysFlashWrite_protected_info, 20);

  descriptor_size = sizeof(DFFS_DESCRIPTOR_TYPE);
  offset = flash_info->descriptor_sector * flash_info->sector_size;


  if (flash_info->use_bsp_driver)
  {
    err =
      bspDrvFlash28fErase_protected_info(
        &(flash_info->bsp_flash_def_inf),
        FALSE,
        flash_info->descriptor_sector
      );
    BSP_DRV_EXIT_IF_ERR(err, 30);

    if (descriptor_size % 2 != 0)
    {
      descriptor_size++;
    }

    err =
      bspDrvFlash28fWrite_protected_info(
        &(flash_info->bsp_flash_def_inf),
        FLASH_28F_CAST(in_mem_buff),
        descriptor_size,
        offset,
        (FLASH_28F_DEF)0,
        NULL
     );
    BSP_DRV_EXIT_IF_ERR(err, 40);
  }
  else
  {
    err =
      sysFlashErase_protected_info(
        &(flash_info->flash_def_inf),
        FALSE,
        flash_info->descriptor_sector
      );
    BSP_DRV_EXIT_IF_ERR(err, 30);

    err =
      sysFlashWrite_protected_info(
        &(flash_info->flash_def_inf),
        FLASH_CAST(in_mem_buff),
        descriptor_size,
        offset,
        (FLASH_DEF)0,
        NULL
     );
    BSP_DRV_EXIT_IF_ERR(err, 40);
  }


exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*  dffs_low_erase_descriptor
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Erase a dffs descriptor on flash.
*INPUT:
*  SOC_SAND_DIRECT:
*    const DFFS_LOW_FLASH_INFO_TYPE *flash_info - flash information.
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
static unsigned int
  dffs_low_erase_descriptor(
    const DFFS_LOW_FLASH_INFO_TYPE *flash_info,
    unsigned char silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_erase_descriptor", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_BOOT_FUNC_VALIDATE(sysFlashErase_protected_info, 20);

  if (flash_info->use_bsp_driver)
  {
    err =
      bspDrvFlash28fErase_protected_info(
        &(flash_info->bsp_flash_def_inf),
        silent,
        flash_info->descriptor_sector
      );
    BSP_DRV_EXIT_IF_ERR(err, 30);
  }
  else
  {
    err =
      sysFlashErase_protected_info(
        &(flash_info->flash_def_inf),
        silent,
        flash_info->descriptor_sector
      );
    BSP_DRV_EXIT_IF_ERR(err, 30);
  }


exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

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
  )
{
  unsigned int
    curr_sector = 0,
    sector_to_read = 0,
    sectors_read = 0,
    nof_sectors = 0,
    curr_len = 0,
    curr_pos = 0;
  unsigned long
    last_sector_size = 0;
  unsigned short
    crc16_from_flash = 0,
    crc16_calculated = 0;
  unsigned int
    crc_err = FALSE,
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_read_file", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 5);
  BSP_DRV_ERR_IF_NULL(file_buff, 10);

  dffs_low_size_to_sectors(
    flash_info,
    flash_info->sector_size,
    file_size,
    &nof_sectors,
    &last_sector_size
  );

  if (nof_sectors == 0)
  {
    BSP_DRV_SET_ERR_AND_EXIT(20);
  }

  BSP_DRV_PRINT_MSG("Reading file from flash...");

  if (silent == BSP_DRV_NOT_SILENT)
  {
    load_indication_start(nof_sectors);
  }

  for (
    curr_sector = 0;
    (curr_sector < (sizeof(sectors_bitmap)*8))&&(sectors_read < nof_sectors);
    curr_sector ++
  )
  {
      if (DFFS_LOW_BIT_IS_ON(sectors_bitmap, curr_sector))
      {
        if (sectors_read == (nof_sectors - 1))
        {
           /* last sector to read */
          curr_len = last_sector_size;
        }
        else
        {
          if (flash_info->use_bsp_driver)
          {
            curr_len = flash_info->sector_size - DFFS_LOW_CRC32_SIZE;
          }
          else
          {
            curr_len = flash_info->sector_size - DFFS_LOW_CRC16_SIZE;
          }
        }

        sector_to_read = curr_sector + flash_info->first_data_sector;

        err =
          dffs_low_read_sector(
            flash_info,
            file_buff + curr_pos,
            curr_len,
            sector_to_read
          );
        BSP_DRV_EXIT_IF_ERR(err, 30);

        err =
          dffs_low_read_crc(
            flash_info,
            &(crc16_from_flash),
            sector_to_read
          );
        BSP_DRV_EXIT_IF_ERR(err, 40);

        crc16_calculated =
          crc16_with_ini(
            file_buff + curr_pos,
            curr_len,
            0xFFFF
          );

        if (crc16_calculated != crc16_from_flash)
        {
          soc_sand_os_printf("CRC error: CRC from flash: 0x%x  Calculated CRC: 0x%x\r\n",
            crc16_from_flash,
            crc16_calculated
            );
          crc_err = TRUE;
        }

        curr_pos += curr_len;

        sectors_read ++;
        if (silent == BSP_DRV_NOT_SILENT)
        {
          load_indication_run(sectors_read);
        }
        else
        {
          load_indication_run_silent(sectors_read);
        }
      }
  }

  if (sectors_read < nof_sectors)
  {
    BSP_DRV_PRINT_ERR_MSG("bitmap does not cover the requested file size");
    BSP_DRV_SET_ERR_AND_EXIT(50);
  }

  *crc_err_encountered = crc_err;

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

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
  )
{
  unsigned int
    curr_sector = 0,
    sector_to_write = 0,
    sectors_written = 0,
    nof_sectors = 0,
    curr_len = 0,
    curr_pos = 0;
  unsigned long
    last_sector_size = 0,
    offset = 0;
  unsigned short
    crc16_calculated = 0;
  unsigned long
    crc32_calculated = 0;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_write_file", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 5);
  BSP_DRV_ERR_IF_NULL(file_buff, 10);

  dffs_low_size_to_sectors(
    flash_info,
    flash_info->sector_size,
    file_size,
    &nof_sectors,
    &last_sector_size
  );

  if (nof_sectors == 0)
  {
    BSP_DRV_SET_ERR_AND_EXIT(20);
  }

  BSP_DRV_PRINT_MSG("Writing file to flash...");

  if (silent == BSP_DRV_NOT_SILENT)
  {
    load_indication_start(nof_sectors);
  }

  for (
    curr_sector = 0;
    (curr_sector < (sizeof(sectors_bitmap)*8))&&(sectors_written < nof_sectors);
    curr_sector ++
  )
  {
      if (DFFS_LOW_BIT_IS_ON(sectors_bitmap, curr_sector))
      {
        if (sectors_written == (nof_sectors - 1))
        {
          /* last sector to write*/
          curr_len = last_sector_size;
        }
        else
        {
          if (flash_info->use_bsp_driver)
          {
            curr_len = flash_info->sector_size - DFFS_LOW_CRC32_SIZE;
          }
          else
          {
            curr_len = flash_info->sector_size - DFFS_LOW_CRC16_SIZE;
          }
        }

        sector_to_write = curr_sector + flash_info->first_data_sector;

        err =
          dffs_low_write_sector(
            flash_info,
            file_buff + curr_pos,
            curr_len,
            sector_to_write
          );
        BSP_DRV_EXIT_IF_ERR(err, 30);

        crc16_calculated =
          crc16_with_ini(
            file_buff + curr_pos,
            curr_len,
            0xFFFF
            );
        crc32_calculated = crc16_calculated;

        offset =
          (flash_info->sector_size) * (sector_to_write + 1);

        if (flash_info->use_bsp_driver)
        {
          offset -= DFFS_LOW_CRC32_SIZE;
        }
        else
        {
          offset -= DFFS_LOW_CRC16_SIZE;
        }

        if (flash_info->use_bsp_driver)
        {
          err =
            bspDrvFlash28fWrite_protected_info(
            &(flash_info->bsp_flash_def_inf),
            FLASH_28F_CAST(&crc32_calculated),
            DFFS_LOW_CRC32_SIZE,
            offset,
            (FLASH_28F_DEF)0,
            NULL
            );
          BSP_DRV_EXIT_IF_ERR(err, 40);
        }
        else
        {
          err =
            sysFlashWrite_protected_info(
            &(flash_info->flash_def_inf),
            FLASH_CAST(&crc16_calculated),
            DFFS_LOW_CRC16_SIZE,
            offset,
            (FLASH_DEF)0,
            NULL
            );
          BSP_DRV_EXIT_IF_ERR(err, 40);
        }


        curr_pos += curr_len;
        sectors_written ++;
        if (silent == BSP_DRV_NOT_SILENT)
        {
          load_indication_run(sectors_written);
        }
        else
        {
          load_indication_run_silent(sectors_written);
        }
      }
  }

 if (sectors_written < nof_sectors)
 {
   BSP_DRV_PRINT_ERR_MSG("bitmap does not cover the requested file size");
   BSP_DRV_SET_ERR_AND_EXIT(50);
 }

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

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
  )
{
  unsigned int
    curr_sector = 0,
    sector_to_erase = 0,
    sectors_erased = 0,
    nof_sectors = 0;
  unsigned long
    last_sector_size = 0;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_erase_file", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);

  dffs_low_size_to_sectors(
    flash_info,
    flash_info->sector_size,
    file_size,
    &nof_sectors,
    &last_sector_size
  );

  if (nof_sectors == 0)
  {
    BSP_DRV_SET_ERR_AND_EXIT(20);
  }

  BSP_DRV_PRINT_MSG("Erasing file from flash...");

  if (silent == BSP_DRV_NOT_SILENT)
  {
    load_indication_start(nof_sectors);
  }

  for (
    curr_sector = 0;
    (curr_sector < (sizeof(sectors_bitmap)*8))&&(sectors_erased < nof_sectors);
    curr_sector ++
  )
  {
      if (DFFS_LOW_BIT_IS_ON(sectors_bitmap, curr_sector))
      {
        sector_to_erase = curr_sector + flash_info->first_data_sector;

        err =
          dffs_low_erase_sector(
            flash_info,
            sector_to_erase
          );
        BSP_DRV_EXIT_IF_ERR(err, 30);

        sectors_erased ++;
        if (silent == BSP_DRV_NOT_SILENT)
        {
          load_indication_run(sectors_erased);
        }
        else
        {
          load_indication_run_silent(sectors_erased);
        }
      }
  }

  if (sectors_erased < nof_sectors)
  {
    BSP_DRV_PRINT_ERR_MSG("bitmap does not cover the requested file size");
    BSP_DRV_SET_ERR_AND_EXIT(40);
  }

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*  dffs_low_clear_dffs
*TYPE:
*  PROC
*DATE:
*  27/06/2006
*FUNCTION:
*  Erase the bits on flash dedicated to DFFS (sescriptor and files).
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
  )
{
  unsigned int
    err = SOC_SAND_OK;
  unsigned int
    curr_sector = 0;

  BSP_DRV_INIT_ERR_DEFS("dffs_low_clear_dffs", silent);

  err =
    dffs_low_erase_descriptor(
      flash_info,
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 10);

  for (
    curr_sector = flash_info->first_data_sector;
    curr_sector < flash_info->last_data_sector;
    curr_sector++
  )
  {
    err =
      dffs_low_erase_sector(
        flash_info,
        curr_sector
      );
  }

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/* } */

