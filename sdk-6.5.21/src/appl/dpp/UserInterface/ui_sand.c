/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <appl/diag/dpp/ref_sys.h>


#ifdef __DUNE_HRP__
  #include <DHRP/dhrp_defs.h>
#elif defined __DUNE_SSF__
#else
#if !DUNE_BCM
  #include <bcm_app/dpp/../H/usrApp.h>

  #include <FMC/fap10m_agent.h>
  #include <FMC/fe200_agent.h>
  #include <FMC/fmc_common_agent.h>

  #include <soc/dpp/SOC_SAND_FE200/fe200_api_general.h>
#endif
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/ui_defx.h>

#include <appl/dpp/UserInterface/ui_defi.h>


#include <soc/dpp/SAND/Utils/sand_trace.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_tcm.h>

#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>

#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <appl/diag/dpp/utils_host_board.h>

#if (defined(LINUX) || defined(UNIX)) || defined(__DUNE_GTO_BCM_CPU__)

unsigned long
  fe200_mem_read(
    int     unit,
    unsigned long    *result_ptr,
    unsigned long    offset,
    unsigned int     size,
    unsigned int     indirect
  )
{
  return 0;
}

unsigned long
  fe200_mem_write(
    int  unit,
    unsigned long *result_ptr,
    unsigned long offset,
    unsigned int  size,
    unsigned int  indirect
  )
{
  return 0;
}
/*} LINUX */
#endif

/*
 */
/*****************************************************
*NAME
*  subject_sand
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'soc_sand' subject.
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
int subject_sand(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  int
    ret;
  char
    err_msg[80*4] = "";

  ret = FALSE ;
  send_string_to_screen("",TRUE);
  /*
   * the rest ot the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'soc_sand').
     * This should be protected against by the calling environment.
     */
    send_string_to_screen(
      "\r\n"
      "*** There are no parameters on the line (just \'soc_sand\'). SW error.\r\n",TRUE) ;
    ret = TRUE ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'soc_sand').
   */
  {
    unsigned int match_index;
    if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_TRANSLATE_TO_PROC_ID_ID,1))
    {
      char
        msg[80*4] = "";
      PARAM_VAL
        *param_proc_id;
      unsigned long
        proc_id,
        return_code,
        error_code;
      char
        *soc_sand_module_name,
        *proc_name,
        *error_name,
        *error_desc;
      /*
       */
      soc_sand_module_name = NULL;
      proc_name        = NULL;
      error_name       = NULL;
      error_desc       = NULL;
      ret = 0;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_SAND_TRANSLATE_TO_PROC_ID_ID, 1,
                        &param_proc_id, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      return_code = param_proc_id->value.ulong_value;
      proc_id    = (unsigned long)soc_sand_get_proc_id_from_error_word   (return_code);
      error_code = (unsigned long)soc_sand_get_error_code_from_error_word(return_code);
      soc_sand_proc_id_to_string(proc_id, &soc_sand_module_name, &proc_name);
      soc_sand_error_code_to_string(error_code, &error_name, &error_desc);
      /*
       */
      start_print_services();
      sal_sprintf(msg,
              "Module Name : %s\n\r"
              "Procude Name: %s",
              soc_sand_module_name,
              proc_name);
      send_string_to_screen(msg, TRUE);
      sal_sprintf(msg,
              "Err Name    : %s\n\r"
              "Err Descrip : ",
              error_name);
      send_string_to_screen(msg, FALSE);
      send_string_to_screen(error_desc, TRUE);
      end_print_services();
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_PRINT_WHEN_WRITING_ID,1))
    {
      unsigned int
        enable = TRUE,
        print_ind = TRUE,
        asic_style = FALSE;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_PRINT_WHEN_WRITING_DIS_ID,1))
      {
        enable = FALSE;
      }
      if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_PRINT_WHEN_WRITING_IND_ID,1))
      {
        print_ind = FALSE;
      }
      if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_PRINT_WHEN_WRITING_ASIC_STYLE_ID,1))
      {
        asic_style = TRUE;
      }
      soc_sand_set_print_when_writing(enable, asic_style, print_ind);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_CALL_SYSTEM_ID,1))
    {
      PARAM_VAL
        *param_val;
       char
        free_text[MAX_CHARS_ON_SCREEN_LINE + 1] ;
      unsigned
        int
          free_text_len ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_SAND_CALL_SYSTEM_ID,1,
              &param_val,VAL_TEXT,err_msg))
      {
        /*
         * Value of filename parameter could not be retrieved.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error (regarding \'rem\'):\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      memset(free_text,BLANK_SPACE,sizeof(free_text)) ;
      free_text[(sizeof(free_text) - 1)] = 0 ;
      free_text_len = strlen(param_val->value.val_text) ;
      if (free_text_len > (sizeof(free_text) - 1))
      {
        free_text_len = (sizeof(free_text) - 1) ;
      }
      memcpy(free_text,param_val->value.val_text,free_text_len) ;
      send_array_to_screen("\r",1) ;
      system(free_text);
      /*send_string_to_screen(free_text,FALSE) ;*/
    }
