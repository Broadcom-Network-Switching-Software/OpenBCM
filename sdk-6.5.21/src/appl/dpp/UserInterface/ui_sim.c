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
#include FAP10_MEM_H
#include <appl/diag/dpp/utils_defi.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_ram_defi.h>

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
/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include <private\timerLibP.h>
#include <shellLib.h>
#include <dbgLib.h>
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

#include <ChipSim/chip_sim_task.h>
#include <ChipSim/chip_sim_command.h>
#include <ChipSim/chip_sim_log.h>
#include <ChipSim/chip_sim_cell.h>

/* } */
#endif

#if SIMULATE_CHIP_SIM
/* {  Includes ChipSim Library.*/
/*
 * For the use of the chip sim task, for time profiling.
 */
UINT32
  ui_sim_get_time_in_microseconds(
    void
    )
{
  return get_watchdog_time(FALSE);
}


STATUS
  subject_sim(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  STATUS
    status=OK;
  char
    msg[160] ;

  send_string_to_screen("\r\n",FALSE) ;
  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'sim').
     * This should be protected against by the calling environment.
     */
    goto subject_fap10_exit;
  }
  else
  {
    /*
     * Enter if there are parameters on the line (not just 'fap10').
     */
    unsigned
      int
        match_index,
        num_handled ;
    PARAM_VAL
      *param_val ;
    int
      chip_ndx;

    num_handled = 0 ;
    if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_START_ID,1))
    {
      UINT32
        start_address[CHIP_SIM_NOF_CHIPS];
      int
        i;
      UINT32
        device_type =0 ;
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SIM_START_ID,1,
              &param_val,VAL_NUMERIC,msg))
      {
        device_type = param_val->value.ulong_value;
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }
      if (0 == device_type)
      {
        /*
         * Invoke the simulator with SOC_SAND_FE200
         */
        start_address[0] = (UINT32)FE_1_BASE_ADDR;
        start_address[1] = (UINT32)FE_2_BASE_ADDR;
        set_pointed_fe_default();

        status = chip_sim_start(
                   CHIP_SIMULATOR_TASK_NAME, CHIP_SIMULATOR_TASK_PRIO,
                   STACK_SIZE, CHIP_SIMULATOR_TASK_ID,
                   SOC_SAND_FE200, 0, start_address, Desc_fe_reg,
                   send_string_to_screen, ui_sim_get_time_in_microseconds, FALSE
                 );
      }
      else if (1 == device_type)
      {
        start_address[0] = (UINT32)FAP10M_1_BASE_ADDR;
        start_address[1] = (UINT32)FAP10M_2_BASE_ADDR;
        set_pointed_fap10m_default();

        status = chip_sim_start(
                   CHIP_SIMULATOR_TASK_NAME, CHIP_SIMULATOR_TASK_PRIO,
                   STACK_SIZE, CHIP_SIMULATOR_TASK_ID,
                   SOC_SAND_FAP10M, 0, start_address, Desc_fap10m_reg,
                   send_string_to_screen, ui_sim_get_time_in_microseconds, TRUE
                 );
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }
      sal_sprintf(msg, "chip_sim_start(...) returned with %d", status);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_END_ID,1))
    {
      status = chip_sim_end(msg);
      send_string_to_screen(msg,TRUE);
      sal_sprintf(msg, "chip_sim_end() returned with %d", status);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_TASK_WAKE_UP_ID,1))
    {
      status = chip_sim_command_wake_up_task(msg);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_TASK_SLEEP_TIME_ID,1))
    {
      UINT32
        sleep =0 ;
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SIM_TASK_SLEEP_TIME_ID,1,
              &param_val,VAL_NUMERIC,msg))
      {
        sleep = param_val->value.ulong_value;
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }

      chip_sim_command_set_task_sleep_time(sleep, msg);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_CNTR_ENABLE_ID,1))
    {
      UINT32
        enable =0 ;
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SIM_CNTR_ENABLE_ID,1,
              &param_val,VAL_NUMERIC,msg))
      {
        enable = param_val->value.ulong_value;
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }

      chip_sim_command_en_counter((enable>0), msg);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_INT_MASK_ALL_ID,1))
    {
      UINT32
        enable =0 ;
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SIM_INT_MASK_ALL_ID,1,
              &param_val,VAL_NUMERIC,msg))
      {
        enable = param_val->value.ulong_value;
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }

      chip_sim_command_mask_all_int((enable>0), msg);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_INT_ENABLE_ID,1))
    {
      UINT32
        enable =0 ;
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SIM_INT_ENABLE_ID,1,
              &param_val,VAL_NUMERIC,msg))
      {
        enable = param_val->value.ulong_value;
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }

      chip_sim_command_en_int((enable>0), msg);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_INTERRUPT_CNT_ID,1))
    {
      UINT32
        nof_int =0 ;
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SIM_INTERRUPT_CNT_ID,1,
              &param_val,VAL_NUMERIC,msg))
      {
        nof_int = param_val->value.ulong_value;
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }

      chip_sim_command_int(0, 0x0300, 5, TRUE, 10000, nof_int, msg);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_INDIRECT_DELAY_ID,1))
    {
      UINT32
        delay =0 ;
      /* get the delay of indirect triger */
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SIM_INDIRECT_DELAY_ID,1,
              &param_val,VAL_NUMERIC,msg))
      {
        delay = param_val->value.ulong_value;
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }

      chip_sim_command_indirect_delay(delay, msg);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_CELL_TX_DELAY_ID,1))
    {
      UINT32
        delay =0 ;
      /* get the cell tx of indirect triger */
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SIM_CELL_TX_DELAY_ID,1,
              &param_val,VAL_NUMERIC,msg))
      {
        delay = param_val->value.ulong_value;
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }

      chip_sim_command_cell_tx_delay(delay, msg);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_CELL_RX_CNT_ID,1))
    {
      UINT32
        nof_cells =0 ;
      char
        cell[SOC_SAND_DATA_CELL_BYTE_SIZE];
      static int AAAA = 1;

      /* get the cell tx of indirect triger */
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SIM_CELL_RX_CNT_ID,1,
              &param_val,VAL_NUMERIC,msg))
      {
        nof_cells = param_val->value.ulong_value;
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }
      sal_memset(cell, AAAA++, SOC_SAND_DATA_CELL_BYTE_SIZE);

      chip_sim_command_data_cell_rx(0, /* chip 0*/
                                    0, /* block 0*/
                                    cell,
                                    SOC_SAND_DATA_CELL_BYTE_SIZE,
                                    FALSE, /* not random*/
                                    10000, /*wait 10000 mili (10-sec)*/
                                    nof_cells,
                                    msg);
      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_TIME_MONITOR_PRINT_ID,1))
    {
      start_print_services();
      chip_sim_command_get_time_monitor(msg);
      send_string_to_screen(msg,FALSE);
      end_print_services();
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_PRINT_CLEAR_LOG_ID,1))
    {
      char *log_p = NULL;
      log_p = chip_sim_log_get_and_lock();
      if (NULL == log_p)
      {
        send_string_to_screen("NO simulator log -- probably simulator is not running", TRUE);
      }
      else
      {
        start_print_services();
        send_string_to_screen(log_p, TRUE);
        end_print_services();
        chip_sim_log_unlock_clear();
      }
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_SET_SPY_LOW_MEM_LOG_ID,1))
    {
      UINT32
        enable =0 ;
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_SIM_SET_SPY_LOW_MEM_LOG_ID,1,
              &param_val,VAL_NUMERIC,msg))
      {
        enable = param_val->value.ulong_value;
      }
      else
      {
        status = ERROR ;
        goto subject_fap10_exit ;
      }
      chip_sim_log_set_spy_low_mem((enable>0));
      goto subject_fap10_exit;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_SIM_INTERRUPT_ASSERT_ID,1))
    {
      chip_sim_command_interrupt_assert(msg);
      taskDelay(5);

      send_string_to_screen(msg,FALSE);
      goto subject_fap10_exit;
    }
    else
    {
      /*
       * Unknowen fap10 variable has been entered.
       */
      send_string_to_screen(
          "\n"
          "*** fap10 error .\r\n",
          TRUE) ;
      status = ERROR ;
      goto subject_fap10_exit;
    }

  }

subject_fap10_exit:
  return status;
}

/* } END Includes ChipSim Library.*/
#else  /* SIMULATE_CHIP_SIM */
/* {  DO NOT Includes ChipSim Library.*/
STATUS
  subject_sim(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  STATUS
    status=OK;

  send_string_to_screen("\r\n",FALSE) ;
  send_string_to_screen("'subject_sim()' function was called.\r\n",FALSE) ;
  send_string_to_screen(" Please define SIMULATE_CHIP_SIM in", TRUE);
  send_string_to_screen(__FILE__, TRUE);
  send_string_to_screen(" to 1 to activated calls to ChipSim library.", TRUE) ;
  return status;
}
/* } END DO NOT Includes ChipSim Library.*/
#endif /* SIMULATE_CHIP_SIM */

