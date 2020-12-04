/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>


#if !DUNE_BCM

/*
 * INCLUDE FILES:
 */
#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defx.h>
/*
 * User interface external include file.
 */
#include <appl/diag/dpp/ui_defx.h>
/*
 * User interface internal include file.
 */
#include <appl/dpp/UserInterface/ui_defi.h>
/*
 * Dune chips include file.
 */
#include <appl/diag/dpp/dune_chips.h>
/*
 * utilities for "ui_general".
 */
#include <ui_general_utils.h>
/* } */
#else
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <taskLib.h>
#include <errnoLib.h>
#include <usrLib.h>
#include <tickLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <logLib.h>
#include <pipeDrv.h>
#include <timers.h>
#include <sigLib.h>
#include <cacheLib.h>
#include <drv/mem/eeprom.h>

#include <tftpLib.h>
/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include <private\timerLibP.h>
#include <shellLib.h>
#include <dbgLib.h>
/*
 * Definitions specific to reference system.
 */
#include <usrapp.h>
/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defx.h>
/*
 * User interface external include file.
 */
#include <appl/diag/dpp/ui_defx.h>
/*
 * User interface internal include file.
 */
#include <appl/dpp/UserInterface/ui_defi.h>
/*
 * Dune chips include file.
 */
#include <appl/diag/dpp/dune_chips.h>
/* } */
#endif
#endif /* !DUNE_BCM */

#include <appl/dpp/UserInterface/ui_cli_files.h>

#include <appl/diag/dpp/utils_string.h>
#if !DUNE_BCM

#include <appl/diag/dpp/utils_ip_mgmt.h>
#else


#if defined(LINUX) || defined(SAND_LOW_LEVEL_SIMULATION)
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#endif

#endif /* !DUNE_BCM */


typedef struct
{
    /*
     * Flag. If 'true' then file has been loaded and is in memory.
     * All elements below are only meaningful if 'loaded' is
     * non-zero.
     */
  unsigned int  loaded ;
    /*
     * Name of file on downloading host and on this system. This
     * is a null terminated string.
     */
  char          file_name[CLI_FILE_NAME_LEN + 1] ;
    /*
     * Base address of memory assigned to this file.
     */
  unsigned char *file_mem_base ;
    /*
     * Number of memory bytes assigned to this file.
     */
  unsigned long  file_size ;
    /*
     * Indication on the current number of users which have opened
     * this file.
     */
  unsigned int  file_opened_counter ;
} CLI_FILE_DESC ;
/*
 * Descriptors of all CLI files in the system.
 */
static
  CLI_FILE_DESC
    Cli_files[NUM_CLI_FILES] ;

static
  uint8
    Ui_file_n = 0;
/*****************************************************
*NAME
*  init_cli_files
*TYPE: PROC
*DATE: 05/MARCH/2003
*FUNCTION:
*  Initialize CLI file system.
*CALLING SEQUENCE:
*  init_cli_files()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  init_cli_files(
    void
  )
{
  int
    ret ;
  unsigned
    int
      ui ;
  CLI_FILE_DESC
    *cli_file_desc ;
  ret = 0 ;
  cli_file_desc = &Cli_files[0] ;
  for (ui = 0 ; ui < get_max_num_cli_files() ; ui++, cli_file_desc++)
  {
    cli_file_desc->loaded = FALSE ;
    /*
     * Just making sure.
     */
    memset(cli_file_desc->file_name,0,sizeof(cli_file_desc->file_name)) ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  get_num_loaded_cli_files
