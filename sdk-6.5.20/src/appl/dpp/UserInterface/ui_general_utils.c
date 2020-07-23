/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

#include <bcm_app/dpp/VxForRef_01/DuneApplicationVersion.h>
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * INCLUDE FILES:
 */
#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
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
#include <sysSymTbl.h>
#include <unldlib.h>

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
#include <appl/diag/dpp/utils_periodic_suspend.h>
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

#include <ui_general_utils.h>

#include <flash28.h>
/* } */
#endif

#include <appl/dpp/UserInterface/ui_general_utils.h>
#include <appl/diag/dpp/utils_convert_s_record.h>



#define BOOT_FILE_NAME_LEN         15
#define BOOT_FILE_NAME   "os_rom.hex"



extern
  int
    inflate(
      const char * src,
            char * dest,
            int    nBytes
    );

/*
 * {
 * define UI_GENERAL_UTILS_MEM_LEAK to 1, in order to do printout of malloc and free
 * in this file.
 *
 * define UI_GENERAL_UTILS_MEM_LEAK to 0, in order to do user regular malloc and free.
 */
#define UI_GENERAL_UTILS_MEM_LEAK  0

#if UI_GENERAL_UTILS_MEM_LEAK

  void PRINT_PLACE(unsigned int line_no)
  {
    d_printf("##%s:%u", "ui_general_utils.c", line_no);
  }

  void*
    local_malloc(size_t __size, unsigned int line_no)
  {
    void
      *ptr;

    PRINT_PLACE(line_no);
    ptr = malloc(__size);

    d_printf("  malloc(%5u)-> 0x%08lX\n\r\n", __size, (unsigned long)ptr);
    return ptr;
  }

  void
    local_free(void* __ptr, unsigned int line_no)
  {

    PRINT_PLACE(line_no);
    d_printf("  free(0x%08lX)\n\r\n", (unsigned long)__ptr);

    free(__ptr);

    return;
  }

  #define _MALLOC_(x) local_malloc(x, __LINE__)
  #define _FREE_(x)   local_free(x, __LINE__)

#else
  #define _MALLOC_(x) malloc(x)
  #define _FREE_(x)   free(x)
#endif
/*
 * }
 */

#ifndef SAND_LOW_LEVEL_SIMULATION

/*****************************************************
*NAME
*  download_application_block_to_flash
*TYPE: PROC
*DATE: 19/AUG/2002
*FUNCTION:
* Utility: Load boot from ip_address to the flash block.
*CALLING SEQUENCE:
*   ERR s = download_boot_block_to_flash(ip_address) ;
*INPUT:
*  SOC_SAND_DIRECT:
*   char *ip_address
*     ip of server where to look the application for.
*  SOC_SAND_INDIRECT:
*   information from nv ram about the application name, sizes, etc..
*OUTPUT:
*  SOC_SAND_DIRECT:
*   ERROR / OK
*  SOC_SAND_INDIRECT:
*  if succeeded write the specific boot file to the flash.
*REMARKS:
*SEE ALSO:
 */
