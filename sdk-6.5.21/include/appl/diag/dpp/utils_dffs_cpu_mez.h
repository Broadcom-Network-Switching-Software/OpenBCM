/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UTILS_DFFS_CPU_MEZ_H_INCLUDED__
/* { */
#define __UTILS_DFFS_CPU_MEZ_H_INCLUDED__

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
#include <appl/diag/dpp/dffs_low.h>
#include <appl/diag/dpp/dffs_descriptor.h>
#include <appl/diag/dpp/dffs_driver.h>
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
  );

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
  );
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
  );

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
  );


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
  );


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
  );


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
  );

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
  );

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
  );


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
  );


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
  );


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
  );


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
  );


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
  );

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
  );

/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __UTILS_DFFS_CPU_MEZ_H_INCLUDED__*/
#endif
