/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include "chip_sim.h"
#include "chip_sim_task.h"
#include "chip_sim_mem.h"
#include "chip_sim_engine.h"
#include "chip_sim_interrupts.h"

#if LINK_FAP21V_LIBRARIES
#include "chip_sim_FAP21V.h"
#endif

#if LINK_T20E_LIBRARIES
#include "chip_sim_T20E.h"
#endif

#include <signal.h>
#include <string.h>
#include <time.h>

extern void init_simulate_fe200_device(unsigned int chip_ver);
extern void init_simulate_fap10m_device(unsigned int chip_ver);
extern void init_simulate_fap20v_device(unsigned int chip_ver);
extern void init_simulate_fap20m_device(unsigned int chip_ver);
extern void init_simulate_timna_device(unsigned int chip_ver);
extern void init_simulate_fap21v_device(unsigned int chip_ver);
extern void init_simulate_petra_device(unsigned int chip_ver);
extern void init_simulate_fe600_device(unsigned int chip_ver);
extern void init_simulate_t20e_device(unsigned int chip_ver);

#if NEGEV_NATIVE
ChipSim missing functions
#else
void
  init_simulate_petra_device(
    unsigned int chip_ver
  )
{
}

void
  assert_irq_6(
    void
  )
{
  soc_sand_os_printf(
    "\n\r"
    "\n\r"
    "****** assert_irq_6 -- Was called *****"
    "\n\r"
    "\n\r"
  );
}

void
  deassert_irq_6(
    void
  )
{
  soc_sand_os_printf(
    "\n\r"
    "\n\r"
    "****** deassert_irq_6 -- Was called *****"
    "\n\r"
    "\n\r"
  );
}

int
  attach_irq(
    void         *irq_procedure,
    unsigned int irq_num,
    int          private_param,
    char         *err_msg
  )
{
  soc_sand_os_printf(
    "\n\r"
    "\n\r"
    "****** attach_irq -- Was called *****"
    "\n\r"
    "\n\r"
  );

  return 0; /*OK*/
}

int
  detach_irq(
    unsigned int irq_num,
    char         *err_msg
  )
{
  soc_sand_os_printf(
    "\n\r"
    "\n\r"
    "****** detach_irq -- Was called *****"
    "\n\r"
    "\n\r"
  );
  return 0; /*OK*/
}

#endif

FUNCPTR_TO_SCREEN Notify_to_screen = NULL;
UINT32FUNCPTR Chip_sim_task_get_time_in_microseconds = NULL;
uint8 Continue_to_run = FALSE;

STATUS chip_sim_task(SOC_SAND_IN uint8 hock_int_handler);



UINT32
  Work_time = 1;

UINT32
  chip_sim_task_get_work_time(
    void
    )
{
  return Work_time;
}

/*****************************************************
*NAME
*  chip_sim_task_is_alive -- indicator that the
*   simulator task is alive.
 */
static
  uint8
    Task_is_alive = FALSE ;

/*****************************************************
*NAME
*   chip_sim_task_is_alive
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:57:20
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
uint8
  chip_sim_task_is_alive(
    void
    )
{
  return Task_is_alive;
}

/*****************************************************
*NAME
*  Sleep_nof_ticks -- number of ticks to sleep between
*  the task work.
 */
static
  int
    Sleep_nof_ticks = 15;
void
  chip_sim_task_set_sleep_time(
    SOC_SAND_IN UINT32 mili_sec
    )
{
  if (mili_sec>1)
  {
    Sleep_nof_ticks = ( (soc_sand_os_sys_clk_rate_get()*mili_sec) + 500 ) / 1000;
  }
}

/* number of milisecs to sleep*/
UINT32
  chip_sim_task_get_sleep_time(
    void
    )
{
  return (Sleep_nof_ticks*1000)/soc_sand_os_sys_clk_rate_get();
}


