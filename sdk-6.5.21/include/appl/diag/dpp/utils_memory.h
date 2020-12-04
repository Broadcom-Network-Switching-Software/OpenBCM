/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef D_UTILS_MEMORY_H_INCLUDED
/* { */
#define D_UTILS_MEMORY_H_INCLUDED

#if !DUNE_BCM

/*
 * General include file for reference design.
 */
#include "Pub/include/ref_sys.h"
/*
 * INCLUDE FILES:
 */
#ifndef __DUNE_SSF__
  #include "../../../../H/usrApp.h"
#endif

#if defined(SAND_LOW_LEVEL_SIMULATION)
/* { */
#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "stdlib.h"
#include "time.h"
/*
 * Utilities include file.
 */
#include "Pub/include/utils_defx.h"
/*
 * User interface external include file.
 */
#include "Pub/include/ui_defx.h"
/*
 * Dune chips include file.
 */
#include "Pub/include/dune_chips.h"
/* } */
#else
/* { */
#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "stdlib.h"
#include "taskLib.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "tickLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "logLib.h"
#include "pipeDrv.h"
#include "timers.h"
#include "sigLib.h"
#include <usrApp.h>
/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include "private\timerLibP.h"
#include "shellLib.h"
#include "dbgLib.h"
#include "flash28.h"
/*
 * Utilities include file.
 */
#include "Pub/include/utils_defx.h"
/*
 * Dune chips include file.
 */
#include "Pub/include/dune_chips.h"
/* } */
#endif

#ifndef __DUNE_SSF__
  #include "FMC/include/fmc_common_agent.h"
#endif
#include "FMC/include/fmc_transport_layer_dcl.h"

#include "RefDesign/include/dune_rpc.h"

#endif /* !DUNE_BCM */

typedef enum
{
  XOR_OP = 0,
  OR_OP,
  AND_OP,
  NO_OPERATION
} OPERATION;

typedef enum
{
  WR_FORMAT_BYTE  = 1,
  WR_FORMAT_SHORT = 2,
  WR_FORMAT_LONG  = 4
} WR_FORMAT;
typedef struct
{
    /*
     * Name of memory block.
     */
  char         *block_name ;
    /*
     * Start address of memory block
     */
  char         *start_address ;
    /*
     * Number of bytes in memory block
     */
  unsigned long block_size ;
    /*
     * Chip select used for this block.
     */
  unsigned int chip_select_number ;
    /*
     * Number of access bits (width).
     */
  unsigned int width ;
    /*
     * Attributes for this block.
     */
  unsigned long attributes ;
    /*
     * Identifier of this block within CLI.
     */
  unsigned int block_equivalent ;
} MEMORY_BLOCK ;
typedef struct
{
    /*
     * Number of elements in array pointed to by 'memory_block'.
     */
  unsigned int num_blocks ;
    /*
     * Array of memory block descriptors.
     */
  MEMORY_BLOCK *memory_block ;
} MEMORY_MAP ;

/*
 * Object: Buff_mem
 * This object enables the user to store 'mem read' results
 * in local memory to later be retriebed. This feature enables
 * faster working with an external script agent (like proComm).
 * Storage is always in units of char (one byte).
 * {
 */
unsigned
  int
    get_buff_mem_size(
      void
    );

/*****************************************************
*NAME
*  is_buff_mem_offset_legit
*TYPE: PROC
*DATE: 12/FEB/2003
*FUNCTION:
*  Check whether specified offset and number of bytes
*  to write into or from local buffer, assigned for 'mem'
*  operations, make a legitimate set.
*CALLING SEQUENCE:
*  is_buff_mem_offset_legit(offset,val_size)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int offset -
*      Offset, in local buffer, in units of bytes,
*      which is planned to start reading from or
*      writing to.
*    unsigned int val_size -
*      Number of bytes planned to be copied.
*  SOC_SAND_INDIRECT:
*    Size of Buff_mem.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    If non zero then input was found illegal.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
 */
int
  is_buff_mem_offset_legit(
    unsigned int offset,
    unsigned int val_size
  );

/*****************************************************
*NAME
*  get_buff_mem_ptr
*TYPE: PROC
*DATE: 12/FEB/2003
*FUNCTION:
*  Get memory address of specified offset in
*  local buffer assigned for 'mem'
*  operations (copy from).
*CALLING SEQUENCE:
*  get_buff_mem_ptr(offset)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int offset -
*      Offset in local buffer (in units of bytes).
*OUTPUT:
*  SOC_SAND_DIRECT:
*    If non zero then input was found illegal and
*    operation has not been carried out.
*  SOC_SAND_INDIRECT:
*    Updated Buff_mem.
*REMARKS:
*
*SEE ALSO:
 */
