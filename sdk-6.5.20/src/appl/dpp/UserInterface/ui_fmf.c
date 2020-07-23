/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if !DUNE_BCM

/*
 * Basic_include_file.
 */

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * INCLUDE FILES:
 */
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
/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include <private/timerLibP.h>
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
#include <appl/diag/dpp/utils_pure_defi.h>
#include <appl/diag/dpp/utils_string.h>
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
 * Software emulation exception include file.
 */
#include <appl/diag/dpp/utils_software_emulation_exp.h>
/*
 * Dune/Marvell RPC mechanism.
 */
#include <appl/diag/dpp/dune_rpc.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/fmfDriver/fmf_sys_wide_defs.h>

#else
int ui_fmf = 0;
#endif

#if FMF_INCLUDED
/* { */
/*
 * SOC_SAND include files
 */
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_trace.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_callback_handles.h>
#include <soc/dpp/SAND/Management/sand_module_management.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
/*
 * SOC_SAND_FAP10M include files
 */
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_links.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_diagnostics.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_routing.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_configuration.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_scheduler_flows.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_scheduler_ports.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_cell.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_packet.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_spatial_multicast.h>
/*
 * SOC_SAND_FE200 include files
 */
#include <soc/dpp/SOC_SAND_FE200/fe200_api_framework.h>
#include <soc/dpp/SOC_SAND_FE200/fe200_api_links.h>
#include <soc/dpp/SOC_SAND_FE200/fe200_api_cell.h>
#include <soc/dpp/SOC_SAND_FE200/fe200_api_routing.h>

/*
 * DCL
 */
#include <DCL/dcl_transport_layer_extern.h>


/*
 * FMF include files.
 */
#include <soc/dpp/fmfDriver/fmf_api_definitions.h>
#include <soc/dpp/FMC_N/General/fmc_n_errors_and_debug.h>


/*
 * The clients's error handler
 * {
 */
/*
 * The error buffer that is registered with the TCM
 */
char Fmf_agent_error_buffer[SOC_SAND_CALLBACK_BUF_SIZE];

/*****************************************************
*NAME
*  disp_fmf_result
*TYPE: PROC
*DATE: 18/SEP/2003
*FUNCTION:
*  Display return value and related information for
*  FMF driver services.
*CALLING SEQUENCE:
*  disp_fmf_result(fmf_api_result,proc_name)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long fmf_api_result -
*      Return value of any FMF driver service.
*    char          *proc_name -
*      Name of procedure for which 'fmf_api_result'
*      was returned.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Printed error details.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  disp_fmf_result(
    unsigned long fmf_api_result,
    char          *proc_name
  )
{
  int
    ret ;
  unsigned
    short
      proc_id ;
  unsigned
    long
      err_id ;
  char
    *err_text,
    *soc_sand_proc_name,
    *soc_sand_module_name ;
  char
    err_msg[80*6] = "" ;
  ret = 0 ;
  err_id = fmf_get_err_text(fmf_api_result,&err_text) ;
  /*
   * Let driver print whatever it wishes before going ahead here.
   */
  taskDelay(60) ;
  if (soc_sand_get_error_code_from_error_word(err_id) != SOC_SAND_OK)
  {
    err_text = "No error code description (or procedure id) found" ;
  }
  proc_id = soc_sand_get_proc_id_from_error_word(fmf_api_result) ;
  fmf_proc_id_to_string((unsigned long)proc_id,&soc_sand_module_name,&soc_sand_proc_name)  ;
  if (soc_sand_get_error_code_from_error_word(fmf_api_result) != SOC_SAND_OK)
  {
    sal_sprintf(err_msg,
      "---->---->\r\n"
      "\'%s\' -\r\n"
      "  returned with code 0x%lX (fail):\r\n\n"
      "Text        :\r\n"
      "%s\r\n"
      "Procedure id: 0x%04X (Mod: %s, Proc: %s)",
      proc_name,fmf_api_result,err_text,
      (unsigned short)proc_id,soc_sand_module_name,soc_sand_proc_name) ;
  }
  else
  {
    sal_sprintf(err_msg,
      "---->---->\r\n"
      "\'%s\' -\r\n"
      "  returned with code 0x%lX (success):\r\n\n"
      "Text        : %s\r\n"
      "Procedure id: 0x%04X (Mod: %s, Proc: %s)",
      proc_name,fmf_api_result,err_text,
      (unsigned short)proc_id,soc_sand_module_name,soc_sand_proc_name) ;
  }
  send_string_to_screen(err_msg,TRUE) ;
  return (ret) ;
}

/*
 */
/*****************************************************
*NAME:
*  fmf_agent_error_handler
*DATE:
*  18/SEP/2003
*FUNCTION:
*  Error handler callback that is registered
*  with the driver
*INPUT:
*  SOC_SAND_DIRECT:
*    - see documentation of user_error_handler()
*      prototype in fmf_api_definition.h
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*  SOC_SAND_INDIRECT:
*REMARKS:
*  None
*SEE ALSO:
 */
STATIC
  int
    fmf_agent_error_handler(
      unsigned long   err_id,
      char            *err_desc,
      char            **new_buf,
      unsigned long   param_01,
      unsigned long   param_02,
      unsigned long   param_03,
      unsigned long   param_04,
      unsigned long   param_05,
      unsigned long   param_06
    )
{
  char
    *proc_name,
    ascii_ver[7],
    text_array[8*80] ;
  int
    ret ;
  unsigned
    long
      time_from_startup ;
  static
    int
      Num_called = 0 ;
  ret = 0 ;
  Num_called++ ;
  /*
   * Get time from startup in microseconds.
   */
  time_from_startup = get_watchdog_time(0) ;
  proc_name = "fmf_agent_error_handler" ;
  {
    /*
     * Make sure print services are ordered (various sources are not mixed)
     * and also long printouts get the option of 'page mode'
     */
    start_print_services() ;
    sal_sprintf(text_array,"\r\n---->\r\n"
           "%s called with error id 0x%lX\r\n"
           "Ordinal: %lu ;  Time from startup: %lu microseconds\r\n"
           "Params : 0x%lX, 0x%lX, 0x%lX, 0x%lX, 0x%lX, 0x%lX\r\n"
           "SW ver : %s\r\n",
           proc_name,err_id,
           (unsigned long)Num_called,(unsigned long)time_from_startup,
           param_01, param_02, param_03, param_04, param_05, param_06,
           version_to_ascii(ascii_ver,soc_get_sand_ver(),1)
        );
    send_string_to_screen(text_array,FALSE) ;
    disp_fmf_result(err_id,proc_name) ;
    end_print_services() ;
  }
  /*
   * No buffer replacing
   */
  new_buf = NULL ;
  return (ret) ;
}
/*
 * The clients's error handler
 * }
 */

