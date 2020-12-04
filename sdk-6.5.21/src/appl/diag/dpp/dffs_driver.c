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


#if !DUNE_BCM

#include "dffs_driver.h"
#include "bsp_drv_error_defs.h"
#include "utils_defx.h"
#include "dunedriver/sand/Utils/include/sand_bitstream.h"
#include "dunedriver/sand/Management/include/sand_general_macros.h"


#ifdef SAND_LOW_LEVEL_SIMULATION
  #include "pub/include/ref_sys.h"
  #include "../../../../h/drv/mem/eeprom.h"
#else
  #include "eeprom.h"
#endif

#endif

/* } */

/*************
 * DEFINES   *
 */
/* { */
#define DFFS_DRV_COLUMNS 8
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

/*
 * Calculate dffs descriptor CRC.
 * It's caller responsibility to verify dffs_descriptor is not NULL
 */
static unsigned short
  dffs_drv_calculate_desc_crc(
    DFFS_DESCRIPTOR_TYPE* dffs_descriptor,
    unsigned char silent
  )
{
  unsigned short
    crc16 = 0;
  unsigned long
    size = 0;

  size =
    sizeof(DFFS_DESCRIPTOR_TYPE) - DFFS_LOW_CRC16_SIZE;

  crc16 =
    crc16_with_ini(
      dffs_descriptor,
      size,
      0xFFFF
    );

  return crc16;
}

/*
 * Calculate dffs descriptor CRC, and verify it equals the contents
 *  of the CRC field.
 * It's caller's responsibility to verify dffs_descriptor is not NULL
 */
