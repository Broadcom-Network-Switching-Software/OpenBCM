
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SCHEDULER_TYPES_H__
#define __DNX_SCHEDULER_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>



typedef struct {
    
    int rate;
    
    int valid;
} dnx_scheduler_db_shaper_t;


typedef struct {
    
    int is_colored;
} dnx_scheduler_db_hr_info_t;


typedef struct {
    
    SHR_BITDCL* modified;
} dnx_scheduler_db_interface_info_t;


typedef struct {
    
    int* reserved_hr_flow_id;
    int nof_reserved_hr;
    
    int* fmq_flow_id;
} dnx_scheduler_db_fmq_info;


typedef struct {
    
    int reserved_erp_hr_flow_id[8];
    int nof_reserved_erp_hr;
} dnx_scheduler_db_general_info;


typedef struct {
    
    dnx_scheduler_db_shaper_t* port;
    
    dnx_scheduler_db_hr_info_t** hr;
    
    dnx_scheduler_db_interface_info_t* interface;
    
    dnx_scheduler_db_fmq_info* fmq;
    
    dnx_scheduler_db_general_info* general;
} dnx_scheduler_db_t;


#endif 
