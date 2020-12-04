/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __DFFS_DESCRIPTOR_H_INCLUDED__
/* { */
#define __DFFS_DESCRIPTOR_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

/*************
 * INCLUDES  *
 */
/* { */

/* } */

/*************
 * DEFINES   *
 */
/* { */
/* max characters in a file name */
#define DFFS_FILENAME_SIZE      25

/* max characters in a date string */
#define DFFS_DATE_STR_SIZE      9

/* max characters in a file description comment string */
#define DFFS_COMMENT_STR_SIZE   66

/* a pattern used for dffs recognition*/
#define DFFS_SIGNATURE_PATTERN  0xAFC33CFA

/* maximal allowed number of files used in DFFS*/
#define DFFS_MAX_FILES          20
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
  enum
  {
    DFFS_VER_01 = 0,
    DFFS_NOF_VERSIONS
  } dffs_versions_enum;

  /* DFFS File Descriptor structure */
  typedef struct
  {
    char name[DFFS_FILENAME_SIZE];
    char date[DFFS_DATE_STR_SIZE];
    unsigned char version;
    char comment[DFFS_COMMENT_STR_SIZE];
    unsigned long attr1;
    unsigned long attr2;
    unsigned long attr3;
    unsigned long member_sectors;
    unsigned long total_size;
  } __ATTRIBUTE_PACKED__ DFFS_FD_TYPE;

  /* DFFS descriptor header structure */
  typedef struct
  {
    unsigned long dffs_signature;
    unsigned char version;
  } __ATTRIBUTE_PACKED__ DFFS_HDR_TYPE;

  /* DFFS descriptor structure */
  typedef struct
  {
    DFFS_HDR_TYPE header;
    unsigned long busy_sectors;       /* occupied sectors bitmap */
    unsigned long sectors_available;
    DFFS_FD_TYPE  fd[DFFS_MAX_FILES];
    unsigned short crc16;
  } __ATTRIBUTE_PACKED__ DFFS_DESCRIPTOR_TYPE;

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


#ifdef  __cplusplus
}
#endif


/* } __DFFS_DESCRIPTOR_H_INCLUDED__*/
#endif
