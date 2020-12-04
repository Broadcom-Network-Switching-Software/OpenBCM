
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_IQS_TYPES_H__
#define __DNX_IQS_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm/cosq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>



typedef struct {
    
    int valid;
    
    bcm_cosq_delay_tolerance_t thresholds;
} dnx_iqs_credit_request_profile_db_t;


typedef struct {
    
    int queue_min;
    
    int queue_max;
    
    int enable;
    
    int retransmit_th;
    
    int* delete_th;
} dnx_iqs_credit_watchdog_db_t;


typedef struct {
    
    dnx_iqs_credit_request_profile_db_t* credit_request_profile;
    
    dnx_iqs_credit_watchdog_db_t credit_watchdog;
} dnx_iqs_db_t;


#endif 
