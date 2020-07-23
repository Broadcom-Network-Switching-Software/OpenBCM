/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

/*
 * Tasks information.
 */
#include <appl/diag/dpp/tasks_info.h>
#include <appl/diag/dpp/utils_ip_mgmt.h>
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
#include <appl/diag/dpp/utils_pure_defi.h>
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
#include <bcm_app/dpp/UserInterface/ui_general_utils.h>
/*
 * Software emulation exception include file.
 *
#include UTILS_SOFT_EMUL_EXP_H
 *
 * Dune/Marvell RPC mechanism.
 */
#include <appl/diag/dpp/dune_rpc.h>
/* } */
#else
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
/*
 * Definitions specific to reference system.
 */
#include <usrapp.h>
#if LINK_PSS_LIBRARIES
#include <prestera/common/gtCommonVersion.h>
#include <gtExtDrv/version/gtExtDrvVersion.h>
#include <appDemo/sysHwConfig/gtAppDemoVersion.h>
#endif
/* } */
#endif

#include <soc/dpp/SAND/Management/sand_general_params.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

  #include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_framework.h>
#if LINK_FAP20V_LIBRARIES
  #include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_framework.h>
#endif
#if LINK_FE200_LIBRARIES
  #include <soc/dpp/SOC_SAND_FE200/fe200_api_framework.h>
#endif
#include <soc/dpp/Timna/timna_api_framework.h>

/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_pure_defi.h>
#include <appl/diag/dpp/utils_string.h>
#include <appl/diag/dpp/utils_software_emulation_exp.h>
#include <appl/diag/dpp/utils_aux_input.h>
#include <appl/diag/dpp/utils_clock.h>
#include <appl/diag/dpp/utils_line_FTG.h>
#include <appl/diag/dpp/utils_line_GFA.h>
#if LINK_FAP21V_LIBRARIES
  #include <appl/diag/dpp/utils_line_GFA_FAP21V.h>
#endif
#include <appl/diag/dpp/utils_line_TEVB.h>
#include <appl/diag/dpp/utils_periodic_suspend.h>
#include <appl/diag/dpp/utils_front_end_host_card.h>
#include <appl/diag/dcmn/utils_board_general.h>
#include <appl/diag/dpp/utils_version.h>
#include <appl/diag/dpp/utils_generic_test.h>
#include <appl/diag/dpp/utils_dffs_cpu_mez.h>
#include <appl/diag/dpp/utils_petra_test_chip_loader.h>

#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/dune_chips.h>

#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_general_utils.h>
#include <appl/dpp/UserInterface/ui_cli_files.h>

#include <appl/diag/dpp/dune_rpc.h>
#include <appl/diag/dpp/dune_rpc_auto_network_learning.h>
#include <appl/diag/dpp/dune_remote_access.h>

#include <FMC/fmc_common_agent.h>
#include <FMC/fmc_transport_layer_dcl.h>

#include <DCL/dcl_transport_layer_intern.h>
#include <DCL/dcl_transport_layer_extern.h>


#if LINK_FAP20V_LIBRARIES
  #include <sweep/fap20v/sweep_fap20v_app.h>
#endif
#include <sweep/Timna/sweep_timna_app.h>


#include <Serdes/serdes_config.h>
#include <Regression/rgr_engine.h>
#include <Regression/rgr_tests_pool.h>

unsigned int Ui_printing_policy_short = TRUE;

void
  ui_printing_policy_set(
    unsigned int short_prints
  )
{
  Ui_printing_policy_short = short_prints;
}

unsigned int
  ui_printing_policy_get(
    void
  )
{
  return Ui_printing_policy_short;
}

/*
 * A few small utilities for checking on bit set and for
 * counting number of bits in an array of long words.
 * The logic of ordering is as follows:
 * BIT(n) on array_of_longs[0] on is called no. 'n'
 * BIT(n) on array_of_longs[1] on is called no. 'n + 32'
 * And so on...
 * Bit(0) is the LS bit.
 * {
 */
unsigned
  int
    get_num_bits_set(
      unsigned long *array_of_longs,
      unsigned int  num_longs_in_array
    )
{
  unsigned
    int
      ii,
      jj,
      ret ;
  ret = 0 ;
  for (ii = 0 ; ii < num_longs_in_array ; ii++, array_of_longs++)
  {
    for (jj = 0 ; jj < BITS_IN_LONG ; jj++)
    {
      if (array_of_longs[0] & BIT(jj))
      {
        ret++ ;
      }
    }
  }
  return (ret) ;
}

unsigned
  int
    is_bit_set(
      unsigned long *array_of_longs,
      unsigned int  bit_to_check
    )
{
  unsigned
    int
      long_index,
      ret ;
  ret = 0 ;
  long_index = bit_to_check / BITS_IN_LONG ;
  bit_to_check -= (long_index * BITS_IN_LONG) ;
  if (array_of_longs[long_index] & BIT(bit_to_check))
  {
    ret = 1 ;
  }
  return (ret) ;
}

int
  time_from_startup_to_str(
    unsigned int time_10_msec,
    char *time_str
  )
{
  int
    ui_ret=0;
  unsigned long
    time_hour,
    time_sec,
    time_msec;

  time_sec = time_10_msec / 100 ;
  time_msec = (time_10_msec - (100 * time_sec)) ;
  if (time_sec >= 3600)
  {
    time_hour = time_sec / 3600 ;
    time_sec = time_sec - (time_hour * 3600) ;
  }
  else
  {
    time_hour = 0 ;
  }
  if (time_hour == 0)
  {
    sal_sprintf(time_str,
      "Time from startup = %lu.%03u seconds",
      (unsigned long)time_sec,
      (unsigned short)time_msec) ;
  }
  else
  {
    sal_sprintf(time_str,
      "Time from startup = %lu hours, %lu seconds",
      (unsigned long)time_hour,
      (unsigned long)time_sec
    );
  }
  return ui_ret;
}
/*****************************************************
*NAME
*  display_time_from_startup
*TYPE: PROC
*DATE: 24/JUNE/2002
*FUNCTION:
*  Print time from startup on screen.
*CALLING SEQUENCE:
*  display_time_from_startup(show)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int show -
*      Indicator of the mode of display:
*        METHOD_REGULAR_DISPLAY -
*          Display one reading and return control to
*          the user.
*        METHOD_PERIODIC_DISPLAY -
*          Display readings continuously (on one line)
*          and return control to the user when escape
*          key is detected.
*  SOC_SAND_INDIRECT:
*    None
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  If input is not METHOD_PERIODIC_DISPLAY, it is
*  assumed to be METHOD_REGULAR_DISPLAY.
*SEE ALSO:
 */
int
  display_time_from_startup(
    unsigned int show
  )
{
  int
    ui_ret ;
  unsigned
    long
      current_time ;
  char
    temp_string[MAX_CHARS_ON_LINE + 1] ;
  char
    rotating_mark ;
  char
    *blank_line ;

  ui_ret = 0 ;
  blank_line = NULL;

  /*
   * Take screen semaphore. Since it may only be taken and
   * given by the same task, do not embed it in set_periodic_on().
   */
  take_screen_semaphore() ;
  if (show == METHOD_PERIODIC_DISPLAY)
  {
    blank_line = get_blank_line() ;
    set_periodic_on() ;
    send_string_to_screen(
          "\r\n\n"
          "To stop continuous display: Hit \'esc\' or \'Ctrl-t\'"
          "\r\n",
          FALSE) ;
  }
  send_array_to_screen("\r\n\n",3) ;
  temp_string[0] = ' ' ;
  temp_string[1] = ' ' ;
  temp_string[2] = ' ' ;
  while (TRUE)
  {
    /*
     * Note: fraction is assumed to be in units of 1/10000 degrees.
     */
    rotating_mark = get_rotating_stick() ;
    current_time = get_watchdog_time(TRUE) ;
    time_from_startup_to_str(
      current_time,
      &(temp_string[3])
    );
    if (show != METHOD_PERIODIC_DISPLAY)
    {
      send_string_to_screen(temp_string,TRUE) ;
      break ;
    }
    else
    {
      fill_blanks(temp_string,MAX_CHARS_ON_SCREEN_LINE,2) ;
      temp_string[0] = rotating_mark ;
      send_array_to_screen("\r",1) ;
      send_string_to_screen(blank_line,FALSE) ;
      send_array_to_screen("\r",1) ;
      send_string_to_screen(temp_string,FALSE) ;
      if (!is_periodic_on())
      {
        send_array_to_screen("\r\n",2) ;
        break ;
      }
      /*
       * Wait some time until the next display.
       */
      elapse_periodic() ;
    }
  }
  give_screen_semaphore() ;
  return (ui_ret) ;
}
/*****************************************************
*NAME
*  display_temperature
*TYPE: PROC
*DATE: 10/APR/2002
*FUNCTION:
*  Print temperature reading on screen.
*CALLING SEQUENCE:
*  display_temperature(index,show)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int index -
*      Identifier of Thermometer.
*        THERMOMETER_MEZANINE_ARR_LOCATION  1
*        THERMOMETER_FABCRIC_ARR_LOCATION   2
*    unsigned int show -
*      Indicator of the mode of display:
*        METHOD_REGULAR_DISPLAY -
*          Display one reading and return control to
*          the user.
*        METHOD_PERIODIC_DISPLAY -
*          Display readings continuously (on one line)
*          and return control to the user when escape
*          key is detected.
*  SOC_SAND_INDIRECT:
*    None
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  If input is not METHOD_PERIODIC_DISPLAY, it is
*  assumed to be METHOD_REGULAR_DISPLAY.
*SEE ALSO:
 */
int
  display_temperature(
    unsigned int index,
    unsigned int show
  )
{
  TEMPERATURE_STATUS *
    temperature_status;
  int
    ui_ret ;
  int
    temperature_int[3],
    temperature_fraction[3],
    temperature_i;
  char
    temp_string[MAX_CHARS_ON_LINE + 1],
    temp_time_string_1[MAX_CHARS_ON_LINE + 1] ,
    temp_time_string_2[MAX_CHARS_ON_LINE + 1] ;
  char
    sign[3];
  char
    rotating_mark ;
  char
    *blank_line ;

  rotating_mark ='|';
  blank_line = NULL;

  /*
   * Take screen semaphore. Since it may only be taken and
   * given by the same task, do not embed it in set_periodic_on().
   */
  take_screen_semaphore() ;
  if (show == METHOD_PERIODIC_DISPLAY)
  {
    blank_line = get_blank_line() ;
    set_periodic_on() ;
    send_string_to_screen(
          "\r\n\n"
          "To stop continuous display: Hit \'esc\' or \'Ctrl-t\'"
          "\r\n",
          FALSE) ;
   send_array_to_screen("\r\n\n",3) ;
  }
  ui_ret = 0 ;
  temp_string[0] = ' ' ;
  temp_string[1] = ' ' ;
  temp_string[2] = ' ' ;
  while (TRUE)
  {
    if (nv_is_bckg_temperatures_en_active())
    {
      /*
       * Background collection is active
       */
      if (is_temperature_valid())
      {
        /*
         * Note: fraction is assumed to be in units of 1/10000 degrees.
         */
        rotating_mark = get_rotating_stick() ;
        temperature_status
          = get_thermo_reading(index);
/*
        get_temperature_full_degrees(index, &temperature_int,&temperature_fraction) ;
*/
        temperature_int[0] =
          temperature_status->current.temperature_integral;
        temperature_fraction[0] =
          temperature_status->current.temperature_frac;
        temperature_int[1] =
          temperature_status->clearable_max.temperature_integral;
        temperature_fraction[1] =
          temperature_status->clearable_max.temperature_frac;
        temperature_int[2] =
          temperature_status->max_from_reset.temperature_integral;
        temperature_fraction[2] =
          temperature_status->max_from_reset.temperature_frac;
        for(temperature_i=0;temperature_i<3;temperature_i++)
        {
          if ((temperature_int[temperature_i]< 0)        ||
              (temperature_fraction[temperature_i] < 0)
             )
          {
            sign[temperature_i] = '-' ;
          }
          else
          {
            sign[temperature_i] = '+' ;
          }
          if (temperature_int[temperature_i] < 0)
          {
            temperature_int[temperature_i] = -temperature_int[temperature_i] ;
          }
          if (temperature_fraction[temperature_i] < 0)
          {
            temperature_fraction[temperature_i] = -temperature_fraction[temperature_i] ;
          }
        }
        if(show != METHOD_PERIODIC_DISPLAY)
        {

          time_from_startup_to_str(
            temperature_status->clearable_max.time_stamp,
            temp_time_string_1
          );
          time_from_startup_to_str(
            temperature_status->max_from_reset.time_stamp,
            temp_time_string_2
          );
          sal_sprintf(&temp_string[3],
               "Current Temperature                  = %c%d.%04d degrees Celsius.\n\r"
            "   Max Temperature since last report    = %c%d.%04d.\n\r"
            "      %s\n\r"
            "   Max Temperature since system startup = %c%d.%04d.\n\r"
            "      %s\n\r",
            sign[0],temperature_int[0],temperature_fraction[0],
            sign[1],temperature_int[1],temperature_fraction[1],
            temp_time_string_1,
            sign[2],temperature_int[2],temperature_fraction[2],
            temp_time_string_2
            ) ;
          clear_clearable_max_temperature(
            index
          );
        }
        else
        {
          sal_sprintf(&temp_string[3],
            "Temperature = %c%d.%04d degrees Celsius.",
            sign[0],temperature_int[0],temperature_fraction[0]) ;
        }

      }
      else
      {
        sal_sprintf(&temp_string[3],
          "No valid temperature reading available."
          ) ;
      }
    }
    else
    {
      /*
       * Background collection is NOT active
       */
      sal_sprintf(&temp_string[3],
        "Background collection is NOT active (consider changing NvRam value)."
        ) ;
    }
    if (show != METHOD_PERIODIC_DISPLAY)
    {
      send_string_to_screen(temp_string,TRUE) ;
      break ;
    }
    else
    {
      fill_blanks(temp_string,MAX_CHARS_ON_SCREEN_LINE,2) ;
      temp_string[0] = rotating_mark ;
      send_array_to_screen("\r",1) ;
      send_string_to_screen(temp_string,FALSE) ;
      if (!is_periodic_on())
      {
        send_array_to_screen("\r\n",2) ;
        break ;
      }
      /*
       * Wait some time until the next display.
       */
      elapse_periodic() ;
    }
  }
  give_screen_semaphore() ;
  return (ui_ret) ;
}
/*****************************************************
*NAME
*  long_print
*TYPE: PROC
*DATE: 04/APR/2003
*FUNCTION:
*  Print a large number of lines every 10 seconds.
*  Just an exercise to test screen semaphore.
*CALLING SEQUENCE:
*  long_print(show)
*INPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    Print text and scenario
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    Activated 'send_string_to_screen()'
*REMARKS:
*  Stop this task via 'general exercise print_task_off'.
*SEE ALSO:
 */
void
  long_print(
    void
  )
{
  unsigned
    int
      uj,
      ui ;
  char
    loc_text[81] ;
  unsigned
    long
      last_disp_time_in_10ms,
      current_disp_time_in_10ms ;
  while (TRUE)
  {
    for (uj = 0 ; uj < 3 ; uj++)
    {
      start_print_services() ;
      for (ui = 0 ; ui < 50 ; ui++)
      {
        sal_sprintf(loc_text,
          "Cycle %lu: Line no. %lu for testing long print activated by two tasks\r\n",
          (unsigned long)uj,(unsigned long)ui) ;
        send_string_to_screen(loc_text,FALSE) ;
      }
      end_print_services() ;
      /*
       * Now consume CPU doing nothing for 10 seconds
       */
      last_disp_time_in_10ms = get_watchdog_time(TRUE) ;
      while (TRUE)
      {
        current_disp_time_in_10ms = get_watchdog_time(TRUE) ;
        /*
         * Change display every 10000 ms.
         */
        while ((current_disp_time_in_10ms - last_disp_time_in_10ms) < 1000)
        {
          current_disp_time_in_10ms = get_watchdog_time(TRUE) ;
        }
        break ;
      }
    }
    start_print_services() ;
    display_temperature(THERMOMETER_MEZANINE_ARR_LOCATION,METHOD_PERIODIC_DISPLAY) ;
    end_print_services() ;
  }
  return ;
}
/*****************************************************
*NAME
*  display_trace_fifo
*TYPE: PROC
*DATE: 16/APR/2003
*FUNCTION:
*  Print contents of real-time-trace fifo on screen.
*CALLING SEQUENCE:
*  display_trace_fifo(clear_contents)
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int clear_contents -
*      Flag. If 'true' then this procedure takes out
*      of fifo (permanently) any read element.
*      Otherwise, contents of fifo remain untouched.
*  SOC_SAND_INDIRECT:
*    Trace fifo (Trace_fifo, see utils_clock.c)
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  This procedure is assumed to have been invoked while
*  trace fifo is locked and does not change.
*  It is assumed that no other procedure is reading
*  the trace fifo at this time.
*SEE ALSO:
 */
int
  display_trace_fifo(
    unsigned int clear_contents
  )
{
  int
    trace_event_id,
    empty,
    ui_ret ;
  unsigned
    long
      trace_params[NUM_TRACE_FIFO_PARAMETERS],
      delta_trace_event_time,
      last_trace_event_time,
      trace_event_time ;
  unsigned
    int
      num_elements,
      ui ;
  char
    event_full_text[TRACE_EVENT_TEXT_SIZE * 4],
    event_text[TRACE_EVENT_TEXT_SIZE],
    info_msg[80*7] ;
  ui_ret = 0 ;
  last_trace_event_time = 0;
  num_elements = get_num_elements_trace_fifo() ;
  sal_sprintf(info_msg,
    "Real time trace fifo\r\n"
    "====================\r\n"
    " No. of valid entries      : %u\r\n"
    " No. of fifo overflow cases: %u\r\n"
    " No. of load attempts\r\n"
    "   while fifo was loacked  : %u\r\n\n"
    " I |  Time    |  Delta   | ID |  Text\r\n"
    "   | (micro)  |  Time    |    |\r\n",
    num_elements,
    get_clear_num_overflows_trace_fifo(),
    get_clear_load_while_locked_trace_fifo()
    ) ;
  send_string_to_screen(info_msg,FALSE) ;
  for (ui = 0 ; ui < num_elements ; ui++)
  {
    if (clear_contents)
    {
      empty =
        get_next_from_trace_fifo(
              &trace_event_id,
                &trace_event_time,event_text,trace_params) ;
    }
    else
    {
      empty =
        get_direct_from_trace_fifo(
              ui,&trace_event_id,
                &trace_event_time,event_text,trace_params) ;
    }
    if (empty)
    {
      /*
       * If everything works properly then software should never
       * get here.
       */
      break ;
    }
    if (ui == 0)
    {
      last_trace_event_time = trace_event_time ;
    }
    delta_trace_event_time = trace_event_time - last_trace_event_time ;
    sal_sprintf(event_full_text,
      event_text,
      trace_params[0],
      trace_params[1],
      trace_params[2],
      trace_params[3]
    ) ;
    sal_sprintf(info_msg,
      "%03u|%010lu|%010lu|%04u|%s\r\n",
      (unsigned short)ui,
      trace_event_time,delta_trace_event_time,
      (unsigned short)trace_event_id,event_full_text
      ) ;
    send_string_to_screen(info_msg,FALSE) ;
    last_trace_event_time = trace_event_time ;
  }
  return (ui_ret) ;
}


STATIC void
  print_ref_deisgn_versions(
    void
  )
{
  char
    ascii_ver[2][SOC_SAND_VER_STRING_SIZE];
  char
    msg[80*10];

  /*
   * Application and Boot
   */
  version_to_ascii(ascii_ver[0],get_prog_ver(),1) ;
  version_to_ascii(ascii_ver[1],get_dune_boot_version(),1);

  sal_sprintf(msg,
    "\r\n\n"
    "DUNE:\r\n"
    "=====\r\n"
    "Dune application version : %s\r\n"
    "Dune boot version        : %s\r\n",
    ascii_ver[0],
    ascii_ver[1]
  ) ;
  send_string_to_screen(msg,FALSE) ;

  /*
   * Soc_sand Drivers
   */
  soc_get_sand_string_ver(ascii_ver[0]);

  sal_sprintf(
    msg,
    "\r\n\n"
    "SOC_SAND:\r\n"
    "=====\r\n"
    "SOC_SAND   Driver Version: %s\r\n",
    ascii_ver[0]
  ) ;
  send_string_to_screen(msg,FALSE) ;

}

