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
#include <appl/diag/dpp/utils_defi.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_ram_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_fe200.h>
/* } */
#else
/* { */
#include <stdio.h>
#include <inetLib.h>
/*
 * Definitions specific to reference system.
 */
#include <usrApp.h>
/*
 * User interface internal include file.
 */
#include <ui_defi.h>
#include <ui_pure_defi_fe200.h>
#include <appl/diag/dpp/utils_host_board.h>

/* } */
#endif

#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/fe200/fe200_api_callback.h>
#include <soc/dpp/fe200/fe200_api_general.h>
#include <Dune/ATM/SOC_SAND_FE200/fe200_atm_all_tests.h>
/*
 * FMC includes
 */
#include <FMC/fmc_common_agent.h>
#include <FMC/fmc_transport_layer.h>
#include <FMC/fmc_transport_layer_dcl.h>
#include <FMC/fe200_agent.h>
#include <FMC/fe200_agent_general.h>
#include <FMC/fe200_agent_callback.h>
#include <FMC/fe200_agent_cell.h>
#include <FMC/fe200_agent_links.h>
#include <FMC/fe200_agent_routing.h>
#include <FMC/fe200_agent_diagnostics.h>
#include <FMC/fe200_agent_statistics.h>
#include <FMC/fe200_agent_statistics.h>
#include <FMC/fe200_tests.h>

#include <ChipSim/chip_sim_task.h>

#include <dune/utilities/utils_fe200_card.h>

#include <appl/diag/dpp/dune_rpc.h>
#include <Sweep/fe200/sweep_fe200_ssr.h>

/*
 * Local prototypes
 * {
 */
unsigned long
  fe200_get_time_in_microseconds(
    void
  ) ;
int
  disp_fe_result(
    const unsigned long fe200_api_result,
    const char          *proc_name,
    const unsigned int  silent
  ) ;
/*
 * }
 */

unsigned long
  *soc_sand_get_chip_descriptor_base_addr(
    unsigned int index
  );
/*
 */
unsigned long
  fe200_get_time_in_microseconds(
    void
  )
{
  return get_watchdog_time(FALSE);
}
/*
 */
unsigned long read1[2], read2[2];
/*
 */
STATIC
  int test_scenario_1_read_callback(
    unsigned long callback_id,
    unsigned long *buf,
    unsigned long **new_buf,
    unsigned long size,
    unsigned long error,
    int unit,
    unsigned long offset,
    unsigned long transaction_id,
    unsigned long x
    )
{
  static unsigned short prev, curr = 0;
  static int num_of_times = 0;
  char str[160];
  unsigned long time_to_print = 0;
  if (buf == read1)
  {
    *new_buf = read2;
  }
  else
  {
    *new_buf = read1;
  }
  prev = curr;
  curr = read_timer_2();
  if(num_of_times > 0)
  {
    time_to_print += (curr - prev);
  }
  if(num_of_times++ > 100)
  {
    sal_sprintf(str, "\r\n----> SOC_SAND_FE200 callback been called 100 times, elapsed time is %lu", time_to_print);
    send_string_to_screen(str, TRUE);
    num_of_times = 0;
  }
  return 0;
}
/*
 * The full test callback tracing mecanism
 * {
 */
/*
 * }
 */
/*****************************************************
*NAME
*  disp_fe_result
*TYPE: PROC
*DATE: 18/FEB/2003
*FUNCTION:
*  Display return value and related information for
*  FE driver services.
*CALLING SEQUENCE:
*  disp_fe_result(fe200_api_result,current_line_ptr)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned long fe200_api_result -
*      Return value of any FE driver service.
*    char          *proc_name -
*      Name of procedure for which 'fe200_api_result'
*      was returned.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  None.
*SEE ALSO:
 */
int
  disp_fe_result(
    const unsigned long fe200_api_result,
    const char          *proc_name,
    const unsigned int  silent
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
    *err_name,
    *err_text,
    *soc_sand_proc_name,
    *soc_sand_module_name ;
  char
    err_msg[80*6] = "" ;
  ret = 0 ;
  err_name = "";
  err_id = fe200_get_err_text(fe200_api_result, &err_name, &err_text) ;
  /*
   * Let driver print whatever it wishes before going ahead here.
   */
  d_taskDelay(60) ;
  if (soc_sand_get_error_code_from_error_word(err_id) != SOC_SAND_OK)
  {
    err_text = "No error code description (or procedure id) found" ;
  }
  proc_id = soc_sand_get_proc_id_from_error_word(fe200_api_result) ;
  fe200_proc_id_to_string((unsigned long)proc_id,&soc_sand_module_name,&soc_sand_proc_name)  ;
  if (soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK)
  {
    sal_sprintf(err_msg,
        "\r\n"
        "---->---->\r\n"
        "\'%s\' -\r\n"
        "  returned with code 0x%lX (Error).\n\r"
        "  %s:%s\n\r",
        proc_name,fe200_api_result,
        err_name,
        err_text) ;
  }
  else
  {
    if (silent)
    {
      err_msg[0] = '\0';
    }
    else
    {
      sal_sprintf(err_msg,
        "\r\n"
        "---->---->\r\n"
        "\'%s\' -\r\n"
        "  returned with code 0x%lX (success).\n\r",
        proc_name,fe200_api_result) ;
    }
  }
  send_string_to_screen(err_msg,FALSE) ;
  return (ret) ;
}

unsigned int
  fe_print_unicast_table(
    int                    unit,
    DISTRIBUTION_ROUTING_UNI_REPORT *distribution_routing_uni_report
  )
{
  unsigned int
    ui;
  char
    err_msg[80*4] = "" ;


    sal_sprintf(err_msg, "\r\n--->get unicast table returned with:\r\n"
             "     reporting entity type    : fe%d\r\n"
             "     debug flags pending      : %c\r\n"
             "     reachability err pending : %c\r\n"
             "     link state bitmap        : 0x%08lX 0x%08lX\r\n"
             "     reporting %d entries starting at offset %d:\r\n",
    distribution_routing_uni_report->reporting_entity_type,
    (distribution_routing_uni_report->debug_flags_pending ? '+':'-'),
    (distribution_routing_uni_report->reachability_error_pending ? '+':'-'),
    distribution_routing_uni_report->link_state_up[0],
    distribution_routing_uni_report->link_state_up[1],
    distribution_routing_uni_report->num_entries,
    distribution_routing_uni_report->offset
    ) ;
  send_string_to_screen(err_msg, TRUE);
  if(!distribution_routing_uni_report->debug_flags_pending)
  {
    switch (distribution_routing_uni_report->reporting_entity_type)
    {
      case (SOC_SAND_FE1_ENTITY):
      {
        send_string_to_screen("chip id | distribution bitmap", TRUE);
        send_string_to_screen("--------+--------------------", TRUE);
        if(distribution_routing_uni_report->changes_only)
        { /* fe1 long report */
          for(ui = 0; ui < distribution_routing_uni_report->num_entries; ++ui)
          {
            sal_sprintf(err_msg, " 0x%04X |     0x%08lX",
              distribution_routing_uni_report->dist_route_entry[ui].fe1_dist_uni_long_entry.index_of_entry,
              distribution_routing_uni_report->dist_route_entry[ui].fe1_dist_uni_long_entry.distribution_entry
            );
            send_string_to_screen(err_msg, TRUE);
          }
        }
        else
        { /* fe1 short report */
          for(ui = 0; ui < distribution_routing_uni_report->num_entries; ++ui)
          {
            sal_sprintf(err_msg, " 0x%04X |     0x%08lX",
              distribution_routing_uni_report->offset + ui,
              distribution_routing_uni_report->dist_route_entry[ui].fe1_dist_uni_short_entry.distribution_entry
            );
            send_string_to_screen(err_msg, TRUE);
          }
        }
        break;
      }
      case (SOC_SAND_FE2_ENTITY):
      {
        unsigned
          long
            links_per_plane;
        /*
         * Get number of planes on target chip.
         */
        fe200_logical_read(
          unit,
          &links_per_plane,
          FE200_PLANES_FIELD
        ) ;
        links_per_plane = BIT(11 - links_per_plane) ;
        send_string_to_screen("plane #|chip id |is reachable|through link #", TRUE);
        send_string_to_screen("-------+--------+------------+--------------", TRUE);
        if(distribution_routing_uni_report->changes_only)
        { /* fe2 long report */
          for(ui = 0; ui < distribution_routing_uni_report->num_entries; ++ui)
          {
            sal_sprintf(err_msg, "   %01ld   | 0x%04lX |     %c      |     %02ld",
             (distribution_routing_uni_report->dist_route_entry[ui].fe2_route_uni_long_entry.index_of_entry) / links_per_plane,
             (distribution_routing_uni_report->dist_route_entry[ui].fe2_route_uni_long_entry.index_of_entry) % links_per_plane,
             (soc_sand_bitstream_test_bit(
            &(distribution_routing_uni_report->dist_route_entry[ui].fe2_route_uni_long_entry.routing_entry), 6) ? '+' : '-'),
             (distribution_routing_uni_report->dist_route_entry[ui].fe2_route_uni_long_entry.routing_entry) & 0x3F
            );
            send_string_to_screen(err_msg, TRUE);
          }
        }
        else
        { /* fe2 short report */
          for(ui = 0; ui < distribution_routing_uni_report->num_entries; ++ui)
          {
            sal_sprintf(err_msg, "   %01ld   | 0x%04lX |     %c      |     %02ld",
             (distribution_routing_uni_report->offset + ui) / links_per_plane,
             (distribution_routing_uni_report->offset + ui) % links_per_plane,
             (soc_sand_bitstream_test_bit(
            &(distribution_routing_uni_report->dist_route_entry[ui].fe2_route_uni_short_entry.routing_entry), 6) ? '+' : '-'),
             (distribution_routing_uni_report->dist_route_entry[ui].fe2_route_uni_short_entry.routing_entry) & 0x3F
            );
            send_string_to_screen(err_msg, TRUE);
          }
        }
        break;
      }
      case (SOC_SAND_FE3_ENTITY):
      {
        send_string_to_screen("link #|valid|chip id", TRUE);
        send_string_to_screen("------+-----+-------", TRUE);
        if(distribution_routing_uni_report->changes_only)
        { /* fe3 long report */
          for(ui = 0; ui < distribution_routing_uni_report->num_entries; ++ui)
          {
            sal_sprintf(err_msg, "  %02d  |  %c  | 0x%04lX",
              distribution_routing_uni_report->dist_route_entry[ui].fe3_route_uni_long_entry.index_of_entry,
             (soc_sand_bitstream_test_bit(
            &(distribution_routing_uni_report->dist_route_entry[ui].fe3_route_uni_long_entry.route_chip_id), 11) ? '+' : '-'),
              distribution_routing_uni_report->dist_route_entry[ui].fe3_route_uni_long_entry.route_chip_id & 0x7FF
            );
            send_string_to_screen(err_msg, TRUE);
          }
        }
        else
        { /* fe3 short report */
          for(ui = 0; ui < distribution_routing_uni_report->num_entries; ++ui)
          {
            sal_sprintf(err_msg, "  %02d  |  %c  | 0x%04lX",
              distribution_routing_uni_report->offset + ui,
             (soc_sand_bitstream_test_bit(
            &(distribution_routing_uni_report->dist_route_entry[ui].fe3_route_uni_short_entry.route_chip_id), 11) ? '+' : '-'),
              distribution_routing_uni_report->dist_route_entry[ui].fe3_route_uni_short_entry.route_chip_id & 0x7FF
            );
            send_string_to_screen(err_msg, TRUE);
          }
        }
        break;
      }
    } /* end of switch */
  } /* if !debug*/
  return 0;
}
/*****************************************************
*NAME
*  subject_fe200
*TYPE: PROC
*DATE: 29/DEC/2002
*FUNCTION:
*  Process input line which has an 'fe200' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_fe200(current_line,current_line_ptr)
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
  subject_fe200(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  int
    ret=0,
    err=0;
  PARAM_VAL
    *param_val ;
  char
    err_msg[80*4] = "" ;
  unsigned
    int
      ui;
  char
    *proc_name ;
  char
    *soc_sand_proc_name ;
  unsigned long
    fe200_api_result ;
  unsigned int
    polling_rate ;
  FE200_AGENT_OUT_STRUCT
    result;
  /* for remote operations */
  FMC_COMMON_OUT_STRUCT out_struct;
  FMC_COMMON_IN_STRUCT  in_struct;
  int
    irq_number = 0;
  polling_rate = 100 ;
  proc_name = "subject_fe200" ;
  ret = FALSE ;

  {
    SOC_SAND_DEVICE_TYPE chip_type;
    unsigned long
      chip_ver,
      dbg_ver;

    soc_sand_get_device_type(0, &chip_type, &chip_ver, &dbg_ver);
    if(chip_type != SOC_SAND_DEV_FE200)
    {
      sal_sprintf(
        err_msg,
        "\r\n\n"
        "*** \'%s command can't be committed over %s device\'.\r\n",
        proc_name,
        soc_sand_DEVICE_TYPE_to_str(chip_type)
      );
      send_string_to_screen(err_msg,TRUE);
      ret = TRUE;
      goto exit;

    }
  }
  /*
   * the rest ot the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'snmp').
     * This should be protected against by the calling environment.
     */
    send_string_to_screen(
      "\r\n\n"
      "*** There are no parameters on the line (just \'fe200\'). SW error.\r\n",TRUE)  ;
    ret = TRUE ;
    goto exit ;
  }

  /*
   * Else, there are parameters on the line (not just 'fe200').
   */
  if(fill_water_marks((unsigned char *)&in_struct, sizeof(FMC_COMMON_IN_STRUCT)))
  {
    send_string_to_screen(
      "\r\n\n"
      "*** SW error in fill_water_marks().\r\n",TRUE)  ;
    ret = TRUE ;
    goto exit ;
  }

  send_string_to_screen("", TRUE);

  {
    unsigned
      int
        match_index ;
    if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_START_DRIVER_ID,1))
    {
      /*
       * Enter if this is a 'fe200 start_driver' request.
       */
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "soc_sand_module_open";
      fe200_agent_sand_module_open(
        SOC_SAND_MAX_DEVICE, /* max devices */
        16, /* system_tick_in_ms */
        70, /* tcm task priority */
        1,  /* min_time_between_tcm_activation */
        FALSE, /* TCM mockup interrupts */
        &result
      );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SHUTDOWN_DRIVER_ID,1))
    {
      /*
       * Enter if this is a 'fe200 shutdown_driver' request.
       */
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "soc_sand_module_close";
      fe200_agent_sand_module_close(&result);
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_REGISTER_DEVICE_ID,1))
    {
      /*
       * Enter if this is a 'fe200 register device' request.
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_REGISTER_DEVICE_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 register_device\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_register_device";
      if((unsigned long)0x40000000 == param_val->value.ulong_value)
      {
        fe200_agent_register_device(param_val->value.ulong_value, reset_fe200_device, 2/*irq*/, &result);
      }
      else if((unsigned long)0x40100000 == param_val->value.ulong_value)
      {
        fe200_agent_register_device(param_val->value.ulong_value, reset_fe200_device, 3/*irq*/, &result);
      }
