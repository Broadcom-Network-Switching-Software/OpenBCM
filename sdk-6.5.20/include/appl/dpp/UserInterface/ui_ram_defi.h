/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_RAM_DEFI_INCLUDED
/* { */
#define UI_RAM_DEFI_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/****************************************************************
*NAME
*  HISTORY_FIFO
*TYPE: BUFFER
*DATE: 31/JAN/2002
*FUNCTION:
*  STRUCTURE CONTAINING information on
*  the history fifo of the CLI.
*ORGANIZATION:
*  STRUCTURE OF TYPE 'HISTORY_FIFO':
    unsigned int index -
      Next index to load in '*history_array'.
      Range: 0 -> (elements_in_array - 1)
    unsigned int num_loaded -
      Number of meaningful entries in '*history_array'.
      Can be up to 'elements_in_array'. Newest entry
      is pointed by (index - 1) [modulo the size of
      the array].
    unsigned int elements_in_array -
      Number of entries in 'history_array'.
    CURRENT_LINE **history_array -
      Array of pointers to structures of
      type CURRENT_LINE.
*USAGE:
*  See ORGANIZATION.
*REMARKS:
*  None.
*SEE ALSO:
 */
EXTERN
  HISTORY_FIFO
    History_fifo ;
/****************************************************************
*NAME
*  CURRENT_LINE_PTR
*TYPE: BUFFER
*DATE: 15/JAN/2002
*FUNCTION:
*  POINTER TO STRUCTURE CONTAINING information on
*  the current line being entered using CLI.
*ORGANIZATION:
*  POINTER TO STRUCTURE OF TYPE 'CURRENT_LINE':
*USAGE:
*  T. B. D.
*REMARKS:
*  This structure is updated as the user types in characters
*  on the current CLI line. It is cleared when 'enter' is hit
*  (on a line without errors) or when changing mode
*  (telnet/local terminal)
*SEE ALSO:
 */
EXTERN
  CURRENT_LINE
    *Current_line_ptr ;
/********************************************************
*NAME
*  Subjects_list
*TYPE: BUFFER
*DATE: 21/JAN/2002
*FUNCTION:
*  Pointer to array containing the  list of
*  subject and related parameters.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'SUBJECT':
*    int   subject_id -
*      Identifier of this subject.
*    char *subj_name -
*      Ascii presentation of subject.
*    PARAM *allowed_params -
*      Array of structures of type PARAM. This is
*      the list of parameters allowed for that
*      subject.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  This pointer initially points to the initial list
*  of subjects in rom. At later stages, it may point to
*  a RAM array, containing subjects added on line.
*SEE ALSO:
 */
EXTERN
  SUBJECT
    *Subjects_list ;
/********************************************************
*NAME
*  Subjects_ascii_list
*TYPE: BUFFER
*DATE: 21/JAN/2002
*FUNCTION:
*  Pointer to ascii array (null terminated)containing
*  the names of all subjects, separated by CR/LF.
*ORGANIZATION:
*  Standard 'C' string.
*USAGE:
*  SEE FUNCTION.
*REMARKS:
*  This pointer initially points to the initial list
*  prepared from subjects in rom (using malloc). If
*  subjects are added on line, memory should be released
*  and reallocated for updated list.
*SEE ALSO:
 */
EXTERN
  char
    *Subjects_ascii_list ;
/********************************************************
*NAME
*  Num_subjects
*TYPE: BUFFER
*DATE: 21/JAN/2002
*FUNCTION:
*  Number of subjects in the system.
*ORGANIZATION:
*  Unsigned int.
*USAGE:
*  SEE FUNCTION.
*REMARKS:
*  This number is initially deduced from the initial list
*  prepared from subjects in rom. If subjects are added
*  on line, this number must be updated.
*SEE ALSO:
 */
EXTERN
  unsigned int
    Num_subjects ;
/********************************************************
*NAME
*  Memory_map
*TYPE: BUFFER
*DATE: 24/FEB/2002
*FUNCTION:
*  Pointer to array containing the  list of
*  memory blocks and related parameters.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'MEMORY_MAP':
*    See ui_pure_defi.h.
*USAGE:
*  See ui_pure_defi.h.
*REMARKS:
*  None.
*SEE ALSO:
 */
EXTERN
  MEMORY_MAP
    *Memory_map ;
/* } */
#endif
