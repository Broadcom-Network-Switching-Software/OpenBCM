/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SW_STATE_HTB_CALLBACKS_H
#define _SW_STATE_HTB_CALLBACKS_H

#include <sal/types.h>

int dnx_algo_lif_mapping_htb_entry_print(
    int unit,
    void *key,
    void *data);

int sw_state_htb_example_entry_print(
    int unit,
    void *key,
    void *data);

int sw_state_htb_multithread_print(
    int unit,
    void *key,
    void *data);

int kbp_ipv4_fwd_tcam_access_mapping_htb_entry_print(
    int unit,
    void *key,
    void *data);

int kbp_ipv6_fwd_tcam_access_mapping_htb_entry_print(
    int unit,
    void *key,
    void *data);

int dnx_algo_lif_local_outlif_valid_combinations_print_cb(
    int unit,
    void *key,
    void *data);

int dnx_algo_lif_table_mngr_valid_fields_htb_entry_print_cb(
    int unit,
    void *key,
    void *data);

int dnx_algo_lif_table_mngr_table_info_htb_entry_print_cb(
    int unit,
    void *key,
    void *data);

int dnx_algo_lif_mngr_local_outlif_info_htb_entry_print_cb(
    int unit,
    void *key,
    void *data);

#endif
