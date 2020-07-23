/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DCMN_INTERRUPTS_HANDLER_H_
#define _DCMN_INTERRUPTS_HANDLER_H_

#include <sal/core/sync.h>
#include <soc/defs.h>
#include <soc/types.h>
#include <bcm/error.h>

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h>
#endif

typedef int (* handle_interrupt_func)(int unit, int block_instance, uint32 en_interrupt, char *msg);
typedef int (* handle_nof_block_instances)(int unit,  soc_block_types_t block_types, int *nof_block_instances);
typedef void (* handle_interrupt_add_interrupt_handler_init)(int unit);

typedef struct interrupt_handle_block_data
{
  int *block_instance_data;
}interrupt_handle_block_data_t;

/*
 * static data structure which holds the info about:
 *             the function suppose to run for each interrupt
 *              info about storm threshold
 */
struct interrupt_handle_data_base {

    handle_interrupt_func *func_arr;/*interrupt action, always run it*/

    /* recurring action function/params */
    handle_interrupt_func *func_arr_recurring_action;/*interrupt action, run it only in a storm*/
    interrupt_handle_block_data_t *recurring_action_counters;
    interrupt_handle_block_data_t *recurring_action_time; 
    interrupt_handle_block_data_t *recurring_action_cycle_time;  /*time in seconds of each cycle, -1 is without recurring_action*/
    interrupt_handle_block_data_t *recurring_action_cycle_counting;  /*threshold in each cycle, -1 is without recurring_action*/
    sal_sem_t interrupts_data_sem;
};

struct interrupt_common_params {
    uint32 nof_interrupts; 
    handle_interrupt_add_interrupt_handler_init interrupt_add_interrupt_handler_init;
    handle_nof_block_instances interrupt_handle_block_instance;
    uint32* int_disable_on_init;
    uint32* int_active_on_init;
    uint32* int_disable_print_on_init;
    soc_mem_t* cached_mem;
};

typedef struct interrupt_handle_data_base interrupt_handle_data_base_t;
typedef struct interrupt_common_params interrupt_common_params_t;

/* 
 * Main CallBack Function
 */
void diag_switch_event_cb(int unit, bcm_switch_event_t event, uint32 arg1, uint32 arg2, uint32 arg3, void *userdata);

/*
 *  init the  data base the first time using it.  
 */
int interrupt_handler_data_base_init(int unit);

/*
 * add_interrupt_handler (without storm threshold)
 * input: 
 * enInter - the interrupt.
 * interruptHandler - how to handle the interrupt.
 */
int interrupt_add_interrupt_handler(int unit, int en_inter, int occurrences, int timeCycle, handle_interrupt_func inter_action, handle_interrupt_func inter_recurring_action);
int interrupt_config_interrupt_handler(int unit, int en_inter, int occurrences, int timeCycle, handle_interrupt_func inter_action, handle_interrupt_func inter_recurring_action);
int handle_interrupt_recurring_detect(int unit, int block_instance, uint32 en_interrupt); 
int interrupt_handler_appl_init(int unit);
int interrupt_handler_appl_deinit(int unit);
int interrupt_handler_appl_nocache_deinit(int unit);
int interrupt_handler_appl_db_init(int unit);


/*
 * Set cache for memories corruption interrupts use
 */
int interrupt_handler_appl_ref_cache_defaults_set(int unit);

/* Interrupt User CB */
void interrupt_user_soc_intr(void *_unit);

void interrupt_appl_cb(int unit, bcm_switch_event_t event, uint32 arg1, uint32 arg2, uint32 arg3, void *userdata);

/*
* cache support
*/
int interrupt_attach_cache(int unit, soc_mem_t mem, int block_instance);
int interrupt_detach_cache(int unit, soc_mem_t mem, int block_instance);
int interrupt_memory_cached(int unit, soc_reg_t mem, int block_instance, int* cached_flag);
int interrupt_memory_could_be_cached(int unit, soc_mem_t cached_mem, int* cache_enable);



#endif /*__DFE_INTERRUPTS_HANDLER_H_ */
