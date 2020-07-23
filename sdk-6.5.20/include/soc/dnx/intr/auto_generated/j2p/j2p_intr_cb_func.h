/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _J_2_P_INTR_CB_FUNC_H_
#define _J_2_P_INTR_CB_FUNC_H_

void j2p_interrupt_cb_init(
    int unit);

int j2p_intr_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
int j2p_intr_recurring_action_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
#endif