char
  *get_buff_mem_ptr(
    unsigned int offset
  );

/*****************************************************
*NAME
*  write_memory
*TYPE: PROC
*DATE: 04/MARCH/2002
*FUNCTION:
*  Write specified contents into specified
*  memory address using specified format.
*CALLING SEQUENCE:
*  write_memory(addr,len,format,write_data)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *addr -
*      Memory address to start writing to.
*    unsigned int len -
*      Number of bytes to display.
*    unsigned int format -
*      Memory access method format:
*        1 - CHAR_EQUIVALENT - Byte
*        2 - SHORT_EQUIVALENT - Short
*        4 - LONG_EQUIVALENT - Long
*        Otherwise - Byte
*    unsigned long *write_data -
*      Pointer to data to write into memory.
*    unsigned int eeprom_access -
*      Flag. If 'true' then access memory using
*      special EEPROM access procedures.
*      Currently, only byte access is allowed for
*      EEPROM.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Displayed memory.
*REMARKS:
*  Calling procedure is assumed to have checked
*  legitimacy of memory range.
*SEE ALSO:
 */
void
  write_memory(
    char          *addr,
    unsigned int  len,
    WR_FORMAT     format,
    unsigned long *write_data,
    unsigned int  eeprom_access,
    unsigned int  silent
  );

/*****************************************************
*NAME
*  write_memory_do_op
*TYPE: PROC
*DATE: 04/MARCH/2002
*FUNCTION:
*  Reading specified address, do xor/and/or with specified data
*  than Write the result into specified memory address
*  using specified format.
*CALLING SEQUENCE:
*  write_memory_do_op(addr,len,format,write_data,op)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *addr -
*      Memory address to start writing to.
*    unsigned int len -
*      Number of bytes to display.
*    unsigned int format -
*      Memory access method format:
*        1 - CHAR_EQUIVALENT - Byte
*        2 - SHORT_EQUIVALENT - Short
*        4 - LONG_EQUIVALENT - Long
*        Otherwise - Byte
*    unsigned long *write_data -
*      Pointer to data to write into memory.
*    unsigned int eeprom_access -
*      Flag. If 'true' then access memory using
*      special EEPROM access procedures.
*      Currently, only byte access is allowed for
*      EEPROM.
*    OPERATION     op -
*     3 bit wise operation - XOR, AND & OR
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Displayed memory.
*REMARKS:
*  Calling procedure is assumed to have checked
*  legitimacy of memory range.
*SEE ALSO:
 */
void
  write_memory_do_op(
    char          *addr,
    unsigned int  len,
    WR_FORMAT     format,
    unsigned long *write_data,
    unsigned int  eeprom_access,
    OPERATION     op,
    unsigned int  silent
  );

unsigned
  int
    get_read_compare_error_counter(
      void
    );

void
  clear_read_compare_error_counter(
    void
  );

void
  inc_read_compare_error_counter(
    void
  );