#if SIMULATE_CHIP_SIM
/* { */
      else if((unsigned long)FE_1_BASE_ADDR == param_val->value.ulong_value)
      {
        fe200_agent_register_device(param_val->value.ulong_value, reset_fe200_device, 2/*irq*/, &result);
      }
      else if((unsigned long)FE_2_BASE_ADDR == param_val->value.ulong_value)
      {
        fe200_agent_register_device(param_val->value.ulong_value, reset_fe200_device, 3/*irq*/, &result);
      }
/* } */
#endif
      else
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 register_device\' error \r\n"
                  "*** or \'base_adress can be only 0x4000000 or 0x4010000\'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "---->\r\n"
          "  fe200_register_device returned with device handle 0x%X\n\r",
                        result.data.register_device_data.device_handle);
        send_string_to_screen(err_msg,TRUE);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_UNREGISTER_DEVICE_ID,1))
    {
      /*
       * Enter if this is a 'fe200 unregister device' request.
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_UNREGISTER_DEVICE_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 unregister_device\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_unregister_device" ;

      if(card_fe200_is_valid_fe200(param_val->value.ulong_value))
      {
        irq_number = card_fe200_get_irq_number(param_val->value.ulong_value);
        fe200_agent_unregister_device(param_val->value.ulong_value, irq_number, &result);
      }
    else
    {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 unregister_device\' error \r\n"
                  "*** or \'unknown unit\'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
    }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GRACEFUL_SHUTDOWN_DEVICE_ID,1))
    {
    /*
       * Enter if this is a 'fe200 graceful shutdown device' request.
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GRACEFUL_SHUTDOWN_DEVICE_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      ) ;

      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 graceful_shutdown_device\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_graceful_shutdown_device";

      if( isDeviceIdFE(param_val->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_SHUTDOWN_DEVICE;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else /* local device id */
      {
        fe200_agent_graceful_shutdown_device(param_val->value.ulong_value, &result);
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GRACEFUL_RESTORE_DEVICE_ID,1))
    {
    /*
       * Enter if this is a 'fe200 graceful restore device' request.
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GRACEFUL_RESTORE_DEVICE_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      ) ;

      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 graceful_restore_device\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_graceful_restore_device";

      if( isDeviceIdFE(param_val->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_ACTIVATE_FE_AND_LINKS;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else /* local device id */
      {
        fe200_agent_graceful_restore_device(param_val->value.ulong_value, &result);
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_CLEAR_DEVICE_PENDING_SERVICES_ID,1))
    {
      /*
       * Enter if this is a 'fe200 clear device pending services' request.
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_CLEAR_DEVICE_PENDING_SERVICES_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 clear_device_pending_services\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_unregister_all_callback_functions";
      if( isDeviceIdFE(param_val->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_UNREGISTER_ALL_CALLBACK_FUNCTIONS;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else /* local device id */
      {
        fe200_agent_unregister_all_callback_functions(param_val->value.ulong_value, &result);
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_STOP_POLLING_TRANSACTON_ID,1))
    {
      /* Enter if this is a 'fe200 stop polling transaction' request. */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_STOP_POLLING_TRANSACTON_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 stop_polling_transaction\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_unregister_callback_function";
      result = fe200_agent_unregister_callback_function((unsigned long)param_val->value.ulong_value);
      fe200_api_result =  result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_STOP_ALL_POLLING_ID,1))
    {
      /* Enter if this is a 'fe200 stop all polling' request. */
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_stop_all_interrupt_and_polling_handling";
      fe200_agent_stop_all_interrupt_and_polling_handling(&result);
      fe200_api_result =  result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_START_ALL_POLLING_ID,1))
    {
      /* Enter if this is a 'fe200 start all polling' request. */
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_start_all_interrupt_and_polling_handling";
      fe200_agent_start_all_interrupt_and_polling_handling( &result);
      fe200_api_result =  result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_PHYSICAL_WRITE_ID,1))
    {
      /* Enter if this is a 'fe200 physical write' request. */
      PARAM_VAL *param_val_device, *param_val_offset, *param_val_size, *param_val_data;
      FE200_AGENT_GENERAL_BUFFER_STRUCT data_to_write;
      ret = 0;
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PHYSICAL_WRITE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PHYSICAL_OFFSET_ID, 1,
                        &param_val_offset, VAL_NUMERIC, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PHYSICAL_SIZE_ID, 1,
                        &param_val_size, VAL_NUMERIC, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PHYSICAL_DATA_ID, 1,
                        &param_val_data, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 physical write\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      send_array_to_screen("\r\n",2);
      data_to_write.size = param_val_size->value.ulong_value;
      memcpy(data_to_write.data, &(param_val_data->value.ulong_value), param_val_size->value.ulong_value);
      if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_PHYSICAL_DIRECT_ID,1))
      {
        soc_sand_proc_name = "fe200_mem_write (direct)" ;
        if( isDeviceIdFE(param_val_device->value.ulong_value) )
        {
          in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
          in_struct.common_union.fe200_struct.proc_id = FE200_MEM_WRITE;
          in_struct.common_union.fe200_struct.data.mem_write_data.unit = param_val_device->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_write_data.buffer_struct = data_to_write;
          in_struct.common_union.fe200_struct.data.mem_write_data.offset = param_val_offset->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_write_data.size = param_val_size->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_write_data.indirect = FALSE;
          if ( fmc_send_buffer_to_device_relative_id(
                  param_val_device->value.ulong_value,
                  sizeof(in_struct),
                  (unsigned char *)&in_struct,
                  sizeof(out_struct),
                  (unsigned char *)&out_struct
             ) )
          {
            sal_sprintf(err_msg,
                      "\r\n\n"
                      "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
            send_string_to_screen(err_msg,TRUE)  ;
            ret = TRUE  ;
            goto exit  ;
          }
          result = out_struct.common_union.fe200_struct;
        }
        else /* local device id */
        {
          fe200_agent_mem_write(
            param_val_device->value.ulong_value,
            data_to_write,
            param_val_offset->value.ulong_value,
            param_val_size->value.ulong_value,
            FALSE,/* direct */
            &result
          );
        }
        fe200_api_result = result.error_id;
        disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_PHYSICAL_INDIRECT_ID,1))
      {
        soc_sand_proc_name = "fe200_mem_write (indirect)";
        if( isDeviceIdFE(param_val_device->value.ulong_value) )
        {
          in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
          in_struct.common_union.fe200_struct.proc_id = FE200_MEM_WRITE;
          in_struct.common_union.fe200_struct.data.mem_write_data.unit = param_val_device->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_write_data.buffer_struct = data_to_write;
          in_struct.common_union.fe200_struct.data.mem_write_data.offset = param_val_offset->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_write_data.size = param_val_size->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_write_data.indirect = TRUE;
          if ( fmc_send_buffer_to_device_relative_id(
                  param_val_device->value.ulong_value,
                  sizeof(in_struct),
                  (unsigned char *)&in_struct,
                  sizeof(out_struct),
                  (unsigned char *)&out_struct
             ) )
          {
            sal_sprintf(err_msg,
                      "\r\n\n"
                      "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
            send_string_to_screen(err_msg,TRUE)  ;
            ret = TRUE  ;
            goto exit  ;
          }
          result = out_struct.common_union.fe200_struct;
        }
        else /* local device id */
        {
          fe200_agent_mem_write(
            param_val_device->value.ulong_value,
            data_to_write,
            param_val_offset->value.ulong_value,
            param_val_size->value.ulong_value,
            TRUE, /* indirect */
            &result
          );
        }
        fe200_api_result = result.error_id;
        disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 physical write\' error: \r\n"
                  "*** Unknown operation.\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_PHYSICAL_READ_ID,1))
    {
      /* Enter if this is a 'fe200 physical read' request. */
      PARAM_VAL *param_val_size, *param_val_device, *param_val_offset;
      unsigned long size, *data;
      ret = 0;
      sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 physical read\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PHYSICAL_READ_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PHYSICAL_OFFSET_ID, 1,
                        &param_val_offset, VAL_NUMERIC, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PHYSICAL_SIZE_ID, 1,
                        &param_val_size, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      size = param_val_size->value.ulong_value;
      send_array_to_screen("\r\n",2);
      if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_PHYSICAL_DIRECT_ID,1))
      {
        data = (unsigned long *)malloc(size);
        if (!data)
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 physical read\' error: \r\n"
                    "*** System does not have dynamic memory of size %lu.\r\n",
                    size) ;
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        soc_sand_proc_name = "fe200_mem_read (direct)" ;
        if( isDeviceIdFE(param_val_device->value.ulong_value) )
        {
          in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
          in_struct.common_union.fe200_struct.proc_id = FE200_MEM_READ;
          in_struct.common_union.fe200_struct.data.mem_read_data.unit = param_val_device->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_read_data.offset = param_val_offset->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_read_data.size = param_val_size->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_read_data.indirect = FALSE;
          if ( fmc_send_buffer_to_device_relative_id(
                  param_val_device->value.ulong_value,
                  sizeof(in_struct),
                  (unsigned char *)&in_struct,
                  sizeof(out_struct),
                  (unsigned char *)&out_struct
             ) )
          {
            sal_sprintf(err_msg,
                      "\r\n\n"
                      "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
            send_string_to_screen(err_msg,TRUE)  ;
            ret = TRUE  ;
            goto exit  ;
          }
          result = out_struct.common_union.fe200_struct;
        }
        else /* local device id */
        {
          fe200_agent_mem_read(
                                param_val_device->value.ulong_value,
                                param_val_offset->value.ulong_value,
                                size,
                                FALSE, /* direct */
                                &result
                              );
        }
        fe200_api_result = result.error_id;
        memcpy(data, result.data.mem_read_data.data, size);
        disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
        err = (soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK);
        if (!err)
        {
          sal_sprintf(err_msg, "----> fe200_mem_read -\r\n"
                           "      The data that was read: ");
          send_string_to_screen(err_msg, FALSE);
          display_memory((char *)data,size / sizeof(long),LONG_EQUIVALENT,HEX_EQUIVALENT,FALSE,FALSE,0);
        }
        free(data);
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_PHYSICAL_INDIRECT_ID,1))
      {
        data = (unsigned long *)malloc(size) ;
        if (!data)
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 physical read\' error: \r\n"
                    "*** System does not have dynamic memory of size %lu.\r\n",
                    size) ;
          send_string_to_screen(err_msg,TRUE);
          ret = TRUE;
          goto exit;
        }
        soc_sand_proc_name = "fe200_mem_read (indirect)";
        if( isDeviceIdFE(param_val_device->value.ulong_value) )
        {
          in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
          in_struct.common_union.fe200_struct.proc_id = FE200_MEM_READ;
          in_struct.common_union.fe200_struct.data.mem_read_data.unit = param_val_device->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_read_data.offset = param_val_offset->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_read_data.size = param_val_size->value.ulong_value;
          in_struct.common_union.fe200_struct.data.mem_read_data.indirect = TRUE;
          if ( fmc_send_buffer_to_device_relative_id(
                  param_val_device->value.ulong_value,
                  sizeof(in_struct),
                  (unsigned char *)&in_struct,
                  sizeof(out_struct),
                  (unsigned char *)&out_struct
             ) )
          {
            sal_sprintf(err_msg,
                      "\r\n\n"
                      "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
            send_string_to_screen(err_msg,TRUE)  ;
            ret = TRUE  ;
            goto exit  ;
          }
          result = out_struct.common_union.fe200_struct;
        }
        fe200_agent_mem_read(
          param_val_device->value.ulong_value,
          param_val_offset->value.ulong_value,
          size,
          TRUE, /* indirect */
          &result
        );
        fe200_api_result = result.error_id;
        memcpy(data, result.data.mem_read_data.data, size);
        disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
        err = (soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK);
        if (!err)
        {
          sal_sprintf(err_msg, "----> fe200_mem_read (indirect) -\r\n"
                           "      The data that was read is: ") ;
          send_string_to_screen(err_msg, FALSE) ;
          display_memory((char *)data,size / sizeof(long),LONG_EQUIVALENT,HEX_EQUIVALENT,FALSE,FALSE,0) ;
        }
        free(data);
      }
      else
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 physical read\' error: \r\n"
                  "*** Unknown operation.\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_DEFERRED_PHYSICAL_READ_ID,1))
    {
      /* Enter if this is a 'fe200 deferred physical read' request. */
      PARAM_VAL
        *param_val_device,
        *param_val_offset,
        *param_val_size,
        *param_val_polling_rate;

      ret = 0;
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_DEFERRED_PHYSICAL_READ_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PHYSICAL_OFFSET_ID, 1,
                        &param_val_offset, VAL_NUMERIC, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PHYSICAL_SIZE_ID, 1,
                        &param_val_size, VAL_NUMERIC, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_POLLING_RATE_ID, 1,
                        &param_val_polling_rate, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 deferred physical read\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      send_array_to_screen("\r\n",2);
      if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_PHYSICAL_DIRECT_ID,1))
      {
        result = fe200_agent_register_callback_function(
                    param_val_device->value.ulong_value,
                    FE200_MEM_READ, FALSE, /*poll*/
                    param_val_polling_rate->value.ulong_value,
                    0x12, /* to act as user callback id */
                    param_val_offset->value.ulong_value,
                    param_val_size->value.ulong_value,
                    FALSE, /* direct */
                    0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF /* param4,5,6 */
                 );
        fe200_api_result = result.error_id;
        soc_sand_proc_name = "fe200_agent_register_callback_function (direct read)" ;
        disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
        err = (soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK);
        if (!err)
        {
          sal_sprintf(err_msg, "---->\r\n"
            "  fe200_agent_register_callback_function returned with deferred handle 0x%X\n\r",
                          result.data.register_callback_data.callback_id ) ;
          send_string_to_screen(err_msg,TRUE)  ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_PHYSICAL_INDIRECT_ID,1))
      {
        result = fe200_agent_register_callback_function(
                    param_val_device->value.ulong_value,
                    FE200_MEM_READ, FALSE, /*poll*/
                    param_val_polling_rate->value.ulong_value,
                    0x13, /* to act as user callback id */
                    param_val_offset->value.ulong_value,
                    param_val_size->value.ulong_value,
                    TRUE, /* indirect */
                    0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF /* param4,5,6 */
                 );
        fe200_api_result = result.error_id;
        soc_sand_proc_name = "fe200_agent_register_callback_function (indirect read)" ;
        disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
        err = (soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK);
        if (!err)
        {
          sal_sprintf(err_msg, "---->\r\n"
            "  fe200_agent_register_callback_function returned with deferred handle 0x%X\n\r",
                          result.data.register_callback_data.callback_id ) ;
          send_string_to_screen(err_msg,TRUE)  ;
        }
      }
      else
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 deferred read\' error: \r\n"
                  "*** Unknown operation.\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_LOGICAL_WRITE_ID,1))
    {
      /* Enter if this is a 'fe200 logical write' request. */
      PARAM_VAL *param_val_device, *param_val_field, *param_val_data;
      ret = 0;
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_LOGICAL_WRITE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_WRITE_FIELD_ID, 1,
                        &param_val_field, VAL_SYMBOL, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PHYSICAL_DATA_ID, 1,
                        &param_val_data, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 logical write\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_logical_write";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_LOGICAL_WRITE;
        in_struct.common_union.fe200_struct.data.logical_write_data.unit = param_val_device->value.ulong_value;
        in_struct.common_union.fe200_struct.data.logical_write_data.field_id = param_val_field->numeric_equivalent;
        in_struct.common_union.fe200_struct.data.logical_write_data.value = param_val_data->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        fe200_agent_logical_write(
          param_val_device->value.ulong_value,
          param_val_data->value.ulong_value,
          param_val_field->numeric_equivalent,
          &result
        );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_LOGICAL_READ_ID,1))
    {
      /* Enter if this is a 'fe200 logical read' request. */
      PARAM_VAL *param_val_device, *param_val_field;
      ret = 0 ;
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_LOGICAL_READ_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_READ_FIELD_ID, 1,
                        &param_val_field, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 logical read\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_logical_read";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_LOGICAL_READ;
        in_struct.common_union.fe200_struct.data.logical_read_data.unit = param_val_device->value.ulong_value;
        in_struct.common_union.fe200_struct.data.logical_read_data.field_id = param_val_field->numeric_equivalent;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        fe200_agent_logical_read(
          param_val_device->value.ulong_value,
          param_val_field->numeric_equivalent,
          &result
        );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "\r\n"
          "----> fe200_logical_read returned with data: 0x%lX\r\n",result.data.logical_read_data.data);
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_DEFERRED_LOGICAL_READ_ID,1))
    {
      /* Enter if this is a 'fe200 deferred logical read' request. */
      PARAM_VAL *param_val_device, *param_val_field, *param_val_polling_rate;
      ret = 0;
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_DEFERRED_LOGICAL_READ_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_READ_FIELD_ID, 1,
                        &param_val_field, VAL_SYMBOL, err_msg
                      ) ;
      ret |= get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_POLLING_RATE_ID, 1,
                        &param_val_polling_rate, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 deferred logical read\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE  ;
        goto exit  ;
      }
      send_array_to_screen("\r\n",2);
      result = fe200_agent_register_callback_function(
                  param_val_device->value.ulong_value,
                  FE200_LOGICAL_READ, FALSE /*poll*/,
                  param_val_polling_rate->value.ulong_value,
                  105 /* callback id*/,
                  param_val_field->numeric_equivalent, /* param1 */
                  0x1,0x2,0x3,0x4,0x5 /* param2-6*/
               );
      soc_sand_proc_name = "fe200_register_callback_function (deferred logical read)" ;
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg,
          "----> fe200_register_callback_function returned with handle 0x%X\r\n",
          result.data.register_callback_data.callback_id);
        send_string_to_screen(err_msg, TRUE);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_PROFILE_ID,1))
    {
      /* Enter if this is a 'fe200 get_profile' request. */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_PROFILE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 get profile\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /**/
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_get_profile";
      fe200_agent_get_profile(
          param_val_device->value.ulong_value,
          &result
      );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "\r\n--->get profile returned with: %s",
                 (result.data.get_profile_data.device_profile) ? "UNKNOWN PROFILE" : " STANDARD PROFILE"
               );
        send_string_to_screen(err_msg, TRUE);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SET_PROFILE_ID,1))
    {
      /*
       * Enter if this is a 'fe200 set_profile' request.
       */
      PARAM_VAL
        *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_PROFILE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set profile\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /**/
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_set_profile";
      fe200_agent_set_profile(
          param_val_device->value.ulong_value, 0, /*  FE200_STANDARD_PROFILE */
          &result
      );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "\r\n--->set profile returned with: Success");
        send_string_to_screen(err_msg, TRUE);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SEND_CELL_ID,1))
    {
      /*
       * Enter if this is a 'fe200 send cell' request.
       */
      SOC_SAND_TX_SR_DATA_CELL tx_cell;
      PARAM_VAL *param_val_device, *param_val_general_purpose;
      /*
       * load default values, for values that might not appear, but must be passed to the api
       */
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'fe200 send cell\' error \r\n"
              "*** or \'unknown parameter \'.\r\n"
              "    Probably SW error\r\n"
      );
      tx_cell.destination_entity_type = SOC_SAND_FE2_ENTITY;
      tx_cell.source_entity_type      = SOC_SAND_DONT_CARE_ENTITY;
      for( ui = 0 ; ui < 8 ; ++ui)
      {
        tx_cell.body_tx[ui] = 0x12345678;
      }
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SEND_CELL_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SEND_CELL_INIT_OUT_LINK_ID, 1,
                        &param_val_general_purpose, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      tx_cell.initial_output_link = param_val_general_purpose->value.ulong_value;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SEND_CELL_FE2_LINK_ID, 1,
                        &param_val_general_purpose, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      tx_cell.fe2_link = param_val_general_purpose->value.ulong_value ;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SEND_CELL_FE3_LINK_ID, 1,
                        &param_val_general_purpose, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      tx_cell.fe3_link = param_val_general_purpose->value.ulong_value ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SEND_CELL_DEST_ENTITY_TYPE_ID,1))
      {
        ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_SEND_CELL_DEST_ENTITY_TYPE_ID, 1,
                  &param_val_general_purpose, VAL_SYMBOL, err_msg
                ) ;
        if (0 != ret)
        {
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        tx_cell.destination_entity_type = param_val_general_purpose->numeric_equivalent ;
      }
      if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SEND_CELL_SRC_ENTITY_TYPE_ID,1))
      {
        ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_SEND_CELL_SRC_ENTITY_TYPE_ID, 1,
                  &param_val_general_purpose, VAL_SYMBOL, err_msg
                ) ;
        if (0 != ret)
        {
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        tx_cell.source_entity_type = param_val_general_purpose->numeric_equivalent ;
      }
      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SEND_CELL_SRC_CHIP_ID, 1,
                        &param_val_general_purpose, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      tx_cell.source_chip_id = param_val_general_purpose->value.ulong_value ;
      /*
       */
      for(ui = 0 ; ui < 8 ; ++ui)
      {
        if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SEND_CELL_BODY_ID,ui+1))
        {
          ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_SEND_CELL_BODY_ID, ui+1,
                  &param_val_general_purpose, VAL_NUMERIC, err_msg
                );
          if (0 != ret)
          {
            send_string_to_screen(err_msg,TRUE);
            ret = TRUE;
            goto exit;
          }
          tx_cell.body_tx[ui] = param_val_general_purpose->value.ulong_value;
        }
      }
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_send_sr_data_cell";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_SEND_SR_DATA_CELL;
        in_struct.common_union.fe200_struct.data.send_cell_data.unit = param_val_device->value.ulong_value;
        in_struct.common_union.fe200_struct.data.send_cell_data.tx_cell = tx_cell;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_send_sr_data_cell(
                    param_val_device->value.ulong_value, tx_cell
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_RECV_CELL_ID,1))
    {
      /* Enter if this is a 'fe200 recv cell' request. */
      SOC_SAND_RX_SR_DATA_CELL   rx_cell;
      PARAM_VAL *param_val_device,  *param_val_links_to_check;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_RECV_CELL_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_RECV_CELL_LINKS_ID, 1,
                        &param_val_links_to_check, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_RECV_SR_DATA_CELL;
        in_struct.common_union.fe200_struct.data.recv_cell_data.unit = param_val_device->value.ulong_value;
        in_struct.common_union.fe200_struct.data.recv_cell_data.links_to_check = param_val_links_to_check->numeric_equivalent;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_recv_sr_data_cell(
                    param_val_device->value.ulong_value, param_val_links_to_check->numeric_equivalent
                 );
      }
      rx_cell = result.data.recv_cell_data.rx_info.rx_cell;
      soc_sand_proc_name = "fe200_recv_sr_data_cell";
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      if (FE200_RECV_SR_DATA_CELL_002 == soc_sand_get_error_code_from_error_word(fe200_api_result))
      {
        sal_sprintf(err_msg,
          "----> fe200_agent_recv_sr_data_cell returned with no cell (data sr fifo is empty)\r\n"
        );
      }
      else
      {
        switch (result.data.recv_cell_data.rx_info.rx_cell.source_entity_type)
        {
          case SOC_SAND_FAP_ENTITY:
          {
            sal_sprintf(err_msg,
              "----> fe200_agent_recv_sr_data_cell returned with\r\n"
              "       src entity type : fap\r\n"
            );
            break;
          }
          case SOC_SAND_FE1_ENTITY:
          {
            sal_sprintf(err_msg,
              "----> fe200_agent_recv_sr_data_cell returned with\r\n"
              "       src entity type : fe1\r\n"
            );
            break;
          }
          case SOC_SAND_FE2_ENTITY:
          {
            sal_sprintf(err_msg,
              "----> fe200_agent_recv_sr_data_cell returned with\r\n"
              "       src entity type : fe2\r\n"
            );
            break;
          }
          case SOC_SAND_FE3_ENTITY:
          {
            sal_sprintf(err_msg,
              "----> fe200_agent_recv_sr_data_cell returned with\r\n"
              "       src entity type : fe3\r\n"
            );
            break;
          }
          default:
          {
            sal_sprintf(err_msg,
              "----> fe200_agent_recv_sr_data_cell returned with\r\n"
              "       src entity type : unknown\r\n"
            );
            break;
          }
        };
        send_string_to_screen(err_msg, FALSE);
        sal_sprintf(err_msg,
                "       src chip id     : 0x%X\r\n"
                "       body            : 0x%lX 0x%lX 0x%lX 0x%lX\r\n"
                "                         0x%lX 0x%lX 0x%lX 0x%lX\r\n",
                result.data.recv_cell_data.rx_info.rx_cell.source_chip_id,
                result.data.recv_cell_data.rx_info.rx_cell.body_tx[0],
                result.data.recv_cell_data.rx_info.rx_cell.body_tx[1],
                result.data.recv_cell_data.rx_info.rx_cell.body_tx[2],
                result.data.recv_cell_data.rx_info.rx_cell.body_tx[3],
                result.data.recv_cell_data.rx_info.rx_cell.body_tx[4],
                result.data.recv_cell_data.rx_info.rx_cell.body_tx[5],
                result.data.recv_cell_data.rx_info.rx_cell.body_tx[6],
                result.data.recv_cell_data.rx_info.rx_cell.body_tx[7]
        );
      }
      send_string_to_screen(err_msg, TRUE);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_DEFRRED_RECV_CELL_ID,1))
    {
      /*
       * Enter if this is a 'fe200 deferred recv cell' request.
       */
      unsigned int int_not_poll;
      PARAM_VAL *param_val_device,  *param_val_links_to_check, *param_val_polling_rate;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_DEFRRED_RECV_CELL_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_RECV_CELL_LINKS_ID, 1,
                        &param_val_links_to_check, VAL_SYMBOL, err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      if (!search_param_val_pairs(current_line,&match_index,PARAM_DEFERRED_FE200_RECV_CELL_POLL_MODE_ID,1))
      {
        int_not_poll = FALSE;
        ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_POLLING_RATE_ID, 1,
                  &param_val_polling_rate, VAL_NUMERIC, err_msg
                ) ;
        if (0 != ret)
        {
          send_string_to_screen(err_msg,TRUE);
          ret = TRUE;
          goto exit;
        }
        result = fe200_agent_register_callback_function(
              param_val_device->value.ulong_value,
              FE200_RECV_SR_DATA_CELL, int_not_poll, /*poll*/
              param_val_polling_rate->value.ulong_value,
              0x15, /* to act as user callback id */
              param_val_links_to_check->numeric_equivalent,
              0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF /* param2,3,4,5,6 */
             );
      }
      else if(!search_param_val_pairs(current_line,&match_index,PARAM_DEFERRED_FE200_RECV_CELL_INT_MODE_ID,1))
      {
        int_not_poll = TRUE;
        result = fe200_agent_register_callback_function(
              param_val_device->value.ulong_value,
              FE200_RECV_SR_DATA_CELL, int_not_poll, /*interrupt*/
              0, 0x16, /* to act as user callback id */
              param_val_links_to_check->numeric_equivalent,
              0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF /* param2,3,4,5,6 */
             );
      }
      fe200_api_result = result.error_id;
      soc_sand_proc_name = "fe200_agent_register_callback_function (recv cell)" ;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "---->\r\n"
          "  fe200_agent_register_callback_function returned with deferred handle 0x%X\n\r",
                result.data.register_callback_data.callback_id ) ;
        send_string_to_screen(err_msg,TRUE)  ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_RESET_SINGLE_SERDES_ID,1))
    {
      /* Enter if this is a 'fe200 reset_single_serdes' request. */
      PARAM_VAL *param_val_device, *param_val_remain_reset, *param_val_link;
      unsigned int remain_reset;
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_RESET_SINGLE_SERDES_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 reset single serdes\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /* get link number */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_SERDES_PARAMS_SERDES_NUMBER_ID, 1,
                        &param_val_link, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 reset single serdes\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /* get remain */
      remain_reset = TRUE;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_RESET_REMAIN_ACTIVE_ID,1))
      {
        ret = get_val_of( current_line, (int *)&match_index,
                          PARAM_FE200_RESET_REMAIN_ACTIVE_ID, 1,
                          &param_val_remain_reset, VAL_SYMBOL, err_msg
                        );
        if (0 != ret)
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 reset single serdes\' error \r\n"
                    "*** or \'unknown parameter \'.\r\n"
                    "    Probably SW error\r\n");
          send_string_to_screen(err_msg,TRUE);
          ret = TRUE;
          goto exit;
        }
        remain_reset = param_val_remain_reset->numeric_equivalent;
      }
      /**/
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_reset_single_serdes";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_RESET_SINGLE_SERDES;
        in_struct.common_union.fe200_struct.data.reset_serdes_data.unit = param_val_device->value.ulong_value;
        in_struct.common_union.fe200_struct.data.reset_serdes_data.serdes_number = param_val_link->value.ulong_value;
        in_struct.common_union.fe200_struct.data.reset_serdes_data.remain_active = remain_reset;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_reset_single_serdes(
                   param_val_device->value.ulong_value,
                   param_val_link->value.ulong_value,
                   remain_reset
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SET_SERDES_PARAMS_ID,1))
    {
      /* Enter if this is a 'fe200 set_serdes_params' request. */
      PARAM_VAL *param_val_device, *param_val_general;
      FE200_AGENT_SERDES_STATUS_STRUCT
      serdes_params;
      int klj;
      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_SERDES_PARAMS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set serdes params\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      for (klj = 0; klj<FE200_NUM_OF_SERDESES; ++klj)
      {
        serdes_params.all_serdes_status.single_serdes_status[klj].valid = FALSE;
      }
      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_SERDES_PARAMS_SERDES_NUMBER_ID, 1,
                        &param_val_general, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set serdes params\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      klj = (int)param_val_general->value.ulong_value;
      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_SERDES_PARAMS_AMPLITUDE_ID, 1,
                        &param_val_general, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set serdes params\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      serdes_params.all_serdes_status.single_serdes_status[klj].amplitude =
                   (unsigned int)param_val_general->value.ulong_value;
      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_SERDES_PARAMS_COMMON_MODE_ID, 1,
                        &param_val_general, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set serdes params\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      serdes_params.all_serdes_status.single_serdes_status[klj].common_mode =
                   (unsigned int)param_val_general->value.ulong_value;
      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_SERDES_PARAMS_MAIN_CONTROL_ID, 1,
                        &param_val_general, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set serdes params\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      serdes_params.all_serdes_status.single_serdes_status[klj].main_control =
                   (unsigned int)param_val_general->value.ulong_value;
      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_SERDES_PARAMS_POWER_DOWN_RX_TX_ID, 1,
                        &param_val_general, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set serdes params\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      serdes_params.all_serdes_status.single_serdes_status[klj].power_down_rx_tx =
                   (unsigned int)param_val_general->value.ulong_value;
      /*
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_SERDES_PARAMS_PRE_EMPHASIS_ID, 1,
                        &param_val_general, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set serdes params\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      serdes_params.all_serdes_status.single_serdes_status[klj].pre_emphasis =
                   (unsigned int)param_val_general->value.ulong_value;
      /*
       */
      serdes_params.all_serdes_status.single_serdes_status[klj].valid = TRUE;
      /*
       */
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_set_serdes_params";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_SET_SERDES_PARAMS;
        in_struct.common_union.fe200_struct.data.set_serdes_params_data.unit = param_val_device->value.ulong_value;
        in_struct.common_union.fe200_struct.data.set_serdes_params_data.serdes_status = serdes_params;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_set_serdes_params(
                   param_val_device->value.ulong_value,
                   serdes_params
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_LINKS_GET_TPVL,1))
    {
      /*
       * Enter if this is a 'fe200 links get_tpvl' request.
       */
      PARAM_VAL
        *param_val_device;
      unsigned long
        tpvl_value;

      soc_sand_proc_name = "fe200_serdes_tpvl_get";
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_LINKS_GET_TPVL, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 links get_tpvl\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /*
       */
      fe200_api_result = fe200_logical_read( param_val_device->value.ulong_value, &tpvl_value, FE200_SERDES_TPVL_FIELD);
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        d_printf("tpvl=%lu\n\r", tpvl_value) ;
      }

    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_LINKS_SET_TPVL,1))
    {
      /*
       * Enter if this is a 'fe200 links set_tpvl' request.
       */
      PARAM_VAL
        *param_val_device;
      unsigned long
        tpvl_value;

      soc_sand_proc_name = "fe200_logical_write";
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_LINKS_SET_TPVL, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 links set_tpvl\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }

      /* TPVL value */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_TPVL_VALUE_ID, 1,
                        &param_val, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 links set_tpvl\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      tpvl_value = param_val->value.ulong_value;

      /*
       */
      fe200_api_result = fe200_logical_write(param_val_device->value.ulong_value, &tpvl_value, FE200_SERDES_TPVL_FIELD);
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;

    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_SERDES_PARAMS_ID,1))
    {
      /*
       * Enter if this is a 'fe200 get_serdes_params' request.
       */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_SERDES_PARAMS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 get serdes params\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /*
       */
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_get_serdes_params";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_GET_SERDES_PARAMS;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_get_serdes_params( param_val_device->value.ulong_value );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        /*
         * lets print the report to the screen
         */
        send_string_to_screen("\r\n #|preEmphsis|amplitutde|commonMode|main ctrl | pd_rxtx  ", TRUE) ;
        send_string_to_screen("--+----------|----------+----------+----------+----------", TRUE) ;
        for(ui = 0 ; ui < 64 ; ++ui)
        {
          if(result.data.serdes_status_data.all_serdes_status.single_serdes_status[ui].valid)
          {
            sal_sprintf(err_msg, "%2d|   0x%X    |   0x%X    |   0x%X    |   0x%X    |   0x%X ", ui,
                    result.data.serdes_status_data.all_serdes_status.single_serdes_status[ui].pre_emphasis,
                    result.data.serdes_status_data.all_serdes_status.single_serdes_status[ui].amplitude,
                    result.data.serdes_status_data.all_serdes_status.single_serdes_status[ui].common_mode,
                    result.data.serdes_status_data.all_serdes_status.single_serdes_status[ui].main_control,
                    result.data.serdes_status_data.all_serdes_status.single_serdes_status[ui].power_down_rx_tx
                    ) ;
            send_string_to_screen(err_msg, TRUE) ;
          }
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_CONNECTIVITY_MAP_ID,1))
    {
      /*
       * Enter if this is a 'fe200 get_connectivity_map' request.
       */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_CONNECTIVITY_MAP_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'fe200 get connectivity map\' error \r\n"
                "*** or \'unknown parameter \'.\r\n"
                "    Probably SW error\r\n"
        );
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /*
       */
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_get_connectivity_map";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_GET_CONNECTIVITY_MAP;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_get_connectivity_map(
                    param_val_device->value.ulong_value
                 );
      }
      fe200_api_result = result.error_id;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        /*
         * lets print the report to the screen
         */
        fe200_print_ALL_LINK_CONNECTIVITY(
          &result.data.links_connectivity_data.all_link_connectivity,
          ui_printing_policy_get()
        );
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_DEFERRED_GET_CONNECTIVITY_MAP_ID,1))
    {
      /*
       * Enter if this is a 'fe200 deferred get connectivity map' request.
       */
      PARAM_VAL *param_val_device,  *param_val_polling_rate;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_DEFERRED_GET_CONNECTIVITY_MAP_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      ret = get_val_of( current_line, (int *)&match_index,
              PARAM_FE200_POLLING_RATE_ID, 1,
              &param_val_polling_rate, VAL_NUMERIC, err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      result = fe200_agent_register_callback_function(
            param_val_device->value.ulong_value,
            FE200_GET_CONNECTIVITY_MAP, FALSE, /*poll*/
            param_val_polling_rate->value.ulong_value,
            0x20, /* to act as user callback id */
            0xDEADBEAF,0xDEADBEAF, 0xDEADBEAF, /* param 1, 2, 3 */
            0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF /* param 4, 5, 6 */
           );
      fe200_api_result = result.error_id;
      soc_sand_proc_name = "fe200_agent_register_callback_function (get_connectivity_map)" ;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "---->\r\n"
          "  fe200_agent_register_callback_function returned with deferred handle 0x%X\n\r",
                result.data.register_callback_data.callback_id ) ;
        send_string_to_screen(err_msg,TRUE)  ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_LINKS_STATUS_ID,1))
    {
      /*
       * Enter if this is a 'fe200 get_links_status' request.
       */
      PARAM_VAL *param_val_device, *param_val_general_purpose;
      unsigned long list_of_links[2], tmp;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_LINKS_STATUS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 get links status\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_get_links_status";
      list_of_links[0] = 0xFFFFFFFF;
      list_of_links[1] = 0xFFFFFFFF;
      for(ui = 0 ; ui < 2 ; ++ui)
      {
        if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_LIST_OF_LINKS_ID,ui+1))
        {
          ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_LIST_OF_LINKS_ID, ui+1,
                  &param_val_general_purpose, VAL_NUMERIC, err_msg
                );
          if (0 != ret)
          {
            send_string_to_screen(err_msg,TRUE)  ;
            ret = TRUE  ;
            goto exit  ;
          }
          list_of_links[ui] = param_val_general_purpose->value.ulong_value;
        }
      }

      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_GET_LINKS_STATUS;
        in_struct.common_union.fe200_struct.data.link_status_data.unit = param_val_device->value.ulong_value;
        in_struct.common_union.fe200_struct.data.link_status_data.list_of_links_0 = list_of_links[0];
        in_struct.common_union.fe200_struct.data.link_status_data.list_of_links_1 = list_of_links[1];
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_get_links_status(
                    param_val_device->value.ulong_value,
                    list_of_links[0], list_of_links[1]
                 );
      }
      /*
       * in order to use link_of_list with bitstream i have to switch them around
       * (cause unlike the api 0 is the lsb and 1 is the msb)
       */
      tmp = list_of_links[0];
      list_of_links[0] = list_of_links[1];
      list_of_links[1] = tmp;
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        /*
         * lets print the report to the screen
         */
        if (result.data.links_status_data.all_link_status.error_in_some)
        {
          send_string_to_screen("\r\n--->get links status found an error on at least one link:\r\n ", TRUE) ;
        }
        else
        {
          send_string_to_screen("\r\n--->get links status found no errors:\r\n ", TRUE) ;
        }

        fe200_print_ALL_LINK_STATUS(
          &result.data.links_status_data.all_link_status,
          ui_printing_policy_get()
        );

      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_DEFERRED_GET_LINKS_STATUS_ID,1))
    {
      /*
       * Enter if this is a 'fe200 deferred get links status' request.
       */
      PARAM_VAL *param_val_device,  *param_val_polling_rate, *param_val_general_purpose;
      unsigned long list_of_links[2];
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_DEFERRED_GET_LINKS_STATUS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      ret = get_val_of( current_line, (int *)&match_index,
              PARAM_FE200_POLLING_RATE_ID, 1,
              &param_val_polling_rate, VAL_NUMERIC, err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      list_of_links[0] = 0xFFFFFFFF;
      list_of_links[1] = 0xFFFFFFFF;
      for(ui = 0 ; ui < 2 ; ++ui)
      {
        if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_LIST_OF_LINKS_ID,ui+1))
        {
          ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_LIST_OF_LINKS_ID, ui+1,
                  &param_val_general_purpose, VAL_NUMERIC, err_msg
                );
          if (0 != ret)
          {
            send_string_to_screen(err_msg,TRUE);
            ret = TRUE;
            goto exit;
          }
          list_of_links[ui] = param_val_general_purpose->value.ulong_value;
        }
      }
      result = fe200_agent_register_callback_function(
            param_val_device->value.ulong_value,
            FE200_GET_LINKS_STATUS, FALSE, /*poll*/
            param_val_polling_rate->value.ulong_value,
            0x25, /* to act as user callback id */
            list_of_links[0], list_of_links[1],
            0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF /* param 3,4,5,6 */
           );
      fe200_api_result = result.error_id;
      soc_sand_proc_name = "fe200_agent_register_callback_function (get_links_status)" ;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "---->\r\n"
          "  fe200_agent_register_callback_function returned with deferred handle 0x%X\n\r",
                result.data.register_callback_data.callback_id );
        send_string_to_screen(err_msg,TRUE);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_STOP_REACHABILITY_ERRORS_ID,1))
    {
      /*
       * Enter if this is a 'fe200 stop_reachability_errors_collection' request.
       */
      PARAM_VAL
        *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_STOP_REACHABILITY_ERRORS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 stop reachability errors collection\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /**/
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_stop_reachability_errors_collection";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_STOP_REACHABILITY_ERROR_LOGGING;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_stop_reachability_errors_collection(
                    param_val_device->value.ulong_value
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "\r\n--->stop reachability errors collection returns: Success");
        send_string_to_screen(err_msg, TRUE);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_START_REACHABILITY_ERRORS_ID,1))
    {
      /*
       * Enter if this is a 'fe200 start_reachability_errors_collection' request.
       */
      PARAM_VAL
        *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_START_REACHABILITY_ERRORS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 start reachability errors collection\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /**/
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_start_reachability_errors_collection";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_START_REACHABILITY_ERROR_LOGGING;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_start_reachability_errors_collection(
                    param_val_device->value.ulong_value
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "\r\n--->start reachability errors collection returns: Success");
        send_string_to_screen(err_msg, TRUE);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_NEXT_REACHABILITY_ERROR_ID,1))
    {
      /*
       * Enter if this is a 'fe200 get_next_reachability_error' request.
       */
      PARAM_VAL
        *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_NEXT_REACHABILITY_ERROR_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 get next reachability error\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_get_next_reachability_error";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_GET_NEXT_REACHABILITY_ERROR;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_get_next_reachability_error(
                param_val_device->value.ulong_value
                   );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        if(!result.data.rechability_error_data.reachability_error_info.error_block_valid)
        {
          send_string_to_screen("\r\n--->get next reachability error returned with no error", TRUE);
        }
        else
        {
          switch (result.data.rechability_error_data.reachability_error_info.reachability_error_id)
          {
            case (REACHABILITY_FE23DID):
            {
              send_string_to_screen(
              "\r\n--->get next reachability error returned with\r\n     duplicate link to destination error:", TRUE);
              sal_sprintf(err_msg, "    happened more then once: %c\r\n"
                       "    time from system start : %ld [1/100th sec]\r\n"
                       "    destination chip id    : 0x%X\r\n"
                       "    new link number        : %02d\r\n"
                       "    stored link number     : %02d\r\n",
               (result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_fe23did_info.error_overflow ?'+':'-'),
                result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_fe23did_info.sys_time_10ms,
                result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_fe23did_info.destination_chip_id,
                result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_fe23did_info.new_link_number,
                result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_fe23did_info.stored_link_number
              );
              send_string_to_screen(err_msg, TRUE);
              break;
            }
            case (REACHABILITY_RMDIDOR):
            {
              send_string_to_screen(
              "\r\n--->get next reachability error returned with\r\n     destination id out of range error:", TRUE);
              sal_sprintf(err_msg, "    happened more then once: %c\r\n"
                       "    time from system start : %ld [1/100th sec]\r\n"
                       "    destination chip id    : 0x%X\r\n"
                       "    error link number      : %02d\r\n",
               (result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_rmdidor_info.error_overflow ?'+':'-'),
                result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_rmdidor_info.sys_time_10ms,
                result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_rmdidor_info.destination_chip_id,
                result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_rmdidor_info.err_link_number
              );
              send_string_to_screen(err_msg, TRUE);
              break;
            }
            case (REACHABILITY_UCCD):
            {
              send_string_to_screen(
              "\r\n--->get next reachability error returned with control\r\n     unreachable destination id error:", TRUE);
              sal_sprintf(err_msg, "    happened more then once: %c\r\n"
                       "    time from system start : %ld [1/100th sec]\r\n"
                       "    source chip id         : 0x%X\r\n"
                       "    destination chip id    : 0x%X\r\n"
                       "    error link number      : %02d\r\n",
                 (result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_uccd_info.error_overflow ?'+':'-'),
                  result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_uccd_info.sys_time_10ms,
                  result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_uccd_info.source_chip_id,
                  result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_uccd_info.destination_chip_id,
                  result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_uccd_info.err_link_number
              );
              send_string_to_screen(err_msg, TRUE);
              break;
            }
            case (REACHABILITY_UDCD):
            {
              send_string_to_screen(
              "\r\n--->get next reachability error returned with data\r\n     unreachable destination id error:", TRUE);
              sal_sprintf(err_msg, "    happened more then once: %c\r\n"
                       "    time from system start : %ld [1/100th sec]\r\n"
                       "    source chip id         : 0x%X\r\n"
                       "    destination chip id    : 0x%X\r\n"
                       "    error link number      : %02d\r\n"
                       "    was it multicast cell ?: %c\r\n",
                 (result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_udcd_info.error_overflow ?'+':'-'),
                  result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_udcd_info.sys_time_10ms,
                  result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_udcd_info.source_chip_id,
                  result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_udcd_info.destination_chip_id,
                  result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_udcd_info.err_link_number,
                 (result.data.rechability_error_data.reachability_error_info.reachability_error.reachability_udcd_info.multicast_cell ? '+':'-')
              );
              send_string_to_screen(err_msg, TRUE);
              break;
            }
            default:
            {
              sal_sprintf(err_msg, "\r\n--->get next reachability error returned with unknown error id: %d",
                        result.data.rechability_error_data.reachability_error_info.reachability_error_id
              );
              send_string_to_screen(err_msg, TRUE);
              break;
            }
          }
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_CLEAR_REACHABILITY_ERRORS_ID,1))
    {
      /* Enter if this is a 'fe200 clear_reachability_errors' request.*/
      PARAM_VAL *param_val_device;
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_CLEAR_REACHABILITY_ERRORS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 clear reachability errors\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /**/
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_clear_reachability_errors";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_CLEAR_REACHABILITY_ERRORS;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_clear_reachability_errors(
                    param_val_device->value.ulong_value
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "\r\n--->clear reachability errors returns: %d errors were cleared",
                result.data.clear_rechability_data.num_of_cleared_msgs
        );
        send_string_to_screen(err_msg, TRUE);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_UNICAST_TABLE_ID,1))
    {
      /* Enter if this is a 'fe200 fe200_agent_get_unicast_table' request. */
      PARAM_VAL
        *param_val_device,
        *param_val_starting_row,
        *param_val_general_purpose ;
      FE200_AGENT_IN_STRUCT
        *fe200_agent_in_struct ;
      FE200_AGENT_OUT_STRUCT
        *fe200_agent_out_struct ;
      unsigned
        int
          unit,
          entity_type ;
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_UNICAST_TABLE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                        "*** \'fe200 get unicast table\' error \r\n"
                        "*** or \'unknown parameter \'.\r\n"
                        "    Probably SW error\r\n"
        );
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      unit = param_val_device->value.ulong_value ;
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
              PARAM_FE200_STARTING_ROW_ID, 1,
              &param_val_starting_row, VAL_NUMERIC, err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      {
        ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_ENTITY_TYPE_ID, 1,
                  &param_val_general_purpose, VAL_SYMBOL, err_msg
                );
        if (0 != ret)
        {
          send_string_to_screen(err_msg,TRUE);
          ret = TRUE;
          goto exit;
        }
        entity_type =
          (unsigned int)param_val_general_purpose->numeric_equivalent ;
      }
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_get_unicast_table";
      if( isDeviceIdFE(unit) )
      {
        {
          in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
          fe200_agent_in_struct = &in_struct.common_union.fe200_struct ;
          fe200_agent_in_struct->proc_id = FE200_LOGICAL_READ;
          fe200_agent_in_struct->data.logical_read_data.unit = unit ;
          fe200_agent_in_struct->data.logical_read_data.field_id = FE200_MODE_FE2_FIELD ;
          if ( fmc_send_buffer_to_device_relative_id(
                unit,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
             ) )
          {
            sal_sprintf(err_msg,
                      "\r\n\n"
                      "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
            send_string_to_screen(err_msg,TRUE)  ;
            ret = TRUE  ;
            goto exit  ;
          }
          fe200_agent_out_struct = &(out_struct.common_union.fe200_struct) ;
          if (soc_sand_get_error_code_from_error_word(fe200_agent_out_struct->error_id) == SOC_SAND_OK)
          {
            /*
             * Enter if access of chip mode was successful.
             */
            if (fe200_agent_out_struct->data.logical_read_data.data)
            {
              /*
               * This is an FE2 device. make sure entity type is FE2 as well.
               */
              if (entity_type != SOC_SAND_FE2_ENTITY)
              {
                sal_sprintf(err_msg,
                          "\r\n\n"
                          "*** \'fe200 get unicast table\' error:\r\n"
                          "    This chip is configured as FE2 while request is to FE1 or FE3\r\n") ;
                send_string_to_screen(err_msg,TRUE)  ;
                ret = TRUE  ;
                goto exit  ;
              }
            }
            else
            {
              /*
               * This is an FE13 device. make sure entity type is FE1 or FE3 as well.
               */
              if ((entity_type != SOC_SAND_FE1_ENTITY) && (entity_type != SOC_SAND_FE3_ENTITY))
              {
                sal_sprintf(err_msg,
                          "\r\n\n"
                          "*** \'fe200 get unicast table\' error:\r\n"
                          "    This chip is configured as FE13 while request is NOT to FE1 or FE3\r\n") ;
                send_string_to_screen(err_msg,TRUE)  ;
                ret = TRUE  ;
                goto exit  ;
              }
            }
          }
          else
          {
            fe200_api_result = result.error_id;
            disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
            sal_sprintf(err_msg,
                      "\r\n\n"
                      "*** Logical read fail - on device 0x%08lX\r\n",
                      unit);
            send_string_to_screen(err_msg,FALSE)  ;
            ret = TRUE  ;
            goto exit  ;
          }
        }
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        fe200_agent_in_struct = &(in_struct.common_union.fe200_struct) ;
        fe200_agent_in_struct->proc_id = FE200_GET_UNICAST_TABLE;
        fe200_agent_in_struct->
            data.routing_table_data.unit = unit ;
        fe200_agent_in_struct->
          data.routing_table_data.entity_type = entity_type ;
        fe200_agent_in_struct->
          data.routing_table_data.starting_entry = param_val_starting_row->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                unit,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct ;
      }
      else
      {
        /*
         * Make sure that requests to entity type FE2 are made to SOC_SAND_FE200,
         * configured as FE2, and requests to entity type FE1 or FE3 are
         * made to SOC_SAND_FE200, configured as FE13.
         */
        fe200_agent_logical_read(
          unit,
          FE200_MODE_FE2_FIELD,
          &result) ;
        if (soc_sand_get_error_code_from_error_word(result.error_id) == SOC_SAND_OK)
        {
          /*
           * Enter if access of chip mode was successful.
           */
          if (result.data.logical_read_data.data)
          {
            /*
             * This is an FE2 device. make sure entity type is FE2 as well.
             */
            if (entity_type != SOC_SAND_FE2_ENTITY)
            {
              sal_sprintf(err_msg,
                        "\r\n\n"
                        "*** \'fe200 get unicast table\' error:\r\n"
                        "    This chip is configured as FE2 while request is to FE1 or FE3\r\n") ;
              send_string_to_screen(err_msg,TRUE)  ;
              ret = TRUE  ;
              goto exit  ;
            }
          }
          else
          {
            /*
             * This is an FE13 device. make sure entity type is FE1 or FE3 as well.
             */
            if ((entity_type != SOC_SAND_FE1_ENTITY) && (entity_type != SOC_SAND_FE3_ENTITY))
            {
              sal_sprintf(err_msg,
                        "\r\n\n"
                        "*** \'fe200 get unicast table\' error:\r\n"
                        "    This chip is configured as FE13 while request is NOT to FE1 or FE3\r\n") ;
              send_string_to_screen(err_msg,TRUE)  ;
              ret = TRUE  ;
              goto exit  ;
            }
          }
          result = fe200_agent_get_unicast_table(
                    unit,
                    entity_type,
                    param_val_starting_row->value.ulong_value
                 );
        }
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      { /* if !err*/
        fe_print_unicast_table(
          unit,
          &(result.data.routing_unicast_report.unicast_report)
        );
      }
      else
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Get unicast table fail - on device 0x%08lX\r\n",
                  unit) ;
        send_string_to_screen(err_msg,FALSE) ;
        ret = TRUE  ;
        goto exit  ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_DEFERRED_GET_UNICAST_TABLE_ID,1))
    {
      /* Enter if this is a 'fe200 fe200_agent_get_unicast_table' request. */
      PARAM_VAL *param_val_device, *param_val_starting_row, *param_val_polling_rate, *param_val_general_purpose;
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_DEFERRED_GET_UNICAST_TABLE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 get unicast table\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
              PARAM_FE200_STARTING_ROW_ID, 1,
              &param_val_starting_row, VAL_NUMERIC, err_msg
            ) ;
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SEND_CELL_SRC_ENTITY_TYPE_ID,1))
      {
        ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_SEND_CELL_SRC_ENTITY_TYPE_ID, 1,
                  &param_val_general_purpose, VAL_SYMBOL, err_msg
                );
        if (0 != ret)
        {
          send_string_to_screen(err_msg,TRUE);
          ret = TRUE;
          goto exit;
        }
      }
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
              PARAM_FE200_POLLING_RATE_ID, 1,
              &param_val_polling_rate, VAL_NUMERIC, err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      result = fe200_agent_register_callback_function(
                  param_val_device->value.ulong_value,
                  FE200_GET_UNICAST_TABLE, FALSE, /*poll*/
                  param_val_polling_rate->value.ulong_value,
                  0x35, /* to act as user callback id */
                  0xDEADBEAF, /* param1 - buffer size will be allocated by the FMC */
                  param_val_general_purpose->numeric_equivalent,
                  param_val_starting_row->value.ulong_value,
                  0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF /* param 4,5,6 */
               );
      fe200_api_result = result.error_id;
      soc_sand_proc_name = "fe200_agent_register_callback_function (get_unicast_table)" ;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "---->\r\n"
          "  fe200_agent_register_callback_function returned with deferred handle 0x%X\n\r",
                result.data.register_callback_data.callback_id ) ;
        send_string_to_screen(err_msg,TRUE)  ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_MULTICAST_TABLE_ID,1))
    {
      /* Enter if this is a 'fe200 fe200_agent_get_multicast_table' request. */
      PARAM_VAL *param_val_device, *param_val_starting_row, *param_val_general_purpose;
      unsigned int entity_type;
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_MULTICAST_TABLE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 get mulitcast table\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
              PARAM_FE200_STARTING_ROW_ID, 1,
              &param_val_starting_row, VAL_NUMERIC, err_msg
            ) ;
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      entity_type = SOC_SAND_FE2_ENTITY;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SEND_CELL_SRC_ENTITY_TYPE_ID,1))
      {
        ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_SEND_CELL_SRC_ENTITY_TYPE_ID, 1,
                  &param_val_general_purpose, VAL_SYMBOL, err_msg
                );
        if (0 != ret)
        {
          send_string_to_screen(err_msg,TRUE);
          ret = TRUE;
          goto exit;
        }
        entity_type = param_val_general_purpose->numeric_equivalent;
      }
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_get_multicast_table";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_GET_MULTICAST_TABLE;
        in_struct.common_union.fe200_struct.data.routing_table_data.unit = param_val_device->value.ulong_value;
        in_struct.common_union.fe200_struct.data.routing_table_data.entity_type = entity_type;
        in_struct.common_union.fe200_struct.data.routing_table_data.starting_entry = param_val_starting_row->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
      }
      else
      {
        result = fe200_agent_get_multicast_table(
                    param_val_device->value.ulong_value,
                    entity_type,
                    param_val_starting_row->value.ulong_value
                  );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "\r\n--->get multicast table returned with:\r\n"
                           "     reporting entity type    : fe%d\r\n"
                           "     debug flags pending      : %s\r\n"
                           "     reachability err pending : %s\r\n"
                           "     link state bitmap        : 0x%08lX 0x%08lX\r\n"
                           "     reporting %d entries starting at offset %d:\r\n",
            result.data.routing_multicast_report.multicast_report.reporting_entity_type,
           (result.data.routing_multicast_report.multicast_report.debug_flags_pending ? "yes":"no"),
           (result.data.routing_multicast_report.multicast_report.reachability_error_pending ? "yes":"no"),
            result.data.routing_multicast_report.multicast_report.link_state_up[0],
            result.data.routing_multicast_report.multicast_report.link_state_up[1],
            result.data.routing_multicast_report.multicast_report.num_entries,
            result.data.routing_multicast_report.multicast_report.offset
        );
        send_string_to_screen(err_msg, TRUE);
        if(!result.data.routing_multicast_report.multicast_report.debug_flags_pending)
        {
          switch (result.data.routing_multicast_report.multicast_report.reporting_entity_type)
          {
            case (SOC_SAND_FE1_ENTITY):
            {
              send_string_to_screen("multicast id|distribution bitmap", TRUE);
              send_string_to_screen("------------+-------------------", TRUE);
              if(result.data.routing_multicast_report.multicast_report.changes_only)
              { /* fe1 long report */
                for(ui = 0; ui < result.data.routing_multicast_report.multicast_report.num_entries; ++ui)
                {
                  sal_sprintf(err_msg, "   0x%04X   |    0x%08lX",
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe1_dist_multi_long_entry.index_of_entry,
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe1_dist_multi_long_entry.distribution_entry
                  );
                  send_string_to_screen(err_msg, TRUE);
                }
              }
              else
              { /* fe1 short report */
                for(ui = 0; ui < result.data.routing_multicast_report.multicast_report.num_entries; ++ui)
                {
                  sal_sprintf(err_msg, "   0x%04X   |    0x%08lX",
                    result.data.routing_multicast_report.multicast_report.offset + ui,
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe1_dist_multi_short_entry.distribution_entry
                  );
                  send_string_to_screen(err_msg, TRUE);
                }
              }
              break;
            }
            case (SOC_SAND_FE2_ENTITY):
            {
              send_string_to_screen("multicast id| replication bitmap", TRUE);
              send_string_to_screen("------------+-------------------", TRUE);
              if(result.data.routing_multicast_report.multicast_report.changes_only)
              { /* fe2 long report */
                for(ui = 0; ui < result.data.routing_multicast_report.multicast_report.num_entries; ++ui)
                {
                  sal_sprintf(err_msg, "   0x%04X   | 0x%08lX%08lX",
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe2_replic_multi_long_entry.index_of_entry,
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe2_replic_multi_long_entry.replication_entry[0],
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe2_replic_multi_long_entry.replication_entry[1]
                  );
                  send_string_to_screen(err_msg, TRUE);
                }
              }
              else
              { /* fe2 short report */
                for(ui = 0; ui < result.data.routing_multicast_report.multicast_report.num_entries; ++ui)
                {
                  sal_sprintf(err_msg, "   0x%04X   | 0x%08lX%08lX",
                    result.data.routing_multicast_report.multicast_report.offset + ui,
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe2_replic_multi_short_entry.replication_entry[0],
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe2_replic_multi_short_entry.replication_entry[1]
                  );
                  send_string_to_screen(err_msg, TRUE);
                }
              }
              break;
            }
            case (SOC_SAND_FE3_ENTITY):
            {
              send_string_to_screen("multicast id|replication bitmap", TRUE);
              send_string_to_screen("------------+------------------", TRUE);
              if(result.data.routing_multicast_report.multicast_report.changes_only)
              { /* fe3 long report */
                for(ui = 0; ui < result.data.routing_multicast_report.multicast_report.num_entries; ++ui)
                {
                  sal_sprintf(err_msg, "   0x%04X   |    0x%08lX",
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe3_replic_multi_long_entry.index_of_entry,
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe3_replic_multi_long_entry.replication_entry
                  );
                  send_string_to_screen(err_msg, TRUE);
                }
              }
              else
              { /* fe3 short report */
                for(ui = 0; ui < result.data.routing_multicast_report.multicast_report.num_entries; ++ui)
                {
                  sal_sprintf(err_msg, "   0x%04X   |    0x%08lX",
                    result.data.routing_multicast_report.multicast_report.offset + ui,
                    result.data.routing_multicast_report.multicast_report.dist_replic_entry[ui].fe3_replic_multi_short_entry.replication_entry
                  );
                  send_string_to_screen(err_msg, TRUE);
                }
              }
              break;
            }
          } /* end of switch */
        } /* if !debug*/
      } /* if !err*/
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SET_MULTICAST_TABLE_ID,1))
    {
      /* Enter if this is a 'fe200 fe200_agent_set_multicast_table' request. */
      PARAM_VAL *param_val_device, *param_val_general_purpose;
      DISTRIBUTION_REPLICATION_MULTI_REPORT mul_rep;
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_MULTICAST_TABLE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set mulitcast table\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /**/
      if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_ENTITY_TYPE_ID,1))
      {
        ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_ENTITY_TYPE_ID, 1,
                  &param_val_general_purpose, VAL_SYMBOL, err_msg
                );
        if (0 != ret)
        {
          send_string_to_screen(err_msg,TRUE);
          ret = TRUE;
          goto exit;
        }
      }
      /**/
      mul_rep.reporting_entity_type = param_val_general_purpose->numeric_equivalent;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_STARTING_ROW_ID, 1,
                        &param_val_general_purpose, VAL_NUMERIC, err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      mul_rep.offset = param_val_general_purpose->value.ulong_value;
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_LIST_OF_LINKS_ID, 1,
                        &param_val_general_purpose, VAL_NUMERIC, err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      switch(mul_rep.reporting_entity_type)
      {
        case (SOC_SAND_FE1_ENTITY):
        {
          mul_rep.dist_replic_entry->fe1_dist_multi_short_entry.distribution_entry = param_val_general_purpose->value.ulong_value;
          break;
        }
        case (SOC_SAND_FE2_ENTITY):
        {
          mul_rep.dist_replic_entry->fe2_replic_multi_short_entry.replication_entry[0] = param_val_general_purpose->value.ulong_value;
          ret = get_val_of( current_line, (int *)&match_index,
                  PARAM_FE200_LIST_OF_LINKS_ID, 2,
                  &param_val_general_purpose, VAL_NUMERIC, err_msg
                  ) ;
          if (0 != ret)
          {
            send_string_to_screen(err_msg,TRUE);
            ret = TRUE;
            goto exit;
          }
          mul_rep.dist_replic_entry->fe2_replic_multi_short_entry.replication_entry[1] = param_val_general_purpose->value.ulong_value;
          break;
        }
        case (SOC_SAND_FE3_ENTITY):
        {
          mul_rep.dist_replic_entry->fe3_replic_multi_short_entry.replication_entry = param_val_general_purpose->value.ulong_value;
          break;
        }
        default:
        {
          sal_sprintf(err_msg, "\r\n\n"
                  "*** \'fe200 set mulitcast table\' error \r\n"
                  "*** or \'expecting source_entity_type \'.\r\n"
          );
          send_string_to_screen(err_msg,TRUE);
          ret = TRUE;
          goto exit;
        }
      }
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_set_multicast_table";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_SET_MULTICAST_TABLE_ENTRY;
        in_struct.common_union.fe200_struct.data.multicast_entry_data.unit = param_val_device->value.ulong_value;
        in_struct.common_union.fe200_struct.data.multicast_entry_data.multicast_entry = mul_rep;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_set_multicast_table_entry(
                    param_val_device->value.ulong_value,
                    mul_rep
                  );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_REPEATER_TABLE_ID,1))
    {
      /* Enter if this is a 'fe200 fe200_agent_get_repeater_table' request. */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_REPEATER_TABLE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                        "*** \'fe200 get repeater table\' error \r\n"
                        "*** or \'unknown parameter \'.\r\n"
                        "    Probably SW error\r\n"
               );
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_get_repeater_table";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_GET_REPEATER_TABLE;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_get_repeater_table(
                    param_val_device->value.ulong_value
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        send_string_to_screen("\r\n--->get repeater table returned with:\r\n\n"
                                  "link # |connected to\r\n"
                                  "-------+------------", TRUE
        );
        for(ui = 0; ui < 64; ++ui)
        {
          sal_sprintf(err_msg, "  %02d   |   %02d",
            ui, result.data.routing_repeater_table.repeater_table.repeater_entry[ui]
          );
          send_string_to_screen(err_msg, TRUE);
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SET_REPEATER_TABLE_ID,1))
    {
      /*
       * Enter if this is a 'fe200 fe200_agent_set_repeater_table' request.
       */
      PARAM_VAL *param_val_device, *param_val_general_purpose;
      REPEATER_ROUTING_TABLE   rept_tab;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_REPEATER_TABLE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                  "*** \'fe200 set repeater table\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      for (ui=0; ui<64; ++ui)
      {
        ret = get_val_of( current_line, (int *)&match_index,
                          PARAM_FE200_SET_REPEATER_TABLE_BODY_ID, ui+1,
                          &param_val_general_purpose, VAL_NUMERIC, err_msg
                        );
        if (!ret)
        {
          rept_tab.repeater_entry[ui] = param_val_general_purpose->value.ulong_value;
        }
        else
        {
          rept_tab.repeater_entry[ui] = (unsigned long)-1;
          ret = 0;
        }
      }
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_set_repeater_table";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_SET_REPEATER_TABLE;
        in_struct.common_union.fe200_struct.data.repeater_table_data.unit = param_val_device->value.ulong_value;
        in_struct.common_union.fe200_struct.data.repeater_table_data.repeater_table = rept_tab;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_set_repeater_table(
                    param_val_device->value.ulong_value,
                    rept_tab
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_LOOPBACK_TYPE_ID,1))
    {
      /*
       * Enter if this is a 'fe200 fe200_agent_get_loopback_type' request.
       */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_LOOPBACK_TYPE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 get loopback type\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_get_loopback_type";
      result = fe200_agent_get_loopback_type(
                  param_val_device->value.ulong_value
               );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        if(result.data.diagnostics_loopback_data.local_loop_on)
        {
          send_string_to_screen("\r\n--->get loopback type returned with:\r\n\n"
                                "    local loopback on\r\n", TRUE
          );
        }
        else
        {
          send_string_to_screen("\r\n--->get loopback type returned with:\r\n\n"
                       "    local loopback off\r\n", TRUE
          );
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SET_LOOPBACK_TYPE_ID,1))
    {
      /* Enter if this is a 'fe200 fe200_agent_set_loopback_type' request. */
      PARAM_VAL *param_val_device, *param_val_loopback_type;
      unsigned int local_loopback_on;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_LOOPBACK_TYPE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                  "*** \'fe200 set loopback type\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_LOOPBACK_TYPE_ID, 1,
                        &param_val_loopback_type, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set loopback type\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      local_loopback_on =  ( ON_EQUIVALENT == param_val_loopback_type->numeric_equivalent);
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_set_loopback_type";
      result = fe200_agent_set_loopback_type(
                 param_val_device->value.ulong_value, local_loopback_on
               );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_PRBS_MODE_ID,1))
    {
      /* Enter if this is a 'fe200 fe200_agent_get_prbs_mode' request. */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_PRBS_MODE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                  "*** \'fe200 get prbs mode\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_get_prbs_mode";
      result = fe200_agent_get_prbs_mode(
                 param_val_device->value.ulong_value
               );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        fe200_print_FE200_PRBS_SETUP(
          &result.data.diagnostics_prbs_setup_data.prbs_setup,
          ui_printing_policy_get()
        );
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SET_PRBS_MODE_ID,1))
    {
      /* Enter if this is a 'fe200 fe200_agent_set_prbs_mode' request. */
      PARAM_VAL *param_val_device, *param_val_generator_active, *param_val_generator_link, *param_val_checker_link;
      unsigned int mac_index;
      FE200_PRBS_SETUP*  prbs_setup;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_PRBS_MODE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                  "*** \'fe200 set prbs mode\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PRBS_ACTIVATE_GENERATOR_ID, 1,
                        &param_val_generator_active, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                  "*** \'fe200 set prbs mode\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PRBS_GENERATOR_LINK_ID, 1,
                        &param_val_generator_link, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                  "*** \'fe200 set prbs mode\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_PRBS_CHECKER_LINK_ID, 1,
                        &param_val_checker_link, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                  "*** \'fe200 set prbs mode\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /*
       * lets check that entered params belong to the same MAC
       */
      if( (ON_EQUIVALENT == param_val_generator_active->numeric_equivalent) &&
        ( ((param_val_generator_link->value.ulong_value >> 4) & 0x0FFFFFFF) !=
        ((param_val_checker_link -> value.ulong_value >> 4) & 0x0FFFFFFF)  )
      )
      {
        sal_sprintf(err_msg, "\r\n\n"
                "*** \'fe200 set prbs mode\' error \r\n"
                "*** \'checker link belong to MAC %c whereas generator link belong to MAC %c\'.\r\n",
                'A'+(unsigned char)((param_val_checker_link -> value.ulong_value >> 4) & 0x0FFFFFFF),
                'A'+(unsigned char)((param_val_generator_link->value.ulong_value >> 4) & 0x0FFFFFFF)
        );
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
        /**/
      result = fe200_agent_get_prbs_mode(
                  param_val_device->value.ulong_value
               );
      mac_index =  ((param_val_checker_link -> value.ulong_value >> 4) & 0x0FFFFFFF);
      prbs_setup = &result.data.diagnostics_prbs_setup_data.prbs_setup;
      prbs_setup->prbs_generator_checker_unit[mac_index].activate_prbs_generator =
                (ON_EQUIVALENT == param_val_generator_active->numeric_equivalent);
      prbs_setup->prbs_generator_checker_unit[mac_index].prbs_generator_link =
                              (param_val_generator_link->value.ulong_value & 0xF);
      prbs_setup->prbs_generator_checker_unit[mac_index].prbs_checker_link   =
                                (param_val_checker_link->value.ulong_value & 0xF);
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_set_prbs_mode";
      result = fe200_agent_set_prbs_mode(
                 param_val_device->value.ulong_value, *prbs_setup
               );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_START_PRBS_TEST_ID,1))
    {
      /* Enter if this is a 'fe200 agent_start_prbs_test' request. */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_START_PRBS_TEST_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 start prbs test\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_start_prbs_test";
      result = fe200_agent_start_prbs_test(
                 param_val_device->value.ulong_value
               );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_STOP_PRBS_TEST_ID,1))
    {
      /* Enter if this is a 'fe200 agent_stop_prbs_test' request. */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_STOP_PRBS_TEST_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                  "*** \'fe200 stop prbs test\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_stop_prbs_test";
      result = fe200_agent_stop_prbs_test(
                 param_val_device->value.ulong_value
               );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_MAC_COUNTER_TYPE_ID,1))
    {
      /* Enter if this is a 'fe200 get_mac_counter_type' request. */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_MAC_COUNTER_TYPE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                         "*** \'fe200 get mac counter type\' error \r\n"
                         "*** or \'unknown parameter \'.\r\n"
                         "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /**/
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_get_mac_counter_type";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_GET_MAC_COUNTER_TYPE;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_get_mac_counter_type(
                    param_val_device->value.ulong_value
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        send_string_to_screen("\r\n--->get mac counter type returned with:\r\n", TRUE);
        switch(result.data.statistics_mac_counters_type_data.tx_type)
        {
          case(COUNT_ALL_CELLS):
          {
            send_string_to_screen("    tx_type: all_cells", TRUE);
            break;
          }
          case(COUNT_CONTROL_CELLS_ONLY):
          {
            send_string_to_screen("    tx_type: contorl_cells", TRUE);
            break;
          }
          case(COUNT_PKT_DATA_CELLS_ONLY):
          {
            send_string_to_screen("    tx_type: non-empty_data_cells", TRUE);
            break;
          }
          case(COUNT_EMPTY_DATA_CELLS_ONLY):
          {
            send_string_to_screen("    tx_type: empty_data_cells", TRUE);
            break;
          }
          default:
          {
            send_string_to_screen("    tx_type: unknown", TRUE);
            break;
          }
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_SET_MAC_COUNTER_TYPE_ID,1))
    {
      /*
       * Enter if this is a 'fe200 fe200_agent_set_mac_counter_type' request.
       */
      PARAM_VAL *param_val_device,
#if !MAC_RX_COUNTER_INTERFERE_WTITH_LEAKYBUCKET
        *param_val_rx_type,
#endif
        *param_val_tx_type;

      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_SET_MAC_COUNTER_TYPE_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 set mac counter type\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE  ;
        goto exit  ;
      }
      /**/
