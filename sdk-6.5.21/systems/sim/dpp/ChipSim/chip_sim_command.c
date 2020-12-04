
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#ifdef LINK_FE200_LIBRARIES
  #include <soc/dpp/SOC_SAND_FE200/fe200_chip_defines.h>
#endif

#include "chip_sim_command.h"
#include "chip_sim_task.h"
#include "chip_sim_mem.h"
#include "chip_sim_engine.h"
#include "chip_sim_interrupts.h"
#include "chip_sim_cell.h"



/*****************************************************
*NAME
*   chip_sim_command_start_sequence
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 10:15:20
*FUNCTION:
* 1. check is the simulator task is alive
* 2. ***********DISABLE******** the interrupts
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_OUT char  msg[] -- error msg -- if needed
*    (2) SOC_SAND_OUT int *int_lock -- interrupt mask
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  -----------------------------------------------------------------------------
*  ---------------- EVERY COMMAND NEED TO START WITH THIS FUNCTION -------------
*  -----------------------------------------------------------------------------
*SEE ALSO:
 */
STATUS
  chip_sim_command_start_sequence(
    SOC_SAND_OUT char  msg[],
    SOC_SAND_OUT uint32 *int_lock
    )
{
  STATUS
    status = OK;

  /* 1 */
  if (!chip_sim_task_is_alive())
  {
    sal_sprintf(msg, "chip_sim_task is not running\r\n");
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  /* 2 */
  soc_sand_os_stop_interrupts(int_lock);

FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*  chip_sim_command_end_sequence
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 10:17:43
*FUNCTION:
* 1. ***********ENABLE******** the interrupts
* 2. wake up the simulator task
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int int_lock
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    void
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  -----------------------------------------------------------------------------
*  ---------------- EVERY COMMAND NEED TO END WITH THIS FUNCTION ---------------
*  -----------------------------------------------------------------------------
*SEE ALSO:
 */
void
  chip_sim_command_end_sequence(
    SOC_SAND_IN long int_lock
    )
{
  /* 1 */
  soc_sand_os_start_interrupts(int_lock);
  /* 2 */
  chip_sim_task_wake_up();
}

/*****************************************************
*NAME
*   chip_sim_command_indirect_delay
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 10:19:59
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 delay in ,mili-seconds
*    (2) SOC_SAND_OUT char  msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_indirect_delay(
    SOC_SAND_IN UINT32 delay,
    SOC_SAND_OUT char  msg[]
    )
{
  STATUS
    status = OK;
  uint32
     int_lock;

  /*-----------------------------------------------------------------------------*/
  if (chip_sim_command_start_sequence(msg, &int_lock))
  /*-----------------------------------------------------------------------------*/
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  if (chip_sim_indirect_set_delay_time(delay))
  {
    sal_sprintf(msg, "Failed\r\n");
  }
  else
  {
    sal_sprintf(msg, "OK\r\n");
  }

  sal_sprintf(msg, "OK\r\n");

  /*-----------------------------------------------------------------------------*/
  chip_sim_command_end_sequence(int_lock);
  /*-----------------------------------------------------------------------------*/
FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*   chip_sim_command_cell_tx_delay
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 10:21:06
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 delay
*    (2) SOC_SAND_OUT char  msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*  STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_cell_tx_delay(
    SOC_SAND_IN UINT32 delay,
    SOC_SAND_OUT char  msg[]
    )
{
  STATUS
    status = OK;
  uint32
    int_lock;

  /*-----------------------------------------------------------------------------*/
  if (chip_sim_command_start_sequence(msg, &int_lock))
  /*-----------------------------------------------------------------------------*/
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  chip_sim_cell_tx_set_delay(delay);
  sal_sprintf(msg, "OK\r\n");

  /*-----------------------------------------------------------------------------*/
  chip_sim_command_end_sequence(int_lock);
  /*-----------------------------------------------------------------------------*/
FUNC_EXIT_POINT:
  return status;
}



/*****************************************************
*NAME
*   chip_sim_command_data_cell_rx
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 10:21:14
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 cnt
*    (2) SOC_SAND_OUT char  msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_data_cell_rx(
    SOC_SAND_IN int           chip_ndx,
    SOC_SAND_IN int           block_ndx,
    SOC_SAND_IN unsigned char *data_buff,
    SOC_SAND_IN int           data_buff_size,
    SOC_SAND_IN uint8          is_random,
    SOC_SAND_IN INT32         interval_mili,
    SOC_SAND_IN UINT32        cells_count,
    SOC_SAND_OUT char         msg[]
    )
{
  STATUS
    status = OK;
  uint32
    int_lock;

  /*-----------------------------------------------------------------------------*/
  if (chip_sim_command_start_sequence(msg, &int_lock))
  /*-----------------------------------------------------------------------------*/
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  if (chip_sim_cell_add_rx_data(chip_ndx,
                                block_ndx,
                                data_buff,
                                data_buff_size,
                                is_random,
                                interval_mili,
                                cells_count)
     )
  {
    sal_sprintf(msg, "Failed\r\n");
  }
  else
  {
    sal_sprintf(msg, "OK\r\n");
  }

  /*-----------------------------------------------------------------------------*/
  chip_sim_command_end_sequence(int_lock);
  /*-----------------------------------------------------------------------------*/
FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*   chip_sim_command_counter
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:04:40
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  uint8  is_random
*    (2) SOC_SAND_OUT char  msg[]

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS

*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */

STATUS
  chip_sim_command_counter(
    SOC_SAND_IN  uint8  is_random,
    SOC_SAND_OUT char  msg[]
    )
{
  STATUS
    status = OK;
  uint32
    int_lock;

  /*-----------------------------------------------------------------------------*/
  if (chip_sim_command_start_sequence(msg, &int_lock))
  /*-----------------------------------------------------------------------------*/
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

#ifdef LINK_FE200_LIBRARIES
/* { */
  if (chip_sim_counter_change(0, SOC_SAND_OFFSETOF(FE_REGS,fe_mac_mac_regs.fe_mac_mac_detail.mac_ber_cntrs_4[0]),
                          is_random, 10, 2, 100))
  {
    sal_sprintf(msg, "Did not find that counter\r\n");
  }
  else
/* } LINK_FE200_LIBRARIES */
#endif
  {
    sal_sprintf(msg, "OK\r\n");
  }

  /*-----------------------------------------------------------------------------*/
  chip_sim_command_end_sequence(int_lock);
  /*-----------------------------------------------------------------------------*/
FUNC_EXIT_POINT:
  return status;
}


/*****************************************************
*NAME
*   chip_sim_command_en_counter
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:04:40
*FUNCTION:
*  enable/disable counters.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  uint8  active
*    (2) SOC_SAND_OUT char  msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_en_counter(
    SOC_SAND_IN  uint8  active,
    SOC_SAND_OUT char  msg[]
    )
{
  STATUS
    status = OK;
  uint32
    int_lock;

  /*-----------------------------------------------------------------------------*/
  if (chip_sim_command_start_sequence(msg, &int_lock))
  /*-----------------------------------------------------------------------------*/
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  chip_sim_engine_set_cntr_enable(active);
  sal_sprintf(msg, "OK\r\n");

  /*-----------------------------------------------------------------------------*/
  chip_sim_command_end_sequence(int_lock);
  /*-----------------------------------------------------------------------------*/
FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*   chip_sim_command_mask_all_int
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:39:06
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  uint8  active
*    (2) SOC_SAND_OUT char  msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_mask_all_int(
    SOC_SAND_IN  uint8  active,
    SOC_SAND_OUT char  msg[]
    )
{
  STATUS
    status = OK;
  uint32
    int_lock;

  /*-----------------------------------------------------------------------------*/
  if (chip_sim_command_start_sequence(msg, &int_lock))
  /*-----------------------------------------------------------------------------*/
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  chip_sim_interrupt_mask_all((UINT32)active&0x1);
  sal_sprintf(msg, "OK\r\n");

  /*-----------------------------------------------------------------------------*/
  chip_sim_command_end_sequence(int_lock);
  /*-----------------------------------------------------------------------------*/
FUNC_EXIT_POINT:
  return status;
}

/*****************************************************
*NAME
*   chip_sim_command_en_int
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:39:12
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  uint8  active
*    (2) SOC_SAND_OUT char  msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_en_int(
    SOC_SAND_IN  uint8  active,
    SOC_SAND_OUT char  msg[]
    )
{
  STATUS
    status = OK;
  uint32
    int_lock;

  /*-----------------------------------------------------------------------------*/
  if (chip_sim_command_start_sequence(msg, &int_lock))
  /*-----------------------------------------------------------------------------*/
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  chip_sim_engine_set_int_enable(active);
  sal_sprintf(msg, "OK\r\n");

  /*-----------------------------------------------------------------------------*/
  chip_sim_command_end_sequence(int_lock);
  /*-----------------------------------------------------------------------------*/
FUNC_EXIT_POINT:
  return status;
}


/*****************************************************
*NAME
*   chip_sim_command_wake_up_task
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:39:18
*FUNCTION:
* 1. check the task is alive.
* 2. Wake it up.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_OUT char  msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_wake_up_task(
    SOC_SAND_OUT char  msg[]
    )
{
  STATUS
    status = OK;
  /* 1 */
  if (!chip_sim_task_is_alive())
  {
    sal_sprintf(msg, "chip_sim_task is not running\r\n");
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }
  /* 2 */
  chip_sim_task_wake_up();

FUNC_EXIT_POINT:
  return status;
}


