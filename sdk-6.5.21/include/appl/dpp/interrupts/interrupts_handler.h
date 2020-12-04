/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DPP_INTERRUPTS_HANDLER_H_
#define _DPP_INTERRUPTS_HANDLER_H_

#include <soc/dpp/ARAD/arad_interrupts.h>

typedef int (* arad_handle_interrupt_func)(int unit, int block_instance, arad_interrupt_type en_arad_interrupt, char *msg);

/*
 * static data strucure which holds the info about: 
 *             the function suppose to run for each interrupt
 *              info about storm threshold
 */
struct arad_interrupt_handle_data_base {
    arad_handle_interrupt_func *func_arr;/*interrupt action, always run it*/

    /* recurring action function/params */
    arad_handle_interrupt_func *func_arr_recurring_action;/*interrupt action, run it only in a storm*/
    int *recurring_action_counters;
    int *recurring_action_time; 
    int *recurring_action_cycle_time;  /*time in seconds of each cycle, -1 is without recurring_action*/
    int *recurring_action_cycle_counting;  /*threshold in each cycle, -1 is without recurring_action*/
};
typedef struct arad_interrupt_handle_data_base arad_interrupt_handle_data_base_t;

/* 
 * Main CallBack Function
 */
void diag_arad_switch_event_cb(int unit, bcm_switch_event_t event, uint32 arg1, uint32 arg2, uint32 arg3, void *userdata);

/*
 *  init the  data base the first time using it.  
 */
int arad_interrupt_handler_data_base_init(int unit);

/*
 * add_interrupt_handler (without storm threshold)
 * input: 
 * enInter - the interrupt.
 * interruptHandler - how to handle the interrupt.
 */
int arad_interrupt_add_interrupt_handler(int unit, arad_interrupt_type en_arad_inter, int occurrences, int timeCycle, arad_handle_interrupt_func inter_action, arad_handle_interrupt_func inter_recurring_action);

int arad_handle_interrupt_recurring_detect(int unit, int block_instance, arad_interrupt_type en_arad_interrupt);

int arad_interrupt_handler_appl_init(int unit);

int arad_interrupt_handler_appl_deinit(int unit);


#endif /*__DPP_INTERRUPTS_HANDLER_H_ */
