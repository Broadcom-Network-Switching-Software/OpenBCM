/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __DFFS_DRIVER_H_INCLUDED__
/* { */
#define __DFFS_DRIVER_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

/*************
 * INCLUDES  *
 */
/* { */
#include "dffs_low.h"
#include "dffs_descriptor.h"
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
  typedef enum {
    DFFS_SHORT_FORMAT = 0,
    DFFS_LONG_FORMAT
  } DFFS_FORMAT_TYPE;

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
  );

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
  );

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
  );

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
  );


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
  );


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
  );


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
  );

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
  );

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
  );


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
  );


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
  );


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
  );


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
  );


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
  );

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
  );


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
  );

/*
 * Calculate dffs descriptor CRC, and verify it equals the contents
 *  of the CRC field.
 * It's caller's responsibility to verify dffs_descriptor is not NULL
 */
unsigned char
  dffs_drv_desc_crc_is_valid(
    DFFS_DESCRIPTOR_TYPE* dffs_descriptor,
    unsigned char silent
  );
/* } */

#ifdef  __cplusplus
}
#endif

/* } __DFFS_DRIVER_H_INCLUDED__*/
#endif
