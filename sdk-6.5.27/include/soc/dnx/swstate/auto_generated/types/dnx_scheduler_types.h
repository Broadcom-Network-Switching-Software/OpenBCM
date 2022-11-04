
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SCHEDULER_TYPES_H__
#define __DNX_SCHEDULER_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>


typedef enum {
    SCH_VIRTUAL_FLOWS_UNINITALIZED = 0,
    SCH_VIRTUAL_FLOWS_ENABLED,
    SCH_VIRTUAL_FLOWS_DISABLED,
    SCH_NOF_VIRTUAL_FLOWS_TYPES,
    
    SCH_VIRTUAL_FLOWS_SW_STATE_TYPE_E_DUMMY
} sch_virtual_flows_sw_state_type_e;



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
    uint8 non_default_flow_order;
    uint8 is_composite_fq_supported;
} dnx_scheduler_db_general_info;

typedef struct {
    sch_virtual_flows_sw_state_type_e is_enabled;
    SHR_BITDCL** is_flow_virtual;
} dnx_scheduler_db_virtual_flows_info_t;

typedef struct {
    uint32 pattern;
    uint32 range_start;
    uint32 range_end;
    uint32 is_enabled;
} dnx_scheduler_db_low_rate_info_t;

typedef struct {
    dnx_scheduler_db_shaper_t** port_shaper;
    dnx_scheduler_db_hr_info_t** hr;
    dnx_scheduler_db_interface_info_t* interface;
    dnx_scheduler_db_fmq_info* fmq;
    dnx_scheduler_db_virtual_flows_info_t virtual_flows;
    dnx_scheduler_db_general_info* general;
    dnx_scheduler_db_low_rate_info_t** low_rate_ranges;
} dnx_scheduler_db_t;


#endif 
