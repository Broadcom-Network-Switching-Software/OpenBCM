
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_PORT_IMB_ACCESS_H__
#define __DNX_PORT_IMB_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>



typedef int (*imbm_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*imbm_init_cb)(
    int unit);


typedef int (*imbm_imbs_in_use_set_cb)(
    int unit, uint32 imbs_in_use);


typedef int (*imbm_imbs_in_use_get_cb)(
    int unit, uint32 *imbs_in_use);


typedef int (*imbm_imb_set_cb)(
    int unit, uint32 imb_idx_0, imb_create_info_t imb);


typedef int (*imbm_imb_get_cb)(
    int unit, uint32 imb_idx_0, imb_create_info_t *imb);


typedef int (*imbm_imb_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*imbm_imb_type_set_cb)(
    int unit, uint32 imb_type_idx_0, imb_dispatch_type_t imb_type);


typedef int (*imbm_imb_type_get_cb)(
    int unit, uint32 imb_type_idx_0, imb_dispatch_type_t *imb_type);


typedef int (*imbm_pmd_lock_counter_set_cb)(
    int unit, uint32 pmd_lock_counter_idx_0, uint32 pmd_lock_counter);


typedef int (*imbm_pmd_lock_counter_get_cb)(
    int unit, uint32 pmd_lock_counter_idx_0, uint32 *pmd_lock_counter);


typedef int (*imbm_credit_tx_reset_mutex_create_cb)(
    int unit);


typedef int (*imbm_credit_tx_reset_mutex_is_created_cb)(
    int unit, uint8 *is_created);


typedef int (*imbm_credit_tx_reset_mutex_take_cb)(
    int unit, int usec);


typedef int (*imbm_credit_tx_reset_mutex_give_cb)(
    int unit);




typedef struct {
    imbm_imbs_in_use_set_cb set;
    imbm_imbs_in_use_get_cb get;
} imbm_imbs_in_use_cbs;


typedef struct {
    imbm_imb_set_cb set;
    imbm_imb_get_cb get;
    imbm_imb_alloc_cb alloc;
} imbm_imb_cbs;


typedef struct {
    imbm_imb_type_set_cb set;
    imbm_imb_type_get_cb get;
} imbm_imb_type_cbs;


typedef struct {
    imbm_pmd_lock_counter_set_cb set;
    imbm_pmd_lock_counter_get_cb get;
} imbm_pmd_lock_counter_cbs;


typedef struct {
    imbm_credit_tx_reset_mutex_create_cb create;
    imbm_credit_tx_reset_mutex_is_created_cb is_created;
    imbm_credit_tx_reset_mutex_take_cb take;
    imbm_credit_tx_reset_mutex_give_cb give;
} imbm_credit_tx_reset_mutex_cbs;


typedef struct {
    imbm_is_init_cb is_init;
    imbm_init_cb init;
    
    imbm_imbs_in_use_cbs imbs_in_use;
    
    imbm_imb_cbs imb;
    
    imbm_imb_type_cbs imb_type;
    
    imbm_pmd_lock_counter_cbs pmd_lock_counter;
    
    imbm_credit_tx_reset_mutex_cbs credit_tx_reset_mutex;
} imbm_cbs;





extern imbm_cbs imbm;

#endif 
