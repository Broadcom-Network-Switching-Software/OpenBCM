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

#if LINK_TIMNA_LIBRARIES
/* { */

/*
 * INCLUDE FILES:
 */
#ifndef SAND_LOW_LEVEL_SIMULATION
/* { */
/* } */
#endif

#include <stdio.h>
/*
 * User interface internal include file.
 */
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_line_tevb.h>


#include <appl/diag/dpp/utils_line_TEVB.h>
#include <appl/diag/dpp/utils_dune_fpga_download.h>
#include <appl/diag/dpp/utils_front_end_host_card.h>
#include <appl/diag/dpp/utils_dffs_tevb.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>


/*
 */

/*
 * Error Handling
 * {
 */

void ui_tevb_reoprt_sw_error_to_user(unsigned int err_id)
{
  char
    err_msg[80*2] = "";

  sal_sprintf(err_msg, "process_tevb_app_line SW error %u\r\n", err_id) ;
  send_string_to_screen(err_msg,TRUE) ;
}





void
  tevb_ui_report_driver_err(
    SOC_SAND_IN unsigned long driver_ui_ret
  )
{
  if(soc_sand_get_error_code_from_error_word(driver_ui_ret) != SOC_SAND_OK)
  {
    send_string_to_screen("\n\r"
                          "Driver error while executing the command",TRUE);
    soc_sand_disp_result(driver_ui_ret);
    send_string_to_screen("\n\r", TRUE);
  }
}


/*
 * }
 */



