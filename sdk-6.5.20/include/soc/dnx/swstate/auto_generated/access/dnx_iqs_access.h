
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_IQS_ACCESS_H__
#define __DNX_IQS_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_iqs_types.h>
#include <bcm/cosq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>



typedef int (*dnx_iqs_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_iqs_db_init_cb)(
    int unit);


typedef int (*dnx_iqs_db_credit_request_profile_alloc_cb)(
    int unit);


typedef int (*dnx_iqs_db_credit_request_profile_valid_set_cb)(
    int unit, uint32 credit_request_profile_idx_0, int valid);


typedef int (*dnx_iqs_db_credit_request_profile_valid_get_cb)(
    int unit, uint32 credit_request_profile_idx_0, int *valid);


typedef int (*dnx_iqs_db_credit_request_profile_thresholds_set_cb)(
    int unit, uint32 credit_request_profile_idx_0, bcm_cosq_delay_tolerance_t thresholds);


typedef int (*dnx_iqs_db_credit_request_profile_thresholds_get_cb)(
    int unit, uint32 credit_request_profile_idx_0, bcm_cosq_delay_tolerance_t *thresholds);


typedef int (*dnx_iqs_db_credit_watchdog_queue_min_set_cb)(
    int unit, int queue_min);


typedef int (*dnx_iqs_db_credit_watchdog_queue_min_get_cb)(
    int unit, int *queue_min);


typedef int (*dnx_iqs_db_credit_watchdog_queue_max_set_cb)(
    int unit, int queue_max);


typedef int (*dnx_iqs_db_credit_watchdog_queue_max_get_cb)(
    int unit, int *queue_max);


typedef int (*dnx_iqs_db_credit_watchdog_enable_set_cb)(
    int unit, int enable);


typedef int (*dnx_iqs_db_credit_watchdog_enable_get_cb)(
    int unit, int *enable);


typedef int (*dnx_iqs_db_credit_watchdog_retransmit_th_set_cb)(
    int unit, int retransmit_th);


typedef int (*dnx_iqs_db_credit_watchdog_retransmit_th_get_cb)(
    int unit, int *retransmit_th);


typedef int (*dnx_iqs_db_credit_watchdog_delete_th_set_cb)(
    int unit, uint32 delete_th_idx_0, int delete_th);


typedef int (*dnx_iqs_db_credit_watchdog_delete_th_get_cb)(
    int unit, uint32 delete_th_idx_0, int *delete_th);


typedef int (*dnx_iqs_db_credit_watchdog_delete_th_alloc_cb)(
    int unit);




typedef struct {
    dnx_iqs_db_credit_request_profile_valid_set_cb set;
    dnx_iqs_db_credit_request_profile_valid_get_cb get;
} dnx_iqs_db_credit_request_profile_valid_cbs;


typedef struct {
    dnx_iqs_db_credit_request_profile_thresholds_set_cb set;
    dnx_iqs_db_credit_request_profile_thresholds_get_cb get;
} dnx_iqs_db_credit_request_profile_thresholds_cbs;


typedef struct {
    dnx_iqs_db_credit_request_profile_alloc_cb alloc;
    
    dnx_iqs_db_credit_request_profile_valid_cbs valid;
    
    dnx_iqs_db_credit_request_profile_thresholds_cbs thresholds;
} dnx_iqs_db_credit_request_profile_cbs;


typedef struct {
    dnx_iqs_db_credit_watchdog_queue_min_set_cb set;
    dnx_iqs_db_credit_watchdog_queue_min_get_cb get;
} dnx_iqs_db_credit_watchdog_queue_min_cbs;


typedef struct {
    dnx_iqs_db_credit_watchdog_queue_max_set_cb set;
    dnx_iqs_db_credit_watchdog_queue_max_get_cb get;
} dnx_iqs_db_credit_watchdog_queue_max_cbs;


typedef struct {
    dnx_iqs_db_credit_watchdog_enable_set_cb set;
    dnx_iqs_db_credit_watchdog_enable_get_cb get;
} dnx_iqs_db_credit_watchdog_enable_cbs;


typedef struct {
    dnx_iqs_db_credit_watchdog_retransmit_th_set_cb set;
    dnx_iqs_db_credit_watchdog_retransmit_th_get_cb get;
} dnx_iqs_db_credit_watchdog_retransmit_th_cbs;


typedef struct {
    dnx_iqs_db_credit_watchdog_delete_th_set_cb set;
    dnx_iqs_db_credit_watchdog_delete_th_get_cb get;
    dnx_iqs_db_credit_watchdog_delete_th_alloc_cb alloc;
} dnx_iqs_db_credit_watchdog_delete_th_cbs;


typedef struct {
    
    dnx_iqs_db_credit_watchdog_queue_min_cbs queue_min;
    
    dnx_iqs_db_credit_watchdog_queue_max_cbs queue_max;
    
    dnx_iqs_db_credit_watchdog_enable_cbs enable;
    
    dnx_iqs_db_credit_watchdog_retransmit_th_cbs retransmit_th;
    
    dnx_iqs_db_credit_watchdog_delete_th_cbs delete_th;
} dnx_iqs_db_credit_watchdog_cbs;


typedef struct {
    dnx_iqs_db_is_init_cb is_init;
    dnx_iqs_db_init_cb init;
    
    dnx_iqs_db_credit_request_profile_cbs credit_request_profile;
    
    dnx_iqs_db_credit_watchdog_cbs credit_watchdog;
} dnx_iqs_db_cbs;





extern dnx_iqs_db_cbs dnx_iqs_db;

#endif 