/*****************************************************
*NAME
*   chip_sim_init_first_time
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 15:42:01
*FUNCTION:
*  First time init
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
STATUS
  chip_sim_init_first_time(
    void
    )
{
  STATUS
    status = OK ;

  chip_sim_mem_init() ;
  Task_is_alive = FALSE ;
  Notify_to_screen = NULL ;
  Work_time = 1 ;
  return status ;
}

static
  uint8
    Chip_sim_first_time = TRUE;

#define CHIP_SIM_TASK_NAME_SIZE 100
static
  char
    Chip_sim_task_name[CHIP_SIM_TASK_NAME_SIZE] = "" ;

static
  long
    Chip_sim_task_msg_q = 0;


/*****************************************************
*NAME
*   chip_sim_task_wake_up
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:57:46
*FUNCTION:
*  Send a msg to the q the simulator sleeps on.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*     void
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  Assuming the chip_sim_task is running.
*  Assuming the Chip_sim_task_msg_q is allocated.
*SEE ALSO:
 */
void
  chip_sim_task_wake_up(
    void
    )
{
  unsigned long
    msg = 1;
  if (0 != Chip_sim_task_msg_q)
  {
    soc_sand_os_msg_q_send(
      (void *)Chip_sim_task_msg_q,(char *)&msg,sizeof(msg),-1,0);
  }
}

/*****************************************************
*NAME
*   chip_sim_task_get_tid
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:57:46
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
int
  chip_sim_task_get_tid(
    void
    )
{
  /*
   * No such function on Windows.
   */
  return -1;
}


/*****************************************************
*NAME
*   chip_sim_start
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 15:42:09
*FUNCTION:
*  Start the chip simulator task.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN char *sim_task_name
*         name of the task this function is to spawn.
*    (2) SOC_SAND_IN int  sim_task_prio
*         priority of the task this function is to spawn.
*    (3) SOC_SAND_IN int  sim_task_stack_size
*         stack size of the task this function is to spawn.
*    (4) SOC_SAND_IN int  sim_task_task_id
*         inner task id of the task this function is to spawn.
*    (5) SOC_SAND_IN SOC_SAND_CHIP_TYPE chip_type
*         Chip type of this simulator to take care of.
*         Currently handle SOC_SAND_FE200, SOC_SAND_FAP10M.
*    (6) SOC_SAND_IN unsigned int     chip_ver
*         Chip version of this simulator to take care of.
*    (7) SOC_SAND_IN UINT32 start_address[CHIP_SIM_NOF_CHIPS]
*           This are real address for the simulator to use.
*           No allocation for this addresses is done.
*    (8) SOC_SAND_IN DESC_FE_REG *desc
*           Register descriptors.
*    (9) FUNCPTR to_screen
*           printing function (send_string_to_screen).
*    (10) UINT32FUNCPTR i_get_time_in_microseconds
*           timeing function (get_time_in_microseconds).
*    (11)BOOL hock_int_handler
*           Does the simulator need to hock its own interrupt handler?
*           The device driver should hock its own ==> FALSE.
*           The interrupt getting attched is "chip_sim_interrupt_cpu_handler"
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
  chip_sim_start(
    SOC_SAND_IN char                        *sim_task_name,
    SOC_SAND_IN int                         sim_task_prio,
    SOC_SAND_IN int                         sim_task_stack_size,
    SOC_SAND_IN int                         sim_task_task_id,
    SOC_SAND_IN SOC_SAND_DEVICE_TYPE            chip_type,
    SOC_SAND_IN unsigned int                chip_ver,
    SOC_SAND_IN UINT32                      start_address[CHIP_SIM_NOF_CHIPS],
    SOC_SAND_IN DESC_FE_REG                 *desc,
    FUNCPTR_TO_SCREEN to_screen,
    UINT32FUNCPTR i_get_time_in_microseconds,
    SOC_SAND_IN uint8 hock_int_handler
    )
{
  STATUS
    status = OK ;
  int
    i ;
  char
    msg[160];

  if (Chip_sim_first_time)
  {
    Chip_sim_first_time = FALSE ;
    chip_sim_init_first_time();
  }

  chip_sim_end(msg);
  Notify_to_screen = to_screen;
  Chip_sim_task_get_time_in_microseconds = i_get_time_in_microseconds;
  Work_time = 1;
  if ( (NULL == Notify_to_screen) ||
       (NULL == Chip_sim_task_get_time_in_microseconds)
     )
  {
    status = ERROR ;
    goto chip_sim_start_exit ;
  }

  {
    /*
     * Test the function pointer this function got
     * are pointing to something real. So, Activate than,
     * hope that no one crash...
     */
    Chip_sim_task_get_time_in_microseconds();
    Notify_to_screen("", FALSE);
  }

  if (chip_sim_mem_malloc(chip_type, chip_ver, desc, start_address))
  {
    status = ERROR ;
    goto chip_sim_start_exit ;
  }
  Chip_sim_task_msg_q = 0xf/* (long)soc_sand_os_msg_q_create(50,sizeof(long)) - See remark in the implemetation of the function. */;
  if (0 == Chip_sim_task_msg_q)
  {
    status = ERROR ;
    goto chip_sim_start_exit ;
  }
  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    if( chip_sim_mem_reset(i) )
    {
      status = ERROR ;
      goto chip_sim_start_exit ;
    }
