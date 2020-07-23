/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement header for soc interrupt handler.
 */

#ifndef _DNXC_INTR_HANDLER_H_
#define _DNXC_INTR_HANDLER_H_

#include <soc/defs.h>
#include <soc/intr.h>

typedef enum dnxc_intr_reset_callback_e
{
    ASIC_RESET_NONE,
    ASIC_PON_RESET,
    ASIC_HARD_RESET,
    ASIC_SOFT_RESET_BLOCKS,
    ASIC_SOFT_RESET_BLOCKS_FABRIC
} dnxc_intr_reset_callback_t;

int dnxc_num_of_ints(
    int unit);

void dnxc_intr_switch_event_cb(
    int unit,
    soc_switch_event_t event,
    uint32 arg1,
    uint32 arg2,
    uint32 arg3,
    void *userdata);
int dnxc_intr_add_handler(
    int unit,
    int en_inter,
    int occurrences,
    int timeCycle,
    soc_handle_interrupt_func inter_action,
    soc_handle_interrupt_func inter_recurring_action);
int dnxc_intr_add_handler_ext(
    int unit,
    int en_inter,
    int occurrences,
    int timeCycle,
    soc_handle_interrupt_func inter_action,
    soc_handle_interrupt_func inter_recurring_action,
    char *msg);
int dnxc_intr_handler_deinit(
    int unit);
int dnxc_intr_handler_short_init(
    int unit);
int dnxc_intr_handler_init(
    int unit);

int dnxc_get_ser_entry_from_cache(
    int unit,
    soc_mem_t mem,
    int copyno,
    int array_index,
    int index,
    uint32 *data_entr);
int dnxc_int_name_to_id(
    int unit,
    char *name);
int dnxc_interrupt_memory_cached(
    int unit,
    soc_reg_t mem,
    int block_instance,
    int *cached_flag);
int dnxc_intr_add_clear_func(
    int unit,
    int en_inter,
    clear_func inter_action);

#endif
