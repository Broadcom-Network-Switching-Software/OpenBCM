/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     Implement CallBacks function for ramon interrupts.
 */
 
#ifndef _RAMON_INTR_CB_FUNC_H_
#define _RAMON_INTR_CB_FUNC_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 


#include <soc/dnxf/ramon/ramon_intr.h>



void ramon_interrupt_cb_init(int unit);

int ramon_intr_handle_generic_none(int unit, int block_instance, ramon_interrupt_type en_ramon_interrupt,char *msg);
int ramon_intr_recurring_action_handle_generic_none(int unit, int block_instance, ramon_interrupt_type en_ramon_interrupt, char *msg);

#endif 
