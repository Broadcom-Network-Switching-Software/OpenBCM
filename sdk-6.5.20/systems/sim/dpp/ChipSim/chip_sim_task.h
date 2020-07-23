/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef _CHIP_SIM_TASK_H_
/* { */
#define _CHIP_SIM_TASK_H_


#ifdef  __cplusplus
extern "C" {
#endif

#include "chip_sim.h"
#include <appl/diag/dpp/dune_chips.h>
#include <appl/diag/dpp/utils_defx.h>

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */
/* } */
#else
/* { */
/* } */
#endif



typedef UINT32 (*UINT32FUNCPTR) (void);
typedef int (*FUNCPTR_TO_SCREEN) (const char *out_string,int  add_cr_lf);

extern UINT32FUNCPTR chip_sim_task_get_time_in_microseconds;
extern FUNCPTR chip_sim_to_screen;

STATUS chip_sim_start(
    SOC_SAND_IN char *sim_task_name,
    SOC_SAND_IN int  sim_task_prio,
    SOC_SAND_IN int  sim_task_stack_size,
    SOC_SAND_IN int  sim_task_task_id,
    SOC_SAND_IN SOC_SAND_DEVICE_TYPE chip_type,
    SOC_SAND_IN unsigned int     chip_ver,
    SOC_SAND_IN UINT32 start_address[CHIP_SIM_NOF_CHIPS],
    SOC_SAND_IN DESC_FE_REG  *desc,
    FUNCPTR_TO_SCREEN to_screen,
    UINT32FUNCPTR get_time_in_microseconds,
    SOC_SAND_IN uint8 hock_int_handler
    );
STATUS chip_sim_end(char msg[]);
uint8 chip_sim_task_is_alive(void);
UINT32 chip_sim_task_get_mili_time(void);
int    chip_sim_task_get_tid(void);
void chip_sim_task_set_sleep_time(SOC_SAND_IN UINT32 mili_sec);
UINT32 chip_sim_task_get_work_time(void);
UINT32 chip_sim_task_get_sleep_time(void);
void   chip_sim_task_wake_up(void);


#ifdef  __cplusplus
}
#endif


/* } _CHIP_SIM_TASK_H_*/
#endif