/*
    {
       char
        *args = "";

       UI_MACROS_INIT_FUNCTION("system call");
       UI_MACROS_GET_TEXT_VAL(PARAM_SAND_CALL_SYSTEM_ID);
       UI_MACROS_CHECK_GET_VAL_OF_ERROR;
       UI_MACROS_LOAD_TEXT_VAL(args);
       system(args);
    }
*/
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_TRACE_PRINT_ID,1))
    {
      start_print_services();
      soc_sand_trace_print();
      end_print_services();
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_TRACE_CLEAR_ID,1))
    {
      soc_sand_trace_init();
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_TCM_PRINT_DELTA_LIST_ID,1))
    {
      start_print_services();
      soc_sand_tcm_print_delta_list();
      end_print_services();
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_OS_RESOURCE_PRINT_ID,1))
    {
      start_print_services();
      end_print_services();
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_STATUS_ID,1))
    {
      start_print_services();
      soc_sand_status_print();
      send_string_to_screen("", TRUE);
#if !DUNE_BCM
#if !(defined(LINUX) || defined(UNIX))
/* { */
      d_printf("sizeof(FAP10M_AGENT_OUT_STRUCT) = %u = %uK \n\r",
             sizeof(FAP10M_AGENT_OUT_STRUCT),
             sizeof(FAP10M_AGENT_OUT_STRUCT)/1024
             );
      d_printf("sizeof(FE200_AGENT_OUT_STRUCT) = %u = %uK \n\r",
             sizeof(FE200_AGENT_OUT_STRUCT),
             sizeof(FE200_AGENT_OUT_STRUCT)/1024
             );
      d_printf("sizeof(FMC_COMMON_IN_STRUCT) = %u = %uK \n\r",
             sizeof(FMC_COMMON_IN_STRUCT),
             sizeof(FMC_COMMON_IN_STRUCT)/1024
             );
      d_printf("sizeof(FMC_COMMON_OUT_STRUCT) = %u = %uK \n\r",
             sizeof(FMC_COMMON_OUT_STRUCT),
             sizeof(FMC_COMMON_OUT_STRUCT)/1024
             );
#endif /*} LINUX */
#endif


#define SOC_SAND_STATUS_MEASURE_MEM_ACCESS_TIME 1
#if defined(SOC_SAND_STATUS_MEASURE_MEM_ACCESS_TIME) && !(defined(LINUX) || defined(UNIX)) && !defined(__DUNE_GTO_BCM_CPU__)
      {
        unsigned short card_type;
        unsigned long  mem_mirror[640];
        long  time_before, time_between, time_after;
#if !DUNE_BCM
#if LINK_FE200_LIBRARIES
        unsigned long  offset;
#endif
#if LINK_FE200_LIBRARIES || LINK_PSS_LIBRARIES

        
        unsigned int   indirect;
        char direct_str[10];
#endif
#endif
        /**/
        host_board_type_from_nv(&card_type);
        switch( card_type )
        {
          case (FABRIC_CARD_01):
          {
            /* measure before */
            time_before = get_watchdog_time(FALSE);
            /**/

            /* read 640 longs */
            soc_sand_physical_read_from_chip(
              mem_mirror, (unsigned long*)0x40000000, 0x1A00, 2560
            );
            /**/

            /* measure between */
            time_between = get_watchdog_time(FALSE);
            /**/

            /* write 640 longs */
            soc_sand_physical_write_to_chip(
              mem_mirror, (unsigned long*)0x40000000, 0x1A00, 2560
            );
            /**/

            /* measure after */
            time_after = get_watchdog_time(FALSE);
            /**/

            /* print */
            d_printf(
              "\r\n"
              " --> Soc_sand status mem access time measurement:\r\n"
              "     read  2560 bytes (640 longs): %lu [us] (%lu [us] for 1 long)\r\n"
              "     write 2560 bytes (640 longs): %lu [us] (%lu [us] for 1 long)\r\n",
              (time_between - time_before), (time_between - time_before)/640,
              (time_after - time_between), (time_after - time_between)/640
            );
            /**/
#if LINK_FE200_LIBRARIES
            for(indirect=0; indirect<2; ++indirect)
            {
              offset = (indirect)? 0x30000000 : 0x1A00;
              strcpy(direct_str, (indirect)? "indirect" : "direct");
              /* measure before */
              time_before = get_watchdog_time(FALSE);
              /**/

              /* read 64 longs */
              fe200_mem_read(0, mem_mirror, offset, 256, indirect);
              /**/

              /* measure between */
              time_between = get_watchdog_time(FALSE);
              /**/

              /* write 64 longs */
              fe200_mem_write(0, mem_mirror, offset, 256, indirect);
              /**/

              /* measure after */
              time_after = get_watchdog_time(FALSE);
              /**/

              /* print */
              d_printf(
                "     fe200_read  %s 256 bytes (64 longs): %lu [us] (%lu [us] for 1 long)\r\n"
                "     fe200_write %s 256 bytes (64 longs): %lu [us] (%lu [us] for 1 long)\r\n",
                direct_str, (time_between - time_before), (time_between - time_before)/64,
                direct_str, (time_after - time_between), (time_after - time_between)/64
              );
            }
#endif /*#if LINK_FE200_LIBRARIES*/
            /**/
            break;
          }
#if LINK_PSS_LIBRARIES
          case (LINE_CARD_01):
          case (LINE_CARD_FAP10M_B):
          {
            /* measure before */
            time_before = get_watchdog_time(FALSE);
            /**/

            /* read 64 longs */
            soc_sand_physical_read_from_chip(
              mem_mirror, (unsigned long*)0x40000000,  0, 2560
            );
            /**/

            /* measure between */
            time_between = get_watchdog_time(FALSE);
            /**/

            /* write 64 longs */
            soc_sand_physical_write_to_chip(
              mem_mirror, (unsigned long*)0x40000000,  0, 2560
            );
            /**/

            /* measure after */
            time_after = get_watchdog_time(FALSE);
            /**/

            /* print */
            d_printf(
              "\r\n --> Soc_sand status mem access time measurement:\r\n"
              "     read  2560 bytes (640 longs): %lu [us] (%lu [us] for 1 long)\r\n"
              "     write 2560 bytes (640 longs): %lu [us] (%lu [us] for 1 long)\r\n",
              (time_between - time_before), (time_between - time_before)/640,
              (time_after - time_between), (time_after - time_between)/640
            );
            /**/
            for(indirect=0; indirect<2; ++indirect)
            {
              /* measure before */
              time_before = get_watchdog_time(FALSE);
              /**/

              /* read 64 longs */
              fap10m_mem_read(0, mem_mirror, 0, 256, indirect);
              /**/

              /* measure between */
              time_between = get_watchdog_time(FALSE);
              /**/

              /* write 64 longs */
              fap10m_mem_write(0, mem_mirror, 0, 256, indirect);
              /**/
              /* measure after */
              time_after = get_watchdog_time(FALSE);
              /**/

              /* print */
              strcpy(direct_str, (indirect)? "indirect" : "direct");
              d_printf(
                "     fap10m_read  %s 256 bytes (64 longs): %lu [us] (%lu [us] for 1 long)\r\n"
                "     fap10m_write %s 256 bytes (64 longs): %lu [us] (%lu [us] for 1 long)\r\n",
                direct_str, (time_between - time_before), (time_between - time_before)/64,
                direct_str, (time_after - time_between), (time_after - time_between)/64
              );
            }
            /**/
            break;
          }
#endif

          default:
          {
            d_printf(
              "\r\n*** Soc_sand status mem access time measurment:\r\n"
              "     Unknown card type (%d) expecting Line (%d/%d) or Fabric (%d) cards \r\n",
              card_type, LINE_CARD_01, LINE_CARD_FAP10M_B, FABRIC_CARD_01
            );
            break;
          }
        }
      }
#endif

      end_print_services();
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_DEVICE_TYPE_ID,1))
    {
      int unit;
      SOC_SAND_DEVICE_TYPE
        chip_type;
      uint32
        chip_ver,
        dbg_ver;
      PARAM_VAL
        *param_val_device;

      /*
       * Show the device type and version.
       */

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_SAND_DEVICE_TYPE_ID,1,
              &param_val_device,VAL_NUMERIC,err_msg))
      {
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      unit =  (int)param_val_device->value.ulong_value;
      soc_sand_get_device_type(
        unit,
        &chip_type,
        &chip_ver,
        &dbg_ver
      );


      start_print_services();
      soc_sand_os_printf( 
        "  The device is %s with hardware version: %d.%d.\n\r",
        soc_sand_DEVICE_TYPE_to_str(chip_type),
        chip_ver,
        dbg_ver
      );
      end_print_services();
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SAND_TCM_ID,1))
    {
      PARAM_VAL
        *soc_tcmenable_id;
      unsigned long
        soc_tcmenable;

      /*
       * Enable/Disable the soc_sand TCM.
       */

      if (get_val_of(
              current_line,(int *)&match_index,PARAM_SAND_TCM_ID,1,
              &soc_tcmenable_id,VAL_SYMBOL,err_msg))
      {
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      soc_tcmenable = (unsigned int)soc_tcmenable_id->numeric_equivalent ;
      soc_sand_tcm_set_enable_flag(soc_tcmenable);
      start_print_services();
      soc_sand_os_printf( 
        "  TCM enable flag was set to %lu.\n\r",
        soc_tcmenable
      );
      end_print_services();
    }
    else
    {
      /*
       * No valid SOC_SAND variable has been entered.
       * There must be at least one!
       */
      send_string_to_screen(
          "\r\n"
          "*** No valid parameter has been indicated for handling.\r\n"
          "    At least one must be entered.\r\n",
          FALSE) ;
      ret = 99 ;
      goto exit ;
    }
  }
exit:
  return (ret) ;
}

