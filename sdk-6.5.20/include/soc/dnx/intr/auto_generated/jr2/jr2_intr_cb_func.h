/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _JR2_INTR_CB_FUNC_H_
#define _JR2_INTR_CB_FUNC_H_

void jr2_interrupt_cb_init(
    int unit);

int jr2_intr_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
int jr2_intr_recurring_action_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
#endif