#if !MAC_RX_COUNTER_INTERFERE_WTITH_LEAKYBUCKET
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_MAC_COUNTER_RX_TYPE_ID, 1,
                        &param_val_rx_type, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
        "*** \'fe200 set mac counter type\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE  ;
        goto exit  ;
      }
#endif
      /**/
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_MAC_COUNTER_TX_TYPE_ID, 1,
                        &param_val_tx_type, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
        "*** \'fe200 set mac counter type\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE  ;
        goto exit  ;
      }
      /**/
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_set_mac_counter_type";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_SET_MAC_COUNTER_TYPE;
        in_struct.common_union.fe200_struct.data.set_mac_counter_type_data.unit = param_val_device->value.ulong_value;
#if !MAC_RX_COUNTER_INTERFERE_WTITH_LEAKYBUCKET
        in_struct.common_union.fe200_struct.data.set_mac_counter_type_data.rx_count_type = param_val_rx_type->numeric_equivalent;
#endif
        in_struct.common_union.fe200_struct.data.set_mac_counter_type_data.tx_count_type = param_val_tx_type->numeric_equivalent;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_set_mac_counter_type(
                    param_val_device->value.ulong_value,
                    param_val_tx_type->numeric_equivalent
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_START_MAC_COUNTERS_COLLECTION_ID,1))
    {
      /* Enter if this is a 'fe200 agent_start_mac_counters_collection' request. */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_START_MAC_COUNTERS_COLLECTION_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                  "*** \'fe200 start mac counters collection\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_start_mac_counters_collection";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_START_MAC_COUNTERS_COLLECTION;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_start_mac_counters_collection(
                   param_val_device->value.ulong_value
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_STOP_MAC_COUNTERS_COLLECTION_ID,1))
    {
      /* Enter if this is a 'fe200 agent_stop_mac_counters_collection' request. */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_STOP_MAC_COUNTERS_COLLECTION_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        sal_sprintf(err_msg, "\r\n\n"
                         "*** \'fe200 stop mac_counters_collection\' error \r\n"
                         "*** or \'unknown parameter \'.\r\n"
                         "    Probably SW error\r\n");
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      send_array_to_screen("\r\n",2);
      soc_sand_proc_name = "fe200_agent_stop_mac_counters_collection";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_STOP_MAC_COUNTERS_COLLECTION;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_stop_mac_counters_collection(
                   param_val_device->value.ulong_value
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_MAC_COUNTERS_ID,1))
    {
      /* Enter if this is a 'fe200 get_count_of_mac_cells' request. */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_MAC_COUNTERS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 get_count_of_mac_cells\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_get_count_of_mac_cells";
      if( isDeviceIdFE(param_val_device->value.ulong_value) )
      {
        in_struct.out_struct_type = FE200_STRUCTURE_TYPE;
        in_struct.common_union.fe200_struct.proc_id = FE200_GET_COUNT_OF_MAC_CELLS;
        in_struct.common_union.fe200_struct.data.just_unit_data.unit = param_val_device->value.ulong_value;
        if ( fmc_send_buffer_to_device_relative_id(
                param_val_device->value.ulong_value,
                sizeof(in_struct),
                (unsigned char *)&in_struct,
                sizeof(out_struct),
                (unsigned char *)&out_struct
           ) )
        {
          sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** \'fe200 fmc_send_buffer_to_device_relative_id\' error \r\n");
          send_string_to_screen(err_msg,TRUE)  ;
          ret = TRUE  ;
          goto exit  ;
        }
        result = out_struct.common_union.fe200_struct;
      }
      else
      {
        result = fe200_agent_get_count_of_mac_cells(
                    param_val_device->value.ulong_value
                 );
      }
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        char *tx_type;
        switch(result.data.statistics_mac_counters_report_data.mac_counters_report.tx_count_type)
        {
          case (COUNT_ALL_CELLS):             tx_type = "all cells"; break;
          case (COUNT_CONTROL_CELLS_ONLY):    tx_type = "control cells"; break;
          case (COUNT_PKT_DATA_CELLS_ONLY):   tx_type = "non-empty data cells"; break;
          case (COUNT_EMPTY_DATA_CELLS_ONLY): tx_type = "empty data cells"; break;
          default:                            tx_type = "unknown type"; break;
        }
        /*
         * lets print the report to the screen
         */
        sal_sprintf(err_msg,
          "\r\n---> get count of mac cells:\r\n\n"
          "tx counter type: %s\r\n",
          tx_type
        );
        send_string_to_screen(err_msg, TRUE) ;
        send_string_to_screen(" #| tx counter |tx-of| crc | ',' |group", TRUE);
        send_string_to_screen("--+------------+-----+-----+-----+-----", TRUE);
        for(ui = 0 ; ui < 64 ; ++ui)
        {
          sal_sprintf(err_msg, "%2d| 0x%08lX |  %c  |  %c  |  %c  |  %c", ui,
             result.data.statistics_mac_counters_report_data.mac_counters_report.single_mac_counter[ui].tx_counter,
            (result.data.statistics_mac_counters_report_data.mac_counters_report.single_mac_counter[ui].tx_counter_overflowed?'+':'-'),
            (result.data.statistics_mac_counters_report_data.mac_counters_report.single_mac_counter[ui].rx_crc_error?'+':'-'),
            (result.data.statistics_mac_counters_report_data.mac_counters_report.single_mac_counter[ui].rx_misalignment_error? '+':'-'),
            (result.data.statistics_mac_counters_report_data.mac_counters_report.single_mac_counter[ui].rx_code_group_error?'+':'-')
          );
          send_string_to_screen(err_msg, TRUE) ;
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_DEFERRED_GET_MAC_COUNTERS_ID,1))
    {
      /*
       * Enter if this is a 'fe200 deferred get mac counters' request.
       */
      PARAM_VAL *param_val_device,  *param_val_polling_rate;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_DEFERRED_GET_MAC_COUNTERS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      ret = get_val_of( current_line, (int *)&match_index,
              PARAM_FE200_POLLING_RATE_ID, 1,
              &param_val_polling_rate, VAL_NUMERIC, err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      result = fe200_agent_register_callback_function(
                  param_val_device->value.ulong_value,
                  FE200_GET_COUNT_OF_MAC_CELLS, FALSE, /*poll*/
                  param_val_polling_rate->value.ulong_value,
                  0x55, /* to act as user callback id */
                  0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF /* param 1,2,3,4,5,6 */
               );
      fe200_api_result = result.error_id;
      soc_sand_proc_name = "fe200_agent_register_callback_function (get_count_of_mac_cells)" ;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "---->\r\n"
          "  fe200_agent_register_callback_function returned with deferred handle 0x%X\n\r",
                result.data.register_callback_data.callback_id );
        send_string_to_screen(err_msg,TRUE);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_GET_BER_COUNTERS_ID,1))
    {
      /*
       * Enter if this is a 'fe200 get_ber_counters' request.
       */
      PARAM_VAL *param_val_device;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_GET_BER_COUNTERS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fe200 get_ber_counters\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_agent_get_ber_counters";
      result = fe200_agent_get_ber_counters(
                  param_val_device->value.ulong_value
               );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        fe200_print_FE200_BER_COUNTERS_REPORT(
          &result.data.diagnostics_ber_counters_report_data.ber_counters_report,
          ui_printing_policy_get()
        );
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_DIAGNOSTICS_DUMP_REGS_ID,1))
    {
      PARAM_VAL *param_val_device;
      /*
       * Enter if this is a 'fap20v dump_regs' request.
       */
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_DIAGNOSTICS_DUMP_REGS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                  "\r\n"
                  "*** \'fe200_diagnostics_dump_regs\' error \r\n"
                  "*** or \'unknown parameter \'.\r\n"
                  "    Probably SW error\r\n") ;
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE  ;
        goto exit  ;
      }
      /*
       */
      send_array_to_screen("\r\n",2) ;
      soc_sand_proc_name = "fe200_diagnostics_dump_regs";
      fe200_diagnostics_dump_regs(
        param_val_device->value.ulong_value
      );
      send_array_to_screen("\r\n",2) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_DEFERRED_GET_BER_COUNTERS_ID,1))
    {
      /*
       * Enter if this is a 'fe200 deferred get ber counters' request.
       */
      PARAM_VAL *param_val_device,  *param_val_polling_rate;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_DEFERRED_GET_BER_COUNTERS_ID, 1,
                        &param_val_device, VAL_NUMERIC, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE)  ;
        ret = TRUE;
        goto exit;
      }
      ret = get_val_of( current_line, (int *)&match_index,
              PARAM_FE200_POLLING_RATE_ID, 1,
              &param_val_polling_rate, VAL_NUMERIC, err_msg
            );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      result = fe200_agent_register_callback_function(
                  param_val_device->value.ulong_value,
                  FE200_GET_BER_COUNTERS, FALSE, /*poll*/
                  param_val_polling_rate->value.ulong_value,
                  0x65, /* to act as user callback id */
                  0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF, 0xDEADBEAF /* param 1,2,3,4,5,6 */
               );
      fe200_api_result = result.error_id;
      soc_sand_proc_name = "fe200_agent_register_callback_function (get_ber_counters)" ;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        sal_sprintf(err_msg, "---->\r\n"
          "  fe200_agent_register_callback_function returned with deferred handle 0x%X\n\r",
                result.data.register_callback_data.callback_id );
        send_string_to_screen(err_msg,TRUE);
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_START_AND_REGISTER_ID,1))
    {
      unsigned int device_handle1, device_handle2;
      unsigned long fe200_api_result, start_address[CHIP_SIM_NOF_CHIPS];
      /*
       * Enter if this is a 'fe200 start driver and register devices' request.
       */
      PARAM_VAL *param_val_interrupt_mockup;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_START_AND_REGISTER_ID, 1,
                        &param_val_interrupt_mockup, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      /*
       * start the driver
       */
      soc_sand_proc_name = "soc_sand_module_open" ;
      fe200_agent_sand_module_open(
        SOC_SAND_MAX_DEVICE, /* max devices */
        16, /* system_tick_in_ms */
        70, /* tcm task priority */
        1,  /* min_time_between_tcm_activation */
        (param_val_interrupt_mockup->numeric_equivalent == 1),  /* TCM mockup interrupts */
        &result
      );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        start_address[0] = (unsigned long)0x40000000;
        start_address[1] = (unsigned long)0x40100000;
        /*
         * register devices
         */
        send_string_to_screen("----> SOC_SAND_FE200 registering 2 SOC_SAND_FE200 devices:", TRUE) ;
        soc_sand_proc_name = "fe200_register_device (No. 1)" ;
        fe200_agent_register_device(
           start_address[0],
           reset_fe200_device,
           ((param_val_interrupt_mockup->numeric_equivalent == 1) ? 2 : -1), /*irq*/
           &result
        );
        fe200_api_result = result.error_id;
        device_handle1 = result.data.register_device_data.device_handle;
        disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
        err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
        if (!err)
        {
          sal_sprintf(err_msg, "---->\r\n"
            "  1. fe200_register_device with handle 0x%X",device_handle1) ;
          send_string_to_screen(err_msg,TRUE) ;
          soc_sand_proc_name = "fe200_register_device (No. 2)" ;
          fe200_agent_register_device(
             start_address[1],
             reset_fe200_device,
             ((param_val_interrupt_mockup->numeric_equivalent == 1) ? 3 : -1),/*irq*/
             &result
          );
          fe200_api_result = result.error_id;
          device_handle2 = result.data.register_device_data.device_handle;
          disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
          err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
          if (!err)
          {
            sal_sprintf(err_msg, "---->\r\n"
              " 2. fe200_register_device returned with handle 0x%X", device_handle2) ;
            send_string_to_screen(err_msg, TRUE) ;
          }
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_UNREGISTER_AND_SHUTDOWN_ID,1))
    {
      PARAM_VAL *param_val_interrupt_mockup;
      ret = get_val_of( current_line, (int *)&match_index,
                        PARAM_FE200_UNREGISTER_AND_SHUTDOWN_ID, 1,
                        &param_val_interrupt_mockup, VAL_SYMBOL, err_msg
                      );
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE);
        ret = TRUE;
        goto exit;
      }
      soc_sand_proc_name = "fe200_unregister_device (No. 2)" ;
      fe200_agent_unregister_device(
         1,
         ((param_val_interrupt_mockup->numeric_equivalent == 1) ? 3 : -1),/*irq*/
         &result
      );
      fe200_api_result = result.error_id;
      disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
      err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
      if (!err)
      {
        soc_sand_proc_name = "fe200_unregister_device (No. 1)" ;
        fe200_agent_unregister_device(
           0,
           ((param_val_interrupt_mockup->numeric_equivalent == 1) ? 2 : -1),/*irq*/
           &result
        );
        fe200_api_result = result.error_id;
        disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE) ;
        err = soc_sand_get_error_code_from_error_word(fe200_api_result) != SOC_SAND_OK;
        if (!err)
        {
            soc_sand_proc_name = "soc_sand_module_close";
            fe200_agent_sand_module_close(&result);
            fe200_api_result = result.error_id;
            disp_fe_result(fe200_api_result,soc_sand_proc_name, TRUE);
            soc_sand_os_resource_print() ;
        }
      }
      /*
      lets un register the FMC for remote connections
      coreSetSendBuffToCoreCallBack(NULL);
      TODO!! - Erase.
      */
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SCENARIO_1_ID,1))
    {
      unsigned long handle1 ;
      unsigned long fe200_api_result;
      SOC_SAND_USER_CALLBACK  user_callback_structure ;

      user_callback_structure.callback_func = test_scenario_1_read_callback ;
      user_callback_structure.result_ptr = read1 ;
      user_callback_structure.param1 = 0x0 ;
      user_callback_structure.param2 = 2*sizeof(unsigned long) ;
      user_callback_structure.param3 = FALSE ; /* direct */
      user_callback_structure.param4 = 0xDEADBEAF ;  /* this data should not be read*/
      user_callback_structure.param5 = 0xDEADBEAF ;  /* this data should not be read*/
      user_callback_structure.param6 = 0xDEADBEAF ;  /* this data should not be read*/
      user_callback_structure.interrupt_not_poll = FALSE ; /* polling */
      user_callback_structure.callback_rate      = 5 ; /* polling rate*/
      user_callback_structure.user_callback_id   = 0xa  ;

      /*
       * start 2 callbacks that read the first 2 words, waiting for them to fail...
       */
      send_string_to_screen("\r\n----> SOC_SAND_FE200 registering the first callback (callback_id = 0xA)\r\n"
                                "       that reads 8 first bytes of the device 0 polling rate = 5 ticks", TRUE) ;
      fe200_api_result =  fe200_register_callback_function(
                            0,
                            FE200_MEM_READ,
                            &user_callback_structure,
                            &handle1
                          ) ;

      sal_sprintf(err_msg, "----> SOC_SAND_FE200 fe200_register_callback_function returned with 0x%lX\r\n"
                       "       and deferred handle 0x%lX", fe200_api_result, handle1) ;
      send_string_to_screen(err_msg, TRUE) ;
/*
 * when testing timing issues - we want only one callback on - the rest of the calles defined out
 */
      send_string_to_screen("----> type 'fe200 general unregister_devices_and_shutdown_driver' to terminate test.\r\n", TRUE) ;
    }