/*****************************************************
*NAME
*  subject_line_tevb
*TYPE: PROC
*DATE: 3/JAN/2007
*FUNCTION:
*  Process input line which has an 'tevb' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int subject_line_tevb(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4] = "",
    data_str[20] = "";
  char
    *proc_name;
  char
    *file_name = "";
  BOOL
    line_tevb_connected = 0 ;
  BOOL
    get_val_of_err ;
  unsigned int
    match_index,
    mem_address,
    short_format,
    verbose,
    silent,
    ret;
  unsigned char
    silent_c;
  unsigned long
    data;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    ERR_006, ERR_007, ERR_008, ERR_009, ERR_010,
    ERR_011, ERR_012, ERR_013, ERR_014, ERR_015,
    NUM_EXIT_CODES
  } ui_ret ;

  proc_name = "subject_line_tevb";
  ui_ret = NO_ERR ;
  get_val_of_err = FALSE;
  short_format = TRUE;

  send_string_to_screen("",TRUE);


  line_tevb_connected = tevb_is_timna_connected();
  if (!line_tevb_connected)
  {
    send_string_to_screen("Line-Card-TEVB is NOT connected to this tevb-card.\n\r", FALSE);
    goto exit ;
  }

  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'line_card').
     */
    send_string_to_screen("Line-Card-TEVB is connected to this cpu-card.\n\r", FALSE);
    goto exit ;
  }


  if (!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_VERBOSE_ID,1))
  {
    verbose = TRUE ;
    silent_c = FALSE;
  }
  else
  {
    verbose = FALSE ;
    silent_c = TRUE;
  }
  silent = !verbose;


  if ( !search_param_val_pairs(current_line,&match_index,PARAM_TEVB_MEM_ID,1))
  {
    /*
     * Operation related to memory access
     */

    if (!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_MEM_READ_ID,1))
    {
      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TEVB_MEM_READ_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        mem_address = (unsigned long)param_val->value.ulong_value ;
      }

      if (tevb_timna_read(mem_address, &data))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      sal_sprintf(data_str,"0x%X: %lu",
        mem_address,data) ;
      send_string_to_screen(data_str,TRUE) ;

    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_MEM_WRITE_ID,1))
    {

      /*
       */
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TEVB_MEM_WRITE_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        mem_address = (unsigned long)param_val->value.ulong_value ;
      }

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_TEVB_MEM_DATA_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
      else
      {
        data = (unsigned long)param_val->value.ulong_value ;
      }


      if (tevb_timna_write(mem_address, data))
      {
        get_val_of_err = TRUE ;
        goto exit;
      }
    }
    else
    {
      /*
       * No valid operation variable has been entered.
       */
      send_string_to_screen(
          "*** Should not reach here.\r\n",
          FALSE) ;
      ui_ret = ERR_001 ;
      goto exit ;
    }
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_FLASH_ID,1)))
  {
    if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_FLASH_DIAGNOSTICS_ID,1)))
    {
      if (tevb_flash_diagnostics(TRUE))
      {
        get_val_of_err = TRUE ;
        soc_sand_os_printf( 
          "-->tevb_flash_diagnostics reports error\n\r");

        goto exit ;
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_FLASH_DOWNLOAD_ID,1)))
    {
      unsigned int
        compressed = FALSE;

      if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_COMPRESSED_ID,1)))
      {
        compressed = TRUE;
      }

      /* download */
      if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_FLASH_FILENAME_ID,1)))
      {
        if (get_val_of(
              current_line,(int *)&match_index,
              PARAM_TEVB_FLASH_FILENAME_ID,1,
              &param_val,VAL_TEXT,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit;
        }
        file_name = param_val->value.val_text;

        if (utils_dffs_tevb_dld_and_write(file_name, silent_c))
        {
          get_val_of_err = TRUE ;
          soc_sand_os_printf( 
          "-->utils_dffs_tevb_dld_and_write reports error\n\r");

          goto exit ;
        }
      }
      else
      {
        /* not specifying filename */
        if (tevb_flash_download(compressed, verbose))
        {
          get_val_of_err = TRUE ;
          soc_sand_os_printf( 
          "-->tevb_flash_download reports error\n\r");

          goto exit ;
        }
      }
    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_FLASH_ERASE_ID,1)))
    {
      /* erase */
      if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_FLASH_FILENAME_ID,1)))
      {
        if (get_val_of(
              current_line,(int *)&match_index,
              PARAM_TEVB_FLASH_FILENAME_ID,1,
              &param_val,VAL_TEXT,err_msg))
        {
          get_val_of_err = TRUE ;
          goto exit;
        }
        file_name = param_val->value.val_text;

        if (utils_dffs_tevb_delete_file(file_name, silent_c))
        {
          get_val_of_err = TRUE ;
          soc_sand_os_printf( 
          "-->utils_dffs_tevb_delete_file reports error\n\r");

          goto exit ;
        }      }
      else
      {
        /* not specifying filename */
        if (tevb_flash_erase(verbose))
        {
          get_val_of_err = TRUE ;
          soc_sand_os_printf( 
          "-->tevb_flash_erase reports error\n\r");

          goto exit ;
        }
      }

    }
    else if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_FLASH_BURN_ID,1)))
    {
      unsigned int
        numeric_val = FALSE;

      if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_COMPRESSED_ID,1)))
      {
        numeric_val = TRUE;
      }

      tevb_flash_burn_fpga(numeric_val, TRUE);
    }
    else if(UI_MACROS_MATCH_ID(PARAM_TEVB_FLASH_DFFS_ID))
    {
      char
        *file_name = "",
        *comment = "",
        *date = "";
      unsigned long
        numeric_val = 0;
      int
        err,
        num_handled = 0;
      DFFS_FORMAT_TYPE
        format_type = DFFS_SHORT_FORMAT;
      char*
        soc_sand_proc_name = "                               ";

      ui_ret = FALSE ;
      num_handled++ ;

      if (UI_MACROS_MATCH_ID(PARAM_TEVB_FLASH_DFFS_DIR_ID))
      {
        soc_sand_proc_name = "utils_dffs_tevb_dir";

        UI_MACROS_GET_SYMBOL_VAL(PARAM_TEVB_FLASH_DFFS_DIR_FORMAT_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_LONG_VAL(numeric_val);

        if (numeric_val == LONG_EQUIVALENT)
        {
          format_type = DFFS_LONG_FORMAT;
        }

        err =
          utils_dffs_tevb_dir(
            format_type,
            silent_c
          );
      }
      else if (UI_MACROS_MATCH_ID(PARAM_TEVB_FLASH_DFFS_DEL_ID))
      {
        soc_sand_proc_name = "utils_dffs_tevb_delete_file";

        UI_MACROS_GET_TEXT_VAL(PARAM_TEVB_FLASH_DFFS_NAME_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_TEXT_VAL(file_name);

        err =
          utils_dffs_tevb_delete_file(
            file_name,
            silent_c
          );
      }
      else if (UI_MACROS_MATCH_ID(PARAM_TEVB_FLASH_DFFS_DLD_ID))
      {
        soc_sand_proc_name = "utils_dffs_tevb_dld_and_write";

        UI_MACROS_GET_TEXT_VAL(PARAM_TEVB_FLASH_DFFS_NAME_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_TEXT_VAL(file_name);

        err =
          utils_dffs_tevb_dld_and_write
          (
            file_name,
            silent_c
          );
      }
      else if (UI_MACROS_MATCH_ID(PARAM_TEVB_FLASH_DFFS_PRINT_ID))
      {
        soc_sand_proc_name = "utils_dffs_tevb_print_file";

        UI_MACROS_GET_TEXT_VAL(PARAM_TEVB_FLASH_DFFS_NAME_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_TEXT_VAL(file_name);

        UI_MACROS_GET_NUMMERIC_VAL(PARAM_TEVB_FLASH_DFFS_PRINT_SIZE_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_LONG_VAL(numeric_val);

        err =
          utils_dffs_tevb_print_file
          (
            file_name,
            numeric_val,
            silent_c
          );
      }
      else if (UI_MACROS_MATCH_ID(PARAM_TEVB_FLASH_DFFS_ADD_COMMENT_ID))
      {
        soc_sand_proc_name = "utils_dffs_tevb_set_comment";

        UI_MACROS_GET_TEXT_VAL(PARAM_TEVB_FLASH_DFFS_NAME_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_TEXT_VAL(file_name);

        UI_MACROS_GET_TEXT_VAL(PARAM_TEVB_FLASH_DFFS_COMMENT_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_TEXT_VAL(comment);

        err =
          utils_dffs_tevb_set_comment(
            file_name,
            comment,
            silent_c
          );
      }
      else if (UI_MACROS_MATCH_ID(PARAM_TEVB_FLASH_DFFS_SHOW_COMMENT_ID))
      {
         soc_sand_proc_name = "utils_dffs_tevb_set_comment";

        UI_MACROS_GET_TEXT_VAL(PARAM_TEVB_FLASH_DFFS_NAME_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_TEXT_VAL(file_name);

        err =
          utils_dffs_tevb_print_comment(
            file_name,
            silent_c
          );
      }
      else if (UI_MACROS_MATCH_ID(PARAM_TEVB_FLASH_DFFS_ADD_ATTR_ID))
      {
        soc_sand_proc_name = "set_attribute";

        UI_MACROS_GET_TEXT_VAL(PARAM_TEVB_FLASH_DFFS_NAME_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_TEXT_VAL(file_name);


        soc_sand_proc_name = "utils_dffs_tevb_set_date";

        UI_MACROS_GET_TEXT_VAL(PARAM_TEVB_FLASH_DFFS_DATE_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_TEXT_VAL(date);

        err =
          utils_dffs_tevb_set_date(
          file_name,
          date,
          silent_c
        );


        soc_sand_proc_name = "utils_dffs_tevb_set_version";

        UI_MACROS_GET_NUMMERIC_VAL(PARAM_TEVB_FLASH_DFFS_VER_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_LONG_VAL(numeric_val);

        err =
          utils_dffs_tevb_set_version(
          file_name,
          (unsigned char)numeric_val,
          silent_c
        );

        soc_sand_proc_name = "utils_dffs_tevb_set_attr";

        UI_MACROS_GET_NUMMERIC_VAL(PARAM_TEVB_FLASH_DFFS_ATTR1_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_LONG_VAL(numeric_val);

        err =
          utils_dffs_tevb_set_attr(
          file_name,
          (unsigned char)numeric_val,
          1,
          silent_c
        );

        soc_sand_proc_name = "utils_dffs_tevb_set_attr";

        UI_MACROS_GET_NUMMERIC_VAL(PARAM_TEVB_FLASH_DFFS_ATTR2_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_LONG_VAL(numeric_val);

        err =
          utils_dffs_tevb_set_attr(
          file_name,
          (unsigned char)numeric_val,
          2,
          silent_c
        );

        soc_sand_proc_name = "utils_dffs_tevb_set_attr";

        UI_MACROS_GET_NUMMERIC_VAL(PARAM_TEVB_FLASH_DFFS_ATTR3_ID);
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_LONG_VAL(numeric_val);

        err =
          utils_dffs_tevb_set_attr(
          file_name,
          (unsigned char)numeric_val,
          3,
          silent_c
          );
      }
      else if(UI_MACROS_MATCH_ID(PARAM_TEVB_FLASH_DFFS_DIAGNOSTICS_ID))
      {
        soc_sand_proc_name = "utils_dffs_tevb_diagnostics";

        err =
            utils_dffs_tevb_diagnostics(
            silent_c
          );
      }

      else
      {
        UI_MACROS_ERR_UNRECOGNIZED_PARAM(dffs);
      }
    }

    else
    {
      /*
       * No valid operation variable has been entered.
       */
      send_string_to_screen(
          "*** Should not reach here.\r\n",
          FALSE) ;
      ui_ret = ERR_002 ;
      goto exit ;
    }
  }
  else if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_FPGA_ID,1)))
  {
    if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_FPGA_DOWNLOAD_ID,1)))
    {
      unsigned int
        numeric_val = FALSE;

      if ((!search_param_val_pairs(current_line,&match_index,PARAM_TEVB_COMPRESSED_ID,1)))
      {
        numeric_val = TRUE;
      }

      ret = tevb_fpga_download(numeric_val, TRUE);

      if(ret != NO_ERR)
      {
        sal_sprintf(data_str,"tevb_fpga_download returned with error %u\n\r", ret);
        send_string_to_screen(data_str,FALSE) ;
        goto exit;
      }
    }
    else
    {
      /*
       * No valid operation variable has been entered.
       */
      send_string_to_screen(
          "*** Should not reach here.\r\n",
          FALSE) ;
      ui_ret = ERR_003 ;
      goto exit ;
    }
  }
  /*
   * Else, there are parameters on the line (not just 'tevb').
   */
  else
  {
    /*
     * No valid TEVB variable has been entered.
     * There must be at least one!
     */
    send_string_to_screen(
        "\r\n"
        "*** No valid parameter has been indicated for handling.\r\n"
        "    At least one must be entered.\r\n",
        FALSE) ;
    ui_ret = ERR_004 ;
    goto exit ;
  }

exit:


  if (get_val_of_err)
  {
    /*
     * Value of some parameter could not be ui_retrieved.
     */
    send_string_to_screen(
        "\r\n\n"
        "*** Procedure \'get_val_of\' ui_returned with error indication:\r\n",TRUE) ;
    send_string_to_screen(err_msg,TRUE) ;
    ui_ret = TRUE ;
  }
  return (ui_ret) ;
}


/* } */
#endif /*LINK_TIMNA_LIBRARIES*/


