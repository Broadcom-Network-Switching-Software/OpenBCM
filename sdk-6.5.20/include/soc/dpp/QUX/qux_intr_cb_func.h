/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     Implement CallBacks function for ARAD interrupts.
 */
 
#ifndef _QUX_INTR_CB_FUNC_H_
#define _QUX_INTR_CB_FUNC_H_


#include <soc/dpp/QUX/qux_intr.h>



void qux_interrupt_cb_init(int unit);

int qux_intr_handle_generic_none(int unit, int block_instance, qux_interrupt_type en_qux_interrupt,char *msg);
int qux_intr_recurring_action_handle_generic_none(int unit, int block_instance, qux_interrupt_type en_qux_interrupt, char *msg);

#endif 