#if LINK_ATM_LIBRARIES

    else if ( (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SCENARIO_2_ID,1)) ||
         (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SCENARIO_3_ID,1))
       )
    {
      int
        test_failed ;
      err =
        fe200_atm_do_all_tests(
          FE200_ATM_FULL_REGRESSION_TEST_ID,
          &test_failed,TRUE
        ) ;
      if (err)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->fe200_atm_do_all_tests returned error code: %d",
          err
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      if (test_failed)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->Full Regression Test has failed with code: %d",
          test_failed
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->Full Regression Test has SUCCEEDED"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }

#endif /*#if LINK_ATM_LIBRARIES*/

  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SCENARIO_4_ID,1))
    {
      result = fe200_agent_verify_cell_module(0,1);
      if( !result.data.verification_data.success_not_failure)
      {
        sal_sprintf(
          err_msg, "\r\n---->\r\n"
          "  fe200_agent_verify_cell_module returned with:\r\n"
          "   success/fail : FAIL\r\n"
          "   error code   : %d\r\n"
          "   proc_id      : 0x%lX\n\r"
          "   error_id     : 0x%lX\n\r"
          "   description  : %s",
          result.data.verification_data.test_error_code,
          result.proc_id, result.error_id,
          result.data.verification_data.test_description
        );
      }
      else
      {
        sal_sprintf(
          err_msg, "\r\n---->\r\n"
          "  fe200_agent_verify_cell_module returned with:\r\n"
          "   %s\r\n", result.data.verification_data.test_description
        );
      }
      send_string_to_screen(err_msg,FALSE);
      /*
       */
      print_fe200_agent_error_logger("fe200_agent_verify_cell_module");
    }

