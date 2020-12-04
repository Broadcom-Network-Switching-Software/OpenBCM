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

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_dffs_cpu_mez.h>
#include <appl/diag/dpp/utils_error_defs.h>

#include <appl/diag/dpp/dffs_driver.h>

#include <appl/dpp/UserInterface/ui_pure_defi.h>
#include <appl/dpp/UserInterface/ui_cli_files.h>


/*************
 * INCLUDES  *
 */
/* { */
#if !DUNE_BCM

#include "../include/utils_dffs_cpu_mez.h"
#include "Bsp_drv/include/dffs_driver.h"
#include "Utilities/include/utils_error_defs.h"
#include "Utilities/include/utils_ip_mgmt.h"
#include "userinterface/include/ui_cli_files.h"

/*Boot { */
#include "../../../../h/usrApp.h"
/*Boot } */

#ifdef SAND_LOW_LEVEL_SIMULATION
  #include "Dune/chipsim/include/chip_sim_flash.h"
#endif /* SAND_LOW_LEVEL_SIMULATION */


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
#define UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(e_err_code) \
{                                         \
  if (Utils_dffs_cpu_is_initialized == FALSE)   \
  {                                       \
    UTILS_SET_ERR_AND_EXIT(e_err_code)  \
  }                                       \
}
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
static DFFS_LOW_FLASH_INFO_TYPE
  Utils_dffs_cpu_flash_info;

static DFFS_DESCRIPTOR_TYPE
  Utils_dffs_cpu_descriptor;

static unsigned char
  Utils_dffs_cpu_is_initialized = FALSE;


/* } */

/*************
 * FUNCTIONS *
 */
/* { */


