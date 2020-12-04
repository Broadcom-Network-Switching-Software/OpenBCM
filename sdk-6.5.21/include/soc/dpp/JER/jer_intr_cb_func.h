/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     Implement CallBacks function for ARAD interrupts.
 */
 
#ifndef _JER_INTR_CB_FUNC_H_
#define _JER_INTR_CB_FUNC_H_


#include <soc/dpp/JER/jer_intr.h>



void jer_interrupt_cb_init(int unit);

int jer_intr_handle_generic_none(int unit, int block_instance, jer_interrupt_type en_jer_interrupt,char *msg);
int jer_intr_recurring_action_handle_generic_none(int unit, int block_instance, jer_interrupt_type en_jer_interrupt, char *msg);

int jer_interrupt_handle_oamppendingevent(int unit, int block_instance, jer_interrupt_type en_arad_interrupt, char *msg);
int jer_interrupt_handle_oamppendingstatevent(int unit, int block_instance, jer_interrupt_type en_arad_interrupt, char *msg);

#endif 
