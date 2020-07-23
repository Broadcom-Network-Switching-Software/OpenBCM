/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     Implement CallBacks function for fe3200 interrupts.
 */
 
#ifndef _FE3200_INTR_CB_FUNC_H_
#define _FE3200_INTR_CB_FUNC_H_


#include <soc/dfe/fe3200/fe3200_intr.h>



void fe3200_interrupt_cb_init(int unit);

int fe3200_intr_handle_generic_none(int unit, int block_instance, fe3200_interrupt_type en_fe3200_interrupt,char *msg);
int fe3200_intr_recurring_action_handle_generic_none(int unit, int block_instance, fe3200_interrupt_type en_fe3200_interrupt, char *msg);

#endif 