/*****************************************************
*NAME
*   chip_sim_command_set_task_sleep_time
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:39:24
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  UINT32 mili_sec
*    (2) SOC_SAND_OUT char   msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_set_task_sleep_time(
    SOC_SAND_IN  UINT32 mili_sec,
    SOC_SAND_OUT char   msg[]
    )
{
  STATUS
    status = OK;
  uint32
    int_lock;

  /*-----------------------------------------------------------------------------*/
  if (chip_sim_command_start_sequence(msg, &int_lock))
  /*-----------------------------------------------------------------------------*/
  {
    /*if the ERROR -- the task is not running -- so just update the time*/
    chip_sim_task_set_sleep_time(mili_sec);
    GOTO_FUNC_EXIT_POINT;
  }
  chip_sim_task_set_sleep_time(mili_sec);

  /*-----------------------------------------------------------------------------*/
  chip_sim_command_end_sequence(int_lock);
  /*-----------------------------------------------------------------------------*/

FUNC_EXIT_POINT:
  return status;
}


/*****************************************************
*NAME
*   chip_sim_command_get_time_monitor
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:39:32
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_OUT char   msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_get_time_monitor(
    SOC_SAND_OUT char   msg[]
    )
{
  STATUS
    status = OK;
  uint32
    int_lock;
  UINT32
    work_time,
    sleep_time;

  /*-----------------------------------------------------------------------------*/
  if (chip_sim_command_start_sequence(msg, &int_lock))
  /*-----------------------------------------------------------------------------*/
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  work_time = chip_sim_task_get_work_time();
  sleep_time = chip_sim_task_get_sleep_time();
  sal_sprintf(msg, "chip_sim_task average \r\n"
               "   work  time is %lu \tmicroseconds, \r\n"
               "   sleep time is %lu \tmilliseconds \r\n",
          (unsigned long)work_time, (unsigned long)sleep_time);

  /*-----------------------------------------------------------------------------*/
  chip_sim_command_end_sequence(int_lock);
  /*-----------------------------------------------------------------------------*/