#if LINK_ATM_LIBRARIES

    else if (
      !search_param_val_pairs(
            current_line,&match_index,PARAM_FE200_TEST_SCENARIO_10_ID,1))
    {
      int
        test_failed ;
      err =
        fe200_atm_do_all_tests(
          FE200_ATM_SELF_CELLS_TEST_ID,
          &test_failed,TRUE
        ) ;
      if (err)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->fe200_atm_do_all_tests returned error code: %d",
          err
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      if (test_failed)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->Self Cells Test has failed with code: %d",
          test_failed
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    else
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->Self Cells Test has SUCCEEDED"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }

#endif /*#if LINK_ATM_LIBRARIES*/

    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SCENARIO_5_ID,1))
    {
#define USE_SELF_LINKS 1
#if USE_SELF_LINKS
/* { */
      result = fe200_agent_verify_self_links_module(0) ;
      if (!result.data.verification_data.success_not_failure)
      {
        sal_sprintf(
          err_msg,
          "\r\n---->\r\n"
          "  fe200_agent_verify_self_links_module returned with:\r\n"
          "   success/fail : FAIL\r\n"
          "   error code   : %d\r\n"
          "   proc_id      : 0x%lX\n\r"
          "   error_id     : 0x%lX\n\r"
          "   description  : %s",
          result.data.verification_data.test_error_code,
          result.proc_id,
          result.error_id,
          result.data.verification_data.test_description
        ) ;
      }
      else
      {
        sal_sprintf(
          err_msg, "\r\n---->\r\n"
          "  fe200_agent_verify_self_links_module returned with:\r\n"
          "   %s\r\n",
          result.data.verification_data.test_description
        ) ;
      }
      send_string_to_screen(err_msg,FALSE);
      /*
       */
      print_fe200_agent_error_logger("fe200_agent_verify_self_links_module");
/* } */
#else
/* { */
      result = fe200_agent_verify_links_module(0,1);
      if( !result.data.verification_data.success_not_failure)
      {
        sal_sprintf(
          err_msg, "\r\n---->\r\n"
          "  fe200_agent_verify_links_module returned with:\r\n"
          "   success/fail : FAIL\r\n"
          "   error code   : %d\r\n"
          "   proc_id      : 0x%lX\n\r"
          "   error_id     : 0x%lX\n\r"
          "   description  : %s",
          result.data.verification_data.test_error_code,
          result.proc_id, result.error_id,
          result.data.verification_data.test_description
        );
      }
      else
      {
        sal_sprintf(
          err_msg, "\r\n---->\r\n"
          "  fe200_agent_verify_links_module returned with:\r\n"
          "   %s\r\n", result.data.verification_data.test_description
        );
      }
      send_string_to_screen(err_msg,FALSE);
      /*
       */
      print_fe200_agent_error_logger("fe200_agent_verify_links_module");
/* } */
#endif
    }

