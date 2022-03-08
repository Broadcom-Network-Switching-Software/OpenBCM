#ifndef _J_2_X_INTR_CB_FUNC_H_
#define _J_2_X_INTR_CB_FUNC_H_

void j2x_interrupt_cb_init(
    int unit);

int j2x_intr_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
int j2x_intr_recurring_action_handle_generic_none(
    int unit,
    int block_instance,
    uint32 en_jer_interrupt,
    char *msg);
#endif
