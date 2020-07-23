/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#include "chip_sim.h"
#include "chip_sim_pkt.h"
#include "chip_sim_mem.h"
#include "chip_sim_log.h"


/*
 * accessing as apears in the spec. doc.
 */
UINT32 access_trig_address;

INT32 delay_milis ;
uint8  has_delayed_task[CHIP_SIM_NOF_CHIPS];
INT32 delayed_task_time_milis[CHIP_SIM_NOF_CHIPS];


void
  chip_sim_pkt_init(
    void
    )
{
  int
    i;
  /*
   * chip_mem.pkt
   */
  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    has_delayed_task[i] = FALSE ;
    delayed_task_time_milis[i] = 0 ;
  }
  delay_milis = 0 ;
}

STATUS
  chip_sim_pkt_init_chip_specifics(
    SOC_SAND_IN UINT32 i_access_trig_address
    )
{
  access_trig_address = i_access_trig_address;
  return OK;
}


/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*     chip_sim_send_pkt_op
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:04:47
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int chip_ndx

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
    chip_sim_send_pkt_op(
      SOC_SAND_IN int chip_ndx
      )
{
  STATUS
    status = OK;
  UINT32
    access_triger = 0;

  /* 1 */
  if (chip_sim_mem_read(chip_ndx, access_trig_address, &access_triger))
  {
    char msg[160];
    sal_sprintf(msg,"chip_sim_send_pkt_op(): ERROR \r\n"
                "    chip_sim_mem_read(chip_ndx(%d), access_trig_address(0x%X)) failed.\r\n",
                 chip_ndx, access_trig_address);
    chip_sim_log_run(msg);
    status = ERROR ;
    GOTO_FUNC_EXIT_POINT ;
  }

  /* 2 */
  if (access_triger != 0x1)
  {
    /* simulator error*/
/*    status = ERROR ; TODO!! - Fix*/
    chip_sim_log_run("chip_sim_send_pkt_op(): ERROR \r\n"
                     "    'access_triger != 0x1' simulator error\r\n");
    GOTO_FUNC_EXIT_POINT ;
  }


  /* 3 */
  /*
 */



FUNC_EXIT_POINT:
/* 4 */
  if (chip_sim_mem_write(chip_ndx, access_trig_address, 0x0)){
      chip_sim_log_run("chip_sim_send_pkt_op()- error: \r\n"
 	              "chip_sim_mem_write() failed.\r\n");
       status = ERROR;
  }

  return status ;
}
/*****************************************************
*NAME
*  chip_sim_mem_is_pkt_trig
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:00:15
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_offset

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
uint8
  chip_sim_mem_is_pkt_trig(
    SOC_SAND_IN UINT32 chip_offset
    )
{
  uint8
    answer = FALSE;

  if (chip_offset == access_trig_address)
  {
    answer = TRUE;
  }

  return answer ;
}


/* Under Interrupt Halting No OS Calls */
/*****************************************************
*NAME
*  chip_sim_mem_get_pkt_delay
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:00:21
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void

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
UINT32
  chip_sim_mem_get_pkt_delay(
    void
    )
{
  return delay_milis ;
}

/* Under Interrupt Halting No OS Calls */
/*set the delay task .*/
/*****************************************************
*NAME
*  chip_sim_mem_set_pkt_delay_op
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 19:00:25
*FUNCTION:
*  Clear from the code.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN chip_ndx

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
  chip_sim_mem_set_pkt_delay_op(
    SOC_SAND_IN int chip_ndx
    )
{
  STATUS
    status = OK ;

  if (has_delayed_task[chip_ndx] == TRUE)
  {
    /* an error by the driver */
    status = ERROR ;
    goto chip_sim_mem_set_pkt_delay_op_exit ;
  }

  has_delayed_task[chip_ndx] = TRUE ;
  delayed_task_time_milis[chip_ndx] = delay_milis ;

chip_sim_mem_set_pkt_delay_op_exit:
  return status ;
}


