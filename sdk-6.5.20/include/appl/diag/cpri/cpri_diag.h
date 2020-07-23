/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cpri_diag.h
 * Purpose:     Extern declarations for phymod diagnostics support.
 */

#ifndef   _CPRI_SDK_DIAG_H_
#define   _CPRI_SDK_DIAG_H_

#include <soc/cprimod/cprimod.h>

int cpri_timesync_reset(int unit);
int sv_testing_cb(int unit, bcm_port_t port, bcm_cpri_interrupt_data_t *data, void *userdata);
int  sv_testing_polling(int unit, bcm_port_t port);
int cpri_diag_test_register_interrupt_callback( int unit, bcm_port_t port);
int cpri_diag_test_callback_register(int unit, int port, int key);
int cpri_diag_test_callback_user_data_set(int unit, int port, int key,
                                          int *user_data, int num_entries);

#endif /* _CPRI_SDK_DIAG_H_ */