*TYPE: PROC
*DATE: 05/MARCH/2003
*FUNCTION:
*  Get number of loaded files in CLI file system.
*CALLING SEQUENCE:
*  get_num_loaded_cli_files(num_loaded_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    int *num_loaded_ptr -
*      This procedure loads pointed memory with number
*      of loaded files. (only meaningful if return
*      value is zero).
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*    num_loaded_ptr
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  get_num_loaded_cli_files(
    int *num_loaded_ptr
  )
{
  int
    num_loaded,
    ret ;
  unsigned
    int
      ui ;
  CLI_FILE_DESC
    *cli_file_desc ;
  ret = 0 ;
  cli_file_desc = &Cli_files[0] ;
  for (ui = 0, num_loaded = 0 ;
                ui < get_max_num_cli_files() ; ui++, cli_file_desc++)
  {
    if (cli_file_desc->loaded)
    {
      num_loaded++ ;
    }
  }
  *num_loaded_ptr = num_loaded ;
  return (ret) ;
}
/*****************************************************
*NAME
*  get_max_num_cli_files
*TYPE: PROC
*DATE: 05/MARCH/2003
*FUNCTION:
*  Get maximal number of files available in CLI
*  file system.
*CALLING SEQUENCE:
*  get_max_num_cli_files()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int -
*      Maximal number of available CLI files.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*REMARKS:
*  None.
*SEE ALSO:
 */
unsigned int
  get_max_num_cli_files(
    void
  )
{
  return (NUM_CLI_FILES) ;
}
/*****************************************************
*NAME
*  get_max_cli_file_size
*TYPE: PROC
*DATE: 07/MARCH/2003
*FUNCTION:
*  Get maximal number of bytes in one file on CLI
*  file system.
*CALLING SEQUENCE:
*  get_max_cli_file_size()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    CLI definitions.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int -
*      Maximal number of bytes in one CLI file.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
unsigned int
  get_max_cli_file_size(
    void
  )
{
  return (SIZEOF_CLI_FILE) ;
}
/*****************************************************
*NAME
*  get_max_cli_filename_size
*TYPE: PROC
*DATE: 07/MARCH/2003
*FUNCTION:
*  Get maximal number of characters in any file name
*  on CLI file system.
*CALLING SEQUENCE:
*  get_max_cli_filename_size()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    CLI definitions.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    unsigned int -
*      Maximal number of characters in one CLI
*      filename.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None.
*SEE ALSO:
 */
unsigned int
  get_max_cli_filename_size(
    void
  )
{
  return (CLI_FILE_NAME_LEN) ;
}
/*****************************************************
*NAME
*  get_cli_file_loaded
*  get_cli_file_name
*  get_cli_file_size
*  get_cli_file_mem_base
*TYPE: PROC
*DATE: 05/MARCH/2003
*FUNCTION:
*  Get information on various items related to CLI
*  file, specified by its handle.
*CALLING SEQUENCE:
*  get_cli_file_loaded(handle,val)
*  get_cli_file_name(handle,val)
*  get_cli_file_size(handle,val)
*  get_cli_file_mem_base(handle,val)
*INPUT:
*  SOC_SAND_DIRECT:
*    int handle -
*      Identifier of file to get information on.
*    void *val -
*      This procedure loads pointed memory by required
*      value (depending on required item):
*        for get_cli_file_loaded -
*          Integer is loaded
*        for get_cli_file_name   -
*          Char pointer is loaded
*        for get_cli_file_size   -
*          Unsigned long is loaded
*        for get_cli_file_mem_base -
*          Unsigned char pointer is loaded
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred and
*      value could not be retrieved.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*    See 'val'
*REMARKS:
*  See CLI_FILE_DESC above for detailed explanation
*  of each parameter.
*SEE ALSO:
 */
int
  get_cli_file_loaded(
    int   handle,
    void  *val
  )
{
  int
    *val_ptr,
    ret ;
  ret = 0 ;
  if ((unsigned int)handle >= (unsigned int)get_max_num_cli_files())
  {
    ret = 1 ;
    goto exit ;
  }
  val_ptr = (int *)val ;
  *val_ptr = Cli_files[handle].loaded ;
exit:
  return (ret) ;
}
int
  get_cli_file_name(
    int   handle,
    void  *val
  )
{
  int
    ret ;
  char
    **val_ptr ;
  CLI_FILE_DESC
    *cli_file_desc ;
  ret = 0 ;
  if ((unsigned int)handle >= (unsigned int)get_max_num_cli_files())
  {
    ret = 1 ;
    goto exit ;
  }
  cli_file_desc = &Cli_files[handle] ;
  val_ptr = (char **)val ;
  if (cli_file_desc->loaded)
  {
    *val_ptr = Cli_files[handle].file_name ;
  }
  else
  {
    ret = 2 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
int
  get_cli_file_mem_base(
    int   handle,
    void  *val
  )
{
  int
    ret ;
  unsigned
    char
      **val_ptr ;
  CLI_FILE_DESC
    *cli_file_desc ;
  ret = 0 ;
  if ((unsigned int)handle >= (unsigned int)get_max_num_cli_files())
  {
    ret = 1 ;
    goto exit ;
  }
  cli_file_desc = &Cli_files[handle] ;
  val_ptr = (unsigned char **)val ;
  if (cli_file_desc->loaded)
  {
    *val_ptr = Cli_files[handle].file_mem_base ;
  }
  else
  {
    ret = 2 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
int
  get_cli_file_size(
    int   handle,
    void  *val
  )
{
  int
    ret ;
  unsigned
    long
      *val_ptr ;
  CLI_FILE_DESC
    *cli_file_desc ;
  ret = 0 ;
  if ((unsigned int)handle >= (unsigned int)get_max_num_cli_files())
  {
    ret = 1 ;
    goto exit ;
  }
  cli_file_desc = &Cli_files[handle] ;
  val_ptr = (unsigned long *)val ;
  if (cli_file_desc->loaded)
  {
    *val_ptr = Cli_files[handle].file_size ;
  }
  else
  {
    ret = 2 ;
    goto exit ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  get_cli_file_info
*TYPE: PROC
*DATE: 05/MARCH/2003
*FUNCTION:
*  Get human readable ASCII information on CLI
*  file, specified by its handle.
*CALLING SEQUENCE:
*  get_cli_file_info(handle,description)
*INPUT:
*  SOC_SAND_DIRECT:
*    int handle -
*      Identifier of file to get information on.
*    char *description -
*      This procedure loads pointed memory by human
*      readable string (null terminated). Caller
*      must prepare a buffer whose size, in bytes,
*      is, at least, 320. Description always ends
*      with CR-LF.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred and
*      information could not be retrieved.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*    See 'description'
*REMARKS:
*  See CLI_FILE_DESC above for detailed explanation
*  of each parameter.
*SEE ALSO:
 */
int
  get_cli_file_info(
    int   handle,
    char  *description
  )
{
  int
    ret ;
  CLI_FILE_DESC
    *cli_file_desc ;

  ret = 0 ;
  if ((unsigned int)handle >= (unsigned int)get_max_num_cli_files())
  {
    ret = 1 ;
    goto exit ;
  }
  cli_file_desc = &Cli_files[handle] ;
  if (!(cli_file_desc->loaded))
  {
    sal_sprintf(description,
      "This file descriptor is empty\r\n"
      ) ;
    goto exit ;
  }
  sal_sprintf(description,
      "File name          : %s\r\n"
      "File size          : %lu bytes\r\n"
      "File memory address: 0x%08lX\r\n",
      cli_file_desc->file_name,
      cli_file_desc->file_size,
      (unsigned long)cli_file_desc->file_mem_base
      ) ;
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  get_cli_file_system_info
*TYPE: PROC
*DATE: 07/MARCH/2003
*FUNCTION:
*  Get human readable ASCII information on the whole
*  CLI file system.
*CALLING SEQUENCE:
*  get_cli_file_system_info(description)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *description -
*      This procedure loads pointed memory by human
*      readable string (null terminated). Caller
*      must prepare a buffer whose size, in bytes,
*      is, at least, 400. Description always ends
*      with CR-LF.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred and
*      information could not be retrieved.
*  SOC_SAND_INDIRECT:
*    CLI static structures and definitions.
*    See 'description'
*REMARKS:
*  See CLI_FILE_DESC above for detailed explanation
*  of each parameter.
*SEE ALSO:
 */
int
  get_cli_file_system_info(
    char  *description
  )
{
  int
    err,
    ret ;
  unsigned
    int
      num_loaded_cli_files ;
  ret = 0 ;
  err =
    get_num_loaded_cli_files((int *)&num_loaded_cli_files) ;
  if (err)
  {
    ret = 1 ;
    goto exit ;
  }
  sal_sprintf(description,
      "Number of available file descriptors: %lu\r\n"
      "Number of currenly loaded files     : %lu\r\n"
      "Maximal size of file name           : %lu characters\r\n"
      "Maximale size of one file           : %lu bytes\r\n",
      (unsigned long)get_max_num_cli_files(),
      (unsigned long)num_loaded_cli_files,
      (unsigned long)get_max_cli_filename_size(),
      (unsigned long)get_max_cli_file_size()
      ) ;
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  get_first_free_cli_file_handle
*TYPE: PROC
*DATE: 05/MARCH/2003
*FUNCTION:
*  Get handle (index) of first file which is NOT loaded
*  from the array of descriptors of CLI files.
*CALLING SEQUENCE:
*  get_first_free_cli_file_handle(handle)
*INPUT:
*  SOC_SAND_DIRECT:
*    int *handle -
*      This procedure loads pointed memory by the handle
*      (index) of the first free entry (If there is any,
*      i.e., If return value is zero).
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If negative then some error has occurred. If
*      positive then no such entry exists. Otherwise
*      *handle is loaded by index of the first free
*      entry (=handle).
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  get_first_free_cli_file_handle(
    int *handle
  )
{
  int
    ret ;
  unsigned
    int
      ui ;
  CLI_FILE_DESC
    *cli_file_desc ;
  ret = 1 ;
  cli_file_desc = &Cli_files[0] ;
  for (ui = 0 ; ui < get_max_num_cli_files() ; ui++, cli_file_desc++)
  {
    if (!(cli_file_desc->loaded))
    {
      *handle = (int)ui ;
      goto exit ;
    }
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  get_cli_file_name_handle
*TYPE: PROC
*DATE: 05/MARCH/2003
*FUNCTION:
*  Check whether specified name is an existing CLI
*  file name and, if it is, return handle (index)
*  identifying it.
*CALLING SEQUENCE:
*  get_cli_file_name_handle(file_name,err_msg,handle)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *file_name -
*      Pointer to name of file to test. This is a
*      null terminated string whose size (incl.
*      terminating null) must be less than
*      (CLI_FILE_NAME_LEN+1)
*    char *err_msg -
*      If an error is encountered then, on output,
*      this program will load pointed memory by
*      a null terminated string with a description
*      of the error.
*      Calling program is supposed to prepare
*      a memry block, for that, of size 160 or more.
*    int  *handle -
*      This procedure loads pointed memory with handle
*      of file specified by 'file_name'. Only
*      meaningful if direct output is zero.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If negative then some error has occurred. If
*      positive then no such name has been found.
*      Otherwise *handle is loaded with required handle.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*    err_msg
*    handle
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  get_cli_file_name_handle(
    const char *file_name,
          char *err_msg,
          int  *handle
  )
{
  int
    ret ;
  unsigned
    int
      ui ;
  char
    local_file_name[CLI_FILE_NAME_LEN + 11] ;
  CLI_FILE_DESC
    *cli_file_desc ;
  ret = 1 ;
  err_msg[0] = 0 ;
  local_file_name[sizeof(local_file_name) - 1] = 0 ;
  strncpy(local_file_name,file_name,sizeof(local_file_name) - 1) ;
  if (strlen(local_file_name) > CLI_FILE_NAME_LEN)
  {
    /*
     * Enter if specified file name is too long.
     */
    sal_sprintf(err_msg,
          "get_cli_file_name_handle():\r\n"
          "  Input file name (starting: %s) is too long (greater than %u)\r\n",
          local_file_name,(unsigned int)CLI_FILE_NAME_LEN) ;
    ret = -1 ;
    goto exit ;
  }
  cli_file_desc = &Cli_files[0] ;
  for (ui = 0 ; ui < get_max_num_cli_files() ; ui++, cli_file_desc++)
  {
    if (cli_file_desc->loaded)
    {
      if (strcmp(cli_file_desc->file_name,file_name) == 0)
      {
        *handle = (int)ui ;
        ret = 0 ;
        goto exit ;
      }
    }
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  start_cli_file_running
*TYPE: PROC
*DATE: 05/MARCH/2003
*FUNCTION:
*  Start a process which will cause existing CLI
*  file, specified by handle, to be executed by
*  CLI interpreter (handle_next_char()).
*CALLING SEQUENCE:
*  start_cli_file_running(handle,err_msg)
*INPUT:
*  SOC_SAND_DIRECT:
*    int  handle -
*      Handle of file to use as input to CLI
*      interpreter.
*    char *err_msg -
*      If an error is encountered then, on output,
*      this program will load pointed memory by
*      a null terminated string with a description
*      of the error.
*      Calling program is supposed to prepare
*      a memry block, for that, of size 160 or more.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If negative then some error has occurred. If
*      positive then file descriptor is not loaded
*      but any downloaded file.
*      Otherwise handling process will start on the
*      next cycle of sys_getch().
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*    err_msg
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  start_cli_file_running(
    int  handle,
    char *err_msg
  )
{
  int
    err,
    ret ;
  CLI_FILE_DESC
    *cli_file_desc ;

  ret = 0 ;
  err_msg[0] = 0 ;
  if ((unsigned int)handle >= get_max_num_cli_files())
  {
    sal_sprintf(err_msg,
      "start_cli_file_running():\r\n"
      "  File handle (%lu) is beyond range of assigned handles (0 -> %lu)\r\n",
      (unsigned long)handle,(unsigned long)(get_max_num_cli_files() - 1)
      ) ;
    ret = -1 ;
    goto exit ;
  }
  cli_file_desc = &Cli_files[handle] ;
  if (!(cli_file_desc->loaded))
  {
    sal_sprintf(err_msg,
      "start_cli_file_running():\r\n"
      "  File handle (%lu) related to a descriptor which is currently empty.\r\n",
      (unsigned long)handle
      ) ;
    ret = 1 ;
    goto exit ;
  }
  /*
   * Now open the file.
   */
  err = open(cli_file_desc->file_name,O_RDONLY,0) ;
  if (err == ERROR)
  {
    sal_sprintf(err_msg,
      "start_cli_file_running():\r\n"
      "  Failed to open file (%s)\r\n",
      cli_file_desc->file_name
      ) ;
    ret = -2 ;
    goto exit ;
  }
  {
    unsigned
      long
        time_from_startup ;
    set_cli_input_fd(err) ;
    set_file_input_mode() ;
    clear_cli_times_available() ;
    time_from_startup = get_watchdog_time(TRUE) ;
    set_cli_start_time(time_from_startup) ;
    putc('\n', stdin);
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  download_cli_file
*TYPE: PROC
*DATE: 05/MARCH/2003
*FUNCTION:
*  Download a file from assigned TFTP server and store
*  in local ram.
*CALLING SEQUENCE:
*  download_cli_file(file_name,err_msg)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *file_name -
*      Pointer to name of file on TFTP server and also
*      on this system. This is a null terminated string
*      whose size must be less than (CLI_FILE_NAME_LEN+1)
*    char *err_msg -
*      If an error is encountered then, on output,
*      this program will load pointed memory by
*      a null terminated string with a description
*      of the error.
*      Calling program is supposed to prepare
*      a memry block, for that, of size 320 or more.
*   unsigned long host_ip -
*     The ip of the host from which the file would
*      be downloaded from.
*     If the value of the host_ip is INVALID_UNICAST_IP
*     (0xFFFFFFFF), the host_ip would be the default
*     downloading host IP.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*    err_msg
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  download_cli_file(
    const char *file_name,
          char *err_msg,
        unsigned long host_ip
  )
{
  int
    num_loaded,
    file_index,
    ret ;
  unsigned
    int
      max_cli_file_size,
      originally_zipped,
      release_device,
      err,
      ui ;
  char
    *local_mem_base,
    *mem_base,
    local_file_name[CLI_FILE_NAME_LEN + 3],
    local_name_n_offset[2][CLI_FILE_NAME_LEN + 11] ;
  CLI_FILE_DESC
    *cli_file_desc ;
  char
    ascii_host_ip[4*4+2] ;
  ret = 0 ;
  err_msg[0] = 0 ;
  release_device = FALSE ;
  get_num_loaded_cli_files(&num_loaded) ;
  if (num_loaded >= get_max_num_cli_files())
  {
    /*
     * Enter if all file entries are occupied.
     */
    sal_sprintf(err_msg,
      "download_cli_file():\r\n"
      "  All CLI file entries (%lu) are already loaded\r\n",
      (unsigned long)num_loaded
      ) ;
    ret = 1 ;
    goto exit ;
  }
  err = get_cli_file_name_handle(file_name,err_msg,&file_index) ;
  if (err == 0)
  {
    /*
     * Enter if there already is such a file name.
     * Reject request and exit.
     */
    sal_sprintf(err_msg,
      "download_cli_file():\r\n"
      "  Specified file name (%s) already exists (handle=%lu)\r\n",
      file_name,
      (unsigned long)file_index
      ) ;
    ret = 2 ;
    goto exit ;
  }
  else if (err < 0)
  {
    /*
     * Enter if some error has been detected. This could,
     * for example, be due to a too long input file name.
     * Reject request and exit.
     */
    ui = strlen(err_msg) ;
    sal_sprintf(&err_msg[ui],
      "download_cli_file():\r\n"
      "  get_cli_file_name_handle() reported an error\r\n"
      ) ;
    ret = 3 ;
    goto exit ;
  }
  /*
   * At this point, there must be a free entry and specified name
   * is legal.
   */
  get_first_free_cli_file_handle(&file_index) ;
  cli_file_desc = &Cli_files[file_index] ;
  /*
   * Set flag to try and release this device, which is only temporary,
   * when exitting this procedure.
   */
  release_device = TRUE ;
  /*
   * Create a 'local device name' which is the (large) memory block
   * initially assign for the downloaded file. Later, its contents
   * are copied to a smaller file, of size as exactly needed, and
   * the original is destroyed.
   */
  strcpy(local_file_name,file_name) ;
  ui = strlen(local_file_name) ;
  strcpy(&local_file_name[ui],"_1") ;
  /*
   * Make sure size of assigned memory is is larger than assigned
   * maximal file size (multiplied by 2).
   */
  max_cli_file_size = get_max_cli_file_size() ;
  err = create_mem_file_system(
            max_cli_file_size * 2,local_file_name,
            local_name_n_offset[0],&local_mem_base,err_msg) ;
  if (err)
  {
    ui = strlen(err_msg) ;
    sal_sprintf(&err_msg[ui],
      "download_cli_file():\r\n"
      "  Memory device creation fail (name: %s)!\r\n",
      local_file_name
      ) ;
    ret = 4 ;
    goto exit ;
  }
  if( (host_ip == INVALID_UNICAST_IP) ||
      (host_ip == DEFAULT_DOWNLOAD_IP)
    )
  {
#if !DUNE_BCM

    host_ip = utils_ip_get_dld_host_ip();
#else
    tgt_host_from_nv(&host_ip);
#endif
  err = (host_ip == 0);
  if (err)
  {
    sal_sprintf(err_msg,
      "download_cli_file():\r\n"
      "  Failed to get IP address of downloading host\r\n"
      ) ;
    ret = 5 ;
    goto exit ;
  }
  }
  sprint_ip(ascii_host_ip,host_ip, TRUE) ;
  sal_sprintf(err_msg,
      "download_cli_file():\r\n"
      "  Failed to load from downloading host:\r\n"
      ) ;
  ui = strlen(err_msg) ;
  err =
    load_memory_file(0,0,
        ascii_host_ip, (char*)file_name,
        0, local_file_name,&err_msg[ui],&num_loaded,
        FALSE,&originally_zipped) ;
  if (err)
  {
    /*
     * If error occurred we return an error to the user,
     * so here we clear the TFTP 'errno' of OS.
     */
    errnoSet(0);
    ret = 6 ;
    goto exit ;
  }
  if ((unsigned int)num_loaded > max_cli_file_size)
  {
    sal_sprintf(err_msg,
      "download_cli_file():\r\n"
      "  Size of downloaded file (%lu) is larger than maximal\r\n"
      "  size allowed on this CLI file system (%lu).\r\n",
      (unsigned long)num_loaded,
      (unsigned long)max_cli_file_size
      ) ;
    ret = 7 ;
    goto exit ;
  }
  /*
   * At this point, we have a local file which occupies too much
   * memory. Create a file of the right size.
   */
  err = create_mem_file_system(
            num_loaded, (char*)file_name,
            local_name_n_offset[1],&mem_base,err_msg) ;
  if (err)
  {
    struct
      stat
        tmp_stat ;
    if (!stat(local_file_name, &tmp_stat))
    {
      /*
       * Enter if local file (device) has been created and, therefore,
       * needs to be deleted (and its memory freed).
       */
#if !DUNE_BCM
      memDevDelete((char*)file_name) ;

#endif
      free(mem_base) ;
    }
    ui = strlen(err_msg) ;
    sal_sprintf(&err_msg[ui],
      "download_cli_file():\r\n"
      "  Memory device creation fail (name: %s)!\r\n",
      file_name
      ) ;
    ret = 8 ;
    goto exit ;
  }
  /*
   * Now copy useful data from original (large) file to new (small) file.
   * We could use copy() by memcpy is faster.
   */
  memcpy(mem_base,local_mem_base,num_loaded) ;
  /*
   * Update local data base (for CLI files).
   */
  strcpy(cli_file_desc->file_name,file_name) ;
  cli_file_desc->file_mem_base = mem_base ;
  cli_file_desc->file_size = (unsigned long)num_loaded ;
  cli_file_desc->file_opened_counter = 0 ;
  cli_file_desc->loaded = TRUE ;
exit:
  if (release_device)
  {
    struct
      stat
        tmp_stat ;
    if (!stat(local_file_name, &tmp_stat))
    {
      /*
       * Enter if local file (device) has been created and, therefore,
       * needs to be deleted (and its memory freed).
       */
#if !DUNE_BCM
      memDevDelete(local_file_name) ;

#endif
      free(local_mem_base) ;
    }
  }
  if (ret)
  {
    d_printf("   -. Download: %-25s.....     fail\n", file_name);
  }
  else
  {
    d_printf("   -. Download: %-25s.....     success\n", file_name);

    /*
     * Precaution...
     */
    err_msg[0] = 0 ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  load_cli_file_from_memory
*TYPE: PROC
*DATE: 17/JUL/2003
*FUNCTION:
*  load a file from assigned memory buffer and store
*  in local ram.
*CALLING SEQUENCE:
*  load_cli_file_from_memory(file_name,inp_mem_base,inp_mem_size,err_msg)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *file_name -
*      Pointer to name of file on TFTP server and also
*      on this system. This is a null terminated string
*      whose size must be less than (CLI_FILE_NAME_LEN+1)
*    char *inp_mem_base -
*      start address of the memory area to copy to file
*    char *inp_mem_size -
*      size in bytes of the memory area to copy to file
*    char *err_msg -
*      If an error is encountered then, on output,
*      this program will load pointed memory by
*      a null terminated string with a description
*      of the error.
*      Calling program is supposed to prepare
*      a memry block, for that, of size 320 or more.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*    err_msg
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  load_cli_file_from_memory(
    char *file_name,
    char *inp_mem_base,
    int   inp_mem_size,
    char *err_msg
  )
{
  int          num_loaded, file_index, ret;
  unsigned int err, ui;
  char *mem_base;
  char  local_name_offset[CLI_FILE_NAME_LEN + 11];
  CLI_FILE_DESC *cli_file_desc;
  /**/
  ret = 0 ;
  err_msg[0] = 0 ;
  get_num_loaded_cli_files(&num_loaded);
  if (num_loaded >= get_max_num_cli_files())
  {
    /*
     * Enter if all file entries are occupied.
     */
    sal_sprintf(err_msg,
      "load_cli_file_from_memory():\r\n"
      "  All CLI file entries (%lu) are already loaded\r\n",
      (unsigned long)num_loaded
      ) ;
    ret = 1 ;
    goto exit ;
  }
  err = get_cli_file_name_handle(file_name,err_msg,&file_index);
  if (err == 0)
  {
    /*
     * Enter if there already is such a file name.
     * Reject request and exit.
     */
    sal_sprintf(err_msg,
      "load_cli_file_from_memory():\r\n"
      "  Specified file name (%s) already exists (handle=%lu)\r\n",
      file_name,
      (unsigned long)file_index
      ) ;
    ret = 2 ;
    goto exit ;
  }
  else if (err < 0)
  {
    /*
     * Enter if some error has been detected. This could,
     * for example, be due to a too long input file name.
     * Reject request and exit.
     */
    ui = strlen(err_msg);
    sal_sprintf(&err_msg[ui],
      "load_cli_file_from_memory():\r\n"
      "  get_cli_file_name_handle() reported an error\r\n"
      );
    ret = 3;
    goto exit;
  }
  /*
   * At this point, there must be a free entry and specified name
   * is legal.
   */
  get_first_free_cli_file_handle(&file_index);
  cli_file_desc = &Cli_files[file_index];
  err = create_mem_file_system(
            inp_mem_size,file_name,
            local_name_offset,&mem_base,err_msg) ;
  if (err)
  {
    ui = strlen(err_msg) ;
    sal_sprintf(&err_msg[ui],
      "load_cli_file_from_memory():\r\n"
      "  Memory device creation fail (name: %s)!\r\n",
      file_name
      ) ;
    ret = 8 ;
    goto exit ;
  }
  /*
   * Now copy useful data from original file to new file.
   * We could use copy() by memcpy is faster.
   */
  memcpy(mem_base,inp_mem_base,inp_mem_size) ;
  /*
   * Update local data base (for CLI files).
   */
  strcpy(cli_file_desc->file_name,file_name) ;
  cli_file_desc->file_mem_base = mem_base ;
  cli_file_desc->file_size = (unsigned long)inp_mem_size ;
  cli_file_desc->file_opened_counter = 0 ;
  cli_file_desc->loaded = TRUE ;
exit:
  if (!ret)
  {
    /*
     * Precaution...
     */
    err_msg[0] = 0 ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  erase_cli_file
*TYPE: PROC
*DATE: 05/MARCH/2003
*FUNCTION:
*  Erase a file from CLI file system (in local ram).
*CALLING SEQUENCE:
*  erase_cli_file(file_name,err_msg)
*INPUT:
*  SOC_SAND_DIRECT:
*    char *file_name -
*      Pointer to name of file on this system. This
*      is a null terminated string  whose size
*      must be less than (CLI_FILE_NAME_LEN+1)
*    char * -
*      If an error is encountered then, on output,
*      this program will load pointed memory by
*      a null terminated string with a description
*      of the error.
*      Calling program is supposed to prepare
*      a memry block, for that, of size 320 or more.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*    err_msg
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  erase_cli_file(
    const char *file_name,
          char *err_msg
  )
{
  int
    file_index,
    ret ;
  unsigned
    int
      err,
      ui ;
  char
    *mem_base ;
  CLI_FILE_DESC
    *cli_file_desc ;
  ret = 0 ;
  err_msg[0] = 0 ;
  sal_sprintf(err_msg,
      "erase_cli_file():\r\n"
      "  get_cli_file_name_handle() reported an error\r\n"
      ) ;
  ui = strlen(err_msg) ;
  err = get_cli_file_name_handle(file_name,&err_msg[ui],&file_index) ;
  if (err > 0)
  {
    /*
     * Enter if there is no such a file name.
     * Reject request and exit.
     */
    sal_sprintf(err_msg,
      "erase_cli_file():\r\n"
      "  Specified file name (%s) could not be found\r\n",
      file_name
      ) ;
    ret = 1 ;
    goto exit ;
  }
  else if (err < 0)
  {
    /*
     * Enter if some error has been detected. This could,
     * for example, be due to a too long input file name.
     * Reject request and exit.
     */
    ret = 2 ;
    goto exit ;
  }
  /*
   * At this point, there must be a file with specified name.
   */
  cli_file_desc = &Cli_files[file_index] ;
  cli_file_desc->loaded = FALSE ;
  mem_base = cli_file_desc->file_mem_base ;
  /*
   * Precaution...
   */
  memset(cli_file_desc->file_name,0,sizeof(cli_file_desc->file_name)) ;
  cli_file_desc->file_mem_base = (unsigned char *)0 ;
  {
    struct
      stat
        tmp_stat ;
    if (!stat((char*)file_name, &tmp_stat))
    {
      /*
       * Enter if local file has been created and, therefore,
       * needs to be deleted (and its memory freed).
       */
#if !DUNE_BCM
      memDevDelete((char*)file_name);

#endif
      free(mem_base);
    }
    else
    {
      /*
       * Enter if local file is not registered with the operating
       * system. This is a serious error condition.
       */
      sal_sprintf(err_msg,
        "erase_cli_file():\r\n"
        "  File name (%s) is not registered with the operating system!\r\n",
        file_name
        ) ;
      ret = 3 ;
      goto exit ;
    }
  }
exit:
  if (!ret)
  {
    /*
     * Precaution...
     */
    err_msg[0] = 0 ;
  }
  return (ret) ;
}


/*****************************************************
*NAME
*  file_via_tftp_send
*TYPE: PROC
*FUNCTION:
*  Send a file through TFTP.
*INPUT:
*  SOC_SAND_DIRECT:
*    char *file_name -
*      Pointer to name of file on this system. This
*      is a null terminated string  whose size
*      must be less than (CLI_FILE_NAME_LEN+1)
*    char * -
*      If an error is encountered then, on output,
*      this program will load pointed memory by
*      a null terminated string with a description
*      of the error.
*      Calling program is supposed to prepare
*      a memry block, for that, of size 320 or more.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*    err_msg
*REMARKS:
*  None.
*SEE ALSO:
 */
uint32
  file_via_tftp_send(
    uint8   *buffer,
    uint32  buffer_size,
    uint8   *file_name
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fd;
  char
    filen;

  Ui_file_n++;
  sal_sprintf(&filen, "%c", Ui_file_n);

  res = load_cli_file_from_memory(
          &filen,
          buffer,
          buffer_size,
          "Unknown error"
        );

  fd = open (&filen, O_RDONLY, 0444);

#if !DUNE_BCM
  res = tftpCopy ("10.0.0.100", 0, file_name, "put", "ascii", fd);
#else
  assert(1);
#endif

  close (fd);

  return res;
}