/*****************************************************
*NAME
*  subject_general
*TYPE: PROC
*DATE: 10/APR/2002
*FUNCTION:
*  Process input line which has a 'general' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_general(current_line,current_line_ptr)
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
  subject_general(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  int
    err ;
  unsigned
    int
      ui,
      uj ;

  UI_MACROS_INIT_FUNCTION("subject_general");

  err = 0;
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'general').
     * This should be protected against by the calling environment.
     */
    send_string_to_screen(
      "\r\n\n"
      "*** There are no parameters on the line (just \'general\').\r\n"
      "    SW error.\r\n",TRUE) ;
    ui_ret = TRUE ;
    goto exit ;
  }
  else
  {
    /*
     * Enter if there are parameters on the line (not just 'general').
     */
    unsigned
      int
        match_index,
        num_handled ;
    num_handled = 0 ;
    if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_MULTI_MEM_ID,1))
    {
      /*
       * Subject: 'multi memory access' for general memory access
       * Enter if this is a 'general multi_mem' request.
       */
      unsigned
        long
          mul_data[MAX_REPEATED_PARAM_VAL],
          mul_address[MAX_REPEATED_PARAM_VAL],
          mul_format[MAX_REPEATED_PARAM_VAL],
          mul_repeat[MAX_REPEATED_PARAM_VAL] ;
      unsigned
        int
          is_read,
          is_write,
          num_addresses,
          num_formats,
          num_repeats,
          num_data,
          sum_repeats,
          sizeof_data_to_write,
          sizeof_data_to_retrieve,
          sizeof_data_item ;
      num_handled++ ;
      send_array_to_screen("\r\n",2) ;
      is_read = is_write = FALSE ;
      num_addresses =
        num_formats =
          num_repeats =
            num_data = 0 ;
      /*
       * The following 'for' MUST make at least one loop!
       */
      for (ui = 1 ; ui <= (sizeof(mul_address)/sizeof(mul_address[0])) ; ui++)
      {
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_MULTI_MEM_ADDRESS_ID,ui,
                &param_val,VAL_NUMERIC,err_msg)
            )
        {
          /*
           * If there are no more address values then quit loop.
           */
          num_addresses = ui - 1 ;
          break ;
        }
        mul_address[ui - 1] = param_val->value.ulong_value ;
      }
      if (num_addresses == 0)
      {
        /*
         * Not even one value of address could not be retrieved.
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** Not even one value of address could not be retrieved!"
            ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      /*
       * Just precaution...
       */
      if (num_addresses > (sizeof(mul_address)/sizeof(mul_address[0])))
      {
        /*
         * Too many address values have been retrieved.
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** Too many values of addresses (%u) have been retrieved!",
            (unsigned int)num_addresses) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      sum_repeats = 0 ;
      /*
       * The following 'for' MUST make at least one loop!
       */
      for (ui = 1 ; ui <= (sizeof(mul_repeat)/sizeof(mul_repeat[0])) ; ui++)
      {
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_MULTI_MEM_REPEAT_ID,ui,
                &param_val,VAL_NUMERIC,err_msg)
            )
        {
          /*
           * If there are no more repeat values then quit loop.
           */
          num_repeats = ui - 1 ;
          break ;
        }
        mul_repeat[ui - 1] = param_val->value.ulong_value ;
        sum_repeats += mul_repeat[ui - 1] ;
      }
      if (num_repeats == 0)
      {
        /*
         * Not even one value of repeat could not be retrieved.
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** Not even one value of repeat could not be retrieved!"
            ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      /*
       * Just precaution...
       */
      if (num_repeats > (sizeof(mul_repeat)/sizeof(mul_repeat[0])))
      {
        /*
         * Too many repeat values have been retrieved.
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** Too many values of repeats (%u) have been retrieved!",
            (unsigned int)num_repeats) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      /*
       * Calculate total number of data bytes required for storing
       * contents of memory to read.
       * This is also the number of data bytes which should be
       * sent to the other side as data to write.
       */
      sizeof_data_to_retrieve = 0 ;
      /*
       * The following 'for' MUST make at least one loop!
       */
      for (ui = 1 ; ui <= (sizeof(mul_format)/sizeof(mul_format[0])) ; ui++)
      {
        unsigned
          int
            current_format ;
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_MULTI_MEM_FORMAT_ID,ui,
                &param_val,VAL_NUMERIC,err_msg)
            )
        {
          /*
           * If there are no more format values then quit loop.
           */
          num_formats = ui - 1 ;
          break ;
        }
        current_format = mul_format[ui - 1] = param_val->value.ulong_value ;
        if (
            (current_format != CHAR_EQUIVALENT)  &&
            (current_format != SHORT_EQUIVALENT) &&
            (current_format != LONG_EQUIVALENT)
            )
        {
          /*
           * Not even one value of format could not be retrieved.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Illegal format value (%lu) at item no. %lu (starting from 0)!",
              (unsigned long)current_format,
              (unsigned long)ui
              ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        switch (current_format)
        {
          default:
          case CHAR_EQUIVALENT:
          {
            sizeof_data_item = sizeof(char) ;
            break ;
          }
          case SHORT_EQUIVALENT:
          {
            sizeof_data_item = sizeof(short) ;
            break ;
          }
          case LONG_EQUIVALENT:
          {
            sizeof_data_item = sizeof(long) ;
            break ;
          }
        }
        sizeof_data_to_retrieve += sizeof_data_item * mul_repeat[ui - 1] ;
      }
      sizeof_data_to_write = sizeof_data_to_retrieve ;
      if (num_formats == 0)
      {
        /*
         * Not even one value of format could not be retrieved.
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** Not even one value of format could not be retrieved!"
            ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      /*
       * Just precaution...
       */
      if (num_formats > (sizeof(mul_format)/sizeof(mul_format[0])))
      {
        /*
         * Too many format values have been retrieved.
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** Too many values of formats (%u) have been retrieved!",
            (unsigned int)num_formats) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      if (num_repeats != num_formats)
      {
        /*
         * Too many repeat values have been retrieved.
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** Num vals of repeats (%lu) not equal to num vals of formats (%lu)!",
            (unsigned long)num_repeats,
            (unsigned long)num_formats) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      if (num_repeats != num_addresses)
      {
        /*
         * Too many repeat values have been retrieved.
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "*** Num vals of repeats (%lu) not equal to num vals of addresses (%lu)!",
            (unsigned long)num_repeats,
            (unsigned long)num_addresses) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_MULTI_MEM_READ_ID,1))
      {
        is_read = TRUE ;
        /*
         * Now send to FMC on the other side.
         */
        {
          unsigned
            int
              buff_byte_size ;
          OTHER_SERVICES_IN_STRUCT
            *in_struct ;
          MEM_MULTI_READ_IN_STRUCT
            *mem_multi_read_in_struct ;
          FMC_COMMON_OUT_STRUCT
            *fmc_common_out_struct ;
          unsigned
            long
              recv_buff_size,
              dest_ip ;
          unsigned
            char
              *recv_buff,
              *buff_data;
          err = get_run_ip_addr(&dest_ip) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** get_run_ip_addr() failed to get IP address") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          buff_byte_size =
            sizeof(*in_struct) - sizeof(in_struct->data) +
            sizeof(in_struct->data.mem_multi_read_data) -
            sizeof(in_struct->data.mem_multi_read_data.read_request_item) +
            (sizeof(in_struct->
              data.mem_multi_read_data.read_request_item) * num_addresses) ;
          in_struct = (OTHER_SERVICES_IN_STRUCT *)malloc(buff_byte_size) ;
          if ((OTHER_SERVICES_IN_STRUCT *)0 == in_struct)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Failed to allocate buffer for in_struct") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          mem_multi_read_in_struct =
              &in_struct->data.mem_multi_read_data ;
          /*
           * Fill buffer to send so as to reduce its size to required
           * minimum
           */
          buff_data = (unsigned char *)in_struct ;
          /*
           * Fill input params:
           * Issue 'multi_mem read' command
           */
          in_struct->proc_id = OTHER_SERVICES_MULTI_READ_TYPE ;
          mem_multi_read_in_struct->
                  num_read_request_item_blocks = num_addresses ;
          for (ui = 0 ; ui < num_addresses ; ui++)
          {
            mem_multi_read_in_struct->
              read_request_item[ui].addr = mul_address[ui] ;
            mem_multi_read_in_struct->
              read_request_item[ui].repeat = (unsigned short) mul_repeat[ui] ;
            mem_multi_read_in_struct->
              read_request_item[ui].format = (unsigned char) mul_format[ui] ;
          }
          /*
           * This is the calculated number of bytes in the
           * 'retrieved data' section.
           */
          recv_buff_size = sizeof_data_to_retrieve ;
          recv_buff_size +=
            (sizeof(FMC_COMMON_OUT_STRUCT) -
              SIZEOF(FMC_COMMON_OUT_STRUCT,common_union) +
              SIZEOF(FMC_COMMON_UNION,
                other_services_out_union.mem_multi_read_out_struct) -
              SIZEOF(FMC_COMMON_UNION,
                other_services_out_union.
                  mem_multi_read_out_struct.read_response_item) +
              (SIZEOF(FMC_COMMON_UNION,
                other_services_out_union.
                  mem_multi_read_out_struct.read_response_item) * num_addresses)
              ) ;
          recv_buff = (unsigned char *)malloc(recv_buff_size) ;
          if ((unsigned char *)0 == recv_buff)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Failed to allocate buffer for fmc_send_buffer_to_slave_dcl() reply") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            if (in_struct)
            {
              free (in_struct) ;
            }
            goto exit ;
          }
          fmc_common_out_struct = (FMC_COMMON_OUT_STRUCT *)recv_buff ;
          /*
           * Just to make it easy to see changes. Not required otherwise.
           */
          memset(recv_buff,0,recv_buff_size) ;
          {
            /*
             * This replaces sendBuffToCore() which requires setup of dune_rpc
             * devices.
             */
            /*
             * This sending action is met, on receiver side, by the following
             * stack:
             *   dcl_rx_unexpected_task()
             *   handle_rx_dcl_device_msg()
             *   fmc_transport_receive_buffer_from_manager()
             * Note:
             * Device_id is not meaningful here but it has to have
             * either BIT(30) or BIT(31) set since that is how devices
             * are identified on the 'old fmc' system.
             */
            end_print_services() ;
            err =
              fmc_send_buffer_to_slave_dcl(
                BIT(31) | BIT(30),
                TRUE,dest_ip,FALSE,
                buff_byte_size,buff_data,
                recv_buff_size,recv_buff,
                NULL
              ) ;
            start_print_services() ;
            free (in_struct) ;
            if (err)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** sendBuffToCore failed to send command to remote unit") ;
              send_string_to_screen(err_msg,TRUE) ;
              ui_ret = TRUE ;
              free (recv_buff) ;
              goto exit ;
            }
          }
          {
            unsigned
              int
                ii,
                too_small_response_buff_size,
                num_read_response_item_blocks ;
            char
              *format_text,
              *data_ptr ;
            READ_RESPONSE_ITEM
              *read_response_item ;
            MEM_MULTI_READ_OUT_STRUCT
              *mem_multi_read_out_struct ;
            mem_multi_read_out_struct =
              &(fmc_common_out_struct->
                  common_union.
                  other_services_out_union.
                  mem_multi_read_out_struct) ;
            too_small_response_buff_size =
              (unsigned int)
                 (mem_multi_read_out_struct->
                      too_small_response_buff_size) ;
            if (too_small_response_buff_size)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Target reports that response buff size (%lu) is too small",
                  (unsigned long)recv_buff_size) ;
              send_string_to_screen(err_msg,TRUE) ;
              ui_ret = TRUE ;
              free (recv_buff) ;
              goto exit ;
            }
            num_read_response_item_blocks =
              (unsigned int)
                 (mem_multi_read_out_struct->
                      num_read_response_item_blocks) ;
            sal_sprintf(err_msg,
              "\r\n"
              "Display received buffer:") ;
            send_string_to_screen(err_msg,TRUE) ;
            sal_sprintf(err_msg,
              "out_struct_type               = %lu\r\n"
              "num_read_response_item_blocks = %lu\r\n",
              (unsigned long)fmc_common_out_struct->out_struct_type,
              (unsigned long)
                mem_multi_read_out_struct->num_read_response_item_blocks
              ) ;
            send_string_to_screen(err_msg,TRUE) ;
            read_response_item =
               &(mem_multi_read_out_struct->read_response_item[0]) ;
            if (num_read_response_item_blocks)
            {
              for (ii = 0 ;
                      ii < num_read_response_item_blocks ; ii++)
              {
                switch (read_response_item->format)
                {
                  default:
                  case CHAR_EQUIVALENT:
                  {
                    format_text = "char" ;
                    break ;
                  }
                  case SHORT_EQUIVALENT:
                  {
                    format_text = "short" ;
                    break ;
                  }
                  case LONG_EQUIVALENT:
                  {
                    format_text = "long" ;
                    break ;
                  }
                }
                sal_sprintf(err_msg,
                  "Block no. %lu:\r\n"
                  "  Address = 0x%08lX\r\n"
                  "  format  = %s\r\n"
                  "  repeat  = %lu",
                  (unsigned long)ii,
                  (unsigned long)read_response_item->addr,
                  format_text,
                  (unsigned long)read_response_item->repeat
                  ) ;
                send_string_to_screen(err_msg,FALSE) ;
                /*
                 * The data itself.
                 */
                data_ptr = (char *)&(read_response_item->data[0]) ;
                display_memory(
                  data_ptr,
                  (unsigned int)read_response_item->repeat,
                  (unsigned int)read_response_item->format,
                  (unsigned int)HEX_EQUIVALENT,
                  TRUE,0,0) ;
                data_ptr +=
                  (read_response_item->repeat * read_response_item->format) ;
                read_response_item = (READ_RESPONSE_ITEM *)data_ptr ;
              }
            }
            else
            {
              sal_sprintf(err_msg,
                "NO \'data\' data has been attached to this message!!!\r\n"
                ) ;
              send_string_to_screen(err_msg,FALSE) ;
            }
          }
          free (recv_buff) ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_MULTI_MEM_WRITE_ID,1))
      {
        /*
         * This is a 'write' command
         * Enter if this is a 'general multimem write ...' request.
         */
        is_write = TRUE ;
        /*
         * The following 'for' MUST make at least one loop!
         */
        for (ui = 1 ; ui <= (sizeof(mul_data)/sizeof(mul_data[0])) ; ui++)
        {
          if (get_val_of(
                  current_line,(int *)&match_index,PARAM_GENERAL_MULTI_MEM_DATA_ID,ui,
                  &param_val,VAL_NUMERIC,err_msg)
              )
          {
            /*
             * If there are no more data values then quit loop.
             */
            num_data = ui - 1 ;
            break ;
          }
          mul_data[ui - 1] = param_val->value.ulong_value ;
        }
        if (num_data == 0)
        {
          /*
           * Not even one value of data could not be retrieved.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Not even one value of data could not be retrieved!"
              ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        /*
         * Just precaution...
         */
        if (num_data > (sizeof(mul_data)/sizeof(mul_data[0])))
        {
          /*
           * Too many data values have been retrieved.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Too many values of data (%u) have been retrieved!",
              (unsigned int)num_data) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        if (num_data != sum_repeats)
        {
          /*
           * Number of data items should be equal to number of repetitions.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Num values of data (%lu) not equal to sum of repeats (%lu)!",
              (unsigned long)num_data,
              (unsigned long)sum_repeats
              ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        /*
         * Now send to FMC on the other side.
         */
        {
          unsigned
            int
              data_byte_size,
              buff_byte_size ;
          OTHER_SERVICES_IN_STRUCT
            *in_struct ;
          MEM_MULTI_WRITE_IN_STRUCT
            *mem_multi_write_in_struct ;
          WRITE_REQUEST_ITEM
            *write_request_item ;
          FMC_COMMON_OUT_STRUCT
            *fmc_common_out_struct ;
          unsigned
            long
              recv_buff_size,
              dest_ip ;
          unsigned
            char
              *recv_buff,
              *buff_data ;
          err = get_run_ip_addr(&dest_ip) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** get_run_ip_addr() failed to get IP address") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          /*
           * Calculate number of bytes in all 'data' sections
           * together
           */
          data_byte_size = sizeof_data_to_write ;
          buff_byte_size =
            data_byte_size +
            sizeof(*in_struct) - sizeof(in_struct->data) +
            sizeof(in_struct->data.mem_multi_write_data) -
            sizeof(in_struct->data.mem_multi_write_data.write_request_item) +
            ((sizeof(in_struct->
              data.mem_multi_write_data.write_request_item[0]) -
                sizeof(in_struct->data.
                  mem_multi_write_data.write_request_item[0].data)) * num_addresses) ;
          in_struct = (OTHER_SERVICES_IN_STRUCT *)malloc(buff_byte_size) ;
          if ((OTHER_SERVICES_IN_STRUCT *)0 == in_struct)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Failed to allocate buffer for in_struct") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          mem_multi_write_in_struct =
              &in_struct->data.mem_multi_write_data ;
          buff_data = (unsigned char *)in_struct ;
          {
            /*
             * Fill input params:
             */
            unsigned
              int
                data_index ;
            unsigned
              char
                *char_ptr ;
            unsigned
              short
                *short_ptr ;
            unsigned
              long
                *long_ptr ;
            in_struct->proc_id = OTHER_SERVICES_MULTI_WRITE_TYPE ;
            mem_multi_write_in_struct->
                    num_write_request_item_blocks = num_addresses ;
            write_request_item =
              &(mem_multi_write_in_struct->write_request_item[0]) ;
            /*
             * Index into mul_data[]. Requireds since mul_data
             * is dynamically assigned to the various blocks.
             */
            data_index = 0 ;
            for (ui = 0 ; ui < num_addresses ; ui++)
            {
              write_request_item->addr = mul_address[ui] ;
              write_request_item->repeat = (unsigned short) mul_repeat[ui] ;
              write_request_item->format = (unsigned char) mul_format[ui] ;
              switch (write_request_item->format)
              {
                default:
                case CHAR_EQUIVALENT:
                {
                  char_ptr =
                    (unsigned char *)&(write_request_item->data[0]) ;
                  for (uj = 0 ; uj < write_request_item->repeat ; uj++)
                  {
                    *char_ptr++ = (unsigned char)mul_data[data_index++] ;
                  }
                  write_request_item = (WRITE_REQUEST_ITEM *)char_ptr ;
                  break ;
                }
                case SHORT_EQUIVALENT:
                {
                  short_ptr =
                    (unsigned short *)&(write_request_item->data[0]) ;
                  for (uj = 0 ; uj < write_request_item->repeat ; uj++)
                  {
                    *short_ptr++ = (unsigned short)mul_data[data_index++] ;
                  }
                  write_request_item = (WRITE_REQUEST_ITEM *)short_ptr ;
                  break ;
                }
                case LONG_EQUIVALENT:
                {
                  long_ptr =
                    (unsigned long *)&(write_request_item->data[0]) ;
                  for (uj = 0 ; uj < write_request_item->repeat ; uj++)
                  {
                    *long_ptr++ = (unsigned long)mul_data[data_index++] ;
                  }
                  write_request_item = (WRITE_REQUEST_ITEM *)long_ptr ;
                  break ;
                }
              }
            }
          }
          /*
           * Calculat number of bytes in response.
           */
          recv_buff_size =
            (sizeof(FMC_COMMON_OUT_STRUCT) -
              SIZEOF(FMC_COMMON_OUT_STRUCT,common_union) +
              SIZEOF(FMC_COMMON_UNION,
                other_services_out_union.mem_multi_write_out_struct)
              ) ;
          recv_buff = (unsigned char *)malloc(recv_buff_size) ;
          if ((unsigned char *)0 == recv_buff)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Failed to allocate buffer for fmc_send_buffer_to_slave_dcl() reply") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            if (in_struct)
            {
              free (in_struct) ;
            }
            goto exit ;
          }
          fmc_common_out_struct = (FMC_COMMON_OUT_STRUCT *)recv_buff ;
          /*
           * Just to make it easy to see changes. Not required otherwise.
           */
          memset(recv_buff,0,recv_buff_size) ;
          {
            /*
             * Send data buffer to other side.
             */
            /*
             * This replaces sendBuffToCore() which requires setup of dune_rpc
             * devices.
             */
            /*
             * This sending action is met, on receiver side, by the following
             * stack:
             *   dcl_rx_unexpected_task()
             *   handle_rx_dcl_device_msg()
             *   fmc_transport_receive_buffer_from_manager()
             * Note:
             * Device_id is not meaningful here but it has to have
             * either BIT(30) or BIT(31) set since that is how devices
             * are identified on the 'old fmc' system.
             */
            end_print_services() ;
            err =
              fmc_send_buffer_to_slave_dcl(
                BIT(31) | BIT(30),
                TRUE,dest_ip,FALSE,
                buff_byte_size,buff_data,
                recv_buff_size,recv_buff,
                NULL
              ) ;
            start_print_services() ;
            free (in_struct) ;
            if (err)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** sendBuffToCore failed to send command to remote unit") ;
              send_string_to_screen(err_msg,TRUE) ;
              ui_ret = TRUE ;
              free (recv_buff) ;
              goto exit ;
            }
          }
          {
            unsigned
              int
                general_error,
                num_bytes_written ;
            MEM_MULTI_WRITE_OUT_STRUCT
              *mem_multi_write_out_struct ;
            mem_multi_write_out_struct =
              &(fmc_common_out_struct->
                  common_union.
                  other_services_out_union.
                  mem_multi_write_out_struct) ;
            general_error =
              (unsigned int)
                 (mem_multi_write_out_struct->
                      general_error) ;
            if (general_error)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Target reports with general error (%lu)",
                  (unsigned long)general_error) ;
              send_string_to_screen(err_msg,TRUE) ;
              ui_ret = TRUE ;
              free (recv_buff) ;
              goto exit ;
            }
            num_bytes_written =
              (unsigned int)
                 (mem_multi_write_out_struct->
                      num_bytes_written) ;
            sal_sprintf(err_msg,
              "\r\n"
              "Display received buffer:") ;
            send_string_to_screen(err_msg,TRUE) ;
            sal_sprintf(err_msg,
              "out_struct_type    = %lu\r\n"
              "num_bytes_written  = %lu\r\n",
              (unsigned long)fmc_common_out_struct->out_struct_type,
              (unsigned long)num_bytes_written
              ) ;
            send_string_to_screen(err_msg,TRUE) ;
            if (num_bytes_written == 0)
            {
              sal_sprintf(err_msg,
                "NO \'data\' data has been written on target!!!\r\n"
                ) ;
              send_string_to_screen(err_msg,FALSE) ;
            }
          }
          free (recv_buff) ;
        }
      }
      else
      {
        /*
         * Illegal 'action': Not 'read' and not 'write'. SW error.
         */
        send_string_to_screen(
            "\r\n\n"
          "*** Illegal \'multi mem action\' - Not \'read\' nor \'write\'. SW error.\r\n",
          TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_DCL_ID,1))
    {
      /*
       * Subject: 'DCL' for various DCL operations
       * Enter if this is a 'general dcl' request.
       */
      unsigned
        long
          dcl_host_ip,
          dest_host ;
      unsigned
        short
          first_file,
          last_file ;
      int
        fail_on_timeout ;
      unsigned
        char
          action ;
      num_handled++ ;
      first_file = last_file = 0 ;
      send_array_to_screen("\r\n",2) ;
      if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_HOST_FILE_ID,1))
      {
        /*
         * This is a 'host_file' command
         * Enter if this is a 'general dcl host_file' request.
         */
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_HOST_FILE_HOST_IP_ID,1,
                &param_val,VAL_IP,err_msg))
        {
          /*
           * 'host_ip' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'host_ip\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        dcl_host_ip = (unsigned long)param_val->value.ip_value ;
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_HOST_FILE_DEST_HOST_ID,1,
                &param_val,VAL_IP,err_msg))
        {
          /*
           * 'dest_host' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'host_ip\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        dest_host = (unsigned long)param_val->value.ip_value ;
        if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_HOST_FILE_SET_ID,1))
        {
          if (get_val_of(
                  current_line,(int *)&match_index,
                  PARAM_GENERAL_HOST_FILE_FIRST_FILE_ID,1,
                  &param_val,VAL_NUMERIC,err_msg))
          {
            /*
             * 'first_file' - could not be retrieved.
             */
            send_string_to_screen(
                "\r\n\n"
                "*** Procedure \'get_val_of\' returned with error\r\n"
                "    (regarding \'dcl host_file set ... first_file\'):\r\n",
                TRUE) ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          first_file = (unsigned short)param_val->value.ulong_value ;
          if (get_val_of(
                  current_line,(int *)&match_index,
                  PARAM_GENERAL_HOST_FILE_LAST_FILE_ID,1,
                  &param_val,VAL_NUMERIC,err_msg))
          {
            /*
             * 'last_file' - could not be retrieved.
             */
            send_string_to_screen(
                "\r\n\n"
                "*** Procedure \'get_val_of\' returned with error\r\n"
                "    (regarding \'dcl host_file set ... last_file\'):\r\n",
                TRUE) ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          last_file = (unsigned short)param_val->value.ulong_value ;
          /*
           * Mark it as a 'set' option.
           */
          action = TRUE ;
        }
        else if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_HOST_FILE_DELETE_ID,1))
        {
          /*
           * Mark it as a 'delete' option.
           */
          action = FALSE ;
        }
        else
        {
          /*
           * Illegal dcl host_file option: Not 'set' and not 'delete'. SW error.
           */
          send_string_to_screen(
              "\r\n\n"
            "*** Illegal DCL \'host_file\' option - Not \'set\' nor \'delete\'. SW error.\r\n",
            TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        {
          /*
           * Now send DCL message to instruct other station on host_file
           * configuration.
           */
          int
            action_failed ;
          DCL_UNIT_HOST_FILE_RESP
            dcl_unit_host_file_resp ;
          err =
            dcl_send_host_file(
              dcl_host_ip,
              &fail_on_timeout,
              dest_host,
              first_file,
              last_file,
              action,
              &action_failed,
              &dcl_unit_host_file_resp) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** dcl_send_host_file() failed to send DCL message") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          if (fail_on_timeout)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** dcl_send_host_file() failed on timeout") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          if (action_failed)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** dcl_send_host_file() failed to carry out operation:\r\n\n") ;
            send_string_to_screen(err_msg,FALSE) ;
            sal_sprintf(err_msg,
                " not_available       : %lu\r\n"
                " get_free_error_id   : %lu\r\n"
                " load_new_error_id   : %lu\r\n"
                " is_host_error_id    : %lu\r\n"
                " delete_host_error_id: %lu\r\n",
                dcl_unit_host_file_resp.not_available,
                dcl_unit_host_file_resp.get_free_error_id ,
                dcl_unit_host_file_resp.load_new_error_id,
                dcl_unit_host_file_resp.is_host_error_id,
                dcl_unit_host_file_resp.delete_host_error_id
                ) ;
            send_string_to_screen(err_msg,TRUE) ;
            goto exit ;
          }
          sal_sprintf(err_msg,
                "\r\n\n"
                "==> dcl_send_host_file(): Operation has been carried out SUCCESSFULLY!"
                "\r\n\n") ;
          send_string_to_screen(err_msg,FALSE) ;
        }
      }
      else
      {
        /*
         * Illegal dcl option: Not 'host_file'. SW error.
         */
        send_string_to_screen(
            "\r\n\n"
          "*** Illegal \'DCL option\' - Not \'host_file\'. SW error.\r\n",
          TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_FPGA_RATE_ID,1))
    {
      /*
       * Subject: 'Rate collection' for Fpga Test Generator (FTG)
       * Enter if this is a 'general fpga_rate' request.
       */
      unsigned
        long
          fpga_report_rate,
          fpga_collection_rate_ms,
          fpga_host_ip,
          mask_of_streams[1],
          mask_of_ports[2] ;
      unsigned
        int
          fpga_continuous_mode,
          fpga_identifier ;
      char
        *fpga_identifier_str ;
      num_handled++ ;
      send_array_to_screen("\r\n",2) ;
      if (get_val_of(
              current_line,(int *)&match_index,
              PARAM_GENERAL_FPGA_RATE_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * 'Identifier of FPGA' parameter could not be retrieved.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error\r\n"
            "    (regarding \'fpga_rate\'):\r\n",
            TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      fpga_identifier = (unsigned int)param_val->numeric_equivalent ;
      switch (fpga_identifier)
      {
        case (FPGA_A_IDENTIFIER):
        {
          fpga_identifier = FPGA_A ;
          fpga_identifier_str = "FPGA_A" ;
          break ;
        }
        case (FPGA_B_IDENTIFIER):
        {
          fpga_identifier = FPGA_B ;
          fpga_identifier_str = "FPGA_B" ;
          break ;
        }
        default:
        {
          /*
           * Illegal 'FPGA identifier'. SW error.
           */
          send_string_to_screen(
              "\r\n\n"
            "*** Illegal \'FPGA identifier\'. SW error.\r\n",
            TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
          break ;
        }
      }
      if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_FPGA_RATE_GET_CURRENT_ID,1))
      {
        /*
         * This is a 'get_current' command
         * Enter if this is a 'general fpga_rate fpga_* get_current' request.
         */
        {
          unsigned
            int
              buff_byte_size ;
          OTHER_SERVICES_IN_STRUCT
            in_struct ;
          RATE_FTG_GET_CURRENT_IN_STRUCT
            *rate_ftg_get_current_data ;
          FMC_COMMON_OUT_STRUCT
            *fmc_common_out_struct ;
          unsigned
            long
              recv_buff_size,
              dest_ip ;
          unsigned
            char
              *recv_buff,
              *buff_data ;
          err = get_run_ip_addr(&dest_ip) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** get_run_ip_addr() failed to get IP address") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          rate_ftg_get_current_data =
              &in_struct.data.rate_ftg_get_current_data ;
          /*
           * Fill buffer to send so as to reduce its size to required
           * minimum
           */
          buff_data = (unsigned char *)&in_struct ;
          buff_byte_size = sizeof(in_struct) ;
          if (fill_water_marks(buff_data,buff_byte_size))
          {
            send_string_to_screen(
              "\r\n\n"
              "*** SW error in fill_water_marks().\r\n",TRUE)  ;
            ui_ret = TRUE ;
            goto exit ;
          }
          /*
           * Fill input params
           */
          /*
           * Issue 'get current' command
           */
          in_struct.proc_id = OTHER_SERVICES_FTG_GET_CURRENT_TYPE ;
          rate_ftg_get_current_data->fpga_id = fpga_identifier ;
          /*
           * This is the maximal size of the 'data' section.
           */
          recv_buff_size =
            NUM_BLOCKS_IN_LOGICAL_BUFF * sizeof(LOGICAL_RATE_BLOCK) ;
          recv_buff_size +=
            (sizeof(FMC_COMMON_OUT_STRUCT) -
              SIZEOF(FMC_COMMON_OUT_STRUCT,common_union) +
              SIZEOF(FMC_COMMON_UNION,
                      other_services_out_union.rate_ftg_get_current_out_struct) -
              SIZEOF(FMC_COMMON_UNION,
                      other_services_out_union.rate_ftg_get_current_out_struct.data)
              ) ;
          recv_buff = (unsigned char *)malloc(recv_buff_size) ;
          if ((unsigned char *)0 == recv_buff)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Failed to allocate buffer for fmc_send_buffer_to_slave_dcl() reply") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          fmc_common_out_struct = (FMC_COMMON_OUT_STRUCT *)recv_buff ;
          /*
           * Just to make it easy to see changed. Not required otherwise.
           */
          memset(recv_buff,0,recv_buff_size) ;
          {
            /*
             * This replaces sendBuffToCore() which requires setup of dune_rpc
             * devices.
             */
            unsigned
              long
                estimated_size ;
            estimated_size = buff_byte_size + 10 ;
            if (
                  !get_water_size(buff_data,buff_byte_size,&estimated_size) &&
                  estimated_size < buff_byte_size
               )
            {
              buff_byte_size = estimated_size ;
            }
            /*
             * This sending action is met, on receiver side, by the following
             * stack:
             *   dcl_rx_unexpected_task()
             *   handle_rx_dcl_device_msg()
             *   fmc_transport_receive_buffer_from_manager()
             * Note:
             * Device_id is not meaningful here but it has to have
             * either BIT(30) or BIT(31) set since that is how devices
             * are identified on the 'old fmc' system.
             */
            end_print_services() ;
            err =
              fmc_send_buffer_to_slave_dcl(
                BIT(31) | BIT(30),
                TRUE,dest_ip,FALSE,
                buff_byte_size,buff_data,
                recv_buff_size,recv_buff,
                NULL
              ) ;
            start_print_services() ;
            if (err)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** sendBuffToCore failed to send command to remote unit") ;
              send_string_to_screen(err_msg,TRUE) ;
              ui_ret = TRUE ;
              free (recv_buff) ;
              goto exit ;
            }
          }
          {
            unsigned
              int
                tot_num_streams,
                tot_num_ports,
                num_streams_in_mask,
                num_ports_in_mask,
                num_elements,
                display_index,
                element_id,
                num_longs,
                long_id,
                str_len,
                block_id,
                num_blocks ;
            char
              logical_buff[80 * 24] ;
            LOGICAL_RATE_BLOCK
              *logical_rate_block ;
            RATE_FTG_GET_CURRENT_OUT_STRUCT
              *rate_ftg_get_current_out_struct ;
            rate_ftg_get_current_out_struct =
              &(fmc_common_out_struct->
                  common_union.
                  other_services_out_union.
                  rate_ftg_get_current_out_struct) ;
            num_blocks =
              (unsigned int)
                 (rate_ftg_get_current_out_struct->num_blocks) ;
            logical_rate_block =
              (LOGICAL_RATE_BLOCK *)
                 &(rate_ftg_get_current_out_struct->data[0]) ;
            sal_sprintf(err_msg,
              "\r\n"
              "Display received buffer:") ;
            send_string_to_screen(err_msg,TRUE) ;
            memcpy(
              mask_of_streams,
              rate_ftg_get_current_out_struct->mask_of_streams,
              sizeof(mask_of_streams)) ;
            memcpy(
              mask_of_ports,
              rate_ftg_get_current_out_struct->mask_of_ports,
              sizeof(mask_of_ports)) ;
            tot_num_streams =
                sizeof(logical_rate_block->stream_rate_table) /
                  sizeof(logical_rate_block->stream_rate_table[0]) ;
            tot_num_ports =
              sizeof(logical_rate_block->port_rate_table) /
                sizeof(logical_rate_block->port_rate_table[0]) ;
            sal_sprintf(err_msg,
              "out_struct_type             = %lu\r\n"
              "time_stamp                  = %lu\r\n"
              "fpga_id                     = %lu\r\n"
              "rate_collection_not_active  = %lu\r\n"
              "get_current_error_id        = %lu\r\n"
              "reply_buff_too_small        = %lu\r\n"
              "is_started_error            = %lu\r\n"
              "get_ovfl_error              = %lu\r\n"
              "total_logic_ovfl_count      = %lu\r\n"
              "self_clear_logic_overflow   = %lu\r\n"
              "total_phys_ovfl_count       = %lu\r\n"
              "self_clear_phys_overflow    = %lu\r\n"
              "fpga_clock                  = %lu\r\n"
              "max_num_blocks              = %lu\r\n"
              "num_blocks                  = %lu\r\n"
              "mask_of_streams             = 0x%08lX\r\n"
              "mask_of_ports               = 0x%08lX,0x%08lX\r\n",
              (unsigned long)fmc_common_out_struct->out_struct_type,
              fmc_common_out_struct->time_stamp,
              rate_ftg_get_current_out_struct->fpga_id,
              rate_ftg_get_current_out_struct->rate_collection_not_active,
              rate_ftg_get_current_out_struct->get_current_error_id,
              rate_ftg_get_current_out_struct->reply_buff_too_small,
              rate_ftg_get_current_out_struct->is_started_error,
              rate_ftg_get_current_out_struct->get_ovfl_error,
              rate_ftg_get_current_out_struct->total_logic_ovfl_count,
              rate_ftg_get_current_out_struct->self_clear_logic_overflow,
              rate_ftg_get_current_out_struct->total_phys_ovfl_count,
              rate_ftg_get_current_out_struct->self_clear_phys_overflow,
              rate_ftg_get_current_out_struct->fpga_clock,
              rate_ftg_get_current_out_struct->max_num_blocks,
              (unsigned long)num_blocks,
              mask_of_streams[0],
              mask_of_ports[0],
              mask_of_ports[1]
              ) ;
            send_string_to_screen(err_msg,TRUE) ;
            num_streams_in_mask =
              get_num_bits_set(
                mask_of_streams,
                sizeof(mask_of_streams) / sizeof(mask_of_streams[0])
                ) ;
            num_ports_in_mask =
              get_num_bits_set(
                mask_of_ports,
                sizeof(mask_of_ports) / sizeof(mask_of_ports[0])
                ) ;
            if (num_blocks)
            {
              for (block_id = 0 ;
                      block_id < num_blocks ; block_id++)
              {
                STREAM_RATE_COM_UNIT
                  *stream_rate_com_unit ;
                PORT_RATE_COM_UNIT
                  *port_rate_com_unit ;
                sal_sprintf(err_msg,
                  "Block no. %lu:\r\n"
                  "  FPGA buffer                       = %lu\r\n"
                  "  Time stamp                        = %lu (micro)\r\n"
                  "  num_elements_in_stream_rate_table = %lu\r\n"
                  "  num_elements_in_port_rate_table   = %lu\r\n",
                  (unsigned long)block_id,
                  (unsigned long)logical_rate_block->src_buffer,
                  (unsigned long)logical_rate_block->collect_time,
                  (unsigned long)logical_rate_block->
                            num_elements_in_stream_rate_table,
                  (unsigned long)logical_rate_block->
                            num_elements_in_port_rate_table
                  ) ;
                send_string_to_screen(err_msg,FALSE) ;
                if (num_streams_in_mask !=
                      logical_rate_block->num_elements_in_stream_rate_table)
                {
                  sal_sprintf(err_msg,
                      "\r\n\n"
                      "*** num_streams_in_mask (%lu) !=\r\n"
                      "      num_elements_in_stream_rate_table (%lu)",
                      (unsigned long)num_streams_in_mask,
                      (unsigned long)
                        (logical_rate_block->num_elements_in_stream_rate_table)) ;
                  send_string_to_screen(err_msg,TRUE) ;
                  ui_ret = TRUE ;
                  free (recv_buff) ;
                  goto exit ;
                }
                if (num_ports_in_mask !=
                      logical_rate_block->num_elements_in_port_rate_table)
                {
                  sal_sprintf(err_msg,
                      "\r\n\n"
                      "*** num_ports_in_mask (%lu) !=\r\n"
                      "      num_elements_in_port_rate_table (%lu)",
                      (unsigned long)num_ports_in_mask,
                      (unsigned long)
                        (logical_rate_block->num_elements_in_port_rate_table)
                      ) ;
                  send_string_to_screen(err_msg,TRUE) ;
                  ui_ret = TRUE ;
                  free (recv_buff) ;
                  goto exit ;
                }
                /*
                 * Stream rate table.
                 */
                sal_sprintf(err_msg,"  Stream rate table:\r\n") ;
                send_string_to_screen(err_msg,FALSE) ;
                num_elements = tot_num_streams ;
                num_longs =
                  sizeof(logical_rate_block->
                    stream_rate_table[0].stream_rate_data) / sizeof(long) ;
                str_len = 0 ;
                logical_buff[0] = 0 ;
                stream_rate_com_unit = &(logical_rate_block->stream_rate_table[0]) ;
                display_index = 0 ;
                for (element_id = 0 ; element_id < num_elements ; element_id++)
                {
                  if (is_bit_set(mask_of_streams,element_id))
                  {
                    if ((display_index % 2) == 0)
                    {
                      sal_sprintf(
                        &logical_buff[str_len],
                        "\r\n%02lu: 0x",(unsigned long)display_index) ;
                      str_len += strlen(&logical_buff[str_len]) ;
                    }
                    for (long_id = 0 ; long_id < num_longs ; long_id++)
                    {
                      sal_sprintf(
                        &logical_buff[str_len],
                        "%02X %08lX ",
                        element_id,
                        stream_rate_com_unit->
                                stream_rate_data[long_id]) ;
                      str_len += strlen(&logical_buff[str_len]) ;
                    }
                    display_index++ ;
                    stream_rate_com_unit++ ;
                  }
                }
                sal_sprintf(&logical_buff[str_len],"\r\n") ;
                str_len += strlen(&logical_buff[str_len]) ;
                send_string_to_screen(logical_buff,FALSE) ;
                /*
                 * Port rate table.
                 */
                port_rate_com_unit = (PORT_RATE_COM_UNIT *)stream_rate_com_unit ;
                sal_sprintf(err_msg,"  Port rate table:\r\n") ;
                send_string_to_screen(err_msg,FALSE) ;
                num_elements = tot_num_ports ;
                num_longs =
                  sizeof(logical_rate_block->
                    port_rate_table[0].port_rate_data) / sizeof(long) ;
                str_len = 0 ;
                logical_buff[0] = 0 ;
                display_index = 0 ;
                for (element_id = 0 ; element_id < num_elements ; element_id++)
                {
                  if (is_bit_set(mask_of_ports,element_id))
                  {
                    if ((display_index % 2) == 0)
                    {
                      sal_sprintf(
                        &logical_buff[str_len],
                        "\r\n%02lu: 0x",(unsigned long)display_index) ;
                      str_len += strlen(&logical_buff[str_len]) ;
                    }
                    for (long_id = 0 ; long_id < num_longs ; long_id++)
                    {
                      sal_sprintf(
                        &logical_buff[str_len],
                        "%02X %08lX ",
                        element_id,
                        port_rate_com_unit->
                                  port_rate_data[long_id]) ;
                      str_len += strlen(&logical_buff[str_len]) ;
                    }
                    display_index++ ;
                    port_rate_com_unit++ ;
                  }
                }
                sal_sprintf(&logical_buff[str_len],"\r\n") ;
                str_len += strlen(&logical_buff[str_len]) ;
                send_string_to_screen(logical_buff,FALSE) ;
                logical_rate_block = (LOGICAL_RATE_BLOCK *)port_rate_com_unit ;
              }
            }
            else
            {
              sal_sprintf(err_msg,
                "NO \'rate collection\' data has been attached to this message!!!\r\n"
                ) ;
            }
          }
          free (recv_buff) ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_FPGA_RATE_START_ID,1))
      {
        /*
         * This is a 'start' command
         * Enter if this is a 'general fpga_rate fpga_* start ...' request.
         */
        mask_of_streams[0] = 0xAAAAAAAA ;
        mask_of_ports[0] = 0x55555555 ;
        mask_of_ports[1] = 0x55555555 ;
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_FPGA_RATE_COLLECTION_RATE_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * 'collection rate' - could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'fpga_rate ... collection_rate\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        fpga_collection_rate_ms = (unsigned int)param_val->value.ulong_value ;
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_FPGA_RATE_CONTINUOUS_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * 'continuous_mode' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'continuous_mode\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        fpga_continuous_mode = (unsigned int)param_val->numeric_equivalent ;
        fpga_host_ip = fpga_report_rate = 0 ;
        if (fpga_continuous_mode == ON_EQUIVALENT)
        {
          /*
           * This is a 'continuous mode' request. More parameters are expected.
           */
          if (get_val_of(
                  current_line,(int *)&match_index,
                  PARAM_GENERAL_FPGA_RATE_REPORT_RATE_ID,1,
                  &param_val,VAL_NUMERIC,err_msg))
          {
            /*
             * 'report_rate' parameter could not be retrieved.
             */
            send_string_to_screen(
                "\r\n\n"
                "*** Procedure \'get_val_of\' returned with error\r\n"
                "    (regarding \'report_rate\'):\r\n",
                TRUE) ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          fpga_report_rate = (unsigned int)param_val->value.ulong_value ;
          if (get_val_of(
                  current_line,(int *)&match_index,
                  PARAM_GENERAL_FPGA_RATE_HOST_IP_ID,1,
                  &param_val,VAL_IP,err_msg))
          {
            /*
             * 'host_ip' parameter could not be retrieved.
             */
            send_string_to_screen(
                "\r\n\n"
                "*** Procedure \'get_val_of\' returned with error\r\n"
                "    (regarding \'host_ip\'):\r\n",
                TRUE) ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          fpga_host_ip = (unsigned long)param_val->value.ip_value ;
        }
        else
        {
          /*
           * This is NOT a 'continuous mode' request. This is, rather, 'polling mode'.
           * No more parameters are expected.
           */
        }
        {
          unsigned
            int
              buff_byte_size ;
          OTHER_SERVICES_IN_STRUCT
            in_struct ;
          RATE_FTG_START_IN_STRUCT
            *rate_ftg_start_data ;
          FMC_COMMON_OUT_STRUCT
            *fmc_common_out_struct ;
          unsigned
            long
              recv_buff_size,
              dest_ip ;
          unsigned
            char
              *recv_buff,
              *buff_data ;
          err = get_run_ip_addr(&dest_ip) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** get_run_ip_addr() failed to get IP address") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          rate_ftg_start_data =
              &in_struct.data.rate_ftg_start_data ;
          /*
           * Fill buffer to send so as to reduce its size to required
           * minimum
           */
          buff_data = (unsigned char *)&in_struct ;
          buff_byte_size = sizeof(in_struct) ;
          if (fill_water_marks(buff_data,buff_byte_size))
          {
            send_string_to_screen(
              "\r\n\n"
              "*** SW error in fill_water_marks().\r\n",TRUE)  ;
            ui_ret = TRUE ;
            goto exit ;
          }
          /*
           * Fill input params
           */
          /*
           * Issue 'start rate collection' command
           */
          in_struct.proc_id = OTHER_SERVICES_FTG_START_TYPE ;
          rate_ftg_start_data->mask_of_streams[0] = mask_of_streams[0] ;
          rate_ftg_start_data->mask_of_ports[0] = mask_of_ports[0] ;
          rate_ftg_start_data->mask_of_ports[1] = mask_of_ports[1] ;
          rate_ftg_start_data->fpga_id = fpga_identifier ;
          rate_ftg_start_data->collection_rate_ms = fpga_collection_rate_ms ;
          rate_ftg_start_data->continuous_mode = fpga_continuous_mode ;
          rate_ftg_start_data->report_rate = fpga_report_rate ;
          rate_ftg_start_data->host_ip = fpga_host_ip ;
          recv_buff_size =
            (sizeof(FMC_COMMON_OUT_STRUCT) -
              SIZEOF(FMC_COMMON_OUT_STRUCT,common_union) +
              SIZEOF(FMC_COMMON_UNION,
                      other_services_out_union.rate_ftg_start_out_struct)) ;
          recv_buff = (unsigned char *)malloc(recv_buff_size) ;
          if ((unsigned char *)0 == recv_buff)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Failed to allocate buffer for fmc_send_buffer_to_slave_dcl() reply") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          fmc_common_out_struct = (FMC_COMMON_OUT_STRUCT *)recv_buff ;
          /*
           * Just to make it easy to see changed. Not required otherwise.
           */
          memset(recv_buff,0,recv_buff_size) ;
          {
            /*
             * This replaces sendBuffToCore() which requires setup of dune_rpc
             * devices.
             */
            unsigned
              long
                estimated_size ;
            estimated_size = buff_byte_size + 10 ;
            if (
                  !get_water_size(buff_data,buff_byte_size,&estimated_size) &&
                  estimated_size < buff_byte_size
               )
            {
              buff_byte_size = estimated_size ;
            }
            /*
             * This sending action is met, on receiver side, by the following
             * stack:
             *   dcl_rx_unexpected_task()
             *   handle_rx_dcl_device_msg()
             *   fmc_transport_receive_buffer_from_manager()
             * Note:
             * Device_id is not meaningful here but it has to have
             * either BIT(30) or BIT(31) set since that is how devices
             * are identified on the 'old fmc' system.
             */
            end_print_services() ;
            err =
              fmc_send_buffer_to_slave_dcl(
                BIT(31) | BIT(30),
                TRUE,dest_ip,FALSE,
                buff_byte_size,buff_data,
                recv_buff_size,recv_buff,
                NULL
              ) ;
            start_print_services() ;
            if (err)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** sendBuffToCore failed to send command to remote unit") ;
              send_string_to_screen(err_msg,TRUE) ;
              ui_ret = TRUE ;
              free (recv_buff) ;
              goto exit ;
            }
          }
          sal_sprintf(err_msg,
            "\r\n"
            "Display received buffer:") ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,
            "out_struct_type = %lu\r\n"
            "time_stamp      = %lu\r\n"
            "fpga_id         = %lu\r\n"
            "clock_error_id  = %lu\r\n"
            "logic_error_id  = %lu\r\n"
            "mode_error_id   = %lu\r\n"
            "start_error_id  = %lu\r\n",
            (unsigned long)fmc_common_out_struct->out_struct_type,
            fmc_common_out_struct->time_stamp,
            fmc_common_out_struct->
                common_union.
                other_services_out_union.
                rate_ftg_start_out_struct.fpga_id,
            fmc_common_out_struct->
                common_union.
                other_services_out_union.
                rate_ftg_start_out_struct.clock_error_id,
            fmc_common_out_struct->
                common_union.
                other_services_out_union.
                rate_ftg_start_out_struct.logic_error_id,
            fmc_common_out_struct->
                common_union.
                other_services_out_union.
                rate_ftg_start_out_struct.mode_error_id,
            fmc_common_out_struct->
                common_union.
                other_services_out_union.
                rate_ftg_start_out_struct.start_error_id
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          free (recv_buff) ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_FPGA_RATE_STOP_ID,1))
      {
        /*
         * This is a 'stop' command
         * Enter if this is a 'general fpga_rate fpga_* stop' request.
         */
        {
          unsigned
            int
              buff_byte_size ;
          OTHER_SERVICES_IN_STRUCT
            in_struct ;
          RATE_FTG_STOP_IN_STRUCT
            *rate_ftg_stop_data ;
          FMC_COMMON_OUT_STRUCT
            *fmc_common_out_struct ;
          unsigned
            long
              recv_buff_size,
              dest_ip ;
          unsigned
            char
              *recv_buff,
              *buff_data ;
          err = get_run_ip_addr(&dest_ip) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** get_run_ip_addr() failed to get IP address") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          rate_ftg_stop_data =
              &in_struct.data.rate_ftg_stop_data ;
          /*
           * Fill buffer to send so as to reduce its size to required
           * minimum
           */
          buff_data = (unsigned char *)&in_struct ;
          buff_byte_size = sizeof(in_struct) ;
          if (fill_water_marks(buff_data,buff_byte_size))
          {
            send_string_to_screen(
              "\r\n\n"
              "*** SW error in fill_water_marks().\r\n",TRUE)  ;
            ui_ret = TRUE ;
            goto exit ;
          }
          /*
           * Fill input params
           */
          /*
           * Issue 'stop rate collection' command
           */
          in_struct.proc_id = OTHER_SERVICES_FTG_STOP_TYPE ;
          rate_ftg_stop_data->fpga_id = fpga_identifier ;
          rate_ftg_stop_data->wait_for_stopped = FALSE ;
          recv_buff_size =
            (sizeof(FMC_COMMON_OUT_STRUCT) -
              SIZEOF(FMC_COMMON_OUT_STRUCT,common_union) +
              SIZEOF(FMC_COMMON_UNION,
                      other_services_out_union.rate_ftg_stop_out_struct)) ;
          recv_buff = (unsigned char *)malloc(recv_buff_size) ;
          if ((unsigned char *)0 == recv_buff)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Failed to allocate buffer for fmc_send_buffer_to_slave_dcl() reply") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          fmc_common_out_struct = (FMC_COMMON_OUT_STRUCT *)recv_buff ;
          /*
           * Just to make it easy to see changed. Not required otherwise.
           */
          memset(recv_buff,0,recv_buff_size) ;
          {
            /*
             * This replaces sendBuffToCore() which requires setup of dune_rpc
             * devices.
             */
            unsigned
              long
                estimated_size ;
            estimated_size = buff_byte_size + 10 ;
            if (
                  !get_water_size(buff_data,buff_byte_size,&estimated_size) &&
                  estimated_size < buff_byte_size
               )
            {
              buff_byte_size = estimated_size ;
            }
            /*
             * This sending action is met, on receiver side, by the following
             * stack:
             *   dcl_rx_unexpected_task()
             *   handle_rx_dcl_device_msg()
             *   fmc_transport_receive_buffer_from_manager()
             * Note:
             * Device_id is not meaningful here but it has to have
             * either BIT(30) or BIT(31) set since that is how devices
             * are identified on the 'old fmc' system.
             */
            end_print_services() ;
            err =
              fmc_send_buffer_to_slave_dcl(
                BIT(31) | BIT(30),
                TRUE,dest_ip,FALSE,
                buff_byte_size,buff_data,
                recv_buff_size,recv_buff,
                NULL
              ) ;
            start_print_services() ;
            if (err)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** sendBuffToCore failed to send command to remote unit") ;
              send_string_to_screen(err_msg,TRUE) ;
              ui_ret = TRUE ;
              free (recv_buff) ;
              goto exit ;
            }
          }
          sal_sprintf(err_msg,
            "\r\n"
            "Display received buffer:") ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,
            "out_struct_type = %lu\r\n"
            "time_stamp      = %lu\r\n"
            "fpga_id         = %lu\r\n"
            "stop_error_id   = %lu\r\n",
            (unsigned long)fmc_common_out_struct->out_struct_type,
            fmc_common_out_struct->time_stamp,
            fmc_common_out_struct->
                common_union.
                other_services_out_union.
                rate_ftg_stop_out_struct.fpga_id,
            fmc_common_out_struct->
                common_union.
                other_services_out_union.
                rate_ftg_stop_out_struct.stop_error_id
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          free (recv_buff) ;
        }
#if LINK_FAP20V_LIBRARIES


        if (0)
        {
          err =
            ftg_stop_ftg_col(fpga_identifier,FALSE,TRUE) ;
          if (err)
          {
            /*
             * 'ftg_start_ftg_col' fail.
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Procedure \'ftg_stop_ftg_col\' returned with error code %d\r\n",
                err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
        }
#endif /*LINK_FAP20V_LIBRARIES*/


      }
      else
      {
        /*
         * Illegal 'action': Not 'start' and not 'stop'. SW error.
         */
        send_string_to_screen(
            "\r\n\n"
          "*** Illegal \'fpga_rate action\' - Not \'start\' nor \'stop\'. SW error.\r\n",
          TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_FPGA_INDIRECT_ID,1))
    {
#if LINK_FTG_LIBRARIES
      /*
       * Subject: Indirect access to Fpga Test Generator (FTG)
       * Enter if this is a 'general fpga_indirect' request.
       */
      unsigned
        long
          num_longs_in_element ;
      unsigned
        int
          display_len,
          fpga_main_block,
          fpga_sub_block,
          fpga_auto_clear,
          fpga_identifier,
          fpga_from_element,
          fpga_to_element,
          fpga_element_number,
          fpga_action ;
      char
        action_text[480],
        *fpga_main_block_str,
        *fpga_sub_block_str,
        *fpga_identifier_str,
        *fpga_action_str ;
      FPGA_SUB_BLOCK_PROPERTIES
        *fpga_sub_block_properties ;
      num_handled++ ;
      send_array_to_screen("\r\n",2) ;
      if (get_val_of(
              current_line,(int *)&match_index,
              PARAM_GENERAL_FPGA_INDIRECT_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * 'Identifier of FPGA' parameter could not be retrieved.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error\r\n"
            "    (regarding \'fpga_indirect\'):\r\n",
            TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      fpga_identifier = (unsigned int)param_val->numeric_equivalent ;
      switch (fpga_identifier)
      {
        case (FPGA_A_IDENTIFIER):
        {
          fpga_identifier = FPGA_A ;
          fpga_identifier_str = "FPGA_A" ;
          break ;
        }
        case (FPGA_B_IDENTIFIER):
        {
          fpga_identifier = FPGA_B ;
          fpga_identifier_str = "FPGA_B" ;
          break ;
        }
        default:
        {
          /*
           * Illegal 'FPGA identifier'. SW error.
           */
          send_string_to_screen(
              "\r\n\n"
            "*** Illegal \'FPGA identifier\'. SW error.\r\n",
            TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
          break ;
        }
      }
      if (get_val_of(
              current_line,(int *)&match_index,
              PARAM_GENERAL_FPGA_IND_READ_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * 'Read Action to carry out on FPGA' - could not be retrieved.
         * Try 'write'
         */
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_FPGA_IND_WRITE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'read\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        else
        {
          fpga_action = FPGA_WRITE_ACTION ;
          fpga_action_str = "FPGA_WRITE_ACTION" ;
        }
      }
      else
      {
        fpga_action = FPGA_READ_ACTION ;
        fpga_action_str = "FPGA_READ_ACTION" ;
      }
      fpga_main_block = (unsigned int)param_val->numeric_equivalent ;
      switch (fpga_main_block)
      {
        case (GENERATOR_ACCESS):
        {
          fpga_main_block = ACCESS_GENERATOR ;
          fpga_main_block_str = "ACCESS_GENERATOR" ;
          break ;
        }
        case (VALIDATOR_ACCESS):
        {
          fpga_main_block = ACCESS_VERIFIER ;
          fpga_main_block_str = "ACCESS_VERIFIER" ;
          break ;
        }
        case (FLOW_CONTROL_ACCESS):
        {
          fpga_main_block = ACCESS_FLOW_CONTROL ;
          fpga_main_block_str = "ACCESS_FLOW_CONTROL" ;
          break ;
        }
        default:
        {
          /*
           * Illegal 'FPGA main_block'. SW error.
           */
          send_string_to_screen(
              "\r\n\n"
            "*** Illegal \'FPGA main_block\'. SW error.\r\n",
            TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
          break ;
        }
      }
      if (fpga_action == FPGA_READ_ACTION)
      {
        /*
         * Here handle parameters which are specific to 'read'
         */
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_FPGA_IND_SUB_BLOCK_READ_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * 'Sub-block to handle on FPGA' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'read\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        fpga_sub_block = (unsigned int)param_val->numeric_equivalent ;
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_FPGA_IND_AUTO_CLEAR_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * 'Sub-block to handle on FPGA' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'auto_clear\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        fpga_auto_clear = (unsigned int)param_val->numeric_equivalent ;
        if (fpga_auto_clear == ON_EQUIVALENT)
        {
          switch (fpga_sub_block)
          {
            case (SUB_BLOCK_STREAM_RATE_TABLE):
            case (SUB_BLOCK_PORT_RATE_TABLE):
            {
              break ;
            }
            default:
            {
              /*
               * Illegal 'FPGA sub-block'. This block does not have 'auto_clear'
               * option.
               */
              send_string_to_screen(
                  "\r\n\n"
                "*** Illegal \'FPGA sub_block\' for \'auto_clear\' action.\r\n",
                TRUE) ;
              ui_ret = TRUE ;
              goto exit ;
              break ;
            }
          }
        }
        switch (fpga_sub_block)
        {
          case (SUB_BLOCK_CALENDAR_TABLE):
          {
            fpga_sub_block = ACCESS_CALENDAR_TABLE ;
            fpga_sub_block_str = "ACCESS_CALENDAR_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_EGRESS_STREAM_TABLE):
          {
            fpga_sub_block = ACCESS_EGRESS_STREAM_TABLE ;
            fpga_sub_block_str = "ACCESS_EGRESS_STREAM_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_EGRESS_PATTERN_TABLE):
          {
            fpga_sub_block = ACCESS_EGRESS_PATTERN_TABLE ;
            fpga_sub_block_str = "ACCESS_EGRESS_PATTERN_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_LOG_MEMORY_TABLE):
          {
            fpga_sub_block = ACCESS_LOG_MEMORY ;
            fpga_sub_block_str = "ACCESS_LOG_MEMORY" ;
            break ;
          }
          case (SUB_BLOCK_CHANNEL_TO_PORT_TABLE):
          {
            fpga_sub_block = ACCESS_CHANNEL_TO_PORT ;
            fpga_sub_block_str = "ACCESS_CHANNEL_TO_PORT" ;
            break ;
          }
          case (SUB_BLOCK_INGRESS_STREAM_TABLE):
          {
            fpga_sub_block = ACCESS_INGRESS_STREAM_TABLE ;
            fpga_sub_block_str = "ACCESS_INGRESS_STREAM_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_INGRESS_PATTERN_TABLE):
          {
            fpga_sub_block = ACCESS_INGRESS_PATTERN_TABLE ;
            fpga_sub_block_str = "ACCESS_INGRESS_PATTERN_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_LOGICAL_MULTICAST_TABLE):
          {
            fpga_sub_block = ACCESS_LOGICAL_MULTICAST_TABLE ;
            fpga_sub_block_str = "ACCESS_LOGICAL_MULTICAST_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_INGRESS_MULTICAST_STREAM_TABLE):
          {
            fpga_sub_block = ACCESS_INGRESS_MULTICAST_STREAM_TABLE ;
            fpga_sub_block_str = "ACCESS_INGRESS_MULTICAST_STREAM_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_STREAM_SELECTION_TABLE):
          {
            fpga_sub_block = ACCESS_STREAM_SELECTION_TABLE ;
            fpga_sub_block_str = "ACCESS_STREAM_SELECTION_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_STREAM_RATE_TABLE):
          {
            fpga_sub_block = ACCESS_STREAM_RATE ;
            fpga_sub_block_str = "ACCESS_STREAM_RATE" ;
            break ;
          }
          case (SUB_BLOCK_PORT_RATE_TABLE):
          {
            fpga_sub_block = ACCESS_PORT_RATE ;
            fpga_sub_block_str = "ACCESS_PORT_RATE" ;
            break ;
          }
          case (SUB_BLOCK_M_DELAY_TABLE):
          {
            fpga_sub_block = ACCESS_M_DELAY_TABLE ;
            fpga_sub_block_str = "ACCESS_M_DELAY_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_LAST_DELAY_TABLE):
          {
            fpga_sub_block = ACCESS_LAST_DELAY_TABLE ;
            fpga_sub_block_str = "ACCESS_LAST_DELAY_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_DELAY_HISTOGRAM_TABLE):
          {
            fpga_sub_block = ACCESS_DELAY_HISTOGRAM_TABLE ;
            fpga_sub_block_str = "ACCESS_DELAY_HISTOGRAM_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_LAST_TIMESTAMP_TABLE):
          {
            fpga_sub_block = ACCESS_LAST_TIMESTAMP_TABLE ;
            fpga_sub_block_str = "ACCESS_LAST_TIMESTAMP_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_INTER_PACKET_DELAY_HISTOGRAM):
          {
            fpga_sub_block = ACCESS_INTER_PACKET_DELAY_HISTOGRAM ;
            fpga_sub_block_str = "ACCESS_INTER_PACKET_DELAY_HISTOGRAM" ;
            break ;
          }
          case (SUB_BLOCK_JITTER_HISTOGRAM_TABLE):
          {
            fpga_sub_block = ACCESS_JITTER_HISTOGRAM_TABLE ;
            fpga_sub_block_str = "ACCESS_JITTER_HISTOGRAM_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_PORT_STATISTICS_TABLE):
          {
            fpga_sub_block = ACCESS_PORT_STATISTICS_TABLE ;
            fpga_sub_block_str = "ACCESS_PORT_STATISTICS_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_CHANNEL_WEIGHT_TABLE):
          {
            fpga_sub_block = ACCESS_CHANNEL_WEIGHT_TABLE ;
            fpga_sub_block_str = "ACCESS_CHANNEL_WEIGHT_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_CHANNEL_FIFO_SIZE_TABLE):
          {
            fpga_sub_block = ACCESS_CHANNEL_FIFO_SIZE_TABLE ;
            fpga_sub_block_str = "ACCESS_CHANNEL_FIFO_SIZE_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_SPILL_OVER_TABLE):
          {
            fpga_sub_block = ACCESS_SPILL_OVER_TABLE ;
            fpga_sub_block_str = "ACCESS_SPILL_OVER_TABLE" ;
            break ;
          }
          default:
          {
            /*
             * Illegal 'FPGA sub-block'. SW error.
             */
            send_string_to_screen(
                "\r\n\n"
              "*** Illegal \'FPGA sub_block\'. SW error.\r\n",
              TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
            break ;
          }
        }
        {
          sal_sprintf(action_text,
            "\r\n"
            "Access %s. Action %s. Main block %s.\r\n"
            "  Sub block %s\r\n",
            fpga_identifier_str,
            fpga_action_str,
            fpga_main_block_str,
            fpga_sub_block_str
            ) ;
          display_len = strlen(action_text) ;
        }
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_FPGA_IND_FROM_ELEMENT_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * First element to handle on 'Read Identifier from FPGA'.
           * Parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'fpga_indirect ... from_element\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        fpga_from_element = (unsigned int)param_val->value.ulong_value ;
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_FPGA_IND_TO_ELEMENT_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Last element to handle on 'Read Identifier from FPGA'.
           * Parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'fpga_indirect...to_element\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        fpga_to_element = (unsigned int)param_val->value.ulong_value ;
        if (fpga_to_element < fpga_from_element)
        {
          /*
           * Last element to handle must be larger or equal to first.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** FTG indirect: Last element (%u) to handle must be\r\n"
              "    larger or equal to first (%u). Quit.\r\n",
              fpga_to_element,fpga_from_element) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
#if LINK_FTG_LIBRARIES
        err =
          ftg_find_sub_block_properties(
            fpga_main_block,fpga_sub_block,&fpga_sub_block_properties) ;
        if (err)
        {
          /*
           * ftg_find_sub_block_properties() failed.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** FTG indirect: ftg_find_sub_block_properties() failure,\r\n"
              "    probably because there is no such (main block,sub block) combination:\r\n"
              "    (%s,%s). Quit.\r\n",
              fpga_main_block_str,
              fpga_sub_block_str) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
#endif /*#if LINK_FTG_LIBRARIES*/
        if (fpga_to_element >=
                  fpga_sub_block_properties->num_elements_in_block)
        {
          /*
           * All elements must be within sub-block's range.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** FTG indirect: All elements must be within\r\n"
              "    sub-block\'s range [0 -> %u). Quit.\r\n",
              fpga_sub_block_properties->num_elements_in_block) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        num_longs_in_element =
          fpga_sub_block_properties->num_longs_in_element ;
        sal_sprintf(&action_text[display_len],
          "  First element no. %u. Last element no. %u.\r\n",
          fpga_from_element,
          fpga_to_element
          ) ;
        send_string_to_screen(action_text,TRUE) ;
        {
          unsigned
            int
              buff_byte_size ;
          OTHER_SERVICES_IN_STRUCT
            in_struct ;
          MEM_FTG_INDIRECT_READ_IN_STRUCT
            *mem_ftg_indirect_read_data ;
          unsigned
            long
              recv_buff_size,
              dest_ip,
              num_elements_to_read ;
          unsigned
            char
              *recv_buff,
              *buff_data ;
          err = get_run_ip_addr(&dest_ip) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** get_run_ip_addr() failed to get IP address") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          mem_ftg_indirect_read_data =
              &in_struct.data.mem_ftg_indirect_read_data ;
          /*
           * Fill buffer to send so as to reduce its size to required
           * minimum
           */
          buff_data = (unsigned char *)&in_struct ;
          buff_byte_size = sizeof(in_struct) ;
          if (fill_water_marks(buff_data,buff_byte_size))
          {
            send_string_to_screen(
              "\r\n\n"
              "*** SW error in fill_water_marks().\r\n",TRUE)  ;
            ui_ret = TRUE ;
            goto exit ;
          }
          /*
           * Fill input params
           */
          /*
           * Read Indirect Operation
           */
          num_elements_to_read = fpga_to_element - fpga_from_element + 1 ;
          in_struct.proc_id = OTHER_SERVICES_FTG_INDIRECT_READ_TYPE ;
          mem_ftg_indirect_read_data->fpga_id = fpga_identifier ;
          mem_ftg_indirect_read_data->main_block_id = fpga_main_block ;
          mem_ftg_indirect_read_data->sub_block_id = fpga_sub_block ;
          mem_ftg_indirect_read_data->
            offset_within_sub_block = fpga_from_element ;
          mem_ftg_indirect_read_data->
            num_elements_to_read  = num_elements_to_read ;
          mem_ftg_indirect_read_data->
            auto_clear  = (unsigned char)fpga_auto_clear ;
          recv_buff_size =
            num_elements_to_read *
                num_longs_in_element * sizeof(unsigned long) ;
          recv_buff_size +=
            (sizeof(FMC_COMMON_OUT_STRUCT) -
              SIZEOF(FMC_COMMON_OUT_STRUCT,common_union) +
              SIZEOF(FMC_COMMON_UNION,
                      other_services_out_union.mem_ftg_indirect_read_data) -
              SIZEOF(MEM_FTG_INDIRECT_READ_OUT_STRUCT,data)) ;
          recv_buff = (unsigned char *)malloc(recv_buff_size) ;
          if ((unsigned char *)0 == recv_buff)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Failed to allocate buffer for fmc_send_buffer_to_slave_dcl() reply") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          /*
           * Just to make it easy to see changed. Not required otherwise.
           */
          memset(recv_buff,0,recv_buff_size) ;
          {
            /*
             * This replaces sendBuffToCore() which requires setup of dune_rpc
             * devices.
             */
            unsigned
              long
                estimated_size ;
            estimated_size = buff_byte_size + 10 ;
            if (
                  !get_water_size(buff_data,buff_byte_size,&estimated_size) &&
                  estimated_size < buff_byte_size
               )
            {
              buff_byte_size = estimated_size ;
            }
            /*
             * This sending action is met, on receiver side, by the following
             * stack:
             *   dcl_rx_unexpected_task()
             *   handle_rx_dcl_device_msg()
             *   fmc_transport_receive_buffer_from_manager()
             * Note:
             * Device_id is not meaningful here but it has to have
             * either BIT(30) or BIT(31) set since that is how devices
             * are identified on the 'old fmc' system.
             */
            err =
              fmc_send_buffer_to_slave_dcl(
                BIT(31) | BIT(30),
                TRUE,dest_ip,FALSE,
                buff_byte_size,buff_data,
                recv_buff_size,recv_buff,
                NULL
              ) ;
            if (err)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** sendBuffToCore failed to send command to remote unit") ;
              send_string_to_screen(err_msg,TRUE) ;
              ui_ret = TRUE ;
              free (recv_buff) ;
              goto exit ;
            }
          }
          sal_sprintf(action_text,"Display received data buffer:") ;
          send_string_to_screen(action_text,TRUE) ;
          display_memory(
              (char *)recv_buff,
              recv_buff_size / sizeof(long),
              LONG_EQUIVALENT,
              HEX_EQUIVALENT,
              FALSE,
              FALSE,
              0
              ) ;
          free (recv_buff) ;
        }
      }
      else
      {
        /*
         * Here handle parameters which are specific to 'WRITE'
         */
        unsigned
          long
            write_data[16] ;
        unsigned
          int
            ui,
            num_to_write ;
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_FPGA_IND_SUB_BLOCK_WRITE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * 'Sub-block to handle on FPGA' parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'write\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        fpga_sub_block = (unsigned int)param_val->numeric_equivalent ;
        switch (fpga_sub_block)
        {
          case (SUB_BLOCK_CALENDAR_TABLE):
          {
            fpga_sub_block = ACCESS_CALENDAR_TABLE ;
            fpga_sub_block_str = "ACCESS_CALENDAR_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_EGRESS_STREAM_TABLE):
          {
            fpga_sub_block = ACCESS_EGRESS_STREAM_TABLE ;
            fpga_sub_block_str = "ACCESS_EGRESS_STREAM_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_EGRESS_PATTERN_TABLE):
          {
            fpga_sub_block = ACCESS_EGRESS_PATTERN_TABLE ;
            fpga_sub_block_str = "ACCESS_EGRESS_PATTERN_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_CHANNEL_TO_PORT_TABLE):
          {
            fpga_sub_block = ACCESS_CHANNEL_TO_PORT ;
            fpga_sub_block_str = "ACCESS_CHANNEL_TO_PORT" ;
            break ;
          }
          case (SUB_BLOCK_INGRESS_STREAM_TABLE):
          {
            fpga_sub_block = ACCESS_INGRESS_STREAM_TABLE ;
            fpga_sub_block_str = "ACCESS_INGRESS_STREAM_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_INGRESS_PATTERN_TABLE):
          {
            fpga_sub_block = ACCESS_INGRESS_PATTERN_TABLE ;
            fpga_sub_block_str = "ACCESS_INGRESS_PATTERN_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_LOGICAL_MULTICAST_TABLE):
          {
            fpga_sub_block = ACCESS_LOGICAL_MULTICAST_TABLE ;
            fpga_sub_block_str = "ACCESS_LOGICAL_MULTICAST_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_INGRESS_MULTICAST_STREAM_TABLE):
          {
            fpga_sub_block = ACCESS_INGRESS_MULTICAST_STREAM_TABLE ;
            fpga_sub_block_str = "ACCESS_INGRESS_MULTICAST_STREAM_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_STREAM_SELECTION_TABLE):
          {
            fpga_sub_block = ACCESS_STREAM_SELECTION_TABLE ;
            fpga_sub_block_str = "ACCESS_STREAM_SELECTION_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_STREAM_RATE_TABLE):
          {
            fpga_sub_block = ACCESS_STREAM_RATE ;
            fpga_sub_block_str = "ACCESS_STREAM_RATE" ;
            break ;
          }
          case (SUB_BLOCK_PORT_RATE_TABLE):
          {
            fpga_sub_block = ACCESS_PORT_RATE ;
            fpga_sub_block_str = "ACCESS_PORT_RATE" ;
            break ;
          }
          case (SUB_BLOCK_M_DELAY_TABLE):
          {
            fpga_sub_block = ACCESS_M_DELAY_TABLE ;
            fpga_sub_block_str = "ACCESS_M_DELAY_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_LAST_DELAY_TABLE):
          {
            fpga_sub_block = ACCESS_LAST_DELAY_TABLE ;
            fpga_sub_block_str = "ACCESS_LAST_DELAY_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_DELAY_HISTOGRAM_TABLE):
          {
            fpga_sub_block = ACCESS_DELAY_HISTOGRAM_TABLE ;
            fpga_sub_block_str = "ACCESS_DELAY_HISTOGRAM_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_LAST_TIMESTAMP_TABLE):
          {
            fpga_sub_block = ACCESS_LAST_TIMESTAMP_TABLE ;
            fpga_sub_block_str = "ACCESS_LAST_TIMESTAMP_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_INTER_PACKET_DELAY_HISTOGRAM):
          {
            fpga_sub_block = ACCESS_INTER_PACKET_DELAY_HISTOGRAM ;
            fpga_sub_block_str = "ACCESS_INTER_PACKET_DELAY_HISTOGRAM" ;
            break ;
          }
          case (SUB_BLOCK_JITTER_HISTOGRAM_TABLE):
          {
            fpga_sub_block = ACCESS_JITTER_HISTOGRAM_TABLE ;
            fpga_sub_block_str = "ACCESS_JITTER_HISTOGRAM_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_PORT_STATISTICS_TABLE):
          {
            fpga_sub_block = ACCESS_PORT_STATISTICS_TABLE ;
            fpga_sub_block_str = "ACCESS_PORT_STATISTICS_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_CHANNEL_WEIGHT_TABLE):
          {
            fpga_sub_block = ACCESS_CHANNEL_WEIGHT_TABLE ;
            fpga_sub_block_str = "ACCESS_CHANNEL_WEIGHT_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_CHANNEL_FIFO_SIZE_TABLE):
          {
            fpga_sub_block = ACCESS_CHANNEL_FIFO_SIZE_TABLE ;
            fpga_sub_block_str = "ACCESS_CHANNEL_FIFO_SIZE_TABLE" ;
            break ;
          }
          case (SUB_BLOCK_SPILL_OVER_TABLE):
          {
            fpga_sub_block = ACCESS_SPILL_OVER_TABLE ;
            fpga_sub_block_str = "ACCESS_SPILL_OVER_TABLE" ;
            break ;
          }
          default:
          {
            /*
             * Illegal 'FPGA sub-block'. SW error.
             */
            send_string_to_screen(
                "\r\n\n"
              "*** Illegal \'FPGA sub_block\' fpr \'write\' action. SW error.\r\n",
              TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
            break ;
          }
        }
        {
          sal_sprintf(action_text,
            "\r\n"
            "Access %s. Action %s. Main block %s.\r\n"
            "  Sub block %s\r\n",
            fpga_identifier_str,
            fpga_action_str,
            fpga_main_block_str,
            fpga_sub_block_str
            ) ;
          display_len = strlen(action_text) ;
        }
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_FPGA_IND_ELEMENT_NUMBER_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Element to handle on 'Write Identifier to FPGA'.
           * Parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "    (regarding \'fpga_indirect ... element_number\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        fpga_element_number = (unsigned int)param_val->value.ulong_value ;
#if LINK_FTG_LIBRARIES
        err =
          ftg_find_sub_block_properties(
            fpga_main_block,fpga_sub_block,&fpga_sub_block_properties) ;
        if (err)
        {
          /*
           * ftg_find_sub_block_properties() failed.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** FTG indirect: ftg_find_sub_block_properties() failure,\r\n"
              "    probably because there is no such (main block,sub block) combination:\r\n"
              "    (%s,%s). Quit.\r\n",
              fpga_main_block_str,
              fpga_sub_block_str) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
#endif /*#if LINK_FTG_LIBRARIES*/
        if (fpga_element_number >=
                  fpga_sub_block_properties->num_elements_in_block)
        {
          /*
           * All elements must be within sub-block's range.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** FTG indirect: Specified element must be within\r\n"
              "    sub-block\'s range [0 -> %u). Quit.\r\n",
              fpga_sub_block_properties->num_elements_in_block) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        num_longs_in_element =
          fpga_sub_block_properties->num_longs_in_element ;
        sal_sprintf(&action_text[display_len],
          "  Element no. %u. Data values to write:\r\n"
          "  0x",
          fpga_element_number
          ) ;
        display_len += strlen(&action_text[display_len]) ;
        /*
         * Get data to write from command line.
         */
        {
          /*
           * Get data to write. There must be at least one data item.
           */
          num_to_write = (sizeof(write_data)/sizeof(write_data[0])) ;
          /*
           * The following 'for' MUST make at least one loop!
           */
          for (ui = 1 ; ui <= (sizeof(write_data)/sizeof(write_data[0])) ; ui++)
          {
            if (get_val_of(
                  current_line,(int *)&match_index,
                  PARAM_GENERAL_FPGA_IND_WRITE_DATA_ID,ui,
                  &param_val,VAL_NUMERIC,err_msg))
            {
              /*
               * If there are no more data values then quit loop.
               */
              num_to_write = ui - 1 ;
              break ;
            }
            write_data[ui - 1] = param_val->value.ulong_value ;
          }
          if (num_to_write == 0)
          {
            /*
             * Not even one value of write data could not be retrieved.
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Not even one value of write data could not be retrieved!"
                ) ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          /*
           * Just precaution...
           */
          if (num_to_write > (sizeof(write_data)/sizeof(write_data[0])))
          {
            /*
             * Too many write data values have been retrieved.
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Too many values of write data (%u) have been retrieved!",
                (unsigned int)num_to_write) ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
        }
        {
          /*
           * Note: Number of long words in element must match number
           * of data items:
           * Exactly ONE data item is used with ONE long word in element.
           */
          if (num_to_write != num_longs_in_element)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Number of longs in element (%u) is not equal to\r\n"
                "    number of data items (%u).",
                (unsigned short)num_longs_in_element,(unsigned short)num_to_write) ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
        }
        {
          /*
           * Here load data to action_text.
           */
          for (ui = 0 ; ui < num_longs_in_element ; ui++)
          {
            if (ui > 0)
            {
              /*
               * Print 8 longs per line.
               */
              if ((ui % 8) == 0)
              {
                sal_sprintf(&action_text[display_len],"\r\n  0x") ;
                display_len += strlen(&action_text[display_len]) ;
              }
            }
            sal_sprintf(&action_text[display_len],
              "%08lX ",
              write_data[ui]
              ) ;
            display_len += strlen(&action_text[display_len]) ;
          }
          sal_sprintf(&action_text[display_len],"\r\n") ;
          display_len += strlen(&action_text[display_len]) ;
        }
        send_string_to_screen(action_text,TRUE) ;
        {
          unsigned
            int
              buff_byte_size ;
          OTHER_SERVICES_IN_STRUCT
            *in_struct ;
          MEM_FTG_INDIRECT_WRITE_IN_STRUCT
            *mem_ftg_indirect_write_data ;
          unsigned
            long
              sizeof_data,
              tx_buff_size,
              recv_buff_size,
              dest_ip,
              num_elements_to_write ;
          unsigned
            char
              *recv_buff,
              *buff_data ;
          err = get_run_ip_addr(&dest_ip) ;
          if (err)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** get_run_ip_addr() failed to get IP address") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          /*
           * For this implementation, write one element.
           */
          num_elements_to_write = 1 ;
          sizeof_data =
            tx_buff_size =
              num_elements_to_write *
                num_longs_in_element * sizeof(unsigned long) ;
          tx_buff_size +=
            (sizeof(OTHER_SERVICES_IN_STRUCT) -
              SIZEOF(OTHER_SERVICES_IN_STRUCT,data) +
              SIZEOF(OTHER_SERVICES_IN_UNION,mem_ftg_indirect_write_data) -
              SIZEOF(OTHER_SERVICES_IN_UNION,mem_ftg_indirect_write_data.data)
            ) ;
          in_struct = (OTHER_SERVICES_IN_STRUCT *)malloc(tx_buff_size) ;
          if ((OTHER_SERVICES_IN_STRUCT *)0 == in_struct)
          {
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Failed to allocate buffer for fmc_send_buffer_to_slave_dcl() request") ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          mem_ftg_indirect_write_data =
              &(in_struct->data.mem_ftg_indirect_write_data) ;
          /*
           * Fill buffer to send so as to reduce its size to required
           * minimum
           */
          buff_data = (unsigned char *)in_struct ;
          buff_byte_size = tx_buff_size ;
          if (fill_water_marks(buff_data,buff_byte_size))
          {
            send_string_to_screen(
              "\r\n\n"
              "*** SW error in fill_water_marks().\r\n",TRUE)  ;
            ui_ret = TRUE ;
            goto exit ;
          }
          /*
           * Fill input params - Write Indirect Operation
           */
          in_struct->proc_id = OTHER_SERVICES_FTG_INDIRECT_WRITE_TYPE ;
          mem_ftg_indirect_write_data->fpga_id = fpga_identifier ;
          mem_ftg_indirect_write_data->main_block_id = fpga_main_block ;
          mem_ftg_indirect_write_data->sub_block_id = fpga_sub_block ;
          mem_ftg_indirect_write_data->
            offset_within_sub_block = fpga_element_number ;
          mem_ftg_indirect_write_data->
            num_elements_to_write  = num_elements_to_write ;
          mem_ftg_indirect_write_data->sizeof_data  = sizeof_data ;
          /*
           * Big endian processor is assumed, throughout.
           */
          memcpy(mem_ftg_indirect_write_data->data,write_data,sizeof_data) ;
          /*
           * No data is expected as response.
           */
          recv_buff_size = 0 ;
          recv_buff = (unsigned char *)0 ;
          {
            /*
             * This replaces sendBuffToCore() which requires setup of dune_rpc
             * devices.
             */
            unsigned
              long
                estimated_size ;
            estimated_size = buff_byte_size + 10 ;
            if (
                  !get_water_size(buff_data,buff_byte_size,&estimated_size) &&
                  estimated_size < buff_byte_size
               )
            {
              buff_byte_size = estimated_size ;
            }
            /*
             * Display tx buffer before sending it.
             */
            sal_sprintf(action_text,"Display tx buffer before sending it:") ;
            send_string_to_screen(action_text,TRUE) ;
            display_memory(
                (char *)buff_data,
                buff_byte_size / sizeof(long),
                LONG_EQUIVALENT,
                HEX_EQUIVALENT,
                FALSE,
                FALSE,
                0
              ) ;
            /*
             * This sending action is met, on receiver side, by the following
             * stack:
             *   dcl_rx_unexpected_task()
             *   handle_rx_dcl_device_msg()
             *   fmc_transport_receive_buffer_from_manager()
             * Note:
             * Device_id is not meaningful here but it has to have
             * either BIT(30) or BIT(31) set since that is how devices
             * are identified on the 'old fmc' system.
             */
            err =
              fmc_send_buffer_to_slave_dcl(
                BIT(31) | BIT(30),
                TRUE,dest_ip,FALSE,
                buff_byte_size,buff_data,
                recv_buff_size,recv_buff,
                NULL
              ) ;
            if (err)
            {
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** sendBuffToCore failed to send command to remote unit") ;
              send_string_to_screen(err_msg,TRUE) ;
              ui_ret = TRUE ;
              free (recv_buff) ;
              goto exit ;
            }
          }
          free (buff_data) ;
        }
      }
#else /* LINK_FTG_LIBRARIES */
      send_string_to_screen("\r\n*** Support for this subject was not compiled", TRUE);
#endif /* LINK_FTG_LIBRARIES */
    }
    else if (!search_param_val_pairs(current_line,&match_index,
                                  PARAM_GENERAL_AUX_INPUT_ID,1))
    {
      send_string_to_screen("", TRUE);

      /*
       * Enter if this is a 'general aux_input' request.
       */
      if (!search_param_val_pairs(current_line,&match_index,
                                PARAM_GENERAL_PRINT_STATUS_ID,1))
      {
        /*
         * Enter if this is a 'general aux_input print_stat' request.
         */
        num_handled++ ;
        aux_input_print_state() ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,
                                PARAM_GENERAL_AUX_INPUT_RESTART_ID,1))
      {
        /*
         * Enter if this is a 'general aux_input restart' request.
         */
        num_handled++ ;
        utils_aux_input_start();
      }
      else
      {
        /*
         * Parameter following 'demo' is unrecognized.
         */
        send_string_to_screen(
            "\r\n"
            "*** Parameter following \'aux_input\' is unrecognizable\r\n",
            TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
#if LINK_SERDES_LIBRARIES
    else if (!search_param_val_pairs(current_line,&match_index,
                                           PARAM_GENERAL_DEMO_ID,1))
    {
      /*
       * Enter if this is a 'general demo' request.
       */
      if (!search_param_val_pairs(current_line,&match_index,
                            PARAM_GENERAL_DEMO_DISPLAY_NETWORK_ID,1))
      {
        /*
         * Enter if this is a 'general trace display_network' request.
         */
        num_handled++ ;
        print_network_learning_table() ;
        remote_access_print_status();
      }
      else if (!search_param_val_pairs(current_line,&match_index,
                                PARAM_GENERAL_DEMO_SHOW_FE_LINK_ID,1))
      {
        /*
         * Enter if this is a 'general trace show_fe_link' request.
         */
        unsigned
          int
            line_card_slot,
            fabric_card_slot,
            fap_link ;
        int
          crate_type,
          fe_number,
          fe_link ;
        num_handled++ ;
        if (get_val_of(
                current_line,(int *)&match_index,
                PARAM_GENERAL_DEMO_CRATE_TYPE_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Value of filename parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error (regarding \'crate_type\'):\r\n",
              TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        crate_type = (int)param_val->numeric_equivalent ;
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_DEMO_LINE_CARD_SLOT_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of filename parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "   (regarding \'line_card_slot\'):\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        line_card_slot = (int)param_val->value.ulong_value ;
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_DEMO_FABRIC_CARD_SLOT_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of filename parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error (regarding \'fabric_card_slot\'):\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        fabric_card_slot = (int)param_val->value.ulong_value ;
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_DEMO_FAP_LINK_ID,1,
                &param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of filename parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error (regarding \'fap_link\'):\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        fap_link = (int)param_val->value.ulong_value ;
        err =
          serdes_get_fe_link_given_fap_link(
            crate_type,line_card_slot,fabric_card_slot,fap_link,
            &fe_number,&fe_link) ;
        if (err < 0)
        {
          sal_sprintf(err_msg,
              "\r\n\n"
              "There is no such connection:\r\n"
              "  No physical line connecting fap link %u of FAP on slot %u\r\n"
              "  with an FE on slot %u\r\n",
              fap_link,line_card_slot,fabric_card_slot) ;
          send_string_to_screen(err_msg,TRUE) ;
        }
        else if (err > 0)
        {
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Error code %lu reported by \'serdes_get_fe_link_given_fap_link()\'\r\n"
              "  with an FE on slot %u\r\n",
              (unsigned long)err,
              fabric_card_slot) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
        }
        else
        {
          sal_sprintf(err_msg,
              "\r\n\n"
              "FE link no. %u connects fap link %u of FAP on slot %u\r\n"
              "with FE no. %u on slot %u\r\n",
              fe_link,fap_link,line_card_slot,fe_number,fabric_card_slot) ;
          send_string_to_screen(err_msg,TRUE) ;
        }
        goto exit ;
      }
      else
      {
        /*
         * Parameter following 'demo' is unrecognized.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Parameter following \'demo\' is unrecognizable\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
#endif /*#if LINK_SERDES_LIBRARIES*/

    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_FREE_UI_BITS_ID,1))
    {
      unsigned int
        subject_i,
        param_i,
        long_i,
        bit_i;
      unsigned long
        mutex_control[NUM_ELEMENTS_MUTEX_CONTROL] ;
      PARAM
        *param_ptr;

      num_handled++;
      send_string_to_screen("",TRUE) ;
      send_string_to_screen("Free space mark as ZERO in the bitmap",TRUE) ;
      for (subject_i=0; Subjects_list_rom[subject_i].subject_id!=SUBJECT_END_OF_LIST; ++subject_i)
      {
        /*
         * For every subject
         */
        d_printf("Subject Name: %s\n\r", Subjects_list_rom[subject_i].subj_name);
        soc_sand_bitstream_clear(mutex_control, NUM_ELEMENTS_MUTEX_CONTROL);

        param_ptr = Subjects_list_rom[subject_i].allowed_params;
        for (param_i=0; param_ptr->param_id != PARAM_END_OF_LIST; ++param_i, ++param_ptr)
        {
          soc_sand_bitstream_or(mutex_control, param_ptr->mutex_control, NUM_ELEMENTS_MUTEX_CONTROL);
        }

        for (long_i=0; long_i<NUM_ELEMENTS_MUTEX_CONTROL; ++long_i)
        {
          d_printf("%u) {", long_i);
          if (mutex_control[long_i] == 0x0)
          {
            d_printf(" All ");
          }
          else if (mutex_control[long_i] == 0xFFFFFFFF)
          {
            d_printf(" None ");
          }
          else
          {
            for (bit_i=0; bit_i<SOC_SAND_NOF_BITS_IN_LONG; ++bit_i)
            {
              if ( ! ( BIT(bit_i) & mutex_control[long_i] ) )
              {
                d_printf("%2u,", bit_i);
              }
            }
          }
          d_printf("}\n\r");
        }
      }
      send_string_to_screen("",TRUE) ;

      for (subject_i=0; Subjects_list_rom[subject_i].subject_id!=SUBJECT_END_OF_LIST; ++subject_i)
      {
        unsigned long
          nof_params;
        nof_params = 0;

        param_ptr = Subjects_list_rom[subject_i].allowed_params;
        for (param_i=0; param_ptr->param_id != PARAM_END_OF_LIST; ++param_i, ++param_ptr)
        {
          nof_params++;
        }

        d_printf(
          "Subject Name: %s, Number of parameters %lu \n\r",
          Subjects_list_rom[subject_i].subj_name,
          nof_params
        );
        if (nof_params>=MAX_PARAMS_PER_SUBJECT)
        {
          d_printf(
            "Error More than Max (%u) \n\r",
            MAX_PARAMS_PER_SUBJECT
          );
        }


      }
      send_string_to_screen("",TRUE) ;


    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_REMARK_ID,1))
    {
       char
        free_text[MAX_CHARS_ON_SCREEN_LINE + 1] ;
      unsigned
        int
          free_text_len ;
      num_handled++ ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GENERAL_REMARK_ID,1,
              &param_val,VAL_TEXT,err_msg))
      {
        /*
         * Value of filename parameter could not be retrieved.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error (regarding \'rem\'):\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
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
      send_string_to_screen(free_text,FALSE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_VERSIONS_ID,1))
    {
      /*
       * Enter if this is a 'general versions' request.
       */

      num_handled++ ;
      print_ref_deisgn_versions();

#if LINK_PSS_LIBRARIES
/* { */
      {
        /*
         * Holds Tapi / Core versions.
         */
        GT_VERSION
          versionInfo ;
        GT_EXT_DRV_VERSION
          ext_drv_version;
        GT_APP_DEMO_VERSION
          app_demo_version;
        if (is_device_located_in_cfg_file())
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "DPSS:\r\n"
            "=====\r\n") ;
          send_string_to_screen(err_msg,FALSE) ;
          gtTapiVersion(&versionInfo) ;
          gtCoreVersion(&versionInfo) ;
          gtCommonVersion(&versionInfo) ;
          sal_sprintf(err_msg,
            "Common Ver. : %s\r\n",versionInfo.version) ;
          send_string_to_screen(err_msg,FALSE) ;
#if DPSS_VERION_1_25_INCLUDE /*{*/
          gtExtDrvVersion(&ext_drv_version);
          sal_sprintf(err_msg,
            "ExtDrv Ver. : %s\r\n",ext_drv_version.version) ;
          send_string_to_screen(err_msg,FALSE) ;
          gtAppDemoVersion(&app_demo_version);
          sal_sprintf(err_msg,
            "AppDemo Ver.: %s\r\n",app_demo_version.version) ;
          send_string_to_screen(err_msg,FALSE) ;
#endif  /*}*/
        }
      }
/* } */
#endif
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_HOST_BOARD_SN_ID,1))
    {
      /*
       * Enter if this is a 'general get_host_sn' request.
       */
      unsigned short
        host_board_serial_number;
      num_handled++ ;
      ui_ret = host_board_sn_from_nv(&host_board_serial_number);
      if (ui_ret)
      {
        sal_sprintf(
          err_msg,
          "Error: host_board_sn_from_nv() - failed."
        );
        gen_err(TRUE,FALSE,ui_ret,0,
                err_msg, proc_name,
                SVR_ERR,0,TRUE,0,-1,FALSE);
        goto exit ;
      }

      sal_sprintf(err_msg,
        "\r\n\n"
        "Host board\'s serial number: %04u.\r\n",
        host_board_serial_number
      ) ;
      send_string_to_screen(err_msg,FALSE) ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_TRACE_ID,1))
    {
      /*
       * Enter if this is a 'general trace' request.
       */
      unsigned
        int
          num_elements ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_TRACE_DISPLAY_N_CLEAR_ID,1))
      {
        /*
         * Enter if this is a 'general trace display_and_clear' request.
         */
        num_handled++ ;
        lock_trace_fifo() ;
        num_elements = get_num_elements_trace_fifo() ;
        if (num_elements == 0)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "==> Real time trace fifo is empty!\r\n"
            ) ;
          send_string_to_screen(err_msg,FALSE) ;
          unlock_trace_fifo() ;
          goto exit ;
        }
        /*
         * At this point, there must be some information on trace fifo.
         */
        send_array_to_screen("\r\n\n",3) ;
        display_trace_fifo(TRUE) ;
        unlock_trace_fifo() ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_TRACE_DISPLAY_ID,1))
      {
        /*
         * Enter if this is a 'general trace display' request.
         */
        num_handled++ ;
        lock_trace_fifo() ;
        num_elements = get_num_elements_trace_fifo() ;
        if (num_elements == 0)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "==> Real time trace fifo is empty!\r\n"
            ) ;
          send_string_to_screen(err_msg,FALSE) ;
          unlock_trace_fifo() ;
          goto exit ;
        }
        /*
         * At this point, there must be some information on trace fifo.
         */
        send_array_to_screen("\r\n\n",3) ;
        display_trace_fifo(FALSE) ;
        unlock_trace_fifo() ;
      }
      else
      {
        /*
         * Parameter following 'trace' is unrecognized.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Parameter following \'trace\' is unrecognizable\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_CLI_ID,1))
    {
      /*
       * Enter if this is a 'general cli' request.
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_CLI_DOWNLOAD_ID,1))
      {
        /*
         * Enter if this is a 'general cli download' request.
         */
        char
          *cli_file_name ;
        unsigned
          long
            host_ip ;
        char
          ascii_host_ip[4*4+2] ;
        num_handled++ ;
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_CLI_FILENAME_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of filename parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error (regarding \'filename\'):\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        cli_file_name = param_val->value.val_text ;
        err = get_cli_file_name_handle(cli_file_name,err_msg,(int *)&ui) ;
        if (err == 0)
        {
          /*
           * Enter if there already is such a file name.
           * Reject request and exit.
           */
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** subject_general():\r\n"
            "    A file by this name (%s) already exists.\r\n"
            "    Erase it before downloading.\r\n",
            cli_file_name
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }

        if (!search_param_val_pairs(current_line,
                      &match_index,PARAM_GENERAL_CLI_DOWNLOAD_HOST_IP_ID,1))
        {
          /*
           *  Downloading IP
           *  get the host id from the user.
           */

          if (!get_val_of(
                  current_line,(int *)&match_index,PARAM_GENERAL_CLI_DOWNLOAD_HOST_IP_ID,1,
                  &param_val,VAL_IP,err_msg))
          {
            sprint_ip(ascii_host_ip, param_val->value.ip_value, TRUE);
      host_ip = param_val->value.ip_value;
          }

        }
        else
        {

       /*
       * Get runtime value of downloading host ip address
       * and covert to ASCII.
       */

      err = get_run_downloading_host_addr(&host_ip) ;
      if (err)
      {
        sal_sprintf(err_msg,
        "\r\n\n"
        "*** subject_general(): Error from get_run_downloading_host_addr().\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      sprint_ip(ascii_host_ip,host_ip, TRUE) ;
    }
        sal_sprintf(err_msg,
            "\r\n\n"
            "Host file selected (%s).\r\n"
            "Use TFTP to get file from %s into local memory..."
            "\r\n",
            cli_file_name,ascii_host_ip
            ) ;
    send_string_to_screen(err_msg,FALSE) ;
        /*
         * Prepare error text in case...
         */
        sal_sprintf(err_msg,
            "\r\n"
            "*** subject_general(): Error from download_cli_file():\r\n"
            ) ;
        ui = strlen(err_msg) ;
        err = download_cli_file(cli_file_name,&err_msg[ui], host_ip) ;
        if (!err)
        {
          sal_sprintf(err_msg,
                "\r\n"
                "%s has been successfully downloaded!\r\n",
                cli_file_name) ;
        }
        else
        {
          ui_ret = TRUE ;
        }
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_CLI_ERASE_ID,1))
      {
        /*
         * Enter if this is a 'general cli erase' request.
         */
        char
          *cli_file_name ;
        num_handled++ ;
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_CLI_FILENAME_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of filename parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error (regarding \'filename\'):\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        cli_file_name = param_val->value.val_text ;
        /*
         * Prepare error text in case...
         */
        sal_sprintf(err_msg,
            "\r\n"
            "*** subject_general(): Error from get_cli_file_name_handle():\r\n"
            ) ;
        ui = strlen(err_msg) ;
        err = get_cli_file_name_handle(cli_file_name,&err_msg[ui],(int *)&ui) ;
        if (err > 0)
        {
          /*
           * Enter if there no such a file name in local system.
           * Reject request and exit.
           */
          sal_sprintf(err_msg,
            "\r\n"
            "*** subject_general():\r\n"
            "    No file by this name (%s) could be found.\r\n",
            cli_file_name
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = FALSE ;
          goto exit ;
        }
        else if (err < 0)
        {
          /*
           * Enter if get_cli_file_name_handle() returned with error indication.
           * Reject request and exit.
           */
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        /*
         * At this point, there must be a file by this name in the system.
         * Now erase the file.
         */
        /*
         * Prepare error text in case...
         */
        sal_sprintf(err_msg,
            "\r\n"
            "*** subject_general(): Error from erase_cli_file():\r\n"
            ) ;
        ui = strlen(err_msg) ;
        err = erase_cli_file(cli_file_name,&err_msg[ui]) ;
        if (!err)
        {
          sal_sprintf(err_msg,
                "\r\n\n"
                "%s has been successfully erased!",
                cli_file_name) ;
        }
        else
        {
          ui_ret = TRUE ;
        }
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_CLI_RUN_ID,1))
      {
        /*
         * Enter if this is a 'general cli run' request.
         */
        char
          *cli_file_name ;
        int
          file_handle ;

        num_handled++ ;
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_CLI_FILENAME_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of filename parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error (regarding \'filename\'):\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        cli_file_name = param_val->value.val_text ;
        /*
         * Prepare error text in case...
         */
        sal_sprintf(err_msg,
            "\r\n"
            "*** subject_general(): Error from get_cli_file_name_handle():\r\n"
            ) ;
        ui = strlen(err_msg) ;
        err = get_cli_file_name_handle(cli_file_name,&err_msg[ui],(int *)&file_handle) ;
        if (err > 0)
        {
          /*
           * Enter if there no such a file name in local system.
           * Reject request and exit.
           */
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** subject_general():\r\n"
            "    No file by this name (%s) could be found.\r\n",
            cli_file_name
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        else if (err < 0)
        {
          /*
           * Enter if get_cli_file_name_handle() returned with error indication.
           * Reject request and exit.
           */
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        /*
         * At this point, there must be a file by this name in the system.
         * Now run the whole file by setting flags that change the behaviour
         * of sys_getch().
         */
        /*
         * Prepare error text in case...
         */
        sal_sprintf(err_msg,
            "\r\n"
            "*** subject_general(): Error from start_cli_file_running():\r\n"
            ) ;
        ui = strlen(err_msg) ;
        err = start_cli_file_running((int)file_handle,&err_msg[ui]) ;
        if (err)
        {
          /*
           * Enter if file could not be made to start running.
           */
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_CLI_DISPLAY_ID,1))
      {
        /*
         * Enter if this is a 'general cli display' request.
         */
        char
          *cli_file_name ;
        int
          file_handle ;
        unsigned
          char
            *file_mem_base ;
        unsigned
          long
            file_size ;
        num_handled++ ;
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_CLI_FILENAME_ID,1,
                &param_val,VAL_TEXT,err_msg))
        {
          /*
           * Value of filename parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error\r\n"
              "   (regarding \'filename\'):\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        cli_file_name = param_val->value.val_text ;
        /*
         * Prepare error text in case...
         */
        sal_sprintf(err_msg,
            "\r\n"
            "*** subject_general(): Error from get_cli_file_name_handle():\r\n"
            ) ;
        ui = strlen(err_msg) ;
        err = get_cli_file_name_handle(cli_file_name,&err_msg[ui],(int *)&file_handle) ;
        if (err > 0)
        {
          /*
           * Enter if there no such a file name in local system.
           * Reject request and exit.
           */
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** subject_general():\r\n"
            "    No file by this name (%s) could be found.\r\n",
            cli_file_name
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        else if (err < 0)
        {
          /*
           * Enter if get_cli_file_name_handle() returned with error indication.
           * Reject request and exit.
           */
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        /*
         * At this point, there must be a file by this name in the system.
         * Now display the whole file.
         * Read directly from memory. It is more elegant to 'open' and
         * 'read' but...
         */
        err = get_cli_file_size((int)file_handle,&file_size) ;
        if (err)
        {
          /*
           * Enter if file size could not be retrieved.
           */
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** subject_general():\r\n"
            "    get_cli_file_size() returned with error %lu.\r\n",
            (unsigned long)err
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        err = get_cli_file_mem_base((int)file_handle,&file_mem_base) ;
        if (err)
        {
          /*
           * Enter if address of memory containing file could not be retrieved.
           */
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** subject_general():\r\n"
            "    get_cli_file_mem_base() returned with error %u.\r\n",
            (unsigned int)err
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        /*
         * At this point, we have file size and base address. Treat it a 'mem read'
         */
        display_memory(
            (char *)file_mem_base,(unsigned int)file_size,
            CHAR_EQUIVALENT,HEX_EQUIVALENT,
            FALSE,FALSE,0) ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_CLI_DIR_ID,1))
      {
        /*
         * Enter if this is a 'general cli dir' request.
         */
        int
          max_num ;
        num_handled++ ;
        sal_sprintf(err_msg,
            "\r\n\n"
            "General information on CLI files:\r\n"
            "=================================\r\n"
            ) ;
        send_string_to_screen(err_msg,FALSE) ;
        err =
          get_cli_file_system_info(err_msg) ;
        if (err)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Error trying to get CLI file system info\r\n"
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          goto exit ;
        }
        send_string_to_screen(err_msg,TRUE) ;
        max_num = get_max_num_cli_files() ;
        sal_sprintf(err_msg,
            "\r\n\n"
            "Directory of CLI files:\r\n"
            "=======================\r\n"
            ) ;
        send_string_to_screen(err_msg,FALSE) ;
        for (ui = 0 ; ui < (unsigned int)max_num ; ui++)
        {
          sal_sprintf(err_msg,
            "File handle --- %02u --------------->\r\n",(unsigned short)ui
            ) ;
          uj = strlen(err_msg) ;
          err = get_cli_file_info((int)ui,&err_msg[uj]) ;
          if (err)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Error trying to get CLI file info on handle %u\r\n",
              (unsigned short)ui
              ) ;
            send_string_to_screen(err_msg,TRUE) ;
            goto exit ;
          }
          send_string_to_screen(err_msg,TRUE) ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_CLI_EXEC_TIME_ID,1))
      {
        /*
         * Enter if this is a 'general cli exec_time' request.
         */
        char
          *cli_file_name ;

        num_handled++ ;
        if (are_cli_times_available())
        {
          unsigned
            long
              elapsed,
              ten_ms,
              seconds ;
          err = get_cli_elapsed_time(&elapsed,&cli_file_name) ;
          if (err)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Error trying to get execution time of last CLI file\r\n"
              ) ;
            send_string_to_screen(err_msg,TRUE) ;
            goto exit ;
          }
          seconds = elapsed / 100 ;
          ten_ms = elapsed - (seconds * 100) ;
          sal_sprintf(err_msg,
            "\r\n\n"
            "Execution time of last CLI file (%s): %lu.%02lu seconds\r\n"
            "====================================================================\r\n",
            cli_file_name,seconds,ten_ms
            ) ;
          send_string_to_screen(err_msg,FALSE) ;
        }
        else
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** No CLI-file execution time currently recorded with CLI file system\r\n"
            ) ;
          send_string_to_screen(err_msg,TRUE) ;
          goto exit ;
        }
      }
      else
      {
        /*
         * Parameter following 'cli' is unrecognized.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Parameter following \'cli\' is unrecognizable\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_CLI_ERROR_COUNTER_ID,1))
    {
      /*
       * Enter if this is a 'general cli_error_counter' request.
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_DISPLAY_ID,1))
      {
        /*
         * Enter if this is a 'general cli_error_counter display' request.
         */
        unsigned
          int
            cli_err_counter ;
        num_handled++ ;
        cli_err_counter = get_cli_err_counter() ;
        sal_sprintf(err_msg,
                "\r\n\n"
                "Accumulated CLI error counter is: 0x%08lX\r\n",
                (unsigned long)cli_err_counter) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_CLEAR_ID,1))
      {
        /*
         * Enter if this is a 'general cli_error_counter clear' request.
         */
        num_handled++ ;
        clear_cli_err_counter() ;
        sal_sprintf(err_msg,
            "\r\n\n"
            "CLI error counter has been erased.\r\n") ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        /*
         * Parameter following 'cli_error_counter' is unrecognized.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Parameter following \'cli_error_counter\' is unrecognizable\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_RUNTIME_LOG_ID,1))
    {
      /*
       * Enter if this is a 'general runtime_log' request.
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_CLEAR_ID,1))
      {
        /*
         * Enter if this is a 'general runtime_log clear' request.
         */
        num_handled++ ;
        sal_sprintf(err_msg,
            "\r\n\n"
            "This action will permanently destroy RUNTIME events log file!\r\n"
            "Hit \'space\' to reconfirm or any other key to\r\n"
            "cancel the operation ===> ") ;
        err = ask_get(
            err_msg,
            EXPECT_CONT_ABORT,FALSE,TRUE) ;
        if (err)
        {
          /*
           * Enter if user has chosen to continue with erasing the
           * block.
           */
          err = clear_runtime_log_block() ;
          if (err)
          {
            /*
             * Enter if 'clear_runtime_log_block' returned with error.
             */
            sal_sprintf(err_msg,
              "\r\n"
              "*** Error indication from \'clear_runtime_log_block\' %d.\r\n",
              err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          else
          {
            /*
             * Enter if 'clear_runtime_log_block' returned with OK.
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "RUNTIME events log file has been erased.\r\n") ;
            send_string_to_screen(err_msg,TRUE) ;
          }
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_DISPLAY_ID,1))
      {
        /*
         * Enter if this is a 'general runtime_log display' request.
         */
        num_handled++ ;
        err = display_runtime_log_block() ;
        if (err)
        {
          /*
           * Enter if 'display_runtime_log_block' returned with error.
           */
          sal_sprintf(err_msg,
            "\r\n"
            "*** Error indication from \'display_runtime_log_block\' %d.\r\n",
            err) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
      }
      else
      {
        /*
         * Parameter following 'runtime_log' is unrecognized.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Parameter following \'runtime_log\' is unrecognizable\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_FLASH_ID,1))
    {
      /*
       * Enter if this is a 'general flash' request.
       */
      if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_FLASH_SHOW_ID,1))
      {

        send_string_to_screen("", TRUE) ;
        num_handled++ ;
        /*
         * Print "flash_descriptor" information.
         */
        print_flash_descriptor_info();
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_ERASE_ID,1))
      {
        PARAM_VAL
          *app_file_param_val ;
        unsigned
          int
            app_file ;
        /*
         * Enter if this is a 'general flash erase' request.
         */
        num_handled++ ;
        if (get_val_of(
                current_line,(int *)&match_index,PARAM_GENERAL_APP_FILE_ID,1,
                &app_file_param_val,VAL_NUMERIC,err_msg))
        {
          /*
           * Value of app_file parameter could not be retrieved.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
        app_file = (unsigned int)app_file_param_val->value.ulong_value ;
        sal_sprintf(err_msg,
            "\r\n\n"
            "This (long) process will permanently destroy FLASH block no. %u (of %u)!\r\n"
            "Hit \'space\' to reconfirm or any other key to\r\n"
            "cancel the operation ===> ",
            app_file,(unsigned short)get_num_app_flash_files()) ;
        err = ask_get(
            err_msg,
            EXPECT_CONT_ABORT,FALSE,TRUE) ;
        if (err)
        {
          /*
           * Enter if user has chosen to continue with erasing the
           * block.
           */
          err = erase_program_flash((unsigned long)(app_file - 1), TRUE) ;
          if (err)
          {
            /*
             * Enter if 'erase_program_flash' returned with error.
             */
            sal_sprintf(err_msg,
              "\r\n"
              "*** Error indication from \'erase_program_flash\'. %d\r\n",
              err) ;
            send_string_to_screen(err_msg,TRUE) ;
            ui_ret = TRUE ;
            goto exit ;
          }
          else
          {
            /*
             * Enter if 'erase_program_flash' returned with OK.
             */
            set_flash_descriptor_block_not_loaded((unsigned long)(app_file - 1));
            sal_sprintf(err_msg,
                "\r\n\n"
                "FLASH block (= application file) no. %u (of %u) has been erased.\r\n",
                app_file,(unsigned short)get_num_app_flash_files()) ;
            send_string_to_screen(err_msg,TRUE) ;
          }
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_DOWNLOAD_ID,1))
      {
        char
          ip_address[MAX_IP_STRING+2]="" ;

        ui_ret = FALSE ;
        num_handled++ ;
        send_string_to_screen("", TRUE) ;
        if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_DOWNLOAD_HOST_IP_ID,1))
        {
          /*
           *  Downloading IP
           *  get the host id from the user.
           */
            if (!get_val_of(
                    current_line,(int *)&match_index,PARAM_GENERAL_DOWNLOAD_HOST_IP_ID,1,
                    &param_val,VAL_IP,err_msg))
            {
              sprint_ip(ip_address, param_val->value.ip_value, TRUE);
            }
        }
        else
        {
          /*
           *  Default downloading host IP
           */
          unsigned
            long
              ip_val = 0 ;

          ip_val = utils_ip_get_dld_host_ip();

          sprint_ip(ip_address, ip_val, TRUE);
        }
        if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_DOWNLOAD_APP_ID,1))
        {
          /*
           * Download application to the flash.
           */
          unsigned
            int
              app_i;
          for (app_i=0; app_i<get_num_app_flash_files() ; app_i++)
          {
            if (!get_flash_descriptor_block_loaded(app_i))
            {
              break;
            }
          }
          if (app_i>=get_num_app_flash_files())
          {
            /*
             * No avaliable space for application.
             */
            send_string_to_screen("---Can not download application. \r\n"
                                  "   No available space in application from flash.\r\n"
                                  "   Please erase one application and return the procedure \r\n"
                                  "   To check the flash status about applications, type:\r\n"
                                  "   'general flash show'\r\n"
                                  "   Erase an application through:\n\r"
                                  "   'general flash erase application_file 1'",TRUE) ;
            goto exit;
          }
          err = download_application_block_to_flash(app_i, ip_address);
        }
        else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_DOWNLOAD_BOOT_ID,1))
        {
          /*
           * Download boot to the flash.
           */
          sal_sprintf(err_msg,
                  "\r\n"
                  "This process will permanently destroy download boot to the FLASH!\r\n"
                  "Hit \'space\' to reconfirm or any other key to\r\n"
                  "cancel the operation ===> ") ;
          err = ask_get(
            err_msg,
            EXPECT_CONT_ABORT,FALSE,TRUE) ;
          if (err)
          {
#ifndef SAND_LOW_LEVEL_SIMULATION
            err = download_boot_block_to_flash(ip_address);
#endif
          }
        }
        else
        {
          /*
           * Error.
           */
          send_string_to_screen(
              "\r\n\n"
              "*** Parameter following \'flash download\' is unrecognizable\r\n",TRUE) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
      }
      else if(UI_MACROS_MATCH_ID(PARAM_GENERAL_FLASH_DFFS_ID))
      {
        char
          *file_name = "",
          *comment = "",
          *date = "";
        unsigned long
          numeric_val = 0;
        DFFS_FORMAT_TYPE
          format_type = DFFS_SHORT_FORMAT;

        ui_ret = FALSE ;
        num_handled++ ;

        if (UI_MACROS_MATCH_ID(PARAM_GENERAL_FLASH_DFFS_DIR_ID))
        {
          soc_sand_proc_name = "utils_dffs_cpu_dir";

          UI_MACROS_GET_SYMBOL_VAL(PARAM_GENERAL_FLASH_DFFS_DIR_FORMAT_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_LONG_VAL(numeric_val);

          if (numeric_val == LONG_EQUIVALENT)
          {
            format_type = DFFS_LONG_FORMAT;
          }

          err =
            utils_dffs_cpu_dir(
              format_type,
              FALSE
            );
        }
        else if (UI_MACROS_MATCH_ID(PARAM_GENERAL_FLASH_DFFS_DEL_ID))
        {
          soc_sand_proc_name = "utils_dffs_cpu_delete_file";

          UI_MACROS_GET_TEXT_VAL(PARAM_GENERAL_FLASH_DFFS_NAME_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_TEXT_VAL(file_name);

          err =
            utils_dffs_cpu_delete_file(
              file_name,
              FALSE
            );
        }
        else if (UI_MACROS_MATCH_ID(PARAM_GENERAL_FLASH_DFFS_DLD_ID))
        {
          soc_sand_proc_name = "utils_dffs_cpu_dld_and_write";

          UI_MACROS_GET_TEXT_VAL(PARAM_GENERAL_FLASH_DFFS_NAME_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_TEXT_VAL(file_name);

          err =
            utils_dffs_cpu_dld_and_write
            (
              file_name,
              FALSE
            );
        }
        else if (UI_MACROS_MATCH_ID(PARAM_GENERAL_FLASH_DFFS_PRINT_ID))
        {
          soc_sand_proc_name = "utils_dffs_cpu_print_file";

          UI_MACROS_GET_TEXT_VAL(PARAM_GENERAL_FLASH_DFFS_NAME_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_TEXT_VAL(file_name);

          UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_FLASH_DFFS_PRINT_SIZE_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_LONG_VAL(numeric_val);

          err =
            utils_dffs_cpu_print_file
            (
              file_name,
              numeric_val,
              FALSE
            );
        }
        else if (UI_MACROS_MATCH_ID(PARAM_GENERAL_FLASH_DFFS_ADD_COMMENT_ID))
        {
          soc_sand_proc_name = "utils_dffs_cpu_set_comment";

          UI_MACROS_GET_TEXT_VAL(PARAM_GENERAL_FLASH_DFFS_NAME_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_TEXT_VAL(file_name);

          UI_MACROS_GET_TEXT_VAL(PARAM_GENERAL_FLASH_DFFS_COMMENT_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_TEXT_VAL(comment);

          err =
            utils_dffs_cpu_set_comment(
              file_name,
              comment,
              FALSE
            );
        }
        else if (UI_MACROS_MATCH_ID(PARAM_GENERAL_FLASH_DFFS_SHOW_COMMENT_ID))
        {
           soc_sand_proc_name = "utils_dffs_cpu_set_comment";

          UI_MACROS_GET_TEXT_VAL(PARAM_GENERAL_FLASH_DFFS_NAME_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_TEXT_VAL(file_name);

          err =
            utils_dffs_cpu_print_comment(
              file_name,
              FALSE
            );
        }
        else if (UI_MACROS_MATCH_ID(PARAM_GENERAL_FLASH_DFFS_ADD_ATTR_ID))
        {
          soc_sand_proc_name = "set_attribute";

          UI_MACROS_GET_TEXT_VAL(PARAM_GENERAL_FLASH_DFFS_NAME_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_TEXT_VAL(file_name);


          soc_sand_proc_name = "utils_dffs_cpu_set_date";

          UI_MACROS_GET_TEXT_VAL(PARAM_GENERAL_FLASH_DFFS_DATE_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_TEXT_VAL(date);

          err =
            utils_dffs_cpu_set_date(
            file_name,
            date,
            FALSE
          );


          soc_sand_proc_name = "utils_dffs_cpu_set_version";

          UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_FLASH_DFFS_VER_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_LONG_VAL(numeric_val);

          err =
            utils_dffs_cpu_set_version(
            file_name,
            (unsigned char)numeric_val,
            FALSE
          );

          soc_sand_proc_name = "utils_dffs_cpu_set_attr";

          UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_FLASH_DFFS_ATTR1_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_LONG_VAL(numeric_val);

          err =
            utils_dffs_cpu_set_attr(
            file_name,
            (unsigned char)numeric_val,
            1,
            FALSE
          );

          soc_sand_proc_name = "utils_dffs_cpu_set_attr";

          UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_FLASH_DFFS_ATTR2_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_LONG_VAL(numeric_val);

          err =
            utils_dffs_cpu_set_attr(
            file_name,
            (unsigned char)numeric_val,
            2,
            FALSE
          );

          soc_sand_proc_name = "utils_dffs_cpu_set_attr";

          UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_FLASH_DFFS_ATTR3_ID);
          UI_MACROS_CHECK_GET_VAL_OF_ERROR;
          UI_MACROS_LOAD_LONG_VAL(numeric_val);

          err =
            utils_dffs_cpu_set_attr(
            file_name,
            (unsigned char)numeric_val,
            3,
            FALSE
            );
        }
        else if(UI_MACROS_MATCH_ID(PARAM_GENERAL_FLASH_DFFS_DIAGNOSTICS_ID))
        {
          soc_sand_proc_name = "utils_dffs_cpu_set_attr";

          err =
              utils_dffs_cpu_diagnostics(
              FALSE
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
         * Parameter following 'flash' is unrecognized.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Parameter following \'flash\' is unrecognizable\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_PERIODIC_SUSPEND_TEST_ID,1))
    {
      err = periodic_suspend_test() ;
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_GET_BOARD_INFO,1))
    {
      err = utils_print_board_info() ;
      goto exit ;
    }

    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_TIME_FROM_STARTUP_ID,1))
    {
      PARAM_VAL
        *show_param_val ;
      unsigned
        int
          show ;
      /*
       * Enter if this is a 'general time_from_startup' request.
       */
      num_handled++ ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GENERAL_SHOW_ID,1,
              &show_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of show parameter could not be retrieved.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      show = (unsigned int)show_param_val->numeric_equivalent ;
      err = display_time_from_startup(show) ;
      if (err)
      {
        /*
         * Enter if 'display_time_from_startup' returned with error.
         */
        sal_sprintf(err_msg,
          "\r\n"
          "*** Error (%d) from \'display_time_from_startup\'.\r\n"
          "    Probably software error...\r\n",
          err) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_TEMPERATURE_ID,1))
    {
      PARAM_VAL *show_param_val, *param_val_sensor;
      unsigned int show, sensor;
      unsigned short  card_type;
      /*
       * Enter if this is a 'general temperature' request.
       */
      num_handled++;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GENERAL_TEMPERATURE_ID,1,
              &param_val_sensor,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of show parameter could not be retrieved.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GENERAL_SHOW_ID,1,
              &show_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of show parameter could not be retrieved.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      show = (unsigned int)show_param_val->numeric_equivalent;
      sensor = (unsigned int)param_val_sensor->numeric_equivalent;
      host_board_type_from_nv(&card_type);

      switch(sensor)
      {
        case (0): /* mezzanine */
        {
          send_string_to_screen("\n\r",TRUE);
          err = display_temperature(1, show);
          break;
        }
        case (1): /* host board 1 */
        {
          if (!get_slot_id() || SOC_UNKNOWN_CARD == card_type || STANDALONE_MEZZANINE == card_type)
          {
            /* this is a standalone board (slot_id == 0) */
            send_string_to_screen(
                "\r\n\n"
                "*** This parameter is only relevant for Fabric Card or Line card\r\n",TRUE) ;
            ui_ret = FALSE ;
            goto exit;
          }
          send_string_to_screen("\n\r",TRUE);
          err = display_temperature(2, show) ;
          break;
        }
        case (2): /* host board 2 */
        {
          if ( (!get_slot_id())            ||
               (SOC_UNKNOWN_CARD == card_type) ||
               (STANDALONE_MEZZANINE == card_type) ||
               (FABRIC_CARD_01 == card_type)
             )
          {
            /* this is a standalone board or fabric card */
            send_string_to_screen(
                "\r\n\n"
                "*** This parameter is only relevant for Line cards\r\n",TRUE) ;
            ui_ret = FALSE ;
            goto exit;
          }
          send_string_to_screen("\n\r",TRUE);
          err = display_temperature(3, show);
          break;
        }
        case (3): /* all sensors */
        {
          send_string_to_screen("\n\r",TRUE);

          if ((!get_slot_id())            ||
              (SOC_UNKNOWN_CARD == card_type) ||
              (STANDALONE_MEZZANINE == card_type)
             )
          {
            send_string_to_screen("Mezzanine thermometer info:",TRUE);
            err = display_temperature(1, show);
            break;
          }
          /*
           * if we got here -> this is not a stand alone card
           * so we cannot display periodic (more then 1 sensor)
           */
          if(show != METHOD_REGULAR_DISPLAY)
          {
            send_string_to_screen(
                "\r\n\n"
                "*** More then one sensor - can not use periodic display",FALSE);
          }
          if(FABRIC_CARD_01 == card_type)
          {
            send_string_to_screen("Mezzanine thermometer info:",TRUE);
            err  = display_temperature(1, METHOD_REGULAR_DISPLAY);
            send_string_to_screen("Fabric card thermometer info:",TRUE);
            err |= display_temperature(2, METHOD_REGULAR_DISPLAY);
            break;
          }
          if(FABRIC_CARD_FE600_01 == card_type)
          {
            send_string_to_screen("Mezzanine thermometer info:",TRUE);
            err  = display_temperature(1, METHOD_REGULAR_DISPLAY);
            send_string_to_screen("Fabric card MAX1617 Local :",TRUE);
            err |= display_temperature(2, METHOD_REGULAR_DISPLAY);
            send_string_to_screen("Fabric card MAX1617 Remote:",TRUE);
            err |= display_temperature(3, METHOD_REGULAR_DISPLAY);
            break;
          }
          if((LINE_CARD_01 == card_type) ||
             (LINE_CARD_FAP10M_B == card_type)
            )
          {
            send_string_to_screen("Mezzanine thermometer info:",TRUE);
            err  = display_temperature(1, METHOD_REGULAR_DISPLAY);
            send_string_to_screen("Line Card FAP-10M:",TRUE);
            err |= display_temperature(2, METHOD_REGULAR_DISPLAY);
            send_string_to_screen("Line Card power block DC/DC:",TRUE);
            err |= display_temperature(3, METHOD_REGULAR_DISPLAY);
            break;
          }
          if (
              (LINE_CARD_GFA_FAP20V == card_type) ||
              (LINE_CARD_GFA_FAP21V == card_type)
             )
          {
            send_string_to_screen("Mezzanine thermometer info:",TRUE);
            err  = display_temperature(1, METHOD_REGULAR_DISPLAY);
            send_string_to_screen("Line Card FAP-20V:",TRUE);
            err |= display_temperature(THERMOMETER_LINECARD_1_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
            send_string_to_screen("Line Card power block DC/DC:",TRUE);
            err |= display_temperature(THERMOMETER_LINECARD_2_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
            if (front_does_card_exist_app())
            {
              /*
               * Check FTG/TEVB also
               */
             send_string_to_screen("Front end thermometer info:\n\r",TRUE);
             err |= display_temperature(THERMOMETER_FRONTEND_3_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
#if LINK_TIMNA_LIBRARIES
             if (
               tevb_is_timna_connected()
               )
             {
               send_string_to_screen("Front end 2nd thermometer info (silicon):\n\r",TRUE);
               err |= display_temperature(THERMOMETER_FRONTEND_4_ARR_LOCATION, METHOD_REGULAR_DISPLAY);

               send_string_to_screen("Front end 2nd thermometer info (ambient):\n\r",TRUE);
               err |= display_temperature(THERMOMETER_FRONTEND_5_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
             }
#endif /*#if LINK_TIMNA_LIBRARIES*/
              }
              break;
            }
           if (bsp_card_is_same_family(card_type,LINE_CARD_GFA_PETRA_X))
            {
              send_string_to_screen("Mezzanine thermometer info:",TRUE);
              err  = display_temperature(1, METHOD_REGULAR_DISPLAY);
              send_string_to_screen("Line Card Soc_petra:",TRUE);
              err |= display_temperature(THERMOMETER_LINECARD_1_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
              send_string_to_screen("MAX1617 Remote:",TRUE);
              err |= display_temperature(THERMOMETER_LINECARD_2_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
              send_string_to_screen("MAX1617 Local :",TRUE);
              err |= display_temperature(THERMOMETER_LINECARD_3_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
              if (front_does_card_exist_app())
              {
                /*
                * Check FTG/TEVB also
                */
                send_string_to_screen("Front end thermometer info:\n\r",TRUE);
                err |= display_temperature(THERMOMETER_FRONTEND_3_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
#if LINK_TIMNA_LIBRARIES
             if (
              tevb_is_timna_connected()
              )
              {
                send_string_to_screen("Front end 2nd thermometer info (silicon):\n\r",TRUE);
                err |= display_temperature(THERMOMETER_FRONTEND_4_ARR_LOCATION, METHOD_REGULAR_DISPLAY);

                send_string_to_screen("Front end 3nd thermometer info (ambient):\n\r",TRUE);
                err |= display_temperature(THERMOMETER_FRONTEND_5_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
              }
#endif /*#if LINK_TIMNA_LIBRARIES*/
            }
            break;
          }
          if (LINE_CARD_TEVB == card_type)
          {

            /*
             * Check FTG/TEVB also
             */
            send_string_to_screen("Front end thermometer info:\n\r",TRUE);
            err |= display_temperature(THERMOMETER_LINECARD_1_ARR_LOCATION, METHOD_REGULAR_DISPLAY);

            send_string_to_screen("Front end 2nd thermometer info (silicon):\n\r",TRUE);
            err |= display_temperature(THERMOMETER_LINECARD_2_ARR_LOCATION, METHOD_REGULAR_DISPLAY);

            break;
          }
          if ((LINE_CARD_GFA_MB_FAP20V == card_type) || (LINE_CARD_GFA_MB_FAP21V == card_type))
          {
            send_string_to_screen("Mezzanine thermometer info:",TRUE);
            err  = display_temperature(1, METHOD_REGULAR_DISPLAY);
#if LINK_FAP20V_LIBRARIES

            if (gfa_is_db_exist_on_board())
            {
              send_string_to_screen("DB thermometer info:",TRUE);
              err  |= display_temperature(THERMOMETER_DB_1_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
            }
#endif /*LINK_FAP20V_LIBRARIES*/

#if LINK_FAP21V_LIBRARIES
            if (gfa_fap21v_is_db_exist_on_board())
                  {
                    send_string_to_screen("DB thermometer info:",TRUE);
                    err  |= display_temperature(THERMOMETER_DB_1_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
                  }
#endif /*LINK_FAP21V_LIBRARIES*/
#if LINK_FAP20V_LIBRARIES

            if (gfa_is_fap20v_exist_on_board())
            {
              send_string_to_screen("Line Card FAP-20V:",TRUE);
              err |= display_temperature(THERMOMETER_LINECARD_1_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
            }
#endif /*LINK_FAP20V_LIBRARIES*/

#if LINK_FAP21V_LIBRARIES
      else if (gfa_fap21v_is_db_exist_on_board())
            {
              send_string_to_screen("Line Card FAP-20V:",TRUE);
              err |= display_temperature(THERMOMETER_LINECARD_1_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
            }
#endif /*LINK_FAP21V_LIBRARIES*/

            send_string_to_screen("Line Card power block DC/DC:",TRUE);
            err |= display_temperature(THERMOMETER_LINECARD_2_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
            if (front_does_card_exist_app())
            {
              /*
               * Check FTG also
               */
             send_string_to_screen("Front end thermometer info:\n\r",TRUE);
             err |= display_temperature(THERMOMETER_FRONTEND_3_ARR_LOCATION, METHOD_REGULAR_DISPLAY);
            }
          }
        }
      }
      if (err)
      {
        /*
         * Enter if 'display_temperature' returned with error.
         */
        sal_sprintf(err_msg,
          "\r\n"
          "*** Error (%d) from \'display_temperature\'.\r\n"
          "    Probably software error...\r\n", err);
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE;
        goto exit;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_WATCHDOG_ID,1))
    {
      PARAM_VAL
        *watchdog_param_val ;
      unsigned
        int
          watchdog_action ;
      /*
       * Enter if this is a 'general watchdog' request.
       */
      num_handled++ ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GENERAL_WATCHDOG_ID,1,
              &watchdog_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of show parameter could not be retrieved.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      watchdog_action = (unsigned int)watchdog_param_val->numeric_equivalent ;
      switch (watchdog_action)
      {
        case STOP_RESET_EQUIVALENT:
        {
          load_do_watchdog_reset(FALSE) ;
          if (is_watchdog_active())
          {
            send_string_to_screen(
              "\r\n\n"
              "=== Watchdog reset stopped. Expecting NMI",TRUE) ;
          }
          else
          {
            send_string_to_screen(
              "\r\n\n"
              "=== Watchdog reset stopped. Watchdog not active now, anyway...",TRUE) ;
          }
          send_string_to_screen(err_msg,TRUE) ;
          break ;
        }
        case INVOKE_HANDLER_EQUIVALENT:
        {
#ifndef SAND_LOW_LEVEL_SIMULATION
          ESFPPC
            esfppc ;
          esfppc.regSet.pc = 0x12345678 ;
          err = set_watchdog_inject_time() ;
          if (err)
          {
            send_string_to_screen(
                "\r\n\n"
                "*** Procedure \'set_watchdog_inject_time\' returned with error indication:\r\n",TRUE) ;
          }
          private_delay(100000) ;
          watchdog_handler(&esfppc) ;
          d_taskUnsafe() ;
          taskDelay(10) ;
          d_taskSafe() ;
#endif /*SAND_LOW_LEVEL_SIMULATION*/
          break ;
        }
        default:
        {
          /*
           * Enter if this is an unknown watchdog action parameter.
           */
          sal_sprintf(err_msg,
            "\r\n"
            "*** Unknown \'watchdog_action\'.\r\n"
            "    Probably software error...\r\n"
          ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_EXERCISES_ID,1))
    {
      PARAM_VAL
        *exercises_param_val ;
      unsigned
        int
          exercises_action ;
      /*
       * Enter if this is a 'general exercises' request.
       */
      num_handled++ ;
      if (get_val_of(
              current_line,(int *)&match_index,PARAM_GENERAL_EXERCISES_ID,1,
              &exercises_param_val,VAL_SYMBOL,err_msg))
      {
        /*
         * Value of show parameter could not be retrieved.
         */
        send_string_to_screen(
            "\r\n\n"
            "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
        send_string_to_screen(err_msg,TRUE) ;
        ui_ret = TRUE ;
        goto exit ;
      }
      exercises_action = (unsigned int)exercises_param_val->numeric_equivalent ;
      send_array_to_screen("\r\n\n",TRUE) ;
      d_taskDelay(1) ;
      switch (exercises_action)
      {
        static
          int
            long_print_tid = 0 ;


    case INIT_FPGA_REAL_TIME_COLLETION:
        {
#if LINK_FAP20V_LIBRARIES
          send_string_to_screen(
            "\r\n\n"
            "===> Call ftg_initialize_ftg_col()",
            TRUE) ;
          ftg_initialize_ftg_col(1);
#endif /*LINK_FAP20V_LIBRARIES*/
          break ;
        }
        case CALL_FPGA_COLLETION_ISR:
        {
#if LINK_FAP20V_LIBRARIES
          send_string_to_screen(
            "\r\n\n"
            "===> Call fpga_all_irq_handler()",
            TRUE) ;
          fpga_all_irq_handler();
#endif /*LINK_FAP20V_LIBRARIES*/
          break ;
        }

        case START_FPGA_A_COLLETION_PROCESS:
        {
#if LINK_FAP20V_LIBRARIES
          unsigned
            long
              mask_of_streams[1],
              mask_of_ports[2] ;
          send_string_to_screen(
            "\r\n\n"
            "===> Call ftg_start_ftg_col(0)",
            TRUE) ;
          mask_of_streams[0] = 0xFFFFFFFF ;
          mask_of_ports[0] = 0xFFFFFFFF ;
          mask_of_ports[1] = 0xFFFFFFFF ;
          ftg_start_ftg_col(FPGA_A,mask_of_streams,mask_of_ports,TRUE) ;
#endif /*LINK_FAP20V_LIBRARIES*/
          break ;
        }
        case START_FPGA_B_COLLETION_PROCESS:
        {
#if LINK_FAP20V_LIBRARIES
          unsigned
            long
              mask_of_streams[1],
              mask_of_ports[2] ;
          send_string_to_screen(
            "\r\n\n"
            "===> Call ftg_start_ftg_col(1)",
            TRUE) ;
          mask_of_streams[0] = 0xFFFFFFFF ;
          mask_of_ports[0] = 0xFFFFFFFF ;
          mask_of_ports[1] = 0xFFFFFFFF ;
          ftg_start_ftg_col(FPGA_B,mask_of_streams,mask_of_ports,TRUE) ;
#endif /*LINK_FAP20V_LIBRARIES*/
          break ;
        }



    case STOP_FPGA_A_COLLETION_PROCESS:
        {
#if LINK_FAP20V_LIBRARIES
          send_string_to_screen(
            "\r\n\n"
            "===> Call ftg_stop_ftg_col(0)",
            TRUE) ;
          ftg_stop_ftg_col(FPGA_A,TRUE,TRUE);
#endif /*LINK_FAP20V_LIBRARIES*/
          break ;
        }
        case STOP_FPGA_B_COLLETION_PROCESS:
        {
#if LINK_FAP20V_LIBRARIES
          send_string_to_screen(
            "\r\n\n"
            "===> Call ftg_stop_ftg_col(1)",
            TRUE) ;
          ftg_stop_ftg_col(FPGA_B,TRUE,TRUE);
#endif /*LINK_FAP20V_LIBRARIES*/
          break ;
        }


    case STOP_FPGA_FULL_COL_SCENARIO:
        {
#if LINK_FAP20V_LIBRARIES
          /*
           * Set clock to 100 Mhz. (Just inform the collection
           * software package.
           */
          unsigned
            long
              mask_of_streams[1],
              mask_of_ports[2] ;
          set_fpga_clock_in_mhz(FPGA_A,100) ;
          set_fpga_clock_in_mhz(FPGA_B,100) ;
          /*
           * Initialize all FPGAs.
           */
          ftg_initialize_ftg_col(TRUE) ;
          /*
           * Make sure tracing is being carried on.
           */
          set_ftg_col_trace(TRUE) ;
          /*
           * Set collection rate to 200 ms.
           */
          logic_set_ftg_col_rate_in_clocks(FPGA_A,20000000) ;
          logic_set_ftg_col_rate_in_clocks(FPGA_B,20000000) ;
          /*
           * Start the collection process.
           */
          send_string_to_screen(
            "\r\n\n"
            "===> Call ftg_start_ftg_col(FPGA_A)",
            TRUE) ;
          mask_of_streams[0] = 0xFFFFFFFF ;
          mask_of_ports[0] = 0xFFFFFFFF ;
          mask_of_ports[1] = 0xFFFFFFFF ;
          ftg_start_ftg_col(FPGA_A,mask_of_streams,mask_of_ports,TRUE) ;
          send_string_to_screen(
            "\r\n\n"
            "===> Call ftg_start_ftg_col(FPGA_B)",
            TRUE) ;
          mask_of_streams[0] = 0xFFFFFFFF ;
          mask_of_ports[0] = 0xFFFFFFFF ;
          mask_of_ports[1] = 0xFFFFFFFF ;
          ftg_start_ftg_col(FPGA_B,mask_of_streams,mask_of_ports,TRUE) ;
          /*
           * Wait about 1 second.
           */
          elapse_periodic() ;
          /*
           * Stop the collection process.
           */


      send_string_to_screen(
            "\r\n\n"
            "===> Call ftg_stop_ftg_col(FPGA_A)",
            TRUE) ;
          ftg_stop_ftg_col(FPGA_A,TRUE,TRUE);
          send_string_to_screen(
            "\r\n"
            "===> Call ftg_stop_ftg_col(FPGA_B)",
            TRUE) ;
          ftg_stop_ftg_col(FPGA_B,TRUE,TRUE);


      display_trace_fifo(FALSE) ;
#endif /*LINK_FAP20V_LIBRARIES*/
          break ;
        }

        case CAUSE_SOFTWARE_EMULATION_EXCEPTION_EQUIVALENT:
        {
          send_string_to_screen(
            "\r\n\n"
            "===> Call cause_software_emulation_exception()",
            TRUE) ;
          cause_software_emulation_exception();
          break ;
        }
        case CALL_SOFTWARE_EMULATION_EXCEPTION_HANDLER_EQUIVALENT:
        {
#ifndef SAND_LOW_LEVEL_SIMULATION
         ESFPPC
            esfppc ;
          send_string_to_screen(
            "\r\n\n"
            "===> Call soft_emu_exp_handler()",
            TRUE) ;
          esfppc.regSet.pc = 0x5678;
          /*
           * Indicator for software testing of that the caller
           * is a tester, and the handler will not reboot.
           * soft_emu_exp_handler() - set it to FALSE.
           */
          set_software_emulation_exp_testing(TRUE) ;
          soft_emu_exp_handler(&esfppc) ;
#endif /*SAND_LOW_LEVEL_SIMULATION*/
            break ;
        }
        case CALL_ALIGNMENT_EXCEPTION_HANDLER_EQUIVALENT:
        {
#ifndef SAND_LOW_LEVEL_SIMULATION
          ESFPPC
            esfppc ;
          esfppc.regSet.pc = 0x1234;
          send_string_to_screen(
            "\r\n\n"
            "===> Call alignment_exception_handler()",
            TRUE) ;
          /*
           * Indicator for software testing of that the caller
           * is a tester, and the handler will not reboot.
           * alignment_exception_handler() - set it to FALSE.
           */
          set_software_emulation_exp_testing(TRUE) ;
          alignment_exception_handler(&esfppc) ;
#endif /*SAND_LOW_LEVEL_SIMULATION*/
          break ;
        }
        case CALL_DSI_EXCEPTION_HANDLER_EQUIVALENT:
        {
#ifndef SAND_LOW_LEVEL_SIMULATION
          ESFPPC
            esfppc ;
          esfppc.regSet.pc = 0xAAAA;
          send_string_to_screen(
            "\r\n\n"
            "===> Call data_tlb_error_handler()",
            TRUE) ;
          /*
           * Indicator for software testing of that the caller
           * is a tester, and the handler will not reboot.
           * alignment_exception_handler() - set it to FALSE.
           */
          set_software_emulation_exp_testing(TRUE) ;
          data_tlb_error_handler(&esfppc) ;
#endif /*SAND_LOW_LEVEL_SIMULATION*/
          break ;
        }
        case CALL_ISI_EXCEPTION_HANDLER_EQUIVALENT:
        {
#ifndef SAND_LOW_LEVEL_SIMULATION
          ESFPPC
            esfppc ;
          esfppc.regSet.pc = 0xBBBB;
          send_string_to_screen(
            "\r\n\n"
            "===> Call inst_tlb_error_handler()",
            TRUE) ;
          /*
           * Indicator for software testing of that the caller
           * is a tester, and the handler will not reboot.
           * alignment_exception_handler() - set it to FALSE.
           */
          set_software_emulation_exp_testing(TRUE) ;
          inst_tlb_error_handler(&esfppc) ;
#endif /*SAND_LOW_LEVEL_SIMULATION*/
          break ;
        }
        case CALL_MAIN_DPSS_INTERRUPT_EQUIVALENT:
        {
#if LINK_PSS_LIBRARIES
          send_string_to_screen(
            "\r\n\n"
            "===> Call interruptMainSR()...\r\n",
            FALSE) ;
          set_inject_cause() ;
          set_simulate_rx_counter(1) ;
          set_simulated_device(0) ;
          interruptMainSR(0) ;
#else
          send_string_to_screen(
             "===> This operation can not be carried out since PSS is not linked in.\r\n"
             "     Please define LINK_PSS_LIBRARIES in dune_rpc.h\r\n"
             "     to 1 to activate calls to PSS.",TRUE) ;
#endif
          break ;
        }
        case LOAD_REAL_TIME_TRACE_EQUIVALENT:
        {
          char
            info_text[TRACE_EVENT_TEXT_SIZE] ;
          unsigned
            long
              times[10] ;
          send_string_to_screen(
            "\r\n\n"
            "===> Load real-time trace fifo with 10 events...\r\n",
            FALSE) ;
          for (ui = 0 ; ui < 10 ; ui++)
          {
            times[ui] = get_watchdog_time(0) ;
            sal_sprintf(info_text,"Event %04u",ui) ;
            put_in_trace_fifo(100 + ui,strlen(info_text),info_text,0,0,0,0) ;
            d_taskDelay(1) ;
          }
          send_string_to_screen("===> Measured times:\r\n\n",FALSE) ;
          for (ui = 0 ; ui < 10 ; ui++)
          {
            sal_sprintf(err_msg,"Event %u: At time %lu microseconds from startup\r\n",ui,times[ui]) ;
            send_string_to_screen(err_msg,FALSE) ;
          }
          break ;
        }
        case EVENT_PRINT_TASK_ON_EQUIVALENT:
        {
#ifndef SAND_LOW_LEVEL_SIMULATION
          if (long_print_tid == 0)
          {
            send_string_to_screen(
                "\r\n\n"
                "=== Expect independent task doing long print every 10 seconds (Just exercise).\r\n",
                FALSE) ;
            long_print_tid =
              taskSpawn(
                "dLongPrint",160,0,STACK_SIZE,(FUNCPTR)long_print,
                0,0,0,0,0,0,0,0,0,0) ;
            if (long_print_tid == ERROR)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Failed to spawn long print task...\r\n\n",
                FALSE) ;
            }
          }
          else
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Long print task has already been started...\r\n\n",
              FALSE) ;
          }
#endif /* SAND_LOW_LEVEL_SIMULATION */
          break ;
        }
        case EVENT_PRINT_TASK_OFF_EQUIVALENT:
        {
#ifndef SAND_LOW_LEVEL_SIMULATION
          STATUS
            status ;
          send_string_to_screen(
              "\r\n\n"
              "=== Stop long print task\r\n",
              TRUE) ;
          if (long_print_tid == 0)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Nothing to stop: Long print task has not been started...\r\n\n",
              FALSE) ;
          }
          else
          {
            status = taskDelete(long_print_tid) ;
            if (status == ERROR)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Failed to delete long print task...\r\n\n",
                FALSE) ;
            }
            else
            {
              long_print_tid = 0 ;
            }
          }
#endif /* SAND_LOW_LEVEL_SIMULATION */
          break ;
        }
        case EVENT_DOUBLE_EQUIVALENT:
        {
          sal_sprintf(err_msg,
              "Double ID via general exercises for gen_err") ;
          gen_err(FALSE,TRUE,0,0,err_msg,
              proc_name,SVR_WRN,EXERCISES_ERR_06,
              FALSE,0,-1,FALSE) ;
          gen_err(FALSE,TRUE,0,0,err_msg,
              proc_name,SVR_WRN,BIT(ACTIVE_EVENT_BIT) | EXERCISES_ERR_06,
              FALSE,0,-1,FALSE) ;
          send_string_to_screen(
              "\r\n\n"
              "=== Expect three records in NVRAM log and on display\r\n",
              TRUE) ;
          d_taskDelay(100) ;
          break ;
        }
        case FATAL_ERROR_EQUIVALENT:
        {
          sal_sprintf(err_msg,
              "Fatal error via general exercises for gen_err") ;
          gen_err(FALSE,TRUE,0,0,err_msg,
              proc_name,SVR_FTL,EXERCISES_ERR_01,
              TRUE,0,-1,FALSE) ;
          send_string_to_screen(
              "\r\n\n"
              "=== Expect reset fatal error record in NVRAM log\r\n",
              TRUE) ;
          d_taskDelay(100) ;
          break ;
        }
        case EVENT_STORM_EQUIVALENT:
        {
          for (ui = 0 ; ui < 10 ; ui++)
          {
            sal_sprintf(err_msg,
              "Message related to general exercises for gen_err - %u",ui) ;
            gen_err(FALSE,TRUE,0,0,err_msg,
              proc_name,SVR_ERR,EXERCISES_ERR_01,
              FALSE,0,-1,FALSE) ;
          }
          d_taskDelay(62) ;
          sal_sprintf(err_msg,
            "Message related to general exercises for gen_err - %u",ui) ;
          gen_err(FALSE,TRUE,0,0,err_msg,
            proc_name,SVR_ERR,EXERCISES_ERR_01,
            FALSE,0,0,FALSE) ;
          send_string_to_screen(
              "\r\n\n"
              "=== Expect to see, above, first and last message of a storm of 11\r\n"
              "=== withing about 1 second",
              TRUE) ;
          break ;
        }
        case EVENT_ALARM_ON_EQUIVALENT:
        {
          send_array_to_screen("\r\n\n",3) ;
          sal_sprintf(err_msg,
              "Message related to sending alarm message") ;
          gen_err(FALSE,TRUE,0,0,err_msg,
              proc_name,SVR_ERR,EXERCISES_ERR_01,
              FALSE,SET_ALARM,-1,FALSE) ;
          send_string_to_screen(
              "\r\n\n"
              "=== Expect to red alarm led to turn on or remain on",
              TRUE) ;
          break ;
        }
        case EVENT_ALARM_OFF_EQUIVALENT:
        {
          send_array_to_screen("\r\n\n",3) ;
          sal_sprintf(err_msg,
              "Message related to sending alarm message") ;
          gen_err(FALSE,TRUE,0,0,err_msg,
              proc_name,SVR_ERR,EXERCISES_ERR_01,
              FALSE,RESET_ALARM,-1,FALSE) ;
          send_string_to_screen(
              "\r\n\n"
              "=== Expect to red alarm led to turn off if number of \'on\'\r\n"
              "=== actions is equal to number of \'off\' actions",
              TRUE) ;
          break ;
        }
        case EVENT_AGING_EQUIVALENT:
        {
          sal_sprintf(err_msg,
              "Message created for testing aging: First storm") ;
          gen_err(FALSE,TRUE,0,0,err_msg,
            proc_name,SVR_ERR,EXERCISES_ERR_04,
            FALSE,0,-1,FALSE) ;
          gen_err(FALSE,TRUE,0,0,err_msg,
            proc_name,SVR_ERR,EXERCISES_ERR_04,
            FALSE,0,-1,FALSE) ;
          sal_sprintf(err_msg,
            "Message created for testing aging: Second storm") ;
          gen_err(FALSE,TRUE,0,0,err_msg,
            proc_name,SVR_ERR,EXERCISES_ERR_05,
            FALSE,0,0,FALSE) ;
          gen_err(FALSE,TRUE,0,0,err_msg,
            proc_name,SVR_ERR,EXERCISES_ERR_05,
            FALSE,0,0,FALSE) ;
          send_string_to_screen(
              "\r\n\n"
              "=== Expect to see, above, two messages, each first of a storm of two\r\n"
              "=== fololowed by two aged out messages",
              TRUE) ;
          break ;
        }
        case ERR_WITHIN_ERR_EQUIVALENT:
        {
          sal_sprintf(err_msg,
            "Create error within error") ;
          gen_err(FALSE,TRUE,0,0,err_msg,
            proc_name,SVR_ERR | SVR_DBG_ONLY,EXERCISES_ERR_03,
            FALSE,0,0,FALSE) ;
          send_string_to_screen(
              "\r\n\n"
              "=== Expect to see, above, GEN_ERR within GEN_ERR\r\n",
              TRUE) ;
          d_taskDelay(1) ;
          break ;
        }
        case CALL_CHAR_BUFF_MODULE_TEST:
        {
          send_string_to_screen(
            "\r\n\n"
            "===> Call char_buff_test()",
            TRUE);
          /*
           * call the test function of the char buffer module
           */
          char_buff_test();
          break;
        }
        default:
        {
          /*
           * Enter if this is an unknown exercises action parameter.
           */
          sal_sprintf(err_msg,
            "\r\n"
            "*** Unknown \'exercises_action\'.\r\n"
            "    Probably software error...\r\n"
          ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ui_ret = TRUE ;
          goto exit ;
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_PETRA_TEST_CHIP_PROGRAM_LOAD_ID,1))
    {
      unsigned long
        chip_id = 0;

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_CHIP_ID_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(chip_id);
#ifdef LINK_TEST_CHIP_LIBRARY
      soc_petra_tc_ldr_load(
        chip_id,
        FALSE
      );
#else
      soc_sand_os_printf( "Test Chip is disabled. LINK_TEST_CHIP_LIBRARY is not defined\n\r");
#endif
      goto exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_DSDA_INIT,1))
    {
      unsigned long
        chip_id = 0;


      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_CHIP_ID_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(chip_id);

#ifdef LINK_TEST_CHIP_LIBRARY
      soc_petra_tc_ldr_fpga_init(silent);
      soc_petra_tc_ldr_synt_init(silent);
      soc_petra_tc_ldr_core_ddr_init(chip_id, silent);
      soc_petra_tc_ldr_dsda_init(chip_id, silent);
#else
      soc_sand_os_printf( "Test Chip is disabled. LINK_TEST_CHIP_LIBRARY is not defined\n\r");
#endif

      goto exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_GENERIC_TEST_ID,1))
    {
      unsigned long
        param0 = 0,
        param1 = 0,
        param2 = 0,
        param3 = 0,
        param4 = 0,
        param5 = 0,
        param6 = 0,
        param7 = 0,
        param8 = 0,
        param9 = 0;

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_GENERIC_PARAM_0_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(param0);

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_GENERIC_PARAM_1_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(param1);

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_GENERIC_PARAM_2_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(param2);

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_GENERIC_PARAM_3_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(param3);

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_GENERIC_PARAM_4_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(param4);

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_GENERIC_PARAM_5_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(param5);

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_GENERIC_PARAM_6_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(param6);

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_GENERIC_PARAM_7_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(param7);

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_GENERIC_PARAM_8_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(param8);

      UI_MACROS_GET_NUMMERIC_VAL(PARAM_GENERAL_GENERIC_PARAM_9_ID);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(param9);

      utils_generic_test(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9);
      goto exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_STOP_NETWORK_LEARNING,1))
    {
      dune_rpc_learn_network_stop();
      goto exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_REGRESSION_TEST_ID,1))
    {
      unsigned int
        scope = RGR_FULL_SCOPE,
        nof_tests = 0,
        err = 0;
      unsigned char
        regression_silent = FALSE;
      const RGR_TST_FUNC*
        tests_pool_addr = NULL;
      RGR_PASS_FAIL_TYPE
        test_result = RGR_PASS;

      if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_REGRESSION_ALL_ID,1))
      {
        if(!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_REGRESSION_FULL_ID,1))
        {
          scope = RGR_FULL_SCOPE;
        }
        else if(!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_REGRESSION_SHORT_ID,1))
        {
          scope = RGR_SHORT_SCOPE;
        }

        nof_tests =
          rgr_tst_get_nof_tests();

        tests_pool_addr =
          rgr_tst_get_pool_addr();

        err =
          rgr_eng_test_all(
            tests_pool_addr,
            nof_tests,
            &test_result,
            scope,
            regression_silent
          );

        if(err)
        {
          send_string_to_screen("\n\rError executing full regression test", TRUE);
        }

      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_REGRESSION_AUTOLEARN_ID,1))
      {
        if(!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_REGRESSION_FULL_ID,1))
        {
          scope = RGR_FULL_SCOPE;
        }
        else if(!search_param_val_pairs(current_line,&match_index,PARAM_GENERAL_REGRESSION_SHORT_ID,1))
        {
          scope = RGR_SHORT_SCOPE;
        }

        err =
          rgr_tst_autolearning_test(
            &test_result,
            scope,
            regression_silent
          );

        if(err)
        {
          send_string_to_screen("\n\rError executing autolearning test", TRUE);
        }
      }
      goto exit;
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
          "    At least one must be entered (e.g. \'temperature\').\r\n",
          TRUE) ;
      ui_ret = TRUE ;
      goto exit ;
    }
  }
exit:
  return (ui_ret) ;
}
/*****************************************************
*NAME
*  APP_FILE_ID_CHECKER
*TYPE: PROC
*DATE: 25/JUNE/2002
*FUNCTION:
*  Check identifier of application file as entered
*  by the user in the context of 'general flash'
*  command line context.
*  Make sure field is valid for this identifier.
*CALLING SEQUENCE:
*  app_file_id_checker(
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
  app_file_id_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  )
{
  int
    ui_ret ;
  unsigned
    int
      num_app_flash_files ;
  CURRENT_LINE
    *current_line ;
  if (partial)
  {
    /*
     * Check validity only after full field has been entered.
     */
    ui_ret = FALSE ;
    goto fese_exit ;
  }
  current_line = current_line_ptr ;
  ui_ret = TRUE ;
  err_msg[0] = 0 ;
  num_app_flash_files = get_num_app_flash_files() ;
  if (((unsigned int)data_value == 0) ||
        ((unsigned int)data_value > num_app_flash_files))
  {
    sal_sprintf(err_msg,
        "\r\n"
        "*** Input file identifier (%lu,%d) is out of bounds (should\r\n"
        "    be between 1 and %lu)\r\n",
        data_value,(int)data_value,
        (unsigned long)num_app_flash_files
        ) ;
    goto fese_exit ;
  }
  ui_ret = FALSE ;
fese_exit:
  return (ui_ret) ;
}
/*****************************************************
*NAME
*  LINE_CARD_SLOT_ID_CHECKER
*TYPE: PROC
*DATE: 10/MARCH/2004
*FUNCTION:
*  Check value of line card slot number as entered
*  by the user in the context of
*  'general demo show_fe_link' command line context.
*  Make sure field is valid for this identifier.
*CALLING SEQUENCE:
*  line_card_slot_id_checker(
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
*    Crate descriptors (See dune_rpc_auto_network_learning.h).
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
  line_card_slot_id_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  )
{
  int
    crate_type,
    err,
    ui_ret ;
  unsigned
    int
      first_line_slot,
      last_line_slot,
      match_index ;
  CURRENT_LINE
    *current_line ;
  char
    *crate_type_text ;
  PARAM_VAL
    *param_val ;
  if (partial)
  {
    /*
     * Check validity only after full field has been entered.
     */
    ui_ret = FALSE ;
    goto exit ;
  }
  current_line = current_line_ptr ;
  ui_ret = TRUE ;
  err_msg[0] = 0 ;
  crate_type_text = "UNKNOWN" ;
  err =
    get_val_of(
      current_line,(int *)&match_index,
      PARAM_GENERAL_DEMO_CRATE_TYPE_ID,1,
      &param_val,VAL_SYMBOL,err_msg
      ) ;
  if (0 != err)
  {
    sal_sprintf(err_msg,
            "\r\n\n"
            "*** \'line_card_slot_id_checker\' error: Could not find \'crate_type\' on line\r\n"
            "    or \'get_val_of()\' failed. Probably SW error\r\n"
    ) ;
    goto exit ;
  }
  crate_type = (int)(param_val->numeric_equivalent) ;
  if (crate_type < 0)
  {
    crate_type = (int)((unsigned int)get_crate_type()) ;
  }
  switch (crate_type)
  {
    case (CRATE_TEST_EVAL):   /*NEGEV*/
    case (CRATE_NEGEV_REV_B): /*NEGEV*/
    {
      first_line_slot = CSYS_FIRST_LINE_SLOT_NEGEV ;
      last_line_slot = CSYS_LAST_LINE_SLOT_NEGEV ;
      crate_type_text = "NEGEV" ;
      break ;
    }
    case(CRATE_32_PORT_40G_STANDALONE): /*GOBI*/
    case(CRATE_64_PORT_10G_STANDALONE): /*GOBI*/
    {
      first_line_slot = CSYS_FIRST_LINE_SLOT_GOBI ;
      last_line_slot = CSYS_FIRST_LINE_SLOT_GOBI ;
      crate_type_text = "GOBI" ;
      break ;
    }
    default:
    {
      sal_sprintf(err_msg,
          "\r\n\n"
          "*** \'line_card_slot_id_checker\' error: Could not identify \'crate_type\' (%lu)\r\n",
          (unsigned long)crate_type
          ) ;
      goto exit ;
    }
  }
  if (((unsigned int)data_value < first_line_slot) ||
        ((unsigned int)data_value > last_line_slot))
  {
    sal_sprintf(err_msg,
        "\r\n"
        "*** Input slot number (%lu) is out of bounds for this crate (%s)\r\n"
        "    Should be between %u and %u\r\n",
        data_value,crate_type_text,
        first_line_slot,last_line_slot
        ) ;
    goto exit ;
  }
  ui_ret = FALSE ;
exit:
  return (ui_ret) ;
}
/*****************************************************
*NAME
*  FABRIC_CARD_SLOT_ID_CHECKER
*TYPE: PROC
*DATE: 10/MARCH/2004
*FUNCTION:
*  Check value of fabric card slot number as entered
*  by the user in the context of
*  'general demo show_fe_link' command line context.
*  Make sure field is valid for this identifier.
*CALLING SEQUENCE:
*  fabric_card_slot_id_checker(
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
*    Crate descriptors (See dune_rpc_auto_network_learning.h).
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
  fabric_card_slot_id_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  )
{
  int
    crate_type,
    err,
    ui_ret ;
  unsigned
    int
      first_fabric_slot,
      last_fabric_slot,
      match_index ;
  CURRENT_LINE
    *current_line ;
  char
    *crate_type_text ;
  PARAM_VAL
    *param_val ;
  if (partial)
  {
    /*
     * Check validity only after full field has been entered.
     */
    ui_ret = FALSE ;
    goto exit ;
  }
  current_line = current_line_ptr ;
  ui_ret = TRUE ;
  err_msg[0] = 0 ;
  crate_type_text = "UNKNOWN" ;
  err =
    get_val_of(
      current_line,(int *)&match_index,
      PARAM_GENERAL_DEMO_CRATE_TYPE_ID,1,
      &param_val,VAL_SYMBOL,err_msg
      ) ;
  if (0 != err)
  {
    sal_sprintf(err_msg,
            "\r\n\n"
            "*** \'fabric_card_slot_id_checker\' error: Could not find \'crate_type\' on line\r\n"
            "    or \'get_val_of()\' failed. Probably SW error\r\n"
    ) ;
    goto exit ;
  }
  crate_type = (int)(param_val->numeric_equivalent) ;
  if (crate_type < 0)
  {
    crate_type = (int)((unsigned int)get_crate_type()) ;
  }
  switch (crate_type)
  {
    case (CRATE_TEST_EVAL):   /*NEGEV*/
    case (CRATE_NEGEV_REV_B): /*NEGEV*/
    {
      first_fabric_slot = CSYS_FIRST_FABRIC_SLOT_NEGEV ;
      last_fabric_slot = CSYS_LAST_FABRIC_SLOT_NEGEV ;
      crate_type_text = "NEGEV" ;
      break ;
    }
    case(CRATE_32_PORT_40G_STANDALONE): /*GOBI*/
    case(CRATE_64_PORT_10G_STANDALONE): /*GOBI*/
    {
      first_fabric_slot = CSYS_FIRST_FABRIC_SLOT_GOBI ;
      last_fabric_slot = CSYS_FIRST_FABRIC_SLOT_GOBI ;
      crate_type_text = "GOBI" ;
      break ;
    }
    default:
    {
      sal_sprintf(err_msg,
          "\r\n\n"
          "*** \'fabric_card_slot_id_checker\' error: Could not identify \'crate_type\' (%lu)\r\n",
          (unsigned long)crate_type
          ) ;
      goto exit ;
    }
  }
  if (((unsigned int)data_value < first_fabric_slot) ||
        ((unsigned int)data_value > last_fabric_slot))
  {
    sal_sprintf(err_msg,
        "\r\n"
        "*** Input slot number (%lu) is out of bounds for this crate (%s)\r\n"
        "    Should be between %u and %u\r\n",
        data_value,crate_type_text,
        first_fabric_slot,last_fabric_slot
        ) ;
    goto exit ;
  }
  ui_ret = FALSE ;
exit:
  return (ui_ret) ;
}