unsigned char
  dffs_drv_desc_crc_is_valid(
    DFFS_DESCRIPTOR_TYPE* dffs_descriptor,
    unsigned char silent
  )
{
  unsigned short
    calculated_crc16 = 0;

  calculated_crc16 =
    dffs_drv_calculate_desc_crc(
      dffs_descriptor,
      silent
    );

  if (calculated_crc16 == dffs_descriptor->crc16)
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
*  dffs_drv_set_defaults
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Sets a DFFS descriptor to a valid default values
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    unsigned char silent - if TRUE, printing is suppressed
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    dffs_descriptor.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_drv_set_defaults(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE* dffs_descriptor,
    unsigned char silent
  )
{
  unsigned short
    crc16 = 0;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_set_defaults", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  memset(
    dffs_descriptor,
    0x0,
    sizeof(DFFS_DESCRIPTOR_TYPE)
  );

  dffs_descriptor->header.version = DFFS_VER_01;
  dffs_descriptor->header.dffs_signature = DFFS_SIGNATURE_PATTERN;

  dffs_descriptor->sectors_available =
    flash_info->last_data_sector - flash_info->first_data_sector + 1;

  crc16 =
    dffs_drv_calculate_desc_crc(
      dffs_descriptor,
      silent
    );

  dffs_descriptor->crc16 = crc16;
exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
* char  dffs_signature_valid
*TYPE:
*  PROC
*DATE:
*  28/06/2006
*FUNCTION:
*  returns TRUE if the signature matches the expected pattern
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    TRUE / FALSE
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned char
  dffs_signature_valid(
    DFFS_DESCRIPTOR_TYPE* dffs_descriptor
  )
{
  unsigned long
    curr_signature;

  curr_signature =
    dffs_descriptor->header.dffs_signature;

  if (curr_signature != DFFS_SIGNATURE_PATTERN)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*****************************************************
*NAME
*  dffs_drv_clear_dffs
*TYPE:
*  PROC
*DATE:
*  03/07/2006
*FUNCTION:
*  Erases DFFS from flash
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_clear_dffs(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    unsigned char silent
  )
{
  unsigned int
     err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_clear_dffs", silent);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 10);
  BSP_DRV_ERR_IF_NULL(flash_info, 20);

  memset(
    dffs_descriptor,
    0x0,
    sizeof(DFFS_DESCRIPTOR_TYPE)
  );

  err =
    dffs_low_clear_dffs(
      flash_info,
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 30);

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
* int  dffs_drv_allocate_sectors
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Allocates the sectors needed for a file of a size file_size
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    unsigned long             file_size - the size to be allocated
*    unsigned long*            sectors_bitmap - bitmap of the allocated
*                               sectors
*    unsigned char silent - if TRUE, printing is suppressed
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    sectors_bitmap.
*REMARKS:
*    DFFS descriptor is only changed in-memory.
*     The function allocating the sectors should also write the descriptor to flash
*     if the allocated sectors are used.
*    If the allocated sectors are not use (i.e. in error situation),
*     they should be freed using dffs_drv_free_sectors.
*SEE ALSO:
 */
static unsigned int
  dffs_drv_allocate_sectors(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    unsigned long             file_size,
    unsigned long*            sectors_bitmap,
    unsigned char             silent
  )
{
  unsigned long
    empty_bitmap = 0,
    busy_bitmap = 0;
  unsigned int
    nof_sectors_needed = 0,
    nof_sectors_found = 0,
    curr_sector = 0,
    effective_sec_size = 0;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_allocate_sectors", silent);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 10);
  BSP_DRV_ERR_IF_NULL(flash_info, 20);
  BSP_DRV_ERR_IF_NULL(sectors_bitmap, 30);

  if (flash_info->use_bsp_driver)
  {
    effective_sec_size = flash_info->sector_size - DFFS_LOW_CRC32_SIZE;
  }
  else
  {
    effective_sec_size = flash_info->sector_size - DFFS_LOW_CRC16_SIZE;
  }


  /* the number of sectors is derived from sector size and the size of the CRC */
  nof_sectors_needed =
    SOC_SAND_DIV_ROUND_UP(file_size, effective_sec_size);

  if (nof_sectors_needed > dffs_descriptor->sectors_available)
  {
    BSP_DRV_PRINT_ERR_MSG("Requested size exceeds available amount");
    BSP_DRV_SET_ERR_AND_EXIT(50);
  }

  busy_bitmap = dffs_descriptor->busy_sectors;

  do
  {
    if (!DFFS_LOW_BIT_IS_ON(busy_bitmap, curr_sector))
    {
      SOC_SAND_SET_BIT(empty_bitmap, 1, curr_sector);
      SOC_SAND_SET_BIT(busy_bitmap, 1, curr_sector);
      nof_sectors_found ++;
    }

    curr_sector++;

  } while(
      (nof_sectors_found < nof_sectors_needed) &&
      (curr_sector <= flash_info->last_data_sector)
    );

  if (nof_sectors_found < nof_sectors_needed)
  {
    BSP_DRV_PRINT_ERR_MSG("Failed to allocate sectors");
    BSP_DRV_SET_ERR_AND_EXIT(50);
  }

  *sectors_bitmap = empty_bitmap;

  /*
   * update dffs descriptor is updated only if empty_sectors allocation succeeded
   * Note! The descriptor is not updated on flash - it will only be updated when
   * writing a file.
   */
  dffs_descriptor->busy_sectors = busy_bitmap;
  dffs_descriptor->sectors_available -= nof_sectors_found;

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
* int  dffs_drv_count_busy_sectors
*TYPE:
*  PROC
*DATE:
*  18/09/2006
*FUNCTION:
*  Calculates the number of busy data sectors.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    unsigned int*             nof_busy_sectors - return value
*    unsigned char silent - if TRUE, printing is suppressed
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    nof_busy_sectors.
*REMARKS:
*    None.
*SEE ALSO:
 */
static unsigned int
  dffs_drv_count_busy_sectors(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    unsigned int*             nof_busy_sectors,
    unsigned char             silent
  )
{
  unsigned long
    busy_bitmap = 0;
  unsigned int
    busy_sectors = 0,
    curr_sector = 0;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_count_busy_sectors", silent);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 10);
  BSP_DRV_ERR_IF_NULL(flash_info, 20);
  BSP_DRV_ERR_IF_NULL(nof_busy_sectors, 30);

  busy_bitmap = dffs_descriptor->busy_sectors;

  do
  {
    if (DFFS_LOW_BIT_IS_ON(busy_bitmap, curr_sector))
    {
      busy_sectors ++;
    }

    curr_sector++;

  } while(
      curr_sector <= flash_info->last_data_sector
    );

  *nof_busy_sectors = busy_bitmap;

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*
 * Mark the sectors set in sectors_bitmap as not busy
 * It's caller responsibility to verify dffs_descriptor is not NULL
 */
/*****************************************************
*NAME
*   dffs_drv_free_sectors
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Returns to DFFS the sectors marked in sectors_bitmap.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    unsigned long         sectors_bitmap - a bitmap of
*                               the sectors to be freed
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
STATIC void
  dffs_drv_free_sectors(
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    unsigned long             sectors_bitmap
  )
{
  unsigned int
    curr_sector = 0,
    nof_sectors_freed = 0;
  unsigned long
    busy_bitmap = 0;

  busy_bitmap = dffs_descriptor->busy_sectors;

  for (curr_sector = 0; curr_sector < (sizeof(sectors_bitmap)*8); curr_sector ++)
  {
    if (DFFS_LOW_BIT_IS_ON(sectors_bitmap, curr_sector))
    {
      SOC_SAND_SET_BIT(busy_bitmap, 0, curr_sector);
      nof_sectors_freed ++;
    }
  }

  dffs_descriptor->busy_sectors = busy_bitmap;
  dffs_descriptor->sectors_available += nof_sectors_freed;
}

/*****************************************************
*NAME
* int   dffs_drv_get_empty_fd
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Returns a pointer to the location in DFFS descriptor of
*   an available file descriptor, according to the member_sectors field.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    DFFS_FD_TYPE**         empty_fd - a pointer a pointer holding
*                           an available file descriptor, or NULL if not found.
*    unsigned char silent - if TRUE, printing is suppressed
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    empty_fd.
*REMARKS:
*    None.
*SEE ALSO:
 */
static unsigned int
  dffs_drv_get_empty_fd(
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    DFFS_FD_TYPE**            empty_fd,
    unsigned char             silent
  )
{
  DFFS_FD_TYPE
    *fd_ptr = NULL;
  unsigned int
    fd_idx = 0,
    found = FALSE;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_get_empty_fd", silent);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 10);

  for (
    fd_idx = 0;
    (fd_idx < DFFS_MAX_FILES)&&(!found);
    fd_idx ++
   )
  {
    fd_ptr = &(dffs_descriptor->fd[fd_idx]);
    if (fd_ptr->member_sectors == 0)
    {
      found = TRUE;
    }
  }

  if (found)
  {
    *empty_fd = fd_ptr;
  }
  else
  {
    *empty_fd = NULL;
  }

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
* int  dffs_drv_get_fd_by_name
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Returns a pointer to the file descriptor of a file named <file_name>
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    DFFS_FD_TYPE**            fd - a pointer to a pointer
*                              holding the file descriptor
*                              of a file "file_name", or NULL if not found.
*    unsigned char silent - if TRUE, printing is suppressed
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    fd.
*REMARKS:
*    None.
*SEE ALSO:
 */
static unsigned int
  dffs_drv_get_fd_by_name(
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    DFFS_FD_TYPE**            fd,
    unsigned char             silent
  )
{
  unsigned char
    found = FALSE;
  DFFS_FD_TYPE
    *fd_ptr = NULL;
  unsigned int
    file_idx = 0;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_get_fd_by_name", silent);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 10);

  for (
    file_idx = 0;
    (file_idx < DFFS_MAX_FILES)&&(!found);
    file_idx ++
  )
  {
    fd_ptr = &(dffs_descriptor->fd[file_idx]);

    if (fd_ptr->member_sectors != 0)
    {
      /* This is a valid fd*/
      found =
        !strncmp(
          fd_ptr->name,
          file_name,
          DFFS_FILENAME_SIZE
        );
    }
  }

  if (found)
  {
    *fd = fd_ptr;
  }
  else
  {
    *fd = NULL;
  }

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*  dffs_drv_file_exists
*TYPE:
*  PROC
*DATE:
*  06/07/2006
*FUNCTION:
*  Checks if the requested file exists.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    unsigned int*             does_file_exist - TRUE if the file exists
*                               on dffs
*    unsigned char silent - if TRUE, printing is suppressed
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    TRUE/FALSE.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_drv_file_exists(
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    unsigned int*             does_file_exist,
    unsigned char             silent
  )
{
  DFFS_FD_TYPE
    *fd = NULL;
  unsigned int
    file_exists = FALSE,
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_file_exists", silent);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 10);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 20);

  if (fd == NULL)
  {
    file_exists = FALSE;
  }
  else
  {
    file_exists = TRUE;
  }

  *does_file_exist = file_exists;

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_get_file_size
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Reads the requested file from flash to the in-memory buffer.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    unsigned long*            file_size - the size of the requested file
*    unsigned char silent - if TRUE, printing is suppressed
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    file_size.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_drv_get_file_size(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    uint32*            file_size,
    unsigned char             silent
  )
{
  DFFS_FD_TYPE*
    fd = NULL;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_get_file_size", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 30);

  if (fd == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("The file with the specified name does not exist");
    BSP_DRV_SET_ERR_AND_EXIT(40);
  }

  *file_size = fd->total_size;

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
*   dffs_drv_file_from_flash
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Reads the requested file from flash to the in-memory buffer.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    char*                     file_size - the size of the requested file
*    char*                     file_buff - a memory allocated for the file
*    unsigned char silent - if TRUE, printing is suppressed
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    file_buff.
*REMARKS:
*    None.
*SEE ALSO:
 */
unsigned int
  dffs_drv_file_from_flash(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    unsigned long             file_size,
    char*                     file_buff,
    unsigned char             silent
  )
{
  DFFS_FD_TYPE*
    fd = NULL;
  unsigned int
    crc_err = FALSE,
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_file_from_flash", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);
  BSP_DRV_ERR_IF_NULL(file_buff, 30);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 50);

  if (fd == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("The file with the specified name does not exist");
    BSP_DRV_SET_ERR_AND_EXIT(55);
  }

  if (fd->total_size != file_size)
  {
    BSP_DRV_EXIT_AND_PRINT_IF_ERR(
      err, 58, "Supplied buffer size differs from the requested file size"
    );
  }

  err =
    dffs_low_read_file(
      flash_info,
      file_buff,
      fd->member_sectors,
      fd->total_size,
      &(crc_err),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 60);

  if(crc_err)
  {
    BSP_DRV_GEN_SVR_ERR(
      "FILE CRC invalid - the file will be deleted!!!",
      "dffs_drv_file_from_flash",
      DFFS_FILE_CRC_ERR
    );

    err =
      dffs_drv_delete_file(
        flash_info,
        dffs_descriptor,
        file_name,
        silent
      );
    BSP_DRV_EXIT_IF_ERR(err, 70);
  }

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_file_to_flash
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Writes the requested file to flash from the in-memory buffer.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    char*                     file_size - the size of the requested file
*    char*                     file_buff - the buffer containing the file
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_file_to_flash(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    unsigned long             file_size,
    char*                     file_buff,
    unsigned char             silent
  )
{
  unsigned long
    sectors_bitmap = 0;
  DFFS_FD_TYPE*
    fd = NULL;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_file_to_flash", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);
  BSP_DRV_ERR_IF_NULL(file_buff, 30);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 50);

  if (fd != NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("A file with the specified name already exists");
    BSP_DRV_SET_ERR_AND_EXIT(60);
  }

  err =
    dffs_drv_get_empty_fd(
      dffs_descriptor,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 70);

  if (fd == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("Exceeded maximal allowed number of files on flash!");
    BSP_DRV_SET_ERR_AND_EXIT(75);
  }

  err =
    dffs_drv_allocate_sectors(
      flash_info,
      dffs_descriptor,
      file_size,
      &(sectors_bitmap),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 80);

  err =
    dffs_low_write_file(
      flash_info,
      file_buff,
      sectors_bitmap,
      file_size,
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 90);

  /*
   * update the file descriptor
   */
  fd->member_sectors = sectors_bitmap;
  fd->total_size = file_size;
  strncpy(
    fd->name,
    file_name,
    DFFS_FILENAME_SIZE-1
  );

  fd->name[DFFS_FILENAME_SIZE-1] = '\0';

  /*
   * Update the DFFS descriptor.
   * Note that the busy_sectors and the sectors_available fields are
   * updated in the allocate_sectors routine.
   */
  dffs_descriptor->crc16 =
    dffs_drv_calculate_desc_crc(
      dffs_descriptor,
      silent
    );

  err =
    dffs_low_write_descriptor(
      flash_info,
      (char*)dffs_descriptor,
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 100);

exit:
  if (err && (sectors_bitmap!= 0))
  {
    dffs_drv_free_sectors(
      dffs_descriptor,
      sectors_bitmap
    );
  }
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_delete_file
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Deletes the requested file.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_delete_file(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    unsigned char             silent
  )
{
  DFFS_FD_TYPE*
    fd = NULL;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_delete_file", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 40);

  if (fd == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("The file with the specified name does not exist");
    BSP_DRV_SET_ERR_AND_EXIT(50);
  }

  dffs_drv_free_sectors(
    dffs_descriptor,
    fd->member_sectors
  );

  fd->member_sectors = 0;
  fd->total_size = 0;

  dffs_descriptor->crc16 =
    dffs_drv_calculate_desc_crc(
      dffs_descriptor,
      silent
    );

  err =
    dffs_low_write_descriptor(
      flash_info,
      (char*)dffs_descriptor,
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 60);

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_dir
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Prints to screen the file information on DFFS files.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    DFFS_FORMAT_TYPE          format - long or short format
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_dir(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    DFFS_FORMAT_TYPE          format,
    unsigned char             silent
  )
{
  DFFS_FD_TYPE
    *fd_ptr = NULL;
  unsigned int
    fd_idx = 0,
    nof_files = 0,
    total_space = 0,
    taken_space = 0,
    busy_sectors = 0,
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_dir", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  d_printf(
    "\n\r\n\r"
    "Dune Flash File System dir: \n\r\n\r"
    "|File Name                |Size-Bytes| Ver  | Date    | Attr1 | Attr2 | Attr3 |\n\r"
    "+-------------------------+----------+------+---------+-------+-------+-------+\n\r"
  );

  for (fd_idx = 0; fd_idx < DFFS_MAX_FILES; fd_idx ++)
  {
    fd_ptr = &(dffs_descriptor->fd[fd_idx]);
    if (fd_ptr->member_sectors != 0)
    {
      nof_files++;
      d_printf(
        "| %-24s| %-9u| %-5u| %-8s| %-6u| %-6u| %-6u|\n\r",
        fd_ptr->name,
        fd_ptr->total_size,
        fd_ptr->version,
        fd_ptr->date,
        fd_ptr->attr1,
        fd_ptr->attr2,
        fd_ptr->attr3
      );

      d_printf(
        "+-------------------------+----------+------+---------+-------+-------+-------+\n\r"
      );
      if (format == DFFS_LONG_FORMAT)
      {
        d_printf(
          "| Comment: %-65s  |\n\r"
          "+-------------------------+----------+------+---------+-------+-------+-------+\n\r",
          fd_ptr->comment
        );
      }
    }
  }

  d_printf(
    "Number of files: %u out of %u\n\r",
    nof_files, DFFS_MAX_FILES
  );

  total_space = ((flash_info->sector_size - DFFS_LOW_CRC16_SIZE) \
    * dffs_descriptor->sectors_available) / 1024;

  err =
    dffs_drv_count_busy_sectors(
      flash_info,
      dffs_descriptor,
      &busy_sectors,
      silent
    );

  taken_space = ((flash_info->sector_size - DFFS_LOW_CRC16_SIZE) \
    * busy_sectors) / 1024;

  d_printf(
    "Used space     : %uK (%uK available)\n\r",
    taken_space,
    (total_space - taken_space)
  );

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_diagnostics
*TYPE:
*  PROC
*DATE:
*  10/07/2006
*FUNCTION:
*  Prints general information on DFFS
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_diagnostics(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    unsigned char             silent
  )
{
  unsigned int
    fd_idx = 0;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_diagnostics", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  d_printf(
    "\n\r\n\r"
    "Dune Flash File System Diagnostics: \n\r"
    "------------------------------------\n\r"
  );

  d_printf(
    "\n\r"
    "+------------+\n\r"
    "| FLASH INFO |\n\r"
    "+------------+\n\r"
    "\n\r"
  );

  d_printf(
    "flash code:          %u\n\r"
    "first data sector:   %u\n\r"
    "last data sector :   %u\n\r"
    "sector size (Bytes): %u\n\r",
    (unsigned int)flash_info->dev_code,
    (unsigned int)flash_info->first_data_sector,
    (unsigned int)flash_info->last_data_sector,
    (unsigned int)flash_info->sector_size
  );

  d_printf(
    "\n\r"
    "+-----------------+\n\r"
    "| DESCRIPTOR INFO |\n\r"
    "+-----------------+\n\r"
    "\n\r"
  );

  d_printf(
    "busy sectors bitmap:         0x%X\n\r"
    "number of available sectors: %u\n\r"
    "DFFS signature             : 0x%X\n\r"
    "DFFS version               : %u\n\r"
    "descriptor crc value       : %u\n\r",
    (unsigned int)dffs_descriptor->busy_sectors,
    (unsigned int)dffs_descriptor->sectors_available,
    (unsigned int)dffs_descriptor->header.dffs_signature,
    (unsigned int)dffs_descriptor->header.version,
    (unsigned int)dffs_descriptor->crc16
  );

  d_printf(
    "\n\r"
    "+-----------------------+\n\r"
    "| FILE DESCRIPTORS INFO |\n\r"
    "+-----------------------+\n\r"
    "\n\r"
  );

  for (fd_idx = 0; fd_idx < DFFS_MAX_FILES; fd_idx++)
  {
    d_printf(
      "File #%u\n\r"
      "----------\n\r",
      fd_idx
    );

    d_printf(
      "name:                  %s\n\r"
      "size (Bytes):          %u\n\r"
      "version:               %u\n\r"
      "creation date:         %s\n\r"
      "mamber sectors bitmap: 0x%X\n\r"
      "attribute 1:           %u\n\r"
      "attribute 2:           %u\n\r"
      "attribute 3:           %u\n\r"
      "comment:               %s\n\r",
      dffs_descriptor->fd[fd_idx].name,
      (unsigned int)dffs_descriptor->fd[fd_idx].total_size,
      (unsigned int)dffs_descriptor->fd[fd_idx].version,
      dffs_descriptor->fd[fd_idx].date,
      (unsigned int)dffs_descriptor->fd[fd_idx].member_sectors,
      (unsigned int)dffs_descriptor->fd[fd_idx].attr1,
      (unsigned int)dffs_descriptor->fd[fd_idx].attr1,
      (unsigned int)dffs_descriptor->fd[fd_idx].attr3,
      dffs_descriptor->fd[fd_idx].comment
    );

  }

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_print_file
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Prints the contents of the requested file to screen.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    unsigned long             size_int_bytes - the number of bytes to print
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_print_file(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    unsigned long             size_in_bytes,
    unsigned char             silent
  )
{
  DFFS_FD_TYPE
    *fd = NULL;
  unsigned int
    file_size = 0,
    size_to_print = 0;
  unsigned long
    *file_buff = NULL;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_print_file", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 50);

  if (fd == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("The file with the specified name does not exist");
    BSP_DRV_SET_ERR_AND_EXIT(60);
  }

  file_size = fd->total_size;
  file_buff =
    soc_sand_os_malloc(
      file_size
    );
  if (file_buff == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("Not enough memory for this file size");
    BSP_DRV_SET_ERR_AND_EXIT(70);
  }

  err =
    dffs_drv_file_from_flash(
      flash_info,
      dffs_descriptor,
      file_name,
      file_size,
      (char*)file_buff,
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 80);

  if (size_in_bytes == 0)
  {
    size_to_print = file_size;
  }
  else
  {
    size_to_print = SOC_SAND_MIN(file_size, size_in_bytes);
  }

#if !DUNE_BCM
  soc_sand_buff_print_all(
    (unsigned char*)file_buff,
    size_to_print,
    DFFS_DRV_COLUMNS
  );
#endif

exit:
  soc_sand_os_free(file_buff);
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_set_version
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Sets the version field of the requested file.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    unsigned char             version - the value to be set
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_set_version(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    unsigned char             version,
    unsigned char             silent
  )
{
  DFFS_FD_TYPE
    *fd = NULL;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_set_version", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 40);

  if (fd == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("The file with the specified name does not exist");
    BSP_DRV_SET_ERR_AND_EXIT(50);
  }

  fd->version = version;

  dffs_descriptor->crc16 =
    dffs_drv_calculate_desc_crc(
      dffs_descriptor,
      silent
    );

  err =
    dffs_low_write_descriptor(
      flash_info,
      (char*)dffs_descriptor,
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 60);

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_set_attr
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Sets the general attribute field of the requested file.'
*  attr_index distinguishes between attr1, attr2 and attr3.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    unsigned long             attribute - the value to be set
*    unsigned char             attr_idx - the index of the general attribute
*                               to be updated.
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_set_attr(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    unsigned long             attribute,
    unsigned char             attr_idx,
    unsigned char             silent
  )
{
  DFFS_FD_TYPE
    *fd = NULL;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_set_attr", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 40);

  if (fd == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("The file with the specified name does not exist");
    BSP_DRV_SET_ERR_AND_EXIT(50);
  }

  switch(attr_idx)
  {
  case (1):
    fd->attr1 = attribute;
    break;
  case (2):
    fd->attr2 = attribute;
    break;
  case (3):
    fd->attr3 = attribute;
    break;
  default:
      BSP_DRV_PRINT_ERR_MSG("Attribute index out of range");
      BSP_DRV_SET_ERR_AND_EXIT(55);
      break;
  }

  dffs_descriptor->crc16 =
    dffs_drv_calculate_desc_crc(
      dffs_descriptor,
      silent
    );

  err =
    dffs_low_write_descriptor(
      flash_info,
      (char*)dffs_descriptor,
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 60);

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_set_date
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Sets the date field of the requested file.
* The date is a string 8 charecters long, i.e. 03/06/06
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    char                      date[DFFS_DATE_STR_SIZE] - the value to be set
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_set_date(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    char                      date[DFFS_DATE_STR_SIZE],
    unsigned char             silent
  )
{
  DFFS_FD_TYPE
    *fd = NULL;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_set_date", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 40);

  if (fd == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("The file with the specified name does not exist");
    BSP_DRV_SET_ERR_AND_EXIT(50);
  }

  strncpy(
    fd->date,
    date,
    DFFS_DATE_STR_SIZE-1
  );

  fd->date[DFFS_DATE_STR_SIZE-1] = '\0';

  dffs_descriptor->crc16 =
    dffs_drv_calculate_desc_crc(
      dffs_descriptor,
      silent
    );

  err =
    dffs_low_write_descriptor(
      flash_info,
      (char*)dffs_descriptor,
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 60);

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_set_comment
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Sets the comment field of the requested file.
*  A comment is a general text of
*   max DFFS_COMMENT_STR_SIZE (20 on creation) charecters.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    char                      comment[DFFS_COMMENT_STR_SIZE] - the value to be set
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_set_comment(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    char                      comment[DFFS_COMMENT_STR_SIZE],
    unsigned char             silent
  )
{
  DFFS_FD_TYPE
    *fd = NULL;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_set_comment", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 40);

  if (fd == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("The file with the specified name does not exist");
    BSP_DRV_SET_ERR_AND_EXIT(50);
  }

  strncpy(
    fd->comment,
    comment,
    DFFS_COMMENT_STR_SIZE-1
  );

  fd->comment[DFFS_COMMENT_STR_SIZE-1] = '\0';

  dffs_descriptor->crc16 =
    dffs_drv_calculate_desc_crc(
      dffs_descriptor,
      silent
    );

  err =
    dffs_low_write_descriptor(
      flash_info,
      (char*)dffs_descriptor,
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 60);

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   dffs_drv_print_comment
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Prints the comment field of the requested file to screen.
*INPUT:
*  SOC_SAND_DIRECT:
*    DFFS_LOW_FLASH_INFO_TYPE* flash_info - flash information
*    DFFS_DESCRIPTOR_TYPE* dffs_descriptor - file system descriptor
*    char*                     file_name - the name of the requested file
*    unsigned char silent - if TRUE, printing is suppressed
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
  dffs_drv_print_comment(
    DFFS_LOW_FLASH_INFO_TYPE* flash_info,
    DFFS_DESCRIPTOR_TYPE*     dffs_descriptor,
    char*                     file_name,
    unsigned char             silent
  )
{
  DFFS_FD_TYPE
    *fd = NULL;
  unsigned int
    err = SOC_SAND_OK;

  BSP_DRV_INIT_ERR_DEFS("dffs_drv_print_comment", silent);
  BSP_DRV_ERR_IF_NULL(flash_info, 10);
  BSP_DRV_ERR_IF_NULL(dffs_descriptor, 20);

  err =
    dffs_drv_get_fd_by_name(
      dffs_descriptor,
      file_name,
      &(fd),
      silent
    );
  BSP_DRV_EXIT_IF_ERR(err, 40);

  if (fd == NULL)
  {
    BSP_DRV_PRINT_ERR_MSG("The file with the specified name does not exist");
    BSP_DRV_SET_ERR_AND_EXIT(50);
  }

  d_printf("\n\rComment of a file %s: \n\r", file_name);
  d_printf("--------------------- \n\r");
  d_printf("%s \n\r", fd->comment);

exit:
  BSP_DRV_EXIT_AND_PRINT_ERR;
}


/* } */