/*****************************************************
*NAME
*  subject_fmf
*TYPE: PROC
*DATE: 18/SEP/2003
*FUNCTION:
*  Process input line which has a 'fmf' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_fmf(current_line,current_line_ptr)
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
int
  subject_fmf(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    ERR_006, ERR_007, ERR_008, ERR_009, ERR_010,
    ERR_011, ERR_012, ERR_013, ERR_014, ERR_015,
    ERR_016, ERR_017, ERR_018, ERR_019, ERR_020,
    ERR_021, ERR_022, ERR_023, ERR_024, ERR_025,
    ERR_026, ERR_027, ERR_028, ERR_029, ERR_030,
    ERR_031, ERR_032, ERR_033, ERR_034, ERR_035,
    ERR_036, ERR_037, ERR_038, ERR_039, ERR_040,
    ERR_041, ERR_042, ERR_043, ERR_044, ERR_045,
    ERR_046, ERR_047,
    NUM_EXIT_CODES
  } ret ;
  int
    err ;
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*10] = "";
  unsigned
    short
      soc_sand_err ;
  unsigned
    int
      get_val_of_err;
  char
    *proc_name ;
  unsigned
    long
      error_id ;
  char
    *soc_sand_proc_name ;
  proc_name = "subject_fmf" ;
  ret = FALSE ;
  get_val_of_err = FALSE ;
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'general').
     * This should be protected against by the calling environment.
     */
    send_string_to_screen(
      "\r\n\n"
      "*** There are no parameters on the line (just \'fmf\'). SW error.\r\n",TRUE) ;
    ret = TRUE ;
    goto exit ;
  }
  else
  {
    /*
     * Enter if there are parameters on the line (not just 'fmf').
     */
    unsigned
      int
        match_index,
        num_handled ;
    num_handled = 0 ;
    if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_DEBUG_ID,1))
    {
      /*
       * Enter if this is a line starting with "fmf debug"
       */
      num_handled++ ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_CONNECT_PP_TO_PP_ID,1))
      {
        OPERATION_RANGE
          operation_range_src_pp,
          operation_range_dst_pp ;
        unsigned
          long
            pp_identifier_src,
            pp_identifier_dst ;
        unsigned
          int
            use_flow,
            use_flow_start_class,
            use_flow_end_class,
            use_flow_dest_id,
            use_flow_first_flow_id ;
        use_flow_start_class = 0 ;
        use_flow_end_class = 0 ;
        use_flow_dest_id = 0 ;
        use_flow_first_flow_id = 0 ;
        /*
         * Enter if this is a line starting with "fmf debug connect_pp_to_pp"
         */
        sal_sprintf(err_msg,
                "\r\n"
                "Going to flow-connect one or many PPs to one or many PPs...\r\n\n"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
        /*
         * Just to silence 'might be used uninitialized'
         */
        pp_identifier_src = pp_identifier_dst = (unsigned int)0 ;
        if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_FROM_ONE_PP_ID,1))
        {
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_PP_ID_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          if (!err)
          {
            pp_identifier_src = (unsigned int)(param_val->value.ulong_value) ;
          }
          else
          {
           /*
            * Could not find "pp_id"
            * on line starting with "fmf debug connect_pp_to_pp from_one_pp".
            * Probably SW error...
            */
            get_val_of_err = TRUE ;
            ret = ERR_001 ;
            goto exit ;
          }
          operation_range_src_pp = SPECIFIED_ONE ;
        }
        else if (!search_param_val_pairs(
                      current_line,&match_index,PARAM_FMF_FROM_ALL_PPS_ID,1))
        {
          operation_range_src_pp = FORCE_ALL ;
        }
        else
        {
         /*
          * Could not find "from_all_pps" or "from_one_pp"
          * on line starting with "fmf debug connect_pp_to_pp".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_002 ;
          goto exit ;
        }
        if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_TO_ONE_PP_ID,1))
        {
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_DST_PP_ID_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          if (!err)
          {
            pp_identifier_dst = (unsigned int)(param_val->value.ulong_value);
          }
          else
          {
           /*
            * Could not find "pp_id"
            * on line starting with "fmf debug connect_pp_to_pp ... to_one_pp".
            * Probably SW error...
            */
            get_val_of_err = TRUE ;
            ret = ERR_003 ;
            goto exit ;
          }
          operation_range_dst_pp = SPECIFIED_ONE ;
        }
        else if (!search_param_val_pairs(
                      current_line,&match_index,PARAM_FMF_TO_ALL_PPS_ID,1))
        {
          operation_range_dst_pp = FORCE_ALL ;
        }
        else
        {
         /*
          * Could not find "to_all_pps" or "to_one_pp"
          * on line starting with "fmf debug connect_pp_to_pp ...".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_004 ;
          goto exit ;
        }
        /*
         * Write code so 'use_flow' might be dropped out.
         */
        if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_USE_FLOW_ID,1))
        {
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_USE_FLOW_ID,1,
                &param_val,VAL_SYMBOL,err_msg) ;
          if (!err)
          {
            if ((unsigned int)(param_val->numeric_equivalent) == ON_EQUIVALENT)
            {
              use_flow = TRUE ;
            }
            else
            {
              use_flow = FALSE ;
            }
            /*
             * If use_flow is set then look here for other parameters. Currently,
             * this is not implemented.
             */
            if (use_flow)
            {
             /*
              * Could not find "use_flow"
              * on line starting with "fmf debug connect_pp_to_pp ...".
              * Probably SW error...
              */
              get_val_of_err = TRUE ;
              ret = ERR_005 ;
              goto exit ;
            }
          }
          else
          {
           /*
            * Found "use_flow"
            * on line starting with "fmf debug connect_pp_to_pp ...".
            * Currerntly, this option is not implemented...
            */
            send_string_to_screen(
              "\r\n\n"
              "*** Found \"use_flow\". Currerntly, this option is not implemented...\r\n",
              FALSE) ;
            ret = ERR_006 ;
            goto exit ;
          }
        }
        else
        {
          use_flow = FALSE ;
        }
        {
          soc_sand_proc_name = "fmf_api_connect_pp_to_pp" ;
          error_id =
            fmf_api_connect_pp_to_pp(
                operation_range_src_pp,
                operation_range_dst_pp,
                pp_identifier_src,pp_identifier_dst,
                use_flow,use_flow_start_class,
                use_flow_end_class,
                use_flow_dest_id,
                use_flow_first_flow_id) ;
          disp_fmf_result(error_id,soc_sand_proc_name) ;
          soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
          if (soc_sand_err != SOC_SAND_OK)
          {
            /*
             * fmf_api_connect_pp_to_pp() returned with error.
             */
            send_string_to_screen(
              "\r\n\n"
              "*** Could not set flows from PP to PP using fmf_api_connect_pp_to_pp().\r\n",
              FALSE) ;
            ret = ERR_007 ;
            goto exit ;
          }
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_INIT_BASE_PARAMS_ID,1))
      {
        unsigned
          int
            device_type,
            device_sub_type,
             ;
        OPERATION_RANGE
          operation_range ;
        /*
         * Enter if this is a line starting with "fmf debug init_base_params"
         */
        sal_sprintf(err_msg,
                "\r\n"
                "Going to initialize base parameters...\r\n\n"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
        /*
         * Just to silence 'might be used uninitialized'
         */
        device_type = device_sub_type = unit = 0 ;
        if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_ONE_SPECIFIC_DEVICE_ID,1))
        {
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_DEVICE_ID_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          if (!err)
          {
            unit = (unsigned int)(param_val->value.ulong_value);
            device_type = SOC_SAND_FMF_DEV_TYPE_FROM_DEV_ID(unit) ;
          }
          else
          {
           /*
            * Could not find "unit"
            * on line starting with "fmf debug init_base_params one_specific_device".
            * Probably SW error...
            */
            get_val_of_err = TRUE ;
            ret = ERR_008 ;
            goto exit ;
          }
          operation_range = SPECIFIED_ONE ;
        }
        else if (!search_param_val_pairs(
                      current_line,&match_index,PARAM_FMF_ALL_SPECIFIED_DEVICES_ID,1))
        {
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_DEVICE_TYPE_ID,1,
                &param_val,VAL_NUMERIC | VAL_SYMBOL,err_msg) ;
          if (!err)
          {
            if (!get_val_of(
                  current_line,(int *)&match_index,PARAM_FMF_DEVICE_TYPE_ID,1,
                  &param_val,VAL_NUMERIC,err_msg))
            {
              device_type = (unsigned int)(param_val->value.ulong_value) ;
            }
            else
            {
              device_type = (unsigned int)(param_val->numeric_equivalent) ;
            }
          }
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_DEVICE_SUB_TYPE_ID,1,
                &param_val,VAL_NUMERIC | VAL_SYMBOL,err_msg) ;
          if (!err)
          {
            if (!get_val_of(
                  current_line,(int *)&match_index,PARAM_FMF_DEVICE_SUB_TYPE_ID,1,
                  &param_val,VAL_NUMERIC,err_msg))
            {
              device_sub_type = (unsigned int)(param_val->value.ulong_value) ;
            }
            else
            {
              device_sub_type = (unsigned int)(param_val->numeric_equivalent) ;
            }
          }
          else
          {
           /*
            * Could not find "device_type" or "device_sub_type"
            * on line starting with "fmf debug init_base_params all_specified_devices".
            * Probably SW error...
            */
            get_val_of_err = TRUE ;
            ret = ERR_011 ;
            goto exit ;
          }
          operation_range = FORCE_ALL ;
        }
        else
        {
         /*
          * Could not find "all_specified_devices" or "one_specific_device"
          * on line starting with "fmf debug init_base_params".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_012 ;
          goto exit ;
        }
        if (operation_range == FORCE_ALL)
        {
          if (device_type == FAP_DEVICE_TYPE_EQUIVALENT)
          {
            if ((device_sub_type != FAP10M_DEVICE_SUBTYPE_EQUIVALENT) &&
                          (device_sub_type != ANY_DEVICE_SUBTYPE_EQUIVALENT))
            {
              /*
               * Illegal device type.
               */
              sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** Illegal input device sub type (= %lu. Should be %lu or %lu)\r\n",
                    (unsigned long)device_sub_type,
                    (unsigned long)FAP10M_DEVICE_SUBTYPE_EQUIVALENT,
                    (unsigned long)ANY_DEVICE_SUBTYPE_EQUIVALENT
                    ) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_014 ;
              goto exit ;
            }
            else
            {
              if (device_sub_type == FAP10M_DEVICE_SUBTYPE_EQUIVALENT)
              {
                device_sub_type = SOC_SAND_FAP10M_FAP_TYPE ;
              }
              else
              {
                device_sub_type = SOC_SAND_FAP_TYPE_NONE ;
              }
            }
          }
          else if ((device_type == FE2_DEVICE_TYPE_EQUIVALENT) ||
                      (device_type == FE13_DEVICE_TYPE_EQUIVALENT))
          {
            if ((device_sub_type != FE200_DEVICE_SUBTYPE_EQUIVALENT) &&
                          (device_sub_type != ANY_DEVICE_SUBTYPE_EQUIVALENT))
            {
              /*
               * Illegal device type.
               */
              sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** Illegal input device sub type (= %lu. Should be %lu or %lu)\r\n",
                    (unsigned long)device_sub_type,
                    (unsigned long)FE200_DEVICE_SUBTYPE_EQUIVALENT,
                    (unsigned long)ANY_DEVICE_SUBTYPE_EQUIVALENT
                    ) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_015 ;
              goto exit ;
            }
            else
            {
              if (device_sub_type == FE200_DEVICE_SUBTYPE_EQUIVALENT)
              {
                device_sub_type = SOC_SAND_FE200_FE_TYPE ;
              }
              else
              {
                device_sub_type = SOC_SAND_FE_TYPE_NONE ;
              }
            }
          }
          else
          {
            if (device_sub_type != ANY_DEVICE_SUBTYPE_EQUIVALENT)
            {
              /*
               * Illegal device type.
               */
              sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** Illegal input device sub type (= %lu. Should be %lu)\r\n",
                    (unsigned long)device_sub_type,
                    (unsigned long)ANY_DEVICE_SUBTYPE_EQUIVALENT
                    ) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_015 ;
              goto exit ;
            }
          }
        }
        else
        {
          if ((device_type == SOC_SAND_DEVICE_TYPE_NONE) || ((device_type >= SOC_SAND_MAX_DEVICE_TYPE)))
          {
            /*
             * Illegal device type.
             */
            sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Illegal input device type (= %lu. Should be less than %lu, ne %lu)\r\n",
                  (unsigned long)device_type,
                  (unsigned long)SOC_SAND_MAX_DEVICE_TYPE,
                  (unsigned long)SOC_SAND_DEVICE_TYPE_NONE
                  ) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_013 ;
            goto exit ;
          }
        }
        {
          soc_sand_proc_name = "fmf_api_init_base_params" ;
          error_id =
            fmf_api_init_base_params(
              operation_range,unit,device_type,
              device_sub_type,(DEVICE_BASE_PARAMS *)0) ;
          disp_fmf_result(error_id,soc_sand_proc_name) ;
          soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
          if (soc_sand_err != SOC_SAND_OK)
          {
            /*
             * fmf_api_init_base_params() returned with error.
             */
            send_string_to_screen(
              "\r\n\n"
              "*** Could not init base parameters using fmf_api_init_base_params().\r\n",
              FALSE) ;
            ret = ERR_016 ;
            goto exit ;
          }
        }
        if ((device_type == SOC_SAND_FAP_DEVICE) || (operation_range == FORCE_ALL))
        {
          unsigned
            int
              num_pp_ports,
              pp_identifier ;
          SOC_SAND_FMF_PP_TYPE
            pp_type ;
          soc_sand_proc_name = "fmf_api_apply_q_wred_model" ;
          error_id =
            fmf_api_apply_q_wred_model(
                (int)(-1),operation_range,SOC_SAND_FMF_CHIP_ID_FROM_DEV_ID(unit)) ;
          disp_fmf_result(error_id,soc_sand_proc_name) ;
          soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
          if (soc_sand_err != SOC_SAND_OK)
          {
            /*
             * fmf_api_apply_q_wred_model() returned with error.
             */
            send_string_to_screen(
              "\r\n\n"
              "*** Could not apply q and wred model using fmf_api_apply_q_wred_model().\r\n",
              FALSE) ;
            ret = ERR_017 ;
            goto exit ;
          }
          num_pp_ports = 12 ;
          pp_identifier = SOC_SAND_FMF_CHIP_ID_FROM_DEV_ID(unit) ;
          pp_type = SOC_SAND_PP_MARVELL_PRESTERA ;
          soc_sand_proc_name = "fmf_api_apply_pp_details" ;
          error_id =
            fmf_api_apply_pp_details(
                operation_range,SOC_SAND_FMF_CHIP_ID_FROM_DEV_ID(unit),
                num_pp_ports,SOC_SAND_FMF_CHIP_ID_FROM_DEV_ID(unit),pp_type
            ) ;
          disp_fmf_result(error_id,soc_sand_proc_name) ;
          soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
          if (soc_sand_err != SOC_SAND_OK)
          {
            /*
             * fmf_api_apply_pp_details() returned with error.
             */
            send_string_to_screen(
              "\r\n\n"
              "*** Could not apply pp details using fmf_api_apply_pp_details().\r\n",
              FALSE) ;
            ret = ERR_018 ;
            goto exit ;
          }
          soc_sand_proc_name = "fmf_api_attach_pp_ports_to_sched" ;
          error_id =
            fmf_api_attach_pp_ports_to_sched(
                operation_range,SOC_SAND_FMF_CHIP_ID_FROM_DEV_ID(unit),
                num_pp_ports,(unsigned int *)0,(unsigned int *)0,0
            ) ;
          disp_fmf_result(error_id,soc_sand_proc_name) ;
          soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
          if (soc_sand_err != SOC_SAND_OK)
          {
            /*
             * fmf_api_attach_pp_ports_to_sched() returned with error.
             */
            send_string_to_screen(
              "\r\n\n"
              "*** Could not attach PP ports using fmf_api_attach_pp_ports_to_sched().\r\n",
              FALSE) ;
            ret = ERR_019 ;
            goto exit ;
          }
          soc_sand_proc_name = "fmf_api_set_port_parameters" ;
          error_id =
            fmf_api_set_port_parameters(
                operation_range,FORCE_ALL,
                0,0,(FMF_SCH_PORT       *)0
            ) ;
          disp_fmf_result(error_id,soc_sand_proc_name) ;
          soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
          if (soc_sand_err != SOC_SAND_OK)
          {
            /*
             * fmf_api_attach_pp_ports_to_sched() returned with error.
             */
            send_string_to_screen(
              "\r\n\n"
              "*** Could not set_port_parameters using fmf_api_set_port_parameters().\r\n",
              FALSE) ;
            ret = ERR_020 ;
            goto exit ;
          }
          soc_sand_proc_name = "fmf_api_apply_class_model" ;
          error_id =
            fmf_api_apply_class_model(
                -1,operation_range,0
            ) ;
          disp_fmf_result(error_id,soc_sand_proc_name) ;
          soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
          if (soc_sand_err != SOC_SAND_OK)
          {
            /*
             * fmf_api_apply_class_model() returned with error.
             */
            send_string_to_screen(
              "\r\n\n"
              "*** Could not apply_class_model using fmf_api_apply_class_model().\r\n",
              FALSE) ;
            ret = ERR_021 ;
            goto exit ;
          }
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_DEBUG_FLAGS_ID,1))
      {
        /*
         * Enter if this is a line starting with "fmf debug debug_flags"
         */
        unsigned
          int
            debug_parameter ;
        err =
          get_val_of(
              current_line,(int *)&match_index,PARAM_FMF_DEBUG_FLAGS_ID,1,
              &param_val,VAL_SYMBOL,err_msg) ;
        if (!err)
        {
          debug_parameter = (unsigned int)(param_val->numeric_equivalent);
        }
        else
        {
         /*
          * Could not find parameter
          * on line starting with "fmf debug debug_flags".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_022 ;
          goto exit ;
        }
        switch (debug_parameter)
        {
          case FMF_DEBUG_ON_EQUIVALENT:
          {
            sal_sprintf(err_msg,
                "\r\n"
                "Going to start FMF debug mode...\r\n\n"
                ) ;
            send_string_to_screen(err_msg,FALSE) ;
            start_fmf_debug_print() ;
            break ;
          }
          case FMF_DEBUG_OFF_EQUIVALENT:
          {
            sal_sprintf(err_msg,
                "\r\n"
                "Going to stop FMF debug mode...\r\n\n"
                ) ;
            send_string_to_screen(err_msg,FALSE) ;
            stop_fmf_debug_print() ;
            break ;
          }
          case DCL_DEBUG_ON_EQUIVALENT:
          {
            sal_sprintf(err_msg,
                "\r\n"
                "Going to start DCL debug mode...\r\n\n"
                ) ;
            send_string_to_screen(err_msg,FALSE) ;
            start_dcl_transport_debug_print() ;
            break ;
          }
          case DCL_DEBUG_OFF_EQUIVALENT:
          {
            sal_sprintf(err_msg,
                "\r\n"
                "Going to stop DCL debug mode...\r\n\n"
                ) ;
            send_string_to_screen(err_msg,FALSE) ;
            stop_dcl_transport_debug_print() ;
            break ;
          }
          case FMC_N_DEBUG_ON_EQUIVALENT:
          {
            sal_sprintf(err_msg,
                "\r\n"
                "Going to start FMC_N debug mode...\r\n\n"
                ) ;
            send_string_to_screen(err_msg,FALSE) ;
            start_fmc_n_debug_print() ;
            break ;
          }
          case FMC_N_DEBUG_OFF_EQUIVALENT:
          {
            sal_sprintf(err_msg,
                "\r\n"
                "Going to stop FMC_N debug mode...\r\n\n"
                ) ;
            send_string_to_screen(err_msg,FALSE) ;
            stop_fmc_n_debug_print() ;
            break ;
          }
          case SHOW_ALL_DEBUG_FLAGS_EQUIVALENT:
          {
            char
              *fmf_text,
              *fmc_n_text,
              *dcl_text ;
            if (is_fmf_debug_print())
            {
              fmf_text =   "FMF debug mode  : ON" ;
            }
            else
            {
              fmf_text =   "FMF debug mode  : OFF" ;
            }
            if (is_dcl_transport_debug_print())
            {
              dcl_text =   "DCL debug mode  : ON" ;
            }
            else
            {
              dcl_text =   "DCL debug mode  : OFF" ;
            }
            if (is_fmc_n_debug_print())
            {
              fmc_n_text = "FMC_N debug mode: ON" ;
            }
            else
            {
              fmc_n_text = "FMC_N debug mode: OFF" ;
            }
            sal_sprintf(err_msg,
                "\r\n"
                "%s\r\n"
                "%s\r\n"
                "%s\r\n",
                fmf_text,fmc_n_text,dcl_text
                ) ;
            send_string_to_screen(err_msg,FALSE) ;
            break ;
          }
          default:
          {
            /*
             * Unknown parameter
             * on line starting with "fmf debug debug_flags".
             * Probably SW error...
             */
            sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** Could not decode \'debug_flags\' parameter (val = %lu).\r\n",
                    (unsigned long)debug_parameter
                ) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_023 ;
            goto exit ;
            break ;
          }
        }
        goto exit ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_SHOW_ID,1))
      {
        /*
         * Enter if this is a line starting with "fmf debug show"
         */
        unsigned
          int
            show_all,
            unit_found,
            unit_id,
            num_devices,
            num_units,
            brief_format ;
        err =
          get_val_of(
              current_line,(int *)&match_index,PARAM_FMF_FORMAT_ID,1,
              &param_val,VAL_SYMBOL,err_msg) ;
        /*
         * Just to silence 'might be used uninitialized'
         */
        unit_id = 0 ;
        if (!err)
        {
          brief_format = (unsigned int)(param_val->numeric_equivalent);
        }
        else
        {
         /*
          * Could not find "format"
          * on line starting with "fmf debug show".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_024 ;
          goto exit ;
        }
        if (brief_format == FORMAT_SHORT_EQUIVALENT)
        {
          brief_format = TRUE ;
        }
        else
        {
          brief_format = FALSE ;
        }
        if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_SHOW_ALL_ID,1))
        {
          /*
           * Enter if this is a line starting with "fmf debug show all"
           */
          show_all = TRUE ;
        }
        else
        {
          /*
           * Enter if this is a line starting with "fmf debug show"
           * follwed by "unit"
           */
          show_all = FALSE ;
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_SHOW_UNIT_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          if (!err)
          {
            unit_id = (unsigned int)(param_val->value.ulong_value) ;
          }
          else
          {
            /*
             * Could not find "unit" or "all"
             * on line starting with "fmf debug show".
             * Probably SW error...
             */
            get_val_of_err = TRUE ;
            ret = ERR_025 ;
            goto exit ;
          }
        }
        sal_sprintf(err_msg,
                "\r\n"
                "Going to show information related to FMF objects...\r\n\n"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
        if (show_all)
        {
          soc_sand_proc_name = "fmf_api_get_num_devices_n_units" ;
          error_id =
            fmf_api_get_num_devices_n_units(&num_devices,&num_units) ;
          disp_fmf_result(error_id,soc_sand_proc_name) ;
          soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
          if (soc_sand_err != SOC_SAND_OK)
          {
            /*
             * fmf_api_get_num_devices_n_units returned with error.
             */
            send_string_to_screen(
                      "\r\n\n"
                      "*** Could not get info using fmf_api_get_num_devices_n_units().\r\n",
                      FALSE) ;
            ret = ERR_026 ;
            goto exit ;
          }
          sal_sprintf(err_msg,
                "\r\n"
                "Number of Units  : %lu\r\n"
                "Number of Devices: %lu\r\n",
                (unsigned long)num_units,(unsigned long)num_devices
                ) ;
          send_string_to_screen(err_msg,FALSE) ;
        }
        else
        {
          /*
           * This must be a request for info on one unit.
           */
          soc_sand_proc_name = "fmf_api_is_there_unit" ;
          error_id = fmf_api_is_there_unit(unit_id,&unit_found) ;
          disp_fmf_result(error_id,soc_sand_proc_name) ;
          soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
          if (soc_sand_err != SOC_SAND_OK)
          {
            /*
             * fmf_api_is_there_unit returned with error.
             */
            send_string_to_screen(
                      "\r\n\n"
                      "*** Could not verify unit validity using fmf_api_is_there_unit().\r\n",
                      FALSE) ;
            ret = ERR_027 ;
            goto exit ;
          }
          if (unit_found)
          {
              sal_sprintf(err_msg,
                "\r\n"
                "Unit %lu is registered...\r\n",
                (unsigned long)unit_id
                ) ;
              send_string_to_screen(err_msg,FALSE) ;
          }
          else
          {
            sal_sprintf(err_msg,
                      "\r\n\n"
                      "*** No such unit (unit_id = %lu) currently registered.\r\n",
                      (unsigned long)unit_id) ;
            send_string_to_screen(err_msg,FALSE) ;
            goto exit ;
          }
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_REGISTER_DRIVER_ID,1))
      {
        /*
         * Enter if this is a line starting with "fmf debug register_driver"
         */
        sal_sprintf(err_msg,
          "\r\n"
          "Going to initialize SOC_SAND and register FMF...\r\n\n"
          ) ;
        send_string_to_screen(err_msg,FALSE) ;
        {
          unsigned
            int
            is_already_opened,
            max_num_devices,
            system_tick_in_ms,
            soc_tcmtask_priority,
            min_time_between_tcm_activation,
            soc_tcmmockup_interrupts ;
          max_num_devices = SOC_SAND_MAX_DEVICE ;
          system_tick_in_ms = 16 ;
          soc_tcmtask_priority = 70 ;
          min_time_between_tcm_activation = 1 ;
          soc_tcmmockup_interrupts = FALSE ;
          soc_sand_proc_name = "soc_sand_module_open" ;
          error_id  =
               soc_sand_module_open(
                        max_num_devices,
                        system_tick_in_ms,
                        soc_tcmtask_priority,
                        min_time_between_tcm_activation,
                        soc_tcmmockup_interrupts,
                        fmf_agent_error_handler,
                        Fmf_agent_error_buffer,&is_already_opened
                       ) ;
          disp_fmf_result(error_id,soc_sand_proc_name) ;
          soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
          if (soc_sand_err != SOC_SAND_OK)
          {
            send_string_to_screen(
                      "\r\n\n"
                      "*** Could not open SOC_SAND using soc_sand_module_open().\r\n",
                      FALSE) ;
            ret = ERR_028 ;
            goto exit ;
          }
        }
        soc_sand_proc_name = "fmf_api_register_system" ;
        error_id = fmf_api_register_system() ;
        disp_fmf_result(error_id,soc_sand_proc_name) ;
        soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
        if (soc_sand_err != SOC_SAND_OK)
        {
          /*
           * fmf_api_register_system returned with error.
           */
          send_string_to_screen(
                    "\r\n\n"
                    "*** Could not register FMF using fmf_api_register_system().\r\n",
                    FALSE) ;
          ret = ERR_029 ;
          goto exit ;
        }
        sal_sprintf(err_msg,
            "\r\n"
            "FMF system has been registered!!\r\n\n"
           ) ;
        send_string_to_screen(err_msg,FALSE) ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_UNREGISTER_DRIVER_ID,1))
      {
        /*
         * Enter if this is a line starting with "fmf debug unregister_driver"
         */
        sal_sprintf(err_msg,
                "\r\n"
                "Going to unregister FMF...\r\n\n"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
        soc_sand_proc_name = "fmf_api_unregister_system" ;
        error_id = fmf_api_unregister_system() ;
        disp_fmf_result(error_id,soc_sand_proc_name) ;
        soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
        if (soc_sand_err != SOC_SAND_OK)
        {
          /*
           * fmf_api_unregister_system() returned with error.
           */
          send_string_to_screen(
            "\r\n\n"
            "*** Could not unregister FMF using fmf_api_unregister_system().\r\n",
            FALSE) ;
          ret = ERR_030 ;
          goto exit ;
        }
        sal_sprintf(err_msg,
                "\r\n"
                "FMF system has been unregistered\r\n\n"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_ADD_DEVICE_ID,1))
      {
        unsigned
          int
            device_type,
            device_sub_type,
            device_chip_id,
            unit_identifier ;
        unsigned
          long
            unit_ip_addr ;
        char
          ip_address_string[3*4+3+2] ;
        /*
         * Enter if this is a line starting with "fmf debug add_device"
         */
        sal_sprintf(err_msg,
                "\r\n"
                "Going to add device to FMF...\r\n\n"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
        /*
         * Just to silence 'might be used uninitialized'
         */
        device_type = device_chip_id = unit_ip_addr = 0 ;
        err =
          get_val_of(
              current_line,(int *)&match_index,PARAM_FMF_DEVICE_CHIP_ID_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          device_chip_id = (unsigned int)(param_val->value.ulong_value) ;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_FMF_UNIT_IP_ADDRESS_ID,1,
              &param_val,VAL_IP,err_msg) ;
        if (!err)
        {
          unit_ip_addr = (unsigned long)(param_val->value.ip_value) ;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_FMF_DEVICE_TYPE_ID,1,
              &param_val,VAL_NUMERIC | VAL_SYMBOL,err_msg) ;
        if (!err)
        {
          if (!get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_DEVICE_TYPE_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
          {
            device_type = (unsigned int)(param_val->value.ulong_value) ;
          }
          else
          {
            device_type = (unsigned int)(param_val->numeric_equivalent) ;
          }
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_FMF_DEVICE_SUB_TYPE_ID,1,
              &param_val,VAL_NUMERIC | VAL_SYMBOL,err_msg) ;
        if (!err)
        {
          if (!get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_DEVICE_SUB_TYPE_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
          {
            device_sub_type = (unsigned int)(param_val->value.ulong_value) ;
          }
          else
          {
            device_sub_type = (unsigned int)(param_val->numeric_equivalent) ;
          }
        }
        else
        {
         /*
          * Could not find "chip_id" or "unit_ip_address"
          * or "device_sub_type" or "device_type"
          * on line starting with "fmf debug add_device".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_033 ;
          goto exit ;
        }
        if ((device_type == SOC_SAND_DEVICE_TYPE_NONE) || ((device_type >= SOC_SAND_MAX_DEVICE_TYPE)))
        {
          /*
           * Illegal device type.
           */
          sal_sprintf(err_msg,
                "\r\n\n"
                "*** Illegal input device type (= %lu. Should be between %lu and %lu, not incl.)\r\n",
                (unsigned long)device_type,
                (unsigned long)SOC_SAND_DEVICE_TYPE_NONE,
                (unsigned long)SOC_SAND_MAX_DEVICE_TYPE
                ) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_034 ;
          goto exit ;
        }
        if (device_type == FAP_DEVICE_TYPE_EQUIVALENT)
        {
          if (device_sub_type != FAP10M_DEVICE_SUBTYPE_EQUIVALENT)
          {
            /*
             * Illegal device type.
             */
            sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Illegal input device sub type (= %lu. Should be %lu)\r\n",
                  (unsigned long)device_sub_type,
                  (unsigned long)FAP10M_DEVICE_SUBTYPE_EQUIVALENT
                  ) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_035 ;
            goto exit ;
          }
          device_sub_type = SOC_SAND_FAP10M_FAP_TYPE ;
        }
        else if ((device_type == FE2_DEVICE_TYPE_EQUIVALENT) ||
                              (device_type == FE13_DEVICE_TYPE_EQUIVALENT))
        {
          if (device_sub_type != FE200_DEVICE_SUBTYPE_EQUIVALENT)
          {
            /*
             * Illegal device type.
             */
            sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Illegal input device sub type (= %lu. Should be %lu)\r\n",
                  (unsigned long)device_sub_type,
                  (unsigned long)FE200_DEVICE_SUBTYPE_EQUIVALENT
                  ) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_036 ;
            goto exit ;
          }
          device_sub_type = SOC_SAND_FE200_FE_TYPE ;
        }
        soc_sand_proc_name = "fmf_api_add_device" ;
        error_id =
          fmf_api_add_device(
            unit_ip_addr,device_chip_id,device_type,
                    device_sub_type,&unit_identifier,&unitentifier) ;
        disp_fmf_result(error_id,soc_sand_proc_name) ;
        soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
        if (soc_sand_err != SOC_SAND_OK)
        {
          /*
           * fmf_api_add_device() returned with error.
           */
          send_string_to_screen(
            "\r\n\n"
            "*** Could not add device using fmf_api_add_device().\r\n",
            FALSE) ;
          ret = ERR_037 ;
          goto exit ;
        }
        sprint_ip(ip_address_string,unit_ip_addr, TRUE) ;
        sal_sprintf(err_msg,
                "\r\n"
                "Unit with IP address: %s, chip id: 0x%08lX, (type;sub_type): (%d;%d) \r\n"
                "  has been accessed.\r\n"
                "Device has been added with:\r\n"
                "  Unit identifier 0x%08lX\r\n\n",
                ip_address_string,(unsigned long)device_chip_id,
                (unsigned int)device_type,(unsigned int)device_sub_type,
                (unsigned long)unit_identifier
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_ACTIVATE_DEVICE_ID,1))
      {
        unsigned
          int
            device_type,
            device_sub_type,
             ;
        OPERATION_RANGE
          operation_range ;
        /*
         * Enter if this is a line starting with "fmf debug activate_device"
         */
        sal_sprintf(err_msg,
                "\r\n"
                "Going to activate device(s) within FMF...\r\n\n"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
        /*
         * Just to silence 'might be used uninitialized'
         */
        device_type = unit = device_sub_type = 0 ;
        if (!search_param_val_pairs(current_line,&match_index,PARAM_FMF_ONE_SPECIFIC_DEVICE_ID,1))
        {
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_DEVICE_ID_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          if (!err)
          {
            unit = (unsigned int)(param_val->value.ulong_value) ;
            device_type = SOC_SAND_FMF_DEV_TYPE_FROM_DEV_ID(unit) ;
          }
          else
          {
           /*
            * Could not find "unit"
            * on line starting with "fmf debug activate_device one_specific_device".
            * Probably SW error...
            */
            get_val_of_err = TRUE ;
            ret = ERR_038 ;
            goto exit ;
          }
          operation_range = SPECIFIED_ONE ;
        }
        else if (!search_param_val_pairs(
                      current_line,&match_index,PARAM_FMF_ALL_SPECIFIED_DEVICES_ID,1))
        {
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_DEVICE_TYPE_ID,1,
                &param_val,VAL_NUMERIC | VAL_SYMBOL,err_msg) ;
          if (!err)
          {
            if (!get_val_of(
                  current_line,(int *)&match_index,PARAM_FMF_DEVICE_TYPE_ID,1,
                  &param_val,VAL_NUMERIC,err_msg))
            {
              device_type = (unsigned int)(param_val->value.ulong_value) ;
            }
            else
            {
              device_type = (unsigned int)(param_val->numeric_equivalent) ;
            }
          }
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_FMF_DEVICE_SUB_TYPE_ID,1,
                &param_val,VAL_NUMERIC | VAL_SYMBOL,err_msg) ;
          if (!err)
          {
            if (!get_val_of(
                  current_line,(int *)&match_index,PARAM_FMF_DEVICE_SUB_TYPE_ID,1,
                  &param_val,VAL_NUMERIC,err_msg))
            {
              device_sub_type = (unsigned int)(param_val->value.ulong_value) ;
            }
            else
            {
              device_sub_type = (unsigned int)(param_val->numeric_equivalent) ;
            }
          }
          else
          {
           /*
            * Could not find "device_type" or "device_sub_type"
            * on line starting with "fmf debug activate_device all_specified_devices".
            * Probably SW error...
            */
            get_val_of_err = TRUE ;
            ret = ERR_041 ;
            goto exit ;
          }
          operation_range = FORCE_ALL ;
        }
        else
        {
         /*
          * Could not find "all_specified_devices" or "one_specific_device"
          * on line starting with "fmf debug activate_device".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_042 ;
          goto exit ;
        }
        if (operation_range == FORCE_ALL)
        {
          if (device_type == FAP_DEVICE_TYPE_EQUIVALENT)
          {
            if ((device_sub_type != FAP10M_DEVICE_SUBTYPE_EQUIVALENT) &&
                          (device_sub_type != ANY_DEVICE_SUBTYPE_EQUIVALENT))
            {
              /*
               * Illegal device type.
               */
              sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** Illegal input device sub type (= %lu. Should be %lu or %lu)\r\n",
                    (unsigned long)device_sub_type,
                    (unsigned long)FAP10M_DEVICE_SUBTYPE_EQUIVALENT,
                    (unsigned long)ANY_DEVICE_SUBTYPE_EQUIVALENT
                    ) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_044 ;
              goto exit ;
            }
            else if (device_sub_type == FAP10M_DEVICE_SUBTYPE_EQUIVALENT)
            {
              device_sub_type = SOC_SAND_FAP10M_FAP_TYPE ;
            }
            else if (device_sub_type != ANY_DEVICE_SUBTYPE_EQUIVALENT)
            {
              device_sub_type = SOC_SAND_FAP_TYPE_NONE ;
            }
          }
          else if ((device_type == FE2_DEVICE_TYPE_EQUIVALENT) ||
                                (device_type == FE13_DEVICE_TYPE_EQUIVALENT))
          {
            if ((device_sub_type != FE200_DEVICE_SUBTYPE_EQUIVALENT) &&
                          (device_sub_type != ANY_DEVICE_SUBTYPE_EQUIVALENT))
            {
              /*
               * Illegal device type.
               */
              sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** Illegal input device sub type (= %lu. Should be %lu or %lu)\r\n",
                    (unsigned long)device_sub_type,
                    (unsigned long)FE200_DEVICE_SUBTYPE_EQUIVALENT,
                    (unsigned long)ANY_DEVICE_SUBTYPE_EQUIVALENT
                    ) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_045 ;
              goto exit ;
            }
            else if (device_sub_type == FE200_DEVICE_SUBTYPE_EQUIVALENT)
            {
              device_sub_type = SOC_SAND_FE200_FE_TYPE ;
            }
            else if (device_sub_type != ANY_DEVICE_SUBTYPE_EQUIVALENT)
            {
              device_sub_type = SOC_SAND_FE_TYPE_NONE ;
            }
          }
        }
        else
        {
          if ((device_type >= SOC_SAND_MAX_DEVICE_TYPE) || (device_type == SOC_SAND_DEVICE_TYPE_NONE))
          {
            /*
             * Illegal device type.
             */
            sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Illegal input device type (= %lu. Should be less than %lu)\r\n",
                  (unsigned long)device_type,
                  (unsigned long)SOC_SAND_MAX_DEVICE_TYPE
                  ) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_043 ;
            goto exit ;
          }
        }
        soc_sand_proc_name = "fmf_api_activate_device" ;
        error_id =
          fmf_api_activate_device(
              operation_range,unit,device_type,device_sub_type) ;
        disp_fmf_result(error_id,soc_sand_proc_name) ;
        soc_sand_err = soc_sand_get_error_code_from_error_word(error_id) ;
        if (soc_sand_err != SOC_SAND_OK)
        {
          /*
           * fmf_api_activate_device() returned with error.
           */
          send_string_to_screen(
            "\r\n\n"
            "*** Could not activate device using fmf_api_activate_device().\r\n",
            FALSE) ;
          ret = ERR_046 ;
          goto exit ;
        }
        if (operation_range == SPECIFIED_ONE)
        {
          sal_sprintf(err_msg,
                "\r\n"
                "Device %lu has been activated\r\n\n",
                unit
                ) ;
          send_string_to_screen(err_msg,FALSE) ;
        }
      }
      else
      {
        /*
         * No parameter related to DEBUG has been found on the line.
         * Probably SW error.
         */
        send_string_to_screen(
          "\r\n\n"
          "*** No parameter related to \'debug\'  has been found on the line.\r\n",
          FALSE) ;
        ret = ERR_047 ;
        goto exit ;
      }
      goto exit ;
    }
    if (num_handled == 0)
    {
      /*
       * No GENERAL variable has been entered.
       * There must be at least one!
       */
      send_string_to_screen(
          "\r\n\n"
          "*** No parameter has been indicated for handling.\r\n"
          "    At least one must be entered.\r\n",
          TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
  }
exit:
  if (get_val_of_err)
  {
    /*
     * Value of some parameter could not be retrieved.
     */
    send_string_to_screen(
        "\r\n\n"
        "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
    send_string_to_screen(err_msg,TRUE) ;
  }
  return (ret) ;
}
/* } */
#else
/* { */
/* } */
#endif