#if SAND_LOW_LEVEL_SIMULATION
#if LINK_FAP21V_LIBRARIES
    if (chip_type == SOC_SAND_DEV_FAP21V)
    {
      fap21v_initialize_device_values((uint32*)start_address[i]);
    }
#endif    	
#if LINK_T20E_LIBRARIES
    if (chip_type == SOC_SAND_DEV_T20E)
    {
      t20e_initialize_device_values(&(start_address[i]));
    }
#endif    	 	
#endif
  }
  switch (chip_type)
  {
  case SOC_SAND_DEV_FE200:
#ifndef FAP20M_STANDALONE_PACKAGE
#if LINK_FE200_LIBRARIES
    init_simulate_fe200_device(chip_ver);
#endif
#endif
    break;
#if LINK_FAP10M_LIBRARIES
  case SOC_SAND_DEV_FAP10M:
    init_simulate_fap10m_device(chip_ver);
    break;
#endif
#if LINK_FAP20V_LIBRARIES
  case SOC_SAND_DEV_FAP20V:
    init_simulate_fap20v_device(chip_ver);
    break;
#endif
#if LINK_FAP21V_LIBRARIES
  case SOC_SAND_DEV_FAP21V:
    init_simulate_fap21v_device(chip_ver);
    break;
#endif
#if LINK_FE600_LIBRARIES
  case SOC_SAND_DEV_FE600:
    init_simulate_fe600_device(chip_ver);
    break;
#endif
#if LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:
    init_simulate_t20e_device(chip_ver);
    break;
#endif
#if LINK_FAP20M_LIBRARIES
  case SOC_SAND_DEV_FAP20M:
    init_simulate_fap20m_device(chip_ver);
    break;
#endif
#if LINK_TIMNA_LIBRARIES
  case SOC_SAND_DEV_TIMNA:
    init_simulate_timna_device(chip_ver);
    break;
#endif
  default:
    status = ERROR ;
    goto chip_sim_start_exit ;
  }


  if(sal_strlen(sim_task_name) > CHIP_SIM_TASK_NAME_SIZE -1) {
    soc_sand_os_printf("task name is too long \n\r");
    status = ERROR;
    goto  chip_sim_start_exit;
  }

  sal_memset(Chip_sim_task_name, 0x0, CHIP_SIM_TASK_NAME_SIZE);
  sal_strncpy(Chip_sim_task_name, sim_task_name, CHIP_SIM_TASK_NAME_SIZE - 1);

  Continue_to_run = TRUE;
  soc_sand_os_task_spawn(Chip_sim_task_name, sim_task_prio,
              0, sim_task_stack_size,
              (SOC_SAND_FUNC_PTR)chip_sim_task, (long)hock_int_handler,
              0,0,0,0,0,0,0,0,
              sim_task_task_id) ;