/*****************************************************
*NAME
* utils_dffs_cpu_get_flash_info
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
  utils_dffs_cpu_get_flash_info(
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

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_get_flash_info");

  if (flash_info==NULL)
  {
    UTILS_SET_ERR_AND_EXIT(5);
    goto exit;
  }


  UTILS_FUNC_VALIDATE(get_flash_device_code, 10);
  UTILS_FUNC_VALIDATE(get_flash_sector_size, 20);
  UTILS_FUNC_VALIDATE(boot_get_fs_version,   30);


  /* Call boot function and ask the type of CPU flash */
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
    UTILS_SET_ERR_AND_EXIT(40);
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
    UTILS_SET_ERR_AND_EXIT(50);
  }

  flash_info->descriptor_sector = (unsigned char)((fs_addr - base_addr)   / sector_size);
  flash_info->first_data_sector = (unsigned char)((data_addr - base_addr) / sector_size);
  flash_info->last_data_sector  = (unsigned char)((top_addr - base_addr)  / sector_size);
  flash_info->dev_code          = dev_code;
  flash_info->sector_size       = sector_size;
  flash_info->use_bsp_driver    = 0;

  flash_info->flash_def_inf.f_type          = dev_code;
  flash_info->flash_def_inf.f_adrs          = base_addr;
  flash_info->flash_def_inf.f_adrs_jump     = 1;
  flash_info->flash_def_inf.f_sector_size   = sector_size;
  flash_info->flash_def_inf.struct_version  = FLASH_DEF_INF_V1;

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
*  utils_dffs_cpu_init
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Brings the DFFS descriptor from flash to the supplied in-memory buffer.
*  Checks validity of the DFFS. If necessary - creates an empty DFFS.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_init(
    unsigned char silent
  )
{
  unsigned char
    signature_valid = FALSE,
    crc_valid = FALSE;
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_init");

  err =
    utils_dffs_cpu_get_flash_info(&(Utils_dffs_cpu_flash_info));
  UTILS_EXIT_IF_ERR(err, 30);

  err =
    dffs_low_is_flash_invalid(
      &(Utils_dffs_cpu_flash_info)
    );
  UTILS_EXIT_AND_PRINT_IF_ERR(err, silent, 35, "Invalid flash version");

  err =
    dffs_low_read_descriptor(
      &(Utils_dffs_cpu_flash_info),
      (char*)&(Utils_dffs_cpu_descriptor),
      silent
    );
  UTILS_EXIT_IF_ERR(err, 40);

  signature_valid =
    dffs_signature_valid(&(Utils_dffs_cpu_descriptor));

  if (signature_valid)
  {
    crc_valid =
      dffs_drv_desc_crc_is_valid(
        &(Utils_dffs_cpu_descriptor),
        silent
      );
  }

  if (!(signature_valid) || !(crc_valid))
  {
    if (!signature_valid)
    {
      UTILS_PRINT_MSG(silent,"DFFS not found on flash");
    }
    else if (!crc_valid)
    {
      UTILS_GEN_SVR_ERR(
        "DFFS CRC invalid - all DFFS data will be lost!!!",
        "utils_dffs_cpu_init",
         DFFS_DESCRIPTOR_CRC_ERR,
         0
      );
    }

    UTILS_PRINT_MSG(silent,"Creating empty DFFS...");

    err =
      dffs_drv_set_defaults(
        &(Utils_dffs_cpu_flash_info),
        &(Utils_dffs_cpu_descriptor),
        silent
      );
    UTILS_EXIT_IF_ERR(err, 50);

    err =
      dffs_low_write_descriptor(
        &(Utils_dffs_cpu_flash_info),
        (char*)&(Utils_dffs_cpu_descriptor),
        silent
      );
    UTILS_EXIT_IF_ERR(err, 60);

    UTILS_PRINT_MSG(silent,"DFFS create - success");
  }

  Utils_dffs_cpu_is_initialized = TRUE;

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*  utils_dffs_cpu_erase
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Erases DFFS from flash.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_erase(
    unsigned char silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_erase");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_clear_dffs(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

  Utils_dffs_cpu_is_initialized = FALSE;

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*  utils_dffs_cpu_file_exists
*TYPE:
*  PROC
*DATE:
*  06/07/2006
*FUNCTION:
*  Checks if the requested file exists.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_file_exists(
    char*                     file_name,
    unsigned int*             does_file_exist,
    unsigned char             silent
  )
{
 unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_get_file_size");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_file_exists(
      &(Utils_dffs_cpu_descriptor),
      file_name,
      does_file_exist,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   utils_dffs_cpu_get_file_size
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Reads the requested file from flash to the in-memory buffer.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_get_file_size(
    char*                     file_name,
    unsigned long*            file_size,
    unsigned char             silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_get_file_size");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_get_file_size(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      file_name,
      (uint32*)file_size,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   utils_dffs_cpu_file_from_flash
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Reads the requested file from flash to the in-memory buffer.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_file_from_flash(
    char*                     file_name,
    unsigned long             file_size,
    char*                     file_buff,
    unsigned char             silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_file_from_flash");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_file_from_flash(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      file_name,
      file_size,
      file_buff,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
*   utils_dffs_cpu_file_to_flash
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Writes the requested file to flash from the in-memory buffer.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_file_to_flash(
    char*                     file_name,
    unsigned long             file_size,
    char*                     file_buff,
    unsigned char             silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_file_to_flash");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_file_to_flash(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      file_name,
      file_size,
      file_buff,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
*   utils_dffs_cpu_delete_file
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Deletes the requested file.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_delete_file(
    char*                     file_name,
    unsigned char             silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_delete_file");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_delete_file(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      file_name,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
*   utils_dffs_cpu_dir
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Prints to screen the file information on DFFS files.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_dir(
    DFFS_FORMAT_TYPE format,
    unsigned char    silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_dir");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_dir(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      format,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   utils_dffs_cpu_diagnostics
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Prints to screen the file information on DFFS files.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_diagnostics(
    unsigned char    silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_dir");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_diagnostics(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   utils_dffs_cpu_print_file
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Prints the contents of the requested file to screen.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_print_file(
    char*                     file_name,
    unsigned long             size_int_bytes,
    unsigned char             silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_print_file");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_print_file(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      file_name,
      size_int_bytes,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
*   utils_dffs_cpu_set_version
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Sets the version field of the requested file.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_set_version(
    char*                     file_name,
    unsigned char             version,
    unsigned char             silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_set_version");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_set_version(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      file_name,
      version,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
*   utils_dffs_cpu_set_attr
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Sets the general attribute field of the requested file.'
*  attr_index distinguishes between attr1, attr2 and attr3.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_set_attr(
    char*                     file_name,
    unsigned long             attribute,
    unsigned char             attr_idx,
    unsigned char             silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_set_attr");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_set_attr(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      file_name,
      attribute,
      attr_idx,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
*   utils_dffs_cpu_set_date
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Sets the date field of the requested file.
* The date is a string 8 charecters long, i.e. 03/06/06
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_set_date(
    char*                     file_name,
    char                      date[DFFS_DATE_STR_SIZE],
    unsigned char             silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_set_date");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_set_date(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      file_name,
      date,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
*   utils_dffs_cpu_set_comment
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
  utils_dffs_cpu_set_comment(
    char*                     file_name,
    char                      comment[DFFS_COMMENT_STR_SIZE],
    unsigned char             silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_set_comment");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_set_comment(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      file_name,
      comment,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}


/*****************************************************
*NAME
*   utils_dffs_cpu_print_comment
*TYPE:
*  PROC
*DATE:
*  02/07/2006
*FUNCTION:
*  Prints the comment field of the requested file to screen.
*INPUT:
*  SOC_SAND_DIRECT:
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
  utils_dffs_cpu_print_comment(
    char*                     file_name,
    unsigned char             silent
  )
{
  unsigned int
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_print_comment");
  UTILS_DFFS_CPU_ERR_IF_NOT_INITIALIZED(10);

  err =
    dffs_drv_print_comment(
      &(Utils_dffs_cpu_flash_info),
      &(Utils_dffs_cpu_descriptor),
      file_name,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*   utils_dffs_cpu_dld_and_write
*TYPE:
*  PROC
*DATE:
*  05/07/2006
*FUNCTION:
*  Downloads a file from a local host and writes it to flash.
*INPUT:
*  SOC_SAND_DIRECT:
*    char* file_name - the name of the file
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
  utils_dffs_cpu_dld_and_write(
    char* file_name,
    unsigned char silent
  )
{
  int
    file_handle;
  unsigned int
    err = SOC_SAND_OK,
    delete_cli_file = FALSE;
  char
    err_msg[8*80] = "";
  unsigned long
    host_ip,
    file_size;
  char
    *file_mem_base ;

  UTILS_INIT_ERR_DEFS("utils_dffs_cpu_dld_and_write");

  if (!silent)
  {
    soc_sand_os_printf(
      "\n\r-->Trying to download file '%s' from the TFTP server:\n\r",
      file_name
    );
  }

  host_ip = 0; /* !DUNE_BCM utils_ip_get_dld_host_ip(); */

  err =
    download_cli_file(
      file_name,
      &err_msg[0],
      host_ip
    );
  UTILS_EXIT_AND_PRINT_IF_ERR(err, silent, 10,"download_cli_file failed");

  delete_cli_file = TRUE;

  err =
    get_cli_file_name_handle(
      file_name,
      err_msg,
      &file_handle
    );
  UTILS_EXIT_AND_PRINT_IF_ERR(err, silent, 20,"get_cli_file_name_handle failed");

  err =
    get_cli_file_size(
      file_handle,
      (void *)&file_size
    );
  UTILS_EXIT_AND_PRINT_IF_ERR(err, silent, 30,"get_cli_file_size failed");

  err =
    get_cli_file_mem_base(
      file_handle,
      (void *)&file_mem_base
    );
  UTILS_EXIT_AND_PRINT_IF_ERR(err, silent, 40,"get_cli_file_mem_base failed");

  if (!silent)
  {
    soc_sand_os_printf("   Download success. File size: %lu bytes.\n\r", file_size);
  }

  err =
    utils_dffs_cpu_file_to_flash(
      file_name,
      file_size,
      file_mem_base,
      silent
    );
  UTILS_EXIT_IF_ERR(err, 50);

exit:
  if (delete_cli_file)
  {
    erase_cli_file(file_name,err_msg);
  }
  if (err)
  {
    UTILS_PRINT_MSG(silent, err_msg);
  }

  UTILS_EXIT_AND_PRINT_ERR;
}
/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif
