/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     Implement CallBacks function for ARAD interrupts.
 */
 
#ifndef _JERP_INTR_CB_FUNC_H_
#define _JERP_INTR_CB_FUNC_H_


#include <soc/dpp/JERP/jerp_intr.h>



void jerp_interrupt_cb_init(int unit);

int jerp_intr_handle_generic_none(int unit, int block_instance, jerp_interrupt_type en_jerp_interrupt,char *msg);
int jerp_intr_recurring_action_handle_generic_none(int unit, int block_instance, jerp_interrupt_type en_jerp_interrupt, char *msg);

#endif 