chip_sim_start_exit:
  return status ;
}

/*****************************************************
*NAME
*   chip_sim_end
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 15:44:19
*FUNCTION:
*  Ends the Chip Simulator task.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1)
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
  chip_sim_end(
    char msg[]
    )
{
  STATUS
    status = OK ;

  /*
   * Notify the task to end it self
   */
  Continue_to_run = FALSE;
  chip_sim_task_wake_up();
  sal_msleep(600);

  Task_is_alive = FALSE ;
  if (chip_sim_mem_free())
  {
    sal_sprintf(msg, "ERROR in chip_sim_mem_free");
    status |= ERROR ;
  }

  if (0 != Chip_sim_task_msg_q)
  {
    status |= soc_sand_os_msg_q_delete((void*)Chip_sim_task_msg_q);
  }
  Chip_sim_task_msg_q = 0 ;

  return status ;
}


/*****************************************************
*NAME
*  chip_sim_task_get_mili_time
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 24-Sep-02 18:57:59
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
  chip_sim_task_get_mili_time(
    void
    )
{
  uint32
    sec,
    nano,
    mili;
  SOC_SAND_RET err;
  err = soc_sand_os_get_time(&sec,&nano) ;
  if (err)
  {
    Notify_to_screen("get_mili_time -- CRACH AND BURN", TRUE);
  }

  mili = sec*1000+nano/1000000;

  return mili;
}

/*****************************************************
*NAME
*   chip_sim_task
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 15:45:11
*FUNCTION:
*  THE one AND ONLY one chip Simulator.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN uint8 hock_int_handler
*     the simulator should NOT provid an hanlder ...
*     But, in case th device driver-- do no have... we hock it here.
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
  chip_sim_task(
    SOC_SAND_IN uint8 hock_int_handler
    )
{
  STATUS
    status = OK ;
  UINT32
    past_time,
    now_time,
    diff_time;
  UINT32
    start_work_time,
    end_work_time,
    diff_work_time;
  char
    msg[160];
  unsigned long
    message;

  Task_is_alive = TRUE ;

  if (hock_int_handler)
  {
    if ( chip_sim_interrupt_cpu_hock(msg) )
    {
      Notify_to_screen("did not hock the interrupt to the CPU exiting chip_sim_task()",TRUE);
      status = ERROR ;
      goto FUNC_EXIT_POINT ;
    }
  }

  now_time = past_time = chip_sim_task_get_mili_time();

  while(Continue_to_run)
  {
    start_work_time = Chip_sim_task_get_time_in_microseconds();
    now_time = chip_sim_task_get_mili_time();
    diff_time =  now_time - past_time;
    past_time = chip_sim_task_get_mili_time();

#if NEGEV_NATIVE
#else
    sal_msleep(1000);
#endif

    if (chip_sim_engine_run(diff_time))
    {
      Notify_to_screen("chip_sim_engine_run returned with ERROR",TRUE);
      status = ERROR ;
      goto FUNC_EXIT_POINT ;
    }

    end_work_time = Chip_sim_task_get_time_in_microseconds();
    diff_work_time = end_work_time - start_work_time;
    Work_time = (Work_time*9)/10 + diff_work_time/10;
    /*
     * sleep
     */
    soc_sand_os_msg_q_recv(
      (void *)Chip_sim_task_msg_q,
      (unsigned char *)&message,sizeof(message),Sleep_nof_ticks);
  }
FUNC_EXIT_POINT:

  if (hock_int_handler)
  {
    if ( chip_sim_interrupt_cpu_unhock(msg) )
    {
      Notify_to_screen(msg,TRUE);
      status = ERROR ;
    }
  }
  Task_is_alive = FALSE ;
  return status ;
}


