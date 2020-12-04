/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement header correction action functions for fe3200icho interrupts.
 */

#ifndef _FE3200_INTR_CORR_ACT_FUNC_H_
#define _FE3200_INTR_CORR_ACT_FUNC_H_


#include <soc/dfe/fe3200/fe3200_intr.h>
#include <soc/dcmn/dcmn_intr_corr_act_func.h>


int fe3200_interrupt_handles_corrective_action(int unit, int block_instance, fe3200_interrupt_type interrupt_id, char *msg, dcmn_int_corr_act_type corr_act, void *param1, void *param2);


int fe3200_interrupt_handles_corrective_action_shutdown_fbr_link(int unit,int block_instance,fe3200_interrupt_type interrupt_id,char *msg);
int
fe3200_interrupt_data_collection_for_shadowing(int unit, int block_instance, fe3200_interrupt_type interrupt_id, char *special_msg,
                                               dcmn_int_corr_act_type* p_corrective_action,
                                               dcmn_interrupt_mem_err_info* shadow_correct_info) ;
#endif 