/*****************************************************
*NAME
*  display_get_mem_counter
*TYPE: PROC
*DATE: 09/FEB/2003
*FUNCTION:
*  Display contents of specified counter related
*  to memory operation.
*CALLING SEQUENCE:
*  display_get_mem_counter(counter_id,silent)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int counter_id -
*      Identifier of counter to display. Current
*      list of counters:
*        VAL_READ_FAILURES_EQUIVALENT -
*          Number of 'val_read' failures.
*      If identifier is unknown then
*      VAL_READ_FAILURES_EQUIVALENT is used by
*      default.
*    unsigned int silent -
*      FALSE
*        Verbose all comments
*      TRUE
*        Print value with no text.
*  SOC_SAND_INDIRECT:
*    Read_compare_error_counter.
*    Last_time_rce_counter_cleared.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Displayed value.
*    Read_compare_error_counter
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  display_get_mem_counter(
    unsigned int counter_id,
    unsigned int silent
  );

/*****************************************************
*NAME
*  display_clear_mem_counter
*TYPE: PROC
*DATE: 09/FEB/2003
*FUNCTION:
*  Clear and display info on specified counter related
*  memory operation.
*CALLING SEQUENCE:
*  display_clear_mem_counter(counter_id,silent)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int counter_id -
*      Identifier of counter to clear. Current
*      list of counters:
*        VAL_READ_FAILURES_EQUIVALENT -
*          Number of 'val_read' failures.
*      If identifier is unknown then
*      VAL_READ_FAILURES_EQUIVALENT is used by
*      default.
*    unsigned int silent -
*      FALSE
*        Verbose all comments
*      TRUE
*        Print nothing.
*  SOC_SAND_INDIRECT:
*    Last_time_rce_counter_cleared.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Displayed info.
*    Last_time_rce_counter_cleared
*    Read_compare_error_counter
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  display_clear_mem_counter(
    unsigned int counter_id,
    unsigned int silent
  );

/*****************************************************
*NAME
*  read_compare_memory
*TYPE: PROC
*DATE: 06/FEB/2003
*FUNCTION:
*  Read contents of specified memory address using
*  specified format and compare (via mask) until
*  timeout has expired or match found.
*CALLING SEQUENCE:
*  read_compare_memory(addr,format,val,
*                      mask,operation,timeout,
*                      silent)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *addr -
*      Memory address to start display of.
*    unsigned int format -
*      Memory access method format:
*        1 - CHAR_EQUIVALENT - Byte
*        2 - SHORT_EQUIVALENT - Short
*        4 - LONG_EQUIVALENT - Long
*        Otherwise - Byte
*    unsigned long val -
*      Value to compare with after mask.
*    unsigned long mask -
*      Mask to apply to read data before comparing
*      with 'val'.
*    unsigned int operation -
*      Compare operation:
*        0 - EQUAL_EQUIVALENT
*        1 - NOT_EQUAL_EQUIVALENT
*    unsigned long timeout -
*      Time period, in microseconds, to carry out
*      compare operation before giving up. (Of course,
*      If match is found before that then this procedure
*      returns with 'success' indication).
*    unsigned int silent -
*      FALSE
*        Verbose all comments
*      TRUE
*        Print nothing on success but indicate error
*        or failure.
*  SOC_SAND_INDIRECT:
*    Read_compare_error_counter.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then compare operation has failed.
*      Otherwise, compare was successful.
*  SOC_SAND_INDIRECT:
*    Read memory (to compare).
*    Read_compare_error_counter
*REMARKS:
*  Calling procedure is assumed to have checked
*  legitimacy of memory range.
*SEE ALSO:
 */
int
  read_compare_memory(
    char          *addr,
    unsigned int  format,
    unsigned long val,
    unsigned long mask,
    unsigned int  operation,
    unsigned long timeout,
    unsigned int  silent
  );

/*****************************************************
*NAME
*  display_memory
*TYPE: PROC
*DATE: 24/FEB/2002
*FUNCTION:
*  Display contents of specified
*  memory address using specified formats.
*CALLING SEQUENCE:
*  display_memory(addr,len,format,disp)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *addr -
*      Memory address to start display of.
*    unsigned int len -
*      Number of bytes/shorts/longs to display.
*    unsigned int format -
*      Memory access method format:
*        1 - CHAR_EQUIVALENT - Byte
*        2 - SHORT_EQUIVALENT - Short
*        4 - LONG_EQUIVALENT - Long
*        Otherwise - Byte
*    unsigned int disp -
*      Display format:
*        1 - HEX_EQUIVALENT - hex
*        2 - UNSIGNED_INTEGER_EQUIVALENT - unsigned_integer
*        3 - SIGNED_INTEGER_EQUIVALENT - signed_integer
*        Otherwise - hex
*    unsigned int silent -
*      FALSE
*        Verbose all comments
*      TRUE
*        Print only the read memory
*    unsigned int use_mem_buff -
*      Flag. If non-zero then the next parameter
*      is meaningful and this procedure is required
*      to store read values in local buffer (for
*      future retrieval):
*      If 'use_mem_buff' is non-zero:
*        If 'use_mem_buff' is '1'
*          then the dedicated 'mem_buff' is used -
*          See get_buff_mem_ptr(). In that case
*          'mem_buff_offset' is the offest into
*          this buffer.
*        Otherwise (different than 1 and different than 0)
*          then the a general memory buffer is used
*          and 'mem_buff_offset' is its absolute
*          address.
*    unsigned long mem_buff_offset -
*      if 'use_mem_buff' is '1':
*        Offset in Buff_mem (local storage buffer) to load
*        read values into. Number of bytes to load is deduced
*        from 'len'.
*      if 'use_mem_buff' is not '1' and is also not '0':
*        General input memory buffer to load read values
*        into. Number of bytes to load is deduced
*        from 'len'.
*      This parameter is only meaningful if 'use_mem_buff'
*      is 'non-zero'.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Displayed memory.
*    loaded Buff_mem
*REMARKS:
*  Calling procedure is assumed to have checked
*  legitimacy of memory range.
*  Paraneters 'mem_buff_offset' and 'len' are
*  assumed to have been checked for legitimacy
*  (concerning Buff_mem) by the caller.
*SEE ALSO:
 */
void
  display_memory(
    char          *addr,
    unsigned int  len,
    unsigned int  format,
    unsigned int  disp,
    unsigned int  silent,
    unsigned int  use_mem_buff,
    unsigned long mem_buff_offset
  );