#if LINK_ATM_LIBRARIES

  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SCENARIO_6_ID,1))
    {
      int
        test_failed ;
      err =
        fe200_atm_do_all_tests(
          FE200_ATM_ROUTING_TEST_ID,
          &test_failed,TRUE
        ) ;
      if (err)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->fe200_atm_do_all_tests returned error code: %d",
          err
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      if (test_failed)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->Routing Test has failed with code: %d",
          test_failed
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->Routing Test has SUCCEEDED"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SCENARIO_7_ID,1))
    {
      int
        test_failed ;
      err =
        fe200_atm_do_all_tests(
          FE200_ATM_STATISTICS_TEST_ID,
          &test_failed,TRUE
        ) ;
      if (err)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->fe200_atm_do_all_tests returned error code: %d",
          err
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      if (test_failed)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->Statistics Test has failed with code: %d",
          test_failed
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->Statistics Test has SUCCEEDED"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SCENARIO_8_ID,1))
    {
      int
        test_failed ;
      err =
        fe200_atm_do_all_tests(
          FE200_ATM_DIAGNOSTICS_TEST_ID,
          &test_failed,TRUE
        ) ;
      if (err)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->fe200_atm_do_all_tests returned error code: %d",
          err
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      if (test_failed)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->Diagnostics Test has failed with code: %d",
          test_failed
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->Diagnostics Test has SUCCEEDED"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SCENARIO_9_ID,1))
    {
      int
        test_failed ;
      err =
        fe200_atm_do_all_tests(
          FE200_ATM_GENERAL_TEST_ID,
          &test_failed,TRUE
        ) ;
      if (err)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->fe200_atm_do_all_tests returned error code: %d",
          err
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      if (test_failed)
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->General Test has failed with code: %d",
          test_failed
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
          "\r\n"
          "--->General Test has SUCCEEDED"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }

#endif /*#if LINK_ATM_LIBRARIES*/

    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SCENARIO_11_ID,1))
    {
      unsigned int
        test_failed ;
      unsigned int
        err;

      err =
        fe200_test_unreg_reg(
          &test_failed,
          FALSE
        );

      if(err)
      {
        soc_sand_os_printf (
          "fe_test_unreg_reg failed to execute, ERROR CODE: %.5u \n\r",
          err
        );
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SSR_RESET_ID,1))
    {
      unsigned int
        err;

      err =
        sweep_fe200_ssr_test_reset(
          FALSE
        );

      if(err)
      {
        soc_sand_os_printf (
          "sweep_fe200_ssr_test_reset failed to execute, ERROR CODE: %.5u \n\r",
          err
        );
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_FE200_TEST_SSR_XOR_ID,1))
    {
      unsigned int
        err;

      err =
        sweep_fe200_ssr_test_xor_check(
          FALSE
        );

      if(err)
      {
        soc_sand_os_printf (
          "sweep_fe200_ssr_test_xor_check failed to execute, ERROR CODE: %.5u \n\r",
          err
        );
      }
    }
    else
    {
      /*
       * Enter if an unknown request.
       */
      send_string_to_screen(
        "\r\n"
        "*** Fe200 command with unknown parameters'.\r\n"
        "    Syntax error/sw error...\r\n",
        TRUE)  ;
      ret = TRUE  ;
      goto exit  ;
    }
  }
exit:
  return (ret)  ;
}



