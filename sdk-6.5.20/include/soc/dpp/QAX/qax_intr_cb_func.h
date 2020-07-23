/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     Implement CallBacks function for ARAD interrupts.
 */
 
#ifndef _QAX_INTR_CB_FUNC_H_
#define _QAX_INTR_CB_FUNC_H_


#include <soc/dpp/QAX/qax_intr.h>



void qax_interrupt_cb_init(int unit);

int qax_intr_handle_generic_none(int unit, int block_instance, qax_interrupt_type en_qax_interrupt,char *msg);
int qax_intr_recurring_action_handle_generic_none(int unit, int block_instance, qax_interrupt_type en_qax_interrupt, char *msg);

#endif 