int
  download_boot_block_to_flash(
    char *ip_address
  )
{
  char proc_name[] = "download_boot_block_to_flash";
  int err = FALSE;
  char
    *boot_flash_base = NULL,
    boot_file_name[BOOT_FILE_NAME_LEN+2] = BOOT_FILE_NAME,
    device_name[MEM_DEVICE_NAME_LEN + 1] = "tmp_name2",
    mem_file_name[MEM_DEVICE_NAME_LEN + 11] = "",
    *mem_file_base = NULL,
    msg[300] ;
  unsigned
    long
      ul,
      boot_flash_size,
      flash_sector_size ;
  int
    data_size,
    boot_flash_offset ;
  unsigned
    int
      originally_zipped ;
  unsigned
    long
      flash1_mem_base_adrs,
      periodic_session;
  unsigned
    char
      flash_type ;
  int
    s_record_byte_size = 0;
  char
    *boot_after_convert = NULL;

  flash_type = get_flash_device_code() ;
  if (flash_type == FLASH_28F640)
  {
    flash1_mem_base_adrs = FLASH1_MEM_BASE_ADRS_8MEG ;
  }
  else
  {
    flash1_mem_base_adrs = FLASH1_MEM_BASE_ADRS_4MEG ;
  }
  boot_flash_size = BOOT_FLASH_SIZE ;
  boot_flash_base = (char*)BOOT_FLASH_BASE_ADRS ;
  boot_flash_offset =  BOOT_FLASH_BASE_ADRS - flash1_mem_base_adrs ;

  flash_sector_size  = get_flash_sector_size();
  /*
   * Make sure size of assigned memory is an integral factor
   * of Flash sector size.
   */
  ul = boot_flash_size / flash_sector_size ;
  boot_flash_size = flash_sector_size * ul ;
  /*
   * Make sure size of assigned memory is is larger than FALSH
   * block size.
   */
  boot_after_convert = (char*) _MALLOC_(boot_flash_size);
  if (gen_err( TRUE, FALSE, (int)(boot_after_convert == NULL), OK, "\r\nError: Allocation for mem fail!",
               proc_name, SVR_ERR, DOWNLOAD_BOOT_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
  {
    goto download_boot_block_to_flash_no_free_mem_exit ;
  }

  err = create_mem_file_system(
            boot_flash_size*ZIP_FACTOR, device_name,
            mem_file_name,&mem_file_base,msg) ;
  if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: Memory device creation fail!",
               proc_name, SVR_ERR, DOWNLOAD_BOOT_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
  {
    goto download_boot_block_to_flash_no_free_mem_exit ;
  }
  /*
   * Make sure base address and file size are exact multiples of
   * 128K.
   */
  ul = (unsigned long)boot_flash_base ;
  ul /= flash_sector_size ;
  boot_flash_base = (char *)(ul * flash_sector_size) ;
  ul = boot_flash_size / flash_sector_size ;
  boot_flash_size = flash_sector_size * ul ;

  /*
   * Here load flash block.
   */
  send_string_to_screen("----Start downloading file S-Record file...", TRUE);
  sal_sprintf(msg, "----Getting from host(%s) \r\n"
               "      the file(%s).", ip_address, boot_file_name);
  send_string_to_screen(msg, TRUE) ;
  /*
   * protect the ftp download from the watchdog
   */
  periodic_suspend_this_task_start(30*100, &periodic_session);
  err =
      load_memory_file(0,0,
                ip_address,boot_file_name,
                0,mem_file_name,msg,
                &data_size,FALSE,&originally_zipped) ;
  periodic_suspend_this_task_end(periodic_session);

  if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: Could not find boot application module from host!",
               proc_name, SVR_ERR, DOWNLOAD_BOOT_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
  {
    send_string_to_screen(msg, TRUE);
    goto download_boot_block_to_flash_exit ;
  }
  send_string_to_screen("----Downloading finished successfully.", TRUE);

  send_string_to_screen("----Start convert the downloaded file from S-Record format to Hex...", TRUE);
  err = convert_s_record_to_hex(mem_file_name, data_size, (unsigned long)boot_flash_base,
                                boot_after_convert, boot_flash_size,
                                &s_record_byte_size, msg, 300);
  if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: Could convert from S-Record !",
               proc_name, SVR_ERR, DOWNLOAD_BOOT_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
  {
    send_string_to_screen(msg, TRUE);
    goto download_boot_block_to_flash_exit ;
  }
  send_string_to_screen("----Conversion finished successfully.", TRUE);

  {
    /*
     * Erase first
     */
    int
      sector,
      base_sector,
      num_sectors,
      last_sector ;

    base_sector = boot_flash_offset / flash_sector_size;
    num_sectors = boot_flash_size / flash_sector_size ;
    last_sector = base_sector + num_sectors ;
    send_string_to_screen("----Erasing Boot...", TRUE);
    load_indication_start(last_sector - base_sector);
    for (sector = base_sector ; sector < last_sector ; sector++)
    {
      err =
        sysFlashErase_protected(
                (unsigned char)flash_type, FALSE,sector) ;

      if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: Failed to erase boot.!",
                   proc_name, SVR_ERR, DOWNLOAD_BOOT_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
      {
        goto download_boot_block_to_flash_exit ;
      }
      load_indication_run(sector - base_sector + 1);
    }

  }
  sal_sprintf(msg,
      "\r\n"
      "----Writing boot... starting at offset (0x%08X) in flash,\r\n"
      "                    write  (0x%08X = %u) bytes.",
      boot_flash_offset, s_record_byte_size, s_record_byte_size) ;
  send_string_to_screen(msg,TRUE) ;
  send_string_to_screen("\r\n----Writing boot...", TRUE);

  load_indication_start(s_record_byte_size);
  /*
   * 0x20 is the size header of the exe file
   * 0x100 is the offset in the boot block.
   * move them to pre define sizes.
   */
  err =
    sysFlashWrite_protected(
      FLASH_CAST (boot_after_convert),s_record_byte_size,
      boot_flash_offset,flash_type,0,load_indication_run) ;

  if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: Failed to write boot.!",
               proc_name, SVR_ERR, DOWNLOAD_BOOT_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
  {
    goto download_boot_block_to_flash_exit ;
  }
  sal_sprintf(msg,
      "\r\n"
      "----Success writing boot from %s.\r\n",
      ip_address) ;
  send_string_to_screen(msg,TRUE) ;

download_boot_block_to_flash_exit:
  memDevDelete(device_name);
  _FREE_(mem_file_base);
  _FREE_(boot_after_convert);

download_boot_block_to_flash_no_free_mem_exit:
  return err;
}

#endif

/*****************************************************
*NAME
*  download_application_block_to_flash
*TYPE: PROC
*DATE: 19/AUG/2002
*FUNCTION:
* Utility: Load application from ip_address to the block number app_i.
*CALLING SEQUENCE:
*   STATUS s = download_application_block_to_flash(app_i,ip_address) ;
*INPUT:
*  SOC_SAND_DIRECT:
*   const unsigned int app_i
*     block number to load the application to.
*   char *ip_address
*     ip of server where to look the application for.
*  SOC_SAND_INDIRECT:
*   information from nv ram about the application name, sizes, etc..
*OUTPUT:
*  SOC_SAND_DIRECT:
*   ERROR / OK
*  SOC_SAND_INDIRECT:
*  if succeeded write the specific file to the flash.
*REMARKS:
*SEE ALSO:
 */
int
  download_application_block_to_flash(
    const unsigned int app_i,
    char *ip_address
  )
{
  char proc_name[] = "download_application_block_to_flash";
  int err = FALSE;
  char
    *app_flash_base,
    dld_file_name[DLD_FILE_NAME_LEN + 1],
    device_name[MEM_DEVICE_NAME_LEN + 1] = "tmp_name1",
    mem_file_name[MEM_DEVICE_NAME_LEN + 11],
    *mem_file_base,
    msg[160],
    err_msg[160];
  unsigned
    long
      ul,
      app_flash_size,
      mem_drv_len,
    flash_sector_size ;
  unsigned
    int
      host_version;
  unsigned
    short
      crc_16 ;
  unsigned
    char
      *flash_image_file,
      ascii_ver[7] ;
  int
    data_size ;
  unsigned
    int
      originally_zipped ;
  unsigned
    long
      flash_allocated_size,
      flash_required_size,
      flash_block_size,
      periodic_session;
  BLOCK_02 block02;

  flash_image_file = NULL;

  flash_sector_size  = get_flash_sector_size();
  err = block_2_from_nv(&block02);
  if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: NV ram read fail (block_2_from_nv)!",
               proc_name, SVR_ERR, DOWNLOAD_APPLICATION_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
  {
    goto download_application_block_to_flash_no_free_mem_exit ;
  }
  mem_drv_len     = block02.mem_drv_len ;
  app_flash_base  = block02.app_flash_base ;
  app_flash_size  = block02.app_flash_size ;
  memcpy(dld_file_name, block02.dld_file_name, DLD_FILE_NAME_LEN+1);
  dld_file_name[DLD_FILE_NAME_LEN] = '\0' ;


  /*
   * Make sure size of assigned memory is an integral factor
   * of Flash sector size.
   */
  ul = mem_drv_len / flash_sector_size ;
  mem_drv_len = flash_sector_size * ul ;
  /*
   * Make sure size of assigned memory is is larger than FALSH
   * block size.
   */
  err = create_mem_file_system(
            mem_drv_len, device_name,
            mem_file_name,&mem_file_base,msg) ;
  if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: Memory device creation fail!",
               proc_name, SVR_ERR, DOWNLOAD_APPLICATION_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
  {
    goto download_application_block_to_flash_no_free_mem_exit ;
  }

  /*
   * Make sure base address and file size are exact multiples of
   * 128K.
   */
  ul = (unsigned long)app_flash_base ;
  ul /= flash_sector_size ;
  app_flash_base = (char *)(ul * flash_sector_size) ;
  ul = app_flash_size / flash_sector_size ;
  app_flash_size = flash_sector_size * ul ;

  /*
   * Here load flash block.
   */
  sal_sprintf(msg, "----Getting from host(%s) \r\n"
               "      the file(%s).", ip_address, dld_file_name);
  send_string_to_screen(msg, TRUE) ;
  periodic_suspend_this_task_start(40*100, &periodic_session);
  err =
      load_memory_file(0,0,
                ip_address,dld_file_name,
                0,mem_file_name,msg,
                &data_size,FALSE,&originally_zipped) ;
  periodic_suspend_this_task_end(periodic_session);


  if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: Could not find or load application module from host!",
               proc_name, SVR_ERR, DOWNLOAD_APPLICATION_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
  {
    send_string_to_screen(msg, TRUE);
    goto download_application_block_to_flash_exit ;
  }

  sal_sprintf(msg, "----Downloading finished successfully.");
  send_string_to_screen(msg, TRUE) ;

  host_version = 0;
  if(originally_zipped)
  {
    if(get_app_ver_from_zipped_file /*boot_function*/)
    {
      err = get_app_ver_from_zipped_file(
        mem_file_name,
        data_size,
        err_msg,
        &host_version
      );
      if (gen_err( TRUE, FALSE, (int)err, OK,
          err_msg,
          proc_name, SVR_ERR, GET_APPLICATION_VERION_FROM_ZIP_ERR_01, FALSE, 0, -1, FALSE)
         )
      {
        goto download_application_block_to_flash_exit ;
      }
    }
    else/*TODO!!*/
    {
      d_printf(
        "************************************************\n\r"
        "************************************************\n\r"
        "WARNNING: You are working with an old boot!!!!!\n\r"
        "************************************************\n\r"
        "************************************************\n\r"
        );
      periodic_suspend_this_task_start(30*100, &periodic_session);
      err = get_application_version_from_zipped_file(
        mem_file_name,
        data_size,
        &host_version
      );
      periodic_suspend_this_task_end(periodic_session);
      if (gen_err( TRUE, FALSE, (int)err, OK,
          "\r\nError: Error in get_application_version_from_zipped_file.!\n\r",
          proc_name, SVR_ERR, DOWNLOAD_APPLICATION_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
      {
        goto download_application_block_to_flash_exit ;
      }
    }
  }

  if(host_version == 0)
  {
    periodic_suspend_this_task_start(30*100, &periodic_session);
    err = get_application_version(mem_file_base, data_size,
                                 originally_zipped, &host_version);
    periodic_suspend_this_task_end(periodic_session);

    if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: Could not find get the file application version.!\n\r"
                                            "Probably file format error",
                 proc_name, SVR_ERR, DOWNLOAD_APPLICATION_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
    {
      goto download_application_block_to_flash_exit ;
    }
  }

  sal_sprintf(msg, "----The downloaded file version is (%u).", host_version);
  send_string_to_screen(msg, TRUE) ;

  flash_block_size = get_flash_descriptor_block_size(app_i) ;
  flash_required_size =
      ((unsigned long)data_size +
            sizeof(FLASH_BLOCK_HEADER) + sizeof(crc_16)) ;
  if (flash_required_size > flash_block_size)
  {
    flash_allocated_size =
      (unsigned long)(flash_block_size -
              sizeof(FLASH_BLOCK_HEADER) - sizeof(crc_16)) ;
    err = 4 ;
    sal_sprintf(msg,
        "Application file size (%lu) bigger\r\n"
        "  than allocated in FLASH block (%lu).\r\n"
        "Quit FLASH burning...\r\n",
        (unsigned long)data_size,
        flash_allocated_size
       ) ;
    gen_err( TRUE, FALSE, (int)err, OK, msg,
             proc_name, SVR_ERR, DOWNLOAD_APPLICATION_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) ;
    goto download_application_block_to_flash_exit ;
  }
  sal_sprintf(msg,
          "----Loading Flash block %u (out of %u) now...\r\n\n",
          (unsigned short)(app_i + 1),
          (unsigned short)get_num_app_flash_files()) ;
  send_string_to_screen(msg, FALSE);
  sal_sprintf(msg,"----Erasing...") ;
  send_string_to_screen(msg, FALSE);
  err = erase_program_flash(app_i, TRUE) ;
  send_string_to_screen("\r\n", FALSE);

  if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: Failed to erase Flash block!",
               proc_name, SVR_ERR, DOWNLOAD_APPLICATION_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
  {
    goto download_application_block_to_flash_exit ;
  }
  else
  {
    /*
     * Flash memory has been erased. Now build image of Flash (header
     * plus file) and burn Flash.
     */
    /*
     * Copy downloaded file into temporary memory and inflate into
     * original file.
     */
    unsigned
      char
        *data_ptr_in_flash_image,
        *downloaded_file ;
    FLASH_BLOCK_HEADER
      *flash_block_header ;
    sal_sprintf(msg,"----Preparing memory image...\r\n\n") ;
    send_string_to_screen(msg, FALSE);
    downloaded_file = (unsigned char *)mem_file_base ;
    flash_image_file = (unsigned char *)_MALLOC_(flash_block_size) ;
    if (flash_image_file == (unsigned char *)0)
    {
      sal_sprintf(msg,
        "Could not allocate memory (temporary storage) for FLASH-image file.\r\n"
        "Quit FLASH burning process.\r\n"
      ) ;
      err = 5 ;
      gen_err( TRUE, FALSE, (int)err, OK, msg,
               proc_name, SVR_ERR, DOWNLOAD_APPLICATION_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) ;
      goto download_application_block_to_flash_exit ;
    }
    memset(flash_image_file,0xFF,flash_block_size) ;
    flash_block_header = (FLASH_BLOCK_HEADER *)flash_image_file ;
    data_ptr_in_flash_image = (unsigned char *)(flash_block_header + 1) ;
    /*
     * Copy downloaded file into flash image (beyond space allocated
     * for header.
     */
    memcpy(data_ptr_in_flash_image,downloaded_file,data_size) ;
    /*
     * Load header of flash block into flash image.
     * {
     */
    memcpy(flash_block_header->block_header.header_type,
            NEW_ELF_TYPE,
            sizeof(flash_block_header->block_header.header_type)) ;
    flash_block_header->
      block_header.flash_block_version = (unsigned short)host_version ;
    flash_block_header->
      block_header.flash_block_is_zipped = originally_zipped ;
    strncpy(
      flash_block_header->block_header.flash_block_file_name,
      dld_file_name,
      sizeof(flash_block_header->block_header.flash_block_file_name) - 1) ;
    flash_block_header->
      block_header.flash_block_file_name[
          sizeof(flash_block_header->block_header.flash_block_file_name) - 1] = 0 ;
    flash_block_header->block_header.flash_block_data_size = (unsigned long)data_size ;
    /*
     * }
     */
    send_string_to_screen("----Writing to FLASH...", FALSE);
    err =
      load_program_flash(
        app_i,flash_image_file,flash_block_size - sizeof(crc_16)) ;
    d_printf("\r\n") ;

    if (gen_err( TRUE, FALSE, (int)err, OK, "\r\nError: Failed to write Flash block!",
                 proc_name, SVR_ERR, DOWNLOAD_APPLICATION_TO_FLASH_ERR_01, FALSE, 0, -1, FALSE) )
    {
      goto download_application_block_to_flash_exit ;
    }
    else
    {
      set_flash_descriptor_block_loaded(app_i);
      set_flash_descriptor_block_is_new_format(app_i,(unsigned char)TRUE) ;
      set_flash_descriptor_block_version(app_i, (unsigned short)host_version);
      set_flash_descriptor_block_file_name(app_i,dld_file_name) ;
      set_flash_descriptor_block_is_zipped(app_i,(unsigned char)originally_zipped) ;
      set_flash_descriptor_block_data_size(app_i,(unsigned long)data_size) ;
      sal_sprintf(msg,
        "----Flash block no. %u (out of %u) has been loaded\r\n"
        "     with application version %s!\r\n",
        (unsigned short)(app_i + 1),
        (unsigned short)get_num_app_flash_files(),
        version_to_ascii(ascii_ver,host_version,1)
        ) ;
      send_string_to_screen(msg, FALSE);
      goto download_application_block_to_flash_exit ;
    }
  }

download_application_block_to_flash_exit:
  memDevDelete(device_name);
  _FREE_(mem_file_base);
  if (flash_image_file != NULL)
  {
    _FREE_(flash_image_file);
  }

download_application_block_to_flash_no_free_mem_exit:
  return err;
}


/*****************************************************
*NAME
*  get_application_version_from_zipped_file
*TYPE: PROC
*DATE: 19/AUG/2002
*FUNCTION:
* Utility: Get the version number from the zipped file, without extending.
*   File should be application file (either zipped or not).
*INPUT:
*  SOC_SAND_DIRECT:
*   char* mem_file_name
*     the name of the zipped file.
*   int data_size
*     file byte size.
*   unsigned int *version
*     output: the version of the file.
*  SOC_SAND_INDIRECT:
*   Non
*OUTPUT:
*  SOC_SAND_DIRECT:
*   ERROR / OK
*  SOC_SAND_INDIRECT:
*  NON.
*REMARKS:
*SEE ALSO:
*  windows_applications\AppendVersionNum\append_app_version_num.c:
*    Where the version number is attached to the zipped file.
*  get_host_app_ver_from_zipped_file (usrAppLoader - vxForRef_01)
 */
/* Temp!! - this function should be out of here until 12/11/04 */
int
  get_application_version_from_zipped_file(
    const char* mem_file_name,
    const int data_size,
    unsigned int *version
  )
{
  int
    memFd;
  long
    pos;
  unsigned
    int
      num_of_bytes;
  char
    buffer[APP_VER_BUFFER_SIZE];
  DUNE_APPLICATION_VERSION
    *ver_ptr;
  STATUS stat=OK;
  char proc_name[] = "get_application_version_from_zipped_file";

  memFd = open(mem_file_name,O_RDONLY,0) ;
  if (memFd == ERROR)
  {
    stat = ERROR;
    gen_err(  TRUE, FALSE, (int)stat, OK, "open() fail. Could not open memory file.",
             proc_name, SVR_ERR, GET_APPLICATION_VERION_FROM_ZIP_ERR_01, FALSE, 0, -1, FALSE);
    goto exit_close ;
  }
  pos = lseek(memFd, data_size-sizeof(DUNE_APPLICATION_VERSION), SEEK_SET);
  num_of_bytes = read( memFd, buffer, sizeof(DUNE_APPLICATION_VERSION));

  if(!memcmp(buffer, APP_VER_EXPECTED_PREFIX, strlen(APP_VER_EXPECTED_PREFIX)))
  {
    ver_ptr = (DUNE_APPLICATION_VERSION *)buffer;
    if( (!strcmp(ver_ptr->prefix, APP_VER_EXPECTED_PREFIX)) &&
      (!strcmp(ver_ptr->suffix, APP_VER_EXPECTED_SUFFIX))
      )
    {
      *version = (int)ver_ptr->version_num;
      goto exit_close;
    }
  }

exit_close:
  close(memFd) ;
  return (stat) ;
}

/*****************************************************
*NAME
*  get_application_version
*TYPE: PROC
*DATE: 19/AUG/2002
*FUNCTION:
* Utility: Get the version number from the file.
*   File should be application file (either zipped or not).
*CALLING SEQUENCE:
*   STATUS s = get_application_version(mem_file_base,data_size,zipped,host_version) ;
*INPUT:
*  SOC_SAND_DIRECT:
*   char* mem_file_base
*     the start of the file pointer.
*   int data_size
*     file byte size.
*   int zipped
*     file is zipped.
*   unsigned int *version
*     output: the version of the file.
*  SOC_SAND_INDIRECT:
*   Non
*OUTPUT:
*  SOC_SAND_DIRECT:
*   ERROR / OK
*  SOC_SAND_INDIRECT:
*  NON.
*REMARKS:
*SEE ALSO:
 */
int
  get_application_version(
    const char* mem_file_base,
    const int data_size,
    const int zipped,
    unsigned int *version  )
{
  MODULE_ID
    module_id ;
  STATUS stat=OK;
  char* local_mem=NULL;
  unsigned int local_mem_size;
  char proc_name[] = "get_application_verion";
  char tmp_name[] = "get_app_ver_tmp:";

  /*
   * Allocate memory for coping the file.
   */
  local_mem_size = data_size;
  if (zipped)
  {
    local_mem_size = data_size*ZIP_FACTOR;
  }
  local_mem = _MALLOC_(local_mem_size);
  stat = (local_mem==NULL);
  if (!gen_err(  TRUE, FALSE, (int)stat, OK, "error allocation space for local_mem",
                   proc_name, SVR_ERR, GET_APPLICATION_VERION_ERR_01, FALSE, 0, -1, FALSE) )
  {
    /*
     * Create device and copy and File.
     */
    stat = memDevCreate(tmp_name, local_mem, local_mem_size);
    if (!gen_err(  TRUE, FALSE, (int)stat, OK, "error creating local device",
                     proc_name, SVR_ERR, GET_APPLICATION_VERION_ERR_01, FALSE, 0, -1, FALSE) )
    {
      if (zipped)
      {
        stat = inflate(mem_file_base,local_mem,data_size) ;
      }
      else
      {
        memcpy(local_mem, mem_file_base, local_mem_size) ;
        stat = OK ;
      }

      if (!gen_err(  TRUE, FALSE, (int)stat, OK, "error inflating file",
                       proc_name, SVR_ERR, GET_APPLICATION_VERION_ERR_01, FALSE, 0, -1, FALSE) )
      {
        module_id = ld(1,TRUE,tmp_name) ;
        stat = (module_id == (MODULE_ID)0);

        if (!gen_err(  TRUE, FALSE, (int)stat, OK, "Illegal host application program!!!",
                         proc_name, SVR_ERR, GET_APPLICATION_VERION_ERR_01, FALSE, 0, -1, FALSE) )
        {
          /*
           * Load and check the file version.
           */
          char func_name[] = "get_prog_ver" ;
          SYM_TYPE
            symType ;
          FUNCPTR
            func_address ;
          stat = symFindByName (sysSymTbl,func_name, (char **)&func_address,&symType) ;

          if (!gen_err(  TRUE, FALSE, (int)stat, OK, "Did not find function in sysSymTbl",
                           proc_name, SVR_ERR, GET_APPLICATION_VERION_ERR_01, FALSE, 0, -1, FALSE) )
          {
            *version = (unsigned int)func_address() ;
          }

          stat = unld(module_id,0) ;
          gen_err( TRUE, FALSE, (int)stat, OK, "Fatal error: unld() returned with error!!!",
                   proc_name, SVR_FTL, GET_APPLICATION_VERION_ERR_01, FALSE, 0, -1, FALSE) ;
        }
      }

      gen_err( TRUE, FALSE, (int)memDevDelete(tmp_name), OK, "error deleting local device",
               proc_name, SVR_ERR, GET_APPLICATION_VERION_ERR_01, FALSE, 0, -1, FALSE) ;
    }
    _FREE_(local_mem);
  }

  return stat;
}

/*****************************************************
*NAME
*  print_flash_descriptor_info
*TYPE: PROC
*DATE: 1/SEP/2002
*FUNCTION:
* Utility: .
*CALLING SEQUENCE:
*   STATUS s = print_flash_descriptor_info() ;
*INPUT:
*  SOC_SAND_DIRECT:
*   Non
*  SOC_SAND_INDIRECT:
*   flash descriptors
*OUTPUT:
*  SOC_SAND_DIRECT:
*   ERROR / OK
*  SOC_SAND_INDIRECT:
*  Prints information to the screen.
*REMARKS:
*SEE ALSO:
 */
int
  print_flash_descriptor_info()
{
  unsigned
    int
      app_i = 0;
  char
    err_msg[80*4] = "";
  char
    acsii_board_version[80] = "";

  for (app_i=0; app_i<get_num_app_flash_files() ; app_i++)
  {
    sal_sprintf(err_msg,"\r\nApplication %d :",app_i+1);
    send_string_to_screen(err_msg, TRUE) ;

    sal_sprintf(err_msg,"   Valid : %d ", (int)get_flah_descriptor_block_valid(app_i)) ;
    send_string_to_screen(err_msg, TRUE) ;
    if( !get_flah_descriptor_block_valid(app_i) )
    {
      continue ;
    }
    sal_sprintf(err_msg,"   Loaded : %d ", (int)get_flash_descriptor_block_loaded(app_i)) ;
    send_string_to_screen(err_msg, TRUE) ;
    if( !get_flash_descriptor_block_loaded(app_i) )
    {
      continue ;
    }
    sal_sprintf(err_msg,"   Size : %d ", (int)get_flash_descriptor_block_size(app_i)) ;
    send_string_to_screen(err_msg, TRUE) ;
    sal_sprintf(err_msg,"   Data Size : %lu ", get_flash_descriptor_block_data_size(app_i)) ;
    send_string_to_screen(err_msg, TRUE) ;
    /*
     */
    version_to_ascii(acsii_board_version,
                     (unsigned int)get_flash_descriptor_block_version(app_i),
                     TRUE);
    sal_sprintf(err_msg,"   Version : %s",acsii_board_version) ;
    send_string_to_screen(err_msg, TRUE) ;
    sal_sprintf(err_msg,"   Zipped : %d ", (int)get_flash_descriptor_block_is_zipped(app_i)) ;
    send_string_to_screen(err_msg, TRUE) ;
    sal_sprintf(err_msg,"   New Format : %d ", (int)get_flash_descriptor_block_is_new_format(app_i)) ;
    send_string_to_screen(err_msg, TRUE) ;
    sal_sprintf(err_msg,"   File Name : %s ", get_flash_descriptor_block_file_name(app_i)) ;
    send_string_to_screen(err_msg, TRUE) ;
    sal_sprintf(err_msg,"   Base : 0x%lX ", (unsigned long)get_flash_descriptor_block_base(app_i)) ;
    send_string_to_screen(err_msg, TRUE) ;
  }
  send_string_to_screen("", TRUE) ;

  return OK;
}