FUNC_EXIT_POINT:
  return status;
}


/*****************************************************
*NAME
*   chip_sim_command_interrupt_assert
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:39:39
*FUNCTION:
* Only for debug the interrupt hocking.
* Inject an interrupt.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_OUT char  msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_interrupt_assert(
    SOC_SAND_OUT char  msg[]
    )
{
  STATUS
    status = OK;
  /* 1 */
  if (!chip_sim_task_is_alive())
  {
    sal_sprintf(msg, "chip_sim_task is not running, setting interrupt any way (probably IRQ6). \r\n");
    sal_msleep(100);
  }
  /* 2 */
  chip_sim_interrupt_cpu_assert();

  return status;
}

/*****************************************************
*NAME
*   chip_sim_command_int
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 26-Sep-02 15:39:47
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  UINT32 nof_int
*    (2) SOC_SAND_OUT char   msg[]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_command_int(
    SOC_SAND_IN int    chip_ndx,
    SOC_SAND_IN UINT32 int_chip_offset,
    SOC_SAND_IN UINT32 int_bit,
    SOC_SAND_IN uint8   is_random,
    SOC_SAND_IN INT32  interval_milisec,
    SOC_SAND_IN UINT32 count,
    SOC_SAND_OUT char  msg[]
    )
{
  STATUS
    status = OK;
  uint32
    int_lock;

  /*-----------------------------------------------------------------------------*/
  if (chip_sim_command_start_sequence(msg, &int_lock))
  /*-----------------------------------------------------------------------------*/
  {
    status = ERROR;
    GOTO_FUNC_EXIT_POINT;
  }

  chip_sim_interrupt_set_interrupt_op(chip_ndx, int_chip_offset, int_bit,
                                      is_random, interval_milisec, count, msg);
  /*chip_sim_interrupt_set_interrupt_op(0, 0x0300, 5, TRUE, 10000, nof_int, msg);*/

  /*-----------------------------------------------------------------------------*/
  chip_sim_command_end_sequence(int_lock);
  /*-----------------------------------------------------------------------------*/
FUNC_EXIT_POINT:
  return status;
}