/*****************************************************
*NAME
*  ptr_to_offset
*TYPE: PROC
*DATE: 22/MAR/2002
*FUNCTION:
*  Convert memory address (within this system's
*  memory map) to offset from the beginning
*  of the corresponding memory block.
*CALLING SEQUENCE:
*  ptr_to_offset(mem_addr,offset_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *mem_addr -
*      Pointer to memory to which access is required.
*    unsigned int *offset_ptr -
*      This procedure will load pointed memory
*      by offset from the corresponding block.
*      only meaningful if return value does not
*      indicate error condition.
*  SOC_SAND_INDIRECT:
*    Memory map details.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    MEMORY_BLOCK * -
*      If non zero then input memory address
*      is within memory map and output
*      poiner is aimed at the corresponding
*      memory block.
*      Otherwise, if return value is NULL, input
*      pointer is not within memory map.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
MEMORY_BLOCK
  *ptr_to_offset(
    char         *mem_addr,
    unsigned int *offset_ptr
  );

/*****************************************************
*NAME
*  is_ptr_in_blocks
*TYPE: PROC
*DATE: 24/FEB/2002
*FUNCTION:
*  Indicate whether input pointer points to a memory
*  location within this system's memory map.
*CALLING SEQUENCE:
*  is_ptr_in_blocks(mem_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *mem_ptr -
*      Pointer to memory to which access is required.
*  SOC_SAND_INDIRECT:
*    Memory map details.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    MEMORY_BLOCK * -
*      If non zero then input pointer is within
*      memory map and poiner is aimed at the
*      corresponding memory block. Otherwise,
*      if return value is NULL, input pointer
*      is not within memory map.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
MEMORY_BLOCK
  *is_ptr_in_blocks(
    char *mem_ptr
  );

/*****************************************************
*NAME
*  block_id_to_ptr
*TYPE: PROC
*DATE: 03/MAR/2002
*FUNCTION:
*  Convert CLI identifier of a memory block into
*  pointer to the corresponding memory block.
*CALLING SEQUENCE:
*  block_id_to_base(block_equivalent)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int block_equivalent -
*      CLI identifier of memory block.
*  SOC_SAND_INDIRECT:
*    Memory map details.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    MEMORY_BLOCK * -
*      If non zero then this is the pointer to the
*      corresponding memory block. Otherwise,
*      if return value is NULL, no corresponding
*      block has been found.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
MEMORY_BLOCK
  *block_id_to_ptr(
    unsigned int block_equivalent
  );

/*****************************************************
*NAME
*  print_mem_banks_info
*TYPE: PROC
*DATE: 22/FEB/2002
*FUNCTION:
*  Print information on all memory banks available
*  via 'mem' command.
*CALLING SEQUENCE:
*  print_mem_banks_info()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Dune chips details.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Displayed info concerning memory banks.
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  print_mem_banks_info(
    void
  );

/*****************************************************
*NAME
*  init_mem_db
*TYPE: PROC
*DATE: 19/FEB/2002
*FUNCTION:
*  Allocate memory for and itialize all variables
*  related to the 'mem' option within CLI.
*CALLING SEQUENCE:
*  init_mem_db()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    Dune chips details.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Allocated initialized memory.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  init_mem_db(
    void
  );

/*****************************************************
*NAME
*  TYPE_MEM_CHECKER
*TYPE: PROC
*DATE: 20/MAY/2003
*FUNCTION:
*  Check memory type as entered by the user in
*  the context of 'mem ... type ' command on CLI.
*  Make sure field is valid for this system ('old'
*  or 'new' mezzanine).
*CALLING SEQUENCE:
*  type_mem_checker(
*             current_line_ptr,data_value,
*             err_msg,partial)
*INPUT:
*  SOC_SAND_DIRECT:
*    void   *current_line_ptr -
*      Pointer to a structure of type 'CURRENT_LINE':
*      current prompt line.
*    unsigned long  data_value -
*      Value entered by the user.
*    char           *err_msg -
*      This output is only meaningful when return value
*      is non-zero. Pointer to string to load with
*      explanation of why validation of input parameter
*      has failed. Caller must prepare space for
*      160 characters (incl. ending null).
*    unsigned long partial -
*      Flag.
*      If 'TRUE' then input field may be
*        partial (i.e. user has not yet filled the
*        whole field.) Check legitimacy so far.
*      If 'FALSE' then input string must be
*        a complete legitimate field. This
*        means that it should conform to all
*        legitimacy checks.
*  SOC_SAND_INDIRECT:
*    Desc_fe_reg array.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then indicated input is
*      out of range or some other error has been
*      encountered.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  type_mem_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  );

#endif /* } D_UTILS_MEMORY_H_INCLUDED */
